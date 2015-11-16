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

#include <ags/audio/recall/ags_route_dssi_audio_run.h>
#include <ags/audio/recall/ags_route_dssi_audio.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_dynamic_connectable.h>
#include <ags/object/ags_plugin.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>

#include <ags/thread/ags_timestamp_thread.h>

#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_dssi_run.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>

void ags_route_dssi_audio_run_class_init(AgsRouteDssiAudioRunClass *route_dssi_audio_run);
void ags_route_dssi_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_route_dssi_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_route_dssi_audio_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_route_dssi_audio_run_init(AgsRouteDssiAudioRun *route_dssi_audio_run);
void ags_route_dssi_audio_run_set_property(GObject *gobject,
					   guint prop_id,
					   const GValue *value,
					   GParamSpec *param_spec);
void ags_route_dssi_audio_run_get_property(GObject *gobject,
					   guint prop_id,
					   GValue *value,
					   GParamSpec *param_spec);
void ags_route_dssi_audio_run_finalize(GObject *gobject);
void ags_route_dssi_audio_run_connect(AgsConnectable *connectable);
void ags_route_dssi_audio_run_disconnect(AgsConnectable *connectable);
void ags_route_dssi_audio_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_route_dssi_audio_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_route_dssi_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_route_dssi_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_route_dssi_audio_run_resolve_dependencies(AgsRecall *recall);
AgsRecall* ags_route_dssi_audio_run_duplicate(AgsRecall *recall,
					      AgsRecallID *recall_id,
					      guint *n_params, GParameter *parameter);
void ags_route_dssi_audio_run_run_pre(AgsRecall *recall);
void ags_route_dssi_audio_run_run_post(AgsRecall *recall);

void ags_route_dssi_audio_run_alloc_input_callback(AgsDelayAudioRun *delay_audio_run,
						   guint nth_run,
						   gdouble delay, guint attack,
						   AgsRouteDssiAudioRun *route_dssi_audio_run);

void ags_route_dssi_audio_run_write_resolve_dependency(AgsFileLookup *file_lookup,
						       GObject *recall);
void ags_route_dssi_audio_run_read_resolve_dependency(AgsFileLookup *file_lookup,
						      GObject *recall);

/**
 * SECTION:ags_route_dssi_audio_run
 * @short_description: route MIDI
 * @title: AgsRouteDssiAudioRun
 * @section_id:
 * @include: ags/audio/recall/ags_route_dssi_audio_run.h
 *
 * The #AgsRouteDssiAudioRun routes MIDI to DSSI recall.
 */

enum{
  PROP_0,
  PROP_DELAY_AUDIO_RUN,
  PROP_COUNT_BEATS_AUDIO_RUN,
};

static gpointer ags_route_dssi_audio_run_parent_class = NULL;
static AgsConnectableInterface* ags_route_dssi_audio_run_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_route_dssi_audio_run_parent_dynamic_connectable_interface;
static AgsPluginInterface *ags_route_dssi_audio_run_parent_plugin_interface;

GType
ags_route_dssi_audio_run_get_type()
{
  static GType ags_type_route_dssi_audio_run = 0;

  if(!ags_type_route_dssi_audio_run){
    static const GTypeInfo ags_route_dssi_audio_run_info = {
      sizeof (AgsRouteDssiAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_route_dssi_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRouteDssiAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_route_dssi_audio_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_route_dssi_audio_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_route_dssi_audio_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_route_dssi_audio_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_route_dssi_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
							   "AgsRouteDssiAudioRun\0",
							   &ags_route_dssi_audio_run_info,
							   0);
    
    g_type_add_interface_static(ags_type_route_dssi_audio_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_route_dssi_audio_run,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);

    g_type_add_interface_static(ags_type_route_dssi_audio_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_route_dssi_audio_run);
}

void
ags_route_dssi_audio_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_route_dssi_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_route_dssi_audio_run_connect;
  connectable->disconnect = ags_route_dssi_audio_run_disconnect;
}

void
ags_route_dssi_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_route_dssi_audio_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_route_dssi_audio_run_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_route_dssi_audio_run_disconnect_dynamic;
}

void
ags_route_dssi_audio_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_route_dssi_audio_run_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->read = ags_route_dssi_audio_run_read;
  plugin->write = ags_route_dssi_audio_run_write;
}

