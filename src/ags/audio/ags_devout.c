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

#include <ags/object/ags_application_context.h>
#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_notation.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <errno.h>

#include <string.h>
#include <math.h>
#include <time.h>

/**
 * SECTION:ags_devout
 * @short_description: Outputting to soundcard
 * @title: AgsDevout
 * @section_id:
 * @include: ags/audio/ags_devout.h
 *
 * #AgsDevout represents a soundcard and supports output.
 */

void ags_devout_class_init(AgsDevoutClass *devout);
void ags_devout_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_devout_init(AgsDevout *devout);
void ags_devout_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_devout_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_devout_disconnect(AgsConnectable *connectable);
void ags_devout_connect(AgsConnectable *connectable);
void ags_devout_finalize(GObject *gobject);

void ags_devout_real_change_bpm(AgsDevout *devout, double bpm);

void ags_devout_switch_buffer_flag(AgsDevout *devout);

void ags_devout_play_functions(AgsDevout *devout);

void ags_devout_list_cards(AgsSoundcard *soundcard,
			   GList **card_id, GList **card_name);
void ags_devout_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
			 guint *channels_min, guint *channels_max,
			 guint *rate_min, guint *rate_max,
			 guint *buffer_size_min, guint *buffer_size_max,
			 GError **error);

void ags_devout_alsa_init(AgsSoundcard *soundcard,
			  GError **error);
void ags_devout_alsa_play(AgsSoundcard *soundcard,
			  GError **error);
void ags_devout_alsa_free(AgsSoundcard *soundcard);

signed short* ags_devout_get_buffer(AgsSoundcard *soundcard);
signed short* ags_devout_get_next_buffer(AgsSoundcard *soundcard);

void ags_devout_set_note_offset(AgsSoundcard *soundcard,
				guint note_offset);
guint ags_devout_get_note_offset(AgsSoundcard *soundcard);

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
  PROP_DEVICE,
  PROP_DSP_CHANNELS,
  PROP_PCM_CHANNELS,
  PROP_BITS,
  PROP_BUFFER_SIZE,
  PROP_FREQUENCY,
  PROP_BUFFER,
  PROP_BPM,
  PROP_ATTACK,
};

static gpointer ags_devout_parent_class = NULL;

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

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_devout_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_devout_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_devout = g_type_register_static(G_TYPE_OBJECT,
					     "AgsDevout\0",
					     &ags_devout_info,
					     0);

    g_type_add_interface_static(ags_type_devout,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_devout,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);
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
  /**
   * AgsDevout:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("application-context\0",
				   "the application context object\0",
				   "The application context object\0",
				   AGS_TYPE_APPLICATION_CONTEXT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /**
   * AgsDevout:device:
   *
   * The alsa soundcard indentifier
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_string("device\0",
				   "the device identifier\0",
				   "The device to perform output to\0",
				   "hw:0\0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsDevout:dsp-channels:
   *
   * The dsp channel count
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_uint("dsp-channels\0",
				 "count of DSP channels\0",
				 "The count of DSP channels to use\0",
				 1,
				 64,
				 2,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DSP_CHANNELS,
				  param_spec);

  /**
   * AgsDevout:pcm-channels:
   *
   * The pcm channel count
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_uint("pcm-channels\0",
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
  /**
   * AgsDevout:bits:
   *
   * The precision of the buffer
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_uint("bits\0",
				 "precision of buffer\0",
				 "The precision to use for a frame\0",
				 1,
				 64,
				 AGS_DEVOUT_DEFAULT_FORMAT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BITS,
				  param_spec);

  /**
   * AgsDevout:buffer-size:
   *
   * The buffer size
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_uint("buffer-size\0",
				 "frame count of a buffer\0",
				 "The count of frames a buffer contains\0",
				 1,
				 44100,
				 940,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsDevout:frequency:
   *
   * The samplerate
   * 
   * Since: 0.4.0
   */
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

  /**
   * AgsDevout:buffer:
   *
   * The buffer
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_pointer("buffer\0",
				    "the buffer\0",
				    "The buffer to play\0",
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);

  /**
   * AgsDevout:bpm:
   *
   * Beats per minute
   * 
   * Since: 0.4.0
   */
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
  /**
   * AgsDevout:bpm:
   *
   * Attack of the buffer
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_pointer("attack\0",
				    "attack of buffer\0",
				    "The attack to use for the buffer\0",
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);


  /* AgsDevoutClass */
  //empty
}

