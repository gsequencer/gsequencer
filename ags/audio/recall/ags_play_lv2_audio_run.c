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

#include <ags/audio/recall/ags_play_lv2_audio_run.h>
#include <ags/audio/recall/ags_play_lv2_audio.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_dynamic_connectable.h>
#include <ags/object/ags_plugin.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>

#include <ags/plugin/ags_lv2_plugin.h>
#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2_event_manager.h>
#include <ags/plugin/ags_lv2_uri_map_manager.h>
#include <ags/plugin/ags_lv2_log_manager.h>
#include <ags/plugin/ags_lv2_worker_manager.h>
#include <ags/plugin/ags_lv2_worker.h>
#include <ags/plugin/ags_lv2_urid_manager.h>
#include <ags/plugin/ags_lv2_option_manager.h>

#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <lv2.h>
#include <lv2/lv2plug.in/ns/ext/event/event.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include <lv2/lv2plug.in/ns/ext/uri-map/uri-map.h>
#include <lv2/lv2plug.in/ns/ext/worker/worker.h>
#include <lv2/lv2plug.in/ns/ext/log/log.h>
#include <lv2/lv2plug.in/ns/ext/event/event.h>
#include <lv2/lv2plug.in/ns/ext/parameters/parameters.h>
#include <lv2/lv2plug.in/ns/ext/buf-size/buf-size.h>
#include <lv2/lv2plug.in/ns/ext/options/options.h>

void ags_play_lv2_audio_run_class_init(AgsPlayLv2AudioRunClass *play_lv2_audio_run);
void ags_play_lv2_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_lv2_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_play_lv2_audio_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_play_lv2_audio_run_init(AgsPlayLv2AudioRun *play_lv2_audio_run);
void ags_play_lv2_audio_run_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_play_lv2_audio_run_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_play_lv2_audio_run_dispose(GObject *gobject);
void ags_play_lv2_audio_run_finalize(GObject *gobject);
void ags_play_lv2_audio_run_connect(AgsConnectable *connectable);
void ags_play_lv2_audio_run_disconnect(AgsConnectable *connectable);
void ags_play_lv2_audio_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_play_lv2_audio_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable); 
void ags_play_lv2_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_play_lv2_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_play_lv2_audio_run_run_init_pre(AgsRecall *recall);
void ags_play_lv2_audio_run_run_pre(AgsRecall *recall);

void ags_play_lv2_audio_run_resolve_dependencies(AgsRecall *recall);
AgsRecall* ags_play_lv2_audio_run_duplicate(AgsRecall *recall,
					    AgsRecallID *recall_id,
					    guint *n_params, GParameter *parameter);

void ags_play_lv2_audio_run_alloc_input_callback(AgsDelayAudioRun *delay_audio_run,
						 guint nth_run,
						 gdouble delay, guint attack,
						 AgsPlayLv2AudioRun *play_lv2_audio_run);

void ags_play_lv2_audio_run_write_resolve_dependency(AgsFileLookup *file_lookup,
						     GObject *recall);
void ags_play_lv2_audio_run_read_resolve_dependency(AgsFileLookup *file_lookup,
						    GObject *recall);

/**
 * SECTION:ags_play_lv2_audio_run
 * @short_description: play lv2
 * @title: AgsPlayLv2AudioRun
 * @section_id:
 * @include: ags/audio/recall/ags_play_lv2_audio_run.h
 *
 * The #AgsPlayLv2AudioRun class play lv2.
 */

enum{
  PROP_0,
  PROP_DELAY_AUDIO_RUN,
  PROP_COUNT_BEATS_AUDIO_RUN,
  PROP_NOTATION,
};

static gpointer ags_play_lv2_audio_run_parent_class = NULL;
static AgsConnectableInterface* ags_play_lv2_audio_run_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_play_lv2_audio_run_parent_dynamic_connectable_interface;
static AgsPluginInterface *ags_play_lv2_audio_run_parent_plugin_interface;

