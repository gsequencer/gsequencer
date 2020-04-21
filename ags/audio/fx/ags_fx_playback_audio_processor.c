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

#include <ags/audio/fx/ags_fx_playback_audio_processor.h>

#include <ags/audio/fx/ags_fx_playback_audio.h>

#include <ags/i18n.h>

void ags_fx_playback_audio_processor_class_init(AgsFxPlaybackAudioProcessorClass *fx_playback_audio_processor);
void ags_fx_playback_audio_processor_init(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor);
void ags_fx_playback_audio_processor_dispose(GObject *gobject);
void ags_fx_playback_audio_processor_finalize(GObject *gobject);

void ags_fx_playback_audio_processor_real_run_inter(AgsRecall *recall);

void ags_fx_playback_audio_processor_real_data_put(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor,
						   gpointer data,
						   guint samplerate,
						   guint buffer_size,
						   guint format,
						   guint data_mode);
void ags_fx_playback_audio_processor_real_data_get(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor,
						   gpointer data,
						   guint samplerate,
						   guint buffer_size,
						   guint format,
						   guint data_mode);

void ags_fx_playback_audio_processor_real_play(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor);
void ags_fx_playback_audio_processor_real_record(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor);
void ags_fx_playback_audio_processor_real_feed(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor);

void ags_fx_playback_audio_processor_real_counter_change(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor);

/**
 * SECTION:ags_fx_playback_audio_processor
 * @short_description: fx playback audio_processor
 * @title: AgsFxPlaybackAudioProcessor
 * @section_id:
 * @include: ags/audio/fx/ags_fx_playback_audio_processor.h
 *
 * The #AgsFxPlaybackAudioProcessor class provides ports to the effect processor.
 */

static gpointer ags_fx_playback_audio_processor_parent_class = NULL;

GType
ags_fx_playback_audio_processor_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_playback_audio_processor = 0;

    static const GTypeInfo ags_fx_playback_audio_processor_info = {
      sizeof (AgsFxPlaybackAudioProcessorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_playback_audio_processor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_processor */
      sizeof (AgsFxPlaybackAudioProcessor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_playback_audio_processor_init,
    };

    ags_type_fx_playback_audio_processor = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
								  "AgsFxPlaybackAudioProcessor",
								  &ags_fx_playback_audio_processor_info,
								  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_playback_audio_processor);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_playback_audio_processor_class_init(AgsFxPlaybackAudioProcessorClass *fx_playback_audio_processor)
{
  GObjectClass *gobject;  
  AgsRecallClass *recall;
  
  ags_fx_playback_audio_processor_parent_class = g_type_class_peek_parent(fx_playback_audio_processor);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_playback_audio_processor;

  gobject->dispose = ags_fx_playback_audio_processor_dispose;
  gobject->finalize = ags_fx_playback_audio_processor_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_playback_audio_processor;
  
  recall->run_inter = ags_fx_playback_audio_processor_real_run_inter;

  /* AgsFxPlaybackAudioProcessorClass */
  fx_playback_audio_processor->data_put = ags_fx_playback_audio_processor_real_data_put;
  fx_playback_audio_processor->data_get = ags_fx_playback_audio_processor_real_data_get;

  fx_playback_audio_processor->play = ags_fx_playback_audio_processor_real_play;
  fx_playback_audio_processor->record = ags_fx_playback_audio_processor_real_record;
  fx_playback_audio_processor->feed = ags_fx_playback_audio_processor_real_feed;

  fx_playback_audio_processor->counter_change = ags_fx_playback_audio_processor_real_counter_change;
}

void
ags_fx_playback_audio_processor_init(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{
  AGS_RECALL(fx_playback_audio_processor)->name = "ags-fx-playback";
  AGS_RECALL(fx_playback_audio_processor)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_playback_audio_processor)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_playback_audio_processor)->xml_type = "ags-fx-playback-audio-processor";
}

void
ags_fx_playback_audio_processor_dispose(GObject *gobject)
{
  AgsFxPlaybackAudioProcessor *fx_playback_audio_processor;
  
  fx_playback_audio_processor = AGS_FX_PLAYBACK_AUDIO_PROCESSOR(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_playback_audio_processor_parent_class)->dispose(gobject);
}

