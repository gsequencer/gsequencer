/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/audio/recall/ags_play_dssi_audio_run.h>
#include <ags/audio/recall/ags_play_dssi_audio.h>

#include <ags/libags.h>

#include <ags/plugin/ags_dssi_manager.h>
#include <ags/plugin/ags_ladspa_conversion.h>
#include <ags/plugin/ags_dssi_plugin.h>

#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ladspa.h>

#include <ags/i18n.h>

void ags_play_dssi_audio_run_class_init(AgsPlayDssiAudioRunClass *play_dssi_audio_run);
void ags_play_dssi_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_dssi_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_play_dssi_audio_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_play_dssi_audio_run_init(AgsPlayDssiAudioRun *play_dssi_audio_run);
void ags_play_dssi_audio_run_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec);
void ags_play_dssi_audio_run_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec);
void ags_play_dssi_audio_run_dispose(GObject *gobject);
void ags_play_dssi_audio_run_finalize(GObject *gobject);
void ags_play_dssi_audio_run_connect(AgsConnectable *connectable);
void ags_play_dssi_audio_run_disconnect(AgsConnectable *connectable);
void ags_play_dssi_audio_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_play_dssi_audio_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable); 
void ags_play_dssi_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_play_dssi_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_play_dssi_audio_run_run_init_pre(AgsRecall *recall);
void ags_play_dssi_audio_run_run_pre(AgsRecall *recall);

void ags_play_dssi_audio_run_resolve_dependencies(AgsRecall *recall);
AgsRecall* ags_play_dssi_audio_run_duplicate(AgsRecall *recall,
					     AgsRecallID *recall_id,
					     guint *n_params, GParameter *parameter);

void ags_play_dssi_audio_run_alloc_input_callback(AgsDelayAudioRun *delay_audio_run,
						  guint nth_run,
						  gdouble delay, guint attack,
						  AgsPlayDssiAudioRun *play_dssi_audio_run);

void ags_play_dssi_audio_run_write_resolve_dependency(AgsFileLookup *file_lookup,
						      GObject *recall);
void ags_play_dssi_audio_run_read_resolve_dependency(AgsFileLookup *file_lookup,
						     GObject *recall);

/**
 * SECTION:ags_play_dssi_audio_run
 * @short_description: play dssi
 * @title: AgsPlayDssiAudioRun
 * @section_id:
 * @include: ags/audio/recall/ags_play_dssi_audio_run.h
 *
 * The #AgsPlayDssiAudioRun class play dssi.
 */

enum{
  PROP_0,
  PROP_DELAY_AUDIO_RUN,
  PROP_COUNT_BEATS_AUDIO_RUN,
  PROP_NOTATION,
};

static gpointer ags_play_dssi_audio_run_parent_class = NULL;
static AgsConnectableInterface* ags_play_dssi_audio_run_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_play_dssi_audio_run_parent_dynamic_connectable_interface;
static AgsPluginInterface *ags_play_dssi_audio_run_parent_plugin_interface;

