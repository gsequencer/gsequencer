/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/recall/ags_play_channel_run_master.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_recycling.h>
#include <ags/audio/recall/ags_stream_channel_run.h>

#include <stdlib.h>
#include <stdio.h>

#include <ags/i18n.h>

void ags_play_channel_run_master_class_init(AgsPlayChannelRunMasterClass *play_channel_run_master);
void ags_play_channel_run_master_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_channel_run_master_init(AgsPlayChannelRunMaster *play_channel_run_master);
void ags_play_channel_run_master_set_property(GObject *gobject,
					      guint prop_id,
					      const GValue *value,
					      GParamSpec *param_spec);
void ags_play_channel_run_master_get_property(GObject *gobject,
					      guint prop_id,
					      GValue *value,
					      GParamSpec *param_spec);
void ags_play_channel_run_master_dispose(GObject *gobject);
void ags_play_channel_run_master_finalize(GObject *gobject);

void ags_play_channel_run_master_connect(AgsConnectable *connectable);
void ags_play_channel_run_master_disconnect(AgsConnectable *connectable);
void ags_play_channel_run_master_connect_connection(AgsConnectable *connectable,
						    GObject *connection);
void ags_play_channel_run_master_disconnect_connection(AgsConnectable *connectable,
						       GObject *connection);

void ags_play_channel_run_master_run_init_pre(AgsRecall *recall);
void ags_play_channel_run_master_resolve_dependency(AgsRecall *recall);

void ags_play_channel_run_master_remap_child_source(AgsPlayChannelRunMaster *play_channel_run_master,
						    AgsRecycling *old_start_region, AgsRecycling *old_end_region,
						    AgsRecycling *new_start_region, AgsRecycling *new_end_region);
void ags_play_channel_run_master_remap_dependencies(AgsPlayChannelRunMaster *play_channel_run_master,
						    AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
						    AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region);

void ags_play_channel_run_master_source_recycling_changed_callback(AgsChannel *channel,
								   AgsRecycling *old_start_region, AgsRecycling *old_end_region,
								   AgsRecycling *new_start_region, AgsRecycling *new_end_region,
								   AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
								   AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region,
								   AgsPlayChannelRunMaster *play_channel_run_master);

void ags_play_channel_run_master_stream_channel_done_callback(AgsRecall *recall,
							      AgsPlayChannelRunMaster *play_channel_run_master);

/**
 * SECTION:ags_play_channel_run_master
 * @short_description: plays channel as toplevel
 * @title: AgsPlayChannelRunMaster
 * @section_id:
 * @include: ags/audio/recall/ags_play_channel_master.h
 *
 * The #AgsPlayChannelRunMaster class plays the channel within toplevel context.
 */

enum{
  PROP_0,
  PROP_STREAM_CHANNEL_RUN,
};

static gpointer ags_play_channel_run_master_parent_class = NULL;
static AgsConnectableInterface *ags_play_channel_run_master_parent_connectable_interface;

GType
ags_play_channel_run_master_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_play_channel_run_master = 0;

    static const GTypeInfo ags_play_channel_run_master_info = {
      sizeof (AgsPlayChannelRunMasterClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_channel_run_master_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayChannelRunMaster),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_channel_run_master_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_channel_run_master_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_channel_run_master = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
							      "AgsPlayChannelRunMaster",
							      &ags_play_channel_run_master_info,
							      0);
    
    g_type_add_interface_static(ags_type_play_channel_run_master,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_play_channel_run_master);
  }

  return g_define_type_id__volatile;
}

