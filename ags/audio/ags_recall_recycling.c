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

#include <ags/audio/ags_recall_recycling.h>

#include <ags/lib/ags_parameter.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_dynamic_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_recall_audio_signal.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/audio/recall/ags_stream_recycling.h>

#include <ags/audio/task/ags_cancel_recall.h>

void ags_recall_recycling_class_init(AgsRecallRecyclingClass *recall_recycling);
void ags_recall_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_recall_recycling_init(AgsRecallRecycling *recall_recycling);
void ags_recall_recycling_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_recall_recycling_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_recall_recycling_connect(AgsConnectable *connectable);
void ags_recall_recycling_disconnect(AgsConnectable *connectable);
void ags_recall_recycling_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_recall_recycling_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_recall_recycling_dispose(GObject *gobject);
void ags_recall_recycling_finalize(GObject *gobject);

AgsRecall* ags_recall_recycling_duplicate(AgsRecall *recall,
					  AgsRecallID *recall_id,
					  guint *n_params, GParameter *parameter);

void ags_recall_recycling_source_add_audio_signal_callback(AgsRecycling *source,
							   AgsAudioSignal *audio_signal,
							   AgsRecallRecycling *recall_recycling);
void ags_recall_recycling_source_remove_audio_signal_callback(AgsRecycling *source,
							      AgsAudioSignal *audio_signal,
							      AgsRecallRecycling *recall_recycling);
void ags_recall_recycling_destination_add_audio_signal_callback(AgsRecycling *destination,
								AgsAudioSignal *audio_signal,
								AgsRecallRecycling *recall_recycling);
void ags_recall_recycling_destination_remove_audio_signal_callback(AgsRecycling *destination,
								   AgsAudioSignal *audio_signal,
								   AgsRecallRecycling *recall_recycling);

/**
 * SECTION:ags_recall_recycling
 * @short_description: recycling context of recall
 * @title: AgsRecallRecycling
 * @section_id:
 * @include: ags/audio/ags_recall_recycling.h
 *
 * #AgsRecallRecycling acts as recycling recall.
 */

enum{
  PROP_0,
  PROP_AUDIO_CHANNEL,
  PROP_DESTINATION,
  PROP_SOURCE,
  PROP_CHILD_DESTINATION,
  PROP_CHILD_SOURCE,
};

static gpointer ags_recall_recycling_parent_class = NULL;
static AgsConnectableInterface* ags_recall_recycling_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_recall_recycling_parent_dynamic_connectable_interface;

