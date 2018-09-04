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

#include <ags/audio/ags_recall_factory.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_generic_recall_channel_run.h>
#include <ags/audio/ags_generic_recall_recycling.h>
#include <ags/audio/ags_recall_dssi.h>
#include <ags/audio/ags_recall_dssi_run.h>
#include <ags/audio/ags_recall_ladspa.h>
#include <ags/audio/ags_recall_ladspa_run.h>
#include <ags/audio/ags_recall_lv2.h>
#include <ags/audio/ags_recall_lv2_run.h>
#include <ags/audio/ags_port.h>

#include <ags/audio/recall/ags_play_audio.h>
#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_channel_run_master.h>
#include <ags/audio/recall/ags_play_channel_run.h>
#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_capture_wave_audio.h>
#include <ags/audio/recall/ags_capture_wave_audio_run.h>
#include <ags/audio/recall/ags_capture_wave_channel.h>
#include <ags/audio/recall/ags_capture_wave_channel_run.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_loop_channel.h>
#include <ags/audio/recall/ags_loop_channel_run.h>
#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_channel_run.h>
#include <ags/audio/recall/ags_prepare_channel.h>
#include <ags/audio/recall/ags_prepare_channel_run.h>
#include <ags/audio/recall/ags_copy_channel.h>
#include <ags/audio/recall/ags_copy_channel_run.h>
#include <ags/audio/recall/ags_feed_channel.h>
#include <ags/audio/recall/ags_feed_channel_run.h>
#include <ags/audio/recall/ags_stream_channel.h>
#include <ags/audio/recall/ags_stream_channel_run.h>
#include <ags/audio/recall/ags_rt_stream_channel.h>
#include <ags/audio/recall/ags_rt_stream_channel_run.h>
#include <ags/audio/recall/ags_buffer_channel.h>
#include <ags/audio/recall/ags_buffer_channel_run.h>
#include <ags/audio/recall/ags_copy_pattern_audio.h>
#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>
#include <ags/audio/recall/ags_play_dssi_audio.h>
#include <ags/audio/recall/ags_play_dssi_audio_run.h>
#include <ags/audio/recall/ags_play_lv2_audio.h>
#include <ags/audio/recall/ags_play_lv2_audio_run.h>
#include <ags/audio/recall/ags_play_notation_audio.h>
#include <ags/audio/recall/ags_play_notation_audio_run.h>
#include <ags/audio/recall/ags_play_wave_audio.h>
#include <ags/audio/recall/ags_play_wave_audio_run.h>
#include <ags/audio/recall/ags_play_wave_channel.h>
#include <ags/audio/recall/ags_play_wave_channel_run.h>
#include <ags/audio/recall/ags_peak_channel.h>
#include <ags/audio/recall/ags_peak_channel_run.h>
#include <ags/audio/recall/ags_mute_audio.h>
#include <ags/audio/recall/ags_mute_audio_run.h>
#include <ags/audio/recall/ags_mute_channel.h>
#include <ags/audio/recall/ags_mute_channel_run.h>
#include <ags/audio/recall/ags_volume_channel.h>
#include <ags/audio/recall/ags_volume_channel_run.h>
#include <ags/audio/recall/ags_envelope_channel.h>
#include <ags/audio/recall/ags_envelope_channel_run.h>
#include <ags/audio/recall/ags_record_midi_audio.h>
#include <ags/audio/recall/ags_record_midi_audio_run.h>
#include <ags/audio/recall/ags_route_dssi_audio.h>
#include <ags/audio/recall/ags_route_dssi_audio_run.h>
#include <ags/audio/recall/ags_route_lv2_audio.h>
#include <ags/audio/recall/ags_route_lv2_audio_run.h>

#include <string.h>

void ags_recall_factory_class_init(AgsRecallFactoryClass *recall_factory_class);
void ags_recall_factory_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_factory_init(AgsRecallFactory *recall_factory);