GType
ags_play_dssi_audio_run_get_type()
{
  static GType ags_type_play_dssi_audio_run = 0;

  if(!ags_type_play_dssi_audio_run){
    static const GTypeInfo ags_play_dssi_audio_run_info = {
      sizeof (AgsPlayDssiAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_dssi_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayDssiAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_dssi_audio_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_dssi_audio_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_dssi_audio_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_play_dssi_audio_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_dssi_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
							  "AgsPlayDssiAudioRun",
							  &ags_play_dssi_audio_run_info,
							  0);

    g_type_add_interface_static(ags_type_play_dssi_audio_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_dssi_audio_run,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_dssi_audio_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return (ags_type_play_dssi_audio_run);
}

void
ags_play_dssi_audio_run_class_init(AgsPlayDssiAudioRunClass *play_dssi_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_play_dssi_audio_run_parent_class = g_type_class_peek_parent(play_dssi_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) play_dssi_audio_run;

  gobject->set_property = ags_play_dssi_audio_run_set_property;
  gobject->get_property = ags_play_dssi_audio_run_get_property;

  gobject->dispose = ags_play_dssi_audio_run_dispose;
  gobject->finalize = ags_play_dssi_audio_run_finalize;

  /* properties */
  /**
   * AgsPlayDssiAudioRun:delay-audio-run:
   *
   * The delay audio run dependency.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("delay-audio-run",
				   i18n_pspec("assigned AgsDelayAudioRun"),
				   i18n_pspec("the AgsDelayAudioRun which emits dssi_alloc_input signal"),
				   AGS_TYPE_DELAY_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY_AUDIO_RUN,
				  param_spec);

  /**
   * AgsPlayDssiAudioRun:count-beats-audio-run:
   *
   * The count beats audio run dependency.
   * 
   * Since: 1.0.0
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
   * AgsPlayDssiAudioRun:notation:
   *
   * The notation containing the notes.
   * 
   * Since: 1.0.0
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
  recall = (AgsRecallClass *) play_dssi_audio_run;

  recall->resolve_dependencies = ags_play_dssi_audio_run_resolve_dependencies;
  recall->duplicate = ags_play_dssi_audio_run_duplicate;

  recall->run_init_pre = ags_play_dssi_audio_run_run_init_pre;
  recall->run_pre = ags_play_dssi_audio_run_run_pre;
}

void
ags_play_dssi_audio_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_play_dssi_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_play_dssi_audio_run_connect;
  connectable->disconnect = ags_play_dssi_audio_run_disconnect;
}

void
ags_play_dssi_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_play_dssi_audio_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_play_dssi_audio_run_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_play_dssi_audio_run_disconnect_dynamic;
}

void
ags_play_dssi_audio_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_play_dssi_audio_run_parent_plugin_interface = g_type_interface_peek_parent(plugin);
  
  plugin->read = ags_play_dssi_audio_run_read;
  plugin->write = ags_play_dssi_audio_run_write;
}

void
ags_play_dssi_audio_run_init(AgsPlayDssiAudioRun *play_dssi_audio_run)
{
  AGS_RECALL(play_dssi_audio_run)->name = "ags-play-dssi";
  AGS_RECALL(play_dssi_audio_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_dssi_audio_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_dssi_audio_run)->xml_type = "ags-play-dssi-audio-run";
  AGS_RECALL(play_dssi_audio_run)->port = NULL;

  play_dssi_audio_run->audio_channels = 0;
  
  play_dssi_audio_run->ladspa_handle = NULL;

  play_dssi_audio_run->input = NULL;
  play_dssi_audio_run->output = NULL;

  play_dssi_audio_run->delta_time = 0;
  
  play_dssi_audio_run->event_buffer = (snd_seq_event_t **) malloc(AGS_PLAY_DSSI_AUDIO_DEFAULT_MIDI_LENGHT * sizeof(snd_seq_event_t *));
  play_dssi_audio_run->event_buffer[0] = NULL;
  
  play_dssi_audio_run->event_count = (unsigned long *) malloc(AGS_PLAY_DSSI_AUDIO_DEFAULT_MIDI_LENGHT * sizeof(unsigned long));
  play_dssi_audio_run->event_count[0] = 0;

  play_dssi_audio_run->key_on = 0;
  
  play_dssi_audio_run->delay_audio_run = NULL;
  play_dssi_audio_run->count_beats_audio_run = NULL;

  play_dssi_audio_run->destination = NULL;

  play_dssi_audio_run->notation = NULL;
  play_dssi_audio_run->timestamp = ags_timestamp_new();

  play_dssi_audio_run->timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  play_dssi_audio_run->timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  play_dssi_audio_run->timestamp->timer.ags_offset.offset = 0;
}

void
ags_play_dssi_audio_run_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec)
{
  AgsPlayDssiAudioRun *play_dssi_audio_run;

  play_dssi_audio_run = AGS_PLAY_DSSI_AUDIO_RUN(gobject);

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      AgsDelayAudioRun *delay_audio_run;
      gboolean is_template;

      delay_audio_run = g_value_get_object(value);

      if(delay_audio_run == play_dssi_audio_run->delay_audio_run){
	return;
      }

      if(delay_audio_run != NULL &&
	 (AGS_RECALL_TEMPLATE & (AGS_RECALL(delay_audio_run)->flags)) != 0){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(play_dssi_audio_run->delay_audio_run != NULL){
	if(is_template){
	  ags_recall_remove_dependency(AGS_RECALL(play_dssi_audio_run),
				       (AgsRecall *) play_dssi_audio_run->delay_audio_run);
	}else{
	  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(play_dssi_audio_run)->flags)) != 0){
	    g_object_disconnect(G_OBJECT(play_dssi_audio_run->delay_audio_run),
				"any_signal::notation-alloc-input",
				G_CALLBACK(ags_play_dssi_audio_run_alloc_input_callback),
				play_dssi_audio_run,
				NULL);
	  }
	}

	g_object_unref(G_OBJECT(play_dssi_audio_run->delay_audio_run));
      }

      if(delay_audio_run != NULL){
	g_object_ref(delay_audio_run);

	if(is_template){
	  ags_recall_add_dependency(AGS_RECALL(play_dssi_audio_run),
				    ags_recall_dependency_new((GObject *) delay_audio_run));
	}else{
	  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(play_dssi_audio_run)->flags)) != 0){
	    g_signal_connect(G_OBJECT(delay_audio_run), "notation-alloc-input",
			     G_CALLBACK(ags_play_dssi_audio_run_alloc_input_callback), play_dssi_audio_run);
	  }
	}
      }

      play_dssi_audio_run->delay_audio_run = delay_audio_run;
    }
    break;
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      AgsCountBeatsAudioRun *count_beats_audio_run;
      gboolean is_template;

      count_beats_audio_run = g_value_get_object(value);

      if(count_beats_audio_run == play_dssi_audio_run->count_beats_audio_run){
	return;
      }

      if((AGS_RECALL_TEMPLATE & (AGS_RECALL(play_dssi_audio_run)->flags)) != 0){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(play_dssi_audio_run->count_beats_audio_run != NULL){
	if(is_template){
	  ags_recall_remove_dependency(AGS_RECALL(play_dssi_audio_run),
				       (AgsRecall *) play_dssi_audio_run->count_beats_audio_run);
	}

	g_object_unref(G_OBJECT(play_dssi_audio_run->count_beats_audio_run));
      }

      if(count_beats_audio_run != NULL){
	g_object_ref(count_beats_audio_run);

	if(is_template){
	  ags_recall_add_dependency(AGS_RECALL(play_dssi_audio_run),
				    ags_recall_dependency_new((GObject *) count_beats_audio_run));
	}
      }

      play_dssi_audio_run->count_beats_audio_run = count_beats_audio_run;
    }
    break;
  case PROP_NOTATION:
    {
      AgsNotation *notation;

      notation = (AgsNotation *) g_value_get_object(value);

      if(play_dssi_audio_run->notation == notation){
	return;
      }

      if(play_dssi_audio_run->notation != NULL){
	g_object_unref(play_dssi_audio_run->notation);
      }

      if(notation != NULL){
	g_object_ref(notation);
      }

      play_dssi_audio_run->notation = notation;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_play_dssi_audio_run_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec)
{
  AgsPlayDssiAudioRun *play_dssi_audio_run;
  
  play_dssi_audio_run = AGS_PLAY_DSSI_AUDIO_RUN(gobject);

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      g_value_set_object(value, G_OBJECT(play_dssi_audio_run->delay_audio_run));
    }
    break;
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      g_value_set_object(value, G_OBJECT(play_dssi_audio_run->count_beats_audio_run));
    }
    break;
  case PROP_NOTATION:
    {
      g_value_set_object(value, play_dssi_audio_run->notation);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_play_dssi_audio_run_dispose(GObject *gobject)
{
  AgsPlayDssiAudioRun *play_dssi_audio_run;

  play_dssi_audio_run = AGS_PLAY_DSSI_AUDIO_RUN(gobject);

  /* delay audio run */
  if(play_dssi_audio_run->delay_audio_run != NULL){
    g_object_unref(G_OBJECT(play_dssi_audio_run->delay_audio_run));

    play_dssi_audio_run->delay_audio_run = NULL;
  }

  /* count beats audio run */
  if(play_dssi_audio_run->count_beats_audio_run != NULL){
    g_object_unref(G_OBJECT(play_dssi_audio_run->count_beats_audio_run));

    play_dssi_audio_run->count_beats_audio_run = NULL;
  }

  /* notation */
  if(play_dssi_audio_run->notation != NULL){
    g_object_unref(G_OBJECT(play_dssi_audio_run->notation));

    play_dssi_audio_run->notation = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_dssi_audio_run_parent_class)->dispose(gobject);
}

void
ags_play_dssi_audio_run_finalize(GObject *gobject)
{
  AgsPlayDssiAudioRun *play_dssi_audio_run;

  play_dssi_audio_run = AGS_PLAY_DSSI_AUDIO_RUN(gobject);

  /* delay audio run */
  if(play_dssi_audio_run->delay_audio_run != NULL){
    g_object_unref(G_OBJECT(play_dssi_audio_run->delay_audio_run));
  }

  /* count beats audio run */
  if(play_dssi_audio_run->count_beats_audio_run != NULL){
    g_object_unref(G_OBJECT(play_dssi_audio_run->count_beats_audio_run));
  }

  /* notation */
  if(play_dssi_audio_run->notation != NULL){
    g_object_unref(G_OBJECT(play_dssi_audio_run->notation));
  }

  /* timestamp */
  if(play_dssi_audio_run->timestamp != NULL){
    g_object_unref(G_OBJECT(play_dssi_audio_run->timestamp));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_dssi_audio_run_parent_class)->finalize(gobject);
}

void
ags_play_dssi_audio_run_connect(AgsConnectable *connectable)
{
  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) != 0){
    return;
  }

  /* call parent */
  ags_play_dssi_audio_run_parent_connectable_interface->connect(connectable);
}

void
ags_play_dssi_audio_run_disconnect(AgsConnectable *connectable)
{
  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) == 0){
    return;
  }

  /* call parent */
  ags_play_dssi_audio_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_play_dssi_audio_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsPlayDssiAudioRun *play_dssi_audio_run;

  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(dynamic_connectable)->flags)) != 0){
    return;
  }

  play_dssi_audio_run = AGS_PLAY_DSSI_AUDIO_RUN(dynamic_connectable);

  /* call parent */
  ags_play_dssi_audio_run_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* connect */
  g_signal_connect(G_OBJECT(play_dssi_audio_run->delay_audio_run), "notation-alloc-input",
		   G_CALLBACK(ags_play_dssi_audio_run_alloc_input_callback), play_dssi_audio_run);  
}

