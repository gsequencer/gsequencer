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

/**
 * SECTION:ags_fx_factory
 * @short_description: Factory pattern
 * @title: AgsFxFactory
 * @section_id:
 * @include: ags/audio/ags_fx_factory.h
 *
 * Factory function to instantiate fx recalls.
 */

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
  AgsChannel *input;
  AgsChannel *channel, *next_channel;

  AgsFxPlaybackAudio *fx_playback_audio;
  AgsFxPlaybackAudioProcessor *fx_playback_audio_processor;
  AgsFxPlaybackChannel *fx_playback_channel;
  AgsFxPlaybackChannelProcessor *fx_playback_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

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
  
  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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

  if(input != NULL){
    g_object_unref(input);
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  AgsChannel *input;
  AgsChannel *channel, *next_channel;

  AgsFxBufferAudio *fx_buffer_audio;
  AgsFxBufferAudioProcessor *fx_buffer_audio_processor;
  AgsFxBufferChannel *fx_buffer_channel;
  AgsFxBufferChannelProcessor *fx_buffer_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

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
  
  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  
  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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

  if(input != NULL){
    g_object_unref(input);
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  AgsChannel *input;
  AgsChannel *channel, *next_channel;

  AgsFxVolumeAudio *fx_volume_audio;
  AgsFxVolumeAudioProcessor *fx_volume_audio_processor;
  AgsFxVolumeChannel *fx_volume_channel;
  AgsFxVolumeChannelProcessor *fx_volume_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

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
  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  
  /* channel - recall context */
  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){
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
				position);
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
				position);
      ags_recall_container_add(recall_container,
			       fx_volume_channel_processor);

      start_recall = g_list_prepend(start_recall,
				    fx_volume_channel_processor);

      ags_connectable_connect(AGS_CONNECTABLE(fx_volume_channel_processor));

      /* iterate */
      next_channel = ags_channel_nth(channel,
				     audio_channels - stop_audio_channel);

      if(channel != NULL){
	g_object_unref(channel);
      }
      
      channel = next_channel;
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

  if(input != NULL){
    g_object_unref(input);
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  AgsChannel *input;
  AgsChannel *channel, *next_channel;

  AgsFxPeakAudio *fx_peak_audio;
  AgsFxPeakAudioProcessor *fx_peak_audio_processor;
  AgsFxPeakChannel *fx_peak_channel;
  AgsFxPeakChannelProcessor *fx_peak_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

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
  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  
  /* channel - recall context */
  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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

  if(input != NULL){
    g_object_unref(input);
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  AgsChannel *input;
  AgsChannel *channel, *next_channel;

  AgsFxEq10Audio *fx_eq10_audio;
  AgsFxEq10AudioProcessor *fx_eq10_audio_processor;
  AgsFxEq10Channel *fx_eq10_channel;
  AgsFxEq10ChannelProcessor *fx_eq10_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

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
  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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

  if(input != NULL){
    g_object_unref(input);
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  AgsChannel *input;
  AgsChannel *channel, *next_channel;

  AgsFxAnalyseAudio *fx_analyse_audio;
  AgsFxAnalyseAudioProcessor *fx_analyse_audio_processor;
  AgsFxAnalyseChannel *fx_analyse_channel;
  AgsFxAnalyseChannelProcessor *fx_analyse_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

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
  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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

  if(input != NULL){
    g_object_unref(input);
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  AgsChannel *input;
  AgsChannel *channel, *next_channel;

  AgsFxEnvelopeAudio *fx_envelope_audio;
  AgsFxEnvelopeAudioProcessor *fx_envelope_audio_processor;
  AgsFxEnvelopeChannel *fx_envelope_channel;
  AgsFxEnvelopeChannelProcessor *fx_envelope_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

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
  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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

  if(input != NULL){
    g_object_unref(input);
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  AgsChannel *input;
  AgsChannel *channel, *next_channel;

  AgsFxPatternAudio *fx_pattern_audio;
  AgsFxPatternAudioProcessor *fx_pattern_audio_processor;
  AgsFxPatternChannel *fx_pattern_channel;
  AgsFxPatternChannelProcessor *fx_pattern_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

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
  
  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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

  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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

  if(input != NULL){
    g_object_unref(input);
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  AgsChannel *input;
  AgsChannel *channel, *next_channel;

  AgsFxNotationAudio *fx_notation_audio;
  AgsFxNotationAudioProcessor *fx_notation_audio_processor;
  AgsFxNotationChannel *fx_notation_channel;
  AgsFxNotationChannelProcessor *fx_notation_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

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

  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){      
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  
  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){      
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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

  if(input != NULL){
    g_object_unref(input);
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  AgsChannel *start_input;
  AgsChannel *input;
  AgsChannel *channel, *next_channel;

  AgsFxLadspaAudio *fx_ladspa_audio;
  AgsFxLadspaAudioProcessor *fx_ladspa_audio_processor;
  AgsFxLadspaChannel *fx_ladspa_channel;
  AgsFxLadspaChannelProcessor *fx_ladspa_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  guint audio_channels;
  guint i, j;
  
  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-ladspa recall container not provided");
    
    return(NULL);
  }
  
  start_recall = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);
  
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
  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  if(input != NULL){
    g_object_unref(input);
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  AgsChannel *start_input;
  AgsChannel *input;
  AgsChannel *channel, *next_channel;

  AgsFxDssiAudio *fx_dssi_audio;
  AgsFxDssiAudioProcessor *fx_dssi_audio_processor;
  AgsFxDssiChannel *fx_dssi_channel;
  AgsFxDssiChannelProcessor *fx_dssi_channel_processor;

  GObject *output_soundcard;
  GObject *input_sequencer;  
  
  GList *start_recall;

  guint audio_channels;
  guint i, j;
  
  if(!AGS_IS_RECALL_CONTAINER(play_container) ||
     !AGS_IS_RECALL_CONTAINER(recall_container)){
    g_warning("ags-fx-dssi recall container not provided");
    
    return(NULL);
  }
  
  start_recall = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);

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

  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  if(input != NULL){
    g_object_unref(input);
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  AgsChannel *start_input;
  AgsChannel *input;
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
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);
  
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

  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  for(i = 0; i < stop_pad - start_pad && channel != NULL; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel && channel != NULL; j++){
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
				position);
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
				position);
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

    /* iterate */
    next_channel = ags_channel_nth(channel,
				   audio_channels - stop_audio_channel);

    if(channel != NULL){
      g_object_unref(channel);
    }
      
    channel = next_channel;
  }

  if(channel != NULL){
    g_object_unref(channel);
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
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  if(input != NULL){
    g_object_unref(input);
  }

  if(channel != NULL){
    g_object_unref(channel);
  }
  
  return(start_recall);
}

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
  }else{
    g_warning("no such plugin - %s", plugin_name);
  }

  return(start_recall);
}
