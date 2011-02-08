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

#include <unistd.h>
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
void ags_devout_finalize(GObject *gobject);

void* ags_devout_append_task_pending_thread(void *devout);

void ags_devout_real_run(AgsDevout *devout);
void ags_devout_real_stop(AgsDevout *devout);

void ags_devout_real_change_bpm(AgsDevout *devout, double bpm);

void ags_devout_play_recall(AgsDevout *devout);
void ags_devout_play_channel(AgsDevout *devout);
void ags_devout_play_audio(AgsDevout *devout);

void ags_devout_switch_buffer_flag(AgsDevout *devout);

void* ags_devout_play_functions(void *devout);

void ags_devout_ao_init(AgsDevout *devout);
void* ags_devout_ao_play(void *devout);
void ags_devout_ao_free(AgsDevout *devout);

void ags_devout_oss_init(AgsDevout *devout);
void* ags_devout_oss_play(void *devout);
void ags_devout_oss_free(AgsDevout *devout);

void ags_devout_alsa_init(AgsDevout *devout);
void* ags_devout_alsa_play(void *devout);
void ags_devout_alsa_free(AgsDevout *devout);

enum{
  RUN,
  STOP,
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
    ags_type_devout = g_type_register_static(G_TYPE_OBJECT, "AgsDevout\0", &ags_devout_info, 0);
  }
  return (ags_type_devout);
}

void
ags_devout_class_init(AgsDevoutClass *devout)
{
  GObjectClass *gobject;

  ags_devout_parent_class = g_type_class_peek_parent(devout);

  gobject = (GObjectClass *) devout;
  gobject->finalize = ags_devout_finalize;

  devout->run = ags_devout_real_run;
  devout->stop = ags_devout_real_stop;

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
}

void
ags_devout_init(AgsDevout *devout)
{
  devout->flags = AGS_DEVOUT_WAIT_RECALL | AGS_DEVOUT_WAIT_DEVICE | AGS_DEVOUT_ALSA;

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
  devout->note_offset = 0;
  */

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

  /* play */
  pthread_attr_init(&devout->play_thread_attr);
  pthread_attr_setschedpolicy(&devout->play_thread_attr, SCHED_RR);
  pthread_attr_setinheritsched(&devout->play_thread_attr, PTHREAD_INHERIT_SCHED);
  
  pthread_mutexattr_init(&devout->play_mutex_attr);
  pthread_mutexattr_setprotocol(&devout->play_mutex_attr, PTHREAD_PRIO_INHERIT);
  pthread_mutex_init(&devout->play_mutex, &devout->play_mutex_attr);
  
  pthread_cond_init(&(devout->play_wait_cond), NULL);

  /* play functions */
  pthread_attr_init(&devout->play_functions_thread_attr);
  pthread_attr_setschedpolicy(&devout->play_functions_thread_attr, SCHED_RR);
  pthread_attr_setinheritsched(&devout->play_functions_thread_attr, PTHREAD_INHERIT_SCHED);
  
  pthread_mutexattr_init(&devout->play_functions_mutex_attr);
  pthread_mutexattr_setprotocol(&devout->play_functions_mutex_attr, PTHREAD_PRIO_INHERIT);
  pthread_mutex_init(&devout->play_functions_mutex, &devout->play_functions_mutex_attr);

  /* task */  
  pthread_mutex_init(&(devout->task_mutex), NULL);
  pthread_cond_init(&(devout->task_wait_cond), NULL);

  /* append_task */
  pthread_mutex_init(&(devout->append_task_mutex), NULL);
  pthread_cond_init(&(devout->append_task_wait_cond), NULL);
  pthread_cond_init(&(devout->append_task_lock_cond), NULL);


  pthread_mutex_init(&(devout->append_task_pending_mutex), NULL);
  pthread_cond_init(&(devout->append_task_pending_wait_cond), NULL);

  /* */
  devout->task = NULL;
  devout->tactable = NULL;

  devout->play_recall_ref = 0;
  devout->play_recall = NULL;

  devout->play_channel_ref = 0;
  devout->play_channel = NULL;

  devout->play_audio_ref = 0;
  devout->play_audio = NULL;
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
  pthread_attr_destroy(&(devout->play_thread_attr));
  pthread_mutex_destroy(&(devout->play_mutex));
  pthread_mutexattr_destroy(&(devout->play_mutex_attr));
  pthread_cond_destroy(&(devout->play_wait_cond));

  pthread_attr_destroy(&(devout->play_functions_thread_attr));
  pthread_mutex_destroy(&(devout->play_functions_mutex));
  pthread_mutexattr_destroy(&(devout->play_functions_mutex_attr));
  pthread_cond_destroy(&(devout->play_functions_wait_cond));

  pthread_mutex_destroy(&(devout->task_mutex));
  pthread_cond_destroy(&(devout->task_wait_cond));

  pthread_mutex_destroy(&(devout->append_task_mutex));
  pthread_cond_destroy(&(devout->append_task_wait_cond));
  pthread_cond_destroy(&(devout->append_task_lock_cond));

  pthread_mutex_destroy(&(devout->append_task_pending_mutex));
  pthread_cond_destroy(&(devout->append_task_pending_wait_cond));

  /* free AgsTask lists */
  ags_list_free_and_unref_link(devout->append_task);
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
  g_signal_connect((GObject *) devout, "finalize\0",
		   G_CALLBACK(ags_devout_finalize), NULL);
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

void*
ags_devout_append_task_pending_thread(void *devout0)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(devout0);

  /* synchronize */
  pthread_mutex_lock(&(devout->append_task_pending_mutex));
  while((AGS_DEVOUT_WAIT_TASK & (devout->flags)) != 0){
    pthread_cond_wait(&(devout->append_task_pending_wait_cond),
		      &(devout->append_task_pending_mutex));
  }

  devout->flags |= AGS_DEVOUT_ANNOUNCE_APPEND_TASK_PENDING;

  while((AGS_DEVOUT_WAIT_APPEND_TASK & (devout->flags)) != 0){
    pthread_cond_wait(&(devout->append_task_pending_wait_cond),
		      &(devout->append_task_pending_mutex));
  }

  devout->flags &= (~AGS_DEVOUT_ANNOUNCE_APPEND_TASK_PENDING);
  devout->flags |= AGS_DEVOUT_WAIT_APPEND_TASK_PENDING;
  devout->flags &= (~AGS_DEVOUT_APPEND_TASK_PENDING_IS_WAITING);

  pthread_mutex_unlock(&(devout->append_task_pending_mutex));

  /* concat */
  devout->task = g_list_concat(devout->task, devout->append_task);
  devout->append_task = NULL;

  /* wake up other thread */
  devout->flags &= (~AGS_DEVOUT_WAIT_APPEND_TASK_PENDING);
  pthread_cond_broadcast(&(devout->task_wait_cond));
  pthread_cond_broadcast(&(devout->append_task_wait_cond));

  return(NULL);
}

