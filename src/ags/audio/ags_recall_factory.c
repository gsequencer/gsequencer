/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/ags_recall_factory.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_port.h>

#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_channel_run_master.h>
#include <ags/audio/recall/ags_play_channel_run.h>
#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_loop_channel.h>
#include <ags/audio/recall/ags_loop_channel_run.h>
#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_channel_run.h>
#include <ags/audio/recall/ags_copy_channel.h>
#include <ags/audio/recall/ags_copy_channel_run.h>
#include <ags/audio/recall/ags_stream_channel.h>
#include <ags/audio/recall/ags_stream_channel_run.h>
#include <ags/audio/recall/ags_copy_pattern_audio.h>
#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>
#include <ags/audio/recall/ags_copy_notation_audio.h>
#include <ags/audio/recall/ags_copy_notation_audio_run.h>
#include <ags/audio/recall/ags_volume_channel.h>
#include <ags/audio/recall/ags_volume_channel_run.h>

#include <string.h>

void ags_recall_factory_class_init(AgsRecallFactoryClass *recall_factory_class);
void ags_recall_factory_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_factory_init(AgsRecallFactory *recall_factory);
void ags_recall_factory_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_recall_factory_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_recall_factory_connect(AgsConnectable *connectable);
void ags_recall_factory_disconnect(AgsConnectable *connectable);

static gpointer ags_recall_factory_parent_class = NULL;

GType
ags_recall_factory_get_type (void)
{
  static GType ags_type_recall_factory = 0;

  if(!ags_type_recall_factory){
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
						     "AgsRecallFactory\0",
						     &ags_recall_factory_info,
						     0);

    g_type_add_interface_static(ags_type_recall_factory,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_recall_factory);
}

void
ags_recall_factory_class_init(AgsRecallFactoryClass *recall_factory)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_recall_factory_parent_class = g_type_class_peek_parent(recall_factory);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_factory;

  gobject->set_property = ags_recall_factory_set_property;
  gobject->get_property = ags_recall_factory_get_property;
}

void
ags_recall_factory_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;
  connectable->connect = ags_recall_factory_connect;
  connectable->disconnect = ags_recall_factory_disconnect;
}

void
ags_recall_factory_init(AgsRecallFactory *recall_factory)
{
}