void
ags_play_channel_run_master_class_init(AgsPlayChannelRunMasterClass *play_channel_run_master)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_play_channel_run_master_parent_class = g_type_class_peek_parent(play_channel_run_master);

  /* GObjectClass */
  gobject = (GObjectClass *) play_channel_run_master;

  gobject->set_property = ags_play_channel_run_master_set_property;
  gobject->get_property = ags_play_channel_run_master_get_property;

  gobject->dispose = ags_play_channel_run_master_dispose;
  gobject->finalize = ags_play_channel_run_master_finalize;

  /* properties */
  /**
   * AgsPlayChannelRunMaster:stream-channel-run: (type GList(AgsStreamChannelRun)) (transfer full)
   * 
   * The assigned stream channel run.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("stream-channel-run",
				    i18n_pspec("assigned AgsStreamChannelRun"),
				    i18n_pspec("an assigned AgsStreamChannelRun"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_STREAM_CHANNEL_RUN,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) play_channel_run_master;

  recall->run_init_pre = ags_play_channel_run_master_run_init_pre;
  recall->resolve_dependency = ags_play_channel_run_master_resolve_dependency;
}

void
ags_play_channel_run_master_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_play_channel_run_master_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_play_channel_run_master_connect;
  connectable->disconnect = ags_play_channel_run_master_disconnect;

  connectable->connect_connection = ags_play_channel_run_master_connect_connection;
  connectable->disconnect_connection = ags_play_channel_run_master_disconnect_connection;
}

void
ags_play_channel_run_master_init(AgsPlayChannelRunMaster *play_channel_run_master)
{
  AGS_RECALL(play_channel_run_master)->name = "ags-play";
  AGS_RECALL(play_channel_run_master)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_channel_run_master)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_channel_run_master)->xml_type = "ags-play-channel-run-master";
  AGS_RECALL(play_channel_run_master)->port = NULL;

  AGS_RECALL(play_channel_run_master)->behaviour_flags |= (AGS_SOUND_BEHAVIOUR_PERSISTENT);
  AGS_RECALL(play_channel_run_master)->child_type = AGS_TYPE_PLAY_RECYCLING;

  play_channel_run_master->flags = 0;

  play_channel_run_master->stream_channel_run = NULL;
}

void
ags_play_channel_run_master_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec)
{
  AgsPlayChannelRunMaster *play_channel_run_master;

  GRecMutex *recall_mutex;

  play_channel_run_master = AGS_PLAY_CHANNEL_RUN_MASTER(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(play_channel_run_master);

  switch(prop_id){
  case PROP_STREAM_CHANNEL_RUN:
    {
      AgsStreamChannelRun  *stream_channel_run;
      
      gboolean is_template;

      stream_channel_run = (AgsStreamChannelRun *) g_value_get_pointer(value);

      g_rec_mutex_lock(recall_mutex);

      if(stream_channel_run == NULL ||
	 g_list_find(play_channel_run_master->stream_channel_run,
		     stream_channel_run) != NULL){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(stream_channel_run != NULL){
	g_object_ref(G_OBJECT(stream_channel_run));
      }

      play_channel_run_master->stream_channel_run = g_list_prepend(play_channel_run_master->stream_channel_run,
								   stream_channel_run);

      g_rec_mutex_unlock(recall_mutex);

      if(ags_recall_test_flags((AgsRecall *) stream_channel_run, AGS_RECALL_TEMPLATE)){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }
      
      if(is_template){
	ags_recall_add_recall_dependency((AgsRecall *) play_channel_run_master,
					 ags_recall_dependency_new((GObject *) stream_channel_run));
      }else{
	if(ags_connectable_is_connected(AGS_CONNECTABLE(play_channel_run_master))){
	  ags_connectable_connect_connection(AGS_CONNECTABLE(play_channel_run_master),
					     (GObject *) stream_channel_run);
	}
      }
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_channel_run_master_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec)
{
  AgsPlayChannelRunMaster *play_channel_run_master;

  GRecMutex *recall_mutex;

  play_channel_run_master = AGS_PLAY_CHANNEL_RUN_MASTER(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(play_channel_run_master);

  switch(prop_id){
  case PROP_STREAM_CHANNEL_RUN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(play_channel_run_master->stream_channel_run,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_channel_run_master_dispose(GObject *gobject)
{
  AgsPlayChannelRunMaster *play_channel_run_master;

  play_channel_run_master = AGS_PLAY_CHANNEL_RUN_MASTER(gobject);

  /* stream channel run */
  if(play_channel_run_master->stream_channel_run != NULL){
    g_list_free_full(play_channel_run_master->stream_channel_run,
		     g_object_unref);

    play_channel_run_master->stream_channel_run = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_channel_run_master_parent_class)->dispose(gobject);
}

void
ags_play_channel_run_master_finalize(GObject *gobject)
{
  AgsPlayChannelRunMaster *play_channel_run_master;

  play_channel_run_master = AGS_PLAY_CHANNEL_RUN_MASTER(gobject);
  
  /* stream channel run */
  if(play_channel_run_master->stream_channel_run != NULL){
    g_list_free_full(play_channel_run_master->stream_channel_run,
		     g_object_unref);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_play_channel_run_master_parent_class)->finalize(gobject);
}

