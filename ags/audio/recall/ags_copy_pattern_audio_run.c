/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/recall/ags_copy_pattern_audio_run.h>

#include <ags/libags.h>

#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_copy_pattern_audio.h>

#include <ags/i18n.h>

void ags_copy_pattern_audio_run_class_init(AgsCopyPatternAudioRunClass *copy_pattern_audio_run);
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
void ags_copy_pattern_audio_run_dispose(GObject *gobject);
void ags_copy_pattern_audio_run_finalize(GObject *gobject);

void ags_copy_pattern_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_copy_pattern_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_copy_pattern_audio_run_resolve_dependency(AgsRecall *recall);
void ags_copy_pattern_audio_run_notify_dependency(AgsRecall *recall,
						  guint dependency, gboolean increase);

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
static AgsPluginInterface *ags_copy_pattern_audio_run_parent_plugin_interface;

GType
ags_copy_pattern_audio_run_get_type()
{
  static GType ags_type_copy_pattern_audio_run = 0;

  if(!ags_type_copy_pattern_audio_run){
    static const GTypeInfo ags_copy_pattern_audio_run_info = {
      sizeof(AgsCopyPatternAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_pattern_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCopyPatternAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_pattern_audio_run_init,
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_copy_pattern_audio_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };    

    ags_type_copy_pattern_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
							     "AgsCopyPatternAudioRun",
							     &ags_copy_pattern_audio_run_info,
							     0);
    
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

  gobject->dispose = ags_copy_pattern_audio_run_dispose;
  gobject->finalize = ags_copy_pattern_audio_run_finalize;

  /* properties */
  /**
   * AgsCopyPatternAudioRun:delay-audio-run:
   *
   * The delay audio run dependency.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("delay-audio-run",
				   i18n_pspec("assigned AgsDelayAudioRun"),
				   i18n_pspec("the AgsDelayAudioRun which emits alloc signal"),
				   AGS_TYPE_DELAY_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY_AUDIO_RUN,
				  param_spec);

  /**
   * AgsCopyPatternAudioRun:count-beats-audio-run:
   *
   * The count beats audio run dependency.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("count-beats-audio-run",
				   i18n_pspec("assigned AgsCountBeatsAudioRun"),
				   i18n_pspec("the AgsCountBeatsAudioRun which emits beat signal"),
				   AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COUNT_BEATS_AUDIO_RUN,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) copy_pattern_audio_run;

  recall->resolve_dependency = ags_copy_pattern_audio_run_resolve_dependency;
  recall->notify_dependency = ags_copy_pattern_audio_run_notify_dependency;
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
  AGS_RECALL(copy_pattern_audio_run)->name = "ags-copy-pattern";
  AGS_RECALL(copy_pattern_audio_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(copy_pattern_audio_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(copy_pattern_audio_run)->xml_type = "ags-copy-pattern-audio-run";
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

  pthread_mutex_t *recall_mutex;
  
  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      AgsDelayAudioRun *delay_audio_run, *old_delay_audio_run;

      gboolean is_template;

      delay_audio_run = (AgsDelayAudioRun *) g_value_get_object(value);
      old_delay_audio_run = NULL;
      
      pthread_mutex_lock(recall_mutex);

      if(copy_pattern_audio_run->delay_audio_run == delay_audio_run){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(copy_pattern_audio_run->delay_audio_run != NULL){
	old_delay_audio_run = copy_pattern_audio_run->delay_audio_run;
	
	g_object_unref(G_OBJECT(copy_pattern_audio_run->delay_audio_run));
      }

      if(delay_audio_run != NULL){
	g_object_ref(G_OBJECT(delay_audio_run));
      }

      copy_pattern_audio_run->delay_audio_run = delay_audio_run;

      pthread_mutex_unlock(recall_mutex);

      /* dependency */
      if(ags_recall_test_flags(recall, AGS_RECALL_TEMPLATE)){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(is_template &&
	 old_delay_audio_run != NULL){
	ags_recall_remove_dependency(AGS_RECALL(copy_pattern_audio_run),
				     (AgsRecall *) old_delay_audio_run);
      }

      if(is_template &&
	 delay_audio_run != NULL){
	ags_recall_add_dependency(AGS_RECALL(copy_pattern_audio_run),
				  ags_recall_dependency_new((GObject *) delay_audio_run));
      }
    }
    break;
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      AgsCountBeatsAudioRun *count_beats_audio_run, *old_count_beats_audio_run;

      gboolean is_template;

      count_beats_audio_run = (AgsCountBeatsAudioRun *) g_value_get_object(value);
      old_count_beats_audio_run = NULL;
      
      pthread_mutex_lock(recall_mutex);

      if(copy_pattern_audio_run->count_beats_audio_run == count_beats_audio_run){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(copy_pattern_audio_run->count_beats_audio_run != NULL){
	old_count_beats_audio_run = copy_pattern_audio_run->count_beats_audio_run;
	
	g_object_unref(G_OBJECT(copy_pattern_audio_run->count_beats_audio_run));
      }

      if(count_beats_audio_run != NULL){
	g_object_ref(G_OBJECT(count_beats_audio_run));
      }

      copy_pattern_audio_run->count_beats_audio_run = count_beats_audio_run;

      pthread_mutex_unlock(recall_mutex);

      /* check template */
      if(count_beats_audio_run != NULL &&
	 ags_recall_test_flags(count_beats_audio_run, AGS_RECALL_TEMPLATE)){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      /* dependency - remove */
      if(is_template &&
	 old_count_beats_audio_run != NULL){
	ags_recall_remove_dependency(AGS_RECALL(copy_pattern_audio_run),
				     (AgsRecall *) old_count_beats_audio_run);
      }
      
      /* dependency - add */
      if(is_template &&
	 count_beats_audio_run != NULL){
	ags_recall_add_dependency(AGS_RECALL(copy_pattern_audio_run),
				  ags_recall_dependency_new((GObject *) count_beats_audio_run));
      }
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
  
  pthread_mutex_t *recall_mutex;
  
  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, copy_pattern_audio_run->delay_audio_run);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, copy_pattern_audio_run->count_beats_audio_run);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_copy_pattern_audio_run_dispose(GObject *gobject)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;

  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(gobject);

  /* delay audio run */
  if(copy_pattern_audio_run->delay_audio_run != NULL){
    g_object_unref(copy_pattern_audio_run->delay_audio_run);

    copy_pattern_audio_run->delay_audio_run = NULL;
  }
  
  /* count beats audio run */
  if(copy_pattern_audio_run->count_beats_audio_run != NULL){
    g_object_unref(copy_pattern_audio_run->count_beats_audio_run);

    copy_pattern_audio_run->count_beats_audio_run = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_copy_pattern_audio_run_parent_class)->dispose(gobject);
}

void
ags_copy_pattern_audio_run_finalize(GObject *gobject)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;

  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(gobject);

  /* delay audio run */
  if(copy_pattern_audio_run->delay_audio_run != NULL){
    g_object_unref(copy_pattern_audio_run->delay_audio_run);
  }
  
  /* count beats audio run */
  if(copy_pattern_audio_run->count_beats_audio_run != NULL){
    g_object_unref(copy_pattern_audio_run->count_beats_audio_run);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_copy_pattern_audio_run_parent_class)->finalize(gobject);
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
		     G_CALLBACK(ags_copy_pattern_audio_run_write_resolve_dependency), AGS_RECALL(plugin));

    list = list->next;
  }

  return(node);
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
	     "xpath",
  	     g_strdup_printf("xpath=//*[@id='%s']", id));
}

