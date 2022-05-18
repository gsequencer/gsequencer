/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/audio/ags_fx_factory.h>

#include <ags/plugin/ags_base_plugin.h>

#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2_plugin.h>

#include <ags/audio/fx/ags_fx_playback_audio.h>
#include <ags/audio/fx/ags_fx_playback_audio_processor.h>
#include <ags/audio/fx/ags_fx_playback_channel.h>
#include <ags/audio/fx/ags_fx_playback_channel_processor.h>
#include <ags/audio/fx/ags_fx_playback_recycling.h>
#include <ags/audio/fx/ags_fx_playback_audio_signal.h>

#include <ags/audio/fx/ags_fx_buffer_audio.h>
#include <ags/audio/fx/ags_fx_buffer_audio_processor.h>
#include <ags/audio/fx/ags_fx_buffer_channel.h>
#include <ags/audio/fx/ags_fx_buffer_channel_processor.h>
#include <ags/audio/fx/ags_fx_buffer_recycling.h>
#include <ags/audio/fx/ags_fx_buffer_audio_signal.h>

#include <ags/audio/fx/ags_fx_volume_audio.h>
#include <ags/audio/fx/ags_fx_volume_audio_processor.h>
#include <ags/audio/fx/ags_fx_volume_channel.h>
#include <ags/audio/fx/ags_fx_volume_channel_processor.h>
#include <ags/audio/fx/ags_fx_volume_recycling.h>
#include <ags/audio/fx/ags_fx_volume_audio_signal.h>

#include <ags/audio/fx/ags_fx_low_pass_audio.h>
#include <ags/audio/fx/ags_fx_low_pass_audio_processor.h>
#include <ags/audio/fx/ags_fx_low_pass_channel.h>
#include <ags/audio/fx/ags_fx_low_pass_channel_processor.h>
#include <ags/audio/fx/ags_fx_low_pass_recycling.h>
#include <ags/audio/fx/ags_fx_low_pass_audio_signal.h>

#include <ags/audio/fx/ags_fx_high_pass_audio.h>
#include <ags/audio/fx/ags_fx_high_pass_audio_processor.h>
#include <ags/audio/fx/ags_fx_high_pass_channel.h>
#include <ags/audio/fx/ags_fx_high_pass_channel_processor.h>
#include <ags/audio/fx/ags_fx_high_pass_recycling.h>
#include <ags/audio/fx/ags_fx_high_pass_audio_signal.h>

#include <ags/audio/fx/ags_fx_peak_audio.h>
#include <ags/audio/fx/ags_fx_peak_audio_processor.h>
#include <ags/audio/fx/ags_fx_peak_channel.h>
#include <ags/audio/fx/ags_fx_peak_channel_processor.h>
#include <ags/audio/fx/ags_fx_peak_recycling.h>
#include <ags/audio/fx/ags_fx_peak_audio_signal.h>

#include <ags/audio/fx/ags_fx_eq10_audio.h>
#include <ags/audio/fx/ags_fx_eq10_audio_processor.h>
#include <ags/audio/fx/ags_fx_eq10_channel.h>
#include <ags/audio/fx/ags_fx_eq10_channel_processor.h>
#include <ags/audio/fx/ags_fx_eq10_recycling.h>
#include <ags/audio/fx/ags_fx_eq10_audio_signal.h>

#include <ags/audio/fx/ags_fx_analyse_audio.h>
#include <ags/audio/fx/ags_fx_analyse_audio_processor.h>
#include <ags/audio/fx/ags_fx_analyse_channel.h>
#include <ags/audio/fx/ags_fx_analyse_channel_processor.h>
#include <ags/audio/fx/ags_fx_analyse_recycling.h>
#include <ags/audio/fx/ags_fx_analyse_audio_signal.h>

#include <ags/audio/fx/ags_fx_two_pass_aliase_audio.h>
#include <ags/audio/fx/ags_fx_two_pass_aliase_audio_processor.h>
#include <ags/audio/fx/ags_fx_two_pass_aliase_channel.h>
#include <ags/audio/fx/ags_fx_two_pass_aliase_channel_processor.h>
#include <ags/audio/fx/ags_fx_two_pass_aliase_recycling.h>
#include <ags/audio/fx/ags_fx_two_pass_aliase_audio_signal.h>

#include <ags/audio/fx/ags_fx_lfo_audio.h>
#include <ags/audio/fx/ags_fx_lfo_audio_processor.h>
#include <ags/audio/fx/ags_fx_lfo_channel.h>
#include <ags/audio/fx/ags_fx_lfo_channel_processor.h>
#include <ags/audio/fx/ags_fx_lfo_recycling.h>
#include <ags/audio/fx/ags_fx_lfo_audio_signal.h>

#include <ags/audio/fx/ags_fx_envelope_audio.h>
#include <ags/audio/fx/ags_fx_envelope_audio_processor.h>
#include <ags/audio/fx/ags_fx_envelope_channel.h>
#include <ags/audio/fx/ags_fx_envelope_channel_processor.h>
#include <ags/audio/fx/ags_fx_envelope_recycling.h>
#include <ags/audio/fx/ags_fx_envelope_audio_signal.h>

#include <ags/audio/fx/ags_fx_pattern_audio.h>
#include <ags/audio/fx/ags_fx_pattern_audio_processor.h>
#include <ags/audio/fx/ags_fx_pattern_channel.h>
#include <ags/audio/fx/ags_fx_pattern_channel_processor.h>
#include <ags/audio/fx/ags_fx_pattern_recycling.h>
#include <ags/audio/fx/ags_fx_pattern_audio_signal.h>

#include <ags/audio/fx/ags_fx_notation_audio.h>
#include <ags/audio/fx/ags_fx_notation_audio_processor.h>
#include <ags/audio/fx/ags_fx_notation_channel.h>
#include <ags/audio/fx/ags_fx_notation_channel_processor.h>
#include <ags/audio/fx/ags_fx_notation_recycling.h>
#include <ags/audio/fx/ags_fx_notation_audio_signal.h>

#include <ags/audio/fx/ags_fx_synth_audio.h>
#include <ags/audio/fx/ags_fx_synth_audio_processor.h>
#include <ags/audio/fx/ags_fx_synth_channel.h>
#include <ags/audio/fx/ags_fx_synth_channel_processor.h>
#include <ags/audio/fx/ags_fx_synth_recycling.h>
#include <ags/audio/fx/ags_fx_synth_audio_signal.h>

#include <ags/audio/fx/ags_fx_fm_synth_audio.h>
#include <ags/audio/fx/ags_fx_fm_synth_audio_processor.h>
#include <ags/audio/fx/ags_fx_fm_synth_channel.h>
#include <ags/audio/fx/ags_fx_fm_synth_channel_processor.h>
#include <ags/audio/fx/ags_fx_fm_synth_recycling.h>
#include <ags/audio/fx/ags_fx_fm_synth_audio_signal.h>

#include <ags/audio/fx/ags_fx_sf2_synth_audio.h>
#include <ags/audio/fx/ags_fx_sf2_synth_audio_processor.h>
#include <ags/audio/fx/ags_fx_sf2_synth_channel.h>
#include <ags/audio/fx/ags_fx_sf2_synth_channel_processor.h>
#include <ags/audio/fx/ags_fx_sf2_synth_recycling.h>
#include <ags/audio/fx/ags_fx_sf2_synth_audio_signal.h>

#include <ags/audio/fx/ags_fx_sfz_synth_audio.h>
#include <ags/audio/fx/ags_fx_sfz_synth_audio_processor.h>
#include <ags/audio/fx/ags_fx_sfz_synth_channel.h>
#include <ags/audio/fx/ags_fx_sfz_synth_channel_processor.h>
#include <ags/audio/fx/ags_fx_sfz_synth_recycling.h>
#include <ags/audio/fx/ags_fx_sfz_synth_audio_signal.h>

#include <ags/audio/fx/ags_fx_ladspa_audio.h>
#include <ags/audio/fx/ags_fx_ladspa_audio_processor.h>
#include <ags/audio/fx/ags_fx_ladspa_channel.h>
#include <ags/audio/fx/ags_fx_ladspa_channel_processor.h>
#include <ags/audio/fx/ags_fx_ladspa_recycling.h>
#include <ags/audio/fx/ags_fx_ladspa_audio_signal.h>

#include <ags/audio/fx/ags_fx_dssi_audio.h>
#include <ags/audio/fx/ags_fx_dssi_audio_processor.h>
#include <ags/audio/fx/ags_fx_dssi_channel.h>
#include <ags/audio/fx/ags_fx_dssi_channel_processor.h>
#include <ags/audio/fx/ags_fx_dssi_recycling.h>
#include <ags/audio/fx/ags_fx_dssi_audio_signal.h>

#include <ags/audio/fx/ags_fx_lv2_audio.h>
#include <ags/audio/fx/ags_fx_lv2_audio_processor.h>
#include <ags/audio/fx/ags_fx_lv2_channel.h>
#include <ags/audio/fx/ags_fx_lv2_channel_processor.h>
#include <ags/audio/fx/ags_fx_lv2_recycling.h>
#include <ags/audio/fx/ags_fx_lv2_audio_signal.h>

#include <ags/config.h>

#if defined(AGS_WITH_VST3)
#include <ags/audio/fx/ags_fx_vst3_audio.h>
#include <ags/audio/fx/ags_fx_vst3_audio_processor.h>
#include <ags/audio/fx/ags_fx_vst3_channel.h>
#include <ags/audio/fx/ags_fx_vst3_channel_processor.h>
#include <ags/audio/fx/ags_fx_vst3_recycling.h>
#include <ags/audio/fx/ags_fx_vst3_audio_signal.h>

#include <ags/plugin/ags_vst3_manager.h>
#include <ags/plugin/ags_vst3_plugin.h>
#endif

/**
 * SECTION:ags_fx_factory
 * @short_description: Factory pattern
 * @title: AgsFxFactory
 * @section_id:
 * @include: ags/audio/ags_fx_factory.h
 *
 * Factory function to instantiate fx recalls.
 */

gint ags_fx_factory_get_output_position(AgsAudio *audio,
					gint position);
gint ags_fx_factory_get_input_position(AgsAudio *audio,
				       gint position);

/* declaration */
GList* ags_fx_factory_create_playback(AgsAudio *audio,
				      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				      gchar *plugin_name,
				      gchar *filename,
				      gchar *effect,
				      guint start_audio_channel, guint stop_audio_channel,
				      guint start_pad, guint stop_pad,
				      gint position,
				      guint create_flags, guint recall_flags);

GList* ags_fx_factory_create_buffer(AgsAudio *audio,
				    AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				    gchar *plugin_name,
				    gchar *filename,
				    gchar *effect,
				    guint start_audio_channel, guint stop_audio_channel,
				    guint start_pad, guint stop_pad,
				    gint position,
				    guint create_flags, guint recall_flags);

GList* ags_fx_factory_create_volume(AgsAudio *audio,
				    AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				    gchar *plugin_name,
				    gchar *filename,
				    gchar *effect,
				    guint start_audio_channel, guint stop_audio_channel,
				    guint start_pad, guint stop_pad,
				    gint position,
				    guint create_flags, guint recall_flags);

GList* ags_fx_factory_create_low_pass(AgsAudio *audio,
				      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				      gchar *plugin_name,
				      gchar *filename,
				      gchar *effect,
				      guint start_audio_channel, guint stop_audio_channel,
				      guint start_pad, guint stop_pad,
				      gint position,
				      guint create_flags, guint recall_flags);

GList* ags_fx_factory_create_high_pass(AgsAudio *audio,
				       AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				       gchar *plugin_name,
				       gchar *filename,
				       gchar *effect,
				       guint start_audio_channel, guint stop_audio_channel,
				       guint start_pad, guint stop_pad,
				       gint position,
				       guint create_flags, guint recall_flags);

GList* ags_fx_factory_create_peak(AgsAudio *audio,
				  AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				  gchar *plugin_name,
				  gchar *filename,
				  gchar *effect,
				  guint start_audio_channel, guint stop_audio_channel,
				  guint start_pad, guint stop_pad,
				  gint position,
				  guint create_flags, guint recall_flags);

GList* ags_fx_factory_create_eq10(AgsAudio *audio,
				  AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				  gchar *plugin_name,
				  gchar *filename,
				  gchar *effect,
				  guint start_audio_channel, guint stop_audio_channel,
				  guint start_pad, guint stop_pad,
				  gint position,
				  guint create_flags, guint recall_flags);

GList* ags_fx_factory_create_analyse(AgsAudio *audio,
				     AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				     gchar *plugin_name,
				     gchar *filename,
				     gchar *effect,
				     guint start_audio_channel, guint stop_audio_channel,
				     guint start_pad, guint stop_pad,
				     gint position,
				     guint create_flags, guint recall_flags);

GList* ags_fx_factory_create_two_pass_aliase(AgsAudio *audio,
					     AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
					     gchar *plugin_name,
					     gchar *filename,
					     gchar *effect,
					     guint start_audio_channel, guint stop_audio_channel,
					     guint start_pad, guint stop_pad,
					     gint position,
					     guint create_flags, guint recall_flags);

GList* ags_fx_factory_create_lfo(AgsAudio *audio,
				 AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				 gchar *plugin_name,
				 gchar *filename,
				 gchar *effect,
				 guint start_audio_channel, guint stop_audio_channel,
				 guint start_pad, guint stop_pad,
				 gint position,
				 guint create_flags, guint recall_flags);

GList* ags_fx_factory_create_envelope(AgsAudio *audio,
				      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				      gchar *plugin_name,
				      gchar *filename,
				      gchar *effect,
				      guint start_audio_channel, guint stop_audio_channel,
				      guint start_pad, guint stop_pad,
				      gint position,
				      guint create_flags, guint recall_flags);

GList* ags_fx_factory_create_pattern(AgsAudio *audio,
				     AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				     gchar *plugin_name,
				     gchar *filename,
				     gchar *effect,
				     guint start_audio_channel, guint stop_audio_channel,
				     guint start_pad, guint stop_pad,
				     gint position,
				     guint create_flags, guint recall_flags);

GList* ags_fx_factory_create_notation(AgsAudio *audio,
				      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				      gchar *plugin_name,
				      gchar *filename,
				      gchar *effect,
				      guint start_audio_channel, guint stop_audio_channel,
				      guint start_pad, guint stop_pad,
				      gint position,
				      guint create_flags, guint recall_flags);

GList* ags_fx_factory_create_synth(AgsAudio *audio,
				   AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				   gchar *plugin_name,
				   gchar *filename,
				   gchar *effect,
				   guint start_audio_channel, guint stop_audio_channel,
				   guint start_pad, guint stop_pad,
				   gint position,
				   guint create_flags, guint recall_flags);

GList* ags_fx_factory_create_sf2_synth(AgsAudio *audio,
				       AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				       gchar *plugin_name,
				       gchar *filename,
				       gchar *effect,
				       guint start_audio_channel, guint stop_audio_channel,
				       guint start_pad, guint stop_pad,
				       gint position,
				       guint create_flags, guint recall_flags);

GList* ags_fx_factory_create_sfz_synth(AgsAudio *audio,
				       AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				       gchar *plugin_name,
				       gchar *filename,
				       gchar *effect,
				       guint start_audio_channel, guint stop_audio_channel,
				       guint start_pad, guint stop_pad,
				       gint position,
				       guint create_flags, guint recall_flags);

GList* ags_fx_factory_create_ladspa(AgsAudio *audio,
				    AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				    gchar *plugin_name,
				    gchar *filename,
				    gchar *effect,
				    guint start_audio_channel, guint stop_audio_channel,
				    guint start_pad, guint stop_pad,
				    gint position,
				    guint create_flags, guint recall_flags);

GList* ags_fx_factory_create_dssi(AgsAudio *audio,
				  AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				  gchar *plugin_name,
				  gchar *filename,
				  gchar *effect,
				  guint start_audio_channel, guint stop_audio_channel,
				  guint start_pad, guint stop_pad,
				  gint position,
				  guint create_flags, guint recall_flags);

GList* ags_fx_factory_create_lv2(AgsAudio *audio,
				 AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				 gchar *plugin_name,
				 gchar *filename,
				 gchar *effect,
				 guint start_audio_channel, guint stop_audio_channel,
				 guint start_pad, guint stop_pad,
				 gint position,
				 guint create_flags, guint recall_flags);

#if defined(AGS_WITH_VST3)
GList* ags_fx_factory_create_vst3(AgsAudio *audio,
				  AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				  gchar *plugin_name,
				  gchar *filename,
				  gchar *effect,
				  guint start_audio_channel, guint stop_audio_channel,
				  guint start_pad, guint stop_pad,
				  gint position,
				  guint create_flags, guint recall_flags);
#endif

GType
ags_fx_factory_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_factory = 0;

#if 0
    //TODO:JK: implement me
    ags_type_fx_factory =
      g_boxed_type_register_static("AgsFxFactory",
				   (GBoxedCopyFunc) ags_fx_factory_copy,
				   (GBoxedFreeFunc) ags_fx_factory_free);
#endif
    
    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_factory);
  }

  return g_define_type_id__volatile;
}

GType
ags_fx_factory_create_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_FX_FACTORY_OUTPUT, "AGS_FX_FACTORY_OUTPUT", "fx-factory-output" },
      { AGS_FX_FACTORY_INPUT, "AGS_FX_FACTORY_INPUT", "fx-factory-input" },
      { AGS_FX_FACTORY_REMAP, "AGS_FX_FACTORY_REMAP", "fx-factory-remap" },
      { AGS_FX_FACTORY_ADD, "AGS_FX_FACTORY_ADD", "fx-factory-add" },
      { AGS_FX_FACTORY_PLAY, "AGS_FX_FACTORY_PLAY", "fx-factory-play" },
      { AGS_FX_FACTORY_RECALL, "AGS_FX_FACTORY_RECALL", "fx-factory-recall" },
      { AGS_FX_FACTORY_BULK, "AGS_FX_FACTORY_BULK", "fx-factory-bulk" },
      { AGS_FX_FACTORY_LIVE, "AGS_FX_FACTORY_LIVE", "fx-factory-live" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsFxFactoryCreateFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

gint
ags_fx_factory_get_output_position(AgsAudio *audio,
				   gint position)
{
  GList *start_recall, *recall;
  
  gint output_position;
  guint i;

  if(position == -1){
    return(-1);
  }
  
  recall = 
    start_recall = ags_audio_get_play(audio);
  
  output_position = 0;

  for(i = 0; i < position && recall != NULL; i++){
    if(ags_recall_test_behaviour_flags(recall->data, AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT)){
      output_position++;
    }
    
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
  
  return(output_position);
}

gint
ags_fx_factory_get_input_position(AgsAudio *audio,
				  gint position)
{
  GList *start_recall, *recall;
  
  gint input_position;
  guint i;

  if(position == -1){
    return(-1);
  }

  recall = 
    start_recall = ags_audio_get_play(audio);
  
  input_position = 0;

  for(i = 0; i < position && recall != NULL; i++){
    if(!ags_recall_test_behaviour_flags(recall->data, AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT)){
      input_position++;
    }
    
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
  
  return(input_position);
}

/* implementation */
GList*
ags_fx_factory_create_playback(AgsAudio *audio,
			       AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			       gchar *plugin_name,
			       gchar *filename,
			       gchar *effect,
			       guint start_audio_channel, guint stop_audio_channel,
			       guint start_pad, guint stop_pad,
			       gint position,
			       guint create_flags, guint recall_flags)
{
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;

  AgsFxPlaybackAudio *fx_playback_audio;
  AgsFxPlaybackAudioProcessor *fx_playback_audio_processor;
  AgsFxPlaybackChannel *fx_playback_channel;
  AgsFxPlaybackChannelProcessor *fx_playback_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  gint channel_position;
  guint audio_channels;
  guint i, j;
  
  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-playback recall container not provided");
    
    return(NULL);
  }
    
  start_recall = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);

  channel_position = 0;
  
  if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
    channel_position = ags_fx_factory_get_output_position(audio,
							  position);
  }else{
    channel_position = ags_fx_factory_get_input_position(audio,
							 position);
  }
  
  /* audio - play context */
  fx_playback_audio = NULL;
  fx_playback_audio_processor = NULL;

  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxPlaybackAudio */
    fx_playback_audio = (AgsFxPlaybackAudio *) g_object_new(AGS_TYPE_FX_PLAYBACK_AUDIO,
							    "output-soundcard", output_soundcard,
							    "audio", audio,
							    "recall-container", play_container,
							    NULL);
    ags_recall_set_flags((AgsRecall *) fx_playback_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_playback_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_MIDI |
				  AGS_SOUND_ABILITY_WAVE));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_playback_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_playback_audio,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_playback_audio);
    
    start_recall = g_list_prepend(start_recall,
				  fx_playback_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_playback_audio));

    /* AgsFxPlaybackAudioProcessor */
    fx_playback_audio_processor = (AgsFxPlaybackAudioProcessor *) g_object_new(AGS_TYPE_FX_PLAYBACK_AUDIO_PROCESSOR,
									       "output-soundcard", output_soundcard,
									       "audio", audio,
									       "recall-audio", fx_playback_audio,
									       "recall-container", play_container,
									       NULL);
    ags_recall_set_flags((AgsRecall *) fx_playback_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_playback_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_MIDI |
				  AGS_SOUND_ABILITY_WAVE));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_playback_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_playback_audio_processor,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_playback_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_playback_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_playback_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_playback_audio = ags_recall_container_get_recall_audio(play_container);

    if(fx_playback_audio != NULL){
      g_object_ref(fx_playback_audio);
    }
    
    start_recall_audio_run = ags_recall_container_get_recall_audio_run(play_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_playback_audio_processor = recall_audio_run->data;
      g_object_ref(fx_playback_audio_processor);
    }
  }
  
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* add recall container */
      ags_channel_add_recall_container(channel,
				       (GObject *) play_container);
	
      /* AgsFxPlaybackChannel */
      fx_playback_channel = (AgsFxPlaybackChannel *) g_object_new(AGS_TYPE_FX_PLAYBACK_CHANNEL,
								  "output-soundcard", output_soundcard,
								  "source", channel,
								  "recall-audio", fx_playback_audio,
								  "recall-container", play_container,
								  NULL);
      ags_recall_set_flags((AgsRecall *) fx_playback_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_playback_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_MIDI |
				    AGS_SOUND_ABILITY_WAVE));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_playback_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_playback_channel,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_playback_channel);
      
      start_recall = g_list_prepend(start_recall,
				    fx_playback_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_playback_channel));

      /* AgsFxPlaybackChannelProcessor */
      fx_playback_channel_processor = (AgsFxPlaybackChannelProcessor *) g_object_new(AGS_TYPE_FX_PLAYBACK_CHANNEL_PROCESSOR,
										     "output-soundcard", output_soundcard,
										     "source", channel,
										     "recall-audio", fx_playback_audio,
										     "recall-audio-run", fx_playback_audio_processor,
										     "recall-channel", fx_playback_channel,
										     "recall-container", play_container,
										     NULL);
      ags_recall_set_flags((AgsRecall *) fx_playback_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_playback_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_MIDI |
				    AGS_SOUND_ABILITY_WAVE));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_playback_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_playback_channel_processor,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_playback_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_playback_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_playback_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_playback_audio != NULL){
      g_object_unref(fx_playback_audio);
    }
    
    if(fx_playback_audio_processor != NULL){
      g_object_unref(fx_playback_audio_processor);
    }
  }
  
  /* audio - recall context */
  fx_playback_audio = NULL;
  fx_playback_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxPlaybackAudio */
    fx_playback_audio = (AgsFxPlaybackAudio *) g_object_new(AGS_TYPE_FX_PLAYBACK_AUDIO,
							    "output-soundcard", output_soundcard,
							    "audio", audio,
							    "recall-container", recall_container,
							    NULL);
    ags_recall_set_flags((AgsRecall *) fx_playback_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_playback_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_playback_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_playback_audio,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_playback_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_playback_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_playback_audio));
    
    /* AgsFxPlaybackAudioProcessor */
    fx_playback_audio_processor = (AgsFxPlaybackAudioProcessor *) g_object_new(AGS_TYPE_FX_PLAYBACK_AUDIO_PROCESSOR,
									       "output-soundcard", output_soundcard,
									       "audio", audio,
									       "recall-audio", fx_playback_audio,
									       "recall-container", recall_container,
									       NULL);
    ags_recall_set_flags((AgsRecall *) fx_playback_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_playback_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_playback_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_playback_audio_processor,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_playback_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_playback_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_playback_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_playback_audio = ags_recall_container_get_recall_audio(recall_container);

    if(fx_playback_audio != NULL){
      g_object_ref(fx_playback_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(recall_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_playback_audio_processor = recall_audio_run->data;
      g_object_ref(fx_playback_audio_processor);
    }
  }
  
  /* channel - recall context */  
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxPlaybackChannel */
      fx_playback_channel = (AgsFxPlaybackChannel *) g_object_new(AGS_TYPE_FX_PLAYBACK_CHANNEL,
								  "output-soundcard", output_soundcard,
								  "source", channel,
								  "recall-audio", fx_playback_audio,
								  "recall-container", recall_container,
								  NULL);
      ags_recall_set_flags((AgsRecall *) fx_playback_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_playback_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_playback_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_playback_channel,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_playback_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_playback_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_playback_channel));

      /* AgsFxPlaybackChannelProcessor */
      fx_playback_channel_processor = (AgsFxPlaybackChannelProcessor *) g_object_new(AGS_TYPE_FX_PLAYBACK_CHANNEL_PROCESSOR,
										     "output-soundcard", output_soundcard,
										     "source", channel,
										     "recall-audio", fx_playback_audio,
										     "recall-audio-run", fx_playback_audio_processor,
										     "recall-channel", fx_playback_channel,
										     "recall-container", recall_container,
										     NULL);
      ags_recall_set_flags((AgsRecall *) fx_playback_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_playback_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_playback_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_playback_channel_processor,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_playback_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_playback_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_playback_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_playback_audio != NULL){
      g_object_unref(fx_playback_audio);
    }
    
    if(fx_playback_audio_processor != NULL){
      g_object_unref(fx_playback_audio_processor);
    }
  }
  
  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(input_sequencer != NULL){
    g_object_unref(input_sequencer);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(start_recall);
}