GType
ags_recall_recycling_get_type()
{
  static GType ags_type_recall_recycling = 0;

  if(!ags_type_recall_recycling){
    static const GTypeInfo ags_recall_recycling_info = {
      sizeof (AgsRecallRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_recycling_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_recycling_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_recycling = g_type_register_static(AGS_TYPE_RECALL,
						       "AgsRecallRecycling",
						       &ags_recall_recycling_info,
						       0);
    
    g_type_add_interface_static(ags_type_recall_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
    
    g_type_add_interface_static(ags_type_recall_recycling,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return(ags_type_recall_recycling);
}

void
ags_recall_recycling_class_init(AgsRecallRecyclingClass *recall_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_recall_recycling_parent_class = g_type_class_peek_parent(recall_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_recycling;

  gobject->set_property = ags_recall_recycling_set_property;
  gobject->get_property = ags_recall_recycling_get_property;

  gobject->dispose = ags_recall_recycling_dispose;
  gobject->finalize = ags_recall_recycling_finalize;

  /* properties */
  /**
   * AgsRecallRecycling:audio-channel:
   *
   * The assigned destination recycling.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_uint("audio-channel",
				 "assigned audio channel",
				 "The audio channel this recall does output to",
				 0,
				 65536,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);

  /**
   * AgsRecallRecycling:destination:
   *
   * The assigned destination recycling.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("destination",
				   "AgsRecycling destination of this recall",
				   "The AgsRecycling destination of this recall",
				   AGS_TYPE_RECYCLING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DESTINATION,
				  param_spec);

  /**
   * AgsRecallRecycling:destination:
   *
   * The assigned source recycling.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("source",
				   "AgsRecycling source of this recall",
				   "The AgsRecycling source of this recall",
				   AGS_TYPE_RECYCLING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOURCE,
				  param_spec);

  /**
   * AgsRecallRecycling:child-destination:
   *
   * The assigned destination audio signal.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("child-destination",
				   "AgsAudioSignal of this recall",
				   "The destination AgsAudioSignal child recall needs",
				   AGS_TYPE_AUDIO_SIGNAL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHILD_DESTINATION,
				  param_spec);

  /**
   * AgsRecallRecycling:child-source:
   *
   * The assigned source audio signal.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("child-source",
				   "AgsAudioSignal of this recall",
				   "The source AgsAudioSignal child recall needs",
				   AGS_TYPE_AUDIO_SIGNAL,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHILD_SOURCE,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_recycling;

  recall->duplicate = ags_recall_recycling_duplicate;

  /* AgsRecallRecyclingClass */
}

void
ags_recall_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_recall_recycling_connectable_parent_interface;

  ags_recall_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_recall_recycling_connect;
  connectable->disconnect = ags_recall_recycling_disconnect;
}

void
ags_recall_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_recall_recycling_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_recall_recycling_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_recall_recycling_disconnect_dynamic;
}

void
ags_recall_recycling_init(AgsRecallRecycling *recall_recycling)
{
  recall_recycling->flags = (AGS_RECALL_RECYCLING_MAP_CHILD_DESTINATION  |
			     AGS_RECALL_RECYCLING_MAP_CHILD_SOURCE);

  recall_recycling->audio_channel = 0;

  recall_recycling->destination = NULL;
  recall_recycling->source = NULL;

  recall_recycling->child_destination = NULL;
  recall_recycling->child_source = NULL;
}

void
ags_recall_recycling_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsRecallRecycling *recall_recycling;

  recall_recycling = AGS_RECALL_RECYCLING(gobject);

  switch(prop_id){
  case PROP_AUDIO_CHANNEL:
    {
      recall_recycling->audio_channel = g_value_get_uint(value);
    }
    break;
  case PROP_DESTINATION:
    {
      AgsRecycling *destination;

      destination = (AgsRecycling *) g_value_get_object(value);

      if(recall_recycling->destination == destination){
	return;
      }

      if(recall_recycling->destination != NULL){
	if((AGS_RECALL_TEMPLATE & (AGS_RECALL(recall_recycling)->flags)) == 0){
	  gobject = G_OBJECT(recall_recycling->destination);
	    
	  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(recall_recycling)->flags)) != 0){
	    //	    g_signal_handler_disconnect(gobject, recall_recycling->destination_add_audio_signal_handler);
	      
	    //	    g_signal_handler_disconnect(gobject, recall_recycling->destination_remove_audio_signal_handler);
	  }
	}

	g_object_unref(G_OBJECT(recall_recycling->destination));
      }

      if(destination != NULL){
	g_object_ref(G_OBJECT(destination));
      }

      if((AGS_RECALL_TEMPLATE & (AGS_RECALL(recall_recycling)->flags)) == 0){
	gobject = G_OBJECT(destination);
	  
	if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(recall_recycling)->flags)) != 0){
	  //	  recall_recycling->destination_add_audio_signal_handler =
	  //	    g_signal_connect_after(gobject, "add_audio_signal",
	  //				   G_CALLBACK(ags_recall_recycling_destination_add_audio_signal_callback), recall_recycling);
	    
	  //	  recall_recycling->destination_remove_audio_signal_handler =
	  //	    g_signal_connect(gobject, "remove_audio_signal",
	  //			     G_CALLBACK(ags_recall_recycling_destination_remove_audio_signal_callback), recall_recycling);
	}
      }

      recall_recycling->destination = destination;
    }
    break;
  case PROP_SOURCE:
    {
      AgsRecycling *source;

      source = (AgsRecycling *) g_value_get_object(value);

      if(recall_recycling->source == source){
	return;
      }

      if(recall_recycling->source != NULL){
	if((AGS_RECALL_TEMPLATE & (AGS_RECALL(recall_recycling)->flags)) == 0){
	  gobject = G_OBJECT(recall_recycling->source);
	    
	  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(recall_recycling)->flags)) != 0){
	    g_signal_handler_disconnect(gobject, recall_recycling->source_add_audio_signal_handler);
	      
	    g_signal_handler_disconnect(gobject, recall_recycling->source_remove_audio_signal_handler);
	  }
	}

	g_object_unref(G_OBJECT(recall_recycling->source));
      }

      if(source != NULL){
	g_object_ref(G_OBJECT(source));
      }

      recall_recycling->source = source;

      if(source != NULL){
	if((AGS_RECALL_TEMPLATE & (AGS_RECALL(recall_recycling)->flags)) == 0){
	  gobject = G_OBJECT(source);

	  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(recall_recycling)->flags)) != 0){
	    recall_recycling->source_add_audio_signal_handler =
	      g_signal_connect_after(gobject, "add_audio_signal",
				     G_CALLBACK(ags_recall_recycling_source_add_audio_signal_callback), recall_recycling);
	    
	    recall_recycling->source_remove_audio_signal_handler =
	      g_signal_connect(gobject, "remove_audio_signal",
	    		       G_CALLBACK(ags_recall_recycling_source_remove_audio_signal_callback), recall_recycling);
	  }
	}
      }
    }
    break;
  case PROP_CHILD_DESTINATION:
    {
      AgsAudioSignal *child_destination;

      child_destination = (AgsAudioSignal *) g_value_get_object(value);

      if(recall_recycling->child_destination == child_destination){
	return;
      }

      if(recall_recycling->child_destination != NULL){
	g_object_unref(G_OBJECT(recall_recycling->child_destination));
      }

      if(child_destination != NULL){
	g_object_ref(G_OBJECT(child_destination));
      }

      recall_recycling->child_destination = child_destination;
    }
    break;
  case PROP_CHILD_SOURCE:
    {
      AgsAudioSignal *child_source;

      child_source = (AgsAudioSignal *) g_value_get_object(value);

      if(g_list_find(recall_recycling->child_source, child_source) != NULL){
	return;
      }

      if(child_source != NULL){
	g_object_ref(G_OBJECT(child_source));

	recall_recycling->child_source = g_list_prepend(recall_recycling->child_source,
							child_source);
      }
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_recycling_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsRecallRecycling *recall_recycling;

  recall_recycling = AGS_RECALL_RECYCLING(gobject);

  switch(prop_id){
  case PROP_AUDIO_CHANNEL:
    {
      g_value_set_uint(value, recall_recycling->audio_channel);
    }
    break;
  case PROP_DESTINATION:
    {
      g_value_set_object(value, recall_recycling->destination);
    }
    break;
  case PROP_SOURCE:
    {
      g_value_set_object(value, recall_recycling->source);
    }
    break;
  case PROP_CHILD_DESTINATION:
    {
      g_value_set_object(value, recall_recycling->child_destination);
    }
    break;
  };
}