void
ags_fx_playback_audio_processor_finalize(GObject *gobject)
{
  AgsFxPlaybackAudioProcessor *fx_playback_audio_processor;
  
  fx_playback_audio_processor = AGS_FX_PLAYBACK_AUDIO_PROCESSOR(gobject);  
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_playback_audio_processor_parent_class)->finalize(gobject);
}

void
ags_fx_playback_audio_processor_real_run_inter(AgsRecall *recall)
{
  //TODO:JK: implement me
}

void
ags_fx_playback_audio_processor_real_data_put(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor,
					      gpointer data,
					      guint samplerate,
					      guint buffer_size,
					      guint format,
					      guint data_mode)
{
  //TODO:JK: implement me
}

void
ags_fx_playback_audio_processor_data_put(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor,
					 gpointer data,
					 guint samplerate,
					 guint buffer_size,
					 guint format,
					 guint data_mode)
{
  g_return_if_fail(AGS_IS_FX_PLAYBACK_AUDIO_PROCESSOR(fx_playback_audio_processor));

  g_object_ref(fx_playback_audio_processor);

  if(AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->data_put != NULL){
    AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->data_put(fx_playback_audio_processor,
										     data,
										     samplerate,
										     buffer_size,
										     format,
										     data_mode);
  }
  
  g_object_unref(fx_playback_audio_processor);
}

void
ags_fx_playback_audio_processor_real_data_get(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor,
					      gpointer data,
					      guint samplerate,
					      guint buffer_size,
					      guint format,
					      guint data_mode)
{
  //TODO:JK: implement me
}

void
ags_fx_playback_audio_processor_data_get(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor,
					 gpointer data,
					 guint samplerate,
					 guint buffer_size,
					 guint format,
					 guint data_mode)
{
  g_return_if_fail(AGS_IS_FX_PLAYBACK_AUDIO_PROCESSOR(fx_playback_audio_processor));

  g_object_ref(fx_playback_audio_processor);

  if(AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->data_get != NULL){
    AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->data_get(fx_playback_audio_processor,
										     data,
										     samplerate,
										     buffer_size,
										     format,
										     data_mode);
  }
  
  g_object_unref(fx_playback_audio_processor);
}

