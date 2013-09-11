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

void ags_devout_real_change_bpm(AgsDevout *devout, double bpm);

void ags_devout_switch_buffer_flag(AgsDevout *devout);

void ags_devout_play_functions(AgsDevout *devout);

void ags_devout_alsa_init(AgsDevout *devout,
			  GError **error);
void ags_devout_alsa_play(AgsDevout *devout,
			  GError **error);
void ags_devout_alsa_free(AgsDevout *devout);

enum{
  PROP_0,
  PROP_DEVICE,
  PROP_DSP_CHANNELS,
  PROP_PCM_CHANNELS,
  PROP_BITS,
  PROP_BUFFER_SIZE,
  PROP_FREQUENCY,
  PROP_BUFFER,
  PROP_BPM,
  PROP_ATTACK,
  PROP_TASK,
};

enum{
  RUN,
  STOP,
  TIC,
  LAST_SIGNAL,
};

static gpointer ags_devout_parent_class = NULL;
static guint devout_signals[LAST_SIGNAL];

/* dangerous - produces a lot of output */
static gboolean DEBUG_DEVOUT = FALSE;

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
  param_spec = g_param_spec_string("device\0",
				   "the device identifier\0",
				   "The device to perform output to\0",
				   "hw:0\0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
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
				 128,
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

  /* AgsDevoutClass */
  devout->tic = NULL;

  devout_signals[TIC] =
    g_signal_new("tic\0",
		 G_TYPE_FROM_CLASS (devout),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsDevoutClass, tic),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

GQuark
ags_devout_error_quark()
{
  return(g_quark_from_static_string("ags-devout-error-quark\0"));
}

void
ags_devout_init(AgsDevout *devout)
{
  devout->flags = AGS_DEVOUT_ALSA;

  devout->dsp_channels = 2;
  devout->pcm_channels = 2;
  devout->bits = 16;
  devout->buffer_size = AGS_DEVOUT_DEFAULT_BUFFER_SIZE;
  devout->frequency = AGS_DEVOUT_DEFAULT_SAMPLERATE;

  //  devout->out.oss.device = NULL;
  devout->out.alsa.handle = NULL;
  devout->out.alsa.device = g_strdup("hw:0\0");

  /*
  devout->offset = 0;

  devout->note_delay = (guint) round((double)devout->frequency / (double)devout->buffer_size * 60.0 / 120.0 / 16.0);
  devout->note_counter = 0;
  */

  //  devout->note_offset = 0;

  devout->buffer = (signed short **) malloc(4 * sizeof(signed short*));
  devout->buffer[0] = (signed short *) malloc(devout->dsp_channels * devout->buffer_size * sizeof(signed short));
  devout->buffer[1] = (signed short *) malloc(devout->dsp_channels * devout->buffer_size * sizeof(signed short));
  devout->buffer[2] = (signed short *) malloc(devout->dsp_channels * devout->buffer_size * sizeof(signed short));
  devout->buffer[3] = (signed short *) malloc(devout->dsp_channels * devout->buffer_size * sizeof(signed short));

  devout->bpm = AGS_DEVOUT_DEFAULT_BPM;
  devout->delay = (guint) AGS_ATTACK_DEFAULT_DELAY * 60 / AGS_DEVOUT_DEFAULT_BPM;
  devout->delay_counter = 0;
  
  g_message("%d\0", devout->delay % devout->buffer_size);

  devout->attack = ags_attack_alloc(devout->delay % devout->buffer_size, devout->buffer_size - (devout->delay % devout->buffer_size),
				    devout->buffer_size - (devout->delay % devout->buffer_size), devout->delay % devout->buffer_size);

  devout->main = NULL;

  /* all AgsAudio */
  devout->audio = NULL;

  /* threads */
  devout->audio_loop = ags_audio_loop_new(G_OBJECT(devout));
  devout->task_thread = AGS_TASK_THREAD(devout->audio_loop->task_thread);
  devout->devout_thread = AGS_DEVOUT_THREAD(devout->audio_loop->devout_thread);
}

void
ags_devout_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(gobject);

  //TODO:JK: implement set functionality
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      if((AGS_DEVOUT_LIBAO & (devout->flags)) != 0){
	//TODO:JK: implement me
      }else if((AGS_DEVOUT_OSS & (devout->flags)) != 0){
	devout->out.oss.device = device;
      }else if((AGS_DEVOUT_ALSA & (devout->flags)) != 0){
	devout->out.alsa.device = device;
      }
    }
    break;
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
  case PROP_DEVICE:
    {
      if((AGS_DEVOUT_LIBAO & (devout->flags)) != 0){
	g_value_set_string(value, ao_driver_info(devout->out.ao.driver_ao)->name);
      }else if((AGS_DEVOUT_OSS & (devout->flags)) != 0){
	g_value_set_string(value, devout->out.oss.device);
      }else if((AGS_DEVOUT_ALSA & (devout->flags)) != 0){
	g_value_set_string(value, devout->out.alsa.device);
      }
    }
    break;
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

  /*  */
  g_object_unref(G_OBJECT(devout->audio_loop));
  g_object_unref(G_OBJECT(devout->task_thread));
  g_object_unref(G_OBJECT(devout->devout_thread));

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

  play->iterator_thread = ags_iterator_thread_new();

  play->source = NULL;
  play->audio_channel = 0;
  play->group_id = 0;

  return(play);
}

