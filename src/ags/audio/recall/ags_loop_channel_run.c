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

#include <ags/audio/recall/ags_loop_channel_run.h>

#include <ags/object/ags_dynamic_connectable.h>

#include <ags/main.h>

#include <ags/util/ags_id_generator.h>

#include <ags/lib/ags_list.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_countable.h>
#include <ags/object/ags_plugin.h>

#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/recall/ags_delay_audio.h>

#include <math.h>

void ags_loop_channel_run_class_init(AgsLoopChannelRunClass *loop_channel_run);
void ags_loop_channel_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_loop_channel_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_loop_channel_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_loop_channel_run_init(AgsLoopChannelRun *loop_channel_run);
void ags_loop_channel_run_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_loop_channel_run_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_loop_channel_run_finalize(GObject *gobject);
void ags_loop_channel_run_connect(AgsConnectable *connectable);
void ags_loop_channel_run_disconnect(AgsConnectable *connectable);
void ags_loop_channel_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_loop_channel_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_loop_channel_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_loop_channel_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_loop_channel_run_resolve_dependencies(AgsRecall *recall);
AgsRecall* ags_loop_channel_run_duplicate(AgsRecall *recall,
					  AgsRecallID *recall_id,
					  guint *n_params, GParameter *parameter);

void ags_loop_channel_run_sequencer_duration_changed_callback(AgsDelayAudio *delay_audio,
							      AgsLoopChannelRun *loop_channel_run);

void ags_loop_channel_run_start_callback(AgsCountBeatsAudioRun *count_beats_audio_run,
					 guint run_order,
					 AgsLoopChannelRun *loop_channel_run);
void ags_loop_channel_run_loop_callback(AgsCountBeatsAudioRun *count_beats_audio_run,
					guint run_order,
					AgsLoopChannelRun *loop_channel_run);
void ags_loop_channel_run_stop_callback(AgsCountBeatsAudioRun *count_beats_audio_run,
					guint run_order,
					AgsLoopChannelRun *loop_channel_run);

void ags_loop_channel_run_write_resolve_dependency(AgsFileLookup *file_lookup,
						   AgsPlugin *plugin);
void ags_loop_channel_run_read_resolve_dependency(AgsFileLookup *file_lookup,
						  AgsPlugin *plugin);

enum{
  PROP_0,
  PROP_COUNT_BEATS_AUDIO_RUN,
};

static gpointer ags_loop_channel_run_parent_class = NULL;
static AgsConnectableInterface *ags_loop_channel_run_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_loop_channel_run_parent_dynamic_connectable_interface;
static AgsPluginInterface *ags_loop_channel_run_parent_plugin_interface;

