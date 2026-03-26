/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/audio/fx/ags_fx_eq10_audio_signal.h>

#include <ags/audio/ags_port.h>

#include <ags/audio/fx/ags_fx_eq10_audio.h>
#include <ags/audio/fx/ags_fx_eq10_channel.h>
#include <ags/audio/fx/ags_fx_eq10_channel_processor.h>
#include <ags/audio/fx/ags_fx_eq10_recycling.h>

#include <ags/i18n.h>

void ags_fx_eq10_audio_signal_class_init(AgsFxEq10AudioSignalClass *fx_eq10_audio_signal);
void ags_fx_eq10_audio_signal_init(AgsFxEq10AudioSignal *fx_eq10_audio_signal);
void ags_fx_eq10_audio_signal_dispose(GObject *gobject);
void ags_fx_eq10_audio_signal_finalize(GObject *gobject);

void ags_fx_eq10_audio_signal_real_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_fx_eq10_audio_signal
 * @short_description: fx eq10 audio signal
 * @title: AgsFxEq10AudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_eq10_audio_signal.h
 *
 * The #AgsFxEq10AudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_eq10_audio_signal_parent_class = NULL;

const gchar *ags_fx_eq10_audio_signal_plugin_name = "ags-fx-eq10";

GType
ags_fx_eq10_audio_signal_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_eq10_audio_signal = 0;

    static const GTypeInfo ags_fx_eq10_audio_signal_info = {
      sizeof (AgsFxEq10AudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_eq10_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxEq10AudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_eq10_audio_signal_init,
    };

    ags_type_fx_eq10_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							   "AgsFxEq10AudioSignal",
							   &ags_fx_eq10_audio_signal_info,
							   0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_eq10_audio_signal);
  }

  return(g_define_type_id__static);
}