void
ags_play_dssi_audio_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsPlayDssiAudioRun *play_dssi_audio_run;

  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(dynamic_connectable)->flags)) == 0){
    return;
  }

  /* call parent */
  ags_play_dssi_audio_run_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  play_dssi_audio_run = AGS_PLAY_DSSI_AUDIO_RUN(dynamic_connectable);

  if(play_dssi_audio_run->delay_audio_run != NULL){
    g_object_disconnect(G_OBJECT(play_dssi_audio_run->delay_audio_run),
			"any_signal::notation-alloc-input",
			G_CALLBACK(ags_play_dssi_audio_run_alloc_input_callback),
			play_dssi_audio_run,
			NULL);
  }
}

void
ags_play_dssi_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsFileLookup *file_lookup;

  xmlNode *iter;

  /* read parent */
  ags_play_dssi_audio_run_parent_plugin_interface->read(file, node, plugin);

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
			       G_CALLBACK(ags_play_dssi_audio_run_read_resolve_dependency), G_OBJECT(plugin));
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
ags_play_dssi_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsFileLookup *file_lookup;

  xmlNode *node, *child;
  xmlNode *dependency_node;
  
  GList *list;

  gchar *id;

  /* write parent */
  node = ags_play_dssi_audio_run_parent_plugin_interface->write(file, parent, plugin);

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
		     G_CALLBACK(ags_play_dssi_audio_run_write_resolve_dependency), G_OBJECT(plugin));

    list = list->next;
  }

  return(node);
}

void
ags_play_dssi_audio_run_resolve_dependencies(AgsRecall *recall)
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
ags_play_dssi_audio_run_duplicate(AgsRecall *recall,
				  AgsRecallID *recall_id,
				  guint *n_params, GParameter *parameter)
{
  AgsPlayDssiAudioRun *copy;

  copy = AGS_PLAY_DSSI_AUDIO_RUN(AGS_RECALL_CLASS(ags_play_dssi_audio_run_parent_class)->duplicate(recall,
												   recall_id,
												   n_params, parameter));

  return((AgsRecall *) copy);
}

