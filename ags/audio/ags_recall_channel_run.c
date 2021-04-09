/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/audio/ags_recall_channel_run.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recycling_context.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_recycling.h>

#include <ags/i18n.h>

void ags_recall_channel_run_class_init(AgsRecallChannelRunClass *recall_channel_run);
void ags_recall_channel_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_channel_run_init(AgsRecallChannelRun *recall_channel_run);
void ags_recall_channel_run_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_recall_channel_run_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_recall_channel_run_dispose(GObject *gobject);
void ags_recall_channel_run_finalize(GObject *gobject);

void ags_recall_channel_run_notify_recall_container_callback(GObject *gobject,
							     GParamSpec *pspec,
							     gpointer user_data);

void ags_recall_channel_run_connect(AgsConnectable *connectable);
void ags_recall_channel_run_disconnect(AgsConnectable *connectable);

AgsRecall* ags_recall_channel_run_duplicate(AgsRecall *recall,
					    AgsRecallID *recall_id,
					    guint *n_params, gchar **parameter_name, GValue *value);

void ags_recall_channel_run_map_recall_recycling(AgsRecallChannelRun *recall_channel_run);

void ags_recall_channel_run_remap_child_source(AgsRecallChannelRun *recall_channel_run,
					       AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
					       AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region);
void ags_recall_channel_run_remap_child_destination(AgsRecallChannelRun *recall_channel_run,
						    AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
						    AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region);
void ags_recall_channel_run_source_recycling_changed_callback(AgsChannel *channel,
							      AgsRecycling *old_start_region, AgsRecycling *old_end_region,
							      AgsRecycling *new_start_region, AgsRecycling *new_end_region,
							      AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
							      AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region,
							      AgsRecallChannelRun *recall_channel_run);

void ags_recall_channel_run_destination_recycling_changed_callback(AgsChannel *channel,
								   AgsRecycling *old_start_region, AgsRecycling *old_end_region,
								   AgsRecycling *new_start_region, AgsRecycling *new_end_region,
								   AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
								   AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region,
								   AgsRecallChannelRun *recall_channel_run);

/**
 * SECTION:ags_recall_channel_run
 * @short_description: dynamic channel context of recall
 * @title: AgsRecallChannelRun
 * @section_id:
 * @include: ags/audio/ags_recall_channel_run.h
 *
 * #AgsRecallChannelRun acts as channel recall run.
 */

enum{
  PROP_0,
  PROP_RECALL_AUDIO,
  PROP_RECALL_AUDIO_RUN,
  PROP_RECALL_CHANNEL,
  PROP_DESTINATION,
  PROP_SOURCE,
};

static gpointer ags_recall_channel_run_parent_class = NULL;
static AgsConnectableInterface* ags_recall_channel_run_parent_connectable_interface;

GType
ags_recall_channel_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_recall_channel_run = 0;

    static const GTypeInfo ags_recall_channel_run_info = {
      sizeof (AgsRecallChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_channel_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_channel_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_channel_run = g_type_register_static(AGS_TYPE_RECALL,
							 "AgsRecallChannelRun",
							 &ags_recall_channel_run_info,
							 0);
    
    g_type_add_interface_static(ags_type_recall_channel_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_recall_channel_run);
  }

  return g_define_type_id__volatile;
}

