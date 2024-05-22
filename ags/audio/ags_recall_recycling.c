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

#include <ags/audio/ags_recall_recycling.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_recall_audio_signal.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/i18n.h>

void ags_recall_recycling_class_init(AgsRecallRecyclingClass *recall_recycling);
void ags_recall_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_recycling_init(AgsRecallRecycling *recall_recycling);
void ags_recall_recycling_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_recall_recycling_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_recall_recycling_dispose(GObject *gobject);
void ags_recall_recycling_finalize(GObject *gobject);

void ags_recall_recycling_connect(AgsConnectable *connectable);
void ags_recall_recycling_disconnect(AgsConnectable *connectable);
void ags_recall_recycling_connect_connection(AgsConnectable *connectable,
					     GObject *connection);
void ags_recall_recycling_disconnect_connection(AgsConnectable *connectable,
						GObject *connection);

AgsRecall* ags_recall_recycling_duplicate(AgsRecall *recall,
					  AgsRecallID *recall_id,
					  guint *n_params, gchar **parameter_name, GValue *value);

void ags_recall_recycling_source_add_audio_signal_callback(AgsRecycling *source,
							   AgsAudioSignal *audio_signal,
							   AgsRecallRecycling *recall_recycling);
void ags_recall_recycling_source_remove_audio_signal_callback(AgsRecycling *source,
							      AgsAudioSignal *audio_signal,
							      AgsRecallRecycling *recall_recycling);