void
ags_play_channel_run_master_connect(AgsConnectable *connectable)
{
  AgsChannel *channel;
  AgsPlayChannelRunMaster *play_channel_run_master;

  GList *list_start, *list;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  play_channel_run_master = AGS_PLAY_CHANNEL_RUN_MASTER(connectable);

  /* call parent */
  ags_play_channel_run_master_parent_connectable_interface->connect(connectable);

  /* source */
  g_object_get(play_channel_run_master,
	       "source", &channel,
	       NULL);

  g_signal_connect(channel, "recycling-changed",
		   G_CALLBACK(ags_play_channel_run_master_source_recycling_changed_callback), play_channel_run_master);

  /* connection */
  g_object_get(play_channel_run_master,
	       "stream-channel-run", &list_start,
	       NULL);

  list = list_start;

  while(list != NULL){
    ags_connectable_connect_connection(connectable,
				       (GObject *) list->data);

    list = list->next;
  }

  /* unref */
  g_object_unref(channel);

  g_list_free_full(list_start,
		   g_object_unref);
}

void
ags_play_channel_run_master_disconnect(AgsConnectable *connectable)
{
  AgsChannel *channel;
  AgsPlayChannelRunMaster *play_channel_run_master;

  GList *list_start, *list;
  
  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  play_channel_run_master = AGS_PLAY_CHANNEL_RUN_MASTER(connectable);

  ags_play_channel_run_master_parent_connectable_interface->disconnect(connectable);

  /* source */
  g_object_get(play_channel_run_master,
	       "source", &channel,
	       NULL);

  g_object_disconnect(channel,
		      "any_signal::recycling-changed",
		      G_CALLBACK(ags_play_channel_run_master_source_recycling_changed_callback), 
		      play_channel_run_master,
		      NULL);

  /* connection */
  g_object_get(play_channel_run_master,
	       "stream-channel-run", &list_start,
	       NULL);

  list = list_start;

  while(list != NULL){
    ags_connectable_disconnect_connection(connectable,
					  (GObject *) list->data);

    list = list->next;
  }

  /* unref */
  g_object_unref(channel);

  g_list_free_full(list_start,
		   g_object_unref);
}

void
ags_play_channel_run_master_connect_connection(AgsConnectable *connectable,
					       GObject *connection)
{
  AgsPlayChannelRunMaster *play_channel_run_master;

  GList *list_start;
  
  play_channel_run_master = AGS_PLAY_CHANNEL_RUN_MASTER(connectable);

  if(connection == NULL){
    return;
  }

  g_object_get(play_channel_run_master,
	       "stream-channel-run", &list_start,
	       NULL);
  
  if(g_list_find(list_start, connection) != NULL){
    g_signal_connect(connection, "done",
		     G_CALLBACK(ags_play_channel_run_master_stream_channel_done_callback), play_channel_run_master);
  }

  g_list_free_full(list_start,
		   g_object_unref);
}

void
ags_play_channel_run_master_disconnect_connection(AgsConnectable *connectable,
						  GObject *connection)
{
  AgsPlayChannelRunMaster *play_channel_run_master;

  GList *list_start;

  play_channel_run_master = AGS_PLAY_CHANNEL_RUN_MASTER(connectable);

  if(connection == NULL){
    return;
  }

  g_object_get(play_channel_run_master,
	       "stream-channel-run", &list_start,
	       NULL);
  
  if(g_list_find(list_start, connection) != NULL){
    g_object_disconnect(connection,
			"any_signal::done",
			G_CALLBACK(ags_play_channel_run_master_stream_channel_done_callback),
			play_channel_run_master,
			NULL);
  }

  g_list_free_full(list_start,
		   g_object_unref);
}

void
ags_play_channel_run_master_run_init_pre(AgsRecall *recall)
{
  AgsChannel *channel;
  AgsRecycling *first_recycling, *last_recycling;
  AgsPlayChannelRunMaster *play_channel_run_master;

  void (*parent_class_run_init_pre)(AgsRecall *recall);
  
  GRecMutex *recall_mutex;

  play_channel_run_master = AGS_PLAY_CHANNEL_RUN_MASTER(recall);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* get parent class */
  parent_class_run_init_pre = AGS_RECALL_CLASS(ags_play_channel_run_master_parent_class)->run_init_pre;
  
  /* call parent */
  parent_class_run_init_pre(recall);

  /* remap */
  g_object_get(play_channel_run_master,
	       "source", &channel,
	       NULL);
  
  g_object_get(channel,
	       "first-recycling", &first_recycling,
	       "last-recycling", &last_recycling,
	       NULL);

  ags_play_channel_run_master_remap_dependencies(play_channel_run_master,
						 NULL, NULL,
						 first_recycling, last_recycling);

  g_object_unref(channel);

  g_object_unref(first_recycling);
  g_object_unref(last_recycling);
}