GType
ags_play_lv2_audio_run_get_type()
{
  static GType ags_type_play_lv2_audio_run = 0;

  if(!ags_type_play_lv2_audio_run){
    static const GTypeInfo ags_play_lv2_audio_run_info = {
      sizeof (AgsPlayLv2AudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_lv2_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayLv2AudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_lv2_audio_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_lv2_audio_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_lv2_audio_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_play_lv2_audio_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_lv2_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
							 "AgsPlayLv2AudioRun\0",
							 &ags_play_lv2_audio_run_info,
							 0);

    g_type_add_interface_static(ags_type_play_lv2_audio_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_lv2_audio_run,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_lv2_audio_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return (ags_type_play_lv2_audio_run);
}

void
ags_play_lv2_audio_run_class_init(AgsPlayLv2AudioRunClass *play_lv2_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_play_lv2_audio_run_parent_class = g_type_class_peek_parent(play_lv2_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) play_lv2_audio_run;

  gobject->set_property = ags_play_lv2_audio_run_set_property;
  gobject->get_property = ags_play_lv2_audio_run_get_property;

  gobject->dispose = ags_play_lv2_audio_run_dispose;
  gobject->finalize = ags_play_lv2_audio_run_finalize;

  /* properties */
  /**
   * AgsPlayLv2AudioRun:delay-audio-run:
   *
   * The delay audio run dependency.
   * 
   * Since: 0.7.134
   */
  param_spec = g_param_spec_object("delay-audio-run\0",
				   "assigned AgsDelayAudioRun\0",
				   "the AgsDelayAudioRun which emits lv2_alloc_input signal\0",
				   AGS_TYPE_DELAY_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY_AUDIO_RUN,
				  param_spec);

  /**
   * AgsPlayLv2AudioRun:count-beats-audio-run:
   *
   * The count beats audio run dependency.
   * 
   * Since: 0.7.134
   */
  param_spec = g_param_spec_object("count-beats-audio-run\0",
				   "assigned AgsCountBeatsAudioRun\0",
				   "the AgsCountBeatsAudioRun which just counts\0",
				   AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COUNT_BEATS_AUDIO_RUN,
				  param_spec);

  /**
   * AgsPlayLv2AudioRun:notation:
   *
   * The notation containing the notes.
   * 
   * Since: 0.7.134
   */
  param_spec = g_param_spec_object("notation\0",
				   "assigned AgsNotation\0",
				   "The AgsNotation containing notes\0",
				   AGS_TYPE_NOTATION,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) play_lv2_audio_run;

  recall->resolve_dependencies = ags_play_lv2_audio_run_resolve_dependencies;
  recall->duplicate = ags_play_lv2_audio_run_duplicate;

  recall->run_init_pre = ags_play_lv2_audio_run_run_init_pre;
  recall->run_pre = ags_play_lv2_audio_run_run_pre;
}

void
ags_play_lv2_audio_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_play_lv2_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_play_lv2_audio_run_connect;
  connectable->disconnect = ags_play_lv2_audio_run_disconnect;
}

void
ags_play_lv2_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_play_lv2_audio_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_play_lv2_audio_run_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_play_lv2_audio_run_disconnect_dynamic;
}

void
ags_play_lv2_audio_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_play_lv2_audio_run_parent_plugin_interface = g_type_interface_peek_parent(plugin);
  
  plugin->read = ags_play_lv2_audio_run_read;
  plugin->write = ags_play_lv2_audio_run_write;
}

void
ags_play_lv2_audio_run_init(AgsPlayLv2AudioRun *play_lv2_audio_run)
{
  AGS_RECALL(play_lv2_audio_run)->name = "ags-play-lv2\0";
  AGS_RECALL(play_lv2_audio_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_lv2_audio_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_lv2_audio_run)->xml_type = "ags-play-lv2-audio-run\0";
  AGS_RECALL(play_lv2_audio_run)->port = NULL;
  
  play_lv2_audio_run->feature = NULL;
  
  play_lv2_audio_run->input = NULL;
  play_lv2_audio_run->output = NULL;

  play_lv2_audio_run->event_port = NULL;
  play_lv2_audio_run->atom_port = NULL;
  
  play_lv2_audio_run->delta_time = 0;
  
  play_lv2_audio_run->event_buffer = NULL;
  play_lv2_audio_run->event_count = NULL;
  
  play_lv2_audio_run->worker_handle = NULL;

  play_lv2_audio_run->delay_audio_run = NULL;
  play_lv2_audio_run->count_beats_audio_run = NULL;

  play_lv2_audio_run->destination = NULL;  
  
  play_lv2_audio_run->notation = NULL;
  play_lv2_audio_run->offset = NULL;
}