/**
 * SECTION:ags_recall_recycling
 * @short_description: The recall base class of recycling context
 * @title: AgsRecallRecycling
 * @section_id:
 * @include: ags/audio/ags_recall_recycling.h
 *
 * #AgsRecallRecycling acts as dynamic recycling recall. It usually does not do any audio processing
 * but providing #AgsRecallAudioSignal child recalls.
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

GType
ags_recall_recycling_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_recall_recycling = 0;

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

    ags_type_recall_recycling = g_type_register_static(AGS_TYPE_RECALL,
						       "AgsRecallRecycling",
						       &ags_recall_recycling_info,
						       0);
    
    g_type_add_interface_static(ags_type_recall_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_recall_recycling);
  }

  return g_define_type_id__volatile;
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
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("audio-channel",
				 i18n_pspec("assigned audio channel"),
				 i18n_pspec("The audio channel this recall does output to"),
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
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("destination",
				   i18n_pspec("AgsRecycling destination of this recall"),
				   i18n_pspec("The AgsRecycling destination of this recall"),
				   AGS_TYPE_RECYCLING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DESTINATION,
				  param_spec);

  /**
   * AgsRecallRecycling:source:
   *
   * The assigned source recycling.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("source",
				   i18n_pspec("AgsRecycling source of this recall"),
				   i18n_pspec("The AgsRecycling source of this recall"),
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
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("child-destination",
				   i18n_pspec("AgsAudioSignal of this recall"),
				   i18n_pspec("The destination AgsAudioSignal child recall needs"),
				   AGS_TYPE_AUDIO_SIGNAL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHILD_DESTINATION,
				  param_spec);

  /**
   * AgsRecallRecycling:child-source: (type GList(AgsAudioSignal)) (transfer full)
   *
   * The assigned source audio signal.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("child-source",
				    i18n_pspec("AgsAudioSignal of this recall"),
				    i18n_pspec("The source AgsAudioSignal child recall needs"),
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

  connectable->connect_connection = ags_recall_recycling_connect_connection;
  connectable->disconnect_connection = ags_recall_recycling_disconnect_connection;
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

  GRecMutex *recall_mutex;

  recall_recycling = AGS_RECALL_RECYCLING(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_recycling);

  switch(prop_id){
  case PROP_AUDIO_CHANNEL:
    {
      g_rec_mutex_lock(recall_mutex);

      recall_recycling->audio_channel = g_value_get_uint(value);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_DESTINATION:
    {
      AgsRecycling *destination;

      destination = (AgsRecycling *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(recall_recycling->destination == destination){
	g_rec_mutex_unlock(recall_mutex);
	
	return;
      }

      if(recall_recycling->destination != NULL){
	g_object_unref(G_OBJECT(recall_recycling->destination));
      }

      if(destination != NULL){
	g_object_ref(G_OBJECT(destination));
      }

      recall_recycling->destination = destination;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_SOURCE:
    {
      AgsRecycling *source;

      source = (AgsRecycling *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(recall_recycling->source == source){
	g_rec_mutex_unlock(recall_mutex);
	
	return;
      }

      if(recall_recycling->source != NULL){
	if((AGS_RECALL_TEMPLATE & (AGS_RECALL(recall_recycling)->flags)) == 0 &&
	   ags_connectable_is_connected(AGS_CONNECTABLE(recall_recycling))){
	  ags_connectable_disconnect_connection(AGS_CONNECTABLE(recall_recycling),
						(GObject *) recall_recycling->source);
	}

	g_object_unref(recall_recycling->source);
      }

      if(source != NULL){
	g_object_ref(G_OBJECT(source));
      }

      recall_recycling->source = source;

      if(source != NULL){
	if((AGS_RECALL_TEMPLATE & (AGS_RECALL(recall_recycling)->flags)) == 0 &&
	   ags_connectable_is_connected(AGS_CONNECTABLE(recall_recycling))){
	  ags_connectable_connect_connection(AGS_CONNECTABLE(recall_recycling),
					     (GObject *) source);
	}
      }

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_CHILD_DESTINATION:
    {
      AgsAudioSignal *child_destination;

      child_destination = (AgsAudioSignal *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(recall_recycling->child_destination == child_destination){
	g_rec_mutex_unlock(recall_mutex);
	
	return;
      }

      if(recall_recycling->child_destination != NULL){
	g_object_unref(G_OBJECT(recall_recycling->child_destination));
      }

      if(child_destination != NULL){
	g_object_ref(G_OBJECT(child_destination));
      }

      recall_recycling->child_destination = child_destination;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_CHILD_SOURCE:
    {
      AgsAudioSignal *child_source;

      child_source = (AgsAudioSignal *) g_value_get_pointer(value);

      g_rec_mutex_lock(recall_mutex);

      if(!AGS_IS_AUDIO_SIGNAL(child_source) ||
	 g_list_find(recall_recycling->child_source, child_source) != NULL){
	g_rec_mutex_unlock(recall_mutex);
	
	return;
      }

      g_object_ref(G_OBJECT(child_source));
      recall_recycling->child_source = g_list_prepend(recall_recycling->child_source,
						      child_source);

      g_rec_mutex_unlock(recall_mutex);
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

  GRecMutex *recall_mutex;

  recall_recycling = AGS_RECALL_RECYCLING(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_recycling);

  switch(prop_id){
  case PROP_AUDIO_CHANNEL:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_uint(value, recall_recycling->audio_channel);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_DESTINATION:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, recall_recycling->destination);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_SOURCE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, recall_recycling->source);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_CHILD_DESTINATION:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, recall_recycling->child_destination);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_CHILD_SOURCE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_pointer(value, g_list_copy_deep(recall_recycling->child_source,
						  (GCopyFunc) g_object_ref,
						  NULL));

      g_rec_mutex_unlock(recall_mutex);
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
  AgsRecycling *source;
  AgsRecallRecycling *recall_recycling;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  recall_recycling = AGS_RECALL_RECYCLING(connectable);
  g_object_get(recall_recycling,
	       "source", &source,
	       NULL);
  
  ags_recall_recycling_parent_connectable_interface->connect(connectable);

  ags_connectable_connect_connection(connectable,
				     (GObject *) source);

  g_object_unref(source);
}

void
ags_recall_recycling_disconnect(AgsConnectable *connectable)
{
  AgsRecycling *source;
  AgsRecallRecycling *recall_recycling;
  
  if(!ags_connectable_is_connected(connectable)){
    return;
  }
  
  recall_recycling = AGS_RECALL_RECYCLING(connectable);
  g_object_get(recall_recycling,
	       "source", &source,
	       NULL);

  ags_recall_recycling_parent_connectable_interface->disconnect(connectable);

  ags_connectable_disconnect_connection(connectable,
					(GObject *) source);

  g_object_unref(source);
}

void
ags_recall_recycling_connect_connection(AgsConnectable *connectable,
					GObject *connection)
{
  AgsRecallRecycling *recall_recycling;

  recall_recycling = AGS_RECALL_RECYCLING(connectable);

  if(connection == NULL){
    return;
  }
  
  if((GObject *) recall_recycling->destination == connection){
    //empty
  }else if((GObject *) recall_recycling->source == connection){
    if(g_signal_handler_find(connection,
			     (G_SIGNAL_MATCH_FUNC | G_SIGNAL_MATCH_DATA),
			     0,
			     0,
			     NULL,
			     G_CALLBACK(ags_recall_recycling_source_add_audio_signal_callback),
			     recall_recycling) == 0){
      g_signal_connect_after(connection, "add-audio-signal",
			     G_CALLBACK(ags_recall_recycling_source_add_audio_signal_callback), recall_recycling);

      g_signal_connect(connection, "remove-audio-signal",
		       G_CALLBACK(ags_recall_recycling_source_remove_audio_signal_callback), recall_recycling);
    }
  }
}

void
ags_recall_recycling_disconnect_connection(AgsConnectable *connectable,
					   GObject *connection)
{
  AgsRecallRecycling *recall_recycling;

  recall_recycling = AGS_RECALL_RECYCLING(connectable);

  if(connection == NULL){
    return;
  }

  if((GObject *) recall_recycling->destination == connection){
    //empty
  }else if((GObject *) recall_recycling->source == connection){    
    g_object_disconnect(connection,
			"any_signal::add-audio-signal",
			G_CALLBACK(ags_recall_recycling_source_add_audio_signal_callback),
			recall_recycling,
			NULL);

    g_object_disconnect(connection,
			"any_signal::remove-audio-signal",
			G_CALLBACK(ags_recall_recycling_source_remove_audio_signal_callback),
			recall_recycling,
			NULL);
  }
}

AgsRecall*
ags_recall_recycling_duplicate(AgsRecall *recall,
			       AgsRecallID *recall_id,
			       guint *n_params, gchar **parameter_name, GValue *value)
{
  AgsRecycling *destination, *source;
  AgsRecallRecycling *recall_recycling, *copy_recall_recycling;

  guint audio_channel;
  
  recall_recycling = AGS_RECALL_RECYCLING(recall);
  
  /* get some fields */
  g_object_get(recall_recycling,
	       "audio-channel", &audio_channel,
	       "destination", &destination,
	       "source", &source,
	       NULL);

  copy_recall_recycling = AGS_RECALL_RECYCLING(AGS_RECALL_CLASS(ags_recall_recycling_parent_class)->duplicate(recall,
													      recall_id,
													      n_params, parameter_name, value));
  g_object_set(copy_recall_recycling,
	       "audio-channel", audio_channel,
	       "destination", destination,
	       "source", source,
	       NULL);

  if(destination != NULL){
    g_object_unref(destination);
  }

  if(source != NULL){
    g_object_unref(source);
  }
  
  return((AgsRecall *) copy_recall_recycling);
}

