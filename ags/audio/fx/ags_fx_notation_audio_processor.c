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

#include <ags/audio/fx/ags_fx_notation_audio_processor.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_notation.h>
#include <ags/audio/ags_note.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recycling_context.h>

#include <ags/audio/midi/ags_midi_util.h>

#include <ags/audio/fx/ags_fx_notation_audio.h>

#include <ags/i18n.h>

void ags_fx_notation_audio_processor_class_init(AgsFxNotationAudioProcessorClass *fx_notation_audio_processor);
void ags_fx_notation_audio_processor_seekable_interface_init(AgsSeekableInterface *seekable);
void ags_fx_notation_audio_processor_countable_interface_init(AgsCountableInterface *countable);
void ags_fx_notation_audio_processor_tactable_interface_init(AgsTactableInterface *tactable);
void ags_fx_notation_audio_processor_init(AgsFxNotationAudioProcessor *fx_notation_audio_processor);
void ags_fx_notation_audio_processor_dispose(GObject *gobject);
void ags_fx_notation_audio_processor_finalize(GObject *gobject);

void ags_fx_notation_audio_processor_seek(AgsSeekable *seekable,
					  gint64 offset,
					  guint whence);

guint64 ags_fx_notation_audio_processor_get_notation_counter(AgsCountable *countable);

gdouble ags_fx_notation_audio_processor_get_bpm(AgsTactable *tactable);
gdouble ags_fx_notation_audio_processor_get_tact(AgsTactable *tactable);
void ags_fx_notation_audio_processor_change_bpm(AgsTactable *tactable, gdouble new_bpm, gdouble old_bpm);
void ags_fx_notation_audio_processor_change_tact(AgsTactable *tactable, gdouble new_tact, gdouble old_tact);

void ags_fx_notation_audio_processor_run_init_pre(AgsRecall *recall);
void ags_fx_notation_audio_processor_run_inter(AgsRecall *recall);

void ags_fx_notation_audio_processor_real_key_on(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
						 AgsNote *note,
						 guint velocity,
						 guint key_mode);
void ags_fx_notation_audio_processor_real_key_off(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
						  AgsNote *note,
						  guint velocity,
						  guint key_mode);
void ags_fx_notation_audio_processor_real_key_pressure(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
						       AgsNote *note,
						       guint velocity,
						       guint key_mode);

void ags_fx_notation_audio_processor_real_play(AgsFxNotationAudioProcessor *fx_notation_audio_processor);
void ags_fx_notation_audio_processor_real_record(AgsFxNotationAudioProcessor *fx_notation_audio_processor);
void ags_fx_notation_audio_processor_real_feed(AgsFxNotationAudioProcessor *fx_notation_audio_processor);

void ags_fx_notation_audio_processor_real_counter_change(AgsFxNotationAudioProcessor *fx_notation_audio_processor);

/**
 * SECTION:ags_fx_notation_audio_processor
 * @short_description: fx notation audio processor
 * @title: AgsFxNotationAudioProcessor
 * @section_id:
 * @include: ags/audio/fx/ags_fx_notation_audio_processor.h
 *
 * The #AgsFxNotationAudioProcessor class provides ports to the effect processor.
 */

static gpointer ags_fx_notation_audio_processor_parent_class = NULL;