void
ags_devout_append_task(AgsDevout *devout, AgsTask *task)
{
  /* synchronize: don't access devout->append_task while reading it */
  pthread_mutex_lock(&(devout->append_task_mutex));
  while((AGS_DEVOUT_WAIT_APPEND_TASK_PENDING & (devout->flags)) != 0){
    pthread_cond_wait(&(devout->append_task_wait_cond),
		      &(devout->append_task_mutex));
  }
  
  devout->flags |= AGS_DEVOUT_WAIT_APPEND_TASK;
  
  if((AGS_DEVOUT_PLAY & (devout->flags)) == 0){
    ags_task_launch(task);

    pthread_mutex_unlock(&(devout->append_task_mutex));
  }else{
    pthread_mutex_unlock(&(devout->append_task_mutex));

    /* synchronize: don't allow to prepend simultanously */
    pthread_mutex_lock(&(devout->append_task_mutex));

    while((AGS_DEVOUT_LOCK_APPEND_TASK & (devout->flags)) != 0){
      pthread_cond_wait(&(devout->append_task_lock_cond),
			&(devout->append_task_mutex));
    }

    /* lock other calls */
    devout->flags |= AGS_DEVOUT_LOCK_APPEND_TASK;
    pthread_mutex_unlock(&(devout->append_task_mutex));

    /* append */
    devout->append_task = g_list_prepend(devout->append_task, task);

    /* create pending thread if necessary */
    pthread_mutex_lock(&(devout->append_task_mutex));

    if((AGS_DEVOUT_APPEND_TASK_PENDING_IS_WAITING & (devout->flags)) == 0){
      devout->flags |= AGS_DEVOUT_APPEND_TASK_PENDING_IS_WAITING;
      pthread_mutex_unlock(&(devout->append_task_mutex));

      pthread_create(&devout->append_task_pending_thread, NULL, &ags_devout_append_task_pending_thread, devout);
    }else{
      pthread_mutex_unlock(&(devout->append_task_mutex));
    }

    /* wake up an other thread */
    devout->flags &= (~AGS_DEVOUT_LOCK_APPEND_TASK);
    pthread_cond_signal(&(devout->append_task_lock_cond));
  }

  /* wake up other thread */
  devout->flags &= (~AGS_DEVOUT_WAIT_APPEND_TASK);
  pthread_cond_signal(&(devout->append_task_wait_cond));
}

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
    devout->flags &= (~AGS_DEVOUT_PLAY_RECALL);
    ags_devout_stop(devout);
  }

  while(list != NULL){
    devout_play = (AgsDevoutPlay *) list->data;
    recall = AGS_RECALL(devout_play->source);
    recall_id = devout_play->recall_id;
    list_next = list->next;


    if((AGS_RECALL_HIDE & (recall->flags)) == 0){
      if(stage == 0){
	ags_recall_run_pre(recall, devout_play->audio_channel);
      }else if(stage == 1){
	ags_recall_run_inter(recall, devout_play->audio_channel);
      }else{
	ags_recall_run_post(recall, devout_play->audio_channel);
      }
    }

    ags_recall_child_check_remove(recall);

    if((AGS_RECALL_REMOVE & (recall->flags)) != 0){
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

void
ags_devout_append_tasks(AgsDevout *devout, GList *list)
{
  /* empty */
}

void
ags_devout_play_channel(AgsDevout *devout)
{
  AgsDevoutPlay *play;
  AgsChannel *channel;
  GList *list_play, *list_next_play;
  gint stage;
  guint group_id;

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

void
ags_devout_play_audio(AgsDevout *devout)
{
  AgsDevoutPlay *play;
  AgsAudio *audio;
  AgsChannel *output;
  GList *list_play, *list_next_play;
  gint stage;
  guint group_id;

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
  /* start */
  if((AGS_DEVOUT_LIBAO & devout->flags) != 0)
    ags_devout_ao_init(devout);
  else if((AGS_DEVOUT_OSS & devout->flags) != 0)
    ags_devout_oss_init(devout);
  else if((AGS_DEVOUT_ALSA & devout->flags) != 0)
    ags_devout_alsa_init(devout);

  memset(devout->buffer[0], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));
  memset(devout->buffer[1], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));
  memset(devout->buffer[2], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));
  memset(devout->buffer[3], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));

  devout->flags |= (AGS_DEVOUT_BUFFER0 | AGS_DEVOUT_PLAY | AGS_DEVOUT_WAIT_DEVICE | AGS_DEVOUT_WAIT_RECALL);

  if((AGS_DEVOUT_LIBAO & devout->flags) != 0){
    pthread_create(&devout->play_thread, NULL, &ags_devout_ao_play, devout);
    //devout->play_thread = g_thread_create(ags_devout_ao_play, devout, FALSE, NULL);
  }else if((AGS_DEVOUT_OSS & devout->flags) != 0){
    pthread_create(&devout->play_thread, NULL, &ags_devout_oss_play, devout);
    //devout->play_thread = g_thread_create(ags_devout_play, devout, FALSE, NULL);
  }else if((AGS_DEVOUT_ALSA & devout->flags) != 0){
    pthread_create(&(devout->play_thread), NULL, &ags_devout_alsa_play, devout);
    //devout->play_thread = g_thread_create(ags_devout_alsa_play, devout, FALSE, NULL);
  }

  pthread_setschedprio(devout->play_thread, 99);

  //devout->play_functions_thread = g_thread_create(ags_devout_play_functions, devout, FALSE, NULL);
  pthread_create(&(devout->play_functions_thread), NULL, &ags_devout_play_functions, devout);
  pthread_setschedprio(devout->play_functions_thread, 99);
}