void
ags_route_dssi_audio_run_class_init(AgsRouteDssiAudioRunClass *route_dssi_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_route_dssi_audio_run_parent_class = g_type_class_peek_parent(route_dssi_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) route_dssi_audio_run;

  gobject->set_property = ags_route_dssi_audio_run_set_property;
  gobject->get_property = ags_route_dssi_audio_run_get_property;

  gobject->finalize = ags_route_dssi_audio_run_finalize;

  /* properties */
  param_spec = g_param_spec_object("delay-audio-run\0",
				   "assigned AgsDelayAudioRun\0",
				   "the AgsDelayAudioRun which emits notation_alloc_input signal\0",
				   AGS_TYPE_DELAY_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY_AUDIO_RUN,
				  param_spec);

  param_spec = g_param_spec_object("count-beats-audio-run\0",
				   "assigned AgsCountBeatsAudioRun\0",
				   "the AgsCountBeatsAudioRun which just counts\0",
				   AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COUNT_BEATS_AUDIO_RUN,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) route_dssi_audio_run;

  recall->resolve_dependencies = ags_route_dssi_audio_run_resolve_dependencies;
  recall->duplicate = ags_route_dssi_audio_run_duplicate;
  recall->run_pre = ags_route_dssi_audio_run_run_pre;
  recall->run_post = ags_route_dssi_audio_run_run_post;
}

void
ags_route_dssi_audio_run_init(AgsRouteDssiAudioRun *route_dssi_audio_run)
{
  AGS_RECALL(route_dssi_audio_run)->name = "ags-play-notation\0";
  AGS_RECALL(route_dssi_audio_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(route_dssi_audio_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(route_dssi_audio_run)->xml_type = "ags-play-notation-audio-run\0";
  AGS_RECALL(route_dssi_audio_run)->port = NULL;

  route_dssi_audio_run->delay_audio_run = NULL;
  route_dssi_audio_run->count_beats_audio_run = NULL;

  route_dssi_audio_run->notation = NULL;
  route_dssi_audio_run->sequencer = NULL;

  route_dssi_audio_run->feed_midi = NULL;
}

void
ags_route_dssi_audio_run_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec)
{
  AgsRouteDssiAudioRun *route_dssi_audio_run;

  route_dssi_audio_run = AGS_ROUTE_DSSI_AUDIO_RUN(gobject);

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      AgsDelayAudioRun *delay_audio_run;
      gboolean is_template;

      delay_audio_run = g_value_get_object(value);

      if(delay_audio_run == route_dssi_audio_run->delay_audio_run){
	return;
      }

      if(delay_audio_run != NULL &&
	 (AGS_RECALL_TEMPLATE & (AGS_RECALL(delay_audio_run)->flags)) != 0){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(route_dssi_audio_run->delay_audio_run != NULL){
	if(is_template){
	  ags_recall_remove_dependency(AGS_RECALL(route_dssi_audio_run),
				       (AgsRecall *) route_dssi_audio_run->delay_audio_run);
	}else{
	  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(route_dssi_audio_run)->flags)) != 0){
	    g_signal_handler_disconnect(G_OBJECT(route_dssi_audio_run),
					route_dssi_audio_run->notation_alloc_input_handler);
	  }
	}

	g_object_unref(G_OBJECT(route_dssi_audio_run->delay_audio_run));
      }

      if(delay_audio_run != NULL){
	g_object_ref(delay_audio_run);

	if(is_template){
	  ags_recall_add_dependency(AGS_RECALL(route_dssi_audio_run),
				    ags_recall_dependency_new((GObject *) delay_audio_run));
	}else{
	  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(route_dssi_audio_run)->flags)) != 0){
	    route_dssi_audio_run->notation_alloc_input_handler =
	      g_signal_connect(G_OBJECT(delay_audio_run), "notation-alloc-input\0",
			       G_CALLBACK(ags_route_dssi_audio_run_alloc_input_callback), route_dssi_audio_run);
	  }
	}
      }

      route_dssi_audio_run->delay_audio_run = delay_audio_run;
    }
    break;
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      AgsCountBeatsAudioRun *count_beats_audio_run;
      gboolean is_template;

      count_beats_audio_run = g_value_get_object(value);

      if(count_beats_audio_run == route_dssi_audio_run->count_beats_audio_run){
	return;
      }

      if(count_beats_audio_run != NULL &&
	 (AGS_RECALL_TEMPLATE & (AGS_RECALL(count_beats_audio_run)->flags)) != 0){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(route_dssi_audio_run->count_beats_audio_run != NULL){
	if(is_template){
	  ags_recall_remove_dependency(AGS_RECALL(route_dssi_audio_run),
				       (AgsRecall *) route_dssi_audio_run->count_beats_audio_run);
	}

	g_object_unref(G_OBJECT(route_dssi_audio_run->count_beats_audio_run));
      }

      if(count_beats_audio_run != NULL){
	g_object_ref(count_beats_audio_run);

	if(is_template){
	  ags_recall_add_dependency(AGS_RECALL(route_dssi_audio_run),
				    ags_recall_dependency_new((GObject *) count_beats_audio_run));
	}
      }

      route_dssi_audio_run->count_beats_audio_run = count_beats_audio_run;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_route_dssi_audio_run_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec)
{
  AgsRouteDssiAudioRun *route_dssi_audio_run;
  
  route_dssi_audio_run = AGS_ROUTE_DSSI_AUDIO_RUN(gobject);

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      g_value_set_object(value, G_OBJECT(route_dssi_audio_run->delay_audio_run));
    }
    break;
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      g_value_set_object(value, G_OBJECT(route_dssi_audio_run->count_beats_audio_run));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_route_dssi_audio_run_finalize(GObject *gobject)
{
  AgsRouteDssiAudioRun *route_dssi_audio_run;

  route_dssi_audio_run = AGS_ROUTE_DSSI_AUDIO_RUN(gobject);

  if(route_dssi_audio_run->delay_audio_run != NULL){
    g_object_unref(G_OBJECT(route_dssi_audio_run->delay_audio_run));
  }

  if(route_dssi_audio_run->count_beats_audio_run != NULL){
    g_object_unref(G_OBJECT(route_dssi_audio_run->count_beats_audio_run));
  }

  G_OBJECT_CLASS(ags_route_dssi_audio_run_parent_class)->finalize(gobject);
}