void
ags_play_dssi_audio_run_run_init_pre(AgsRecall *recall)
{  
  AgsPlayDssiAudio *play_dssi_audio;
  AgsPlayDssiAudioRun *play_dssi_audio_run;

  AgsDssiPlugin *dssi_plugin;

  AgsMutexManager *mutex_manager;

  GObject *soundcard;

  guint output_lines, input_lines;
  guint samplerate;
  guint buffer_size;
  unsigned long i, i_stop;
    
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *soundcard_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *recall_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  play_dssi_audio_run = AGS_PLAY_DSSI_AUDIO_RUN(recall);
  play_dssi_audio = AGS_PLAY_DSSI_AUDIO(AGS_RECALL_AUDIO_RUN(play_dssi_audio_run)->recall_audio);

  /* get soundcard */
  soundcard = recall->soundcard;

  /* soundcard mutex */
  pthread_mutex_lock(application_mutex);

  soundcard_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) soundcard);
  
  pthread_mutex_unlock(application_mutex);

  /* get presets */
  pthread_mutex_lock(soundcard_mutex);

  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    NULL,
			    &samplerate,
			    &buffer_size,
			    NULL);

  pthread_mutex_unlock(soundcard_mutex);

  /* recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = AGS_RECALL(play_dssi_audio)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recall_mutex);

  dssi_plugin = play_dssi_audio->plugin;

  output_lines = play_dssi_audio->output_lines;
  input_lines = play_dssi_audio->input_lines;
  
  pthread_mutex_unlock(recall_mutex);
    
  if(input_lines > 0){
    play_dssi_audio_run->input = (LADSPA_Data *) malloc(input_lines *
							buffer_size *
							sizeof(LADSPA_Data));
  }

  play_dssi_audio_run->output = (float *) malloc(output_lines *
						 buffer_size *
						 sizeof(LADSPA_Data));

  if(input_lines < output_lines){
    i_stop = output_lines;
  }else{
    i_stop = input_lines;
  }
  
  play_dssi_audio_run->audio_channels = i_stop;

  if(i_stop > 0){
    play_dssi_audio_run->ladspa_handle = (LADSPA_Handle *) malloc(i_stop *
								  sizeof(LADSPA_Handle));
  }else{
    play_dssi_audio_run->ladspa_handle = NULL;
  }
  
  for(i = 0; i < i_stop; i++){
    /* instantiate dssi */
    play_dssi_audio_run->ladspa_handle[i] = (LADSPA_Handle *) ags_base_plugin_instantiate(AGS_BASE_PLUGIN(dssi_plugin),
											  samplerate);

#ifdef AGS_DEBUG
    g_message("instantiate DSSI handle %d %d",
	      play_dssi_audio->bank,
	      play_dssi_audio->program);
#endif
  }

  ags_play_dssi_audio_run_load_ports(play_dssi_audio_run);

  for(i = 0; i < i_stop; i++){
    ags_base_plugin_activate(AGS_BASE_PLUGIN(dssi_plugin),
			     play_dssi_audio_run->ladspa_handle[i]);
    
#ifdef AGS_DEBUG
    g_message("instantiate DSSI handle");
#endif

  }

  /* call parent */
  AGS_RECALL_CLASS(ags_play_dssi_audio_run_parent_class)->run_init_pre(recall);
}

