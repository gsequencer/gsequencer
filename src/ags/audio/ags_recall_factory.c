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
#include <ags/audio/ags_recall_channel_run_dummy.h>
#include <ags/audio/ags_recall_recycling_dummy.h>
#include <ags/audio/ags_recall_ladspa.h>
#include <ags/audio/ags_recall_ladspa_run.h>
#include <ags/audio/ags_port.h>

#include <ags/audio/recall/ags_play_audio.h>
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
#include <ags/audio/recall/ags_buffer_channel.h>
#include <ags/audio/recall/ags_buffer_channel_run.h>
#include <ags/audio/recall/ags_copy_pattern_audio.h>
#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>
#include <ags/audio/recall/ags_play_notation_audio.h>
#include <ags/audio/recall/ags_play_notation_audio_run.h>
#include <ags/audio/recall/ags_peak_channel.h>
#include <ags/audio/recall/ags_peak_channel_run.h>
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
GList* ags_recall_factory_create_copy(AgsAudio *audio,
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
GList* ags_recall_factory_create_volume(AgsAudio *audio,
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
ags_recall_factory_create_play(AgsAudio *audio,
			       AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			       gchar *plugin_name,
			       guint start_audio_channel, guint stop_audio_channel,
			       guint start_pad, guint stop_pad,
			       guint create_flags, guint recall_flags)
{
  AgsPlayChannel *play_channel;
  AgsPlayChannelRun *play_channel_run;
  AgsChannel *start, *channel;
  AgsPort *port;
  GList *list;
  guint i, j;
  
  if(audio == NULL){
    return(NULL);
  }

  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(audio->output,
				start_pad * audio->audio_channels);
  }else{
    start =
      channel = ags_channel_nth(audio->input,
				start_pad * audio->audio_channels);
  }

  list = NULL;

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
	ags_channel_add_recall_container(channel, (GObject *) play_container);

	/* AgsPlayChannel */
	play_channel = (AgsPlayChannel *) g_object_new(AGS_TYPE_PLAY_CHANNEL,
						       "soundcard\0", audio->soundcard,
						       "source\0", channel,
						       "recall_container\0", play_container,
						       NULL);
	ags_recall_set_flags(AGS_RECALL(play_channel), (AGS_RECALL_TEMPLATE |
							(((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							AGS_RECALL_PLAYBACK));
	play_channel->audio_channel->port_value.ags_port_uint = start_audio_channel + j;
	ags_channel_add_recall(channel, (GObject *) play_channel, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(play_channel));

	/* AgsPlayChannelRun */
	play_channel_run = (AgsPlayChannelRun *) g_object_new(AGS_TYPE_PLAY_CHANNEL_RUN,
							      "soundcard\0", audio->soundcard,
							      "source\0", channel,
							      "recall_channel\0", play_channel,
							      "recall_container\0", play_container,
							      NULL);
	ags_recall_set_flags(AGS_RECALL(play_channel_run), (AGS_RECALL_TEMPLATE |
							    (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							    AGS_RECALL_PLAYBACK));
	ags_channel_add_recall(channel, (GObject *) play_channel_run, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(play_channel_run));

	/* iterate */
	channel = channel->next;
      }

      channel = ags_channel_nth(channel,
				audio->audio_channels - stop_audio_channel);
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
	ags_channel_add_recall_container(channel, (GObject *) recall_container);

	/* AgsPlayChannel */
	play_channel = (AgsPlayChannel *) g_object_new(AGS_TYPE_PLAY_CHANNEL,
						       "soundcard\0", AGS_SOUNDCARD(audio->soundcard),
						       "source\0", channel,
						       "recall_container\0", recall_container,
						       NULL);
	ags_recall_set_flags(AGS_RECALL(play_channel), (AGS_RECALL_TEMPLATE |
							(((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							AGS_RECALL_PLAYBACK));
	play_channel->audio_channel->port_value.ags_port_uint = start_audio_channel + j;
	ags_channel_add_recall(channel, (GObject *) play_channel, FALSE);
	ags_connectable_connect(AGS_CONNECTABLE(play_channel));

	/* AgsPlayChannelRun */
	play_channel_run = (AgsPlayChannelRun *) g_object_new(AGS_TYPE_PLAY_CHANNEL_RUN,
							      "soundcard\0", audio->soundcard,
							      "source\0", channel,
							      "recall_channel\0", play_channel,
							      "recall_container\0", recall_container,
							      NULL);
	ags_recall_set_flags(AGS_RECALL(play_channel_run), (AGS_RECALL_TEMPLATE |
							    (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							    AGS_RECALL_PLAYBACK));
	ags_channel_add_recall(channel, (GObject *) play_channel_run, FALSE);
	ags_connectable_connect(AGS_CONNECTABLE(play_channel_run));

	/* iterate */
	channel = channel->next;
      }

      channel = ags_channel_nth(channel,
				audio->audio_channels - stop_audio_channel);
    }
  }
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
  AgsChannel *start, *channel;
  AgsPort *port;
  GList *list;
  guint i, j;

  if(audio == NULL){
    return(NULL);
  }

  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(audio->output,
				start_pad * audio->audio_channels);
  }else{
    start =
      channel = ags_channel_nth(audio->input,
				start_pad * audio->audio_channels);
  }

  list = NULL;

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
						 "soundcard\0", audio->soundcard,
						 "audio\0", audio,
						 "recall_container\0", play_container,
						 NULL);
      AGS_RECALL(play_audio)->flags |= (AGS_RECALL_TEMPLATE |
					(((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
					AGS_RECALL_PLAYBACK |
					AGS_RECALL_SEQUENCER |
					AGS_RECALL_NOTATION);
      ags_audio_add_recall(audio, (GObject *) play_audio, TRUE);
    }else{
      GList *list;

      if(play_container == NULL){
	list = ags_recall_find_type(audio->play, AGS_TYPE_PLAY_AUDIO);

	play_audio = AGS_PLAY_AUDIO(list->data);

	play_container = AGS_RECALL_CONTAINER(AGS_RECALL(play_audio)->container);
      }else{
	play_audio = AGS_PLAY_AUDIO(play_container->recall_audio);
      }
    }

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	ags_channel_add_recall_container(channel, (GObject *) play_container);

	/* AgsPlayChannel */
	play_channel = (AgsPlayChannel *) g_object_new(AGS_TYPE_PLAY_CHANNEL,
						       "soundcard\0", AGS_SOUNDCARD(audio->soundcard),
						       "source\0", channel,
						       "recall_container\0", play_container,
						       NULL);
	ags_recall_set_flags(AGS_RECALL(play_channel), (AGS_RECALL_TEMPLATE |
							(((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							AGS_RECALL_PLAYBACK |
							AGS_RECALL_SEQUENCER |
							AGS_RECALL_NOTATION));
	play_channel->audio_channel->port_value.ags_port_uint = start_audio_channel + j;
	ags_channel_add_recall(channel, (GObject *) play_channel, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(play_channel));
      
	/* AgsPlayChannelRun */
	play_channel_run_master = (AgsPlayChannelRunMaster *) g_object_new(AGS_TYPE_PLAY_CHANNEL_RUN_MASTER,
									   "soundcard\0", audio->soundcard,
									   "source\0", channel,
									   "recall_channel\0", play_channel,
									   "recall_container\0", play_container,
									   NULL);
	ags_recall_set_flags(AGS_RECALL(play_channel_run_master), (AGS_RECALL_TEMPLATE |
								   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
								   AGS_RECALL_PLAYBACK |
								   AGS_RECALL_SEQUENCER |
								   AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) play_channel_run_master, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(play_channel_run_master));

	/* iterate */
	channel = channel->next;
      }

      channel = ags_channel_nth(channel,
				audio->audio_channels - stop_audio_channel);
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
						 "soundcard\0", audio->soundcard,
						 "audio\0", audio,
						 "recall_container\0", recall_container,
						 NULL);
      AGS_RECALL(play_audio)->flags |= (AGS_RECALL_TEMPLATE |
					(((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
					AGS_RECALL_PLAYBACK |
					AGS_RECALL_SEQUENCER |
					AGS_RECALL_NOTATION);
      ags_audio_add_recall(audio, (GObject *) play_audio, FALSE);
    }else{
      GList *list;
      
      if(recall_container == NULL){
	list = ags_recall_find_type(audio->recall, AGS_TYPE_PLAY_AUDIO);

	play_audio = AGS_PLAY_AUDIO(list->data);

	recall_container = AGS_RECALL_CONTAINER(AGS_RECALL(play_audio)->container);
      }else{
	play_audio = AGS_PLAY_AUDIO(recall_container->recall_audio);
      }
    }

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	ags_channel_add_recall_container(channel, (GObject *) recall_container);

	/* AgsPlayChannel */
	play_channel = (AgsPlayChannel *) g_object_new(AGS_TYPE_PLAY_CHANNEL,
						       "soundcard\0", AGS_SOUNDCARD(audio->soundcard),
						       "source\0", channel,
						       "recall_container\0", recall_container,
						       NULL);
	ags_recall_set_flags(AGS_RECALL(play_channel), (AGS_RECALL_TEMPLATE |
							(((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							AGS_RECALL_PLAYBACK |
							AGS_RECALL_SEQUENCER |
							AGS_RECALL_NOTATION));
	play_channel->audio_channel->port_value.ags_port_uint = start_audio_channel + j;
	ags_channel_add_recall(channel, (GObject *) play_channel, FALSE);
	ags_connectable_connect(AGS_CONNECTABLE(play_channel));
	
	/* AgsPlayChannelRun */
	play_channel_run_master = (AgsPlayChannelRunMaster *) g_object_new(AGS_TYPE_PLAY_CHANNEL_RUN_MASTER,
									   "soundcard\0", audio->soundcard,
									   "source\0", channel,
									   "recall_channel\0", play_channel,
									   "recall_container\0", recall_container,
									   NULL);
	ags_recall_set_flags(AGS_RECALL(play_channel_run_master), (AGS_RECALL_TEMPLATE |
								   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
								   AGS_RECALL_PLAYBACK |
								   AGS_RECALL_SEQUENCER |
								   AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) play_channel_run_master, FALSE);
	ags_connectable_connect(AGS_CONNECTABLE(play_channel_run_master));

	/* iterate */
	channel = channel->next;
      }

      channel = ags_channel_nth(channel,
				audio->audio_channels - stop_audio_channel);
    }
  }
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
  AgsChannel *start, *channel;
  AgsPort *port;
  GList *list;
  guint i, j;

  if(audio == NULL){
    return(NULL);
  }

  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(audio->output,
				start_pad * audio->audio_channels);
  }else{
    start =
      channel = ags_channel_nth(audio->input,
				start_pad * audio->audio_channels);
  }

  list = NULL;

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
	ags_channel_add_recall_container(channel, (GObject *) play_container);

	/* AgsCopyChannel */
	copy_channel = (AgsCopyChannel *) g_object_new(AGS_TYPE_COPY_CHANNEL,
						       "soundcard\0", audio->soundcard,
						       "source\0", channel,
						       "recall_container\0", play_container,
						       NULL);
	ags_recall_set_flags(AGS_RECALL(copy_channel), (AGS_RECALL_TEMPLATE |
							(((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							AGS_RECALL_PLAYBACK |
							AGS_RECALL_SEQUENCER |
							AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) copy_channel, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(copy_channel));

	/* AgsCopyChannelRun */
	copy_channel_run = (AgsCopyChannelRun *) g_object_new(AGS_TYPE_COPY_CHANNEL_RUN,
							      "soundcard\0", audio->soundcard,
							      "source\0", channel,
							      "recall_channel\0", copy_channel,
							      "recall_container\0", play_container,
							      NULL);
	ags_recall_set_flags(AGS_RECALL(copy_channel_run), (AGS_RECALL_TEMPLATE |
							    (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							    AGS_RECALL_PLAYBACK |
							    AGS_RECALL_SEQUENCER |
							    AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) copy_channel_run, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(copy_channel_run));

	/* iterate */
	channel = channel->next;
      }

      channel = ags_channel_nth(channel,
				audio->audio_channels - stop_audio_channel);
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
	ags_channel_add_recall_container(channel, (GObject *) recall_container);

	/* AgsCopyChannel */
	copy_channel = (AgsCopyChannel *) g_object_new(AGS_TYPE_COPY_CHANNEL,
						       "soundcard\0", AGS_SOUNDCARD(audio->soundcard),
						       "source\0", channel,
						       "recall_container\0", recall_container,
						       NULL);
	ags_recall_set_flags(AGS_RECALL(copy_channel), (AGS_RECALL_TEMPLATE |
							(((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							AGS_RECALL_PLAYBACK |
							AGS_RECALL_SEQUENCER |
							AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) copy_channel, FALSE);
	ags_connectable_connect(AGS_CONNECTABLE(copy_channel));

	/* AgsCopyChannelRun */
	copy_channel_run = (AgsCopyChannelRun *) g_object_new(AGS_TYPE_COPY_CHANNEL_RUN,
							      "soundcard\0", audio->soundcard,
							      "source\0", channel,
							      "recall_channel\0", copy_channel,
							      "recall_container\0", recall_container,
							      NULL);
	ags_recall_set_flags(AGS_RECALL(copy_channel_run), (AGS_RECALL_TEMPLATE |
							    (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							    AGS_RECALL_PLAYBACK |
							    AGS_RECALL_SEQUENCER |
							    AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) copy_channel_run, FALSE);
	ags_connectable_connect(AGS_CONNECTABLE(copy_channel_run));

	/* iterate */
	channel = channel->next;
      }

      channel = ags_channel_nth(channel,
				audio->audio_channels - stop_audio_channel);
    }
  }
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
  AgsChannel *start, *channel;
  AgsPort *port;
  GList *list;
  guint i, j;
  
  if(audio == NULL){
    return(NULL);
  }

  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(audio->output,
				start_pad * audio->audio_channels);
  }else{
    start =
      channel = ags_channel_nth(audio->input,
				start_pad * audio->audio_channels);
  }

  list = NULL;

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
	ags_channel_add_recall_container(channel, (GObject *) play_container);

	/* AgsStreamChannel */
	stream_channel = (AgsStreamChannel *) g_object_new(AGS_TYPE_STREAM_CHANNEL,
							   "soundcard\0", audio->soundcard,
							   "source\0", channel,
							   "recall_container\0", play_container,
							   NULL);
							      
	ags_recall_set_flags(AGS_RECALL(stream_channel), (AGS_RECALL_TEMPLATE |
							  (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							  AGS_RECALL_PLAYBACK |
							  AGS_RECALL_SEQUENCER |
							  AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) stream_channel, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(stream_channel));

	/* AgsStreamChannelRun */
	stream_channel_run = (AgsStreamChannelRun *) g_object_new(AGS_TYPE_STREAM_CHANNEL_RUN,
								  "soundcard\0", audio->soundcard,
								  "recall-channel\0", stream_channel,
								  "source\0", channel,
								  "recall_container\0", play_container,
								  NULL);
	ags_recall_set_flags(AGS_RECALL(stream_channel_run), (AGS_RECALL_TEMPLATE |
							      (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							      AGS_RECALL_PLAYBACK |
							      AGS_RECALL_SEQUENCER |
							      AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) stream_channel_run, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(stream_channel_run));

	/* iterate */
	channel = channel->next;
      }

      channel = ags_channel_nth(channel,
				audio->audio_channels - stop_audio_channel);
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
	ags_channel_add_recall_container(channel, (GObject *) recall_container);

	/* AgsStreamChannel */
	stream_channel = (AgsStreamChannel *) g_object_new(AGS_TYPE_STREAM_CHANNEL,
							   "soundcard\0", audio->soundcard,
							   "source\0", channel,
							   "recall_container\0", recall_container,
							   NULL);
							      
	ags_recall_set_flags(AGS_RECALL(stream_channel), (AGS_RECALL_TEMPLATE |
							  (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							  AGS_RECALL_PLAYBACK |
							  AGS_RECALL_SEQUENCER |
							  AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) stream_channel, FALSE);
	ags_connectable_connect(AGS_CONNECTABLE(stream_channel));

	/* AgsStreamChannelRun */
	stream_channel_run = (AgsStreamChannelRun *) g_object_new(AGS_TYPE_STREAM_CHANNEL_RUN,
								  "soundcard\0", audio->soundcard,
								  "recall_channel\0", stream_channel,
								  "source\0", channel,
								  "recall_container\0", recall_container,
								  NULL);
	ags_recall_set_flags(AGS_RECALL(stream_channel_run), (AGS_RECALL_TEMPLATE |
							      (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							      AGS_RECALL_PLAYBACK |
							      AGS_RECALL_SEQUENCER |
							      AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) stream_channel_run, FALSE);
	ags_connectable_connect(AGS_CONNECTABLE(stream_channel_run));

	/* iterate */
	channel = channel->next;
      }

      channel = ags_channel_nth(channel,
				audio->audio_channels - stop_audio_channel);
    }
  }
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
  AgsChannel *start, *channel;
  AgsPort *port;
  GList *list;
  guint i, j;

  if(audio == NULL){
    return(NULL);
  }

  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(audio->output,
				start_pad * audio->audio_channels);
  }else{
    start =
      channel = ags_channel_nth(audio->input,
				start_pad * audio->audio_channels);
  }

  list = NULL;

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
	ags_channel_add_recall_container(channel, (GObject *) play_container);

	/* AgsBufferChannel */
	buffer_channel = (AgsBufferChannel *) g_object_new(AGS_TYPE_BUFFER_CHANNEL,
							   "soundcard\0", audio->soundcard,
							   "source\0", channel,
							   "recall_container\0", play_container,
							   NULL);
							      
	ags_recall_set_flags(AGS_RECALL(buffer_channel), (AGS_RECALL_TEMPLATE |
							  (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							  AGS_RECALL_PLAYBACK |
							  AGS_RECALL_SEQUENCER |
							  AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) buffer_channel, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(buffer_channel));

	/* AgsBufferChannelRun */
	buffer_channel_run = (AgsBufferChannelRun *) g_object_new(AGS_TYPE_BUFFER_CHANNEL_RUN,
								  "soundcard\0", audio->soundcard,
								  "recall_channel\0", buffer_channel,
								  "source\0", channel,
								  "destination\0", ags_channel_nth(audio->output,
												   channel->audio_channel),
								  "recall_container\0", play_container,
								  NULL);
	ags_recall_set_flags(AGS_RECALL(buffer_channel_run), (AGS_RECALL_TEMPLATE |
							      (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							      AGS_RECALL_PLAYBACK |
							      AGS_RECALL_SEQUENCER |
							      AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) buffer_channel_run, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(buffer_channel_run));

	/* iterate */
	channel = channel->next;
      }

      channel = ags_channel_nth(channel,
				audio->audio_channels - stop_audio_channel);
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
	ags_channel_add_recall_container(channel, (GObject *) recall_container);

	/* AgsBufferChannel */
	buffer_channel = (AgsBufferChannel *) g_object_new(AGS_TYPE_BUFFER_CHANNEL,
							   "soundcard\0", audio->soundcard,
							   "source\0", channel,
							   "recall_container\0", recall_container,
							   NULL);
							      
	ags_recall_set_flags(AGS_RECALL(buffer_channel), (AGS_RECALL_TEMPLATE |
							  (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							  AGS_RECALL_PLAYBACK |
							  AGS_RECALL_SEQUENCER |
							  AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) buffer_channel, FALSE);
	ags_connectable_connect(AGS_CONNECTABLE(buffer_channel));

	/* AgsBufferChannelRun */
	buffer_channel_run = (AgsBufferChannelRun *) g_object_new(AGS_TYPE_BUFFER_CHANNEL_RUN,
								  "soundcard\0", audio->soundcard,
								  "recall_channel\0", buffer_channel,
								  "source\0", channel,
								  "destination\0", ags_channel_nth(audio->output,
												   channel->audio_channel),
								  "recall_container\0", recall_container,
								  NULL);
	ags_recall_set_flags(AGS_RECALL(buffer_channel_run), (AGS_RECALL_TEMPLATE |
							      (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							      AGS_RECALL_PLAYBACK |
							      AGS_RECALL_SEQUENCER |
							      AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) buffer_channel_run, FALSE);
	ags_connectable_connect(AGS_CONNECTABLE(buffer_channel_run));

	/* iterate */
	channel = channel->next;
      }

      channel = ags_channel_nth(channel,
				audio->audio_channels - stop_audio_channel);
    }
  }
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
  AgsChannel *start, *channel;
  AgsPort *port;
  GList *list;
  guint i, j;

  if(audio == NULL){
    return(NULL);
  }

  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(audio->output,
				start_pad * audio->audio_channels);
  }else{
    start =
      channel = ags_channel_nth(audio->input,
				start_pad * audio->audio_channels);
  }

  list = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if(play_container == NULL){
      play_container = ags_recall_container_new();
    }

    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) play_container);

    delay_audio = (AgsDelayAudio *) g_object_new(AGS_TYPE_DELAY_AUDIO,
						 "soundcard\0", audio->soundcard,
						 "audio\0", audio,
						 "recall_container\0", play_container,
						 NULL);
    ags_recall_set_flags(AGS_RECALL(delay_audio), (AGS_RECALL_TEMPLATE |
						   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
						   AGS_RECALL_PLAYBACK |
						   AGS_RECALL_SEQUENCER |
						   AGS_RECALL_NOTATION));
    ags_audio_add_recall(audio, (GObject *) delay_audio, TRUE);
    ags_connectable_connect(AGS_CONNECTABLE(delay_audio));

    delay_audio_run = (AgsDelayAudioRun *) g_object_new(AGS_TYPE_DELAY_AUDIO_RUN,
							"soundcard\0", audio->soundcard,
							"recall_audio\0", delay_audio,
							"recall_container\0", play_container,
							NULL);
    ags_recall_set_flags(AGS_RECALL(delay_audio_run), (AGS_RECALL_TEMPLATE |
						       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
						       AGS_RECALL_PLAYBACK |
						       AGS_RECALL_SEQUENCER |
						       AGS_RECALL_NOTATION));
    ags_audio_add_recall(audio, (GObject *) delay_audio_run, TRUE);
    ags_connectable_connect(AGS_CONNECTABLE(delay_audio_run));
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    if(recall_container == NULL){
      recall_container = ags_recall_container_new();
    }

    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    delay_audio = (AgsDelayAudio *) g_object_new(AGS_TYPE_DELAY_AUDIO,
						 "soundcard\0", audio->soundcard,
						 "audio\0", audio,
						 "recall_container\0", recall_container,
						 NULL);
    ags_recall_set_flags(AGS_RECALL(delay_audio), (AGS_RECALL_TEMPLATE |
						   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
						   AGS_RECALL_PLAYBACK |
						   AGS_RECALL_SEQUENCER |
						   AGS_RECALL_NOTATION));
    ags_audio_add_recall(audio, (GObject *) delay_audio, FALSE);
    ags_connectable_connect(AGS_CONNECTABLE(delay_audio));

    delay_audio_run = (AgsDelayAudioRun *) g_object_new(AGS_TYPE_DELAY_AUDIO_RUN,
							"soundcard\0", audio->soundcard,
							"recall_audio\0", delay_audio,
							"recall_container\0", recall_container,
							//TODO:JK: add missing dependency "delay-audio\0"
							NULL);
    ags_recall_set_flags(AGS_RECALL(delay_audio_run), (AGS_RECALL_TEMPLATE |
						       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
						       AGS_RECALL_PLAYBACK |
						       AGS_RECALL_SEQUENCER |
						       AGS_RECALL_NOTATION));
    ags_audio_add_recall(audio, (GObject *) delay_audio_run, FALSE);
    ags_connectable_connect(AGS_CONNECTABLE(delay_audio_run));
  }
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
  AgsChannel *start, *channel;
  AgsPort *port;
  GList *list;
  guint i, j;

  if(audio == NULL){
    return(NULL);
  }

  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(audio->output,
				start_pad * audio->audio_channels);
  }else{
    start =
      channel = ags_channel_nth(audio->input,
				start_pad * audio->audio_channels);
  }

  list = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if(play_container == NULL){
      play_container = ags_recall_container_new();
    }

    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) play_container);

    count_beats_audio = (AgsCountBeatsAudio *) g_object_new(AGS_TYPE_COUNT_BEATS_AUDIO,
							    "soundcard\0", audio->soundcard,
							    "audio\0", audio,
							    "recall_container\0", play_container,
							    NULL);
    ags_recall_set_flags(AGS_RECALL(count_beats_audio), (AGS_RECALL_TEMPLATE |
							 (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							 AGS_RECALL_PLAYBACK |
							 AGS_RECALL_SEQUENCER |
							 AGS_RECALL_NOTATION));
    ags_audio_add_recall(audio, (GObject *) count_beats_audio, TRUE);
    ags_connectable_connect(AGS_CONNECTABLE(count_beats_audio));

    count_beats_audio_run = (AgsCountBeatsAudioRun *) g_object_new(AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
								   "soundcard\0", audio->soundcard,
								   "recall_audio\0", count_beats_audio,
								   "recall_container\0", play_container,
								   NULL);
    ags_recall_set_flags(AGS_RECALL(count_beats_audio_run), (AGS_RECALL_TEMPLATE |
							     (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							     AGS_RECALL_PLAYBACK |
							     AGS_RECALL_SEQUENCER |
							     AGS_RECALL_NOTATION));
    ags_audio_add_recall(audio, (GObject *) count_beats_audio_run, TRUE);
    ags_connectable_connect(AGS_CONNECTABLE(count_beats_audio_run));
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    if(recall_container == NULL){
      recall_container = ags_recall_container_new();
    }

    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    count_beats_audio = (AgsCountBeatsAudio *) g_object_new(AGS_TYPE_COUNT_BEATS_AUDIO,
							    "soundcard\0", audio->soundcard,
							    "audio\0", audio,
							    "recall_container\0", recall_container,
							    NULL);
    ags_recall_set_flags(AGS_RECALL(count_beats_audio), (AGS_RECALL_TEMPLATE |
							 (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							 AGS_RECALL_PLAYBACK |
							 AGS_RECALL_SEQUENCER |
							 AGS_RECALL_NOTATION));
    ags_audio_add_recall(audio, (GObject *) count_beats_audio, FALSE);
    ags_connectable_connect(AGS_CONNECTABLE(count_beats_audio));

    count_beats_audio_run = (AgsCountBeatsAudioRun *) g_object_new(AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
								   "soundcard\0", audio->soundcard,
								   "recall_audio\0", count_beats_audio,
								   "recall_container\0", recall_container,
								   //TODO:JK: add missing dependency "delay-audio\0"
								   NULL);
    ags_recall_set_flags(AGS_RECALL(count_beats_audio_run), (AGS_RECALL_TEMPLATE |
							     (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							     AGS_RECALL_PLAYBACK |
							     AGS_RECALL_SEQUENCER |
							     AGS_RECALL_NOTATION));
    ags_audio_add_recall(audio, (GObject *) count_beats_audio_run, FALSE);
    ags_connectable_connect(AGS_CONNECTABLE(count_beats_audio_run));
  }
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
  AgsChannel *start, *channel;
  AgsPort *port;
  GList *list;
  guint i, j;

  if(audio == NULL){
    return(NULL);
  }

  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(audio->output,
				start_pad * audio->audio_channels);
  }else{
    start =
      channel = ags_channel_nth(audio->input,
				start_pad * audio->audio_channels);
  }

  list = NULL;

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
	ags_channel_add_recall_container(channel, (GObject *) play_container);

	/* AgsLoopChannel */
	loop_channel = (AgsLoopChannel *) g_object_new(AGS_TYPE_LOOP_CHANNEL,
						       "soundcard\0", audio->soundcard,
						       "source\0", channel,
						       "recall_container\0", play_container,
						       //TODO:JK: add missing dependency "delay_audio\0"
						       NULL);
	ags_recall_set_flags(AGS_RECALL(loop_channel), (AGS_RECALL_TEMPLATE |
							(((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							AGS_RECALL_PLAYBACK |
							AGS_RECALL_SEQUENCER |
							AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) loop_channel, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(loop_channel));

	/* AgsLoopChannelRun */
	loop_channel_run = (AgsLoopChannelRun *) g_object_new(AGS_TYPE_LOOP_CHANNEL_RUN,
							      "soundcard\0", audio->soundcard,
							      "source\0", channel,
							      "recall_channel\0", loop_channel,
							      "recall_container\0", play_container,
							      //TODO:JK: add missing dependency "count_beats_audio_run\0"
							      NULL);
	ags_recall_set_flags(AGS_RECALL(loop_channel_run), (AGS_RECALL_TEMPLATE |
							    (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							    AGS_RECALL_PLAYBACK |
							    AGS_RECALL_SEQUENCER |
							    AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) loop_channel_run, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(loop_channel_run));

	/* iterate */
	channel = channel->next;
      }

      channel = ags_channel_nth(channel,
				audio->audio_channels - stop_audio_channel);
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
	ags_channel_add_recall_container(channel, (GObject *) recall_container);

	/* AgsLoopChannel */
	loop_channel = (AgsLoopChannel *) g_object_new(AGS_TYPE_LOOP_CHANNEL,
						       "soundcard\0", audio->soundcard,
						       "source\0", channel,
						       "recall_container\0", recall_container,
						       //TODO:JK: add missing dependency "delay_audio\0"
						       NULL);
	ags_recall_set_flags(AGS_RECALL(loop_channel), (AGS_RECALL_TEMPLATE |
							(((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							AGS_RECALL_PLAYBACK |
							AGS_RECALL_SEQUENCER |
							AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) loop_channel, FALSE);
	ags_connectable_connect(AGS_CONNECTABLE(loop_channel));

	/* AgsLoopChannelRun */
	loop_channel_run = (AgsLoopChannelRun *) g_object_new(AGS_TYPE_LOOP_CHANNEL_RUN,
							      "soundcard\0", audio->soundcard,
							      "source\0", channel,
							      "recall_channel\0", loop_channel,
							      "recall_container\0", recall_container,
							      //TODO:JK: add missing dependency "count_beats_audio_run\0"
							      NULL);
	ags_recall_set_flags(AGS_RECALL(loop_channel_run), (AGS_RECALL_TEMPLATE |
							    (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							    AGS_RECALL_PLAYBACK |
							    AGS_RECALL_SEQUENCER |
							    AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) loop_channel_run, FALSE);
	ags_connectable_connect(AGS_CONNECTABLE(loop_channel_run));

	/* iterate */
	channel = channel->next;
      }

      channel = ags_channel_nth(channel,
				audio->audio_channels - stop_audio_channel);
    }
  }
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
  AgsChannel *start, *channel;
  AgsPort *port;
  GList *list;
  guint i, j;

  if(audio == NULL){
    return(NULL);
  }

  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(audio->output,
				start_pad * audio->audio_channels);
  }else{
    start =
      channel = ags_channel_nth(audio->input,
				start_pad * audio->audio_channels);
  }

  list = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if((AGS_RECALL_FACTORY_REMAP & (create_flags)) == 0){
      if(play_container == NULL){
	play_container = ags_recall_container_new();
      }

      play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
      ags_audio_add_recall_container(audio, (GObject *) play_container);

      /* AgsCopyPatternAudio */
      copy_pattern_audio = (AgsCopyPatternAudio *) g_object_new(AGS_TYPE_COPY_PATTERN_AUDIO,
								"soundcard\0", audio->soundcard,
								"audio\0", audio,
								"recall_container\0", play_container,
								NULL);
      AGS_RECALL(copy_pattern_audio)->flags |= (AGS_RECALL_TEMPLATE |
						(((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
						AGS_RECALL_SEQUENCER);
      ags_audio_add_recall(audio, (GObject *) copy_pattern_audio, TRUE);

      /* AgsCopyPatternAudioRun */
      copy_pattern_audio_run = (AgsCopyPatternAudioRun *) g_object_new(AGS_TYPE_COPY_PATTERN_AUDIO_RUN,
								       "soundcard\0", audio->soundcard,
								       // "recall_audio\0", copy_pattern_audio,
								       "recall_container\0", play_container,
								       //TODO:JK: add missing dependency "count_beats_audio_run\0"
								       NULL);
      AGS_RECALL(copy_pattern_audio_run)->flags |= (AGS_RECALL_TEMPLATE |
						    (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
						    AGS_RECALL_SEQUENCER);
      ags_audio_add_recall(audio, (GObject *) copy_pattern_audio_run, TRUE);
    }else{
      GList *list;

      if(play_container == NULL){
	list = ags_recall_find_type(audio->play, AGS_TYPE_COPY_PATTERN_AUDIO);

	copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);

	play_container = AGS_RECALL_CONTAINER(AGS_RECALL(copy_pattern_audio)->container);

	list = ags_recall_find_template(play_container->recall_audio_run);
	copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(list->data);
      }else{
	copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(play_container->recall_audio);

	list = ags_recall_find_template(play_container->recall_audio_run);
	copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(list->data);
      }
    }

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	ags_channel_add_recall_container(channel, (GObject *) play_container);

	/* AgsCopyPatternChannel in channel->recall */
	copy_pattern_channel = (AgsCopyPatternChannel *) g_object_new(AGS_TYPE_COPY_PATTERN_CHANNEL,
								      "soundcard\0", AGS_SOUNDCARD(audio->soundcard),
								      "source\0", channel,
								      // "destination\0", destination,
								      "recall_container\0", play_container,
								      // "pattern\0", channel->pattern->data,
								      NULL);
	ags_recall_set_flags(AGS_RECALL(copy_pattern_channel), (AGS_RECALL_TEMPLATE |
								(((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
								AGS_RECALL_SEQUENCER));
	ags_channel_add_recall(channel, (GObject *) copy_pattern_channel, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(copy_pattern_channel));

	/* AgsCopyPatternChannelRun */
	copy_pattern_channel_run = (AgsCopyPatternChannelRun *) g_object_new(AGS_TYPE_COPY_PATTERN_CHANNEL_RUN,
									     "soundcard\0", audio->soundcard,
									     "source\0", channel,
									     // "destination\0", destination,
									     // "recall_channel\0", copy_pattern_channel,
									     // "recall_audio_run\0", copy_pattern_audio_run,
									     "recall_container\0", play_container,
									     NULL);
	ags_recall_set_flags(AGS_RECALL(copy_pattern_channel_run), (AGS_RECALL_TEMPLATE |
								    (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
								    AGS_RECALL_SEQUENCER));
	ags_channel_add_recall(channel, (GObject *) copy_pattern_channel_run, TRUE);   
	ags_connectable_connect(AGS_CONNECTABLE(copy_pattern_channel_run));

	/* iterate */
	channel = channel->next;
      }

      channel = ags_channel_nth(channel,
				audio->audio_channels - stop_audio_channel);
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

      /* AgsCopyPatternAudio */
      copy_pattern_audio = (AgsCopyPatternAudio *) g_object_new(AGS_TYPE_COPY_PATTERN_AUDIO,
								"soundcard\0", audio->soundcard,
								"audio\0", audio,
								"recall_container\0", recall_container,
								NULL);
      AGS_RECALL(copy_pattern_audio)->flags |= (AGS_RECALL_TEMPLATE |
						(((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
						AGS_RECALL_SEQUENCER);
      ags_audio_add_recall(audio, (GObject *) copy_pattern_audio, FALSE);

      /* AgsCopyPatternAudioRun */
      copy_pattern_audio_run = (AgsCopyPatternAudioRun *) g_object_new(AGS_TYPE_COPY_PATTERN_AUDIO_RUN,
								       "soundcard\0", audio->soundcard,
								       // "recall_audio\0", copy_pattern_audio,
								       "recall_container\0", recall_container,
								       //TODO:JK: add missing dependency "count_beats_audio_run\0"
								       NULL);
      AGS_RECALL(copy_pattern_audio_run)->flags |= (AGS_RECALL_TEMPLATE |
						    (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
						    AGS_RECALL_SEQUENCER);
      ags_audio_add_recall(audio, (GObject *) copy_pattern_audio_run, FALSE);
    }else{
      GList *list;

      if(recall_container == NULL){
	list = ags_recall_template_find_type(audio->recall, AGS_TYPE_COPY_PATTERN_AUDIO);

	if(list != NULL){
	  copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);
	  
	  recall_container = AGS_RECALL_CONTAINER(AGS_RECALL(copy_pattern_audio)->container);

	  list = ags_recall_find_template(recall_container->recall_audio_run);

	  if(list != NULL){
	    copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(list->data);
	  }
	}
      }else{
	copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(recall_container->recall_audio);

	list = ags_recall_find_template(recall_container->recall_audio_run);
	copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(list->data);
      }
    }

    for(i = 0; i < stop_pad - start_pad; i++){
      channel = ags_channel_nth(channel,
				start_audio_channel);
      
      for(j = 0; j < stop_audio_channel - start_audio_channel; j++){
	ags_channel_add_recall_container(channel, (GObject *) recall_container);

	/* AgsCopyPatternChannel in channel->recall */
	copy_pattern_channel = (AgsCopyPatternChannel *) g_object_new(AGS_TYPE_COPY_PATTERN_CHANNEL,
								      "soundcard\0", AGS_SOUNDCARD(audio->soundcard),
								      "source\0", channel,
								      // "destination\0", destination,
								      "recall_container\0", recall_container,
								      //"pattern\0", channel->pattern->data,
								      NULL);
	ags_recall_set_flags(AGS_RECALL(copy_pattern_channel), (AGS_RECALL_TEMPLATE |
								(((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
								AGS_RECALL_SEQUENCER));
	ags_channel_add_recall(channel, (GObject *) copy_pattern_channel, FALSE);
	ags_connectable_connect(AGS_CONNECTABLE(copy_pattern_channel));

	/* AgsCopyPatternChannelRun */
	copy_pattern_channel_run = (AgsCopyPatternChannelRun *) g_object_new(AGS_TYPE_COPY_PATTERN_CHANNEL_RUN,
									     "soundcard\0", audio->soundcard,
									     "source\0", channel,
									     // "destination\0", destination,
									     // "recall_channel\0", copy_pattern_channel,
									     // "recall_audio_run\0", copy_pattern_audio_run,
									     "recall_container\0", recall_container,
									     NULL);
	ags_recall_set_flags(AGS_RECALL(copy_pattern_channel_run), (AGS_RECALL_TEMPLATE |
								    (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
								    AGS_RECALL_SEQUENCER));
	ags_channel_add_recall(channel, (GObject *) copy_pattern_channel_run, FALSE);   
	ags_connectable_connect(AGS_CONNECTABLE(copy_pattern_channel_run));

	/* iterate */
	channel = channel->next;
      }

      channel = ags_channel_nth(channel,
				audio->audio_channels - stop_audio_channel);
    }
  }
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
  AgsChannel *start, *channel;
  AgsPort *port;
  GList *list;
  guint i, j;

  if(audio == NULL){
    return(NULL);
  }

  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(audio->output,
				start_pad * audio->audio_channels);
  }else{
    start =
      channel = ags_channel_nth(audio->input,
				start_pad * audio->audio_channels);
  }

  list = NULL;

  /* play */
  if((AGS_RECALL_FACTORY_PLAY & (create_flags)) != 0){
    if(play_container == NULL){
      play_container = ags_recall_container_new();
    }

    play_container->flags |= AGS_RECALL_CONTAINER_PLAY;
    ags_audio_add_recall_container(audio, (GObject *) play_container);

    play_notation_audio = (AgsPlayNotationAudio *) g_object_new(AGS_TYPE_PLAY_NOTATION_AUDIO,
								"soundcard\0", audio->soundcard,
								"audio\0", audio,
								"recall_container\0", play_container,
								NULL);
    ags_recall_set_flags(AGS_RECALL(play_notation_audio), (AGS_RECALL_TEMPLATE |
							   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							   AGS_RECALL_PLAYBACK |
							   AGS_RECALL_SEQUENCER |
							   AGS_RECALL_NOTATION));
    ags_audio_add_recall(audio, (GObject *) play_notation_audio, TRUE);
    ags_connectable_connect(AGS_CONNECTABLE(play_notation_audio));

    play_notation_audio_run = (AgsPlayNotationAudioRun *) g_object_new(AGS_TYPE_PLAY_NOTATION_AUDIO_RUN,
								       "soundcard\0", audio->soundcard,
								       "recall_audio\0", play_notation_audio,
								       "recall_container\0", play_container,
								       NULL);
    ags_recall_set_flags(AGS_RECALL(play_notation_audio_run), (AGS_RECALL_TEMPLATE |
							       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							       AGS_RECALL_PLAYBACK |
							       AGS_RECALL_SEQUENCER |
							       AGS_RECALL_NOTATION));
    ags_audio_add_recall(audio, (GObject *) play_notation_audio_run, TRUE);
    ags_connectable_connect(AGS_CONNECTABLE(play_notation_audio_run));
  }

  /* recall */
  if((AGS_RECALL_FACTORY_RECALL & (create_flags)) != 0){
    if(recall_container == NULL){
      recall_container = ags_recall_container_new();
    }

    ags_audio_add_recall_container(audio, (GObject *) recall_container);

    play_notation_audio = (AgsPlayNotationAudio *) g_object_new(AGS_TYPE_PLAY_NOTATION_AUDIO,
								"soundcard\0", audio->soundcard,
								"audio\0", audio,
								"recall_container\0", recall_container,
								NULL);
    ags_recall_set_flags(AGS_RECALL(play_notation_audio), (AGS_RECALL_TEMPLATE |
							   (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							   AGS_RECALL_PLAYBACK |
							   AGS_RECALL_SEQUENCER |
							   AGS_RECALL_NOTATION));
    ags_audio_add_recall(audio, (GObject *) play_notation_audio, FALSE);
    ags_connectable_connect(AGS_CONNECTABLE(play_notation_audio));

    play_notation_audio_run = (AgsPlayNotationAudioRun *) g_object_new(AGS_TYPE_PLAY_NOTATION_AUDIO_RUN,
								       "soundcard\0", audio->soundcard,
								       "recall_audio\0", play_notation_audio,
								       "recall_container\0", recall_container,
								       //TODO:JK: add missing dependency "delay-audio\0"
								       NULL);
    ags_recall_set_flags(AGS_RECALL(play_notation_audio_run), (AGS_RECALL_TEMPLATE |
							       (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							       AGS_RECALL_PLAYBACK |
							       AGS_RECALL_SEQUENCER |
							       AGS_RECALL_NOTATION));
    ags_audio_add_recall(audio, (GObject *) play_notation_audio_run, FALSE);
    ags_connectable_connect(AGS_CONNECTABLE(play_notation_audio_run));
  }
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
  AgsChannel *start, *channel;
  AgsPort *port;
  GList *list;
  guint i, j;
  
  if(audio == NULL){
    return(NULL);
  }

  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(audio->output,
				start_pad * audio->audio_channels);
  }else{
    start =
      channel = ags_channel_nth(audio->input,
				start_pad * audio->audio_channels);
  }

  list = NULL;

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
	ags_channel_add_recall_container(channel, (GObject *) play_container);

	/* AgsPeakChannel */
	peak_channel = (AgsPeakChannel *) g_object_new(AGS_TYPE_PEAK_CHANNEL,
						       "soundcard\0", audio->soundcard,
						       "source\0", channel,
						       "recall_container\0", play_container,
						       NULL);
							      
	ags_recall_set_flags(AGS_RECALL(peak_channel), (AGS_RECALL_TEMPLATE |
							(((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							AGS_RECALL_PLAYBACK |
							AGS_RECALL_SEQUENCER |
							AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) peak_channel, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(peak_channel));


	/* AgsPeakChannelRun */
	peak_channel_run = (AgsPeakChannelRun *) g_object_new(AGS_TYPE_PEAK_CHANNEL_RUN,
							      "soundcard\0", audio->soundcard,
							      "recall-channel\0", peak_channel,
							      "source\0", channel,
							      "recall_container\0", recall_container,
							      NULL);
	ags_recall_set_flags(AGS_RECALL(peak_channel_run), (AGS_RECALL_TEMPLATE |
							    (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							    AGS_RECALL_PLAYBACK |
							    AGS_RECALL_SEQUENCER |
							    AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) peak_channel_run, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(peak_channel_run));

	/* iterate */
	channel = channel->next;
      }

      channel = ags_channel_nth(channel,
				audio->audio_channels - stop_audio_channel);
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
	ags_channel_add_recall_container(channel, (GObject *) recall_container);

	/* AgsPeakChannel */
	peak_channel = (AgsPeakChannel *) g_object_new(AGS_TYPE_PEAK_CHANNEL,
						       "soundcard\0", audio->soundcard,
						       "source\0", channel,
						       "recall_container\0", recall_container,
						       NULL);
							      
	ags_recall_set_flags(AGS_RECALL(peak_channel), (AGS_RECALL_TEMPLATE |
							(((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							AGS_RECALL_PLAYBACK |
							AGS_RECALL_SEQUENCER |
							AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) peak_channel, FALSE);
	ags_connectable_connect(AGS_CONNECTABLE(peak_channel));

	/* AgsPeakChannelRun */
	peak_channel_run = (AgsPeakChannelRun *) g_object_new(AGS_TYPE_PEAK_CHANNEL_RUN,
							      "soundcard\0", audio->soundcard,
							      "recall-channel\0", peak_channel,
							      "source\0", channel,
							      "recall_container\0", recall_container,
							      NULL);
	ags_recall_set_flags(AGS_RECALL(peak_channel_run), (AGS_RECALL_TEMPLATE |
							    (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							    AGS_RECALL_PLAYBACK |
							    AGS_RECALL_SEQUENCER |
							    AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) peak_channel_run, FALSE);
	ags_connectable_connect(AGS_CONNECTABLE(peak_channel_run));

	/* iterate */
	channel = channel->next;
      }

      channel = ags_channel_nth(channel,
				audio->audio_channels - stop_audio_channel);
    }
  }
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
  AgsChannel *start, *channel;
  AgsPort *port;
  GList *list;
  guint i, j;
  
  if(audio == NULL){
    return(NULL);
  }

  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(audio->output,
				start_pad * audio->audio_channels);
  }else{
    start =
      channel = ags_channel_nth(audio->input,
				start_pad * audio->audio_channels);
  }

  list = NULL;

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
	ags_channel_add_recall_container(channel, (GObject *) play_container);

	/* AgsVolumeChannel */
	volume_channel = (AgsVolumeChannel *) g_object_new(AGS_TYPE_VOLUME_CHANNEL,
							   "soundcard\0", audio->soundcard,
							   "source\0", channel,
							   "recall_container\0", play_container,
							   NULL);
							      
	ags_recall_set_flags(AGS_RECALL(volume_channel), (AGS_RECALL_TEMPLATE |
							  (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							  AGS_RECALL_PLAYBACK |
							  AGS_RECALL_SEQUENCER |
							  AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) volume_channel, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(volume_channel));

	/* AgsVolumeChannelRun */
	volume_channel_run = (AgsVolumeChannelRun *) g_object_new(AGS_TYPE_VOLUME_CHANNEL_RUN,
								  "soundcard\0", audio->soundcard,
								  "recall-channel\0", volume_channel,
								  "source\0", channel,
								  "recall_container\0", play_container,
								  NULL);
	ags_recall_set_flags(AGS_RECALL(volume_channel_run), (AGS_RECALL_TEMPLATE |
							      (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							      AGS_RECALL_PLAYBACK |
							      AGS_RECALL_SEQUENCER |
							      AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) volume_channel_run, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(volume_channel_run));

	/* iterate */
	channel = channel->next;
      }

      channel = ags_channel_nth(channel,
				audio->audio_channels - stop_audio_channel);
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
	ags_channel_add_recall_container(channel, (GObject *) recall_container);

	/* AgsVolumeChannel */
	volume_channel = (AgsVolumeChannel *) g_object_new(AGS_TYPE_VOLUME_CHANNEL,
							   "soundcard\0", audio->soundcard,
							   "source\0", channel,
							   "recall_container\0", recall_container,
							   NULL);
							      
	ags_recall_set_flags(AGS_RECALL(volume_channel), (AGS_RECALL_TEMPLATE |
							  (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							  AGS_RECALL_PLAYBACK |
							  AGS_RECALL_SEQUENCER |
							  AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) volume_channel, FALSE);
	ags_connectable_connect(AGS_CONNECTABLE(volume_channel));

	/* AgsVolumeChannelRun */
	volume_channel_run = (AgsVolumeChannelRun *) g_object_new(AGS_TYPE_VOLUME_CHANNEL_RUN,
								  "soundcard\0", audio->soundcard,
								  "recall_channel\0", volume_channel,
								  "source\0", channel,
								  "recall_container\0", recall_container,
								  NULL);
	ags_recall_set_flags(AGS_RECALL(volume_channel_run), (AGS_RECALL_TEMPLATE |
							      (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							      AGS_RECALL_PLAYBACK |
							      AGS_RECALL_SEQUENCER |
							      AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) volume_channel_run, FALSE);
	ags_connectable_connect(AGS_CONNECTABLE(volume_channel_run));

	/* iterate */
	channel = channel->next;
      }

      channel = ags_channel_nth(channel,
				audio->audio_channels - stop_audio_channel);
    }
  }
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
  AgsRecallChannelRunDummy *recall_channel_run_dummy;
  AgsChannel *start, *channel;
  AgsPort *port;
  GList *list;
  guint i, j;
  
  if(audio == NULL){
    return(NULL);
  }

  if((AGS_RECALL_FACTORY_OUTPUT & (create_flags)) != 0){
    start =
      channel = ags_channel_nth(audio->output,
				start_pad * audio->audio_channels);
  }else{
    start =
      channel = ags_channel_nth(audio->input,
				start_pad * audio->audio_channels);
  }

  list = NULL;

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
	ags_channel_add_recall_container(channel, (GObject *) play_container);

	/* AgsRecallLadspa */
	recall_ladspa = (AgsRecallLadspa *) g_object_new(AGS_TYPE_RECALL_LADSPA,
							 "soundcard\0", audio->soundcard,
							 "source\0", channel,
							 "recall_container\0", play_container,
							 NULL);
							      
	ags_recall_set_flags(AGS_RECALL(recall_ladspa), (AGS_RECALL_TEMPLATE |
							 (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							 AGS_RECALL_PLAYBACK |
							 AGS_RECALL_SEQUENCER |
							 AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) recall_ladspa, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(recall_ladspa));

	/* AgsRecallChannelRunDummy */
	recall_channel_run_dummy = ags_recall_channel_run_dummy_new(channel,
								    AGS_TYPE_RECALL_RECYCLING_DUMMY,
								    AGS_TYPE_RECALL_LADSPA_RUN);

	g_object_set(recall_channel_run_dummy,
		     "soundcard\0", audio->soundcard,
		     // "recall_channel\0", recall_ladspa,
		     "source\0", channel,
		     "recall_container\0", play_container,
		     NULL);
	ags_recall_set_flags(AGS_RECALL(recall_channel_run_dummy), (AGS_RECALL_TEMPLATE |
								    (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
								    AGS_RECALL_PLAYBACK |
								    AGS_RECALL_SEQUENCER |
								    AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) recall_channel_run_dummy, TRUE);
	ags_connectable_connect(AGS_CONNECTABLE(recall_channel_run_dummy));

	/* iterate */
	channel = channel->next;
      }

      channel = ags_channel_nth(channel,
				audio->audio_channels - stop_audio_channel);
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
	ags_channel_add_recall_container(channel, (GObject *) recall_container);

	/* AgsRecallLadspa */
	recall_ladspa = (AgsRecallLadspa *) g_object_new(AGS_TYPE_RECALL_LADSPA,
							 "soundcard\0", audio->soundcard,
							 "source\0", channel,
							 "recall_container\0", recall_container,
							 NULL);
							      
	ags_recall_set_flags(AGS_RECALL(recall_ladspa), (AGS_RECALL_TEMPLATE |
							 (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
							 AGS_RECALL_PLAYBACK |
							 AGS_RECALL_SEQUENCER |
							 AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) recall_ladspa, FALSE);
	ags_connectable_connect(AGS_CONNECTABLE(recall_ladspa));

	/* AgsRecallChannelRunDummy */
	recall_channel_run_dummy = ags_recall_channel_run_dummy_new(channel,
								    AGS_TYPE_RECALL_RECYCLING_DUMMY,
								    AGS_TYPE_RECALL_LADSPA_RUN);
	g_object_set(recall_channel_run_dummy,
		     "soundcard\0", audio->soundcard,
		     // "recall_channel\0", recall_ladspa,
		     "source\0", channel,
		     "recall_container\0", recall_container,
		     NULL);
	ags_recall_set_flags(AGS_RECALL(recall_channel_run_dummy), (AGS_RECALL_TEMPLATE |
								    (((AGS_RECALL_FACTORY_OUTPUT & create_flags) != 0) ? AGS_RECALL_OUTPUT_ORIENTATED: AGS_RECALL_INPUT_ORIENTATED) |
								    AGS_RECALL_PLAYBACK |
								    AGS_RECALL_SEQUENCER |
								    AGS_RECALL_NOTATION));
	ags_channel_add_recall(channel, (GObject *) recall_channel_run_dummy, FALSE);
	ags_connectable_connect(AGS_CONNECTABLE(recall_channel_run_dummy));

	/* iterate */
	channel = channel->next;
      }

      channel = ags_channel_nth(channel,
				audio->audio_channels - stop_audio_channel);
    }
  }
}

/**
 * ags_recall_factory_create:
 * @audio: an #AgsAudio that should keep the recalls
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
 * Since: 0.4
 */
GList*
ags_recall_factory_create(AgsAudio *audio,
			  AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
			  gchar *plugin_name,
			  guint start_audio_channel, guint stop_audio_channel,
			  guint start_pad, guint stop_pad,
			  guint create_flags, guint recall_flags)
{
  GList *list;

  list = NULL;

#ifdef AGS_DEBUG
  g_message("AgsRecallFactory creating: %s[%d,%d]\0", plugin_name, stop_pad, stop_audio_channel);
#endif

  if(!strncmp(plugin_name,
	      "ags-delay\0",
	      10)){
    ags_recall_factory_create_delay(audio,
				    play_container, recall_container,
				    plugin_name,
				    start_audio_channel, stop_audio_channel,
				    start_pad, stop_pad,
				    create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-count-beats\0",
		    16)){
    ags_recall_factory_create_count_beats(audio,
					  play_container, recall_container,
					  plugin_name,
					  start_audio_channel, stop_audio_channel,
					  start_pad, stop_pad,
					  create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-stream\0",
		    11)){
    ags_recall_factory_create_stream(audio,
				     play_container, recall_container,
				     plugin_name,
				     start_audio_channel, stop_audio_channel,
				     start_pad, stop_pad,
				     create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-loop\0",
		    9)){
    ags_recall_factory_create_loop(audio,
				   play_container, recall_container,
				   plugin_name,
				   start_audio_channel, stop_audio_channel,
				   start_pad, stop_pad,
				   create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-play-master\0",
		    16)){
    ags_recall_factory_create_play_master(audio,
					  play_container, recall_container,
					  plugin_name,
					  start_audio_channel, stop_audio_channel,
					  start_pad, stop_pad,
					  create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-copy\0",
		    9)){
    ags_recall_factory_create_copy(audio,
				   play_container, recall_container,
				   plugin_name,
				   start_audio_channel, stop_audio_channel,
				   start_pad, stop_pad,
				   create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-buffer\0",
		    11)){
    ags_recall_factory_create_buffer(audio,
				     play_container, recall_container,
				     plugin_name,
				     start_audio_channel, stop_audio_channel,
				     start_pad, stop_pad,
				     create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-play\0",
		    9)){
    ags_recall_factory_create_play(audio,
				   play_container, recall_container,
				   plugin_name,
				   start_audio_channel, stop_audio_channel,
				   start_pad, stop_pad,
				   create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-copy-pattern\0",
		    17)){
    ags_recall_factory_create_copy_pattern(audio,
					   play_container, recall_container,
					   plugin_name,
					   start_audio_channel, stop_audio_channel,
					   start_pad, stop_pad,
					   create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-play-notation\0",
		    18)){
    ags_recall_factory_create_play_notation(audio,
					    play_container, recall_container,
					    plugin_name,
					    start_audio_channel, stop_audio_channel,
					    start_pad, stop_pad,
					    create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-peak\0",
		    9)){
    ags_recall_factory_create_peak(audio,
				   play_container, recall_container,
				   plugin_name,
				   start_audio_channel, stop_audio_channel,
				   start_pad, stop_pad,
				   create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-volume\0",
		    11)){
    ags_recall_factory_create_volume(audio,
				     play_container, recall_container,
				     plugin_name,
				     start_audio_channel, stop_audio_channel,
				     start_pad, stop_pad,
				     create_flags, recall_flags);
  }else if(!strncmp(plugin_name,
		    "ags-ladspa\0",
		    11)){
    ags_recall_factory_create_ladspa(audio,
				     play_container, recall_container,
				     plugin_name,
				     start_audio_channel, stop_audio_channel,
				     start_pad, stop_pad,
				     create_flags, recall_flags);
  }

  return(list);
}

void
ags_recall_factory_remove(AgsAudio *audio,
			  AgsRecallContainer *recall_container)
{
  //TODO:JK: implement me
}

/**
 * ags_recall_factory_new:
 * @factory: the #AgsRecall depending on
 *
 * Creates a #AgsRecallFactory
 *
 * Returns: a new #AgsRecallFactory
 * 
 * Since: 0.4
 */
AgsRecallFactory*
ags_recall_factory_new()
{
  AgsRecallFactory *recall_factory;

  recall_factory = (AgsRecallFactory *) g_object_new(AGS_TYPE_RECALL_FACTORY,
						     NULL);

  return(recall_factory);
}