GType
ags_fx_notation_audio_processor_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_notation_audio_processor = 0;

    static const GTypeInfo ags_fx_notation_audio_processor_info = {
      sizeof (AgsFxNotationAudioProcessorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_notation_audio_processor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_processor */
      sizeof (AgsFxNotationAudioProcessor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_notation_audio_processor_init,
    };

    static const GInterfaceInfo ags_seekable_interface_info = {
      (GInterfaceInitFunc) ags_fx_notation_audio_processor_seekable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_countable_interface_info = {
      (GInterfaceInitFunc) ags_fx_notation_audio_processor_countable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_tactable_interface_info = {
      (GInterfaceInitFunc) ags_fx_notation_audio_processor_tactable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_fx_notation_audio_processor = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
								  "AgsFxNotationAudioProcessor",
								  &ags_fx_notation_audio_processor_info,
								  0);

    g_type_add_interface_static(ags_type_fx_notation_audio_processor,
				AGS_TYPE_COUNTABLE,
				&ags_countable_interface_info);

    g_type_add_interface_static(ags_type_fx_notation_audio_processor,
				AGS_TYPE_SEEKABLE,
				&ags_seekable_interface_info);

    g_type_add_interface_static(ags_type_fx_notation_audio_processor,
				AGS_TYPE_TACTABLE,
				&ags_tactable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_notation_audio_processor);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_notation_audio_processor_class_init(AgsFxNotationAudioProcessorClass *fx_notation_audio_processor)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  
  ags_fx_notation_audio_processor_parent_class = g_type_class_peek_parent(fx_notation_audio_processor);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_notation_audio_processor;

  gobject->dispose = ags_fx_notation_audio_processor_dispose;
  gobject->finalize = ags_fx_notation_audio_processor_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_notation_audio_processor;
  
  recall->run_init_pre = ags_fx_notation_audio_processor_run_init_pre;
  recall->run_inter = ags_fx_notation_audio_processor_run_inter;
  
  /* AgsFxNotationAudioProcessorClass */
  fx_notation_audio_processor->key_on = ags_fx_notation_audio_processor_real_key_on;
  fx_notation_audio_processor->key_off = ags_fx_notation_audio_processor_real_key_off;
  fx_notation_audio_processor->key_pressure = ags_fx_notation_audio_processor_real_key_pressure;

  fx_notation_audio_processor->play = ags_fx_notation_audio_processor_real_play;
  fx_notation_audio_processor->record = ags_fx_notation_audio_processor_real_record;
  fx_notation_audio_processor->feed = ags_fx_notation_audio_processor_real_feed;

  fx_notation_audio_processor->counter_change = ags_fx_notation_audio_processor_real_counter_change;
}

void
ags_fx_notation_audio_processor_seekable_interface_init(AgsSeekableInterface *seekable)
{
  seekable->seek = ags_fx_notation_audio_processor_seek;
}

void
ags_fx_notation_audio_processor_countable_interface_init(AgsCountableInterface *countable)
{
  countable->get_sequencer_counter = NULL;
  countable->get_notation_counter = ags_fx_notation_audio_processor_get_notation_counter;
  countable->get_wave_counter = NULL;
  countable->get_midi_counter = NULL;
}

void
ags_fx_notation_audio_processor_tactable_interface_init(AgsTactableInterface *tactable)
{
  tactable->get_bpm = ags_fx_notation_audio_processor_get_bpm;
  tactable->get_tact = ags_fx_notation_audio_processor_get_tact;

  tactable->get_sequencer_duration = NULL;
  tactable->get_notation_duration = NULL;
  tactable->get_wave_duration = NULL;
  tactable->get_midi_duration = NULL;

  tactable->change_sequencer_duration = NULL;
  tactable->change_notation_duration = NULL;
  tactable->change_wave_duration = NULL;
  tactable->change_midi_duration = NULL;

  tactable->change_bpm = ags_fx_notation_audio_processor_change_bpm;
  tactable->change_tact = ags_fx_notation_audio_processor_change_tact;
}

void
ags_fx_notation_audio_processor_init(AgsFxNotationAudioProcessor *fx_notation_audio_processor)
{
  AGS_RECALL(fx_notation_audio_processor)->name = "ags-fx-notation";
  AGS_RECALL(fx_notation_audio_processor)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_notation_audio_processor)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_notation_audio_processor)->xml_type = "ags-fx-notation-audio-processor";

  /* counter */
  fx_notation_audio_processor->delay_completion = 0.0;

  fx_notation_audio_processor->delay_counter = 0.0;
  fx_notation_audio_processor->offset_counter = 0;

  fx_notation_audio_processor->current_delay_counter = 0.0;
  fx_notation_audio_processor->current_offset_counter = 0;

  /* timestamp */
  fx_notation_audio_processor->timestamp = ags_timestamp_new();
  g_object_ref(fx_notation_audio_processor->timestamp);
  
  fx_notation_audio_processor->timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  fx_notation_audio_processor->timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  fx_notation_audio_processor->timestamp->timer.ags_offset.offset = 0;

  /* recording */
  fx_notation_audio_processor->recording_note = NULL;
  fx_notation_audio_processor->recording_audio_signal = NULL;

  /* feeding */
  fx_notation_audio_processor->feeding_note = NULL;
  fx_notation_audio_processor->feeding_audio_signal = NULL;
}

void
ags_fx_notation_audio_processor_dispose(GObject *gobject)
{
  AgsFxNotationAudioProcessor *fx_notation_audio_processor;
  
  fx_notation_audio_processor = AGS_FX_NOTATION_AUDIO_PROCESSOR(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_notation_audio_processor_parent_class)->dispose(gobject);
}

void
ags_fx_notation_audio_processor_finalize(GObject *gobject)
{
  AgsFxNotationAudioProcessor *fx_notation_audio_processor;
  
  fx_notation_audio_processor = AGS_FX_NOTATION_AUDIO_PROCESSOR(gobject);  

  /* timestamp */
  if(fx_notation_audio_processor->timestamp != NULL){
    g_object_unref((GObject *) fx_notation_audio_processor->timestamp);
  }

  /* recording - note */
  if(fx_notation_audio_processor->recording_note != NULL){
    g_list_free_full(fx_notation_audio_processor->recording_note,
		     (GDestroyNotify) g_object_unref);
  }

  /* recording - audio signal */
  if(fx_notation_audio_processor->recording_audio_signal != NULL){
    g_list_free_full(fx_notation_audio_processor->recording_audio_signal,
		     (GDestroyNotify) g_object_unref);
  }

  /* feeding - note */
  if(fx_notation_audio_processor->feeding_note != NULL){
    g_list_free_full(fx_notation_audio_processor->feeding_note,
		     (GDestroyNotify) g_object_unref);
  }

  /* feeding - audio signal */
  if(fx_notation_audio_processor->feeding_audio_signal != NULL){
    g_list_free_full(fx_notation_audio_processor->feeding_audio_signal,
		     (GDestroyNotify) g_object_unref);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_notation_audio_processor_parent_class)->finalize(gobject);
}

void
ags_fx_notation_audio_processor_seek(AgsSeekable *seekable,
				     gint64 offset,
				     guint whence)
{
  AgsFxNotationAudio *fx_notation_audio;
  AgsFxNotationAudioProcessor *fx_notation_audio_processor;
  AgsPort *port;

  gdouble notation_duration;
  
  GRecMutex *recall_mutex;

  gdouble delay;
  guint64 notation_counter;

  GValue value = {0,};

  fx_notation_audio = NULL;
  
  fx_notation_audio_processor = AGS_FX_NOTATION_AUDIO_PROCESSOR(seekable);
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio_processor);

  g_object_get(fx_notation_audio_processor,
	       "recall-audio", &fx_notation_audio,
	       NULL);

  /* delay */
  port = NULL;

  delay = AGS_SOUNDCARD_DEFAULT_DELAY;
  
  notation_duration = ceil(AGS_NOTATION_DEFAULT_DURATION * delay);
      
  g_object_get(fx_notation_audio,
	       "delay", &port,
	       NULL);

  if(port != NULL){
    g_value_init(&value, G_TYPE_DOUBLE);

    ags_port_safe_read(port, &value);
    
    delay = g_value_get_double(&value);

    g_value_unset(&value);
	
    g_object_unref(port);
  }

  /* duration */
  port = NULL;

  g_object_get(fx_notation_audio,
	       "duration", &port,
	       NULL);

  if(port != NULL){
    g_value_init(&value, G_TYPE_DOUBLE);

    ags_port_safe_read(port, &value);
    
    notation_duration = g_value_get_double(&value);

    g_value_unset(&value);
	
    g_object_unref(port);
  }
  
  switch(whence){
  case AGS_SEEK_CUR:
    {
      g_rec_mutex_lock(recall_mutex);

      notation_counter = fx_notation_audio_processor->offset_counter;

      if(notation_counter + offset < 0){
	notation_counter = (guint64) notation_duration - (guint64) ((offset - notation_counter) % (guint64) notation_duration);
      }else{
	notation_counter = (guint64) (notation_counter + offset) % (guint64) notation_duration;
      }

      fx_notation_audio_processor->delay_counter =
	fx_notation_audio_processor->current_delay_counter = 0.0;

      fx_notation_audio_processor->offset_counter = 
	fx_notation_audio_processor->current_offset_counter = notation_counter;
  
      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case AGS_SEEK_END:
    {      
      g_rec_mutex_lock(recall_mutex);

      notation_counter = fx_notation_audio_processor->offset_counter;

      /* notation */
      if(notation_duration + offset < 0){
	notation_counter = (guint64) notation_duration - ((guint64) (offset - notation_duration) % (guint64) notation_duration);
      }else{
	notation_counter = (guint64) (notation_duration + offset) % (guint64) notation_duration;
      }

      fx_notation_audio_processor->delay_counter =
	fx_notation_audio_processor->current_delay_counter = 0.0;

      fx_notation_audio_processor->offset_counter =
	fx_notation_audio_processor->current_offset_counter = notation_counter;
  
      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case AGS_SEEK_SET:
    {
      g_rec_mutex_lock(recall_mutex);

      fx_notation_audio_processor->delay_counter =
	fx_notation_audio_processor->current_delay_counter = 0.0;
      
      fx_notation_audio_processor->offset_counter =
	fx_notation_audio_processor->current_offset_counter = offset;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  }

  if(fx_notation_audio != NULL){
    g_object_unref(fx_notation_audio);
  }
}

guint64
ags_fx_notation_audio_processor_get_notation_counter(AgsCountable *countable)
{
  AgsFxNotationAudioProcessor *fx_notation_audio_processor;
  
  guint64 notation_counter;

  GRecMutex *recall_mutex;

  fx_notation_audio_processor = AGS_FX_NOTATION_AUDIO_PROCESSOR(countable);
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio_processor);

  /* bpm */
  g_rec_mutex_lock(recall_mutex);

  notation_counter = fx_notation_audio_processor->offset_counter;
  
  g_rec_mutex_unlock(recall_mutex);
  
  return(notation_counter);
}

gdouble
ags_fx_notation_audio_processor_get_bpm(AgsTactable *tactable)
{
  AgsFxNotationAudio *fx_notation_audio;
  AgsFxNotationAudioProcessor *fx_notation_audio_processor;
  AgsPort *port;
  
  gdouble bpm;
  
  GValue value = {0,};
  
  fx_notation_audio  = NULL;
  
  fx_notation_audio_processor = AGS_FX_NOTATION_AUDIO_PROCESSOR(tactable);

  port = NULL;
  
  bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  g_object_get(fx_notation_audio_processor,
	       "recall-audio", &fx_notation_audio,
	       NULL);
  
  if(fx_notation_audio != NULL){
    g_object_get(fx_notation_audio,
		 "bpm", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value, G_TYPE_DOUBLE);

      ags_port_safe_read(port, &value);

      bpm = g_value_get_double(&value);

      g_value_unset(&value);
      
      g_object_unref(port);
    }
  }
  
  if(fx_notation_audio != NULL){
    g_object_unref(fx_notation_audio);
  }
  
  return(bpm);
}

gdouble
ags_fx_notation_audio_processor_get_tact(AgsTactable *tactable)
{
  AgsFxNotationAudio *fx_notation_audio;
  AgsFxNotationAudioProcessor *fx_notation_audio_processor;
  AgsPort *port;
  
  gdouble tact;
  
  GValue value = {0,};
  
  fx_notation_audio  = NULL;
  
  fx_notation_audio_processor = AGS_FX_NOTATION_AUDIO_PROCESSOR(tactable);

  port = NULL;
  
  tact = AGS_SOUNDCARD_DEFAULT_TACT;

  g_object_get(fx_notation_audio_processor,
	       "recall-audio", &fx_notation_audio,
	       NULL);
  
  if(fx_notation_audio != NULL){
    g_object_get(fx_notation_audio,
		 "tact", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value, G_TYPE_DOUBLE);

      ags_port_safe_read(port, &value);

      tact = g_value_get_double(&value);

      g_value_unset(&value);
      
      g_object_unref(port);
    }
  }
  
  if(fx_notation_audio != NULL){
    g_object_unref(fx_notation_audio);
  }
  
  return(tact);
}