void
ags_route_dssi_audio_run_connect(AgsConnectable *connectable)
{
  AgsRouteDssiAudioRun *route_dssi_audio_run;

  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) != 0){
    return;
  }

  /* call parent */
  ags_route_dssi_audio_run_parent_connectable_interface->connect(connectable);

  route_dssi_audio_run = AGS_ROUTE_DSSI_AUDIO_RUN(connectable);
}

void
ags_route_dssi_audio_run_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_route_dssi_audio_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_route_dssi_audio_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsRouteDssiAudioRun *route_dssi_audio_run;

  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(dynamic_connectable)->flags)) != 0){
    return;
  }

  route_dssi_audio_run = AGS_ROUTE_DSSI_AUDIO_RUN(dynamic_connectable);

  /* call parent */
  ags_route_dssi_audio_run_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* connect */
  route_dssi_audio_run->notation_alloc_input_handler =
    g_signal_connect(G_OBJECT(route_dssi_audio_run->delay_audio_run), "notation-alloc-input\0",
		     G_CALLBACK(ags_route_dssi_audio_run_alloc_input_callback), route_dssi_audio_run);
  
}

void
ags_route_dssi_audio_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsRouteDssiAudioRun *route_dssi_audio_run;

  /* call parent */
  ags_route_dssi_audio_run_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  route_dssi_audio_run = AGS_ROUTE_DSSI_AUDIO_RUN(dynamic_connectable);

  if(route_dssi_audio_run->delay_audio_run != NULL){
    g_signal_handler_disconnect(G_OBJECT(route_dssi_audio_run->delay_audio_run), route_dssi_audio_run->notation_alloc_input_handler);
  }
}

void
ags_route_dssi_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsFileLookup *file_lookup;
  xmlNode *iter;

  /* read parent */
  ags_route_dssi_audio_run_parent_plugin_interface->read(file, node, plugin);

  /* read depenendency */
  iter = node->children;

  while(iter != NULL){
    if(iter->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp(iter->name,
		     "ags-dependency-list\0",
		     19)){
	xmlNode *dependency_node;

	dependency_node = iter->children;

	while(dependency_node != NULL){
	  if(dependency_node->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(dependency_node->name,
			   "ags-dependency\0",
			   15)){
	      file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
							   "file\0", file,
							   "node\0", dependency_node,
							   "reference\0", G_OBJECT(plugin),
							   NULL);
	      ags_file_add_lookup(file, (GObject *) file_lookup);
	      g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
			       G_CALLBACK(ags_route_dssi_audio_run_read_resolve_dependency), G_OBJECT(plugin));
	    }
	  }
	  
	  dependency_node = dependency_node->next;
	}
      }
    }

    iter = iter->next;
  }
}