void
ags_recall_recycling_dispose(GObject *gobject)
{
  AgsRecallRecycling *recall_recycling;

  recall_recycling = AGS_RECALL_RECYCLING(gobject);

  /* destination */
  if(recall_recycling->destination != NULL){
    g_object_unref(G_OBJECT(recall_recycling->destination));

    recall_recycling->destination = NULL;
  }

  /* source */
  if(recall_recycling->source != NULL){
    g_object_unref(G_OBJECT(recall_recycling->source));

    recall_recycling->source = NULL;
  }

  /* child destination */
  if(recall_recycling->child_destination != NULL){
    g_object_unref(G_OBJECT(recall_recycling->child_destination));

    recall_recycling->child_destination = NULL;
  }

  /* child source */
  if(recall_recycling->child_source != NULL){
    g_list_free_full(recall_recycling->child_source,
		     g_object_unref);

    recall_recycling->child_source = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_recycling_parent_class)->dispose(gobject);
}

void
ags_recall_recycling_finalize(GObject *gobject)
{
  AgsRecallRecycling *recall_recycling;

  recall_recycling = AGS_RECALL_RECYCLING(gobject);

  /* destination */
  if(recall_recycling->destination != NULL){
    g_object_unref(G_OBJECT(recall_recycling->destination));
  }

  /* source */
  if(recall_recycling->source != NULL){
    g_object_unref(G_OBJECT(recall_recycling->source));
  }

  /* child destination */
  if(recall_recycling->child_destination != NULL){
    g_object_unref(G_OBJECT(recall_recycling->child_destination));
  }

  /* child source */
  if(recall_recycling->child_source != NULL){
    g_list_free_full(recall_recycling->child_source,
		     g_object_unref);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_recall_recycling_parent_class)->finalize(gobject);
}