void
ags_fx_notation_audio_processor_change_bpm(AgsTactable *tactable, gdouble new_bpm, gdouble old_bpm)
{
  AgsFxNotationAudio *fx_notation_audio;
  AgsFxNotationAudioProcessor *fx_notation_audio_processor;
  AgsPort *port;

  GObject *output_soundcard;
    
  GValue value = {0,};

  output_soundcard = NULL;
  
  fx_notation_audio  = NULL;
  
  fx_notation_audio_processor = AGS_FX_NOTATION_AUDIO_PROCESSOR(tactable);

  port = NULL;
  
  g_object_get(fx_notation_audio_processor,
	       "output-soundcard", &output_soundcard,
	       "recall-audio", &fx_notation_audio,
	       NULL);

  /* delay */
  if(fx_notation_audio != NULL){
    port = NULL;
    
    g_object_get(fx_notation_audio,
		 "delay", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value, G_TYPE_DOUBLE);

      g_value_set_double(&value, ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(output_soundcard)));

      ags_port_safe_write(port, &value);

      g_value_unset(&value);

      g_object_unref(port);
    }
  }
  
  /* bpm */
  if(fx_notation_audio != NULL){
    port = NULL;
    
    g_object_get(fx_notation_audio,
		 "bpm", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value, G_TYPE_DOUBLE);

      g_value_set_double(&value, new_bpm);

      ags_port_safe_write(port, &value);

      g_value_unset(&value);
      
      g_object_unref(port);
    }
  }
  
  if(fx_notation_audio != NULL){
    g_object_unref(fx_notation_audio);
  }
}

void
ags_fx_notation_audio_processor_change_tact(AgsTactable *tactable, gdouble new_tact, gdouble old_tact)
{
  AgsFxNotationAudio *fx_notation_audio;
  AgsFxNotationAudioProcessor *fx_notation_audio_processor;
  AgsPort *port;
    
  GObject *output_soundcard;

  GValue value = {0,};

  output_soundcard = NULL;
  
  fx_notation_audio  = NULL;
  
  fx_notation_audio_processor = AGS_FX_NOTATION_AUDIO_PROCESSOR(tactable);

  port = NULL;
  
  g_object_get(fx_notation_audio_processor,
	       "output-soundcard", &output_soundcard,
	       "recall-audio", &fx_notation_audio,
	       NULL);
  /* delay */
  if(fx_notation_audio != NULL){
    port = NULL;
    
    g_object_get(fx_notation_audio,
		 "delay", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value, G_TYPE_DOUBLE);

      g_value_set_double(&value, ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(output_soundcard)));

      ags_port_safe_write(port, &value);

      g_value_unset(&value);

      g_object_unref(port);
    }
  }
  
  /* tact */
  if(fx_notation_audio != NULL){
    port = NULL;
    
    g_object_get(fx_notation_audio,
		 "tact", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value, G_TYPE_DOUBLE);

      g_value_set_double(&value, new_tact);

      ags_port_safe_write(port, &value);

      g_value_unset(&value);
      
      g_object_unref(port);
    }
  }
  
  if(fx_notation_audio != NULL){
    g_object_unref(fx_notation_audio);
  }
}

void
ags_fx_notation_audio_processor_run_init_pre(AgsRecall *recall)
{
  AgsFxNotationAudioProcessor *fx_notation_audio_processor;

  gdouble delay_counter;
  
  GRecMutex *fx_notation_audio_processor_mutex;

  fx_notation_audio_processor = AGS_FX_NOTATION_AUDIO_PROCESSOR(recall);
  
  fx_notation_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio_processor);

  /* get delay counter */
  g_rec_mutex_lock(fx_notation_audio_processor_mutex);
    
  fx_notation_audio_processor->delay_counter = 0;
  fx_notation_audio_processor->offset_counter = 0;

  fx_notation_audio_processor->current_delay_counter = 0;
  fx_notation_audio_processor->current_offset_counter = 0;

  g_rec_mutex_unlock(fx_notation_audio_processor_mutex);
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_notation_audio_processor_parent_class)->run_init_pre(recall);
}

