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

#include <ags/audio/recall/ags_play_notation_audio_run.h>

#include <ags/libags.h>

#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_play_notation_audio.h>
#include <ags/audio/recall/ags_delay_audio.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>

#include <ags/i18n.h>

void ags_play_notation_audio_run_class_init(AgsPlayNotationAudioRunClass *play_notation_audio_run);
void ags_play_notation_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_notation_audio_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_play_notation_audio_run_init(AgsPlayNotationAudioRun *play_notation_audio_run);
void ags_play_notation_audio_run_set_property(GObject *gobject,
					      guint prop_id,
					      const GValue *value,
					      GParamSpec *param_spec);
void ags_play_notation_audio_run_get_property(GObject *gobject,
					      guint prop_id,
					      GValue *value,
					      GParamSpec *param_spec);
void ags_play_notation_audio_run_dispose(GObject *gobject);
void ags_play_notation_audio_run_finalize(GObject *gobject);

void ags_play_notation_audio_run_connect(AgsConnectable *connectable);
void ags_play_notation_audio_run_disconnect(AgsConnectable *connectable);
void ags_play_notation_audio_run_connect_connection(AgsConnectable *connectable,
						    GObject *connection);
void ags_play_notation_audio_run_disconnect_connection(AgsConnectable *connectable,
						       GObject *connection);

void ags_play_notation_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_play_notation_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_play_notation_audio_run_resolve_dependency(AgsRecall *recall);

void ags_play_notation_audio_run_alloc_input_callback(AgsDelayAudioRun *delay_audio_run,
						      guint nth_run,
						      gdouble delay, guint attack,
						      AgsPlayNotationAudioRun *play_notation_audio_run);

void ags_play_notation_audio_run_write_resolve_dependency(AgsFileLookup *file_lookup,
							  GObject *recall);
void ags_play_notation_audio_run_read_resolve_dependency(AgsFileLookup *file_lookup,
							 GObject *recall);

/**
 * SECTION:ags_play_notation_audio_run
 * @short_description: play notation
 * @title: AgsPlayNotationAudioRun
 * @section_id:
 * @include: ags/audio/recall/ags_play_notation_audio_run.h
 *
 * The #AgsPlayNotationAudioRun class play notation.
 */

enum{
  PROP_0,
  PROP_DELAY_AUDIO_RUN,
  PROP_COUNT_BEATS_AUDIO_RUN,
  PROP_NOTATION,
};

static gpointer ags_play_notation_audio_run_parent_class = NULL;
static AgsConnectableInterface* ags_play_notation_audio_run_parent_connectable_interface;
static AgsPluginInterface *ags_play_notation_audio_run_parent_plugin_interface;

GType
ags_play_notation_audio_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_play_notation_audio_run = 0;

    static const GTypeInfo ags_play_notation_audio_run_info = {
      sizeof (AgsPlayNotationAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_notation_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayNotationAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_notation_audio_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_notation_audio_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_play_notation_audio_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_notation_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
							      "AgsPlayNotationAudioRun",
							      &ags_play_notation_audio_run_info,
							      0);

    g_type_add_interface_static(ags_type_play_notation_audio_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_notation_audio_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_play_notation_audio_run);
  }

  return g_define_type_id__volatile;
}