GList*
ags_devout_list_cards()
{
  snd_ctl_t *card_handle;
  snd_ctl_card_info_t *card_info;
  GList *list;
  char *name;
  char str[65];
  int card_num;
  int error;

  list = NULL;
  card_num = -1;

  while(TRUE){
    error = snd_card_next(&card_num);

    if(card_num < 0){
      break;
    }

    if(error < 0){
      continue;
    }

    sprintf(str, "hw:%i\0", card_num);
    error = snd_ctl_open(&card_handle, str, 0);

    if(error < 0){
      continue;
    }

    snd_ctl_card_info_alloca(&card_info);
    error = snd_ctl_card_info(card_handle, card_info);

    if(error < 0){
      continue;
    }

    list = g_list_prepend(list, g_strdup(snd_ctl_card_info_get_name(card_info)));

    snd_ctl_close(card_handle);
  }

  snd_config_update_free_global();

  list = g_list_reverse(list);

  return(list);
}

void
ags_devout_pcm_info(char *card_id,
		    guint *channels_min, guint *channels_max,
		    guint *rate_min, guint *rate_max,
		    guint *buffer_size_min, guint *buffer_size_max,
		    GError **error)
{
  int rc;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  unsigned int val;
  int dir;
  snd_pcm_uframes_t frames;
  int err;

  /* Open PCM device for playback. */
  handle = NULL;

  rc = snd_pcm_open(&handle, card_id, SND_PCM_STREAM_PLAYBACK, 0);

  if(rc < 0) {
    g_message("unable to open pcm device: %s\n\0", snd_strerror(rc));

    g_set_error(error,
		AGS_DEVOUT_ERROR,
		AGS_DEVOUT_ERROR_LOCKED_SOUNDCARD,
		"unable to open pcm device: %s\n\0",
		snd_strerror(rc));

    return;
  }

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);

  /* channels */
  snd_pcm_hw_params_get_channels_min(params, &val);
  *channels_min = val;

  snd_pcm_hw_params_get_channels_max(params, &val);
  *channels_max = val;

  /* samplerate */
  dir = 0;
  snd_pcm_hw_params_get_rate_min(params, &val, &dir);
  *rate_min = val;

  dir = 0;
  snd_pcm_hw_params_get_rate_max(params, &val, &dir);
  *rate_max = val;

  /* buffer size */
  dir = 0;
  snd_pcm_hw_params_get_buffer_size_min(params, &frames);
  *buffer_size_min = frames;

  dir = 0;
  snd_pcm_hw_params_get_buffer_size_max(params, &frames);
  *buffer_size_max = frames;

  snd_pcm_close(handle);
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

void
ags_devout_tic(AgsDevout *devout)
{
  g_return_if_fail(AGS_IS_DEVOUT(devout));

  if((AGS_DEVOUT_PLAY & devout->flags) == 0){
    g_message("ags_devout_tic: not playing\0");
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

void
ags_devout_alsa_init(AgsDevout *devout,
		     GError **error)
{
  int rc;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  unsigned int val;
  int dir;
  snd_pcm_uframes_t frames;
  int err;

  /* Open PCM device for playback. */
  handle = NULL;

  rc = snd_pcm_open(&handle, devout->out.alsa.device, SND_PCM_STREAM_PLAYBACK, 0);

  if(rc < 0) {
    g_message("unable to open pcm device: %s\n\0", snd_strerror(rc));
    return;
  }

  devout->out.alsa.handle = handle;

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&(params));

  devout->out.alsa.params = params;

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);

  /* Set the desired hardware parameters. */

  /* Interleaved mode */
  snd_pcm_hw_params_set_access(handle, params,
			       SND_PCM_ACCESS_RW_INTERLEAVED);

  /* Signed 16-bit little-endian format */
  snd_pcm_hw_params_set_format(devout->out.alsa.handle, devout->out.alsa.params,
                              SND_PCM_FORMAT_S16_LE);

  /* Two channels (stereo) */
  snd_pcm_hw_params_set_channels(handle, params, 2);

  /* 44100 bits/second sampling rate (CD quality) */
  val = 44100;//(unsigned int) devout->frequency;
  dir = 0;
  snd_pcm_hw_params_set_rate(handle, params,
			     val, dir);

  /* Set period size to devout->buffer_size frames. */
  frames = 128;//devout->buffer_size;

  snd_pcm_hw_params_set_buffer_size(handle,
				    params, 128);

  snd_pcm_hw_params_set_period_size(handle,
				    params, frames, dir);

  //snd_pcm_hw_params_set_rate_resample(devout->out.alsa.handle,
  //				      devout->out.alsa.params, frames);

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);

  if(rc < 0) {
    g_message("unable to set hw parameters: %s\0", snd_strerror(rc));
    return;
  }

  devout->delay_counter = 0;
}