GList*
ags_fx_factory_create_buffer(AgsAudio *audio,
			     AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			     gchar *plugin_name,
			     gchar *filename,
			     gchar *effect,
			     guint start_audio_channel, guint stop_audio_channel,
			     guint start_pad, guint stop_pad,
			     gint position,
			     guint create_flags, guint recall_flags)
{
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;

  AgsFxBufferAudio *fx_buffer_audio;
  AgsFxBufferAudioProcessor *fx_buffer_audio_processor;
  AgsFxBufferChannel *fx_buffer_channel;
  AgsFxBufferChannelProcessor *fx_buffer_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  gint channel_position;
  guint audio_channels;
  guint i, j;

  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-buffer recall container not provided");
    
    return(NULL);
  }

  start_recall = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);

  channel_position = 0;
  
  if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
    channel_position = ags_fx_factory_get_output_position(audio,
							  position);
  }else{
    channel_position = ags_fx_factory_get_input_position(audio,
							 position);
  }
  
  /* audio - play context */
  fx_buffer_audio = NULL;
  fx_buffer_audio_processor = NULL;

  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxBufferAudio */
    fx_buffer_audio = (AgsFxBufferAudio *) g_object_new(AGS_TYPE_FX_BUFFER_AUDIO,
							    "output-soundcard", output_soundcard,
							    "audio", audio,
							    "recall-container", play_container,
							    NULL);
    ags_recall_set_flags((AgsRecall *) fx_buffer_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_buffer_audio,
				 0);
    ags_recall_set_behaviour_flags((AgsRecall *) fx_buffer_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_buffer_audio,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_buffer_audio);
    
    start_recall = g_list_prepend(start_recall,
				  fx_buffer_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_buffer_audio));

    /* AgsFxBufferAudioProcessor */
    fx_buffer_audio_processor = (AgsFxBufferAudioProcessor *) g_object_new(AGS_TYPE_FX_BUFFER_AUDIO_PROCESSOR,
									       "output-soundcard", output_soundcard,
									       "audio", audio,
									       "recall-audio", fx_buffer_audio,
									       "recall-container", play_container,
									       NULL);
    ags_recall_set_flags((AgsRecall *) fx_buffer_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_buffer_audio_processor,
				 0);
    ags_recall_set_behaviour_flags((AgsRecall *) fx_buffer_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_buffer_audio_processor,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_buffer_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_buffer_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_buffer_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_buffer_audio = ags_recall_container_get_recall_audio(play_container);

    if(fx_buffer_audio != NULL){
      g_object_ref(fx_buffer_audio);
    }
    
    start_recall_audio_run = ags_recall_container_get_recall_audio_run(play_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_buffer_audio_processor = recall_audio_run->data;
      g_object_ref(fx_buffer_audio_processor);
    }
  }
    
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* add recall container */
      ags_channel_add_recall_container(channel,
				       (GObject *) play_container);
	
      /* AgsFxBufferChannel */
      fx_buffer_channel = (AgsFxBufferChannel *) g_object_new(AGS_TYPE_FX_BUFFER_CHANNEL,
								  "output-soundcard", output_soundcard,
								  "source", channel,
								  "recall-audio", fx_buffer_audio,
								  "recall-container", play_container,
								  NULL);
      ags_recall_set_flags((AgsRecall *) fx_buffer_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_buffer_channel,
				   0);
      ags_recall_set_behaviour_flags((AgsRecall *) fx_buffer_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_buffer_channel,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_buffer_channel);
      
      start_recall = g_list_prepend(start_recall,
				    fx_buffer_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_buffer_channel));

      /* AgsFxBufferChannelProcessor */
      fx_buffer_channel_processor = (AgsFxBufferChannelProcessor *) g_object_new(AGS_TYPE_FX_BUFFER_CHANNEL_PROCESSOR,
										     "output-soundcard", output_soundcard,
										     "source", channel,
										     "recall-audio", fx_buffer_audio,
										     "recall-audio-run", fx_buffer_audio_processor,
										     "recall-channel", fx_buffer_channel,
										     "recall-container", play_container,
										     NULL);
      ags_recall_set_flags((AgsRecall *) fx_buffer_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_buffer_channel_processor,
				   0);
      ags_recall_set_behaviour_flags((AgsRecall *) fx_buffer_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_buffer_channel_processor,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_buffer_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_buffer_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_buffer_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_buffer_audio != NULL){
      g_object_unref(fx_buffer_audio);
    }
    
    if(fx_buffer_audio_processor != NULL){
      g_object_unref(fx_buffer_audio_processor);
    }
  }
  
  /* audio - recall context */
  fx_buffer_audio = NULL;
  fx_buffer_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxBufferAudio */
    fx_buffer_audio = (AgsFxBufferAudio *) g_object_new(AGS_TYPE_FX_BUFFER_AUDIO,
							"output-soundcard", output_soundcard,
							"audio", audio,
							"recall-container", recall_container,
							NULL);
    ags_recall_set_flags((AgsRecall *) fx_buffer_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_buffer_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_MIDI |
				  AGS_SOUND_ABILITY_WAVE));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_buffer_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_buffer_audio,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_buffer_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_buffer_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_buffer_audio));

    /* AgsFxBufferAudioProcessor */
    fx_buffer_audio_processor = (AgsFxBufferAudioProcessor *) g_object_new(AGS_TYPE_FX_BUFFER_AUDIO_PROCESSOR,
									   "output-soundcard", output_soundcard,
									   "audio", audio,
									   "recall-audio", fx_buffer_audio,
									   "recall-container", recall_container,
									   NULL);
    ags_recall_set_flags((AgsRecall *) fx_buffer_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_buffer_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_MIDI |
				  AGS_SOUND_ABILITY_WAVE));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_buffer_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_buffer_audio_processor,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_buffer_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_buffer_audio_processor);
    
    ags_connectable_connect(AGS_CONNECTABLE(fx_buffer_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_buffer_audio = ags_recall_container_get_recall_audio(recall_container);

    if(fx_buffer_audio != NULL){
      g_object_ref(fx_buffer_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(recall_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_buffer_audio_processor = recall_audio_run->data;
      g_object_ref(fx_buffer_audio_processor);
    }
  }
    
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* add recall container */
      ags_channel_add_recall_container(channel,
				       (GObject *) recall_container);
	
      /* AgsFxBufferChannel */
      fx_buffer_channel = (AgsFxBufferChannel *) g_object_new(AGS_TYPE_FX_BUFFER_CHANNEL,
							      "output-soundcard", output_soundcard,
							      "source", channel,
							      "recall-audio", fx_buffer_audio,
							      "recall-container", recall_container,
							      NULL);
      ags_recall_set_flags((AgsRecall *) fx_buffer_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_buffer_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_MIDI |
				    AGS_SOUND_ABILITY_WAVE));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_buffer_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_buffer_channel,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_buffer_channel);
      
      start_recall = g_list_prepend(start_recall,
				    fx_buffer_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_buffer_channel));

      /* AgsFxBufferChannelProcessor */
      fx_buffer_channel_processor = (AgsFxBufferChannelProcessor *) g_object_new(AGS_TYPE_FX_BUFFER_CHANNEL_PROCESSOR,
										 "output-soundcard", output_soundcard,
										 "source", channel,
										 "recall-audio", fx_buffer_audio,
										 "recall-audio-run", fx_buffer_audio_processor,
										 "recall-channel", fx_buffer_channel,
										 "recall-container", recall_container,
										 NULL);
      ags_recall_set_flags((AgsRecall *) fx_buffer_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_buffer_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_MIDI |
				    AGS_SOUND_ABILITY_WAVE));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_buffer_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_buffer_channel_processor,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_buffer_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_buffer_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_buffer_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_buffer_audio != NULL){
      g_object_unref(fx_buffer_audio);
    }
    
    if(fx_buffer_audio_processor != NULL){
      g_object_unref(fx_buffer_audio_processor);
    }
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(input_sequencer != NULL){
    g_object_unref(input_sequencer);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(start_recall);
}

GList*
ags_fx_factory_create_volume(AgsAudio *audio,
			     AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			     gchar *plugin_name,
			     gchar *filename,
			     gchar *effect,
			     guint start_audio_channel, guint stop_audio_channel,
			     guint start_pad, guint stop_pad,
			     gint position,
			     guint create_flags, guint recall_flags)
{
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;

  AgsFxVolumeAudio *fx_volume_audio;
  AgsFxVolumeAudioProcessor *fx_volume_audio_processor;
  AgsFxVolumeChannel *fx_volume_channel;
  AgsFxVolumeChannelProcessor *fx_volume_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  gint channel_position;
  guint audio_channels;
  guint i, j;
  
  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-volume recall container not provided");
    
    return(NULL);
  }
  
  start_recall = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);

  channel_position = 0;
  
  if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
    channel_position = ags_fx_factory_get_output_position(audio,
							  position);
  }else{
    channel_position = ags_fx_factory_get_input_position(audio,
							 position);
  }
  
  /* audio - play context */
  fx_volume_audio = NULL;
  fx_volume_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxVolumeAudio */
    fx_volume_audio = (AgsFxVolumeAudio *) g_object_new(AGS_TYPE_FX_VOLUME_AUDIO,
							"output-soundcard", output_soundcard,
							"audio", audio,
							"recall-container", play_container,
							NULL);
    ags_recall_set_flags((AgsRecall *) fx_volume_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_volume_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_volume_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_volume_audio,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_volume_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_volume_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_volume_audio));
    
    /* AgsFxVolumeAudioProcessor */
    fx_volume_audio_processor = (AgsFxVolumeAudioProcessor *) g_object_new(AGS_TYPE_FX_VOLUME_AUDIO_PROCESSOR,
									   "output-soundcard", output_soundcard,
									   "audio", audio,
									   "recall-audio", fx_volume_audio,
									   "recall-container", play_container,
									   NULL);
    ags_recall_set_flags((AgsRecall *) fx_volume_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_volume_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_volume_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_volume_audio_processor,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_volume_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_volume_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_volume_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_volume_audio = ags_recall_container_get_recall_audio(play_container);

    if(fx_volume_audio != NULL){
      g_object_ref(fx_volume_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(play_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_volume_audio_processor = recall_audio_run->data;
      g_object_ref(fx_volume_audio_processor);
    }
  }
  
  /* channel - play context */
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxVolumeChannel */
      fx_volume_channel = (AgsFxVolumeChannel *) g_object_new(AGS_TYPE_FX_VOLUME_CHANNEL,
							      "output-soundcard", output_soundcard,
							      "source", channel,
							      "recall-audio", fx_volume_audio,
							      "recall-container", play_container,
							      NULL);
      ags_recall_set_flags((AgsRecall *) fx_volume_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_volume_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_volume_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_volume_channel,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_volume_channel);
      
      start_recall = g_list_prepend(start_recall,
				    fx_volume_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_volume_channel));

      /* AgsFxVolumeChannelProcessor */
      fx_volume_channel_processor = (AgsFxVolumeChannelProcessor *) g_object_new(AGS_TYPE_FX_VOLUME_CHANNEL_PROCESSOR,
										 "output-soundcard", output_soundcard,
										 "source", channel,
										 "recall-audio", fx_volume_audio,
										 "recall-audio-run", fx_volume_audio_processor,
										 "recall-channel", fx_volume_channel,
										 "recall-container", play_container,
										 NULL);
      ags_recall_set_flags((AgsRecall *) fx_volume_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_volume_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_volume_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_volume_channel_processor,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_volume_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_volume_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_volume_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_volume_audio != NULL){
      g_object_unref(fx_volume_audio);
    }
    
    if(fx_volume_audio_processor != NULL){
      g_object_unref(fx_volume_audio_processor);
    }
  }
  
  /* audio - recall context */
  fx_volume_audio = NULL;
  fx_volume_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxVolumeAudio */
    fx_volume_audio = (AgsFxVolumeAudio *) g_object_new(AGS_TYPE_FX_VOLUME_AUDIO,
							"output-soundcard", output_soundcard,
							"audio", audio,
							"recall-container", play_container,
							NULL);
    ags_recall_set_flags((AgsRecall *) fx_volume_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_volume_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_volume_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_volume_audio,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_volume_audio);
    
    start_recall = g_list_prepend(start_recall,
				  fx_volume_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_volume_audio));
    
    /* AgsFxVolumeAudioProcessor */
    fx_volume_audio_processor = (AgsFxVolumeAudioProcessor *) g_object_new(AGS_TYPE_FX_VOLUME_AUDIO_PROCESSOR,
									   "output-soundcard", output_soundcard,
									   "audio", audio,
									   "recall-audio", fx_volume_audio,
									   "recall-container", play_container,
									   NULL);
    ags_recall_set_flags((AgsRecall *) fx_volume_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_volume_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_volume_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_volume_audio_processor,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_volume_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_volume_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_volume_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_volume_audio = ags_recall_container_get_recall_audio(recall_container);

    if(fx_volume_audio != NULL){
      g_object_ref(fx_volume_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(recall_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_volume_audio_processor = recall_audio_run->data;
      g_object_ref(fx_volume_audio_processor);
    }
  }
  
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxVolumeChannel */
      fx_volume_channel = (AgsFxVolumeChannel *) g_object_new(AGS_TYPE_FX_VOLUME_CHANNEL,
							      "output-soundcard", output_soundcard,
							      "source", channel,
							      "recall-audio", fx_volume_audio,
							      "recall-container", recall_container,
							      NULL);
      ags_recall_set_flags((AgsRecall *) fx_volume_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_volume_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_volume_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_volume_channel,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_volume_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_volume_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_volume_channel));

      /* AgsFxVolumeChannelProcessor */
      fx_volume_channel_processor = (AgsFxVolumeChannelProcessor *) g_object_new(AGS_TYPE_FX_VOLUME_CHANNEL_PROCESSOR,
										 "output-soundcard", output_soundcard,
										 "source", channel,
										 "recall-audio", fx_volume_audio,
										 "recall-audio-run", fx_volume_audio_processor,
										 "recall-channel", fx_volume_channel,
										 "recall-container", recall_container,
										 NULL);
      ags_recall_set_flags((AgsRecall *) fx_volume_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_volume_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_volume_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_volume_channel_processor,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_volume_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_volume_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_volume_channel_processor));

      /* iterate */
      next_channel = ags_channel_nth(channel,
				     audio_channels - stop_audio_channel + 1);

      if(channel != NULL){
	g_object_unref(channel);
      }
      
      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }  

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_volume_audio != NULL){
      g_object_unref(fx_volume_audio);
    }
    
    if(fx_volume_audio_processor != NULL){
      g_object_unref(fx_volume_audio_processor);
    }
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(input_sequencer != NULL){
    g_object_unref(input_sequencer);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(start_recall);
}

GList*
ags_fx_factory_create_low_pass(AgsAudio *audio,
			       AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			       gchar *plugin_name,
			       gchar *filename,
			       gchar *effect,
			       guint start_audio_channel, guint stop_audio_channel,
			       guint start_pad, guint stop_pad,
			       gint position,
			       guint create_flags, guint recall_flags)
{
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;

  AgsFxLowPassAudio *fx_low_pass_audio;
  AgsFxLowPassAudioProcessor *fx_low_pass_audio_processor;
  AgsFxLowPassChannel *fx_low_pass_channel;
  AgsFxLowPassChannelProcessor *fx_low_pass_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  gint channel_position;
  guint audio_channels;
  guint i, j;
  
  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-low-pass recall container not provided");
    
    return(NULL);
  }
  
  start_recall = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);

  channel_position = 0;
  
  if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
    channel_position = ags_fx_factory_get_output_position(audio,
							  position);
  }else{
    channel_position = ags_fx_factory_get_input_position(audio,
							 position);
  }
  
  /* audio - play context */
  fx_low_pass_audio = NULL;
  fx_low_pass_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxLowPassAudio */
    fx_low_pass_audio = (AgsFxLowPassAudio *) g_object_new(AGS_TYPE_FX_LOW_PASS_AUDIO,
							   "output-soundcard", output_soundcard,
							   "audio", audio,
							   "recall-container", play_container,
							   NULL);
    ags_recall_set_flags((AgsRecall *) fx_low_pass_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_low_pass_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_low_pass_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_low_pass_audio,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_low_pass_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_low_pass_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_low_pass_audio));
    
    /* AgsFxLowPassAudioProcessor */
    fx_low_pass_audio_processor = (AgsFxLowPassAudioProcessor *) g_object_new(AGS_TYPE_FX_LOW_PASS_AUDIO_PROCESSOR,
									      "output-soundcard", output_soundcard,
									      "audio", audio,
									      "recall-audio", fx_low_pass_audio,
									      "recall-container", play_container,
									      NULL);
    ags_recall_set_flags((AgsRecall *) fx_low_pass_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_low_pass_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_low_pass_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_low_pass_audio_processor,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_low_pass_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_low_pass_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_low_pass_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_low_pass_audio = ags_recall_container_get_recall_audio(play_container);

    if(fx_low_pass_audio != NULL){
      g_object_ref(fx_low_pass_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(play_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_low_pass_audio_processor = recall_audio_run->data;
      g_object_ref(fx_low_pass_audio_processor);
    }
  }
  
  /* channel - play context */
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxLowPassChannel */
      fx_low_pass_channel = (AgsFxLowPassChannel *) g_object_new(AGS_TYPE_FX_LOW_PASS_CHANNEL,
								 "output-soundcard", output_soundcard,
								 "source", channel,
								 "recall-audio", fx_low_pass_audio,
								 "recall-container", play_container,
								 NULL);
      ags_recall_set_flags((AgsRecall *) fx_low_pass_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_low_pass_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_low_pass_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_low_pass_channel,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_low_pass_channel);
      
      start_recall = g_list_prepend(start_recall,
				    fx_low_pass_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_low_pass_channel));

      /* AgsFxLowPassChannelProcessor */
      fx_low_pass_channel_processor = (AgsFxLowPassChannelProcessor *) g_object_new(AGS_TYPE_FX_LOW_PASS_CHANNEL_PROCESSOR,
										    "output-soundcard", output_soundcard,
										    "source", channel,
										    "recall-audio", fx_low_pass_audio,
										    "recall-audio-run", fx_low_pass_audio_processor,
										    "recall-channel", fx_low_pass_channel,
										    "recall-container", play_container,
										    NULL);
      ags_recall_set_flags((AgsRecall *) fx_low_pass_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_low_pass_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_low_pass_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_low_pass_channel_processor,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_low_pass_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_low_pass_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_low_pass_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_low_pass_audio != NULL){
      g_object_unref(fx_low_pass_audio);
    }
    
    if(fx_low_pass_audio_processor != NULL){
      g_object_unref(fx_low_pass_audio_processor);
    }
  }
  
  /* audio - recall context */
  fx_low_pass_audio = NULL;
  fx_low_pass_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxLowPassAudio */
    fx_low_pass_audio = (AgsFxLowPassAudio *) g_object_new(AGS_TYPE_FX_LOW_PASS_AUDIO,
							   "output-soundcard", output_soundcard,
							   "audio", audio,
							   "recall-container", play_container,
							   NULL);
    ags_recall_set_flags((AgsRecall *) fx_low_pass_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_low_pass_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_low_pass_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_low_pass_audio,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_low_pass_audio);
    
    start_recall = g_list_prepend(start_recall,
				  fx_low_pass_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_low_pass_audio));
    
    /* AgsFxLowPassAudioProcessor */
    fx_low_pass_audio_processor = (AgsFxLowPassAudioProcessor *) g_object_new(AGS_TYPE_FX_LOW_PASS_AUDIO_PROCESSOR,
									      "output-soundcard", output_soundcard,
									      "audio", audio,
									      "recall-audio", fx_low_pass_audio,
									      "recall-container", play_container,
									      NULL);
    ags_recall_set_flags((AgsRecall *) fx_low_pass_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_low_pass_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_low_pass_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_low_pass_audio_processor,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_low_pass_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_low_pass_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_low_pass_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_low_pass_audio = ags_recall_container_get_recall_audio(recall_container);

    if(fx_low_pass_audio != NULL){
      g_object_ref(fx_low_pass_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(recall_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_low_pass_audio_processor = recall_audio_run->data;
      g_object_ref(fx_low_pass_audio_processor);
    }
  }
  
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxLowPassChannel */
      fx_low_pass_channel = (AgsFxLowPassChannel *) g_object_new(AGS_TYPE_FX_LOW_PASS_CHANNEL,
								 "output-soundcard", output_soundcard,
								 "source", channel,
								 "recall-audio", fx_low_pass_audio,
								 "recall-container", recall_container,
								 NULL);
      ags_recall_set_flags((AgsRecall *) fx_low_pass_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_low_pass_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_low_pass_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_low_pass_channel,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_low_pass_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_low_pass_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_low_pass_channel));

      /* AgsFxLowPassChannelProcessor */
      fx_low_pass_channel_processor = (AgsFxLowPassChannelProcessor *) g_object_new(AGS_TYPE_FX_LOW_PASS_CHANNEL_PROCESSOR,
										    "output-soundcard", output_soundcard,
										    "source", channel,
										    "recall-audio", fx_low_pass_audio,
										    "recall-audio-run", fx_low_pass_audio_processor,
										    "recall-channel", fx_low_pass_channel,
										    "recall-container", recall_container,
										    NULL);
      ags_recall_set_flags((AgsRecall *) fx_low_pass_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_low_pass_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_low_pass_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_low_pass_channel_processor,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_low_pass_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_low_pass_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_low_pass_channel_processor));

      /* iterate */
      next_channel = ags_channel_nth(channel,
				     audio_channels - stop_audio_channel + 1);

      if(channel != NULL){
	g_object_unref(channel);
      }
      
      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }  

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_low_pass_audio != NULL){
      g_object_unref(fx_low_pass_audio);
    }
    
    if(fx_low_pass_audio_processor != NULL){
      g_object_unref(fx_low_pass_audio_processor);
    }
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(input_sequencer != NULL){
    g_object_unref(input_sequencer);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(start_recall);
}