void
ags_play_dssi_audio_run_run_pre(AgsRecall *recall)
{  
  AgsAudio *audio;
  AgsChannel *output, *input;
  AgsChannel *channel;
  AgsChannel *selected_channel;
  AgsRecycling *recycling;
  AgsRecyclingContext *recycling_context;
  AgsAudioSignal *destination;
  AgsPlayDssiAudio *play_dssi_audio;
  AgsPlayDssiAudioRun *play_dssi_audio_run;
  
  AgsDssiPlugin *dssi_plugin;
  
  AgsMutexManager *mutex_manager;

  GObject *soundcard;

  snd_seq_event_t *seq_event;
  snd_seq_event_t **event_buffer;
  unsigned long *event_count;

  guint audio_flags;
  guint output_lines, input_lines;
  guint audio_channel;
  guint samplerate;
  guint buffer_size;
  guint format;
  guint copy_mode_in, copy_mode_out;
  unsigned long i, i_stop;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *soundcard_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *recycling_mutex;
  pthread_mutex_t *recall_mutex;
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  play_dssi_audio_run = AGS_PLAY_DSSI_AUDIO_RUN(recall);
  play_dssi_audio = AGS_PLAY_DSSI_AUDIO(AGS_RECALL_AUDIO_RUN(play_dssi_audio_run)->recall_audio);

  destination = play_dssi_audio_run->destination;
  
  soundcard = recall->soundcard;

  /* soundcard mutex */
  pthread_mutex_lock(application_mutex);

  soundcard_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) soundcard);
  
  pthread_mutex_unlock(application_mutex);

  /* get presets */
  pthread_mutex_lock(soundcard_mutex);

  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    NULL,
			    NULL,
			    &buffer_size,
			    NULL);

  pthread_mutex_unlock(soundcard_mutex);

  /* recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = AGS_RECALL(play_dssi_audio)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recall_mutex);

  dssi_plugin = play_dssi_audio->plugin;

  input_lines = play_dssi_audio->input_lines;
  output_lines = play_dssi_audio->output_lines;
  
  pthread_mutex_unlock(recall_mutex);

  /* get audio */
  audio = AGS_RECALL_AUDIO_RUN(play_dssi_audio_run)->audio;

  /* audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);
  
  /* get recycling and recycling context */
  pthread_mutex_lock(audio_mutex);

  audio_flags = audio->flags;
  
  output = audio->output;
  input = audio->input;

  recycling = recall->recall_id->recycling;
  recycling_context = recall->recall_id->recycling_context;
  
  pthread_mutex_unlock(audio_mutex);

  /* get recycling mutex */
  pthread_mutex_lock(application_mutex);

  recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) recycling);
	
  pthread_mutex_unlock(application_mutex);

  /* get channel */
  pthread_mutex_lock(recycling_mutex);

  channel = (AgsChannel *) recycling->channel;

  pthread_mutex_unlock(recycling_mutex);

  /* lookup channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel);
	
  pthread_mutex_unlock(application_mutex);

  /* get audio channel */
  pthread_mutex_lock(channel_mutex);
  
  audio_channel = channel->audio_channel;

  pthread_mutex_unlock(channel_mutex);
    
  /* get channel */
  if((AGS_AUDIO_NOTATION_DEFAULT & (audio_flags)) != 0){
    selected_channel = ags_channel_nth(input,
				       audio_channel);
  }else{
    selected_channel = ags_channel_nth(output,
				       audio_channel);
  }

  /* recycling */
  recycling = selected_channel->first_recycling;
  recall->flags &= (~AGS_RECALL_PERSISTENT);

  pthread_mutex_lock(application_mutex);

  recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) recycling);
	
  pthread_mutex_unlock(application_mutex);

  if(destination == NULL){
    gdouble delay;
    guint attack;
    guint length;

    //TODO:JK: unclear
    attack = 0;
    delay = 0.0;

    /* create new audio signal */
    play_dssi_audio_run->destination = 
      destination = ags_audio_signal_new((GObject *) soundcard,
					 (GObject *) recycling,
					 (GObject *) recall->recall_id);
    //TODO:JK: create property
    g_object_ref(play_dssi_audio_run->destination);
  
    ags_recycling_create_audio_signal_with_defaults(recycling,
						    destination,
						    delay, attack);
    length = 1; // (guint) (2.0 * soundcard->delay[soundcard->tic_counter]) + 1;
    ags_audio_signal_stream_resize(destination,
				   length);

    ags_connectable_connect(AGS_CONNECTABLE(destination));
  
    destination->stream_current = destination->stream_beginning;

    pthread_mutex_lock(recycling_mutex);

    ags_recycling_add_audio_signal(recycling,
				   destination);

    pthread_mutex_unlock(recycling_mutex);

#ifdef AGS_DEBUG
    g_message("play %x to %x", destination, recall_id);
    g_message("creating destination");
#endif  
  }

  /* select program */
  if(play_dssi_audio->plugin_descriptor->select_program != NULL){
    AgsPort *current;
    
    GList *list, *port;
    
    gchar *specifier;

    LADSPA_Data *port_data;
    
    unsigned long port_count;

    GValue value = {0,};

    g_value_init(&value,
		 G_TYPE_FLOAT);    
    
    /* retrieve port data */
    pthread_mutex_lock(audio_mutex);

    port = AGS_RECALL(play_dssi_audio)->port;

    port_count = play_dssi_audio->plugin_descriptor->LADSPA_Plugin->PortCount;
    port_data = (LADSPA_Data *) malloc(port_count * sizeof(LADSPA_Data));

    /* update port */
    for(i = 0; i < port_count; i++){
      specifier = play_dssi_audio->plugin_descriptor->LADSPA_Plugin->PortNames[i];
      list = port;

      while(list != NULL){
	current = list->data;

	if(!g_strcmp0(specifier,
		      current->specifier)){
	  break;
	}

	list = list->next;
      }

      if(list == NULL){
	port_data[i] = 0.0;

	//	g_warning("didn't find port");
      }else{
	ags_port_safe_read_raw(current, &value);
      
	port_data[i] = g_value_get_float(&value);
      }
    }

    pthread_mutex_unlock(audio_mutex);

    /* select program */
    pthread_mutex_lock(recall_mutex);

    for(i = 0; i < i_stop; i++){
      play_dssi_audio->plugin_descriptor->select_program(play_dssi_audio_run->ladspa_handle[i],
							 play_dssi_audio->bank,
							 play_dssi_audio->program);

      //      g_message("b p %u %u", play_dssi_audio->bank, play_dssi_audio->program);
    }

    pthread_mutex_unlock(recall_mutex);

    /* reset port data */    
    pthread_mutex_lock(audio_mutex);

    for(i = 0; i < port_count; i++){
      specifier = play_dssi_audio->plugin_descriptor->LADSPA_Plugin->PortNames[i];
      list = port;

      while(list != NULL){
	current = list->data;

	if(!g_strcmp0(specifier,
		      current->specifier)){

	  g_value_set_float(&value, port_data[i]);
	  ags_port_safe_write_raw(current, &value);
	
	  break;
	}

	list = list->next;
      }
    }

    pthread_mutex_unlock(audio_mutex);

    free(port_data);
  }

  /*
   * process data
   */
  
  /* recycling */
  pthread_mutex_lock(channel_mutex);
	
  recycling = selected_channel->first_recycling;

  pthread_mutex_unlock(channel_mutex);
  	
  /* lookup recycling mutex */
  pthread_mutex_lock(application_mutex);

  recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) recycling);
	
  pthread_mutex_unlock(application_mutex);

  /* create audio data */
  pthread_mutex_lock(recycling_mutex);
  
  buffer_size = destination->buffer_size;
  format = destination->format;
  
  ags_audio_buffer_util_clear_buffer(destination->stream_current->data, 1,
				     buffer_size, ags_audio_buffer_util_format_from_soundcard(format));

  pthread_mutex_unlock(recycling_mutex);

  /* get copy mode and clear buffer */
  copy_mode_in = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT,
						     ags_audio_buffer_util_format_from_soundcard(format));

  copy_mode_out = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						      AGS_AUDIO_BUFFER_UTIL_FLOAT);
  
  if(play_dssi_audio_run->output != NULL){
    ags_audio_buffer_util_clear_float(play_dssi_audio_run->output, output_lines,
				      buffer_size);
  }

  if(play_dssi_audio_run->input != NULL){
    ags_audio_buffer_util_clear_float(play_dssi_audio_run->input, input_lines,
				      buffer_size);
  }

  /* copy data */
  pthread_mutex_lock(recycling_mutex);

  if(play_dssi_audio_run->input != NULL){
    ags_audio_buffer_util_copy_buffer_to_buffer(play_dssi_audio_run->input, (guint) input_lines, 0,
						destination->stream_current->data, 1, 0,
						(guint) buffer_size, copy_mode_in);
  }

  /* process data */
  if(play_dssi_audio_run->event_buffer != NULL &&
     play_dssi_audio_run->key_on != 0){
    event_buffer = play_dssi_audio_run->event_buffer;
    event_count = play_dssi_audio_run->event_count;
    
    while(event_buffer[0] != NULL){
      seq_event = event_buffer[0];
      
      ags_base_plugin_run(dssi_plugin,
			  play_dssi_audio_run->ladspa_handle[0],
			  seq_event,
			  buffer_size);
      
      event_buffer++;
      event_count++;
    }
  }

  /* copy data */
  if(play_dssi_audio_run->output != NULL){
    ags_audio_buffer_util_copy_buffer_to_buffer(destination->stream_current->data, 1, 0,
						play_dssi_audio_run->output, (guint) output_lines, 0,
						(guint) buffer_size, copy_mode_out);
  }

  pthread_mutex_unlock(recycling_mutex);

  /* call parent */
  AGS_RECALL_CLASS(ags_play_dssi_audio_run_parent_class)->run_pre(recall);
}