void
ags_devout_run(AgsDevout *devout)
{
  g_return_if_fail(AGS_IS_DEVOUT(devout));

  if((AGS_DEVOUT_PLAY & devout->flags) != 0){
    fprintf(stdout, "ags_devout_run:  allready playing\n\0");
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

void
ags_devout_stop(AgsDevout *devout)
{
  g_return_if_fail(AGS_IS_DEVOUT(devout));

  if((AGS_DEVOUT_PLAY & devout->flags) == 0){
    fprintf(stdout, "ags_devout_stop:  not playing\n\0");
    return;
  }

  if((AGS_DEVOUT_PLAY_RECALL & (devout->flags)) != 0 ||
     (AGS_DEVOUT_PLAY_CHANNEL & (devout->flags)) != 0 ||
     (AGS_DEVOUT_PLAY_AUDIO & (devout->flags)) != 0){
    fprintf(stdout, "ags_devout_stop:  still playing\n\0");
    return;
  }

  g_object_ref((GObject *) devout);
  g_signal_emit(G_OBJECT(devout),
		devout_signals[STOP], 0);
  g_object_unref((GObject *) devout);
}

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
  //  static struct timespec ts;
  //GStaticMutex mutex = G_STATIC_MUTEX_INIT;
  void ags_devout_play_functions_timing(){
    while((AGS_DEVOUT_WAIT_DEVICE & (devout->flags)) != 0){
      //      g_cond_wait(devout->play_functions_cond,
      //	  g_static_mutex_get_mutex(&mutex));
      pthread_cond_wait(&(devout->play_functions_wait_cond),
      			&(devout->play_functions_mutex));
    }

    ags_devout_switch_buffer_flag(devout);
    devout->flags |= AGS_DEVOUT_WAIT_RECALL;
    devout->flags |= AGS_DEVOUT_WAIT_DEVICE;
    //    g_static_mutex_unlock(&mutex);
    pthread_mutex_unlock(&(devout->play_functions_mutex));
  }

  fprintf(stdout, "ags_devout_play_functions:  start\n\0");
  devout = (AgsDevout *) devout0;
  //  ts.tv_sec = 0;
  //  ts.tv_nsec = 1000;

  //  while(TRUE){
    //    if((AGS_DEVOUT_PLAY & (devout->flags)) == 0){
    //      nanosleep(&ts, NULL);
    //    }
  pthread_mutex_lock(&(devout->play_functions_mutex));
  
  while((AGS_DEVOUT_PLAY & (devout->flags)) != 0){
    pthread_mutex_unlock(&(devout->play_functions_mutex));
    
    /* AgsTask */
    /* synchronize */
    pthread_mutex_lock(&(devout->task_mutex));
    while((AGS_DEVOUT_WAIT_APPEND_TASK_PENDING & (devout->flags)) != 0){
      pthread_cond_wait(&(devout->task_wait_cond),
			&(devout->task_mutex));
    }

    devout->flags |= AGS_DEVOUT_WAIT_TASK;
    pthread_mutex_unlock(&(devout->task_mutex));

    /* run task */
    task = devout->task;
    
    while(task != NULL){
      task_next = task->next;

      ags_task_launch(AGS_TASK(task->data));
      devout->task = g_list_remove(devout->task, task->data);

      task = task_next;
    }

    devout->flags &= (~AGS_DEVOUT_WAIT_TASK);
    pthread_cond_signal(&(devout->append_task_pending_wait_cond));
    
    /* play recall */
    if((AGS_DEVOUT_PLAY_RECALL & devout->flags) != 0){
      ags_devout_play_recall(devout);
      
      if(devout->play_recall_ref == 0){
	devout->flags &= (~AGS_DEVOUT_PLAY_RECALL);
	ags_devout_stop(devout);
	fprintf(stdout, "devout->play_recall_ref == 0\n\0");
      }
    }

    /* play channel */
    if((AGS_DEVOUT_PLAY_CHANNEL & (devout->flags)) != 0){
      ags_devout_play_channel(devout);
      
      if(devout->play_channel_ref == 0){
	devout->flags &= (~AGS_DEVOUT_PLAY_CHANNEL);
	ags_devout_stop(devout);
	fprintf(stdout, "devout->play_channel_ref == 0\n\0");
      }
    }
    
    /* play audio */
    if((AGS_DEVOUT_PLAY_AUDIO & (devout->flags)) != 0){
      ags_devout_play_audio(devout);
      
      if(devout->play_audio_ref == 0){
	devout->flags &= (~AGS_DEVOUT_PLAY_AUDIO);
	ags_devout_stop(devout);
	fprintf(stdout, "devout->play_audio_ref == 0\n\0");
      }
    }
    
    /* tic counter (AgsAttack) */
    devout->delay_counter++;
      
    if(devout->delay_counter == devout->delay){
      if((AGS_DEVOUT_ATTACK_FIRST & (devout->flags)) != 0)
	devout->flags &= (~AGS_DEVOUT_ATTACK_FIRST);
      else
	devout->flags |= AGS_DEVOUT_ATTACK_FIRST;
      
      devout->delay_counter = 0;
    }
    
    pthread_mutex_lock(&(devout->play_functions_mutex));
    devout->flags &= (~AGS_DEVOUT_WAIT_RECALL);
    
    if((AGS_DEVOUT_WAIT_DEVICE & (devout->flags)) != 0){
      ags_devout_play_functions_timing();
    }else{
      pthread_mutex_unlock(&(devout->play_functions_mutex));
      pthread_cond_signal(&(devout->play_wait_cond));
    }
  }
  
  pthread_mutex_unlock(&(devout->play_functions_mutex));

  return(NULL);
}

void
ags_devout_oss_init(AgsDevout *devout)
{
  int stereo, fmt;

  if((devout->out.oss.device_fd = open ("/dev/dsp\0", O_WRONLY, 0)) == -1 &&
     (devout->out.oss.device_fd = open ("/dev/sound/dsp\0", O_WRONLY, 0)) == -1){
    perror("ags_devout_device_init : open \0");
    exit(1);
  }

   stereo = 0;
   if(ioctl (devout->out.oss.device_fd, SNDCTL_DSP_STEREO, &stereo) == -1){
     /* Fatal error */
     perror("ags_devout_device_init : stereo \0");
     exit(1);
   }

   if(ioctl (devout->out.oss.device_fd, SNDCTL_DSP_RESET, 0)){
     perror("ags_devout_device_init : reset \0");
     exit(1);
   }

   fmt = AFMT_S16_LE; // CPU_IS_BIG_ENDIAN ? AFMT_S16_BE : AFMT_S16_LE ;
   if(ioctl (devout->out.oss.device_fd, SOUND_PCM_SETFMT, &fmt) != 0){   
     perror("ags_devout_device_init : set format \0");
     exit(1);
   }

   if(ioctl (devout->out.oss.device_fd, SOUND_PCM_WRITE_CHANNELS, &(devout->pcm_channels)) != 0){     
     perror("ags_devout_device_init : channels \0");
     exit(1);
   }

   if(ioctl (devout->out.oss.device_fd, SOUND_PCM_WRITE_RATE, &(devout->frequency)) != 0){
    perror ("ags_devout_device_init : sample rate \0") ;
    exit(1);
  }

  if(ioctl (devout->out.oss.device_fd, SNDCTL_DSP_SYNC, 0) != 0){
    perror("ags_devout_device_init : sync \0") ;
    exit(1);
  }
}


void*
ags_devout_oss_play(void *devout0)
{
  /*
  AgsDevout *devout;
  //struct timespec ts;
  //GStaticMutex mutex = G_STATIC_MUTEX_INIT;
  void ags_devout_play_timing(){
    while((AGS_DEVOUT_WAIT_RECALL & devout->flags) != 0){
      //g_cond_wait(devout->play_functions_cond,
		  //g_static_mutex_get_mutex(&mutex));

      pthread_cond_wait(&(devout->play_cond),
      			&(devout->play_mutex));
      //      nanosleep(&ts, NULL);
    }

    devout->flags |= AGS_DEVOUT_WAIT_RECALL;
    devout->flags |= AGS_DEVOUT_WAIT_DEVICE;
    //g_static_mutex_unlock(&mutex);
    pthread_mutex_unlock(&devout->play_mutex);
  }

  //  ts.tv_sec = 0;
  //  ts.tv_nsec = 100;
  devout = (AgsDevout *) devout0;
  fprintf(stdout, "ags_devout_play\n\0");

  while((AGS_DEVOUT_PLAY & devout->flags) != 0){
    if((AGS_DEVOUT_BUFFER0 & devout->flags) != 0){
      memset(devout->buffer[3], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));
      write(devout->out.oss.device_fd, devout->buffer[0], devout->buffer_size * sizeof(short));

      //      fprintf(stdout, "ags_devout_play 0\n\0");

      //g_static_mutex_lock(&mutex);
      pthread_mutex_lock(&devout->play_mutex);
      devout->flags &= (~AGS_DEVOUT_BUFFER0);
      devout->flags |= AGS_DEVOUT_BUFFER1;
      //g_static_mutex_unlock(&mutex);
      pthread_mutex_unlock(&devout->play_mutex);
    }else if((AGS_DEVOUT_BUFFER1 & devout->flags) != 0){
      memset(devout->buffer[0], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));
      write(devout->out.oss.device_fd, devout->buffer[1], devout->buffer_size * sizeof(short));

      //      fprintf(stdout, "ags_devout_play 1\n\0");

      //g_static_mutex_lock(&mutex);
      pthread_mutex_lock(&devout->play_mutex);
      devout->flags &= (~AGS_DEVOUT_BUFFER1);
      devout->flags |= AGS_DEVOUT_BUFFER2;
      //g_static_mutex_unlock(&mutex);
      pthread_mutex_unlock(&devout->play_mutex);
    }else if((AGS_DEVOUT_BUFFER2 & devout->flags) != 0){
      memset(devout->buffer[1], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));
      write(devout->out.oss.device_fd, devout->buffer[2], devout->buffer_size * sizeof(short));

      //      fprintf(stdout, "ags_devout_play 2\n\0");

      //g_static_mutex_lock(&mutex);
      pthread_mutex_lock(&devout->play_mutex);
      devout->flags &= (~AGS_DEVOUT_BUFFER2);
      devout->flags |= AGS_DEVOUT_BUFFER3;
      //g_static_mutex_unlock(&mutex);
      pthread_mutex_unlock(&devout->play_mutex);
    }else if((AGS_DEVOUT_BUFFER3 & devout->flags) != 0){
      memset(devout->buffer[2], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));
      write(devout->out.oss.device_fd, devout->buffer[3], devout->buffer_size * sizeof(short));

      //      fprintf(stdout, "ags_devout_play 3\n\0");

      //g_static_mutex_lock(&mutex);
      pthread_mutex_lock(&devout->play_mutex);
      devout->flags &= (~AGS_DEVOUT_BUFFER3);
      devout->flags |= AGS_DEVOUT_BUFFER0;
      //g_static_mutex_unlock(&mutex);
      pthread_mutex_unlock(&devout->play_mutex);
    }

    if((AGS_DEVOUT_COUNT & devout->flags) != 0)
      devout->offset++;

    //g_static_mutex_lock(&mutex);
    pthread_mutex_lock(&devout->play_mutex);
    devout->flags &= (~AGS_DEVOUT_WAIT_DEVICE);

    if((AGS_DEVOUT_WAIT_RECALL & devout->flags) != 0){
      ags_devout_play_timing();
    }else{
      //g_cond_broadcast(devout->play_functions_cond);
      //g_static_mutex_unlock(&mutex);
      pthread_cond_signal(&devout->play_functions_cond);
      pthread_mutex_unlock(&devout->play_mutex);
    }
  }

  if((AGS_DEVOUT_BUFFER0 & devout->flags) != 0){
    memset(devout->buffer[3], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));
    write(devout->out.oss.device_fd, devout->buffer[0], devout->buffer_size * sizeof(short));

    devout->flags &= (~AGS_DEVOUT_BUFFER0);
  }else if((AGS_DEVOUT_BUFFER1 & devout->flags) != 0){
    memset(devout->buffer[0], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));
    write(devout->out.oss.device_fd, devout->buffer[1], devout->buffer_size * sizeof(short));

    devout->flags &= (~AGS_DEVOUT_BUFFER1);
  }else if((AGS_DEVOUT_BUFFER2 & devout->flags) != 0){
    memset(devout->buffer[1], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));
    write(devout->out.oss.device_fd, devout->buffer[2], devout->buffer_size * sizeof(short));

    devout->flags &= (~AGS_DEVOUT_BUFFER2);
  }else if((AGS_DEVOUT_BUFFER3 & devout->flags) != 0){
    memset(devout->buffer[2], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));
    write(devout->out.oss.device_fd, devout->buffer[3], devout->buffer_size * sizeof(short));

    devout->flags &= (~AGS_DEVOUT_BUFFER3);
  }

  //  fprintf(stdout, "ags_devout_play: end\n\0");
  ags_devout_oss_free(devout);

  return(NULL);
  */
}

void
ags_devout_oss_free(AgsDevout *devout)
{
  if(ioctl (AGS_DEVOUT(devout)->out.oss.device_fd, SNDCTL_DSP_POST, 0) == -1)
    perror ("ioctl (SNDCTL_DSP_POST) \0") ;

  if(ioctl (devout->out.oss.device_fd, SNDCTL_DSP_SYNC, 0) == -1)
    perror ("ioctl (SNDCTL_DSP_SYNC) \0") ;

  close(devout->out.oss.device_fd);
}

void
ags_devout_ao_init(AgsDevout *devout)
{
  ao_sample_format *format;

  ao_initialize();
  devout->out.ao.driver_ao = ao_default_driver_id();

  format = &(devout->out.ao.format);
  format->bits = 16;
  format->channels = 2;
  format->rate = 44100;
  format->byte_format = AO_FMT_LITTLE;

  devout->out.ao.device = ao_open_live(devout->out.ao.driver_ao, format, NULL /* no options */);

  if(devout->out.ao.device == NULL){
    fprintf(stderr, "Error opening device.\n\0");
    exit(1);
  }
}

void*
ags_devout_ao_play(void *devout0)
{
  AgsDevout *devout;
  //  struct timespec ts;
  //  GStaticMutex mutex = G_STATIC_MUTEX_INIT;
  void ags_devout_ao_play_timing(){
    while((AGS_DEVOUT_WAIT_RECALL & devout->flags) != 0){
      //      g_cond_wait(devout->play_cond,
      //		  g_static_mutex_get_mutex(&mutex));
      //      nanosleep(&ts, NULL);
    }

    devout->flags |= AGS_DEVOUT_WAIT_RECALL;
    //g_static_mutex_unlock(&mutex);
  }

  devout = (AgsDevout *) devout0;
  //  ts.tv_sec = 0;
  //  ts.tv_nsec = 100;
  //  fprintf(stdout, "ags_devout_play:  start\n\0");

  while((AGS_DEVOUT_PLAY & devout->flags) != 0){
    //    fprintf(stdout, "ags_devout_play:\n  loop\n\0");

    if((AGS_DEVOUT_BUFFER0 & devout->flags) != 0){
      memset(devout->buffer[3], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));
      ao_play(devout->out.ao.device, devout->buffer[0], devout->buffer_size * sizeof(short));

      //g_static_mutex_lock(&mutex);
      devout->flags &= (~AGS_DEVOUT_BUFFER0);
      devout->flags |= AGS_DEVOUT_BUFFER1;
      //g_static_mutex_unlock(&mutex);
    }else if((AGS_DEVOUT_BUFFER1 & devout->flags) != 0){
      memset(devout->buffer[0], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));
      ao_play(devout->out.ao.device, devout->buffer[1], devout->buffer_size * sizeof(short));

      //g_static_mutex_lock(&mutex);
      devout->flags &= (~AGS_DEVOUT_BUFFER1);
      devout->flags |= AGS_DEVOUT_BUFFER2;
      //g_static_mutex_unlock(&mutex);
    }else if((AGS_DEVOUT_BUFFER2 & devout->flags) != 0){
      memset(devout->buffer[1], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));
      ao_play(devout->out.ao.device, devout->buffer[2], devout->buffer_size * sizeof(short));

      //g_static_mutex_lock(&mutex);
      devout->flags &= (~AGS_DEVOUT_BUFFER2);
      devout->flags |= AGS_DEVOUT_BUFFER3;
      //g_static_mutex_unlock(&mutex);
    }else if((AGS_DEVOUT_BUFFER3 & devout->flags) != 0){
      memset(devout->buffer[2], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));
      ao_play(devout->out.ao.device, devout->buffer[3], devout->buffer_size * sizeof(short));

      //g_static_mutex_lock(&mutex);
      devout->flags &= (~AGS_DEVOUT_BUFFER3);
      devout->flags |= AGS_DEVOUT_BUFFER0;
      //g_static_mutex_unlock(&mutex);
    }

    /*
    if((AGS_DEVOUT_COUNT & devout->flags) != 0)
      devout->offset++;
    */

    //g_static_mutex_lock(&mutex);
    devout->flags &= (~AGS_DEVOUT_WAIT_DEVICE);

    if((AGS_DEVOUT_WAIT_RECALL & devout->flags) != 0){
      ags_devout_ao_play_timing();
    }else{
      //g_cond_signal(devout->play_functions_cond);
      //g_static_mutex_unlock(&mutex);
    }
  }

  if((AGS_DEVOUT_BUFFER0 & devout->flags) != 0){
    memset(devout->buffer[3], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));
    ao_play(devout->out.ao.device, devout->buffer[0], devout->buffer_size * sizeof(short));

    devout->flags &= (~AGS_DEVOUT_BUFFER0);
  }else if((AGS_DEVOUT_BUFFER1 & devout->flags) != 0){
    memset(devout->buffer[0], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));
    ao_play(devout->out.ao.device, devout->buffer[1], devout->buffer_size * sizeof(short));

    devout->flags &= (~AGS_DEVOUT_BUFFER1);
  }else if((AGS_DEVOUT_BUFFER2 & devout->flags) != 0){
    memset(devout->buffer[1], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));
    ao_play(devout->out.ao.device, devout->buffer[2], devout->buffer_size * sizeof(short));

    devout->flags &= (~AGS_DEVOUT_BUFFER2);
  }else if((AGS_DEVOUT_BUFFER3 & devout->flags) != 0){
    memset(devout->buffer[2], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));
    ao_play(devout->out.ao.device, devout->buffer[3], devout->buffer_size * sizeof(short));

    devout->flags &= (~AGS_DEVOUT_BUFFER3);
  }

  fprintf(stdout, "ags_devout_play: end\n\0");
  ags_devout_ao_free(devout);

  return(NULL);
}