void
ags_play_channel_run_master_resolve_dependency(AgsRecall *recall)
{
  AgsRecall *template;
  AgsRecallContainer *recall_container;
  AgsRecallID *recall_id;
  AgsRecallDependency *recall_dependency;
  AgsStreamChannelRun *stream_channel_run;

  GObject *dependency;

  GList *list_start, *list;  
  
  guint i, i_stop;

  /* get some fields */
  g_object_get(recall,
	       "recall-container", &recall_container,
	       NULL);

  g_object_get(recall_container,
	       "recall-channel-run", &list_start,
	       NULL);

  list = ags_recall_find_template(list_start);

  if(list == NULL){
    g_warning("AgsRecallClass::resolve - missing dependency");

    g_object_unref(recall_container);

    g_list_free_full(list_start,
		     g_object_unref);
    
    return;
  }
  
  template = AGS_RECALL(list->data);
  g_list_free_full(list_start,
		   g_object_unref);

  g_object_get(template,
	       "recall-dependency", &list_start,
	       NULL);

  g_object_get(recall,
	       "recall-id", &recall_id,
	       NULL);

  /* prepare to resolve */
  stream_channel_run = NULL;

  list = list_start;
  
  for(i = 0; list != NULL;){
    recall_dependency = AGS_RECALL_DEPENDENCY(list->data);

    g_object_get(recall_dependency,
		 "dependency", &dependency,
		 NULL);

    if(AGS_IS_STREAM_CHANNEL_RUN(dependency)){
      stream_channel_run = (AgsStreamChannelRun *) ags_recall_dependency_resolve(recall_dependency,
										 recall_id);

      g_object_set(G_OBJECT(recall),
		   "stream-channel-run", stream_channel_run,
		   NULL);

      i++;
    }

    g_object_unref(dependency);
    
    list = list->next;
  }

  /* unref */
  g_object_unref(recall_container);

  g_list_free_full(list_start,
		   g_object_unref);

  g_object_unref(recall_id);
}

void
ags_play_channel_run_master_remap_dependencies(AgsPlayChannelRunMaster *play_channel_run_master,
					       AgsRecycling *old_start_region, AgsRecycling *old_end_region,
					       AgsRecycling *new_start_region, AgsRecycling *new_end_region)
{
  AgsChannel *current;
  AgsRecycling *recycling, *next_recycling;
  AgsRecycling *end_recycling;
  AgsRecallID *recall_id;
  AgsRecyclingContext *recycling_context;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_PLAY_CHANNEL_RUN_MASTER(play_channel_run_master)){
    return;
  }
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(play_channel_run_master);

  /* get recycling context */
  g_object_get(play_channel_run_master,
	       "recall-id", &recall_id,
	       NULL);

  recycling_context = NULL;
  
  if(recall_id != NULL){
    g_object_get(recall_id,
		 "recycling-context", &recycling_context,
		 NULL);
  }
  
  /* remove old */
  if(old_start_region != NULL){
    GList *list_start, *list;
    
    current = NULL;

    recycling = old_start_region;
    g_object_ref(recycling);
    
    end_recycling = ags_recycling_next(old_end_region);

    next_recycling = NULL;
    
    while(recycling != end_recycling){
      AgsChannel *tmp_channel;

      g_object_get(recycling,
		   "channel", &tmp_channel,
		   NULL);
      
      if(current != tmp_channel){
	current = tmp_channel;

	g_object_get(play_channel_run_master,
		     "recall-dependency", &list_start,
		     NULL);

	list = list_start;
	
	while((list = ags_recall_dependency_find_dependency_by_provider(list,
									(GObject *) current)) != NULL){
	  GObject *dependency;

	  GList *start_stream_channel_run;
	  
	  g_object_get(list->data,
		       "dependency", &dependency,
		        NULL);
	  
	  /* remove dependency */
	  ags_recall_remove_recall_dependency((AgsRecall *) play_channel_run_master,
					      list->data);

	  /* remove stream channel run */
	  g_object_get(play_channel_run_master,
		       "stream-channel-run", &start_stream_channel_run,
		       NULL);

	  if(g_list_find(start_stream_channel_run,
			 dependency) != NULL){
	    g_rec_mutex_lock(recall_mutex);
  
	    play_channel_run_master->stream_channel_run = g_list_remove(play_channel_run_master->stream_channel_run,
									dependency);

	    g_rec_mutex_unlock(recall_mutex);

	    g_object_unref(dependency);
	  }

	  g_list_free_full(start_stream_channel_run,
			   g_object_unref);

	  g_object_unref(dependency);
	  
	  /* iterate */
	  list = list->next;
	}

	g_list_free_full(list_start,
			 g_object_unref);
      }

      /* unref */
      g_object_unref(tmp_channel);

      /* iterate */
      next_recycling = ags_recycling_next(recycling);

      g_object_unref(recycling);

      recycling = next_recycling;
    }

    /* unref */
    if(end_recycling != NULL){
      g_object_unref(end_recycling);
    }

    if(next_recycling != NULL){
      g_object_unref(next_recycling);
    }
  }

  /* add new */
  if(new_start_region != NULL){
    AgsRecallContainer *recall_container;
    AgsPlayChannelRunMaster *current_master;
    
    GList *list_start, *list;
    GList *master_start, *master;
    
    current = NULL;

    recycling = new_start_region;
    g_object_ref(recycling);

    end_recycling = ags_recycling_next(new_end_region);

    next_recycling = NULL;
    
    while(recycling != end_recycling){
      AgsChannel *tmp_channel;

      g_object_get(recycling,
		   "channel", &tmp_channel,
		   NULL);
      
      if(current != tmp_channel){
	current = tmp_channel;

	g_object_get(current,
		     "play", &list_start,
		     NULL);

	list = list_start;

	while((list = ags_recall_find_type_with_recycling_context(list, AGS_TYPE_STREAM_CHANNEL_RUN, (GObject *) recycling_context)) != NULL){
	  g_object_set(play_channel_run_master,
		       "stream-channel-run", list->data,
		       NULL);

	  g_object_get(play_channel_run_master,
		       "recall-container", &recall_container,
		       NULL);

	  g_object_get(recall_container,
		       "recall-channel-run", &master_start,
		       NULL);
	  
	  master = master_start;

	  while(master != NULL){
	    current_master = AGS_PLAY_CHANNEL_RUN_MASTER(master->data);

	    if(!ags_recall_test_flags((AgsRecall *) current_master, AGS_RECALL_TEMPLATE)){
	      g_object_set(G_OBJECT(current_master),
			   "stream-channel-run", AGS_STREAM_CHANNEL_RUN(list->data),
			   NULL);
	    }
	    
	    /* iterate */
	    master = master->next;
	  }

	  g_object_unref(recall_container);

	  g_list_free_full(master_start,
			   g_object_unref);

	  /* iterate */
	  list = list->next;
	}

	g_list_free_full(list_start,
			 g_object_unref);
      }

      /* unref */
      g_object_unref(tmp_channel);

      /* iterate */
      next_recycling = ags_recycling_next(recycling);

      g_object_unref(recycling);

      recycling = next_recycling;
    }

    /* unref */
    if(end_recycling != NULL){
      g_object_unref(end_recycling);
    }

    if(next_recycling != NULL){
      g_object_unref(next_recycling);
    }
  }

  /* unref */
  if(recall_id != NULL){
    g_object_unref(recall_id);
  }
  
  if(recycling_context != NULL){
    g_object_unref(recycling_context);
  }
}

