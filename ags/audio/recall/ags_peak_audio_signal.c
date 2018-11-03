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
 *
 * Introduction to Digital Filters with Audio Applications
 * Copyright (C) 2007-2018 Julius O. Smith III
 * 
 * License: STK-4.3
 * 
 * See COPYING.stk-4.3 for further deatails
 * 
 * https://ccrma.stanford.edu/~jos
 */

#include <ags/audio/recall/ags_peak_audio_signal.h>

#include <ags/libags.h>

#include <ags/audio/recall/ags_peak_channel.h>
#include <ags/audio/recall/ags_peak_channel_run.h>
#include <ags/audio/recall/ags_peak_recycling.h>

#include <ags/i18n.h>

void ags_peak_audio_signal_class_init(AgsPeakAudioSignalClass *peak_audio_signal);
void ags_peak_audio_signal_init(AgsPeakAudioSignal *peak_audio_signal);
void ags_peak_audio_signal_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_peak_audio_signal_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_peak_audio_signal_finalize(GObject *gobject);

void ags_peak_audio_signal_run_inter(AgsRecall *recall);
AgsRecall* ags_peak_audio_signal_duplicate(AgsRecall *recall,
					   AgsRecallID *recall_id,
					   guint *n_params, GParameter *parameter);

/**
 * SECTION:ags_peak_audio_signal
 * @short_description: peaks audio signal
 * @title: AgsPeakAudioSignal
 * @section_id:
 * @include: ags/audio/recall/ags_peak_audio_signal.h
 *
 * The #AgsPeakAudioSignal class peaks the audio signal.
 */

enum{
  PROP_0,
  PROP_PEAK,
};

static gpointer ags_peak_audio_signal_parent_class = NULL;

GType
ags_peak_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_peak_audio_signal = 0;

    static const GTypeInfo ags_peak_audio_signal_info = {
      sizeof (AgsPeakAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_peak_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPeakAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_peak_audio_signal_init,
    };

    ags_type_peak_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							"AgsPeakAudioSignal",
							&ags_peak_audio_signal_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_peak_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_peak_audio_signal_class_init(AgsPeakAudioSignalClass *peak_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  GParamSpec *param_spec;

  ags_peak_audio_signal_parent_class = g_type_class_peek_parent(peak_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) peak_audio_signal;

  gobject->set_property = ags_peak_audio_signal_set_property;
  gobject->get_property = ags_peak_audio_signal_get_property;

  gobject->finalize = ags_peak_audio_signal_finalize;

  /* properties */
  /**
   * AgsPeakAudioSignal:peak:
   * 
   * The peak.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_double("peak",
				   i18n_pspec("resulting peak"),
				   i18n_pspec("The peak resulted"),
				   0.0,
				   1.0,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PEAK,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) peak_audio_signal;

  recall->run_inter = ags_peak_audio_signal_run_inter;
}

void
ags_peak_audio_signal_init(AgsPeakAudioSignal *peak_audio_signal)
{
  AGS_RECALL(peak_audio_signal)->child_type = G_TYPE_NONE;

  peak_audio_signal->peak = 0.0;
}

void
ags_peak_audio_signal_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsPeakAudioSignal *peak_audio_signal;

  pthread_mutex_t *recall_mutex;

  peak_audio_signal = AGS_PEAK_AUDIO_SIGNAL(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_PEAK:
    {
      gdouble peak;

      peak = g_value_get_double(value);

      pthread_mutex_lock(recall_mutex);

      peak_audio_signal->peak = peak;
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_peak_audio_signal_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsPeakAudioSignal *peak_audio_signal;

  pthread_mutex_t *recall_mutex;

  peak_audio_signal = AGS_PEAK_AUDIO_SIGNAL(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_PEAK:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_double(value, peak_audio_signal->peak);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_peak_audio_signal_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_peak_audio_signal_parent_class)->finalize(gobject);
}

void
ags_peak_audio_signal_run_inter(AgsRecall *recall)
{
  AgsAudioSignal *source;
  AgsPeakChannel *peak_channel;
  AgsPeakChannelRun *peak_channel_run;
  AgsPeakRecycling *peak_recycling;
  AgsPeakAudioSignal *peak_audio_signal;

  guint samplerate;
  guint buffer_size;
  guint format;
  
  void (*parent_class_run_inter)(AgsRecall *recall);  

  pthread_mutex_t *recall_mutex;

  peak_audio_signal = (AgsPeakAudioSignal *) recall;

  /* get mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = recall->obj_mutex;
  
  parent_class_run_inter = AGS_RECALL_CLASS(ags_peak_audio_signal_parent_class)->run_inter;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* call parent */
  parent_class_run_inter(recall);

  /* get some fields */
  g_object_get(peak_audio_signal,
	       "source", &source,
	       NULL);

  if(source->stream_current != NULL){
    g_object_get(peak_audio_signal,
		 "parent", &peak_recycling,
		 NULL);
    
    g_object_get(peak_recycling,
		 "parent", &peak_channel_run,
		 NULL);

    g_object_get(peak_channel_run,
		 "recall-channel", &peak_channel,
		 NULL);

    g_object_get(source,
		 "samplerate", &samplerate,
		 "buffer-size", &buffer_size,
		 "format", &format,
		 NULL);
    
    ags_peak_channel_buffer_add(peak_channel,
				source->stream_current->data,
				samplerate,
				buffer_size,
				format);
  }else{
    ags_recall_done(recall);
  }
}

/**
 * ags_peak_audio_signal_new:
 * @source: the #AgsAudioSignal
 *
 * Create a new instance of #AgsPeakAudioSignal
 *
 * Returns: the new #AgsPeakAudioSignal
 *
 * Since: 2.0.0
 */
AgsPeakAudioSignal*
ags_peak_audio_signal_new(AgsAudioSignal *source)
{
  AgsPeakAudioSignal *peak_audio_signal;

  peak_audio_signal = (AgsPeakAudioSignal *) g_object_new(AGS_TYPE_PEAK_AUDIO_SIGNAL,
							  "source", source,
							  NULL);

  return(peak_audio_signal);
}
