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

#include <ags/audio/recall/ags_play_channel_run.h>

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

void ags_play_channel_run_class_init(AgsPlayChannelRunClass *play_channel_run);
void ags_play_channel_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_channel_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_play_channel_run_init(AgsPlayChannelRun *play_channel_run);
void ags_play_channel_run_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_play_channel_run_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_play_channel_run_connect(AgsConnectable *connectable);
void ags_play_channel_run_disconnect(AgsConnectable *connectable);
void ags_play_channel_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_play_channel_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_play_channel_run_finalize(GObject *gobject);

void ags_play_channel_run_run_init_pre(AgsRecall *recall);
void ags_play_channel_run_done(AgsRecall *recall);
void ags_play_channel_run_remove(AgsRecall *recall);
void ags_play_channel_run_cancel(AgsRecall *recall);
void ags_play_channel_run_resolve_dependencies(AgsRecall *recall);
AgsRecall* ags_play_channel_run_duplicate(AgsRecall *recall,
					  AgsRecallID *recall_id,
					  guint *n_params, GParameter *parameter);

void ags_play_channel_run_stream_channel_done_callback(AgsRecall *recall,
						       AgsPlayChannelRun *play_channel_run);

enum{
  PROP_0,
  PROP_STREAM_CHANNEL_RUN,
};

static gpointer ags_play_channel_run_parent_class = NULL;
static AgsConnectableInterface *ags_play_channel_run_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_play_channel_run_parent_dynamic_connectable_interface;

GType
ags_play_channel_run_get_type()
{
  static GType ags_type_play_channel_run = 0;

  if(!ags_type_play_channel_run){
    static const GTypeInfo ags_play_channel_run_info = {
      sizeof (AgsPlayChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_channel_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_channel_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_channel_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
						       "AgsPlayChannelRun\0",
						       &ags_play_channel_run_info,
						       0);

    g_type_add_interface_static(ags_type_play_channel_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_channel_run,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return(ags_type_play_channel_run);
}

void
ags_play_channel_run_class_init(AgsPlayChannelRunClass *play_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_play_channel_run_parent_class = g_type_class_peek_parent(play_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) play_channel_run;

  gobject->set_property = ags_play_channel_run_set_property;
  gobject->get_property = ags_play_channel_run_get_property;

  gobject->finalize = ags_play_channel_run_finalize;

  /* properties */
  param_spec = g_param_spec_object("stream_channel_run\0",
				   "assigned AgsStreamChannelRun\0",
				   "the assigned AgsStreamChannelRun\0",
				   AGS_TYPE_STREAM_CHANNEL_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_STREAM_CHANNEL_RUN,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) play_channel_run;

  recall->run_init_pre = ags_play_channel_run_run_init_pre;
  recall->done = ags_play_channel_run_done;
  recall->cancel = ags_play_channel_run_cancel;
  recall->resolve_dependencies = ags_play_channel_run_resolve_dependencies;
  recall->duplicate = ags_play_channel_run_duplicate;
}

void
ags_play_channel_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_play_channel_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_play_channel_run_connect;
  connectable->disconnect = ags_play_channel_run_disconnect;
}

void
ags_play_channel_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_play_channel_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_play_channel_run_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_play_channel_run_disconnect_dynamic;
}

void
ags_play_channel_run_init(AgsPlayChannelRun *play_channel_run)
{
  AGS_RECALL(play_channel_run)->flags |= AGS_RECALL_INPUT_ORIENTATED;
  AGS_RECALL(play_channel_run)->child_type = AGS_TYPE_PLAY_RECYCLING;

  play_channel_run->flags = 0;

  play_channel_run->stream_channel_run = NULL;
}

void
ags_play_channel_run_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsPlayChannelRun *play_channel_run;

  play_channel_run = AGS_PLAY_CHANNEL_RUN(gobject);

  switch(prop_id){
  case PROP_STREAM_CHANNEL_RUN:
    {
      AgsStreamChannelRun *stream_channel_run;
      gboolean is_template;

      stream_channel_run = (AgsStreamChannelRun *) g_value_get_object(value);

      if(stream_channel_run == play_channel_run->stream_channel_run)
	return;

      if(stream_channel_run != NULL &&
	 (AGS_RECALL_TEMPLATE & (AGS_RECALL(stream_channel_run)->flags)) != 0){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(play_channel_run->stream_channel_run != NULL){
	if(is_template){
	  ags_recall_remove_dependency(AGS_RECALL(play_channel_run),
				       (AgsRecall *) play_channel_run->stream_channel_run);
	}else{
	  if((AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(play_channel_run)->flags)) != 0){
	    g_signal_handler_disconnect(G_OBJECT(play_channel_run),
					play_channel_run->done_handler);
	  }
	}

	g_object_unref(G_OBJECT(play_channel_run->stream_channel_run));
      }

      if(stream_channel_run != NULL){
	g_object_ref(G_OBJECT(stream_channel_run));

	if(is_template){
	  ags_recall_add_dependency(AGS_RECALL(play_channel_run),
				    ags_recall_dependency_new((GObject *) stream_channel_run));
	}else{
	  if((AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(play_channel_run)->flags)) != 0){
	    play_channel_run->done_handler =
	      g_signal_connect(G_OBJECT(stream_channel_run), "done\0",
			       G_CALLBACK(ags_play_channel_run_stream_channel_done_callback), play_channel_run);
	  }
	}
      }

      play_channel_run->stream_channel_run = stream_channel_run;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_channel_run_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsPlayChannelRun *play_channel_run;

  play_channel_run = AGS_PLAY_CHANNEL_RUN(gobject);

  switch(prop_id){
  case PROP_STREAM_CHANNEL_RUN:
    {
      g_value_set_object(value, G_OBJECT(play_channel_run->stream_channel_run));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_channel_run_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_play_channel_run_parent_class)->finalize(gobject);
}

void
ags_play_channel_run_connect(AgsConnectable *connectable)
{
  AgsPlayChannelRun *play_channel_run;

  ags_play_channel_run_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_play_channel_run_disconnect(AgsConnectable *connectable)
{
  ags_play_channel_run_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_play_channel_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsPlayChannel *play_channel;
  AgsPlayChannelRun *play_channel_run;
  GObject *gobject;

  ags_play_channel_run_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* AgsPlayChannelRun */
  play_channel_run = AGS_PLAY_CHANNEL_RUN(dynamic_connectable);

  /* stream_channel_run */
  gobject = G_OBJECT(play_channel_run->stream_channel_run);

  play_channel_run->done_handler =
    g_signal_connect(gobject, "done\0",
		     G_CALLBACK(ags_play_channel_run_stream_channel_done_callback), play_channel_run);
}

void
ags_play_channel_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsPlayChannel *play_channel;
  AgsPlayChannelRun *play_channel_run;
  GObject *gobject;

  ags_play_channel_run_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  /* AgsPlayChannelRun */
  play_channel_run = AGS_PLAY_CHANNEL_RUN(dynamic_connectable);

  /* AgsPlayChannel */
  play_channel = AGS_PLAY_CHANNEL(AGS_RECALL_CHANNEL_RUN(play_channel_run)->recall_channel);

  /* stream_channel_run */
  gobject = G_OBJECT(play_channel_run->stream_channel_run);

  g_signal_handler_disconnect(gobject, play_channel_run->done_handler);
}

void
ags_play_channel_run_run_init_pre(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_play_channel_run_parent_class)->run_init_pre(recall);

  /* empty */
}

void
ags_play_channel_run_done(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_play_channel_run_parent_class)->done(recall);

  /* empty */
}