GList*
ags_fx_factory_create_high_pass(AgsAudio *audio,
				AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				gchar *plugin_name,
				gchar *filename,
				gchar *effect,
				guint start_audio_channel, guint stop_audio_channel,
				guint start_pad, guint stop_pad,
				gint position,
				guint create_flags, guint recall_flags)
{
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;

  AgsFxHighPassAudio *fx_high_pass_audio;
  AgsFxHighPassAudioProcessor *fx_high_pass_audio_processor;
  AgsFxHighPassChannel *fx_high_pass_channel;
  AgsFxHighPassChannelProcessor *fx_high_pass_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  gint channel_position;
  guint audio_channels;
  guint i, j;
  
  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-high-pass recall container not provided");
    
    return(NULL);
  }
  
  start_recall = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);

  channel_position = 0;
  
  if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
    channel_position = ags_fx_factory_get_output_position(audio,
							  position);
  }else{
    channel_position = ags_fx_factory_get_input_position(audio,
							 position);
  }
  
  /* audio - play context */
  fx_high_pass_audio = NULL;
  fx_high_pass_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxHighPassAudio */
    fx_high_pass_audio = (AgsFxHighPassAudio *) g_object_new(AGS_TYPE_FX_HIGH_PASS_AUDIO,
							     "output-soundcard", output_soundcard,
							     "audio", audio,
							     "recall-container", play_container,
							     NULL);
    ags_recall_set_flags((AgsRecall *) fx_high_pass_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_high_pass_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_high_pass_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_high_pass_audio,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_high_pass_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_high_pass_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_high_pass_audio));
    
    /* AgsFxHighPassAudioProcessor */
    fx_high_pass_audio_processor = (AgsFxHighPassAudioProcessor *) g_object_new(AGS_TYPE_FX_HIGH_PASS_AUDIO_PROCESSOR,
										"output-soundcard", output_soundcard,
										"audio", audio,
										"recall-audio", fx_high_pass_audio,
										"recall-container", play_container,
										NULL);
    ags_recall_set_flags((AgsRecall *) fx_high_pass_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_high_pass_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_high_pass_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_high_pass_audio_processor,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_high_pass_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_high_pass_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_high_pass_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_high_pass_audio = ags_recall_container_get_recall_audio(play_container);

    if(fx_high_pass_audio != NULL){
      g_object_ref(fx_high_pass_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(play_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_high_pass_audio_processor = recall_audio_run->data;
      g_object_ref(fx_high_pass_audio_processor);
    }
  }
  
  /* channel - play context */
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxHighPassChannel */
      fx_high_pass_channel = (AgsFxHighPassChannel *) g_object_new(AGS_TYPE_FX_HIGH_PASS_CHANNEL,
								   "output-soundcard", output_soundcard,
								   "source", channel,
								   "recall-audio", fx_high_pass_audio,
								   "recall-container", play_container,
								   NULL);
      ags_recall_set_flags((AgsRecall *) fx_high_pass_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_high_pass_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_high_pass_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_high_pass_channel,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_high_pass_channel);
      
      start_recall = g_list_prepend(start_recall,
				    fx_high_pass_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_high_pass_channel));

      /* AgsFxHighPassChannelProcessor */
      fx_high_pass_channel_processor = (AgsFxHighPassChannelProcessor *) g_object_new(AGS_TYPE_FX_HIGH_PASS_CHANNEL_PROCESSOR,
										      "output-soundcard", output_soundcard,
										      "source", channel,
										      "recall-audio", fx_high_pass_audio,
										      "recall-audio-run", fx_high_pass_audio_processor,
										      "recall-channel", fx_high_pass_channel,
										      "recall-container", play_container,
										      NULL);
      ags_recall_set_flags((AgsRecall *) fx_high_pass_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_high_pass_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_high_pass_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_high_pass_channel_processor,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_high_pass_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_high_pass_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_high_pass_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_high_pass_audio != NULL){
      g_object_unref(fx_high_pass_audio);
    }
    
    if(fx_high_pass_audio_processor != NULL){
      g_object_unref(fx_high_pass_audio_processor);
    }
  }
  
  /* audio - recall context */
  fx_high_pass_audio = NULL;
  fx_high_pass_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxHighPassAudio */
    fx_high_pass_audio = (AgsFxHighPassAudio *) g_object_new(AGS_TYPE_FX_HIGH_PASS_AUDIO,
							     "output-soundcard", output_soundcard,
							     "audio", audio,
							     "recall-container", play_container,
							     NULL);
    ags_recall_set_flags((AgsRecall *) fx_high_pass_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_high_pass_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_high_pass_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_high_pass_audio,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_high_pass_audio);
    
    start_recall = g_list_prepend(start_recall,
				  fx_high_pass_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_high_pass_audio));
    
    /* AgsFxHighPassAudioProcessor */
    fx_high_pass_audio_processor = (AgsFxHighPassAudioProcessor *) g_object_new(AGS_TYPE_FX_HIGH_PASS_AUDIO_PROCESSOR,
										"output-soundcard", output_soundcard,
										"audio", audio,
										"recall-audio", fx_high_pass_audio,
										"recall-container", play_container,
										NULL);
    ags_recall_set_flags((AgsRecall *) fx_high_pass_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_high_pass_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_high_pass_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_high_pass_audio_processor,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_high_pass_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_high_pass_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_high_pass_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_high_pass_audio = ags_recall_container_get_recall_audio(recall_container);

    if(fx_high_pass_audio != NULL){
      g_object_ref(fx_high_pass_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(recall_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_high_pass_audio_processor = recall_audio_run->data;
      g_object_ref(fx_high_pass_audio_processor);
    }
  }
  
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxHighPassChannel */
      fx_high_pass_channel = (AgsFxHighPassChannel *) g_object_new(AGS_TYPE_FX_HIGH_PASS_CHANNEL,
								   "output-soundcard", output_soundcard,
								   "source", channel,
								   "recall-audio", fx_high_pass_audio,
								   "recall-container", recall_container,
								   NULL);
      ags_recall_set_flags((AgsRecall *) fx_high_pass_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_high_pass_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_high_pass_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_high_pass_channel,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_high_pass_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_high_pass_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_high_pass_channel));

      /* AgsFxHighPassChannelProcessor */
      fx_high_pass_channel_processor = (AgsFxHighPassChannelProcessor *) g_object_new(AGS_TYPE_FX_HIGH_PASS_CHANNEL_PROCESSOR,
										      "output-soundcard", output_soundcard,
										      "source", channel,
										      "recall-audio", fx_high_pass_audio,
										      "recall-audio-run", fx_high_pass_audio_processor,
										      "recall-channel", fx_high_pass_channel,
										      "recall-container", recall_container,
										      NULL);
      ags_recall_set_flags((AgsRecall *) fx_high_pass_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_high_pass_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_high_pass_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_high_pass_channel_processor,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_high_pass_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_high_pass_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_high_pass_channel_processor));

      /* iterate */
      next_channel = ags_channel_nth(channel,
				     audio_channels - stop_audio_channel + 1);

      if(channel != NULL){
	g_object_unref(channel);
      }
      
      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }  

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_high_pass_audio != NULL){
      g_object_unref(fx_high_pass_audio);
    }
    
    if(fx_high_pass_audio_processor != NULL){
      g_object_unref(fx_high_pass_audio_processor);
    }
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(input_sequencer != NULL){
    g_object_unref(input_sequencer);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(start_recall);
}

GList*
ags_fx_factory_create_peak(AgsAudio *audio,
			   AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			   gchar *plugin_name,
			   gchar *filename,
			   gchar *effect,
			   guint start_audio_channel, guint stop_audio_channel,
			   guint start_pad, guint stop_pad,
			   gint position,
			   guint create_flags, guint recall_flags)
{
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;

  AgsFxPeakAudio *fx_peak_audio;
  AgsFxPeakAudioProcessor *fx_peak_audio_processor;
  AgsFxPeakChannel *fx_peak_channel;
  AgsFxPeakChannelProcessor *fx_peak_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  gint channel_position;
  guint audio_channels;
  guint i, j;
  
  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-peak recall container not provided");
    
    return(NULL);
  }
  
  start_recall = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);
  
  channel_position = 0;
  
  if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
    channel_position = ags_fx_factory_get_output_position(audio,
							  position);
  }else{
    channel_position = ags_fx_factory_get_input_position(audio,
							 position);
  }

  /* audio - play context */
  fx_peak_audio = NULL;
  fx_peak_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxPeakAudio */
    fx_peak_audio = (AgsFxPeakAudio *) g_object_new(AGS_TYPE_FX_PEAK_AUDIO,
						    "output-soundcard", output_soundcard,
						    "audio", audio,
						    "recall-container", play_container,
						    NULL);
    ags_recall_set_flags((AgsRecall *) fx_peak_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_peak_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_peak_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_peak_audio,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_peak_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_peak_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_peak_audio));
    
    /* AgsFxPeakAudioProcessor */
    fx_peak_audio_processor = (AgsFxPeakAudioProcessor *) g_object_new(AGS_TYPE_FX_PEAK_AUDIO_PROCESSOR,
								       "output-soundcard", output_soundcard,
								       "audio", audio,
								       "recall-audio", fx_peak_audio,
								       "recall-container", play_container,
								       NULL);
    ags_recall_set_flags((AgsRecall *) fx_peak_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_peak_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_peak_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_peak_audio_processor,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_peak_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_peak_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_peak_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_peak_audio = ags_recall_container_get_recall_audio(play_container);

    if(fx_peak_audio != NULL){
      g_object_ref(fx_peak_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(play_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_peak_audio_processor = recall_audio_run->data;
      g_object_ref(fx_peak_audio_processor);
    }
  }
  
  /* channel - play context */
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxPeakChannel */
      fx_peak_channel = (AgsFxPeakChannel *) g_object_new(AGS_TYPE_FX_PEAK_CHANNEL,
							  "output-soundcard", output_soundcard,
							  "source", channel,
							  "recall-audio", fx_peak_audio,
							  "recall-container", play_container,
							  NULL);
      ags_recall_set_flags((AgsRecall *) fx_peak_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_peak_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_peak_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_peak_channel,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_peak_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_peak_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_peak_channel));

      /* AgsFxPeakChannelProcessor */
      fx_peak_channel_processor = (AgsFxPeakChannelProcessor *) g_object_new(AGS_TYPE_FX_PEAK_CHANNEL_PROCESSOR,
									     "output-soundcard", output_soundcard,
									     "source", channel,
									     "recall-audio", fx_peak_audio,
									     "recall-audio-run", fx_peak_audio_processor,
									     "recall-channel", fx_peak_channel,
									     "recall-container", play_container,
									     NULL);
      ags_recall_set_flags((AgsRecall *) fx_peak_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_peak_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_peak_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_peak_channel_processor,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_peak_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_peak_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_peak_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  /* audio - recall context */
  fx_peak_audio = NULL;
  fx_peak_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxPeakAudio */
    fx_peak_audio = (AgsFxPeakAudio *) g_object_new(AGS_TYPE_FX_PEAK_AUDIO,
						    "output-soundcard", output_soundcard,
						    "audio", audio,
						    "recall-container", recall_container,
						    NULL);
    ags_recall_set_flags((AgsRecall *) fx_peak_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_peak_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_peak_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_peak_audio,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_peak_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_peak_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_peak_audio));
    
    /* AgsFxPeakAudioProcessor */
    fx_peak_audio_processor = (AgsFxPeakAudioProcessor *) g_object_new(AGS_TYPE_FX_PEAK_AUDIO_PROCESSOR,
								       "output-soundcard", output_soundcard,
								       "audio", audio,
								       "recall-audio", fx_peak_audio,
								       "recall-container", recall_container,
								       NULL);
    ags_recall_set_flags((AgsRecall *) fx_peak_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_peak_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_peak_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_peak_audio_processor,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_peak_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_peak_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_peak_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_peak_audio = ags_recall_container_get_recall_audio(recall_container);

    if(fx_peak_audio != NULL){
      g_object_ref(fx_peak_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(recall_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_peak_audio_processor = recall_audio_run->data;
      g_object_ref(fx_peak_audio_processor);
    }
  }
  
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxPeakChannel */
      fx_peak_channel = (AgsFxPeakChannel *) g_object_new(AGS_TYPE_FX_PEAK_CHANNEL,
							  "output-soundcard", output_soundcard,
							  "source", channel,
							  "recall-audio", fx_peak_audio,
							  "recall-container", recall_container,
							  NULL);
      ags_recall_set_flags((AgsRecall *) fx_peak_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_peak_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_peak_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_peak_channel,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_peak_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_peak_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_peak_channel));

      /* AgsFxPeakChannelProcessor */
      fx_peak_channel_processor = (AgsFxPeakChannelProcessor *) g_object_new(AGS_TYPE_FX_PEAK_CHANNEL_PROCESSOR,
									     "output-soundcard", output_soundcard,
									     "source", channel,
									     "recall-audio", fx_peak_audio,
									     "recall-audio-run", fx_peak_audio_processor,
									     "recall-channel", fx_peak_channel,
									     "recall-container", recall_container,
									     NULL);
      ags_recall_set_flags((AgsRecall *) fx_peak_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_peak_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_peak_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_peak_channel_processor,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_peak_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_peak_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_peak_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_peak_audio != NULL){
      g_object_unref(fx_peak_audio);
    }
    
    if(fx_peak_audio_processor != NULL){
      g_object_unref(fx_peak_audio_processor);
    }
  }
  
  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(input_sequencer != NULL){
    g_object_unref(input_sequencer);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(start_recall);
}

GList*
ags_fx_factory_create_eq10(AgsAudio *audio,
			   AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			   gchar *plugin_name,
			   gchar *filename,
			   gchar *effect,
			   guint start_audio_channel, guint stop_audio_channel,
			   guint start_pad, guint stop_pad,
			   gint position,
			   guint create_flags, guint recall_flags)
{
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;

  AgsFxEq10Audio *fx_eq10_audio;
  AgsFxEq10AudioProcessor *fx_eq10_audio_processor;
  AgsFxEq10Channel *fx_eq10_channel;
  AgsFxEq10ChannelProcessor *fx_eq10_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  gint channel_position;
  guint audio_channels;
  guint i, j;
  
  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-eq10 recall container not provided");
    
    return(NULL);
  }
  
  start_recall = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);

  channel_position = 0;
  
  if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
    channel_position = ags_fx_factory_get_output_position(audio,
							  position);
  }else{
    channel_position = ags_fx_factory_get_input_position(audio,
							 position);
  }
  
  /* audio - play context */
  fx_eq10_audio = NULL;
  fx_eq10_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxEq10Audio */
    fx_eq10_audio = (AgsFxEq10Audio *) g_object_new(AGS_TYPE_FX_EQ10_AUDIO,
						    "output-soundcard", output_soundcard,
						    "audio", audio,
						    "recall-container", play_container,
						    NULL);
    ags_recall_set_flags((AgsRecall *) fx_eq10_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_eq10_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_eq10_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_eq10_audio,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_eq10_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_eq10_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_eq10_audio));
    
    /* AgsFxEq10AudioProcessor */
    fx_eq10_audio_processor = (AgsFxEq10AudioProcessor *) g_object_new(AGS_TYPE_FX_EQ10_AUDIO_PROCESSOR,
								       "output-soundcard", output_soundcard,
								       "audio", audio,
								       "recall-audio", fx_eq10_audio,
								       "recall-container", play_container,
								       NULL);
    ags_recall_set_flags((AgsRecall *) fx_eq10_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_eq10_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_eq10_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_eq10_audio_processor,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_eq10_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_eq10_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_eq10_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_eq10_audio = ags_recall_container_get_recall_audio(play_container);

    if(fx_eq10_audio != NULL){
      g_object_ref(fx_eq10_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(play_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_eq10_audio_processor = recall_audio_run->data;
      g_object_ref(fx_eq10_audio_processor);
    }
  }
  
  /* channel - play context */
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxEq10Channel */
      fx_eq10_channel = (AgsFxEq10Channel *) g_object_new(AGS_TYPE_FX_EQ10_CHANNEL,
							  "output-soundcard", output_soundcard,
							  "source", channel,
							  "recall-audio", fx_eq10_audio,
							  "recall-container", play_container,
							  NULL);
      ags_recall_set_flags((AgsRecall *) fx_eq10_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_eq10_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_eq10_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_eq10_channel,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_eq10_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_eq10_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_eq10_channel));

      /* AgsFxEq10ChannelProcessor */
      fx_eq10_channel_processor = (AgsFxEq10ChannelProcessor *) g_object_new(AGS_TYPE_FX_EQ10_CHANNEL_PROCESSOR,
									     "output-soundcard", output_soundcard,
									     "source", channel,
									     "recall-audio", fx_eq10_audio,
									     "recall-audio-run", fx_eq10_audio_processor,
									     "recall-channel", fx_eq10_channel,
									     "recall-container", play_container,
									     NULL);
      ags_recall_set_flags((AgsRecall *) fx_eq10_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_eq10_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_eq10_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_eq10_channel_processor,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_eq10_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_eq10_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_eq10_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_eq10_audio != NULL){
      g_object_unref(fx_eq10_audio);
    }
    
    if(fx_eq10_audio_processor != NULL){
      g_object_unref(fx_eq10_audio_processor);
    }
  }

  /* audio - recall context */
  fx_eq10_audio = NULL;
  fx_eq10_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxEq10Audio */
    fx_eq10_audio = (AgsFxEq10Audio *) g_object_new(AGS_TYPE_FX_EQ10_AUDIO,
						    "output-soundcard", output_soundcard,
						    "audio", audio,
						    "recall-container", recall_container,
						    NULL);
    ags_recall_set_flags((AgsRecall *) fx_eq10_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_eq10_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_eq10_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_eq10_audio,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_eq10_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_eq10_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_eq10_audio));
    
    /* AgsFxEq10AudioProcessor */
    fx_eq10_audio_processor = (AgsFxEq10AudioProcessor *) g_object_new(AGS_TYPE_FX_EQ10_AUDIO_PROCESSOR,
								       "output-soundcard", output_soundcard,
								       "audio", audio,
								       "recall-audio", fx_eq10_audio,
								       "recall-container", recall_container,
								       NULL);
    ags_recall_set_flags((AgsRecall *) fx_eq10_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_eq10_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_eq10_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_eq10_audio_processor,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_eq10_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_eq10_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_eq10_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_eq10_audio = ags_recall_container_get_recall_audio(recall_container);

    if(fx_eq10_audio != NULL){
      g_object_ref(fx_eq10_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(recall_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_eq10_audio_processor = recall_audio_run->data;
      g_object_ref(fx_eq10_audio_processor);
    }
  }

  /* channel - recall context */
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxEq10Channel */
      fx_eq10_channel = (AgsFxEq10Channel *) g_object_new(AGS_TYPE_FX_EQ10_CHANNEL,
							  "output-soundcard", output_soundcard,
							  "source", channel,
							  "recall-audio", fx_eq10_audio,
							  "recall-container", recall_container,
							  NULL);
      ags_recall_set_flags((AgsRecall *) fx_eq10_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_eq10_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_eq10_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_eq10_channel,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_eq10_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_eq10_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_eq10_channel));

      /* AgsFxEq10ChannelProcessor */
      fx_eq10_channel_processor = (AgsFxEq10ChannelProcessor *) g_object_new(AGS_TYPE_FX_EQ10_CHANNEL_PROCESSOR,
									     "output-soundcard", output_soundcard,
									     "source", channel,
									     "recall-audio", fx_eq10_audio,
									     "recall-audio-run", fx_eq10_audio_processor,
									     "recall-channel", fx_eq10_channel,
									     "recall-container", recall_container,
									     NULL);
      ags_recall_set_flags((AgsRecall *) fx_eq10_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_eq10_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_eq10_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_eq10_channel_processor,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_eq10_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_eq10_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_eq10_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_eq10_audio != NULL){
      g_object_unref(fx_eq10_audio);
    }
    
    if(fx_eq10_audio_processor != NULL){
      g_object_unref(fx_eq10_audio_processor);
    }
  }
  
  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(input_sequencer != NULL){
    g_object_unref(input_sequencer);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(start_recall);
}