void
ags_recall_recycling_source_add_audio_signal_callback(AgsRecycling *source,
						      AgsAudioSignal *audio_signal,
						      AgsRecallRecycling *recall_recycling)
{
  AgsChannel *channel;
  AgsChannel *destination_channel;
  AgsRecallChannelRun *recall_channel_run;
  AgsRecallAudioSignal *recall_audio_signal;
  AgsRecallID *recall_id, *source_recall_id;
  AgsRecyclingContext *recycling_context, *source_recycling_context;
  
  GObject *output_soundcard;
  
  GType child_type;

  GList *list_start;
  
  gint sound_scope;
  guint audio_channel;
  gboolean success;
  
  GRecMutex *recall_mutex;

  if(ags_audio_signal_test_flags(audio_signal, AGS_AUDIO_SIGNAL_TEMPLATE) ||
     ags_audio_signal_test_flags(audio_signal, AGS_AUDIO_SIGNAL_RT_TEMPLATE)){
    return;
  }
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_recycling);

  /* get some fields */
  g_object_get(recall_recycling,
	       "output-soundcard", &output_soundcard,
	       NULL);
  
  g_rec_mutex_lock(recall_mutex);

  sound_scope = AGS_RECALL(recall_recycling)->sound_scope;
  
  child_type = AGS_RECALL(recall_recycling)->child_type;

  audio_channel = recall_recycling->audio_channel;
  
  g_rec_mutex_unlock(recall_mutex);

  g_object_get(recall_recycling,
	       "parent", &recall_channel_run,
	       NULL);

  g_object_get(recall_channel_run,
	       "source", &channel,
	       NULL);

  g_object_get(recall_recycling,
	       "recall-id", &recall_id,
	       NULL);
  
  if(recall_id == NULL){
    if(output_soundcard != NULL){
      g_object_unref(output_soundcard);
    }

    g_object_unref(recall_channel_run);
    
    g_object_unref(channel);
    
    return;
  }

  g_object_get(audio_signal,
	       "recall-id", &source_recall_id,
	       NULL);

  if(source_recall_id == NULL){
    if(output_soundcard != NULL){
      g_object_unref(output_soundcard);
    }

    g_object_unref(recall_channel_run);
    
    g_object_unref(channel);

    g_object_unref(recall_id);

    return;
  }

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);
  
  if(recycling_context == NULL){
    if(output_soundcard != NULL){
      g_object_unref(output_soundcard);
    }

    g_object_unref(recall_channel_run);
    
    g_object_unref(channel);

    g_object_unref(recall_id);

    g_object_unref(source_recall_id);

    return;
  }

  g_object_get(source_recall_id,
	       "recycling-context", &source_recycling_context,
	       NULL);

  if(source_recycling_context == NULL){
    if(output_soundcard != NULL){
      g_object_unref(output_soundcard);
    }

    g_object_unref(recall_channel_run);
    
    g_object_unref(channel);

    g_object_unref(recall_id);

    g_object_unref(source_recall_id);

    g_object_unref(recycling_context);

    return;
  }
  
  if(recycling_context != source_recycling_context){
    if(output_soundcard != NULL){
      g_object_unref(output_soundcard);
    }

    g_object_unref(recall_channel_run);
    
    g_object_unref(channel);

    g_object_unref(recall_id);

    g_object_unref(source_recall_id);

    g_object_unref(recycling_context);

    g_object_unref(source_recycling_context);

    return;
  }

  g_object_get(recall_channel_run,
	       "destination", &destination_channel,
	       NULL);

  if(destination_channel != NULL){
    AgsRecyclingContext *parent_recycling_context;
    
    g_object_get(destination_channel,
		 "recall-id", &list_start,
		 NULL);
  
    g_object_get(recycling_context,
		 "parent", &parent_recycling_context,
		 NULL);

    success = (destination_channel == NULL ||
	       ags_recall_id_find_recycling_context(list_start,
						    parent_recycling_context) != NULL) ? TRUE: FALSE;

    g_object_unref(parent_recycling_context);
    
    g_list_free_full(list_start,
		     g_object_unref);
  
    if(!success){
      goto ags_recall_recycling_source_add_audio_signal_callback_END;
    }
  }
  
  if(!ags_recall_id_check_sound_scope(recall_id, sound_scope)){
    goto ags_recall_recycling_source_add_audio_signal_callback_END;
  }
  