void
ags_fx_notation_audio_processor_run_inter(AgsRecall *recall)
{
  AgsFxNotationAudioProcessor *fx_notation_audio_processor;
  AgsRecallID *recall_id;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;

  gint sound_scope;
  gdouble delay_counter;
  
  GRecMutex *fx_notation_audio_processor_mutex;

  fx_notation_audio_processor = AGS_FX_NOTATION_AUDIO_PROCESSOR(recall);
  
  fx_notation_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio_processor);

  recall_id = NULL;

  sound_scope = ags_recall_get_sound_scope(recall);

  g_object_get(recall,
	       "recall-id", &recall_id,
	       NULL);

  if(!ags_recall_id_check_sound_scope(recall_id, sound_scope)){
    if(recall_id != NULL){
      g_object_unref(recall_id);
    }

    AGS_RECALL_CLASS(ags_fx_notation_audio_processor_parent_class)->run_inter(recall);
    
    return;
  }

  recycling_context = NULL;
  parent_recycling_context = NULL;
  
  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);
  
  /* get delay counter */
  g_rec_mutex_lock(fx_notation_audio_processor_mutex);

  fx_notation_audio_processor->delay_counter = fx_notation_audio_processor->current_delay_counter;
  fx_notation_audio_processor->offset_counter = fx_notation_audio_processor->current_offset_counter;
  
  delay_counter = fx_notation_audio_processor->delay_counter;

  g_rec_mutex_unlock(fx_notation_audio_processor_mutex);

  /* run */
  if(ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_NOTATION)){
    if(parent_recycling_context == NULL &&
       delay_counter == 0.0){
      ags_fx_notation_audio_processor_play(fx_notation_audio_processor);
    }
  }

  if(ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_MIDI)){
    if(parent_recycling_context == NULL){
      ags_fx_notation_audio_processor_record(fx_notation_audio_processor);
    }
  }
  
  if(ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_PLAYBACK)){
    if(parent_recycling_context == NULL){
      ags_fx_notation_audio_processor_feed(fx_notation_audio_processor);
    }
  }
  
  /* counter change */
  ags_fx_notation_audio_processor_counter_change(fx_notation_audio_processor);

  if(recall_id != NULL){
    g_object_unref(recall_id);
  }

  if(recycling_context != NULL){
    g_object_unref(recycling_context);
  }

  if(parent_recycling_context != NULL){
    g_object_unref(parent_recycling_context);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_notation_audio_processor_parent_class)->run_inter(recall);
}

void
ags_fx_notation_audio_processor_real_key_on(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
					    AgsNote *note,
					    guint velocity,
					    guint key_mode)
{
  AgsAudio *audio;
  AgsChannel *start_input;
  AgsChannel *input, *selected_input;
  AgsRecallID *recall_id;
  AgsRecyclingContext *recycling_context;
  AgsFxNotationAudio *fx_notation_audio;
  AgsPort *port;

  gdouble delay;
  guint64 offset_counter;
  guint input_pads;
  guint audio_channel;
  guint y;

  GValue value = {0,};

  GRecMutex *fx_notation_audio_processor_mutex;

  fx_notation_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio_processor);

  audio = NULL;

  start_input = NULL;

  recall_id = NULL;

  fx_notation_audio = NULL;
  
  g_object_get(fx_notation_audio_processor,
	       "audio", &audio,
	       "recall-id", &recall_id,
	       "recall-audio", &fx_notation_audio,
	       "audio-channel", &audio_channel,
	       NULL);

  g_object_get(audio,
	       "input", &start_input,
	       NULL);
  
  recycling_context = NULL;
  
  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  g_rec_mutex_lock(fx_notation_audio_processor_mutex);
    
  offset_counter = fx_notation_audio_processor->offset_counter;

  g_rec_mutex_unlock(fx_notation_audio_processor_mutex);

  /* get delay */
  delay = AGS_SOUNDCARD_DEFAULT_DELAY;
  
  if(fx_notation_audio != NULL){
    port = NULL;
    
    g_object_get(fx_notation_audio,
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
  }

  input_pads = 0;
  
  g_object_get(audio,
	       "input-pads", &input_pads,
	       NULL);

  y = 0;
  
  g_object_get(note,
	       "y", &y,
	       NULL);
  
  input = ags_channel_nth(start_input,
			  audio_channel);

  if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
    selected_input = ags_channel_pad_nth(input,
					 input_pads - y - 1);
  }else{
    selected_input = ags_channel_pad_nth(input,
					 y);
  }
  
  if(selected_input != NULL){
    AgsRecycling *first_recycling, *last_recycling;
    AgsRecycling *recycling, *next_recycling;
    AgsRecycling *end_recycling;
    AgsRecallID *child_recall_id;
    
    GObject *output_soundcard;

    GList *start_list, *list;

    guint attack;
    
    output_soundcard = NULL;

    first_recycling = NULL;
    last_recycling = NULL;
    
    g_object_get(selected_input,
		 "output-soundcard", &output_soundcard,
		 "first-recycling", &first_recycling,
		 "last-recycling", &last_recycling,
		 NULL);

    attack = 0;

    //TODO:JK: uncomment me
#if 0
    if(output_soundcard != NULL){
      attack = ags_soundcard_get_attack(AGS_SOUNDCARD(output_soundcard));
    }
#endif
    
    end_recycling = ags_recycling_next(last_recycling);

    /* get child recall id */
    start_list = NULL;
    
    g_object_get(selected_input,
		 "recall-id", &start_list,
		 NULL);

    list = start_list;
    child_recall_id = NULL;

    while(child_recall_id == NULL &&
	  list != NULL){
      AgsRecyclingContext *current_recycling_context, *current_parent_recycling_context;

      g_object_get(list->data,
		   "recycling-context", &current_recycling_context,
		   NULL);

      g_object_get(current_recycling_context,
		   "parent", &current_parent_recycling_context,
		   NULL);
	  
      if(current_parent_recycling_context == recycling_context){
	child_recall_id = (AgsRecallID *) list->data;
	g_object_ref(child_recall_id);
      }

      if(current_recycling_context != NULL){
	g_object_unref(current_recycling_context);
      }

      if(current_parent_recycling_context != NULL){
	g_object_unref(current_parent_recycling_context);
      }
      
      /* iterate */
      list = list->next;
    }

    recycling = first_recycling;
    g_object_ref(recycling);
    
//    g_message(" - audio processor");

    while(recycling != end_recycling){
      AgsAudioSignal *template, *audio_signal;

      GRecMutex *recycling_mutex;

      recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

      g_rec_mutex_lock(recycling_mutex);

      template = ags_audio_signal_get_template(recycling->audio_signal);
      
      g_rec_mutex_unlock(recycling_mutex);
      
      /* create audio signal */
      audio_signal = ags_audio_signal_new((GObject *) output_soundcard,
					  (GObject *) recycling,
					  (GObject *) child_recall_id);
      ags_audio_signal_set_flags(audio_signal, AGS_AUDIO_SIGNAL_STREAM);
      g_object_set(audio_signal,
		   "template", template,
		   "note", note,
		   "attack", attack,
		   NULL);

      ags_audio_signal_stream_resize(audio_signal,
				     2);      

      audio_signal->stream_current = audio_signal->stream;

      ags_connectable_connect(AGS_CONNECTABLE(audio_signal));
      ags_recycling_add_audio_signal(recycling,
				     audio_signal);

//      g_message(" `- added");

      if(key_mode == AGS_FX_NOTATION_AUDIO_PROCESSOR_KEY_MODE_RECORD){
	g_object_ref(audio_signal);

	g_rec_mutex_lock(fx_notation_audio_processor_mutex);
	
	fx_notation_audio_processor->recording_audio_signal = g_list_prepend(fx_notation_audio_processor->recording_audio_signal,
									     audio_signal);

	g_rec_mutex_unlock(fx_notation_audio_processor_mutex);
      }else if(key_mode == AGS_FX_NOTATION_AUDIO_PROCESSOR_KEY_MODE_FEED){
	g_object_ref(audio_signal);

	g_rec_mutex_lock(fx_notation_audio_processor_mutex);
	
	fx_notation_audio_processor->feeding_audio_signal = g_list_prepend(fx_notation_audio_processor->feeding_audio_signal,
									   audio_signal);

	g_rec_mutex_unlock(fx_notation_audio_processor_mutex);
      }
      
      /* iterate */
      next_recycling = ags_recycling_next(recycling);

      g_object_unref(recycling);

      recycling = next_recycling;
    }

    if(output_soundcard != NULL){
      g_object_unref(output_soundcard);
    }

    if(first_recycling != NULL){
      g_object_unref(first_recycling);
    }

    if(last_recycling != NULL){
      g_object_unref(last_recycling);
    }

    if(recycling != NULL){
      g_object_unref(recycling);
    }
    
    if(end_recycling != NULL){
      g_object_unref(end_recycling);
    }

    g_list_free_full(start_list,
		     g_object_unref);

    if(child_recall_id != NULL){
      g_object_unref(child_recall_id);
    }
  }

  if(audio != NULL){
    g_object_unref(audio);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  if(input != NULL){
    g_object_unref(input);
  }

  if(selected_input != NULL){
    g_object_unref(selected_input);
  }

  if(recall_id != NULL){
    g_object_unref(recall_id);
  }
  
  if(fx_notation_audio != NULL){
    g_object_unref(fx_notation_audio);
  }
}

