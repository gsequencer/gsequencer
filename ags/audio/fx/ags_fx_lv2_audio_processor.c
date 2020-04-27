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

#include <ags/audio/fx/ags_fx_lv2_audio_processor.h>

#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_lv2_plugin.h>

#include <ags/audio/fx/ags_fx_lv2_audio.h>
#include <ags/audio/fx/ags_fx_lv2_audio_processor.h>

#include <ags/i18n.h>

void ags_fx_lv2_audio_processor_class_init(AgsFxLv2AudioProcessorClass *fx_lv2_audio_processor);
void ags_fx_lv2_audio_processor_init(AgsFxLv2AudioProcessor *fx_lv2_audio_processor);
void ags_fx_lv2_audio_processor_dispose(GObject *gobject);
void ags_fx_lv2_audio_processor_finalize(GObject *gobject);

void ags_fx_lv2_audio_processor_key_on(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
				       AgsNote *note,
				       guint velocity,
				       guint key_mode);

/**
 * SECTION:ags_fx_lv2_audio_processor
 * @short_description: fx lv2 audio processor
 * @title: AgsFxLv2AudioProcessor
 * @section_id:
 * @include: ags/audio/fx/ags_fx_lv2_audio_processor.h
 *
 * The #AgsFxLv2AudioProcessor class provides ports to the effect processor.
 */

static gpointer ags_fx_lv2_audio_processor_parent_class = NULL;

GType
ags_fx_lv2_audio_processor_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_lv2_audio_processor = 0;

    static const GTypeInfo ags_fx_lv2_audio_processor_info = {
      sizeof (AgsFxLv2AudioProcessorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_lv2_audio_processor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_processor */
      sizeof (AgsFxLv2AudioProcessor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_lv2_audio_processor_init,
    };

    ags_type_fx_lv2_audio_processor = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO_PROCESSOR,
							     "AgsFxLv2AudioProcessor",
							     &ags_fx_lv2_audio_processor_info,
							     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_lv2_audio_processor);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_lv2_audio_processor_class_init(AgsFxLv2AudioProcessorClass *fx_lv2_audio_processor)
{
  AgsFxNotationAudioProcessorClass *fx_notation_audio_processor;
  GObjectClass *gobject;
  
  ags_fx_lv2_audio_processor_parent_class = g_type_class_peek_parent(fx_lv2_audio_processor);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_lv2_audio_processor;

  gobject->dispose = ags_fx_lv2_audio_processor_dispose;
  gobject->finalize = ags_fx_lv2_audio_processor_finalize;

  /* AgsFxNotationAudioProcessorClass */
  fx_notation_audio_processor = (AgsFxNotationAudioProcessorClass *) fx_lv2_audio_processor;
  
  fx_notation_audio_processor->key_on = ags_fx_lv2_audio_processor_key_on;
}

void
ags_fx_lv2_audio_processor_init(AgsFxLv2AudioProcessor *fx_lv2_audio_processor)
{
  AGS_RECALL(fx_lv2_audio_processor)->name = "ags-fx-lv2";
  AGS_RECALL(fx_lv2_audio_processor)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_lv2_audio_processor)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_lv2_audio_processor)->xml_type = "ags-fx-lv2-audio-processor";
}

void
ags_fx_lv2_audio_processor_dispose(GObject *gobject)
{
  AgsFxLv2AudioProcessor *fx_lv2_audio_processor;
  
  fx_lv2_audio_processor = AGS_FX_LV2_AUDIO_PROCESSOR(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_lv2_audio_processor_parent_class)->dispose(gobject);
}

void
ags_fx_lv2_audio_processor_finalize(GObject *gobject)
{
  AgsFxLv2AudioProcessor *fx_lv2_audio_processor;
  
  fx_lv2_audio_processor = AGS_FX_LV2_AUDIO_PROCESSOR(gobject);  
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_lv2_audio_processor_parent_class)->finalize(gobject);
}