#ifdef AGS_DEBUG  
  g_message("add %s",
	    G_OBJECT_TYPE_NAME(recall_recycling));

  g_message("ags_recall_recycling_source_add_audio_signal_callback %s[%llx]",
	    G_OBJECT_TYPE_NAME(recall_recycling), (long long unsigned int) recall_recycling);

  g_message("ags_recall_recycling_source_add_audio_signal - channel: %s[%u]",
	    G_OBJECT_TYPE_NAME(recall_recycling),
	    AGS_CHANNEL(recall_recycling->source->channel)->line);

  if(!AGS_IS_INPUT(source->channel)){
    g_message("ags_recall_recycling_source_add_audio_signal_callback[%s]", G_OBJECT_TYPE_NAME(recall));
  }
#endif

  if(child_type != G_TYPE_NONE){
    recall_audio_signal = g_object_new(child_type,
				       "output-soundcard", output_soundcard,
				       "recall-id", source_recall_id,
				       "audio-channel", audio_channel,
				       "source", audio_signal,
				       NULL);

    AGS_RECALL(recall_audio_signal)->sound_scope = sound_scope;
    
#if 0 // AGS_DEBUG
    g_message(" + recall recycling %s 0x%x", G_OBJECT_TYPE_NAME(recall_audio_signal), audio_signal);
#endif
    
    ags_recall_add_child((AgsRecall *) recall_recycling, (AgsRecall *) recall_audio_signal);
  }

ags_recall_recycling_source_add_audio_signal_callback_END:
  
  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  g_object_unref(recall_channel_run);
    
  g_object_unref(channel);

  g_object_unref(recall_id);

  g_object_unref(source_recall_id);

  g_object_unref(recycling_context);

  g_object_unref(source_recycling_context);

  if(destination_channel != NULL){
    g_object_unref(destination_channel);
  }
}

