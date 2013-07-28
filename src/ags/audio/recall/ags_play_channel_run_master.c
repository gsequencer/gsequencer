/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/audio/recall/ags_play_channel_run_master.h>

#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_dynamic_connectable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/task/ags_cancel_recall.h>

#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_recycling.h>

#include <stdlib.h>
#include <stdio.h>

void ags_play_channel_run_master_class_init(AgsPlayChannelRunMasterClass *play_channel_run_master);
void ags_play_channel_run_master_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_channel_run_master_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_play_channel_run_master_init(AgsPlayChannelRunMaster *play_channel_run_master);
void ags_play_channel_run_master_set_property(GObject *gobject,
					      guint prop_id,
					      const GValue *value,
					      GParamSpec *param_spec);
void ags_play_channel_run_master_get_property(GObject *gobject,
					      guint prop_id,
					      GValue *value,
					      GParamSpec *param_spec);
void ags_play_channel_run_master_connect(AgsConnectable *connectable);
void ags_play_channel_run_master_disconnect(AgsConnectable *connectable);
void ags_play_channel_run_master_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_play_channel_run_master_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_play_channel_run_master_finalize(GObject *gobject);

void ags_play_channel_run_master_run_init_pre(AgsRecall *recall);
void ags_play_channel_run_master_done(AgsRecall *recall);
void ags_play_channel_run_master_remove(AgsRecall *recall);
void ags_play_channel_run_master_cancel(AgsRecall *recall);
void ags_play_channel_run_master_resolve_dependencies(AgsRecall *recall);
AgsRecall* ags_play_channel_run_master_duplicate(AgsRecall *recall,
						 AgsRecallID *recall_id,
						 guint *n_params, GParameter *parameter);

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
							      AgsPlayChannelRunMasterStreamer *streamer);

enum{
  PROP_0,
  PROP_STREAM_CHANNEL_RUN,
};

static gpointer ags_play_channel_run_master_parent_class = NULL;
static AgsConnectableInterface *ags_play_channel_run_master_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_play_channel_run_master_parent_dynamic_connectable_interface;

GType
ags_play_channel_run_master_get_type()
{
  static GType ags_type_play_channel_run_master = 0;

  if(!ags_type_play_channel_run_master){
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

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_channel_run_master_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_channel_run_master = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
							      "AgsPlayChannelRunMaster\0",
							      &ags_play_channel_run_master_info,
							      0);
    
    g_type_add_interface_static(ags_type_play_channel_run_master,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_channel_run_master,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return(ags_type_play_channel_run_master);
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

  gobject->finalize = ags_play_channel_run_master_finalize;

  /* properties */
  param_spec = g_param_spec_object("stream_channel_run\0",
				   "assigned AgsStreamChannelRun\0",
				   "an assigned AgsStreamChannelRun\0",
				   AGS_TYPE_STREAM_CHANNEL_RUN,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_STREAM_CHANNEL_RUN,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) play_channel_run_master;

  recall->run_init_pre = ags_play_channel_run_master_run_init_pre;
  recall->done = ags_play_channel_run_master_done;
  recall->cancel = ags_play_channel_run_master_cancel;
  recall->resolve_dependencies = ags_play_channel_run_master_resolve_dependencies;
  recall->duplicate = ags_play_channel_run_master_duplicate;
}

void
ags_play_channel_run_master_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_play_channel_run_master_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_play_channel_run_master_connect;
  connectable->disconnect = ags_play_channel_run_master_disconnect;
}

void
ags_play_channel_run_master_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_play_channel_run_master_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_play_channel_run_master_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_play_channel_run_master_disconnect_dynamic;
}

void
ags_play_channel_run_master_init(AgsPlayChannelRunMaster *play_channel_run_master)
{
  AGS_RECALL(play_channel_run_master)->flags |= AGS_RECALL_INPUT_ORIENTATED;
  AGS_RECALL(play_channel_run_master)->child_type = AGS_TYPE_PLAY_RECYCLING;

  play_channel_run_master->flags = 0;

  play_channel_run_master->streamer = NULL;
}