void
ags_play_lv2_audio_run_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsPlayLv2AudioRun *play_lv2_audio_run;

  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(gobject);

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      AgsDelayAudioRun *delay_audio_run;
      gboolean is_template;

      delay_audio_run = g_value_get_object(value);

      if(delay_audio_run == play_lv2_audio_run->delay_audio_run){
	return;
      }

      if(delay_audio_run != NULL &&
	 (AGS_RECALL_TEMPLATE & (AGS_RECALL(delay_audio_run)->flags)) != 0){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(play_lv2_audio_run->delay_audio_run != NULL){
	if(is_template){
	  ags_recall_remove_dependency(AGS_RECALL(play_lv2_audio_run),
				       (AgsRecall *) play_lv2_audio_run->delay_audio_run);
	}else{
	  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(play_lv2_audio_run)->flags)) != 0){
	    g_object_disconnect(G_OBJECT(play_lv2_audio_run->delay_audio_run),
				"notation-alloc-input\0",
				G_CALLBACK(ags_play_lv2_audio_run_alloc_input_callback),
				play_lv2_audio_run,
				NULL);
	  }
	}

	g_object_unref(G_OBJECT(play_lv2_audio_run->delay_audio_run));
      }

      if(delay_audio_run != NULL){
	g_object_ref(delay_audio_run);

	if(is_template){
	  ags_recall_add_dependency(AGS_RECALL(play_lv2_audio_run),
				    ags_recall_dependency_new((GObject *) delay_audio_run));
	}else{
	  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(play_lv2_audio_run)->flags)) != 0){
	    g_signal_connect(G_OBJECT(delay_audio_run), "notation-alloc-input\0",
			     G_CALLBACK(ags_play_lv2_audio_run_alloc_input_callback), play_lv2_audio_run);
	  }
	}
      }

      play_lv2_audio_run->delay_audio_run = delay_audio_run;
    }
    break;
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      AgsCountBeatsAudioRun *count_beats_audio_run;
      gboolean is_template;

      count_beats_audio_run = g_value_get_object(value);

      if(count_beats_audio_run == play_lv2_audio_run->count_beats_audio_run){
	return;
      }

      if(count_beats_audio_run != NULL &&
	 (AGS_RECALL_TEMPLATE & (AGS_RECALL(count_beats_audio_run)->flags)) != 0){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(play_lv2_audio_run->count_beats_audio_run != NULL){
	if(is_template){
	  ags_recall_remove_dependency(AGS_RECALL(play_lv2_audio_run),
				       (AgsRecall *) play_lv2_audio_run->count_beats_audio_run);
	}

	g_object_unref(G_OBJECT(play_lv2_audio_run->count_beats_audio_run));
      }

      if(count_beats_audio_run != NULL){
	g_object_ref(count_beats_audio_run);

	if(is_template){
	  ags_recall_add_dependency(AGS_RECALL(play_lv2_audio_run),
				    ags_recall_dependency_new((GObject *) count_beats_audio_run));
	}
      }

      play_lv2_audio_run->count_beats_audio_run = count_beats_audio_run;
    }
    break;
  case PROP_NOTATION:
    {
      AgsNotation *notation;

      notation = (AgsNotation *) g_value_get_object(value);

      if(play_lv2_audio_run->notation == notation){
	return;
      }

      if(play_lv2_audio_run->notation != NULL){
	g_object_unref(play_lv2_audio_run->notation);
      }

      if(notation != NULL){
	g_object_ref(notation);
      }

      play_lv2_audio_run->notation = notation;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_play_lv2_audio_run_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsPlayLv2AudioRun *play_lv2_audio_run;
  
  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(gobject);

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      g_value_set_object(value, G_OBJECT(play_lv2_audio_run->delay_audio_run));
    }
    break;
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      g_value_set_object(value, G_OBJECT(play_lv2_audio_run->count_beats_audio_run));
    }
    break;
  case PROP_NOTATION:
    {
      g_value_set_object(value, play_lv2_audio_run->notation);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_play_lv2_audio_run_dispose(GObject *gobject)
{
  AgsPlayLv2AudioRun *play_lv2_audio_run;

  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(gobject);

  /* delay audio run */
  if(play_lv2_audio_run->delay_audio_run != NULL){
    g_object_unref(G_OBJECT(play_lv2_audio_run->delay_audio_run));

    play_lv2_audio_run->delay_audio_run = NULL;
  }

  /* count beats audio run */
  if(play_lv2_audio_run->count_beats_audio_run != NULL){
    g_object_unref(G_OBJECT(play_lv2_audio_run->count_beats_audio_run));

    play_lv2_audio_run->count_beats_audio_run = NULL;
  }

  /* notation */
  if(play_lv2_audio_run->notation != NULL){
    g_object_unref(G_OBJECT(play_lv2_audio_run->notation));

    play_lv2_audio_run->notation = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_lv2_audio_run_parent_class)->dispose(gobject);
}

void
ags_play_lv2_audio_run_finalize(GObject *gobject)
{
  AgsPlayLv2AudioRun *play_lv2_audio_run;

  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(gobject);

  /* delay audio run */
  if(play_lv2_audio_run->delay_audio_run != NULL){
    g_object_unref(G_OBJECT(play_lv2_audio_run->delay_audio_run));
  }

  /* count beats audio run */
  if(play_lv2_audio_run->count_beats_audio_run != NULL){
    g_object_unref(G_OBJECT(play_lv2_audio_run->count_beats_audio_run));
  }

  /* notation */
  if(play_lv2_audio_run->notation != NULL){
    g_object_unref(G_OBJECT(play_lv2_audio_run->notation));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_lv2_audio_run_parent_class)->finalize(gobject);
}

void
ags_play_lv2_audio_run_connect(AgsConnectable *connectable)
{
  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) != 0){
    return;
  }

  /* call parent */
  ags_play_lv2_audio_run_parent_connectable_interface->connect(connectable);
}

void
ags_play_lv2_audio_run_disconnect(AgsConnectable *connectable)
{
  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) == 0){
    return;
  }

  /* call parent */
  ags_play_lv2_audio_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_play_lv2_audio_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsPlayLv2AudioRun *play_lv2_audio_run;

  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(dynamic_connectable)->flags)) != 0){
    return;
  }

  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(dynamic_connectable);

  /* call parent */
  ags_play_lv2_audio_run_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* connect */
  g_signal_connect(G_OBJECT(play_lv2_audio_run->delay_audio_run), "notation-alloc-input\0",
		   G_CALLBACK(ags_play_lv2_audio_run_alloc_input_callback), play_lv2_audio_run);  
}

void
ags_play_lv2_audio_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsPlayLv2AudioRun *play_lv2_audio_run;

  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(dynamic_connectable)->flags)) == 0){
    return;
  }

  /* call parent */
  ags_play_lv2_audio_run_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(dynamic_connectable);

  if(play_lv2_audio_run->delay_audio_run != NULL){
    g_object_disconnect(G_OBJECT(play_lv2_audio_run->delay_audio_run),
			"notation-alloc-input\0",
			G_CALLBACK(ags_play_lv2_audio_run_alloc_input_callback),
			play_lv2_audio_run,
			NULL);
  }
}