void
ags_copy_pattern_audio_run_read_resolve_dependency(AgsFileLookup *file_lookup,
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

void
ags_copy_pattern_audio_run_resolve_dependency(AgsRecall *recall)
{
  AgsRecall *template;
  AgsRecallContainer *recall_container;
  AgsRecallID *parent_recall_id;
  AgsRecallID *recall_id;
  AgsRecyclingContext *parent_recycling_context;
  AgsRecyclingContext *recycling_context;
  
  AgsRecallDependency *recall_dependency;
  AgsDelayAudioRun *delay_audio_run;
  AgsCountBeatsAudioRun *count_beats_audio_run;

  GObject *dependency;
  
  GList *list_start, *list;
  
  guint i, i_stop;

  g_object_get(recall,
	       "recall-container", &recall_container,
	       NULL);

  g_object_get(recall_container,
	       "recall-audio-run", &list_start,
	       NULL);
  
  list = ags_recall_find_template(list_start);

  if(list == NULL){
    g_warning("AgsRecallClass::resolve - missing dependency");

    return;
  }

  template = AGS_RECALL(list->data);
  g_list_free(list_start);
  
  g_object_get(template,
	       "recall-dependency", &list_start,
	       NULL);
  
  g_object_get(recall,
	       "recall-id", &recall_id,
	       NULL);

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);

  g_object_get(parent_recycling_context,
	       "recall-id", &parent_recall_id,
	       NULL);
  
  /* prepare to resolve */
  delay_audio_run = NULL;
  count_beats_audio_run = NULL;

  list = list_start;
  
  i_stop = 2;

  for(i = 0; i < i_stop && list != NULL;){
    recall_dependency = AGS_RECALL_DEPENDENCY(list->data);

    g_object_get(recall_dependency,
		 "dependency", &dependency,
		 NULL);
    
    if(AGS_IS_DELAY_AUDIO_RUN(dependency)){
      delay_audio_run = (AgsDelayAudioRun *) ags_recall_dependency_resolve(recall_dependency,
									   parent_recall_id);

      i++;
    }else if(AGS_IS_COUNT_BEATS_AUDIO_RUN(dependency)){
      count_beats_audio_run = (AgsCountBeatsAudioRun *) ags_recall_dependency_resolve(recall_dependency,
										      parent_recall_id);

      i++;
    }

    list = list->next;
  }

  g_list_free(list_start);
  
  g_object_set(G_OBJECT(recall),
	       "delay-audio-run", delay_audio_run,
	       "count-beats-audio-run", count_beats_audio_run,
	       NULL);
}