void
ags_fx_eq10_audio_signal_class_init(AgsFxEq10AudioSignalClass *fx_eq10_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_fx_eq10_audio_signal_parent_class = g_type_class_peek_parent(fx_eq10_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_eq10_audio_signal;

  gobject->dispose = ags_fx_eq10_audio_signal_dispose;
  gobject->finalize = ags_fx_eq10_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_eq10_audio_signal;
  
  recall->run_inter = ags_fx_eq10_audio_signal_real_run_inter;
}

void
ags_fx_eq10_audio_signal_init(AgsFxEq10AudioSignal *fx_eq10_audio_signal)
{
  AGS_RECALL(fx_eq10_audio_signal)->name = "ags-fx-eq10";
  AGS_RECALL(fx_eq10_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_eq10_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_eq10_audio_signal)->xml_type = "ags-fx-eq10-audio-signal";

  fx_eq10_audio_signal->amplifier10_util = AGS_AMPLIFIER10_UTIL_INITIALIZER;
}

void
ags_fx_eq10_audio_signal_dispose(GObject *gobject)
{
  AgsFxEq10AudioSignal *fx_eq10_audio_signal;
  
  fx_eq10_audio_signal = AGS_FX_EQ10_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_eq10_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_eq10_audio_signal_finalize(GObject *gobject)
{
  AgsFxEq10AudioSignal *fx_eq10_audio_signal;
  
  fx_eq10_audio_signal = AGS_FX_EQ10_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_eq10_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_eq10_audio_signal_real_run_inter(AgsRecall *recall)
{
  AgsAudioSignal *source;
  AgsFxEq10Audio *fx_eq10_audio;
  AgsFxEq10Channel *fx_eq10_channel;
  AgsFxEq10ChannelProcessor *fx_eq10_channel_processor;
  AgsFxEq10Recycling *fx_eq10_recycling;
  AgsFxEq10AudioSignal *fx_eq10_audio_signal;

  gdouble *output_buffer;
  gdouble *input_buffer;

  gint sound_scope;

  gfloat pressure;
  gfloat peak_28hz;
  gfloat peak_56hz;
  gfloat peak_112hz;
  gfloat peak_224hz;
  gfloat peak_448hz;
  gfloat peak_896hz;
  gfloat peak_1792hz;
  gfloat peak_3584hz;
  gfloat peak_7168hz;
  gfloat peak_14336hz;

  guint output_copy_mode, input_copy_mode;
  guint samplerate;
  guint buffer_size;
  guint format;  
  guint i;
  
  GRecMutex *fx_eq10_channel_mutex;
  GRecMutex *stream_mutex;

  fx_eq10_audio_signal = (AgsFxEq10AudioSignal *) recall;
  
  stream_mutex = NULL;
  
  sound_scope = ags_recall_get_sound_scope(recall);
  
  source = NULL;

  fx_eq10_audio = NULL;
  fx_eq10_channel = NULL;
  fx_eq10_channel_processor = NULL;
  fx_eq10_recycling = NULL;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  
  g_object_get(recall,
	       "parent", &fx_eq10_recycling,
	       "source", &source,
	       NULL);

  g_object_get(fx_eq10_recycling,
	       "parent", &fx_eq10_channel_processor,
	       NULL);

  g_object_get(fx_eq10_channel_processor,
	       "recall-audio", &fx_eq10_audio,
	       "recall-channel", &fx_eq10_channel,
	       NULL);

  g_object_get(source,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);
  
  ags_amplifier10_util_set_buffer_length(&(fx_eq10_audio_signal->amplifier10_util),
					 buffer_size);
  
  ags_amplifier10_util_set_format(&(fx_eq10_audio_signal->amplifier10_util),
				  AGS_SOUNDCARD_DOUBLE);
  
  ags_amplifier10_util_set_samplerate(&(fx_eq10_audio_signal->amplifier10_util),
				      samplerate);
  
  /* copy mode */
  output_copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(&(fx_eq10_audio_signal->audio_buffer_util),
								     ags_audio_buffer_util_format_from_soundcard(&(fx_eq10_audio_signal->audio_buffer_util),
														 format),
								     AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  input_copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(&(fx_eq10_audio_signal->audio_buffer_util),
								    AGS_AUDIO_BUFFER_UTIL_DOUBLE,
								    ags_audio_buffer_util_format_from_soundcard(&(fx_eq10_audio_signal->audio_buffer_util),
														format));

  output_buffer = NULL;
  input_buffer = NULL;
  
  if(fx_eq10_channel != NULL){
    fx_eq10_channel_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_eq10_channel);

    g_rec_mutex_lock(fx_eq10_channel_mutex);

    output_buffer = fx_eq10_channel->input_data[sound_scope]->output;
    input_buffer = fx_eq10_channel->input_data[sound_scope]->input;

    g_rec_mutex_unlock(fx_eq10_channel_mutex);
  }
  
  ags_amplifier10_util_set_source(&(fx_eq10_audio_signal->amplifier10_util),
				  input_buffer);
  ags_amplifier10_util_set_destination(&(fx_eq10_audio_signal->amplifier10_util),
				       output_buffer);
  
  peak_28hz = 1.0;
  peak_56hz = 1.0;
  peak_112hz = 1.0;
  peak_224hz = 1.0;
  peak_448hz = 1.0;
  peak_896hz = 1.0;
  peak_1792hz = 1.0;
  peak_3584hz = 1.0;
  peak_7168hz = 1.0;
  peak_14336hz = 1.0;

  pressure = 1.0;

  if(fx_eq10_channel != NULL){
    AgsPort *port;

    GValue value = {0,};

    /* peak 28hz */
    g_object_get(fx_eq10_channel,
		 "peak-28hz", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      peak_28hz = g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* peak 56hz */
    g_object_get(fx_eq10_channel,
		 "peak-56hz", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      peak_56hz = g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* peak 112hz */
    g_object_get(fx_eq10_channel,
		 "peak-112hz", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      peak_112hz = g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* peak 224hz */
    g_object_get(fx_eq10_channel,
		 "peak-224hz", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      peak_224hz = g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* peak 448hz */
    g_object_get(fx_eq10_channel,
		 "peak-448hz", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      peak_448hz = g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* peak 896hz */
    g_object_get(fx_eq10_channel,
		 "peak-896hz", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      peak_896hz = g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* peak 1792hz */
    g_object_get(fx_eq10_channel,
		 "peak-1792hz", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      peak_1792hz = g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* peak 3584hz */
    g_object_get(fx_eq10_channel,
		 "peak-3584hz", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      peak_3584hz = g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* peak 7168hz */
    g_object_get(fx_eq10_channel,
		 "peak-7168hz", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      peak_7168hz = g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* peak 14336hz */
    g_object_get(fx_eq10_channel,
		 "peak-14336hz", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      peak_14336hz = g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);

    /* pressure */
    g_object_get(fx_eq10_channel,
		 "pressure", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      pressure = g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);
  }
  
  ags_amplifier10_util_set_amp_0_gain(&(fx_eq10_audio_signal->amplifier10_util),
				      peak_28hz);
  
  ags_amplifier10_util_set_amp_1_gain(&(fx_eq10_audio_signal->amplifier10_util),
				      peak_56hz);
  
  ags_amplifier10_util_set_amp_2_gain(&(fx_eq10_audio_signal->amplifier10_util),
				      peak_112hz);
  
  ags_amplifier10_util_set_amp_3_gain(&(fx_eq10_audio_signal->amplifier10_util),
				      peak_224hz);
  
  ags_amplifier10_util_set_amp_4_gain(&(fx_eq10_audio_signal->amplifier10_util),
				      peak_448hz);
  
  ags_amplifier10_util_set_amp_5_gain(&(fx_eq10_audio_signal->amplifier10_util),
				      peak_896hz);
  
  ags_amplifier10_util_set_amp_6_gain(&(fx_eq10_audio_signal->amplifier10_util),
				      peak_1792hz);
  
  ags_amplifier10_util_set_amp_7_gain(&(fx_eq10_audio_signal->amplifier10_util),
				      peak_3584hz);
  
  ags_amplifier10_util_set_amp_8_gain(&(fx_eq10_audio_signal->amplifier10_util),
				      peak_7168hz);
  
  ags_amplifier10_util_set_amp_9_gain(&(fx_eq10_audio_signal->amplifier10_util),
				      peak_14336hz);
  
  ags_amplifier10_util_set_filter_gain(&(fx_eq10_audio_signal->amplifier10_util),
				       pressure);
  
  if(fx_eq10_channel != NULL &&
     source != NULL &&
     source->stream_current != NULL){
    stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(source);

    g_rec_mutex_lock(fx_eq10_channel_mutex);
    
    /* clear/copy - preserve trailing */
    if(buffer_size > 8){
      /* clear buffer */
      ags_audio_buffer_util_clear_double(&(fx_eq10_audio_signal->audio_buffer_util),
					 input_buffer, 1,
					 buffer_size - 2);
    
      /* copy input */
      g_rec_mutex_lock(stream_mutex);
      
      ags_audio_buffer_util_copy_buffer_to_buffer(&(fx_eq10_audio_signal->audio_buffer_util),
						  input_buffer, 1, 0,
						  source->stream_current->data, 1, 0,
						  buffer_size - 2, input_copy_mode);
      
      g_rec_mutex_unlock(stream_mutex);
    }

    /* equalizer */
    ags_amplifier10_util_process(&(fx_eq10_audio_signal->amplifier10_util));

    /* clear buffer and copy output  */
    g_rec_mutex_lock(stream_mutex);

    ags_audio_buffer_util_clear_buffer(&(fx_eq10_audio_signal->audio_buffer_util),
				       source->stream_current->data, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(&(fx_eq10_audio_signal->audio_buffer_util),
												format));
    
    ags_audio_buffer_util_copy_buffer_to_buffer(&(fx_eq10_audio_signal->audio_buffer_util),
						source->stream_current->data, 1, 0,
						output_buffer, 1, 0,
						buffer_size, output_copy_mode);

    g_rec_mutex_unlock(stream_mutex);

    g_rec_mutex_unlock(fx_eq10_channel_mutex);
  }
  
  if(source == NULL ||
     source->stream_current == NULL){
    ags_recall_done(recall);
  }

  /* unref */  
  if(source != NULL){
    g_object_unref(source);
  }
  
  if(fx_eq10_audio != NULL){
    g_object_unref(fx_eq10_audio);
  }
  
  if(fx_eq10_channel != NULL){
    g_object_unref(fx_eq10_channel);
  }

  if(fx_eq10_channel_processor != NULL){
    g_object_unref(fx_eq10_channel_processor);
  }

  if(fx_eq10_recycling != NULL){
    g_object_unref(fx_eq10_recycling);
  }

  /* call parent */
  AGS_RECALL_CLASS(ags_fx_eq10_audio_signal_parent_class)->run_inter(recall);
}

/**
 * ags_fx_eq10_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxEq10AudioSignal
 *
 * Returns: the new #AgsFxEq10AudioSignal
 *
 * Since: 3.3.0
 */
AgsFxEq10AudioSignal*
ags_fx_eq10_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxEq10AudioSignal *fx_eq10_audio_signal;

  fx_eq10_audio_signal = (AgsFxEq10AudioSignal *) g_object_new(AGS_TYPE_FX_EQ10_AUDIO_SIGNAL,
							       "source", audio_signal,
							       NULL);

  return(fx_eq10_audio_signal);
}
