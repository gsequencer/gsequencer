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
#include <ags/object/ags_soundcard.h>
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
#include <ags/audio/ags_recall_dssi.h>
#include <ags/audio/ags_recall_dssi_run.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>

#include <alsa/seq_midi_event.h>

#include <ags/i18n.h>

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
void ags_route_dssi_audio_run_dispose(GObject *gobject);
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

void ags_route_dssi_audio_run_run_post(AgsRecall *recall);

void ags_route_dssi_audio_run_alloc_input_callback(AgsDelayAudioRun *delay_audio_run,
						   guint nth_run,
						   gdouble delay, guint attack,
						   AgsRouteDssiAudioRun *route_dssi_audio_run);

void ags_route_dssi_audio_run_write_resolve_dependency(AgsFileLookup *file_lookup,
						       GObject *recall);
void ags_route_dssi_audio_run_read_resolve_dependency(AgsFileLookup *file_lookup,
						      GObject *recall);

void ags_route_dssi_audio_run_feed_midi(AgsRecall *recall,
					AgsNote *note);

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
							   "AgsRouteDssiAudioRun",
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

  gobject->dispose = ags_route_dssi_audio_run_dispose;
  gobject->finalize = ags_route_dssi_audio_run_finalize;

  /* properties */
  /**
   * AgsRouteDssiAudioRun:delay-audio-run:
   * 
   * The delay audio run dependency.
   * 
   * Since: 0.7.122.7
   */
  param_spec = g_param_spec_object("delay-audio-run",
				   i18n_pspec("assigned AgsDelayAudioRun"),
				   i18n_pspec("the AgsDelayAudioRun which emits notation_alloc_input signal"),
				   AGS_TYPE_DELAY_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY_AUDIO_RUN,
				  param_spec);

  /**
   * AgsRouteDssiAudioRun:count-beats-audio-run:
   * 
   * The count beats audio run dependency.
   * 
   * Since: 0.7.122.7
   */
  param_spec = g_param_spec_object("count-beats-audio-run",
				   i18n_pspec("assigned AgsCountBeatsAudioRun"),
				   i18n_pspec("the AgsCountBeatsAudioRun which just counts"),
				   AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COUNT_BEATS_AUDIO_RUN,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) route_dssi_audio_run;

  recall->resolve_dependencies = ags_route_dssi_audio_run_resolve_dependencies;
  recall->duplicate = ags_route_dssi_audio_run_duplicate;
  recall->run_post = ags_route_dssi_audio_run_run_post;
}