void
ags_fx_notation_audio_processor_key_on(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
				       AgsNote *note,
				       guint velocity,
				       guint key_mode)
{
  g_return_if_fail(AGS_IS_FX_NOTATION_AUDIO_PROCESSOR(fx_notation_audio_processor));

  g_object_ref(fx_notation_audio_processor);

  if(AGS_FX_NOTATION_AUDIO_PROCESSOR_GET_CLASS(fx_notation_audio_processor)->key_on != NULL){
    AGS_FX_NOTATION_AUDIO_PROCESSOR_GET_CLASS(fx_notation_audio_processor)->key_on(fx_notation_audio_processor,
										   note,
										   velocity,
										   key_mode);
  }
  
  g_object_unref(fx_notation_audio_processor);
}

void
ags_fx_notation_audio_processor_real_key_off(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
					     AgsNote *note,
					     guint velocity,
					     guint key_mode)
{
  //TODO:JK: implement me
}

void
ags_fx_notation_audio_processor_key_off(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
					AgsNote *note,
					guint velocity,
					guint key_mode)
{
  g_return_if_fail(AGS_IS_FX_NOTATION_AUDIO_PROCESSOR(fx_notation_audio_processor));

  g_object_ref(fx_notation_audio_processor);

  if(AGS_FX_NOTATION_AUDIO_PROCESSOR_GET_CLASS(fx_notation_audio_processor)->key_off != NULL){
    AGS_FX_NOTATION_AUDIO_PROCESSOR_GET_CLASS(fx_notation_audio_processor)->key_off(fx_notation_audio_processor,
										    note,
										    velocity,
										    key_mode);
  }
  
  g_object_unref(fx_notation_audio_processor);
}

void
ags_fx_notation_audio_processor_real_key_pressure(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
						  AgsNote *note,
						  guint velocity,
						  guint key_mode)
{
  //TODO:JK: implement me
}

void
ags_fx_notation_audio_processor_key_pressure(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
					     AgsNote *note,
					     guint velocity,
					     guint key_mode)
{
  g_return_if_fail(AGS_IS_FX_NOTATION_AUDIO_PROCESSOR(fx_notation_audio_processor));

  g_object_ref(fx_notation_audio_processor);

  if(AGS_FX_NOTATION_AUDIO_PROCESSOR_GET_CLASS(fx_notation_audio_processor)->key_pressure != NULL){
    AGS_FX_NOTATION_AUDIO_PROCESSOR_GET_CLASS(fx_notation_audio_processor)->key_pressure(fx_notation_audio_processor,
											 note,
											 velocity,
											 key_mode);
  }
  
  g_object_unref(fx_notation_audio_processor);
}

void
ags_fx_notation_audio_processor_real_play(AgsFxNotationAudioProcessor *fx_notation_audio_processor)
{
  AgsAudio *audio;

  AgsTimestamp *timestamp;
  
  GList *start_notation, *notation;

  guint64 offset_counter;
  guint audio_channel;
  
  GRecMutex *fx_notation_audio_processor_mutex;

  fx_notation_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio_processor);
  
  g_object_get(fx_notation_audio_processor,
	       "audio", &audio,
	       "audio-channel", &audio_channel,
	       NULL);

  if(audio == NULL){
    return;
  }
  
  g_object_get(audio,
	       "notation", &start_notation,
	       NULL);
  
  /* timestamp and offset counter */
  g_rec_mutex_lock(fx_notation_audio_processor_mutex);
    
  timestamp = fx_notation_audio_processor->timestamp;
  
  offset_counter = fx_notation_audio_processor->offset_counter;

  g_rec_mutex_unlock(fx_notation_audio_processor_mutex);

  ags_timestamp_set_ags_offset(timestamp,
			       AGS_NOTATION_DEFAULT_OFFSET * floor(offset_counter / AGS_NOTATION_DEFAULT_OFFSET));

  /* find near timestamp */
  notation = ags_notation_find_near_timestamp(start_notation, audio_channel,
					      timestamp);

  if(notation != NULL){
    GList *start_note, *note;
    
    start_note = ags_notation_find_offset(notation->data,
					  offset_counter,
					  FALSE);

    note = start_note;

    while(note != NULL){
      ags_fx_notation_audio_processor_key_on(fx_notation_audio_processor,
					     note->data,
					     AGS_FX_NOTATION_AUDIO_PROCESSOR_DEFAULT_KEY_ON_VELOCITY,
					     AGS_FX_NOTATION_AUDIO_PROCESSOR_KEY_MODE_PLAY);

      /* iterate */
      note = note->next;
    }

    g_list_free_full(start_note,
		     (GDestroyNotify) g_object_unref);
  }

  g_object_unref(audio);
  
  g_list_free_full(start_notation,
		   (GDestroyNotify) g_object_unref);  
}

void
ags_fx_notation_audio_processor_play(AgsFxNotationAudioProcessor *fx_notation_audio_processor)
{
  g_return_if_fail(AGS_IS_FX_NOTATION_AUDIO_PROCESSOR(fx_notation_audio_processor));

  g_object_ref(fx_notation_audio_processor);

  if(AGS_FX_NOTATION_AUDIO_PROCESSOR_GET_CLASS(fx_notation_audio_processor)->play != NULL){
    AGS_FX_NOTATION_AUDIO_PROCESSOR_GET_CLASS(fx_notation_audio_processor)->play(fx_notation_audio_processor);
  }
  
  g_object_unref(fx_notation_audio_processor);
}