void
ags_recall_recycling_connect(AgsConnectable *connectable)
{
  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) != 0){
    return;
  }

  ags_recall_recycling_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_recall_recycling_disconnect(AgsConnectable *connectable)
{
  ags_recall_recycling_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_recall_recycling_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsRecallRecycling *recall_recycling;
  GObject *gobject;

  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(dynamic_connectable)->flags)) != 0){
    return;
  }

  ags_recall_recycling_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* AgsRecallRecycling */
  recall_recycling = AGS_RECALL_RECYCLING(dynamic_connectable);

  /* destination */
  if(recall_recycling->destination != NULL){
    gobject = G_OBJECT(recall_recycling->destination);

    //    recall_recycling->destination_add_audio_signal_handler =
    //      g_signal_connect_after(gobject, "add_audio_signal",
    //			     G_CALLBACK(ags_recall_recycling_destination_add_audio_signal_callback), recall_recycling);
      
    //    recall_recycling->destination_remove_audio_signal_handler =
    //      g_signal_connect(gobject, "remove_audio_signal",
    //    		       G_CALLBACK(ags_recall_recycling_destination_remove_audio_signal_callback), recall_recycling);
  }

  /* source */
  gobject = G_OBJECT(recall_recycling->source);

  recall_recycling->source_add_audio_signal_handler =
    g_signal_connect_after(gobject, "add_audio_signal",
			   G_CALLBACK(ags_recall_recycling_source_add_audio_signal_callback), recall_recycling);

  recall_recycling->source_remove_audio_signal_handler =
    g_signal_connect(gobject, "remove_audio_signal",
  		     G_CALLBACK(ags_recall_recycling_source_remove_audio_signal_callback), recall_recycling);
}

void
ags_recall_recycling_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsRecallRecycling *recall_recycling;
  GObject *gobject;
  
  recall_recycling = AGS_RECALL_RECYCLING(dynamic_connectable);

  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(recall_recycling)->flags)) == 0){
    return;
  }

  /* destination */
  if(recall_recycling->destination != NULL &&
     (AGS_RECALL_TEMPLATE & (AGS_RECALL(recall_recycling)->flags)) == 0){

    gobject = G_OBJECT(recall_recycling->destination);
    
    //    g_signal_handler_disconnect(gobject, recall_recycling->destination_add_audio_signal_handler);
    //    g_signal_handler_disconnect(gobject, recall_recycling->destination_remove_audio_signal_handler);
  }

  /* source */
  if(recall_recycling->source != NULL &&
     (AGS_RECALL_TEMPLATE & (AGS_RECALL(recall_recycling)->flags)) == 0){
    gobject = G_OBJECT(recall_recycling->source);

    g_signal_handler_disconnect(gobject, recall_recycling->source_add_audio_signal_handler);
    
    g_signal_handler_disconnect(gobject, recall_recycling->source_remove_audio_signal_handler);
  }

  /* call parent */
  ags_recall_recycling_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);
}