GQuark
ags_devout_error_quark()
{
  return(g_quark_from_static_string("ags-devout-error-quark\0"));
}

void
ags_devout_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_devout_connect;
  connectable->disconnect = ags_devout_disconnect;
}

void
ags_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->list_cards = ags_devout_list_cards;
  soundcard->pcm_info = ags_devout_pcm_info;
  soundcard->play_init = ags_devout_alsa_init;
  soundcard->play = ags_devout_alsa_play;
  soundcard->stop = ags_devout_alsa_free;
  soundcard->get_buffer = ags_devout_get_buffer;
  soundcard->get_next_buffer = ags_devout_get_next_buffer;
  soundcard->set_note_offset = ags_devout_set_note_offset;
  soundcard->get_note_offset = ags_devout_get_note_offset;
}

void
ags_devout_init(AgsDevout *devout)
{
  gdouble delay;
  guint default_tact_frames;
  guint default_period;
  guint i;
  
  /* flags */
  devout->flags = (AGS_DEVOUT_ALSA);

  /* quality */
  devout->dsp_channels = AGS_DEVOUT_DEFAULT_DSP_CHANNELS;
  devout->pcm_channels = AGS_DEVOUT_DEFAULT_PCM_CHANNELS;
  devout->bits = AGS_DEVOUT_DEFAULT_FORMAT;
  devout->buffer_size = AGS_DEVOUT_DEFAULT_BUFFER_SIZE;
  devout->frequency = AGS_DEVOUT_DEFAULT_SAMPLERATE;

  //  devout->out.oss.device = NULL;
  devout->out.alsa.handle = NULL;
  devout->out.alsa.device = AGS_DEVOUT_DEFAULT_DEVICE;

  /* buffer */
  devout->buffer = (signed short **) malloc(4 * sizeof(signed short*));
  devout->buffer[0] = (signed short *) malloc(devout->dsp_channels * devout->buffer_size * sizeof(signed short));
  devout->buffer[1] = (signed short *) malloc(devout->dsp_channels * devout->buffer_size * sizeof(signed short));
  devout->buffer[2] = (signed short *) malloc(devout->dsp_channels * devout->buffer_size * sizeof(signed short));
  devout->buffer[3] = (signed short *) malloc(devout->dsp_channels * devout->buffer_size * sizeof(signed short));

  /* bpm */
  devout->bpm = AGS_DEVOUT_DEFAULT_BPM;

  /* delay and attack */
  devout->delay = (gdouble *) malloc((int) 2 * AGS_DEVOUT_DEFAULT_PERIOD *
				     sizeof(gdouble));
  
  devout->attack = (guint *) malloc((int) 2 * AGS_DEVOUT_DEFAULT_PERIOD *
				    sizeof(guint));

  devout->note_offset = 0;
  
  delay = ((gdouble) devout->frequency / (gdouble) devout->buffer_size) * (gdouble)(60.0 / devout->bpm);
  //  g_message("delay : %f\0", delay);
  default_tact_frames = (guint) (delay * devout->buffer_size);
  default_period = (1.0 / AGS_DEVOUT_DEFAULT_PERIOD) * (default_tact_frames);

  devout->attack[0] = 0;
  devout->delay[0] = delay;
  
  for(i = 1; i < (int)  2.0 * AGS_DEVOUT_DEFAULT_PERIOD; i++){
    devout->attack[i] = (guint) ((i * default_tact_frames + devout->attack[i - 1]) / (AGS_DEVOUT_DEFAULT_PERIOD / (delay * i))) % (guint) (devout->buffer_size);
    //    g_message("%d\0", devout->attack[i]);
  }
  
  for(i = 1; i < (int) 2.0 * AGS_DEVOUT_DEFAULT_PERIOD; i++){
    devout->delay[i] = ((gdouble) (default_tact_frames + devout->attack[i])) / (gdouble) devout->buffer_size;
    //    g_message("%f\0", devout->delay[i]);
  }

  /*  */
  devout->delay_counter = 0;
  devout->tic_counter = 0;

  /* parent */
  devout->application_context = NULL;
  devout->application_mutex = NULL;

  /* all AgsAudio */
  devout->audio = NULL;
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
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = g_value_get_object(value);

      if(devout->application_context == application_context){
	return;
      }

      if(devout->application_context != NULL){
	g_object_unref(G_OBJECT(devout->application_context));
      }

      if(application_context != NULL){
	AgsConfig *config;
	
	g_object_ref(G_OBJECT(application_context));

	devout->application_mutex = &(application_context->mutex);
	
	config = application_context->config;
	
	devout->dsp_channels = g_ascii_strtoull(ags_config_get_value(config,
								     AGS_CONFIG_DEVOUT,
								     "dsp-channels\0"),
						NULL,
						10);
	devout->pcm_channels = g_ascii_strtoull(ags_config_get_value(config,
								     AGS_CONFIG_DEVOUT,
								     "pcm-channels\0"),
						NULL,
						10);
	devout->bits = AGS_DEVOUT_DEFAULT_FORMAT;
	devout->buffer_size = g_ascii_strtoull(ags_config_get_value(config,
								    AGS_CONFIG_DEVOUT,
								    "buffer-size\0"),
					       NULL,
					       10);
	devout->frequency = g_ascii_strtoull(ags_config_get_value(config,
								  AGS_CONFIG_DEVOUT,
								  "samplerate\0"),
					     NULL,
					     10);

	//  devout->out.oss.device = NULL;
	devout->out.alsa.handle = NULL;
	devout->out.alsa.device = g_strdup(ags_config_get_value(config,
								AGS_CONFIG_DEVOUT,
								"alsa-handle\0"));
      }else{
	devout->application_mutex = NULL;
      }

      devout->application_context = application_context;
    }
    break;
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      if((AGS_DEVOUT_LIBAO & (devout->flags)) != 0){
	//TODO:JK: implement me
      }else if((AGS_DEVOUT_OSS & (devout->flags)) != 0){
	devout->out.oss.device = g_strdup(device);
      }else if((AGS_DEVOUT_ALSA & (devout->flags)) != 0){
	devout->out.alsa.device = g_strdup(device);
      }
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      if(dsp_channels == devout->dsp_channels){
	return;
      }

      devout->dsp_channels = dsp_channels;
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels;

      pcm_channels = g_value_get_uint(value);

      if(pcm_channels == devout->pcm_channels){
	return;
      }

      devout->pcm_channels = pcm_channels;

      free(devout->buffer[0]);
      free(devout->buffer[1]);
      free(devout->buffer[2]);
      free(devout->buffer[3]);

      devout->buffer[0] = (signed short *) malloc((pcm_channels * devout->buffer_size) * sizeof(signed short));
      devout->buffer[1] = (signed short *) malloc((pcm_channels * devout->buffer_size) * sizeof(signed short));
      devout->buffer[2] = (signed short *) malloc((pcm_channels * devout->buffer_size) * sizeof(signed short));
      devout->buffer[3] = (signed short *) malloc((pcm_channels * devout->buffer_size) * sizeof(signed short));
    }
    break;
  case PROP_BITS:
    {
	//TODO:JK: implement me
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      if(buffer_size == devout->buffer_size){
	return;
      }

      devout->buffer_size = buffer_size;

      free(devout->buffer[0]);
      free(devout->buffer[1]);
      free(devout->buffer[2]);
      free(devout->buffer[3]);

      devout->buffer[0] = (signed short *) malloc((devout->pcm_channels * buffer_size) * sizeof(signed short));
      devout->buffer[1] = (signed short *) malloc((devout->pcm_channels * buffer_size) * sizeof(signed short));
      devout->buffer[2] = (signed short *) malloc((devout->pcm_channels * buffer_size) * sizeof(signed short));
      devout->buffer[3] = (signed short *) malloc((devout->pcm_channels * buffer_size) * sizeof(signed short));
    }
    break;
  case PROP_FREQUENCY:
    {
      guint frequency;

      frequency = g_value_get_uint(value);

      if(frequency == devout->frequency){
	return;
      }

      devout->frequency = frequency;
    }
    break;
  case PROP_BUFFER:
    {
	//TODO:JK: implement me
    }
    break;
  case PROP_BPM:
    {
	//TODO:JK: implement me
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
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, devout->application_context);
    }
    break;
  case PROP_DEVICE:
    {
      if((AGS_DEVOUT_LIBAO & (devout->flags)) != 0){
	//TODO:JK: implement me
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

  /* call parent */
  G_OBJECT_CLASS(ags_devout_parent_class)->finalize(gobject);
}

void
ags_devout_connect(AgsConnectable *connectable)
{
  AgsDevout *devout;
  GList *list;

  devout = AGS_DEVOUT(connectable);
  
  list = devout->audio;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_devout_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_devout_play_domain_alloc:
 *
 * Allocs an #AgsDevoutPlayDomain.
 *
 * Returns: the devout play domain
 *
 * Since: 0.4
 */
AgsDevoutPlayDomain*
ags_devout_play_domain_alloc()
{
  AgsDevoutPlayDomain *devout_play_domain;

  devout_play_domain = (AgsDevoutPlayDomain *) malloc(sizeof(AgsDevoutPlayDomain));

  devout_play_domain->domain = NULL;

  devout_play_domain->playback = FALSE;
  devout_play_domain->sequencer = FALSE;
  devout_play_domain->notation = FALSE;

  devout_play_domain->devout_play = NULL;

  return(devout_play_domain);
}

/**
 * ags_devout_play_domain_free:
 * @devout_play_domain: the devout play domain
 *
 * Frees an #AgsDevoutPlayDomain.
 *
 * Since: 0.4
 */
void
ags_devout_play_domain_free(AgsDevoutPlayDomain *devout_play_domain)
{
  g_list_free(devout_play_domain->devout_play);

  free(devout_play_domain);
}


/**
 * ags_devout_play_alloc:
 *
 * Allocs an #AgsDevoutPlay.
 *
 * Returns: the devout play
 *
 * Since: 0.4
 */
AgsDevoutPlay*
ags_devout_play_alloc()
{
  AgsDevoutPlay *play;

  play = (AgsDevoutPlay *) malloc(sizeof(AgsDevoutPlay));

  play->flags = 0;

  play->iterator_thread = (AgsIteratorThread **) malloc(3 * sizeof(AgsIteratorThread *));

  play->iterator_thread[0] = ags_iterator_thread_new();
  play->iterator_thread[1] = ags_iterator_thread_new();
  play->iterator_thread[2] = ags_iterator_thread_new();

  play->source = NULL;
  play->audio_channel = 0;

  play->recall_id = (AgsDevoutPlay **) malloc(3 * sizeof(AgsDevoutPlay *));

  play->recall_id[0] = NULL;
  play->recall_id[1] = NULL;
  play->recall_id[2] = NULL;

  return(play);
}

/**
 * ags_devout_play_free:
 * @devout_play: the devout play
 *
 * Frees an #AgsDevoutPlay.
 *
 * Since: 0.4
 */
void
ags_devout_play_free(AgsDevoutPlay *play)
{
  g_object_unref(G_OBJECT(play->iterator_thread[0]));
  g_object_unref(G_OBJECT(play->iterator_thread[1]));
  g_object_unref(G_OBJECT(play->iterator_thread[2]));

  free(play->iterator_thread);
}

/**
 * ags_devout_play_find_source:
 * @devout_play: a #GList containing #AgsDevoutPlay-struct
 * 
 * Find source
 *
 * Returns: the matching devout play
 *
 * Since: 0.4
 */
AgsDevoutPlay*
ags_devout_play_find_source(GList *devout_play,
			    GObject *source)
{
  while(devout_play != NULL){
    if(AGS_DEVOUT_PLAY(devout_play->data)->source == source){
      return(devout_play->data);
    }

    devout_play = devout_play->next;
  }

  return(NULL);
}

/**
 * ags_devout_list_cards:
 * @soundcard: the #AgsSoundcard
 * @card_id: alsa identifier
 * @card_name: card name
 *
 * List available soundcards.
 *
 * Since: 0.4
 */
void
ags_devout_list_cards(AgsSoundcard *soundcard,
		      GList **card_id, GList **card_name)
{
  snd_ctl_t *card_handle;
  snd_ctl_card_info_t *card_info;
  char *name;
  gchar *str;
  int card_num;
  int error;

  *card_id = NULL;
  *card_name = NULL;
  card_num = -1;

  while(TRUE){
    error = snd_card_next(&card_num);

    if(card_num < 0){
      break;
    }

    if(error < 0){
      continue;
    }

    str = g_strdup_printf("hw:%i\0", card_num);
    error = snd_ctl_open(&card_handle, str, 0);

    if(error < 0){
      continue;
    }

    snd_ctl_card_info_alloca(&card_info);
    error = snd_ctl_card_info(card_handle, card_info);

    if(error < 0){
      continue;
    }

    *card_id = g_list_prepend(*card_id, str);
    *card_name = g_list_prepend(*card_name, g_strdup(snd_ctl_card_info_get_name(card_info)));

    snd_ctl_close(card_handle);
  }

  snd_config_update_free_global();

  *card_id = g_list_reverse(*card_id);
  *card_name = g_list_reverse(*card_name);
}

/**
 * ags_devout_pcm_info:
 * @soundcard: the #AgsSoundcard
 * @card_id: alsa identifier
 * @channels_min: minimum channels supported
 * @channels_max: maximum channels supported
 * @rate_min: minimum samplerate supported
 * @rate_max: maximum samplerate supported
 * @buffer_size_min: minimum buffer size supported
 * @buffer_size_max maximum buffer size supported
 * @error: on success %NULL
 *
 * List soundcard settings.
 *
 * Since: 0.4
 */
void
ags_devout_pcm_info(AgsSoundcard *soundcard,
		    char *card_id,
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
ags_devout_alsa_init(AgsSoundcard *soundcard,
		     GError **error)
{
  AgsDevout *devout;
  
  int rc;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *hwparams;
  unsigned int val;
  snd_pcm_uframes_t frames;
  unsigned int rate;
  unsigned int rrate;
  unsigned int channels;
  snd_pcm_uframes_t size;
  snd_pcm_sframes_t buffer_size;
  snd_pcm_sframes_t period_size;
  snd_pcm_sw_params_t *swparams;
  int period_event = 0;
  int err, dir;

  static unsigned int period_time = 100000;
  static snd_pcm_format_t format = SND_PCM_FORMAT_S16;

  devout = AGS_DEVOUT(soundcard);

  devout->note_offset = 0;
  
  /* Open PCM device for playback. */
  if ((err = snd_pcm_open(&handle, devout->out.alsa.device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
    printf("Playback open error: %s\n", snd_strerror(err));
    g_set_error(error,
		AGS_DEVOUT_ERROR,
		AGS_DEVOUT_ERROR_LOCKED_SOUNDCARD,
		"unable to open pcm device: %s\n\0",
		snd_strerror(err));
    return;
  }

  snd_pcm_hw_params_alloca(&hwparams);
  snd_pcm_sw_params_alloca(&swparams);

  /* choose all parameters */
  err = snd_pcm_hw_params_any(handle, hwparams);
  if (err < 0) {
    printf("Broken configuration for playback: no configurations available: %s\n", snd_strerror(err));
    return;
  }

  /* set hardware resampling */
  err = snd_pcm_hw_params_set_rate_resample(handle, hwparams, 1);
  if (err < 0) {
    printf("Resampling setup failed for playback: %s\n", snd_strerror(err));
    return;
  }

  /* set the interleaved read/write format */
  err = snd_pcm_hw_params_set_access(handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
  if (err < 0) {
    printf("Access type not available for playback: %s\n", snd_strerror(err));
    return;
  }

  /* set the sample format */
  err = snd_pcm_hw_params_set_format(handle, hwparams, format);
  if (err < 0) {
    printf("Sample format not available for playback: %s\n", snd_strerror(err));
    return;
  }

  /* set the count of channels */
  channels = devout->dsp_channels;
  err = snd_pcm_hw_params_set_channels(handle, hwparams, channels);
  if (err < 0) {
    printf("Channels count (%i) not available for playbacks: %s\n", channels, snd_strerror(err));
    return;
  }

  /* set the stream rate */
  rate = devout->frequency;
  rrate = rate;
  err = snd_pcm_hw_params_set_rate_near(handle, hwparams, &rrate, 0);
  if (err < 0) {
    printf("Rate %iHz not available for playback: %s\n", rate, snd_strerror(err));
    return;
  }

  if (rrate != rate) {
    printf("Rate doesn't match (requested %iHz, get %iHz)\n", rate, err);
    //    exit(-EINVAL);
    return;
  }

  /* set the buffer size */
  size = devout->buffer_size;
  err = snd_pcm_hw_params_set_buffer_size(handle, hwparams, size);
  if (err < 0) {
    printf("Unable to set buffer size %i for playback: %s\n", size, snd_strerror(err));
    return;
  }

  buffer_size = size;

  /* set the period time */
  period_time = MSEC_PER_SEC / devout->frequency;
  dir = -1;
  err = snd_pcm_hw_params_set_period_time_near(handle, hwparams, &period_time, &dir);
  if (err < 0) {
    printf("Unable to set period time %i for playback: %s\n", period_time, snd_strerror(err));
    return;
  }

  err = snd_pcm_hw_params_get_period_size(hwparams, &size, &dir);
  if (err < 0) {
    printf("Unable to get period size for playback: %s\n", snd_strerror(err));
    return;
  }
  period_size = size;

  /* write the parameters to device */
  err = snd_pcm_hw_params(handle, hwparams);
  if (err < 0) {
    printf("Unable to set hw params for playback: %s\n", snd_strerror(err));
    return;
  }

  /* get the current swparams */
  err = snd_pcm_sw_params_current(handle, swparams);
  if (err < 0) {
    printf("Unable to determine current swparams for playback: %s\n", snd_strerror(err));
    return;
  }

  /* start the transfer when the buffer is almost full: */
  /* (buffer_size / avail_min) * avail_min */
  err = snd_pcm_sw_params_set_start_threshold(handle, swparams, (buffer_size / period_size) * period_size);
  if (err < 0) {
    printf("Unable to set start threshold mode for playback: %s\n", snd_strerror(err));
    return;
  }

  /* allow the transfer when at least period_size samples can be processed */
  /* or disable this mechanism when period event is enabled (aka interrupt like style processing) */
  err = snd_pcm_sw_params_set_avail_min(handle, swparams, period_event ? buffer_size : period_size);
  if (err < 0) {
    printf("Unable to set avail min for playback: %s\n", snd_strerror(err));
    return;
  }

  /* write the parameters to the playback device */
  err = snd_pcm_sw_params(handle, swparams);
  if (err < 0) {
    printf("Unable to set sw params for playback: %s\n", snd_strerror(err));
    return;
  }

  /*  */
  devout->out.alsa.handle = handle;
  devout->delay_counter = 0.0;
  devout->tic_counter = 0;
}

void
ags_devout_alsa_play(AgsSoundcard *soundcard,
		     GError **error)
{
  AgsDevout *devout;
  gdouble delay;

  devout = AGS_DEVOUT(soundcard);
  
  /*  */
  if((AGS_DEVOUT_BUFFER0 & (devout->flags)) != 0){
    memset(devout->buffer[3], 0, (size_t) devout->dsp_channels * devout->buffer_size * sizeof(signed short));
      
    devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle,
					 devout->buffer[0],
					 (snd_pcm_uframes_t) (devout->buffer_size));

    if((AGS_DEVOUT_NONBLOCKING & (devout->flags)) == 0){
      if(devout->out.alsa.rc == -EPIPE){
	/* EPIPE means underrun */
	snd_pcm_prepare(devout->out.alsa.handle);

#ifdef AGS_DEBUG
	g_message("underrun occurred\0");
#endif
      }else if(devout->out.alsa.rc == -ESTRPIPE){
	static const struct timespec idle = {
	  0,
	  4000,
	};

	int err;

	while((err = snd_pcm_resume(devout->out.alsa.handle)) == -EAGAIN)
	  nanosleep(&idle, NULL); /* wait until the suspend flag is released */
	if(err < 0){
	  err = snd_pcm_prepare(devout->out.alsa.handle);
	}
      }else if(devout->out.alsa.rc < 0){
	g_message("error from writei: %s\0", snd_strerror(devout->out.alsa.rc));
      }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
	g_message("short write, write %d frames\0", devout->out.alsa.rc);
      }
    }      
    //      g_message("ags_devout_play 0\0");
  }else if((AGS_DEVOUT_BUFFER1 & (devout->flags)) != 0){
    memset(devout->buffer[0], 0, (size_t) devout->dsp_channels * devout->buffer_size * sizeof(signed short));

    devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle,
					 devout->buffer[1],
					 (snd_pcm_uframes_t) (devout->buffer_size));
     
    if((AGS_DEVOUT_NONBLOCKING & (devout->flags)) == 0){
      if(devout->out.alsa.rc == -EPIPE){
	/* EPIPE means underrun */
	snd_pcm_prepare(devout->out.alsa.handle);

#ifdef AGS_DEBUG
	g_message("underrun occurred\0");
#endif
      }else if(devout->out.alsa.rc == -ESTRPIPE){
	static const struct timespec idle = {
	  0,
	  4000,
	};

	int err;	

	while((err = snd_pcm_resume(devout->out.alsa.handle)) == -EAGAIN)
	  nanosleep(&idle, NULL); /* wait until the suspend flag is released */
	if(err < 0){
	  err = snd_pcm_prepare(devout->out.alsa.handle);
	}
      }else if(devout->out.alsa.rc < 0){
	g_message("error from writei: %s\0", snd_strerror(devout->out.alsa.rc));
      }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
	g_message("short write, write %d frames\0", devout->out.alsa.rc);
      }
    }      
    //      g_message("ags_devout_play 1\0");
  }else if((AGS_DEVOUT_BUFFER2 & (devout->flags)) != 0){
    memset(devout->buffer[1], 0, (size_t) devout->dsp_channels * devout->buffer_size * sizeof(signed short));
      
    devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle,
					 devout->buffer[2],
					 (snd_pcm_uframes_t) (devout->buffer_size));

    if((AGS_DEVOUT_NONBLOCKING & (devout->flags)) == 0){
      if(devout->out.alsa.rc == -EPIPE){
	/* EPIPE means underrun */
	snd_pcm_prepare(devout->out.alsa.handle);

#ifdef AGS_DEBUG
	g_message("underrun occurred\0");
#endif
      }else if(devout->out.alsa.rc == -ESTRPIPE){
	static const struct timespec idle = {
	  0,
	  4000,
	};

	int err;

	while((err = snd_pcm_resume(devout->out.alsa.handle)) == -EAGAIN)
	  nanosleep(&idle, NULL); /* wait until the suspend flag is released */
	if(err < 0){
	  err = snd_pcm_prepare(devout->out.alsa.handle);
	}
      }else if(devout->out.alsa.rc < 0){
	g_message("error from writei: %s\0", snd_strerror(devout->out.alsa.rc));
      }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
	g_message("short write, write %d frames\0", devout->out.alsa.rc);
      }
    }
    //      g_message("ags_devout_play 2\0");
  }else if((AGS_DEVOUT_BUFFER3 & devout->flags) != 0){
    memset(devout->buffer[2], 0, (size_t) devout->dsp_channels * devout->buffer_size * sizeof(signed short));
      
    devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle,
					 devout->buffer[3],
					 (snd_pcm_uframes_t) (devout->buffer_size));

    if((AGS_DEVOUT_NONBLOCKING & (devout->flags)) == 0){
      if(devout->out.alsa.rc == -EPIPE){
	snd_pcm_prepare(devout->out.alsa.handle);

#ifdef AGS_DEBUG
	g_message("underrun occurred\0");
#endif
      }else if(devout->out.alsa.rc == -ESTRPIPE){
	static const struct timespec idle = {
	  0,
	  4000,
	};

	int err;

	while((err = snd_pcm_resume(devout->out.alsa.handle)) == -EAGAIN)
	  nanosleep(&idle, NULL); /* wait until the suspend flag is released */
	if(err < 0){
	  err = snd_pcm_prepare(devout->out.alsa.handle);
	}
      }else if(devout->out.alsa.rc < 0){
	g_message("error from writei: %s\0", snd_strerror(devout->out.alsa.rc));
      }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
	g_message("short write, write %d frames\0", devout->out.alsa.rc);
      }
    }
    //      g_message("ags_devout_play 3\0");
  }

  /* determine if attack should be switched */
  delay = devout->delay[devout->tic_counter];
  devout->delay_counter += 1.0;

  ///TODO:JK: fix me
  if(devout->delay_counter >= delay){ //devout->delay[devout->tic_counter]
    devout->tic_counter += 1;

    if(devout->tic_counter == AGS_DEVOUT_DEFAULT_PERIOD){
      devout->tic_counter = 0;
    }

    /* delay */
    devout->delay_counter = 0.0;
    ags_soundcard_offset_changed(soundcard,
				 devout->note_offset);
  } 

  /* tic */
  ags_soundcard_tic(soundcard);

  /* switch buffer flags */
  ags_devout_switch_buffer_flag(devout);

  snd_pcm_prepare(devout->out.alsa.handle);
}