GList*
ags_fx_factory_create_analyse(AgsAudio *audio,
			      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			      gchar *plugin_name,
			      gchar *filename,
			      gchar *effect,
			      guint start_audio_channel, guint stop_audio_channel,
			      guint start_pad, guint stop_pad,
			      gint position,
			      guint create_flags, guint recall_flags)
{
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;

  AgsFxAnalyseAudio *fx_analyse_audio;
  AgsFxAnalyseAudioProcessor *fx_analyse_audio_processor;
  AgsFxAnalyseChannel *fx_analyse_channel;
  AgsFxAnalyseChannelProcessor *fx_analyse_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  gint channel_position;
  guint audio_channels;
  guint i, j;
  
  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-analyse recall container not provided");
    
    return(NULL);
  }
  
  start_recall = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);

  channel_position = 0;
  
  if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
    channel_position = ags_fx_factory_get_output_position(audio,
							  position);
  }else{
    channel_position = ags_fx_factory_get_input_position(audio,
							 position);
  }

  /* audio - play context */
  fx_analyse_audio = NULL;
  fx_analyse_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxAnalyseAudio */
    fx_analyse_audio = (AgsFxAnalyseAudio *) g_object_new(AGS_TYPE_FX_ANALYSE_AUDIO,
							  "output-soundcard", output_soundcard,
							  "audio", audio,
							  "recall-container", play_container,
							  NULL);
    ags_recall_set_flags((AgsRecall *) fx_analyse_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_analyse_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_analyse_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_analyse_audio,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_analyse_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_analyse_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_analyse_audio));
    
    /* AgsFxAnalyseAudioProcessor */
    fx_analyse_audio_processor = (AgsFxAnalyseAudioProcessor *) g_object_new(AGS_TYPE_FX_ANALYSE_AUDIO_PROCESSOR,
									     "output-soundcard", output_soundcard,
									     "audio", audio,
									     "recall-audio", fx_analyse_audio,
									     "recall-container", play_container,
									     NULL);
    ags_recall_set_flags((AgsRecall *) fx_analyse_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_analyse_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_analyse_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_analyse_audio_processor,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_analyse_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_analyse_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_analyse_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_analyse_audio = ags_recall_container_get_recall_audio(play_container);

    if(fx_analyse_audio != NULL){
      g_object_ref(fx_analyse_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(play_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_analyse_audio_processor = recall_audio_run->data;
      g_object_ref(fx_analyse_audio_processor);
    }
  }
  
  /* channel - play context */
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxAnalyseChannel */
      fx_analyse_channel = (AgsFxAnalyseChannel *) g_object_new(AGS_TYPE_FX_ANALYSE_CHANNEL,
								"output-soundcard", output_soundcard,
								"source", channel,
								"recall-audio", fx_analyse_audio,
								"recall-container", play_container,
								NULL);
      ags_recall_set_flags((AgsRecall *) fx_analyse_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_analyse_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_analyse_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_analyse_channel,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_analyse_channel);
      
      start_recall = g_list_prepend(start_recall,
				    fx_analyse_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_analyse_channel));

      /* AgsFxAnalyseChannelProcessor */
      fx_analyse_channel_processor = (AgsFxAnalyseChannelProcessor *) g_object_new(AGS_TYPE_FX_ANALYSE_CHANNEL_PROCESSOR,
										   "output-soundcard", output_soundcard,
										   "source", channel,
										   "recall-audio", fx_analyse_audio,
										   "recall-audio-run", fx_analyse_audio_processor,
										   "recall-channel", fx_analyse_channel,
										   "recall-container", play_container,
										   NULL);
      ags_recall_set_flags((AgsRecall *) fx_analyse_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_analyse_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_analyse_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_analyse_channel_processor,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_analyse_channel_processor);
      
      start_recall = g_list_prepend(start_recall,
				    fx_analyse_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_analyse_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_analyse_audio != NULL){
      g_object_unref(fx_analyse_audio);
    }
    
    if(fx_analyse_audio_processor != NULL){
      g_object_unref(fx_analyse_audio_processor);
    }
  }

  /* audio - recall context */
  fx_analyse_audio = NULL;
  fx_analyse_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxAnalyseAudio */
    fx_analyse_audio = (AgsFxAnalyseAudio *) g_object_new(AGS_TYPE_FX_ANALYSE_AUDIO,
							  "output-soundcard", output_soundcard,
							  "audio", audio,
							  "recall-container", recall_container,
							  NULL);
    ags_recall_set_flags((AgsRecall *) fx_analyse_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_analyse_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_analyse_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_analyse_audio,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_analyse_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_analyse_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_analyse_audio));
    
    /* AgsFxAnalyseAudioProcessor */
    fx_analyse_audio_processor = (AgsFxAnalyseAudioProcessor *) g_object_new(AGS_TYPE_FX_ANALYSE_AUDIO_PROCESSOR,
									     "output-soundcard", output_soundcard,
									     "audio", audio,
									     "recall-audio", fx_analyse_audio,
									     "recall-container", recall_container,
									     NULL);
    ags_recall_set_flags((AgsRecall *) fx_analyse_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_analyse_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_analyse_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_analyse_audio_processor,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_analyse_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_analyse_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_analyse_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_analyse_audio = ags_recall_container_get_recall_audio(recall_container);

    if(fx_analyse_audio != NULL){
      g_object_ref(fx_analyse_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(recall_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_analyse_audio_processor = recall_audio_run->data;
      g_object_ref(fx_analyse_audio_processor);
    }    
  }

  /* channel - recall context */
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxAnalyseChannel */
      fx_analyse_channel = (AgsFxAnalyseChannel *) g_object_new(AGS_TYPE_FX_ANALYSE_CHANNEL,
								"output-soundcard", output_soundcard,
								"source", channel,
								"recall-audio", fx_analyse_audio,
								"recall-container", recall_container,
								NULL);
      ags_recall_set_flags((AgsRecall *) fx_analyse_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_analyse_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_analyse_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_analyse_channel,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_analyse_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_analyse_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_analyse_channel));

      /* AgsFxAnalyseChannelProcessor */
      fx_analyse_channel_processor = (AgsFxAnalyseChannelProcessor *) g_object_new(AGS_TYPE_FX_ANALYSE_CHANNEL_PROCESSOR,
										   "output-soundcard", output_soundcard,
										   "source", channel,
										   "recall-audio", fx_analyse_audio,
										   "recall-audio-run", fx_analyse_audio_processor,
										   "recall-channel", fx_analyse_channel,
										   "recall-container", recall_container,
										   NULL);
      ags_recall_set_flags((AgsRecall *) fx_analyse_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_analyse_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_analyse_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_analyse_channel_processor,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_analyse_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_analyse_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_analyse_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_analyse_audio != NULL){
      g_object_unref(fx_analyse_audio);
    }
    
    if(fx_analyse_audio_processor != NULL){
      g_object_unref(fx_analyse_audio_processor);
    }
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(input_sequencer != NULL){
    g_object_unref(input_sequencer);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(start_recall);
}

GList*
ags_fx_factory_create_two_pass_aliase(AgsAudio *audio,
				      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				      gchar *plugin_name,
				      gchar *filename,
				      gchar *effect,
				      guint start_audio_channel, guint stop_audio_channel,
				      guint start_pad, guint stop_pad,
				      gint position,
				      guint create_flags, guint recall_flags)
{
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;

  AgsFxTwoPassAliaseAudio *fx_two_pass_aliase_audio;
  AgsFxTwoPassAliaseAudioProcessor *fx_two_pass_aliase_audio_processor;
  AgsFxTwoPassAliaseChannel *fx_two_pass_aliase_channel;
  AgsFxTwoPassAliaseChannelProcessor *fx_two_pass_aliase_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  gint channel_position;
  guint audio_channels;
  guint i, j;
  
  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-two-pass-aliase recall container not provided");
    
    return(NULL);
  }
  
  start_recall = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);

  channel_position = 0;
  
  if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
    channel_position = ags_fx_factory_get_output_position(audio,
							  position);
  }else{
    channel_position = ags_fx_factory_get_input_position(audio,
							 position);
  }
  
  /* audio - play context */
  fx_two_pass_aliase_audio = NULL;
  fx_two_pass_aliase_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxTwoPassAliaseAudio */
    fx_two_pass_aliase_audio = (AgsFxTwoPassAliaseAudio *) g_object_new(AGS_TYPE_FX_TWO_PASS_ALIASE_AUDIO,
									"output-soundcard", output_soundcard,
									"audio", audio,
									"recall-container", play_container,
									NULL);
    ags_recall_set_flags((AgsRecall *) fx_two_pass_aliase_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_two_pass_aliase_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_two_pass_aliase_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_two_pass_aliase_audio,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_two_pass_aliase_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_two_pass_aliase_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_two_pass_aliase_audio));
    
    /* AgsFxTwoPassAliaseAudioProcessor */
    fx_two_pass_aliase_audio_processor = (AgsFxTwoPassAliaseAudioProcessor *) g_object_new(AGS_TYPE_FX_TWO_PASS_ALIASE_AUDIO_PROCESSOR,
											   "output-soundcard", output_soundcard,
											   "audio", audio,
											   "recall-audio", fx_two_pass_aliase_audio,
											   "recall-container", play_container,
											   NULL);
    ags_recall_set_flags((AgsRecall *) fx_two_pass_aliase_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_two_pass_aliase_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_two_pass_aliase_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_two_pass_aliase_audio_processor,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_two_pass_aliase_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_two_pass_aliase_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_two_pass_aliase_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_two_pass_aliase_audio = ags_recall_container_get_recall_audio(play_container);

    if(fx_two_pass_aliase_audio != NULL){
      g_object_ref(fx_two_pass_aliase_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(play_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_two_pass_aliase_audio_processor = recall_audio_run->data;
      g_object_ref(fx_two_pass_aliase_audio_processor);
    }
  }
  
  /* channel - play context */
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxTwoPassAliaseChannel */
      fx_two_pass_aliase_channel = (AgsFxTwoPassAliaseChannel *) g_object_new(AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL,
									      "output-soundcard", output_soundcard,
									      "source", channel,
									      "recall-audio", fx_two_pass_aliase_audio,
									      "recall-container", play_container,
									      NULL);
      ags_recall_set_flags((AgsRecall *) fx_two_pass_aliase_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_two_pass_aliase_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_two_pass_aliase_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_two_pass_aliase_channel,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_two_pass_aliase_channel);
      
      start_recall = g_list_prepend(start_recall,
				    fx_two_pass_aliase_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_two_pass_aliase_channel));

      /* AgsFxTwoPassAliaseChannelProcessor */
      fx_two_pass_aliase_channel_processor = (AgsFxTwoPassAliaseChannelProcessor *) g_object_new(AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL_PROCESSOR,
												 "output-soundcard", output_soundcard,
												 "source", channel,
												 "recall-audio", fx_two_pass_aliase_audio,
												 "recall-audio-run", fx_two_pass_aliase_audio_processor,
												 "recall-channel", fx_two_pass_aliase_channel,
												 "recall-container", play_container,
												 NULL);
      ags_recall_set_flags((AgsRecall *) fx_two_pass_aliase_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_two_pass_aliase_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_two_pass_aliase_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_two_pass_aliase_channel_processor,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_two_pass_aliase_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_two_pass_aliase_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_two_pass_aliase_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_two_pass_aliase_audio != NULL){
      g_object_unref(fx_two_pass_aliase_audio);
    }
    
    if(fx_two_pass_aliase_audio_processor != NULL){
      g_object_unref(fx_two_pass_aliase_audio_processor);
    }
  }
  
  /* audio - recall context */
  fx_two_pass_aliase_audio = NULL;
  fx_two_pass_aliase_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxTwoPassAliaseAudio */
    fx_two_pass_aliase_audio = (AgsFxTwoPassAliaseAudio *) g_object_new(AGS_TYPE_FX_TWO_PASS_ALIASE_AUDIO,
									"output-soundcard", output_soundcard,
									"audio", audio,
									"recall-container", play_container,
									NULL);
    ags_recall_set_flags((AgsRecall *) fx_two_pass_aliase_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_two_pass_aliase_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_two_pass_aliase_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_two_pass_aliase_audio,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_two_pass_aliase_audio);
    
    start_recall = g_list_prepend(start_recall,
				  fx_two_pass_aliase_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_two_pass_aliase_audio));
    
    /* AgsFxTwoPassAliaseAudioProcessor */
    fx_two_pass_aliase_audio_processor = (AgsFxTwoPassAliaseAudioProcessor *) g_object_new(AGS_TYPE_FX_TWO_PASS_ALIASE_AUDIO_PROCESSOR,
											   "output-soundcard", output_soundcard,
											   "audio", audio,
											   "recall-audio", fx_two_pass_aliase_audio,
											   "recall-container", play_container,
											   NULL);
    ags_recall_set_flags((AgsRecall *) fx_two_pass_aliase_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_two_pass_aliase_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_two_pass_aliase_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_two_pass_aliase_audio_processor,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_two_pass_aliase_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_two_pass_aliase_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_two_pass_aliase_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_two_pass_aliase_audio = ags_recall_container_get_recall_audio(recall_container);

    if(fx_two_pass_aliase_audio != NULL){
      g_object_ref(fx_two_pass_aliase_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(recall_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_two_pass_aliase_audio_processor = recall_audio_run->data;
      g_object_ref(fx_two_pass_aliase_audio_processor);
    }
  }
  
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxTwoPassAliaseChannel */
      fx_two_pass_aliase_channel = (AgsFxTwoPassAliaseChannel *) g_object_new(AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL,
									      "output-soundcard", output_soundcard,
									      "source", channel,
									      "recall-audio", fx_two_pass_aliase_audio,
									      "recall-container", recall_container,
									      NULL);
      ags_recall_set_flags((AgsRecall *) fx_two_pass_aliase_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_two_pass_aliase_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_two_pass_aliase_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_two_pass_aliase_channel,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_two_pass_aliase_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_two_pass_aliase_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_two_pass_aliase_channel));

      /* AgsFxTwoPassAliaseChannelProcessor */
      fx_two_pass_aliase_channel_processor = (AgsFxTwoPassAliaseChannelProcessor *) g_object_new(AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL_PROCESSOR,
												 "output-soundcard", output_soundcard,
												 "source", channel,
												 "recall-audio", fx_two_pass_aliase_audio,
												 "recall-audio-run", fx_two_pass_aliase_audio_processor,
												 "recall-channel", fx_two_pass_aliase_channel,
												 "recall-container", recall_container,
												 NULL);
      ags_recall_set_flags((AgsRecall *) fx_two_pass_aliase_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_two_pass_aliase_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_two_pass_aliase_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_two_pass_aliase_channel_processor,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_two_pass_aliase_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_two_pass_aliase_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_two_pass_aliase_channel_processor));

      /* iterate */
      next_channel = ags_channel_nth(channel,
				     audio_channels - stop_audio_channel + 1);

      if(channel != NULL){
	g_object_unref(channel);
      }
      
      channel = next_channel;
    }
  }  

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_two_pass_aliase_audio != NULL){
      g_object_unref(fx_two_pass_aliase_audio);
    }
    
    if(fx_two_pass_aliase_audio_processor != NULL){
      g_object_unref(fx_two_pass_aliase_audio_processor);
    }
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(input_sequencer != NULL){
    g_object_unref(input_sequencer);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(start_recall);
}

GList*
ags_fx_factory_create_lfo(AgsAudio *audio,
			  AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			  gchar *plugin_name,
			  gchar *filename,
			  gchar *effect,
			  guint start_audio_channel, guint stop_audio_channel,
			  guint start_pad, guint stop_pad,
			  gint position,
			  guint create_flags, guint recall_flags)
{
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;

  AgsFxLfoAudio *fx_lfo_audio;
  AgsFxLfoAudioProcessor *fx_lfo_audio_processor;
  AgsFxLfoChannel *fx_lfo_channel;
  AgsFxLfoChannelProcessor *fx_lfo_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  gint channel_position;
  guint audio_channels;
  guint i, j;
  
  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-lfo recall container not provided");
    
    return(NULL);
  }
  
  start_recall = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);

  channel_position = 0;
  
  if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
    channel_position = ags_fx_factory_get_output_position(audio,
							  position);
  }else{
    channel_position = ags_fx_factory_get_input_position(audio,
							 position);
  }
  
  /* audio - play context */
  fx_lfo_audio = NULL;
  fx_lfo_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxLfoAudio */
    fx_lfo_audio = (AgsFxLfoAudio *) g_object_new(AGS_TYPE_FX_LFO_AUDIO,
						  "output-soundcard", output_soundcard,
						  "audio", audio,
						  "recall-container", play_container,
						  NULL);
    ags_recall_set_flags((AgsRecall *) fx_lfo_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_lfo_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_lfo_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_lfo_audio,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_lfo_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_lfo_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_lfo_audio));
    
    /* AgsFxLfoAudioProcessor */
    fx_lfo_audio_processor = (AgsFxLfoAudioProcessor *) g_object_new(AGS_TYPE_FX_LFO_AUDIO_PROCESSOR,
								     "output-soundcard", output_soundcard,
								     "audio", audio,
								     "recall-audio", fx_lfo_audio,
								     "recall-container", play_container,
								     NULL);
    ags_recall_set_flags((AgsRecall *) fx_lfo_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_lfo_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_lfo_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_lfo_audio_processor,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_lfo_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_lfo_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_lfo_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_lfo_audio = ags_recall_container_get_recall_audio(play_container);

    if(fx_lfo_audio != NULL){
      g_object_ref(fx_lfo_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(play_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_lfo_audio_processor = recall_audio_run->data;
      g_object_ref(fx_lfo_audio_processor);
    }
  }
  
  /* channel - play context */
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxLfoChannel */
      fx_lfo_channel = (AgsFxLfoChannel *) g_object_new(AGS_TYPE_FX_LFO_CHANNEL,
							"output-soundcard", output_soundcard,
							"source", channel,
							"recall-audio", fx_lfo_audio,
							"recall-container", play_container,
							NULL);
      ags_recall_set_flags((AgsRecall *) fx_lfo_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_lfo_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_lfo_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_lfo_channel,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_lfo_channel);
      
      start_recall = g_list_prepend(start_recall,
				    fx_lfo_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_lfo_channel));

      /* AgsFxLfoChannelProcessor */
      fx_lfo_channel_processor = (AgsFxLfoChannelProcessor *) g_object_new(AGS_TYPE_FX_LFO_CHANNEL_PROCESSOR,
									   "output-soundcard", output_soundcard,
									   "source", channel,
									   "recall-audio", fx_lfo_audio,
									   "recall-audio-run", fx_lfo_audio_processor,
									   "recall-channel", fx_lfo_channel,
									   "recall-container", play_container,
									   NULL);
      ags_recall_set_flags((AgsRecall *) fx_lfo_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_lfo_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_lfo_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_lfo_channel_processor,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_lfo_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_lfo_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_lfo_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_lfo_audio != NULL){
      g_object_unref(fx_lfo_audio);
    }
    
    if(fx_lfo_audio_processor != NULL){
      g_object_unref(fx_lfo_audio_processor);
    }
  }
  
  /* audio - recall context */
  fx_lfo_audio = NULL;
  fx_lfo_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxLfoAudio */
    fx_lfo_audio = (AgsFxLfoAudio *) g_object_new(AGS_TYPE_FX_LFO_AUDIO,
						  "output-soundcard", output_soundcard,
						  "audio", audio,
						  "recall-container", play_container,
						  NULL);
    ags_recall_set_flags((AgsRecall *) fx_lfo_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_lfo_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_lfo_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_lfo_audio,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_lfo_audio);
    
    start_recall = g_list_prepend(start_recall,
				  fx_lfo_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_lfo_audio));
    
    /* AgsFxLfoAudioProcessor */
    fx_lfo_audio_processor = (AgsFxLfoAudioProcessor *) g_object_new(AGS_TYPE_FX_LFO_AUDIO_PROCESSOR,
								     "output-soundcard", output_soundcard,
								     "audio", audio,
								     "recall-audio", fx_lfo_audio,
								     "recall-container", play_container,
								     NULL);
    ags_recall_set_flags((AgsRecall *) fx_lfo_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_lfo_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_lfo_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_lfo_audio_processor,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_lfo_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_lfo_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_lfo_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_lfo_audio = ags_recall_container_get_recall_audio(recall_container);

    if(fx_lfo_audio != NULL){
      g_object_ref(fx_lfo_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(recall_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_lfo_audio_processor = recall_audio_run->data;
      g_object_ref(fx_lfo_audio_processor);
    }
  }
  
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxLfoChannel */
      fx_lfo_channel = (AgsFxLfoChannel *) g_object_new(AGS_TYPE_FX_LFO_CHANNEL,
							"output-soundcard", output_soundcard,
							"source", channel,
							"recall-audio", fx_lfo_audio,
							"recall-container", recall_container,
							NULL);
      ags_recall_set_flags((AgsRecall *) fx_lfo_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_lfo_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_lfo_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_lfo_channel,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_lfo_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_lfo_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_lfo_channel));

      /* AgsFxLfoChannelProcessor */
      fx_lfo_channel_processor = (AgsFxLfoChannelProcessor *) g_object_new(AGS_TYPE_FX_LFO_CHANNEL_PROCESSOR,
									   "output-soundcard", output_soundcard,
									   "source", channel,
									   "recall-audio", fx_lfo_audio,
									   "recall-audio-run", fx_lfo_audio_processor,
									   "recall-channel", fx_lfo_channel,
									   "recall-container", recall_container,
									   NULL);
      ags_recall_set_flags((AgsRecall *) fx_lfo_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_lfo_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_lfo_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_lfo_channel_processor,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_lfo_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_lfo_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_lfo_channel_processor));

      /* iterate */
      next_channel = ags_channel_nth(channel,
				     audio_channels - stop_audio_channel + 1);

      if(channel != NULL){
	g_object_unref(channel);
      }
      
      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }  

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_lfo_audio != NULL){
      g_object_unref(fx_lfo_audio);
    }
    
    if(fx_lfo_audio_processor != NULL){
      g_object_unref(fx_lfo_audio_processor);
    }
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(input_sequencer != NULL){
    g_object_unref(input_sequencer);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(start_recall);
}

GList*
ags_fx_factory_create_envelope(AgsAudio *audio,
			       AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			       gchar *plugin_name,
			       gchar *filename,
			       gchar *effect,
			       guint start_audio_channel, guint stop_audio_channel,
			       guint start_pad, guint stop_pad,
			       gint position,
			       guint create_flags, guint recall_flags)
{
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;

  AgsFxEnvelopeAudio *fx_envelope_audio;
  AgsFxEnvelopeAudioProcessor *fx_envelope_audio_processor;
  AgsFxEnvelopeChannel *fx_envelope_channel;
  AgsFxEnvelopeChannelProcessor *fx_envelope_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  gint channel_position;
  guint audio_channels;
  guint i, j;
  
  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-envelope recall container not provided");
    
    return(NULL);
  }
  
  start_recall = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);
  
  channel_position = 0;
  
  if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
    channel_position = ags_fx_factory_get_output_position(audio,
							  position);
  }else{
    channel_position = ags_fx_factory_get_input_position(audio,
							 position);
  }

  /* audio - play context */
  fx_envelope_audio = NULL;
  fx_envelope_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxEnvelopeAudio */
    fx_envelope_audio = (AgsFxEnvelopeAudio *) g_object_new(AGS_TYPE_FX_ENVELOPE_AUDIO,
							    "output-soundcard", output_soundcard,
							    "audio", audio,
							    "recall-container", play_container,
							    NULL);
    ags_recall_set_flags((AgsRecall *) fx_envelope_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_envelope_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_envelope_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_envelope_audio,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_envelope_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_envelope_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_envelope_audio));
    
    /* AgsFxEnvelopeAudioProcessor */
    fx_envelope_audio_processor = (AgsFxEnvelopeAudioProcessor *) g_object_new(AGS_TYPE_FX_ENVELOPE_AUDIO_PROCESSOR,
									       "output-soundcard", output_soundcard,
									       "audio", audio,
									       "recall-audio", fx_envelope_audio,
									       "recall-container", play_container,
									       NULL);
    ags_recall_set_flags((AgsRecall *) fx_envelope_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_envelope_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_envelope_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_envelope_audio_processor,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_envelope_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_envelope_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_envelope_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_envelope_audio = ags_recall_container_get_recall_audio(play_container);

    if(fx_envelope_audio != NULL){
      g_object_ref(fx_envelope_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(play_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_envelope_audio_processor = recall_audio_run->data;
      g_object_ref(fx_envelope_audio_processor);
    }
  }

  /* channel - play context */
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxEnvelopeChannel */
      fx_envelope_channel = (AgsFxEnvelopeChannel *) g_object_new(AGS_TYPE_FX_ENVELOPE_CHANNEL,
								  "output-soundcard", output_soundcard,
								  "source", channel,
								  "recall-audio", fx_envelope_audio,
								  "recall-container", play_container,
								  NULL);
      ags_recall_set_flags((AgsRecall *) fx_envelope_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_envelope_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_envelope_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_envelope_channel,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_envelope_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_envelope_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_envelope_channel));

      /* AgsFxEnvelopeChannelProcessor */
      fx_envelope_channel_processor = (AgsFxEnvelopeChannelProcessor *) g_object_new(AGS_TYPE_FX_ENVELOPE_CHANNEL_PROCESSOR,
										     "output-soundcard", output_soundcard,
										     "source", channel,
										     "recall-audio", fx_envelope_audio,
										     "recall-audio-run", fx_envelope_audio_processor,
										     "recall-channel", fx_envelope_channel,
										     "recall-container", play_container,
										     NULL);
      ags_recall_set_flags((AgsRecall *) fx_envelope_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_envelope_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_envelope_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_envelope_channel_processor,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_envelope_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_envelope_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_envelope_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_envelope_audio != NULL){
      g_object_unref(fx_envelope_audio);
    }
    
    if(fx_envelope_audio_processor != NULL){
      g_object_unref(fx_envelope_audio_processor);
    }
  }

  /* audio - recall context */
  fx_envelope_audio = NULL;
  fx_envelope_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxEnvelopeAudio */
    fx_envelope_audio = (AgsFxEnvelopeAudio *) g_object_new(AGS_TYPE_FX_ENVELOPE_AUDIO,
							    "output-soundcard", output_soundcard,
							    "audio", audio,
							    "recall-container", recall_container,
							    NULL);
    ags_recall_set_flags((AgsRecall *) fx_envelope_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_envelope_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_envelope_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_envelope_audio,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_envelope_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_envelope_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_envelope_audio));
    
    /* AgsFxEnvelopeAudioProcessor */
    fx_envelope_audio_processor = (AgsFxEnvelopeAudioProcessor *) g_object_new(AGS_TYPE_FX_ENVELOPE_AUDIO_PROCESSOR,
									       "output-soundcard", output_soundcard,
									       "audio", audio,
									       "recall-audio", fx_envelope_audio,
									       "recall-container", recall_container,
									       NULL);
    ags_recall_set_flags((AgsRecall *) fx_envelope_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_envelope_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_envelope_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_envelope_audio_processor,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_envelope_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_envelope_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_envelope_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_envelope_audio = ags_recall_container_get_recall_audio(recall_container);

    if(fx_envelope_audio != NULL){
      g_object_ref(fx_envelope_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(recall_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_envelope_audio_processor = recall_audio_run->data;
      g_object_ref(fx_envelope_audio_processor);
    }
  }

  /* channel - recall context */
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxEnvelopeChannel */
      fx_envelope_channel = (AgsFxEnvelopeChannel *) g_object_new(AGS_TYPE_FX_ENVELOPE_CHANNEL,
								  "output-soundcard", output_soundcard,
								  "source", channel,
								  "recall-audio", fx_envelope_audio,
								  "recall-container", recall_container,
								  NULL);
      ags_recall_set_flags((AgsRecall *) fx_envelope_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_envelope_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_envelope_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_envelope_channel,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_envelope_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_envelope_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_envelope_channel));

      /* AgsFxEnvelopeChannelProcessor */
      fx_envelope_channel_processor = (AgsFxEnvelopeChannelProcessor *) g_object_new(AGS_TYPE_FX_ENVELOPE_CHANNEL_PROCESSOR,
										     "output-soundcard", output_soundcard,
										     "source", channel,
										     "recall-audio", fx_envelope_audio,
										     "recall-audio-run", fx_envelope_audio_processor,
										     "recall-channel", fx_envelope_channel,
										     "recall-container", recall_container,
										     NULL);
      ags_recall_set_flags((AgsRecall *) fx_envelope_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_envelope_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_envelope_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_envelope_channel_processor,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_envelope_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_envelope_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_envelope_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_envelope_audio != NULL){
      g_object_unref(fx_envelope_audio);
    }
    
    if(fx_envelope_audio_processor != NULL){
      g_object_unref(fx_envelope_audio_processor);
    }
  }
  
  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(input_sequencer != NULL){
    g_object_unref(input_sequencer);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(start_recall);
}