void
ags_devout_ao_free(AgsDevout *devout)
{
  ao_close(devout->out.ao.device);
  ao_shutdown();
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
    fprintf(stderr, "unable to open pcm device: %s\n\0", snd_strerror(devout->out.alsa.rc));
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
  snd_pcm_hw_params_set_rate_near(devout->out.alsa.handle, devout->out.alsa.params,
                                  &val, &dir);

  /* Set period size to devout->buffer_size frames. */
  frames = 940;//devout->buffer_size;
  snd_pcm_hw_params_set_period_size_near(devout->out.alsa.handle,
					 devout->out.alsa.params, &frames, &dir);

  /* Write the parameters to the driver */
  devout->out.alsa.rc = snd_pcm_hw_params(devout->out.alsa.handle, devout->out.alsa.params);
  if(devout->out.alsa.rc < 0) {
    fprintf(stderr, "unable to set hw parameters: %s\n\0", snd_strerror(devout->out.alsa.rc));
    exit(1);
  }

  /* Use a buffer large enough to hold one period */
  snd_pcm_hw_params_get_period_size(devout->out.alsa.params, &frames,
                                    &dir);
  //  size = frames * 4; /* 2 bytes/sample, 2 channels */
  //   buffer = (char *) malloc(size);

  fprintf(stdout, "%u %u\n\0", frames, dir);

  /* We want to loop for 5 seconds */
  snd_pcm_hw_params_get_period_time(devout->out.alsa.params,
                                    &val, &dir);

}