void
ags_play_lv2_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsFileLookup *file_lookup;

  xmlNode *iter;

  /* read parent */
  ags_play_lv2_audio_run_parent_plugin_interface->read(file, node, plugin);

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
			       G_CALLBACK(ags_play_lv2_audio_run_read_resolve_dependency), G_OBJECT(plugin));
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
ags_play_lv2_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsFileLookup *file_lookup;

  xmlNode *node, *child;
  xmlNode *dependency_node;
  
  GList *list;

  gchar *id;

  /* write parent */
  node = ags_play_lv2_audio_run_parent_plugin_interface->write(file, parent, plugin);

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
		     G_CALLBACK(ags_play_lv2_audio_run_write_resolve_dependency), G_OBJECT(plugin));

    list = list->next;
  }

  return(node);
}

void
ags_play_lv2_audio_run_resolve_dependencies(AgsRecall *recall)
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
	       "delay-audio-run\0", delay_audio_run,
	       "count-beats-audio-run\0", count_beats_audio_run,
	       NULL);
}

AgsRecall*
ags_play_lv2_audio_run_duplicate(AgsRecall *recall,
				 AgsRecallID *recall_id,
				 guint *n_params, GParameter *parameter)
{
  AgsPlayLv2AudioRun *copy;

  copy = AGS_PLAY_LV2_AUDIO_RUN(AGS_RECALL_CLASS(ags_play_lv2_audio_run_parent_class)->duplicate(recall,
												 recall_id,
												 n_params, parameter));

  return((AgsRecall *) copy);
}