void
ags_recall_recycling_source_remove_audio_signal_callback(AgsRecycling *source,
							 AgsAudioSignal *audio_signal,
							 AgsRecallRecycling *recall_recycling)
{
  AgsChannel *channel;
  AgsChannel *destination_channel;
  AgsAudioSignal *current_audio_signal;    
  AgsRecallChannelRun *recall_channel_run;
  AgsRecallAudioSignal *recall_audio_signal;
  AgsRecallID *recall_id, *source_recall_id;
  AgsRecyclingContext *recycling_context, *source_recycling_context;
  AgsRecyclingContext *parent_recycling_context;

  GList *list_start, *list;

  gboolean success;

  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) != 0 ||
     (AGS_AUDIO_SIGNAL_RT_TEMPLATE & (audio_signal->flags)) != 0){
    return;
  }
  
  g_object_get(recall_recycling,
	       "parent", &recall_channel_run,
	       NULL);

  g_object_get(recall_channel_run,
	       "source", &channel,
	       NULL);

  g_object_get(recall_recycling,
	       "recall-id", &recall_id,
	       NULL);
  
  if(recall_id == NULL){
    g_object_unref(recall_channel_run);
    
    g_object_unref(channel);

    return;
  }

  g_object_get(audio_signal,
	       "recall-id", &source_recall_id,
	       NULL);

  if(source_recall_id == NULL){
    g_object_unref(recall_channel_run);
    
    g_object_unref(channel);

    g_object_unref(recall_id);

    return;
  }

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);
  
  if(recycling_context == NULL){
    g_object_unref(recall_channel_run);
    
    g_object_unref(channel);

    g_object_unref(recall_id);

    g_object_unref(source_recall_id);

    return;
  }
  
  g_object_get(source_recall_id,
	       "recycling-context", &source_recycling_context,
	       NULL);

  if(source_recycling_context == NULL){
    g_object_unref(recall_channel_run);
    
    g_object_unref(channel);

    g_object_unref(recall_id);

    g_object_unref(source_recall_id);

    g_object_unref(recycling_context);

    return;
  }

  g_object_get(recall_channel_run,
	       "destination", &destination_channel,
	       NULL);

  if(destination_channel != NULL){
    g_object_get(destination_channel,
		 "recall-id", &list_start,
		 NULL);
    
    g_object_get(recycling_context,
		 "parent", &parent_recycling_context,
		 NULL);
    
    success = (destination_channel == NULL ||
	       ags_recall_id_find_recycling_context(list_start,
						    parent_recycling_context) != NULL) ? TRUE: FALSE;

    g_list_free_full(list_start,
		     g_object_unref);
    
    if(!success){
      goto ags_recall_recycling_source_remove_audio_signal_callback_END;
    }
  }
  
#ifdef AGS_DEBUG
  g_message("ags_recall_recycling_source_remove_audio_signal - channel: %s[%u]\n",
	    G_OBJECT_TYPE_NAME(recall_recycling),
	    AGS_CHANNEL(recall_recycling->source->channel)->line);
#endif

#if 0
  g_object_get(recall_recycling,
	       "child", &list_start,
	       NULL);
  
  list = list_start;
  
  while(list != NULL){
    recall_audio_signal = AGS_RECALL_AUDIO_SIGNAL(list->data);

    g_object_get(recall_audio_signal,
		 "source", &current_audio_signal,
		 NULL);
    
    if(current_audio_signal == audio_signal){
      g_message("- recall recycling %s", G_OBJECT_TYPE_NAME(recall_audio_signal));
      ags_recall_done((AgsRecall *) recall_audio_signal);
    }

    g_object_unref(current_audio_signal);
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
#endif
  
ags_recall_recycling_source_remove_audio_signal_callback_END:
  
  g_object_unref(recall_channel_run);
    
  g_object_unref(channel);

  g_object_unref(recall_id);

  g_object_unref(source_recall_id);

  g_object_unref(recycling_context);

  g_object_unref(source_recycling_context);

  if(destination_channel != NULL){
    g_object_unref(destination_channel);
  }
}

/**
 * ags_recall_recycling_new:
 *
 * Creates a new instance of #AgsRecallRecycling
 *
 * Returns: the new #AgsRecallRecycling
 * 
 * Since: 3.0.0
 */
AgsRecallRecycling*
ags_recall_recycling_new()
{
  AgsRecallRecycling *recall_recycling;

  recall_recycling = (AgsRecallRecycling *) g_object_new(AGS_TYPE_RECALL_RECYCLING,
							 NULL);

  return(recall_recycling);
}
