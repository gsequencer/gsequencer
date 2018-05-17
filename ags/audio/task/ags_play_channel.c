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

#include <ags/audio/task/ags_play_channel.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_audio_thread.h>
#include <ags/audio/thread/ags_channel_thread.h>

#include <ags/i18n.h>

void ags_play_channel_class_init(AgsPlayChannelClass *play_channel);
void ags_play_channel_init(AgsPlayChannel *play_channel);
void ags_play_channel_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_play_channel_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_play_channel_dispose(GObject *gobject);
void ags_play_channel_finalize(GObject *gobject);

void ags_play_channel_launch(AgsTask *task);

/**
 * SECTION:ags_play_channel
 * @short_description: play channel object to audio loop
 * @title: AgsPlayChannel
 * @section_id:
 * @include: ags/audio/task/ags_play_channel.h
 *
 * The #AgsPlayChannel task plays #AgsChannel to #AgsAudioLoop.
 */

static gpointer ags_play_channel_parent_class = NULL;

enum{
  PROP_0,
  PROP_AUDIO_LOOP,
  PROP_CHANNEL,
};

GType
ags_play_channel_get_type()
{
  static GType ags_type_play_channel = 0;

  if(!ags_type_play_channel){
    static const GTypeInfo ags_play_channel_info = {
      sizeof (AgsPlayChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_channel_init,
    };

    ags_type_play_channel = g_type_register_static(AGS_TYPE_TASK,
						   "AgsPlayChannel",
						   &ags_play_channel_info,
						   0);
  }
  
  return(ags_type_play_channel);
}

void
ags_play_channel_class_init(AgsPlayChannelClass *play_channel)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;
  
  ags_play_channel_parent_class = g_type_class_peek_parent(play_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) play_channel;

  gobject->set_property = ags_play_channel_set_property;
  gobject->get_property = ags_play_channel_get_property;

  gobject->dispose = ags_play_channel_dispose;
  gobject->finalize = ags_play_channel_finalize;

  /* properties */
  /**
   * AgsPlayChannel:channel:
   *
   * The assigned #AgsChannel
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("channel",
				   i18n_pspec("channel of play channel"),
				   i18n_pspec("The channel of play channel task"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /* AgsTaskClass */
  task = (AgsTaskClass *) play_channel;

  task->launch = ags_play_channel_launch;
}

void
ags_play_channel_init(AgsPlayChannel *play_channel)
{
  play_channel->channel = NULL;
}

void
ags_play_channel_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsPlayChannel *play_channel;

  play_channel = AGS_PLAY_CHANNEL(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      if(play_channel->channel == (GObject *) channel){
	return;
      }

      if(play_channel->channel != NULL){
	g_object_unref(play_channel->channel);
      }

      if(channel != NULL){
	g_object_ref(channel);
      }

      play_channel->channel = (GObject *) channel;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_channel_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsPlayChannel *play_channel;

  play_channel = AGS_PLAY_CHANNEL(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      g_value_set_object(value, play_channel->channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_channel_dispose(GObject *gobject)
{
  AgsPlayChannel *play_channel;

  play_channel = AGS_PLAY_CHANNEL(gobject);

  if(play_channel->channel != NULL){
    g_object_unref(play_channel->channel);

    play_channel->channel = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_play_channel_parent_class)->dispose(gobject);
}

void
ags_play_channel_finalize(GObject *gobject)
{
  AgsPlayChannel *play_channel;

  play_channel = AGS_PLAY_CHANNEL(gobject);

  if(play_channel->channel != NULL){
    g_object_unref(play_channel->channel);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_play_channel_parent_class)->finalize(gobject);
}

void
ags_play_channel_launch(AgsTask *task)
{
  AgsChannel *channel;
  AgsPlaybackDomain *playback_domain;
  AgsPlayback *playback;
  
  AgsPlayChannel *play_channel;

  AgsAudioLoop *audio_loop;

  AgsApplicationContext *application_context;

  gint sound_scope;
  static const guint staging_flags = (AGS_SOUND_STAGING_CHECK_RT_DATA |
				      AGS_SOUND_STAGING_RUN_INIT_PRE |
				      AGS_SOUND_STAGING_RUN_INIT_INTER |
				      AGS_SOUND_STAGING_RUN_INIT_POST);

  play_channel = AGS_PLAY_CHANNEL(task);

  channel = (AgsChannel *) play_channel->channel;

  g_object_get(channel,
	       "playback", &playback,
	       NULL);

  g_object_get(playback,
	       "playback-domain", &playback_domain,
	       NULL);
  
  sound_scope = play_audio->sound_scope;

  application_context = ags_application_context_get_instance();

  g_object_get(application_context,
	       "main-loop", &audio_loop,
	       NULL);


  /* add channel to AgsAudioLoop */
  ags_audio_loop_add_channel(audio_loop,
			     (GObject *) channel);


  if(sound_scope >= 0){
    /* get some fields */
    g_object_get(channel,
		 "first-recycling", &recycling,
		 NULL);

    /* create recall id and recycling context */
    recall_id = ags_recall_id_new();

    recycling_context = ags_recycling_context_new(1);
    ags_recycling_context_replace(recycling_context,
				  recycling,
				  0);

    g_object_set(recycling_context,
		 "recall-id", recall_id,
		 NULL);
      
    g_object_set(recall_id,
		 "recycling-context", recycling_context,
		 NULL);
      
    ags_playback_set_recall_id(playback,
			       recall_id,
			       sound_scope);

    /* add to start queue */
    ags_thread_add_start_queue(ags_playback_domain_get_audio_thread(playback_domain,
								    sound_scope),
			       ags_playback_get_channel_thread(playback,
							       sound_scope));
    
    /* play init */
    ags_channel_recursive_run_stage(channel,
				    sound_scope, staging_flags);

    /* add to start queue */
    ags_thread_add_start_queue(audio_loop,
			       ags_playback_domain_get_audio_thread(playback_domain,
								    sound_scope));
  }else{
    gint i;

    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){      
      list = list_start;

      /* get some fields */
      g_object_get(channel,
		   "first-recycling", &recycling,
		   NULL);

      /* create recall id and recycling context */
      recall_id = ags_recall_id_new();

      recycling_context = ags_recycling_context_new(1);
      ags_recycling_context_replace(recycling_context,
				    recycling,
				    0);

      g_object_set(recycling_context,
		   "recall-id", recall_id,
		   NULL);
      
      g_object_set(recall_id,
		   "recycling-context", recycling_context,
		   NULL);
	
      ags_playback_set_recall_id(playback,
				 recall_id,
				 i);

      /* add to start queue */
      ags_thread_add_start_queue(ags_playback_domain_get_audio_thread(playback_domain,
								      i),
				 ags_playback_get_channel_thread(playback,
								 i));

      /* play init */
      ags_channel_recursive_run_stage(channel,
				      i, staging_flags);

      /* add to start queue */
      ags_thread_add_start_queue(audio_loop,
				 ags_playback_domain_get_audio_thread(playback_domain,
								      i));
    }
  }
}

/**
 * ags_play_channel_new:
 * @channel: the #AgsChannel to play
 *
 * Create a new instance of #AgsPlayChannel.
 *
 * Returns: the new #AgsPlayChannel.
 *
 * Since: 2.0.0
 */
AgsPlayChannel*
ags_play_channel_new(AgsChannel *channel)
{
  AgsPlayChannel *play_channel;

  play_channel = (AgsPlayChannel *) g_object_new(AGS_TYPE_PLAY_CHANNEL,
						 "channel", channel,
						 NULL);

  return(play_channel);
}