void
ags_play_channel_run_master_source_recycling_changed_callback(AgsChannel *channel,
							      AgsRecycling *old_start_region, AgsRecycling *old_end_region,
							      AgsRecycling *new_start_region, AgsRecycling *new_end_region,
							      AgsRecycling *old_start_changed_region, AgsRecycling *old_end_changed_region,
							      AgsRecycling *new_start_changed_region, AgsRecycling *new_end_changed_region,
							      AgsPlayChannelRunMaster *play_channel_run_master)
{
  if(ags_recall_test_flags((AgsRecall *) play_channel_run_master, AGS_RECALL_TEMPLATE)){
    ags_play_channel_run_master_remap_dependencies(play_channel_run_master,
						   old_start_changed_region, old_end_changed_region,
						   new_start_changed_region, new_end_changed_region);
  }
}

void
ags_play_channel_run_master_stream_channel_done_callback(AgsRecall *recall,
							 AgsPlayChannelRunMaster *play_channel_run_master)
{
  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(play_channel_run_master);

  /* remove stream channel run */
  g_rec_mutex_lock(recall_mutex);

  play_channel_run_master->stream_channel_run = g_list_remove(play_channel_run_master->stream_channel_run,
							      recall);
  g_object_unref(recall);

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_play_channel_master_run_new:
 * @source: the #AgsChannel
 *
 * Create a new instance of #AgsPlayChannelRunMaster
 *
 * Returns: the new #AgsPlayChannelRunMaster
 *
 * Since: 3.0.0
 */
AgsPlayChannelRunMaster*
ags_play_channel_run_master_new(AgsChannel *source)
{
  AgsPlayChannelRunMaster *play_channel_run_master;

  play_channel_run_master = (AgsPlayChannelRunMaster *) g_object_new(AGS_TYPE_PLAY_CHANNEL_RUN_MASTER,
								     "source", source,
								     NULL);
  
  return(play_channel_run_master);
}
