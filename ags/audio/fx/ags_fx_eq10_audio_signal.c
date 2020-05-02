/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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
#include <ags/audio/ags_audio_buffer_util.h>

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

static const gchar *ags_fx_eq10_audio_signal_plugin_name = "ags-fx-eq10";

GType
ags_fx_eq10_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_eq10_audio_signal);
  }

  return g_define_type_id__volatile;
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
  
  gdouble *output_buffer;
  gdouble *input_buffer;
  gdouble *cache_28hz;
  gdouble *cache_56hz;
  gdouble *cache_112hz;
  gdouble *cache_224hz;
  gdouble *cache_448hz;
  gdouble *cache_896hz;
  gdouble *cache_1792hz;
  gdouble *cache_3584hz;
  gdouble *cache_7168hz;
  gdouble *cache_14336hz;

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

  /* copy mode */
  output_copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
							 AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  input_copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
							ags_audio_buffer_util_format_from_soundcard(format));

  output_buffer = NULL;
  input_buffer = NULL;

  cache_28hz = NULL;
  cache_56hz = NULL;
  cache_112hz = NULL;
  cache_224hz = NULL;
  cache_448hz = NULL;
  cache_896hz = NULL;
  cache_1792hz = NULL;
  cache_3584hz = NULL;
  cache_7168hz = NULL;
  cache_14336hz = NULL;
  
  if(fx_eq10_channel != NULL){
    fx_eq10_channel_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_eq10_channel);

    g_rec_mutex_lock(fx_eq10_channel_mutex);

    output_buffer = fx_eq10_channel->input_data[sound_scope]->output;
    input_buffer = fx_eq10_channel->input_data[sound_scope]->input;

    cache_28hz = fx_eq10_channel->input_data[sound_scope]->cache_28hz;
    cache_56hz = fx_eq10_channel->input_data[sound_scope]->cache_56hz;
    cache_112hz = fx_eq10_channel->input_data[sound_scope]->cache_112hz;
    cache_224hz = fx_eq10_channel->input_data[sound_scope]->cache_224hz;
    cache_448hz = fx_eq10_channel->input_data[sound_scope]->cache_448hz;
    cache_896hz = fx_eq10_channel->input_data[sound_scope]->cache_896hz;
    cache_1792hz = fx_eq10_channel->input_data[sound_scope]->cache_1792hz;
    cache_3584hz = fx_eq10_channel->input_data[sound_scope]->cache_3584hz;
    cache_7168hz = fx_eq10_channel->input_data[sound_scope]->cache_7168hz;
    cache_14336hz = fx_eq10_channel->input_data[sound_scope]->cache_14336hz;

    g_rec_mutex_unlock(fx_eq10_channel_mutex);
  }
  
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

  if(fx_eq10_channel != NULL &&
     source != NULL &&
     source->stream_current != NULL){
    stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(source);

    g_rec_mutex_lock(fx_eq10_channel_mutex);

    /* clear/copy - preserve trailing */
    if(buffer_size > 8){
      /* clear buffer */
      ags_audio_buffer_util_clear_double(input_buffer, 1,
					 buffer_size - 2);
    
      /* copy input */
      g_rec_mutex_lock(stream_mutex);
      
      ags_audio_buffer_util_copy_buffer_to_buffer(input_buffer, 1, 0,
						  source->stream_current->data, 1, 0,
						  buffer_size - 2, input_copy_mode);
      
      g_rec_mutex_unlock(stream_mutex);
    }

    /* equalizer */
    for(i = 0; i + 8 < buffer_size; i += 8){
      gdouble resonator;
      gdouble frequency;
    
      /* 28Hz */
      resonator = peak_28hz;
      frequency = 2.0 * M_PI * 28.0 / (gdouble) samplerate;
    
      if(i == 0){
	cache_28hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_28hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_28hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	cache_28hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_28hz[0] - (resonator * resonator) * cache_28hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];
	cache_28hz[2] = input_buffer[2] - (resonator * input_buffer[0]) + (2.0 * resonator * cos(frequency)) * cache_28hz[1] - (resonator * resonator) * cache_28hz[0];
	cache_28hz[3] = input_buffer[3] - (resonator * input_buffer[1]) + (2.0 * resonator * cos(frequency)) * cache_28hz[2] - (resonator * resonator) * cache_28hz[1];
	cache_28hz[4] = input_buffer[4] - (resonator * input_buffer[2]) + (2.0 * resonator * cos(frequency)) * cache_28hz[3] - (resonator * resonator) * cache_28hz[2];
	cache_28hz[5] = input_buffer[5] - (resonator * input_buffer[3]) + (2.0 * resonator * cos(frequency)) * cache_28hz[4] - (resonator * resonator) * cache_28hz[3];
	cache_28hz[6] = input_buffer[6] - (resonator * input_buffer[4]) + (2.0 * resonator * cos(frequency)) * cache_28hz[5] - (resonator * resonator) * cache_28hz[4];
	cache_28hz[7] = input_buffer[7] - (resonator * input_buffer[5]) + (2.0 * resonator * cos(frequency)) * cache_28hz[6] - (resonator * resonator) * cache_28hz[5];
      }else{
	cache_28hz[0] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_28hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_28hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	cache_28hz[1] = input_buffer[i + 1] - (resonator * input_buffer[i - 1]) + (2.0 * resonator * cos(frequency)) * cache_28hz[0] - (resonator * resonator) * cache_28hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];
	cache_28hz[2] = input_buffer[i + 2] - (resonator * input_buffer[i]) + (2.0 * resonator * cos(frequency)) * cache_28hz[1] - (resonator * resonator) * cache_28hz[0];
	cache_28hz[3] = input_buffer[i + 3] - (resonator * input_buffer[i + 1]) + (2.0 * resonator * cos(frequency)) * cache_28hz[2] - (resonator * resonator) * cache_28hz[1];
	cache_28hz[4] = input_buffer[i + 4] - (resonator * input_buffer[i + 2]) + (2.0 * resonator * cos(frequency)) * cache_28hz[3] - (resonator * resonator) * cache_28hz[2];
	cache_28hz[5] = input_buffer[i + 5] - (resonator * input_buffer[i + 3]) + (2.0 * resonator * cos(frequency)) * cache_28hz[4] - (resonator * resonator) * cache_28hz[3];
	cache_28hz[6] = input_buffer[i + 6] - (resonator * input_buffer[i + 4]) + (2.0 * resonator * cos(frequency)) * cache_28hz[5] - (resonator * resonator) * cache_28hz[4];
	cache_28hz[7] = input_buffer[i + 7] - (resonator * input_buffer[i + 5]) + (2.0 * resonator * cos(frequency)) * cache_28hz[6] - (resonator * resonator) * cache_28hz[5];
      }

      /* 56Hz */
      resonator = peak_56hz;
      frequency = 2.0 * M_PI * 56.0 / (gdouble) samplerate;
    
      if(i == 0){
	cache_56hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_56hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_56hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	cache_56hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_56hz[0] - (resonator * resonator) * cache_56hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];
	cache_56hz[2] = input_buffer[2] - (resonator * input_buffer[0]) + (2.0 * resonator * cos(frequency)) * cache_56hz[1] - (resonator * resonator) * cache_56hz[0];
	cache_56hz[3] = input_buffer[3] - (resonator * input_buffer[1]) + (2.0 * resonator * cos(frequency)) * cache_56hz[2] - (resonator * resonator) * cache_56hz[1];
	cache_56hz[4] = input_buffer[4] - (resonator * input_buffer[2]) + (2.0 * resonator * cos(frequency)) * cache_56hz[3] - (resonator * resonator) * cache_56hz[2];
	cache_56hz[5] = input_buffer[5] - (resonator * input_buffer[3]) + (2.0 * resonator * cos(frequency)) * cache_56hz[4] - (resonator * resonator) * cache_56hz[3];
	cache_56hz[6] = input_buffer[6] - (resonator * input_buffer[4]) + (2.0 * resonator * cos(frequency)) * cache_56hz[5] - (resonator * resonator) * cache_56hz[4];
	cache_56hz[7] = input_buffer[7] - (resonator * input_buffer[5]) + (2.0 * resonator * cos(frequency)) * cache_56hz[6] - (resonator * resonator) * cache_56hz[5];
      }else{
	cache_56hz[0] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_56hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_56hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	cache_56hz[1] = input_buffer[i + 1] - (resonator * input_buffer[i - 1]) + (2.0 * resonator * cos(frequency)) * cache_56hz[0] - (resonator * resonator) * cache_56hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];
	cache_56hz[2] = input_buffer[i + 2] - (resonator * input_buffer[i]) + (2.0 * resonator * cos(frequency)) * cache_56hz[1] - (resonator * resonator) * cache_56hz[0];
	cache_56hz[3] = input_buffer[i + 3] - (resonator * input_buffer[i + 1]) + (2.0 * resonator * cos(frequency)) * cache_56hz[2] - (resonator * resonator) * cache_56hz[1];
	cache_56hz[4] = input_buffer[i + 4] - (resonator * input_buffer[i + 2]) + (2.0 * resonator * cos(frequency)) * cache_56hz[3] - (resonator * resonator) * cache_56hz[2];
	cache_56hz[5] = input_buffer[i + 5] - (resonator * input_buffer[i + 3]) + (2.0 * resonator * cos(frequency)) * cache_56hz[4] - (resonator * resonator) * cache_56hz[3];
	cache_56hz[6] = input_buffer[i + 6] - (resonator * input_buffer[i + 4]) + (2.0 * resonator * cos(frequency)) * cache_56hz[5] - (resonator * resonator) * cache_56hz[4];
	cache_56hz[7] = input_buffer[i + 7] - (resonator * input_buffer[i + 5]) + (2.0 * resonator * cos(frequency)) * cache_56hz[6] - (resonator * resonator) * cache_56hz[5];
      }

      /* 112Hz */
      resonator = peak_112hz;
      frequency = 2.0 * M_PI * 112.0 / (gdouble) samplerate;
    
      if(i == 0){
	cache_112hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_112hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_112hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	cache_112hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_112hz[0] - (resonator * resonator) * cache_112hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];
	cache_112hz[2] = input_buffer[2] - (resonator * input_buffer[0]) + (2.0 * resonator * cos(frequency)) * cache_112hz[1] - (resonator * resonator) * cache_112hz[0];
	cache_112hz[3] = input_buffer[3] - (resonator * input_buffer[1]) + (2.0 * resonator * cos(frequency)) * cache_112hz[2] - (resonator * resonator) * cache_112hz[1];
	cache_112hz[4] = input_buffer[4] - (resonator * input_buffer[2]) + (2.0 * resonator * cos(frequency)) * cache_112hz[3] - (resonator * resonator) * cache_112hz[2];
	cache_112hz[5] = input_buffer[5] - (resonator * input_buffer[3]) + (2.0 * resonator * cos(frequency)) * cache_112hz[4] - (resonator * resonator) * cache_112hz[3];
	cache_112hz[6] = input_buffer[6] - (resonator * input_buffer[4]) + (2.0 * resonator * cos(frequency)) * cache_112hz[5] - (resonator * resonator) * cache_112hz[4];
	cache_112hz[7] = input_buffer[7] - (resonator * input_buffer[5]) + (2.0 * resonator * cos(frequency)) * cache_112hz[6] - (resonator * resonator) * cache_112hz[5];
      }else{
	cache_112hz[0] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_112hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_112hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	cache_112hz[1] = input_buffer[i + 1] - (resonator * input_buffer[i - 1]) + (2.0 * resonator * cos(frequency)) * cache_112hz[0] - (resonator * resonator) * cache_112hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];
	cache_112hz[2] = input_buffer[i + 2] - (resonator * input_buffer[i]) + (2.0 * resonator * cos(frequency)) * cache_112hz[1] - (resonator * resonator) * cache_112hz[0];
	cache_112hz[3] = input_buffer[i + 3] - (resonator * input_buffer[i + 1]) + (2.0 * resonator * cos(frequency)) * cache_112hz[2] - (resonator * resonator) * cache_112hz[1];
	cache_112hz[4] = input_buffer[i + 4] - (resonator * input_buffer[i + 2]) + (2.0 * resonator * cos(frequency)) * cache_112hz[3] - (resonator * resonator) * cache_112hz[2];
	cache_112hz[5] = input_buffer[i + 5] - (resonator * input_buffer[i + 3]) + (2.0 * resonator * cos(frequency)) * cache_112hz[4] - (resonator * resonator) * cache_112hz[3];
	cache_112hz[6] = input_buffer[i + 6] - (resonator * input_buffer[i + 4]) + (2.0 * resonator * cos(frequency)) * cache_112hz[5] - (resonator * resonator) * cache_112hz[4];
	cache_112hz[7] = input_buffer[i + 7] - (resonator * input_buffer[i + 5]) + (2.0 * resonator * cos(frequency)) * cache_112hz[6] - (resonator * resonator) * cache_112hz[5];
      }

      /* 224Hz */
      resonator = peak_224hz;
      frequency = 2.0 * M_PI * 224.0 / (gdouble) samplerate;
    
      if(i == 0){
	cache_224hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_224hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_224hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	cache_224hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_224hz[0] - (resonator * resonator) * cache_224hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];
	cache_224hz[2] = input_buffer[2] - (resonator * input_buffer[0]) + (2.0 * resonator * cos(frequency)) * cache_224hz[1] - (resonator * resonator) * cache_224hz[0];
	cache_224hz[3] = input_buffer[3] - (resonator * input_buffer[1]) + (2.0 * resonator * cos(frequency)) * cache_224hz[2] - (resonator * resonator) * cache_224hz[1];
	cache_224hz[4] = input_buffer[4] - (resonator * input_buffer[2]) + (2.0 * resonator * cos(frequency)) * cache_224hz[3] - (resonator * resonator) * cache_224hz[2];
	cache_224hz[5] = input_buffer[5] - (resonator * input_buffer[3]) + (2.0 * resonator * cos(frequency)) * cache_224hz[4] - (resonator * resonator) * cache_224hz[3];
	cache_224hz[6] = input_buffer[6] - (resonator * input_buffer[4]) + (2.0 * resonator * cos(frequency)) * cache_224hz[5] - (resonator * resonator) * cache_224hz[4];
	cache_224hz[7] = input_buffer[7] - (resonator * input_buffer[5]) + (2.0 * resonator * cos(frequency)) * cache_224hz[6] - (resonator * resonator) * cache_224hz[5];
      }else{
	cache_224hz[0] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_224hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_224hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	cache_224hz[1] = input_buffer[i + 1] - (resonator * input_buffer[i - 1]) + (2.0 * resonator * cos(frequency)) * cache_224hz[0] - (resonator * resonator) * cache_224hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];
	cache_224hz[2] = input_buffer[i + 2] - (resonator * input_buffer[i]) + (2.0 * resonator * cos(frequency)) * cache_224hz[1] - (resonator * resonator) * cache_224hz[0];
	cache_224hz[3] = input_buffer[i + 3] - (resonator * input_buffer[i + 1]) + (2.0 * resonator * cos(frequency)) * cache_224hz[2] - (resonator * resonator) * cache_224hz[1];
	cache_224hz[4] = input_buffer[i + 4] - (resonator * input_buffer[i + 2]) + (2.0 * resonator * cos(frequency)) * cache_224hz[3] - (resonator * resonator) * cache_224hz[2];
	cache_224hz[5] = input_buffer[i + 5] - (resonator * input_buffer[i + 3]) + (2.0 * resonator * cos(frequency)) * cache_224hz[4] - (resonator * resonator) * cache_224hz[3];
	cache_224hz[6] = input_buffer[i + 6] - (resonator * input_buffer[i + 4]) + (2.0 * resonator * cos(frequency)) * cache_224hz[5] - (resonator * resonator) * cache_224hz[4];
	cache_224hz[7] = input_buffer[i + 7] - (resonator * input_buffer[i + 5]) + (2.0 * resonator * cos(frequency)) * cache_224hz[6] - (resonator * resonator) * cache_224hz[5];
      }

      /* 448Hz */
      resonator = peak_448hz;
      frequency = 2.0 * M_PI * 448.0 / (gdouble) samplerate;
    
      if(i == 0){
	cache_448hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_448hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_448hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	cache_448hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_448hz[0] - (resonator * resonator) * cache_448hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];
	cache_448hz[2] = input_buffer[2] - (resonator * input_buffer[0]) + (2.0 * resonator * cos(frequency)) * cache_448hz[1] - (resonator * resonator) * cache_448hz[0];
	cache_448hz[3] = input_buffer[3] - (resonator * input_buffer[1]) + (2.0 * resonator * cos(frequency)) * cache_448hz[2] - (resonator * resonator) * cache_448hz[1];
	cache_448hz[4] = input_buffer[4] - (resonator * input_buffer[2]) + (2.0 * resonator * cos(frequency)) * cache_448hz[3] - (resonator * resonator) * cache_448hz[2];
	cache_448hz[5] = input_buffer[5] - (resonator * input_buffer[3]) + (2.0 * resonator * cos(frequency)) * cache_448hz[4] - (resonator * resonator) * cache_448hz[3];
	cache_448hz[6] = input_buffer[6] - (resonator * input_buffer[4]) + (2.0 * resonator * cos(frequency)) * cache_448hz[5] - (resonator * resonator) * cache_448hz[4];
	cache_448hz[7] = input_buffer[7] - (resonator * input_buffer[5]) + (2.0 * resonator * cos(frequency)) * cache_448hz[6] - (resonator * resonator) * cache_448hz[5];
      }else{
	cache_448hz[0] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_448hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_448hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	cache_448hz[1] = input_buffer[i + 1] - (resonator * input_buffer[i - 1]) + (2.0 * resonator * cos(frequency)) * cache_448hz[0] - (resonator * resonator) * cache_448hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];
	cache_448hz[2] = input_buffer[i + 2] - (resonator * input_buffer[i]) + (2.0 * resonator * cos(frequency)) * cache_448hz[1] - (resonator * resonator) * cache_448hz[0];
	cache_448hz[3] = input_buffer[i + 3] - (resonator * input_buffer[i + 1]) + (2.0 * resonator * cos(frequency)) * cache_448hz[2] - (resonator * resonator) * cache_448hz[1];
	cache_448hz[4] = input_buffer[i + 4] - (resonator * input_buffer[i + 2]) + (2.0 * resonator * cos(frequency)) * cache_448hz[3] - (resonator * resonator) * cache_448hz[2];
	cache_448hz[5] = input_buffer[i + 5] - (resonator * input_buffer[i + 3]) + (2.0 * resonator * cos(frequency)) * cache_448hz[4] - (resonator * resonator) * cache_448hz[3];
	cache_448hz[6] = input_buffer[i + 6] - (resonator * input_buffer[i + 4]) + (2.0 * resonator * cos(frequency)) * cache_448hz[5] - (resonator * resonator) * cache_448hz[4];
	cache_448hz[7] = input_buffer[i + 7] - (resonator * input_buffer[i + 5]) + (2.0 * resonator * cos(frequency)) * cache_448hz[6] - (resonator * resonator) * cache_448hz[5];
      }

      /* 896Hz */
      resonator = peak_896hz;
      frequency = 2.0 * M_PI * 896.0 / (gdouble) samplerate;
    
      if(i == 0){
	cache_896hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_896hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_896hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	cache_896hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_896hz[0] - (resonator * resonator) * cache_896hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];
	cache_896hz[2] = input_buffer[2] - (resonator * input_buffer[0]) + (2.0 * resonator * cos(frequency)) * cache_896hz[1] - (resonator * resonator) * cache_896hz[0];
	cache_896hz[3] = input_buffer[3] - (resonator * input_buffer[1]) + (2.0 * resonator * cos(frequency)) * cache_896hz[2] - (resonator * resonator) * cache_896hz[1];
	cache_896hz[4] = input_buffer[4] - (resonator * input_buffer[2]) + (2.0 * resonator * cos(frequency)) * cache_896hz[3] - (resonator * resonator) * cache_896hz[2];
	cache_896hz[5] = input_buffer[5] - (resonator * input_buffer[3]) + (2.0 * resonator * cos(frequency)) * cache_896hz[4] - (resonator * resonator) * cache_896hz[3];
	cache_896hz[6] = input_buffer[6] - (resonator * input_buffer[4]) + (2.0 * resonator * cos(frequency)) * cache_896hz[5] - (resonator * resonator) * cache_896hz[4];
	cache_896hz[7] = input_buffer[7] - (resonator * input_buffer[5]) + (2.0 * resonator * cos(frequency)) * cache_896hz[6] - (resonator * resonator) * cache_896hz[5];
      }else{
	cache_896hz[0] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_896hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_896hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	cache_896hz[1] = input_buffer[i + 1] - (resonator * input_buffer[i - 1]) + (2.0 * resonator * cos(frequency)) * cache_896hz[0] - (resonator * resonator) * cache_896hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];
	cache_896hz[2] = input_buffer[i + 2] - (resonator * input_buffer[i]) + (2.0 * resonator * cos(frequency)) * cache_896hz[1] - (resonator * resonator) * cache_896hz[0];
	cache_896hz[3] = input_buffer[i + 3] - (resonator * input_buffer[i + 1]) + (2.0 * resonator * cos(frequency)) * cache_896hz[2] - (resonator * resonator) * cache_896hz[1];
	cache_896hz[4] = input_buffer[i + 4] - (resonator * input_buffer[i + 2]) + (2.0 * resonator * cos(frequency)) * cache_896hz[3] - (resonator * resonator) * cache_896hz[2];
	cache_896hz[5] = input_buffer[i + 5] - (resonator * input_buffer[i + 3]) + (2.0 * resonator * cos(frequency)) * cache_896hz[4] - (resonator * resonator) * cache_896hz[3];
	cache_896hz[6] = input_buffer[i + 6] - (resonator * input_buffer[i + 4]) + (2.0 * resonator * cos(frequency)) * cache_896hz[5] - (resonator * resonator) * cache_896hz[4];
	cache_896hz[7] = input_buffer[i + 7] - (resonator * input_buffer[i + 5]) + (2.0 * resonator * cos(frequency)) * cache_896hz[6] - (resonator * resonator) * cache_896hz[5];
      }

      /* 1792Hz */
      resonator = peak_1792hz;
      frequency = 2.0 * M_PI * 1792.0 / (gdouble) samplerate;
    
      if(i == 0){
	cache_1792hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_1792hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	cache_1792hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[0] - (resonator * resonator) * cache_1792hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];
	cache_1792hz[2] = input_buffer[2] - (resonator * input_buffer[0]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[1] - (resonator * resonator) * cache_1792hz[0];
	cache_1792hz[3] = input_buffer[3] - (resonator * input_buffer[1]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[2] - (resonator * resonator) * cache_1792hz[1];
	cache_1792hz[4] = input_buffer[4] - (resonator * input_buffer[2]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[3] - (resonator * resonator) * cache_1792hz[2];
	cache_1792hz[5] = input_buffer[5] - (resonator * input_buffer[3]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[4] - (resonator * resonator) * cache_1792hz[3];
	cache_1792hz[6] = input_buffer[6] - (resonator * input_buffer[4]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[5] - (resonator * resonator) * cache_1792hz[4];
	cache_1792hz[7] = input_buffer[7] - (resonator * input_buffer[5]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[6] - (resonator * resonator) * cache_1792hz[5];
      }else{
	cache_1792hz[0] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_1792hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	cache_1792hz[1] = input_buffer[i + 1] - (resonator * input_buffer[i - 1]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[0] - (resonator * resonator) * cache_1792hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];
	cache_1792hz[2] = input_buffer[i + 2] - (resonator * input_buffer[i]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[1] - (resonator * resonator) * cache_1792hz[0];
	cache_1792hz[3] = input_buffer[i + 3] - (resonator * input_buffer[i + 1]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[2] - (resonator * resonator) * cache_1792hz[1];
	cache_1792hz[4] = input_buffer[i + 4] - (resonator * input_buffer[i + 2]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[3] - (resonator * resonator) * cache_1792hz[2];
	cache_1792hz[5] = input_buffer[i + 5] - (resonator * input_buffer[i + 3]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[4] - (resonator * resonator) * cache_1792hz[3];
	cache_1792hz[6] = input_buffer[i + 6] - (resonator * input_buffer[i + 4]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[5] - (resonator * resonator) * cache_1792hz[4];
	cache_1792hz[7] = input_buffer[i + 7] - (resonator * input_buffer[i + 5]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[6] - (resonator * resonator) * cache_1792hz[5];
      }

      /* 3584Hz */
      resonator = peak_3584hz;
      frequency = 2.0 * M_PI * 3584.0 / (gdouble) samplerate;
    
      if(i == 0){
	cache_3584hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_3584hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	cache_3584hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[0] - (resonator * resonator) * cache_3584hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];
	cache_3584hz[2] = input_buffer[2] - (resonator * input_buffer[0]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[1] - (resonator * resonator) * cache_3584hz[0];
	cache_3584hz[3] = input_buffer[3] - (resonator * input_buffer[1]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[2] - (resonator * resonator) * cache_3584hz[1];
	cache_3584hz[4] = input_buffer[4] - (resonator * input_buffer[2]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[3] - (resonator * resonator) * cache_3584hz[2];
	cache_3584hz[5] = input_buffer[5] - (resonator * input_buffer[3]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[4] - (resonator * resonator) * cache_3584hz[3];
	cache_3584hz[6] = input_buffer[6] - (resonator * input_buffer[4]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[5] - (resonator * resonator) * cache_3584hz[4];
	cache_3584hz[7] = input_buffer[7] - (resonator * input_buffer[5]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[6] - (resonator * resonator) * cache_3584hz[5];
      }else{
	cache_3584hz[0] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_3584hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	cache_3584hz[1] = input_buffer[i + 1] - (resonator * input_buffer[i - 1]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[0] - (resonator * resonator) * cache_3584hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];
	cache_3584hz[2] = input_buffer[i + 2] - (resonator * input_buffer[i]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[1] - (resonator * resonator) * cache_3584hz[0];
	cache_3584hz[3] = input_buffer[i + 3] - (resonator * input_buffer[i + 1]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[2] - (resonator * resonator) * cache_3584hz[1];
	cache_3584hz[4] = input_buffer[i + 4] - (resonator * input_buffer[i + 2]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[3] - (resonator * resonator) * cache_3584hz[2];
	cache_3584hz[5] = input_buffer[i + 5] - (resonator * input_buffer[i + 3]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[4] - (resonator * resonator) * cache_3584hz[3];
	cache_3584hz[6] = input_buffer[i + 6] - (resonator * input_buffer[i + 4]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[5] - (resonator * resonator) * cache_3584hz[4];
	cache_3584hz[7] = input_buffer[i + 7] - (resonator * input_buffer[i + 5]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[6] - (resonator * resonator) * cache_3584hz[5];
      }

      /* 7168Hz */
      resonator = peak_7168hz;
      frequency = 2.0 * M_PI * 7168.0 / (gdouble) samplerate;
    
      if(i == 0){
	cache_7168hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_7168hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	cache_7168hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[0] - (resonator * resonator) * cache_7168hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];
	cache_7168hz[2] = input_buffer[2] - (resonator * input_buffer[0]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[1] - (resonator * resonator) * cache_7168hz[0];
	cache_7168hz[3] = input_buffer[3] - (resonator * input_buffer[1]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[2] - (resonator * resonator) * cache_7168hz[1];
	cache_7168hz[4] = input_buffer[4] - (resonator * input_buffer[2]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[3] - (resonator * resonator) * cache_7168hz[2];
	cache_7168hz[5] = input_buffer[5] - (resonator * input_buffer[3]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[4] - (resonator * resonator) * cache_7168hz[3];
	cache_7168hz[6] = input_buffer[6] - (resonator * input_buffer[4]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[5] - (resonator * resonator) * cache_7168hz[4];
	cache_7168hz[7] = input_buffer[7] - (resonator * input_buffer[5]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[6] - (resonator * resonator) * cache_7168hz[5];
      }else{
	cache_7168hz[0] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_7168hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	cache_7168hz[1] = input_buffer[i + 1] - (resonator * input_buffer[i - 1]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[0] - (resonator * resonator) * cache_7168hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];
	cache_7168hz[2] = input_buffer[i + 2] - (resonator * input_buffer[i]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[1] - (resonator * resonator) * cache_7168hz[0];
	cache_7168hz[3] = input_buffer[i + 3] - (resonator * input_buffer[i + 1]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[2] - (resonator * resonator) * cache_7168hz[1];
	cache_7168hz[4] = input_buffer[i + 4] - (resonator * input_buffer[i + 2]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[3] - (resonator * resonator) * cache_7168hz[2];
	cache_7168hz[5] = input_buffer[i + 5] - (resonator * input_buffer[i + 3]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[4] - (resonator * resonator) * cache_7168hz[3];
	cache_7168hz[6] = input_buffer[i + 6] - (resonator * input_buffer[i + 4]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[5] - (resonator * resonator) * cache_7168hz[4];
	cache_7168hz[7] = input_buffer[i + 7] - (resonator * input_buffer[i + 5]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[6] - (resonator * resonator) * cache_7168hz[5];
      }

      /* 14336Hz */
      resonator = peak_14336hz;
      frequency = 2.0 * M_PI * 14336.0 / (gdouble) samplerate;
    
      if(i == 0){
	cache_14336hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_14336hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	cache_14336hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[0] - (resonator * resonator) * cache_14336hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];
	cache_14336hz[2] = input_buffer[2] - (resonator * input_buffer[0]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[1] - (resonator * resonator) * cache_14336hz[0];
	cache_14336hz[3] = input_buffer[3] - (resonator * input_buffer[1]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[2] - (resonator * resonator) * cache_14336hz[1];
	cache_14336hz[4] = input_buffer[4] - (resonator * input_buffer[2]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[3] - (resonator * resonator) * cache_14336hz[2];
	cache_14336hz[5] = input_buffer[5] - (resonator * input_buffer[3]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[4] - (resonator * resonator) * cache_14336hz[3];
	cache_14336hz[6] = input_buffer[6] - (resonator * input_buffer[4]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[5] - (resonator * resonator) * cache_14336hz[4];
	cache_14336hz[7] = input_buffer[7] - (resonator * input_buffer[5]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[6] - (resonator * resonator) * cache_14336hz[5];
      }else{
	cache_14336hz[0] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_14336hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	cache_14336hz[1] = input_buffer[i + 1] - (resonator * input_buffer[i - 1]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[0] - (resonator * resonator) * cache_14336hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];
	cache_14336hz[2] = input_buffer[i + 2] - (resonator * input_buffer[i]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[1] - (resonator * resonator) * cache_14336hz[0];
	cache_14336hz[3] = input_buffer[i + 3] - (resonator * input_buffer[i + 1]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[2] - (resonator * resonator) * cache_14336hz[1];
	cache_14336hz[4] = input_buffer[i + 4] - (resonator * input_buffer[i + 2]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[3] - (resonator * resonator) * cache_14336hz[2];
	cache_14336hz[5] = input_buffer[i + 5] - (resonator * input_buffer[i + 3]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[4] - (resonator * resonator) * cache_14336hz[3];
	cache_14336hz[6] = input_buffer[i + 6] - (resonator * input_buffer[i + 4]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[5] - (resonator * resonator) * cache_14336hz[4];
	cache_14336hz[7] = input_buffer[i + 7] - (resonator * input_buffer[i + 5]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[6] - (resonator * resonator) * cache_14336hz[5];
      }

      /* clear/copy - handle trailing */
      if(i == 0){
	/* clear buffer */
	ags_audio_buffer_util_clear_double(input_buffer + (buffer_size - 3), 1,
					   2);
    
	/* copy input */
	g_rec_mutex_lock(stream_mutex);
	
	ags_audio_buffer_util_copy_buffer_to_buffer(input_buffer, 1, buffer_size - 3,
						    source->stream_current->data, 1, buffer_size - 3,
						    2, input_copy_mode);

	g_rec_mutex_unlock(stream_mutex);
      }

      /* fill output */
      ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						  cache_28hz, 1,
						  AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE);

      ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						  cache_56hz, 1,
						  AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE);

      ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						  cache_112hz, 1,
						  AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE);

      ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						  cache_224hz, 1,
						  AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE);

      ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						  cache_448hz, 1,
						  AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE);

      ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						  cache_896hz, 1,
						  AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE);

      ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						  cache_1792hz, 1,
						  AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE);

      ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						  cache_3584hz, 1,
						  AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE);

      ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						  cache_7168hz, 1,
						  AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE);

      ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						  cache_14336hz, 1,
						  AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE);
    }

    if(buffer_size >= 4){
      for(; i < buffer_size; i++){
	gdouble resonator;
	gdouble frequency;
      
	/* clear buffer */
	ags_audio_buffer_util_clear_double(input_buffer + i, 1,
					   1);
      
	/* copy input */
	g_rec_mutex_lock(stream_mutex);
	
	ags_audio_buffer_util_copy_buffer_to_buffer(input_buffer, 1, i,
						    source->stream_current->data, 1, i,
						    1, input_copy_mode);

	g_rec_mutex_unlock(stream_mutex);

	if(i == 0){
	  resonator = peak_28hz;
	  frequency = 2.0 * M_PI * 28.0 / (gdouble) samplerate;
	  cache_28hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_28hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_28hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	
	  resonator = peak_56hz;
	  frequency = 2.0 * M_PI * 56.0 / (gdouble) samplerate;
	  cache_56hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_56hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_56hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];

	  resonator = peak_112hz;
	  frequency = 2.0 * M_PI * 112.0 / (gdouble) samplerate;
	  cache_112hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_112hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_112hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];

	  resonator = peak_224hz;
	  frequency = 2.0 * M_PI * 224.0 / (gdouble) samplerate;
	  cache_224hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_224hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_224hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];

	  resonator = peak_448hz;
	  frequency = 2.0 * M_PI * 448.0 / (gdouble) samplerate;
	  cache_448hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_448hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_448hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];

	  resonator = peak_896hz;
	  frequency = 2.0 * M_PI * 896.0 / (gdouble) samplerate;
	  cache_896hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_896hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_896hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];

	  resonator = peak_1792hz;
	  frequency = 2.0 * M_PI * 1792.0 / (gdouble) samplerate;
	  cache_1792hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_1792hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];

	  resonator = peak_3584hz;
	  frequency = 2.0 * M_PI * 3584.0 / (gdouble) samplerate;
	  cache_3584hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_3584hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];

	  resonator = peak_7168hz;
	  frequency = 2.0 * M_PI * 7168.0 / (gdouble) samplerate;
	  cache_7168hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_7168hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];

	  resonator = peak_14336hz;
	  frequency = 2.0 * M_PI * 14336.0 / (gdouble) samplerate;
	  cache_14336hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_14336hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	}else if(i == 1){
	  resonator = peak_28hz;
	  frequency = 2.0 * M_PI * 28.0 / (gdouble) samplerate;
	  cache_28hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_28hz[0] - (resonator * resonator) * cache_28hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];

	  resonator = peak_56hz;
	  frequency = 2.0 * M_PI * 56.0 / (gdouble) samplerate;
	  cache_56hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_56hz[0] - (resonator * resonator) * cache_56hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];

	  resonator = peak_112hz;
	  frequency = 2.0 * M_PI * 112.0 / (gdouble) samplerate;
	  cache_112hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_112hz[0] - (resonator * resonator) * cache_112hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];

	  resonator = peak_224hz;
	  frequency = 2.0 * M_PI * 224.0 / (gdouble) samplerate;
	  cache_224hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_224hz[0] - (resonator * resonator) * cache_224hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];

	  resonator = peak_448hz;
	  frequency = 2.0 * M_PI * 448.0 / (gdouble) samplerate;
	  cache_448hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_448hz[0] - (resonator * resonator) * cache_448hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];

	  resonator = peak_896hz;
	  frequency = 2.0 * M_PI * 896.0 / (gdouble) samplerate;
	  cache_896hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_896hz[0] - (resonator * resonator) * cache_896hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];

	  resonator = peak_1792hz;
	  frequency = 2.0 * M_PI * 1792.0 / (gdouble) samplerate;
	  cache_1792hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[0] - (resonator * resonator) * cache_1792hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];

	  resonator = peak_3584hz;
	  frequency = 2.0 * M_PI * 3584.0 / (gdouble) samplerate;
	  cache_3584hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[0] - (resonator * resonator) * cache_3584hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];

	  resonator = peak_7168hz;
	  frequency = 2.0 * M_PI * 7168.0 / (gdouble) samplerate;
	  cache_7168hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[0] - (resonator * resonator) * cache_7168hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];

	  resonator = peak_14336hz;
	  frequency = 2.0 * M_PI * 14336.0 / (gdouble) samplerate;
	  cache_14336hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[0] - (resonator * resonator) * cache_14336hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1];
	}else{
	  resonator = peak_28hz;
	  frequency = 2.0 * M_PI * 28.0 / (gdouble) samplerate;
	  cache_28hz[i % 8] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_28hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_28hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];

	  resonator = peak_56hz;
	  frequency = 2.0 * M_PI * 56.0 / (gdouble) samplerate;
	  cache_56hz[i % 8] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_56hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_56hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];

	  resonator = peak_112hz;
	  frequency = 2.0 * M_PI * 112.0 / (gdouble) samplerate;
	  cache_112hz[i % 8] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_112hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_112hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];

	  resonator = peak_224hz;
	  frequency = 2.0 * M_PI * 224.0 / (gdouble) samplerate;
	  cache_224hz[i % 8] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_224hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_224hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];

	  resonator = peak_448hz;
	  frequency = 2.0 * M_PI * 448.0 / (gdouble) samplerate;
	  cache_448hz[i % 8] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_448hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_448hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];

	  resonator = peak_896hz;
	  frequency = 2.0 * M_PI * 896.0 / (gdouble) samplerate;
	  cache_896hz[i % 8] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_896hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_896hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];

	  resonator = peak_1792hz;
	  frequency = 2.0 * M_PI * 1792.0 / (gdouble) samplerate;
	  cache_1792hz[i % 8] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_1792hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];

	  resonator = peak_3584hz;
	  frequency = 2.0 * M_PI * 3584.0 / (gdouble) samplerate;
	  cache_3584hz[i % 8] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_3584hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];

	  resonator = peak_7168hz;
	  frequency = 2.0 * M_PI * 7168.0 / (gdouble) samplerate;
	  cache_7168hz[i % 8] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_7168hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];

	  resonator = peak_14336hz;
	  frequency = 2.0 * M_PI * 14336.0 / (gdouble) samplerate;
	  cache_14336hz[i % 8] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 1] - (resonator * resonator) * cache_14336hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE - 2];
	}
      }

      /* fill output */
      ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						  cache_28hz + (i % 8), 1,
						  1);

      ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						  cache_56hz + (i % 8), 1,
						  1);

      ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						  cache_112hz + (i % 8), 1,
						  1);

      ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						  cache_224hz + (i % 8), 1,
						  1);

      ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						  cache_448hz + (i % 8), 1,
						  1);

      ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						  cache_896hz + (i % 8), 1,
						  1);

      ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						  cache_1792hz + (i % 8), 1,
						  1);

      ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						  cache_3584hz + (i % 8), 1,
						  1);

      ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						  cache_7168hz + (i % 8), 1,
						  1);

      ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						  cache_14336hz + (i % 8), 1,
						  1);
    }

    /* apply boost */
    ags_audio_buffer_util_volume_double(output_buffer, 1,
					buffer_size,
					pressure);
  
    /* clear buffer and copy output  */
    g_rec_mutex_lock(stream_mutex);

    ags_audio_buffer_util_clear_buffer(source->stream_current->data, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(format));
    
    ags_audio_buffer_util_copy_buffer_to_buffer(source->stream_current->data, 1, 0,
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