GType
ags_loop_channel_run_get_type()
{
  static GType ags_type_loop_channel_run = 0;

  if(!ags_type_loop_channel_run){
    static const GTypeInfo ags_loop_channel_run_info = {
      sizeof (AgsLoopChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_loop_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLoopChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_loop_channel_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_loop_channel_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_loop_channel_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_loop_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
						       "AgsLoopChannelRun\0",
						       &ags_loop_channel_run_info,
						       0);
    g_type_add_interface_static(ags_type_loop_channel_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_loop_channel_run,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return (ags_type_loop_channel_run);
}

void
ags_loop_channel_run_class_init(AgsLoopChannelRunClass *loop_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_loop_channel_run_parent_class = g_type_class_peek_parent(loop_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) loop_channel_run;

  gobject->set_property = ags_loop_channel_run_set_property;
  gobject->get_property = ags_loop_channel_run_get_property;

  gobject->finalize = ags_loop_channel_run_finalize;

  /* properties */
  param_spec = g_param_spec_object("count-beats-audio-run\0",
				   "assigned AgsCountBeatsAudioRun\0",
				   "The pointer to a counter object which indicates when looping should happen\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COUNT_BEATS_AUDIO_RUN,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) loop_channel_run;

  recall->duplicate = ags_loop_channel_run_duplicate;
  recall->resolve_dependencies = ags_loop_channel_run_resolve_dependencies;
}

void
ags_loop_channel_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_loop_channel_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_loop_channel_run_connect;
  connectable->disconnect = ags_loop_channel_run_disconnect;
}

void
ags_loop_channel_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_loop_channel_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_loop_channel_run_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_loop_channel_run_disconnect_dynamic;
}

void
ags_loop_channel_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_loop_channel_run_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->read = ags_loop_channel_run_read;
  plugin->write = ags_loop_channel_run_write;
}

void
ags_loop_channel_run_init(AgsLoopChannelRun *loop_channel_run)
{
  AGS_RECALL(loop_channel_run)->name = "ags-loop\0";
  AGS_RECALL(loop_channel_run)->version = AGS_EFFECTS_DEFAULT_VERSION;
  AGS_RECALL(loop_channel_run)->build_id = AGS_BUILD_ID;
  AGS_RECALL(loop_channel_run)->xml_type = "ags-loop-channel-run\0";
  AGS_RECALL(loop_channel_run)->port = NULL;

  AGS_RECALL(loop_channel_run)->flags |= AGS_RECALL_INPUT_ORIENTATED;
  AGS_RECALL(loop_channel_run)->child_type = G_TYPE_NONE;

  loop_channel_run->count_beats_audio_run = NULL;
}

void
ags_loop_channel_run_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsLoopChannelRun *loop_channel_run;

  loop_channel_run = AGS_LOOP_CHANNEL_RUN(gobject);

  switch(prop_id){
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      AgsCountBeatsAudioRun *count_beats_audio_run;
      gboolean is_template;

      count_beats_audio_run = (AgsCountBeatsAudioRun *) g_value_get_object(value);

      if(loop_channel_run->count_beats_audio_run == count_beats_audio_run)
	return;

      if(count_beats_audio_run != NULL &&
	 (AGS_RECALL_TEMPLATE & (AGS_RECALL(count_beats_audio_run)->flags)) != 0){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }


      if(loop_channel_run->count_beats_audio_run != NULL){
	if(is_template){
	  ags_recall_remove_dependency(AGS_RECALL(loop_channel_run),
				       (AgsRecall *) loop_channel_run->count_beats_audio_run);
	}else{
	  if((AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(loop_channel_run)->flags)) != 0){
	    /* AgsCountBeatsAudioRun */
	    g_signal_handler_disconnect(G_OBJECT(loop_channel_run->count_beats_audio_run),
					loop_channel_run->start_handler);
	    g_signal_handler_disconnect(G_OBJECT(loop_channel_run->count_beats_audio_run),
					loop_channel_run->loop_handler);
	    g_signal_handler_disconnect(G_OBJECT(loop_channel_run->count_beats_audio_run),
					loop_channel_run->stop_handler);
	  }

	  g_object_unref(loop_channel_run->count_beats_audio_run);
	}
      }

      if(count_beats_audio_run != NULL){
	g_object_ref(count_beats_audio_run);
	
	if(is_template){
	  ags_recall_add_dependency(AGS_RECALL(loop_channel_run),
				    ags_recall_dependency_new((GObject *) count_beats_audio_run));
	}else{
	  if((AGS_RECALL_RUN_INITIALIZED & (AGS_RECALL(loop_channel_run)->flags)) != 0){
	    /* AgsCountBeatsAudioRun */
	    loop_channel_run->start_handler =
	      g_signal_connect(G_OBJECT(loop_channel_run->count_beats_audio_run), "sequencer-start\0",
			       G_CALLBACK(ags_loop_channel_run_loop_callback), loop_channel_run);
	    
	    loop_channel_run->loop_handler =
	      g_signal_connect(G_OBJECT(loop_channel_run->count_beats_audio_run), "sequencer-loop\0",
			       G_CALLBACK(ags_loop_channel_run_loop_callback), loop_channel_run);
	    loop_channel_run->stop_handler =
	      g_signal_connect(G_OBJECT(loop_channel_run->count_beats_audio_run), "sequencer-stop\0",
			       G_CALLBACK(ags_loop_channel_run_loop_callback), loop_channel_run);
	  }
	}
      }

      loop_channel_run->count_beats_audio_run = count_beats_audio_run;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_loop_channel_run_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsLoopChannelRun *loop_channel_run;

  loop_channel_run = AGS_LOOP_CHANNEL_RUN(gobject);

  switch(prop_id){
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      g_value_set_object(value, loop_channel_run->count_beats_audio_run);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_loop_channel_run_finalize(GObject *gobject)
{
  AgsLoopChannelRun *loop_channel_run;

  loop_channel_run = AGS_LOOP_CHANNEL_RUN(gobject);

  if(loop_channel_run->count_beats_audio_run != NULL){
    g_object_unref(G_OBJECT(loop_channel_run->count_beats_audio_run));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_loop_channel_run_parent_class)->finalize(gobject);
}

void
ags_loop_channel_run_connect(AgsConnectable *connectable)
{
  ags_loop_channel_run_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_loop_channel_run_disconnect(AgsConnectable *connectable)
{
  ags_loop_channel_run_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_loop_channel_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsLoopChannelRun *loop_channel_run;

  ags_loop_channel_run_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  loop_channel_run = AGS_LOOP_CHANNEL_RUN(dynamic_connectable);

  if(loop_channel_run->count_beats_audio_run != NULL){
    /* AgsCountBeatsAudioRun */
    loop_channel_run->start_handler =
      g_signal_connect(G_OBJECT(loop_channel_run->count_beats_audio_run), "sequencer-start\0",
		       G_CALLBACK(ags_loop_channel_run_start_callback), loop_channel_run);
    
    loop_channel_run->loop_handler =
      g_signal_connect(G_OBJECT(loop_channel_run->count_beats_audio_run), "sequencer-loop\0",
		       G_CALLBACK(ags_loop_channel_run_loop_callback), loop_channel_run);
    
    loop_channel_run->stop_handler =
      g_signal_connect(G_OBJECT(loop_channel_run->count_beats_audio_run), "sequencer-stop\0",
		       G_CALLBACK(ags_loop_channel_run_stop_callback), loop_channel_run);
  }
}

void
ags_loop_channel_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsLoopChannelRun *loop_channel_run;

  ags_loop_channel_run_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  loop_channel_run = AGS_LOOP_CHANNEL_RUN(dynamic_connectable);

  if(loop_channel_run->count_beats_audio_run != NULL){
    /* AgsCountBeatsAudioRun */
    g_signal_handler_disconnect(G_OBJECT(loop_channel_run->count_beats_audio_run),
				loop_channel_run->start_handler);
    g_signal_handler_disconnect(G_OBJECT(loop_channel_run->count_beats_audio_run),
				loop_channel_run->loop_handler);
    g_signal_handler_disconnect(G_OBJECT(loop_channel_run->count_beats_audio_run),
				loop_channel_run->stop_handler);
  }
}

void
ags_loop_channel_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsFileLookup *file_lookup;
  xmlNode *iter;

  /* read parent */
  ags_loop_channel_run_parent_plugin_interface->read(file, node, plugin);

  /* read depenendency */
  iter = node->parent->parent->children;

  while(iter != NULL){
    if(iter->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(iter->name,
		     "ags-dependency-list\0",
		     19)){
	xmlNode *dependency_node;

	dependency_node = iter->children;

	while(dependency_node != NULL){
	  if(dependency_node->type == XML_ELEMENT_NODE){
	    file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
							 "file\0", file,
							 "node\0", dependency_node,
							 "reference\0", plugin,
							 NULL);
	    ags_file_add_lookup(file, (GObject *) file_lookup);
	    g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
			     G_CALLBACK(ags_loop_channel_run_read_resolve_dependency), plugin);
	  }
	  
	  dependency_node = dependency_node->next;
	}
      }
    }

    iter = iter->next;
  }
}