void 
ags_play_channel_run_cancel(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_play_channel_run_parent_class)->cancel(recall);

  /* empty */
}

void
ags_play_channel_run_resolve_dependencies(AgsRecall *recall)
{
  AgsRecall *template;
  AgsPlayChannelRun *play_channel_run;
  AgsRecallDependency *recall_dependency;
  AgsStreamChannelRun *stream_channel_run;
  GList *list;
  AgsGroupId group_id;
  guint i, i_stop;

  play_channel_run = AGS_PLAY_CHANNEL_RUN(recall);

  template = AGS_RECALL(ags_recall_find_template(AGS_RECALL_CONTAINER(recall->container)->recall_channel_run)->data);

  list = template->dependencies;
  group_id = recall->recall_id->group_id;

  stream_channel_run = NULL;

  i_stop = 1;

  for(i = 0; i < i_stop && list != NULL;){
    recall_dependency = AGS_RECALL_DEPENDENCY(list->data);

    if(AGS_IS_STREAM_CHANNEL_RUN(recall_dependency->dependency)){
      stream_channel_run = (AgsStreamChannelRun *) ags_recall_dependency_resolve(recall_dependency, group_id);

      i++;
    }

    list = list->next;
  }

  g_object_set(G_OBJECT(recall),
	       "stream_channel_run\0", stream_channel_run,
	       NULL);
}

AgsRecall*
ags_play_channel_run_duplicate(AgsRecall *recall,
			       AgsRecallID *recall_id,
			       guint *n_params, GParameter *parameter)
{
  AgsPlayChannelRun *play_channel_run, *copy;
  
  play_channel_run = (AgsPlayChannelRun *) recall;
  copy = (AgsPlayChannelRun *) AGS_RECALL_CLASS(ags_play_channel_run_parent_class)->duplicate(recall,
											      recall_id,
											      n_params, parameter);

  return((AgsRecall *) copy);
}

void
ags_play_channel_run_stream_channel_done_callback(AgsRecall *recall,
						  AgsPlayChannelRun *play_channel_run)
{
  play_channel_run->flags |= AGS_PLAY_CHANNEL_RUN_TERMINATING;
}

AgsPlayChannelRun*
ags_play_channel_run_new(AgsStreamChannelRun *stream_channel_run)
{
  AgsPlayChannelRun *play_channel_run;

  play_channel_run = (AgsPlayChannelRun *) g_object_new(AGS_TYPE_PLAY_CHANNEL_RUN,
							"stream_channel_run\0", stream_channel_run,
							NULL);
  
  return(play_channel_run);
}
