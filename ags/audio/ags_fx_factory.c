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

#include <ags/audio/fx/ags_fx_dssi_audio.h>
#include <ags/audio/fx/ags_fx_dssi_audio_processor.h>
#include <ags/audio/fx/ags_fx_dssi_channel.h>
#include <ags/audio/fx/ags_fx_dssi_channel_processor.h>
#include <ags/audio/fx/ags_fx_dssi_recycling.h>
#include <ags/audio/fx/ags_fx_dssi_audio_signal.h>

/**
 * SECTION:ags_fx_factory
 * @short_description: Factory pattern
 * @title: AgsFxFactory
 * @section_id:
 * @include: ags/audio/ags_fx_factory.h
 *
 * Factory function to instantiate fx recalls.
 */

GList* ags_fx_factory_create_pattern(AgsAudio *audio,
				     AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				     gchar *plugin_name,
				     gchar *filename,
				     gchar *effect,
				     guint start_audio_channel, guint stop_audio_channel,
				     guint start_pad, guint stop_pad,
				     guint create_flags, guint recall_flags);

GList* ags_fx_factory_create_notation(AgsAudio *audio,
				      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				      gchar *plugin_name,
				      gchar *filename,
				      gchar *effect,
				      guint start_audio_channel, guint stop_audio_channel,
				      guint start_pad, guint stop_pad,
				      guint create_flags, guint recall_flags);

GList* ags_fx_factory_create_dssi(AgsAudio *audio,
				  AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				  gchar *plugin_name,
				  gchar *filename,
				  gchar *effect,
				  guint start_audio_channel, guint stop_audio_channel,
				  guint start_pad, guint stop_pad,
				  guint create_flags, guint recall_flags);