void
ags_recall_factory_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsRecallFactory *recall_factory;

  recall_factory = AGS_RECALL_FACTORY(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_factory_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsRecallFactory *recall_factory;

  recall_factory = AGS_RECALL_FACTORY(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_factory_connect(AgsConnectable *connectable)
{
  AgsRecallFactory *recall_factory;

  recall_factory = AGS_RECALL_FACTORY(connectable);
}

void
ags_recall_factory_disconnect(AgsConnectable *connectable)
{
  AgsRecallFactory *recall_factory;

  recall_factory = AGS_RECALL_FACTORY(connectable);
}

GList*
ags_recall_factory_create(AgsAudio *audio,
			  gchar *plugin_name,
			  guint start_audio_channel, guint stop_audio_channel,
			  guint start_pad, guint stop_pad,
			  gboolean is_output)
{
  AgsChannel *start, *channel;
  AgsPort *port;
  GList *list;
  guint i, j;
  
  if(audio == NULL){
    return(NULL);
  }

  if(is_output){
    start =
      channel = ags_channel_nth(audio->output,
				start_pad * audio->audio_channels);
  }else{
    start =
      channel = ags_channel_nth(audio->input,
				start_pad * audio->audio_channels);;
  }

  list = NULL;

  if(!strncmp(plugin_name,
	      "ags-delay\0",
	      9)){
    AgsRecallContainer *recall_container;
    AgsDelayAudio *delay_audio;
    AgsDelayAudioRun *delay_audio_run;

    /* play */
    recall_container = ags_recall_container_new();
    recall_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    delay_audio = (AgsDelayAudio *) g_object_new(AGS_TYPE_DELAY_AUDIO,
						 "devout\0", audio->devout,
						 "audio\0", audio,
						 "recall_container\0", recall_container,
						 NULL);
    ags_recall_set_flags(AGS_RECALL(delay_audio), (AGS_RECALL_TEMPLATE |
						   AGS_RECALL_PLAYBACK |
						   AGS_RECALL_SEQUENCER |
						   AGS_RECALL_NOTATION));
    ags_audio_add_recall(audio, (GObject *) delay_audio, TRUE);

    delay_audio_run = (AgsDelayAudioRun *) g_object_new(AGS_TYPE_DELAY_AUDIO_RUN,
							"devout\0", audio->devout,
							"recall_audio\0", delay_audio,
							"recall_container\0", recall_container,
							NULL);
    ags_recall_set_flags(AGS_RECALL(delay_audio_run), (AGS_RECALL_TEMPLATE |
						       AGS_RECALL_PLAYBACK |
						       AGS_RECALL_SEQUENCER |
						       AGS_RECALL_NOTATION));
    ags_audio_add_recall(audio, (GObject *) delay_audio_run, TRUE);

    /* recall */
    recall_container = ags_recall_container_new();
    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    delay_audio = (AgsDelayAudio *) g_object_new(AGS_TYPE_DELAY_AUDIO,
						 "devout\0", audio->devout,
						 "audio\0", audio,
						 "recall_container\0", recall_container,
						 NULL);
    AGS_RECALL(delay_audio)->flags |= (AGS_RECALL_TEMPLATE);
    ags_audio_add_recall(audio, (GObject *) delay_audio, FALSE);

    delay_audio_run = (AgsDelayAudioRun *) g_object_new(AGS_TYPE_DELAY_AUDIO_RUN,
							"devout\0", audio->devout,
							"recall_audio\0", delay_audio,
							"recall_container\0", recall_container,
							NULL);
    ags_recall_set_flags(AGS_RECALL(delay_audio_run), (AGS_RECALL_TEMPLATE |
						       AGS_RECALL_PLAYBACK |
						       AGS_RECALL_SEQUENCER |
						       AGS_RECALL_NOTATION));
    ags_audio_add_recall(audio, (GObject *) delay_audio_run, FALSE);
  }else if(!strncmp(plugin_name,
		    "ags-count-beats\0",
		    15)){
    AgsRecallContainer *recall_container;
    AgsCountBeatsAudio *count_beats_audio;
    AgsCountBeatsAudioRun *count_beats_audio_run;

    /* play */
    recall_container = ags_recall_container_new();
    recall_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    count_beats_audio = (AgsCountBeatsAudio *) g_object_new(AGS_TYPE_COUNT_BEATS_AUDIO,
							    "devout\0", audio->devout,
							    "audio\0", audio,
							    "recall_container\0", recall_container,
							    NULL);
    ags_recall_set_flags(AGS_RECALL(count_beats_audio), (AGS_RECALL_TEMPLATE |
							 AGS_RECALL_PLAYBACK |
							 AGS_RECALL_SEQUENCER |
							 AGS_RECALL_NOTATION));
    ags_audio_add_recall(audio, (GObject *) count_beats_audio, TRUE);

    count_beats_audio_run = (AgsCountBeatsAudioRun *) g_object_new(AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
								   "devout\0", audio->devout,
								   "recall_audio\0", count_beats_audio,
								   "recall_container\0", recall_container,
								   NULL);
    ags_recall_set_flags(AGS_RECALL(count_beats_audio_run), (AGS_RECALL_TEMPLATE |
							     AGS_RECALL_PLAYBACK |
							     AGS_RECALL_SEQUENCER |
							     AGS_RECALL_NOTATION));
    ags_audio_add_recall(audio, (GObject *) count_beats_audio_run, TRUE);

    /* recall */
    recall_container = ags_recall_container_new();
    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    count_beats_audio = (AgsCountBeatsAudio *) g_object_new(AGS_TYPE_COUNT_BEATS_AUDIO,
							    "devout\0", audio->devout,
							    "audio\0", audio,
							    "recall_container\0", recall_container,
							    NULL);
    ags_recall_set_flags(AGS_RECALL(count_beats_audio), (AGS_RECALL_TEMPLATE |
							 AGS_RECALL_PLAYBACK |
							 AGS_RECALL_SEQUENCER |
							 AGS_RECALL_NOTATION));
    ags_audio_add_recall(audio, (GObject *) count_beats_audio, FALSE);

    count_beats_audio_run = (AgsCountBeatsAudioRun *) g_object_new(AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
								   "devout\0", audio->devout,
								   "recall_audio\0", count_beats_audio,
								   "recall_container\0", recall_container,
								   NULL);
    ags_recall_set_flags(AGS_RECALL(count_beats_audio_run), (AGS_RECALL_TEMPLATE |
							     AGS_RECALL_PLAYBACK |
							     AGS_RECALL_SEQUENCER |
							     AGS_RECALL_NOTATION));
    ags_audio_add_recall(audio, (GObject *) count_beats_audio_run, FALSE);
  }else if(!strncmp(plugin_name,
		    "ags-stream\0",
		    10)){
    AgsRecallContainer *recall_container;
    AgsStreamChannel *stream_channel;
    AgsStreamChannelRun *stream_channel_run;

    /* play */
    recall_container = ags_recall_container_new();
    recall_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	ags_channel_add_recall_container(channel, (GObject *) recall_container);

	/* AgsStreamChannel */
	stream_channel = (AgsStreamChannel *) g_object_new(AGS_TYPE_STREAM_CHANNEL,
							   "devout\0", audio->devout,
							   "source\0", channel,
							   "recall_container\0", recall_container,
							   NULL);
	ags_recall_set_flags(AGS_RECALL(stream_channel), (AGS_RECALL_TEMPLATE |
							  AGS_RECALL_PLAYBACK |
							  AGS_RECALL_SEQUENCER |
							  AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) stream_channel, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(stream_channel));

	/* AgsStreamChannelRun */
	stream_channel_run = (AgsStreamChannelRun *) g_object_new(AGS_TYPE_STREAM_CHANNEL_RUN,
								  "devout\0", audio->devout,
								  "source\0", channel,
								  "recall_channel\0", stream_channel,
								  "recall_container\0", recall_container,
								  NULL);
	ags_recall_set_flags(AGS_RECALL(stream_channel_run), (AGS_RECALL_TEMPLATE |
							      AGS_RECALL_PLAYBACK |
							      AGS_RECALL_SEQUENCER |
							      AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) stream_channel_run, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(stream_channel_run));

	/* iterate */
	channel = channel->next;
      }
    }

    /* recall */
    channel = start;

    recall_container = ags_recall_container_new();
    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	ags_channel_add_recall_container(channel, (GObject *) recall_container);
    
	/* AgsStreamChannel */
	stream_channel = (AgsStreamChannel *) g_object_new(AGS_TYPE_STREAM_CHANNEL,
							   "devout\0", audio->devout,
							   "source\0", channel,
							   "recall_container\0", recall_container,
							   NULL);
	ags_recall_set_flags(AGS_RECALL(stream_channel), (AGS_RECALL_TEMPLATE |
							  AGS_RECALL_PLAYBACK |
							  AGS_RECALL_SEQUENCER |
							  AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) stream_channel, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(stream_channel));

	/* AgsStreamChannelRun */
	stream_channel_run = (AgsStreamChannelRun *) g_object_new(AGS_TYPE_STREAM_CHANNEL_RUN,
								  "devout\0", audio->devout,
								  "source\0", channel,
								  "recall_channel\0", stream_channel,
								  "recall_container\0", recall_container,
								  NULL);
	ags_recall_set_flags(AGS_RECALL(stream_channel_run), (AGS_RECALL_TEMPLATE |
							      AGS_RECALL_PLAYBACK |
							      AGS_RECALL_SEQUENCER |
							      AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) stream_channel_run, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(stream_channel_run));

	/* iterate */
	channel = channel->next;
      }
    }
  }else if(!strncmp(plugin_name,
		    "ags-loop\0",
		    8)){
    AgsRecallContainer *recall_container;
    AgsLoopChannel *loop_channel;
    AgsLoopChannelRun *loop_channel_run;

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	//TODO:JK: implement me
      }
    }
  }else if(!strncmp(plugin_name,
		    "ags-play-master\0",
		    15)){
    AgsRecallContainer *recall_container;
    AgsPlayChannel *play_channel;
    AgsPlayChannelRunMaster *play_channel_run_master;

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	//TODO:JK: implement me
      }
    }
  }else if(!strncmp(plugin_name,
		    "ags-play\0",
		    8)){
    AgsRecallContainer *recall_container;
    AgsPlayChannel *play_channel;
    AgsPlayChannelRun *play_channel_run;

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	//TODO:JK: implement me
      }
    }
  }else if(!strncmp(plugin_name,
		    "ags-copy-pattern\0",
		    16)){
    AgsRecallContainer *recall_container;
    AgsCopyPatternAudio *copy_pattern_audio;
    AgsCopyPatternAudioRun *copy_pattern_audio_run;
    AgsCopyPatternChannel *copy_pattern_channel;
    AgsCopyPatternChannelRun *copy_pattern_channel_run;

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	//TODO:JK: implement me
      }
    }
  }else if(!strncmp(plugin_name,
		    "ags-copy-notation\0",
		    17)){
    AgsRecallContainer *recall_container;
    AgsCopyNotationAudio *copy_notation_audio;
    AgsCopyNotationAudioRun *copy_notation_audio_run;

    //TODO:JK: implement me
  }else if(!strncmp(plugin_name,
		    "ags-volume\0",
		    10)){
    AgsRecallContainer *recall_container;
    AgsVolumeChannel *volume_channel;
    AgsVolumeChannelRun *volume_channel_run;

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	//TODO:JK: implement me
      }
    }
  }

  return(list);
}

AgsRecallFactory*
ags_recall_factory_new()
{
  AgsRecallFactory *recall_factory;

  recall_factory = (AgsRecallFactory *) g_object_new(AGS_TYPE_RECALL_FACTORY,
						     NULL);

  return(recall_factory);
}