GList*
ags_fx_factory_create_pattern(AgsAudio *audio,
			      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			      gchar *plugin_name,
			      gchar *filename,
			      gchar *effect,
			      guint start_audio_channel, guint stop_audio_channel,
			      guint start_pad, guint stop_pad,
			      gint position,
			      guint create_flags, guint recall_flags)
{
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;

  AgsFxPatternAudio *fx_pattern_audio;
  AgsFxPatternAudioProcessor *fx_pattern_audio_processor;
  AgsFxPatternChannel *fx_pattern_channel;
  AgsFxPatternChannelProcessor *fx_pattern_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  gint channel_position;
  guint audio_channels;
  guint i, j;
  
  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-pattern recall container not provided");
    
    return(NULL);
  }
    
  start_recall = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);
  
  channel_position = 0;
  
  if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
    channel_position = ags_fx_factory_get_output_position(audio,
							  position);
  }else{
    channel_position = ags_fx_factory_get_input_position(audio,
							 position);
  }

  /* audio - play context */
  fx_pattern_audio = NULL;
  fx_pattern_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxPatternAudio */
    fx_pattern_audio = (AgsFxPatternAudio *) g_object_new(AGS_TYPE_FX_PATTERN_AUDIO,
							  "output-soundcard", output_soundcard,
							  "audio", audio,
							  "recall-container", play_container,
							  NULL);
    ags_recall_set_flags((AgsRecall *) fx_pattern_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_pattern_audio,
				 (AGS_SOUND_ABILITY_SEQUENCER));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_pattern_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_pattern_audio,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_pattern_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_pattern_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_pattern_audio));

    /* AgsFxPatternAudioProcessor */
    fx_pattern_audio_processor = (AgsFxPatternAudioProcessor *) g_object_new(AGS_TYPE_FX_PATTERN_AUDIO_PROCESSOR,
									     "output-soundcard", output_soundcard,
									     "audio", audio,
									     "recall-audio", fx_pattern_audio,
									     "recall-container", play_container,
									     NULL);
    ags_recall_set_flags((AgsRecall *) fx_pattern_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_pattern_audio_processor,
				 (AGS_SOUND_ABILITY_SEQUENCER));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_pattern_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_pattern_audio_processor,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_pattern_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_pattern_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_pattern_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_pattern_audio = ags_recall_container_get_recall_audio(play_container);

    if(fx_pattern_audio != NULL){
      g_object_ref(fx_pattern_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(play_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_pattern_audio_processor = recall_audio_run->data;
      g_object_ref(fx_pattern_audio_processor);
    }
  }
  
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      ags_channel_add_recall_container(channel,
				       (GObject *) recall_container);
	
      /* AgsFxPatternChannel */
      fx_pattern_channel = (AgsFxPatternChannel *) g_object_new(AGS_TYPE_FX_PATTERN_CHANNEL,
								"output-soundcard", output_soundcard,
								"source", channel,
								"recall-audio", fx_pattern_audio,
								"recall-container", play_container,
								NULL);
      ags_recall_set_flags((AgsRecall *) fx_pattern_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_pattern_channel,
				   (AGS_SOUND_ABILITY_SEQUENCER));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_pattern_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_pattern_channel,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_pattern_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_pattern_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_pattern_channel));

      /* AgsFxPatternChannelProcessor */
      fx_pattern_channel_processor = (AgsFxPatternChannelProcessor *) g_object_new(AGS_TYPE_FX_PATTERN_CHANNEL_PROCESSOR,
										   "output-soundcard", output_soundcard,
										   "source", channel,
										   "recall-audio", fx_pattern_audio,
										   "recall-audio-run", fx_pattern_audio_processor,
										   "recall-channel", fx_pattern_channel,
										   "recall-container", play_container,
										   NULL);
      ags_recall_set_flags((AgsRecall *) fx_pattern_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_pattern_channel_processor,
				   (AGS_SOUND_ABILITY_SEQUENCER));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_pattern_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_pattern_channel_processor,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_pattern_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_pattern_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_pattern_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_pattern_audio != NULL){
      g_object_unref(fx_pattern_audio);
    }
    
    if(fx_pattern_audio_processor != NULL){
      g_object_unref(fx_pattern_audio_processor);
    }
  }

  /* audio - recall context */
  fx_pattern_audio = NULL;
  fx_pattern_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxPatternAudio */
    fx_pattern_audio = (AgsFxPatternAudio *) g_object_new(AGS_TYPE_FX_PATTERN_AUDIO,
							  "output-soundcard", output_soundcard,
							  "audio", audio,
							  "recall-container", recall_container,
							  NULL);
    ags_recall_set_flags((AgsRecall *) fx_pattern_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_pattern_audio,
				 (AGS_SOUND_ABILITY_SEQUENCER));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_pattern_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_pattern_audio,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_pattern_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_pattern_audio);
    ags_connectable_connect(AGS_CONNECTABLE(fx_pattern_audio));

    /* AgsFxPatternAudioProcessor */
    fx_pattern_audio_processor = (AgsFxPatternAudioProcessor *) g_object_new(AGS_TYPE_FX_PATTERN_AUDIO_PROCESSOR,
									     "output-soundcard", output_soundcard,
									     "audio", audio,
									     "recall-audio", fx_pattern_audio,
									     "recall-container", recall_container,
									     NULL);
    ags_recall_set_flags((AgsRecall *) fx_pattern_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_pattern_audio_processor,
				 (AGS_SOUND_ABILITY_SEQUENCER));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_pattern_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_pattern_audio_processor,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_pattern_audio_processor);
    
    start_recall = g_list_prepend(start_recall,
				  fx_pattern_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_pattern_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_pattern_audio = ags_recall_container_get_recall_audio(recall_container);

    if(fx_pattern_audio != NULL){
      g_object_ref(fx_pattern_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(recall_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_pattern_audio_processor = recall_audio_run->data;
      g_object_ref(fx_pattern_audio_processor);
    }
  }

  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      ags_channel_add_recall_container(channel,
				       (GObject *) recall_container);
	
      /* AgsFxPatternChannel */
      fx_pattern_channel = (AgsFxPatternChannel *) g_object_new(AGS_TYPE_FX_PATTERN_CHANNEL,
								"output-soundcard", output_soundcard,
								"source", channel,
								"recall-audio", fx_pattern_audio,
								"recall-container", recall_container,
								NULL);
      ags_recall_set_flags((AgsRecall *) fx_pattern_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_pattern_channel,
				   (AGS_SOUND_ABILITY_SEQUENCER));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_pattern_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_pattern_channel,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_pattern_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_pattern_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_pattern_channel));

      /* AgsFxPatternChannelProcessor */
      fx_pattern_channel_processor = (AgsFxPatternChannelProcessor *) g_object_new(AGS_TYPE_FX_PATTERN_CHANNEL_PROCESSOR,
										   "output-soundcard", output_soundcard,
										   "source", channel,
										   "recall-audio", fx_pattern_audio,
										   "recall-audio-run", fx_pattern_audio_processor,
										   "recall-channel", fx_pattern_channel,
										   "recall-container", recall_container,
										   NULL);
      ags_recall_set_flags((AgsRecall *) fx_pattern_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_pattern_channel_processor,
				   (AGS_SOUND_ABILITY_SEQUENCER));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_pattern_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_pattern_channel_processor,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_pattern_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_pattern_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_pattern_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_pattern_audio != NULL){
      g_object_unref(fx_pattern_audio);
    }
    
    if(fx_pattern_audio_processor != NULL){
      g_object_unref(fx_pattern_audio_processor);
    }
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(input_sequencer != NULL){
    g_object_unref(input_sequencer);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(start_recall);
}