void
ags_copy_pattern_audio_run_notify_dependency(AgsRecall *recall,
					     guint dependency, gboolean increase)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;

  pthread_mutex_t *recall_mutex;

  copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(recall);

  /* get mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = recall->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* notify */
  pthread_mutex_lock(recall_mutex);

  switch(dependency){
  case AGS_RECALL_NOTIFY_RUN:
    break;
  case AGS_RECALL_NOTIFY_AUDIO:
    break;
  case AGS_RECALL_NOTIFY_AUDIO_RUN:
    break;
  case AGS_RECALL_NOTIFY_CHANNEL:
    break;
  case AGS_RECALL_NOTIFY_CHANNEL_RUN:
    if(increase){
      copy_pattern_audio_run->hide_ref += 1;
    }else{
      copy_pattern_audio_run->hide_ref -= 1;
    }
    break;
  default:
    g_message("ags_copy_pattern_audio_run.c - ags_copy_pattern_audio_run_notify: unknown notify");
  }


  pthread_mutex_unlock(recall_mutex);
}

/**
 * ags_copy_pattern_audio_run_new:
 * @delay_audio_run: the #AgsDelayAudioRun dependency
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun dependency
 *
 * Create a new instance of #AgsCopyPatternAudioRun
 *
 * Returns: the new #AgsCopyPatternAudioRun
 *
 * Since: 2.0.0
 */
AgsCopyPatternAudioRun*
ags_copy_pattern_audio_run_new(AgsDelayAudioRun *delay_audio_run,
			       AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AgsCopyPatternAudioRun *copy_pattern_audio_run;

  copy_pattern_audio_run = (AgsCopyPatternAudioRun *) g_object_new(AGS_TYPE_COPY_PATTERN_AUDIO_RUN,
								   "delay-audio-run", delay_audio_run,
								   "count-beats-audio-run", count_beats_audio_run,
								   NULL);

  return(copy_pattern_audio_run);
}
