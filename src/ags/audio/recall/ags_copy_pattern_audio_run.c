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

#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_audio.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_dynamic_connectable.h>
#include <ags/object/ags_plugin.h>

#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>

#include <ags/audio/ags_recall_container.h>

void ags_copy_pattern_audio_run_class_init(AgsCopyPatternAudioRunClass *copy_pattern_audio_run);
void ags_copy_pattern_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_copy_pattern_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_copy_pattern_audio_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_copy_pattern_audio_run_init(AgsCopyPatternAudioRun *copy_pattern_audio_run);
void ags_copy_pattern_audio_run_set_property(GObject *gobject,
					     guint prop_id,
					     const GValue *value,
					     GParamSpec *param_spec);
void ags_copy_pattern_audio_run_get_property(GObject *gobject,
					     guint prop_id,
					     GValue *value,
					     GParamSpec *param_spec);
void ags_copy_pattern_audio_run_finalize(GObject *gobject);
void ags_copy_pattern_audio_run_connect(AgsConnectable *connectable);
void ags_copy_pattern_audio_run_disconnect(AgsConnectable *connectable);
void ags_copy_pattern_audio_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_copy_pattern_audio_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_copy_pattern_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_copy_pattern_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_copy_pattern_audio_run_resolve_dependencies(AgsRecall *recall);
AgsRecall* ags_copy_pattern_audio_run_duplicate(AgsRecall *recall,
						AgsRecallID *recall_id,
						guint *n_params, GParameter *parameter);
void ags_copy_pattern_audio_run_notify_dependency(AgsRecall *recall, guint notify_mode, gint count);

void ags_copy_pattern_audio_run_write_resolve_dependency(AgsFileLookup *file_lookup,
							GObject *recall);
void ags_copy_pattern_audio_run_read_resolve_dependency(AgsFileLookup *file_lookup,
							GObject *recall);

/**
 * SECTION:ags_copy_pattern_audio_run
 * @short_description: copy pattern
 * @title: AgsCopyPatternAudioRun
 * @section_id:
 * @include: ags/audio/recall/ags_copy_pattern_audio_run.h
 *
 * The #AgsCopyPatternAudioRun class copy pattern.
 */

enum{
  PROP_0,
  PROP_DELAY_AUDIO_RUN,
  PROP_COUNT_BEATS_AUDIO_RUN,
};

static gpointer ags_copy_pattern_audio_run_parent_class = NULL;
static AgsConnectableInterface* ags_copy_pattern_audio_run_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_copy_pattern_audio_run_parent_dynamic_connectable_interface;
static AgsPluginInterface *ags_copy_pattern_audio_run_parent_plugin_interface;

GType
ags_copy_pattern_audio_run_get_type()
{
  static GType ags_type_copy_pattern_audio_run = 0;

  if(!ags_type_copy_pattern_audio_run){
    static const GTypeInfo ags_copy_pattern_audio_run_info = {
      sizeof (AgsCopyPatternAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_pattern_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyPatternAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_pattern_audio_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_pattern_audio_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_pattern_audio_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_copy_pattern_audio_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };    

    ags_type_copy_pattern_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
							     "AgsCopyPatternAudioRun\0",
							     &ags_copy_pattern_audio_run_info,
							     0);
    
    g_type_add_interface_static(ags_type_copy_pattern_audio_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_copy_pattern_audio_run,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);

    g_type_add_interface_static(ags_type_copy_pattern_audio_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_copy_pattern_audio_run);
}

void
ags_copy_pattern_audio_run_class_init(AgsCopyPatternAudioRunClass *copy_pattern_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_copy_pattern_audio_run_parent_class = g_type_class_peek_parent(copy_pattern_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) copy_pattern_audio_run;

  gobject->set_property = ags_copy_pattern_audio_run_set_property;
  gobject->get_property = ags_copy_pattern_audio_run_get_property;

  gobject->finalize = ags_copy_pattern_audio_run_finalize;

  /* properties */
  param_spec = g_param_spec_object("delay-audio-run\0",
				   "assigned AgsDelayAudioRun\0",
				   "the AgsDelayAudioRun which emits alloc signal\0",
				   AGS_TYPE_DELAY_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY_AUDIO_RUN,
				  param_spec);

  param_spec = g_param_spec_object("count-beats-audio-run\0",
				   "assigned AgsCountBeatsAudioRun\0",
				   "the AgsCountBeatsAudioRun which emits beat signal\0",
				   AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COUNT_BEATS_AUDIO_RUN,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) copy_pattern_audio_run;

  recall->resolve_dependencies = ags_copy_pattern_audio_run_resolve_dependencies;
  recall->duplicate = ags_copy_pattern_audio_run_duplicate;
  recall->notify_dependency = ags_copy_pattern_audio_run_notify_dependency;
}

void
ags_copy_pattern_audio_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_copy_pattern_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_copy_pattern_audio_run_connect;
  connectable->disconnect = ags_copy_pattern_audio_run_disconnect;
}

