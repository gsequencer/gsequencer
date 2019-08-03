/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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
 *
 * Introduction to Digital Filters with Audio Applications
 * Copyright (C) 2007-2018 Julius O. Smith III
 * 
 * License: STK-4.3
 * 
 * See COPYING.stk-4.3 for further deatails
 * 
 * https://ccrma.stanford.edu/~jos
 * 
 * provided the foundation of the modified equalizer algorithm, this
 *   recall is based on frequency response filter.
 */

#include <ags/audio/recall/ags_eq10_audio_signal.h>

#include <ags/libags.h>

#include <ags/audio/ags_input.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/recall/ags_eq10_channel.h>
#include <ags/audio/recall/ags_eq10_channel_run.h>
#include <ags/audio/recall/ags_eq10_recycling.h>

#include <math.h>

void ags_eq10_audio_signal_class_init(AgsEq10AudioSignalClass *eq10_audio_signal);
void ags_eq10_audio_signal_init(AgsEq10AudioSignal *eq10_audio_signal);
void ags_eq10_audio_signal_finalize(GObject *gobject);

void ags_eq10_audio_signal_run_init_pre(AgsRecall *recall);
void ags_eq10_audio_signal_run_inter(AgsRecall *recall);
AgsRecall* ags_eq10_audio_signal_duplicate(AgsRecall *recall,
					   AgsRecallID *recall_id,
					   guint *n_params, GParameter *parameter);

/**
 * SECTION:ags_eq10_audio_signal
 * @short_description: 10 band equalizer audio signal
 * @title: AgsEq10AudioSignal
 * @section_id:
 * @include: ags/audio/recall/ags_eq10_audio_signal.h
 *
 * The #AgsEq10AudioSignal class eq10s the audio signal.
 */

static gpointer ags_eq10_audio_signal_parent_class = NULL;