void
ags_devout_alsa_free(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(soundcard);
  
  snd_pcm_drain(devout->out.alsa.handle);
  snd_pcm_close(devout->out.alsa.handle);
  devout->out.alsa.handle = NULL;
  devout->flags &= (~(AGS_DEVOUT_BUFFER0 &
		      AGS_DEVOUT_BUFFER1 &
		      AGS_DEVOUT_BUFFER2 &
		      AGS_DEVOUT_BUFFER3));
}

signed short*
ags_devout_get_buffer(AgsSoundcard *soundcard)
{
  AgsDevout *devout;
  signed short *buffer;
  
  devout = AGS_DEVOUT(soundcard);

  if((AGS_DEVOUT_BUFFER0 & (devout->flags)) != 0){
    buffer = devout->buffer[0];
  }else if((AGS_DEVOUT_BUFFER1 & (devout->flags)) != 0){
    buffer = devout->buffer[1];
  }else if((AGS_DEVOUT_BUFFER2 & (devout->flags)) != 0){
    buffer = devout->buffer[2];
  }else if((AGS_DEVOUT_BUFFER3 & (devout->flags)) != 0){
    buffer = devout->buffer[3];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

signed short*
ags_devout_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsDevout *devout;
  signed short *buffer;
  
  devout = AGS_DEVOUT(soundcard);

  if((AGS_DEVOUT_BUFFER0 & (devout->flags)) != 0){
    buffer = devout->buffer[1];
  }else if((AGS_DEVOUT_BUFFER1 & (devout->flags)) != 0){
    buffer = devout->buffer[2];
  }else if((AGS_DEVOUT_BUFFER2 & (devout->flags)) != 0){
    buffer = devout->buffer[3];
  }else if((AGS_DEVOUT_BUFFER3 & (devout->flags)) != 0){
    buffer = devout->buffer[0];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void
ags_devout_set_note_offset(AgsSoundcard *soundcard,
			   guint note_offset)
{
  AGS_DEVOUT(soundcard)->note_offset = note_offset;
}

guint
ags_devout_get_note_offset(AgsSoundcard *soundcard)
{
  return(AGS_DEVOUT(soundcard)->note_offset);
}

/**
 * ags_devout_switch_buffer_flag:
 * @devout: an #AgsDevout
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 0.3
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

/**
 * ags_devout_add_audio:
 * @devout: an #AgsDevout
 * @audio: the #AgsAudio to add
 *
 * Add audio to devout.
 *
 * Since: 0.4
 */
void
ags_devout_add_audio(AgsDevout *devout, GObject *audio)
{
  if(g_list_find(devout->audio,
		 audio) != NULL){
    return;
  }

  g_object_ref(G_OBJECT(audio));
  devout->audio = g_list_prepend(devout->audio,
				 audio);
}

/**
 * ags_devout_remove_audio:
 * @devout: an #AgsDevout
 * @audio: the #AgsAudio to remove
 *
 * Remove audio of devout.
 *
 * Since: 0.4
 */
void
ags_devout_remove_audio(AgsDevout *devout, GObject *audio)
{
  devout->audio = g_list_remove(devout->audio,
				audio);
  g_object_unref(G_OBJECT(audio));
}

/**
 * ags_devout_new:
 * @application_context: the #AgsApplicationContext
 *
 * Creates an #AgsDevout, refering to @application_context.
 *
 * Returns: a new #AgsDevout
 *
 * Since: 0.3
 */
AgsDevout*
ags_devout_new(GObject *application_context)
{
  AgsDevout *devout;

  devout = (AgsDevout *) g_object_new(AGS_TYPE_DEVOUT,
				      "application_context\0", application_context,
				      NULL);
  
  return(devout);
}