void
ags_play_lv2_audio_run_run_init_pre(AgsRecall *recall)
{
  GObject *soundcard;
  
  AgsPlayLv2Audio *play_lv2_audio;
  AgsPlayLv2AudioRun *play_lv2_audio_run;

  AgsMutexManager *mutex_manager;
  
  gchar *path;

  guint total_feature;
  guint nth;
  guint audio_channel;
  guint samplerate;
  guint buffer_size;
  float *ptr_samplerate;
  float *ptr_buffer_size;
  uint32_t i;

  LV2_URI_Map_Feature *uri_map_feature;
  
  LV2_Worker_Schedule_Handle worker_handle;
  LV2_Worker_Schedule *worker_schedule;

  LV2_Log_Log *log_feature;

  LV2_Event_Feature *event_feature;

  LV2_URID_Map *urid_map;
  LV2_URID_Unmap *urid_unmap;

  LV2_Options_Interface *options_interface;
  LV2_Options_Option *options;
  
  LV2_Feature **feature;
  
  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(recall);
  play_lv2_audio = AGS_PLAY_LV2_AUDIO(AGS_RECALL_AUDIO_RUN(play_lv2_audio_run)->recall_audio);

  /* set up feature */
  soundcard = recall->soundcard;

  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    NULL,
			    &samplerate,
			    &buffer_size,
			    NULL);
  
  path = g_strndup(play_lv2_audio->filename,
		   rindex(play_lv2_audio->filename, '/') - play_lv2_audio->filename + 1);

  /**/
  total_feature = 8;
  nth = 0;
    
  play_lv2_audio_run->feature = 
    feature = (LV2_Feature **) malloc(total_feature * sizeof(LV2_Feature *));
  
  /* URI map feature */  
  uri_map_feature = (LV2_URI_Map_Feature *) malloc(sizeof(LV2_URI_Map_Feature));
  uri_map_feature->callback_data = NULL;
  uri_map_feature->uri_to_id = ags_lv2_uri_map_manager_uri_to_id;
  
  feature[nth] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
  feature[nth]->URI = LV2_URI_MAP_URI;
  feature[nth]->data = uri_map_feature;

  nth++;
  
  /* worker feature */
  if((AGS_PLAY_LV2_AUDIO_HAS_WORKER & (play_lv2_audio->flags)) != 0){
    play_lv2_audio_run->worker_handle = 
      worker_handle = ags_lv2_worker_manager_pull_worker(ags_lv2_worker_manager_get_instance());
  
    worker_schedule = (LV2_Worker_Schedule *) malloc(sizeof(LV2_Worker_Schedule));
    worker_schedule->handle = worker_handle;
    worker_schedule->schedule_work = ags_lv2_worker_schedule_work;
  
    feature[nth] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
    feature[nth]->URI = LV2_WORKER__schedule;
    feature[nth]->data = worker_schedule;

    nth++;
  }else{
    worker_handle = NULL;
  }
  
  /* log feature */
  log_feature = (LV2_Log_Log *) malloc(sizeof(LV2_Log_Log));
  
  log_feature->handle = NULL;
  log_feature->printf = ags_lv2_log_manager_printf;
  log_feature->vprintf = ags_lv2_log_manager_vprintf;

  feature[nth] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
  feature[nth]->URI = LV2_LOG__log;
  feature[nth]->data = log_feature;

  nth++;
  
  /* event feature */
  event_feature = (LV2_Event_Feature *) malloc(sizeof(LV2_Event_Feature));
  
  event_feature->callback_data = NULL;
  event_feature->lv2_event_ref = ags_lv2_event_manager_lv2_event_ref;
  event_feature->lv2_event_unref = ags_lv2_event_manager_lv2_event_unref;
  
  feature[nth] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
  feature[nth]->URI = LV2_EVENT_URI;
  feature[nth]->data = event_feature;

  nth++;
    
  /* URID map feature */
  urid_map = (LV2_URID_Map *) malloc(sizeof(LV2_URID_Map));
  urid_map->handle = NULL;
  urid_map->map = ags_lv2_urid_manager_map;
  
  feature[nth] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
  feature[nth]->URI = LV2_URID_MAP_URI;
  feature[nth]->data = urid_map;

  nth++;
  
  /* URID unmap feature */
  urid_unmap = (LV2_URID_Unmap *) malloc(sizeof(LV2_URID_Unmap));
  urid_unmap->handle = NULL;
  urid_unmap->unmap = ags_lv2_urid_manager_unmap;
  
  feature[nth] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
  feature[nth]->URI = LV2_URID_UNMAP_URI;
  feature[nth]->data = urid_unmap;

  nth++;

  /* Options interface */
  options_interface = (LV2_Options_Interface *) malloc(sizeof(LV2_Options_Interface));
  options_interface->set = ags_lv2_option_manager_lv2_options_set;
  options_interface->get = ags_lv2_option_manager_lv2_options_get;
  
  feature[nth] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
  feature[nth]->URI = LV2_OPTIONS_URI;
  feature[nth]->data = options_interface;

  nth++;

  /* terminate */
  for(; nth < total_feature; nth++){
    feature[nth] = NULL;
  }
  
  /* set up buffer */  
  play_lv2_audio_run->input = (float *) malloc(play_lv2_audio->input_lines *
					       buffer_size *
					       sizeof(float));
  play_lv2_audio_run->output = (float *) malloc(play_lv2_audio->output_lines *
						buffer_size *
						sizeof(float));

  play_lv2_audio_run->lv2_handle = (LV2_Handle *) malloc(sizeof(LV2_Handle));

  
  /* instantiate lv2 */
  play_lv2_audio_run->lv2_handle[0] = (LV2_Handle) play_lv2_audio->plugin_descriptor->instantiate(play_lv2_audio->plugin_descriptor,
												  (float) samplerate,
												  path,
												  feature);
  
  /* some options */
  options = (LV2_Options_Option *) malloc(6 * sizeof(LV2_Options_Option));

  /* samplerate */
  options[0].context = LV2_OPTIONS_INSTANCE;
  options[0].subject = 0;
  options[0].key = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
					       LV2_PARAMETERS__sampleRate);

  ptr_samplerate = (float *) malloc(sizeof(float));
  ptr_samplerate[0] = samplerate;
  
  options[0].size = sizeof(float);
  options[0].type = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
						LV2_ATOM__Float);
  options[0].value = ptr_samplerate;
  
  /* min-block-length */
  options[1].context = LV2_OPTIONS_INSTANCE;
  options[1].subject = 0;
  options[1].key = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
					       LV2_BUF_SIZE__minBlockLength);

  ptr_buffer_size = (float *) malloc(sizeof(float));
  ptr_buffer_size[0] = buffer_size;
  
  options[1].size = sizeof(float);
  options[1].type = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
						LV2_ATOM__Int);
  options[1].value = ptr_buffer_size;

  /* max-block-length */
  options[2].context = LV2_OPTIONS_INSTANCE;
  options[2].subject = 0;
  options[2].key = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
					       LV2_BUF_SIZE__maxBlockLength);

  ptr_buffer_size = (float *) malloc(sizeof(float));
  ptr_buffer_size[0] = buffer_size;

  options[2].size = sizeof(float);
  options[2].type = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
						LV2_ATOM__Int);
  options[2].value = ptr_buffer_size;

  /* terminate */
  options[3].context = LV2_OPTIONS_INSTANCE;
  options[3].subject = 0;
  options[3].key = 0;

  options[3].size = 0;
  options[3].type = 0;
  options[3].value = NULL;

  /* set options */
  ags_lv2_option_manager_lv2_options_set(play_lv2_audio_run->lv2_handle[0],
					 options);

  /*  */  
  if(worker_handle != NULL){
    AGS_LV2_WORKER(worker_handle)->handle = play_lv2_audio_run->lv2_handle[0];
  }
  
  free(path);
  
#ifdef AGS_DEBUG
  g_message("instantiate LV2 handle\0");