xmlNode*
ags_loop_channel_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsFileLookup *file_lookup;
  xmlNode *node, *child;
  xmlNode *dependency_node;
  GList *list;

  /* write parent */
  node = ags_loop_channel_run_parent_plugin_interface->write(file, parent, plugin);

  /* write dependencies */
  child = xmlNewNode(NULL,
		     "ags-dependency-list\0");

  xmlNewProp(child,
	     AGS_FILE_ID_PROP,
	     ags_id_generator_create_uuid());

  list = AGS_RECALL(plugin)->dependencies;

  while(list != NULL){
    dependency_node = xmlNewNode(NULL,
				 "ags-dependency\0");

    xmlNewProp(dependency_node,
	       AGS_FILE_ID_PROP,
	       ags_id_generator_create_uuid());

    xmlAddChild(child,
		dependency_node);

    file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
						 "file\0", file,
						 "node\0", dependency_node,
						 "reference\0", list->data,
						 NULL);
    ags_file_add_lookup(file, (GObject *) file_lookup);
    g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		     G_CALLBACK(ags_loop_channel_run_write_resolve_dependency), plugin);

    list = list->next;
  }

  xmlAddChild(parent->parent,
	      child);

  return(node);
}

void
ags_loop_channel_run_resolve_dependencies(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsChannel *channel;
  AgsRecall *template;
  AgsLoopChannelRun *loop_channel_run;
  AgsRecallDependency *recall_dependency;
  AgsCountBeatsAudioRun *count_beats_audio_run;
  GList *list;
  AgsRecallID *recall_id;
  guint i, i_stop;

  loop_channel_run = AGS_LOOP_CHANNEL_RUN(recall);

  template = AGS_RECALL(ags_recall_find_template(AGS_RECALL_CONTAINER(recall->container)->recall_channel_run)->data);

  list = template->dependencies;

  channel = AGS_RECALL_CHANNEL_RUN(loop_channel_run)->source;
  audio = AGS_AUDIO(channel->audio);

  recall_id = recall->recall_id;

  count_beats_audio_run = NULL;
  i_stop = 1;

  for(i = 0; i < i_stop && list != NULL;){
    recall_dependency = AGS_RECALL_DEPENDENCY(list->data);

    if(AGS_IS_COUNT_BEATS_AUDIO_RUN(recall_dependency->dependency)){
      count_beats_audio_run = (AgsCountBeatsAudioRun *) ags_recall_dependency_resolve(recall_dependency, recall_id);

      i++;
    }

    list = list->next;
  }
  
  g_object_set(G_OBJECT(recall),
	       "count-beats-audio-run\0", count_beats_audio_run,
	       NULL);
}