AgsRecall*
ags_recall_recycling_duplicate(AgsRecall *recall,
			       AgsRecallID *recall_id,
			       guint *n_params, GParameter *parameter)
{
  AgsRecallRecycling *recall_recycling, *copy;
  GList *list;

  recall_recycling = AGS_RECALL_RECYCLING(recall);

  parameter = ags_parameter_grow(G_OBJECT_TYPE(recall),
				 parameter, n_params,
				 "soundcard", recall->soundcard,
				 "audio_channel", recall_recycling->audio_channel,
				 "destination", recall_recycling->destination,
				 "source", recall_recycling->source,
				 NULL);

  copy = AGS_RECALL_RECYCLING(AGS_RECALL_CLASS(ags_recall_recycling_parent_class)->duplicate(recall,
											     recall_id,
											     n_params, parameter));

  return((AgsRecall *) copy);
}

GList*
ags_recall_recycling_get_child_source(AgsRecallRecycling *recall_recycling)
{
  GList *child_source;

  child_source = g_list_copy(recall_recycling->child_source);

  return(child_source);
}

void
ags_recall_recycling_source_add_audio_signal_callback(AgsRecycling *source,
						      AgsAudioSignal *audio_signal,
						      AgsRecallRecycling *recall_recycling)
{
  AgsChannel *channel;
  AgsRecall *recall;
  AgsRecallAudioSignal *recall_audio_signal;
  AgsRecallID *recall_id;

  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(source == NULL ||
     recall_recycling == NULL ||
     audio_signal == NULL){
    return;
  }

  recall = AGS_RECALL(recall_recycling);

  //FIXME:JK: work-around for crashing while resetting link
  if(recall->parent == NULL){
    return;
  }
  
  channel = AGS_RECALL_CHANNEL_RUN(recall->parent)->source;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(mutex);

  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) != 0 ||
     audio_signal->recall_id == NULL ||
     recall->recall_id == NULL){
    pthread_mutex_unlock(mutex);
    return;
  }

  recall_id = (AgsRecallID *) audio_signal->recall_id;

  if(recall_id->recycling_context == NULL){
    pthread_mutex_unlock(mutex);
    return;
  }
  
  if(recall_id->recycling_context != recall->recall_id->recycling_context){
    if(AGS_IS_INPUT(channel)){
      if(channel->link != NULL){
	if(recall_id->recycling_context->parent != NULL){
	  if(recall_id->recycling_context->parent != recall->recall_id->recycling_context){
  	    pthread_mutex_unlock(mutex);
	    return;
	  }
	}else{
	  pthread_mutex_unlock(mutex);
	  return;
	}
      }else{
	pthread_mutex_unlock(mutex);
	return;
      }
    }else{
      pthread_mutex_unlock(mutex);
      return;
    }
  }

  
  if(AGS_RECALL_CHANNEL_RUN(recall->parent)->destination != NULL &&
     ags_recall_id_find_recycling_context(AGS_RECALL_CHANNEL_RUN(recall->parent)->destination->recall_id,
					    recall_id->recycling_context->parent) == NULL){
    if(AGS_RECALL_CHANNEL_RUN(recall->parent)->source->link != NULL){
      if(ags_recall_id_find_recycling_context(AGS_RECALL_CHANNEL_RUN(recall->parent)->destination->recall_id,
						recall_id->recycling_context->parent->parent) == NULL){
	pthread_mutex_unlock(mutex);
	return;
      }
    }else{
      pthread_mutex_unlock(mutex);
      return;
    }
  }

  if(((AGS_RECALL_ID_PLAYBACK & (recall_id->flags)) != 0 &&  (AGS_RECALL_PLAYBACK & (recall->flags)) == 0) ||
     ((AGS_RECALL_ID_SEQUENCER & (recall_id->flags)) != 0 && (AGS_RECALL_SEQUENCER & (recall->flags)) == 0) ||
     ((AGS_RECALL_ID_NOTATION & (recall_id->flags)) != 0 && (AGS_RECALL_NOTATION & (recall->flags)) == 0)){
    pthread_mutex_unlock(mutex);
    return;
  }

