/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/recall/ags_eq10_audio_signal.h>
#include <ags/audio/recall/ags_eq10_channel.h>

#include <ags/libags.h>

#include <ags/audio/ags_input.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <math.h>

void ags_eq10_audio_signal_class_init(AgsEq10AudioSignalClass *eq10_audio_signal);
void ags_eq10_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_eq10_audio_signal_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_eq10_audio_signal_init(AgsEq10AudioSignal *eq10_audio_signal);
void ags_eq10_audio_signal_connect(AgsConnectable *connectable);
void ags_eq10_audio_signal_disconnect(AgsConnectable *connectable);
void ags_eq10_audio_signal_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_eq10_audio_signal_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_eq10_audio_signal_finalize(GObject *gobject);

void ags_eq10_audio_signal_run_init_pre(AgsRecall *recall);
void ags_eq10_audio_signal_run_inter(AgsRecall *recall);
AgsRecall* ags_eq10_audio_signal_duplicate(AgsRecall *recall,
					   AgsRecallID *recall_id,
					   guint *n_params, GParameter *parameter);

/**
 * SECTION:ags_eq10_audio_signal
 * @short_description: eq10s audio signal
 * @title: AgsEq10AudioSignal
 * @section_id:
 * @include: ags/audio/recall/ags_eq10_audio_signal.h
 *
 * The #AgsEq10AudioSignal class eq10s the audio signal.
 */

static gpointer ags_eq10_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_eq10_audio_signal_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_eq10_audio_signal_parent_dynamic_connectable_interface;

GType
ags_eq10_audio_signal_get_type()
{
  static GType ags_type_eq10_audio_signal = 0;

  if(!ags_type_eq10_audio_signal){
    static const GTypeInfo ags_eq10_audio_signal_info = {
      sizeof (AgsEq10AudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_eq10_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsEq10AudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_eq10_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_eq10_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_eq10_audio_signal_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_eq10_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							"AgsEq10AudioSignal",
							&ags_eq10_audio_signal_info,
							0);

    g_type_add_interface_static(ags_type_eq10_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_eq10_audio_signal,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return (ags_type_eq10_audio_signal);
}

void
ags_eq10_audio_signal_class_init(AgsEq10AudioSignalClass *eq10_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_eq10_audio_signal_parent_class = g_type_class_peek_parent(eq10_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) eq10_audio_signal;

  gobject->finalize = ags_eq10_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) eq10_audio_signal;

  recall->run_init_pre = ags_eq10_audio_signal_run_init_pre;
  recall->run_inter = ags_eq10_audio_signal_run_inter;
}

void
ags_eq10_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_eq10_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_eq10_audio_signal_connect;
  connectable->disconnect = ags_eq10_audio_signal_disconnect;
}

void
ags_eq10_audio_signal_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_eq10_audio_signal_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_eq10_audio_signal_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_eq10_audio_signal_disconnect_dynamic;
}

