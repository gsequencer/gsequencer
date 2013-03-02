/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/ags_devout.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_audio_signal.h>

#include <ags/audio/recall/ags_play_channel.h>

#include <ags/audio/file/ags_audio_file.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <errno.h>

#include <string.h>
#include <math.h>
#include <time.h>

void ags_devout_class_init(AgsDevoutClass *devout);
void ags_devout_init(AgsDevout *devout);
void ags_devout_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_devout_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_devout_finalize(GObject *gobject);

void* ags_devout_append_task_thread(void *ptr);
void* ags_devout_append_tasks_thread(void *ptr);

void ags_devout_real_run(AgsDevout *devout);
void ags_devout_real_stop(AgsDevout *devout);

void ags_devout_real_change_bpm(AgsDevout *devout, double bpm);

void ags_devout_play_recall(AgsDevout *devout);
void ags_devout_play_channel(AgsDevout *devout);
void ags_devout_play_audio(AgsDevout *devout);

void ags_devout_switch_buffer_flag(AgsDevout *devout);

void* ags_devout_play_functions(void *devout);

void ags_devout_alsa_init(AgsDevout *devout);
void* ags_devout_alsa_play(void *devout);
void ags_devout_alsa_free(AgsDevout *devout);

enum{
  PROP_0,
  PROP_DSP_CHANNELS,
  PROP_PCM_CHANNELS,
  PROP_BITS,
  PROP_BUFFER_SIZE,
  PROP_FREQUENCY,
  PROP_BUFFER,
  PROP_BPM,
  PROP_ATTACK,
  PROP_TASK,
  PROP_PLAY_RECALL,
  PROP_PLAY_CHANNEL,
  PROP_PLAY_AUDIO,
};

enum{
  RUN,
  STOP,
  TIC,
  LAST_SIGNAL,
};

static gpointer ags_devout_parent_class = NULL;
static guint devout_signals[LAST_SIGNAL];

GType
ags_devout_get_type (void)
{
  static GType ags_type_devout = 0;

  if(!ags_type_devout){
    static const GTypeInfo ags_devout_info = {
      sizeof (AgsDevoutClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_devout_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsDevout),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_devout_init,
    };

    ags_type_devout = g_type_register_static(G_TYPE_OBJECT,
					     "AgsDevout\0",
					     &ags_devout_info,
					     0);
  }

  return (ags_type_devout);
}