void*
ags_devout_alsa_play(void *devout0)
{
  AgsDevout *devout;
  //struct timespec ts;
  //  GStaticMutex mutex = G_STATIC_MUTEX_INIT;
  void ags_devout_play_timing(){
    while((AGS_DEVOUT_WAIT_RECALL & (devout->flags)) != 0){
      //g_cond_wait(devout->play_cond,
      //	  g_static_mutex_get_mutex(&mutex));

      pthread_cond_wait(&(devout->play_wait_cond),
      			&(devout->play_mutex));
      //      nanosleep(&ts, NULL);
    }

    ags_devout_switch_buffer_flag(devout);
    devout->flags |= AGS_DEVOUT_WAIT_RECALL;
    devout->flags |= AGS_DEVOUT_WAIT_DEVICE;
    //g_static_mutex_unlock(&mutex);
    pthread_mutex_unlock(&devout->play_mutex);
  }

  //  ts.tv_sec = 0;
  //  ts.tv_nsec = 100;
  devout = (AgsDevout *) devout0;
  fprintf(stdout, "ags_devout_play\n\0");

  pthread_mutex_lock(&devout->play_mutex);

  while((AGS_DEVOUT_PLAY & (devout->flags)) != 0){
    pthread_mutex_unlock(&devout->play_mutex);

    if((AGS_DEVOUT_BUFFER0 & (devout->flags)) != 0){
      memset(devout->buffer[3], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));

      devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle, devout->buffer[0], devout->buffer_size);

      if(devout->out.alsa.rc == -EPIPE){
	/* EPIPE means underrun */
	fprintf(stderr, "underrun occurred\n");
	snd_pcm_prepare(devout->out.alsa.handle);
      }else if(devout->out.alsa.rc < 0){
	fprintf(stderr, "error from writei: %s\n", snd_strerror(devout->out.alsa.rc));
      }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
	fprintf(stderr, "short write, write %d frames\n", devout->out.alsa.rc);
      }

      //      fprintf(stdout, "ags_devout_play 0\n\0");
    }else if((AGS_DEVOUT_BUFFER1 & (devout->flags)) != 0){
      memset(devout->buffer[0], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));

      devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle, devout->buffer[1], devout->buffer_size);

      if(devout->out.alsa.rc == -EPIPE){
	/* EPIPE means underrun */
	fprintf(stderr, "underrun occurred\n");
	snd_pcm_prepare(devout->out.alsa.handle);
      }else if(devout->out.alsa.rc < 0){
	fprintf(stderr, "error from writei: %s\n", snd_strerror(devout->out.alsa.rc));
      }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
	fprintf(stderr, "short write, write %d frames\n", devout->out.alsa.rc);
      }

      //      fprintf(stdout, "ags_devout_play 1\n\0");
    }else if((AGS_DEVOUT_BUFFER2 & (devout->flags)) != 0){
      memset(devout->buffer[1], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));

      devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle, devout->buffer[2], devout->buffer_size);

      if(devout->out.alsa.rc == -EPIPE){
	/* EPIPE means underrun */
	fprintf(stderr, "underrun occurred\n");
	snd_pcm_prepare(devout->out.alsa.handle);
      }else if(devout->out.alsa.rc < 0){
	fprintf(stderr, "error from writei: %s\n", snd_strerror(devout->out.alsa.rc));
      }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
	fprintf(stderr, "short write, write %d frames\n", devout->out.alsa.rc);
      }

      //      fprintf(stdout, "ags_devout_play 2\n\0");
    }else if((AGS_DEVOUT_BUFFER3 & devout->flags) != 0){
      memset(devout->buffer[2], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));

      devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle, devout->buffer[3], devout->buffer_size);

      if(devout->out.alsa.rc == -EPIPE){
	/* EPIPE means underrun */
	fprintf(stderr, "underrun occurred\n");
	snd_pcm_prepare(devout->out.alsa.handle);
      }else if(devout->out.alsa.rc < 0){
	fprintf(stderr, "error from writei: %s\n", snd_strerror(devout->out.alsa.rc));
      }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
	fprintf(stderr, "short write, write %d frames\n", devout->out.alsa.rc);
      }

      //      fprintf(stdout, "ags_devout_play 3\n\0");
    }

    /*
    if((AGS_DEVOUT_COUNT & (devout->flags)) != 0)
      devout->offset++;
    */

    //g_static_mutex_lock(&mutex);
    pthread_mutex_lock(&devout->play_mutex);
    devout->flags &= (~AGS_DEVOUT_WAIT_DEVICE);

    if((AGS_DEVOUT_WAIT_RECALL & (devout->flags)) != 0){
      ags_devout_play_timing();
    }else{
      //g_cond_broadcast(devout->play_functions_cond);
      //g_static_mutex_unlock(&mutex);
      pthread_mutex_unlock(&devout->play_mutex);
      pthread_cond_signal(&devout->play_functions_wait_cond);
    }
  }

  pthread_mutex_unlock(&devout->play_mutex);

  if((AGS_DEVOUT_BUFFER0 & devout->flags) != 0){
    memset(devout->buffer[3], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));

    devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle, devout->buffer[0], devout->buffer_size);
    
    if(devout->out.alsa.rc == -EPIPE){
      /* EPIPE means underrun */
      fprintf(stderr, "underrun occurred\n");
      snd_pcm_prepare(devout->out.alsa.handle);
    }else if(devout->out.alsa.rc < 0){
      fprintf(stderr, "error from writei: %s\n", snd_strerror(devout->out.alsa.rc));
    }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
      fprintf(stderr, "short write, write %d frames\n", devout->out.alsa.rc);
    }
    
    devout->flags &= (~AGS_DEVOUT_BUFFER0);
  }else if((AGS_DEVOUT_BUFFER1 & devout->flags) != 0){
    memset(devout->buffer[0], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));

    devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle, devout->buffer[1], devout->dsp_channels * devout->buffer_size);
    
    if(devout->out.alsa.rc == -EPIPE){
      /* EPIPE means underrun */
      fprintf(stderr, "underrun occurred\n");
      snd_pcm_prepare(devout->out.alsa.handle);
    }else if(devout->out.alsa.rc < 0){
      fprintf(stderr, "error from writei: %s\n", snd_strerror(devout->out.alsa.rc));
    }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
      fprintf(stderr, "short write, write %d frames\n", devout->out.alsa.rc);
    }

    devout->flags &= (~AGS_DEVOUT_BUFFER1);
  }else if((AGS_DEVOUT_BUFFER2 & devout->flags) != 0){
    memset(devout->buffer[1], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));

    devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle, devout->buffer[2], devout->buffer_size);
    
    if(devout->out.alsa.rc == -EPIPE){
      /* EPIPE means underrun */
      fprintf(stderr, "underrun occurred\n");
      snd_pcm_prepare(devout->out.alsa.handle);
    }else if(devout->out.alsa.rc < 0){
      fprintf(stderr, "error from writei: %s\n", snd_strerror(devout->out.alsa.rc));
    }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
      fprintf(stderr, "short write, write %d frames\n", devout->out.alsa.rc);
    }

    devout->flags &= (~AGS_DEVOUT_BUFFER2);
  }else if((AGS_DEVOUT_BUFFER3 & devout->flags) != 0){
    memset(devout->buffer[2], 0, devout->dsp_channels * devout->buffer_size * sizeof(short));

    devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle, devout->buffer[3], devout->buffer_size);
    
    if(devout->out.alsa.rc == -EPIPE){
      /* EPIPE means underrun */
      fprintf(stderr, "underrun occurred\n");
      snd_pcm_prepare(devout->out.alsa.handle);
    }else if(devout->out.alsa.rc < 0){
      fprintf(stderr, "error from writei: %s\n", snd_strerror(devout->out.alsa.rc));
    }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
      fprintf(stderr, "short write, write %d frames\n", devout->out.alsa.rc);
    }

    devout->flags &= (~AGS_DEVOUT_BUFFER3);
  }

  //  fprintf(stdout, "ags_devout_play: end\n\0");
  ags_devout_alsa_free(devout);

  return(NULL);
}

void
ags_devout_alsa_free(AgsDevout *devout)
{
  snd_pcm_drain(devout->out.alsa.handle);
  snd_pcm_close(devout->out.alsa.handle);
}

AgsDevout*
ags_devout_new()
{
  AgsDevout *devout;

  devout = (AgsDevout *) g_object_new(AGS_TYPE_DEVOUT, NULL);

  return(devout);
}