void
ags_play_dssi_audio_run_alloc_input_callback(AgsDelayAudioRun *delay_audio_run,
					     guint nth_run,
					     gdouble delay, guint attack,
					     AgsPlayDssiAudioRun *play_dssi_audio_run)
{
  GObject *soundcard;
  AgsAudio *audio;
  AgsChannel *output, *input;
  AgsChannel *channel;
  AgsChannel *selected_channel;
  AgsRecycling *recycling;
  AgsNotation *notation;
  AgsNote *note;

  AgsPlayDssiAudio *play_dssi_audio;
  AgsDelayAudio *delay_audio;
  
  AgsMutexManager *mutex_manager;

  snd_seq_event_t *seq_event;
  snd_seq_event_t **event_buffer;
  unsigned long *event_count;

  GList *list;
  GList *current_position;
  GList *start_append_note, *append_note;
  GList *start_remove_note, *remove_note;

  guint audio_flags;
  guint audio_start_mapping;
  guint midi_start_mapping, midi_end_mapping;
  guint notation_counter;
  guint note_x0, note_x1;
  guint note_y;  
  guint input_pads;
  guint selected_key;
  guint audio_channel;
  guint i;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *recycling_mutex;

  if(delay != 0.0){
    return;
  }  

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  play_dssi_audio = AGS_PLAY_DSSI_AUDIO(AGS_RECALL_AUDIO_RUN(play_dssi_audio_run)->recall_audio);

  delay_audio = AGS_DELAY_AUDIO(AGS_RECALL_AUDIO_RUN(delay_audio_run)->recall_audio);
  
  soundcard = (GObject *) AGS_RECALL(play_dssi_audio_run)->soundcard;

  /* get audio */
  audio = AGS_RECALL_AUDIO_RUN(play_dssi_audio_run)->audio;

  /* audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /*  */
  pthread_mutex_lock(audio_mutex);

  audio_flags = audio->flags;

  output = audio->output;
  input = audio->input;

  list = audio->notation;//(GList *) g_value_get_pointer(&value);

  pthread_mutex_unlock(audio_mutex);
  
  if(list == NULL){
    return;
  }

  /* get recycling and recycling context */
  pthread_mutex_lock(audio_mutex);

  recycling = AGS_RECALL(play_dssi_audio_run)->recall_id->recycling;
  
  pthread_mutex_unlock(audio_mutex);

  /* get recycling mutex */
  pthread_mutex_lock(application_mutex);

  recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) recycling);
	
  pthread_mutex_unlock(application_mutex);

  /* get channel */
  pthread_mutex_lock(recycling_mutex);

  channel = (AgsChannel *) recycling->channel;

  pthread_mutex_unlock(recycling_mutex);

  /* lookup channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel);
	
  pthread_mutex_unlock(application_mutex);

  /* get audio channel */
  pthread_mutex_lock(channel_mutex);
  
  audio_channel = channel->audio_channel;

  pthread_mutex_unlock(channel_mutex);

  /* get channel */
  if((AGS_AUDIO_NOTATION_DEFAULT & (audio_flags)) != 0){
    selected_channel = ags_channel_nth(input,
				       audio_channel);
  }else{
    selected_channel = ags_channel_nth(output,
				       audio_channel);
  }
  
  /* get notation */
  notation = NULL;
  current_position = NULL;
  
  start_append_note = NULL;
  start_remove_note = NULL;

  pthread_mutex_lock(audio_mutex);
  
  notation_counter = play_dssi_audio_run->count_beats_audio_run->notation_counter;

  input_pads = audio->input_pads;

  audio_start_mapping = audio->audio_start_mapping;

  midi_start_mapping = audio->midi_start_mapping;
  midi_end_mapping = audio->midi_end_mapping;

  play_dssi_audio_run->timestamp->timer.ags_offset.offset = AGS_NOTATION_DEFAULT_OFFSET * floor(notation_counter / AGS_NOTATION_DEFAULT_OFFSET);

  list = ags_notation_find_near_timestamp(audio->notation, audio_channel,
					  play_dssi_audio_run->timestamp);
  
  if(list != NULL){
    notation = list->data;
    
    current_position = notation->notes;
  }
  
  /*
   * feed midi
   */  
  while(current_position != NULL){
    gboolean success;
      
    note = AGS_NOTE(current_position->data);

    note_x0 = note->x[0];
    note_x1 = note->x[1];

    note_y = note->y;

    if(note_x0 == notation_counter){
      start_append_note = g_list_prepend(start_append_note,
					 note);
      g_object_ref(note);
    }else if(note_x1 == notation_counter){
      start_remove_note = g_list_prepend(start_remove_note,
					 note);
      g_object_ref(note);
    }else if(note_x0 > notation_counter){
      break;
    }
    
    /* iterate */
    current_position = current_position->next;
  }
  
  pthread_mutex_unlock(audio_mutex);

  append_note =
    start_append_note = g_list_reverse(start_append_note);
  
  remove_note =
    start_remove_note = g_list_reverse(start_remove_note);


  /* append */
  while(append_note != NULL){
    note = append_note->data;
    
    pthread_mutex_lock(audio_mutex);

    note_y = note->y;
    
    pthread_mutex_unlock(audio_mutex);
  
    /* send key-on */
    if((AGS_AUDIO_REVERSE_MAPPING & (audio_flags)) != 0){
      selected_key = input_pads - note_y - 1;
    }else{
      selected_key = note_y;
    }

    /* key on */
    seq_event = (snd_seq_event_t *) malloc(sizeof(snd_seq_event_t));
    memset(seq_event, 0, sizeof(snd_seq_event_t));
    //memset(seq_event, 0, sizeof(snd_seq_event_t));
	      
    seq_event->type = SND_SEQ_EVENT_NOTEON;

    seq_event->data.note.channel = 0;
    seq_event->data.note.note = 0x7f & (selected_key - audio_start_mapping + midi_start_mapping);
    seq_event->data.note.velocity = 127;

    /* find end */
    i = 0;
      
    if(play_dssi_audio_run->event_buffer != NULL){
      event_buffer = play_dssi_audio_run->event_buffer;
      
      while(event_buffer[0] != NULL){
	event_buffer++;
	event_count++;
	i++;
      }

      if(i + 1 < AGS_PLAY_DSSI_AUDIO_DEFAULT_MIDI_LENGHT){
	play_dssi_audio_run->event_buffer[i] = seq_event;
	play_dssi_audio_run->event_buffer[i + 1] = NULL;
		  
	play_dssi_audio_run->event_count[i] = 1;
	play_dssi_audio_run->event_count[i + 1] = 0;

	play_dssi_audio_run->key_on += 1;
      }
    }
    
    /* iterate */
    g_object_unref(append_note->data);

    append_note = append_note->next;
  }

  /* remove */
  while(remove_note != NULL){
    gint match_index;
    gboolean success;
      
    note = remove_note->data;
    
    pthread_mutex_lock(audio_mutex);

    note_y = note->y;
    
    pthread_mutex_unlock(audio_mutex);
  
    /* send key-on */
    if((AGS_AUDIO_REVERSE_MAPPING & (audio_flags)) != 0){
      selected_key = input_pads - note_y - 1;
    }else{
      selected_key = note_y;
    }
    
    /* find end */
    i = 0;
    match_index = -1;
      
    if(play_dssi_audio_run->event_buffer != NULL){
      event_buffer = play_dssi_audio_run->event_buffer;
      event_count = play_dssi_audio_run->event_count;
      
      while(event_buffer[0] != NULL){
	if(event_buffer[0]->data.note.note == (0x7f & (selected_key - audio_start_mapping + midi_start_mapping))){
	  match_index = i;
	  //	    free(event_buffer[0]);
	}
	  
	event_buffer++;
	event_count++;
	i++;
      }
    }

    /* clear note */
    if(i > 0 &&
       match_index != -1){
      if(match_index + 1 != i){
	memmove(&(play_dssi_audio_run->event_buffer[match_index]),
		&(play_dssi_audio_run->event_buffer[match_index + 1]),
		i - 1);
      }

      play_dssi_audio_run->event_buffer[i - 1] = NULL;
      play_dssi_audio_run->event_count[i - 1] = 0;
	
      play_dssi_audio_run->event_buffer[i] = NULL;
      play_dssi_audio_run->event_count[i] = 0;

      if(play_dssi_audio_run->key_on > 0){
	play_dssi_audio_run->key_on -= 1;
      }
    }

    /* iterate */
    g_object_unref(remove_note->data);

    remove_note = remove_note->next;
  }

  g_list_free(start_append_note);
  g_list_free(start_remove_note);
}  