void
ags_devout_alsa_play(AgsDevout *devout,
		     GError **error)
{
  AgsDevoutThread *devout_thread;
 
  devout_thread = devout->devout_thread;
    
  /*  */
  if((AGS_DEVOUT_BUFFER0 & (devout->flags)) != 0){
    memset(devout->buffer[3], 0, (size_t) 256 * sizeof(signed short));
      
    devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle, devout->buffer[0], (snd_pcm_sframes_t) 128);
      
    if(devout->out.alsa.rc == -EPIPE){
      /* EPIPE means underrun */
      g_message("underrun occurred\0");
      snd_pcm_prepare(devout->out.alsa.handle);
    }else if(devout->out.alsa.rc < 0){
      g_message("error from writei: %s\0", snd_strerror(devout->out.alsa.rc));
    }else if(devout->out.alsa.rc != (int) 128) {
      g_message("short write, write %d frames\0", devout->out.alsa.rc);
    }
      
    //      g_message("ags_devout_play 0\0");
  }else if((AGS_DEVOUT_BUFFER1 & (devout->flags)) != 0){
    memset(devout->buffer[0], 0, (size_t) 256 * sizeof(signed short));

    devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle, devout->buffer[1], (snd_pcm_sframes_t) 128);
      
    if(devout->out.alsa.rc == -EPIPE){
      /* EPIPE means underrun */
      g_message("underrun occurred\0");
      snd_pcm_prepare(devout->out.alsa.handle);
    }else if(devout->out.alsa.rc < 0){
      g_message("error from writei: %s\0", snd_strerror(devout->out.alsa.rc));
    }else if(devout->out.alsa.rc != (int) 128) {
      g_message("short write, write %d frames\0", devout->out.alsa.rc);
    }
      
    //      g_message("ags_devout_play 1\0");
  }else if((AGS_DEVOUT_BUFFER2 & (devout->flags)) != 0){
    memset(devout->buffer[1], 0, (size_t) 256 * sizeof(signed short));
      
    devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle, devout->buffer[2], (snd_pcm_sframes_t) 128);
      
    if(devout->out.alsa.rc == -EPIPE){
      /* EPIPE means underrun */
      g_message("underrun occurred\0");
      snd_pcm_prepare(devout->out.alsa.handle);
    }else if(devout->out.alsa.rc < 0){
      g_message("error from writei: %s\0", snd_strerror(devout->out.alsa.rc));
    }else if(devout->out.alsa.rc != (int) 128) {
      g_message("short write, write %d frames\0", devout->out.alsa.rc);
    }

    //      g_message("ags_devout_play 2\0");
  }else if((AGS_DEVOUT_BUFFER3 & devout->flags) != 0){
    memset(devout->buffer[2], 0, (size_t) 256 * sizeof(signed short));
      
    devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle, devout->buffer[3], (snd_pcm_sframes_t) 128);
      
    if(devout->out.alsa.rc == -EPIPE){
      /* EPIPE means underrun */
      g_message("underrun occurred\0");
      snd_pcm_prepare(devout->out.alsa.handle);
    }else if(devout->out.alsa.rc < 0){
      g_message("error from writei: %s\0", snd_strerror(devout->out.alsa.rc));
    }else if(devout->out.alsa.rc != (int) 128) {
      g_message("short write, write %d frames\0", devout->out.alsa.rc);
    }

    //      g_message("ags_devout_play 3\0");
  }

  /*
    if((AGS_DEVOUT_COUNT & (devout->flags)) != 0)
    devout->offset++;
  */

  /* determine if attack should be switched */
  devout->delay_counter += 1;

  if(devout->delay_counter == devout->delay){
    if((AGS_DEVOUT_ATTACK_FIRST & (devout->flags)) != 0){
      devout->flags &= (~AGS_DEVOUT_ATTACK_FIRST);
    }else{
      devout->flags |= AGS_DEVOUT_ATTACK_FIRST;
    }

    devout->delay_counter = 0;
  } 

  /* switch buffer flags */
  ags_devout_switch_buffer_flag(devout);
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
  /* start audio_loop */
  ags_thread_start(AGS_THREAD(devout->audio_loop));

  /* start fifo - push */
  //  pthread_create(&(devout->push->thread), NULL, &ags_devout_gate_control_push, devout->push);
  //  pthread_setschedprio(devout->push->thread, 99);

  /* start fifo - pop */
  //  pthread_create(&(devout->pop->thread), NULL, &ags_devout_gate_control_pop, devout->pop);
  //  pthread_setschedprio(devout->pop->thread, 99);
}

AgsDevout*
ags_devout_new(GObject *main)
{
  AgsDevout *devout;

  devout = (AgsDevout *) g_object_new(AGS_TYPE_DEVOUT, NULL);
  
  devout->main = main;
  devout->audio_loop->main = main;

  return(devout);
}