GList*
ags_fx_factory_create_notation(AgsAudio *audio,
			       AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			       gchar *plugin_name,
			       gchar *filename,
			       gchar *effect,
			       guint start_audio_channel, guint stop_audio_channel,
			       guint start_pad, guint stop_pad,
			       gint position,
			       guint create_flags, guint recall_flags)
{
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;

  AgsFxNotationAudio *fx_notation_audio;
  AgsFxNotationAudioProcessor *fx_notation_audio_processor;
  AgsFxNotationChannel *fx_notation_channel;
  AgsFxNotationChannelProcessor *fx_notation_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  gint channel_position;
  guint audio_channels;
  guint i, j;

  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-notation recall container not provided");
    
    return(NULL);
  }
  
  start_recall = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);

  channel_position = 0;
  
  if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
    channel_position = ags_fx_factory_get_output_position(audio,
							  position);
  }else{
    channel_position = ags_fx_factory_get_input_position(audio,
							 position);
  }

  /* audio - play context */
  fx_notation_audio = NULL;
  fx_notation_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxNotationAudio */
    fx_notation_audio = (AgsFxNotationAudio *) g_object_new(AGS_TYPE_FX_NOTATION_AUDIO,
							    "output-soundcard", output_soundcard,
							    "audio", audio,
							    "recall-container", play_container,
							    NULL);
    ags_recall_set_flags((AgsRecall *) fx_notation_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_notation_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_notation_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_notation_audio,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_notation_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_notation_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_notation_audio));

    /* AgsFxNotationAudioProcessor */
    fx_notation_audio_processor = (AgsFxNotationAudioProcessor *) g_object_new(AGS_TYPE_FX_NOTATION_AUDIO_PROCESSOR,
									       "output-soundcard", output_soundcard,
									       "audio", audio,
									       "recall-audio", fx_notation_audio,
									       "recall-container", play_container,
									       NULL);
    ags_recall_set_flags((AgsRecall *) fx_notation_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_notation_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_notation_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_notation_audio_processor,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_notation_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_notation_audio_processor);
    ags_connectable_connect(AGS_CONNECTABLE(fx_notation_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_notation_audio = ags_recall_container_get_recall_audio(play_container);

    if(fx_notation_audio != NULL){
      g_object_ref(fx_notation_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(play_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_notation_audio_processor = recall_audio_run->data;
      g_object_ref(fx_notation_audio_processor);
    }
  }

  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){      
      /* add recall container */
      ags_channel_add_recall_container(channel,
				       (GObject *) recall_container);
	
      /* AgsFxNotationChannel */
      fx_notation_channel = (AgsFxNotationChannel *) g_object_new(AGS_TYPE_FX_NOTATION_CHANNEL,
								  "output-soundcard", output_soundcard,
								  "source", channel,
								  "recall-audio", fx_notation_audio,
								  "recall-container", play_container,
								  NULL);
      ags_recall_set_flags((AgsRecall *) fx_notation_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_notation_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_notation_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_notation_channel,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_notation_channel);
      
      start_recall = g_list_prepend(start_recall,
				    fx_notation_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_notation_channel));

      /* AgsFxNotationChannelProcessor */
      fx_notation_channel_processor = (AgsFxNotationChannelProcessor *) g_object_new(AGS_TYPE_FX_NOTATION_CHANNEL_PROCESSOR,
										     "output-soundcard", output_soundcard,
										     "source", channel,
										     "recall-audio", fx_notation_audio,
										     "recall-audio-run", fx_notation_audio_processor,
										     "recall-channel", fx_notation_channel,
										     "recall-container", play_container,
										     NULL);
      ags_recall_set_flags((AgsRecall *) fx_notation_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_notation_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_notation_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_notation_channel_processor,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_notation_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_notation_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_notation_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_notation_audio != NULL){
      g_object_unref(fx_notation_audio);
    }
    
    if(fx_notation_audio_processor != NULL){
      g_object_unref(fx_notation_audio_processor);
    }
  }
  
  /* audio - recall context */
  fx_notation_audio = NULL;
  fx_notation_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){  
    /* AgsFxNotationAudio */
    fx_notation_audio = (AgsFxNotationAudio *) g_object_new(AGS_TYPE_FX_NOTATION_AUDIO,
							    "output-soundcard", output_soundcard,
							    "audio", audio,
							    "recall-container", recall_container,
							    NULL);
    ags_recall_set_flags((AgsRecall *) fx_notation_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_notation_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_notation_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_notation_audio,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_notation_audio);
    
    start_recall = g_list_prepend(start_recall,
				  fx_notation_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_notation_audio));

    /* AgsFxNotationAudioProcessor */
    fx_notation_audio_processor = (AgsFxNotationAudioProcessor *) g_object_new(AGS_TYPE_FX_NOTATION_AUDIO_PROCESSOR,
									       "output-soundcard", output_soundcard,
									       "audio", audio,
									       "recall-audio", fx_notation_audio,
									       "recall-container", recall_container,
									       NULL);
    ags_recall_set_flags((AgsRecall *) fx_notation_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_notation_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_notation_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_notation_audio_processor,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_notation_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_notation_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_notation_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_notation_audio = ags_recall_container_get_recall_audio(recall_container);

    if(fx_notation_audio != NULL){
      g_object_ref(fx_notation_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(recall_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_notation_audio_processor = recall_audio_run->data;
      g_object_ref(fx_notation_audio_processor);
    }
  }
  
  for(i = 0; i < stop_pad - start_pad; i++){
    channel = ags_channel_pad_nth(start_input,
				  start_pad + i);

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){      
      /* add recall container */
      ags_channel_add_recall_container(channel,
				       (GObject *) recall_container);
	
      /* AgsFxNotationChannel */
      fx_notation_channel = (AgsFxNotationChannel *) g_object_new(AGS_TYPE_FX_NOTATION_CHANNEL,
								  "output-soundcard", output_soundcard,
								  "source", channel,
								  "recall-audio", fx_notation_audio,
								  "recall-container", recall_container,
								  NULL);
      ags_recall_set_flags((AgsRecall *) fx_notation_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_notation_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_notation_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_notation_channel,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_notation_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_notation_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_notation_channel));

      /* AgsFxNotationChannelProcessor */
      fx_notation_channel_processor = (AgsFxNotationChannelProcessor *) g_object_new(AGS_TYPE_FX_NOTATION_CHANNEL_PROCESSOR,
										     "output-soundcard", output_soundcard,
										     "source", channel,
										     "recall-audio", fx_notation_audio,
										     "recall-audio-run", fx_notation_audio_processor,
										     "recall-channel", fx_notation_channel,
										     "recall-container", recall_container,
										     NULL);
      ags_recall_set_flags((AgsRecall *) fx_notation_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_notation_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_notation_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_notation_channel_processor,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_notation_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_notation_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_notation_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_notation_audio != NULL){
      g_object_unref(fx_notation_audio);
    }
    
    if(fx_notation_audio_processor != NULL){
      g_object_unref(fx_notation_audio_processor);
    }
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(input_sequencer != NULL){
    g_object_unref(input_sequencer);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(start_recall);
}

GList*
ags_fx_factory_create_synth(AgsAudio *audio,
			    AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			    gchar *plugin_name,
			    gchar *filename,
			    gchar *effect,
			    guint start_audio_channel, guint stop_audio_channel,
			    guint start_pad, guint stop_pad,
			    gint position,
			    guint create_flags, guint recall_flags)
{
  AgsChannel *start_input, *start_output;
  AgsChannel *channel, *next_channel;

  AgsFxSynthAudio *fx_synth_audio;
  AgsFxSynthAudioProcessor *fx_synth_audio_processor;
  AgsFxSynthChannel *fx_synth_channel;
  AgsFxSynthChannelProcessor *fx_synth_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  gint channel_position;
  guint audio_channels;
  guint i, j;
  
  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-synth recall container not provided");
    
    return(NULL);
  }
  
  start_recall = NULL;

  start_output = NULL;

  start_input = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "output", &start_output,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);

  channel_position = 0;
  
  if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
    channel_position = ags_fx_factory_get_output_position(audio,
							  position);
  }else{
    channel_position = ags_fx_factory_get_input_position(audio,
							 position);
  }

  /* audio - play context */
  fx_synth_audio = NULL;
  fx_synth_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxSynthAudio */
    fx_synth_audio = (AgsFxSynthAudio *) g_object_new(AGS_TYPE_FX_SYNTH_AUDIO,
						      "output-soundcard", output_soundcard,
						      "audio", audio,
						      "recall-container", play_container,
						      "filename", filename,
						      "effect", effect,						    
						      NULL);
    ags_recall_set_flags((AgsRecall *) fx_synth_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_synth_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_synth_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_synth_audio,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_synth_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_synth_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_synth_audio));
    
    /* AgsFxSynthAudioProcessor */
    fx_synth_audio_processor = (AgsFxSynthAudioProcessor *) g_object_new(AGS_TYPE_FX_SYNTH_AUDIO_PROCESSOR,
									 "output-soundcard", output_soundcard,
									 "audio", audio,
									 "recall-audio", fx_synth_audio,
									 "recall-container", play_container,
									 "filename", filename,
									 "effect", effect,						    
									 NULL);
    ags_recall_set_flags((AgsRecall *) fx_synth_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_synth_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_synth_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_synth_audio_processor,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_synth_audio_processor);
    
    start_recall = g_list_prepend(start_recall,
				  fx_synth_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_synth_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_synth_audio = ags_recall_container_get_recall_audio(play_container);

    if(fx_synth_audio != NULL){
      g_object_ref(fx_synth_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(play_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_synth_audio_processor = recall_audio_run->data;
      g_object_ref(fx_synth_audio_processor);
    }
  }

  /* channel - play context */
  for(i = 0; i < stop_pad - start_pad; i++){
    if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
      channel = ags_channel_pad_nth(start_output,
				    start_pad + i);
    }else{
      channel = ags_channel_pad_nth(start_input,
				    start_pad + i);
    }

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* add recall container */
      ags_channel_add_recall_container(channel,
				       (GObject *) recall_container);
	
      /* AgsFxSynthChannel */
      fx_synth_channel = (AgsFxSynthChannel *) g_object_new(AGS_TYPE_FX_SYNTH_CHANNEL,
							    "output-soundcard", output_soundcard,
							    "source", channel,
							    "recall-audio", fx_synth_audio,
							    "recall-container", play_container,
							    "filename", filename,
							    "effect", effect,						    
							    NULL);
      ags_recall_set_flags((AgsRecall *) fx_synth_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_synth_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_synth_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_synth_channel,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_synth_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_synth_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_synth_channel));

      /* AgsFxSynthChannelProcessor */
      fx_synth_channel_processor = (AgsFxSynthChannelProcessor *) g_object_new(AGS_TYPE_FX_SYNTH_CHANNEL_PROCESSOR,
									       "output-soundcard", output_soundcard,
									       "source", channel,
									       "recall-audio", fx_synth_audio,
									       "recall-audio-run", fx_synth_audio_processor,
									       "recall-channel", fx_synth_channel,
									       "recall-container", play_container,
									       "filename", filename,
									       "effect", effect,						    
									       NULL);
      ags_recall_set_flags((AgsRecall *) fx_synth_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_synth_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_synth_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_synth_channel_processor,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_synth_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_synth_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_synth_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_synth_audio != NULL){
      g_object_unref(fx_synth_audio);
    }
    
    if(fx_synth_audio_processor != NULL){
      g_object_unref(fx_synth_audio_processor);
    }
  }
  
  /* audio - recall context */
  fx_synth_audio = NULL;
  fx_synth_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxSynthAudio */
    fx_synth_audio = (AgsFxSynthAudio *) g_object_new(AGS_TYPE_FX_SYNTH_AUDIO,
						      "output-soundcard", output_soundcard,
						      "audio", audio,
						      "recall-container", recall_container,
						      "filename", filename,
						      "effect", effect,						    
						      NULL);
    ags_recall_set_flags((AgsRecall *) fx_synth_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_synth_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_synth_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_synth_audio,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_synth_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_synth_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_synth_audio));
    
    /* AgsFxSynthAudioProcessor */
    fx_synth_audio_processor = (AgsFxSynthAudioProcessor *) g_object_new(AGS_TYPE_FX_SYNTH_AUDIO_PROCESSOR,
									 "output-soundcard", output_soundcard,
									 "audio", audio,
									 "recall-audio", fx_synth_audio,
									 "recall-container", recall_container,
									 "filename", filename,
									 "effect", effect,						    
									 NULL);
    ags_recall_set_flags((AgsRecall *) fx_synth_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_synth_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_synth_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_synth_audio_processor,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_synth_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_synth_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_synth_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_synth_audio = ags_recall_container_get_recall_audio(recall_container);

    if(fx_synth_audio != NULL){
      g_object_ref(fx_synth_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(recall_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_synth_audio_processor = recall_audio_run->data;
      g_object_ref(fx_synth_audio_processor);
    }
  }

  /* channel - recall context */  
  for(i = 0; i < stop_pad - start_pad; i++){
    if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
      channel = ags_channel_pad_nth(start_output,
				    start_pad + i);
    }else{
      channel = ags_channel_pad_nth(start_input,
				    start_pad + i);
    }

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxSynthChannel */
      fx_synth_channel = (AgsFxSynthChannel *) g_object_new(AGS_TYPE_FX_SYNTH_CHANNEL,
							    "output-soundcard", output_soundcard,
							    "source", channel,
							    "recall-audio", fx_synth_audio,
							    "recall-container", recall_container,
							    "filename", filename,
							    "effect", effect,						    
							    NULL);
      ags_recall_set_flags((AgsRecall *) fx_synth_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_synth_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_synth_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_synth_channel,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_synth_channel);
      
      start_recall = g_list_prepend(start_recall,
				    fx_synth_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_synth_channel));

      /* AgsFxSynthChannelProcessor */
      fx_synth_channel_processor = (AgsFxSynthChannelProcessor *) g_object_new(AGS_TYPE_FX_SYNTH_CHANNEL_PROCESSOR,
									       "output-soundcard", output_soundcard,
									       "source", channel,
									       "recall-audio", fx_synth_audio,
									       "recall-audio-run", fx_synth_audio_processor,
									       "recall-channel", fx_synth_channel,
									       "recall-container", recall_container,
									       "filename", filename,
									       "effect", effect,						    
									       NULL);
      ags_recall_set_flags((AgsRecall *) fx_synth_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_synth_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_synth_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_synth_channel_processor,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_synth_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_synth_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_synth_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_synth_audio != NULL){
      g_object_unref(fx_synth_audio);
    }
    
    if(fx_synth_audio_processor != NULL){
      g_object_unref(fx_synth_audio_processor);
    }
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(input_sequencer != NULL){
    g_object_unref(input_sequencer);
  }
  
  if(start_output != NULL){
    g_object_unref(start_output);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(start_recall);
}

GList*
ags_fx_factory_create_fm_synth(AgsAudio *audio,
			       AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			       gchar *plugin_name,
			       gchar *filename,
			       gchar *effect,
			       guint start_audio_channel, guint stop_audio_channel,
			       guint start_pad, guint stop_pad,
			       gint position,
			       guint create_flags, guint recall_flags)
{
  AgsChannel *start_input, *start_output;
  AgsChannel *channel, *next_channel;

  AgsFxFMSynthAudio *fx_fm_synth_audio;
  AgsFxFMSynthAudioProcessor *fx_fm_synth_audio_processor;
  AgsFxFMSynthChannel *fx_fm_synth_channel;
  AgsFxFMSynthChannelProcessor *fx_fm_synth_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  gint channel_position;
  guint audio_channels;
  guint i, j;
  
  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-fm-synth recall container not provided");
    
    return(NULL);
  }
  
  start_recall = NULL;

  start_output = NULL;

  start_input = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "output", &start_output,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);

  channel_position = 0;
  
  if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
    channel_position = ags_fx_factory_get_output_position(audio,
							  position);
  }else{
    channel_position = ags_fx_factory_get_input_position(audio,
							 position);
  }

  /* audio - play context */
  fx_fm_synth_audio = NULL;
  fx_fm_synth_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxFMSynthAudio */
    fx_fm_synth_audio = (AgsFxFMSynthAudio *) g_object_new(AGS_TYPE_FX_FM_SYNTH_AUDIO,
							   "output-soundcard", output_soundcard,
							   "audio", audio,
							   "recall-container", play_container,
							   "filename", filename,
							   "effect", effect,						    
							   NULL);
    ags_recall_set_flags((AgsRecall *) fx_fm_synth_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_fm_synth_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_fm_synth_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_fm_synth_audio,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_fm_synth_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_fm_synth_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_fm_synth_audio));
    
    /* AgsFxFMSynthAudioProcessor */
    fx_fm_synth_audio_processor = (AgsFxFMSynthAudioProcessor *) g_object_new(AGS_TYPE_FX_FM_SYNTH_AUDIO_PROCESSOR,
									      "output-soundcard", output_soundcard,
									      "audio", audio,
									      "recall-audio", fx_fm_synth_audio,
									      "recall-container", play_container,
									      "filename", filename,
									      "effect", effect,						    
									      NULL);
    ags_recall_set_flags((AgsRecall *) fx_fm_synth_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_fm_synth_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_fm_synth_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_fm_synth_audio_processor,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_fm_synth_audio_processor);
    
    start_recall = g_list_prepend(start_recall,
				  fx_fm_synth_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_fm_synth_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_fm_synth_audio = ags_recall_container_get_recall_audio(play_container);

    if(fx_fm_synth_audio != NULL){
      g_object_ref(fx_fm_synth_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(play_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_fm_synth_audio_processor = recall_audio_run->data;
      g_object_ref(fx_fm_synth_audio_processor);
    }
  }

  /* channel - play context */
  for(i = 0; i < stop_pad - start_pad; i++){
    if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
      channel = ags_channel_pad_nth(start_output,
				    start_pad + i);
    }else{
      channel = ags_channel_pad_nth(start_input,
				    start_pad + i);
    }

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* add recall container */
      ags_channel_add_recall_container(channel,
				       (GObject *) recall_container);
	
      /* AgsFxFMSynthChannel */
      fx_fm_synth_channel = (AgsFxFMSynthChannel *) g_object_new(AGS_TYPE_FX_FM_SYNTH_CHANNEL,
								 "output-soundcard", output_soundcard,
								 "source", channel,
								 "recall-audio", fx_fm_synth_audio,
								 "recall-container", play_container,
								 "filename", filename,
								 "effect", effect,						    
								 NULL);
      ags_recall_set_flags((AgsRecall *) fx_fm_synth_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_fm_synth_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_fm_synth_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_fm_synth_channel,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_fm_synth_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_fm_synth_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_fm_synth_channel));

      /* AgsFxFMSynthChannelProcessor */
      fx_fm_synth_channel_processor = (AgsFxFMSynthChannelProcessor *) g_object_new(AGS_TYPE_FX_FM_SYNTH_CHANNEL_PROCESSOR,
										    "output-soundcard", output_soundcard,
										    "source", channel,
										    "recall-audio", fx_fm_synth_audio,
										    "recall-audio-run", fx_fm_synth_audio_processor,
										    "recall-channel", fx_fm_synth_channel,
										    "recall-container", play_container,
										    "filename", filename,
										    "effect", effect,						    
										    NULL);
      ags_recall_set_flags((AgsRecall *) fx_fm_synth_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_fm_synth_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_fm_synth_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_fm_synth_channel_processor,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_fm_synth_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_fm_synth_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_fm_synth_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_fm_synth_audio != NULL){
      g_object_unref(fx_fm_synth_audio);
    }
    
    if(fx_fm_synth_audio_processor != NULL){
      g_object_unref(fx_fm_synth_audio_processor);
    }
  }
  
  /* audio - recall context */
  fx_fm_synth_audio = NULL;
  fx_fm_synth_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxFMSynthAudio */
    fx_fm_synth_audio = (AgsFxFMSynthAudio *) g_object_new(AGS_TYPE_FX_FM_SYNTH_AUDIO,
							   "output-soundcard", output_soundcard,
							   "audio", audio,
							   "recall-container", recall_container,
							   "filename", filename,
							   "effect", effect,						    
							   NULL);
    ags_recall_set_flags((AgsRecall *) fx_fm_synth_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_fm_synth_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_fm_synth_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_fm_synth_audio,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_fm_synth_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_fm_synth_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_fm_synth_audio));
    
    /* AgsFxFMSynthAudioProcessor */
    fx_fm_synth_audio_processor = (AgsFxFMSynthAudioProcessor *) g_object_new(AGS_TYPE_FX_FM_SYNTH_AUDIO_PROCESSOR,
									      "output-soundcard", output_soundcard,
									      "audio", audio,
									      "recall-audio", fx_fm_synth_audio,
									      "recall-container", recall_container,
									      "filename", filename,
									      "effect", effect,						    
									      NULL);
    ags_recall_set_flags((AgsRecall *) fx_fm_synth_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_fm_synth_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_fm_synth_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_fm_synth_audio_processor,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_fm_synth_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_fm_synth_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_fm_synth_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_fm_synth_audio = ags_recall_container_get_recall_audio(recall_container);

    if(fx_fm_synth_audio != NULL){
      g_object_ref(fx_fm_synth_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(recall_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_fm_synth_audio_processor = recall_audio_run->data;
      g_object_ref(fx_fm_synth_audio_processor);
    }
  }

  /* channel - recall context */  
  for(i = 0; i < stop_pad - start_pad; i++){
    if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
      channel = ags_channel_pad_nth(start_output,
				    start_pad + i);
    }else{
      channel = ags_channel_pad_nth(start_input,
				    start_pad + i);
    }

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxFMSynthChannel */
      fx_fm_synth_channel = (AgsFxFMSynthChannel *) g_object_new(AGS_TYPE_FX_FM_SYNTH_CHANNEL,
								 "output-soundcard", output_soundcard,
								 "source", channel,
								 "recall-audio", fx_fm_synth_audio,
								 "recall-container", recall_container,
								 "filename", filename,
								 "effect", effect,						    
								 NULL);
      ags_recall_set_flags((AgsRecall *) fx_fm_synth_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_fm_synth_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_fm_synth_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_fm_synth_channel,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_fm_synth_channel);
      
      start_recall = g_list_prepend(start_recall,
				    fx_fm_synth_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_fm_synth_channel));

      /* AgsFxFMSynthChannelProcessor */
      fx_fm_synth_channel_processor = (AgsFxFMSynthChannelProcessor *) g_object_new(AGS_TYPE_FX_FM_SYNTH_CHANNEL_PROCESSOR,
										    "output-soundcard", output_soundcard,
										    "source", channel,
										    "recall-audio", fx_fm_synth_audio,
										    "recall-audio-run", fx_fm_synth_audio_processor,
										    "recall-channel", fx_fm_synth_channel,
										    "recall-container", recall_container,
										    "filename", filename,
										    "effect", effect,						    
										    NULL);
      ags_recall_set_flags((AgsRecall *) fx_fm_synth_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_fm_synth_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_fm_synth_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_fm_synth_channel_processor,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_fm_synth_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_fm_synth_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_fm_synth_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_fm_synth_audio != NULL){
      g_object_unref(fx_fm_synth_audio);
    }
    
    if(fx_fm_synth_audio_processor != NULL){
      g_object_unref(fx_fm_synth_audio_processor);
    }
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(input_sequencer != NULL){
    g_object_unref(input_sequencer);
  }
  
  if(start_output != NULL){
    g_object_unref(start_output);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(start_recall);
}

GList*
ags_fx_factory_create_sf2_synth(AgsAudio *audio,
				AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				gchar *plugin_name,
				gchar *filename,
				gchar *effect,
				guint start_audio_channel, guint stop_audio_channel,
				guint start_pad, guint stop_pad,
				gint position,
				guint create_flags, guint recall_flags)
{
  AgsChannel *start_input, *start_output;
  AgsChannel *channel, *next_channel;

  AgsFxSF2SynthAudio *fx_sf2_synth_audio;
  AgsFxSF2SynthAudioProcessor *fx_sf2_synth_audio_processor;
  AgsFxSF2SynthChannel *fx_sf2_synth_channel;
  AgsFxSF2SynthChannelProcessor *fx_sf2_synth_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  gint channel_position;
  guint audio_channels;
  guint i, j;
  
  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-sf2-synth recall container not provided");
    
    return(NULL);
  }
  
  start_recall = NULL;

  start_output = NULL;

  start_input = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "output", &start_output,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);

  channel_position = 0;
  
  if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
    channel_position = ags_fx_factory_get_output_position(audio,
							  position);
  }else{
    channel_position = ags_fx_factory_get_input_position(audio,
							 position);
  }

  /* audio - play context */
  fx_sf2_synth_audio = NULL;
  fx_sf2_synth_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxSF2SynthAudio */
    fx_sf2_synth_audio = (AgsFxSF2SynthAudio *) g_object_new(AGS_TYPE_FX_SF2_SYNTH_AUDIO,
							     "output-soundcard", output_soundcard,
							     "audio", audio,
							     "recall-container", play_container,
							     "filename", filename,
							     "effect", effect,						    
							     NULL);
    ags_recall_set_flags((AgsRecall *) fx_sf2_synth_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_sf2_synth_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_sf2_synth_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_sf2_synth_audio,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_sf2_synth_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_sf2_synth_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_sf2_synth_audio));
    
    /* AgsFxSF2SynthAudioProcessor */
    fx_sf2_synth_audio_processor = (AgsFxSF2SynthAudioProcessor *) g_object_new(AGS_TYPE_FX_SF2_SYNTH_AUDIO_PROCESSOR,
										"output-soundcard", output_soundcard,
										"audio", audio,
										"recall-audio", fx_sf2_synth_audio,
										"recall-container", play_container,
										"filename", filename,
										"effect", effect,						    
										NULL);
    ags_recall_set_flags((AgsRecall *) fx_sf2_synth_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_sf2_synth_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_sf2_synth_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_sf2_synth_audio_processor,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_sf2_synth_audio_processor);
    
    start_recall = g_list_prepend(start_recall,
				  fx_sf2_synth_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_sf2_synth_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_sf2_synth_audio = ags_recall_container_get_recall_audio(play_container);

    if(fx_sf2_synth_audio != NULL){
      g_object_ref(fx_sf2_synth_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(play_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_sf2_synth_audio_processor = recall_audio_run->data;
      g_object_ref(fx_sf2_synth_audio_processor);
    }
  }

  /* channel - play context */
  for(i = 0; i < stop_pad - start_pad; i++){
    if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
      channel = ags_channel_pad_nth(start_output,
				    start_pad + i);
    }else{
      channel = ags_channel_pad_nth(start_input,
				    start_pad + i);
    }

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* add recall container */
      ags_channel_add_recall_container(channel,
				       (GObject *) recall_container);
	
      /* AgsFxSF2SynthChannel */
      fx_sf2_synth_channel = (AgsFxSF2SynthChannel *) g_object_new(AGS_TYPE_FX_SF2_SYNTH_CHANNEL,
								   "output-soundcard", output_soundcard,
								   "source", channel,
								   "recall-audio", fx_sf2_synth_audio,
								   "recall-container", play_container,
								   "filename", filename,
								   "effect", effect,						    
								   NULL);
      ags_recall_set_flags((AgsRecall *) fx_sf2_synth_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_sf2_synth_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_sf2_synth_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_sf2_synth_channel,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_sf2_synth_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_sf2_synth_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_sf2_synth_channel));

      /* AgsFxSF2SynthChannelProcessor */
      fx_sf2_synth_channel_processor = (AgsFxSF2SynthChannelProcessor *) g_object_new(AGS_TYPE_FX_SF2_SYNTH_CHANNEL_PROCESSOR,
										      "output-soundcard", output_soundcard,
										      "source", channel,
										      "recall-audio", fx_sf2_synth_audio,
										      "recall-audio-run", fx_sf2_synth_audio_processor,
										      "recall-channel", fx_sf2_synth_channel,
										      "recall-container", play_container,
										      "filename", filename,
										      "effect", effect,						    
										      NULL);
      ags_recall_set_flags((AgsRecall *) fx_sf2_synth_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_sf2_synth_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_sf2_synth_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_sf2_synth_channel_processor,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_sf2_synth_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_sf2_synth_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_sf2_synth_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_sf2_synth_audio != NULL){
      g_object_unref(fx_sf2_synth_audio);
    }
    
    if(fx_sf2_synth_audio_processor != NULL){
      g_object_unref(fx_sf2_synth_audio_processor);
    }
  }
  
  /* audio - recall context */
  fx_sf2_synth_audio = NULL;
  fx_sf2_synth_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxSF2SynthAudio */
    fx_sf2_synth_audio = (AgsFxSF2SynthAudio *) g_object_new(AGS_TYPE_FX_SF2_SYNTH_AUDIO,
							     "output-soundcard", output_soundcard,
							     "audio", audio,
							     "recall-container", recall_container,
							     "filename", filename,
							     "effect", effect,						    
							     NULL);
    ags_recall_set_flags((AgsRecall *) fx_sf2_synth_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_sf2_synth_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_sf2_synth_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_sf2_synth_audio,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_sf2_synth_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_sf2_synth_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_sf2_synth_audio));
    
    /* AgsFxSF2SynthAudioProcessor */
    fx_sf2_synth_audio_processor = (AgsFxSF2SynthAudioProcessor *) g_object_new(AGS_TYPE_FX_SF2_SYNTH_AUDIO_PROCESSOR,
										"output-soundcard", output_soundcard,
										"audio", audio,
										"recall-audio", fx_sf2_synth_audio,
										"recall-container", recall_container,
										"filename", filename,
										"effect", effect,						    
										NULL);
    ags_recall_set_flags((AgsRecall *) fx_sf2_synth_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_sf2_synth_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_sf2_synth_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_sf2_synth_audio_processor,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_sf2_synth_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_sf2_synth_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_sf2_synth_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_sf2_synth_audio = ags_recall_container_get_recall_audio(recall_container);

    if(fx_sf2_synth_audio != NULL){
      g_object_ref(fx_sf2_synth_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(recall_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_sf2_synth_audio_processor = recall_audio_run->data;
      g_object_ref(fx_sf2_synth_audio_processor);
    }
  }

  /* channel - recall context */  
  for(i = 0; i < stop_pad - start_pad; i++){
    if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
      channel = ags_channel_pad_nth(start_output,
				    start_pad + i);
    }else{
      channel = ags_channel_pad_nth(start_input,
				    start_pad + i);
    }

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxSF2SynthChannel */
      fx_sf2_synth_channel = (AgsFxSF2SynthChannel *) g_object_new(AGS_TYPE_FX_SF2_SYNTH_CHANNEL,
								   "output-soundcard", output_soundcard,
								   "source", channel,
								   "recall-audio", fx_sf2_synth_audio,
								   "recall-container", recall_container,
								   "filename", filename,
								   "effect", effect,						    
								   NULL);
      ags_recall_set_flags((AgsRecall *) fx_sf2_synth_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_sf2_synth_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_sf2_synth_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_sf2_synth_channel,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_sf2_synth_channel);
      
      start_recall = g_list_prepend(start_recall,
				    fx_sf2_synth_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_sf2_synth_channel));

      /* AgsFxSF2SynthChannelProcessor */
      fx_sf2_synth_channel_processor = (AgsFxSF2SynthChannelProcessor *) g_object_new(AGS_TYPE_FX_SF2_SYNTH_CHANNEL_PROCESSOR,
										      "output-soundcard", output_soundcard,
										      "source", channel,
										      "recall-audio", fx_sf2_synth_audio,
										      "recall-audio-run", fx_sf2_synth_audio_processor,
										      "recall-channel", fx_sf2_synth_channel,
										      "recall-container", recall_container,
										      "filename", filename,
										      "effect", effect,						    
										      NULL);
      ags_recall_set_flags((AgsRecall *) fx_sf2_synth_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_sf2_synth_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_sf2_synth_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_sf2_synth_channel_processor,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_sf2_synth_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_sf2_synth_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_sf2_synth_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_sf2_synth_audio != NULL){
      g_object_unref(fx_sf2_synth_audio);
    }
    
    if(fx_sf2_synth_audio_processor != NULL){
      g_object_unref(fx_sf2_synth_audio_processor);
    }
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(input_sequencer != NULL){
    g_object_unref(input_sequencer);
  }
  
  if(start_output != NULL){
    g_object_unref(start_output);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(start_recall);
}

GList*
ags_fx_factory_create_sfz_synth(AgsAudio *audio,
				AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				gchar *plugin_name,
				gchar *filename,
				gchar *effect,
				guint start_audio_channel, guint stop_audio_channel,
				guint start_pad, guint stop_pad,
				gint position,
				guint create_flags, guint recall_flags)
{
  AgsChannel *start_input, *start_output;
  AgsChannel *channel, *next_channel;

  AgsFxSFZSynthAudio *fx_sfz_synth_audio;
  AgsFxSFZSynthAudioProcessor *fx_sfz_synth_audio_processor;
  AgsFxSFZSynthChannel *fx_sfz_synth_channel;
  AgsFxSFZSynthChannelProcessor *fx_sfz_synth_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  gint channel_position;
  guint audio_channels;
  guint i, j;
  
  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-sfz-synth recall container not provided");
    
    return(NULL);
  }
  
  start_recall = NULL;

  start_output = NULL;

  start_input = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "output", &start_output,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);

  channel_position = 0;
  
  if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
    channel_position = ags_fx_factory_get_output_position(audio,
							  position);
  }else{
    channel_position = ags_fx_factory_get_input_position(audio,
							 position);
  }

  /* audio - play context */
  fx_sfz_synth_audio = NULL;
  fx_sfz_synth_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxSFZSynthAudio */
    fx_sfz_synth_audio = (AgsFxSFZSynthAudio *) g_object_new(AGS_TYPE_FX_SFZ_SYNTH_AUDIO,
							     "output-soundcard", output_soundcard,
							     "audio", audio,
							     "recall-container", play_container,
							     "filename", filename,
							     "effect", effect,						    
							     NULL);
    ags_recall_set_flags((AgsRecall *) fx_sfz_synth_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_sfz_synth_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_sfz_synth_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_sfz_synth_audio,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_sfz_synth_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_sfz_synth_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_sfz_synth_audio));
    
    /* AgsFxSFZSynthAudioProcessor */
    fx_sfz_synth_audio_processor = (AgsFxSFZSynthAudioProcessor *) g_object_new(AGS_TYPE_FX_SFZ_SYNTH_AUDIO_PROCESSOR,
										"output-soundcard", output_soundcard,
										"audio", audio,
										"recall-audio", fx_sfz_synth_audio,
										"recall-container", play_container,
										"filename", filename,
										"effect", effect,						    
										NULL);
    ags_recall_set_flags((AgsRecall *) fx_sfz_synth_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_sfz_synth_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_sfz_synth_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_sfz_synth_audio_processor,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_sfz_synth_audio_processor);
    
    start_recall = g_list_prepend(start_recall,
				  fx_sfz_synth_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_sfz_synth_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_sfz_synth_audio = ags_recall_container_get_recall_audio(play_container);

    if(fx_sfz_synth_audio != NULL){
      g_object_ref(fx_sfz_synth_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(play_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_sfz_synth_audio_processor = recall_audio_run->data;
      g_object_ref(fx_sfz_synth_audio_processor);
    }
  }

  /* channel - play context */
  for(i = 0; i < stop_pad - start_pad; i++){
    if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
      channel = ags_channel_pad_nth(start_output,
				    start_pad + i);
    }else{
      channel = ags_channel_pad_nth(start_input,
				    start_pad + i);
    }

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* add recall container */
      ags_channel_add_recall_container(channel,
				       (GObject *) recall_container);
	
      /* AgsFxSFZSynthChannel */
      fx_sfz_synth_channel = (AgsFxSFZSynthChannel *) g_object_new(AGS_TYPE_FX_SFZ_SYNTH_CHANNEL,
								   "output-soundcard", output_soundcard,
								   "source", channel,
								   "recall-audio", fx_sfz_synth_audio,
								   "recall-container", play_container,
								   "filename", filename,
								   "effect", effect,						    
								   NULL);
      ags_recall_set_flags((AgsRecall *) fx_sfz_synth_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_sfz_synth_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_sfz_synth_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_sfz_synth_channel,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_sfz_synth_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_sfz_synth_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_sfz_synth_channel));

      /* AgsFxSFZSynthChannelProcessor */
      fx_sfz_synth_channel_processor = (AgsFxSFZSynthChannelProcessor *) g_object_new(AGS_TYPE_FX_SFZ_SYNTH_CHANNEL_PROCESSOR,
										      "output-soundcard", output_soundcard,
										      "source", channel,
										      "recall-audio", fx_sfz_synth_audio,
										      "recall-audio-run", fx_sfz_synth_audio_processor,
										      "recall-channel", fx_sfz_synth_channel,
										      "recall-container", play_container,
										      "filename", filename,
										      "effect", effect,						    
										      NULL);
      ags_recall_set_flags((AgsRecall *) fx_sfz_synth_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_sfz_synth_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_sfz_synth_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_sfz_synth_channel_processor,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_sfz_synth_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_sfz_synth_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_sfz_synth_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_sfz_synth_audio != NULL){
      g_object_unref(fx_sfz_synth_audio);
    }
    
    if(fx_sfz_synth_audio_processor != NULL){
      g_object_unref(fx_sfz_synth_audio_processor);
    }
  }
  
  /* audio - recall context */
  fx_sfz_synth_audio = NULL;
  fx_sfz_synth_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxSFZSynthAudio */
    fx_sfz_synth_audio = (AgsFxSFZSynthAudio *) g_object_new(AGS_TYPE_FX_SFZ_SYNTH_AUDIO,
							     "output-soundcard", output_soundcard,
							     "audio", audio,
							     "recall-container", recall_container,
							     "filename", filename,
							     "effect", effect,						    
							     NULL);
    ags_recall_set_flags((AgsRecall *) fx_sfz_synth_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_sfz_synth_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_sfz_synth_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_sfz_synth_audio,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_sfz_synth_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_sfz_synth_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_sfz_synth_audio));
    
    /* AgsFxSFZSynthAudioProcessor */
    fx_sfz_synth_audio_processor = (AgsFxSFZSynthAudioProcessor *) g_object_new(AGS_TYPE_FX_SFZ_SYNTH_AUDIO_PROCESSOR,
										"output-soundcard", output_soundcard,
										"audio", audio,
										"recall-audio", fx_sfz_synth_audio,
										"recall-container", recall_container,
										"filename", filename,
										"effect", effect,						    
										NULL);
    ags_recall_set_flags((AgsRecall *) fx_sfz_synth_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_sfz_synth_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_sfz_synth_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_sfz_synth_audio_processor,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_sfz_synth_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_sfz_synth_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_sfz_synth_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_sfz_synth_audio = ags_recall_container_get_recall_audio(recall_container);

    if(fx_sfz_synth_audio != NULL){
      g_object_ref(fx_sfz_synth_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(recall_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_sfz_synth_audio_processor = recall_audio_run->data;
      g_object_ref(fx_sfz_synth_audio_processor);
    }
  }

  /* channel - recall context */  
  for(i = 0; i < stop_pad - start_pad; i++){
    if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
      channel = ags_channel_pad_nth(start_output,
				    start_pad + i);
    }else{
      channel = ags_channel_pad_nth(start_input,
				    start_pad + i);
    }

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxSFZSynthChannel */
      fx_sfz_synth_channel = (AgsFxSFZSynthChannel *) g_object_new(AGS_TYPE_FX_SFZ_SYNTH_CHANNEL,
								   "output-soundcard", output_soundcard,
								   "source", channel,
								   "recall-audio", fx_sfz_synth_audio,
								   "recall-container", recall_container,
								   "filename", filename,
								   "effect", effect,						    
								   NULL);
      ags_recall_set_flags((AgsRecall *) fx_sfz_synth_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_sfz_synth_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_sfz_synth_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_sfz_synth_channel,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_sfz_synth_channel);
      
      start_recall = g_list_prepend(start_recall,
				    fx_sfz_synth_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_sfz_synth_channel));

      /* AgsFxSFZSynthChannelProcessor */
      fx_sfz_synth_channel_processor = (AgsFxSFZSynthChannelProcessor *) g_object_new(AGS_TYPE_FX_SFZ_SYNTH_CHANNEL_PROCESSOR,
										      "output-soundcard", output_soundcard,
										      "source", channel,
										      "recall-audio", fx_sfz_synth_audio,
										      "recall-audio-run", fx_sfz_synth_audio_processor,
										      "recall-channel", fx_sfz_synth_channel,
										      "recall-container", recall_container,
										      "filename", filename,
										      "effect", effect,						    
										      NULL);
      ags_recall_set_flags((AgsRecall *) fx_sfz_synth_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_sfz_synth_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_sfz_synth_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_sfz_synth_channel_processor,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_sfz_synth_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_sfz_synth_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_sfz_synth_channel_processor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_sfz_synth_audio != NULL){
      g_object_unref(fx_sfz_synth_audio);
    }
    
    if(fx_sfz_synth_audio_processor != NULL){
      g_object_unref(fx_sfz_synth_audio_processor);
    }
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(input_sequencer != NULL){
    g_object_unref(input_sequencer);
  }
  
  if(start_output != NULL){
    g_object_unref(start_output);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(start_recall);
}

GList*
ags_fx_factory_create_ladspa(AgsAudio *audio,
			     AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			     gchar *plugin_name,
			     gchar *filename,
			     gchar *effect,
			     guint start_audio_channel, guint stop_audio_channel,
			     guint start_pad, guint stop_pad,
			     gint position,
			     guint create_flags, guint recall_flags)
{
  AgsChannel *start_input, *start_output;
  AgsChannel *channel, *next_channel;

  AgsFxLadspaAudio *fx_ladspa_audio;
  AgsFxLadspaAudioProcessor *fx_ladspa_audio_processor;
  AgsFxLadspaChannel *fx_ladspa_channel;
  AgsFxLadspaChannelProcessor *fx_ladspa_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  gint channel_position;
  guint audio_channels;
  guint i, j;
  
  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-ladspa recall container not provided");
    
    return(NULL);
  }
  
  start_recall = NULL;

  start_output = NULL;

  start_input = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "output", &start_output,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);
  
  channel_position = 0;
  
  if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
    channel_position = ags_fx_factory_get_output_position(audio,
							  position);
  }else{
    channel_position = ags_fx_factory_get_input_position(audio,
							 position);
  }

  /* audio - play context */
  fx_ladspa_audio = NULL;
  fx_ladspa_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxLadspaAudio */
    fx_ladspa_audio = (AgsFxLadspaAudio *) g_object_new(AGS_TYPE_FX_LADSPA_AUDIO,
							"output-soundcard", output_soundcard,
							"audio", audio,
							"recall-container", play_container,
							"filename", filename,
							"effect", effect,						    
							NULL);
    ags_recall_set_flags((AgsRecall *) fx_ladspa_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_ladspa_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_ladspa_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_ladspa_audio,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_ladspa_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_ladspa_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_ladspa_audio));
    
    /* AgsFxLadspaAudioProcessor */
    fx_ladspa_audio_processor = (AgsFxLadspaAudioProcessor *) g_object_new(AGS_TYPE_FX_LADSPA_AUDIO_PROCESSOR,
									   "output-soundcard", output_soundcard,
									   "audio", audio,
									   "recall-audio", fx_ladspa_audio,
									   "recall-container", play_container,
									   "filename", filename,
									   "effect", effect,						    
									   NULL);
    ags_recall_set_flags((AgsRecall *) fx_ladspa_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_ladspa_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_ladspa_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_ladspa_audio_processor,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_ladspa_audio_processor);
    
    start_recall = g_list_prepend(start_recall,
				  fx_ladspa_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_ladspa_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_ladspa_audio = ags_recall_container_get_recall_audio(play_container);

    if(fx_ladspa_audio != NULL){
      g_object_ref(fx_ladspa_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(play_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_ladspa_audio_processor = recall_audio_run->data;
      g_object_ref(fx_ladspa_audio_processor);
    }
  }

  /* channel - play context */
  for(i = 0; i < stop_pad - start_pad; i++){
    if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
      channel = ags_channel_pad_nth(start_output,
				    start_pad + i);
    }else{
      channel = ags_channel_pad_nth(start_input,
				    start_pad + i);
    }

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxLadspaChannel */
      fx_ladspa_channel = (AgsFxLadspaChannel *) g_object_new(AGS_TYPE_FX_LADSPA_CHANNEL,
							      "output-soundcard", output_soundcard,
							      "source", channel,
							      "recall-audio", fx_ladspa_audio,
							      "recall-container", play_container,
							      "filename", filename,
							      "effect", effect,						    
							      NULL);
      ags_recall_set_flags((AgsRecall *) fx_ladspa_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_ladspa_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_ladspa_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_ladspa_channel,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_ladspa_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_ladspa_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_ladspa_channel));

      /* AgsFxLadspaChannelProcessor */
      fx_ladspa_channel_processor = (AgsFxLadspaChannelProcessor *) g_object_new(AGS_TYPE_FX_LADSPA_CHANNEL_PROCESSOR,
										 "output-soundcard", output_soundcard,
										 "source", channel,
										 "recall-audio", fx_ladspa_audio,
										 "recall-audio-run", fx_ladspa_audio_processor,
										 "recall-channel", fx_ladspa_channel,
										 "recall-container", play_container,
										 "filename", filename,
										 "effect", effect,						    
										 NULL);
      ags_recall_set_flags((AgsRecall *) fx_ladspa_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_ladspa_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_ladspa_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_ladspa_channel_processor,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_ladspa_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_ladspa_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_ladspa_channel_processor));

      /* load */
      ags_fx_ladspa_channel_load_plugin(fx_ladspa_channel);
      ags_fx_ladspa_channel_load_port(fx_ladspa_channel);

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_ladspa_audio != NULL){
      g_object_unref(fx_ladspa_audio);
    }
    
    if(fx_ladspa_audio_processor != NULL){
      g_object_unref(fx_ladspa_audio_processor);
    }
  }

  /* audio - recall context */
  fx_ladspa_audio = NULL;
  fx_ladspa_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxLadspaAudio */
    fx_ladspa_audio = (AgsFxLadspaAudio *) g_object_new(AGS_TYPE_FX_LADSPA_AUDIO,
							"output-soundcard", output_soundcard,
							"audio", audio,
							"recall-container", recall_container,
							"filename", filename,
							"effect", effect,						    
							NULL);
    ags_recall_set_flags((AgsRecall *) fx_ladspa_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_ladspa_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_ladspa_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_ladspa_audio,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_ladspa_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_ladspa_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_ladspa_audio));
    
    /* AgsFxLadspaAudioProcessor */
    fx_ladspa_audio_processor = (AgsFxLadspaAudioProcessor *) g_object_new(AGS_TYPE_FX_LADSPA_AUDIO_PROCESSOR,
									   "output-soundcard", output_soundcard,
									   "audio", audio,
									   "recall-audio", fx_ladspa_audio,
									   "recall-container", recall_container,
									   "filename", filename,
									   "effect", effect,						    
									   NULL);
    ags_recall_set_flags((AgsRecall *) fx_ladspa_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_ladspa_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_WAVE |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_ladspa_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_ladspa_audio_processor,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_ladspa_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_ladspa_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_ladspa_audio_processor));
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_ladspa_audio = ags_recall_container_get_recall_audio(recall_container);

    if(fx_ladspa_audio != NULL){
      g_object_ref(fx_ladspa_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(recall_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_ladspa_audio_processor = recall_audio_run->data;
      g_object_ref(fx_ladspa_audio_processor);
    }
  }

  /* channel - recall context */
  
  for(i = 0; i < stop_pad - start_pad; i++){
    if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
      channel = ags_channel_pad_nth(start_output,
				    start_pad + i);
    }else{
      channel = ags_channel_pad_nth(start_input,
				    start_pad + i);
    }

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxLadspaChannel */
      fx_ladspa_channel = (AgsFxLadspaChannel *) g_object_new(AGS_TYPE_FX_LADSPA_CHANNEL,
							      "output-soundcard", output_soundcard,
							      "source", channel,
							      "recall-audio", fx_ladspa_audio,
							      "recall-container", recall_container,
							      "filename", filename,
							      "effect", effect,						    
							      NULL);
      ags_recall_set_flags((AgsRecall *) fx_ladspa_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_ladspa_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_ladspa_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_ladspa_channel,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_ladspa_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_ladspa_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_ladspa_channel));

      /* AgsFxLadspaChannelProcessor */
      fx_ladspa_channel_processor = (AgsFxLadspaChannelProcessor *) g_object_new(AGS_TYPE_FX_LADSPA_CHANNEL_PROCESSOR,
										 "output-soundcard", output_soundcard,
										 "source", channel,
										 "recall-audio", fx_ladspa_audio,
										 "recall-audio-run", fx_ladspa_audio_processor,
										 "recall-channel", fx_ladspa_channel,
										 "recall-container", recall_container,
										 "filename", filename,
										 "effect", effect,						    
										 NULL);
      ags_recall_set_flags((AgsRecall *) fx_ladspa_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_ladspa_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_ladspa_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_ladspa_channel_processor,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_ladspa_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_ladspa_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_ladspa_channel_processor));

      /* load */
      ags_fx_ladspa_channel_load_plugin(fx_ladspa_channel);
      ags_fx_ladspa_channel_load_port(fx_ladspa_channel);

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_ladspa_audio != NULL){
      g_object_unref(fx_ladspa_audio);
    }
    
    if(fx_ladspa_audio_processor != NULL){
      g_object_unref(fx_ladspa_audio_processor);
    }
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(input_sequencer != NULL){
    g_object_unref(input_sequencer);
  }
  
  if(start_output != NULL){
    g_object_unref(start_output);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(start_recall);
}