AgsRecall*
ags_loop_channel_run_duplicate(AgsRecall *recall,
			       AgsRecallID *recall_id,
			       guint *n_params, GParameter *parameter)
{
  AgsLoopChannelRun *loop_channel_run, *copy;

  loop_channel_run = AGS_LOOP_CHANNEL_RUN(recall);
  copy = (AgsLoopChannelRun *) AGS_RECALL_CLASS(ags_loop_channel_run_parent_class)->duplicate(recall,
											      recall_id,
											      n_params, parameter);
  
  return((AgsRecall *) copy);
}

void
ags_loop_channel_run_create_audio_signals(AgsLoopChannelRun *loop_channel_run)
{
  AgsDevout *devout;
  AgsRecycling *recycling;
  AgsAudioSignal *audio_signal;
  guint delay, attack;
  guint tic_counter_incr;

  //  g_message("debug\0");

  devout = AGS_DEVOUT(AGS_RECALL(loop_channel_run)->devout);

  /* recycling */
  recycling = AGS_RECALL_CHANNEL_RUN(loop_channel_run)->source->first_recycling;

  /* delay and attack */
  tic_counter_incr = devout->tic_counter + 1;

  attack = devout->attack[((tic_counter_incr == AGS_NOTATION_TICS_PER_BEAT) ?
			   0:
			   tic_counter_incr)];
  delay = devout->delay[((tic_counter_incr == AGS_NOTATION_TICS_PER_BEAT) ?
			 0:
			 tic_counter_incr)];

  while(recycling != AGS_RECALL_CHANNEL_RUN(loop_channel_run)->source->last_recycling->next){
    audio_signal = ags_audio_signal_new((GObject *) devout,
					(GObject *) recycling,
					(GObject *) AGS_RECALL(loop_channel_run)->recall_id);
    audio_signal->stream_current = audio_signal->stream_beginning;
    
    /*  */
    ags_recycling_create_audio_signal_with_defaults(recycling,
						    audio_signal,
						    delay, attack);
    audio_signal->stream_current = audio_signal->stream_beginning;

    ags_connectable_connect(AGS_CONNECTABLE(audio_signal));    
    ags_recycling_add_audio_signal(recycling,
				   audio_signal);

    g_message("+++++++++++++++++++++++++\n\nloop channel created: AgsAudioSignal#%llx[%u]\n\n+++++++++++++++++++++++++\0",
	      (long long unsigned int) audio_signal, audio_signal->length);

    recycling = recycling->next;
  }
}

