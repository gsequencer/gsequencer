/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/audio/ags_libao.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>

#include <ags/main.h>

#include <string.h>
#include <math.h>
#include <time.h>

#include <ags/audio/ags_notation.h>

void ags_libao_class_init(AgsLibaoClass *libao);
void ags_libao_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_libao_init(AgsLibao *libao);
void ags_libao_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_libao_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_libao_disconnect(AgsConnectable *connectable);
void ags_libao_connect(AgsConnectable *connectable);
void ags_libao_finalize(GObject *gobject);

void ags_libao_real_change_bpm(AgsLibao *libao, double bpm);

void ags_libao_switch_buffer_flag(AgsLibao *libao);

void ags_libao_output_init(AgsLibao *libao,
			  GError **error);
void ags_libao_output_play(AgsLibao *libao,
			  GError **error);
void ags_libao_output_free(AgsLibao *libao);

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

enum{
  RUN,
  STOP,
  TIC,
  LAST_SIGNAL,
};

static gpointer ags_libao_parent_class = NULL;
static guint libao_signals[LAST_SIGNAL];

/* dangerous - produces a lot of output */
static gboolean DEBUG_LIBAO = FALSE;

GType
ags_libao_get_type (void)
{
  static GType ags_type_libao = 0;

  if(!ags_type_libao){
    static const GTypeInfo ags_libao_info = {
      sizeof (AgsLibaoClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_libao_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLibao),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_libao_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_libao_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_libao = g_type_register_static(G_TYPE_OBJECT,
					     "AgsLibao\0",
					     &ags_libao_info,
					     0);

    g_type_add_interface_static(ags_type_libao,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_libao);
}

void
ags_libao_class_init(AgsLibaoClass *libao)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_libao_parent_class = g_type_class_peek_parent(libao);

  /* GObjectClass */
  gobject = (GObjectClass *) libao;

  gobject->set_property = ags_libao_set_property;
  gobject->get_property = ags_libao_get_property;

  gobject->finalize = ags_libao_finalize;

  /* properties */
  param_spec = g_param_spec_object("application-context\0",
				   "the application context object\0",
				   "The application context object\0",
				   AGS_TYPE_APPLICATION_CONTEXT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

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


  /* AgsLibaoClass */
  libao->play_init = ags_libao_output_init;
  libao->play = ags_libao_output_play;
  libao->stop = ags_libao_output_free;

  libao->tic = NULL;
  libao->note_offset_changed = NULL;

  libao_signals[TIC] =
    g_signal_new("tic\0",
		 G_TYPE_FROM_CLASS (libao),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsLibaoClass, tic),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

GQuark
ags_libao_error_quark()
{
  return(g_quark_from_static_string("agsao-error-quark\0"));
}

void
ags_libao_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_libao_connect;
  connectable->disconnect = ags_libao_disconnect;
}

void
ags_libao_init(AgsLibao *libao)
{
  guint default_tact_frames;
  guint default_tic_frames;
  guint i;
  
  /* flags */
  libao->flags = (AGS_LIBAO_ALSA);

  /* quality */
  libao->dsp_channels = 2;
  libao->pcm_channels = 2;
  libao->bits = 16;
  libao->buffer_size = AGS_LIBAO_DEFAULT_BUFFER_SIZE;
  libao->frequency = AGS_LIBAO_DEFAULT_SAMPLERATE;

  //  libao->out.ao.device = NULL;

  /* buffer */
  libao->buffer = (signed short **) malloc(4 * sizeof(signed short*));
  libao->buffer[0] = (signed short *) malloc(libao->dsp_channels * libao->buffer_size * sizeof(signed short));
  libao->buffer[1] = (signed short *) malloc(libao->dsp_channels * libao->buffer_size * sizeof(signed short));
  libao->buffer[2] = (signed short *) malloc(libao->dsp_channels * libao->buffer_size * sizeof(signed short));
  libao->buffer[3] = (signed short *) malloc(libao->dsp_channels * libao->buffer_size * sizeof(signed short));

  /* bpm */
  libao->bpm = AGS_LIBAO_DEFAULT_BPM;

  /* delay and attack */
  libao->delay = (guint *) malloc((int) ceil(2.0 * AGS_NOTATION_TICS_PER_BEAT) *
				   sizeof(gdouble));

  libao->attack = (guint *) malloc((int) ceil(2.0 * AGS_NOTATION_TICS_PER_BEAT) *
				   sizeof(guint));

  default_tact_frames = (guint) (AGS_LIBAO_DEFAULT_DELAY * AGS_LIBAO_DEFAULT_BUFFER_SIZE);
  default_tic_frames = (guint) (default_tact_frames * AGS_NOTATION_MINIMUM_NOTE_LENGTH);

  memset(libao->delay, 0, (int) (ceil(2.0 * AGS_NOTATION_TICS_PER_BEAT) * sizeof(guint)));
  memset(libao->delay, 0, (int) (ceil(2.0 * AGS_NOTATION_TICS_PER_BEAT) * sizeof(guint)));

  for(i = 0; i < (int) ceil(2.0 * AGS_NOTATION_TICS_PER_BEAT); i++){
    libao->attack[i] = (i * default_tic_frames) % AGS_LIBAO_DEFAULT_BUFFER_SIZE;
  }

  for(i = 0; i < (int) ceil(2.0 * AGS_NOTATION_TICS_PER_BEAT); i++){
    //    libao->delay[i] = AGS_LIBAO_DEFAULT_BUFFER_SIZE / (default_tic_frames) / (AGS_LIBAO_DEFAULT_SAMPLERATE / AGS_NOTATION_DEFAULT_JIFFIE);
  }

  /*  */
  libao->delay_counter = 0;
  libao->tic_counter = 0;

  /* parent */
  libao->application_context = NULL;

  /* all AgsAudio */
  libao->audio = NULL;
}

void
ags_libao_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsLibao *libao;

  libao = AGS_LIBAO(gobject);

  //TODO:JK: implement set functionality
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = g_value_get_object(value);

      if(libao->application_context == application_context){
	return;
      }

      if(libao->application_context != NULL){
	g_object_unref(G_OBJECT(libao->application_context));
      }

      if(application_context != NULL){
	g_object_ref(G_OBJECT(application_context));
      }

      libao->application_context = application_context;
    }
    break;
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      if((AGS_LIBAO_LIBAO & (libao->flags)) != 0){
	//TODO:JK: implement me
      }
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      if(dsp_channels == libao->dsp_channels){
	return;
      }

      libao->dsp_channels = dsp_channels;
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels;

      pcm_channels = g_value_get_uint(value);

      if(pcm_channels == libao->pcm_channels){
	return;
      }

      libao->pcm_channels = pcm_channels;

      free(libao->buffer[0]);
      free(libao->buffer[1]);
      free(libao->buffer[2]);
      free(libao->buffer[3]);

      libao->buffer[0] = (signed short *) malloc((pcm_channels * libao->buffer_size) * sizeof(signed short));
      libao->buffer[1] = (signed short *) malloc((pcm_channels * libao->buffer_size) * sizeof(signed short));
      libao->buffer[2] = (signed short *) malloc((pcm_channels * libao->buffer_size) * sizeof(signed short));
      libao->buffer[3] = (signed short *) malloc((pcm_channels * libao->buffer_size) * sizeof(signed short));
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

      if(buffer_size == libao->buffer_size){
	return;
      }

      libao->buffer_size = buffer_size;

      free(libao->buffer[0]);
      free(libao->buffer[1]);
      free(libao->buffer[2]);
      free(libao->buffer[3]);

      libao->buffer[0] = (signed short *) malloc((libao->pcm_channels * buffer_size) * sizeof(signed short));
      libao->buffer[1] = (signed short *) malloc((libao->pcm_channels * buffer_size) * sizeof(signed short));
      libao->buffer[2] = (signed short *) malloc((libao->pcm_channels * buffer_size) * sizeof(signed short));
      libao->buffer[3] = (signed short *) malloc((libao->pcm_channels * buffer_size) * sizeof(signed short));
    }
    break;
  case PROP_FREQUENCY:
    {
      guint frequency;

      frequency = g_value_get_uint(value);

      if(frequency == libao->frequency){
	return;
      }

      libao->frequency = frequency;
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
ags_libao_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsLibao *libao;

  libao = AGS_LIBAO(gobject);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      if((AGS_LIBAO_LIBAO & (libao->flags)) != 0){
	g_value_set_string(value, ao_driver_info(libao->out.ao.driver_ao)->name);
      }
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      g_value_set_uint(value, libao->dsp_channels);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_value_set_uint(value, libao->pcm_channels);
    }
    break;
  case PROP_BITS:
    {
      g_value_set_uint(value, libao->bits);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value, libao->buffer_size);
    }
    break;
  case PROP_FREQUENCY:
    {
      g_value_set_uint(value, libao->frequency);
    }
    break;
  case PROP_BUFFER:
    {
      g_value_set_pointer(value, libao->buffer);
    }
    break;
  case PROP_BPM:
    {
      g_value_set_double(value, libao->bpm);
    }
    break;
  case PROP_ATTACK:
    {
      g_value_set_pointer(value, libao->attack);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_libao_finalize(GObject *gobject)
{
  AgsLibao *libao;
  GList *list, *list_next;

  libao = AGS_LIBAO(gobject);

  /* free output buffer */
  free(libao->buffer[0]);
  free(libao->buffer[1]);
  free(libao->buffer[2]);
  free(libao->buffer[3]);

  /* free buffer array */
  free(libao->buffer);

  /* free AgsAttack */
  free(libao->attack);

  /* call parent */
  G_OBJECT_CLASS(ags_libao_parent_class)->finalize(gobject);
}

void
ags_libao_connect(AgsConnectable *connectable)
{
  AgsLibao *libao;
  GList *list;

  libao = AGS_LIBAO(connectable);
  
  list = libao->audio;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_libao_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

AgsLibaoPlayDomain*
ags_libao_play_domain_alloc()
{
  AgsLibaoPlayDomain *libao_play_domain;

  libao_play_domain = (AgsLibaoPlayDomain *) malloc(sizeof(AgsLibaoPlayDomain));

  libao_play_domain->domain = NULL;

  libao_play_domain->playback = FALSE;
  libao_play_domain->sequencer = FALSE;
  libao_play_domain->notation = FALSE;

  libao_play_domain->libao_play = NULL;

  return(libao_play_domain);
}

void
ags_libao_play_domain_free(AgsLibaoPlayDomain *libao_play_domain)
{
  g_list_free(libao_play_domain->libao_play);

  free(libao_play_domain);
}

AgsLibaoPlay*
ags_libao_play_alloc()
{
  AgsLibaoPlay *play;

  play = (AgsLibaoPlay *) malloc(sizeof(AgsLibaoPlay));

  play->flags = 0;

  play->iterator_thread = (AgsIteratorThread **) malloc(3 * sizeof(AgsIteratorThread *));

  play->iterator_thread[0] = ags_iterator_thread_new();
  play->iterator_thread[1] = ags_iterator_thread_new();
  play->iterator_thread[2] = ags_iterator_thread_new();

  play->source = NULL;
  play->audio_channel = 0;

  play->recall_id[0] = NULL;
  play->recall_id[1] = NULL;
  play->recall_id[2] = NULL;

  return(play);
}

void
ags_libao_play_free(AgsLibaoPlay *play)
{
  g_object_unref(G_OBJECT(play->iterator_thread[0]));
  g_object_unref(G_OBJECT(play->iterator_thread[1]));
  g_object_unref(G_OBJECT(play->iterator_thread[2]));

  free(play->iterator_thread);
}

void
ags_libao_list_cards(GList **card_id, GList **card_name)
{
}

void
ags_libao_pcm_info(char *card_id,
		   guint *channels_min, guint *channels_max,
		   guint *rate_min, guint *rate_max,
		   guint *buffer_size_min, guint *buffer_size_max,
		   GError **error)
{
}

void
ags_libao_add_audio(AgsLibao *libao, GObject *audio)
{
  g_object_ref(G_OBJECT(audio));
  libao->audio = g_list_prepend(libao->audio,
				 audio);
}

void
ags_libao_remove_audio(AgsLibao *libao, GObject *audio)
{
  libao->audio = g_list_remove(libao->audio,
				audio);
  g_object_unref(G_OBJECT(audio));
}

void
ags_libao_tic(AgsLibao *libao)
{
  g_return_if_fail(AGS_IS_LIBAO(libao));

  if((AGS_LIBAO_PLAY & libao->flags) == 0){
    g_message("ags_libao_tic: not playing\0");
    return;
  }

  g_object_ref((GObject *) libao);
  g_signal_emit(G_OBJECT(libao),
		libao_signals[TIC], 0);
  g_object_unref((GObject *) libao);
}

/**
 * ags_libao_switch_buffer_flag:
 * @libao an #AgsLibao
 *
 * The buffer flag indicates the currently played buffer.
 */
void
ags_libao_switch_buffer_flag(AgsLibao *libao)
{
  if((AGS_LIBAO_BUFFER0 & (libao->flags)) != 0){
    libao->flags &= (~AGS_LIBAO_BUFFER0);
    libao->flags |= AGS_LIBAO_BUFFER1;
  }else if((AGS_LIBAO_BUFFER1 & (libao->flags)) != 0){
    libao->flags &= (~AGS_LIBAO_BUFFER1);
    libao->flags |= AGS_LIBAO_BUFFER2;
  }else if((AGS_LIBAO_BUFFER2 & (libao->flags)) != 0){
    libao->flags &= (~AGS_LIBAO_BUFFER2);
    libao->flags |= AGS_LIBAO_BUFFER3;
  }else if((AGS_LIBAO_BUFFER3 & (libao->flags)) != 0){
    libao->flags &= (~AGS_LIBAO_BUFFER3);
    libao->flags |= AGS_LIBAO_BUFFER0;
  }
}

void
ags_libao_output_init(AgsLibao *libao,
		   GError **error)
{
  ao_sample_format *format;

  format = (ao_sample_format *) malloc(sizeof(ao_sample_format));

  format->bits = libao->bits;
  format->rate = libao->frequency;
  format->channels = libao->dsp_channels;
  format->byte_format = AO_FMT_LITTLE;
  format->matrix = g_strdup("L,R\0");

  libao->out.ao.format = format;
  libao->out.ao.device = ao_open_live(libao->out.ao.driver_ao,
				      libao->out.ao.format,
				       NULL);
}

void
ags_libao_output_play(AgsLibao *libao,
		      GError **error)
{
  int rc;

  /*  */
  if((AGS_LIBAO_BUFFER0 & (libao->flags)) != 0){
    memset(libao->buffer[3], 0, (size_t) libao->dsp_channels * libao->buffer_size * sizeof(signed short));
      
    rc = ao_play(libao->out.ao.device,
		 (void *) libao->buffer[0],
		 libao->dsp_channels * libao->buffer_size * sizeof(signed short));
  }else if((AGS_LIBAO_BUFFER1 & (libao->flags)) != 0){
    memset(libao->buffer[0], 0, (size_t) libao->dsp_channels * libao->buffer_size * sizeof(signed short));

    rc = ao_play(libao->out.ao.device,
		 (void *) libao->buffer[1],
		 libao->dsp_channels * libao->buffer_size * sizeof(signed short));
  }else if((AGS_LIBAO_BUFFER2 & (libao->flags)) != 0){
    memset(libao->buffer[1], 0, (size_t) libao->dsp_channels * libao->buffer_size * sizeof(signed short));

    rc = ao_play(libao->out.ao.device,
		 (void *) libao->buffer[2],
		 libao->dsp_channels * libao->buffer_size * sizeof(signed short));
  }else if((AGS_LIBAO_BUFFER3 & libao->flags) != 0){
    memset(libao->buffer[2], 0, (size_t) libao->dsp_channels * libao->buffer_size * sizeof(signed short));

    rc = ao_play(libao->out.ao.device,
		 (void *) libao->buffer[3],
		 libao->dsp_channels * libao->buffer_size * sizeof(signed short));
  }

  /*
    if((AGS_LIBAO_COUNT & (libao->flags)) != 0)
    libao->offset++;
  */

  /* determine if attack should be switched */
  libao->delay_counter += (AGS_LIBAO_DEFAULT_DELAY *
			   AGS_NOTATION_MINIMUM_NOTE_LENGTH);

  if(libao->delay_counter >= libao->delay[libao->tic_counter]){
    /* tic */
    //    ags_libao_tic(libao);

    libao->tic_counter += 1;

    if(libao->tic_counter == AGS_NOTATION_TICS_PER_BEAT){
      libao->tic_counter = 0;
    }

    /* delay */
    libao->delay_counter = 0;
  }

  /* switch buffer flags */
  ags_libao_switch_buffer_flag(libao);
}

void
ags_libao_output_free(AgsLibao *libao)
{
  ao_close(libao->out.ao.device);
}

AgsLibao*
ags_libao_new(GObject *application_context)
{
  AgsLibao *libao;

  libao = (AgsLibao *) g_object_new(AGS_TYPE_LIBAO,
				    "application-context", application_context,
				    NULL);  

  return(libao);
}