void
ags_play_channel_run_master_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec)
{
  AgsPlayChannelRunMaster *play_channel_run_master;

  play_channel_run_master = AGS_PLAY_CHANNEL_RUN_MASTER(gobject);

  switch(prop_id){
  case PROP_STREAM_CHANNEL_RUN:
    {
      AgsStreamChannelRun *stream_channel_run;
      AgsPlayChannelRunMasterStreamer *streamer;
      gboolean is_template;

      stream_channel_run = (AgsStreamChannelRun *) g_value_get_object(value);

      if(g_list_find(play_channel_run_master->streamer, stream_channel_run) != NULL)
	return;

      if(stream_channel_run != NULL &&
	 (AGS_RECALL_TEMPLATE & (AGS_RECALL(stream_channel_run)->flags)) != 0){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(stream_channel_run != NULL){
	g_object_ref(G_OBJECT(stream_channel_run));

	streamer = ags_play_channel_run_master_streamer_alloc(play_channel_run_master,
							      stream_channel_run);

	if(is_template){
	  ags_recall_add_dependency(AGS_RECALL(play_channel_run_master),
				    ags_recall_dependency_new((GObject *) stream_channel_run));
	}else{
	  if((AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(play_channel_run_master)->flags)) != 0){
	    streamer->done_handler =
	      g_signal_connect(G_OBJECT(stream_channel_run), "done\0",
			       G_CALLBACK(ags_play_channel_run_master_stream_channel_done_callback), play_channel_run_master);
	  }
	}
      }

      play_channel_run_master->streamer = g_list_prepend(play_channel_run_master->streamer, streamer);
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

  play_channel_run_master = AGS_PLAY_CHANNEL_RUN_MASTER(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_channel_run_master_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_play_channel_run_master_parent_class)->finalize(gobject);
}

void
ags_play_channel_run_master_connect(AgsConnectable *connectable)
{
  AgsPlayChannel *play_channel;
  AgsPlayChannelRunMaster *play_channel_run_master;
  GObject *gobject;

  ags_play_channel_run_master_parent_connectable_interface->connect(connectable);

  /* AgsPlayChannelRunMaster */
  play_channel_run_master = AGS_PLAY_CHANNEL_RUN_MASTER(connectable);

  /* AgsPlayChannel */
  play_channel = AGS_PLAY_CHANNEL(AGS_RECALL_CHANNEL_RUN(play_channel_run_master)->recall_channel);

  /* source */
  gobject = G_OBJECT(AGS_RECALL_CHANNEL(play_channel)->source);

  play_channel_run_master->source_recycling_changed_handler =
    g_signal_connect(gobject, "recycling_changed\0",
		     G_CALLBACK(ags_play_channel_run_master_source_recycling_changed_callback), play_channel_run_master);
}

void
ags_play_channel_run_master_disconnect(AgsConnectable *connectable)
{
  AgsPlayChannel *play_channel;
  AgsPlayChannelRunMaster *play_channel_run_master;
  GObject *gobject;

  ags_play_channel_run_master_parent_connectable_interface->disconnect(connectable);

  /* AgsPlayChannelRunMaster */
  play_channel_run_master = AGS_PLAY_CHANNEL_RUN_MASTER(connectable);

  /* AgsPlayChannel */
  play_channel = AGS_PLAY_CHANNEL(AGS_RECALL_CHANNEL_RUN(play_channel_run_master)->recall_channel);

  /* source */
  gobject = G_OBJECT(AGS_RECALL_CHANNEL(play_channel)->source);

  g_signal_handler_disconnect(gobject, play_channel_run_master->source_recycling_changed_handler);
}

void
ags_play_channel_run_master_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsPlayChannelRunMaster *play_channel_run_master;
  GObject *gobject;
  AgsPlayChannelRunMasterStreamer *streamer;
  GList *list;

  ags_play_channel_run_master_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* AgsPlayChannelRunMaster */
  play_channel_run_master = AGS_PLAY_CHANNEL_RUN_MASTER(dynamic_connectable);

  /* stream_channel_run */
  list = play_channel_run_master->streamer;

  while(list != NULL){
    streamer = (AgsPlayChannelRunMasterStreamer *) list->data;

    gobject = G_OBJECT(streamer->stream_channel_run);

    streamer->done_handler =
      g_signal_connect(gobject, "done\0",
		       G_CALLBACK(ags_play_channel_run_master_stream_channel_done_callback), streamer);

    list = list->next;
  }
}

void
ags_play_channel_run_master_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsPlayChannelRunMaster *play_channel_run_master;
  GObject *gobject;
  AgsPlayChannelRunMasterStreamer *streamer;
  GList *list;

  ags_play_channel_run_master_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  /* AgsPlayChannelRunMaster */
  play_channel_run_master = AGS_PLAY_CHANNEL_RUN_MASTER(dynamic_connectable);

  /* stream_channel_run */
  list = play_channel_run_master->streamer;

  while(list != NULL){
    streamer = (AgsPlayChannelRunMasterStreamer *) list->data;
    
    gobject = G_OBJECT(streamer->stream_channel_run);

    g_signal_handler_disconnect(gobject, streamer->done_handler);

    list = list->next;
  }
}