void
ags_copy_pattern_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_copy_pattern_audio_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_copy_pattern_audio_run_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_copy_pattern_audio_run_disconnect_dynamic;
}

void
ags_copy_pattern_audio_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_copy_pattern_audio_run_parent_plugin_interface = g_type_interface_peek_parent(plugin);
  
  plugin->read = ags_copy_pattern_audio_run_read;
  plugin->write = ags_copy_pattern_audio_run_write;
}

void
ags_copy_pattern_audio_run_init(AgsCopyPatternAudioRun *copy_pattern_audio_run)
{
  AGS_RECALL(copy_pattern_audio_run)->name = "ags-copy-pattern\0";
  AGS_RECALL(copy_pattern_audio_run)->version = AGS_EFFECTS_DEFAULT_VERSION;
  AGS_RECALL(copy_pattern_audio_run)->build_id = AGS_BUILD_ID;
  AGS_RECALL(copy_pattern_audio_run)->xml_type = "ags-copy-pattern-audio-run\0";
  AGS_RECALL(copy_pattern_audio_run)->port = NULL;

  copy_pattern_audio_run->hide_ref = 0;
  copy_pattern_audio_run->hide_ref_counter = 0;

  copy_pattern_audio_run->delay_audio_run = NULL;
  copy_pattern_audio_run->count_beats_audio_run = NULL;
}

void
ags_copy_pattern_audio_run_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;

  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(gobject);

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      AgsCountBeatsAudioRun *delay_audio_run;
      gboolean is_template;

      delay_audio_run = (AgsCountBeatsAudioRun *) g_value_get_object(value);

      if(copy_pattern_audio_run->delay_audio_run == delay_audio_run)
	return;

      if(delay_audio_run != NULL &&
	 (AGS_RECALL_TEMPLATE & (AGS_RECALL(delay_audio_run)->flags)) != 0){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(copy_pattern_audio_run->delay_audio_run != NULL){
	if(is_template){
	  ags_recall_remove_dependency(AGS_RECALL(copy_pattern_audio_run),
				       (AgsRecall *) copy_pattern_audio_run->delay_audio_run);
	}

	g_object_unref(G_OBJECT(copy_pattern_audio_run->delay_audio_run));
      }

      if(delay_audio_run != NULL){
	g_object_ref(G_OBJECT(delay_audio_run));

	if(is_template){
	  ags_recall_add_dependency(AGS_RECALL(copy_pattern_audio_run),
				    ags_recall_dependency_new((GObject *) delay_audio_run));
	}
      }

      copy_pattern_audio_run->delay_audio_run = delay_audio_run;
    }
    break;
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      AgsCountBeatsAudioRun *count_beats_audio_run;
      gboolean is_template;

      count_beats_audio_run = (AgsCountBeatsAudioRun *) g_value_get_object(value);

      if(copy_pattern_audio_run->count_beats_audio_run == count_beats_audio_run)
	return;

      if(count_beats_audio_run != NULL &&
	 (AGS_RECALL_TEMPLATE & (AGS_RECALL(count_beats_audio_run)->flags)) != 0){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(copy_pattern_audio_run->count_beats_audio_run != NULL){
	if(is_template){
	  ags_recall_remove_dependency(AGS_RECALL(copy_pattern_audio_run),
				       (AgsRecall *) copy_pattern_audio_run->count_beats_audio_run);
	}

	g_object_unref(G_OBJECT(copy_pattern_audio_run->count_beats_audio_run));
      }

      if(count_beats_audio_run != NULL){
	g_object_ref(G_OBJECT(count_beats_audio_run));

	if(is_template){
	  ags_recall_add_dependency(AGS_RECALL(copy_pattern_audio_run),
				    ags_recall_dependency_new((GObject *) count_beats_audio_run));
	}
      }

      copy_pattern_audio_run->count_beats_audio_run = count_beats_audio_run;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_copy_pattern_audio_run_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;
  
  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(gobject);

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      g_value_set_object(value, copy_pattern_audio_run->delay_audio_run);
    }
    break;
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      g_value_set_object(value, copy_pattern_audio_run->count_beats_audio_run);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_copy_pattern_audio_run_finalize(GObject *gobject)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;

  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(gobject);

  if(copy_pattern_audio_run->delay_audio_run != NULL)
    g_object_unref(copy_pattern_audio_run->delay_audio_run);

  if(copy_pattern_audio_run->count_beats_audio_run != NULL)
    g_object_unref(copy_pattern_audio_run->count_beats_audio_run);

  /* call parent */
  G_OBJECT_CLASS(ags_copy_pattern_audio_run_parent_class)->finalize(gobject);
}