#ifdef AGS_DEBUG
  g_message("add %s:%x -> %x @ %x",
	    G_OBJECT_TYPE_NAME(recall),
	    recall->recall_id,
	    audio_signal->recall_id,
	    recall->recall_id->recycling_context);

  g_message("ags_recall_recycling_source_add_audio_signal_callback %s[%llx]",
	    G_OBJECT_TYPE_NAME(recall_recycling), (long long unsigned int) recall_recycling);

  g_message("ags_recall_recycling_source_add_audio_signal - channel: %s[%u]",
	    G_OBJECT_TYPE_NAME(recall_recycling),
	    AGS_CHANNEL(recall_recycling->source->channel)->line);

  if(!AGS_IS_INPUT(source->channel)){
    g_message("ags_recall_recycling_source_add_audio_signal_callback[%s]", G_OBJECT_TYPE_NAME(recall));
  }
#endif

  if((AGS_RECALL_RECYCLING_MAP_CHILD_SOURCE & (recall_recycling->flags)) != 0){
    //    g_object_set(G_OBJECT(recall_recycling),
    //		 "child-source", audio_signal,
    //		 NULL);
  }

  pthread_mutex_unlock(mutex);

  if(AGS_RECALL(recall_recycling)->child_type != G_TYPE_NONE){
    recall_audio_signal = g_object_new(AGS_RECALL(recall_recycling)->child_type,
				       "soundcard", recall->soundcard,
				       "recall_id", audio_signal->recall_id,
				       "audio_channel", recall_recycling->audio_channel,
				       //				       "destination", ((AGS_RECALL(recall_recycling)->child_type == AGS_TYPE_COPY_AUDIO_SIGNAL) ? recall_recycling->child_destination: NULL),
				       "source", audio_signal,
				       NULL);

#ifdef AGS_DEBUG
    g_message("%s", G_OBJECT_TYPE_NAME(recall_audio_signal));
#endif
    
    ags_recall_add_child(AGS_RECALL(recall_recycling), AGS_RECALL(recall_audio_signal));
  }
}

void
ags_recall_recycling_source_remove_audio_signal_callback(AgsRecycling *source,
							 AgsAudioSignal *audio_signal,
							 AgsRecallRecycling *recall_recycling)
{
  AgsChannel *channel;
  AgsRecall *recall;
  AgsCancelRecall *cancel_recall;
  AgsRecallAudioSignal *recall_audio_signal;

  AgsMutexManager *mutex_manager;

  AgsConfig *config;
  
  GList *list, *list_start;

  gchar *str;
  
  gboolean performance_mode;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(source == NULL ||
     recall_recycling == NULL ||
     audio_signal == NULL){
    return;
  }

  recall = AGS_RECALL(recall_recycling);

  //FIXME:JK: work-around for crashing while resetting link
  if(recall->parent == NULL){
    return;
  }

  channel = AGS_RECALL_CHANNEL_RUN(recall->parent)->source;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(mutex);

  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) != 0 ||
     audio_signal->recall_id == NULL ||
     recall->recall_id == NULL){
    pthread_mutex_unlock(mutex);
    return;
  }
  
  if(AGS_RECALL_ID(audio_signal->recall_id)->recycling_context != recall->recall_id->recycling_context){
    if(AGS_IS_INPUT(channel)){
      if(channel->link != NULL){
	if(AGS_RECALL_ID(audio_signal->recall_id)->recycling_context->parent != NULL){
	  if(AGS_RECALL_ID(audio_signal->recall_id)->recycling_context->parent != recall->recall_id->recycling_context){
	    pthread_mutex_unlock(mutex);
	    return;
	  }
	}else{
	  if(AGS_RECALL_ID(audio_signal->recall_id)->recycling_context != recall->recall_id->recycling_context){
	    pthread_mutex_unlock(mutex);
	    return;
	  }
	}
      }else{
	pthread_mutex_unlock(mutex);
	return;
      }
    }else{
      pthread_mutex_unlock(mutex);
      return;
    }
  }
  
  if(AGS_RECALL_CHANNEL_RUN(recall->parent)->destination != NULL &&
     ags_recall_id_find_recycling_context(AGS_RECALL_CHANNEL_RUN(recall->parent)->destination->recall_id,
					  recall->recall_id->recycling_context->parent) == NULL){
    pthread_mutex_unlock(mutex);
    return;
  }
  