void
ags_recall_channel_run_class_init(AgsRecallChannelRunClass *recall_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_recall_channel_run_parent_class = g_type_class_peek_parent(recall_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_channel_run;

  gobject->set_property = ags_recall_channel_run_set_property;
  gobject->get_property = ags_recall_channel_run_get_property;

  gobject->dispose = ags_recall_channel_run_dispose;
  gobject->finalize = ags_recall_channel_run_finalize;

  /* properties */
  /**
   * AgsRecallChannelRun:recall-audio:
   *
   * The audio context of this recall.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("recall-audio",
				   i18n_pspec("AgsRecallAudio of this recall"),
				   i18n_pspec("The AgsRecallAudio which this recall needs"),
				   AGS_TYPE_RECALL_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_AUDIO,
				  param_spec);

  /**
   * AgsRecallChannelRun:recall-audio-run:
   *
   * The audio run context of this recall.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("recall-audio-run",
				   i18n_pspec("AgsRecallAudioRun of this recall"),
				   i18n_pspec("The AgsRecallAudioRun which this recall needs"),
				   AGS_TYPE_RECALL_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_AUDIO_RUN,
				  param_spec);

  /**
   * AgsRecallChannelRun:recall-channel:
   *
   * The channel context of this recall.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("recall-channel",
				   i18n_pspec("AsgRecallChannel of this recall"),
				   i18n_pspec("The AgsRecallChannel which this recall needs"),
				   AGS_TYPE_RECALL_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_CHANNEL,
				  param_spec);

  /**
   * AgsRecallChannelRun:destination:
   *
   * The channel to do output to.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("destination",
				   i18n_pspec("destination of output"),
				   i18n_pspec("The destination AgsChannel where it will output to"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DESTINATION,
				  param_spec);

  /**
   * AgsRecallChannelRun:source:
   *
   * The channel to do input from.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("source",
				   i18n_pspec("source of input"),
				   i18n_pspec("The source AgsChannel where it will take the input from"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOURCE,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_channel_run;

  recall->duplicate = ags_recall_channel_run_duplicate;
}

void
ags_recall_channel_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_channel_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_recall_channel_run_connect;
  connectable->disconnect = ags_recall_channel_run_disconnect;
}

void
ags_recall_channel_run_init(AgsRecallChannelRun *recall_channel_run)
{
  g_signal_connect_after(recall_channel_run, "notify::recall-container",
			 G_CALLBACK(ags_recall_channel_run_notify_recall_container_callback), NULL);

  recall_channel_run->recall_audio = NULL;
  recall_channel_run->recall_audio_run = NULL;
  recall_channel_run->recall_channel = NULL;

  recall_channel_run->source = NULL;
  recall_channel_run->destination = NULL;
}


void
ags_recall_channel_run_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsRecallChannelRun *recall_channel_run;

  GRecMutex *recall_mutex;

  recall_channel_run = AGS_RECALL_CHANNEL_RUN(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_channel_run);

  switch(prop_id){
  case PROP_RECALL_AUDIO:
    {
      AgsRecallAudio *recall_audio;

      recall_audio = (AgsRecallAudio *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(recall_channel_run->recall_audio == recall_audio){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(recall_channel_run->recall_audio != NULL){
	g_object_unref(recall_channel_run->recall_audio);
      }

      if(recall_audio != NULL){
	g_object_ref(G_OBJECT(recall_audio));
      }

      recall_channel_run->recall_audio = recall_audio;
      
      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RECALL_AUDIO_RUN:
    {
      AgsRecallAudioRun *recall_audio_run;

      recall_audio_run = (AgsRecallAudioRun *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(recall_channel_run->recall_audio_run == recall_audio_run){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(recall_channel_run->recall_audio_run != NULL){
	g_object_unref(recall_channel_run->recall_audio_run);
      }

      if(recall_audio_run != NULL){
	g_object_ref(G_OBJECT(recall_audio_run));
      }

      recall_channel_run->recall_audio_run = recall_audio_run;
      
      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RECALL_CHANNEL:
    {
      AgsRecallChannel *recall_channel;

      recall_channel = (AgsRecallChannel *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(recall_channel_run->recall_channel == recall_channel){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(recall_channel_run->recall_channel != NULL){
	g_object_unref(recall_channel_run->recall_channel);
      }

      if(recall_channel != NULL){
	g_object_ref(G_OBJECT(recall_channel));
      }

      recall_channel_run->recall_channel = recall_channel;
      
      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_DESTINATION:
    {
      AgsChannel *destination;
      AgsChannel *old_destination;

      destination = (AgsChannel *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(recall_channel_run->destination == destination){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      old_destination = recall_channel_run->destination;

      if(destination != NULL){    
	g_object_ref(G_OBJECT(destination));
      }

      recall_channel_run->destination = destination;

      /* child destination */
#if 0
      if(destination == recall_channel_run->source){
	g_warning("destination == recall_channel_run->source");
      }
#endif
      
      if(old_destination != NULL){
	g_object_unref(G_OBJECT(old_destination));
      }

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_SOURCE:
    {
      AgsChannel *source;
      AgsChannel *old_source;

      source = (AgsChannel *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(recall_channel_run->source == source){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      old_source = recall_channel_run->source;

      if(source != NULL){
	g_object_ref(G_OBJECT(source));
      }

      recall_channel_run->source = source;

#if 0
      if(source == recall_channel_run->destination){
	g_warning("source == recall_channel_run->destination");
      }
#endif
      
      if(old_source != NULL){
	g_object_unref(G_OBJECT(old_source));
      }

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_channel_run_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsRecallChannelRun *recall_channel_run;

  GRecMutex *recall_mutex;

  recall_channel_run = AGS_RECALL_CHANNEL_RUN(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_channel_run);

  switch(prop_id){
  case PROP_RECALL_AUDIO:
    {
      g_rec_mutex_lock(recall_mutex);
      
      g_value_set_object(value, recall_channel_run->recall_audio);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RECALL_AUDIO_RUN:
    {
      g_rec_mutex_lock(recall_mutex);
      
      g_value_set_object(value, recall_channel_run->recall_audio_run);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RECALL_CHANNEL:
    {
      g_rec_mutex_lock(recall_mutex);
      
      g_value_set_object(value, recall_channel_run->recall_channel);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_DESTINATION:
    {
      g_rec_mutex_lock(recall_mutex);
      
      g_value_set_object(value, recall_channel_run->destination);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_SOURCE:
    {
      g_rec_mutex_lock(recall_mutex);
      
      g_value_set_object(value, recall_channel_run->source);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_channel_run_dispose(GObject *gobject)
{
  AgsRecallChannelRun *recall_channel_run;

  recall_channel_run = AGS_RECALL_CHANNEL_RUN(gobject);
  
  /* recall audio */
  if(recall_channel_run->recall_audio != NULL){
    gpointer tmp;

    tmp = recall_channel_run->recall_audio;

    recall_channel_run->recall_audio = NULL;

    g_object_unref(tmp);
  }

  /* recall audio run */
  if(recall_channel_run->recall_audio_run != NULL){
    gpointer tmp;

    tmp = recall_channel_run->recall_audio_run;

    recall_channel_run->recall_audio_run = NULL;

    g_object_unref(tmp);
  }
  
  /* recall channel */
  if(recall_channel_run->recall_channel != NULL){
    gpointer tmp;

    tmp = recall_channel_run->recall_channel;

    recall_channel_run->recall_channel = NULL;

    g_object_unref(tmp);
  }

  /* destination */
  if(recall_channel_run->destination != NULL){
    gpointer tmp;

    tmp = recall_channel_run->destination;

    recall_channel_run->destination = NULL;

    g_object_unref(tmp);
  }

  /* source */
  if(recall_channel_run->source != NULL){
    gpointer tmp;

    tmp = recall_channel_run->source;

    recall_channel_run->source = NULL;

    g_object_unref(tmp);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_channel_run_parent_class)->dispose(gobject);
}

void
ags_recall_channel_run_finalize(GObject *gobject)
{
  AgsRecallChannelRun *recall_channel_run;

  recall_channel_run = AGS_RECALL_CHANNEL_RUN(gobject);

  /* recall audio */
  if(recall_channel_run->recall_audio != NULL){
    gpointer tmp;

    tmp = recall_channel_run->recall_audio;

    recall_channel_run->recall_audio = NULL;

    g_object_unref(G_OBJECT(recall_channel_run->recall_audio));
  }

  /* recall audio run */
  if(recall_channel_run->recall_audio_run != NULL){
    gpointer tmp;

    tmp = recall_channel_run->recall_audio_run;

    recall_channel_run->recall_audio_run = NULL;

    g_object_unref(G_OBJECT(recall_channel_run->recall_audio_run));
  }
  
  /* recall channel */
  if(recall_channel_run->recall_channel != NULL){
    gpointer tmp;

    tmp = recall_channel_run->recall_channel;

    recall_channel_run->recall_channel = NULL;

    g_object_unref(G_OBJECT(recall_channel_run->recall_channel));
  }

  /* destination */
  if(recall_channel_run->destination != NULL){
    gpointer tmp;

    tmp = recall_channel_run->destination;

    recall_channel_run->destination = NULL;

    g_object_unref(recall_channel_run->destination);
  }

  /* source */
  if(recall_channel_run->source != NULL){
    gpointer tmp;

    tmp = recall_channel_run->source;

    recall_channel_run->source = NULL;

    g_object_unref(recall_channel_run->source);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_channel_run_parent_class)->finalize(gobject);
}

void
ags_recall_channel_run_notify_recall_container_callback(GObject *gobject,
							GParamSpec *pspec,
							gpointer user_data)
{
  AgsChannel *source;
  AgsRecallContainer *recall_container;
  AgsRecallChannelRun *recall_channel_run;

  recall_channel_run = AGS_RECALL_CHANNEL_RUN(gobject);

  /* get some fields */
  g_object_get(recall_channel_run,
	       "recall-container", &recall_container,
	       NULL);
  
  if(recall_container != NULL){
    AgsRecallAudio *recall_audio;
    AgsRecallID *recall_id;
    
    GList *list_start, *list;

    g_object_get(recall_channel_run,
		 "source", &source,
		 "recall-id", &recall_id,
		 NULL);

    /* recall audio */
    g_object_get(recall_container,
		 "recall-audio", &recall_audio,
		 NULL);
    
    g_object_set(recall_channel_run,
		 "recall-audio", recall_audio,
		 NULL);
    
    /* recall audio run */
    g_object_get(recall_container,
		 "recall-audio-run", &list_start,
		 NULL);

    if(recall_id != NULL){
      AgsRecyclingContext *recycling_context;
      
      g_object_get(recall_id,
		   "recycling-context", &recycling_context,
		   NULL);
      
      if((list = ags_recall_find_recycling_context(list_start,
						   (GObject *) recycling_context)) != NULL){
	g_object_set(recall_channel_run,
		     "recall-audio-run", list->data,
		     NULL);
      }

      g_object_unref(recycling_context);
    }else if(ags_recall_test_flags(recall_channel_run, AGS_RECALL_TEMPLATE)){
      if((list = ags_recall_find_template(list_start)) != NULL){	
	g_object_set(recall_channel_run,
		     "recall-audio-run", list->data,
		     NULL);
      }
    }

    g_list_free_full(list_start,
		     (GDestroyNotify) g_object_unref);

    /* recall channel */
    g_object_get(recall_container,
		 "recall-channel", &list_start,
		 NULL);
    
    if((list = ags_recall_find_provider(list_start,
					(GObject *) source)) != NULL){
      g_object_set(recall_channel_run,
		   "recall-channel", list->data,
		   NULL);
    }

    g_list_free_full(list_start,
		     (GDestroyNotify) g_object_unref);

    if(recall_audio != NULL){
      g_object_unref(recall_audio);
    }

    /* unref */
    if(source != NULL){
      g_object_unref(source);
    }
    
    g_object_unref(recall_container);

    if(recall_id != NULL){
      g_object_unref(recall_id);
    }
  }else{
    g_object_set(recall_channel_run,
		 "recall-audio", NULL,
		 "recall-audio-run", NULL,
		 "recall-channel", NULL,
		 NULL);
  }
}

void
ags_recall_channel_run_connect(AgsConnectable *connectable)
{
  AgsChannel *destination, *source;
  AgsRecallChannelRun *recall_channel_run;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  ags_recall_channel_run_parent_connectable_interface->connect(connectable);

  /* recall channel run */
  recall_channel_run = AGS_RECALL_CHANNEL_RUN(connectable);
  
  /* get some fields */
  g_object_get(recall_channel_run,
	       "destination", &destination,
	       "source", &source,
	       NULL);

  /* destination */
  if(destination != NULL){
    g_signal_connect(destination, "recycling-changed",
		     G_CALLBACK(ags_recall_channel_run_destination_recycling_changed_callback), recall_channel_run);

    g_object_unref(destination);
  }

  /* source */
  if(source != NULL){
    g_signal_connect(source, "recycling-changed",
		     G_CALLBACK(ags_recall_channel_run_source_recycling_changed_callback), recall_channel_run);

    g_object_unref(source);
  }
}

void
ags_recall_channel_run_disconnect(AgsConnectable *connectable)
{
  AgsChannel *destination, *source;
  AgsRecallChannelRun *recall_channel_run;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  ags_recall_channel_run_parent_connectable_interface->disconnect(connectable);

  /* recall channel run */
  recall_channel_run = AGS_RECALL_CHANNEL_RUN(connectable);
  
  /* get some fields */
  g_object_get(recall_channel_run,
	       "destination", &destination,
	       "source", &source,
	       NULL);
  
  /* destination */
  if(destination != NULL){
    g_object_disconnect(destination,
			"any_signal::recycling-changed",
			G_CALLBACK(ags_recall_channel_run_destination_recycling_changed_callback),
			recall_channel_run,
			NULL);

    g_object_unref(destination);
  }

  /* source */
  if(source != NULL){
    g_object_disconnect(source,
			"any_signal::recycling-changed",
			G_CALLBACK(ags_recall_channel_run_source_recycling_changed_callback),
			recall_channel_run,
			NULL);

    g_object_unref(source);
  }
}

AgsRecall*
ags_recall_channel_run_duplicate(AgsRecall *recall,
				 AgsRecallID *recall_id,
				 guint *n_params, gchar **parameter_name, GValue *value)
{
  AgsChannel *destination, *source;
  AgsRecallAudio *recall_audio;
  AgsRecallAudioRun *recall_audio_run;
  AgsRecallChannel *recall_channel;
  AgsRecallChannelRun *recall_channel_run, *copy_recall_channel_run;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;
  AgsRecyclingContext *next_recycling_context;
  
  GList *list_start, *list;
  
  GRecMutex *recall_mutex;

  recall_channel_run = AGS_RECALL_CHANNEL_RUN(recall);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);
  
  /* get some fields */
  g_rec_mutex_lock(recall_mutex);

  recall_audio = recall_channel_run->recall_audio;
  //  recall_audio_run = recall_channel_run->recall_audio_run;
  recall_channel = recall_channel_run->recall_channel;
    
  destination = recall_channel_run->destination;
  source = recall_channel_run->source;
  
  g_rec_mutex_unlock(recall_mutex);

  next_recycling_context = NULL;
  
  if(destination != NULL){
    g_object_get(destination,
		 "recall-id", &list_start,
		 NULL);

    next_recycling_context = (AgsRecyclingContext *) ags_recall_id_find_recycling_context(list_start,
											  recall_id->recycling_context->parent);
    g_list_free_full(list_start,
		     (GDestroyNotify) g_object_unref);
  }
  
  if(destination != NULL &&
     next_recycling_context == NULL){
    return(NULL);
  }
  
  /* duplicate */
  copy_recall_channel_run = (AgsRecallChannelRun *) AGS_RECALL_CLASS(ags_recall_channel_run_parent_class)->duplicate(recall,
														     recall_id,
														     n_params, parameter_name, value);
  g_object_set(copy_recall_channel_run,
	       "recall-audio", recall_audio,
	       //	       "recall-audio-run", recall_audio_run,
	       "recall-channel", recall_channel,
	       "source", source,
	       "destination", destination,
	       NULL);
  
  /* remap */
  if(destination != NULL){
    AgsRecycling *first_recycling, *last_recycling;
    
    /* get some fields */
    g_object_get(destination,
		 "first-recycling", &first_recycling,
		 "last-recycling", &last_recycling,
		 NULL);
    
    ags_recall_channel_run_remap_child_destination(copy_recall_channel_run,
						   NULL, NULL,
						   first_recycling, last_recycling);

    if(first_recycling != NULL){
      g_object_unref(first_recycling);
      g_object_unref(last_recycling);
    }
  }else if(source != NULL){
    AgsRecycling *first_recycling, *last_recycling;

    /* get some fields */
    g_object_get(source,
		 "first-recycling", &first_recycling,
		 "last-recycling", &last_recycling,
		 NULL);
    
    ags_recall_channel_run_remap_child_source(copy_recall_channel_run,
					      NULL, NULL,
					      first_recycling, last_recycling);

    if(first_recycling != NULL){
      g_object_unref(first_recycling);
      g_object_unref(last_recycling);
    }
  }

  return((AgsRecall *) copy_recall_channel_run);
}

void
ags_recall_channel_run_map_recall_recycling(AgsRecallChannelRun *recall_channel_run)
{
  AgsChannel *destination, *source;
  AgsRecycling *destination_first_recycling, *destination_last_recycling;
  AgsRecycling *destination_recycling, *destination_next_recycling, *destination_end_recycling;
  AgsRecycling *source_first_recycling, *source_last_recycling;
  AgsRecycling *source_recycling, *source_next_recycling, *source_end_recycling;
  AgsRecallID *recall_id;

  GObject *output_soundcard, *input_soundcard;
  
  GType child_type;

  guint recall_flags;
  guint ability_flags;
  guint behaviour_flags;
  gint sound_scope;
  gint output_soundcard_channel, input_soundcard_channel;
  guint samplerate;
  guint buffer_size;
  guint format;
  
  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_channel_run);

  /* get some fields */
  g_object_get(recall_channel_run,
	       "destination", &destination,
	       "source", &source,
	       NULL);

  g_rec_mutex_lock(recall_mutex);

  child_type = AGS_RECALL(recall_channel_run)->child_type;
  
  g_rec_mutex_unlock(recall_mutex);

  /* check instantiable child */
  if(source == NULL ||
     child_type == G_TYPE_NONE ||
     ags_recall_test_flags(recall_channel_run, AGS_RECALL_TEMPLATE)){
    if(destination != NULL){
      g_object_unref(destination);
    }
    
    if(source != NULL){
      g_object_unref(source);
    }
    
    return;
  }

  /* AgsRecycling - source */
  g_object_get(source,
	       "first-recycling", &source_first_recycling,
	       "last-recycling", &source_last_recycling,
	       NULL);

  /* AgsRecycling - destination*/
  destination_first_recycling = NULL;
  destination_last_recycling = NULL;

  if(destination != NULL){
    g_object_get(destination,
		 "first-recycling", &destination_first_recycling,
		 "last-recycling", &destination_last_recycling,
		 NULL);
  }

  /* map */
  if(source_first_recycling != NULL){
    /* get some fields */
    g_object_get(source_first_recycling,
		 "recall-id", &recall_id,
		 "output-soundcard", &output_soundcard,
		 "input-soundcard", &input_soundcard,
		 NULL);
    
    g_rec_mutex_lock(recall_mutex);

    ability_flags = AGS_RECALL(recall_channel_run)->ability_flags;
    behaviour_flags = AGS_RECALL(recall_channel_run)->behaviour_flags;
    sound_scope = AGS_RECALL(recall_channel_run)->sound_scope;
        
    output_soundcard_channel = AGS_RECALL(recall_channel_run)->output_soundcard_channel;

    input_soundcard_channel = AGS_RECALL(recall_channel_run)->input_soundcard_channel;

    samplerate = AGS_RECALL(recall_channel_run)->samplerate;
    buffer_size = AGS_RECALL(recall_channel_run)->buffer_size;
    format =  AGS_RECALL(recall_channel_run)->format;
    
    g_rec_mutex_unlock(recall_mutex);    

    /* get end recycling */
    source_end_recycling = ags_recycling_next(source_last_recycling);

    /*  */
    source_recycling = source_first_recycling;
    g_object_ref(source_recycling);
    
    while(source_recycling != source_end_recycling){
#ifdef AGS_DEBUG
      g_message("ags_recall_channel_run_map_recall_recycling %d", source->line);
#endif

      /*  */
      destination_recycling = destination_first_recycling;
      
      if(destination_first_recycling != NULL){
	g_object_ref(destination_recycling);
	
	/* get end recycling */
	destination_end_recycling = ags_recycling_next(destination_last_recycling);
      }else{
	destination_end_recycling = NULL;
      }
      
      do{
	AgsRecallRecycling *recall_recycling;

	/* instantiate */
	recall_recycling = g_object_new(child_type,
					"recall-id", recall_id,
					"output-soundcard", output_soundcard,
					"output-soundcard-channel", output_soundcard_channel,
					"input-soundcard", input_soundcard,
					"input-soundcard-channel", input_soundcard_channel,
					"samplerate", samplerate,
					"buffer-size", buffer_size,
					"format", format,
					"source", source_recycling,
					"destination", destination_recycling,
					NULL);

	ags_recall_set_ability_flags((AgsRecall *) recall_recycling,
				     ability_flags);
	ags_recall_set_behaviour_flags((AgsRecall *) recall_recycling,
				       behaviour_flags);
	ags_recall_set_sound_scope((AgsRecall *) recall_recycling,
				   sound_scope);
	
	ags_recall_add_child((AgsRecall *) recall_channel_run,
			     (AgsRecall *) recall_recycling);
	
	if(destination_recycling != NULL){
	  /* iterate */
	  destination_next_recycling = ags_recycling_next(destination_recycling);

	  g_object_unref(destination_recycling);
      
	  destination_recycling = destination_next_recycling;
	}
      }while(destination_recycling != destination_end_recycling);

      if(destination_recycling != NULL){
	g_object_unref(destination_recycling);
      }

      if(destination_end_recycling != NULL){
	g_object_unref(destination_end_recycling);
      }
      
      /* iterate */
      source_next_recycling = ags_recycling_next(source_recycling);

      g_object_unref(source_recycling);
      
      source_recycling = source_next_recycling;
    }
    
    /* unref */
    if(recall_id != NULL){
      g_object_unref(recall_id);
    }

    if(output_soundcard != NULL){
      g_object_unref(output_soundcard);
    }

     if(input_soundcard != NULL){
      g_object_unref(input_soundcard);
    }

    if(source_recycling != NULL){
      g_object_unref(source_recycling);
    }

    if(source_end_recycling != NULL){
      g_object_unref(source_end_recycling);
    }
  }

  /* unref */
  if(destination != NULL){
    g_object_unref(destination);
  }
    
  if(source != NULL){
    g_object_unref(source);
  }

  if(destination_first_recycling != NULL){
    g_object_unref(destination_first_recycling);
    g_object_unref(destination_last_recycling);
  }

  if(source_first_recycling != NULL){
    g_object_unref(source_first_recycling);
    g_object_unref(source_last_recycling);
  }
}

void
ags_recall_channel_run_remap_child_source(AgsRecallChannelRun *recall_channel_run,
					  AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
					  AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region)
{
  AgsChannel *destination, *source;
  AgsRecycling *destination_first_recycling;
  AgsRecycling *source_recycling, *source_next_recycling, *source_end_recycling;
  AgsRecallID *recall_id;

  GObject *output_soundcard, *input_soundcard;

  GList *list_start, *list;
  
  GType child_type;

  gint output_soundcard_channel, input_soundcard_channel;
  guint samplerate;
  guint buffer_size;
  guint format;
  
  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_channel_run);

  /* get some fields */
  g_object_get(recall_channel_run,
	       "destination", &destination,
	       "source", &source,
	       NULL);

  g_rec_mutex_lock(recall_mutex);

  child_type = AGS_RECALL(recall_channel_run)->child_type;
  
  g_rec_mutex_unlock(recall_mutex);

  /* check instantiable child */
  if(source == NULL ||
     child_type == G_TYPE_NONE ||
     ags_recall_test_flags(recall_channel_run, AGS_RECALL_TEMPLATE)){
    if(destination != NULL){
      g_object_unref(destination);
    }
    
    if(source != NULL){
      g_object_unref(source);
    }
    
    return;
  }

  /* remove old */
  if(old_start_changed_region != NULL){
    /* get end recycling */
    source_end_recycling = ags_recycling_next(old_end_changed_region);

    /*  */
    source_recycling = old_start_changed_region;
    g_object_ref(source_recycling);
    
    while(source_recycling != source_end_recycling){
      /* get children */
      g_object_get(recall_channel_run,
		   "child", &list_start,
		   NULL);

      list = list_start;
      
      while(list != NULL){
	AgsRecycling *current_source;
	AgsRecall *current_recall;

	current_recall = AGS_RECALL(list->data);

	g_object_get(current_recall,
		     "source", &current_source,
		     NULL);
	
	if(current_source == source_recycling){
	  ags_recall_cancel(current_recall);
	  ags_recall_remove_child((AgsRecall *) recall_channel_run,
				  (AgsRecall *) current_recall);

	  ags_connectable_disconnect(AGS_CONNECTABLE(current_recall));
	  g_object_run_dispose((GObject *) current_recall);
	  g_object_unref((GObject *) current_recall);
	}

	list = list->next;
      }

      g_list_free_full(list_start,
		       (GDestroyNotify) g_object_unref);

      /* iterate */
      source_next_recycling = ags_recycling_next(source_recycling);

      g_object_unref(source_recycling);

      source_recycling = source_next_recycling;
    }

    /* unref */
    if(source_recycling != NULL){
      g_object_unref(source_recycling);
    }

    if(source_end_recycling != NULL){
      g_object_unref(source_end_recycling);
    }
  }

  /* add new */
  if(new_start_changed_region != NULL){
    /* get some fields */
    g_object_get(new_start_changed_region,
		 "output-soundcard", &output_soundcard,
		 "input-soundcard", &input_soundcard,
		 NULL);

    g_object_get(recall_channel_run,
      		 "recall-id", &recall_id,
		 NULL);
    
    g_rec_mutex_lock(recall_mutex);

    output_soundcard_channel = AGS_RECALL(recall_channel_run)->output_soundcard_channel;

    input_soundcard_channel = AGS_RECALL(recall_channel_run)->input_soundcard_channel;

    samplerate = AGS_RECALL(recall_channel_run)->samplerate;
    buffer_size = AGS_RECALL(recall_channel_run)->buffer_size;
    format =  AGS_RECALL(recall_channel_run)->format;
    
    g_rec_mutex_unlock(recall_mutex);    

    if(destination != NULL){
      g_object_get(destination,
		   "first-recycling", &destination_first_recycling,
		   NULL);
    }else{
      destination_first_recycling = NULL;
    }
    
    /* get end recycling */
    source_end_recycling = ags_recycling_next(new_end_changed_region);

    /*  */
    source_recycling = new_start_changed_region;
    g_object_ref(source_recycling);
    
    while(source_recycling != source_end_recycling){
      AgsRecallRecycling *recall_recycling;

      //      g_message("%s[%d]", G_OBJECT_TYPE_NAME(recall_channel_run), recall_channel_run->source->line);
      
      recall_recycling = g_object_new(child_type,
				      "recall-id", recall_id,
				      "output-soundcard", output_soundcard,
				      "output-soundcard-channel", output_soundcard_channel,
				      "input-soundcard", input_soundcard,
				      "input-soundcard-channel", input_soundcard_channel,
				      "samplerate", samplerate,
				      "buffer-size", buffer_size,
				      "format", format,
				      "source", source_recycling,
				      "destination", destination_first_recycling,
				      NULL);
	
      ags_recall_add_child((AgsRecall *) recall_channel_run,
			   (AgsRecall *) recall_recycling);
      ags_connectable_connect(AGS_CONNECTABLE(recall_recycling));
      
      /* iterate */
      source_next_recycling = ags_recycling_next(source_recycling);

      g_object_unref(source_recycling);

      source_recycling = source_next_recycling;
    }
    
    /* unref */
    if(recall_id != NULL){
      g_object_unref(recall_id);
    }

    if(output_soundcard != NULL){
      g_object_unref(output_soundcard);
    }

     if(input_soundcard != NULL){
      g_object_unref(input_soundcard);
    }

     if(source_recycling != NULL){
      g_object_unref(source_recycling);
    }

    if(source_end_recycling != NULL){
      g_object_unref(source_end_recycling);
    }
  }

  /* unref */
  if(destination != NULL){
    g_object_unref(destination);
  }
    
  if(source != NULL){
    g_object_unref(source);
  }
}

void
ags_recall_channel_run_remap_child_destination(AgsRecallChannelRun *recall_channel_run,
					       AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
					       AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region)
{
  AgsChannel *source;
  AgsRecycling *destination_first_recycling, *destination_last_recycling;
  AgsRecycling *destination_recycling, *destination_next_recycling, *destination_end_recycling;
  AgsRecycling *source_first_recycling, *source_last_recycling;
  AgsRecycling *source_recycling, *source_next_recycling, *source_end_recycling;
  AgsRecallID *recall_id;

  GObject *output_soundcard, *input_soundcard;
  
  GType child_type;

  gint output_soundcard_channel, input_soundcard_channel;
  guint samplerate;
  guint buffer_size;
  guint format;
  
  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_channel_run);

  /* get some fields */
  g_object_get(recall_channel_run,
	       "source", &source,
	       NULL);

  g_rec_mutex_lock(recall_mutex);

  child_type = AGS_RECALL(recall_channel_run)->child_type;
  
  g_rec_mutex_unlock(recall_mutex);

  /* check instantiable child */
  if(source == NULL ||
     child_type == G_TYPE_NONE ||
     ags_recall_test_flags(recall_channel_run, AGS_RECALL_TEMPLATE)){
    if(source != NULL){
      g_object_unref(source);
    }

    return;
  }

  /* remove old */
  if(old_start_changed_region != NULL){
    /* get end recycling */
    destination_end_recycling = ags_recycling_next(old_end_changed_region);

    /*  */
    destination_recycling = old_start_changed_region;
    g_object_ref(destination_recycling);
    
    while(destination_recycling != destination_end_recycling){
      GList *list_start, *list;
      
      /* get children */
      g_object_get(recall_channel_run,
		   "child", &list_start,
		   NULL);

      list = list_start;

      while(list != NULL){
	AgsRecycling *current_destination;
	AgsRecall *current_recall;

	current_recall = AGS_RECALL(list->data);

	g_object_get(current_recall,
		     "destination", &current_destination,
		     NULL);
	
	if(current_destination == destination_recycling){
	  ags_recall_cancel(current_recall);
	  ags_recall_remove_child((AgsRecall *) recall_channel_run,
				  (AgsRecall *) current_recall);

	  ags_connectable_disconnect(AGS_CONNECTABLE(current_recall));
	  g_object_run_dispose((GObject *) current_recall);
	  g_object_unref((GObject *) current_recall);
	}

	list = list->next;
      }

      g_list_free_full(list_start,
		       (GDestroyNotify) g_object_unref);

      /* iterate */
      destination_next_recycling = ags_recycling_next(destination_recycling);

      g_object_unref(destination_recycling);

      destination_recycling = destination_next_recycling;
    }

    if(destination_recycling != NULL){
      g_object_unref(destination_recycling);
    }      

    if(destination_end_recycling != NULL){
      g_object_unref(destination_end_recycling);
    }
  }

  /* add new */
  if(new_start_changed_region == NULL ||
     source == NULL){
    return;
  }
  
  g_object_get(source,
	       "first-recycling", &source_first_recycling,
	       "last-recycling", &source_last_recycling,
	       NULL);
      
  if(source_first_recycling != NULL){
    /* get some fields */
    g_rec_mutex_lock(recall_mutex);
    
    recall_id = AGS_RECALL(recall_channel_run)->recall_id;
    
    output_soundcard = AGS_RECALL(recall_channel_run)->output_soundcard;
    output_soundcard_channel = AGS_RECALL(recall_channel_run)->output_soundcard_channel;

    input_soundcard = AGS_RECALL(recall_channel_run)->input_soundcard;
    input_soundcard_channel = AGS_RECALL(recall_channel_run)->input_soundcard_channel;

    samplerate = AGS_RECALL(recall_channel_run)->samplerate;
    buffer_size = AGS_RECALL(recall_channel_run)->buffer_size;
    format =  AGS_RECALL(recall_channel_run)->format;
    
    g_rec_mutex_unlock(recall_mutex);    

    /* get end recycling */
    destination_end_recycling = ags_recycling_next(new_end_changed_region);

    /*  */
    destination_recycling = new_start_changed_region;
    g_object_ref(destination_recycling);
    
    while(destination_recycling != destination_end_recycling){
      /* get end recycling */
      source_end_recycling = ags_recycling_next(source_last_recycling);

      /*  */
      source_recycling = source_first_recycling;
      g_object_ref(source_recycling);
	
      while(source_recycling != source_end_recycling){
	AgsRecallRecycling *recall_recycling;
	
	recall_recycling = g_object_new(child_type,
					"recall-id", recall_id,
					"output-soundcard", output_soundcard,
					"output-soundcard-channel", output_soundcard_channel,
					"input-soundcard", input_soundcard,
					"input-soundcard-channel", input_soundcard_channel,
					"samplerate", samplerate,
					"buffer-size", buffer_size,
					"format", format,
					"source", source_recycling,
					"destination", destination_recycling,
					NULL);
	  
	ags_recall_add_child(AGS_RECALL(recall_channel_run), AGS_RECALL(recall_recycling));
	ags_connectable_connect(AGS_CONNECTABLE(recall_recycling));

	/* iterate */
	source_next_recycling = ags_recycling_next(source_recycling);

	g_object_unref(source_recycling);

	source_recycling = source_next_recycling;
      }

      if(source_recycling != NULL){
	g_object_unref(source_recycling);
      }
      
      /* iterate */
      destination_next_recycling = ags_recycling_next(destination_recycling);

      g_object_unref(destination_recycling);

      destination_recycling = destination_next_recycling;
    }

    if(destination_recycling != NULL){
      g_object_unref(destination_recycling);
    }      

    if(destination_end_recycling != NULL){
      g_object_unref(destination_end_recycling);
    }
  }      

  if(source != NULL){
    g_object_unref(source);
  }
}

void
ags_recall_channel_run_source_recycling_changed_callback(AgsChannel *channel,
							 AgsRecycling *old_start_region, AgsRecycling *old_end_region,
							 AgsRecycling *new_start_region, AgsRecycling *new_end_region,
							 AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
							 AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region,
							 AgsRecallChannelRun *recall_channel_run)
{
  AgsChannel *destination;

  g_object_get(recall_channel_run,
	       "destination", &destination,
	       NULL);
  
  if(destination != NULL){
    AgsRecycling *first_recycling, *last_recycling;

    g_object_get(destination,
		 "first-recycling", &first_recycling,
		 "last-recycling", &last_recycling,
		 NULL);
    
    ags_recall_channel_run_remap_child_source(recall_channel_run,
					      NULL, NULL,
					      NULL, NULL);
    ags_recall_channel_run_remap_child_destination(recall_channel_run,
						   NULL, NULL,
						   first_recycling, last_recycling);

    if(first_recycling != NULL){
      g_object_unref(first_recycling);
      g_object_unref(last_recycling);
    }
  }else{
    ags_recall_channel_run_remap_child_source(recall_channel_run,
					      old_start_changed_region, old_end_changed_region,
					      new_start_changed_region, new_end_changed_region);
  }
}

void
ags_recall_channel_run_destination_recycling_changed_callback(AgsChannel *channel,
							      AgsRecycling *old_start_region, AgsRecycling *old_end_region,
							      AgsRecycling *new_start_region, AgsRecycling *new_end_region,
							      AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
							      AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region,
							      AgsRecallChannelRun *recall_channel_run)
{
  /* empty */
}

/**
 * ags_recall_channel_run_get_recall_audio:
 * @recall_channel_run: the #AgsRecallChannelRun
 * 
 * Get recall audio.
 * 
 * Returns: (transfer full): the #AgsRecallAudio
 * 
 * Since: 3.1.0
 */
AgsRecallAudio*
ags_recall_channel_run_get_recall_audio(AgsRecallChannelRun *recall_channel_run)
{
  AgsRecallAudio *recall_audio;

  if(!AGS_IS_RECALL_CHANNEL_RUN(recall_channel_run)){
    return(NULL);
  }

  g_object_get(recall_channel_run,
	       "recall-audio", &recall_audio,
	       NULL);

  return(recall_audio);
}

/**
 * ags_recall_channel_run_set_recall_audio:
 * @recall_channel_run: the #AgsRecallChannelRun
 * @recall_audio: the #AgsRecallAudio
 * 
 * Set recall audio.
 * 
 * Since: 3.1.0
 */
void
ags_recall_channel_run_set_recall_audio(AgsRecallChannelRun *recall_channel_run, AgsRecallAudio *recall_audio)
{
  if(!AGS_IS_RECALL_CHANNEL_RUN(recall_channel_run)){
    return;
  }

  g_object_set(recall_channel_run,
	       "recall-audio", recall_audio,
	       NULL);
}

/**
 * ags_recall_channel_run_get_recall_audio_run:
 * @recall_channel_run: the #AgsRecallChannelRun
 * 
 * Get recall audio.
 * 
 * Returns: (transfer full): the #AgsRecallAudioRun
 * 
 * Since: 3.1.0
 */
AgsRecallAudioRun*
ags_recall_channel_run_get_recall_audio_run(AgsRecallChannelRun *recall_channel_run)
{
  AgsRecallAudioRun *recall_audio_run;

  if(!AGS_IS_RECALL_CHANNEL_RUN(recall_channel_run)){
    return(NULL);
  }

  g_object_get(recall_channel_run,
	       "recall-audio-run", &recall_audio_run,
	       NULL);

  return(recall_audio_run);
}

/**
 * ags_recall_channel_run_set_recall_audio_run:
 * @recall_channel_run: the #AgsRecallChannelRun
 * @recall_audio_run: the #AgsRecallAudioRun
 * 
 * Set recall audio.
 * 
 * Since: 3.1.0
 */
void
ags_recall_channel_run_set_recall_audio_run(AgsRecallChannelRun *recall_channel_run, AgsRecallAudioRun *recall_audio_run)
{
  if(!AGS_IS_RECALL_CHANNEL_RUN(recall_channel_run)){
    return;
  }

  g_object_set(recall_channel_run,
	       "recall-audio-run", recall_audio_run,
	       NULL);
}

/**
 * ags_recall_channel_run_get_recall_channel:
 * @recall_channel_run: the #AgsRecallChannelRun
 * 
 * Get recall audio.
 * 
 * Returns: (transfer full): the #AgsRecallChannel
 * 
 * Since: 3.1.0
 */
AgsRecallChannel*
ags_recall_channel_run_get_recall_channel(AgsRecallChannelRun *recall_channel_run)
{
  AgsRecallChannel *recall_channel;

  if(!AGS_IS_RECALL_CHANNEL_RUN(recall_channel_run)){
    return(NULL);
  }

  g_object_get(recall_channel_run,
	       "recall-channel", &recall_channel,
	       NULL);

  return(recall_channel);
}

/**
 * ags_recall_channel_run_set_recall_channel:
 * @recall_channel_run: the #AgsRecallChannelRun
 * @recall_channel: the #AgsRecallChannel
 * 
 * Set recall audio.
 * 
 * Since: 3.1.0
 */
void
ags_recall_channel_run_set_recall_channel(AgsRecallChannelRun *recall_channel_run, AgsRecallChannel *recall_channel)
{
  if(!AGS_IS_RECALL_CHANNEL_RUN(recall_channel_run)){
    return;
  }

  g_object_set(recall_channel_run,
	       "recall-channel", recall_channel,
	       NULL);
}

/**
 * ags_recall_channel_run_get_destination:
 * @recall_channel_run: the #AgsRecallDestination
 * 
 * Get destination.
 * 
 * Returns: (transfer full): the #AgsDestination
 * 
 * Since: 3.1.0
 */
AgsChannel*
ags_recall_channel_run_get_destination(AgsRecallChannelRun *recall_channel_run)
{
  AgsChannel *destination;

  if(!AGS_IS_RECALL_CHANNEL_RUN(recall_channel_run)){
    return(NULL);
  }

  g_object_get(recall_channel_run,
	       "destination", &destination,
	       NULL);

  return(destination);
}

/**
 * ags_recall_channel_run_set_destination:
 * @recall_channel_run: the #AgsRecallChannelRun
 * @destination: the #AgsChannel
 * 
 * Set destination.
 * 
 * Since: 3.1.0
 */
void
ags_recall_channel_run_set_destination(AgsRecallChannelRun *recall_channel_run, AgsChannel *destination)
{
  if(!AGS_IS_RECALL_CHANNEL_RUN(recall_channel_run)){
    return;
  }

  g_object_set(recall_channel_run,
	       "destination", destination,
	       NULL);
}

/**
 * ags_recall_channel_run_get_source:
 * @recall_channel_run: the #AgsRecallSource
 * 
 * Get source.
 * 
 * Returns: (transfer full): the #AgsSource
 * 
 * Since: 3.1.0
 */
AgsChannel*
ags_recall_channel_run_get_source(AgsRecallChannelRun *recall_channel_run)
{
  AgsChannel *source;

  if(!AGS_IS_RECALL_CHANNEL_RUN(recall_channel_run)){
    return(NULL);
  }

  g_object_get(recall_channel_run,
	       "source", &source,
	       NULL);

  return(source);
}

/**
 * ags_recall_channel_run_set_source:
 * @recall_channel_run: the #AgsRecallChannelRun
 * @source: the #AgsChannel
 * 
 * Set source.
 * 
 * Since: 3.1.0
 */
void
ags_recall_channel_run_set_source(AgsRecallChannelRun *recall_channel_run, AgsChannel *source)
{
  if(!AGS_IS_RECALL_CHANNEL_RUN(recall_channel_run)){
    return;
  }

  g_object_set(recall_channel_run,
	       "source", source,
	       NULL);
}

/**
 * ags_recall_channel_run_new:
 *
 * Creates an #AgsRecallChannelRun
 *
 * Returns: a new #AgsRecallChannelRun
 *
 * Since: 3.0.0
 */
AgsRecallChannelRun*
ags_recall_channel_run_new()
{
  AgsRecallChannelRun *recall_channel_run;

  recall_channel_run = (AgsRecallChannelRun *) g_object_new(AGS_TYPE_RECALL_CHANNEL_RUN,
							    NULL);

  return(recall_channel_run);
}