void
ags_loop_channel_run_start_callback(AgsCountBeatsAudioRun *count_beats_audio_run,
				    guint run_order,
				    AgsLoopChannelRun *loop_channel_run)
{
  
  g_message("ags_loop_channel_run_start_callback - run_order: %u; %u\n\0",
	    AGS_RECALL_CHANNEL_RUN(loop_channel_run)->run_order,
	    run_order);

  if(AGS_RECALL_CHANNEL_RUN(loop_channel_run)->run_order == run_order){
    ags_loop_channel_run_create_audio_signals(loop_channel_run);
  }
}

void 
ags_loop_channel_run_loop_callback(AgsCountBeatsAudioRun *count_beats_audio_run,
				   guint run_order,
				   AgsLoopChannelRun *loop_channel_run)
{
  g_message("ags_loop_channel_run_loop_callback - run_order: %u; %u\0",
	    AGS_RECALL_CHANNEL_RUN(loop_channel_run)->run_order,
	    run_order);
  
  if(AGS_RECALL_CHANNEL_RUN(loop_channel_run)->run_order == run_order){
    ags_loop_channel_run_create_audio_signals(loop_channel_run);
  }
}

void 
ags_loop_channel_run_stop_callback(AgsCountBeatsAudioRun *count_beats_audio_run,
				   guint run_order,
				   AgsLoopChannelRun *loop_channel_run)
{
}

void
ags_loop_channel_run_write_resolve_dependency(AgsFileLookup *file_lookup,
					      AgsPlugin *plugin)
{
  AgsFileIdRef *id_ref;
  gchar *id;

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_reference(file_lookup->file, file_lookup->ref);

  id = xmlGetProp(id_ref->node, AGS_FILE_ID_PROP);

  xmlNewProp(file_lookup->node,
	     "xpath\0",
	     g_strdup_printf("xpath=*/[@id='%s']\0", id));
}

void
ags_loop_channel_run_read_resolve_dependency(AgsFileLookup *file_lookup,
					     AgsPlugin *plugin)
{
  AgsFileIdRef *id_ref;
  gchar *xpath;

  xpath = (gchar *) xmlGetProp(file_lookup->node,
			       "xpath\0");

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_xpath(file_lookup->file, xpath);

  if(AGS_IS_COUNT_BEATS_AUDIO_RUN(id_ref->ref)){
    g_object_set(G_OBJECT(plugin),
		 "count-beats-audio-run\0", id_ref->ref,
		 NULL);
  }
}

AgsLoopChannelRun*
ags_loop_channel_run_new(AgsChannel *channel,
			 AgsCountBeatsAudioRun *count_beats_audio_run,
			 gboolean is_template)
{
  AgsLoopChannelRun *loop_channel_run;

  loop_channel_run = (AgsLoopChannelRun *) g_object_new(AGS_TYPE_LOOP_CHANNEL_RUN,
							"source\0", channel,
							"count-beats-audio-run\0", count_beats_audio_run,
							NULL);

  if(is_template){
    GList *list;

    AGS_RECALL(loop_channel_run)->flags = AGS_RECALL_TEMPLATE;

    list = NULL;
    //    list = g_list_prepend(list, ags_recall_dependency_new(G_OBJECT(count_beats_audio_run)));
  }

  return(loop_channel_run);
}