void
ags_fx_lv2_audio_processor_key_on(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
				  AgsNote *note,
				  guint velocity,
				  guint key_mode)
{
  AgsAudio *audio;
  AgsFxLv2Audio *fx_lv2_audio;

  guint sound_scope;
  guint audio_channel;
  guint audio_start_mapping;
  guint midi_start_mapping;
  gint midi_note;
  guint y;
  
  GRecMutex *fx_lv2_audio_mutex;

  audio = NULL;
  
  audio_start_mapping = 0;
  midi_start_mapping = 0;

  y = 0;

  sound_scope = ags_recall_get_sound_scope(fx_notation_audio_processor);

  audio_channel = 0;
  
  g_object_get(fx_notation_audio_processor,
	       "audio", &audio,
	       "recall-audio", &fx_lv2_audio,
	       "audio-channel", &audio_channel,
	       NULL);

  fx_lv2_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_lv2_audio);

  g_object_get(audio,
	       "audio-start-mapping", &audio_start_mapping,
	       "midi-start-mapping", &midi_start_mapping,
	       NULL);

  g_object_get(note,
	       "y", &y,
	       NULL);
  
  midi_note = (y - audio_start_mapping + midi_start_mapping);

  if(midi_note >= 0 &&
     midi_note < 128){
    if(ags_fx_lv2_audio_test_flags(fx_lv2_audio, AGS_FX_LV2_AUDIO_LIVE_INSTRUMENT)){
      AgsFxLv2AudioScopeData *scope_data;
      AgsFxLv2AudioChannelData *channel_data;

      g_rec_mutex_lock(fx_lv2_audio_mutex);

      scope_data = fx_lv2_audio->scope_data[sound_scope];

      channel_data = scope_data->channel_data[audio_channel];

      if(channel_data->event_port != NULL){
	ags_lv2_plugin_event_buffer_append_midi(channel_data->event_port,
						AGS_FX_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
						channel_data->input_data[midi_note]->event_buffer,
						1);
      }else if(channel_data->atom_port != NULL){
	ags_lv2_plugin_atom_sequence_append_midi(channel_data->atom_port,
						 AGS_FX_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
						 channel_data->input_data[midi_note]->event_buffer,
						 1);
      }
      
      g_rec_mutex_unlock(fx_lv2_audio_mutex);
    }else{
      AgsFxLv2AudioScopeData *scope_data;
      AgsFxLv2AudioChannelData *channel_data;
      AgsFxLv2AudioInputData *input_data;

      g_rec_mutex_lock(fx_lv2_audio_mutex);

      scope_data = fx_lv2_audio->scope_data[sound_scope];

      channel_data = scope_data->channel_data[audio_channel];

      input_data = channel_data->input_data[midi_note];

      if(input_data->event_port != NULL){
	ags_lv2_plugin_event_buffer_append_midi(input_data->event_port,
						AGS_FX_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
						input_data->event_buffer,
						1);
      }else if(input_data->atom_port != NULL){
	ags_lv2_plugin_atom_sequence_append_midi(input_data->atom_port,
						 AGS_FX_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
						 input_data->event_buffer,
						 1);
      }
      
      g_rec_mutex_unlock(fx_lv2_audio_mutex);
    }
  }
  
  /* call parent */
  AGS_FX_NOTATION_AUDIO_PROCESSOR_CLASS(ags_fx_lv2_audio_processor_parent_class)->key_on(fx_notation_audio_processor,
											 note,
											 velocity,
											 key_mode);
}

/**
 * ags_fx_lv2_audio_processor_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxLv2AudioProcessor
 *
 * Returns: the new #AgsFxLv2AudioProcessor
 *
 * Since: 3.3.0
 */
AgsFxLv2AudioProcessor*
ags_fx_lv2_audio_processor_new(AgsAudio *audio)
{
  AgsFxLv2AudioProcessor *fx_lv2_audio_processor;

  fx_lv2_audio_processor = (AgsFxLv2AudioProcessor *) g_object_new(AGS_TYPE_FX_LV2_AUDIO_PROCESSOR,
								   "audio", audio,
								   NULL);

  return(fx_lv2_audio_processor);
}