void
ags_play_notation_audio_run_class_init(AgsPlayNotationAudioRunClass *play_notation_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_play_notation_audio_run_parent_class = g_type_class_peek_parent(play_notation_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) play_notation_audio_run;

  gobject->set_property = ags_play_notation_audio_run_set_property;
  gobject->get_property = ags_play_notation_audio_run_get_property;

  gobject->dispose = ags_play_notation_audio_run_dispose;
  gobject->finalize = ags_play_notation_audio_run_finalize;

  /* properties */
  /**
   * AgsPlayNotationAudioRun:delay-audio-run:
   *
   * The delay audio run dependency.
   * 
   * Since: 2.0.0
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
   * AgsPlayNotationAudioRun:count-beats-audio-run:
   *
   * The count beats audio run dependency.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("count-beats-audio-run",
				   i18n_pspec("assigned AgsCountBeatsAudioRun"),
				   i18n_pspec("the AgsCountBeatsAudioRun which just counts"),
				   AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COUNT_BEATS_AUDIO_RUN,
				  param_spec);

  /**
   * AgsPlayNotationAudioRun:notation:
   *
   * The notation containing the notes.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("notation",
				   i18n_pspec("assigned AgsNotation"),
				   i18n_pspec("The AgsNotation containing notes"),
				   AGS_TYPE_NOTATION,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) play_notation_audio_run;

  recall->resolve_dependency = ags_play_notation_audio_run_resolve_dependency;
}

void
ags_play_notation_audio_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_play_notation_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_play_notation_audio_run_connect;
  connectable->disconnect = ags_play_notation_audio_run_disconnect;

  connectable->connect_connection = ags_play_notation_audio_run_connect_connection;
  connectable->disconnect_connection = ags_play_notation_audio_run_disconnect_connection;
}

void
ags_play_notation_audio_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_play_notation_audio_run_parent_plugin_interface = g_type_interface_peek_parent(plugin);
  
  plugin->read = ags_play_notation_audio_run_read;
  plugin->write = ags_play_notation_audio_run_write;
}

void
ags_play_notation_audio_run_init(AgsPlayNotationAudioRun *play_notation_audio_run)
{
  ags_recall_set_ability_flags((AgsRecall *) play_notation_audio_run, (AGS_SOUND_ABILITY_NOTATION));

  AGS_RECALL(play_notation_audio_run)->name = "ags-play-notation";
  AGS_RECALL(play_notation_audio_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_notation_audio_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_notation_audio_run)->xml_type = "ags-play-notation-audio-run";
  AGS_RECALL(play_notation_audio_run)->port = NULL;

  play_notation_audio_run->delay_audio_run = NULL;
  play_notation_audio_run->count_beats_audio_run = NULL;

  play_notation_audio_run->notation = NULL;

  play_notation_audio_run->timestamp = ags_timestamp_new();
  g_object_ref(play_notation_audio_run->timestamp);
  
  play_notation_audio_run->timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  play_notation_audio_run->timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  play_notation_audio_run->timestamp->timer.ags_offset.offset = 0;
}

void
ags_play_notation_audio_run_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec)
{
  AgsPlayNotationAudioRun *play_notation_audio_run;

  pthread_mutex_t *recall_mutex;

  play_notation_audio_run = AGS_PLAY_NOTATION_AUDIO_RUN(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      AgsDelayAudioRun *delay_audio_run, *old_delay_audio_run;

      gboolean is_template;

      delay_audio_run = g_value_get_object(value);
      old_delay_audio_run = NULL;
      
      pthread_mutex_lock(recall_mutex);

      if(delay_audio_run == play_notation_audio_run->delay_audio_run){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(play_notation_audio_run->delay_audio_run != NULL){
	old_delay_audio_run = play_notation_audio_run->delay_audio_run;

	g_object_unref(G_OBJECT(play_notation_audio_run->delay_audio_run));
      }

      if(delay_audio_run != NULL){
	g_object_ref(delay_audio_run);
      }

      pthread_mutex_unlock(recall_mutex);

      /* check template */
      if(delay_audio_run != NULL &&
	 ags_recall_test_flags((AgsRecall *) play_notation_audio_run, AGS_RECALL_TEMPLATE)){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      /* old - dependency/connection */
      if(is_template){
	if(old_delay_audio_run != NULL){
	  AgsRecallDependency *recall_dependency;

	  GList *list;
	  
	  recall_dependency = NULL;
	  list = ags_recall_dependency_find_dependency(AGS_RECALL(play_notation_audio_run)->recall_dependency,
						       (GObject *) old_delay_audio_run);

	  if(list != NULL){
	    recall_dependency = list->data;
	  }
	  
	  ags_recall_remove_recall_dependency(AGS_RECALL(play_notation_audio_run),
					      recall_dependency);
	}
      }else{
	if(ags_connectable_is_connected(AGS_CONNECTABLE(play_notation_audio_run))){
	  ags_connectable_disconnect_connection(AGS_CONNECTABLE(play_notation_audio_run),
						(GObject *) old_delay_audio_run);
	}
      }

      /* new - dependency/connection */
      pthread_mutex_lock(recall_mutex);

      play_notation_audio_run->delay_audio_run = delay_audio_run;

      pthread_mutex_unlock(recall_mutex);

      if(delay_audio_run != NULL){
	if(is_template){
	  ags_recall_add_recall_dependency(AGS_RECALL(play_notation_audio_run),
					   ags_recall_dependency_new((GObject *) delay_audio_run));
	}else{
	  if(ags_connectable_is_connected(AGS_CONNECTABLE(play_notation_audio_run))){
	    ags_connectable_connect_connection(AGS_CONNECTABLE(play_notation_audio_run),
					       (GObject *) delay_audio_run);
	  }
	}
      }
    }
    break;
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      AgsCountBeatsAudioRun *count_beats_audio_run, *old_count_beats_audio_run;

      gboolean is_template;

      count_beats_audio_run = g_value_get_object(value);
      old_count_beats_audio_run = NULL;
      
      pthread_mutex_lock(recall_mutex);

      if(count_beats_audio_run == play_notation_audio_run->count_beats_audio_run){
	pthread_mutex_unlock(recall_mutex);
	
	return;
      }

      if((AGS_RECALL_TEMPLATE & (AGS_RECALL(play_notation_audio_run)->flags)) != 0){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(play_notation_audio_run->count_beats_audio_run != NULL){
	old_count_beats_audio_run = play_notation_audio_run->count_beats_audio_run;
	
	g_object_unref(G_OBJECT(play_notation_audio_run->count_beats_audio_run));
      }

      if(count_beats_audio_run != NULL){
	g_object_ref(count_beats_audio_run);
      }

      play_notation_audio_run->count_beats_audio_run = count_beats_audio_run;

      pthread_mutex_unlock(recall_mutex);

      /* check template */
      if(count_beats_audio_run != NULL &&
	 ags_recall_test_flags((AgsRecall *) play_notation_audio_run, AGS_RECALL_TEMPLATE)){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      /* dependency - remove */
      if(is_template){
	if(old_count_beats_audio_run != NULL){
	  AgsRecallDependency *recall_dependency;

	  GList *list;
	  
	  recall_dependency = NULL;
	  list = ags_recall_dependency_find_dependency(AGS_RECALL(play_notation_audio_run)->recall_dependency,
						       (GObject *) old_count_beats_audio_run);

	  if(list != NULL){
	    recall_dependency = list->data;
	  }
	  
	  ags_recall_remove_recall_dependency(AGS_RECALL(play_notation_audio_run),
					      recall_dependency);
	}
      }

      /* dependency - add */
      if(is_template &&
	 count_beats_audio_run != NULL){
	ags_recall_add_recall_dependency(AGS_RECALL(play_notation_audio_run),
					 ags_recall_dependency_new((GObject *) count_beats_audio_run));
      }
    }
    break;
  case PROP_NOTATION:
    {
      AgsNotation *notation;

      notation = (AgsNotation *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(play_notation_audio_run->notation == notation){
	pthread_mutex_unlock(recall_mutex);
      
	return;
      }

      if(play_notation_audio_run->notation != NULL){
	g_object_unref(play_notation_audio_run->notation);
      }

      if(notation != NULL){
	g_object_ref(notation);
      }

      play_notation_audio_run->notation = notation;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_play_notation_audio_run_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec)
{
  AgsPlayNotationAudioRun *play_notation_audio_run;

  pthread_mutex_t *recall_mutex;
  
  play_notation_audio_run = AGS_PLAY_NOTATION_AUDIO_RUN(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, G_OBJECT(play_notation_audio_run->delay_audio_run));

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, G_OBJECT(play_notation_audio_run->count_beats_audio_run));

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_NOTATION:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, play_notation_audio_run->notation);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_play_notation_audio_run_dispose(GObject *gobject)
{
  AgsPlayNotationAudioRun *play_notation_audio_run;

  play_notation_audio_run = AGS_PLAY_NOTATION_AUDIO_RUN(gobject);

  /* delay audio run */
  if(play_notation_audio_run->delay_audio_run != NULL){
    g_object_unref(G_OBJECT(play_notation_audio_run->delay_audio_run));

    play_notation_audio_run->delay_audio_run = NULL;
  }

  /* count beats audio run */
  if(play_notation_audio_run->count_beats_audio_run != NULL){
    g_object_unref(G_OBJECT(play_notation_audio_run->count_beats_audio_run));

    play_notation_audio_run->count_beats_audio_run = NULL;
  }

  /* notation */
  if(play_notation_audio_run->notation != NULL){
    g_object_unref(G_OBJECT(play_notation_audio_run->notation));

    play_notation_audio_run->notation = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_notation_audio_run_parent_class)->dispose(gobject);
}

void
ags_play_notation_audio_run_finalize(GObject *gobject)
{
  AgsPlayNotationAudioRun *play_notation_audio_run;

  play_notation_audio_run = AGS_PLAY_NOTATION_AUDIO_RUN(gobject);

  /* delay audio run */
  if(play_notation_audio_run->delay_audio_run != NULL){
    g_object_unref(G_OBJECT(play_notation_audio_run->delay_audio_run));
  }

  /* count beats audio run */
  if(play_notation_audio_run->count_beats_audio_run != NULL){
    g_object_unref(G_OBJECT(play_notation_audio_run->count_beats_audio_run));
  }

  /* notation */
  if(play_notation_audio_run->notation != NULL){
    g_object_unref(G_OBJECT(play_notation_audio_run->notation));
  }

  /* timestamp */
  if(play_notation_audio_run->timestamp != NULL){
    g_object_unref(G_OBJECT(play_notation_audio_run->timestamp));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_notation_audio_run_parent_class)->finalize(gobject);
}

void
ags_play_notation_audio_run_connect(AgsConnectable *connectable)
{
  AgsPlayNotationAudioRun *play_notation_audio_run;
  AgsDelayAudioRun *delay_audio_run;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  play_notation_audio_run = AGS_PLAY_NOTATION_AUDIO_RUN(connectable);
  
  g_object_get(play_notation_audio_run,
	       "delay-audio-run", &delay_audio_run,
	       NULL);

  ags_connectable_connect_connection(connectable, (GObject *) delay_audio_run);
  
  /* call parent */
  ags_play_notation_audio_run_parent_connectable_interface->connect(connectable);

  if(delay_audio_run != NULL){
    g_object_unref(delay_audio_run);
  }
}

void
ags_play_notation_audio_run_disconnect(AgsConnectable *connectable)
{
  AgsPlayNotationAudioRun *play_notation_audio_run;  
  AgsDelayAudioRun *delay_audio_run;
  
  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  play_notation_audio_run = AGS_PLAY_NOTATION_AUDIO_RUN(connectable);
  
  g_object_get(play_notation_audio_run,
	       "delay-audio-run", &delay_audio_run,
	       NULL);

  ags_connectable_disconnect_connection(connectable, (GObject *) delay_audio_run);

  /* call parent */
  ags_play_notation_audio_run_parent_connectable_interface->disconnect(connectable);

  if(delay_audio_run != NULL){
    g_object_unref(delay_audio_run);
  }
}

void
ags_play_notation_audio_run_connect_connection(AgsConnectable *connectable, GObject *connection)
{
  AgsPlayNotationAudioRun *play_notation_audio_run;  
  AgsDelayAudioRun *delay_audio_run;

  if(connection == NULL){
    return;
  }

  play_notation_audio_run = AGS_PLAY_NOTATION_AUDIO_RUN(connectable);

  g_object_get(play_notation_audio_run,
	       "delay-audio-run", &delay_audio_run,
	       NULL);

  if(connection == (GObject *) delay_audio_run){
    g_signal_connect(G_OBJECT(delay_audio_run), "notation-alloc-input",
		     G_CALLBACK(ags_play_notation_audio_run_alloc_input_callback), play_notation_audio_run);  
  }

  if(delay_audio_run != NULL){
    g_object_unref(delay_audio_run);
  }
}

void
ags_play_notation_audio_run_disconnect_connection(AgsConnectable *connectable, GObject *connection)
{
  AgsPlayNotationAudioRun *play_notation_audio_run;  
  AgsDelayAudioRun *delay_audio_run;

  if(connection == NULL){
    return;
  }
  
  play_notation_audio_run = AGS_PLAY_NOTATION_AUDIO_RUN(connectable);

  g_object_get(play_notation_audio_run,
	       "delay-audio-run", &delay_audio_run,
	       NULL);

  if(connection == (GObject *) delay_audio_run){
    g_object_disconnect(G_OBJECT(delay_audio_run),
			"any_signal::notation-alloc-input",
			G_CALLBACK(ags_play_notation_audio_run_alloc_input_callback),
			play_notation_audio_run,
			NULL);
  }

  if(delay_audio_run != NULL){
    g_object_unref(delay_audio_run);
  }
}

void
ags_play_notation_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsFileLookup *file_lookup;

  xmlNode *iter;

  /* read parent */
  ags_play_notation_audio_run_parent_plugin_interface->read(file, node, plugin);

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
			       G_CALLBACK(ags_play_notation_audio_run_read_resolve_dependency), G_OBJECT(plugin));
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
ags_play_notation_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsFileLookup *file_lookup;

  xmlNode *node, *child;
  xmlNode *dependency_node;
  
  GList *list;

  gchar *id;

  /* write parent */
  node = ags_play_notation_audio_run_parent_plugin_interface->write(file, parent, plugin);

  /* write dependencies */
  child = xmlNewNode(NULL,
		     "ags-dependency-list");

  xmlNewProp(child,
	     AGS_FILE_ID_PROP,
	     ags_id_generator_create_uuid());

  xmlAddChild(node,
	      child);

  list = AGS_RECALL(plugin)->recall_dependency;

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
		     G_CALLBACK(ags_play_notation_audio_run_write_resolve_dependency), G_OBJECT(plugin));

    list = list->next;
  }

  return(node);
}