void
ags_devout_class_init(AgsDevoutClass *devout)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_devout_parent_class = g_type_class_peek_parent(devout);

  /* GObjectClass */
  gobject = (GObjectClass *) devout;

  gobject->set_property = ags_devout_set_property;
  gobject->get_property = ags_devout_get_property;

  gobject->finalize = ags_devout_finalize;

  /* properties */
  param_spec = g_param_spec_uint("dsp_channels\0",
				 "count of DSP channels\0",
				 "The count of DSP channels to use\0",
				 1,
				 64,
				 2,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DSP_CHANNELS,
				  param_spec);

  param_spec = g_param_spec_uint("pcm_channels\0",
				 "count of PCM channels\0",
				 "The count of PCM channels to use\0",
				 1,
				 64,
				 2,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PCM_CHANNELS,
				  param_spec);

  /*
   * TODO:JK: add support for other quality than 16 bit
   */
  param_spec = g_param_spec_uint("bits\0",
				 "precision of buffer\0",
				 "The precision to use for a frame\0",
				 1,
				 64,
				 16,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BITS,
				  param_spec);

  param_spec = g_param_spec_uint("buffer_size\0",
				 "frame count of a buffer\0",
				 "The count of frames a buffer contains\0",
				 1,
				 44100,
				 940,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  param_spec = g_param_spec_uint("frequency\0",
				 "frames per second\0",
				 "The frames count played during a second\0",
				 8000,
				 96000,
				 44100,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FREQUENCY,
				  param_spec);

  param_spec = g_param_spec_pointer("buffer\0",
				    "the buffer\0",
				    "The buffer to play\0",
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);

  param_spec = g_param_spec_double("bpm\0",
				   "beats per minute\0",
				   "Beats per minute to use\0",
				   1.0,
				   240.0,
				   120.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BPM,
				  param_spec);

  param_spec = g_param_spec_pointer("attack\0",
				    "attack of buffer\0",
				    "The attack to use for the buffer\0",
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);

  param_spec = g_param_spec_object("task\0",
				   "task to launch\0",
				   "A task to launch\0",
				   AGS_TYPE_TASK,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TASK,
				  param_spec);

  param_spec = g_param_spec_object("play_recall\0",
				   "recall to run\0",
				   "A recall to run\0",
				   AGS_TYPE_RECALL,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAY_RECALL,
				  param_spec);

  param_spec = g_param_spec_object("play_channel\0",
				   "channel to run\0",
				   "A channel to run\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAY_CHANNEL,
				  param_spec);

  param_spec = g_param_spec_object("play_audio\0",
				   "audio to run\0",
				   "A audio to run\0",
				   AGS_TYPE_AUDIO,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAY_AUDIO,
				  param_spec);

  /* AgsDevoutClass */
  devout->run = ags_devout_real_run;
  devout->stop = ags_devout_real_stop;
  devout->tic = NULL;

  devout_signals[RUN] =
    g_signal_new("run\0",
		 G_TYPE_FROM_CLASS (devout),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsDevoutClass, run),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  devout_signals[STOP] =
    g_signal_new("stop\0",
		 G_TYPE_FROM_CLASS (devout),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsDevoutClass, stop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  devout_signals[TIC] =
    g_signal_new("tic\0",
		 G_TYPE_FROM_CLASS (devout),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsDevoutClass, tic),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_devout_init(AgsDevout *devout)
{
  devout->flags = AGS_DEVOUT_ALSA;

  devout->dsp_channels = 2;
  devout->pcm_channels = 2;
  devout->bits = 16;
  devout->buffer_size = 940;
  devout->frequency = 44100;

  //  devout->out.oss.device = NULL;
  /*
  devout->offset = 0;

  devout->note_delay = (guint) round((double)devout->frequency / (double)devout->buffer_size * 60.0 / 120.0 / 16.0);
  devout->note_counter = 0;
  */

  //  devout->note_offset = 0;

  devout->buffer = (short **) malloc(4 * sizeof(short*));
  devout->buffer[0] = (short *) malloc(devout->dsp_channels * devout->buffer_size * sizeof(short));
  devout->buffer[1] = (short *) malloc(devout->dsp_channels * devout->buffer_size * sizeof(short));
  devout->buffer[2] = (short *) malloc(devout->dsp_channels * devout->buffer_size * sizeof(short));
  devout->buffer[3] = (short *) malloc(devout->dsp_channels * devout->buffer_size * sizeof(short));

  devout->bpm = 0.0;
  devout->delay = 0;
  devout->delay_counter = 0;
  devout->attack = ags_attack_alloc(0, devout->buffer_size,
				    0, devout->buffer_size);

  /* main_loop */
  pthread_attr_init(&devout->main_loop_thread_attr);
  pthread_attr_setinheritsched(&devout->main_loop_thread_attr, PTHREAD_INHERIT_SCHED);
  
  pthread_mutexattr_init(&devout->main_loop_mutex_attr);
  pthread_mutex_init(&devout->main_loop_mutex, &devout->main_loop_mutex_attr);
  
  pthread_cond_init(&(devout->main_loop_wait_cond), NULL);

  devout->wait_sync = 1;

  /* play */
  pthread_attr_init(&devout->play_thread_attr);
  //  pthread_attr_setschedpolicy(&devout->play_thread_attr, SCHED_RR);
  pthread_attr_setinheritsched(&devout->play_thread_attr, PTHREAD_INHERIT_SCHED);
  
  pthread_mutexattr_init(&devout->play_mutex_attr);
  //  pthread_mutexattr_setprotocol(&devout->play_mutex_attr, PTHREAD_PRIO_INHERIT);
  pthread_mutex_init(&devout->play_mutex, &devout->play_mutex_attr);
  
  pthread_cond_init(&(devout->play_wait_cond), NULL);

  /* play functions */
  pthread_attr_init(&devout->play_functions_thread_attr);
  //  pthread_attr_setschedpolicy(&devout->play_functions_thread_attr, SCHED_RR);
  pthread_attr_setinheritsched(&devout->play_functions_thread_attr, PTHREAD_INHERIT_SCHED);
  
  pthread_mutexattr_init(&devout->play_functions_mutex_attr);
  //  pthread_mutexattr_setprotocol(&devout->play_functions_mutex_attr, PTHREAD_PRIO_INHERIT);
  pthread_mutex_init(&devout->play_functions_mutex, &devout->play_functions_mutex_attr);
  pthread_cond_init(&(devout->play_functions_wait_cond), NULL);

  /* task */
  pthread_attr_init(&devout->task_thread_attr);
  //  pthread_attr_setschedpolicy(&devout->task_thread_attr, SCHED_RR);
  pthread_attr_setinheritsched(&devout->task_thread_attr, PTHREAD_INHERIT_SCHED);
  
  pthread_mutexattr_init(&devout->task_mutex_attr);
  //  pthread_mutexattr_setprotocol(&devout->task_mutex_attr, PTHREAD_PRIO_INHERIT);
  pthread_mutex_init(&(devout->task_mutex), NULL);

  pthread_cond_init(&(devout->task_wait_cond), NULL);
 
  /* append_task */
  pthread_mutex_init(&(devout->append_task_mutex), NULL);
  pthread_cond_init(&(devout->append_task_wait_cond), NULL);

  /* */
  devout->task = NULL;
  devout->tasks_queued = 0;
  devout->tasks_pending = 0;
  devout->audio = NULL;

  devout->play_recall_ref = 0;
  devout->play_recall = NULL;

  devout->play_channel_ref = 0;
  devout->play_channel = NULL;

  devout->play_audio_ref = 0;
  devout->play_audio = NULL;
}


void
ags_devout_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(gobject);

  /*
   * TODO:JK: implement set functionality
   */
  
  switch(prop_id){
  case PROP_DSP_CHANNELS:
    {
    }
    break;
  case PROP_PCM_CHANNELS:
    {
    }
    break;
  case PROP_BITS:
    {
    }
    break;
  case PROP_BUFFER_SIZE:
    {
    }
    break;
  case PROP_FREQUENCY:
    {
    }
    break;
  case PROP_BUFFER:
    {
    }
    break;
  case PROP_BPM:
    {
    }
    break;
  case PROP_TASK:
    {
    }
    break;
  case PROP_PLAY_RECALL:
    {
    }
    break;
  case PROP_PLAY_CHANNEL:
    {
    }
    break;
  case PROP_PLAY_AUDIO:
    {
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_devout_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(gobject);
  
  switch(prop_id){
  case PROP_DSP_CHANNELS:
    {
      g_value_set_uint(value, devout->dsp_channels);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_value_set_uint(value, devout->pcm_channels);
    }
    break;
  case PROP_BITS:
    {
      g_value_set_uint(value, devout->bits);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value, devout->buffer_size);
    }
    break;
  case PROP_FREQUENCY:
    {
      g_value_set_uint(value, devout->frequency);
    }
    break;
  case PROP_BUFFER:
    {
      g_value_set_pointer(value, devout->buffer);
    }
    break;
  case PROP_BPM:
    {
      g_value_set_double(value, devout->bpm);
    }
    break;
  case PROP_ATTACK:
    {
      g_value_set_pointer(value, devout->attack);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_devout_finalize(GObject *gobject)
{
  AgsDevout *devout;
  GList *list, *list_next;

  devout = AGS_DEVOUT(gobject);

  /* free output buffer */
  free(devout->buffer[0]);
  free(devout->buffer[1]);
  free(devout->buffer[2]);
  free(devout->buffer[3]);

  /* free buffer array */
  free(devout->buffer);

  /* free AgsAttack */
  free(devout->attack);

  /* free thread related structs */
  pthread_attr_destroy(&(devout->main_loop_thread_attr));
  pthread_mutex_destroy(&(devout->main_loop_mutex));
  pthread_mutexattr_destroy(&(devout->main_loop_mutex_attr));

  pthread_attr_destroy(&(devout->play_thread_attr));
  pthread_mutex_destroy(&(devout->play_mutex));
  pthread_mutexattr_destroy(&(devout->play_mutex_attr));
  pthread_cond_destroy(&(devout->play_wait_cond));

  pthread_attr_destroy(&(devout->play_functions_thread_attr));
  pthread_mutex_destroy(&(devout->play_functions_mutex));
  pthread_mutexattr_destroy(&(devout->play_functions_mutex_attr));
  pthread_cond_destroy(&(devout->play_functions_wait_cond));

  pthread_attr_destroy(&(devout->task_thread_attr));
  pthread_mutex_destroy(&(devout->task_mutex));
  pthread_mutexattr_destroy(&(devout->task_mutex_attr));
  pthread_cond_destroy(&(devout->task_wait_cond));

  pthread_mutex_destroy(&(devout->append_task_mutex));
  pthread_cond_destroy(&(devout->append_task_wait_cond));

  /* free AgsTask lists */
  ags_list_free_and_unref_link(devout->task);

  /* free AgsDevoutPlay lists */
  ags_list_free_and_free_link(devout->play_recall);
  ags_list_free_and_free_link(devout->play_channel);
  ags_list_free_and_free_link(devout->play_audio);

  /* call parent */
  G_OBJECT_CLASS(ags_devout_parent_class)->finalize(gobject);
}

void
ags_devout_connect(AgsDevout *devout)
{
}

AgsDevoutPlay*
ags_devout_play_alloc()
{
  AgsDevoutPlay *play;

  play = (AgsDevoutPlay *) malloc(sizeof(AgsDevoutPlay));

  play->flags = 0;
  play->source = NULL;
  play->audio_channel = 0;
  play->group_id = 0;

  return(play);
}

void
ags_devout_add_audio(AgsDevout *devout, GObject *audio)
{
  g_object_ref(G_OBJECT(audio));
  devout->audio = g_list_prepend(devout->audio,
				 audio);
}

void
ags_devout_remove_audio(AgsDevout *devout, GObject *audio)
{
  devout->audio = g_list_remove(devout->audio,
				audio);
  g_object_unref(G_OBJECT(audio));
}

/**
 * ags_devout_main_loop_thread:
 * 
 *
 *
 */
void*
ags_devout_main_loop_thread(void *devout0)
{
  AgsDevout *devout;
  gboolean initial_run;
  useconds_t idle;
  gboolean start_play;

  devout = AGS_DEVOUT(devout0);

  initial_run = TRUE;

  idle = 1000 * round(1000.0 * (double) devout->buffer_size  / (double) devout->frequency / 8.0);

  devout->wait_sync = 0;
  devout->wait_sync_task = FALSE;
  devout->append_task_suspend = 0;

  start_play = FALSE;

  while((AGS_DEVOUT_SHUTDOWN & (devout->flags)) == 0){
    /* suspend until everything has been done */
    pthread_mutex_lock(&(devout->main_loop_mutex));

    g_message("loop begin\0");

    if(devout->wait_sync != 0){
      devout->flags &= (~AGS_DEVOUT_WAIT_SYNC);
      
      while(devout->wait_sync != 0){
	g_message("loop@0[%d]\0", devout->wait_sync);
	
	pthread_cond_wait(&(devout->main_loop_wait_cond),
			  &(devout->main_loop_mutex));
      }
    }

    devout->flags |= (AGS_DEVOUT_WAIT_SYNC);

    pthread_mutex_unlock(&(devout->main_loop_mutex));

    /* wake up waiting threads */
    /* wake up output thread */
    pthread_mutex_lock(&(devout->main_loop_mutex));

    if((AGS_DEVOUT_PLAY & (devout->flags)) != 0){
      devout->wait_sync = devout->wait_sync + 1;

      if((AGS_DEVOUT_WAIT_PLAY & (devout->flags)) == 0){
	devout->play_suspend = FALSE;
	pthread_mutex_unlock(&(devout->main_loop_mutex));

	pthread_cond_signal(&(devout->play_wait_cond));
      }else{
	pthread_mutex_unlock(&(devout->main_loop_mutex));
      }
    }else{
      pthread_mutex_unlock(&(devout->main_loop_mutex));
    }

    /* task */
    pthread_mutex_lock(&(devout->main_loop_mutex));

    devout->wait_sync_task = TRUE;
    devout->task_suspend = FALSE;

    /* start task thread if necessary */
    if(initial_run){
      devout->task_suspend = TRUE;
      g_message("loop@start:0\0");
      pthread_mutex_unlock(&(devout->main_loop_mutex));

      pthread_create(&(devout->task_thread), NULL, &ags_devout_task_thread, devout);
      pthread_setschedprio(devout->task_thread, 99);

    }else{
      if((AGS_DEVOUT_WAIT_TASK & (devout->flags)) == 0){
	pthread_mutex_unlock(&(devout->main_loop_mutex));

	/* wake up task thread */      
	pthread_cond_signal(&(devout->task_wait_cond));

	devout->flags |= AGS_DEVOUT_WAIT_SYNC_TASK;
      }else{
	pthread_mutex_unlock(&(devout->main_loop_mutex));
      }
    }

    /* wait for task */
    pthread_mutex_lock(&(devout->main_loop_mutex));

    devout->flags &= (~AGS_DEVOUT_WAIT_SYNC_TASK);
      
    while(devout->wait_sync_task){
      g_message("loop@wait:1\0");
	
      pthread_cond_wait(&(devout->main_loop_wait_cond),
			&(devout->main_loop_mutex));      
    }
	
    g_message("unlocked\0");
      
    devout->flags |= AGS_DEVOUT_WAIT_SYNC_TASK;
      
    pthread_mutex_unlock(&(devout->main_loop_mutex));
  
    /* wake up audio processing functions */
    pthread_mutex_lock(&(devout->main_loop_mutex));

    if((AGS_DEVOUT_PLAY & (devout->flags)) != 0){
      devout->wait_sync = devout->wait_sync + 1;

      if((AGS_DEVOUT_WAIT_PLAY_FUNCTIONS & (devout->flags)) == 0){
	devout->play_functions_suspend = FALSE;
	pthread_mutex_unlock(&(devout->main_loop_mutex));
    
	pthread_cond_signal(&(devout->play_functions_wait_cond));
      }else{
	pthread_mutex_unlock(&(devout->main_loop_mutex));
      }
    }else{
      if((AGS_DEVOUT_START_PLAY & (devout->flags)) != 0){
	start_play = TRUE;
      }

      pthread_mutex_unlock(&(devout->main_loop_mutex));

      if(start_play){
	devout->flags &= (~AGS_DEVOUT_START_PLAY);
	start_play = FALSE;

	devout->wait_sync = devout->wait_sync + 2;
	ags_devout_run(devout);
      }
    }

    /* wake up tasks */
    pthread_mutex_lock(&(devout->main_loop_mutex));

    if(devout->tasks_pending > 0){
      devout->append_task_suspend = devout->tasks_pending;
      devout->wait_sync = devout->wait_sync + devout->tasks_pending;

      pthread_mutex_unlock(&(devout->main_loop_mutex));

      pthread_cond_broadcast(&(devout->append_task_wait_cond));
    }else{
      pthread_mutex_unlock(&(devout->main_loop_mutex));
    }

    if(initial_run){
      initial_run = FALSE;
    }
  }

  pthread_exit(NULL);
}

void*
ags_devout_task_thread(void *devout0)
{
  AgsDevout *devout;
  GList *task, *start, *end, *new_start, *old_list;
  struct timespec play_idle;
  useconds_t idle;
  gboolean initial_run;

  devout = AGS_DEVOUT(devout0);

  play_idle.tv_sec = 0;
  play_idle.tv_nsec = 10 * round(1000.0 * (double) devout->buffer_size  / (double) devout->frequency / 8.0);
  idle = 1000 * round(1000.0 * (double) devout->buffer_size  / (double) devout->frequency / 8.0);

  initial_run = TRUE;
  devout->task_suspend = TRUE;

  while((AGS_DEVOUT_SHUTDOWN & (devout->flags)) == 0){
    /* sync */
    /* wake up main_loop */
    pthread_mutex_lock(&(devout->task_mutex));

    devout->task_suspend = TRUE;
    devout->wait_sync_task = FALSE;

    if((AGS_DEVOUT_WAIT_SYNC_TASK & (devout->flags)) == 0){
      pthread_mutex_unlock(&(devout->task_mutex));

      pthread_cond_signal(&(devout->main_loop_wait_cond));

      devout->flags |= AGS_DEVOUT_WAIT_TASK;
    }else{
      pthread_mutex_unlock(&(devout->task_mutex));
    }

    /* suspend */
    pthread_mutex_lock(&(devout->task_mutex));

    if(!initial_run){
      devout->flags &= (~AGS_DEVOUT_WAIT_TASK);
	
      while(devout->task_suspend){
	pthread_cond_wait(&(devout->task_wait_cond),
			  &(devout->task_mutex));
      }
    }

    devout->flags |= AGS_DEVOUT_WAIT_TASK;
      
    pthread_mutex_unlock(&(devout->task_mutex));

    start = 
      task = devout->task;
    devout->task = NULL;

    /* launch tasks */
    if(task != NULL){
      while(task != NULL){
	g_message("ags_devout_task_thread - launching task: %s\n\0", G_OBJECT_TYPE_NAME(task->data));
	ags_task_launch(AGS_TASK(task->data));
	
	task = task->next;
      }

      g_list_free(start);
    }

    /* sleep if wanted */
    if((AGS_DEVOUT_PLAY & (devout->flags)) != 0){
      //      nanosleep(&play_idle, NULL);
      g_message("################ DEBUG ################\0");
    }else{
      usleep(idle);
      g_message("################ DEBUG ################\0");
    }

    if(initial_run){
      initial_run = FALSE;
    }
  }

  pthread_exit(NULL);
}

void*
ags_devout_append_task_thread(void *ptr)
{
  AgsDevoutAppend *append;
  AgsDevout *devout;
  AgsTask *task;
  gboolean initial_wait;

  append = (AgsDevoutAppend *) ptr;

  devout = append->devout;
  task = AGS_TASK(append->data);

  free(append);

  /* sync */
  pthread_mutex_lock(&(devout->append_task_mutex));

  initial_wait = TRUE;
  devout->tasks_pending += 1;

  while(devout->append_task_suspend == 0 ||
	initial_wait){
    initial_wait = FALSE;

    pthread_cond_wait(&(devout->append_task_wait_cond),
			&(devout->append_task_mutex));
  }

  /*  */  
  devout->append_task_suspend -= 1;
  devout->wait_sync -= 1;

  /* append to queue */
  devout->tasks_queued += 1;
  devout->task = g_list_append(devout->task, task);

  devout->tasks_queued -= 1;
  devout->tasks_pending -= 1;

  /*  */
  if((AGS_DEVOUT_WAIT_SYNC & (devout->flags)) == 0){
    pthread_mutex_unlock(&(devout->append_task_mutex));

    pthread_cond_signal(&(devout->main_loop_wait_cond));
  }else{
    pthread_mutex_unlock(&(devout->append_task_mutex));
  }

  /*  */
  pthread_exit(NULL);
}

/**
 * ags_devout_append_task:
 * @devout an #AgsDevout
 * @task an #AgsTask
 *
 * Adds the task to devout.
 */
void
ags_devout_append_task(AgsDevout *devout, AgsTask *task)
{
  AgsDevoutAppend *append;
  pthread_t thread;

  append = (AgsDevoutAppend *) malloc(sizeof(AgsDevoutAppend));

  append->devout = devout;
  append->data = task;

  pthread_create(&thread, NULL,
		 &ags_devout_append_task_thread, append);
}


void*
ags_devout_append_tasks_thread(void *ptr)
{
  AgsDevoutAppend *append;
  AgsDevout *devout;
  GList *list;
  gboolean initial_wait;

  append = (AgsDevoutAppend *) ptr;

  devout = append->devout;
  list = (GList *) append->data;

  free(append);

  /* sync */
  pthread_mutex_lock(&(devout->append_task_mutex));

  initial_wait = TRUE;
  devout->tasks_pending += 1;
  
  while(devout->append_task_suspend == 0 ||
	initial_wait){
    initial_wait = FALSE;

    pthread_cond_wait(&(devout->append_task_wait_cond),
			&(devout->append_task_mutex));
  }
  
  /*  */  
  devout->append_task_suspend -= 1;
  devout->wait_sync -= 1;

  /* concat with queue */
  devout->tasks_queued += 1;
  devout->task = g_list_concat(devout->task, list);

  devout->tasks_queued -= 1;
  devout->tasks_pending -= 1;

  /*  */
  if((AGS_DEVOUT_WAIT_SYNC & (devout->flags)) == 0){
    pthread_mutex_unlock(&(devout->append_task_mutex));

    pthread_cond_signal(&(devout->main_loop_wait_cond));
  }else{
    pthread_mutex_unlock(&(devout->append_task_mutex));
  }

  /*  */
  pthread_exit(NULL);
}

/**
 * ags_devout_append_tasks:
 * @devout an #AgsDevout
 * @list a GList with #AgsTask as data
 *
 * Concats the list with @devout's internal task list. Don't
 * free the list you pass. It will be freed for you.
 */
void
ags_devout_append_tasks(AgsDevout *devout, GList *list)
{
  AgsDevoutAppend *append;
  pthread_t thread;

  append = (AgsDevoutAppend *) malloc(sizeof(AgsDevoutAppend));

  append->devout = devout;
  append->data = list;

  pthread_create(&thread, NULL,
		 &ags_devout_append_tasks_thread, append);
}

/**
 * ags_devout_play_recall:
 * @devout an #AgsDevout
 *
 * Runs all recalls assigned with @devout. You may want to use
 * #AgsAppendRecall task to add an #AgsRecall.
 */
void
ags_devout_play_recall(AgsDevout *devout)
{
  AgsDevoutPlay *devout_play;
  AgsRecall *recall;
  AgsRecallID *recall_id;
  GList *list, *list_next;
  guint stage;
  //GStaticMutex mutex = G_STATIC_MUTEX_INIT;
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  devout->flags |= AGS_DEVOUT_PLAYING_RECALL;
  stage = 0;

 ags_devout_play_recall0:

  list = devout->play_recall;

  if(list == NULL){
    if((AGS_DEVOUT_PLAY_RECALL_TERMINATING & (devout->flags)) != 0){
      devout->flags &= (~(AGS_DEVOUT_PLAY_RECALL |
			  AGS_DEVOUT_PLAY_RECALL_TERMINATING));
      ags_devout_stop(devout);
    }else{
      devout->flags |= AGS_DEVOUT_PLAY_RECALL_TERMINATING;
    }
  }

  devout->flags &= (~AGS_DEVOUT_PLAY_RECALL_TERMINATING);

  while(list != NULL){
    devout_play = (AgsDevoutPlay *) list->data;
    recall = AGS_RECALL(devout_play->source);
    recall_id = devout_play->recall_id;
    list_next = list->next;


    if((AGS_RECALL_HIDE & (recall->flags)) == 0){
      if(stage == 0){
	ags_recall_run_pre(recall);
      }else if(stage == 1){
	ags_recall_run_inter(recall);
      }else{
	ags_recall_run_post(recall);
      }
    }

    ags_recall_child_check_remove(recall);

    if((AGS_RECALL_REMOVE & (recall->flags)) != 0){
      //TODO:JK: check if these mutices can be removed
      //g_static_mutex_lock(&mutex);
      pthread_mutex_lock(&mutex);
      devout->play_recall_ref--;
      devout->play_recall = g_list_remove(devout->play_recall, (gpointer) recall);
      //g_static_mutex_unlock(&mutex);
      pthread_mutex_unlock(&mutex);

      ags_recall_remove(recall);
    }

    list = list_next;
  }

  if(stage == 0){
    stage = 1;
    goto ags_devout_play_recall0;
  }else if(stage == 1){
    stage = 2;
    goto ags_devout_play_recall0;
  }

  devout->flags &= (~AGS_DEVOUT_PLAYING_RECALL);
}

/**
 * ags_devout_play_channel:
 * @devout an #AgsDevout
 *
 * Runs all recalls descending recursively and ascending till next 
 * #AgsRecycling around prior added #AgsChannel with #AgsAppendChannel
 * task.
 */
void
ags_devout_play_channel(AgsDevout *devout)
{
  AgsDevoutPlay *play;
  AgsChannel *channel;
  GList *list_play, *list_next_play;
  gint stage;
  AgsGroupId group_id;

  if(devout->play_channel == NULL){
    if((AGS_DEVOUT_PLAY_CHANNEL_TERMINATING & (devout->flags)) != 0){
      devout->flags &= (~(AGS_DEVOUT_PLAY_CHANNEL |
			  AGS_DEVOUT_PLAY_CHANNEL_TERMINATING));
      ags_devout_stop(devout);
    }else{
      devout->flags |= AGS_DEVOUT_PLAY_CHANNEL_TERMINATING;
    }
  }

  devout->flags &= (~AGS_DEVOUT_PLAY_CHANNEL_TERMINATING);

  /* entry point */
  devout->flags |= AGS_DEVOUT_PLAYING_CHANNEL;

  /* run the 3 stages */
  for(stage = 0; stage < 3; stage++){
    list_play = devout->play_channel;

    while(list_play != NULL){
      list_next_play = list_play->next;

      play = (AgsDevoutPlay *) list_play->data;
      channel = AGS_CHANNEL(play->source);
      group_id = play->group_id;

      ags_channel_recursive_play(channel, group_id, stage);

      if((AGS_DEVOUT_PLAY_REMOVE & (play->flags)) != 0){
	devout->play_channel_ref--;
	devout->play_channel = g_list_remove(devout->play_channel, (gpointer) play);
      }

      list_play = list_next_play;
    }
  }
}

/**
 * ags_devout_play_audio:
 * @devout an #AgsDevout
 *
 * Like ags_devout_play_channel() except that it runs all channels within
 * #AgsAudio.
 */
void
ags_devout_play_audio(AgsDevout *devout)
{
  AgsDevoutPlay *play;
  AgsAudio *audio;
  AgsChannel *output;
  GList *list_play, *list_next_play;
  gint stage;
  AgsGroupId group_id;

  if(devout->play_audio == NULL){
    if((AGS_DEVOUT_PLAY_AUDIO_TERMINATING & (devout->flags)) != 0){
      devout->flags &= (~(AGS_DEVOUT_PLAY_AUDIO |
			  AGS_DEVOUT_PLAY_AUDIO_TERMINATING));
      ags_devout_stop(devout);
    }else{
      devout->flags |= AGS_DEVOUT_PLAY_AUDIO_TERMINATING;
    }
  }

  devout->flags &= (~AGS_DEVOUT_PLAY_AUDIO_TERMINATING);

  /* entry point */
  devout->flags |= AGS_DEVOUT_PLAYING_AUDIO;

  /* run the 3 stages */
  for(stage = 0; stage < 3; stage++){
    list_play = devout->play_audio;

    while(list_play != NULL){
      list_next_play = list_play->next;

      play = (AgsDevoutPlay *) list_play->data;
      audio = AGS_AUDIO(play->source);
      group_id = play->group_id;

      output = audio->output;

      while(output != NULL){
	ags_channel_recursive_play(output, group_id, stage);
	
	output = output->next;
      }

      if((AGS_DEVOUT_PLAY_REMOVE & (play->flags)) != 0){
	devout->play_audio_ref--;
	devout->play_audio = g_list_remove(devout->play_audio, (gpointer) play);
      }

      list_play = list_next_play;
    }
  }
}

void
ags_devout_real_run(AgsDevout *devout)
{
  if((AGS_DEVOUT_PLAY & (devout->flags)) != 0){
    return;
  }

  /* start */
  ags_devout_alsa_init(devout);

  memset(devout->buffer[0], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));
  memset(devout->buffer[1], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));
  memset(devout->buffer[2], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));
  memset(devout->buffer[3], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));

  devout->flags |= (AGS_DEVOUT_BUFFER0 |
		    AGS_DEVOUT_PLAY);

  pthread_create(&(devout->play_functions_thread), NULL, &ags_devout_play_functions, devout);
  pthread_setschedprio(devout->play_functions_thread, 99);

  pthread_create(&(devout->play_thread), NULL, &ags_devout_alsa_play, devout);
  pthread_setschedprio(devout->play_thread, 99);
}

/**
 * ags_devout_real_run:
 * @devout an #AgsDevout
 * 
 * Starts the sound card output thread.
 */
void
ags_devout_run(AgsDevout *devout)
{
  g_return_if_fail(AGS_IS_DEVOUT(devout));

  if((AGS_DEVOUT_PLAY & (devout->flags)) != 0){
    g_message("ags_devout_run:  allready playing\n\0");
    return;
  }

  g_object_ref((GObject *) devout);
  g_signal_emit(G_OBJECT(devout),
		devout_signals[RUN], 0);
  g_object_unref((GObject *) devout);
}

void
ags_devout_real_stop(AgsDevout *devout)
{
  devout->flags &= ~(AGS_DEVOUT_PLAY);
}

/**
 * ags_devout_real_run:
 * @devout an #AgsDevout
 * 
 * Stops the sound card output thread.
 */
void
ags_devout_stop(AgsDevout *devout)
{
  g_return_if_fail(AGS_IS_DEVOUT(devout));

  if((AGS_DEVOUT_PLAY & (devout->flags)) == 0){
    g_message("ags_devout_stop:  not playing\n\0");
    return;
  }

  if((AGS_DEVOUT_PLAY_RECALL & (devout->flags)) != 0 ||
     (AGS_DEVOUT_PLAY_CHANNEL & (devout->flags)) != 0 ||
     (AGS_DEVOUT_PLAY_AUDIO & (devout->flags)) != 0){
    g_message("ags_devout_stop:  still playing\n\0");
    return;
  }

  g_object_ref((GObject *) devout);
  g_signal_emit(G_OBJECT(devout),
		devout_signals[STOP], 0);
  g_object_unref((GObject *) devout);
}

void
ags_devout_tic(AgsDevout *devout)
{
  g_return_if_fail(AGS_IS_DEVOUT(devout));

  if((AGS_DEVOUT_PLAY & devout->flags) == 0){
    g_message("ags_devout_tic:  not playing\n\0");
    return;
  }

  g_object_ref((GObject *) devout);
  g_signal_emit(G_OBJECT(devout),
		devout_signals[TIC], 0);
  g_object_unref((GObject *) devout);
}

/**
 * ags_devout_switch_buffer_flag:
 * @devout an #AgsDevout
 *
 * The buffer flag indicates the currently played buffer.
 */
void
ags_devout_switch_buffer_flag(AgsDevout *devout)
{
  if((AGS_DEVOUT_BUFFER0 & (devout->flags)) != 0){
    devout->flags &= (~AGS_DEVOUT_BUFFER0);
    devout->flags |= AGS_DEVOUT_BUFFER1;
  }else if((AGS_DEVOUT_BUFFER1 & (devout->flags)) != 0){
    devout->flags &= (~AGS_DEVOUT_BUFFER1);
    devout->flags |= AGS_DEVOUT_BUFFER2;
  }else if((AGS_DEVOUT_BUFFER2 & (devout->flags)) != 0){
    devout->flags &= (~AGS_DEVOUT_BUFFER2);
    devout->flags |= AGS_DEVOUT_BUFFER3;
  }else if((AGS_DEVOUT_BUFFER3 & (devout->flags)) != 0){
    devout->flags &= (~AGS_DEVOUT_BUFFER3);
    devout->flags |= AGS_DEVOUT_BUFFER0;
  }
}

void*
ags_devout_play_functions(void *devout0)
{
  AgsDevout *devout;
  GList *task, *task_next;
  gboolean initial_run;

  g_message("ags_devout_play_functions:  start\n\0");
  devout = (AgsDevout *) devout0;
  
  initial_run = TRUE;
  devout->play_functions_suspend = TRUE;

  while((AGS_DEVOUT_PLAY & (devout->flags)) != 0){
    /* sync */
    /* wake up main_loop */
    pthread_mutex_lock(&(devout->play_functions_mutex));

    devout->play_functions_suspend = TRUE;

    if(!initial_run){
      devout->wait_sync -= 1;
    }

    if((AGS_DEVOUT_WAIT_SYNC & (devout->flags)) == 0){
      pthread_mutex_unlock(&(devout->play_functions_mutex));

      if(!initial_run){
	pthread_cond_signal(&(devout->main_loop_wait_cond));
      }

      devout->flags |= AGS_DEVOUT_WAIT_PLAY_FUNCTIONS;
    }else{
      pthread_mutex_unlock(&(devout->play_functions_mutex));
    }

    /* suspend */
    pthread_mutex_lock(&(devout->play_functions_mutex));

    if(!initial_run){
      devout->flags &= (~AGS_DEVOUT_WAIT_PLAY_FUNCTIONS);
      
      while(devout->play_functions_suspend){
	pthread_cond_wait(&(devout->play_functions_wait_cond),
			  &(devout->play_functions_mutex));
      }
    }
    
    devout->flags |= AGS_DEVOUT_WAIT_PLAY_FUNCTIONS;
    
    pthread_mutex_unlock(&(devout->play_functions_mutex));

    /* play channel */
    if((AGS_DEVOUT_PLAY_CHANNEL & (devout->flags)) != 0){
      ags_devout_play_channel(devout);
      
      if(devout->play_channel_ref == 0){
	devout->flags &= (~AGS_DEVOUT_PLAY_CHANNEL);
	ags_devout_stop(devout);
	g_message("devout->play_channel_ref == 0\n\0");
      }
    }
    
    /* play audio */
    if((AGS_DEVOUT_PLAY_AUDIO & (devout->flags)) != 0){
      ags_devout_play_audio(devout);
      
      if(devout->play_audio_ref == 0){
	devout->flags &= (~AGS_DEVOUT_PLAY_AUDIO);
	ags_devout_stop(devout);
	g_message("devout->play_audio_ref == 0\n\0");
      }
    }
    
    /* determine if attack should be switched */
    devout->delay_counter++;
      
    if(devout->delay_counter == devout->delay){
      if((AGS_DEVOUT_ATTACK_FIRST & (devout->flags)) != 0)
	devout->flags &= (~AGS_DEVOUT_ATTACK_FIRST);
      else
	devout->flags |= AGS_DEVOUT_ATTACK_FIRST;
      
      devout->delay_counter = 0;
    }

    if(initial_run){
      initial_run = FALSE;
    }
  }
  
  pthread_exit(NULL);
}

void
ags_devout_alsa_init(AgsDevout *devout)
{
  unsigned int val;
  int dir;
  snd_pcm_uframes_t frames;
  int err;

  /* Open PCM device for playback. */
  devout->out.alsa.rc = snd_pcm_open(&devout->out.alsa.handle, "default\0",
				     SND_PCM_STREAM_PLAYBACK, 0);

  if(devout->out.alsa.rc < 0) {
    g_message("unable to open pcm device: %s\n\0", snd_strerror(devout->out.alsa.rc));
    exit(1);
  }

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&devout->out.alsa.params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(devout->out.alsa.handle, devout->out.alsa.params);

  /* Set the desired hardware parameters. */

  /* Interleaved mode */
  snd_pcm_hw_params_set_access(devout->out.alsa.handle, devout->out.alsa.params,
			       SND_PCM_ACCESS_RW_INTERLEAVED);

  /* Signed 16-bit little-endian format */
  snd_pcm_hw_params_set_format(devout->out.alsa.handle, devout->out.alsa.params,
                              SND_PCM_FORMAT_S16_LE);

  /* Two channels (stereo) */
  snd_pcm_hw_params_set_channels(devout->out.alsa.handle, devout->out.alsa.params, 2);

  /* 44100 bits/second sampling rate (CD quality) */
  val = 44100;//(unsigned int) devout->frequency;
  dir = 0;
  snd_pcm_hw_params_set_rate(devout->out.alsa.handle, devout->out.alsa.params,
			     val, dir);

  /* Set period size to devout->buffer_size frames. */
  frames = 940;//devout->buffer_size;

  snd_pcm_hw_params_set_period_size(devout->out.alsa.handle,
				    devout->out.alsa.params, frames, dir);


  snd_pcm_hw_params_set_rate_resample(devout->out.alsa.handle,
				      devout->out.alsa.params, frames);

  /* Write the parameters to the driver */
  devout->out.alsa.rc = snd_pcm_hw_params(devout->out.alsa.handle, devout->out.alsa.params);
  if(devout->out.alsa.rc < 0) {
    g_message("unable to set hw parameters: %s\n\0", snd_strerror(devout->out.alsa.rc));
    exit(1);
  }

  /* Use a buffer large enough to hold one period */
  //  snd_pcm_hw_params_get_period_size(devout->out.alsa.params, &frames,
  //                                &dir);
  //  size = frames * 4; /* 2 bytes/sample, 2 channels */
  //   buffer = (char *) malloc(size);

  //  g_message("%u %u\n\0", frames, dir);

  /* We want to loop */
  //  snd_pcm_hw_params_get_period_time(devout->out.alsa.params,
  //                                  &val, &dir);

}

void*
ags_devout_alsa_play(void *devout0)
{
  AgsDevout *devout;
  gboolean initial_run;

  devout = (AgsDevout *) devout0;
  g_message("ags_devout_play\n\0");

  initial_run = TRUE;
  devout->play_suspend = TRUE;

  while((AGS_DEVOUT_PLAY & (devout->flags)) != 0){
    /* sync */
    /* wake up main_loop */
    pthread_mutex_lock(&(devout->play_mutex));

    devout->play_suspend = TRUE;

    if(!initial_run){
      devout->wait_sync -= 1;
    }

    if((AGS_DEVOUT_WAIT_SYNC & (devout->flags)) == 0){
      pthread_mutex_unlock(&(devout->play_mutex));

      if(!initial_run){
	pthread_cond_signal(&(devout->main_loop_wait_cond));
      }
      
      devout->flags |= AGS_DEVOUT_WAIT_PLAY_FUNCTIONS;
    }else{
      pthread_mutex_unlock(&(devout->play_mutex));
    }

    pthread_mutex_lock(&(devout->play_mutex));

    /* suspend */
    if(!initial_run){
      devout->flags &= (~AGS_DEVOUT_WAIT_PLAY_FUNCTIONS);

      while(devout->play_suspend){
	pthread_cond_wait(&(devout->play_wait_cond),
			  &(devout->play_mutex));
      }
    }

    devout->flags |= AGS_DEVOUT_WAIT_PLAY_FUNCTIONS;

    pthread_mutex_unlock(&(devout->play_mutex));

    /*  */
    if((AGS_DEVOUT_BUFFER0 & (devout->flags)) != 0){
      memset(devout->buffer[3], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));

      devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle, devout->buffer[0], devout->buffer_size);

      if(devout->out.alsa.rc == -EPIPE){
	/* EPIPE means underrun */
	g_message("underrun occurred\n");
	snd_pcm_prepare(devout->out.alsa.handle);
      }else if(devout->out.alsa.rc < 0){
	g_message("error from writei: %s\n", snd_strerror(devout->out.alsa.rc));
      }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
	g_message("short write, write %d frames\n", devout->out.alsa.rc);
      }

      //      g_message("ags_devout_play 0\n\0");
    }else if((AGS_DEVOUT_BUFFER1 & (devout->flags)) != 0){
      memset(devout->buffer[0], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));

      devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle, devout->buffer[1], devout->buffer_size);

      if(devout->out.alsa.rc == -EPIPE){
	/* EPIPE means underrun */
	g_message("underrun occurred\n");
	snd_pcm_prepare(devout->out.alsa.handle);
      }else if(devout->out.alsa.rc < 0){
	g_message("error from writei: %s\n", snd_strerror(devout->out.alsa.rc));
      }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
	g_message("short write, write %d frames\n", devout->out.alsa.rc);
      }

      //      g_message("ags_devout_play 1\n\0");
    }else if((AGS_DEVOUT_BUFFER2 & (devout->flags)) != 0){
      memset(devout->buffer[1], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));

      devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle, devout->buffer[2], devout->buffer_size);

      if(devout->out.alsa.rc == -EPIPE){
	/* EPIPE means underrun */
	g_message("underrun occurred\n");
	snd_pcm_prepare(devout->out.alsa.handle);
      }else if(devout->out.alsa.rc < 0){
	g_message("error from writei: %s\n", snd_strerror(devout->out.alsa.rc));
      }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
	g_message("short write, write %d frames\n", devout->out.alsa.rc);
      }

      //      g_message("ags_devout_play 2\n\0");
    }else if((AGS_DEVOUT_BUFFER3 & devout->flags) != 0){
      memset(devout->buffer[2], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));

      devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle, devout->buffer[3], devout->buffer_size);

      if(devout->out.alsa.rc == -EPIPE){
	/* EPIPE means underrun */
	g_message("underrun occurred\n");
	snd_pcm_prepare(devout->out.alsa.handle);
      }else if(devout->out.alsa.rc < 0){
	g_message("error from writei: %s\n", snd_strerror(devout->out.alsa.rc));
      }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
	g_message("short write, write %d frames\n", devout->out.alsa.rc);
      }

      //      g_message("ags_devout_play 3\n\0");
    }

    /*
    if((AGS_DEVOUT_COUNT & (devout->flags)) != 0)
      devout->offset++;
    */

    if(initial_run){
      initial_run = FALSE;
    }
  }

  if((AGS_DEVOUT_BUFFER0 & devout->flags) != 0){
    memset(devout->buffer[3], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));

    devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle, devout->buffer[0], devout->buffer_size);
    
    if(devout->out.alsa.rc == -EPIPE){
      /* EPIPE means underrun */
      g_message("underrun occurred\n");
      snd_pcm_prepare(devout->out.alsa.handle);
    }else if(devout->out.alsa.rc < 0){
      g_message("error from writei: %s\n", snd_strerror(devout->out.alsa.rc));
    }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
      g_message("short write, write %d frames\n", devout->out.alsa.rc);
    }
    
    devout->flags &= (~AGS_DEVOUT_BUFFER0);
  }else if((AGS_DEVOUT_BUFFER1 & devout->flags) != 0){
    memset(devout->buffer[0], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));

    devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle, devout->buffer[1], devout->dsp_channels * devout->buffer_size);
    
    if(devout->out.alsa.rc == -EPIPE){
      /* EPIPE means underrun */
      g_message("underrun occurred\n");
      snd_pcm_prepare(devout->out.alsa.handle);
    }else if(devout->out.alsa.rc < 0){
      g_message("error from writei: %s\n", snd_strerror(devout->out.alsa.rc));
    }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
      g_message("short write, write %d frames\n", devout->out.alsa.rc);
    }

    devout->flags &= (~AGS_DEVOUT_BUFFER1);
  }else if((AGS_DEVOUT_BUFFER2 & devout->flags) != 0){
    memset(devout->buffer[1], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));

    devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle, devout->buffer[2], devout->buffer_size);
    
    if(devout->out.alsa.rc == -EPIPE){
      /* EPIPE means underrun */
      g_message("underrun occurred\n");
      snd_pcm_prepare(devout->out.alsa.handle);
    }else if(devout->out.alsa.rc < 0){
      g_message("error from writei: %s\n", snd_strerror(devout->out.alsa.rc));
    }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
      g_message("short write, write %d frames\n", devout->out.alsa.rc);
    }

    devout->flags &= (~AGS_DEVOUT_BUFFER2);
  }else if((AGS_DEVOUT_BUFFER3 & devout->flags) != 0){
    memset(devout->buffer[2], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));

    devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle, devout->buffer[3], devout->buffer_size);
    
    if(devout->out.alsa.rc == -EPIPE){
      /* EPIPE means underrun */
      g_message("underrun occurred\n");
      snd_pcm_prepare(devout->out.alsa.handle);
    }else if(devout->out.alsa.rc < 0){
      g_message("error from writei: %s\n", snd_strerror(devout->out.alsa.rc));
    }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
      g_message("short write, write %d frames\n", devout->out.alsa.rc);
    }

    devout->flags &= (~AGS_DEVOUT_BUFFER3);
  }

  //  g_message("ags_devout_play: end\n\0");
  ags_devout_alsa_free(devout);

  pthread_exit(NULL);
}

void
ags_devout_alsa_free(AgsDevout *devout)
{
  snd_pcm_drain(devout->out.alsa.handle);
  snd_pcm_close(devout->out.alsa.handle);
}

void
ags_devout_start_default_threads(AgsDevout *devout)
{
  /* start main_loop */
  pthread_create(&(devout->main_loop_thread), NULL, &ags_devout_main_loop_thread, devout);
  pthread_setschedprio(devout->main_loop_thread, 99);
}

AgsDevout*
ags_devout_new()
{
  AgsDevout *devout;

  devout = (AgsDevout *) g_object_new(AGS_TYPE_DEVOUT, NULL);

  return(devout);
}