GList*
ags_fx_factory_create_pattern(AgsAudio *audio,
			      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			      gchar *plugin_name,
			      gchar *filename,
			      gchar *effect,
			      guint start_audio_channel, guint stop_audio_channel,
			      guint start_pad, guint stop_pad,
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
  
  GList *start_container, *container;
  GList *start_recall;

  gboolean found_play_container;
  guint audio_channels;
  guint i, j;
  
  if((AGS_FX_FACTORY_PLAY & (create_flags)) == 0){
    return(NULL);
  }
  
  start_recall = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "input", &start_input,
	       "recall-container", &start_container,
	       "audio-channels", &audio_channels,
	       NULL);
  
  found_play_container = FALSE;

  container = ags_recall_container_find(start_container,
					AGS_TYPE_FX_PATTERN_AUDIO,
					(AGS_RECALL_CONTAINER_FIND_TYPE |
					 AGS_RECALL_CONTAINER_FIND_TEMPLATE),
					NULL);

  if(container != NULL){
    found_play_container = TRUE;

    if(play_container == NULL){
      play_container = container->data;
    }
  }

  g_list_free_full(start_container,
		   (GDestroyNotify) g_object_unref);
  
  if(!found_play_container){
    if(play_container == NULL){
      play_container = ags_recall_container_new();

      ags_recall_container_set_flags(play_container,
				     AGS_RECALL_CONTAINER_PLAY);

      ags_audio_add_recall_container(audio,
				     (GObject *) play_container);
    }
  }

  fx_pattern_audio = NULL;
  fx_pattern_audio_processor = NULL;
  
  if(!found_play_container){
    /* AgsFxPatternAudio */
    fx_pattern_audio = (AgsFxPatternAudio *) g_object_new(AGS_TYPE_FX_PATTERN_AUDIO,
							  "output-soundcard", output_soundcard,
							  "input-sequencer", input_sequencer,
							  "audio", audio,
							  "recall-container", play_container,
							  NULL);
    ags_recall_set_flags((AgsRecall *) fx_pattern_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_pattern_audio,
				 (AGS_SOUND_ABILITY_SEQUENCER));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_pattern_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) fx_pattern_audio, TRUE);
    start_recall = g_list_prepend(start_recall,
				  fx_pattern_audio);

    g_object_set(play_container,
		 "recall-audio", fx_pattern_audio,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(fx_pattern_audio));

    /* AgsFxPatternAudioProcessor */
    fx_pattern_audio_processor = (AgsFxPatternAudioProcessor *) g_object_new(AGS_TYPE_FX_PATTERN_AUDIO_PROCESSOR,
									     "output-soundcard", output_soundcard,
									     "input-sequencer", input_sequencer,
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

    ags_audio_add_recall(audio, (GObject *) fx_pattern_audio_processor, TRUE);
    start_recall = g_list_prepend(start_recall,
				  fx_pattern_audio_processor);

    g_object_set(play_container,
		 "recall-audio-run", fx_pattern_audio_processor,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(fx_pattern_audio_processor));
  }else{
    GList *start_list, *list;
    
    fx_pattern_audio = ags_recall_container_get_recall_audio(play_container);
    g_object_unref(fx_pattern_audio);
    
    start_list = ags_recall_container_get_recall_audio_run(play_container);

    list = ags_recall_find_template(start_list);

    if(list != NULL){
      fx_pattern_audio_processor = list->data;
    }

    g_list_free_full(start_list,
		     (GDestroyNotify) g_object_unref);
  }

  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;
  g_object_ref(channel);

  for(i = 0; i < stop_pad - start_pad; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      AgsRecallContainer *current_play_container;

      current_play_container = NULL;
      
      g_object_get(channel,
		   "recall-container", &start_container,
		   NULL);

      container = ags_recall_container_find(start_container,
					    AGS_TYPE_FX_PATTERN_CHANNEL,
					    (AGS_RECALL_CONTAINER_FIND_TYPE |
					     AGS_RECALL_CONTAINER_FIND_TEMPLATE),
					    NULL);

      if(container != NULL){
	current_play_container = container->data;
      }

      g_list_free_full(start_container,
		       (GDestroyNotify) g_object_unref);
      
      /* add recall container */
      if(current_play_container == NULL){
	ags_channel_add_recall_container(channel,
					 (GObject *) play_container);
	
	/* AgsFxPatternChannel */
	fx_pattern_channel = (AgsFxPatternChannel *) g_object_new(AGS_TYPE_FX_PATTERN_CHANNEL,
								  "output-soundcard", output_soundcard,
								  "input-sequencer", input_sequencer,
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

	ags_channel_add_recall(channel, (GObject *) fx_pattern_channel, TRUE);
	start_recall = g_list_prepend(start_recall,
				      fx_pattern_channel);

	g_object_set(play_container,
		     "recall-channel", fx_pattern_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(fx_pattern_channel));

	/* AgsFxPatternChannelProcessor */
	fx_pattern_channel_processor = (AgsFxPatternChannelProcessor *) g_object_new(AGS_TYPE_FX_PATTERN_CHANNEL_PROCESSOR,
										     "output-soundcard", output_soundcard,
										     "input-sequencer", input_sequencer,
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

	ags_channel_add_recall(channel, (GObject *) fx_pattern_channel_processor, TRUE);
	start_recall = g_list_prepend(start_recall,
				      fx_pattern_channel_processor);

	g_object_set(play_container,
		     "recall-channel-run", fx_pattern_channel_processor,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(fx_pattern_channel_processor));
      }else{
	g_warning("ags-fx-pattern recall already added");
      }

      /* iterate */
      next_channel = ags_channel_nth(channel,
				     audio_channels - stop_audio_channel);

      if(channel != NULL){
	g_object_unref(channel);
      }
      
      channel = next_channel;
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
  
  GList *start_container, *container;
  GList *start_recall;

  gboolean found_play_container;
  guint audio_channels;
  guint i, j;
  
  if((AGS_FX_FACTORY_PLAY & (create_flags)) == 0){
    return(NULL);
  }
  
  start_recall = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "input", &start_input,
	       "recall-container", &start_container,
	       "audio-channels", &audio_channels,
	       NULL);
  
  found_play_container = FALSE;

  container = ags_recall_container_find(start_container,
					AGS_TYPE_FX_NOTATION_AUDIO,
					(AGS_RECALL_CONTAINER_FIND_TYPE |
					 AGS_RECALL_CONTAINER_FIND_TEMPLATE),
					NULL);

  if(container != NULL){
    found_play_container = TRUE;

    if(play_container == NULL){
      play_container = container->data;
    }
  }

  g_list_free_full(start_container,
		   (GDestroyNotify) g_object_unref);
  
  if(!found_play_container){
    if(play_container == NULL){
      play_container = ags_recall_container_new();

      ags_recall_container_set_flags(play_container,
				     AGS_RECALL_CONTAINER_PLAY);

      ags_audio_add_recall_container(audio,
				     (GObject *) play_container);
    }
  }

  fx_notation_audio = NULL;
  fx_notation_audio_processor = NULL;
  
  if(!found_play_container){
    /* AgsFxNotationAudio */
    fx_notation_audio = (AgsFxNotationAudio *) g_object_new(AGS_TYPE_FX_NOTATION_AUDIO,
							    "output-soundcard", output_soundcard,
							    "input-sequencer", input_sequencer,
							    "audio", audio,
							    "recall-container", play_container,
							    NULL);
    ags_recall_set_flags((AgsRecall *) fx_notation_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_notation_audio,
				 (AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_notation_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) fx_notation_audio, TRUE);
    start_recall = g_list_prepend(start_recall,
				  fx_notation_audio);

    g_object_set(play_container,
		 "recall-audio", fx_notation_audio,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(fx_notation_audio));

    /* AgsFxNotationAudioProcessor */
    fx_notation_audio_processor = (AgsFxNotationAudioProcessor *) g_object_new(AGS_TYPE_FX_NOTATION_AUDIO_PROCESSOR,
									       "output-soundcard", output_soundcard,
									       "input-sequencer", input_sequencer,
									       "audio", audio,
									       "recall-audio", fx_notation_audio,
									       "recall-container", play_container,
									       NULL);
    ags_recall_set_flags((AgsRecall *) fx_notation_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_notation_audio_processor,
				 (AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_notation_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) fx_notation_audio_processor, TRUE);
    start_recall = g_list_prepend(start_recall,
				  fx_notation_audio_processor);

    g_object_set(play_container,
		 "recall-audio-run", fx_notation_audio_processor,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(fx_notation_audio_processor));
  }else{
    GList *start_list, *list;
    
    fx_notation_audio = ags_recall_container_get_recall_audio(play_container);
    g_object_unref(fx_notation_audio);
    
    start_list = ags_recall_container_get_recall_audio_run(play_container);

    list = ags_recall_find_template(start_list);

    if(list != NULL){
      fx_notation_audio_processor = list->data;
    }

    g_list_free_full(start_list,
		     (GDestroyNotify) g_object_unref);
  }

  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;
  g_object_ref(channel);

  for(i = 0; i < stop_pad - start_pad; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      AgsRecallContainer *current_play_container;

      current_play_container = NULL;
      
      g_object_get(channel,
		   "recall-container", &start_container,
		   NULL);

      container = ags_recall_container_find(start_container,
					    AGS_TYPE_FX_NOTATION_CHANNEL,
					    (AGS_RECALL_CONTAINER_FIND_TYPE |
					     AGS_RECALL_CONTAINER_FIND_TEMPLATE),
					    NULL);

      if(container != NULL){
	current_play_container = container->data;
      }

      g_list_free_full(start_container,
		       (GDestroyNotify) g_object_unref);
      
      /* add recall container */
      if(current_play_container == NULL){
	ags_channel_add_recall_container(channel,
					 (GObject *) play_container);
	
	/* AgsFxNotationChannel */
	fx_notation_channel = (AgsFxNotationChannel *) g_object_new(AGS_TYPE_FX_NOTATION_CHANNEL,
								    "output-soundcard", output_soundcard,
								    "input-sequencer", input_sequencer,
								    "source", channel,
								    "recall-audio", fx_notation_audio,
								    "recall-container", play_container,
								    NULL);
	ags_recall_set_flags((AgsRecall *) fx_notation_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags((AgsRecall *) fx_notation_channel,
				     (AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_MIDI));
	ags_recall_set_behaviour_flags((AgsRecall *) fx_notation_channel,
				       (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) fx_notation_channel, TRUE);
	start_recall = g_list_prepend(start_recall,
				      fx_notation_channel);

	g_object_set(play_container,
		     "recall-channel", fx_notation_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(fx_notation_channel));

	/* AgsFxNotationChannelProcessor */
	fx_notation_channel_processor = (AgsFxNotationChannelProcessor *) g_object_new(AGS_TYPE_FX_NOTATION_CHANNEL_PROCESSOR,
										       "output-soundcard", output_soundcard,
										       "input-sequencer", input_sequencer,
										       "source", channel,
										       "recall-audio", fx_notation_audio,
										       "recall-audio-run", fx_notation_audio_processor,
										       "recall-channel", fx_notation_channel,
										       "recall-container", play_container,
										       NULL);
	ags_recall_set_flags((AgsRecall *) fx_notation_channel_processor,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags((AgsRecall *) fx_notation_channel_processor,
				     (AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_MIDI));
	ags_recall_set_behaviour_flags((AgsRecall *) fx_notation_channel_processor,
				       (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) fx_notation_channel_processor, TRUE);
	start_recall = g_list_prepend(start_recall,
				      fx_notation_channel_processor);

	g_object_set(play_container,
		     "recall-channel-run", fx_notation_channel_processor,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(fx_notation_channel_processor));
      }else{
	g_warning("ags-fx-notation recall already added");
      }

      /* iterate */
      next_channel = ags_channel_nth(channel,
				     audio_channels - stop_audio_channel);

      if(channel != NULL){
	g_object_unref(channel);
      }
      
      channel = next_channel;
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
  
  GList *start_container, *container;
  GList *start_recall;

  gboolean found_play_container;
  guint audio_channels;
  guint i, j;
  
  if((AGS_FX_FACTORY_PLAY & (create_flags)) == 0){
    return(NULL);
  }
  
  start_recall = NULL;
  
  g_object_get(audio,
	       "output-soundcard", &output_soundcard,
	       "input-sequencer", &input_sequencer,
	       "input", &start_input,
	       "recall-container", &start_container,
	       "audio-channels", &audio_channels,
	       NULL);
  
  found_play_container = FALSE;

  container = ags_recall_container_find(start_container,
					AGS_TYPE_FX_DSSI_AUDIO,
					(AGS_RECALL_CONTAINER_FIND_TYPE |
					 AGS_RECALL_CONTAINER_FIND_TEMPLATE),
					NULL);

  if(container != NULL){
    found_play_container = TRUE;

    if(play_container == NULL){
      play_container = container->data;
    }
  }

  g_list_free_full(start_container,
		   (GDestroyNotify) g_object_unref);
  
  if(!found_play_container){
    if(play_container == NULL){
      play_container = ags_recall_container_new();

      ags_recall_container_set_flags(play_container,
				     AGS_RECALL_CONTAINER_PLAY);

      ags_audio_add_recall_container(audio,
				     (GObject *) play_container);
    }
  }

  fx_dssi_audio = NULL;
  fx_dssi_audio_processor = NULL;
  
  if(!found_play_container){
    /* AgsFxDssiAudio */
    fx_dssi_audio = (AgsFxDssiAudio *) g_object_new(AGS_TYPE_FX_DSSI_AUDIO,
						    "output-soundcard", output_soundcard,
						    "input-sequencer", input_sequencer,
						    "audio", audio,
						    "recall-container", play_container,
						    "filename", filename,
						    "effect", effect,						    
						    NULL);
    ags_recall_set_flags((AgsRecall *) fx_dssi_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_dssi_audio,
				 (AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_dssi_audio,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) fx_dssi_audio, TRUE);
    start_recall = g_list_prepend(start_recall,
				  fx_dssi_audio);

    g_object_set(play_container,
		 "recall-audio", fx_dssi_audio,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(fx_dssi_audio));
    
    /* AgsFxDssiAudioProcessor */
    fx_dssi_audio_processor = (AgsFxDssiAudioProcessor *) g_object_new(AGS_TYPE_FX_DSSI_AUDIO_PROCESSOR,
								       "output-soundcard", output_soundcard,
								       "input-sequencer", input_sequencer,
								       "audio", audio,
								       "recall-audio", fx_dssi_audio,
								       "recall-container", play_container,
								       "filename", filename,
								       "effect", effect,						    
								       NULL);
    ags_recall_set_flags((AgsRecall *) fx_dssi_audio_processor,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags((AgsRecall *) fx_dssi_audio_processor,
				 (AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags((AgsRecall *) fx_dssi_audio_processor,
				   (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) fx_dssi_audio_processor, TRUE);
    start_recall = g_list_prepend(start_recall,
				  fx_dssi_audio_processor);

    g_object_set(play_container,
		 "recall-audio-run", fx_dssi_audio_processor,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(fx_dssi_audio_processor));

    /* load */
    if((AGS_FX_FACTORY_LIVE & (create_flags)) == 0){
      ags_fx_dssi_audio_unset_flags(fx_dssi_audio,
				    AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT);
    }
    
    ags_fx_dssi_audio_load_plugin(fx_dssi_audio);
    ags_fx_dssi_audio_load_port(fx_dssi_audio);
  }else{
    GList *start_list, *list;
    
    fx_dssi_audio = ags_recall_container_get_recall_audio(play_container);
    g_object_unref(fx_dssi_audio);
    
    start_list = ags_recall_container_get_recall_audio_run(play_container);

    list = ags_recall_find_template(start_list);

    if(list != NULL){
      fx_dssi_audio_processor = list->data;
    }

    g_list_free_full(start_list,
		     (GDestroyNotify) g_object_unref);
  }

  input = ags_channel_nth(start_input,
			  start_pad * audio_channels);

  channel = input;
  g_object_ref(channel);

  for(i = 0; i < stop_pad - start_pad; i++){
    next_channel = ags_channel_nth(channel,
				   start_audio_channel);

    g_object_unref(channel);

    channel = next_channel;
      
    for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
      AgsRecallContainer *current_play_container;

      current_play_container = NULL;
      
      g_object_get(channel,
		   "recall-container", &start_container,
		   NULL);

      container = ags_recall_container_find(start_container,
					    AGS_TYPE_FX_DSSI_CHANNEL,
					    (AGS_RECALL_CONTAINER_FIND_TYPE |
					     AGS_RECALL_CONTAINER_FIND_TEMPLATE),
					    NULL);

      if(container != NULL){
	current_play_container = container->data;
      }

      g_list_free_full(start_container,
		       (GDestroyNotify) g_object_unref);
      
      /* add recall container */
      if(current_play_container == NULL){
	ags_channel_add_recall_container(channel,
					 (GObject *) play_container);
	
	/* AgsFxDssiChannel */
	fx_dssi_channel = (AgsFxDssiChannel *) g_object_new(AGS_TYPE_FX_DSSI_CHANNEL,
							    "output-soundcard", output_soundcard,
							    "input-sequencer", input_sequencer,
							    "source", channel,
							    "recall-audio", fx_dssi_audio,
							    "recall-container", play_container,
							    "filename", filename,
							    "effect", effect,						    
							    NULL);
	ags_recall_set_flags((AgsRecall *) fx_dssi_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags((AgsRecall *) fx_dssi_channel,
				     (AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_MIDI));
	ags_recall_set_behaviour_flags((AgsRecall *) fx_dssi_channel,
				       (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) fx_dssi_channel, TRUE);
	start_recall = g_list_prepend(start_recall,
				      fx_dssi_channel);

	g_object_set(play_container,
		     "recall-channel", fx_dssi_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(fx_dssi_channel));

	/* AgsFxDssiChannelProcessor */
	fx_dssi_channel_processor = (AgsFxDssiChannelProcessor *) g_object_new(AGS_TYPE_FX_DSSI_CHANNEL_PROCESSOR,
									       "output-soundcard", output_soundcard,
									       "input-sequencer", input_sequencer,
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
				     (AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_MIDI));
	ags_recall_set_behaviour_flags((AgsRecall *) fx_dssi_channel_processor,
				       (((AGS_FX_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) fx_dssi_channel_processor, TRUE);
	start_recall = g_list_prepend(start_recall,
				      fx_dssi_channel_processor);

	g_object_set(play_container,
		     "recall-channel-run", fx_dssi_channel_processor,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(fx_dssi_channel_processor));

	/* load */
	ags_fx_dssi_channel_load_port(fx_dssi_channel);
      }else{
	g_warning("ags-fx-dssi recall already added");
      }

      /* iterate */
      next_channel = ags_channel_nth(channel,
				     audio_channels - stop_audio_channel);

      if(channel != NULL){
	g_object_unref(channel);
      }
      
      channel = next_channel;
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
			  18)){
    //TODO:JK: implement me
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-play",
				12)){
    //TODO:JK: implement me
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-volume",
				14)){
    //TODO:JK: implement me
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-peak",
				12)){
    //TODO:JK: implement me
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-eq10",
				12)){
    //TODO:JK: implement me
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-analyse",
				15)){
    //TODO:JK: implement me
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-envelope",
				16)){
    //TODO:JK: implement me
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-wave",
				12)){
    //TODO:JK: implement me
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-pattern",
				15)){
    ags_fx_factory_create_pattern(audio,
				  play_container, recall_container,
				  plugin_name,
				  filename,
				  effect,
				  start_audio_channel, stop_audio_channel,
				  start_pad, stop_pad,
				  create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-notation",
				16)){
    ags_fx_factory_create_notation(audio,
				   play_container, recall_container,
				   plugin_name,
				   filename,
				   effect,
				   start_audio_channel, stop_audio_channel,
				   start_pad, stop_pad,
				   create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-ladspa",
				14)){
    //TODO:JK: implement me
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-dssi",
				12)){
    ags_fx_factory_create_dssi(audio,
			       play_container, recall_container,
			       plugin_name,
			       filename,
			       effect,
			       start_audio_channel, stop_audio_channel,
			       start_pad, stop_pad,
			       create_flags, recall_flags);
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-lv2",
				11)){
    //TODO:JK: implement me
  }else{
    g_warning("no such plugin - %s", plugin_name);
  }

  return(start_recall);
}