void
ags_play_dssi_audio_run_write_resolve_dependency(AgsFileLookup *file_lookup,
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
ags_play_dssi_audio_run_read_resolve_dependency(AgsFileLookup *file_lookup,
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
 * ags_play_dssi_audio_run_load_ports:
 * @play_dssi_audio_run: an #AgsRecallDssiRun
 *
 * Set up DSSI ports.
 *
 * Since: 1.0.0
 */
void
ags_play_dssi_audio_run_load_ports(AgsPlayDssiAudioRun *play_dssi_audio_run)
{
  AgsPlayDssiAudio *play_dssi_audio;
  AgsDssiPlugin *dssi_plugin;
  AgsPort *current;

  GList *port;
  GList *list;

  gchar *plugin_name;
  gchar *specifier;
  gchar *path;
  
  guint output_lines, input_lines;
  unsigned long port_count;
  unsigned long i, j, j_stop;

  DSSI_Descriptor *plugin_descriptor;
  LADSPA_PortDescriptor *port_descriptor;
  
  pthread_mutex_t *recall_mutex;
  pthread_mutex_t *base_plugin_mutex;

  play_dssi_audio = AGS_PLAY_DSSI_AUDIO(AGS_RECALL_AUDIO_RUN(play_dssi_audio_run)->recall_audio);

  /* recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = AGS_RECALL(play_dssi_audio)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recall_mutex);

  port = AGS_RECALL(play_dssi_audio)->port;

  dssi_plugin = play_dssi_audio->plugin;

  plugin_descriptor = play_dssi_audio->plugin_descriptor;

  input_lines = play_dssi_audio->input_lines;
  output_lines = play_dssi_audio->output_lines;

  pthread_mutex_unlock(recall_mutex);
    
  /* base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());

  base_plugin_mutex = AGS_BASE_PLUGIN(dssi_plugin)->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(base_plugin_mutex);

  port_count = plugin_descriptor->LADSPA_Plugin->PortCount;
  
  port_descriptor = plugin_descriptor->LADSPA_Plugin->PortDescriptors;

  pthread_mutex_unlock(base_plugin_mutex);

  if(input_lines < output_lines){
    j_stop = output_lines;
  }else{
    j_stop = input_lines;
  }

  for(i = 0; i < port_count; i++){
    int descriptor;
    
    pthread_mutex_lock(base_plugin_mutex);

    descriptor = port_descriptor[i];
    
    pthread_mutex_unlock(base_plugin_mutex);

    if(LADSPA_IS_PORT_CONTROL(descriptor)){
      if(LADSPA_IS_PORT_INPUT(descriptor) ||
	 LADSPA_IS_PORT_OUTPUT(descriptor)){
	LADSPA_Data *port_pointer;
	
	pthread_mutex_lock(base_plugin_mutex);
	
	specifier = g_strdup(plugin_descriptor->LADSPA_Plugin->PortNames[i]);

	pthread_mutex_unlock(base_plugin_mutex);

	/* check port */
	pthread_mutex_lock(recall_mutex);

	list = port;
	current = NULL;
	
	while(list != NULL){
	  current = list->data;

	  if(!g_strcmp0(specifier,
			current->specifier)){
	    break;
	  }

	  list = list->next;
	}

	pthread_mutex_unlock(recall_mutex);

	g_free(specifier);
	
	for(j = 0; j < j_stop; j++){
#ifdef AGS_DEBUG
	  g_message("connecting port[%d]: %d/%d - %f", j, i, port_count, current->port_value.ags_port_ladspa);
#endif
	  pthread_mutex_lock(current->mutex);
	  
	  port_pointer = (LADSPA_Data *) &(current->port_value.ags_port_ladspa);

	  pthread_mutex_unlock(current->mutex);
	  
	  /* connect */
	  pthread_mutex_lock(base_plugin_mutex);

	  plugin_descriptor->LADSPA_Plugin->connect_port(play_dssi_audio_run->ladspa_handle[j],
							 i,
							 port_pointer);

	  pthread_mutex_unlock(base_plugin_mutex);
	}
      }
    }
  }

  /* connect audio port */
  for(j = 0; j < play_dssi_audio->input_lines; j++){
    unsigned long port_index;

    /* port index */
    pthread_mutex_lock(recall_mutex);
    
    port_index = play_dssi_audio->input_port[j];

    pthread_mutex_unlock(recall_mutex);

    /* connect */
    pthread_mutex_lock(base_plugin_mutex);

    plugin_descriptor->LADSPA_Plugin->connect_port(play_dssi_audio_run->ladspa_handle[j],
						   port_index,
						   &(play_dssi_audio_run->input[j]));

    pthread_mutex_unlock(base_plugin_mutex);
  }
  
  for(j = 0; j < play_dssi_audio->output_lines; j++){    
    unsigned long port_index;

    /* port index */
    pthread_mutex_lock(recall_mutex);

    port_index = play_dssi_audio->output_port[j];

    pthread_mutex_unlock(recall_mutex);

    /* connect */
    pthread_mutex_lock(base_plugin_mutex);

    plugin_descriptor->LADSPA_Plugin->connect_port(play_dssi_audio_run->ladspa_handle[j],
						   port_index,
						   &(play_dssi_audio_run->output[j]));

    pthread_mutex_unlock(base_plugin_mutex);
  }
}

/**
 * ags_play_dssi_audio_run_new:
 *
 * Creates an #AgsPlayDssiAudioRun
 *
 * Returns: a new #AgsPlayDssiAudioRun
 *
 * Since: 1.0.0
 */
AgsPlayDssiAudioRun*
ags_play_dssi_audio_run_new()
{
  AgsPlayDssiAudioRun *play_dssi_audio_run;

  play_dssi_audio_run = (AgsPlayDssiAudioRun *) g_object_new(AGS_TYPE_PLAY_DSSI_AUDIO_RUN,
							     NULL);

  return(play_dssi_audio_run);
}
