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

#include <ags/audio/task/ags_start_channel.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_audio_thread.h>
#include <ags/audio/thread/ags_channel_thread.h>

#include <ags/i18n.h>

void ags_start_channel_class_init(AgsStartChannelClass *start_channel);
void ags_start_channel_init(AgsStartChannel *start_channel);
void ags_start_channel_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_start_channel_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_start_channel_dispose(GObject *gobject);
void ags_start_channel_finalize(GObject *gobject);

void ags_start_channel_launch(AgsTask *task);

/**
 * SECTION:ags_start_channel
 * @short_description: start channel object to audio loop
 * @title: AgsStartChannel
 * @section_id:
 * @include: ags/audio/task/ags_start_channel.h
 *
 * The #AgsStartChannel task starts #AgsChannel to #AgsAudioLoop.
 */

static gpointer ags_start_channel_parent_class = NULL;

enum{
  PROP_0,
  PROP_CHANNEL,
  PROP_SOUND_SCOPE,
};

GType
ags_start_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_start_channel = 0;

    static const GTypeInfo ags_start_channel_info = {
      sizeof (AgsStartChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_start_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsStartChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_start_channel_init,
    };

    ags_type_start_channel = g_type_register_static(AGS_TYPE_TASK,
						    "AgsStartChannel",
						    &ags_start_channel_info,
						    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_start_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_start_channel_class_init(AgsStartChannelClass *start_channel)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;
  
  ags_start_channel_parent_class = g_type_class_peek_parent(start_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) start_channel;

  gobject->set_property = ags_start_channel_set_property;
  gobject->get_property = ags_start_channel_get_property;

  gobject->dispose = ags_start_channel_dispose;
  gobject->finalize = ags_start_channel_finalize;

  /* properties */
  /**
   * AgsStartChannel:channel:
   *
   * The assigned #AgsChannel
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("channel",
				   i18n_pspec("channel of start channel"),
				   i18n_pspec("The channel of start channel task"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /**
   * AgsStartChannel:sound-scope:
   *
   * The effects sound-scope.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_int("sound-scope",
				 i18n_pspec("sound scope"),
				 i18n_pspec("The sound scope"),
				 -1,
				 AGS_SOUND_SCOPE_LAST,
				 -1,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUND_SCOPE,
				  param_spec);

  /* AgsTaskClass */
  task = (AgsTaskClass *) start_channel;

  task->launch = ags_start_channel_launch;
}

void
ags_start_channel_init(AgsStartChannel *start_channel)
{
  start_channel->channel = NULL;

  start_channel->sound_scope = -1;
}

void
ags_start_channel_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsStartChannel *start_channel;

  start_channel = AGS_START_CHANNEL(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      if(start_channel->channel == (GObject *) channel){
	return;
      }

      if(start_channel->channel != NULL){
	g_object_unref(start_channel->channel);
      }

      if(channel != NULL){
	g_object_ref(channel);
      }

      start_channel->channel = (GObject *) channel;
    }
    break;
  case PROP_SOUND_SCOPE:
    {
      start_channel->sound_scope = g_value_get_int(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_start_channel_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsStartChannel *start_channel;

  start_channel = AGS_START_CHANNEL(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      g_value_set_object(value, start_channel->channel);
    }
    break;
  case PROP_SOUND_SCOPE:
    {
      g_value_set_int(value, start_channel->sound_scope);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_start_channel_dispose(GObject *gobject)
{
  AgsStartChannel *start_channel;

  start_channel = AGS_START_CHANNEL(gobject);

  if(start_channel->channel != NULL){
    g_object_unref(start_channel->channel);

    start_channel->channel = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_start_channel_parent_class)->dispose(gobject);
}

void
ags_start_channel_finalize(GObject *gobject)
{
  AgsStartChannel *start_channel;

  start_channel = AGS_START_CHANNEL(gobject);

  if(start_channel->channel != NULL){
    g_object_unref(start_channel->channel);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_start_channel_parent_class)->finalize(gobject);
}

void
ags_start_channel_launch(AgsTask *task)
{
  AgsAudio *audio;
  AgsChannel *channel;
  AgsRecycling *recycling;
  AgsPlaybackDomain *playback_domain;
  AgsPlayback *playback;
  AgsRecallID *audio_recall_id, *channel_recall_id;
  AgsRecyclingContext *recycling_context;
  
  AgsStartChannel *start_channel;

  AgsAudioLoop *audio_loop;

  AgsApplicationContext *application_context;

  GList *list_start, *list;
  
  gint sound_scope;
  static const guint staging_flags = (AGS_SOUND_STAGING_CHECK_RT_DATA |
				      AGS_SOUND_STAGING_RUN_INIT_PRE |
				      AGS_SOUND_STAGING_RUN_INIT_INTER |
				      AGS_SOUND_STAGING_RUN_INIT_POST);

  start_channel = AGS_START_CHANNEL(task);

  channel = (AgsChannel *) start_channel->channel;

  g_object_get(channel,
	       "audio", &audio,
	       "playback", &playback,
	       NULL);

  g_object_get(playback,
	       "playback-domain", &playback_domain,
	       NULL);
  
  sound_scope = start_channel->sound_scope;

  application_context = ags_application_context_get_instance();

  g_object_get(application_context,
	       "main-loop", &audio_loop,
	       NULL);

  /* add channel to AgsAudioLoop */
  ags_audio_loop_add_channel(audio_loop,
			     (GObject *) channel);

  /* flag to playback channel */
  audio_loop->flags |= AGS_AUDIO_LOOP_PLAY_CHANNEL;

  if(sound_scope >= 0){
    /* get some fields */
    g_object_get(channel,
		 "first-recycling", &recycling,
		 NULL);

    /* create recall id and recycling context */
    audio_recall_id = ags_recall_id_new();
    ags_recall_id_set_sound_scope(audio_recall_id, sound_scope);
    ags_audio_add_recall_id(audio,
			    audio_recall_id);
    
    channel_recall_id = ags_recall_id_new();
    ags_recall_id_set_sound_scope(channel_recall_id, sound_scope);
    ags_channel_add_recall_id(channel,
			      channel_recall_id);
    
    recycling_context = ags_recycling_context_new(1);
    ags_recycling_context_replace(recycling_context,
				  recycling,
				  0);

    g_object_set(recycling_context,
		 "recall-id", audio_recall_id,
		 NULL);
      
    g_object_set(audio_recall_id,
		 "recycling-context", recycling_context,
		 NULL);

    g_object_set(channel_recall_id,
		 "recycling-context", recycling_context,
		 NULL);
      
    ags_playback_set_recall_id(playback,
			       channel_recall_id,
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
      audio_recall_id = ags_recall_id_new();
      ags_recall_id_set_sound_scope(audio_recall_id, sound_scope);
      ags_audio_add_recall_id(audio,
			      audio_recall_id);

      channel_recall_id = ags_recall_id_new();
      ags_recall_id_set_sound_scope(channel_recall_id, sound_scope);
      ags_channel_add_recall_id(channel,
				channel_recall_id);
    
      recycling_context = ags_recycling_context_new(1);
      ags_recycling_context_replace(recycling_context,
				    recycling,
				    0);

      g_object_set(recycling_context,
		   "recall-id", audio_recall_id,
		   NULL);
      
      g_object_set(audio_recall_id,
		   "recycling-context", recycling_context,
		   NULL);

      g_object_set(channel_recall_id,
		   "recycling-context", recycling_context,
		   NULL);
	
      ags_playback_set_recall_id(playback,
				 channel_recall_id,
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
 * ags_start_channel_new:
 * @channel: the #AgsChannel to start
 *
 * Create a new instance of #AgsStartChannel.
 *
 * Returns: the new #AgsStartChannel.
 *
 * Since: 2.0.0
 */
AgsStartChannel*
ags_start_channel_new(AgsChannel *channel,
		      gint sound_scope)
{
  AgsStartChannel *start_channel;

  start_channel = (AgsStartChannel *) g_object_new(AGS_TYPE_START_CHANNEL,
						   "channel", channel,
						   "sound-scope", sound_scope,
						   NULL);

  return(start_channel);
}