xmlNode*
ags_route_dssi_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsFileLookup *file_lookup;
  xmlNode *node, *child;
  xmlNode *dependency_node;
  GList *list;
  gchar *id;

  /* write parent */
  node = ags_route_dssi_audio_run_parent_plugin_interface->write(file, parent, plugin);

  /* write dependencies */
  child = xmlNewNode(NULL,
		     "ags-dependency-list\0");

  xmlNewProp(child,
	     AGS_FILE_ID_PROP,
	     ags_id_generator_create_uuid());

  xmlAddChild(node,
	      child);

  list = AGS_RECALL(plugin)->dependencies;

  while(list != NULL){
    id = ags_id_generator_create_uuid();

    dependency_node = xmlNewNode(NULL,
				 "ags-dependency\0");

    xmlNewProp(dependency_node,
	       AGS_FILE_ID_PROP,
	       id);

    xmlAddChild(child,
		dependency_node);

    file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
						 "file\0", file,
						 "node\0", dependency_node,
						 "reference\0", list->data,
						 NULL);
    ags_file_add_lookup(file, (GObject *) file_lookup);
    g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		     G_CALLBACK(ags_route_dssi_audio_run_write_resolve_dependency), G_OBJECT(plugin));

    list = list->next;
  }

  return(node);
}

void
ags_route_dssi_audio_run_resolve_dependencies(AgsRecall *recall)
{
  AgsRecall *template;
  AgsRecallContainer *recall_container;
  AgsRouteDssiAudioRun *route_dssi_audio_run;
  AgsRecallDependency *recall_dependency;
  AgsDelayAudioRun *delay_audio_run;
  AgsCountBeatsAudioRun *count_beats_audio_run;
  GList *list;
  AgsRecallID *recall_id;
  guint i, i_stop;

  route_dssi_audio_run = AGS_ROUTE_DSSI_AUDIO_RUN(recall);
  
  recall_container = AGS_RECALL_CONTAINER(recall->container);
  
  list = ags_recall_find_template(recall_container->recall_audio_run);

  if(list != NULL){
    template = AGS_RECALL(list->data);
  }else{
    g_warning("AgsRecallClass::resolve - missing dependency");
    return;
  }

  list = template->dependencies;
  delay_audio_run = NULL;
  count_beats_audio_run = NULL;
  i_stop = 2;

  for(i = 0; i < i_stop && list != NULL;){
    recall_dependency = AGS_RECALL_DEPENDENCY(list->data);

    if(AGS_IS_DELAY_AUDIO_RUN(recall_dependency->dependency)){
      if(((AGS_RECALL_INPUT_ORIENTATED & (recall->flags)) != 0 &&
	  (AGS_RECALL_INPUT_ORIENTATED & (AGS_RECALL(recall_dependency->dependency)->flags)) != 0) ||
	 ((AGS_RECALL_OUTPUT_ORIENTATED & (recall->flags)) != 0 &&
	  (AGS_RECALL_OUTPUT_ORIENTATED & (AGS_RECALL(recall_dependency->dependency)->flags)) != 0)){
	recall_id = recall->recall_id;
      }else{
	recall_id = (AgsRecallID *) recall->recall_id->recycling_context->parent->recall_id;
      }

      delay_audio_run = (AgsDelayAudioRun *) ags_recall_dependency_resolve(recall_dependency, recall_id);

      i++;
    }else if(AGS_IS_COUNT_BEATS_AUDIO_RUN(recall_dependency->dependency)){
      if(((AGS_RECALL_INPUT_ORIENTATED & (recall->flags)) != 0 &&
	  (AGS_RECALL_INPUT_ORIENTATED & (AGS_RECALL(recall_dependency->dependency)->flags)) != 0) ||
	 ((AGS_RECALL_OUTPUT_ORIENTATED & (recall->flags)) != 0 &&
	  (AGS_RECALL_OUTPUT_ORIENTATED & (AGS_RECALL(recall_dependency->dependency)->flags)) != 0)){
	recall_id = recall->recall_id;
      }else{
	recall_id = (AgsRecallID *) recall->recall_id->recycling_context->parent->recall_id;
      }

      count_beats_audio_run = (AgsCountBeatsAudioRun *) ags_recall_dependency_resolve(recall_dependency, recall_id);

      i++;
    }

    list = list->next;
  }

  g_object_set(G_OBJECT(recall),
	       "delay-audio-run\0", delay_audio_run,
	       "count-beats-audio-run\0", count_beats_audio_run,
	       NULL);
}