void
ags_play_notation_audio_run_resolve_dependency(AgsRecall *recall)
{
  AgsRecall *template;
  AgsRecallID *recall_id;
  AgsRecallContainer *recall_container;
  AgsRecallDependency *recall_dependency;
  AgsDelayAudioRun *delay_audio_run;
  AgsCountBeatsAudioRun *count_beats_audio_run;

  GList *list_start, *list;

  guint i, i_stop;
  
  g_object_get(recall,
	       "recall-id", &recall_id,
	       "recall-container", &recall_container,
	       NULL);

  g_object_get(recall_container,
	       "recall-audio-run", &list_start,
	       NULL);

  template = NULL;
  list = ags_recall_find_template(list_start);

  if(list != NULL){
    template = AGS_RECALL(list->data);
  }

  g_list_free_full(list_start,
		   g_object_unref);

  g_object_get(template,
	       "recall-dependency", &list_start,
	       NULL);

  list = list_start;

  delay_audio_run = NULL;
  count_beats_audio_run = NULL;

  i_stop = 2;

  for(i = 0; i < i_stop && list != NULL;){
    GObject *dependency;
    
    recall_dependency = AGS_RECALL_DEPENDENCY(list->data);
    
    g_object_get(recall_dependency,
		 "dependency", &dependency,
		 NULL);
    
    if(AGS_IS_DELAY_AUDIO_RUN(dependency)){
      delay_audio_run = (AgsDelayAudioRun *) ags_recall_dependency_resolve(recall_dependency,
									   recall_id);
      
      i++;
    }else if(AGS_IS_COUNT_BEATS_AUDIO_RUN(dependency)){
      count_beats_audio_run = (AgsCountBeatsAudioRun *) ags_recall_dependency_resolve(recall_dependency,
										      recall_id);

      i++;
    }

    g_object_unref(dependency);
    
    /* iterate */
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
  
  g_object_set(G_OBJECT(recall),
	       "delay-audio-run", delay_audio_run,
	       "count-beats-audio-run", count_beats_audio_run,
	       NULL);

  /* unref */
  g_object_unref(recall_id);

  g_object_unref(recall_container);
}

void
ags_play_notation_audio_run_alloc_input_callback(AgsDelayAudioRun *delay_audio_run,
						 guint nth_run,
						 gdouble delay, guint attack,
						 AgsPlayNotationAudioRun *play_notation_audio_run)
{
  AgsAudio *audio;
  AgsChannel *output, *input;
  AgsChannel *selected_channel, *channel, *next_pad;
  AgsRecycling *first_recycling, *last_recycling;
  AgsRecycling *recycling, *next_recycling;
  AgsRecycling *end_recycling;
  AgsAudioSignal *audio_signal;
  AgsNotation *notation;
  AgsNote *note;
  AgsRecallID *recall_id;
  AgsRecyclingContext *recycling_context;
  AgsPlayNotationAudio *play_notation_audio;
  AgsDelayAudio *delay_audio;
  AgsCountBeatsAudioRun *count_beats_audio_run;
  
  AgsTimestamp *timestamp;
  
  GObject *output_soundcard;
  
  GList *start_current_position, *current_position;
  GList *start_list, *list;

  gchar *str;

  guint audio_flags;
  guint pads;
  guint notation_counter;
  guint output_pads, input_pads;
  guint audio_channel;
  guint samplerate;
  guint i;
  
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *recycling_mutex;

  auto void ags_play_notation_audio_run_alloc_input_callback_play_notation(AgsNotation *notation);

  void ags_play_notation_audio_run_alloc_input_callback_play_notation(AgsNotation *notation)
  {
    AgsPort *port;
    
    gdouble notation_delay;

    GValue value = {0,};

    /* get notation delay */
    g_object_get(delay_audio,
		 "notation-delay", &port,
		 NULL);
    
    g_value_init(&value,
		 G_TYPE_DOUBLE);
    
    ags_port_safe_read(port,
		       &value);

    notation_delay = g_value_get_double(&value);
    g_value_unset(&value);

    g_object_unref(port);
    
    /*  */
    start_current_position = ags_notation_find_offset(notation,
						      notation_counter,
						      FALSE);

    current_position = start_current_position;
    
    while(current_position != NULL){
      AgsRecallID *child_recall_id;

      GList *start_list, *list;

      guint y;
      
      note = AGS_NOTE(current_position->data);
      g_object_get(note,
		   "y", &y,
		   NULL);
      
      if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
	selected_channel = ags_channel_pad_nth(channel,
					       pads - y - 1);
      }else{
	selected_channel = ags_channel_pad_nth(channel,
					       y);
      }

      if(selected_channel == NULL){
	current_position = current_position->next;

	continue;
      }

      /* get child recall id */
      g_object_get(selected_channel,
		   "recall-id", &start_list,
		   NULL);

      list = start_list;
      child_recall_id = NULL;

      while(list != NULL){
	AgsRecallID *current_recall_id;
	AgsRecyclingContext *current_recycling_context, *current_parent_recycling_context;

	g_object_get(list->data,
		     "recycling-context", &current_recycling_context,
		     NULL);

	g_object_get(current_recycling_context,
		     "parent", &current_parent_recycling_context,
		     NULL);
	  
	if(current_parent_recycling_context == recycling_context){
	  child_recall_id = (AgsRecallID *) list->data;
	    
	  break;
	}

	/* iterate */
	list = list->next;
      }

      g_list_free_full(start_list,
		       g_object_unref);

      /* recycling */
      g_object_get(selected_channel,
		   "first-recycling", &first_recycling,
		   "last-recycling", &last_recycling,
		   NULL);

      recycling = first_recycling;
      g_object_ref(recycling);

      end_recycling = ags_recycling_next(last_recycling);
      
      g_object_set(note,
		   "rt-attack", attack,
		   NULL);

#ifdef AGS_DEBUG	
      g_message("playing[%u|%u]: %u | %u\n", audio_channel, selected_channel->pad, note->x[0], note->y);
#endif

      next_recycling = NULL;
      
      while(recycling != end_recycling){
	g_object_set(note,
		     "rt-offset", 0,
		     NULL);
	  
	if(!ags_recall_global_get_rt_safe()){
	  /* create audio signal */
	  audio_signal = ags_audio_signal_new((GObject *) output_soundcard,
					      (GObject *) recycling,
					      (GObject *) child_recall_id);
	  g_object_set(audio_signal,
		       "note", note,
		       NULL);
	  
	  if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_PATTERN_MODE)){
	    ags_recycling_create_audio_signal_with_defaults(recycling,
							    audio_signal,
							    0.0, 0);
	  }else{
	    guint note_x0, note_x1;

	    note_x0 = notation_counter;

	    g_object_get(note,
			 "x1", &note_x1,
			 NULL);
	      
	    /* create audio signal with frame count */
	    ags_recycling_create_audio_signal_with_frame_count(recycling,
							       audio_signal,
							       (guint) (((gdouble) audio_signal->buffer_size * notation_delay) * (gdouble) (note_x1 - note_x0)),
							       0.0, 0);
	  }
	  
	  audio_signal->stream_current = audio_signal->stream;
	    
	  ags_connectable_connect(AGS_CONNECTABLE(audio_signal));

	  /* lock and add */
	  ags_recycling_add_audio_signal(recycling,
					 audio_signal);
	  //g_object_unref(audio_signal);
	}else{
	  GList *start_list, *list;

	  g_object_get(recycling,
		       "audio-signal", &start_list,
		       NULL);
	    
	  audio_signal = NULL;
	  list = ags_audio_signal_find_by_recall_id(start_list,
						    (GObject *) child_recall_id);
	    
	  if(list != NULL){
	    audio_signal = list->data;

	    g_object_set(audio_signal,
			 "delay", notation_delay,
			 "note", note,
			 NULL);
	  }

	  g_list_free_full(start_list,
			   g_object_unref);
	}
	  
	/* iterate */
	next_recycling = ags_recycling_next(recycling);

	g_object_unref(recycling);

	recycling = next_recycling;
      }

      /* unref */
      g_object_unref(selected_channel);

      if(first_recycling != NULL){
	g_object_unref(first_recycling);
	g_object_unref(last_recycling);
      }
      
      if(end_recycling != NULL){
	g_object_unref(end_recycling);
      }

      if(next_recycling != NULL){
	g_object_unref(next_recycling);
      }

      /* iterate */
      current_position = current_position->next;
    }

    g_list_free_full(start_current_position,
		     g_object_unref);
  }
  
  if(delay != 0.0){
    //    g_message("d %f", delay);
    return;
  }

  g_object_get(play_notation_audio_run,
	       "audio", &audio,
	       NULL);
  
  g_object_get(audio,
	       "notation", &start_list,
	       NULL);
  
  if(start_list == NULL){
    g_object_unref(audio);
    
    return;
  }

  /* get some fields */
  g_object_get(play_notation_audio_run,
	       "recall-id", &recall_id,
	       "recall-audio", &play_notation_audio,
	       "output-soundcard", &output_soundcard,
	       "audio-channel", &audio_channel,
	       "samplerate", &samplerate,
	       "delay-audio-run", &delay_audio_run,
	       "count-beats-audio-run", &count_beats_audio_run,
	       NULL);

  timestamp = play_notation_audio_run->timestamp;
  
  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);
  
  g_object_get(delay_audio_run,
	       "recall-audio", &delay_audio,
	       NULL);

  /* audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get audio channel */
  pthread_mutex_lock(audio_mutex);

  audio_flags = audio->flags;

  input_pads = audio->input_pads;
  output_pads = audio->output_pads;
  
  start_output = audio->output;

  if(start_output != NULL){
    g_object_ref(start_output);
  }
  
  start_input = audio->input;

  if(start_input != NULL){
    g_object_ref(start_input);
  }

  pthread_mutex_unlock(audio_mutex);

  /* get channel */
  if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT)){
    channel = ags_channel_nth(start_input,
			      audio_channel);
    pads = input_pads;
  }else{
    channel = ags_channel_nth(start_output,
			      audio_channel);
    pads = output_pads;
  }
  
  /* play notation */
  notation = NULL;
  
  g_object_get(count_beats_audio_run,
	       "notation-counter", &notation_counter,
	       NULL);

  ags_timestamp_set_ags_offset(timestamp,
			       AGS_NOTATION_DEFAULT_OFFSET * floor(notation_counter / AGS_NOTATION_DEFAULT_OFFSET));
  
  list = ags_notation_find_near_timestamp(start_list, audio_channel,
					  timestamp);
  
  if(list != NULL){
    notation = list->data;
  }

  if(notation != NULL){
    ags_play_notation_audio_run_alloc_input_callback_play_notation(notation);
  }  

  /* unref */
  g_object_unref(audio);
  
  g_object_unref(play_notation_audio);
  
  g_object_unref(output_soundcard);
  
  g_object_unref(recall_id);

  g_object_unref(delay_audio_run);
  
  g_object_unref(count_beats_audio_run);

  g_list_free_full(start_list,
		   g_object_unref);

  if(start_output != NULL){
    g_object_unref(start_output);
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }

  if(channel != NULL){
    g_object_unref(channel);
  }
}

void
ags_play_notation_audio_run_write_resolve_dependency(AgsFileLookup *file_lookup,
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
ags_play_notation_audio_run_read_resolve_dependency(AgsFileLookup *file_lookup,
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
 * ags_play_notation_audio_run_new:
 * @audio: the #AgsAudio
 * @delay_audio_run: the #AgsDelayAudioRun dependency
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun dependency
 *
 * Create a new instance of #AgsPlayNotationAudioRun
 *
 * Returns: the new #AgsPlayNotationAudioRun
 *
 * Since: 2.0.0
 */
AgsPlayNotationAudioRun*
ags_play_notation_audio_run_new(AgsAudio *audio,
				AgsDelayAudioRun *delay_audio_run,
				AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AgsPlayNotationAudioRun *play_notation_audio_run;

  play_notation_audio_run = (AgsPlayNotationAudioRun *) g_object_new(AGS_TYPE_PLAY_NOTATION_AUDIO_RUN,
								     "audio", audio,
								     "delay-audio-run", delay_audio_run,
								     "count-beats-audio-run", count_beats_audio_run,
								     NULL);

  return(play_notation_audio_run);
}