GList*
ags_fx_factory_create_dssi(AgsAudio *audio,
			   AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			   gchar *plugin_name,
			   gchar *filename,
			   gchar *effect,
			   guint start_audio_channel, guint stop_audio_channel,
			   guint start_pad, guint stop_pad,
			   gint position,
			   guint create_flags, guint recall_flags)
{
  AgsChannel *start_input, *start_output;
  AgsChannel *channel, *next_channel;

  AgsFxDssiAudio *fx_dssi_audio;
  AgsFxDssiAudioProcessor *fx_dssi_audio_processor;
  AgsFxDssiChannel *fx_dssi_channel;
  AgsFxDssiChannelProcessor *fx_dssi_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  gint channel_position;
  guint audio_channels;
  guint i, j;
  
  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-dssi recall container not provided");
    
    return(NULL);
  }
  
  start_recall = NULL;

  start_output = NULL;

  start_input = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "output", &start_output,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);

  channel_position = 0;
  
  if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
    channel_position = ags_fx_factory_get_output_position(audio,
							  position);
  }else{
    channel_position = ags_fx_factory_get_input_position(audio,
							 position);
  }

  /* audio - play context */
  fx_dssi_audio = NULL;
  fx_dssi_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxDssiAudio */
    fx_dssi_audio = (AgsFxDssiAudio *) g_object_new(AGS_TYPE_FX_DSSI_AUDIO,
						    "output-soundcard", output_soundcard,
						    "audio", audio,
						    "recall-container", play_container,
						    "filename", filename,
						    "effect", effect,						    
						    NULL);
    ags_recall_set_flags((AgsRecall *) fx_dssi_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_dssi_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_dssi_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_dssi_audio,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_dssi_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_dssi_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_dssi_audio));
    
    /* AgsFxDssiAudioProcessor */
    fx_dssi_audio_processor = (AgsFxDssiAudioProcessor *) g_object_new(AGS_TYPE_FX_DSSI_AUDIO_PROCESSOR,
								       "output-soundcard", output_soundcard,
								       "audio", audio,
								       "recall-audio", fx_dssi_audio,
								       "recall-container", play_container,
								       "filename", filename,
								       "effect", effect,						    
								       NULL);
    ags_recall_set_flags((AgsRecall *) fx_dssi_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_dssi_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_dssi_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_dssi_audio_processor,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_dssi_audio_processor);
    
    start_recall = g_list_prepend(start_recall,
				  fx_dssi_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_dssi_audio_processor));

    /* load */
    if((AGS_FX_FACTORY_LIVE & (create_flags)) == 0){
      ags_fx_dssi_audio_unset_flags(fx_dssi_audio,
				    AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT);
    }
    
    ags_fx_dssi_audio_load_plugin(fx_dssi_audio);
    ags_fx_dssi_audio_load_port(fx_dssi_audio);
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_dssi_audio = ags_recall_container_get_recall_audio(play_container);

    if(fx_dssi_audio != NULL){
      g_object_ref(fx_dssi_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(play_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_dssi_audio_processor = recall_audio_run->data;
      g_object_ref(fx_dssi_audio_processor);
    }
  }

  /* channel - play context */
  for(i = 0; i < stop_pad - start_pad; i++){
    if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
      channel = ags_channel_pad_nth(start_output,
				    start_pad + i);
    }else{
      channel = ags_channel_pad_nth(start_input,
				    start_pad + i);
    }

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* add recall container */
      ags_channel_add_recall_container(channel,
				       (GObject *) recall_container);
	
      /* AgsFxDssiChannel */
      fx_dssi_channel = (AgsFxDssiChannel *) g_object_new(AGS_TYPE_FX_DSSI_CHANNEL,
							  "output-soundcard", output_soundcard,
							  "source", channel,
							  "recall-audio", fx_dssi_audio,
							  "recall-container", play_container,
							  "filename", filename,
							  "effect", effect,						    
							  NULL);
      ags_recall_set_flags((AgsRecall *) fx_dssi_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_dssi_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_dssi_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_dssi_channel,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_dssi_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_dssi_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_dssi_channel));

      /* AgsFxDssiChannelProcessor */
      fx_dssi_channel_processor = (AgsFxDssiChannelProcessor *) g_object_new(AGS_TYPE_FX_DSSI_CHANNEL_PROCESSOR,
									     "output-soundcard", output_soundcard,
									     "source", channel,
									     "recall-audio", fx_dssi_audio,
									     "recall-audio-run", fx_dssi_audio_processor,
									     "recall-channel", fx_dssi_channel,
									     "recall-container", play_container,
									     "filename", filename,
									     "effect", effect,						    
									     NULL);
      ags_recall_set_flags((AgsRecall *) fx_dssi_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_dssi_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_dssi_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_dssi_channel_processor,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_dssi_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_dssi_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_dssi_channel_processor));

      /* load */
      ags_fx_dssi_channel_load_port(fx_dssi_channel);

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_dssi_audio != NULL){
      g_object_unref(fx_dssi_audio);
    }
    
    if(fx_dssi_audio_processor != NULL){
      g_object_unref(fx_dssi_audio_processor);
    }
  }
  
  /* audio - recall context */
  fx_dssi_audio = NULL;
  fx_dssi_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxDssiAudio */
    fx_dssi_audio = (AgsFxDssiAudio *) g_object_new(AGS_TYPE_FX_DSSI_AUDIO,
						    "output-soundcard", output_soundcard,
						    "audio", audio,
						    "recall-container", recall_container,
						    "filename", filename,
						    "effect", effect,						    
						    NULL);
    ags_recall_set_flags((AgsRecall *) fx_dssi_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_dssi_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_dssi_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_dssi_audio,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_dssi_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_dssi_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_dssi_audio));
    
    /* AgsFxDssiAudioProcessor */
    fx_dssi_audio_processor = (AgsFxDssiAudioProcessor *) g_object_new(AGS_TYPE_FX_DSSI_AUDIO_PROCESSOR,
								       "output-soundcard", output_soundcard,
								       "audio", audio,
								       "recall-audio", fx_dssi_audio,
								       "recall-container", recall_container,
								       "filename", filename,
								       "effect", effect,						    
								       NULL);
    ags_recall_set_flags((AgsRecall *) fx_dssi_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_dssi_audio_processor,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_dssi_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_dssi_audio_processor,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_dssi_audio_processor);

    start_recall = g_list_prepend(start_recall,
				  fx_dssi_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_dssi_audio_processor));

    /* load */
    if((AGS_FX_FACTORY_LIVE & (create_flags)) == 0){
      ags_fx_dssi_audio_unset_flags(fx_dssi_audio,
				    AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT);
    }
    
    ags_fx_dssi_audio_load_plugin(fx_dssi_audio);
    ags_fx_dssi_audio_load_port(fx_dssi_audio);
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_dssi_audio = ags_recall_container_get_recall_audio(recall_container);

    if(fx_dssi_audio != NULL){
      g_object_ref(fx_dssi_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(recall_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_dssi_audio_processor = recall_audio_run->data;
      g_object_ref(fx_dssi_audio_processor);
    }
  }

  /* channel - recall context */  
  for(i = 0; i < stop_pad - start_pad; i++){
    if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
      channel = ags_channel_pad_nth(start_output,
				    start_pad + i);
    }else{
      channel = ags_channel_pad_nth(start_input,
				    start_pad + i);
    }

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxDssiChannel */
      fx_dssi_channel = (AgsFxDssiChannel *) g_object_new(AGS_TYPE_FX_DSSI_CHANNEL,
							  "output-soundcard", output_soundcard,
							  "source", channel,
							  "recall-audio", fx_dssi_audio,
							  "recall-container", recall_container,
							  "filename", filename,
							  "effect", effect,						    
							  NULL);
      ags_recall_set_flags((AgsRecall *) fx_dssi_channel,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_dssi_channel,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_dssi_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_dssi_channel,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_dssi_channel);
      
      start_recall = g_list_prepend(start_recall,
				    fx_dssi_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_dssi_channel));

      /* AgsFxDssiChannelProcessor */
      fx_dssi_channel_processor = (AgsFxDssiChannelProcessor *) g_object_new(AGS_TYPE_FX_DSSI_CHANNEL_PROCESSOR,
									     "output-soundcard", output_soundcard,
									     "source", channel,
									     "recall-audio", fx_dssi_audio,
									     "recall-audio-run", fx_dssi_audio_processor,
									     "recall-channel", fx_dssi_channel,
									     "recall-container", recall_container,
									     "filename", filename,
									     "effect", effect,						    
									     NULL);
      ags_recall_set_flags((AgsRecall *) fx_dssi_channel_processor,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags((AgsRecall *) fx_dssi_channel_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
      ags_recall_set_behaviour_flags((AgsRecall *) fx_dssi_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_dssi_channel_processor,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_dssi_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_dssi_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_dssi_channel_processor));

      /* load */
      ags_fx_dssi_channel_load_port(fx_dssi_channel);

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_dssi_audio != NULL){
      g_object_unref(fx_dssi_audio);
    }
    
    if(fx_dssi_audio_processor != NULL){
      g_object_unref(fx_dssi_audio_processor);
    }
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(input_sequencer != NULL){
    g_object_unref(input_sequencer);
  }
  
  if(start_output != NULL){
    g_object_unref(start_output);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(start_recall);
}

GList*
ags_fx_factory_create_lv2(AgsAudio *audio,
			  AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			  gchar *plugin_name,
			  gchar *filename,
			  gchar *effect,
			  guint start_audio_channel, guint stop_audio_channel,
			  guint start_pad, guint stop_pad,
			  gint position,
			  guint create_flags, guint recall_flags)
{
  AgsChannel *start_input, *start_output;
  AgsChannel *channel, *next_channel;

  AgsFxLv2Audio *fx_lv2_audio;
  AgsFxLv2AudioProcessor *fx_lv2_audio_processor;
  AgsFxLv2Channel *fx_lv2_channel;
  AgsFxLv2ChannelProcessor *fx_lv2_channel_processor;

  AgsLv2Manager *lv2_manager;
  AgsLv2Plugin *lv2_plugin;
  
  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  gint channel_position;
  guint audio_channels;
  guint i, j;
  gboolean is_instrument;
  
  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-lv2 recall container not provided");
    
    return(NULL);
  }

  lv2_manager = ags_lv2_manager_get_instance();  

  lv2_plugin = ags_lv2_manager_find_lv2_plugin(lv2_manager,
					       filename, effect);
  
  is_instrument = ags_base_plugin_test_flags((AgsBasePlugin *) lv2_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT);

  start_recall = NULL;

  start_output = NULL;

  start_input = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "output", &start_output,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);
  
  channel_position = 0;
  
  if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
    channel_position = ags_fx_factory_get_output_position(audio,
							  position);
  }else{
    channel_position = ags_fx_factory_get_input_position(audio,
							 position);
  }

  /* audio - play context */
  fx_lv2_audio = NULL;
  fx_lv2_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxLv2Audio */
    fx_lv2_audio = (AgsFxLv2Audio *) g_object_new(AGS_TYPE_FX_LV2_AUDIO,
						  "output-soundcard", output_soundcard,
						  "audio", audio,
						  "recall-container", play_container,
						  "filename", filename,
						  "effect", effect,			    
						  NULL);
    ags_recall_set_flags((AgsRecall *) fx_lv2_audio,
			 (AGS_RECALL_TEMPLATE));

    if(is_instrument){
      ags_recall_set_ability_flags((AgsRecall *) fx_lv2_audio,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
    }else{
      ags_recall_set_ability_flags((AgsRecall *) fx_lv2_audio,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
    }
  
    ags_recall_set_behaviour_flags((AgsRecall *) fx_lv2_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_lv2_audio,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_lv2_audio);
    
    start_recall = g_list_prepend(start_recall,
				  fx_lv2_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_lv2_audio));
    
    /* AgsFxLv2AudioProcessor */
    fx_lv2_audio_processor = (AgsFxLv2AudioProcessor *) g_object_new(AGS_TYPE_FX_LV2_AUDIO_PROCESSOR,
								     "output-soundcard", output_soundcard,
								     "audio", audio,
								     "recall-audio", fx_lv2_audio,
								     "recall-container", play_container,
								     "filename", filename,
								     "effect", effect,						    
								     NULL);
    ags_recall_set_flags((AgsRecall *) fx_lv2_audio_processor,
			 (AGS_RECALL_TEMPLATE));

    if(is_instrument){
      ags_recall_set_ability_flags((AgsRecall *) fx_lv2_audio_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
    }else{
      ags_recall_set_ability_flags((AgsRecall *) fx_lv2_audio_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
    }

    ags_recall_set_behaviour_flags((AgsRecall *) fx_lv2_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_lv2_audio_processor,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_lv2_audio_processor);
    
    start_recall = g_list_prepend(start_recall,
				  fx_lv2_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_lv2_audio_processor));

    /* load */
    if((AGS_FX_FACTORY_LIVE & (create_flags)) == 0){
      ags_fx_lv2_audio_unset_flags(fx_lv2_audio,
				   AGS_FX_LV2_AUDIO_LIVE_INSTRUMENT);
    }
    
    ags_fx_lv2_audio_load_plugin(fx_lv2_audio);
    ags_fx_lv2_audio_load_port(fx_lv2_audio);
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_lv2_audio = ags_recall_container_get_recall_audio(play_container);

    if(fx_lv2_audio != NULL){
      g_object_ref(fx_lv2_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(play_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_lv2_audio_processor = recall_audio_run->data;
      g_object_ref(fx_lv2_audio_processor);
    }

    ags_fx_lv2_audio_load_plugin(fx_lv2_audio);
    ags_fx_lv2_audio_load_port(fx_lv2_audio);
  }

  /* channel - play context */  
  for(i = 0; i < stop_pad - start_pad; i++){
    if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
      channel = ags_channel_pad_nth(start_output,
				    start_pad + i);
    }else{
      channel = ags_channel_pad_nth(start_input,
				    start_pad + i);
    }

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* add recall container */
      ags_channel_add_recall_container(channel,
				       (GObject *) recall_container);
	
      /* AgsFxLv2Channel */
      fx_lv2_channel = (AgsFxLv2Channel *) g_object_new(AGS_TYPE_FX_LV2_CHANNEL,
							"output-soundcard", output_soundcard,
							"source", channel,
							"recall-audio", fx_lv2_audio,
							"recall-container", play_container,
							"filename", filename,
							"effect", effect,						    
							NULL);
      ags_recall_set_flags((AgsRecall *) fx_lv2_channel,
			   (AGS_RECALL_TEMPLATE));

      if(is_instrument){
	ags_recall_set_ability_flags((AgsRecall *) fx_lv2_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_MIDI));
      }else{
	ags_recall_set_ability_flags((AgsRecall *) fx_lv2_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_WAVE |
				      AGS_SOUND_ABILITY_MIDI));
      }

      ags_recall_set_behaviour_flags((AgsRecall *) fx_lv2_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_lv2_channel,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_lv2_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_lv2_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_lv2_channel));

      /* AgsFxLv2ChannelProcessor */
      fx_lv2_channel_processor = (AgsFxLv2ChannelProcessor *) g_object_new(AGS_TYPE_FX_LV2_CHANNEL_PROCESSOR,
									   "output-soundcard", output_soundcard,
									   "source", channel,
									   "recall-audio", fx_lv2_audio,
									   "recall-audio-run", fx_lv2_audio_processor,
									   "recall-channel", fx_lv2_channel,
									   "recall-container", play_container,
									   "filename", filename,
									   "effect", effect,						    
									   NULL);
      ags_recall_set_flags((AgsRecall *) fx_lv2_channel_processor,
			   (AGS_RECALL_TEMPLATE));

      if(is_instrument){
	ags_recall_set_ability_flags((AgsRecall *) fx_lv2_channel_processor,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_MIDI));
      }else{
	ags_recall_set_ability_flags((AgsRecall *) fx_lv2_channel_processor,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_WAVE |
				      AGS_SOUND_ABILITY_MIDI));
      }

      ags_recall_set_behaviour_flags((AgsRecall *) fx_lv2_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_lv2_channel_processor,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_lv2_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_lv2_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_lv2_channel_processor));

      /* load */
      ags_fx_lv2_channel_load_plugin(fx_lv2_channel);
      ags_fx_lv2_channel_load_port(fx_lv2_channel);

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_lv2_audio != NULL){
      g_object_unref(fx_lv2_audio);
    }
    
    if(fx_lv2_audio_processor != NULL){
      g_object_unref(fx_lv2_audio_processor);
    }
  }
  
  /* audio - recall context */
  fx_lv2_audio = NULL;
  fx_lv2_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxLv2Audio */
    fx_lv2_audio = (AgsFxLv2Audio *) g_object_new(AGS_TYPE_FX_LV2_AUDIO,
						  "output-soundcard", output_soundcard,
						  "audio", audio,
						  "recall-container", recall_container,
						  "filename", filename,
						  "effect", effect,						    
						  NULL);
    ags_recall_set_flags((AgsRecall *) fx_lv2_audio,
			 (AGS_RECALL_TEMPLATE));

    if(is_instrument){
      ags_recall_set_ability_flags((AgsRecall *) fx_lv2_audio,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
    }else{
      ags_recall_set_ability_flags((AgsRecall *) fx_lv2_audio,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
    }

    ags_recall_set_behaviour_flags((AgsRecall *) fx_lv2_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_lv2_audio,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_lv2_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_lv2_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_lv2_audio));
    
    /* AgsFxLv2AudioProcessor */
    fx_lv2_audio_processor = (AgsFxLv2AudioProcessor *) g_object_new(AGS_TYPE_FX_LV2_AUDIO_PROCESSOR,
								     "output-soundcard", output_soundcard,
								     "audio", audio,
								     "recall-audio", fx_lv2_audio,
								     "recall-container", recall_container,
								     "filename", filename,
								     "effect", effect,						    
								     NULL);
    ags_recall_set_flags((AgsRecall *) fx_lv2_audio_processor,
			 (AGS_RECALL_TEMPLATE));

    if(is_instrument){
      ags_recall_set_ability_flags((AgsRecall *) fx_lv2_audio_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
    }else{
      ags_recall_set_ability_flags((AgsRecall *) fx_lv2_audio_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
    }

    ags_recall_set_behaviour_flags((AgsRecall *) fx_lv2_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_lv2_audio_processor,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_lv2_audio_processor);
    
    start_recall = g_list_prepend(start_recall,
				  fx_lv2_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_lv2_audio_processor));

    /* load */
    if((AGS_FX_FACTORY_LIVE & (create_flags)) == 0){
      ags_fx_lv2_audio_unset_flags(fx_lv2_audio,
				   AGS_FX_LV2_AUDIO_LIVE_INSTRUMENT);
    }
    
    ags_fx_lv2_audio_load_plugin(fx_lv2_audio);
    ags_fx_lv2_audio_load_port(fx_lv2_audio);
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_lv2_audio = ags_recall_container_get_recall_audio(recall_container);

    if(fx_lv2_audio != NULL){
      g_object_ref(fx_lv2_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(recall_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_lv2_audio_processor = recall_audio_run->data;
      g_object_ref(fx_lv2_audio_processor);
    }
  }

  /* channel - recall context */  
  for(i = 0; i < stop_pad - start_pad; i++){
    if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
      channel = ags_channel_pad_nth(start_output,
				    start_pad + i);
    }else{
      channel = ags_channel_pad_nth(start_input,
				    start_pad + i);
    }

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxLv2Channel */
      fx_lv2_channel = (AgsFxLv2Channel *) g_object_new(AGS_TYPE_FX_LV2_CHANNEL,
							"output-soundcard", output_soundcard,
							"source", channel,
							"recall-audio", fx_lv2_audio,
							"recall-container", recall_container,
							"filename", filename,
							"effect", effect,						    
							NULL);
      ags_recall_set_flags((AgsRecall *) fx_lv2_channel,
			   (AGS_RECALL_TEMPLATE));

      if(is_instrument){
	ags_recall_set_ability_flags((AgsRecall *) fx_lv2_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_MIDI));
      }else{
	ags_recall_set_ability_flags((AgsRecall *) fx_lv2_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_WAVE |
				      AGS_SOUND_ABILITY_MIDI));
      }

      ags_recall_set_behaviour_flags((AgsRecall *) fx_lv2_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_lv2_channel,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_lv2_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_lv2_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_lv2_channel));

      /* AgsFxLv2ChannelProcessor */
      fx_lv2_channel_processor = (AgsFxLv2ChannelProcessor *) g_object_new(AGS_TYPE_FX_LV2_CHANNEL_PROCESSOR,
									   "output-soundcard", output_soundcard,
									   "source", channel,
									   "recall-audio", fx_lv2_audio,
									   "recall-audio-run", fx_lv2_audio_processor,
									   "recall-channel", fx_lv2_channel,
									   "recall-container", recall_container,
									   "filename", filename,
									   "effect", effect,						    
									   NULL);
      ags_recall_set_flags((AgsRecall *) fx_lv2_channel_processor,
			   (AGS_RECALL_TEMPLATE));

      if(is_instrument){
	ags_recall_set_ability_flags((AgsRecall *) fx_lv2_channel_processor,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_MIDI));
      }else{
	ags_recall_set_ability_flags((AgsRecall *) fx_lv2_channel_processor,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_WAVE |
				      AGS_SOUND_ABILITY_MIDI));
      }

      ags_recall_set_behaviour_flags((AgsRecall *) fx_lv2_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_lv2_channel_processor,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_lv2_channel_processor);
      
      start_recall = g_list_prepend(start_recall,
				    fx_lv2_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_lv2_channel_processor));

      /* load */
      ags_fx_lv2_channel_load_plugin(fx_lv2_channel);
      ags_fx_lv2_channel_load_port(fx_lv2_channel);

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_lv2_audio != NULL){
      g_object_unref(fx_lv2_audio);
    }
    
    if(fx_lv2_audio_processor != NULL){
      g_object_unref(fx_lv2_audio_processor);
    }
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(input_sequencer != NULL){
    g_object_unref(input_sequencer);
  }
  
  if(start_output != NULL){
    g_object_unref(start_output);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }
  
  return(start_recall);
}