#endif

  ags_play_lv2_audio_run_load_ports(play_lv2_audio_run);

  /* can't be done in ags_play_lv2_audio_run_run_init_inter since possebility of overlapping buffers */
  /* connect audio port */
  for(i = 0; i < play_lv2_audio->input_lines; i++){
#ifdef AGS_DEBUG
    g_message("connect port: %d\0", play_lv2_audio->input_port[i]);
#endif
    
    play_lv2_audio->plugin_descriptor->connect_port(play_lv2_audio_run->lv2_handle[0],
						    play_lv2_audio->input_port[i],
						    play_lv2_audio_run->input);
  }

  for(i = 0; i < play_lv2_audio->output_lines; i++){
#ifdef AGS_DEBUG
    g_message("connect port: %d\0", play_lv2_audio->output_port[i]);
#endif
    
    play_lv2_audio->plugin_descriptor->connect_port(play_lv2_audio_run->lv2_handle[0],
						    play_lv2_audio->output_port[i],
						    play_lv2_audio_run->output);
  }

  /* connect event port */
  if((AGS_PLAY_LV2_AUDIO_HAS_EVENT_PORT & (play_lv2_audio->flags)) != 0){
    play_lv2_audio_run->event_port = ags_lv2_plugin_alloc_event_buffer(AGS_PLAY_LV2_AUDIO_DEFAULT_MIDI_LENGHT);
    
    play_lv2_audio->plugin_descriptor->connect_port(play_lv2_audio_run->lv2_handle[0],
						    play_lv2_audio->event_port,
						    play_lv2_audio_run->event_port);
  }
  
  /* connect atom port */
  if((AGS_PLAY_LV2_AUDIO_HAS_ATOM_PORT & (play_lv2_audio->flags)) != 0){
    play_lv2_audio_run->atom_port = ags_lv2_plugin_alloc_atom_sequence(AGS_PLAY_LV2_AUDIO_DEFAULT_MIDI_LENGHT);
    
    play_lv2_audio->plugin_descriptor->connect_port(play_lv2_audio_run->lv2_handle[0],
						    play_lv2_audio->atom_port,
						    play_lv2_audio_run->atom_port);   
  }

  /* activate */
  if(play_lv2_audio->plugin_descriptor->activate != NULL){
    play_lv2_audio->plugin_descriptor->activate(play_lv2_audio_run->lv2_handle[0]);
  }

  /* call parent */
  AGS_RECALL_CLASS(ags_play_lv2_audio_run_parent_class)->run_init_pre(recall);
}

void
ags_play_lv2_audio_run_run_pre(AgsRecall *recall)
{
  GObject *soundcard;
  
  AgsPlayLv2Audio *play_lv2_audio;
  AgsPlayLv2AudioRun *play_lv2_audio_run;

  guint buffer_size;
  uint32_t i;
  
  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(recall);
  play_lv2_audio = AGS_PLAY_LV2_AUDIO(AGS_RECALL_AUDIO_RUN(play_lv2_audio_run)->recall_audio);

  soundcard = recall->soundcard;

  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    NULL,
			    NULL,
			    &buffer_size,
			    NULL);

  if(play_lv2_audio_run->destination == NULL){
    AgsAudio *audio;
    AgsChannel *channel;
    AgsChannel *selected_channel;
    AgsRecycling *recycling;
    AgsAudioSignal *destination;
    AgsRecyclingContext *recycling_context;

    AgsMutexManager *mutex_manager;

    gdouble delay;
    guint attack;
    guint length;
    guint audio_channel;
    
    pthread_mutex_t *application_mutex;
    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *channel_mutex;
    pthread_mutex_t *recycling_mutex;
    
    audio = AGS_RECALL_AUDIO(play_lv2_audio)->audio;

    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

    /* lookup channel mutex */
    pthread_mutex_lock(application_mutex);

    channel = (AgsChannel *) AGS_RECYCLING(AGS_RECALL(play_lv2_audio_run)->recall_id->recycling)->channel;
    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) channel);
	
    pthread_mutex_unlock(application_mutex);

    /* lookup channel mutex */
    pthread_mutex_lock(application_mutex);

    channel = (AgsChannel *) AGS_RECYCLING(AGS_RECALL(play_lv2_audio_run)->recall_id->recycling)->channel;
    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) channel);
	
    pthread_mutex_unlock(application_mutex);

    /* get audio channel */
    pthread_mutex_lock(channel_mutex);
  
    audio_channel = channel->audio_channel;

    pthread_mutex_unlock(channel_mutex);
  
    /* get channel */
    if((AGS_AUDIO_NOTATION_DEFAULT & (audio->flags)) != 0){
      selected_channel = ags_channel_nth(audio->input,
					 audio_channel);
    }else{
      selected_channel = ags_channel_nth(audio->output,
					 audio_channel);
    }

    /* recycling */
    recycling = selected_channel->first_recycling;
    recall->flags &= (~AGS_RECALL_PERSISTENT);

    pthread_mutex_lock(application_mutex);

    recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) recycling);
	
    pthread_mutex_unlock(application_mutex);

    /* recycling context */
    recycling_context = recall->recall_id->recycling_context;

    //TODO:JK: unclear
    attack = 0;
    delay = 0.0;

    /* create new audio signal */
    play_lv2_audio_run->destination = 
      destination = ags_audio_signal_new((GObject *) soundcard,
					 (GObject *) recycling,
					 (GObject *) recall->recall_id);
    //TODO:JK: create property
    g_object_ref(play_lv2_audio_run->destination);
  
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
    g_message("play %x to %x\0", destination, recall_id);
    g_message("creating destination\0");
#endif  
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_play_lv2_audio_run_parent_class)->run_pre(recall);
}