void
ags_fx_playback_audio_processor_real_play(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{
  //TODO:JK: implement me
}

void
ags_fx_playback_audio_processor_play(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{
  g_return_if_fail(AGS_IS_FX_PLAYBACK_AUDIO_PROCESSOR(fx_playback_audio_processor));

  g_object_ref(fx_playback_audio_processor);

  if(AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->play != NULL){
    AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->play(fx_playback_audio_processor);
  }
  
  g_object_unref(fx_playback_audio_processor);
}

void
ags_fx_playback_audio_processor_real_record(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{
  //TODO:JK: implement me
}

void
ags_fx_playback_audio_processor_record(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{
  g_return_if_fail(AGS_IS_FX_PLAYBACK_AUDIO_PROCESSOR(fx_playback_audio_processor));

  g_object_ref(fx_playback_audio_processor);
  
  if(AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->record != NULL){
    AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->record(fx_playback_audio_processor);
  }
  
  g_object_unref(fx_playback_audio_processor);
}

void
ags_fx_playback_audio_processor_real_feed(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{
  //TODO:JK: implement me
}

void
ags_fx_playback_audio_processor_feed(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{
  g_return_if_fail(AGS_IS_FX_PLAYBACK_AUDIO_PROCESSOR(fx_playback_audio_processor));

  g_object_ref(fx_playback_audio_processor);

  if(AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->feed != NULL){
    AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->feed(fx_playback_audio_processor);
  }
  
  g_object_unref(fx_playback_audio_processor);
}

void
ags_fx_playback_audio_processor_real_master(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{
  //TODO:JK: implement me
}

void
ags_fx_playback_audio_processor_master(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{
  g_return_if_fail(AGS_IS_FX_PLAYBACK_AUDIO_PROCESSOR(fx_playback_audio_processor));

  g_object_ref(fx_playback_audio_processor);

  if(AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->master != NULL){
    AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->master(fx_playback_audio_processor);
  }
  
  g_object_unref(fx_playback_audio_processor);
}

void
ags_fx_playback_audio_processor_real_counter_change(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{
  AgsFxPlaybackAudio *fx_playback_audio;

  gdouble delay;
  gboolean loop;
  guint64 loop_start, loop_end;
  
  GValue value = {0,};

  GRecMutex *fx_playback_audio_processor_mutex;
  
  fx_playback_audio_processor = NULL;
  fx_playback_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio_processor);

  g_object_get(fx_playback_audio_processor,
	       "recall-audio", &fx_playback_audio,
	       NULL);

  delay = AGS_SOUNDCARD_DEFAULT_DELAY;

  loop = FALSE;

  loop_start = AGS_FX_PLAYBACK_AUDIO_DEFAULT_LOOP_START;
  loop_end = AGS_FX_PLAYBACK_AUDIO_DEFAULT_LOOP_END;
  
  if(fx_playback_audio != NULL){
    AgsPort *port;

    /* delay */
    g_object_get(fx_playback_audio,
		 "delay", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value,
		   G_TYPE_DOUBLE);
    
      ags_port_safe_read(port,
			 &value);

      delay = g_value_get_double(&value);
      g_value_unset(&value);

      g_object_unref(port);
    }

    /* loop */
    g_object_get(fx_playback_audio,
		 "loop", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value,
		   G_TYPE_BOOLEAN);
    
      ags_port_safe_read(port,
			 &value);

      loop = g_value_get_boolean(&value);
      g_value_unset(&value);

      g_object_unref(port);
    }

    /* loop-start */
    g_object_get(fx_playback_audio,
		 "loop-start", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value,
		   G_TYPE_UINT64);
    
      ags_port_safe_read(port,
			 &value);

      loop_start = g_value_get_uint64(&value);
      g_value_unset(&value);

      g_object_unref(port);
    }

    /* loop-end */
    g_object_get(fx_playback_audio,
		 "loop-end", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value,
		   G_TYPE_UINT64);
    
      ags_port_safe_read(port,
			 &value);

      loop_end = g_value_get_uint64(&value);
      g_value_unset(&value);

      g_object_unref(port);
    }
  }

  if(delay + 1.0 >= delay){
    g_rec_mutex_lock(fx_playback_audio_processor_mutex);
    
    fx_playback_audio_processor->delay_counter = 0.0;

    if(loop &&
       fx_playback_audio_processor->offset_counter + 1 >= loop_end){
      fx_playback_audio_processor->offset_counter = loop_start;
    }else{
      fx_playback_audio_processor->offset_counter += 1;
    }
    
    g_rec_mutex_unlock(fx_playback_audio_processor_mutex);
  }else{
    g_rec_mutex_lock(fx_playback_audio_processor_mutex);
    
    fx_playback_audio_processor->delay_counter += 1.0;

    g_rec_mutex_unlock(fx_playback_audio_processor_mutex);
  }
  
  if(fx_playback_audio != NULL){
    g_object_unref(fx_playback_audio);
  }
}

void
ags_fx_playback_audio_processor_counter_change(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{
  g_return_if_fail(AGS_IS_FX_PLAYBACK_AUDIO_PROCESSOR(fx_playback_audio_processor));

  g_object_ref(fx_playback_audio_processor);
  
  if(AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->counter_change != NULL){
    AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->counter_change(fx_playback_audio_processor);
  }
  
  g_object_unref(fx_playback_audio_processor);
}

/**
 * ags_fx_playback_audio_processor_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxPlaybackAudioProcessor
 *
 * Returns: the new #AgsFxPlaybackAudioProcessor
 *
 * Since: 3.3.0
 */
AgsFxPlaybackAudioProcessor*
ags_fx_playback_audio_processor_new(AgsAudio *audio)
{
  AgsFxPlaybackAudioProcessor *fx_playback_audio_processor;

  fx_playback_audio_processor = (AgsFxPlaybackAudioProcessor *) g_object_new(AGS_TYPE_FX_PLAYBACK_AUDIO_PROCESSOR,
									     "audio", audio,
									     NULL);

  return(fx_playback_audio_processor);
}