#ifdef AGS_DEBUG
  g_message("ags_recall_recycling_source_remove_audio_signal - channel: %s[%u]\n",
	    G_OBJECT_TYPE_NAME(recall_recycling),
	    AGS_CHANNEL(recall_recycling->source->channel)->line);
#endif

  config = ags_config_get_instance();

  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "engine-mode");

  if(str != NULL &&
     !g_ascii_strncasecmp(str,
			  "performance",
			  12)){
    performance_mode = TRUE;
  }else{
    performance_mode = FALSE;
  }
  
  list_start = 
    list = ags_recall_get_children(recall);
  
  while(list != NULL){
    recall_audio_signal = AGS_RECALL_AUDIO_SIGNAL(list->data);

    if((recall_audio_signal->source == audio_signal) &&
       (AGS_RECALL_DONE & (AGS_RECALL(recall_audio_signal)->flags)) == 0){
      ags_recall_done(recall_audio_signal);
      //   	cancel_recall = ags_cancel_recall_new(AGS_RECALL(recall_audio_signal),
      //				      NULL);
	
      //	ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(soundcard->application_context)->main_loop)->task_thread),
      //			    (AgsTask *) cancel_recall);
    }

    list = list->next;
  }

  g_list_free(list_start);

  if((AGS_RECALL_RECYCLING_MAP_CHILD_SOURCE & (recall_recycling->flags)) != 0){
    if(g_list_find(recall_recycling->child_source,
		   audio_signal) != NULL){
      //      recall_recycling->child_source = g_list_remove(recall_recycling->child_source,
      //					     audio_signal);
      //      g_object_unref(audio_signal);
    }
  }

  pthread_mutex_unlock(mutex);
}

void
ags_recall_recycling_destination_add_audio_signal_callback(AgsRecycling *destination,
							   AgsAudioSignal *audio_signal,
							   AgsRecallRecycling *recall_recycling)
{
  AgsChannel *channel;
  AgsRecall *recall;

  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(destination == NULL ||
     recall_recycling == NULL ||
     audio_signal == NULL){
    return;
  }

  recall = AGS_RECALL(recall_recycling);
  
  //FIXME:JK: work-around for crashing while resetting link
  if(recall->parent == NULL){
    return;
  }
  
  channel = AGS_RECALL_CHANNEL_RUN(recall->parent)->source;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(mutex);

  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) != 0 ||
     audio_signal->recall_id == NULL ||
     recall->recall_id == NULL){
    pthread_mutex_unlock(mutex);
    return;
  }
  
  if(AGS_RECALL_ID(audio_signal->recall_id)->recycling_context != recall->recall_id->recycling_context){
    if(AGS_IS_INPUT(channel)){
      if(channel->link != NULL){
	if(AGS_RECALL_ID(audio_signal->recall_id)->recycling_context->parent != recall->recall_id->recycling_context){
	  pthread_mutex_unlock(mutex);
	  return;
	}
      }else{
	pthread_mutex_unlock(mutex);
	return;
      }
    }else{
      pthread_mutex_unlock(mutex);
      return;
    }
  }
  
#ifdef AGS_DEBUG
  g_message("ags_recall_recycling_destination_add_audio_signal_callback %s[%llx]",
	    G_OBJECT_TYPE_NAME(recall_recycling), (long long unsigned int) recall_recycling);
    
  if((AGS_RECALL_RECYCLING_MAP_CHILD_DESTINATION & (recall_recycling->flags)) != 0){
    g_message("ags_recall_recycling_destination_add_audio_signal - channel: %s[%u]\n",
	      G_OBJECT_TYPE_NAME(recall_recycling),
	      AGS_CHANNEL(recall_recycling->source->channel)->line);
  }
    
  g_message(" -- g_object_set -- child_destination@%llx", (long long unsigned int) audio_signal);