GType
ags_eq10_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_eq10_audio_signal = 0;

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

    ags_type_eq10_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							"AgsEq10AudioSignal",
							&ags_eq10_audio_signal_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_eq10_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_eq10_audio_signal_class_init(AgsEq10AudioSignalClass *eq10_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

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
ags_eq10_audio_signal_run_init_pre(AgsRecall *recall)
{
  AgsAudioSignal *source;
  AgsEq10AudioSignal *eq10_audio_signal;

  guint buffer_size;

  void (*parent_class_run_init_pre)(AgsRecall *recall);  

  pthread_mutex_t *recall_mutex;

  eq10_audio_signal = (AgsEq10AudioSignal *) recall;

  /* get parent class */
  AGS_RECALL_LOCK_CLASS();
  
  parent_class_run_init_pre = AGS_RECALL_CLASS(ags_eq10_audio_signal_parent_class)->run_init_pre;

  AGS_RECALL_UNLOCK_CLASS();

  /* get mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);
  
  /* call parent */
  parent_class_run_init_pre(recall);

  /* get some fields */
  g_object_get(eq10_audio_signal,
	       "source", &source,
	       NULL);

  g_object_get(source,
	       "buffer-size", &buffer_size,
	       NULL);

  //FIXME:JK: memory leak
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

  g_object_unref(source);
}

void
ags_eq10_audio_signal_run_inter(AgsRecall *recall)
{
  AgsAudioSignal *source;
  AgsRecallID *recall_id;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;
  AgsPort *port;
  AgsEq10Channel *eq10_channel;
  AgsEq10ChannelRun *eq10_channel_run;
  AgsEq10Recycling *eq10_recycling;
  AgsEq10AudioSignal *eq10_audio_signal;

  GList *note;
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
  guint format;
  guint i;

  GValue value = {0,};

  void (*parent_class_run_inter)(AgsRecall *recall);
  
  pthread_mutex_t *recall_mutex;
  
  eq10_audio_signal = (AgsEq10AudioSignal *) recall;

  /* get parent class */
  AGS_RECALL_LOCK_CLASS();
  
  parent_class_run_inter = AGS_RECALL_CLASS(ags_eq10_audio_signal_parent_class)->run_inter;

  AGS_RECALL_UNLOCK_CLASS();
  
  /* get mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* call parent */
  parent_class_run_inter(recall);

  /* get some fields */
  g_object_get(eq10_audio_signal,
	       "source", &source,
	       "recall-id", &recall_id,
	       NULL);

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);

  g_object_get(source,
	       "note", &note,
	       NULL);
  
  if(ags_recall_global_get_rt_safe() &&
     parent_recycling_context != NULL &&
     note == NULL){
    g_object_unref(source);

    g_object_unref(recall_id);
    
    g_object_unref(recycling_context);
    g_object_unref(parent_recycling_context);
    
    return;
  }

  stream_source = source->stream_current;

  if(stream_source == NULL){
    ags_recall_done(recall);

    g_object_unref(source);

    g_object_unref(recall_id);
    
    g_object_unref(recycling_context);

    if(parent_recycling_context != NULL){
      g_object_unref(parent_recycling_context);
    }
    
    g_list_free_full(note,
		     g_object_unref);
    
    return;
  }

  g_object_get(source,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);

  g_object_get(eq10_audio_signal,
	       "parent", &eq10_recycling,
	       NULL);

  g_object_get(eq10_recycling,
	       "parent", &eq10_channel_run,
	       NULL);

  g_object_get(eq10_channel_run,
	       "recall-channel", &eq10_channel,
	       NULL);

  /* copy mode */
  output_copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
							 AGS_AUDIO_BUFFER_UTIL_DOUBLE);

  input_copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
							ags_audio_buffer_util_format_from_soundcard(format));
  
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
  g_value_init(&value,
	       G_TYPE_FLOAT);

  g_object_get(eq10_channel,
	       "pressure", &port,
	       NULL);
  ags_port_safe_read(port,
		     &value);
  
  pressure = g_value_get_float(&value) / 10.0;
  g_value_reset(&value);

  g_object_unref(port);
  
  g_object_get(eq10_channel,
	       "peak-28hz", &port,
	       NULL);
  ags_port_safe_read(port,
		     &value);
  
  peak_28hz = g_value_get_float(&value) / 2.0 - 0.01;
  g_value_reset(&value);

  g_object_unref(port);

  g_object_get(eq10_channel,
	       "peak-56hz", &port,
	       NULL);
  ags_port_safe_read(port,
		     &value);

  peak_56hz = g_value_get_float(&value) / 2.0 - 0.01;
  g_value_reset(&value);
  
  g_object_unref(port);

  g_object_get(eq10_channel,
	       "peak-112hz", &port,
	       NULL);
  ags_port_safe_read(port,
		     &value);

  peak_112hz = g_value_get_float(&value) / 2.0 - 0.01;
  g_value_reset(&value);
  
  g_object_unref(port);

  g_object_get(eq10_channel,
	       "peak-224hz", &port,
	       NULL);
  ags_port_safe_read(port,
		     &value);

  peak_224hz = g_value_get_float(&value) / 2.0 - 0.01;
  g_value_reset(&value);

  g_object_unref(port);

  g_object_get(eq10_channel,
	       "peak-448hz", &port,
	       NULL);
  ags_port_safe_read(port,
		     &value);
  
  peak_448hz = g_value_get_float(&value) / 2.0 - 0.01;
  g_value_reset(&value);

  g_object_unref(port);

  g_object_get(eq10_channel,
	       "peak-896hz", &port,
	       NULL);  
  ags_port_safe_read(port,
		     &value);
  
  peak_896hz = g_value_get_float(&value) / 2.0 - 0.01;
  g_value_reset(&value);

  g_object_unref(port);

  g_object_get(eq10_channel,
	       "peak-1792hz", &port,
	       NULL);
  ags_port_safe_read(port,
		     &value);
  
  peak_1792hz = g_value_get_float(&value) / 2.0 - 0.01;
  g_value_reset(&value);
  
  g_object_unref(port);

  g_object_get(eq10_channel,
	       "peak-3584hz", &port,
	       NULL);
  ags_port_safe_read(port,
		     &value);
  
  peak_3584hz = g_value_get_float(&value) / 2.0 - 0.01;
  g_value_reset(&value);

  g_object_unref(port);

  g_object_get(eq10_channel,
	       "peak-7168hz", &port,
	       NULL);
  ags_port_safe_read(port,
		     &value);
  
  peak_7168hz = g_value_get_float(&value) / 2.0 - 0.01;
  g_value_reset(&value);
  
  g_object_unref(port);

  g_object_get(eq10_channel,
	       "peak-14336hz", &port,
	       NULL);
  ags_port_safe_read(port, &value);
  
  peak_14336hz = g_value_get_float(&value) / 2.0 - 0.01;

  g_value_unset(&value);
  
  g_object_unref(port);

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

  /* unref */
  g_object_unref(source);

  g_object_unref(recall_id);
    
  g_object_unref(recycling_context);

  if(parent_recycling_context != NULL){
    g_object_unref(parent_recycling_context);
  }
  
  g_list_free_full(note,
		   g_object_unref);
  
  g_object_unref(eq10_recycling);

  g_object_unref(eq10_channel);
  g_object_unref(eq10_channel_run);
}

/**
 * ags_eq10_audio_signal_new:
 * @source: the #AgsAudioSignal
 *
 * Create a new instance of #AgsEq10AudioSignal
 *
 * Returns: the new #AgsEq10AudioSignal
 *
 * Since: 2.0.0
 */
AgsEq10AudioSignal*
ags_eq10_audio_signal_new(AgsAudioSignal *source)
{
  AgsEq10AudioSignal *eq10_audio_signal;

  eq10_audio_signal = (AgsEq10AudioSignal *) g_object_new(AGS_TYPE_EQ10_AUDIO_SIGNAL,
							  "source", source,
							  NULL);

  return(eq10_audio_signal);
}
