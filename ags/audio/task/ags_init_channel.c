/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/audio/task/ags_init_channel.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>

void ags_init_channel_class_init(AgsInitChannelClass *init_channel);
void ags_init_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_init_channel_init(AgsInitChannel *init_channel);
void ags_init_channel_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_init_channel_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_init_channel_connect(AgsConnectable *connectable);
void ags_init_channel_disconnect(AgsConnectable *connectable);
void ags_init_channel_finalize(GObject *gobject);

void ags_init_channel_launch(AgsTask *task);

/**
 * SECTION:ags_init_channel
 * @short_description: init channel task
 * @title: AgsInitChannel
 * @section_id:
 * @include: ags/audio/task/ags_init_channel.h
 *
 * The #AgsInitChannel task inits #AgsChannel.
 */

static gpointer ags_init_channel_parent_class = NULL;
static AgsConnectableInterface *ags_init_channel_parent_connectable_interface;

enum{
  PROP_0,
  PROP_CHANNEL,
  PROP_PLAY_PAD,
  PROP_DO_PLAYBACK,
  PROP_DO_SEQUENCER,
  PROP_DO_NOTATION,
};

GType
ags_init_channel_get_type()
{
  static GType ags_type_init_channel = 0;

  if(!ags_type_init_channel){
    static const GTypeInfo ags_init_channel_info = {
      sizeof (AgsInitChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_init_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsInitChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_init_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_init_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_init_channel = g_type_register_static(AGS_TYPE_TASK,
						   "AgsInitChannel\0",
						   &ags_init_channel_info,
						   0);

    g_type_add_interface_static(ags_type_init_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_init_channel);
}

void
ags_init_channel_class_init(AgsInitChannelClass *init_channel)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_init_channel_parent_class = g_type_class_peek_parent(init_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) init_channel;

  gobject->set_property = ags_init_channel_set_property;
  gobject->get_property = ags_init_channel_get_property;

  gobject->finalize = ags_init_channel_finalize;

  /* properties */
  /**
   * AgsInitChannel:channel:
   *
   * The assigned #AgsChannel
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_object("channel\0",
				   "channel of init channel\0",
				   "The channel of init channel task\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /**
   * AgsInitChannel:play-pad:
   *
   * Do play pad.
   * 
   * Since: 0.7.117
   */
  param_spec =  g_param_spec_boolean("play-pad\0",
				     "play pad\0",
				     "play pad of channel\0",
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAY_PAD,
				  param_spec);

  /**
   * AgsInitChannel:do-playback:
   *
   * The effects do-playback.
   * 
   * Since: 0.7.117
   */
  param_spec =  g_param_spec_boolean("do-playback\0",
				     "do playback\0",
				     "Do playback of channel\0",
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DO_PLAYBACK,
				  param_spec);

  /**
   * AgsInitChannel:do-sequencer:
   *
   * The effects do-sequencer.
   * 
   * Since: 0.7.117
   */
  param_spec =  g_param_spec_boolean("do-sequencer\0",
				     "do sequencer\0",
				     "Do sequencer of channel\0",
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DO_SEQUENCER,
				  param_spec);

  /**
   * AgsInitChannel:do-notation:
   *
   * The effects do-notation.
   * 
   * Since: 0.7.117
   */
  param_spec =  g_param_spec_boolean("do-notation\0",
				     "do notation\0",
				     "Do notation of channel\0",
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DO_NOTATION,
				  param_spec);

  /* AgsTaskClass */
  task = (AgsTaskClass *) init_channel;

  task->launch = ags_init_channel_launch;
}

void
ags_init_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_init_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_init_channel_connect;
  connectable->disconnect = ags_init_channel_disconnect;
}

void
ags_init_channel_init(AgsInitChannel *init_channel)
{
  init_channel->channel = NULL;
  init_channel->play_pad = FALSE;

  init_channel->do_playback = FALSE;
  init_channel->do_sequencer = FALSE;
  init_channel->do_notation = FALSE;
}

void
ags_init_channel_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsInitChannel *init_channel;

  init_channel = AGS_INIT_CHANNEL(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      if(init_channel->channel == (GObject *) channel){
	return;
      }

      if(init_channel->channel != NULL){
	g_object_unref(init_channel->channel);
      }

      if(channel != NULL){
	g_object_ref(channel);
      }

      init_channel->channel = (GObject *) channel;
    }
    break;
  case PROP_PLAY_PAD:
    {
      gboolean play_pad;

      play_pad = g_value_get_boolean(value);

      init_channel->play_pad = play_pad;
    }
    break;
  case PROP_DO_PLAYBACK:
    {
      gboolean do_playback;

      do_playback = g_value_get_boolean(value);

      init_channel->do_playback = do_playback;
    }
    break;
  case PROP_DO_SEQUENCER:
    {
      gboolean do_sequencer;

      do_sequencer = g_value_get_boolean(value);

      init_channel->do_sequencer = do_sequencer;
    }
    break;
  case PROP_DO_NOTATION:
    {
      gboolean do_notation;

      do_notation = g_value_get_boolean(value);

      init_channel->do_notation = do_notation;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_init_channel_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsInitChannel *init_channel;

  init_channel = AGS_INIT_CHANNEL(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      g_value_set_object(value, init_channel->channel);
    }
    break;
  case PROP_PLAY_PAD:
    {
      g_value_set_boolean(value, init_channel->play_pad);
    }
    break;
  case PROP_DO_PLAYBACK:
    {
      g_value_set_boolean(value, init_channel->do_playback);
    }
    break;
  case PROP_DO_SEQUENCER:
    {
      g_value_set_boolean(value, init_channel->do_sequencer);
    }
    break;
  case PROP_DO_NOTATION:
    {
      g_value_set_boolean(value, init_channel->do_notation);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_init_channel_connect(AgsConnectable *connectable)
{
  ags_init_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_init_channel_disconnect(AgsConnectable *connectable)
{
  ags_init_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_init_channel_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_init_channel_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_init_channel_launch(AgsTask *task)
{
  AgsPlaybackDomain *playback_domain;
  AgsChannel *channel;
  AgsRecallID *recall_id;
  
  AgsInitChannel *init_channel;

  GList *list, *list_start;
  GList *start_queue;

  gint stage;
  guint nth_domain;
  gboolean arrange_recall_id, duplicate_templates, resolve_dependencies;

  init_channel = AGS_INIT_CHANNEL(task);
  playback_domain = (AgsPlaybackDomain *) AGS_AUDIO(AGS_CHANNEL(init_channel->channel)->audio)->playback_domain;
  
  list = NULL;
  list_start = NULL;

  start_queue = NULL;

  /* init channel */
  if(init_channel->play_pad){
    AgsChannel *next_pad;

    next_pad = init_channel->channel->next_pad;

    for(stage = 0; stage < 3; stage++){

      channel = init_channel->channel;
      list = list_start;
      
      if(stage == 0){
	arrange_recall_id = TRUE;
	duplicate_templates = TRUE;
	resolve_dependencies = TRUE;
      }else{
	arrange_recall_id = FALSE;
	duplicate_templates = FALSE;
	resolve_dependencies = FALSE;

	if(list == NULL){
	  break;
	}
      }
      
      while(channel != next_pad){
	if(stage == 0){
	  recall_id = NULL;
	  
	  if(init_channel->do_playback){
	    nth_domain = 0;
	    
	    g_atomic_int_or(&(AGS_PLAYBACK(channel->playback)->flags),
			    AGS_PLAYBACK_PLAYBACK);

	    recall_id = ags_channel_recursive_play_init(channel, stage,
							arrange_recall_id, duplicate_templates,
							TRUE, FALSE, FALSE,
							resolve_dependencies,
							NULL);
	    AGS_PLAYBACK(channel->playback)->recall_id[0] = recall_id;

	    start_queue = g_list_prepend(start_queue,
					 AGS_PLAYBACK(channel->playback)->channel_thread[0]);
	  }
	  
	  if(init_channel->do_sequencer){
	    nth_domain = 1;

	    g_atomic_int_or(&(AGS_PLAYBACK(channel->playback)->flags),
			      AGS_PLAYBACK_SEQUENCER);

	    recall_id = ags_channel_recursive_play_init(channel, stage,
							arrange_recall_id, duplicate_templates,
							FALSE, TRUE, FALSE,
							resolve_dependencies,
							NULL);
	    AGS_PLAYBACK(channel->playback)->recall_id[1] = recall_id;

	    start_queue = g_list_prepend(start_queue,
					 AGS_PLAYBACK(channel->playback)->channel_thread[1]);
	  }
	  
	  if(init_channel->do_notation){
	    nth_domain = 2;

	    g_atomic_int_or(&(AGS_PLAYBACK(channel->playback)->flags),
			    AGS_PLAYBACK_NOTATION);

	    recall_id = ags_channel_recursive_play_init(channel, stage,
							arrange_recall_id, duplicate_templates,
							FALSE, FALSE, TRUE,
							resolve_dependencies,
							NULL);
	    AGS_PLAYBACK(channel->playback)->recall_id[2] = recall_id;

	    start_queue = g_list_prepend(start_queue,
					 AGS_PLAYBACK(channel->playback)->channel_thread[2]);
	  }

	  if(recall_id != NULL){
	    list_start = g_list_append(list_start,
				       recall_id);
	  }
	}else{
	  if(init_channel->do_playback){
	    nth_domain = 0;

	    ags_channel_recursive_play_init(channel, stage,
					    arrange_recall_id, duplicate_templates,
					    TRUE, FALSE, FALSE,
					    resolve_dependencies,
					    AGS_RECALL_ID(list->data));
	  }

	  if(init_channel->do_sequencer){
	    nth_domain = 1;
	    
	    ags_channel_recursive_play_init(channel, stage,
					    arrange_recall_id, duplicate_templates,
					    FALSE, TRUE, FALSE,
					    resolve_dependencies,
					    AGS_RECALL_ID(list->data));
	  }

	  if(init_channel->do_notation){
	    nth_domain = 2;
	    
	    ags_channel_recursive_play_init(channel, stage,
					    arrange_recall_id, duplicate_templates,
					    FALSE, FALSE, TRUE,
					    resolve_dependencies,
					    AGS_RECALL_ID(list->data));
	
	  }

	  list = list->next;
	}

	channel = channel->next;
      }
    }
  }else{
    AgsRecallID *recall_id;

    for(stage = 0; stage < 3; stage++){

      channel = init_channel->channel;
      list = list_start;
      
      if(stage == 0){
	arrange_recall_id = TRUE;
	duplicate_templates = TRUE;
	resolve_dependencies = TRUE;
      }else{
	arrange_recall_id = FALSE;
	duplicate_templates = FALSE;
	resolve_dependencies = FALSE;

	if(list == NULL){
	  break;
	}
      }
      
      if(stage == 0){
	recall_id = NULL;
	
	if(init_channel->do_playback){
	    nth_domain = 0;
	    
	    g_atomic_int_or(&(AGS_PLAYBACK(channel->playback)->flags),
			  AGS_PLAYBACK_PLAYBACK);

	  recall_id = ags_channel_recursive_play_init(channel, stage,
						      arrange_recall_id, duplicate_templates,
						      TRUE, FALSE, FALSE,
						      resolve_dependencies,
						      NULL);
	  AGS_PLAYBACK(channel->playback)->recall_id[0] = recall_id;

	  start_queue = g_list_prepend(start_queue,
				       AGS_PLAYBACK(channel->playback)->channel_thread[0]);
	}
	  
	if(init_channel->do_sequencer){
	  nth_domain = 1;
	    
	  g_atomic_int_or(&(AGS_PLAYBACK(channel->playback)->flags),
			  AGS_PLAYBACK_SEQUENCER);

	  recall_id = ags_channel_recursive_play_init(channel, stage,
						      arrange_recall_id, duplicate_templates,
						      FALSE, TRUE, FALSE,
						      resolve_dependencies,
						      NULL);
	  AGS_PLAYBACK(channel->playback)->recall_id[1] = recall_id;

	  start_queue = g_list_prepend(start_queue,
				       AGS_PLAYBACK(channel->playback)->channel_thread[1]);
	}
	  
	if(init_channel->do_notation){
	  nth_domain = 2;
	  
	  g_atomic_int_or(&(AGS_PLAYBACK(channel->playback)->flags),
			  AGS_PLAYBACK_NOTATION);

	  recall_id = ags_channel_recursive_play_init(channel, stage,
						      arrange_recall_id, duplicate_templates,
						      FALSE, FALSE, TRUE,
						      resolve_dependencies,
						      NULL);
	  AGS_PLAYBACK(channel->playback)->recall_id[2] = recall_id;

	  start_queue = g_list_prepend(start_queue,
				       AGS_PLAYBACK(channel->playback)->channel_thread[2]);
	}

	if(recall_id != NULL){
	  list_start = g_list_append(list_start,
				     recall_id);
	}
      }else{
	if(init_channel->do_playback){
	  ags_channel_recursive_play_init(channel, stage,
					  arrange_recall_id, duplicate_templates,
					  TRUE, FALSE, FALSE,
					  resolve_dependencies,
					  AGS_RECALL_ID(list->data));
	}

	if(init_channel->do_sequencer){
	  ags_channel_recursive_play_init(channel, stage,
					  arrange_recall_id, duplicate_templates,
					  FALSE, TRUE, FALSE,
					  resolve_dependencies,
					  AGS_RECALL_ID(list->data));
	}

	if(init_channel->do_notation){
	  ags_channel_recursive_play_init(channel, stage,
					  arrange_recall_id, duplicate_templates,
					  FALSE, FALSE, TRUE,
					  resolve_dependencies,
					  AGS_RECALL_ID(list->data));
	
	}
      }
    }
  }

  /*  */
  if(start_queue != NULL){
    if(g_atomic_pointer_get(&(AGS_THREAD(playback_domain->audio_thread[nth_domain])->start_queue)) != NULL){
      g_atomic_pointer_set(&(AGS_THREAD(playback_domain->audio_thread[nth_domain])->start_queue),
			   g_list_concat(start_queue,
					 g_atomic_pointer_get(&(AGS_THREAD(playback_domain->audio_thread[nth_domain])->start_queue))));
    }else{
      g_atomic_pointer_set(&(AGS_THREAD(playback_domain->audio_thread[nth_domain])->start_queue),
			   start_queue);
    }
  }
}

/**
 * ags_init_channel_new:
 * @channel: the #AgsChannel
 * @play_pad: %TRUE all channels of accordig pad are initialized
 * @do_playback: init playback
 * @do_sequencer: init sequencer
 * @do_notation: init notation
 *
 * Creates an #AgsInitChannel.
 *
 * Returns: an new #AgsInitChannel.
 *
 * Since: 0.4
 */
AgsInitChannel*
ags_init_channel_new(AgsChannel *channel, gboolean play_pad,
		     gboolean do_playback, gboolean do_sequencer, gboolean do_notation)
{
  AgsInitChannel *init_channel;

  init_channel = (AgsInitChannel *) g_object_new(AGS_TYPE_INIT_CHANNEL,
						 NULL);

  init_channel->channel = channel;
  init_channel->play_pad = play_pad;

  init_channel->do_playback = do_playback;
  init_channel->do_sequencer = do_sequencer;
  init_channel->do_notation = do_notation;

  return(init_channel);
}