AgsRecall*
ags_route_dssi_audio_run_duplicate(AgsRecall *recall,
				   AgsRecallID *recall_id,
				   guint *n_params, GParameter *parameter)
{
  AgsRouteDssiAudioRun *copy, *route_dssi_audio_run;

  copy = AGS_ROUTE_DSSI_AUDIO_RUN(AGS_RECALL_CLASS(ags_route_dssi_audio_run_parent_class)->duplicate(recall,
												     recall_id,
												     n_params, parameter));

  return((AgsRecall *) copy);
}

void
ags_route_dssi_audio_run_run_pre(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsChannel *channel, *current;
  AgsNote *note;
  AgsRecallDssiRun *recall_dssi_run;

  AgsDelayAudio *delay_audio;
  AgsDelayAudioRun *delay_audio_run;
  AgsCountBeatsAudioRun *count_beats_audio_run;
  AgsRouteDssiAudio *route_dssi_audio;
  AgsRouteDssiAudioRun *route_dssi_audio_run;

  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;
  AgsConfig *config;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  GList *list_note;
  GList *list_recall;

  snd_midi_event_t *parser;
  snd_seq_event_t *seq_event;

  gchar *str;
  unsigned char buffer[7];
  long seq_length;
  long velocity, pressure;
  guint samplerate;
  guint buffer_length;
  guint audio_channel;
  gdouble notation_delay;
  guint start_frame, end_frame;

  GValue value = {0,};
  
  route_dssi_audio_run = AGS_ROUTE_DSSI_AUDIO_RUN(recall);
  route_dssi_audio = AGS_ROUTE_DSSI_AUDIO(AGS_RECALL_AUDIO_RUN(route_dssi_audio_run)->recall_audio);
  
  delay_audio_run = route_dssi_audio_run->delay_audio_run;
  delay_audio = AGS_DELAY_AUDIO(AGS_RECALL_AUDIO_RUN(delay_audio_run)->recall_audio);
  
  count_beats_audio_run = route_dssi_audio_run->count_beats_audio_run;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* read config */
  config = ags_config_get_instance();

  pthread_mutex_lock(application_mutex);
  
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "buffer-size\0");
  buffer_length = g_ascii_strtoull(str,
				   NULL,
				   10);
  free(str);

  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "samplerate\0");
  samplerate = g_ascii_strtoull(str,
				NULL,
				10);
  free(str);

  pthread_mutex_unlock(application_mutex);

  /* get notation delay */
  g_value_init(&value, G_TYPE_DOUBLE);

  ags_port_safe_read(delay_audio->notation_delay, &value);

  notation_delay = g_value_get_double(&value);

  /* get audio and channel */
  audio = AGS_RECALL_AUDIO_RUN(recall)->recall_audio->audio;

  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  audio_channel = AGS_CHANNEL(AGS_RECYCLING(recall->recall_id->recycling)->channel)->audio_channel;
  
  if((AGS_AUDIO_NOTATION_DEFAULT & (audio->flags)) == 0){
    channel = ags_channel_nth(audio->output,
			      audio_channel);
  }else{
    channel = ags_channel_nth(audio->input,
			      audio_channel);
  }

  /* feed MIDI to AgsRecallDssiRun */
  list_note = route_dssi_audio_run->feed_midi;

  snd_midi_event_new(32,
		     &parser);
  snd_midi_event_init(parser);
		     
  while(list_note != NULL){
    note = AGS_NOTE(list_note->data);

    current = ags_channel_pad_nth(channel,
				  note->y);

    if(current != NULL){
      list_recall = current->recall;

      while((list_recall = ags_recall_find_type_with_recycling_context(list_recall,
								       AGS_TYPE_RECALL_DSSI_RUN,
								       recall->recall_id->recycling_context)) != NULL){
	recall_dssi_run = list_recall->data;
	
	if(delay_audio_run->notation_counter == 0 &&
	   count_beats_audio_run->notation_counter == note->x[0]){
	  /* key on */
	  seq_event = (snd_seq_event_t *) malloc(sizeof(snd_seq_event_t));

	  seq_length = ags_midi_buffer_util_get_varlength_size(route_dssi_audio_run->delta_time);

	  velocity = ags_midi_util_envelope_to_velocity(note->attack,
							note->decay,
							note->sustain,
							note->release,
							note->ratio,
							samplerate,
							0, buffer_length);
	  ags_midi_buffer_util_put_key_on(&buffer,
					  route_dssi_audio_run->delta_time,
					  0,
					  channel->pad,
					  velocity);
	  seq_length += 3;
	  
	  snd_midi_event_encode(parser, &buffer, seq_length, seq_event);
	}else if(count_beats_audio_run->notation_counter >= note->x[1]){
	  /* key off */
	  seq_event = (snd_seq_event_t *) malloc(sizeof(snd_seq_event_t));

	  seq_length = ags_midi_buffer_util_get_varlength_size(route_dssi_audio_run->delta_time);
	  start_frame = (count_beats_audio_run->notation_counter * notation_delay * buffer_length) + delay_audio_run->notation_counter * buffer_length;
	  end_frame = start_frame + buffer_length;
	  
	  velocity = ags_midi_util_envelope_to_velocity(note->attack,
							note->decay,
							note->sustain,
							note->release,
							note->ratio,
							samplerate,
							start_frame, end_frame);
	  ags_midi_buffer_util_put_key_off(&buffer,
					   route_dssi_audio_run->delta_time,
					   0,
					   channel->pad,
					   velocity);
	  seq_length += 3;
	  
	  snd_midi_event_encode(parser, &buffer, seq_length, seq_event);
	}else{
	  /* key pressure */
	  seq_event = (snd_seq_event_t *) malloc(sizeof(snd_seq_event_t));

	  seq_length = ags_midi_buffer_util_get_varlength_size(route_dssi_audio_run->delta_time);
	  start_frame = (count_beats_audio_run->notation_counter * notation_delay * buffer_length) + delay_audio_run->notation_counter * buffer_length;
	  end_frame = start_frame + buffer_length;
	  
	  pressure = ags_midi_util_envelope_to_pressure(note->attack,
							note->decay,
							note->sustain,
							note->release,
							note->ratio,
							samplerate,
							start_frame, end_frame);
	  ags_midi_buffer_util_put_key_pressure(&buffer,
						route_dssi_audio_run->delta_time,
						0,
						channel->pad,
						pressure);
	  seq_length += 3;
	  
	  snd_midi_event_encode(parser, &buffer, seq_length, seq_event);
	}

	list_recall = list_recall->next;
      }
    }
    
    list_note = list_note->next;
  }
}