void
ags_play_lv2_audio_run_alloc_input_callback(AgsDelayAudioRun *delay_audio_run,
					    guint nth_run,
					    gdouble delay, guint attack,
					    AgsPlayLv2AudioRun *play_lv2_audio_run)
{
  GObject *soundcard;
  AgsAudio *audio;
  AgsChannel *channel;
  AgsChannel *selected_channel;
  AgsRecycling *recycling;
  AgsAudioSignal *audio_signal;
  AgsNotation *notation;
  AgsNote *note;

  AgsPlayLv2Audio *play_lv2_audio;
  AgsDelayAudio *delay_audio;
  
  AgsMutexManager *mutex_manager;
  
  snd_seq_event_t *seq_event;

  GList *current_position;
  GList *list;

  gchar *str;

  guint audio_start_mapping;
  guint midi_start_mapping, midi_end_mapping;
  guint notation_counter;
  guint note_x0, note_x1;
  guint note_y;  
  guint input_pads;
  guint selected_key;
  guint audio_channel;
  guint copy_mode_in, copy_mode_out;
  uint32_t buffer_size;
  uint32_t i;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *recycling_mutex;

  if(delay != 0.0){
    //    g_message("d %f\0", delay);
    return;
  }

  play_lv2_audio = AGS_PLAY_LV2_AUDIO(AGS_RECALL_AUDIO_RUN(play_lv2_audio_run)->recall_audio);

  delay_audio = AGS_DELAY_AUDIO(AGS_RECALL_AUDIO_RUN(delay_audio_run)->recall_audio);
  
  audio = AGS_RECALL_AUDIO(play_lv2_audio)->audio;
  soundcard = (GObject *) AGS_RECALL(play_lv2_audio_run)->soundcard;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* read config and audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /*  */
  pthread_mutex_lock(audio_mutex);

  list = audio->notation;//(GList *) g_value_get_pointer(&value);

  pthread_mutex_unlock(audio_mutex);
  
  if(list == NULL){
    return;
  }

  /* lookup channel mutex */
  pthread_mutex_lock(application_mutex);

  channel = (AgsChannel *) AGS_RECYCLING(AGS_RECALL(delay_audio_run)->recall_id->recycling)->channel;
  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel);
	
  pthread_mutex_unlock(application_mutex);

  /* get audio channel */
  pthread_mutex_lock(channel_mutex);
  
  audio_channel = channel->audio_channel;

  pthread_mutex_unlock(channel_mutex);

  /* get channel */
  if((AGS_AUDIO_NOTATION_DEFAULT & (audio->flags)) != 0){
    selected_channel = ags_channel_nth(audio->input,
				       audio_channel);
  }else{
    selected_channel = ags_channel_nth(audio->output,
				       audio_channel);
  }
  
  /* get notation */
  pthread_mutex_lock(audio_mutex);
  
  //TODO:JK: make it advanced
  notation = AGS_NOTATION(g_list_nth(list, audio_channel)->data);//AGS_NOTATION(ags_notation_find_near_timestamp(list, audio_channel,
  //						   timestamp_thread->timestamp)->data);

  current_position = notation->notes; // start_loop
  notation_counter = play_lv2_audio_run->count_beats_audio_run->notation_counter;
  
  input_pads = audio->input_pads;

  audio_start_mapping = audio->audio_start_mapping;

  midi_start_mapping = audio->midi_start_mapping;
  midi_end_mapping = audio->midi_end_mapping;

  pthread_mutex_unlock(audio_mutex);

  /*
   * feed midi
   */
  
  while(current_position != NULL){
    pthread_mutex_lock(audio_mutex);
      
    note = AGS_NOTE(current_position->data);
    note_x0 = note->x[0];
    note_x1 = note->x[1];
    note_y = note->y;
      
    pthread_mutex_unlock(audio_mutex);
  
    if(note_x0 == notation_counter){
      /* send key-on */
      if((AGS_AUDIO_REVERSE_MAPPING & (audio->flags)) != 0){
	selected_key = input_pads - note->y - 1;
      }else{
	selected_key = note->y;
      }

      /* key on */
      seq_event = (snd_seq_event_t *) malloc(sizeof(snd_seq_event_t));
      memset(seq_event, 0, sizeof(snd_seq_event_t));

      seq_event->type = SND_SEQ_EVENT_NOTEON;

      seq_event->data.note.channel = 0;
      seq_event->data.note.note = 0x7f & (selected_key - audio_start_mapping + midi_start_mapping);
      seq_event->data.note.velocity = 127;

      /* write to port */
      if((AGS_PLAY_LV2_AUDIO_HAS_ATOM_PORT & (play_lv2_audio->flags)) != 0){
	ags_lv2_plugin_atom_sequence_append_midi(play_lv2_audio_run->atom_port,
						 AGS_PLAY_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
						 seq_event,
						 1);
      }else if((AGS_PLAY_LV2_AUDIO_HAS_EVENT_PORT & (play_lv2_audio->flags)) != 0){
	ags_lv2_plugin_event_buffer_append_midi(play_lv2_audio_run->event_port,
						AGS_PLAY_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
						seq_event,
						1);
      }

      free(seq_event);
    }else if(note_x1 == notation_counter){
      /* send key off */
      if((AGS_AUDIO_REVERSE_MAPPING & (audio->flags)) != 0){
	selected_key = input_pads - note->y - 1;
      }else{
	selected_key = note->y;
      }

      /* write to port */
      if((AGS_PLAY_LV2_AUDIO_HAS_ATOM_PORT & (play_lv2_audio->flags)) != 0){
	ags_lv2_plugin_atom_sequence_remove_midi(play_lv2_audio_run->atom_port,
						 AGS_PLAY_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
						 (0x7f & (selected_key - audio_start_mapping + midi_start_mapping)));
      }else if((AGS_PLAY_LV2_AUDIO_HAS_EVENT_PORT & (play_lv2_audio->flags)) != 0){
	ags_lv2_plugin_event_buffer_remove_midi(play_lv2_audio_run->event_port,
						AGS_PLAY_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
						(0x7f & (selected_key - audio_start_mapping + midi_start_mapping)));
      }
    }else if(note_x0 > notation_counter){
      break;
    }
    
    pthread_mutex_lock(audio_mutex);
    
    current_position = current_position->next;

    pthread_mutex_unlock(audio_mutex);
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
  
  audio_signal = play_lv2_audio_run->destination;
  buffer_size = audio_signal->buffer_size;

  ags_audio_buffer_util_clear_buffer(audio_signal->stream_current->data, 1,
				     buffer_size, ags_audio_buffer_util_format_from_soundcard(audio_signal->format));

  /* get copy mode and clear buffer */
  copy_mode_in = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT,
						     ags_audio_buffer_util_format_from_soundcard(audio_signal->format));

  copy_mode_out = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(audio_signal->format),
						      AGS_AUDIO_BUFFER_UTIL_FLOAT);
  
  if(play_lv2_audio_run->output != NULL){
    ags_audio_buffer_util_clear_float(play_lv2_audio_run->output, play_lv2_audio->output_lines,
				      buffer_size);
  }

  if(play_lv2_audio_run->input != NULL){
    ags_audio_buffer_util_clear_float(play_lv2_audio_run->input, play_lv2_audio->input_lines,
				      buffer_size);
  }

  /* copy data  */
  if(play_lv2_audio_run->input != NULL){
    ags_audio_buffer_util_copy_buffer_to_buffer(play_lv2_audio_run->input, (guint) play_lv2_audio->input_lines, 0,
						audio_signal->stream_current->data, 1, 0,
						(guint) buffer_size, copy_mode_in);
  }
  
  /* process data */
  play_lv2_audio->plugin_descriptor->run(play_lv2_audio_run->lv2_handle[0],
					 (uint32_t) buffer_size);

  /* copy data */
  if(play_lv2_audio_run->output != NULL){
    ags_audio_buffer_util_clear_buffer(audio_signal->stream_current->data, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(audio_signal->format));
    
    ags_audio_buffer_util_copy_buffer_to_buffer(audio_signal->stream_current->data, 1, 0,
						play_lv2_audio_run->output, (guint) play_lv2_audio->output_lines, 0,
						(guint) buffer_size, copy_mode_out);
  }

  pthread_mutex_unlock(recycling_mutex);
}