#endif
    
  //  g_object_set(G_OBJECT(recall_recycling),
  //	       "child_destination", audio_signal,
  //	       NULL);

  if((AGS_RECALL_RECYCLING_MAP_CHILD_SOURCE & (recall_recycling->flags)) != 0){
    if(recall_recycling->child_source != NULL){
      //      g_list_free_full(recall_recycling->child_source,
      //	       g_object_unref);
    }
    
    //    recall_recycling->child_source = NULL;
  }

  pthread_mutex_unlock(mutex);
}

void
ags_recall_recycling_destination_remove_audio_signal_callback(AgsRecycling *destination,
							      AgsAudioSignal *audio_signal,
							      AgsRecallRecycling *recall_recycling)
{
  AgsChannel *channel;
  AgsRecall *recall;
  AgsCancelRecall *cancel_recall;
  AgsRecallAudioSignal *recall_audio_signal;

  AgsMutexManager *mutex_manager;

  GList *list, *list_start;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  if(destination == NULL ||
     recall_recycling == NULL ||
     audio_signal == NULL){
    return;
  }

  recall = AGS_RECALL(recall_recycling);

  //FIXME:JK: work-around for crashing while resetting link
  if(recall->parent == NULL){
    return;
  }

  channel = AGS_RECALL_CHANNEL_RUN(recall->parent)->source;
    
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(mutex);

  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) != 0 ||
     audio_signal->recall_id == NULL ||
     recall->recall_id == NULL){
    pthread_mutex_unlock(mutex);
    return;
  }

  if(AGS_RECALL_ID(audio_signal->recall_id)->recycling_context != recall->recall_id->recycling_context){
    if(AGS_IS_INPUT(channel)){
      if(channel->link != NULL){
	if(AGS_RECALL_ID(audio_signal->recall_id)->recycling_context->parent != recall->recall_id->recycling_context){
	  pthread_mutex_unlock(mutex);
	  return;
	}
      }else{
	pthread_mutex_unlock(mutex);
	return;
      }
    }else{
      pthread_mutex_unlock(mutex);
      return;
    }
  }

#ifdef AGS_DEBUG
  g_message("ags_recall_recycling_destination_remove_audio_signal - channel: %s[%u]\n",
	    G_OBJECT_TYPE_NAME(recall_recycling),
	    AGS_CHANNEL(recall_recycling->source->channel)->line);
#endif

  list_start = 
    list = ags_recall_get_children(AGS_RECALL(recall_recycling));

  while(list != NULL){
    recall_audio_signal = AGS_RECALL_AUDIO_SIGNAL(list->data);

    if(recall_audio_signal->destination == audio_signal &&
       (AGS_RECALL_DONE & (AGS_RECALL(recall_audio_signal)->flags)) == 0){
      //      ags_recall_done(recall_audio_signal);
      //	cancel_recall = ags_cancel_recall_new(AGS_RECALL(recall_audio_signal),
      //				      NULL);

      //	ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(soundcard->application_context)->main_loop)->task_thread),
      //			    (AgsTask *) cancel_recall);
    }

    list = list->next;
  }

  g_list_free(list_start);
  
  if((AGS_RECALL_RECYCLING_MAP_CHILD_DESTINATION & (recall_recycling->flags)) != 0){
    if(recall_recycling->child_destination == audio_signal){
      //      g_object_set(G_OBJECT(recall_recycling),
      //	   "child_destination", NULL,
      //	   NULL);
    }
  }

  if((AGS_RECALL_RECYCLING_MAP_CHILD_SOURCE & (recall_recycling->flags)) != 0){
    if(recall_recycling->child_source != NULL){
      //      recall_recycling->child_source = NULL;
    }

    //    recall_recycling->child_source = NULL;
  }

  pthread_mutex_unlock(mutex);
}

/**
 * ags_recall_recycling_new:
 *
 * Creates a #AgsRecallRecycling
 *
 * Returns: a new #AgsRecallRecycling
 * 
 * Since: 0.4
 */
AgsRecallRecycling*
ags_recall_recycling_new()
{
  AgsRecallRecycling *recall_recycling;

  recall_recycling = (AgsRecallRecycling *) g_object_new(AGS_TYPE_RECALL_RECYCLING,
							 NULL);

  return(recall_recycling);
}