void
ags_eq10_audio_signal_init(AgsEq10AudioSignal *eq10_audio_signal)
{
  AGS_RECALL(eq10_audio_signal)->name = "ags-eq10";
  AGS_RECALL(eq10_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(eq10_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(eq10_audio_signal)->xml_type = "ags-eq10-audio-signal";
  AGS_RECALL(eq10_audio_signal)->port = NULL;

  eq10_audio_signal->output_buffer = NULL;
  eq10_audio_signal->input_buffer = NULL;  
}

void
ags_eq10_audio_signal_finalize(GObject *gobject)
{
  AgsEq10AudioSignal *eq10_audio_signal;

  eq10_audio_signal = (AgsEq10AudioSignal *) gobject;

  g_free(eq10_audio_signal->output_buffer);
  g_free(eq10_audio_signal->input_buffer);
  
  /* call parent */
  G_OBJECT_CLASS(ags_eq10_audio_signal_parent_class)->finalize(gobject);
}

void
ags_eq10_audio_signal_connect(AgsConnectable *connectable)
{
  /* call parent */
  ags_eq10_audio_signal_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_eq10_audio_signal_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_eq10_audio_signal_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_eq10_audio_signal_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_eq10_audio_signal_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_eq10_audio_signal_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_eq10_audio_signal_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_eq10_audio_signal_run_init_pre(AgsRecall *recall)
{
  AgsAudioSignal *source;

  AgsEq10AudioSignal *eq10_audio_signal;

  guint buffer_size;

  eq10_audio_signal = recall;

  /* call parent */
  AGS_RECALL_CLASS(ags_eq10_audio_signal_parent_class)->run_init_pre(recall);

  source = AGS_RECALL_AUDIO_SIGNAL(eq10_audio_signal)->source;

  buffer_size = source->buffer_size;
  
  eq10_audio_signal->output_buffer = (double *) malloc(buffer_size * sizeof(double));
  eq10_audio_signal->input_buffer = (double *) malloc(buffer_size * sizeof(double));  

  /* initially empty */
  ags_audio_buffer_util_clear_double(eq10_audio_signal->cache_28hz, 1,
				     AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE);
  ags_audio_buffer_util_clear_double(eq10_audio_signal->cache_56hz, 1,
				     AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE);
  ags_audio_buffer_util_clear_double(eq10_audio_signal->cache_112hz, 1,
				     AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE);
  ags_audio_buffer_util_clear_double(eq10_audio_signal->cache_224hz, 1,
				     AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE);
  ags_audio_buffer_util_clear_double(eq10_audio_signal->cache_448hz, 1,
				     AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE);
  ags_audio_buffer_util_clear_double(eq10_audio_signal->cache_896hz, 1,
				     AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE);
  ags_audio_buffer_util_clear_double(eq10_audio_signal->cache_1792hz, 1,
				     AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE);
  ags_audio_buffer_util_clear_double(eq10_audio_signal->cache_3584hz, 1,
				     AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE);
  ags_audio_buffer_util_clear_double(eq10_audio_signal->cache_7168hz, 1,
				     AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE);
  ags_audio_buffer_util_clear_double(eq10_audio_signal->cache_14336hz, 1,
				     AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE);
}

void
ags_eq10_audio_signal_run_inter(AgsRecall *recall)
{
  AgsAudioSignal *source;

  AgsEq10Channel *eq10_channel;
  AgsEq10AudioSignal *eq10_audio_signal;

  GList *stream_source;

  double *output_buffer;
  double *input_buffer;
  double *cache_28hz;
  double *cache_56hz;
  double *cache_112hz;
  double *cache_224hz;
  double *cache_448hz;
  double *cache_896hz;
  double *cache_1792hz;
  double *cache_3584hz;
  double *cache_7168hz;
  double *cache_14336hz;
  
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
  
  guint output_copy_mode;
  guint input_copy_mode;
  guint buffer_size;
  guint samplerate;
  guint i;

  GValue value = {0,};
  
  eq10_audio_signal = recall;

  /* call parent */
  AGS_RECALL_CLASS(ags_eq10_audio_signal_parent_class)->run_inter(recall);

  source = AGS_RECALL_AUDIO_SIGNAL(eq10_audio_signal)->source;

  if(recall->rt_safe &&
     recall->recall_id->recycling_context->parent != NULL &&
     source->note == NULL){
    return;
  }

  stream_source = source->stream_current;

  if(stream_source == NULL){
    ags_recall_done(recall);

    return;
  }

  eq10_channel = AGS_EQ10_CHANNEL(AGS_RECALL_CHANNEL_RUN(recall->parent->parent)->recall_channel);

  /* copy mode */
  output_copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(source->format),
							 AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  input_copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
							ags_audio_buffer_util_format_from_soundcard(source->format));

  buffer_size = source->buffer_size;
  samplerate = source->samplerate;
  
  output_buffer = eq10_audio_signal->output_buffer;
  input_buffer = eq10_audio_signal->input_buffer;

  cache_28hz = eq10_audio_signal->cache_28hz;
  cache_56hz = eq10_audio_signal->cache_56hz;
  cache_112hz = eq10_audio_signal->cache_112hz;
  cache_224hz = eq10_audio_signal->cache_224hz;
  cache_448hz = eq10_audio_signal->cache_448hz;
  cache_896hz = eq10_audio_signal->cache_896hz;
  cache_1792hz = eq10_audio_signal->cache_1792hz;
  cache_3584hz = eq10_audio_signal->cache_3584hz;
  cache_7168hz = eq10_audio_signal->cache_7168hz;
  cache_14336hz = eq10_audio_signal->cache_14336hz;
  
  /* retrieve port values */
  g_value_init(&value, G_TYPE_FLOAT);

  ags_port_safe_read(eq10_channel->pressure, &value);
  
  pressure = g_value_get_float(&value) / 10.0;
  g_value_reset(&value);
  
  ags_port_safe_read(eq10_channel->peak_28hz, &value);
  
  peak_28hz = g_value_get_float(&value) / 2.0 - 0.01;
  g_value_reset(&value);

  ags_port_safe_read(eq10_channel->peak_56hz, &value);
  
  peak_56hz = g_value_get_float(&value) / 2.0 - 0.01;
  g_value_reset(&value);

  ags_port_safe_read(eq10_channel->peak_112hz, &value);
  
  peak_112hz = g_value_get_float(&value) / 2.0 - 0.01;
  g_value_reset(&value);

  ags_port_safe_read(eq10_channel->peak_224hz, &value);
  
  peak_224hz = g_value_get_float(&value) / 2.0 - 0.01;
  g_value_reset(&value);

  ags_port_safe_read(eq10_channel->peak_448hz, &value);
  
  peak_448hz = g_value_get_float(&value) / 2.0 - 0.01;
  g_value_reset(&value);
  
  ags_port_safe_read(eq10_channel->peak_896hz, &value);
  
  peak_896hz = g_value_get_float(&value) / 2.0 - 0.01;
  g_value_reset(&value);

  ags_port_safe_read(eq10_channel->peak_1792hz, &value);
  
  peak_1792hz = g_value_get_float(&value) / 2.0 - 0.01;
  g_value_reset(&value);

  ags_port_safe_read(eq10_channel->peak_3584hz, &value);
  
  peak_3584hz = g_value_get_float(&value) / 2.0 - 0.01;
  g_value_reset(&value);

  ags_port_safe_read(eq10_channel->peak_7168hz, &value);
  
  peak_7168hz = g_value_get_float(&value) / 2.0 - 0.01;
  g_value_reset(&value);

  ags_port_safe_read(eq10_channel->peak_14336hz, &value);
  
  peak_14336hz = g_value_get_float(&value) / 2.0 - 0.01;

  g_value_unset(&value);

  /* clear/copy - preserve trailing */
  if(buffer_size > 8){
    /* clear buffer */
    ags_audio_buffer_util_clear_double(input_buffer, 1,
				       buffer_size - 2);
    
    /* copy input */
    ags_audio_buffer_util_copy_buffer_to_buffer(input_buffer, 1, 0,
						stream_source->data, 1, 0,
						buffer_size - 2, input_copy_mode);
  }
  
  /* equalizer */
  for(i = 0; i + 8 < buffer_size; i += 8){
    gdouble resonator;
    gdouble frequency;
    
    /* 28Hz */
    resonator = peak_28hz;
    frequency = 2.0 * M_PI * 28.0 / (gdouble) samplerate;
    
    if(i == 0){
      cache_28hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_28hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_28hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
      cache_28hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_28hz[0] - (resonator * resonator) * cache_28hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];
      cache_28hz[2] = input_buffer[2] - (resonator * input_buffer[0]) + (2.0 * resonator * cos(frequency)) * cache_28hz[1] - (resonator * resonator) * cache_28hz[0];
      cache_28hz[3] = input_buffer[3] - (resonator * input_buffer[1]) + (2.0 * resonator * cos(frequency)) * cache_28hz[2] - (resonator * resonator) * cache_28hz[1];
      cache_28hz[4] = input_buffer[4] - (resonator * input_buffer[2]) + (2.0 * resonator * cos(frequency)) * cache_28hz[3] - (resonator * resonator) * cache_28hz[2];
      cache_28hz[5] = input_buffer[5] - (resonator * input_buffer[3]) + (2.0 * resonator * cos(frequency)) * cache_28hz[4] - (resonator * resonator) * cache_28hz[3];
      cache_28hz[6] = input_buffer[6] - (resonator * input_buffer[4]) + (2.0 * resonator * cos(frequency)) * cache_28hz[5] - (resonator * resonator) * cache_28hz[4];
      cache_28hz[7] = input_buffer[7] - (resonator * input_buffer[5]) + (2.0 * resonator * cos(frequency)) * cache_28hz[6] - (resonator * resonator) * cache_28hz[5];
    }else{
      cache_28hz[0] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_28hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_28hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
      cache_28hz[1] = input_buffer[i + 1] - (resonator * input_buffer[i - 1]) + (2.0 * resonator * cos(frequency)) * cache_28hz[0] - (resonator * resonator) * cache_28hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];
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
      cache_56hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_56hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_56hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
      cache_56hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_56hz[0] - (resonator * resonator) * cache_56hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];
      cache_56hz[2] = input_buffer[2] - (resonator * input_buffer[0]) + (2.0 * resonator * cos(frequency)) * cache_56hz[1] - (resonator * resonator) * cache_56hz[0];
      cache_56hz[3] = input_buffer[3] - (resonator * input_buffer[1]) + (2.0 * resonator * cos(frequency)) * cache_56hz[2] - (resonator * resonator) * cache_56hz[1];
      cache_56hz[4] = input_buffer[4] - (resonator * input_buffer[2]) + (2.0 * resonator * cos(frequency)) * cache_56hz[3] - (resonator * resonator) * cache_56hz[2];
      cache_56hz[5] = input_buffer[5] - (resonator * input_buffer[3]) + (2.0 * resonator * cos(frequency)) * cache_56hz[4] - (resonator * resonator) * cache_56hz[3];
      cache_56hz[6] = input_buffer[6] - (resonator * input_buffer[4]) + (2.0 * resonator * cos(frequency)) * cache_56hz[5] - (resonator * resonator) * cache_56hz[4];
      cache_56hz[7] = input_buffer[7] - (resonator * input_buffer[5]) + (2.0 * resonator * cos(frequency)) * cache_56hz[6] - (resonator * resonator) * cache_56hz[5];
    }else{
      cache_56hz[0] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_56hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_56hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
      cache_56hz[1] = input_buffer[i + 1] - (resonator * input_buffer[i - 1]) + (2.0 * resonator * cos(frequency)) * cache_56hz[0] - (resonator * resonator) * cache_56hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];
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
      cache_112hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_112hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_112hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
      cache_112hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_112hz[0] - (resonator * resonator) * cache_112hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];
      cache_112hz[2] = input_buffer[2] - (resonator * input_buffer[0]) + (2.0 * resonator * cos(frequency)) * cache_112hz[1] - (resonator * resonator) * cache_112hz[0];
      cache_112hz[3] = input_buffer[3] - (resonator * input_buffer[1]) + (2.0 * resonator * cos(frequency)) * cache_112hz[2] - (resonator * resonator) * cache_112hz[1];
      cache_112hz[4] = input_buffer[4] - (resonator * input_buffer[2]) + (2.0 * resonator * cos(frequency)) * cache_112hz[3] - (resonator * resonator) * cache_112hz[2];
      cache_112hz[5] = input_buffer[5] - (resonator * input_buffer[3]) + (2.0 * resonator * cos(frequency)) * cache_112hz[4] - (resonator * resonator) * cache_112hz[3];
      cache_112hz[6] = input_buffer[6] - (resonator * input_buffer[4]) + (2.0 * resonator * cos(frequency)) * cache_112hz[5] - (resonator * resonator) * cache_112hz[4];
      cache_112hz[7] = input_buffer[7] - (resonator * input_buffer[5]) + (2.0 * resonator * cos(frequency)) * cache_112hz[6] - (resonator * resonator) * cache_112hz[5];
    }else{
      cache_112hz[0] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_112hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_112hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
      cache_112hz[1] = input_buffer[i + 1] - (resonator * input_buffer[i - 1]) + (2.0 * resonator * cos(frequency)) * cache_112hz[0] - (resonator * resonator) * cache_112hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];
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
      cache_224hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_224hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_224hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
      cache_224hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_224hz[0] - (resonator * resonator) * cache_224hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];
      cache_224hz[2] = input_buffer[2] - (resonator * input_buffer[0]) + (2.0 * resonator * cos(frequency)) * cache_224hz[1] - (resonator * resonator) * cache_224hz[0];
      cache_224hz[3] = input_buffer[3] - (resonator * input_buffer[1]) + (2.0 * resonator * cos(frequency)) * cache_224hz[2] - (resonator * resonator) * cache_224hz[1];
      cache_224hz[4] = input_buffer[4] - (resonator * input_buffer[2]) + (2.0 * resonator * cos(frequency)) * cache_224hz[3] - (resonator * resonator) * cache_224hz[2];
      cache_224hz[5] = input_buffer[5] - (resonator * input_buffer[3]) + (2.0 * resonator * cos(frequency)) * cache_224hz[4] - (resonator * resonator) * cache_224hz[3];
      cache_224hz[6] = input_buffer[6] - (resonator * input_buffer[4]) + (2.0 * resonator * cos(frequency)) * cache_224hz[5] - (resonator * resonator) * cache_224hz[4];
      cache_224hz[7] = input_buffer[7] - (resonator * input_buffer[5]) + (2.0 * resonator * cos(frequency)) * cache_224hz[6] - (resonator * resonator) * cache_224hz[5];
    }else{
      cache_224hz[0] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_224hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_224hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
      cache_224hz[1] = input_buffer[i + 1] - (resonator * input_buffer[i - 1]) + (2.0 * resonator * cos(frequency)) * cache_224hz[0] - (resonator * resonator) * cache_224hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];
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
      cache_448hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_448hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_448hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
      cache_448hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_448hz[0] - (resonator * resonator) * cache_448hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];
      cache_448hz[2] = input_buffer[2] - (resonator * input_buffer[0]) + (2.0 * resonator * cos(frequency)) * cache_448hz[1] - (resonator * resonator) * cache_448hz[0];
      cache_448hz[3] = input_buffer[3] - (resonator * input_buffer[1]) + (2.0 * resonator * cos(frequency)) * cache_448hz[2] - (resonator * resonator) * cache_448hz[1];
      cache_448hz[4] = input_buffer[4] - (resonator * input_buffer[2]) + (2.0 * resonator * cos(frequency)) * cache_448hz[3] - (resonator * resonator) * cache_448hz[2];
      cache_448hz[5] = input_buffer[5] - (resonator * input_buffer[3]) + (2.0 * resonator * cos(frequency)) * cache_448hz[4] - (resonator * resonator) * cache_448hz[3];
      cache_448hz[6] = input_buffer[6] - (resonator * input_buffer[4]) + (2.0 * resonator * cos(frequency)) * cache_448hz[5] - (resonator * resonator) * cache_448hz[4];
      cache_448hz[7] = input_buffer[7] - (resonator * input_buffer[5]) + (2.0 * resonator * cos(frequency)) * cache_448hz[6] - (resonator * resonator) * cache_448hz[5];
    }else{
      cache_448hz[0] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_448hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_448hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
      cache_448hz[1] = input_buffer[i + 1] - (resonator * input_buffer[i - 1]) + (2.0 * resonator * cos(frequency)) * cache_448hz[0] - (resonator * resonator) * cache_448hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];
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
      cache_896hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_896hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_896hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
      cache_896hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_896hz[0] - (resonator * resonator) * cache_896hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];
      cache_896hz[2] = input_buffer[2] - (resonator * input_buffer[0]) + (2.0 * resonator * cos(frequency)) * cache_896hz[1] - (resonator * resonator) * cache_896hz[0];
      cache_896hz[3] = input_buffer[3] - (resonator * input_buffer[1]) + (2.0 * resonator * cos(frequency)) * cache_896hz[2] - (resonator * resonator) * cache_896hz[1];
      cache_896hz[4] = input_buffer[4] - (resonator * input_buffer[2]) + (2.0 * resonator * cos(frequency)) * cache_896hz[3] - (resonator * resonator) * cache_896hz[2];
      cache_896hz[5] = input_buffer[5] - (resonator * input_buffer[3]) + (2.0 * resonator * cos(frequency)) * cache_896hz[4] - (resonator * resonator) * cache_896hz[3];
      cache_896hz[6] = input_buffer[6] - (resonator * input_buffer[4]) + (2.0 * resonator * cos(frequency)) * cache_896hz[5] - (resonator * resonator) * cache_896hz[4];
      cache_896hz[7] = input_buffer[7] - (resonator * input_buffer[5]) + (2.0 * resonator * cos(frequency)) * cache_896hz[6] - (resonator * resonator) * cache_896hz[5];
    }else{
      cache_896hz[0] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_896hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_896hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
      cache_896hz[1] = input_buffer[i + 1] - (resonator * input_buffer[i - 1]) + (2.0 * resonator * cos(frequency)) * cache_896hz[0] - (resonator * resonator) * cache_896hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];
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
      cache_1792hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_1792hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
      cache_1792hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[0] - (resonator * resonator) * cache_1792hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];
      cache_1792hz[2] = input_buffer[2] - (resonator * input_buffer[0]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[1] - (resonator * resonator) * cache_1792hz[0];
      cache_1792hz[3] = input_buffer[3] - (resonator * input_buffer[1]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[2] - (resonator * resonator) * cache_1792hz[1];
      cache_1792hz[4] = input_buffer[4] - (resonator * input_buffer[2]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[3] - (resonator * resonator) * cache_1792hz[2];
      cache_1792hz[5] = input_buffer[5] - (resonator * input_buffer[3]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[4] - (resonator * resonator) * cache_1792hz[3];
      cache_1792hz[6] = input_buffer[6] - (resonator * input_buffer[4]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[5] - (resonator * resonator) * cache_1792hz[4];
      cache_1792hz[7] = input_buffer[7] - (resonator * input_buffer[5]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[6] - (resonator * resonator) * cache_1792hz[5];
    }else{
      cache_1792hz[0] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_1792hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
      cache_1792hz[1] = input_buffer[i + 1] - (resonator * input_buffer[i - 1]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[0] - (resonator * resonator) * cache_1792hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];
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
      cache_3584hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_3584hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
      cache_3584hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[0] - (resonator * resonator) * cache_3584hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];
      cache_3584hz[2] = input_buffer[2] - (resonator * input_buffer[0]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[1] - (resonator * resonator) * cache_3584hz[0];
      cache_3584hz[3] = input_buffer[3] - (resonator * input_buffer[1]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[2] - (resonator * resonator) * cache_3584hz[1];
      cache_3584hz[4] = input_buffer[4] - (resonator * input_buffer[2]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[3] - (resonator * resonator) * cache_3584hz[2];
      cache_3584hz[5] = input_buffer[5] - (resonator * input_buffer[3]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[4] - (resonator * resonator) * cache_3584hz[3];
      cache_3584hz[6] = input_buffer[6] - (resonator * input_buffer[4]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[5] - (resonator * resonator) * cache_3584hz[4];
      cache_3584hz[7] = input_buffer[7] - (resonator * input_buffer[5]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[6] - (resonator * resonator) * cache_3584hz[5];
    }else{
      cache_3584hz[0] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_3584hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
      cache_3584hz[1] = input_buffer[i + 1] - (resonator * input_buffer[i - 1]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[0] - (resonator * resonator) * cache_3584hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];
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
      cache_7168hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_7168hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
      cache_7168hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[0] - (resonator * resonator) * cache_7168hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];
      cache_7168hz[2] = input_buffer[2] - (resonator * input_buffer[0]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[1] - (resonator * resonator) * cache_7168hz[0];
      cache_7168hz[3] = input_buffer[3] - (resonator * input_buffer[1]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[2] - (resonator * resonator) * cache_7168hz[1];
      cache_7168hz[4] = input_buffer[4] - (resonator * input_buffer[2]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[3] - (resonator * resonator) * cache_7168hz[2];
      cache_7168hz[5] = input_buffer[5] - (resonator * input_buffer[3]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[4] - (resonator * resonator) * cache_7168hz[3];
      cache_7168hz[6] = input_buffer[6] - (resonator * input_buffer[4]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[5] - (resonator * resonator) * cache_7168hz[4];
      cache_7168hz[7] = input_buffer[7] - (resonator * input_buffer[5]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[6] - (resonator * resonator) * cache_7168hz[5];
    }else{
      cache_7168hz[0] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_7168hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
      cache_7168hz[1] = input_buffer[i + 1] - (resonator * input_buffer[i - 1]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[0] - (resonator * resonator) * cache_7168hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];
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
      cache_14336hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_14336hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
      cache_14336hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[0] - (resonator * resonator) * cache_14336hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];
      cache_14336hz[2] = input_buffer[2] - (resonator * input_buffer[0]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[1] - (resonator * resonator) * cache_14336hz[0];
      cache_14336hz[3] = input_buffer[3] - (resonator * input_buffer[1]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[2] - (resonator * resonator) * cache_14336hz[1];
      cache_14336hz[4] = input_buffer[4] - (resonator * input_buffer[2]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[3] - (resonator * resonator) * cache_14336hz[2];
      cache_14336hz[5] = input_buffer[5] - (resonator * input_buffer[3]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[4] - (resonator * resonator) * cache_14336hz[3];
      cache_14336hz[6] = input_buffer[6] - (resonator * input_buffer[4]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[5] - (resonator * resonator) * cache_14336hz[4];
      cache_14336hz[7] = input_buffer[7] - (resonator * input_buffer[5]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[6] - (resonator * resonator) * cache_14336hz[5];
    }else{
      cache_14336hz[0] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_14336hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
      cache_14336hz[1] = input_buffer[i + 1] - (resonator * input_buffer[i - 1]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[0] - (resonator * resonator) * cache_14336hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];
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
      ags_audio_buffer_util_copy_buffer_to_buffer(input_buffer, 1, buffer_size - 3,
						  stream_source->data, 1, buffer_size - 3,
						  2, input_copy_mode);
    }

    /* fill output */
    ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						cache_28hz, 1,
						AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE);

    ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						cache_56hz, 1,
						AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE);

    ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						cache_112hz, 1,
						AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE);

    ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						cache_224hz, 1,
						AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE);

    ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						cache_448hz, 1,
						AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE);

    ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						cache_896hz, 1,
						AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE);

    ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						cache_1792hz, 1,
						AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE);

    ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						cache_3584hz, 1,
						AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE);

    ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						cache_7168hz, 1,
						AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE);

    ags_audio_buffer_util_copy_double_to_double(output_buffer + i, 1,
						cache_14336hz, 1,
						AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE);
  }

  if(buffer_size >= 4){
    for(; i < buffer_size; i++){
      gdouble resonator;
      gdouble frequency;
      
      /* clear buffer */
      ags_audio_buffer_util_clear_double(input_buffer + i, 1,
					 1);
      
      /* copy input */
      ags_audio_buffer_util_copy_buffer_to_buffer(input_buffer, 1, i,
						  stream_source->data, 1, i,
						  1, input_copy_mode);

      if(i == 0){
	resonator = peak_28hz;
	frequency = 2.0 * M_PI * 28.0 / (gdouble) samplerate;
	cache_28hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_28hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_28hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
	
	resonator = peak_56hz;
	frequency = 2.0 * M_PI * 56.0 / (gdouble) samplerate;
	cache_56hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_56hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_56hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];

	resonator = peak_112hz;
	frequency = 2.0 * M_PI * 112.0 / (gdouble) samplerate;
	cache_112hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_112hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_112hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];

	resonator = peak_224hz;
	frequency = 2.0 * M_PI * 224.0 / (gdouble) samplerate;
	cache_224hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_224hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_224hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];

	resonator = peak_448hz;
	frequency = 2.0 * M_PI * 448.0 / (gdouble) samplerate;
	cache_448hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_448hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_448hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];

	resonator = peak_896hz;
	frequency = 2.0 * M_PI * 896.0 / (gdouble) samplerate;
	cache_896hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_896hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_896hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];

	resonator = peak_1792hz;
	frequency = 2.0 * M_PI * 1792.0 / (gdouble) samplerate;
	cache_1792hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_1792hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];

	resonator = peak_3584hz;
	frequency = 2.0 * M_PI * 3584.0 / (gdouble) samplerate;
	cache_3584hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_3584hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];

	resonator = peak_7168hz;
	frequency = 2.0 * M_PI * 7168.0 / (gdouble) samplerate;
	cache_7168hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_7168hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];

	resonator = peak_14336hz;
	frequency = 2.0 * M_PI * 14336.0 / (gdouble) samplerate;
	cache_14336hz[0] = input_buffer[0] - (resonator * input_buffer[buffer_size - 2]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_14336hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
      }else if(i == 1){
	resonator = peak_28hz;
	frequency = 2.0 * M_PI * 28.0 / (gdouble) samplerate;
        cache_28hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_28hz[0] - (resonator * resonator) * cache_28hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];

	resonator = peak_56hz;
	frequency = 2.0 * M_PI * 56.0 / (gdouble) samplerate;
        cache_56hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_56hz[0] - (resonator * resonator) * cache_56hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];

	resonator = peak_112hz;
	frequency = 2.0 * M_PI * 112.0 / (gdouble) samplerate;
        cache_112hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_112hz[0] - (resonator * resonator) * cache_112hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];

	resonator = peak_224hz;
	frequency = 2.0 * M_PI * 224.0 / (gdouble) samplerate;
        cache_224hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_224hz[0] - (resonator * resonator) * cache_224hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];

	resonator = peak_448hz;
	frequency = 2.0 * M_PI * 448.0 / (gdouble) samplerate;
        cache_448hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_448hz[0] - (resonator * resonator) * cache_448hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];

	resonator = peak_896hz;
	frequency = 2.0 * M_PI * 896.0 / (gdouble) samplerate;
        cache_896hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_896hz[0] - (resonator * resonator) * cache_896hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];

	resonator = peak_1792hz;
	frequency = 2.0 * M_PI * 1792.0 / (gdouble) samplerate;
        cache_1792hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[0] - (resonator * resonator) * cache_1792hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];

	resonator = peak_3584hz;
	frequency = 2.0 * M_PI * 3584.0 / (gdouble) samplerate;
        cache_3584hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[0] - (resonator * resonator) * cache_3584hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];

	resonator = peak_7168hz;
	frequency = 2.0 * M_PI * 7168.0 / (gdouble) samplerate;
        cache_7168hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[0] - (resonator * resonator) * cache_7168hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];

	resonator = peak_14336hz;
	frequency = 2.0 * M_PI * 14336.0 / (gdouble) samplerate;
        cache_14336hz[1] = input_buffer[1] - (resonator * input_buffer[buffer_size - 1]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[0] - (resonator * resonator) * cache_14336hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1];
      }else{
	resonator = peak_28hz;
	frequency = 2.0 * M_PI * 28.0 / (gdouble) samplerate;
	cache_28hz[i % 8] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_28hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_28hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];

	resonator = peak_56hz;
	frequency = 2.0 * M_PI * 56.0 / (gdouble) samplerate;
	cache_56hz[i % 8] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_56hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_56hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];

	resonator = peak_112hz;
	frequency = 2.0 * M_PI * 112.0 / (gdouble) samplerate;
	cache_112hz[i % 8] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_112hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_112hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];

	resonator = peak_224hz;
	frequency = 2.0 * M_PI * 224.0 / (gdouble) samplerate;
	cache_224hz[i % 8] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_224hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_224hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];

	resonator = peak_448hz;
	frequency = 2.0 * M_PI * 448.0 / (gdouble) samplerate;
	cache_448hz[i % 8] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_448hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_448hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];

	resonator = peak_896hz;
	frequency = 2.0 * M_PI * 896.0 / (gdouble) samplerate;
	cache_896hz[i % 8] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_896hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_896hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];

	resonator = peak_1792hz;
	frequency = 2.0 * M_PI * 1792.0 / (gdouble) samplerate;
	cache_1792hz[i % 8] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_1792hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_1792hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];

	resonator = peak_3584hz;
	frequency = 2.0 * M_PI * 3584.0 / (gdouble) samplerate;
	cache_3584hz[i % 8] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_3584hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_3584hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];

	resonator = peak_7168hz;
	frequency = 2.0 * M_PI * 7168.0 / (gdouble) samplerate;
	cache_7168hz[i % 8] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_7168hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_7168hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];

	resonator = peak_14336hz;
	frequency = 2.0 * M_PI * 14336.0 / (gdouble) samplerate;
	cache_14336hz[i % 8] = input_buffer[i] - (resonator * input_buffer[i - 2]) + (2.0 * resonator * cos(frequency)) * cache_14336hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 1] - (resonator * resonator) * cache_14336hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE - 2];
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
  
  /* copy output */
  ags_audio_buffer_util_copy_buffer_to_buffer(stream_source->data, 1, 0,
					      output_buffer, 1, 0,
					      buffer_size, output_copy_mode);
}

/**
 * ags_eq10_audio_signal_new:
 * @audio_signal: an #AgsAudioSignal
 *
 * Creates an #AgsEq10AudioSignal
 *
 * Returns: a new #AgsEq10AudioSignal
 *
 * Since: 1.5.0
 */
AgsEq10AudioSignal*
ags_eq10_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsEq10AudioSignal *eq10_audio_signal;

  eq10_audio_signal = (AgsEq10AudioSignal *) g_object_new(AGS_TYPE_EQ10_AUDIO_SIGNAL,
							  NULL);

  return(eq10_audio_signal);
}