void
ags_route_dssi_audio_run_init(AgsRouteDssiAudioRun *route_dssi_audio_run)
{
  AGS_RECALL(route_dssi_audio_run)->name = "ags-route-dssi";
  AGS_RECALL(route_dssi_audio_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(route_dssi_audio_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(route_dssi_audio_run)->xml_type = "ags-route-dssi-audio-run";
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
	      g_signal_connect(G_OBJECT(delay_audio_run), "notation-alloc-input",
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
ags_route_dssi_audio_run_dispose(GObject *gobject)
{
  AgsRouteDssiAudioRun *route_dssi_audio_run;

  route_dssi_audio_run = AGS_ROUTE_DSSI_AUDIO_RUN(gobject);

  /* delay audio run */
  if(route_dssi_audio_run->delay_audio_run != NULL){
    g_object_unref(G_OBJECT(route_dssi_audio_run->delay_audio_run));

    route_dssi_audio_run->delay_audio_run = NULL;
  }

  /* count beats audio run */
  if(route_dssi_audio_run->count_beats_audio_run != NULL){
    g_object_unref(G_OBJECT(route_dssi_audio_run->count_beats_audio_run));

    route_dssi_audio_run->count_beats_audio_run = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_route_dssi_audio_run_parent_class)->dispose(gobject);
}

void
ags_route_dssi_audio_run_finalize(GObject *gobject)
{
  AgsRouteDssiAudioRun *route_dssi_audio_run;

  route_dssi_audio_run = AGS_ROUTE_DSSI_AUDIO_RUN(gobject);

  /* delay audio run */
  if(route_dssi_audio_run->delay_audio_run != NULL){
    g_object_unref(G_OBJECT(route_dssi_audio_run->delay_audio_run));
  }

  /* count beats audio run */
  if(route_dssi_audio_run->count_beats_audio_run != NULL){
    g_object_unref(G_OBJECT(route_dssi_audio_run->count_beats_audio_run));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_route_dssi_audio_run_parent_class)->finalize(gobject);
}

void
ags_route_dssi_audio_run_connect(AgsConnectable *connectable)
{
  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) != 0){
    return;
  }

  /* call parent */
  ags_route_dssi_audio_run_parent_connectable_interface->connect(connectable);
}

void
ags_route_dssi_audio_run_disconnect(AgsConnectable *connectable)
{
  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) == 0){
    return;
  }

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
    g_signal_connect(G_OBJECT(route_dssi_audio_run->delay_audio_run), "notation-alloc-input",
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
		     "ags-dependency-list",
		     19)){
	xmlNode *dependency_node;

	dependency_node = iter->children;

	while(dependency_node != NULL){
	  if(dependency_node->type == XML_ELEMENT_NODE){
	    if(!xmlStrncmp(dependency_node->name,
			   "ags-dependency",
			   15)){
	      file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
							   "file", file,
							   "node", dependency_node,
							   "reference", G_OBJECT(plugin),
							   NULL);
	      ags_file_add_lookup(file, (GObject *) file_lookup);
	      g_signal_connect(G_OBJECT(file_lookup), "resolve",
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
		     "ags-dependency-list");

  xmlNewProp(child,
	     AGS_FILE_ID_PROP,
	     ags_id_generator_create_uuid());

  xmlAddChild(node,
	      child);

  list = AGS_RECALL(plugin)->dependencies;

  while(list != NULL){
    id = ags_id_generator_create_uuid();

    dependency_node = xmlNewNode(NULL,
				 "ags-dependency");

    xmlNewProp(dependency_node,
	       AGS_FILE_ID_PROP,
	       id);

    xmlAddChild(child,
		dependency_node);

    file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
						 "file", file,
						 "node", dependency_node,
						 "reference", list->data,
						 NULL);
    ags_file_add_lookup(file, (GObject *) file_lookup);
    g_signal_connect(G_OBJECT(file_lookup), "resolve",
		     G_CALLBACK(ags_route_dssi_audio_run_write_resolve_dependency), G_OBJECT(plugin));

    list = list->next;
  }

  return(node);
}

void
ags_route_dssi_audio_run_resolve_dependencies(AgsRecall *recall)
{
  AgsRecall *template;
  AgsRecallID *recall_id;
  AgsRecallContainer *recall_container;
  AgsRecallDependency *recall_dependency;

  AgsDelayAudioRun *delay_audio_run;
  AgsCountBeatsAudioRun *count_beats_audio_run;

  GList *list;

  guint i, i_stop;

  
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
	       "delay-audio-run", delay_audio_run,
	       "count-beats-audio-run", count_beats_audio_run,
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
ags_route_dssi_audio_run_feed_midi(AgsRecall *recall,
				   AgsNote *note)
{
  AgsAudio *audio;
  AgsChannel *channel, *selected_channel;
  AgsChannel *link;
  AgsRecallDssiRun *recall_dssi_run;
  AgsRecallID *child_recall_id;
  
  AgsDelayAudioRun *delay_audio_run;
  AgsRouteDssiAudioRun *route_dssi_audio_run;

  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  GList *channel_dummy, *recycling_dummy, *dssi;
  GList *list_recall;
  GList *list;
  
  snd_seq_event_t *seq_event;

  gchar *str;

  guint audio_start_mapping;
  guint midi_start_mapping, midi_end_mapping;
  guint audio_channel;
  guint input_pads;
  guint selected_pad;
  guint start_frame, end_frame;
  guint note_y;
  long seq_length;
  long velocity, pressure;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *selected_channel_mutex;
  pthread_mutex_t *link_mutex;
  
  route_dssi_audio_run = AGS_ROUTE_DSSI_AUDIO_RUN(recall);
  
  delay_audio_run = route_dssi_audio_run->delay_audio_run;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio */
  audio = AGS_RECALL_AUDIO_RUN(recall)->recall_audio->audio;

  /* audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* get audio fields */
  pthread_mutex_lock(audio_mutex);

  audio_start_mapping = audio->audio_start_mapping;

  midi_start_mapping = audio->midi_start_mapping;
  midi_end_mapping = audio->midi_end_mapping;

  channel = audio->input;
  input_pads = audio->input_pads;

  note_y = note->y;
  
  pthread_mutex_unlock(audio_mutex);

  /* channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  /* get audio channel */
  pthread_mutex_lock(channel_mutex);

  audio_channel = AGS_CHANNEL(AGS_RECYCLING(recall->recall_id->recycling)->channel)->audio_channel;
    
  pthread_mutex_unlock(channel_mutex);

  /* retrieve channel */
  channel = ags_channel_nth(channel,
			    audio_channel);

  if((AGS_AUDIO_REVERSE_MAPPING & (audio->flags)) != 0){
    selected_channel = ags_channel_pad_nth(channel, audio_start_mapping + input_pads - note_y - 1);
  }else{
    selected_channel = ags_channel_pad_nth(channel,
					   audio_start_mapping + note_y);
  }

  /* selected channel mutex */
  pthread_mutex_lock(application_mutex);

  selected_channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						    (GObject *) selected_channel);
  
  pthread_mutex_unlock(application_mutex);

  /* check within mapping */
  pthread_mutex_lock(selected_channel_mutex);

  link = selected_channel->link;
  selected_pad = selected_channel->pad;
  
  pthread_mutex_unlock(selected_channel_mutex);

  if(selected_pad - audio_start_mapping + midi_start_mapping < midi_start_mapping ||
     selected_pad - audio_start_mapping + midi_start_mapping >= midi_end_mapping){
    return;
  }
  
  /* link mutex */
  if(link != NULL){
    pthread_mutex_lock(application_mutex);
    
    link_mutex = ags_mutex_manager_lookup(mutex_manager,
					  (GObject *) link);
    
    pthread_mutex_unlock(application_mutex);
  }else{
    link_mutex = NULL;
  }
  
  /* get recall id */
  child_recall_id = NULL;
    
  if(selected_channel != NULL){
    if(link == NULL){
      pthread_mutex_lock(selected_channel_mutex);

      list = selected_channel->recall_id;

      while(list != NULL){
	if(AGS_RECALL_ID(list->data)->recycling_context->parent == AGS_RECALL(delay_audio_run)->recall_id->recycling_context){
	  child_recall_id = (AgsRecallID *) list->data;
	  break;
	}
	  
	list = list->next;
      }

      pthread_mutex_unlock(selected_channel_mutex);
    }else{
      pthread_mutex_lock(link_mutex);

      list = link->recall_id;

      while(list != NULL){
	if(AGS_RECALL_ID(list->data)->recycling_context->parent->parent == AGS_RECALL(delay_audio_run)->recall_id->recycling_context){
	  child_recall_id = (AgsRecallID *) list->data;
	  break;
	}
	  
	list = list->next;
      }

      pthread_mutex_unlock(link_mutex);
    }

    /* get dssi run */
    if(child_recall_id != NULL){
      //FIXME:JK: use filename and effect to identify
      pthread_mutex_lock(selected_channel_mutex);

      list_recall = ags_recall_template_find_type(selected_channel->recall,
						  AGS_TYPE_RECALL_DSSI);
      channel_dummy = NULL;
      
      if(list_recall != NULL){
	channel_dummy = AGS_RECALL_CONTAINER(AGS_RECALL(list_recall->data)->container)->recall_channel_run;
      }

      while(channel_dummy != NULL){

	if(AGS_RECALL(channel_dummy->data)->recall_id != NULL &&
	   AGS_RECALL(channel_dummy->data)->recall_id->recycling_context == child_recall_id->recycling_context){
	  recycling_dummy = AGS_RECALL(channel_dummy->data)->children;

	  while(recycling_dummy != NULL){
	    dssi = AGS_RECALL(recycling_dummy->data)->children;

	    while(dssi != NULL){
	      recall_dssi_run = AGS_RECALL_DSSI_RUN(dssi->data);
	    
	      if(recall_dssi_run->event_buffer == NULL){
		/* prepend note */
		//		route_dssi_audio_run->feed_midi = g_list_prepend(route_dssi_audio_run->feed_midi,
		//						 note);
	      
		recall_dssi_run->route_dssi_audio_run = (GObject *) route_dssi_audio_run;
	      
		/* key on */
		seq_event = (snd_seq_event_t *) malloc(sizeof(snd_seq_event_t));
		memset(seq_event, 0, sizeof(snd_seq_event_t));
		//memset(seq_event, 0, sizeof(snd_seq_event_t));
	      
		seq_event->type = SND_SEQ_EVENT_NOTEON;

		seq_event->data.note.channel = 0;
		seq_event->data.note.note = 0x7f & (selected_channel->pad - audio_start_mapping + midi_start_mapping);
		seq_event->data.note.velocity = 127;

		AGS_RECALL_AUDIO_SIGNAL(recall_dssi_run)->audio_channel = audio_channel;
		recall_dssi_run->note = (GObject *) note;
		  
		recall_dssi_run->event_buffer = (snd_seq_event_t **) malloc(2 * sizeof(snd_seq_event_t *));
		recall_dssi_run->event_buffer[0] = seq_event;
		recall_dssi_run->event_buffer[1] = NULL;
		  
		recall_dssi_run->event_count = (unsigned long *) malloc(2 * sizeof(unsigned long));
		recall_dssi_run->event_count[0] = 1;
		recall_dssi_run->event_count[1] = 0;
	      }

	      //fixme:jk: remove notes

	      dssi = dssi->next;
	    }

	    recycling_dummy = recycling_dummy->next;
	  }	 
	}
	
	channel_dummy = channel_dummy->next;
      }

      pthread_mutex_unlock(selected_channel_mutex);
    }
  }
}

void
ags_route_dssi_audio_run_alloc_input_callback(AgsDelayAudioRun *delay_audio_run,
					      guint nth_run,
					      gdouble delay, guint attack,
					      AgsRouteDssiAudioRun *route_dssi_audio_run)
{
  GObject *soundcard;
  AgsAudio *audio;
  AgsNotation *notation;
  AgsNote *note;

  AgsRouteDssiAudio *route_dssi_audio;

  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;
  AgsConfig *config;
  
  GList *current_position;
  GList *list;

  guint audio_channel;
  guint audio_start_mapping, audio_end_mapping;
  guint note_y;
  guint note_x0;
  guint i;
  gchar *str;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  if((guint) floor(delay) != 0){
    //    g_message("d %f", delay);
    return;
  }

  route_dssi_audio = AGS_ROUTE_DSSI_AUDIO(AGS_RECALL_AUDIO_RUN(route_dssi_audio_run)->recall_audio);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  audio = AGS_RECALL_AUDIO(route_dssi_audio)->audio;

  /* audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) AGS_RECYCLING(AGS_RECALL(delay_audio_run)->recall_id->recycling)->channel);
  
  pthread_mutex_unlock(application_mutex);

  /* audio channel */
  pthread_mutex_lock(channel_mutex);
  
  audio_channel = AGS_CHANNEL(AGS_RECYCLING(AGS_RECALL(delay_audio_run)->recall_id->recycling)->channel)->audio_channel;

  pthread_mutex_unlock(channel_mutex);

  /*  */  
  pthread_mutex_lock(audio_mutex);

 //TODO:JK: make it advanced
  list = audio->notation;
  notation = AGS_NOTATION(g_list_nth(list, audio_channel)->data);//AGS_NOTATION(ags_notation_find_near_timestamp(list, audio_channel,
    //						   timestamp_thread->timestamp)->data);

  current_position = notation->notes; // start_loop

  pthread_mutex_unlock(audio_mutex);

  while(current_position != NULL){
    pthread_mutex_lock(audio_mutex);

    audio_start_mapping = audio->audio_start_mapping;
    audio_end_mapping = audio->audio_end_mapping;
    
    note = AGS_NOTE(current_position->data);

    note_y = note->y;
    note_x0 = note->x[0];
    
    pthread_mutex_unlock(audio_mutex);

    //    g_message("--- %f %f ; %d %d",
    //	      note->stream_delay, delay,
    //	      note_x0, route_dssi_audio_run->count_beats_audio_run->notation_counter);

    //FIXME:JK: should consider delay
    if(note_y >= audio_start_mapping &&
       note_y < audio_end_mapping &&
       note_x0 == route_dssi_audio_run->count_beats_audio_run->notation_counter){ // && floor(note->stream_delay) == floor(delay)
      //      g_object_ref(note);
      ags_route_dssi_audio_run_feed_midi((AgsRecall *) route_dssi_audio_run,
					 note);      
    }else if(note_x0 > route_dssi_audio_run->count_beats_audio_run->notation_counter){
      break;
    }
    
    current_position = current_position->next;
  }
}

void
ags_route_dssi_audio_run_run_post(AgsRecall *recall)
{
  AgsAudio *audio;
  
  AgsDelayAudio *delay_audio;
  AgsDelayAudioRun *delay_audio_run;
  AgsCountBeatsAudioRun *count_beats_audio_run;
  AgsRouteDssiAudioRun *route_dssi_audio_run;

  AgsApplicationContext *application_context;
  AgsConfig *config;
  GObject *soundcard;
  
  AgsMutexManager *mutex_manager;
 
  gchar *str;
  guint buffer_length;
  gdouble bpm;
  gdouble notation_delay;
  gdouble x;
  
  pthread_mutex_t *application_mutex;

  GValue value = {0,};

  route_dssi_audio_run = AGS_ROUTE_DSSI_AUDIO_RUN(recall);
  
  delay_audio_run = route_dssi_audio_run->delay_audio_run;
  delay_audio = AGS_DELAY_AUDIO(AGS_RECALL_AUDIO_RUN(delay_audio_run)->recall_audio);
  
  count_beats_audio_run = route_dssi_audio_run->count_beats_audio_run;
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  audio = AGS_RECALL_AUDIO_RUN(recall)->recall_audio->audio;
  
  soundcard = audio->soundcard;

  /* read config */
  config = ags_config_get_instance();

  pthread_mutex_lock(application_mutex);

  bpm = ags_soundcard_get_bpm(AGS_SOUNDCARD(soundcard));

  /* buffer size */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "buffer-size");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "buffer-size");
  }

  if(str != NULL){
    buffer_length = g_ascii_strtoull(str,
				     NULL,
				     10);
    free(str);
  }else{
    buffer_length = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  }

  pthread_mutex_unlock(application_mutex);
  
  /* get notation delay */
  g_value_init(&value, G_TYPE_DOUBLE);

  ags_port_safe_read(delay_audio->notation_delay, &value);

  notation_delay = g_value_get_double(&value);
  g_value_unset(&value);

  /*  */
  x = (((count_beats_audio_run->notation_counter * notation_delay) + delay_audio_run->notation_counter) * buffer_length);
  
  route_dssi_audio_run->delta_time = x / 16.0 / bpm * 60.0 / ((USEC_PER_SEC * bpm / 4.0) / (4.0 * bpm) / USEC_PER_SEC);
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
	     "xpath",
  	     g_strdup_printf("xpath=//*[@id='%s']", id));
}

void
ags_route_dssi_audio_run_read_resolve_dependency(AgsFileLookup *file_lookup,
						 GObject *recall)
{
  AgsFileIdRef *id_ref;
  gchar *xpath;

  xpath = (gchar *) xmlGetProp(file_lookup->node,
			       "xpath");

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_xpath(file_lookup->file, xpath);

  if(AGS_IS_DELAY_AUDIO_RUN(id_ref->ref)){
    g_object_set(G_OBJECT(recall),
		 "delay-audio-run", id_ref->ref,
		 NULL);
  }else if(AGS_IS_COUNT_BEATS_AUDIO_RUN(id_ref->ref)){
    g_object_set(G_OBJECT(recall),
		 "count-beats-audio-run", id_ref->ref,
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