void
ags_play_channel_run_master_run_init_pre(AgsRecall *recall)
{
  AgsRecallChannelRun *recall_channel_run;
  AgsPlayChannelRunMaster *play_channel_run_master;

  AGS_RECALL_CLASS(ags_play_channel_run_master_parent_class)->run_init_pre(recall);

  play_channel_run_master = AGS_PLAY_CHANNEL_RUN_MASTER(recall);
  recall_channel_run = AGS_RECALL_CHANNEL_RUN(recall);

  ags_play_channel_run_master_remap_dependencies(play_channel_run_master,
						 NULL, NULL,
						 recall_channel_run->source->first_recycling, recall_channel_run->source->last_recycling);
}

void
ags_play_channel_run_master_done(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_play_channel_run_master_parent_class)->done(recall);

  /* empty */
}

void 
ags_play_channel_run_master_cancel(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_play_channel_run_master_parent_class)->cancel(recall);

  //TODO:JK: implement me
}

void
ags_play_channel_run_master_resolve_dependencies(AgsRecall *recall)
{
  AgsRecall *template;
  AgsPlayChannelRunMaster *play_channel_run_master;
  AgsRecallDependency *recall_dependency;
  AgsStreamChannelRun *stream_channel_run;
  GList *list;
  AgsGroupId group_id;
  guint i, i_stop;

  play_channel_run_master = AGS_PLAY_CHANNEL_RUN_MASTER(recall);

  template = AGS_RECALL(ags_recall_find_template(AGS_RECALL_CONTAINER(recall->container)->recall_channel_run)->data);

  list = template->dependencies;
  group_id = recall->recall_id->group_id;

  stream_channel_run = NULL;

  i_stop = 1;

  for(i = 0; i < i_stop && list != NULL;){
    recall_dependency = AGS_RECALL_DEPENDENCY(list->data);

    if(AGS_IS_STREAM_CHANNEL_RUN(recall_dependency->dependency)){
      stream_channel_run = (AgsStreamChannelRun *) ags_recall_dependency_resolve(recall_dependency, group_id);

      g_object_set(G_OBJECT(recall),
		   "stream_channel_run\0", stream_channel_run,
		   NULL);

      i++;
    }

    list = list->next;
  }
}

AgsRecall*
ags_play_channel_run_master_duplicate(AgsRecall *recall,
				      AgsRecallID *recall_id,
				      guint *n_params, GParameter *parameter)
{
  AgsPlayChannelRunMaster *play_channel_run_master, *copy;
  
  play_channel_run_master = (AgsPlayChannelRunMaster *) recall;
  copy = (AgsPlayChannelRunMaster *) AGS_RECALL_CLASS(ags_play_channel_run_master_parent_class)->duplicate(recall,
													   recall_id,
													   n_params, parameter);

  return((AgsRecall *) copy);
}