void
ags_copy_pattern_audio_run_connect(AgsConnectable *connectable)
{
  /* call parent */
  ags_copy_pattern_audio_run_parent_connectable_interface->connect(connectable);
}

void
ags_copy_pattern_audio_run_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_copy_pattern_audio_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_copy_pattern_audio_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_copy_pattern_audio_run_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);
}

void
ags_copy_pattern_audio_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_copy_pattern_audio_run_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);
}

void
ags_copy_pattern_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsFileLookup *file_lookup;
  xmlNode *iter;

  /* read parent */
  ags_copy_pattern_audio_run_parent_plugin_interface->read(file, node, plugin);

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
			       G_CALLBACK(ags_copy_pattern_audio_run_read_resolve_dependency), AGS_RECALL(plugin));
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
ags_copy_pattern_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsFileLookup *file_lookup;
  xmlNode *node, *child;
  xmlNode *dependency_node;
  GList *list;
  gchar *id;

  /* write parent */
  node = ags_copy_pattern_audio_run_parent_plugin_interface->write(file, parent, plugin);

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
		     G_CALLBACK(ags_copy_pattern_audio_run_write_resolve_dependency), AGS_RECALL(plugin));

    list = list->next;
  }

  return(node);
}

AgsRecall*
ags_copy_pattern_audio_run_duplicate(AgsRecall *recall,
				     AgsRecallID *recall_id,
				     guint *n_params, GParameter *parameter)
{
  AgsCopyPatternAudioRun *copy;

  copy = AGS_COPY_PATTERN_AUDIO_RUN(AGS_RECALL_CLASS(ags_copy_pattern_audio_run_parent_class)->duplicate(recall,
													 recall_id,
													 n_params, parameter));

  return((AgsRecall *) copy);
}

void
ags_copy_pattern_audio_run_notify_dependency(AgsRecall *recall, guint notify_mode, gint count)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;

  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(recall);

  switch(notify_mode){
  case AGS_RECALL_NOTIFY_RUN:
    break;
  case AGS_RECALL_NOTIFY_AUDIO:
    break;
  case AGS_RECALL_NOTIFY_AUDIO_RUN:
    break;
  case AGS_RECALL_NOTIFY_CHANNEL:
    break;
  case AGS_RECALL_NOTIFY_CHANNEL_RUN:
    copy_pattern_audio_run->hide_ref += count;
    g_message("copy_pattern_audio_run->hide_ref: %u\n\0", copy_pattern_audio_run->hide_ref);
    break;
  default:
    g_message("ags_copy_pattern_audio_run.c - ags_copy_pattern_audio_run_notify: unknown notify\0");
  }
}

void
ags_copy_pattern_audio_run_resolve_dependencies(AgsRecall *recall)
{
  AgsRecall *template;
  AgsRecallContainer *recall_container;
  AgsCopyPatternAudioRun *copy_pattern_audio_run;
  AgsRecallDependency *recall_dependency;
  AgsDelayAudioRun *delay_audio_run;
  AgsCountBeatsAudioRun *count_beats_audio_run;
  GList *list;
  AgsRecallID *recall_id;
  guint i, i_stop;
  
  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(recall);
  
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
    recall_id = recall->recall_id;
      
    if(AGS_IS_DELAY_AUDIO_RUN(recall_dependency->dependency)){
      delay_audio_run = (AgsDelayAudioRun *) ags_recall_dependency_resolve(recall_dependency,
									   recall_id->recycling_container->parent->recall_id);

      i++;
    }else if(AGS_IS_COUNT_BEATS_AUDIO_RUN(recall_dependency->dependency)){
      count_beats_audio_run = (AgsCountBeatsAudioRun *) ags_recall_dependency_resolve(recall_dependency,
										      recall_id->recycling_container->parent->recall_id);

      i++;
    }

    list = list->next;
  }

  g_object_set(G_OBJECT(recall),
	       "delay-audio-run\0", delay_audio_run,
	       "count-beats-audio-run\0", count_beats_audio_run,
	       NULL);
}

void
ags_copy_pattern_audio_run_write_resolve_dependency(AgsFileLookup *file_lookup,
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
ags_copy_pattern_audio_run_read_resolve_dependency(AgsFileLookup *file_lookup,
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
 * ags_copy_pattern_audio_run_new:
 * @count_beats_audio_run: an #AgsCountBeatsAudioRun as dependency
 *
 * Creates an #AgsCopyPatternAudioRun
 *
 * Returns: a new #AgsCopyPatternAudioRun
 *
 * Since: 0.4
 */
AgsCopyPatternAudioRun*
ags_copy_pattern_audio_run_new(AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;

  copy_pattern_audio_run = (AgsCopyPatternAudioRun *) g_object_new(AGS_TYPE_COPY_PATTERN_AUDIO_RUN,
								   "count-beats-audio-run\0", count_beats_audio_run,
								   NULL);

  return(copy_pattern_audio_run);
}