GList* ags_recall_factory_create_play(AgsAudio *audio,
				      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				      gchar *plugin_name,
				      guint start_audio_channel, guint stop_audio_channel,
				      guint start_pad, guint stop_pad,
				      guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_play_master(AgsAudio *audio,
					     AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
					     gchar *plugin_name,
					     guint start_audio_channel, guint stop_audio_channel,
					     guint start_pad, guint stop_pad,
					     guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_prepare(AgsAudio *audio,
					 AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
					 gchar *plugin_name,
					 guint start_audio_channel, guint stop_audio_channel,
					 guint start_pad, guint stop_pad,
					 guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_copy(AgsAudio *audio,
				      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				      gchar *plugin_name,
				      guint start_audio_channel, guint stop_audio_channel,
				      guint start_pad, guint stop_pad,
				      guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_feed(AgsAudio *audio,
				      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				      gchar *plugin_name,
				      guint start_audio_channel, guint stop_audio_channel,
				      guint start_pad, guint stop_pad,
				      guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_stream(AgsAudio *audio,
					AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
					gchar *plugin_name,
					guint start_audio_channel, guint stop_audio_channel,
					guint start_pad, guint stop_pad,
					guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_rt_stream(AgsAudio *audio,
					   AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
					   gchar *plugin_name,
					   guint start_audio_channel, guint stop_audio_channel,
					   guint start_pad, guint stop_pad,
					   guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_buffer(AgsAudio *audio,
					AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
					gchar *plugin_name,
					guint start_audio_channel, guint stop_audio_channel,
					guint start_pad, guint stop_pad,
					guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_delay(AgsAudio *audio,
				       AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				       gchar *plugin_name,
				       guint start_audio_channel, guint stop_audio_channel,
				       guint start_pad, guint stop_pad,
				       guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_count_beats(AgsAudio *audio,
					     AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
					     gchar *plugin_name,
					     guint start_audio_channel, guint stop_audio_channel,
					     guint start_pad, guint stop_pad,
					     guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_loop(AgsAudio *audio,
				      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				      gchar *plugin_name,
				      guint start_audio_channel, guint stop_audio_channel,
				      guint start_pad, guint stop_pad,
				      guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_copy_pattern(AgsAudio *audio,
					      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
					      gchar *plugin_name,
					      guint start_audio_channel, guint stop_audio_channel,
					      guint start_pad, guint stop_pad,
					      guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_play_wave(AgsAudio *audio,
					   AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
					   gchar *plugin_name,
					   guint start_audio_channel, guint stop_audio_channel,
					   guint start_pad, guint stop_pad,
					   guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_capture_wave(AgsAudio *audio,
					      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
					      gchar *plugin_name,
					      guint start_audio_channel, guint stop_audio_channel,
					      guint start_pad, guint stop_pad,
					      guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_play_dssi(AgsAudio *audio,
					   AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
					   gchar *plugin_name,
					   guint start_audio_channel, guint stop_audio_channel,
					   guint start_pad, guint stop_pad,
					   guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_play_lv2(AgsAudio *audio,
					  AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
					  gchar *plugin_name,
					  guint start_audio_channel, guint stop_audio_channel,
					  guint start_pad, guint stop_pad,
					  guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_play_notation(AgsAudio *audio,
					       AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
					       gchar *plugin_name,
					       guint start_audio_channel, guint stop_audio_channel,
					       guint start_pad, guint stop_pad,
					       guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_peak(AgsAudio *audio,
				      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				      gchar *plugin_name,
				      guint start_audio_channel, guint stop_audio_channel,
				      guint start_pad, guint stop_pad,
				      guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_mute(AgsAudio *audio,
				      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				      gchar *plugin_name,
				      guint start_audio_channel, guint stop_audio_channel,
				      guint start_pad, guint stop_pad,
				      guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_volume(AgsAudio *audio,
					AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
					gchar *plugin_name,
					guint start_audio_channel, guint stop_audio_channel,
					guint start_pad, guint stop_pad,
					guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_envelope(AgsAudio *audio,
					  AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
					  gchar *plugin_name,
					  guint start_audio_channel, guint stop_audio_channel,
					  guint start_pad, guint stop_pad,
					  guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_ladspa(AgsAudio *audio,
					AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
					gchar *plugin_name,
					guint start_audio_channel, guint stop_audio_channel,
					guint start_pad, guint stop_pad,
					guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_lv2(AgsAudio *audio,
				     AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				     gchar *plugin_name,
				     guint start_audio_channel, guint stop_audio_channel,
				     guint start_pad, guint stop_pad,
				     guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_dssi(AgsAudio *audio,
				      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				      gchar *plugin_name,
				      guint start_audio_channel, guint stop_audio_channel,
				      guint start_pad, guint stop_pad,
				      guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_record_midi(AgsAudio *audio,
					     AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
					     gchar *plugin_name,
					     guint start_audio_channel, guint stop_audio_channel,
					     guint start_pad, guint stop_pad,
					     guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_route_dssi(AgsAudio *audio,
					    AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
					    gchar *plugin_name,
					    guint start_audio_channel, guint stop_audio_channel,
					    guint start_pad, guint stop_pad,
					    guint create_flags, guint recall_flags);
GList* ags_recall_factory_create_route_lv2(AgsAudio *audio,
					   AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
					   gchar *plugin_name,
					   guint start_audio_channel, guint stop_audio_channel,
					   guint start_pad, guint stop_pad,
					   guint create_flags, guint recall_flags);

/**
 * SECTION:ags_recall_factory
 * @short_description: Factory pattern
 * @title: AgsRecallFactory
 * @section_id:
 * @include: ags/audio/ags_recall_factory.h
 *
 * #AgsRecallFactory instantiates and sets up recalls.
 */

static gpointer ags_recall_factory_parent_class = NULL;

GType
ags_recall_factory_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_recall_factory = 0;

    static const GTypeInfo ags_recall_factory_info = {
      sizeof (AgsRecallFactoryClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_factory_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallFactory),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_factory_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_factory_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_factory = g_type_register_static(G_TYPE_OBJECT,
						     "AgsRecallFactory",
						     &ags_recall_factory_info,
						     0);

    g_type_add_interface_static(ags_type_recall_factory,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_recall_factory);
  }

  return g_define_type_id__volatile;
}

void
ags_recall_factory_class_init(AgsRecallFactoryClass *recall_factory)
{
  ags_recall_factory_parent_class = g_type_class_peek_parent(recall_factory);
}

void
ags_recall_factory_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;
  connectable->connect = NULL;
  connectable->disconnect = NULL;
}

void
ags_recall_factory_init(AgsRecallFactory *recall_factory)
{
}

GList*
ags_recall_factory_create_play(AgsAudio *audio,
			       AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			       gchar *plugin_name,
			       guint start_audio_channel, guint stop_audio_channel,
			       guint start_pad, guint stop_pad,
			       guint create_flags, guint recall_flags)
{
  AgsPlayChannel *play_channel;
  AgsPlayChannelRun *play_channel_run;
  AgsChannel *output, *input;
  AgsChannel *start, *channel;
  AgsPort *port;

  GObject *output_soundcard;
  
  GList *list_start, list;
  GList *recall;

  guint audio_channels;
  guint i, j;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;

  output = audio->output;
  input = audio->input;
  
  audio_channels = audio->audio_channels;
  
  pthread_mutex_unlock(audio_mutex);

  /* get channel */
  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(output,
				start_pad * audio_channels);
  }else{
    start =
      channel = ags_channel_nth(input,
				start_pad * audio_channels);
  }

  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if(play_container == NULL){
      play_container = ags_recall_container_new();
    }

    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) play_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) play_container);

	/* AgsPlayChannel */
	play_channel = (AgsPlayChannel *) g_object_new(AGS_TYPE_PLAY_CHANNEL,
						       "output-soundcard", output_soundcard,
						       "source", channel,
						       "recall-container", play_container,
						       NULL);
	ags_recall_set_flags(play_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(play_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK));
	ags_recall_set_behaviour_flags(play_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	play_channel->audio_channel->port_value.ags_port_uint = start_audio_channel + j;
	ags_channel_add_recall(channel, (GObject *) play_channel, TRUE);
	recall = g_list_prepend(recall,
				play_channel);

	g_object_set(play_container,
		     "recall-channel", play_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(play_channel));
	
	/* AgsPlayChannelRun */
	play_channel_run = (AgsPlayChannelRun *) g_object_new(AGS_TYPE_PLAY_CHANNEL_RUN,
							      "output-soundcard", output_soundcard,
							      "source", channel,
							      "recall-channel", play_channel,
							      "recall-container", play_container,
							      NULL);
	ags_recall_set_flags(play_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(play_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK));
	ags_recall_set_behaviour_flags(play_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) play_channel_run, TRUE);
	recall = g_list_prepend(recall,
				play_channel_run);

	g_object_set(play_container,
		     "recall-channel-run", play_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(play_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    channel = start;

    if(recall_container == NULL){
      recall_container = ags_recall_container_new();
    }

    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) recall_container);

	/* AgsPlayChannel */
	play_channel = (AgsPlayChannel *) g_object_new(AGS_TYPE_PLAY_CHANNEL,
						       "output-soundcard", output_soundcard,
						       "source", channel,
						       "recall-container", recall_container,
						       NULL);
	ags_recall_set_flags(play_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(play_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK));
	ags_recall_set_behaviour_flags(play_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	play_channel->audio_channel->port_value.ags_port_uint = start_audio_channel + j;
	ags_channel_add_recall(channel, (GObject *) play_channel, FALSE);
	recall = g_list_prepend(recall,
				play_channel);

	g_object_set(recall_container,
		     "recall-channel", play_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(play_channel));

	/* AgsPlayChannelRun */
	play_channel_run = (AgsPlayChannelRun *) g_object_new(AGS_TYPE_PLAY_CHANNEL_RUN,
							      "output-soundcard", output_soundcard,
							      "source", channel,
							      "recall-channel", play_channel,
							      "recall-container", recall_container,
							      NULL);
	ags_recall_set_flags(play_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(play_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK));
	ags_recall_set_behaviour_flags(play_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) play_channel_run, FALSE);
	recall = g_list_prepend(recall,
				play_channel_run);

	g_object_set(recall_container,
		     "recall-channel-run", play_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(play_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_play_master(AgsAudio *audio,
				      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				      gchar *plugin_name,
				      guint start_audio_channel, guint stop_audio_channel,
				      guint start_pad, guint stop_pad,
				      guint create_flags, guint recall_flags)
{
  AgsPlayAudio *play_audio;
  AgsPlayChannel *play_channel;
  AgsPlayChannelRunMaster *play_channel_run_master;
  AgsChannel *output, *input;
  AgsChannel *start, *channel;
  AgsPort *port;

  GObject *output_soundcard;
  
  GList *list_start, *list;
  GList *recall;

  guint audio_channels;
  guint i, j;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;

  output = audio->output;
  input = audio->input;
  
  audio_channels = audio->audio_channels;
  
  pthread_mutex_unlock(audio_mutex);

  /* get channel */
  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(output,
				start_pad * audio_channels);
  }else{
    start =
      channel = ags_channel_nth(input,
				start_pad * audio_channels);
  }

  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if((AGS_RECALL_FACTORY_REMAP & (create_flags)) == 0){
      if(play_container == NULL){
	play_container = ags_recall_container_new();
      }

      play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
      ags_audio_add_recall_container(audio, (GObject *) play_container);

      /*  */
      play_audio = (AgsPlayAudio *) g_object_new(AGS_TYPE_PLAY_AUDIO,
						 "output-soundcard", output_soundcard,
						 "audio", audio,
						 "recall-container", play_container,
						 NULL);
      ags_recall_set_flags(play_audio,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags(play_audio,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_MIDI |
				    AGS_SOUND_ABILITY_WAVE));
      ags_recall_set_behaviour_flags(play_audio,
				     (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));


      ags_audio_add_recall(audio, (GObject *) play_audio, TRUE);
      recall = g_list_prepend(recall,
			      play_audio);
    }else{
      if(play_container == NULL){
	g_object_get(audio,
		     "play", &list_start,
		     NULL);

	list = ags_recall_find_type(list_start,
				    AGS_TYPE_PLAY_AUDIO);
	play_audio = AGS_PLAY_AUDIO(list->data);
	g_list_free(list_start);
	
	recall = g_list_prepend(recall,
				play_audio);

	g_object_get(play_audio,
		     "recall-container", &play_container,
		     NULL);
      }else{
	g_object_get(play_container,
		     "recall-audio", &play_audio,
		     NULL);
	recall = g_list_prepend(recall,
				play_audio);
      }
    }

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) play_container);

	/* AgsPlayChannel */
	play_channel = (AgsPlayChannel *) g_object_new(AGS_TYPE_PLAY_CHANNEL,
						       "output-soundcard", output_soundcard,
						       "source", channel,
						       "recall-container", play_container,
						       NULL);
	ags_recall_set_flags(play_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(play_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(play_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	play_channel->audio_channel->port_value.ags_port_uint = start_audio_channel + j;
	ags_channel_add_recall(channel, (GObject *) play_channel, TRUE);
	recall = g_list_prepend(recall,
				play_channel);

	g_object_set(play_container,
		     "recall-channel", play_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(play_channel));
      
	/* AgsPlayChannelRun */
	play_channel_run_master = (AgsPlayChannelRunMaster *) g_object_new(AGS_TYPE_PLAY_CHANNEL_RUN_MASTER,
									   "output-soundcard", output_soundcard,
									   "source", channel,
									   "recall-channel", play_channel,
									   "recall-container", play_container,
									   NULL);
	ags_recall_set_flags(play_channel_run_master,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(play_channel_run_master,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(play_channel_run_master,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) play_channel_run_master, TRUE);
	recall = g_list_prepend(recall,
				play_channel_run_master);

	g_object_set(play_container,
		     "recall-channel-run", play_channel_run_master,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(play_channel_run_master));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    channel = start;

    if((AGS_RECALL_FACTORY_REMAP & (create_flags)) == 0){
      if(recall_container == NULL){
	recall_container = ags_recall_container_new();
      }

      ags_audio_add_recall_container(audio, (GObject *) recall_container);

      /*  */
      play_audio = (AgsPlayAudio *) g_object_new(AGS_TYPE_PLAY_AUDIO,
						 "output-soundcard", output_soundcard,
						 "audio", audio,
						 "recall-container", recall_container,
						 NULL);
      ags_recall_set_flags(play_audio,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags(play_audio,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_MIDI |
				    AGS_SOUND_ABILITY_WAVE));
      ags_recall_set_behaviour_flags(play_audio,
				     (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_audio_add_recall(audio, (GObject *) play_audio, FALSE);
      recall = g_list_prepend(recall,
			      play_audio);
    }else{
      if(recall_container == NULL){
	g_object_get(audio,
		     "recall", &list_start,
		     NULL);

	list = ags_recall_find_type(list_start,
				    AGS_TYPE_PLAY_AUDIO);
	play_audio = AGS_PLAY_AUDIO(list->data);
	g_list_free(list_start);
	
	recall = g_list_prepend(recall,
				play_audio);

	g_object_get(play_audio,
		     "recall-container", &recall_container,
		     NULL);
      }else{
	g_object_get(recall_container,
		     "recall-audio", &play_audio,
		     NULL);
	recall = g_list_prepend(recall,
				play_audio);
      }
    }

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) recall_container);

	/* AgsPlayChannel */
	play_channel = (AgsPlayChannel *) g_object_new(AGS_TYPE_PLAY_CHANNEL,
						       "output-soundcard", output_soundcard,
						       "source", channel,
						       "recall-container", recall_container,
						       NULL);
	ags_recall_set_flags(play_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(play_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(play_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	play_channel->audio_channel->port_value.ags_port_uint = start_audio_channel + j;
	ags_channel_add_recall(channel, (GObject *) play_channel, FALSE);
	recall = g_list_prepend(recall,
				play_channel);

	g_object_set(recall_container,
		     "recall-channel", play_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(play_channel));
	
	/* AgsPlayChannelRun */
	play_channel_run_master = (AgsPlayChannelRunMaster *) g_object_new(AGS_TYPE_PLAY_CHANNEL_RUN_MASTER,
									   "output-soundcard", output_soundcard,
									   "source", channel,
									   "recall-channel", play_channel,
									   "recall-container", recall_container,
									   NULL);
	ags_recall_set_flags(play_channel_run_master,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(play_channel_run_master,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(play_channel_run_master,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) play_channel_run_master, FALSE);
	recall = g_list_prepend(recall,
				play_channel_run_master);

	g_object_set(recall_container,
		     "recall-channel-run", play_channel_run_master,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(play_channel_run_master));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_prepare(AgsAudio *audio,
				  AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				  gchar *plugin_name,
				  guint start_audio_channel, guint stop_audio_channel,
				  guint start_pad, guint stop_pad,
				  guint create_flags, guint recall_flags)
{
  AgsPrepareChannel *prepare_channel;
  AgsPrepareChannelRun *prepare_channel_run;
  AgsChannel *output, *input;
  AgsChannel *start, *channel, *current;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint audio_channels;
  guint i, j;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *current_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;

  output = audio->output;
  input = audio->input;
  
  audio_channels = audio->audio_channels;
  
  pthread_mutex_unlock(audio_mutex);

  /* get channel */
  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(output,
				start_pad * audio_channels);
  }else{
    start =
      channel = ags_channel_nth(input,
				start_pad * audio_channels);
  }

  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    gboolean found_prepare;

    channel = start;
    
    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	current = ags_channel_nth(output,
				  start_audio_channel + j);
	
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* check output */
	while(current != NULL){
	  /* get channel mutex */
	  pthread_mutex_lock(ags_channel_get_class_mutex());

	  current_mutex = current->obj_mutex;
  
	  pthread_mutex_unlock(ags_channel_get_class_mutex());

	  /* check output present */
	  g_object_get(channel,
		       "play", &list_start,
		       NULL);

	  list = list_start;
	  found_prepare = FALSE;	  

	  while((list = ags_recall_template_find_type(list,
						      AGS_TYPE_PREPARE_CHANNEL)) != NULL){
	    AgsChannel *current_destination;

	    g_object_get(list->data,
			 "destination", &current_destination,
			 NULL);
	    
	    if(current_destination == current){
	      found_prepare = TRUE;
	      break;
	    }
	
	    list = list->next;
	  }

	  g_list_free(list_start);
	  
	  if(found_prepare){
	    /* iterate - current */
	    pthread_mutex_lock(current_mutex);

	    current = current->next_pad;

	    pthread_mutex_unlock(current_mutex);
	    
	    continue;
	  }
	
	  if(play_container == NULL){
	    play_container = ags_recall_container_new();
	    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
	    ags_audio_add_recall_container(audio, (GObject *) play_container);
	  }
	
	  ags_channel_add_recall_container(channel, (GObject *) play_container);

	  /* AgsPrepareChannel */
	  prepare_channel = (AgsPrepareChannel *) g_object_new(AGS_TYPE_PREPARE_CHANNEL,
							       "output-soundcard", output_soundcard,
							       "source", channel,
							       "destination", current,
							       "recall-container", play_container,
							       NULL);							      
	  ags_recall_set_flags(prepare_channel,
			       (AGS_RECALL_TEMPLATE));
	  ags_recall_set_ability_flags(prepare_channel,
				       (AGS_SOUND_ABILITY_PLAYBACK |
					AGS_SOUND_ABILITY_NOTATION |
					AGS_SOUND_ABILITY_SEQUENCER |
					AGS_SOUND_ABILITY_MIDI |
					AGS_SOUND_ABILITY_WAVE));
	  ags_recall_set_behaviour_flags(prepare_channel,
					 (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	  ags_channel_add_recall(channel,
				 prepare_channel, TRUE);
	  
	  recall = g_list_prepend(recall,
				  prepare_channel);
	  
	  g_object_set(play_container,
		       "recall-channel", prepare_channel,
		       NULL);
	  ags_connectable_connect(AGS_CONNECTABLE(prepare_channel));

	  /* AgsPrepareChannelRun */
	  prepare_channel_run = (AgsPrepareChannelRun *) g_object_new(AGS_TYPE_PREPARE_CHANNEL_RUN,
								      "output-soundcard", output_soundcard,
								      "recall-channel", prepare_channel,
								      "source", channel,
								      "destination", current,
								      "recall-container", play_container,
								      NULL);
	  ags_recall_set_flags(prepare_channel_run,
			       (AGS_RECALL_TEMPLATE));
	  ags_recall_set_ability_flags(prepare_channel_run,
				       (AGS_SOUND_ABILITY_PLAYBACK |
					AGS_SOUND_ABILITY_NOTATION |
					AGS_SOUND_ABILITY_SEQUENCER |
					AGS_SOUND_ABILITY_MIDI |
					AGS_SOUND_ABILITY_WAVE));
	  ags_recall_set_behaviour_flags(prepare_channel_run,
					 (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	  ags_channel_add_recall(channel,
				 prepare_channel_run, TRUE);

	  recall = g_list_prepend(recall,
				  prepare_channel_run);

	  g_object_set(play_container,
		       "recall-channel-run", prepare_channel_run,
		       NULL);
	  ags_connectable_connect(AGS_CONNECTABLE(prepare_channel_run));

	  /* iterate */
	  pthread_mutex_lock(current_mutex);
	  
	  current = current->next_pad;

	  pthread_mutex_unlock(current_mutex);
	}

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    gboolean found_prepare;
    
    channel = start;
    
    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	current = ags_channel_nth(output,
				  start_audio_channel + j);
	
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* check output */
	while(current != NULL){
	  /* get channel mutex */
	  pthread_mutex_lock(ags_channel_get_class_mutex());

	  current_mutex = current->obj_mutex;
  
	  pthread_mutex_unlock(ags_channel_get_class_mutex());

	  /* check output present */
	  g_object_get(channel,
		       "recall", &list_start,
		       NULL);

	  list = list_start;
	  found_prepare = FALSE;
	
	  while((list = ags_recall_template_find_type(list,
						      AGS_TYPE_PREPARE_CHANNEL)) != NULL){
	    AgsChannel *current_destination;

	    g_object_get(list->data,
			 "destination", &current_destination,
			 NULL);

	    if(current_destination == current){
	      found_prepare = TRUE;
	      break;
	    }
	
	    list = list->next;
	  }

	  g_list_free(list_start);
	
	  if(found_prepare){
	    /* iterate */
	    pthread_mutex_lock(current_mutex);
	  
	    current = current->next_pad;

	    pthread_mutex_unlock(current_mutex);

	    continue;
	  }

	  if(recall_container == NULL){
	    recall_container = ags_recall_container_new();
	    ags_audio_add_recall_container(audio, (GObject *) recall_container);
	  }
	
	  ags_channel_add_recall_container(channel, (GObject *) recall_container);

	  /* AgsPrepareChannel */
	  prepare_channel = (AgsPrepareChannel *) g_object_new(AGS_TYPE_PREPARE_CHANNEL,
							       "output-soundcard", output_soundcard,
							       "source", channel,
							       "destination", current,
							       "recall-container", recall_container,
							       NULL);
	  ags_recall_set_flags(prepare_channel,
			       (AGS_RECALL_TEMPLATE));
	  ags_recall_set_ability_flags(prepare_channel,
				       (AGS_SOUND_ABILITY_PLAYBACK |
					AGS_SOUND_ABILITY_NOTATION |
					AGS_SOUND_ABILITY_SEQUENCER |
					AGS_SOUND_ABILITY_MIDI |
					AGS_SOUND_ABILITY_WAVE));
	  ags_recall_set_behaviour_flags(prepare_channel,
					 (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	  ags_channel_add_recall(channel,
				 prepare_channel, FALSE);

	  recall = g_list_prepend(recall,
				  prepare_channel);

	  g_object_set(recall_container,
		       "recall-channel", prepare_channel,
		       NULL);
	  ags_connectable_connect(AGS_CONNECTABLE(prepare_channel));

	  /* AgsPrepareChannelRun */
	  prepare_channel_run = (AgsPrepareChannelRun *) g_object_new(AGS_TYPE_PREPARE_CHANNEL_RUN,
								      "output-soundcard", output_soundcard,
								      "recall-channel", prepare_channel,
								      "source", channel,
								      "destination", current,
								      "recall-container", recall_container,
								      NULL);
	  ags_recall_set_flags(prepare_channel_run,
			       (AGS_RECALL_TEMPLATE));
	  ags_recall_set_ability_flags(prepare_channel_run,
				       (AGS_SOUND_ABILITY_PLAYBACK |
					AGS_SOUND_ABILITY_NOTATION |
					AGS_SOUND_ABILITY_SEQUENCER |
					AGS_SOUND_ABILITY_MIDI |
					AGS_SOUND_ABILITY_WAVE));
	  ags_recall_set_behaviour_flags(prepare_channel_run,
					 (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	  ags_channel_add_recall(channel,
				 prepare_channel_run, FALSE);

	  recall = g_list_prepend(recall,
				  prepare_channel_run);

	  g_object_set(recall_container,
		       "recall-channel-run", prepare_channel_run,
		       NULL);
	  ags_connectable_connect(AGS_CONNECTABLE(prepare_channel_run));

	  /* iterate */
	  pthread_mutex_lock(current_mutex);
	  
	  current = current->next_pad;

	  pthread_mutex_unlock(current_mutex);
	}
	
	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_copy(AgsAudio *audio,
			       AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			       gchar *plugin_name,
			       guint start_audio_channel, guint stop_audio_channel,
			       guint start_pad, guint stop_pad,
			       guint create_flags, guint recall_flags)
{
  AgsCopyChannel *copy_channel;
  AgsCopyChannelRun *copy_channel_run;
  AgsChannel *output, *input;
  AgsChannel *start, *channel, *current;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint audio_channels;
  guint i, j;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *current_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;

  output = audio->output;
  input = audio->input;
  
  audio_channels = audio->audio_channels;
  
  pthread_mutex_unlock(audio_mutex);

  /* get channel */
  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(output,
				start_pad * audio_channels);
  }else{
    start =
      channel = ags_channel_nth(input,
				start_pad * audio_channels);
  }

  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    gboolean found_copy;

    channel = start;
    
    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	current = ags_channel_nth(output,
				  start_audio_channel + j);

	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());
	
	/* check output */
	while(current != NULL){
	  /* get channel mutex */
	  pthread_mutex_lock(ags_channel_get_class_mutex());

	  current_mutex = current->obj_mutex;
  
	  pthread_mutex_unlock(ags_channel_get_class_mutex());

	  /* check output present */
	  g_object_get(channel,
		       "play", &list_start,
		       NULL);

	  list = list_start;
	  found_copy = FALSE;

	  while((list = ags_recall_template_find_type(list,
						      AGS_TYPE_COPY_CHANNEL)) != NULL){
	    AgsChannel *current_destination;

	    g_object_get(list->data,
			 "destination", &current_destination,
			 NULL);

	    if(current_destination == current){
	      found_copy = TRUE;
	      break;
	    }
	
	    list = list->next;
	  }

	  g_list_free(list_start);

	  if(found_copy){
	    /* iterate - current */
	    pthread_mutex_lock(current_mutex);

	    current = current->next_pad;

	    pthread_mutex_unlock(current_mutex);
	    
	    continue;
	  }
	
	  if(play_container == NULL){
	    play_container = ags_recall_container_new();
	    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
	    ags_audio_add_recall_container(audio, (GObject *) play_container);
	  }
	
	  ags_channel_add_recall_container(channel, (GObject *) play_container);

	  /* AgsCopyChannel */
	  copy_channel = (AgsCopyChannel *) g_object_new(AGS_TYPE_COPY_CHANNEL,
							 "output-soundcard", output_soundcard,
							 "source", channel,
							 "destination", current,
							 "recall-container", play_container,
							 NULL);
	  ags_recall_set_flags(copy_channel,
			       (AGS_RECALL_TEMPLATE));
	  ags_recall_set_ability_flags(copy_channel,
				       (AGS_SOUND_ABILITY_PLAYBACK |
					AGS_SOUND_ABILITY_NOTATION |
					AGS_SOUND_ABILITY_SEQUENCER |
					AGS_SOUND_ABILITY_MIDI |
					AGS_SOUND_ABILITY_WAVE));
	  ags_recall_set_behaviour_flags(copy_channel,
					 (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	  ags_channel_add_recall(channel,
				 copy_channel, TRUE);

	  recall = g_list_prepend(recall,
				  copy_channel);

	  g_object_set(play_container,
		       "recall-channel", copy_channel,
		       NULL);
	  ags_connectable_connect(AGS_CONNECTABLE(copy_channel));

	  /* AgsCopyChannelRun */
	  copy_channel_run = (AgsCopyChannelRun *) g_object_new(AGS_TYPE_COPY_CHANNEL_RUN,
								"output-soundcard", output_soundcard,
								"recall-channel", copy_channel,
								"source", channel,
								"destination", current,
								"recall-container", play_container,
								NULL);
	  ags_recall_set_flags(copy_channel_run,
			       (AGS_RECALL_TEMPLATE));
	  ags_recall_set_ability_flags(copy_channel_run,
				       (AGS_SOUND_ABILITY_PLAYBACK |
					AGS_SOUND_ABILITY_NOTATION |
					AGS_SOUND_ABILITY_SEQUENCER |
					AGS_SOUND_ABILITY_MIDI |
					AGS_SOUND_ABILITY_WAVE));
	  ags_recall_set_behaviour_flags(copy_channel_run,
					 (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	  ags_channel_add_recall(channel,
				 copy_channel_run, TRUE);

	  recall = g_list_prepend(recall,
				  copy_channel_run);

	  g_object_set(play_container,
		       "recall-channel-run", copy_channel_run,
		       NULL);
	  ags_connectable_connect(AGS_CONNECTABLE(copy_channel_run));

	  /* iterate */
	  pthread_mutex_lock(current_mutex);
	  
	  current = current->next_pad;

	  pthread_mutex_unlock(current_mutex);
	}

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    gboolean found_copy;
    
    channel = start;
    
    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	current = ags_channel_nth(output,
				  start_audio_channel + j);

	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());
	
	/* check output */
	while(current != NULL){
	  /* get channel mutex */
	  pthread_mutex_lock(ags_channel_get_class_mutex());

	  current_mutex = current->obj_mutex;
  
	  pthread_mutex_unlock(ags_channel_get_class_mutex());

	  /* check output present */
	  g_object_get(channel,
		       "recall", &list_start,
		       NULL);

	  list = list_start;
	  found_copy = FALSE;
	
	  while((list = ags_recall_template_find_type(list,
						      AGS_TYPE_COPY_CHANNEL)) != NULL){
	    AgsChannel *current_destination;

	    g_object_get(list->data,
			 "destination", &current_destination,
			 NULL);

	    if(current_destination == current){
	      found_copy = TRUE;
	      break;
	    }
	
	    list = list->next;
	  }

	  g_list_free(list_start);
	
	  if(found_copy){
	    /* iterate */
	    pthread_mutex_lock(current_mutex);
	  
	    current = current->next_pad;

	    pthread_mutex_unlock(current_mutex);
	    
	    continue;
	  }

	  if(recall_container == NULL){
	    recall_container = ags_recall_container_new();
	    ags_audio_add_recall_container(audio, (GObject *) recall_container);
	  }
	
	  ags_channel_add_recall_container(channel, (GObject *) recall_container);

	  /* AgsCopyChannel */
	  copy_channel = (AgsCopyChannel *) g_object_new(AGS_TYPE_COPY_CHANNEL,
							 "output-soundcard", output_soundcard,
							 "source", channel,
							 "destination", current,
							 "recall-container", recall_container,
							 NULL);
	  ags_recall_set_flags(copy_channel,
			       (AGS_RECALL_TEMPLATE));
	  ags_recall_set_ability_flags(copy_channel,
				       (AGS_SOUND_ABILITY_PLAYBACK |
					AGS_SOUND_ABILITY_NOTATION |
					AGS_SOUND_ABILITY_SEQUENCER |
					AGS_SOUND_ABILITY_MIDI |
					AGS_SOUND_ABILITY_WAVE));
	  ags_recall_set_behaviour_flags(copy_channel,
					 (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	  ags_channel_add_recall(channel,
				 copy_channel, FALSE);
	  
	  recall = g_list_prepend(recall,
				  copy_channel);

	  g_object_set(recall_container,
		       "recall-channel", copy_channel,
		       NULL);
	  ags_connectable_connect(AGS_CONNECTABLE(copy_channel));

	  /* AgsCopyChannelRun */
	  copy_channel_run = (AgsCopyChannelRun *) g_object_new(AGS_TYPE_COPY_CHANNEL_RUN,
								"output-soundcard", output_soundcard,
								"recall-channel", copy_channel,
								"source", channel,
								"destination", current,
								"recall-container", recall_container,
								NULL);
	  ags_recall_set_flags(copy_channel_run,
			       (AGS_RECALL_TEMPLATE));
	  ags_recall_set_ability_flags(copy_channel_run,
				       (AGS_SOUND_ABILITY_PLAYBACK |
					AGS_SOUND_ABILITY_NOTATION |
					AGS_SOUND_ABILITY_SEQUENCER |
					AGS_SOUND_ABILITY_MIDI |
					AGS_SOUND_ABILITY_WAVE));
	  ags_recall_set_behaviour_flags(copy_channel_run,
					 (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	  ags_channel_add_recall(channel,
				 copy_channel_run, FALSE);

	  recall = g_list_prepend(recall,
				  copy_channel_run);

	  g_object_set(recall_container,
		       "recall-channel-run", copy_channel_run,
		       NULL);
	  ags_connectable_connect(AGS_CONNECTABLE(copy_channel_run));

	  /* iterate */
	  pthread_mutex_lock(current_mutex);
	  
	  current = current->next_pad;

	  pthread_mutex_unlock(current_mutex);
	}
	
	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_feed(AgsAudio *audio,
			       AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			       gchar *plugin_name,
			       guint start_audio_channel, guint stop_audio_channel,
			       guint start_pad, guint stop_pad,
			       guint create_flags, guint recall_flags)
{
  AgsFeedChannel *feed_channel;
  AgsFeedChannelRun *feed_channel_run;
  AgsChannel *output, *input;
  AgsChannel *start, *channel;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint audio_channels;
  guint i, j;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;

  output = audio->output;
  input = audio->input;
  
  audio_channels = audio->audio_channels;
  
  pthread_mutex_unlock(audio_mutex);

  /* get channel */
  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(output,
				start_pad * audio_channels);
  }else{
    start =
      channel = ags_channel_nth(input,
				start_pad * audio_channels);
  }

  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if(play_container == NULL){
      play_container = ags_recall_container_new();
    }

    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) play_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) play_container);

	/* AgsFeedChannel */
	feed_channel = (AgsFeedChannel *) g_object_new(AGS_TYPE_FEED_CHANNEL,
						       "output-soundcard", output_soundcard,
						       "source", channel,
						       "recall-container", play_container,
						       NULL);
	ags_recall_set_flags(feed_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(feed_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(feed_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) feed_channel, TRUE);
	recall = g_list_prepend(recall,
				feed_channel);

	g_object_set(play_container,
		     "recall-channel", feed_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(feed_channel));

	/* AgsFeedChannelRun */
	feed_channel_run = (AgsFeedChannelRun *) g_object_new(AGS_TYPE_FEED_CHANNEL_RUN,
							      "output-soundcard", output_soundcard,
							      "recall-channel", feed_channel,
							      "source", channel,
							      "recall-container", play_container,
							      NULL);
	ags_recall_set_flags(feed_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(feed_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(feed_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) feed_channel_run, TRUE);
	recall = g_list_prepend(recall,
				feed_channel_run);

	g_object_set(play_container,
		     "recall-channel-run", feed_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(feed_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    channel = start;

    if(recall_container == NULL){
      recall_container = ags_recall_container_new();
    }

    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) recall_container);

	/* AgsFeedChannel */
	feed_channel = (AgsFeedChannel *) g_object_new(AGS_TYPE_FEED_CHANNEL,
						       "output-soundcard", output_soundcard,
						       "source", channel,
						       "recall-container", recall_container,
						       NULL);
	ags_recall_set_flags(feed_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(feed_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(feed_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) feed_channel, FALSE);
	recall = g_list_prepend(recall,
				feed_channel);

	g_object_set(recall_container,
		     "recall-channel", feed_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(feed_channel));

	/* AgsFeedChannelRun */
	feed_channel_run = (AgsFeedChannelRun *) g_object_new(AGS_TYPE_FEED_CHANNEL_RUN,
							      "output-soundcard", output_soundcard,
							      "recall-channel", feed_channel,
							      "source", channel,
							      "recall-container", recall_container,
							      NULL);
	ags_recall_set_flags(feed_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(feed_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(feed_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) feed_channel_run, FALSE);
	recall = g_list_prepend(recall,
				feed_channel_run);

	g_object_set(recall_container,
		     "recall-channel-run", feed_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(feed_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_stream(AgsAudio *audio,
				 AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				 gchar *plugin_name,
				 guint start_audio_channel, guint stop_audio_channel,
				 guint start_pad, guint stop_pad,
				 guint create_flags, guint recall_flags)
{
  AgsStreamChannel *stream_channel;
  AgsStreamChannelRun *stream_channel_run;
  AgsChannel *output, *input;
  AgsChannel *start, *channel;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint audio_channels;
  guint i, j;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;

  output = audio->output;
  input = audio->input;
  
  audio_channels = audio->audio_channels;
  
  pthread_mutex_unlock(audio_mutex);

  /* get channel */
  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(output,
				start_pad * audio_channels);
  }else{
    start =
      channel = ags_channel_nth(input,
				start_pad * audio_channels);
  }

  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if(play_container == NULL){
      play_container = ags_recall_container_new();
    }

    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) play_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) play_container);

	/* AgsStreamChannel */
	stream_channel = (AgsStreamChannel *) g_object_new(AGS_TYPE_STREAM_CHANNEL,
							   "output-soundcard", output_soundcard,
							   "source", channel,
							   "recall-container", play_container,
							   NULL);
	ags_recall_set_flags(stream_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(stream_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(stream_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) stream_channel, TRUE);
	recall = g_list_prepend(recall,
				stream_channel);

	g_object_set(play_container,
		     "recall-channel", stream_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(stream_channel));

	/* AgsStreamChannelRun */
	stream_channel_run = (AgsStreamChannelRun *) g_object_new(AGS_TYPE_STREAM_CHANNEL_RUN,
								  "output-soundcard", output_soundcard,
								  "recall-channel", stream_channel,
								  "source", channel,
								  "recall-container", play_container,
								  NULL);
	ags_recall_set_flags(stream_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(stream_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(stream_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) stream_channel_run, TRUE);
	recall = g_list_prepend(recall,
				stream_channel_run);

	g_object_set(play_container,
		     "recall-channel-run", stream_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(stream_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    channel = start;

    if(recall_container == NULL){
      recall_container = ags_recall_container_new();
    }

    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) recall_container);

	/* AgsStreamChannel */
	stream_channel = (AgsStreamChannel *) g_object_new(AGS_TYPE_STREAM_CHANNEL,
							   "output-soundcard", output_soundcard,
							   "source", channel,
							   "recall-container", recall_container,
							   NULL);
	ags_recall_set_flags(stream_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(stream_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(stream_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) stream_channel, FALSE);
	recall = g_list_prepend(recall,
				stream_channel);

	g_object_set(recall_container,
		     "recall-channel", stream_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(stream_channel));

	/* AgsStreamChannelRun */
	stream_channel_run = (AgsStreamChannelRun *) g_object_new(AGS_TYPE_STREAM_CHANNEL_RUN,
								  "output-soundcard", output_soundcard,
								  "recall-channel", stream_channel,
								  "source", channel,
								  "recall-container", recall_container,
								  NULL);
	ags_recall_set_flags(stream_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(stream_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(stream_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) stream_channel_run, FALSE);
	recall = g_list_prepend(recall,
				stream_channel_run);

	g_object_set(recall_container,
		     "recall-channel-run", stream_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(stream_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_rt_stream(AgsAudio *audio,
				    AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				    gchar *plugin_name,
				    guint start_audio_channel, guint stop_audio_channel,
				    guint start_pad, guint stop_pad,
				    guint create_flags, guint recall_flags)
{
  AgsRtStreamChannel *rt_stream_channel;
  AgsRtStreamChannelRun *rt_stream_channel_run;
  AgsChannel *output, *input;
  AgsChannel *start, *channel;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint audio_channels;
  guint i, j;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;

  output = audio->output;
  input = audio->input;
  
  audio_channels = audio->audio_channels;
  
  pthread_mutex_unlock(audio_mutex);

  /* get channel */
  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(output,
				start_pad * audio_channels);
  }else{
    start =
      channel = ags_channel_nth(input,
				start_pad * audio_channels);
  }

  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if(play_container == NULL){
      play_container = ags_recall_container_new();
    }

    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) play_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) play_container);

	/* AgsRtStreamChannel */
	rt_stream_channel = (AgsRtStreamChannel *) g_object_new(AGS_TYPE_RT_STREAM_CHANNEL,
								"output-soundcard", output_soundcard,
								"source", channel,
								"recall-container", play_container,
								NULL);
	ags_recall_set_flags(rt_stream_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(rt_stream_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(rt_stream_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) rt_stream_channel, TRUE);
	recall = g_list_prepend(recall,
				rt_stream_channel);

	g_object_set(play_container,
		     "recall-channel", rt_stream_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(rt_stream_channel));

	/* AgsRtStreamChannelRun */
	rt_stream_channel_run = (AgsRtStreamChannelRun *) g_object_new(AGS_TYPE_RT_STREAM_CHANNEL_RUN,
								       "output-soundcard", output_soundcard,
								       "recall-channel", rt_stream_channel,
								       "source", channel,
								       "recall-container", play_container,
								       NULL);
	ags_recall_set_flags(rt_stream_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(rt_stream_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(rt_stream_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) rt_stream_channel_run, TRUE);
	recall = g_list_prepend(recall,
				rt_stream_channel_run);

	g_object_set(play_container,
		     "recall-channel-run", rt_stream_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(rt_stream_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    channel = start;

    if(recall_container == NULL){
      recall_container = ags_recall_container_new();
    }

    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) recall_container);

	/* AgsRtStreamChannel */
	rt_stream_channel = (AgsRtStreamChannel *) g_object_new(AGS_TYPE_RT_STREAM_CHANNEL,
								"output-soundcard", output_soundcard,
								"source", channel,
								"recall-container", recall_container,
								NULL);
	ags_recall_set_flags(rt_stream_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(rt_stream_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(rt_stream_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) rt_stream_channel, FALSE);
	recall = g_list_prepend(recall,
				rt_stream_channel);

	g_object_set(recall_container,
		     "recall-channel", rt_stream_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(rt_stream_channel));

	/* AgsRtStreamChannelRun */
	rt_stream_channel_run = (AgsRtStreamChannelRun *) g_object_new(AGS_TYPE_RT_STREAM_CHANNEL_RUN,
								       "output-soundcard", output_soundcard,
								       "recall-channel", rt_stream_channel,
								       "source", channel,
								       "recall-container", recall_container,
								       NULL);
	ags_recall_set_flags(rt_stream_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(rt_stream_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(rt_stream_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) rt_stream_channel_run, FALSE);
	recall = g_list_prepend(recall,
				rt_stream_channel_run);

	g_object_set(recall_container,
		     "recall-channel-run", rt_stream_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(rt_stream_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_buffer(AgsAudio *audio,
				 AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				 gchar *plugin_name,
				 guint start_audio_channel, guint stop_audio_channel,
				 guint start_pad, guint stop_pad,
				 guint create_flags, guint recall_flags)
{
  AgsBufferChannel *buffer_channel;
  AgsBufferChannelRun *buffer_channel_run;
  AgsChannel *output, *input;
  AgsChannel *start, *channel, *current;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint audio_channels;
  guint i, j;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *current_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;

  output = audio->output;
  input = audio->input;
  
  audio_channels = audio->audio_channels;
  
  pthread_mutex_unlock(audio_mutex);

  /* get channel */
  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(output,
				start_pad * audio_channels);
  }else{
    start =
      channel = ags_channel_nth(input,
				start_pad * audio_channels);
  }

  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    gboolean found_buffer;

    channel = start;
    
    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	current = ags_channel_nth(output,
				  start_audio_channel + j);
	
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* check output */
	while(current != NULL){
	  /* get channel mutex */
	  pthread_mutex_lock(ags_channel_get_class_mutex());

	  current_mutex = current->obj_mutex;
  
	  pthread_mutex_unlock(ags_channel_get_class_mutex());

	  /* check output present */
	  g_object_get(channel,
		       "play", &list_start,
		       NULL);

	  list = list_start;
 	  found_buffer = FALSE;

	  while((list = ags_recall_template_find_type(list,
						      AGS_TYPE_BUFFER_CHANNEL)) != NULL){
	    AgsChannel *current_destination;

	    g_object_get(list->data,
			 "destination", &current_destination,
			 NULL);

	    if(current_destination == current){
	      found_buffer = TRUE;
	      break;
	    }
	
	    list = list->next;
	  }

	  g_list_free(list_start);
      
	  if(found_buffer){
	    /* iterate - current */
	    pthread_mutex_lock(current_mutex);

	    current = current->next_pad;

	    pthread_mutex_unlock(current_mutex);
	    
	    continue;
	  }
	
	  if(play_container == NULL){
	    play_container = ags_recall_container_new();
	    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
	    ags_audio_add_recall_container(audio, (GObject *) play_container);
	  }
	
	  ags_channel_add_recall_container(channel, (GObject *) play_container);

	  /* AgsBufferChannel */
	  buffer_channel = (AgsBufferChannel *) g_object_new(AGS_TYPE_BUFFER_CHANNEL,
							     "output-soundcard", output_soundcard,
							     "source", channel,
							     "destination", current,
							     "recall-container", play_container,
							     NULL);
	  ags_recall_set_flags(buffer_channel,
			       (AGS_RECALL_TEMPLATE));
	  ags_recall_set_ability_flags(buffer_channel,
				       (AGS_SOUND_ABILITY_PLAYBACK |
					AGS_SOUND_ABILITY_NOTATION |
					AGS_SOUND_ABILITY_SEQUENCER |
					AGS_SOUND_ABILITY_MIDI |
					AGS_SOUND_ABILITY_WAVE));
	  ags_recall_set_behaviour_flags(buffer_channel,
					 (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	  ags_channel_add_recall(channel,
				 buffer_channel, TRUE);

	  recall = g_list_prepend(recall,
				  buffer_channel);

	  g_object_set(play_container,
		       "recall-channel", buffer_channel,
		       NULL);
	  ags_connectable_connect(AGS_CONNECTABLE(buffer_channel));

	  /* AgsBufferChannelRun */
	  buffer_channel_run = (AgsBufferChannelRun *) g_object_new(AGS_TYPE_BUFFER_CHANNEL_RUN,
								    "output-soundcard", output_soundcard,
								    "recall-channel", buffer_channel,
								    "source", channel,
								    "destination", current,
								    "recall-container", play_container,
								    NULL);
	  ags_recall_set_flags(buffer_channel_run,
			       (AGS_RECALL_TEMPLATE));
	  ags_recall_set_ability_flags(buffer_channel_run,
				       (AGS_SOUND_ABILITY_PLAYBACK |
					AGS_SOUND_ABILITY_NOTATION |
					AGS_SOUND_ABILITY_SEQUENCER |
					AGS_SOUND_ABILITY_MIDI |
					AGS_SOUND_ABILITY_WAVE));
	  ags_recall_set_behaviour_flags(buffer_channel_run,
					 (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	  ags_channel_add_recall(channel,
				 buffer_channel_run, TRUE);

	  recall = g_list_prepend(recall,
				  buffer_channel_run);

	  g_object_set(play_container,
		       "recall-channel-run", buffer_channel_run,
		       NULL);
	  ags_connectable_connect(AGS_CONNECTABLE(buffer_channel_run));

	  /* iterate - current */
	  pthread_mutex_lock(current_mutex);

	  current = current->next_pad;

	  pthread_mutex_unlock(current_mutex);
	}

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    gboolean found_buffer;
    
    channel = start;
    
    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	current = ags_channel_nth(output,
				  start_audio_channel + j);
	
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* check output */
	while(current != NULL){
	  /* get channel mutex */
	  pthread_mutex_lock(ags_channel_get_class_mutex());

	  current_mutex = current->obj_mutex;
  
	  pthread_mutex_unlock(ags_channel_get_class_mutex());

	  /* check output present */
	  g_object_get(channel,
		       "recall", &list_start,
		       NULL);

	  list = list_start;
	  found_buffer = FALSE;
	
	  while((list = ags_recall_template_find_type(list,
						      AGS_TYPE_BUFFER_CHANNEL)) != NULL){
	    AgsChannel *current_destination;

	    g_object_get(list->data,
			 "destination", &current_destination,
			 NULL);

	    if(current_destination == current){
	      found_buffer = TRUE;
	      break;
	    }
	
	    list = list->next;
	  }

	  g_list_free(list_start);
	
	  if(found_buffer){
	    /* iterate - current */
	    pthread_mutex_lock(current_mutex);

	    current = current->next_pad;

	    pthread_mutex_unlock(current_mutex);

	    continue;
	  }

	  if(recall_container == NULL){
	    recall_container = ags_recall_container_new();
	    ags_audio_add_recall_container(audio, (GObject *) recall_container);
	  }
	
	  ags_channel_add_recall_container(channel, (GObject *) recall_container);

	  /* AgsBufferChannel */
	  buffer_channel = (AgsBufferChannel *) g_object_new(AGS_TYPE_BUFFER_CHANNEL,
							     "output-soundcard", output_soundcard,
							     "source", channel,
							     "destination", current,
							     "recall-container", recall_container,
							     NULL);
	  ags_recall_set_flags(buffer_channel,
			       (AGS_RECALL_TEMPLATE));
	  ags_recall_set_ability_flags(buffer_channel,
				       (AGS_SOUND_ABILITY_PLAYBACK |
					AGS_SOUND_ABILITY_NOTATION |
					AGS_SOUND_ABILITY_SEQUENCER |
					AGS_SOUND_ABILITY_MIDI |
					AGS_SOUND_ABILITY_WAVE));
	  ags_recall_set_behaviour_flags(buffer_channel,
					 (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	  ags_channel_add_recall(channel,
				 buffer_channel, FALSE);

	  recall = g_list_prepend(recall,
				  buffer_channel);

	  g_object_set(recall_container,
		       "recall-channel", buffer_channel,
		       NULL);
	  ags_connectable_connect(AGS_CONNECTABLE(buffer_channel));

	  /* AgsBufferChannelRun */
	  buffer_channel_run = (AgsBufferChannelRun *) g_object_new(AGS_TYPE_BUFFER_CHANNEL_RUN,
								    "output-soundcard", output_soundcard,
								    "recall-channel", buffer_channel,
								    "source", channel,
								    "destination", current,
								    "recall-container", recall_container,
								    NULL);
	  ags_recall_set_flags(buffer_channel_run,
			       (AGS_RECALL_TEMPLATE));
	  ags_recall_set_ability_flags(buffer_channel_run,
				       (AGS_SOUND_ABILITY_PLAYBACK |
					AGS_SOUND_ABILITY_NOTATION |
					AGS_SOUND_ABILITY_SEQUENCER |
					AGS_SOUND_ABILITY_MIDI |
					AGS_SOUND_ABILITY_WAVE));
	  ags_recall_set_behaviour_flags(buffer_channel_run,
					 (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	  ags_channel_add_recall(channel,
				 buffer_channel_run, FALSE);

	  recall = g_list_prepend(recall,
				  buffer_channel_run);

	  g_object_set(recall_container,
		       "recall-channel-run", buffer_channel_run,
		       NULL);
	  ags_connectable_connect(AGS_CONNECTABLE(buffer_channel_run));

	  /* iterate - current */
	  pthread_mutex_lock(current_mutex);

	  current = current->next_pad;

	  pthread_mutex_unlock(current_mutex);
	}
	
	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_delay(AgsAudio *audio,
				AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				gchar *plugin_name,
				guint start_audio_channel, guint stop_audio_channel,
				guint start_pad, guint stop_pad,
				guint create_flags, guint recall_flags)
{
  AgsDelayAudio *delay_audio;
  AgsDelayAudioRun *delay_audio_run;
  AgsPort *port;

  GObject *output_soundcard;
  
  GList *list_start, *list;
  GList *recall;

  guint i, j;

  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;
  
  pthread_mutex_unlock(audio_mutex);

  /* list */
  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if(play_container == NULL){
      play_container = ags_recall_container_new();
    }

    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) play_container);

    delay_audio = (AgsDelayAudio *) g_object_new(AGS_TYPE_DELAY_AUDIO,
						 "output-soundcard", output_soundcard,
						 "audio", audio,
						 "recall-container", play_container,
						 NULL);
    ags_recall_set_flags(delay_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(delay_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_MIDI |
				  AGS_SOUND_ABILITY_WAVE));
    ags_recall_set_behaviour_flags(delay_audio,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) delay_audio, TRUE);
    recall = g_list_prepend(recall,
			    delay_audio);

    g_object_set(play_container,
		 "recall-audio", delay_audio,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(delay_audio));

    delay_audio_run = (AgsDelayAudioRun *) g_object_new(AGS_TYPE_DELAY_AUDIO_RUN,
							"output-soundcard", output_soundcard,
							"recall-audio", delay_audio,
							"recall-container", play_container,
							NULL);
    ags_recall_set_flags(delay_audio_run,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(delay_audio_run,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_MIDI |
				  AGS_SOUND_ABILITY_WAVE));
    ags_recall_set_behaviour_flags(delay_audio_run,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) delay_audio_run, TRUE);
    recall = g_list_prepend(recall,
			    delay_audio_run);

    g_object_set(play_container,
		 "recall-audio-run", delay_audio_run,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(delay_audio_run));
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    if(recall_container == NULL){
      recall_container = ags_recall_container_new();
    }

    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    delay_audio = (AgsDelayAudio *) g_object_new(AGS_TYPE_DELAY_AUDIO,
						 "output-soundcard", output_soundcard,
						 "audio", audio,
						 "recall-container", recall_container,
						 NULL);
    ags_recall_set_flags(delay_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(delay_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_MIDI |
				  AGS_SOUND_ABILITY_WAVE));
    ags_recall_set_behaviour_flags(delay_audio,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) delay_audio, FALSE);
    recall = g_list_prepend(recall,
			    delay_audio);

    g_object_set(recall_container,
		 "recall-audio", delay_audio,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(delay_audio));

    delay_audio_run = (AgsDelayAudioRun *) g_object_new(AGS_TYPE_DELAY_AUDIO_RUN,
							"output-soundcard", output_soundcard,
							"recall-audio", delay_audio,
							"recall-container", recall_container,
							//TODO:JK: add missing dependency "delay-audio"
							NULL);
    ags_recall_set_flags(delay_audio_run,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(delay_audio_run,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_MIDI |
				  AGS_SOUND_ABILITY_WAVE));
    ags_recall_set_behaviour_flags(delay_audio_run,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) delay_audio_run, FALSE);
    recall = g_list_prepend(recall,
			    delay_audio_run);

    g_object_set(recall_container,
		 "recall-audio-run", delay_audio_run,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(delay_audio_run));
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_count_beats(AgsAudio *audio,
				      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				      gchar *plugin_name,
				      guint start_audio_channel, guint stop_audio_channel,
				      guint start_pad, guint stop_pad,
				      guint create_flags, guint recall_flags)
{
  AgsCountBeatsAudio *count_beats_audio;
  AgsCountBeatsAudioRun *count_beats_audio_run;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint i, j;

  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;
  
  pthread_mutex_unlock(audio_mutex);

  /* list */
  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if(play_container == NULL){
      play_container = ags_recall_container_new();
    }

    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) play_container);

    count_beats_audio = (AgsCountBeatsAudio *) g_object_new(AGS_TYPE_COUNT_BEATS_AUDIO,
							    "output-soundcard", output_soundcard,
							    "audio", audio,
							    "recall-container", play_container,
							    NULL);
    ags_recall_set_flags(count_beats_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(count_beats_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_MIDI |
				  AGS_SOUND_ABILITY_WAVE));
    ags_recall_set_behaviour_flags(count_beats_audio,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) count_beats_audio, TRUE);
    recall = g_list_prepend(recall,
			    count_beats_audio);

    g_object_set(play_container,
		 "recall-audio", count_beats_audio,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(count_beats_audio));

    count_beats_audio_run = (AgsCountBeatsAudioRun *) g_object_new(AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
								   "output-soundcard", output_soundcard,
								   "recall-audio", count_beats_audio,
								   "recall-container", play_container,
								   NULL);
    ags_recall_set_flags(count_beats_audio_run,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(count_beats_audio_run,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_MIDI |
				  AGS_SOUND_ABILITY_WAVE));
    ags_recall_set_behaviour_flags(count_beats_audio_run,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) count_beats_audio_run, TRUE);
    recall = g_list_prepend(recall,
			    count_beats_audio_run);

    g_object_set(play_container,
		 "recall-audio-run", count_beats_audio_run,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(count_beats_audio_run));
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    if(recall_container == NULL){
      recall_container = ags_recall_container_new();
    }

    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    count_beats_audio = (AgsCountBeatsAudio *) g_object_new(AGS_TYPE_COUNT_BEATS_AUDIO,
							    "output-soundcard", output_soundcard,
							    "audio", audio,
							    "recall-container", recall_container,
							    NULL);
    ags_recall_set_flags(count_beats_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(count_beats_audio,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_MIDI |
				  AGS_SOUND_ABILITY_WAVE));
    ags_recall_set_behaviour_flags(count_beats_audio,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) count_beats_audio, FALSE);
    recall = g_list_prepend(recall,
			    count_beats_audio);

    g_object_set(recall_container,
		 "recall-audio", count_beats_audio,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(count_beats_audio));

    count_beats_audio_run = (AgsCountBeatsAudioRun *) g_object_new(AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
								   "output-soundcard", output_soundcard,
								   "recall-audio", count_beats_audio,
								   "recall-container", recall_container,
								   //TODO:JK: add missing dependency "delay-audio"
								   NULL);
    ags_recall_set_flags(count_beats_audio_run,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(count_beats_audio_run,
				 (AGS_SOUND_ABILITY_PLAYBACK |
				  AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_SEQUENCER |
				  AGS_SOUND_ABILITY_MIDI |
				  AGS_SOUND_ABILITY_WAVE));
    ags_recall_set_behaviour_flags(count_beats_audio_run,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) count_beats_audio_run, FALSE);
    recall = g_list_prepend(recall,
			    count_beats_audio_run);

    g_object_set(recall_container,
		 "recall-audio-run", count_beats_audio_run,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(count_beats_audio_run));
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_loop(AgsAudio *audio,
			       AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			       gchar *plugin_name,
			       guint start_audio_channel, guint stop_audio_channel,
			       guint start_pad, guint stop_pad,
			       guint create_flags, guint recall_flags)
{
  AgsLoopChannel *loop_channel;
  AgsLoopChannelRun *loop_channel_run;
  AgsChannel *output, *input;
  AgsChannel *start, *channel;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint audio_channels;
  guint i, j;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;

  output = audio->output;
  input = audio->input;
  
  audio_channels = audio->audio_channels;
  
  pthread_mutex_unlock(audio_mutex);

  /* get channel */
  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(output,
				start_pad * audio_channels);
  }else{
    start =
      channel = ags_channel_nth(input,
				start_pad * audio_channels);
  }

  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if((AGS_RECALL_FACTORY_REMAP & (create_flags)) == 0){
      if(play_container == NULL){
	play_container = ags_recall_container_new();
      }

      play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
      ags_audio_add_recall_container(audio, (GObject *) play_container);
    }else{
      //TODO:JK: implement me
    }

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) play_container);

	/* AgsLoopChannel */
	loop_channel = (AgsLoopChannel *) g_object_new(AGS_TYPE_LOOP_CHANNEL,
						       "output-soundcard", output_soundcard,
						       "source", channel,
						       "recall-container", play_container,
						       //TODO:JK: add missing dependency "delay_audio"
						       NULL);
	ags_recall_set_flags(loop_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(loop_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(loop_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) loop_channel, TRUE);
	recall = g_list_prepend(recall,
				loop_channel);

	g_object_set(play_container,
		     "recall-channel", loop_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(loop_channel));

	/* AgsLoopChannelRun */
	loop_channel_run = (AgsLoopChannelRun *) g_object_new(AGS_TYPE_LOOP_CHANNEL_RUN,
							      "output-soundcard", output_soundcard,
							      "source", channel,
							      "recall-channel", loop_channel,
							      "recall-container", play_container,
							      //TODO:JK: add missing dependency "count_beats_audio_run"
							      NULL);
	ags_recall_set_flags(loop_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(loop_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(loop_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) loop_channel_run, TRUE);
	recall = g_list_prepend(recall,
				loop_channel_run);

	g_object_set(play_container,
		     "recall-channel-run", loop_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(loop_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    channel = start;

    if((AGS_RECALL_FACTORY_REMAP & (create_flags)) == 0){
      if(recall_container == NULL){
	recall_container = ags_recall_container_new();
      }

      ags_audio_add_recall_container(audio, (GObject *) recall_container);
    }else{
      //TODO:JK: implement me
    }

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) recall_container);

	/* AgsLoopChannel */
	loop_channel = (AgsLoopChannel *) g_object_new(AGS_TYPE_LOOP_CHANNEL,
						       "output-soundcard", output_soundcard,
						       "source", channel,
						       "recall-container", recall_container,
						       //TODO:JK: add missing dependency "delay_audio"
						       NULL);
	ags_recall_set_flags(loop_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(loop_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(loop_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) loop_channel, FALSE);
	recall = g_list_prepend(recall,
				loop_channel);

	g_object_set(recall_container,
		     "recall-channel", loop_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(loop_channel));

	/* AgsLoopChannelRun */
	loop_channel_run = (AgsLoopChannelRun *) g_object_new(AGS_TYPE_LOOP_CHANNEL_RUN,
							      "output-soundcard", output_soundcard,
							      "source", channel,
							      "recall-channel", loop_channel,
							      "recall-container", recall_container,
							      //TODO:JK: add missing dependency "count_beats_audio_run"
							      NULL);
	ags_recall_set_flags(loop_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(loop_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(loop_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) loop_channel_run, FALSE);
	recall = g_list_prepend(recall,
				loop_channel_run);

	g_object_set(recall_container,
		     "recall-channel-run", loop_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(loop_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}


GList*
ags_recall_factory_create_copy_pattern(AgsAudio *audio,
				       AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				       gchar *plugin_name,
				       guint start_audio_channel, guint stop_audio_channel,
				       guint start_pad, guint stop_pad,
				       guint create_flags, guint recall_flags)
{
  AgsCopyPatternAudio *copy_pattern_audio;
  AgsCopyPatternAudioRun *copy_pattern_audio_run;
  AgsCopyPatternChannel *copy_pattern_channel;
  AgsCopyPatternChannelRun *copy_pattern_channel_run;
  AgsChannel *output, *input;
  AgsChannel *start, *channel;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint audio_channels;
  guint i, j;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;

  output = audio->output;
  input = audio->input;
  
  audio_channels = audio->audio_channels;
  
  pthread_mutex_unlock(audio_mutex);

  /* get channel */
  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(output,
				start_pad * audio_channels);
  }else{
    start =
      channel = ags_channel_nth(input,
				start_pad * audio_channels);
  }

  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if((AGS_RECALL_FACTORY_REMAP & (create_flags)) == 0 ||
       ags_recall_find_type(audio->play, AGS_TYPE_COPY_PATTERN_AUDIO) == NULL){
      if(play_container == NULL){
	play_container = ags_recall_container_new();
      }

      play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
      ags_audio_add_recall_container(audio, (GObject *) play_container);

      /* AgsCopyPatternAudio */
      copy_pattern_audio = (AgsCopyPatternAudio *) g_object_new(AGS_TYPE_COPY_PATTERN_AUDIO,
								"output-soundcard", output_soundcard,
								"audio", audio,
								"recall-container", play_container,
								NULL);
      ags_recall_set_flags(copy_pattern_audio,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags(copy_pattern_audio,
				   (AGS_SOUND_ABILITY_SEQUENCER));
      ags_recall_set_behaviour_flags(copy_pattern_audio,
				     (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_audio_add_recall(audio, (GObject *) copy_pattern_audio, TRUE);
      recall = g_list_prepend(recall,
			      copy_pattern_audio);

      g_object_set(play_container,
		   "recall-audio", copy_pattern_audio,
		   NULL);

      /* AgsCopyPatternAudioRun */
      copy_pattern_audio_run = (AgsCopyPatternAudioRun *) g_object_new(AGS_TYPE_COPY_PATTERN_AUDIO_RUN,
								       "output-soundcard", output_soundcard,
								       "audio", audio,
								       "recall-container", play_container,
								       "recall-audio", copy_pattern_audio,
								       //TODO:JK: add missing dependency "count_beats_audio_run"
								       NULL);
      ags_recall_set_flags(copy_pattern_audio_run,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags(copy_pattern_audio_run,
				   (AGS_SOUND_ABILITY_SEQUENCER));
      ags_recall_set_behaviour_flags(copy_pattern_audio_run,
				     (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_audio_add_recall(audio, (GObject *) copy_pattern_audio_run, TRUE);
      recall = g_list_prepend(recall,
			      copy_pattern_audio_run);

      g_object_set(play_container,
		   "recall-audio-run", copy_pattern_audio_run,
		   NULL);
    }else{
      if(play_container == NULL){
	g_object_get(audio,
		     "play", &list_start,
		     NULL);

	list = ags_recall_find_type(list_start,
				    AGS_TYPE_COPY_PATTERN_AUDIO);
	copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);
	g_list_free(list_start);
	
	recall = g_list_prepend(recall,
				copy_pattern_audio);

	g_object_get(copy_pattern_audio,
		     "recall-container", &play_container,
		     NULL);

	g_object_get(play_container,
		     "recall-audio-run", &list_start,
		     NULL);
	
	list = ags_recall_find_template(list_start);
	copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(list->data);
	g_list_free(list_start);
	
	recall = g_list_prepend(recall,
				copy_pattern_audio_run);
      }else{
	g_object_get(play_container,
		     "recall-audio", &copy_pattern_audio,
		     NULL);
	recall = g_list_prepend(recall,
				copy_pattern_audio);

	g_object_get(play_container,
		     "recall-audio-run", &list_start,
		     NULL);

	list = ags_recall_find_template(list_start);
	copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(list->data);
	g_list_free(list_start);
	
	recall = g_list_prepend(recall,
				copy_pattern_audio_run);
      }
    }

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) play_container);

	/* AgsCopyPatternChannel in channel->recall */
	copy_pattern_channel = (AgsCopyPatternChannel *) g_object_new(AGS_TYPE_COPY_PATTERN_CHANNEL,
								      "output-soundcard", output_soundcard,
								      "source", channel,
								      // "destination", destination,
								      "recall-container", play_container,
								      "recall-audio", copy_pattern_audio,
								      // "pattern", channel->pattern->data,
								      NULL);
	ags_recall_set_flags(copy_pattern_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(copy_pattern_channel,
				     (AGS_SOUND_ABILITY_SEQUENCER));
	ags_recall_set_behaviour_flags(copy_pattern_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) copy_pattern_channel, TRUE);
	recall = g_list_prepend(recall,
				copy_pattern_channel);

	g_object_set(play_container,
		     "recall-channel", copy_pattern_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(copy_pattern_channel));

	/* AgsCopyPatternChannelRun */
	copy_pattern_channel_run = (AgsCopyPatternChannelRun *) g_object_new(AGS_TYPE_COPY_PATTERN_CHANNEL_RUN,
									     "output-soundcard", output_soundcard,
									     "source", channel,
									     // "destination", destination,
									     // "recall_audio_run", copy_pattern_audio_run,
									     "recall-container", play_container,
									     "recall-audio", copy_pattern_audio,
									     "recall-audio-run", copy_pattern_audio_run,
									     "recall-channel", copy_pattern_channel,
									     NULL);
	ags_recall_set_flags(copy_pattern_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(copy_pattern_channel_run,
				     (AGS_SOUND_ABILITY_SEQUENCER));
	ags_recall_set_behaviour_flags(copy_pattern_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) copy_pattern_channel_run, TRUE);   
	recall = g_list_prepend(recall,
				copy_pattern_channel_run);

	g_object_set(play_container,
		     "recall-channel-run", copy_pattern_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(copy_pattern_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    channel = start;
 
    if((AGS_RECALL_FACTORY_REMAP & (create_flags)) == 0 ||
       ags_recall_find_type(audio->recall, AGS_TYPE_COPY_PATTERN_AUDIO) == NULL){
      if(recall_container == NULL){
	recall_container = ags_recall_container_new();
      }

      ags_audio_add_recall_container(audio, (GObject *) recall_container);

      /* AgsCopyPatternAudio */
      copy_pattern_audio = (AgsCopyPatternAudio *) g_object_new(AGS_TYPE_COPY_PATTERN_AUDIO,
								"output-soundcard", output_soundcard,
								"audio", audio,
								"recall-container", recall_container,
								NULL);
      ags_recall_set_flags(copy_pattern_audio,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags(copy_pattern_audio,
				   (AGS_SOUND_ABILITY_SEQUENCER));
      ags_recall_set_behaviour_flags(copy_pattern_audio,
				     (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_audio_add_recall(audio, (GObject *) copy_pattern_audio, FALSE);
      recall = g_list_prepend(recall,
			      copy_pattern_audio);

      g_object_set(recall_container,
		   "recall-audio", copy_pattern_audio,
		   NULL);
      
      /* AgsCopyPatternAudioRun */
      copy_pattern_audio_run = (AgsCopyPatternAudioRun *) g_object_new(AGS_TYPE_COPY_PATTERN_AUDIO_RUN,
								       "output-soundcard", output_soundcard,
								       "recall-container", recall_container,
								       "recall-audio", copy_pattern_audio,
								       //TODO:JK: add missing dependency "count_beats_audio_run"
								       NULL);
      ags_recall_set_flags(copy_pattern_audio_run,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags(copy_pattern_audio_run,
				   (AGS_SOUND_ABILITY_SEQUENCER));
      ags_recall_set_behaviour_flags(copy_pattern_audio_run,
				     (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_audio_add_recall(audio, (GObject *) copy_pattern_audio_run, FALSE);
      recall = g_list_prepend(recall,
			      copy_pattern_audio_run);

      g_object_set(recall_container,
		   "recall-audio-run", copy_pattern_audio_run,
		   NULL);
    }else{
      if(recall_container == NULL){	
	g_object_get(audio,
		     "recall", &list_start,
		     NULL);

	list = ags_recall_find_type(list_start,
				    AGS_TYPE_COPY_PATTERN_AUDIO);
	copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);
	g_list_free(list_start);
	
	recall = g_list_prepend(recall,
				copy_pattern_audio);

	g_object_get(copy_pattern_audio,
		     "recall-container", &recall_container,
		     NULL);

	g_object_get(recall_container,
		     "recall-audio-run", &list_start,
		     NULL);
	
	list = ags_recall_find_template(list_start);
	copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(list->data);
	g_list_free(list_start);
	
	recall = g_list_prepend(recall,
				copy_pattern_audio_run);
      }else{
	g_object_get(recall_container,
		     "recall-audio", &copy_pattern_audio,
		     NULL);
	recall = g_list_prepend(recall,
				copy_pattern_audio);

	g_object_get(recall_container,
		     "recall-audio-run", &list_start,
		     NULL);

	list = ags_recall_template_find_type(list_start,
					     AGS_TYPE_COPY_PATTERN_AUDIO_RUN);
	copy_pattern_audio_run = list->data;
	recall = g_list_prepend(recall,
				copy_pattern_audio_run);

	g_list_free(list_start);
      }
    }

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) recall_container);

	/* AgsCopyPatternChannel in channel->recall */
	copy_pattern_channel = (AgsCopyPatternChannel *) g_object_new(AGS_TYPE_COPY_PATTERN_CHANNEL,
								      "output-soundcard", output_soundcard,
								      "source", channel,
								      // "destination", destination,
								      "recall-container", recall_container,
								      "recall-audio", copy_pattern_audio,
								      //"pattern", channel->pattern->data,
								      NULL);
	ags_recall_set_flags(copy_pattern_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(copy_pattern_channel,
				     (AGS_SOUND_ABILITY_SEQUENCER));
	ags_recall_set_behaviour_flags(copy_pattern_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) copy_pattern_channel, FALSE);
	recall = g_list_prepend(recall,
				copy_pattern_channel);

	g_object_set(recall_container,
		     "recall-channel", copy_pattern_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(copy_pattern_channel));

	/* AgsCopyPatternChannelRun */
	copy_pattern_channel_run = (AgsCopyPatternChannelRun *) g_object_new(AGS_TYPE_COPY_PATTERN_CHANNEL_RUN,
									     "output-soundcard", output_soundcard,
									     "source", channel,
									     // "destination", destination,
									     // "recall_audio_run", copy_pattern_audio_run,
									     "recall-container", recall_container,
									     "recall-audio", copy_pattern_audio,
									     "recall-audio-run", copy_pattern_audio_run,
									     "recall-channel", copy_pattern_channel,
									     NULL);
	ags_recall_set_flags(copy_pattern_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(copy_pattern_channel_run,
				     (AGS_SOUND_ABILITY_SEQUENCER));
	ags_recall_set_behaviour_flags(copy_pattern_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) copy_pattern_channel_run, FALSE);   
	recall = g_list_prepend(recall,
				copy_pattern_channel_run);

	g_object_set(recall_container,
		     "recall-channel-run", copy_pattern_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(copy_pattern_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_play_wave(AgsAudio *audio,
				    AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				    gchar *plugin_name,
				    guint start_audio_channel, guint stop_audio_channel,
				    guint start_pad, guint stop_pad,
				    guint create_flags, guint recall_flags)
{
  AgsPlayWaveAudio *play_wave_audio;
  AgsPlayWaveAudioRun *play_wave_audio_run;
  AgsPlayWaveChannel *play_wave_channel;
  AgsPlayWaveChannelRun *play_wave_channel_run;
  AgsChannel *output, *input;
  AgsChannel *start, *channel;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint audio_channels;
  guint i, j;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;

  output = audio->output;
  input = audio->input;
  
  audio_channels = audio->audio_channels;
  
  pthread_mutex_unlock(audio_mutex);

  /* get channel */
  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(output,
				start_pad * audio_channels);
  }else{
    start =
      channel = ags_channel_nth(input,
				start_pad * audio_channels);
  }

  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if((AGS_RECALL_FACTORY_REMAP & (create_flags)) == 0 ||
       ags_recall_find_type(audio->play, AGS_TYPE_PLAY_WAVE_AUDIO) == NULL){
      if(play_container == NULL){
	play_container = ags_recall_container_new();
      }

      play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
      ags_audio_add_recall_container(audio, (GObject *) play_container);

      /* AgsPlayWaveAudio */
      play_wave_audio = (AgsPlayWaveAudio *) g_object_new(AGS_TYPE_PLAY_WAVE_AUDIO,
							  "output-soundcard", output_soundcard,
							  "audio", audio,
							  "recall-container", play_container,
							  NULL);
      ags_recall_set_flags(play_wave_audio,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags(play_wave_audio,
				   (AGS_SOUND_ABILITY_WAVE));
      ags_recall_set_behaviour_flags(play_wave_audio,
				     (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_audio_add_recall(audio, (GObject *) play_wave_audio, TRUE);
      recall = g_list_prepend(recall,
			      play_wave_audio);

      g_object_set(play_container,
		   "recall-audio", play_wave_audio,
		   NULL);

      /* AgsPlayWaveAudioRun */
      play_wave_audio_run = (AgsPlayWaveAudioRun *) g_object_new(AGS_TYPE_PLAY_WAVE_AUDIO_RUN,
								 "output-soundcard", output_soundcard,
								 "audio", audio,
								 "recall-container", play_container,
								 "recall-audio", play_wave_audio,
								 //TODO:JK: add missing dependency "count_beats_audio_run"
								 NULL);
      ags_recall_set_flags(play_wave_audio_run,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags(play_wave_audio_run,
				   (AGS_SOUND_ABILITY_WAVE));
      ags_recall_set_behaviour_flags(play_wave_audio_run,
				     (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_audio_add_recall(audio, (GObject *) play_wave_audio_run, TRUE);
      recall = g_list_prepend(recall,
			      play_wave_audio_run);

      g_object_set(play_container,
		   "recall-audio-run", play_wave_audio_run,
		   NULL);
    }else{
      if(play_container == NULL){
	g_object_get(audio,
		     "play", &list_start,
		     NULL);

	list = ags_recall_find_type(list_start,
				    AGS_TYPE_PLAY_WAVE_AUDIO);
	play_wave_audio = AGS_PLAY_WAVE_AUDIO(list->data);
	g_list_free(list_start);
	
	recall = g_list_prepend(recall,
				play_wave_audio);

	g_object_get(play_wave_audio,
		     "recall-container", &play_container,
		     NULL);

	g_object_get(play_container,
		     "recall-audio-run", &list_start,
		     NULL);
	
	list = ags_recall_find_template(list_start);
	play_wave_audio_run = AGS_PLAY_WAVE_AUDIO_RUN(list->data);
	g_list_free(list_start);
	
	recall = g_list_prepend(recall,
				play_wave_audio_run);
      }else{
	g_object_get(play_container,
		     "recall-audio", &play_wave_audio,
		     NULL);
	recall = g_list_prepend(recall,
				play_wave_audio);

	g_object_get(play_container,
		     "recall-audio-run", &list_start,
		     NULL);

	list = ags_recall_find_template(list_start);
	play_wave_audio_run = AGS_PLAY_WAVE_AUDIO_RUN(list->data);
	g_list_free(list_start);
	
	recall = g_list_prepend(recall,
				play_wave_audio_run);
      }
    }

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) play_container);

	/* AgsPlayWaveChannel in channel->recall */
	play_wave_channel = (AgsPlayWaveChannel *) g_object_new(AGS_TYPE_PLAY_WAVE_CHANNEL,
								"output-soundcard", output_soundcard,
								"source", channel,
								// "destination", destination,
								"recall-container", play_container,
								"recall-audio", play_wave_audio,
								// "pattern", channel->pattern->data,
								NULL);
	ags_recall_set_flags(play_wave_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(play_wave_channel,
				     (AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(play_wave_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) play_wave_channel, TRUE);
	recall = g_list_prepend(recall,
				play_wave_channel);

	g_object_set(play_container,
		     "recall-channel", play_wave_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(play_wave_channel));

	/* AgsPlayWaveChannelRun */
	play_wave_channel_run = (AgsPlayWaveChannelRun *) g_object_new(AGS_TYPE_PLAY_WAVE_CHANNEL_RUN,
								       "output-soundcard", output_soundcard,
								       "source", channel,
								       // "destination", destination,
								       // "recall_audio_run", play_wave_audio_run,
								       "recall-container", play_container,
								       "recall-audio", play_wave_audio,
								       "recall-audio-run", play_wave_audio_run,
								       "recall-channel", play_wave_channel,
								       NULL);
	ags_recall_set_flags(play_wave_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(play_wave_channel_run,
				     (AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(play_wave_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) play_wave_channel_run, TRUE);   
	recall = g_list_prepend(recall,
				play_wave_channel_run);

	g_object_set(play_container,
		     "recall-channel-run", play_wave_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(play_wave_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    channel = start;
 
    if((AGS_RECALL_FACTORY_REMAP & (create_flags)) == 0 ||
       ags_recall_find_type(audio->recall, AGS_TYPE_PLAY_WAVE_AUDIO) == NULL){
      if(recall_container == NULL){
	recall_container = ags_recall_container_new();
      }

      ags_audio_add_recall_container(audio, (GObject *) recall_container);

      /* AgsPlayWaveAudio */
      play_wave_audio = (AgsPlayWaveAudio *) g_object_new(AGS_TYPE_PLAY_WAVE_AUDIO,
							  "output-soundcard", output_soundcard,
							  "audio", audio,
							  "recall-container", recall_container,
							  NULL);
      ags_recall_set_flags(play_wave_audio,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags(play_wave_audio,
				   (AGS_SOUND_ABILITY_WAVE));
      ags_recall_set_behaviour_flags(play_wave_audio,
				     (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_audio_add_recall(audio, (GObject *) play_wave_audio, FALSE);
      recall = g_list_prepend(recall,
			      play_wave_audio);

      g_object_set(recall_container,
		   "recall-audio", play_wave_audio,
		   NULL);
      
      /* AgsPlayWaveAudioRun */
      play_wave_audio_run = (AgsPlayWaveAudioRun *) g_object_new(AGS_TYPE_PLAY_WAVE_AUDIO_RUN,
								 "output-soundcard", output_soundcard,
								 "recall-container", recall_container,
								 "recall-audio", play_wave_audio,
								 //TODO:JK: add missing dependency "count_beats_audio_run"
								 NULL);
      ags_recall_set_flags(play_wave_audio_run,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags(play_wave_audio_run,
				   (AGS_SOUND_ABILITY_WAVE));
      ags_recall_set_behaviour_flags(play_wave_audio_run,
				     (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_audio_add_recall(audio, (GObject *) play_wave_audio_run, FALSE);
      recall = g_list_prepend(recall,
			      play_wave_audio_run);

      g_object_set(recall_container,
		   "recall-audio-run", play_wave_audio_run,
		   NULL);
    }else{
      if(recall_container == NULL){	
	g_object_get(audio,
		     "recall", &list_start,
		     NULL);

	list = ags_recall_find_type(list_start,
				    AGS_TYPE_PLAY_WAVE_AUDIO);
	play_wave_audio = AGS_PLAY_WAVE_AUDIO(list->data);
	g_list_free(list_start);
	
	recall = g_list_prepend(recall,
				play_wave_audio);

	g_object_get(play_wave_audio,
		     "recall-container", &recall_container,
		     NULL);

	g_object_get(recall_container,
		     "recall-audio-run", &list_start,
		     NULL);
	
	list = ags_recall_find_template(list_start);
	play_wave_audio_run = AGS_PLAY_WAVE_AUDIO_RUN(list->data);
	g_list_free(list_start);
	
	recall = g_list_prepend(recall,
				play_wave_audio_run);
      }else{
	g_object_get(recall_container,
		     "recall-audio", &play_wave_audio,
		     NULL);
	recall = g_list_prepend(recall,
				play_wave_audio);

	g_object_get(recall_container,
		     "recall-audio-run", &list_start,
		     NULL);

	list = ags_recall_template_find_type(list_start,
					     AGS_TYPE_PLAY_WAVE_AUDIO_RUN);
	play_wave_audio_run = list->data;
	recall = g_list_prepend(recall,
				play_wave_audio_run);

	g_list_free(list_start);
      }
    }

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) recall_container);
	
	/* AgsPlayWaveChannel in channel->recall */
	play_wave_channel = (AgsPlayWaveChannel *) g_object_new(AGS_TYPE_PLAY_WAVE_CHANNEL,
								"output-soundcard", output_soundcard,
								"source", channel,
								// "destination", destination,
								"recall-container", recall_container,
								"recall-audio", play_wave_audio,
								//"pattern", channel->pattern->data,
								NULL);
	ags_recall_set_flags(play_wave_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(play_wave_channel,
				     (AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(play_wave_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) play_wave_channel, FALSE);
	recall = g_list_prepend(recall,
				play_wave_channel);

	g_object_set(recall_container,
		     "recall-channel", play_wave_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(play_wave_channel));

	/* AgsPlayWaveChannelRun */
	play_wave_channel_run = (AgsPlayWaveChannelRun *) g_object_new(AGS_TYPE_PLAY_WAVE_CHANNEL_RUN,
								       "output-soundcard", output_soundcard,
								       "source", channel,
								       // "destination", destination,
								       // "recall_audio_run", play_wave_audio_run,
								       "recall-container", recall_container,
								       "recall-audio", play_wave_audio,
								       "recall-audio-run", play_wave_audio_run,
								       "recall-channel", play_wave_channel,
								       NULL);
	ags_recall_set_flags(play_wave_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(play_wave_channel_run,
				     (AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(play_wave_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) play_wave_channel_run, FALSE);   
	recall = g_list_prepend(recall,
				play_wave_channel_run);

	g_object_set(recall_container,
		     "recall-channel-run", play_wave_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(play_wave_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_capture_wave(AgsAudio *audio,
				       AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				       gchar *plugin_name,
				       guint start_audio_channel, guint stop_audio_channel,
				       guint start_pad, guint stop_pad,
				       guint create_flags, guint recall_flags)
{
  AgsCaptureWaveAudio *capture_wave_audio;
  AgsCaptureWaveAudioRun *capture_wave_audio_run;
  AgsCaptureWaveChannel *capture_wave_channel;
  AgsCaptureWaveChannelRun *capture_wave_channel_run;
  AgsChannel *output, *input;
  AgsChannel *start, *channel;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint audio_channels;
  guint i, j;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;

  output = audio->output;
  input = audio->input;
  
  audio_channels = audio->audio_channels;
  
  pthread_mutex_unlock(audio_mutex);

  /* get channel */
  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(output,
				start_pad * audio_channels);
  }else{
    start =
      channel = ags_channel_nth(input,
				start_pad * audio_channels);
  }

  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if((AGS_RECALL_FACTORY_REMAP & (create_flags)) == 0 ||
       ags_recall_find_type(audio->play, AGS_TYPE_CAPTURE_WAVE_AUDIO) == NULL){
      if(play_container == NULL){
	play_container = ags_recall_container_new();
      }

      play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
      ags_audio_add_recall_container(audio, (GObject *) play_container);

      /* AgsCaptureWaveAudio */
      capture_wave_audio = (AgsCaptureWaveAudio *) g_object_new(AGS_TYPE_CAPTURE_WAVE_AUDIO,
								"output-soundcard", output_soundcard,
								"audio", audio,
								"recall-container", play_container,
								NULL);
      ags_recall_set_flags(capture_wave_audio,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags(capture_wave_audio,
				   (AGS_SOUND_ABILITY_WAVE));
      ags_recall_set_behaviour_flags(capture_wave_audio,
				     (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_audio_add_recall(audio, (GObject *) capture_wave_audio, TRUE);
      recall = g_list_prepend(recall,
			      capture_wave_audio);

      g_object_set(play_container,
		   "recall-audio", capture_wave_audio,
		   NULL);

      /* AgsCaptureWaveAudioRun */
      capture_wave_audio_run = (AgsCaptureWaveAudioRun *) g_object_new(AGS_TYPE_CAPTURE_WAVE_AUDIO_RUN,
								       "output-soundcard", output_soundcard,
								       "audio", audio,
								       "recall-container", play_container,
								       "recall-audio", capture_wave_audio,
								       //TODO:JK: add missing dependency "count_beats_audio_run"
								       NULL);
      ags_recall_set_flags(capture_wave_audio_run,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags(capture_wave_audio_run,
				   (AGS_SOUND_ABILITY_WAVE));
      ags_recall_set_behaviour_flags(capture_wave_audio_run,
				     (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_audio_add_recall(audio, (GObject *) capture_wave_audio_run, TRUE);
      recall = g_list_prepend(recall,
			      capture_wave_audio_run);

      g_object_set(play_container,
		   "recall-audio-run", capture_wave_audio_run,
		   NULL);
    }else{
      if(play_container == NULL){
	g_object_get(audio,
		     "play", &list_start,
		     NULL);

	list = ags_recall_find_type(list_start,
				    AGS_TYPE_CAPTURE_WAVE_AUDIO);
	capture_wave_audio = AGS_CAPTURE_WAVE_AUDIO(list->data);
	g_list_free(list_start);
	
	recall = g_list_prepend(recall,
				capture_wave_audio);

	g_object_get(capture_wave_audio,
		     "recall-container", &play_container,
		     NULL);

	g_object_get(play_container,
		     "recall-audio-run", &list_start,
		     NULL);
	
	list = ags_recall_find_template(list_start);
	capture_wave_audio_run = AGS_CAPTURE_WAVE_AUDIO_RUN(list->data);
	g_list_free(list_start);
	
	recall = g_list_prepend(recall,
				capture_wave_audio_run);
      }else{
	g_object_get(play_container,
		     "recall-audio", &capture_wave_audio,
		     NULL);
	recall = g_list_prepend(recall,
				capture_wave_audio);

	g_object_get(play_container,
		     "recall-audio-run", &list_start,
		     NULL);

	list = ags_recall_find_template(list_start);
	capture_wave_audio_run = AGS_CAPTURE_WAVE_AUDIO_RUN(list->data);
	g_list_free(list_start);
	
	recall = g_list_prepend(recall,
				capture_wave_audio_run);
      }
    }

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) play_container);

	/* AgsCaptureWaveChannel in channel->recall */
	capture_wave_channel = (AgsCaptureWaveChannel *) g_object_new(AGS_TYPE_CAPTURE_WAVE_CHANNEL,
								      "output-soundcard", output_soundcard,
								      "source", channel,
								      // "destination", destination,
								      "recall-container", play_container,
								      "recall-audio", capture_wave_audio,
								      // "pattern", channel->pattern->data,
								      NULL);
	ags_recall_set_flags(capture_wave_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(capture_wave_channel,
				     (AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(capture_wave_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) capture_wave_channel, TRUE);
	recall = g_list_prepend(recall,
				capture_wave_channel);

	g_object_set(play_container,
		     "recall-channel", capture_wave_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(capture_wave_channel));

	/* AgsCaptureWaveChannelRun */
	capture_wave_channel_run = (AgsCaptureWaveChannelRun *) g_object_new(AGS_TYPE_CAPTURE_WAVE_CHANNEL_RUN,
									     "output-soundcard", output_soundcard,
									     "source", channel,
									     // "destination", destination,
									     // "recall_audio_run", capture_wave_audio_run,
									     "recall-container", play_container,
									     "recall-audio", capture_wave_audio,
									     "recall-audio-run", capture_wave_audio_run,
									     "recall-channel", capture_wave_channel,
									     NULL);
	ags_recall_set_flags(capture_wave_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(capture_wave_channel_run,
				     (AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(capture_wave_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) capture_wave_channel_run, TRUE);   
	recall = g_list_prepend(recall,
				capture_wave_channel_run);

	g_object_set(play_container,
		     "recall-channel-run", capture_wave_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(capture_wave_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    channel = start;
 
    if((AGS_RECALL_FACTORY_REMAP & (create_flags)) == 0 ||
       ags_recall_find_type(audio->recall, AGS_TYPE_CAPTURE_WAVE_AUDIO) == NULL){
      if(recall_container == NULL){
	recall_container = ags_recall_container_new();
      }

      ags_audio_add_recall_container(audio, (GObject *) recall_container);

      /* AgsCaptureWaveAudio */
      capture_wave_audio = (AgsCaptureWaveAudio *) g_object_new(AGS_TYPE_CAPTURE_WAVE_AUDIO,
								"output-soundcard", output_soundcard,
								"audio", audio,
								"recall-container", recall_container,
								NULL);
      ags_recall_set_flags(capture_wave_audio,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags(capture_wave_audio,
				   (AGS_SOUND_ABILITY_WAVE));
      ags_recall_set_behaviour_flags(capture_wave_audio,
				     (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_audio_add_recall(audio, (GObject *) capture_wave_audio, FALSE);
      recall = g_list_prepend(recall,
			      capture_wave_audio);

      g_object_set(recall_container,
		   "recall-audio", capture_wave_audio,
		   NULL);
      
      /* AgsCaptureWaveAudioRun */
      capture_wave_audio_run = (AgsCaptureWaveAudioRun *) g_object_new(AGS_TYPE_CAPTURE_WAVE_AUDIO_RUN,
								       "output-soundcard", output_soundcard,
								       "recall-container", recall_container,
								       "recall-audio", capture_wave_audio,
								       //TODO:JK: add missing dependency "count_beats_audio_run"
								       NULL);
      ags_recall_set_flags(capture_wave_audio_run,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags(capture_wave_audio_run,
				   (AGS_SOUND_ABILITY_WAVE));
      ags_recall_set_behaviour_flags(capture_wave_audio_run,
				     (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_audio_add_recall(audio, (GObject *) capture_wave_audio_run, FALSE);
      recall = g_list_prepend(recall,
			      capture_wave_audio_run);

      g_object_set(recall_container,
		   "recall-audio-run", capture_wave_audio_run,
		   NULL);
    }else{
      if(recall_container == NULL){	
	g_object_get(audio,
		     "recall", &list_start,
		     NULL);

	list = ags_recall_find_type(list_start,
				    AGS_TYPE_CAPTURE_WAVE_AUDIO);
	capture_wave_audio = AGS_CAPTURE_WAVE_AUDIO(list->data);
	g_list_free(list_start);
	
	recall = g_list_prepend(recall,
				capture_wave_audio);

	g_object_get(capture_wave_audio,
		     "recall-container", &recall_container,
		     NULL);

	g_object_get(recall_container,
		     "recall-audio-run", &list_start,
		     NULL);
	
	list = ags_recall_find_template(list_start);
	capture_wave_audio_run = AGS_CAPTURE_WAVE_AUDIO_RUN(list->data);
	g_list_free(list_start);
	
	recall = g_list_prepend(recall,
				capture_wave_audio_run);
      }else{
	g_object_get(recall_container,
		     "recall-audio", &capture_wave_audio,
		     NULL);
	recall = g_list_prepend(recall,
				capture_wave_audio);

	g_object_get(recall_container,
		     "recall-audio-run", &list_start,
		     NULL);

	list = ags_recall_template_find_type(list_start,
					     AGS_TYPE_CAPTURE_WAVE_AUDIO_RUN);
	capture_wave_audio_run = list->data;
	recall = g_list_prepend(recall,
				capture_wave_audio_run);

	g_list_free(list_start);
      }
    }

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) recall_container);
	
	/* AgsCaptureWaveChannel in channel->recall */
	capture_wave_channel = (AgsCaptureWaveChannel *) g_object_new(AGS_TYPE_CAPTURE_WAVE_CHANNEL,
								      "output-soundcard", output_soundcard,
								      "source", channel,
								      // "destination", destination,
								      "recall-container", recall_container,
								      "recall-audio", capture_wave_audio,
								      //"pattern", channel->pattern->data,
								      NULL);
	ags_recall_set_flags(capture_wave_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(capture_wave_channel,
				     (AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(capture_wave_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) capture_wave_channel, FALSE);
	recall = g_list_prepend(recall,
				capture_wave_channel);

	g_object_set(recall_container,
		     "recall-channel", capture_wave_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(capture_wave_channel));

	/* AgsCaptureWaveChannelRun */
	capture_wave_channel_run = (AgsCaptureWaveChannelRun *) g_object_new(AGS_TYPE_CAPTURE_WAVE_CHANNEL_RUN,
									     "output-soundcard", output_soundcard,
									     "source", channel,
									     // "destination", destination,
									     // "recall_audio_run", capture_wave_audio_run,
									     "recall-container", recall_container,
									     "recall-audio", capture_wave_audio,
									     "recall-audio-run", capture_wave_audio_run,
									     "recall-channel", capture_wave_channel,
									     NULL);
	ags_recall_set_flags(capture_wave_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(capture_wave_channel_run,
				     (AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(capture_wave_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) capture_wave_channel_run, FALSE);   
	recall = g_list_prepend(recall,
				capture_wave_channel_run);

	g_object_set(recall_container,
		     "recall-channel-run", capture_wave_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(capture_wave_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_play_dssi(AgsAudio *audio,
				    AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				    gchar *plugin_name,
				    guint start_audio_channel, guint stop_audio_channel,
				    guint start_pad, guint stop_pad,
				    guint create_flags, guint recall_flags)
{
  AgsPlayDssiAudio *play_dssi_audio;
  AgsPlayDssiAudioRun *play_dssi_audio_run;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint i, j;

  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;
  
  pthread_mutex_unlock(audio_mutex);

  /* list */
  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if(play_container == NULL){
      play_container = ags_recall_container_new();
    }

    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) play_container);

    play_dssi_audio = (AgsPlayDssiAudio *) g_object_new(AGS_TYPE_PLAY_DSSI_AUDIO,
							"output-soundcard", output_soundcard,
							"audio", audio,
							"recall-container", play_container,
							NULL);
    ags_recall_set_flags(play_dssi_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(play_dssi_audio,
				 (AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(play_dssi_audio,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    if((AGS_RECALL_FACTORY_BULK & create_flags) != 0){
      ags_recall_set_behaviour_flags(play_dssi_audio,
				     (AGS_SOUND_BEHAVIOUR_BULK_MODE));
    }
    
    ags_audio_add_recall(audio, (GObject *) play_dssi_audio, TRUE);
    recall = g_list_prepend(recall,
			    play_dssi_audio);

    g_object_set(play_container,
		 "recall-audio", play_dssi_audio,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(play_dssi_audio));

    play_dssi_audio_run = (AgsPlayDssiAudioRun *) g_object_new(AGS_TYPE_PLAY_DSSI_AUDIO_RUN,
							       "output-soundcard", output_soundcard,
							       "recall-audio", play_dssi_audio,
							       "recall-container", play_container,
							       NULL);
    ags_recall_set_flags(play_dssi_audio_run,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(play_dssi_audio_run,
				 (AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(play_dssi_audio_run,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) play_dssi_audio_run, TRUE);
    recall = g_list_prepend(recall,
			    play_dssi_audio_run);

    g_object_set(play_container,
		 "recall-audio-run", play_dssi_audio_run,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(play_dssi_audio_run));
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    if(recall_container == NULL){
      recall_container = ags_recall_container_new();
    }

    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    play_dssi_audio = (AgsPlayDssiAudio *) g_object_new(AGS_TYPE_PLAY_DSSI_AUDIO,
							"output-soundcard", output_soundcard,
							"audio", audio,
							"recall-container", recall_container,
							NULL);
    ags_recall_set_flags(play_dssi_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(play_dssi_audio,
				 (AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(play_dssi_audio,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    if((AGS_RECALL_FACTORY_BULK & create_flags) != 0){
      ags_recall_set_behaviour_flags(play_dssi_audio,
				     (AGS_SOUND_BEHAVIOUR_BULK_MODE));
    }
    
    ags_audio_add_recall(audio, (GObject *) play_dssi_audio, FALSE);
    recall = g_list_prepend(recall,
			    play_dssi_audio);

    g_object_set(recall_container,
		 "recall-audio", play_dssi_audio,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(play_dssi_audio));

    play_dssi_audio_run = (AgsPlayDssiAudioRun *) g_object_new(AGS_TYPE_PLAY_DSSI_AUDIO_RUN,
							       "output-soundcard", output_soundcard,
							       "recall-audio", play_dssi_audio,
							       "recall-container", recall_container,
							       //TODO:JK: add missing dependency "delay-audio"
							       NULL);
    ags_recall_set_flags(play_dssi_audio_run,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(play_dssi_audio_run,
				 (AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(play_dssi_audio_run,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) play_dssi_audio_run, FALSE);
    recall = g_list_prepend(recall,
			    play_dssi_audio_run);

    g_object_set(recall_container,
		 "recall-audio-run", play_dssi_audio_run,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(play_dssi_audio_run));
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_play_lv2(AgsAudio *audio,
				   AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				   gchar *plugin_name,
				   guint start_audio_channel, guint stop_audio_channel,
				   guint start_pad, guint stop_pad,
				   guint create_flags, guint recall_flags)
{
  AgsPlayLv2Audio *play_lv2_audio;
  AgsPlayLv2AudioRun *play_lv2_audio_run;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint i, j;

  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;
  
  pthread_mutex_unlock(audio_mutex);

  /* list */
  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if(play_container == NULL){
      play_container = ags_recall_container_new();
    }

    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) play_container);

    play_lv2_audio = (AgsPlayLv2Audio *) g_object_new(AGS_TYPE_PLAY_LV2_AUDIO,
						      "output-soundcard", output_soundcard,
						      "audio", audio,
						      "recall-container", play_container,
						      NULL);
    ags_recall_set_flags(play_lv2_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(play_lv2_audio,
				 (AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(play_lv2_audio,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) play_lv2_audio, TRUE);
    recall = g_list_prepend(recall,
			    play_lv2_audio);

    g_object_set(play_container,
		 "recall-audio", play_lv2_audio,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(play_lv2_audio));

    play_lv2_audio_run = (AgsPlayLv2AudioRun *) g_object_new(AGS_TYPE_PLAY_LV2_AUDIO_RUN,
							     "output-soundcard", output_soundcard,
							     "recall-audio", play_lv2_audio,
							     "recall-container", play_container,
							     NULL);
    ags_recall_set_flags(play_lv2_audio_run,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(play_lv2_audio_run,
				 (AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(play_lv2_audio_run,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    if((AGS_RECALL_FACTORY_BULK & create_flags) != 0){
      ags_recall_set_behaviour_flags(play_lv2_audio,
				     (AGS_SOUND_BEHAVIOUR_BULK_MODE));
    }

    ags_audio_add_recall(audio, (GObject *) play_lv2_audio_run, TRUE);
    recall = g_list_prepend(recall,
			    play_lv2_audio_run);

    g_object_set(play_container,
		 "recall-audio-run", play_lv2_audio_run,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(play_lv2_audio_run));
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    if(recall_container == NULL){
      recall_container = ags_recall_container_new();
    }

    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    play_lv2_audio = (AgsPlayLv2Audio *) g_object_new(AGS_TYPE_PLAY_LV2_AUDIO,
						      "output-soundcard", output_soundcard,
						      "audio", audio,
						      "recall-container", recall_container,
						      NULL);
    ags_recall_set_flags(play_lv2_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(play_lv2_audio,
				 (AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(play_lv2_audio,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));
    
    if((AGS_RECALL_FACTORY_BULK & create_flags) != 0){
      ags_recall_set_behaviour_flags(play_lv2_audio,
				     (AGS_SOUND_BEHAVIOUR_BULK_MODE));
    }

    ags_audio_add_recall(audio, (GObject *) play_lv2_audio, FALSE);
    recall = g_list_prepend(recall,
			    play_lv2_audio);

    g_object_set(recall_container,
		 "recall-audio", play_lv2_audio,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(play_lv2_audio));

    play_lv2_audio_run = (AgsPlayLv2AudioRun *) g_object_new(AGS_TYPE_PLAY_LV2_AUDIO_RUN,
							     "output-soundcard", output_soundcard,
							     "recall-audio", play_lv2_audio,
							     "recall-container", recall_container,
							     //TODO:JK: add missing dependency "delay-audio"
							     NULL);
    ags_recall_set_flags(play_lv2_audio_run,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(play_lv2_audio_run,
				 (AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(play_lv2_audio_run,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) play_lv2_audio_run, FALSE);
    recall = g_list_prepend(recall,
			    play_lv2_audio_run);

    g_object_set(recall_container,
		 "recall-audio-run", play_lv2_audio_run,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(play_lv2_audio_run));
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_play_notation(AgsAudio *audio,
					AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
					gchar *plugin_name,
					guint start_audio_channel, guint stop_audio_channel,
					guint start_pad, guint stop_pad,
					guint create_flags, guint recall_flags)
{
  AgsPlayNotationAudio *play_notation_audio;
  AgsPlayNotationAudioRun *play_notation_audio_run;
  AgsPort *port;

  GObject *output_soundcard;
  
  GList *list_start, *list;
  GList *recall;

  guint i, j;

  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;
  
  pthread_mutex_unlock(audio_mutex);

  /* list */
  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if(play_container == NULL){
      play_container = ags_recall_container_new();
    }

    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) play_container);

    play_notation_audio = (AgsPlayNotationAudio *) g_object_new(AGS_TYPE_PLAY_NOTATION_AUDIO,
								"output-soundcard", output_soundcard,
								"audio", audio,
								"recall-container", play_container,
								NULL);
    ags_recall_set_flags(play_notation_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(play_notation_audio,
				 (AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(play_notation_audio,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) play_notation_audio, TRUE);
    recall = g_list_prepend(recall,
			    play_notation_audio);

    g_object_set(play_container,
		 "recall-audio", play_notation_audio,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(play_notation_audio));

    play_notation_audio_run = (AgsPlayNotationAudioRun *) g_object_new(AGS_TYPE_PLAY_NOTATION_AUDIO_RUN,
								       "output-soundcard", output_soundcard,
								       "recall-audio", play_notation_audio,
								       "recall-container", play_container,
								       NULL);
    ags_recall_set_flags(play_notation_audio_run,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(play_notation_audio_run,
				 (AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(play_notation_audio_run,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) play_notation_audio_run, TRUE);
    recall = g_list_prepend(recall,
			    play_notation_audio_run);

    g_object_set(play_container,
		 "recall-audio-run", play_notation_audio_run,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(play_notation_audio_run));
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    if(recall_container == NULL){
      recall_container = ags_recall_container_new();
    }

    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    play_notation_audio = (AgsPlayNotationAudio *) g_object_new(AGS_TYPE_PLAY_NOTATION_AUDIO,
								"output-soundcard", output_soundcard,
								"audio", audio,
								"recall-container", recall_container,
								NULL);
    ags_recall_set_flags(play_notation_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(play_notation_audio,
				 (AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(play_notation_audio,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) play_notation_audio, FALSE);
    recall = g_list_prepend(recall,
			    play_notation_audio);

    g_object_set(recall_container,
		 "recall-audio", play_notation_audio,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(play_notation_audio));

    play_notation_audio_run = (AgsPlayNotationAudioRun *) g_object_new(AGS_TYPE_PLAY_NOTATION_AUDIO_RUN,
								       "output-soundcard", output_soundcard,
								       "recall-audio", play_notation_audio,
								       "recall-container", recall_container,
								       //TODO:JK: add missing dependency "delay-audio"
								       NULL);
    ags_recall_set_flags(play_notation_audio_run,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(play_notation_audio_run,
				 (AGS_SOUND_ABILITY_NOTATION |
				  AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(play_notation_audio_run,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) play_notation_audio_run, FALSE);
    recall = g_list_prepend(recall,
			    play_notation_audio_run);

    g_object_set(recall_container,
		 "recall-audio-run", play_notation_audio_run,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(play_notation_audio_run));
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_peak(AgsAudio *audio,
			       AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			       gchar *plugin_name,
			       guint start_audio_channel, guint stop_audio_channel,
			       guint start_pad, guint stop_pad,
			       guint create_flags, guint recall_flags)
{
  AgsPeakChannel *peak_channel;
  AgsPeakChannelRun *peak_channel_run;
  AgsChannel *output, *input;
  AgsChannel *start, *channel;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint audio_channels;
  guint i, j;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;

  output = audio->output;
  input = audio->input;
  
  audio_channels = audio->audio_channels;
  
  pthread_mutex_unlock(audio_mutex);

  /* get channel */
  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(output,
				start_pad * audio_channels);
  }else{
    start =
      channel = ags_channel_nth(input,
				start_pad * audio_channels);
  }

  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if(play_container == NULL){
      play_container = ags_recall_container_new();
    }

    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) play_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) play_container);

	/* AgsPeakChannel */
	peak_channel = (AgsPeakChannel *) g_object_new(AGS_TYPE_PEAK_CHANNEL,
						       "output-soundcard", output_soundcard,
						       "source", channel,
						       "recall-container", play_container,
						       NULL);
	ags_recall_set_flags(peak_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(peak_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(peak_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) peak_channel, TRUE);
	recall = g_list_prepend(recall,
				peak_channel);

	g_object_set(play_container,
		     "recall-channel", peak_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(peak_channel));

	/* AgsPeakChannelRun */
	peak_channel_run = (AgsPeakChannelRun *) g_object_new(AGS_TYPE_PEAK_CHANNEL_RUN,
							      "output-soundcard", output_soundcard,
							      "recall-channel", peak_channel,
							      "source", channel,
							      "recall-container", play_container,
							      NULL);
	ags_recall_set_flags(peak_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(peak_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(peak_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) peak_channel_run, TRUE);
	recall = g_list_prepend(recall,
				peak_channel_run);

	g_object_set(play_container,
		     "recall-channel-run", peak_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(peak_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    channel = start;

    if(recall_container == NULL){
      recall_container = ags_recall_container_new();
    }

    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) recall_container);

	/* AgsPeakChannel */
	peak_channel = (AgsPeakChannel *) g_object_new(AGS_TYPE_PEAK_CHANNEL,
						       "output-soundcard", output_soundcard,
						       "source", channel,
						       "recall-container", recall_container,
						       NULL);
	ags_recall_set_flags(peak_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(peak_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(peak_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) peak_channel, FALSE);
	recall = g_list_prepend(recall,
				peak_channel);

	g_object_set(recall_container,
		     "recall-channel", peak_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(peak_channel));

	/* AgsPeakChannelRun */
	peak_channel_run = (AgsPeakChannelRun *) g_object_new(AGS_TYPE_PEAK_CHANNEL_RUN,
							      "output-soundcard", output_soundcard,
							      "recall-channel", peak_channel,
							      "source", channel,
							      "recall-container", recall_container,
							      NULL);
	ags_recall_set_flags(peak_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(peak_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(peak_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) peak_channel_run, FALSE);
	recall = g_list_prepend(recall,
				peak_channel_run);

	g_object_set(recall_container,
		     "recall-channel-run", peak_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(peak_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_mute(AgsAudio *audio,
			       AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			       gchar *plugin_name,
			       guint start_audio_channel, guint stop_audio_channel,
			       guint start_pad, guint stop_pad,
			       guint create_flags, guint recall_flags)
{
  AgsMuteAudio *mute_audio;
  AgsMuteAudioRun *mute_audio_run;
  AgsMuteChannel *mute_channel;
  AgsMuteChannelRun *mute_channel_run;
  AgsChannel *output, *input;
  AgsChannel *start, *channel;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint audio_channels;
  guint i, j;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;

  output = audio->output;
  input = audio->input;
  
  audio_channels = audio->audio_channels;
  
  pthread_mutex_unlock(audio_mutex);

  /* get channel */
  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(output,
				start_pad * audio_channels);
  }else{
    start =
      channel = ags_channel_nth(input,
				start_pad * audio_channels);
  }

  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if((AGS_RECALL_FACTORY_REMAP & (create_flags)) == 0){
      if(play_container == NULL){
	play_container = ags_recall_container_new();
      }

      play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
      ags_audio_add_recall_container(audio, (GObject *) play_container);

      /* AgsMuteAudio */
      mute_audio = (AgsMuteAudio *) g_object_new(AGS_TYPE_MUTE_AUDIO,
						 "output-soundcard", output_soundcard,
						 "audio", audio,
						 "recall-container", play_container,
						 NULL);
      ags_recall_set_flags(mute_audio,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags(mute_audio,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_MIDI |
				    AGS_SOUND_ABILITY_WAVE));
      ags_recall_set_behaviour_flags(mute_audio,
				     (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_audio_add_recall(audio, (GObject *) mute_audio, TRUE);
      recall = g_list_prepend(recall,
			      mute_audio);

      g_object_set(play_container,
		   "recall-audio", mute_audio,
		   NULL);

      /* AgsMuteAudioRun */
      mute_audio_run = (AgsMuteAudioRun *) g_object_new(AGS_TYPE_MUTE_AUDIO_RUN,
							"output-soundcard", output_soundcard,
							// "recall-audio", mute_audio,
							"recall-container", play_container,
							//TODO:JK: add missing dependency "count_beats_audio_run"
							NULL);
      ags_recall_set_flags(mute_audio_run,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags(mute_audio_run,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_MIDI |
				    AGS_SOUND_ABILITY_WAVE));
      ags_recall_set_behaviour_flags(mute_audio_run,
				     (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_audio_add_recall(audio, (GObject *) mute_audio_run, TRUE);
      recall = g_list_prepend(recall,
			      mute_audio_run);

      g_object_set(play_container,
		   "recall-audio-run", mute_audio_run,
		   NULL);
    }else{
      if(play_container == NULL){
	g_object_get(audio,
		     "play", &list_start,
		     NULL);

	list = ags_recall_find_type(list_start,
				    AGS_TYPE_MUTE_AUDIO);
	mute_audio = AGS_MUTE_AUDIO(list->data);
	g_list_free(list_start);
	
	recall = g_list_prepend(recall,
				mute_audio);

	g_object_get(mute_audio,
		     "recall-container", &play_container,
		     NULL);
      }else{
	g_object_get(play_container,
		     "recall-audio", &mute_audio,
		     NULL);
	recall = g_list_prepend(recall,
				mute_audio);
      }
    }    
  }
  
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if(play_container == NULL){
      play_container = ags_recall_container_new();
    }

    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) play_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) play_container);

	/* AgsMuteChannel */
	mute_channel = (AgsMuteChannel *) g_object_new(AGS_TYPE_MUTE_CHANNEL,
						       "output-soundcard", output_soundcard,
						       "source", channel,
						       "recall-container", play_container,
						       NULL);
	ags_recall_set_flags(mute_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(mute_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(mute_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) mute_channel, TRUE);
	recall = g_list_prepend(recall,
				mute_channel);

	g_object_set(play_container,
		     "recall-channel", mute_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(mute_channel));

	/* AgsMuteChannelRun */
	mute_channel_run = (AgsMuteChannelRun *) g_object_new(AGS_TYPE_MUTE_CHANNEL_RUN,
							      "output-soundcard", output_soundcard,
							      "recall-channel", mute_channel,
							      "source", channel,
							      "recall-container", play_container,
							      NULL);
	ags_recall_set_flags(mute_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(mute_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(mute_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) mute_channel_run, TRUE);
	recall = g_list_prepend(recall,
				mute_channel_run);

	g_object_set(play_container,
		     "recall-channel-run", mute_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(mute_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    channel = start;
 
    if((AGS_RECALL_FACTORY_REMAP & (create_flags)) == 0){
      if(recall_container == NULL){
	recall_container = ags_recall_container_new();
      }

      ags_audio_add_recall_container(audio, (GObject *) recall_container);

      /* AgsMuteAudio */
      mute_audio = (AgsMuteAudio *) g_object_new(AGS_TYPE_MUTE_AUDIO,
						 "output-soundcard", output_soundcard,
						 "audio", audio,
						 "recall-container", recall_container,
						 NULL);
      ags_recall_set_flags(mute_audio,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags(mute_audio,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_MIDI |
				    AGS_SOUND_ABILITY_WAVE));
      ags_recall_set_behaviour_flags(mute_audio,
				     (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_audio_add_recall(audio, (GObject *) mute_audio, FALSE);
      recall = g_list_prepend(recall,
			      mute_audio);

      g_object_set(recall_container,
		   "recall-audio", mute_audio,
		   NULL);

      /* AgsMuteAudioRun */
      mute_audio_run = (AgsMuteAudioRun *) g_object_new(AGS_TYPE_MUTE_AUDIO_RUN,
							"output-soundcard", output_soundcard,
							// "recall-audio", mute_audio,
							"recall-container", recall_container,
							//TODO:JK: add missing dependency "count_beats_audio_run"
							NULL);
      ags_recall_set_flags(mute_audio_run,
			   (AGS_RECALL_TEMPLATE));
      ags_recall_set_ability_flags(mute_audio_run,
				   (AGS_SOUND_ABILITY_PLAYBACK |
				    AGS_SOUND_ABILITY_NOTATION |
				    AGS_SOUND_ABILITY_SEQUENCER |
				    AGS_SOUND_ABILITY_MIDI |
				    AGS_SOUND_ABILITY_WAVE));
      ags_recall_set_behaviour_flags(mute_audio_run,
				     (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

      ags_audio_add_recall(audio, (GObject *) mute_audio_run, FALSE);
      recall = g_list_prepend(recall,
			      mute_audio_run);

      g_object_set(recall_container,
		   "recall-audio-run", mute_audio_run,
		   NULL);
    }else{
      if(recall_container == NULL){
	g_object_get(audio,
		     "recall", &list_start,
		     NULL);

	list = ags_recall_find_type(list_start,
				    AGS_TYPE_MUTE_AUDIO);
	mute_audio = AGS_MUTE_AUDIO(list->data);
	g_list_free(list_start);
	
	recall = g_list_prepend(recall,
				mute_audio);

	g_object_get(mute_audio,
		     "recall-container", &recall_container,
		     NULL);
      }else{
	g_object_get(recall_container,
		     "recall-audio", &mute_audio,
		     NULL);
	recall = g_list_prepend(recall,
				mute_audio);
      }
    }

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) recall_container);

	/* AgsMuteChannel */
	mute_channel = (AgsMuteChannel *) g_object_new(AGS_TYPE_MUTE_CHANNEL,
						       "output-soundcard", output_soundcard,
						       "source", channel,
						       "recall-container", recall_container,
						       NULL);
	ags_recall_set_flags(mute_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(mute_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(mute_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) mute_channel, FALSE);
	recall = g_list_prepend(recall,
				mute_channel);

	g_object_set(recall_container,
		     "recall-channel", mute_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(mute_channel));

	/* AgsMuteChannelRun */
	mute_channel_run = (AgsMuteChannelRun *) g_object_new(AGS_TYPE_MUTE_CHANNEL_RUN,
							      "output-soundcard", output_soundcard,
							      "recall-channel", mute_channel,
							      "source", channel,
							      "recall-container", recall_container,
							      NULL);
	ags_recall_set_flags(mute_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(mute_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(mute_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) mute_channel_run, FALSE);
	recall = g_list_prepend(recall,
				mute_channel_run);

	g_object_set(recall_container,
		     "recall-channel-run", mute_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(mute_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_volume(AgsAudio *audio,
				 AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				 gchar *plugin_name,
				 guint start_audio_channel, guint stop_audio_channel,
				 guint start_pad, guint stop_pad,
				 guint create_flags, guint recall_flags)
{
  AgsVolumeChannel *volume_channel;
  AgsVolumeChannelRun *volume_channel_run;
  AgsChannel *output, *input;
  AgsChannel *start, *channel;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint audio_channels;
  guint i, j;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;

  output = audio->output;
  input = audio->input;
  
  audio_channels = audio->audio_channels;
  
  pthread_mutex_unlock(audio_mutex);

  /* get channel */
  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(output,
				start_pad * audio_channels);
  }else{
    start =
      channel = ags_channel_nth(input,
				start_pad * audio_channels);
  }

  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if(play_container == NULL){
      play_container = ags_recall_container_new();
    }

    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) play_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) play_container);

	/* AgsVolumeChannel */
	volume_channel = (AgsVolumeChannel *) g_object_new(AGS_TYPE_VOLUME_CHANNEL,
							   "output-soundcard", output_soundcard,
							   "source", channel,
							   "recall-container", play_container,
							   NULL);
	ags_recall_set_flags(volume_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(volume_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(volume_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) volume_channel, TRUE);
	recall = g_list_prepend(recall,
				volume_channel);

	g_object_set(play_container,
		     "recall-channel", volume_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(volume_channel));

	/* AgsVolumeChannelRun */
	volume_channel_run = (AgsVolumeChannelRun *) g_object_new(AGS_TYPE_VOLUME_CHANNEL_RUN,
								  "output-soundcard", output_soundcard,
								  "recall-channel", volume_channel,
								  "source", channel,
								  "recall-container", play_container,
								  NULL);
	ags_recall_set_flags(volume_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(volume_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(volume_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) volume_channel_run, TRUE);
	recall = g_list_prepend(recall,
				volume_channel_run);

	g_object_set(play_container,
		     "recall-channel-run", volume_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(volume_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    channel = start;

    if(recall_container == NULL){
      recall_container = ags_recall_container_new();
    }

    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) recall_container);

	/* AgsVolumeChannel */
	volume_channel = (AgsVolumeChannel *) g_object_new(AGS_TYPE_VOLUME_CHANNEL,
							   "output-soundcard", output_soundcard,
							   "source", channel,
							   "recall-container", recall_container,
							   NULL);
	ags_recall_set_flags(volume_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(volume_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(volume_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) volume_channel, FALSE);
	recall = g_list_prepend(recall,
				volume_channel);

	g_object_set(recall_container,
		     "recall-channel", volume_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(volume_channel));

	/* AgsVolumeChannelRun */
	volume_channel_run = (AgsVolumeChannelRun *) g_object_new(AGS_TYPE_VOLUME_CHANNEL_RUN,
								  "output-soundcard", output_soundcard,
								  "recall-channel", volume_channel,
								  "source", channel,
								  "recall-container", recall_container,
								  NULL);
	ags_recall_set_flags(volume_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(volume_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(volume_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) volume_channel_run, FALSE);
	recall = g_list_prepend(recall,
				volume_channel_run);

	g_object_set(recall_container,
		     "recall-channel-run", volume_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(volume_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_envelope(AgsAudio *audio,
				   AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				   gchar *plugin_name,
				   guint start_audio_channel, guint stop_audio_channel,
				   guint start_pad, guint stop_pad,
				   guint create_flags, guint recall_flags)
{
  AgsEnvelopeChannel *envelope_channel;
  AgsEnvelopeChannelRun *envelope_channel_run;
  AgsChannel *output, *input;
  AgsChannel *start, *channel;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint audio_channels;
  guint i, j;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;

  output = audio->output;
  input = audio->input;
  
  audio_channels = audio->audio_channels;
  
  pthread_mutex_unlock(audio_mutex);

  /* get channel */
  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(output,
				start_pad * audio_channels);
  }else{
    start =
      channel = ags_channel_nth(input,
				start_pad * audio_channels);
  }

  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if(play_container == NULL){
      play_container = ags_recall_container_new();
    }

    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) play_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) play_container);

	/* AgsEnvelopeChannel */
	envelope_channel = (AgsEnvelopeChannel *) g_object_new(AGS_TYPE_ENVELOPE_CHANNEL,
							       "output-soundcard", output_soundcard,
							       "source", channel,
							       "recall-container", play_container,
							       NULL);
	ags_recall_set_flags(envelope_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(envelope_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(envelope_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) envelope_channel, TRUE);
	recall = g_list_prepend(recall,
				envelope_channel);

	g_object_set(play_container,
		     "recall-channel", envelope_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(envelope_channel));

	/* AgsEnvelopeChannelRun */
	envelope_channel_run = (AgsEnvelopeChannelRun *) g_object_new(AGS_TYPE_ENVELOPE_CHANNEL_RUN,
								      "output-soundcard", output_soundcard,
								      "recall-channel", envelope_channel,
								      "source", channel,
								      "recall-container", play_container,
								      NULL);
	ags_recall_set_flags(envelope_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(envelope_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(envelope_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) envelope_channel_run, TRUE);
	recall = g_list_prepend(recall,
				envelope_channel_run);

	g_object_set(play_container,
		     "recall-channel-run", envelope_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(envelope_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    channel = start;

    if(recall_container == NULL){
      recall_container = ags_recall_container_new();
    }

    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) recall_container);

	/* AgsEnvelopeChannel */
	envelope_channel = (AgsEnvelopeChannel *) g_object_new(AGS_TYPE_ENVELOPE_CHANNEL,
							       "output-soundcard", output_soundcard,
							       "source", channel,
							       "recall-container", recall_container,
							       NULL);
	ags_recall_set_flags(envelope_channel,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(envelope_channel,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(envelope_channel,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) envelope_channel, FALSE);
	recall = g_list_prepend(recall,
				envelope_channel);

	g_object_set(recall_container,
		     "recall-channel", envelope_channel,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(envelope_channel));

	/* AgsEnvelopeChannelRun */
	envelope_channel_run = (AgsEnvelopeChannelRun *) g_object_new(AGS_TYPE_ENVELOPE_CHANNEL_RUN,
								      "output-soundcard", output_soundcard,
								      "recall-channel", envelope_channel,
								      "source", channel,
								      "recall-container", recall_container,
								      NULL);
	ags_recall_set_flags(envelope_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(envelope_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI |
				      AGS_SOUND_ABILITY_WAVE));
	ags_recall_set_behaviour_flags(envelope_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) envelope_channel_run, FALSE);
	recall = g_list_prepend(recall,
				envelope_channel_run);

	g_object_set(recall_container,
		     "recall-channel-run", envelope_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(envelope_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_ladspa(AgsAudio *audio,
				 AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				 gchar *plugin_name,
				 guint start_audio_channel, guint stop_audio_channel,
				 guint start_pad, guint stop_pad,
				 guint create_flags, guint recall_flags)
{
  AgsRecallLadspa *recall_ladspa;
  AgsGenericRecallChannelRun *generic_recall_channel_run;
  AgsChannel *output, *input;
  AgsChannel *start, *channel;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint audio_channels;
  guint i, j;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;

  output = audio->output;
  input = audio->input;
  
  audio_channels = audio->audio_channels;
  
  pthread_mutex_unlock(audio_mutex);

  /* get channel */
  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(output,
				start_pad * audio_channels);
  }else{
    start =
      channel = ags_channel_nth(input,
				start_pad * audio_channels);
  }

  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if(play_container == NULL){
      play_container = ags_recall_container_new();
    }

    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) play_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) play_container);

	/* AgsRecallLadspa */
	recall_ladspa = (AgsRecallLadspa *) g_object_new(AGS_TYPE_RECALL_LADSPA,
							 "output-soundcard", output_soundcard,
							 "source", channel,
							 "recall-container", play_container,
							 NULL);
	ags_recall_set_flags(recall_ladspa,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(recall_ladspa,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI));
	ags_recall_set_behaviour_flags(recall_ladspa,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) recall_ladspa, TRUE);
	recall = g_list_prepend(recall,
				recall_ladspa);

	g_object_set(play_container,
		     "recall-channel", recall_ladspa,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(recall_ladspa));

	/* AgsGenericRecallChannelRun */
	generic_recall_channel_run = ags_generic_recall_channel_run_new(channel,
									AGS_TYPE_GENERIC_RECALL_RECYCLING,
									AGS_TYPE_RECALL_LADSPA_RUN);

	g_object_set(generic_recall_channel_run,
		     "output-soundcard", output_soundcard,
		     // "recall-channel", recall_ladspa,
		     "source", channel,
		     "recall-container", play_container,
		     NULL);
	ags_recall_set_flags(generic_recall_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(generic_recall_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI));
	ags_recall_set_behaviour_flags(generic_recall_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) generic_recall_channel_run, TRUE);
	recall = g_list_prepend(recall,
				generic_recall_channel_run);

	g_object_set(play_container,
		     "recall-channel-run", generic_recall_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(generic_recall_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    channel = start;

    if(recall_container == NULL){
      recall_container = ags_recall_container_new();
    }

    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) recall_container);

	/* AgsRecallLadspa */
	recall_ladspa = (AgsRecallLadspa *) g_object_new(AGS_TYPE_RECALL_LADSPA,
							 "output-soundcard", output_soundcard,
							 "source", channel,
							 "recall-container", recall_container,
							 NULL);
	ags_recall_set_flags(recall_ladspa,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(recall_ladspa,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI));
	ags_recall_set_behaviour_flags(recall_ladspa,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) recall_ladspa, FALSE);
	recall = g_list_prepend(recall,
				recall_ladspa);

	g_object_set(recall_container,
		     "recall-channel-run", recall_ladspa,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(recall_ladspa));

	/* AgsGenericRecallChannelRun */
	generic_recall_channel_run = ags_generic_recall_channel_run_new(channel,
									AGS_TYPE_GENERIC_RECALL_RECYCLING,
									AGS_TYPE_RECALL_LADSPA_RUN);
	g_object_set(generic_recall_channel_run,
		     "output-soundcard", output_soundcard,
		     // "recall-channel", recall_ladspa,
		     "source", channel,
		     "recall-container", recall_container,
		     NULL);
	ags_recall_set_flags(generic_recall_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(generic_recall_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI));
	ags_recall_set_behaviour_flags(generic_recall_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) generic_recall_channel_run, FALSE);
	recall = g_list_prepend(recall,
				generic_recall_channel_run);

	g_object_set(recall_container,
		     "recall-channel-run", generic_recall_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(generic_recall_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_lv2(AgsAudio *audio,
			      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			      gchar *plugin_name,
			      guint start_audio_channel, guint stop_audio_channel,
			      guint start_pad, guint stop_pad,
			      guint create_flags, guint recall_flags)
{
  AgsRecallLv2 *recall_lv2;
  AgsGenericRecallChannelRun *generic_recall_channel_run;
  AgsChannel *output, *input;
  AgsChannel *start, *channel;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint audio_channels;
  guint i, j;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;

  output = audio->output;
  input = audio->input;
  
  audio_channels = audio->audio_channels;
  
  pthread_mutex_unlock(audio_mutex);

  /* get channel */
  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(output,
				start_pad * audio_channels);
  }else{
    start =
      channel = ags_channel_nth(input,
				start_pad * audio_channels);
  }

  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if(play_container == NULL){
      play_container = ags_recall_container_new();
    }

    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) play_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) play_container);

	/* AgsRecallLv2 */
	recall_lv2 = (AgsRecallLv2 *) g_object_new(AGS_TYPE_RECALL_LV2,
						   "output-soundcard", output_soundcard,
						   "source", channel,
						   "recall-container", play_container,
						   NULL);
	ags_recall_set_flags(recall_lv2,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(recall_lv2,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI));
	ags_recall_set_behaviour_flags(recall_lv2,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) recall_lv2, TRUE);
	recall = g_list_prepend(recall,
				recall_lv2);

	g_object_set(play_container,
		     "recall-channel", recall_lv2,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(recall_lv2));

	/* AgsGenericRecallChannelRun */
	generic_recall_channel_run = ags_generic_recall_channel_run_new(channel,
									AGS_TYPE_GENERIC_RECALL_RECYCLING,
									AGS_TYPE_RECALL_LV2_RUN);

	g_object_set(generic_recall_channel_run,
		     "output-soundcard", output_soundcard,
		     // "recall-channel", recall_lv2,
		     "source", channel,
		     "recall-container", play_container,
		     NULL);
	ags_recall_set_flags(generic_recall_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(generic_recall_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI));
	ags_recall_set_behaviour_flags(generic_recall_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) generic_recall_channel_run, TRUE);
	recall = g_list_prepend(recall,
				generic_recall_channel_run);

	g_object_set(play_container,
		     "recall-channel-run", generic_recall_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(generic_recall_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    channel = start;

    if(recall_container == NULL){
      recall_container = ags_recall_container_new();
    }

    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) recall_container);

	/* AgsRecallLv2 */
	recall_lv2 = (AgsRecallLv2 *) g_object_new(AGS_TYPE_RECALL_LV2,
						   "output-soundcard", output_soundcard,
						   "source", channel,
						   "recall-container", recall_container,
						   NULL);
	ags_recall_set_flags(recall_lv2,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(recall_lv2,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI));
	ags_recall_set_behaviour_flags(recall_lv2,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) recall_lv2, FALSE);
	recall = g_list_prepend(recall,
				recall_lv2);

	g_object_set(recall_container,
		     "recall-channel", recall_lv2,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(recall_lv2));

	/* AgsGenericRecallChannelRun */
	generic_recall_channel_run = ags_generic_recall_channel_run_new(channel,
									AGS_TYPE_GENERIC_RECALL_RECYCLING,
									AGS_TYPE_RECALL_LV2_RUN);
	g_object_set(generic_recall_channel_run,
		     "output-soundcard", output_soundcard,
		     // "recall-channel", recall_lv2,
		     "source", channel,
		     "recall-container", recall_container,
		     NULL);
	ags_recall_set_flags(generic_recall_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(generic_recall_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI));
	ags_recall_set_behaviour_flags(generic_recall_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) generic_recall_channel_run, FALSE);
	recall = g_list_prepend(recall,
				generic_recall_channel_run);

	g_object_set(recall_container,
		     "recall-channel-run", generic_recall_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(generic_recall_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_dssi(AgsAudio *audio,
			       AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			       gchar *plugin_name,
			       guint start_audio_channel, guint stop_audio_channel,
			       guint start_pad, guint stop_pad,
			       guint create_flags, guint recall_flags)
{
  AgsRecallDssi *recall_dssi;
  AgsGenericRecallChannelRun *generic_recall_channel_run;
  AgsChannel *output, *input;
  AgsChannel *start, *channel;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint audio_channels;
  guint i, j;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;

  output = audio->output;
  input = audio->input;
  
  audio_channels = audio->audio_channels;
  
  pthread_mutex_unlock(audio_mutex);

  /* get channel */
  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(output,
				start_pad * audio_channels);
  }else{
    start =
      channel = ags_channel_nth(input,
				start_pad * audio_channels);
  }

  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if(play_container == NULL){
      play_container = ags_recall_container_new();
    }

    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) play_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) play_container);

	/* AgsRecallDssi */
	recall_dssi = (AgsRecallDssi *) g_object_new(AGS_TYPE_RECALL_DSSI,
						     "output-soundcard", output_soundcard,
						     "source", channel,
						     "recall-container", play_container,
						     NULL);
	ags_recall_set_flags(recall_dssi,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(recall_dssi,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI));
	ags_recall_set_behaviour_flags(recall_dssi,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) recall_dssi, TRUE);
	recall = g_list_prepend(recall,
				recall_dssi);

	g_object_set(play_container,
		     "recall-channel", recall_dssi,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(recall_dssi));

	/* AgsGenericRecallChannelRun */
	generic_recall_channel_run = ags_generic_recall_channel_run_new(channel,
									AGS_TYPE_GENERIC_RECALL_RECYCLING,
									AGS_TYPE_RECALL_DSSI_RUN);

	g_object_set(generic_recall_channel_run,
		     "output-soundcard", output_soundcard,
		     // "recall-channel", recall_dssi,
		     "source", channel,
		     "recall-container", play_container,
		     NULL);
	ags_recall_set_flags(generic_recall_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(generic_recall_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI));
	ags_recall_set_behaviour_flags(generic_recall_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) generic_recall_channel_run, TRUE);
	recall = g_list_prepend(recall,
				generic_recall_channel_run);

	g_object_set(play_container,
		     "recall-channel-run", generic_recall_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(generic_recall_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    channel = start;

    if(recall_container == NULL){
      recall_container = ags_recall_container_new();
    }

    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* add recall container */
	ags_channel_add_recall_container(channel,
					 (GObject *) recall_container);

	/* AgsRecallDssi */
	recall_dssi = (AgsRecallDssi *) g_object_new(AGS_TYPE_RECALL_DSSI,
						     "output-soundcard", output_soundcard,
						     "source", channel,
						     "recall-container", recall_container,
						     NULL);
	ags_recall_set_flags(recall_dssi,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(recall_dssi,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI));
	ags_recall_set_behaviour_flags(recall_dssi,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) recall_dssi, FALSE);
	recall = g_list_prepend(recall,
				recall_dssi);

	g_object_set(recall_container,
		     "recall-channel", recall_dssi,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(recall_dssi));

	/* AgsGenericRecallChannelRun */
	generic_recall_channel_run = ags_generic_recall_channel_run_new(channel,
									AGS_TYPE_GENERIC_RECALL_RECYCLING,
									AGS_TYPE_RECALL_DSSI_RUN);
	g_object_set(generic_recall_channel_run,
		     "output-soundcard", output_soundcard,
		     // "recall-channel", recall_dssi,
		     "source", channel,
		     "recall-container", recall_container,
		     NULL);
	ags_recall_set_flags(generic_recall_channel_run,
			     (AGS_RECALL_TEMPLATE));
	ags_recall_set_ability_flags(generic_recall_channel_run,
				     (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_MIDI));
	ags_recall_set_behaviour_flags(generic_recall_channel_run,
				       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

	ags_channel_add_recall(channel, (GObject *) generic_recall_channel_run, FALSE);
	recall = g_list_prepend(recall,
				generic_recall_channel_run);

	g_object_set(recall_container,
		     "recall-channel-run", generic_recall_channel_run,
		     NULL);
	ags_connectable_connect(AGS_CONNECTABLE(generic_recall_channel_run));

	/* iterate */
	pthread_mutex_lock(channel_mutex);
	
	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
      }

      channel = ags_channel_nth(channel,
				audio_channels - stop_audio_channel);
    }
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_record_midi(AgsAudio *audio,
				      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				      gchar *plugin_name,
				      guint start_audio_channel, guint stop_audio_channel,
				      guint start_pad, guint stop_pad,
				      guint create_flags, guint recall_flags)
{
  AgsRecordMidiAudio *record_midi_audio;
  AgsRecordMidiAudioRun *record_midi_audio_run;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint i, j;

  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;
  
  pthread_mutex_unlock(audio_mutex);

  /* list */
  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if(play_container == NULL){
      play_container = ags_recall_container_new();
    }

    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) play_container);

    record_midi_audio = (AgsRecordMidiAudio *) g_object_new(AGS_TYPE_RECORD_MIDI_AUDIO,
							    "output-soundcard", output_soundcard,
							    "audio", audio,
							    "recall-container", play_container,
							    NULL);
    ags_recall_set_flags(record_midi_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(record_midi_audio,
				 (AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(record_midi_audio,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) record_midi_audio, TRUE);
    recall = g_list_prepend(recall,
			    record_midi_audio);

    g_object_set(play_container,
		 "recall-audio", record_midi_audio,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(record_midi_audio));

    record_midi_audio_run = (AgsRecordMidiAudioRun *) g_object_new(AGS_TYPE_RECORD_MIDI_AUDIO_RUN,
								   "output-soundcard", output_soundcard,
								   "recall-audio", record_midi_audio,
								   "recall-container", play_container,
								   NULL);
    ags_recall_set_flags(record_midi_audio_run,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(record_midi_audio_run,
				 (AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(record_midi_audio_run,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) record_midi_audio_run, TRUE);
    recall = g_list_prepend(recall,
			    record_midi_audio_run);

    g_object_set(play_container,
		 "recall-audio-run", record_midi_audio_run,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(record_midi_audio_run));
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    if(recall_container == NULL){
      recall_container = ags_recall_container_new();
    }

    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    record_midi_audio = (AgsRecordMidiAudio *) g_object_new(AGS_TYPE_RECORD_MIDI_AUDIO,
							    "output-soundcard", output_soundcard,
							    "audio", audio,
							    "recall-container", recall_container,
							    NULL);
    ags_recall_set_flags(record_midi_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(record_midi_audio,
				 (AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(record_midi_audio,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) record_midi_audio, FALSE);
    recall = g_list_prepend(recall,
			    record_midi_audio);

    g_object_set(recall_container,
		 "recall-audio", record_midi_audio,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(record_midi_audio));

    record_midi_audio_run = (AgsRecordMidiAudioRun *) g_object_new(AGS_TYPE_RECORD_MIDI_AUDIO_RUN,
								   "output-soundcard", output_soundcard,
								   "recall-audio", record_midi_audio,
								   "recall-container", recall_container,
								   //TODO:JK: add missing dependency "delay-audio"
								   NULL);
    ags_recall_set_flags(record_midi_audio_run,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(record_midi_audio_run,
				 (AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(record_midi_audio_run,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) record_midi_audio_run, FALSE);
    recall = g_list_prepend(recall,
			    record_midi_audio_run);

    g_object_set(recall_container,
		 "recall-audio-run", record_midi_audio_run,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(record_midi_audio_run));
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_route_dssi(AgsAudio *audio,
				     AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				     gchar *plugin_name,
				     guint start_audio_channel, guint stop_audio_channel,
				     guint start_pad, guint stop_pad,
				     guint create_flags, guint recall_flags)
{
  AgsRouteDssiAudio *route_dssi_audio;
  AgsRouteDssiAudioRun *route_dssi_audio_run;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint i, j;

  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;
  
  pthread_mutex_unlock(audio_mutex);

  /* list */
  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if(play_container == NULL){
      play_container = ags_recall_container_new();
    }

    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) play_container);

    route_dssi_audio = (AgsRouteDssiAudio *) g_object_new(AGS_TYPE_ROUTE_DSSI_AUDIO,
							  "output-soundcard", output_soundcard,
							  "audio", audio,
							  "recall-container", play_container,
							  NULL);
    ags_recall_set_flags(route_dssi_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(route_dssi_audio,
				 (AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(route_dssi_audio,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) route_dssi_audio, TRUE);
    recall = g_list_prepend(recall,
			    route_dssi_audio);

    g_object_set(play_container,
		 "recall-audio", route_dssi_audio,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(route_dssi_audio));

    route_dssi_audio_run = (AgsRouteDssiAudioRun *) g_object_new(AGS_TYPE_ROUTE_DSSI_AUDIO_RUN,
								 "output-soundcard", output_soundcard,
								 "recall-audio", route_dssi_audio,
								 "recall-container", play_container,
								 NULL);
    ags_recall_set_flags(route_dssi_audio_run,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(route_dssi_audio_run,
				 (AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(route_dssi_audio_run,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) route_dssi_audio_run, TRUE);
    recall = g_list_prepend(recall,
			    route_dssi_audio_run);

    g_object_set(play_container,
		 "recall-audio-run", route_dssi_audio_run,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(route_dssi_audio_run));
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    if(recall_container == NULL){
      recall_container = ags_recall_container_new();
    }

    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    route_dssi_audio = (AgsRouteDssiAudio *) g_object_new(AGS_TYPE_ROUTE_DSSI_AUDIO,
							  "output-soundcard", output_soundcard,
							  "audio", audio,
							  "recall-container", recall_container,
							  NULL);
    ags_recall_set_flags(route_dssi_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(route_dssi_audio,
				 (AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(route_dssi_audio,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) route_dssi_audio, FALSE);
    recall = g_list_prepend(recall,
			    route_dssi_audio);

    g_object_set(recall_container,
		 "recall-audio", route_dssi_audio,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(route_dssi_audio));

    route_dssi_audio_run = (AgsRouteDssiAudioRun *) g_object_new(AGS_TYPE_ROUTE_DSSI_AUDIO_RUN,
								 "output-soundcard", output_soundcard,
								 "recall-audio", route_dssi_audio,
								 "recall-container", recall_container,
								 //TODO:JK: add missing dependency "delay-audio"
								 NULL);
    ags_recall_set_flags(route_dssi_audio_run,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(route_dssi_audio_run,
				 (AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(route_dssi_audio_run,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) route_dssi_audio_run, FALSE);
    recall = g_list_prepend(recall,
			    route_dssi_audio_run);

    g_object_set(recall_container,
		 "recall-audio-run", route_dssi_audio_run,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(route_dssi_audio_run));
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

GList*
ags_recall_factory_create_route_lv2(AgsAudio *audio,
				    AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				    gchar *plugin_name,
				    guint start_audio_channel, guint stop_audio_channel,
				    guint start_pad, guint stop_pad,
				    guint create_flags, guint recall_flags)
{
  AgsRouteLv2Audio *route_lv2_audio;
  AgsRouteLv2AudioRun *route_lv2_audio_run;
  AgsPort *port;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *recall;

  guint i, j;

  pthread_mutex_t *audio_mutex;

  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_mutex);

  output_soundcard = audio->output_soundcard;
  
  pthread_mutex_unlock(audio_mutex);

  /* list */
  recall = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if(play_container == NULL){
      play_container = ags_recall_container_new();
    }

    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) play_container);

    route_lv2_audio = (AgsRouteLv2Audio *) g_object_new(AGS_TYPE_ROUTE_LV2_AUDIO,
							"output-soundcard", output_soundcard,
							"audio", audio,
							"recall-container", play_container,
							NULL);
    ags_recall_set_flags(route_lv2_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(route_lv2_audio,
				 (AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(route_lv2_audio,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) route_lv2_audio, TRUE);
    recall = g_list_prepend(recall,
			    route_lv2_audio);

    g_object_set(play_container,
		 "recall-audio", route_lv2_audio,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(route_lv2_audio));

    route_lv2_audio_run = (AgsRouteLv2AudioRun *) g_object_new(AGS_TYPE_ROUTE_LV2_AUDIO_RUN,
							       "output-soundcard", output_soundcard,
							       "recall-audio", route_lv2_audio,
							       "recall-container", play_container,
							       NULL);
    ags_recall_set_flags(route_lv2_audio_run,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(route_lv2_audio_run,
				 (AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(route_lv2_audio_run,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) route_lv2_audio_run, TRUE);
    recall = g_list_prepend(recall,
			    route_lv2_audio_run);

    g_object_set(play_container,
		 "recall-audio-run", route_lv2_audio_run,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(route_lv2_audio_run));
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    if(recall_container == NULL){
      recall_container = ags_recall_container_new();
    }

    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    route_lv2_audio = (AgsRouteLv2Audio *) g_object_new(AGS_TYPE_ROUTE_LV2_AUDIO,
							"output-soundcard", output_soundcard,
							"audio", audio,
							"recall-container", recall_container,
							NULL);
    ags_recall_set_flags(route_lv2_audio,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(route_lv2_audio,
				 (AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(route_lv2_audio,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) route_lv2_audio, FALSE);
    recall = g_list_prepend(recall,
			    route_lv2_audio);

    g_object_set(recall_container,
		 "recall-audio", route_lv2_audio,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(route_lv2_audio));

    route_lv2_audio_run = (AgsRouteLv2AudioRun *) g_object_new(AGS_TYPE_ROUTE_LV2_AUDIO_RUN,
							       "output-soundcard", output_soundcard,
							       "recall-audio", route_lv2_audio,
							       "recall-container", recall_container,
							       //TODO:JK: add missing dependency "route_lv2-audio"
							       NULL);
    ags_recall_set_flags(route_lv2_audio_run,
			 (AGS_RECALL_TEMPLATE));
    ags_recall_set_ability_flags(route_lv2_audio_run,
				 (AGS_SOUND_ABILITY_MIDI));
    ags_recall_set_behaviour_flags(route_lv2_audio_run,
				   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT: AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT));

    ags_audio_add_recall(audio, (GObject *) route_lv2_audio_run, FALSE);
    recall = g_list_prepend(recall,
			    route_lv2_audio_run);

    g_object_set(recall_container,
		 "recall-audio-run", route_lv2_audio_run,
		 NULL);
    ags_connectable_connect(AGS_CONNECTABLE(route_lv2_audio_run));
  }

  /* return instantiated recall */
  recall = g_list_reverse(recall);

  return(recall);
}

/**
 * ags_recall_factory_create:
 * @audio: an #AgsAudio that should keep the recalls
 * @play_container: an #AgsRecallContainer to indetify what recall to use
 * @recall_container: an #AgsRecallContainer to indetify what recall to use
 * @plugin_name: the plugin identifier to instantiate 
 * @start_audio_channel: the first audio channel to apply
 * @stop_audio_channel: the last audio channel to apply
 * @start_pad: the first pad to apply
 * @stop_pad: the last pad to apply
 * @create_flags: modify the behaviour of this function
 * @recall_flags: flags to be set for #AgsRecall
 *
 * Instantiate #AgsRecall by this factory.
 *
 * Returns: The available AgsPort objects of the plugin to modify.
 * 
 * Since: 2.0.0
 */
GList*
ags_recall_factory_create(AgsAudio *audio,
			  AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			  gchar *plugin_name,
			  guint start_audio_channel, guint stop_audio_channel,
			  guint start_pad, guint stop_pad,
			  guint create_flags, guint recall_flags)
{
  GList *recall;
  
  recall = NULL;

#ifdef AGS_DEBUG
  g_message("AgsRecallFactory creating: %s[%d,%d]", plugin_name, stop_pad, stop_audio_channel);
#endif

  if(!strncmp(plugin_name,
	      "ags-delay",
	      10)){
    recall = ags_recall_factory_create_delay(audio,
					     play_container, recall_container,
					     plugin_name,
					     start_audio_channel, stop_audio_channel,
					     start_pad, stop_pad,
					     create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-count-beats",
		    16)){
    recall = ags_recall_factory_create_count_beats(audio,
						   play_container, recall_container,
						   plugin_name,
						   start_audio_channel, stop_audio_channel,
						   start_pad, stop_pad,
						   create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-stream",
		    11)){
    recall = ags_recall_factory_create_stream(audio,
					      play_container, recall_container,
					      plugin_name,
					      start_audio_channel, stop_audio_channel,
					      start_pad, stop_pad,
					      create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-rt-stream",
		    14)){
    recall = ags_recall_factory_create_rt_stream(audio,
						 play_container, recall_container,
						 plugin_name,
						 start_audio_channel, stop_audio_channel,
						 start_pad, stop_pad,
						 create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-loop",
		    9)){
    recall = ags_recall_factory_create_loop(audio,
					    play_container, recall_container,
					    plugin_name,
					    start_audio_channel, stop_audio_channel,
					    start_pad, stop_pad,
					    create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-play-master",
		    16)){
    recall = ags_recall_factory_create_play_master(audio,
						   play_container, recall_container,
						   plugin_name,
						   start_audio_channel, stop_audio_channel,
						   start_pad, stop_pad,
						   create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-prepare",
		    12)){
    recall = ags_recall_factory_create_prepare(audio,
					       play_container, recall_container,
					       plugin_name,
					       start_audio_channel, stop_audio_channel,
					       start_pad, stop_pad,
					       create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-copy",
		    9)){
    recall = ags_recall_factory_create_copy(audio,
					    play_container, recall_container,
					    plugin_name,
					    start_audio_channel, stop_audio_channel,
					    start_pad, stop_pad,
					    create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-buffer",
		    11)){
    recall = ags_recall_factory_create_buffer(audio,
					      play_container, recall_container,
					      plugin_name,
					      start_audio_channel, stop_audio_channel,
					      start_pad, stop_pad,
					      create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-play",
		    9)){
    recall = ags_recall_factory_create_play(audio,
					    play_container, recall_container,
					    plugin_name,
					    start_audio_channel, stop_audio_channel,
					    start_pad, stop_pad,
					    create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-copy-pattern",
		    17)){
    recall = ags_recall_factory_create_copy_pattern(audio,
						    play_container, recall_container,
						    plugin_name,
						    start_audio_channel, stop_audio_channel,
						    start_pad, stop_pad,
						    create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-play-wave",
		    13)){
    recall = ags_recall_factory_create_play_wave(audio,
						 play_container, recall_container,
						 plugin_name,
						 start_audio_channel, stop_audio_channel,
						 start_pad, stop_pad,
						 create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-capture-wave",
		    17)){
    recall = ags_recall_factory_create_capture_wave(audio,
						    play_container, recall_container,
						    plugin_name,
						    start_audio_channel, stop_audio_channel,
						    start_pad, stop_pad,
						    create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-play-dssi",
		    14)){
    recall = ags_recall_factory_create_play_dssi(audio,
						 play_container, recall_container,
						 plugin_name,
						 start_audio_channel, stop_audio_channel,
						 start_pad, stop_pad,
						 create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-play-lv2",
		    13)){
    recall = ags_recall_factory_create_play_lv2(audio,
						play_container, recall_container,
						plugin_name,
						start_audio_channel, stop_audio_channel,
						start_pad, stop_pad,
						create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-play-notation",
		    18)){
    recall = ags_recall_factory_create_play_notation(audio,
						     play_container, recall_container,
						     plugin_name,
						     start_audio_channel, stop_audio_channel,
						     start_pad, stop_pad,
						     create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-peak",
		    9)){
    recall = ags_recall_factory_create_peak(audio,
					    play_container, recall_container,
					    plugin_name,
					    start_audio_channel, stop_audio_channel,
					    start_pad, stop_pad,
					    create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-mute",
		    9)){
    recall = ags_recall_factory_create_mute(audio,
					    play_container, recall_container,
					    plugin_name,
					    start_audio_channel, stop_audio_channel,
					    start_pad, stop_pad,
					    create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-volume",
		    11)){
    recall = ags_recall_factory_create_volume(audio,
					      play_container, recall_container,
					      plugin_name,
					      start_audio_channel, stop_audio_channel,
					      start_pad, stop_pad,
					      create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-envelope",
		    11)){
    recall = ags_recall_factory_create_envelope(audio,
						play_container, recall_container,
						plugin_name,
						start_audio_channel, stop_audio_channel,
						start_pad, stop_pad,
						create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-ladspa",
		    11)){
    recall = ags_recall_factory_create_ladspa(audio,
					      play_container, recall_container,
					      plugin_name,
					      start_audio_channel, stop_audio_channel,
					      start_pad, stop_pad,
					      create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-dssi",
		    9)){
    recall = ags_recall_factory_create_dssi(audio,
					    play_container, recall_container,
					    plugin_name,
					    start_audio_channel, stop_audio_channel,
					    start_pad, stop_pad,
					    create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-lv2",
		    8)){
    recall = ags_recall_factory_create_lv2(audio,
					   play_container, recall_container,
					   plugin_name,
					   start_audio_channel, stop_audio_channel,
					   start_pad, stop_pad,
					   create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-record-midi",
		    16)){
    recall = ags_recall_factory_create_record_midi(audio,
						   play_container, recall_container,
						   plugin_name,
						   start_audio_channel, stop_audio_channel,
						   start_pad, stop_pad,
						   create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-route-dssi",
		    15)){
    recall = ags_recall_factory_create_route_dssi(audio,
						  play_container, recall_container,
						  plugin_name,
						  start_audio_channel, stop_audio_channel,
						  start_pad, stop_pad,
						  create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-route-lv2",
		    14)){
    recall = ags_recall_factory_create_route_lv2(audio,
						 play_container, recall_container,
						 plugin_name,
						 start_audio_channel, stop_audio_channel,
						 start_pad, stop_pad,
						 create_flags, recall_flags);
  }

  return(recall);
}

/**
 * ags_recall_factory_new:
 *
 * Create a new instance of #AgsRecallFactory
 *
 * Returns: the new #AgsRecallFactory
 * 
 * Since: 2.0.0
 */
AgsRecallFactory*
ags_recall_factory_new()
{
  AgsRecallFactory *recall_factory;

  recall_factory = (AgsRecallFactory *) g_object_new(AGS_TYPE_RECALL_FACTORY,
						     NULL);

  return(recall_factory);
}