void
ags_play_channel_run_master_remap_dependencies(AgsPlayChannelRunMaster *play_channel_run_master,
					       AgsRecycling *old_start_region, AgsRecycling *old_end_region,
					       AgsRecycling *new_start_region, AgsRecycling *new_end_region)
{
  AgsRecallContainer *container;
  AgsPlayChannelRunMaster *current_master;
  AgsChannel *current;
  AgsRecycling *recycling;
  GList *list, *streamer, *master;

  /* remove old */
  if(old_start_region != NULL){
    current = NULL;
    recycling = old_start_region;
    
    while(recycling != old_end_region->next){
      if(current != AGS_CHANNEL(recycling->channel)){
	current = AGS_CHANNEL(recycling->channel);

	list = ags_recall_get_dependencies(AGS_RECALL(play_channel_run_master));

	while((list = ags_recall_dependency_find_dependency_by_provider(list,
									G_OBJECT(current))) != NULL){
	  /* remove dependency */
	  ags_recall_remove_dependency(AGS_RECALL(play_channel_run_master),
				       AGS_RECALL(AGS_RECALL_DEPENDENCY(list->data)->dependency));

	  /* remove streamer */
	  streamer = ags_play_channel_run_master_find_streamer(play_channel_run_master->streamer,
							       AGS_STREAM_CHANNEL_RUN(AGS_RECALL_DEPENDENCY(list->data)->dependency));

	  if(streamer != NULL){
	    play_channel_run_master->streamer = g_list_remove(play_channel_run_master->streamer,
							      streamer->data);

	  }

	  list = list->next;
	}
      }

      recycling = recycling->next;
    }
  }

  /* add new */
  if(new_start_region != NULL){
    current = NULL;
    recycling = new_start_region;
    
    while(recycling != new_end_region->next){
      if(current != AGS_CHANNEL(recycling->channel)){
	current = AGS_CHANNEL(recycling->channel);

	list = ags_recall_template_find_type(current->play,
					     AGS_TYPE_STREAM_CHANNEL_RUN);

	if(list != NULL){
	  g_object_set(G_OBJECT(play_channel_run_master),
		       "stream_channel_run\0", AGS_STREAM_CHANNEL_RUN(list->data),
		       NULL);

	  container = AGS_RECALL_CONTAINER(AGS_RECALL(play_channel_run_master)->container);

	  master = container->recall_channel_run;

	  while(master != NULL){
	    current_master = AGS_PLAY_CHANNEL_RUN_MASTER(master->data);

	    if((AGS_RECALL_TEMPLATE & (AGS_RECALL(current_master)->flags)) == 0){
	      g_object_set(G_OBJECT(current_master),
			   "stream_channel_run\0", AGS_STREAM_CHANNEL_RUN(list->data),
			   NULL);
	    }
	    
	    master = master->next;
	  }
	}
      }

      recycling = recycling->next;
    }
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
  if((AGS_RECALL_TEMPLATE & (play_channel_run_master->flags)) != 0){
    ags_play_channel_run_master_remap_dependencies(play_channel_run_master,
						   old_start_changed_region, old_end_changed_region,
						   new_start_changed_region, new_end_changed_region);
  }
}

void
ags_play_channel_run_master_stream_channel_done_callback(AgsRecall *recall,
							 AgsPlayChannelRunMasterStreamer *streamer)
{
  AgsPlayChannelRunMaster *play_channel_run_master;

  play_channel_run_master = streamer->play_channel_run_master;

  play_channel_run_master->streamer = g_list_remove(play_channel_run_master->streamer,
						    streamer);

  if(play_channel_run_master->streamer == NULL){
    //    play_channel_run_master->flags |= AGS_PLAY_CHANNEL_RUN_MASTER_TERMINATING;
  }
}

AgsPlayChannelRunMasterStreamer*
ags_play_channel_run_master_streamer_alloc(AgsPlayChannelRunMaster *play_channel_run_master,
					   AgsStreamChannelRun *stream_channel_run)
{
  AgsPlayChannelRunMasterStreamer *streamer;

  streamer = (AgsPlayChannelRunMasterStreamer *) malloc(sizeof(AgsPlayChannelRunMasterStreamer));

  streamer->play_channel_run_master = play_channel_run_master;
  streamer->stream_channel_run = stream_channel_run;

  return(streamer);
}

GList*
ags_play_channel_run_master_find_streamer(GList *list,
					  AgsStreamChannelRun *stream_channel_run)
{
  while(list != NULL){
    if(AGS_PLAY_CHANNEL_RUN_MASTER_STREAMER(list->data)->stream_channel_run == stream_channel_run)
      return(list);

    list = list->next;
  }

  return(NULL);
}

AgsPlayChannelRunMaster*
ags_play_channel_run_master_new()
{
  AgsPlayChannelRunMaster *play_channel_run_master;

  play_channel_run_master = (AgsPlayChannelRunMaster *) g_object_new(AGS_TYPE_PLAY_CHANNEL_RUN_MASTER,
								     NULL);
  
  return(play_channel_run_master);
}