void
ags_route_dssi_audio_run_alloc_input_callback(AgsDelayAudioRun *delay_audio_run,
					      guint nth_run,
					      gdouble delay, guint attack,
					      AgsRouteDssiAudioRun *route_dssi_audio_run)
{
  //TODO:JK: implement me
}

void
ags_route_dssi_audio_run_run_post(AgsRecall *recall)
{
  //TODO:JK: implement me
}

void
ags_route_dssi_audio_run_write_resolve_dependency(AgsFileLookup *file_lookup,
						  GObject *recall)
{
  AgsFileIdRef *id_ref;
  gchar *id;

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_reference(file_lookup->file,
							      AGS_RECALL_DEPENDENCY(file_lookup->ref)->dependency);

  id = xmlGetProp(id_ref->node, AGS_FILE_ID_PROP);

  xmlNewProp(file_lookup->node,
	     "xpath\0",
  	     g_strdup_printf("xpath=//*[@id='%s']\0", id));
}

void
ags_route_dssi_audio_run_read_resolve_dependency(AgsFileLookup *file_lookup,
						 GObject *recall)
{
  AgsFileIdRef *id_ref;
  gchar *xpath;

  xpath = (gchar *) xmlGetProp(file_lookup->node,
			       "xpath\0");

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_xpath(file_lookup->file, xpath);

  if(AGS_IS_DELAY_AUDIO_RUN(id_ref->ref)){
    g_object_set(G_OBJECT(recall),
		 "delay-audio-run\0", id_ref->ref,
		 NULL);
  }else if(AGS_IS_COUNT_BEATS_AUDIO_RUN(id_ref->ref)){
    g_object_set(G_OBJECT(recall),
		 "count-beats-audio-run\0", id_ref->ref,
		 NULL);
  }
}

/**
 * ags_route_dssi_audio_run_new:
 *
 * Creates an #AgsRouteDssiAudioRun
 *
 * Returns: a new #AgsRouteDssiAudioRun
 *
 * Since: 0.7.1
 */
AgsRouteDssiAudioRun*
ags_route_dssi_audio_run_new()
{
  AgsRouteDssiAudioRun *route_dssi_audio_run;

  route_dssi_audio_run = (AgsRouteDssiAudioRun *) g_object_new(AGS_TYPE_ROUTE_DSSI_AUDIO_RUN,
							       NULL);
  
  return(route_dssi_audio_run);
}