void
ags_play_lv2_audio_run_write_resolve_dependency(AgsFileLookup *file_lookup,
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
ags_play_lv2_audio_run_read_resolve_dependency(AgsFileLookup *file_lookup,
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
 * ags_play_lv2_audio_run_load_ports:
 * @play_lv2_audio_run: an #AgsRecallLv2Run
 *
 * Set up LV2 ports.
 *
 * Since: 0.7.134
 */
void
ags_play_lv2_audio_run_load_ports(AgsPlayLv2AudioRun *play_lv2_audio_run)
{
  AgsPlayLv2Audio *play_lv2_audio;
  AgsPort *current;
  
  AgsLv2Plugin *lv2_plugin;
  
  GList *port;
  GList *port_descriptor;

  gchar *specifier;
  
  uint32_t i, j;

  play_lv2_audio = AGS_PLAY_LV2_AUDIO(AGS_RECALL_AUDIO_RUN(play_lv2_audio_run)->recall_audio);

  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       play_lv2_audio->filename, play_lv2_audio->effect);

  if(AGS_BASE_PLUGIN(lv2_plugin)->port != NULL){
    port_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->port;

    for(i = 0; port_descriptor != NULL; i++){
      if((AGS_PORT_DESCRIPTOR_CONTROL & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	port = AGS_RECALL(play_lv2_audio)->port;
	current = NULL;
	  
	specifier = AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name;
	    
	while(port != NULL){
	  if(!g_strcmp0(specifier,
			AGS_PORT(port->data)->specifier)){
	    current = port->data;
	    break;
	  }

	  port = port->next;
	}

#ifdef AGS_DEBUG
	g_message("connect port: %d\0", AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_index);
#endif
	play_lv2_audio->plugin_descriptor->connect_port(play_lv2_audio_run->lv2_handle[0],
							(uint32_t) AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_index,
							(float *) &(current->port_value.ags_port_float));
      }

      port_descriptor = port_descriptor->next;
    }
  }
}

/**
 * ags_play_lv2_audio_run_new:
 *
 * Creates an #AgsPlayLv2AudioRun
 *
 * Returns: a new #AgsPlayLv2AudioRun
 *
 * Since: 0.7.134
 */
AgsPlayLv2AudioRun*
ags_play_lv2_audio_run_new()
{
  AgsPlayLv2AudioRun *play_lv2_audio_run;

  play_lv2_audio_run = (AgsPlayLv2AudioRun *) g_object_new(AGS_TYPE_PLAY_LV2_AUDIO_RUN,
							   NULL);

  return(play_lv2_audio_run);
}