#if defined(AGS_WITH_VST3)
GList*
ags_fx_factory_create_vst3(AgsAudio *audio,
			   AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			   gchar *plugin_name,
			   gchar *filename,
			   gchar *effect,
			   guint start_audio_channel, guint stop_audio_channel,
			   guint start_pad, guint stop_pad,
			   gint position,
			   guint create_flags, guint recall_flags)
{
  AgsChannel *start_input, *start_output;
  AgsChannel *channel, *next_channel;

  AgsFxVst3Audio *fx_vst3_audio;
  AgsFxVst3AudioProcessor *fx_vst3_audio_processor;
  AgsFxVst3Channel *fx_vst3_channel;
  AgsFxVst3ChannelProcessor *fx_vst3_channel_processor;

  AgsVst3Manager *vst3_manager;
  AgsVst3Plugin *vst3_plugin;
  
  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  gint channel_position;
  guint audio_channels;
  guint i, j;
  gboolean is_instrument;
  
  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-vst3 recall container not provided");
    
    return(NULL);
  }

  vst3_manager = ags_vst3_manager_get_instance();  

  vst3_plugin = ags_vst3_manager_find_vst3_plugin(vst3_manager,
						  filename, effect);
  
  is_instrument = ags_base_plugin_test_flags((AgsBasePlugin *) vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT);

  start_recall = NULL;

  start_output = NULL;

  start_input = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "output", &start_output,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);
  
  channel_position = 0;
  
  if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
    channel_position = ags_fx_factory_get_output_position(audio,
							  position);
  }else{
    channel_position = ags_fx_factory_get_input_position(audio,
							 position);
  }

  /* audio - play context */
  fx_vst3_audio = NULL;
  fx_vst3_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxVst3Audio */
    fx_vst3_audio = (AgsFxVst3Audio *) g_object_new(AGS_TYPE_FX_VST3_AUDIO,
						    "output-soundcard", output_soundcard,
						    "audio", audio,
						    "recall-container", play_container,
						    "filename", filename,
						    "effect", effect,			    
						    NULL);
    ags_recall_set_flags((AgsRecall *) fx_vst3_audio,
			 (AGS_RECALL_TEMPLATE));

    if(is_instrument){
      ags_recall_set_ability_flags((AgsRecall *) fx_vst3_audio,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
    }else{
      ags_recall_set_ability_flags((AgsRecall *) fx_vst3_audio,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
    }
  
    ags_recall_set_behaviour_flags((AgsRecall *) fx_vst3_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_vst3_audio,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_vst3_audio);
    
    start_recall = g_list_prepend(start_recall,
				  fx_vst3_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_vst3_audio));
    
    /* AgsFxVst3AudioProcessor */
    fx_vst3_audio_processor = (AgsFxVst3AudioProcessor *) g_object_new(AGS_TYPE_FX_VST3_AUDIO_PROCESSOR,
								       "output-soundcard", output_soundcard,
								       "audio", audio,
								       "recall-audio", fx_vst3_audio,
								       "recall-container", play_container,
								       "filename", filename,
								       "effect", effect,						    
								       NULL);
    ags_recall_set_flags((AgsRecall *) fx_vst3_audio_processor,
			 (AGS_RECALL_TEMPLATE));

    if(is_instrument){
      ags_recall_set_ability_flags((AgsRecall *) fx_vst3_audio_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
    }else{
      ags_recall_set_ability_flags((AgsRecall *) fx_vst3_audio_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
    }

    ags_recall_set_behaviour_flags((AgsRecall *) fx_vst3_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_vst3_audio_processor,
			    TRUE,
			    position);
    ags_recall_container_add(play_container,
			     fx_vst3_audio_processor);
    
    start_recall = g_list_prepend(start_recall,
				  fx_vst3_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_vst3_audio_processor));

    /* load */
    if((AGS_FX_FACTORY_LIVE & (create_flags)) == 0){
      ags_fx_vst3_audio_unset_flags(fx_vst3_audio,
				    AGS_FX_VST3_AUDIO_LIVE_INSTRUMENT);
    }
    
    ags_fx_vst3_audio_load_plugin(fx_vst3_audio);
    ags_fx_vst3_audio_load_port(fx_vst3_audio);
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_vst3_audio = ags_recall_container_get_recall_audio(play_container);

    if(fx_vst3_audio != NULL){
      g_object_ref(fx_vst3_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(play_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_vst3_audio_processor = recall_audio_run->data;
      g_object_ref(fx_vst3_audio_processor);
    }

    ags_fx_vst3_audio_load_plugin(fx_vst3_audio);
    ags_fx_vst3_audio_load_port(fx_vst3_audio);
  }

  /* channel - play context */  
  for(i = 0; i < stop_pad - start_pad; i++){
    if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
      channel = ags_channel_pad_nth(start_output,
				    start_pad + i);
    }else{
      channel = ags_channel_pad_nth(start_input,
				    start_pad + i);
    }

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* add recall container */
      ags_channel_add_recall_container(channel,
				       (GObject *) recall_container);
	
      /* AgsFxVst3Channel */
      fx_vst3_channel = (AgsFxVst3Channel *) g_object_new(AGS_TYPE_FX_VST3_CHANNEL,
							  "output-soundcard", output_soundcard,
							  "source", channel,
							  "recall-audio", fx_vst3_audio,
							  "recall-container", play_container,
							  "filename", filename,
							  "effect", effect,						    
							  NULL);
      ags_recall_set_flags((AgsRecall *) fx_vst3_channel,
			   (AGS_RECALL_TEMPLATE));

      if(is_instrument){
	ags_recall_set_ability_flags((AgsRecall *) fx_vst3_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_MIDI));
      }else{
	ags_recall_set_ability_flags((AgsRecall *) fx_vst3_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_WAVE |
				      AGS_SOUND_ABILITY_MIDI));
      }

      ags_recall_set_behaviour_flags((AgsRecall *) fx_vst3_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_vst3_channel,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_vst3_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_vst3_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_vst3_channel));

      /* AgsFxVst3ChannelProcessor */
      fx_vst3_channel_processor = (AgsFxVst3ChannelProcessor *) g_object_new(AGS_TYPE_FX_VST3_CHANNEL_PROCESSOR,
									     "output-soundcard", output_soundcard,
									     "source", channel,
									     "recall-audio", fx_vst3_audio,
									     "recall-audio-run", fx_vst3_audio_processor,
									     "recall-channel", fx_vst3_channel,
									     "recall-container", play_container,
									     "filename", filename,
									     "effect", effect,						    
									     NULL);
      ags_recall_set_flags((AgsRecall *) fx_vst3_channel_processor,
			   (AGS_RECALL_TEMPLATE));

      if(is_instrument){
	ags_recall_set_ability_flags((AgsRecall *) fx_vst3_channel_processor,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_MIDI));
      }else{
	ags_recall_set_ability_flags((AgsRecall *) fx_vst3_channel_processor,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_WAVE |
				      AGS_SOUND_ABILITY_MIDI));
      }

      ags_recall_set_behaviour_flags((AgsRecall *) fx_vst3_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_vst3_channel_processor,
				TRUE,
				channel_position);
      ags_recall_container_add(play_container,
			       fx_vst3_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_vst3_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_vst3_channel_processor));

      /* load */
      ags_fx_vst3_channel_load_plugin(fx_vst3_channel);
      ags_fx_vst3_channel_load_port(fx_vst3_channel);

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_vst3_audio != NULL){
      g_object_unref(fx_vst3_audio);
    }
    
    if(fx_vst3_audio_processor != NULL){
      g_object_unref(fx_vst3_audio_processor);
    }
  }
  
  /* audio - recall context */
  fx_vst3_audio = NULL;
  fx_vst3_audio_processor = NULL;
  
  if((AGS_FX_FACTORY_REMAP & (create_flags)) == 0){
    /* AgsFxVst3Audio */
    fx_vst3_audio = (AgsFxVst3Audio *) g_object_new(AGS_TYPE_FX_VST3_AUDIO,
						    "output-soundcard", output_soundcard,
						    "audio", audio,
						    "recall-container", recall_container,
						    "filename", filename,
						    "effect", effect,						    
						    NULL);
    ags_recall_set_flags((AgsRecall *) fx_vst3_audio,
			 (AGS_RECALL_TEMPLATE));

    if(is_instrument){
      ags_recall_set_ability_flags((AgsRecall *) fx_vst3_audio,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
    }else{
      ags_recall_set_ability_flags((AgsRecall *) fx_vst3_audio,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
    }

    ags_recall_set_behaviour_flags((AgsRecall *) fx_vst3_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_vst3_audio,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_vst3_audio);

    start_recall = g_list_prepend(start_recall,
				  fx_vst3_audio);

    ags_connectable_connect(AGS_CONNECTABLE(fx_vst3_audio));
    
    /* AgsFxVst3AudioProcessor */
    fx_vst3_audio_processor = (AgsFxVst3AudioProcessor *) g_object_new(AGS_TYPE_FX_VST3_AUDIO_PROCESSOR,
								       "output-soundcard", output_soundcard,
								       "audio", audio,
								       "recall-audio", fx_vst3_audio,
								       "recall-container", recall_container,
								       "filename", filename,
								       "effect", effect,						    
								       NULL);
    ags_recall_set_flags((AgsRecall *) fx_vst3_audio_processor,
			 (AGS_RECALL_TEMPLATE));

    if(is_instrument){
      ags_recall_set_ability_flags((AgsRecall *) fx_vst3_audio_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_MIDI));
    }else{
      ags_recall_set_ability_flags((AgsRecall *) fx_vst3_audio_processor,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_WAVE |
				    AGS_SOUND_ABILITY_MIDI));
    }

    ags_recall_set_behaviour_flags((AgsRecall *) fx_vst3_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_insert_recall(audio, (GObject *) fx_vst3_audio_processor,
			    FALSE,
			    position);
    ags_recall_container_add(recall_container,
			     fx_vst3_audio_processor);
    
    start_recall = g_list_prepend(start_recall,
				  fx_vst3_audio_processor);

    ags_connectable_connect(AGS_CONNECTABLE(fx_vst3_audio_processor));

    /* load */
    if((AGS_FX_FACTORY_LIVE & (create_flags)) == 0){
      ags_fx_vst3_audio_unset_flags(fx_vst3_audio,
				    AGS_FX_VST3_AUDIO_LIVE_INSTRUMENT);
    }
    
    ags_fx_vst3_audio_load_plugin(fx_vst3_audio);
    ags_fx_vst3_audio_load_port(fx_vst3_audio);
  }else{
    GList *start_recall_audio_run, *recall_audio_run;
    
    fx_vst3_audio = ags_recall_container_get_recall_audio(recall_container);

    if(fx_vst3_audio != NULL){
      g_object_ref(fx_vst3_audio);
    }

    start_recall_audio_run = ags_recall_container_get_recall_audio_run(recall_container);

    if((recall_audio_run = ags_recall_find_template(start_recall_audio_run)) != NULL){
      fx_vst3_audio_processor = recall_audio_run->data;
      g_object_ref(fx_vst3_audio_processor);
    }
  }

  /* channel - recall context */  
  for(i = 0; i < stop_pad - start_pad; i++){
    if((AGS_FX_FACTORY_OUTPUT & create_flags) != 0){
      channel = ags_channel_pad_nth(start_output,
				    start_pad + i);
    }else{
      channel = ags_channel_pad_nth(start_input,
				    start_pad + i);
    }

    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
    
    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      /* AgsFxVst3Channel */
      fx_vst3_channel = (AgsFxVst3Channel *) g_object_new(AGS_TYPE_FX_VST3_CHANNEL,
							  "output-soundcard", output_soundcard,
							  "source", channel,
							  "recall-audio", fx_vst3_audio,
							  "recall-container", recall_container,
							  "filename", filename,
							  "effect", effect,						    
							  NULL);
      ags_recall_set_flags((AgsRecall *) fx_vst3_channel,
			   (AGS_RECALL_TEMPLATE));

      if(is_instrument){
	ags_recall_set_ability_flags((AgsRecall *) fx_vst3_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_MIDI));
      }else{
	ags_recall_set_ability_flags((AgsRecall *) fx_vst3_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_WAVE |
				      AGS_SOUND_ABILITY_MIDI));
      }

      ags_recall_set_behaviour_flags((AgsRecall *) fx_vst3_channel,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_vst3_channel,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_vst3_channel);

      start_recall = g_list_prepend(start_recall,
				    fx_vst3_channel);

      ags_connectable_connect(AGS_CONNECTABLE(fx_vst3_channel));

      /* AgsFxVst3ChannelProcessor */
      fx_vst3_channel_processor = (AgsFxVst3ChannelProcessor *) g_object_new(AGS_TYPE_FX_VST3_CHANNEL_PROCESSOR,
									     "output-soundcard", output_soundcard,
									     "source", channel,
									     "recall-audio", fx_vst3_audio,
									     "recall-audio-run", fx_vst3_audio_processor,
									     "recall-channel", fx_vst3_channel,
									     "recall-container", recall_container,
									     "filename", filename,
									     "effect", effect,						    
									     NULL);
      ags_recall_set_flags((AgsRecall *) fx_vst3_channel_processor,
			   (AGS_RECALL_TEMPLATE));

      if(is_instrument){
	ags_recall_set_ability_flags((AgsRecall *) fx_vst3_channel_processor,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_MIDI));
      }else{
	ags_recall_set_ability_flags((AgsRecall *) fx_vst3_channel_processor,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_WAVE |
				      AGS_SOUND_ABILITY_MIDI));
      }

      ags_recall_set_behaviour_flags((AgsRecall *) fx_vst3_channel_processor,
				     (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_channel_insert_recall(channel, (GObject *) fx_vst3_channel_processor,
				FALSE,
				channel_position);
      ags_recall_container_add(recall_container,
			       fx_vst3_channel_processor);
      
      start_recall = g_list_prepend(start_recall,
				    fx_vst3_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_vst3_channel_processor));

      /* load */
      ags_fx_vst3_channel_load_plugin(fx_vst3_channel);
      ags_fx_vst3_channel_load_port(fx_vst3_channel);

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }

  if((AGS_FX_FACTORY_REMAP & (create_flags)) != 0){
    if(fx_vst3_audio != NULL){
      g_object_unref(fx_vst3_audio);
    }
    
    if(fx_vst3_audio_processor != NULL){
      g_object_unref(fx_vst3_audio_processor);
    }
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(input_sequencer != NULL){
    g_object_unref(input_sequencer);
  }
  
  if(start_output != NULL){
    g_object_unref(start_output);
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }
  
  return(start_recall);
}
#endif

/**
 * ags_fx_factory_create:
 * @audio: an #AgsAudio that should keep the recalls
 * @play_container: an #AgsRecallContainer to indetify what recall to use
 * @recall_container: an #AgsRecallContainer to indetify what recall to use
 * @plugin_name: the plugin identifier to instantiate
 * @filename: the plugin filename
 * @effect: the plugin effect
 * @start_audio_channel: the first audio channel to apply
 * @stop_audio_channel: the last audio channel to apply
 * @start_pad: the first pad to apply
 * @stop_pad: the last pad to apply
 * @position: the position to insert the recall
 * @create_flags: modify the behaviour of this function
 * @recall_flags: flags to be set for #AgsRecall
 *
 * Instantiate #AgsRecall by this factory.
 *
 * Returns: (element-type AgsAudio.Recall) (transfer full): The #GList-struct containing #AgsRecall
 * 
 * Since: 3.3.0
 */
GList*
ags_fx_factory_create(AgsAudio *audio,
		      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
		      gchar *plugin_name,
		      gchar *filename,
		      gchar *effect,
		      guint start_audio_channel, guint stop_audio_channel,
		      guint start_pad, guint stop_pad,
		      gint position,
		      guint create_flags, guint recall_flags)
{
  GList *start_recall;

  if(!AGS_IS_AUDIO(audio) ||
     plugin_name == NULL){
    return(NULL);
  }
  
  start_recall = NULL;

  if(!g_ascii_strncasecmp(plugin_name,
			  "ags-fx-buffer",
			  14)){
    start_recall = ags_fx_factory_create_buffer(audio,
						play_container, recall_container,
						plugin_name,
						filename,
						effect,
						start_audio_channel, stop_audio_channel,
						start_pad, stop_pad,
						position,
						create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-playback",
				16)){
    start_recall = ags_fx_factory_create_playback(audio,
						  play_container, recall_container,
						  plugin_name,
						  filename,
						  effect,
						  start_audio_channel, stop_audio_channel,
						  start_pad, stop_pad,
						  position,
						  create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-volume",
				14)){
    start_recall = ags_fx_factory_create_volume(audio,
						play_container, recall_container,
						plugin_name,
						filename,
						effect,
						start_audio_channel, stop_audio_channel,
						start_pad, stop_pad,
						position,
						create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-low-pass",
				16)){
    start_recall = ags_fx_factory_create_low_pass(audio,
						  play_container, recall_container,
						  plugin_name,
						  filename,
						  effect,
						  start_audio_channel, stop_audio_channel,
						  start_pad, stop_pad,
						  position,
						  create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-high-pass",
				17)){
    start_recall = ags_fx_factory_create_high_pass(audio,
						   play_container, recall_container,
						   plugin_name,
						   filename,
						   effect,
						   start_audio_channel, stop_audio_channel,
						   start_pad, stop_pad,
						   position,
						   create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-peak",
				12)){
    start_recall = ags_fx_factory_create_peak(audio,
					      play_container, recall_container,
					      plugin_name,
					      filename,
					      effect,
					      start_audio_channel, stop_audio_channel,
					      start_pad, stop_pad,
					      position,
					      create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-eq10",
				12)){
    start_recall = ags_fx_factory_create_eq10(audio,
					      play_container, recall_container,
					      plugin_name,
					      filename,
					      effect,
					      start_audio_channel, stop_audio_channel,
					      start_pad, stop_pad,
					      position,
					      create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-analyse",
				15)){
    start_recall = ags_fx_factory_create_analyse(audio,
						 play_container, recall_container,
						 plugin_name,
						 filename,
						 effect,
						 start_audio_channel, stop_audio_channel,
						 start_pad, stop_pad,
						 position,
						 create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-two-pass-aliase",
				22)){
    start_recall = ags_fx_factory_create_two_pass_aliase(audio,
							 play_container, recall_container,
							 plugin_name,
							 filename,
							 effect,
							 start_audio_channel, stop_audio_channel,
							 start_pad, stop_pad,
							 position,
							 create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-lfo",
				11)){
    start_recall = ags_fx_factory_create_lfo(audio,
					     play_container, recall_container,
					     plugin_name,
					     filename,
					     effect,
					     start_audio_channel, stop_audio_channel,
					     start_pad, stop_pad,
					     position,
					     create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-envelope",
				16)){
    start_recall = ags_fx_factory_create_envelope(audio,
						  play_container, recall_container,
						  plugin_name,
						  filename,
						  effect,
						  start_audio_channel, stop_audio_channel,
						  start_pad, stop_pad,
						  position,
						  create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-pattern",
				15)){
    start_recall = ags_fx_factory_create_pattern(audio,
						 play_container, recall_container,
						 plugin_name,
						 filename,
						 effect,
						 start_audio_channel, stop_audio_channel,
						 start_pad, stop_pad,
						 position,
						 create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-notation",
				16)){
    start_recall = ags_fx_factory_create_notation(audio,
						  play_container, recall_container,
						  plugin_name,
						  filename,
						  effect,
						  start_audio_channel, stop_audio_channel,
						  start_pad, stop_pad,
						  position,
						  create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-synth",
				13)){
    start_recall = ags_fx_factory_create_synth(audio,
					       play_container, recall_container,
					       plugin_name,
					       filename,
					       effect,
					       start_audio_channel, stop_audio_channel,
					       start_pad, stop_pad,
					       position,
					       create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-fm-synth",
				16)){
    start_recall = ags_fx_factory_create_fm_synth(audio,
						  play_container, recall_container,
						  plugin_name,
						  filename,
						  effect,
						  start_audio_channel, stop_audio_channel,
						  start_pad, stop_pad,
						  position,
						  create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-sf2-synth",
				17)){
    start_recall = ags_fx_factory_create_sf2_synth(audio,
						   play_container, recall_container,
						   plugin_name,
						   filename,
						   effect,
						   start_audio_channel, stop_audio_channel,
						   start_pad, stop_pad,
						   position,
						   create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-sfz-synth",
				17)){
    start_recall = ags_fx_factory_create_sfz_synth(audio,
						   play_container, recall_container,
						   plugin_name,
						   filename,
						   effect,
						   start_audio_channel, stop_audio_channel,
						   start_pad, stop_pad,
						   position,
						   create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-ladspa",
				14)){
    start_recall = ags_fx_factory_create_ladspa(audio,
						play_container, recall_container,
						plugin_name,
						filename,
						effect,
						start_audio_channel, stop_audio_channel,
						start_pad, stop_pad,
						position,
						create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-dssi",
				12)){
    start_recall = ags_fx_factory_create_dssi(audio,
					      play_container, recall_container,
					      plugin_name,
					      filename,
					      effect,
					      start_audio_channel, stop_audio_channel,
					      start_pad, stop_pad,
					      position,
					      create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-lv2",
				11)){
    start_recall = ags_fx_factory_create_lv2(audio,
					     play_container, recall_container,
					     plugin_name,
					     filename,
					     effect,
					     start_audio_channel, stop_audio_channel,
					     start_pad, stop_pad,
					     position,
					     create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-vst3",
				12)){
#if defined(AGS_WITH_VST3)
    start_recall = ags_fx_factory_create_vst3(audio,
					      play_container, recall_container,
					      plugin_name,
					      filename,
					      effect,
					      start_audio_channel, stop_audio_channel,
					      start_pad, stop_pad,
					      position,
					      create_flags, recall_flags);
#endif
  }else{
    g_warning("no such plugin - %s", plugin_name);
  }

  return(start_recall);
}