void
ags_fx_notation_audio_processor_real_record(AgsFxNotationAudioProcessor *fx_notation_audio_processor)
{
  AgsAudio *audio;
  AgsPort *port;
  AgsNotation *current_notation;
  AgsFxNotationAudio *fx_notation_audio;
  
  AgsTimestamp *timestamp;
  
  GObject *input_sequencer;

  GList *start_notation, *notation;
  GList *start_note, *note;
  GList *start_recording_note, *recording_note;	

  guchar *midi_buffer;

  guint input_pads;
  guint audio_start_mapping;
  guint midi_start_mapping, midi_end_mapping;
  guint midi_channel;
  gdouble delay;
  guint64 offset_counter;
  guint audio_channel;
  guint buffer_length;
  gboolean reverse_mapping;
  gboolean pattern_mode;

  GValue value = {0,};
  
  GRecMutex *fx_notation_audio_processor_mutex;

  fx_notation_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio_processor);

  audio = NULL;

  fx_notation_audio = NULL;

  audio_channel = 0;
  
  g_object_get(fx_notation_audio_processor,
	       "audio", &audio,
	       "recall-audio", &fx_notation_audio,
	       "audio-channel", &audio_channel,
	       NULL);

  if(audio == NULL){
    if(fx_notation_audio != NULL){
      g_object_unref(fx_notation_audio);
    }

    return;
  }

  input_sequencer = NULL;

  input_pads = 0;

  audio_start_mapping = 0;
  
  midi_start_mapping = 0;
  midi_end_mapping = 0;

  midi_channel = 0;
  
  g_object_get(audio,
	       "input-sequencer", &input_sequencer,
	       "input-pads", &input_pads,
	       "audio-start-mapping", &audio_start_mapping,
	       "midi-start-mapping", &midi_start_mapping,
	       "midi-end-mapping", &midi_end_mapping,
	       "midi-channel", &midi_channel,
	       NULL);

  if(input_sequencer == NULL){
    if(audio != NULL){
      g_object_unref(audio);
    }
    
    if(fx_notation_audio != NULL){
      g_object_unref(fx_notation_audio);
    }
    
    return;
  }

  current_notation = NULL;
  
  start_notation = NULL;
  start_note = NULL;

  g_object_get(audio,
	       "notation", &start_notation,
	       NULL);

  /* get delay */
  delay = AGS_SOUNDCARD_DEFAULT_DELAY;

  if(fx_notation_audio != NULL){        
    g_object_get(fx_notation_audio,
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
  }

  /* timestamp and offset counter */
  g_rec_mutex_lock(fx_notation_audio_processor_mutex);
    
  timestamp = fx_notation_audio_processor->timestamp;
  
  offset_counter = fx_notation_audio_processor->offset_counter;

  g_rec_mutex_unlock(fx_notation_audio_processor_mutex);

  ags_timestamp_set_ags_offset(timestamp,
			       AGS_NOTATION_DEFAULT_OFFSET * floor(offset_counter / AGS_NOTATION_DEFAULT_OFFSET));

  /* test flags */
  reverse_mapping = ags_audio_test_behaviour_flags(audio,
						   AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING);
  
  pattern_mode = ags_audio_test_behaviour_flags(audio,
						AGS_SOUND_BEHAVIOUR_PATTERN_MODE);
  
  /* find near timestamp */
  notation = ags_notation_find_near_timestamp(start_notation, audio_channel,
					      timestamp);

  if(notation != NULL){
    current_notation = notation->data;
  }
 
  /* retrieve buffer */
  midi_buffer = ags_sequencer_get_buffer(AGS_SEQUENCER(input_sequencer),
					 &buffer_length);
  
  ags_sequencer_lock_buffer(AGS_SEQUENCER(input_sequencer),
			    midi_buffer);

  if(midi_buffer != NULL){
    guchar *midi_iter;
    
    /* parse bytes */
    midi_iter = midi_buffer;

    while(midi_iter < midi_buffer + buffer_length){
      if(ags_midi_util_is_key_on(midi_iter)){
	/* check midi channel */
	if(midi_channel == (0x0f & midi_iter[0])){
	  AgsNote *current_note;

	  gint y;

	  current_note = NULL;
	  y = -1;
	
	  /* check mapping */
	  if((0x7f & midi_iter[1]) >= midi_start_mapping &&
	     (0x7f & midi_iter[1]) <= midi_end_mapping){
	    /* check channel */
	    if(!reverse_mapping){
	      y = audio_start_mapping + ((0x7f & midi_iter[1]) - midi_start_mapping);
	    }else{
	      y = input_pads - (audio_start_mapping + ((0x7f & midi_iter[1]) - midi_start_mapping)) - 1;
	    }
	  }

	  if(y >= 0 &&
	     y < input_pads){
	    g_rec_mutex_lock(fx_notation_audio_processor_mutex);

	    start_recording_note = g_list_copy_deep(fx_notation_audio_processor->recording_note,
						    (GCopyFunc) g_object_ref,
						    NULL);
	  
	    g_rec_mutex_unlock(fx_notation_audio_processor_mutex);

	    recording_note = start_recording_note;

	    while(recording_note != NULL){
	      guint current_y;

	      current_y = 0;
	      
	      g_object_get(recording_note->data,
			   "y", &current_y,
			   NULL);
	    
	      if(current_y == y){
		current_note = recording_note->data;
	      
		break;
	      }
	    
	      /* iterate */
	      recording_note = recording_note->next;
	    }
	  
	    if(current_note == NULL){
	      current_note = ags_note_new();
	    
	      current_note->x[0] = offset_counter;
	      current_note->x[1] = offset_counter + 1;
	      
	      current_note->y = y;
		
	      if(!pattern_mode){
		fx_notation_audio_processor->recording_note = g_list_prepend(fx_notation_audio_processor->recording_note,
									     current_note);
		g_object_ref(current_note);

		ags_note_set_flags(current_note,
				   AGS_NOTE_FEED);
	      }

	      g_object_ref(current_note);
	      start_note = g_list_prepend(start_note,
					  current_note);
	    
	      /* check notation */
	      if(current_notation == NULL){
		current_notation = ags_notation_new((GObject *) audio,
						    audio_channel);

		ags_timestamp_set_ags_offset(current_notation->timestamp,
					     ags_timestamp_get_ags_offset(timestamp));

		ags_audio_add_notation(audio,
				       (GObject *) current_notation);
	      }

	      /* add note */
	      ags_notation_add_note(current_notation,
				    current_note,
				    FALSE);
	    }else{
	      if((0x7f & (midi_iter[2])) == 0){
		/* note-off */
		ags_note_unset_flags(current_note,
				     AGS_NOTE_FEED);

		fx_notation_audio_processor->recording_note = g_list_remove(fx_notation_audio_processor->recording_note,
									    current_note);
		g_object_unref(current_note);
	      }
	    }

	    g_list_free_full(start_recording_note,
			     (GDestroyNotify) g_object_unref);
	  }
	}
	
	midi_iter += 3;
      }else if(ags_midi_util_is_key_off(midi_iter)){
	/* check midi channel */
	if(midi_channel == (0x0f & midi_iter[0])){
	  AgsNote *current_note;

	  gint y;

	  current_note = NULL;
	  y = -1;
	
	  /* check mapping */
	  if((0x7f & midi_iter[1]) >= midi_start_mapping &&
	     (0x7f & midi_iter[1]) <= midi_end_mapping){
	    /* check channel */
	    if(!reverse_mapping){
	      y = audio_start_mapping + ((0x7f & midi_iter[1]) - midi_start_mapping);
	    }else{
	      y = input_pads - (audio_start_mapping + ((0x7f & midi_iter[1]) - midi_start_mapping)) - 1;
	    }
	  }

	  if(y >= 0 &&
	     y < input_pads){
	    g_rec_mutex_lock(fx_notation_audio_processor_mutex);

	    start_recording_note = g_list_copy_deep(fx_notation_audio_processor->recording_note,
						    (GCopyFunc) g_object_ref,
						    NULL);
	  
	    g_rec_mutex_unlock(fx_notation_audio_processor_mutex);

	    recording_note = start_recording_note;

	    while(recording_note != NULL){
	      guint current_y;

	      current_y = 0;
	      
	      g_object_get(recording_note->data,
			   "y", &current_y,
			   NULL);
	    
	      if(current_y == y){
		current_note = recording_note->data;
	      
		break;
	      }
	    
	      /* iterate */
	      recording_note = recording_note->next;
	    }
	  
	    if(current_note != NULL){
	      ags_note_unset_flags(current_note,
				   AGS_NOTE_FEED);
	      
	      fx_notation_audio_processor->recording_note = g_list_remove(fx_notation_audio_processor->recording_note,
									  current_note);
	      g_object_unref(current_note);
	    }

	    g_list_free_full(start_recording_note,
			     (GDestroyNotify) g_object_unref);
	  }
	}
      
	midi_iter += 3;
      }else if(ags_midi_util_is_key_pressure(midi_iter)){
	midi_iter += 3;
      }else if(ags_midi_util_is_change_parameter(midi_iter)){
	/* change parameter */
	//TODO:JK: implement me	  
	  
	midi_iter += 3;
      }else if(ags_midi_util_is_pitch_bend(midi_iter)){
	/* change parameter */
	//TODO:JK: implement me	  
	  
	midi_iter += 3;
      }else if(ags_midi_util_is_change_program(midi_iter)){
	/* change program */
	//TODO:JK: implement me	  
	  
	midi_iter += 2;
      }else if(ags_midi_util_is_change_pressure(midi_iter)){
	/* change pressure */
	//TODO:JK: implement me	  
	  
	midi_iter += 2;
      }else if(ags_midi_util_is_sysex(midi_iter)){
	guint n;
	  
	/* sysex */
	n = 0;
	  
	while(midi_iter[n] != 0xf7){
	  n++;
	}

	//TODO:JK: implement me	  
	  
	midi_iter += (n + 1);
      }else if(ags_midi_util_is_song_position(midi_iter)){
	/* song position */
	//TODO:JK: implement me	  
	  
	midi_iter += 3;
      }else if(ags_midi_util_is_song_select(midi_iter)){
	/* song select */
	//TODO:JK: implement me	  
	  
	midi_iter += 2;
      }else if(ags_midi_util_is_tune_request(midi_iter)){
	/* tune request */
	//TODO:JK: implement me	  
	  
	midi_iter += 1;
      }else if(ags_midi_util_is_meta_event(midi_iter)){
	/* meta event */
	//TODO:JK: implement me	  
	  
	midi_iter += (3 + midi_iter[2]);
      }else{
	g_warning("ags_fx_notation_audio_processor.c - unexpected byte %x", midi_iter[0]);
	  
	midi_iter++;
      }
    }
  }
  
  ags_sequencer_unlock_buffer(AGS_SEQUENCER(input_sequencer),
			      midi_buffer);

  /* key on */
  note = start_note;

  while(note != NULL){
    ags_fx_notation_audio_processor_key_on(fx_notation_audio_processor,
					   note->data,
					   AGS_FX_NOTATION_AUDIO_PROCESSOR_DEFAULT_KEY_ON_VELOCITY,
					   AGS_FX_NOTATION_AUDIO_PROCESSOR_KEY_MODE_RECORD);
    
    note = note->next;
  }

  /* update */
  g_rec_mutex_lock(fx_notation_audio_processor_mutex);

  start_recording_note = g_list_copy_deep(fx_notation_audio_processor->recording_note,
					  (GCopyFunc) g_object_ref,
					  NULL);
	  
  g_rec_mutex_unlock(fx_notation_audio_processor_mutex);
  
  recording_note = start_recording_note;

  while(recording_note != NULL){
    guint current_x1;

    current_x1 = 0;
    
    g_object_get(recording_note->data,
		 "x1", &current_x1,
		 NULL);
    
    if(current_x1 < offset_counter){
      g_object_set(recording_note->data,
		   "x1", current_x1 + 1,
		   NULL);
    }
	    
    /* iterate */
    recording_note = recording_note->next;
  }
  
  /* unref */
  g_list_free_full(start_recording_note,
		   (GDestroyNotify) g_object_unref);

  if(audio != NULL){
    g_object_unref(audio);  
  }
  
  if(fx_notation_audio != NULL){
    g_object_unref(fx_notation_audio);
  }

  if(input_sequencer != NULL){
    g_object_unref(input_sequencer);
  }
  
  g_list_free_full(start_notation,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(start_note,
		   (GDestroyNotify) g_object_unref);
}

void
ags_fx_notation_audio_processor_record(AgsFxNotationAudioProcessor *fx_notation_audio_processor)
{
  g_return_if_fail(AGS_IS_FX_NOTATION_AUDIO_PROCESSOR(fx_notation_audio_processor));

  g_object_ref(fx_notation_audio_processor);
  
  if(AGS_FX_NOTATION_AUDIO_PROCESSOR_GET_CLASS(fx_notation_audio_processor)->record != NULL){
    AGS_FX_NOTATION_AUDIO_PROCESSOR_GET_CLASS(fx_notation_audio_processor)->record(fx_notation_audio_processor);
  }
  
  g_object_unref(fx_notation_audio_processor);
}

void
ags_fx_notation_audio_processor_real_feed(AgsFxNotationAudioProcessor *fx_notation_audio_processor)
{
  AgsAudio *audio;
  AgsPort *port;
  AgsFxNotationAudio *fx_notation_audio;

  GList *start_feed_note, *feed_note;
  GList *start_feeding_note, *feeding_note;
  
  gdouble delay;
  guint64 offset_counter;

  GValue value = {0,};
  
  GRecMutex *fx_notation_audio_processor_mutex;

  fx_notation_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio_processor);

  audio = NULL;

  fx_notation_audio = NULL;
  
  g_object_get(fx_notation_audio_processor,
	       "audio", &audio,
	       "recall-audio", &fx_notation_audio,
	       NULL);

  g_rec_mutex_lock(fx_notation_audio_processor_mutex);

  offset_counter = fx_notation_audio_processor->offset_counter;

  g_rec_mutex_unlock(fx_notation_audio_processor_mutex);

  /* get delay */
  delay = AGS_SOUNDCARD_DEFAULT_DELAY;
  
  if(fx_notation_audio != NULL){
    port = NULL;
    
    g_object_get(fx_notation_audio,
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
  }

  /* get feed note */
  feed_note = 
    start_feed_note = ags_fx_notation_audio_get_feed_note(fx_notation_audio);

  /* check new */
  while(feed_note != NULL){
    gboolean is_new;
    
    g_rec_mutex_lock(fx_notation_audio_processor_mutex);

    is_new = (g_list_find(fx_notation_audio_processor->feeding_note, feed_note->data) == NULL) ? TRUE: FALSE;
    
    g_rec_mutex_unlock(fx_notation_audio_processor_mutex);

    if(is_new){
      ags_fx_notation_audio_processor_key_on(fx_notation_audio_processor,
					     feed_note->data,
					     AGS_FX_NOTATION_AUDIO_PROCESSOR_DEFAULT_KEY_ON_VELOCITY,
					     AGS_FX_NOTATION_AUDIO_PROCESSOR_KEY_MODE_FEED);
    }
    
    feed_note = feed_note->next;
  }

  /* check removed and update */
  g_rec_mutex_lock(fx_notation_audio_processor_mutex);

  feeding_note = 
    start_feeding_note = g_list_copy_deep(fx_notation_audio_processor->feeding_note,
					  (GCopyFunc) g_object_ref,
					  NULL);
  
  g_rec_mutex_unlock(fx_notation_audio_processor_mutex);
  
  while(feeding_note != NULL){
    gboolean is_removed;

    is_removed = (g_list_find(start_feed_note, feeding_note->data) == NULL) ? TRUE: FALSE;

    if(!is_removed){
      guint current_x1;

      current_x1 = 0;
      
      g_object_get(feeding_note->data,
		   "x1", &current_x1,
		   NULL);
    
      if(current_x1 < offset_counter){
	g_object_set(feeding_note->data,
		     "x1", current_x1 + 1,
		     NULL);
      }      
    }else{
      g_rec_mutex_lock(fx_notation_audio_processor_mutex);
      
      start_feeding_note = g_list_remove(fx_notation_audio_processor->feeding_note,
					 feeding_note->data); 
      
      g_rec_mutex_unlock(fx_notation_audio_processor_mutex);

      g_object_unref(feeding_note->data);
    }

    feeding_note = feeding_note->next;
  }  

  /* unref */
  if(audio != NULL){
    g_object_unref(audio);
  }

  if(fx_notation_audio != NULL){
    g_object_unref(fx_notation_audio);
  }

  g_list_free_full(start_feed_note,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(start_feeding_note,
		   (GDestroyNotify) g_object_unref);
}

void
ags_fx_notation_audio_processor_feed(AgsFxNotationAudioProcessor *fx_notation_audio_processor)
{
  g_return_if_fail(AGS_IS_FX_NOTATION_AUDIO_PROCESSOR(fx_notation_audio_processor));

  g_object_ref(fx_notation_audio_processor);

  if(AGS_FX_NOTATION_AUDIO_PROCESSOR_GET_CLASS(fx_notation_audio_processor)->feed != NULL){
    AGS_FX_NOTATION_AUDIO_PROCESSOR_GET_CLASS(fx_notation_audio_processor)->feed(fx_notation_audio_processor);
  }
  
  g_object_unref(fx_notation_audio_processor);
}

void
ags_fx_notation_audio_processor_real_counter_change(AgsFxNotationAudioProcessor *fx_notation_audio_processor)
{
  AgsFxNotationAudio *fx_notation_audio;

  GObject *output_soundcard;

  gdouble delay;
  guint delay_counter;
  guint offset_counter;
  gboolean loop;
  guint64 loop_start, loop_end;
  
  GValue value = {0,};

  GRecMutex *fx_notation_audio_processor_mutex;
  
  fx_notation_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio_processor);

  output_soundcard = NULL;

  fx_notation_audio = NULL;

  g_object_get(fx_notation_audio_processor,
	       "output-soundcard", &output_soundcard,
	       "recall-audio", &fx_notation_audio,
	       NULL);

  delay = AGS_SOUNDCARD_DEFAULT_DELAY;

  loop = FALSE;

  loop_start = AGS_FX_NOTATION_AUDIO_DEFAULT_LOOP_START;
  loop_end = AGS_FX_NOTATION_AUDIO_DEFAULT_LOOP_END;
  
  if(fx_notation_audio != NULL){
    AgsPort *port;

    /* delay */
    port = NULL;
    
    g_object_get(fx_notation_audio,
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
    port = NULL;

    g_object_get(fx_notation_audio,
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
    port = NULL;

    g_object_get(fx_notation_audio,
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
    port = NULL;

    g_object_get(fx_notation_audio,
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

  g_rec_mutex_lock(fx_notation_audio_processor_mutex);

  delay_counter = floor(fx_notation_audio_processor->delay_counter);

  offset_counter = fx_notation_audio_processor->offset_counter;
  
  g_rec_mutex_unlock(fx_notation_audio_processor_mutex);  

  if(output_soundcard != NULL){
    delay = ags_soundcard_get_delay(AGS_SOUNDCARD(output_soundcard));

    delay_counter = ags_soundcard_get_delay_counter(AGS_SOUNDCARD(output_soundcard));
  }

  if(delay_counter + 1.0 >= delay){
    g_rec_mutex_lock(fx_notation_audio_processor_mutex);
    
    fx_notation_audio_processor->current_delay_counter = 0.0;

    if(loop &&
       offset_counter + 1 >= loop_end){
      fx_notation_audio_processor->current_offset_counter = loop_start;
    }else{
      fx_notation_audio_processor->current_offset_counter += 1;
    }

    g_rec_mutex_unlock(fx_notation_audio_processor_mutex);
  }else{    
    g_rec_mutex_lock(fx_notation_audio_processor_mutex);
    
    fx_notation_audio_processor->current_delay_counter += 1.0;

    g_rec_mutex_unlock(fx_notation_audio_processor_mutex);
  }
  
  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  /* unref */
  if(fx_notation_audio != NULL){
    g_object_unref(fx_notation_audio);
  }
}

void
ags_fx_notation_audio_processor_counter_change(AgsFxNotationAudioProcessor *fx_notation_audio_processor)
{
  g_return_if_fail(AGS_IS_FX_NOTATION_AUDIO_PROCESSOR(fx_notation_audio_processor));

  g_object_ref(fx_notation_audio_processor);
  
  if(AGS_FX_NOTATION_AUDIO_PROCESSOR_GET_CLASS(fx_notation_audio_processor)->counter_change != NULL){
    AGS_FX_NOTATION_AUDIO_PROCESSOR_GET_CLASS(fx_notation_audio_processor)->counter_change(fx_notation_audio_processor);
  }
  
  g_object_unref(fx_notation_audio_processor);
}

/**
 * ags_fx_notation_audio_processor_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxNotationAudioProcessor
 *
 * Returns: the new #AgsFxNotationAudioProcessor
 *
 * Since: 3.3.0
 */
AgsFxNotationAudioProcessor*
ags_fx_notation_audio_processor_new(AgsAudio *audio)
{
  AgsFxNotationAudioProcessor *fx_notation_audio_processor;

  fx_notation_audio_processor = (AgsFxNotationAudioProcessor *) g_object_new(AGS_TYPE_FX_NOTATION_AUDIO_PROCESSOR,
									     "audio", audio,
									     NULL);

  return(fx_notation_audio_processor);
}
