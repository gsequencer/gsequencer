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

#include <ags/audio/recall/ags_play_lv2_audio_run.h>
#include <ags/audio/recall/ags_play_lv2_audio.h>

#include <ags/libags.h>

#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2_plugin.h>
#include <ags/plugin/ags_plugin_port.h>

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

#include <lv2.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ags/i18n.h>

void ags_play_lv2_audio_run_class_init(AgsPlayLv2AudioRunClass *play_lv2_audio_run);
void ags_play_lv2_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
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
void ags_play_lv2_audio_run_connect_connection(AgsConnectable *connectable,
						GObject *connection);
void ags_play_lv2_audio_run_disconnect_connection(AgsConnectable *connectable,
						   GObject *connection);

void ags_play_lv2_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_play_lv2_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_play_lv2_audio_run_run_init_pre(AgsRecall *recall);
void ags_play_lv2_audio_run_run_pre(AgsRecall *recall);

void ags_play_lv2_audio_run_resolve_dependency(AgsRecall *recall);

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
  PROP_DESTINATION,
  PROP_NOTATION,
};

static gpointer ags_play_lv2_audio_run_parent_class = NULL;
static AgsConnectableInterface* ags_play_lv2_audio_run_parent_connectable_interface;
static AgsPluginInterface *ags_play_lv2_audio_run_parent_plugin_interface;

GType
ags_play_lv2_audio_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_play_lv2_audio_run;

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

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_play_lv2_audio_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_lv2_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
							 "AgsPlayLv2AudioRun",
							 &ags_play_lv2_audio_run_info,
							 0);

    g_type_add_interface_static(ags_type_play_lv2_audio_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_lv2_audio_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);

    g_once_init_leave (&g_define_type_id__volatile, ags_type_play_lv2_audio_run);
  }

  return g_define_type_id__volatile;
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
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("delay-audio-run",
				   i18n_pspec("assigned AgsDelayAudioRun"),
				   i18n_pspec("the AgsDelayAudioRun which emits lv2_alloc_input signal"),
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
   * AgsPlayLv2AudioRun:destination:
   *
   * The destination's audio signal.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("destination",
				   i18n_pspec("destination"),
				   i18n_pspec("The destination audio signal"),
				   AGS_TYPE_AUDIO_SIGNAL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DESTINATION,
				  param_spec);

  /**
   * AgsPlayLv2AudioRun:notation:
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
  recall = (AgsRecallClass *) play_lv2_audio_run;

  recall->resolve_dependency = ags_play_lv2_audio_run_resolve_dependency;

  recall->run_init_pre = ags_play_lv2_audio_run_run_init_pre;
  recall->run_pre = ags_play_lv2_audio_run_run_pre;
}

void
ags_play_lv2_audio_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_play_lv2_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_play_lv2_audio_run_connect;
  connectable->disconnect = ags_play_lv2_audio_run_disconnect;

  connectable->connect_connection = ags_play_lv2_audio_run_connect_connection;
  connectable->disconnect_connection = ags_play_lv2_audio_run_disconnect_connection;
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
  ags_recall_set_ability_flags((AgsRecall *) play_lv2_audio_run, (AGS_SOUND_ABILITY_SEQUENCER |
								  AGS_SOUND_ABILITY_NOTATION |
								  AGS_SOUND_ABILITY_WAVE |
								  AGS_SOUND_ABILITY_MIDI));

  AGS_RECALL(play_lv2_audio_run)->name = "ags-play-lv2";
  AGS_RECALL(play_lv2_audio_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_lv2_audio_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_lv2_audio_run)->xml_type = "ags-play-lv2-audio-run";
  AGS_RECALL(play_lv2_audio_run)->port = NULL;

  play_lv2_audio_run->lv2_handle = NULL;
  
  play_lv2_audio_run->port_data = NULL;

  play_lv2_audio_run->input = NULL;
  play_lv2_audio_run->output = NULL;

  play_lv2_audio_run->event_port = NULL;
  play_lv2_audio_run->atom_port = NULL;
  
  play_lv2_audio_run->delta_time = 0;
  
  play_lv2_audio_run->event_buffer = NULL;
  play_lv2_audio_run->event_count = NULL;

  play_lv2_audio_run->key_on = 0;
  
  play_lv2_audio_run->worker_handle = NULL;

  play_lv2_audio_run->delay_audio_run = NULL;
  play_lv2_audio_run->count_beats_audio_run = NULL;

  play_lv2_audio_run->destination = NULL;  
  
  play_lv2_audio_run->notation = NULL;
  
  play_lv2_audio_run->timestamp = ags_timestamp_new();
  g_object_ref(play_lv2_audio_run->timestamp);
  
  play_lv2_audio_run->timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  play_lv2_audio_run->timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  play_lv2_audio_run->timestamp->timer.ags_offset.offset = 0;
}

void
ags_play_lv2_audio_run_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsPlayLv2AudioRun *play_lv2_audio_run;

  pthread_mutex_t *recall_mutex;

  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(gobject);

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

      if(delay_audio_run == play_lv2_audio_run->delay_audio_run){
	pthread_mutex_unlock(recall_mutex);

	return;
      }
      
      if(play_lv2_audio_run->delay_audio_run != NULL){
	old_delay_audio_run = play_lv2_audio_run->delay_audio_run;

	g_object_unref(G_OBJECT(play_lv2_audio_run->delay_audio_run));
      }

      if(delay_audio_run != NULL){
	g_object_ref(delay_audio_run);
      }

      pthread_mutex_unlock(recall_mutex);

      /* check template */
      if(delay_audio_run != NULL &&
	 ags_recall_test_flags((AgsRecall *) play_lv2_audio_run, AGS_RECALL_TEMPLATE)){
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
	  list = ags_recall_dependency_find_dependency(AGS_RECALL(play_lv2_audio_run)->recall_dependency,
						       old_delay_audio_run);

	  if(list != NULL){
	    recall_dependency = list->data;
	  }
	  
	  ags_recall_remove_recall_dependency(AGS_RECALL(play_lv2_audio_run),
					      recall_dependency);
	}
      }else{
	if(ags_connectable_is_connected(AGS_CONNECTABLE(play_lv2_audio_run))){
	  ags_connectable_disconnect_connection(AGS_CONNECTABLE(play_lv2_audio_run),
						(GObject *) old_delay_audio_run);
	}
      }

      /* new - dependency/connection */
      pthread_mutex_lock(recall_mutex);

      play_lv2_audio_run->delay_audio_run = delay_audio_run;

      pthread_mutex_unlock(recall_mutex);

      if(delay_audio_run != NULL){
	if(is_template){
	  ags_recall_add_recall_dependency(AGS_RECALL(play_lv2_audio_run),
					   ags_recall_dependency_new((GObject *) delay_audio_run));
	}else{
	  if(ags_connectable_is_connected(AGS_CONNECTABLE(play_lv2_audio_run))){
	    ags_connectable_connect_connection(AGS_CONNECTABLE(play_lv2_audio_run),
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

      if(count_beats_audio_run == play_lv2_audio_run->count_beats_audio_run){
	pthread_mutex_unlock(recall_mutex);
	
	return;
      }

      if((AGS_RECALL_TEMPLATE & (AGS_RECALL(play_lv2_audio_run)->flags)) != 0){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(play_lv2_audio_run->count_beats_audio_run != NULL){
	old_count_beats_audio_run = play_lv2_audio_run->count_beats_audio_run;

	g_object_unref(G_OBJECT(play_lv2_audio_run->count_beats_audio_run));
      }

      if(count_beats_audio_run != NULL){
	g_object_ref(count_beats_audio_run);
      }

      play_lv2_audio_run->count_beats_audio_run = count_beats_audio_run;

      pthread_mutex_unlock(recall_mutex);

      /* check template */
      if(count_beats_audio_run != NULL &&
	 ags_recall_test_flags((AgsRecall *) play_lv2_audio_run, AGS_RECALL_TEMPLATE)){
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
	  list = ags_recall_dependency_find_dependency(AGS_RECALL(play_lv2_audio_run)->recall_dependency,
						       old_count_beats_audio_run);

	  if(list != NULL){
	    recall_dependency = list->data;
	  }
	  
	  ags_recall_remove_recall_dependency(AGS_RECALL(play_lv2_audio_run),
					      recall_dependency);
	}
      }

      /* dependency - add */
      if(is_template &&
	 count_beats_audio_run != NULL){
	ags_recall_add_recall_dependency(AGS_RECALL(play_lv2_audio_run),
					 ags_recall_dependency_new((GObject *) count_beats_audio_run));
      }
    }
    break;
  case PROP_DESTINATION:
    {
      AgsAudioSignal *destination;

      destination = (AgsAudioSignal *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(play_lv2_audio_run->destination == destination){
	pthread_mutex_unlock(recall_mutex);
      
	return;
      }

      if(play_lv2_audio_run->destination != NULL){
	g_object_unref(play_lv2_audio_run->destination);
      }

      if(destination != NULL){
	g_object_ref(destination);
      }

      play_lv2_audio_run->destination = destination;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_NOTATION:
    {
      AgsNotation *notation;

      notation = (AgsNotation *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(play_lv2_audio_run->notation == notation){
	pthread_mutex_unlock(recall_mutex);
      
	return;
      }

      if(play_lv2_audio_run->notation != NULL){
	g_object_unref(play_lv2_audio_run->notation);
      }

      if(notation != NULL){
	g_object_ref(notation);
      }

      play_lv2_audio_run->notation = notation;

      pthread_mutex_unlock(recall_mutex);
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
  
  pthread_mutex_t *recall_mutex;

  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, G_OBJECT(play_lv2_audio_run->delay_audio_run));

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, G_OBJECT(play_lv2_audio_run->count_beats_audio_run));

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_DESTINATION:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, play_lv2_audio_run->destination);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_NOTATION:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, play_lv2_audio_run->notation);

      pthread_mutex_unlock(recall_mutex);
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

  g_free(play_lv2_audio_run->port_data);

  g_free(play_lv2_audio_run->input);
  g_free(play_lv2_audio_run->output);

  //FIXME:JK: memory leak
  g_free(play_lv2_audio_run->event_port);
  g_free(play_lv2_audio_run->atom_port);
  
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

  /* timestamp */
  if(play_lv2_audio_run->timestamp != NULL){
    g_object_unref(G_OBJECT(play_lv2_audio_run->timestamp));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_lv2_audio_run_parent_class)->finalize(gobject);
}

void
ags_play_lv2_audio_run_connect(AgsConnectable *connectable)
{
  AgsPlayLv2AudioRun *play_lv2_audio_run;
  AgsDelayAudioRun *delay_audio_run;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(connectable);
  
  g_object_get(play_lv2_audio_run,
	       "delay-audio-run", &delay_audio_run,
	       NULL);

  ags_connectable_connect_connection(connectable, (GObject *) delay_audio_run);
  
  /* call parent */
  ags_play_lv2_audio_run_parent_connectable_interface->connect(connectable);
}

void
ags_play_lv2_audio_run_disconnect(AgsConnectable *connectable)
{
  AgsPlayLv2AudioRun *play_lv2_audio_run;  
  AgsDelayAudioRun *delay_audio_run;
  
  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(connectable);
  
  g_object_get(play_lv2_audio_run,
	       "delay-audio-run", &delay_audio_run,
	       NULL);

  ags_connectable_disconnect_connection(connectable, (GObject *) delay_audio_run);

  /* call parent */
  ags_play_lv2_audio_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_play_lv2_audio_run_connect_connection(AgsConnectable *connectable, GObject *connection)
{
  AgsPlayLv2AudioRun *play_lv2_audio_run;  
  AgsDelayAudioRun *delay_audio_run;

  if(connection == NULL){
    return;
  }

  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(connectable);

  g_object_get(play_lv2_audio_run,
	       "delay-audio-run", &delay_audio_run,
	       NULL);

  if(connection == (GObject *) delay_audio_run){
    g_signal_connect(G_OBJECT(delay_audio_run), "notation-alloc-input",
		     G_CALLBACK(ags_play_lv2_audio_run_alloc_input_callback), play_lv2_audio_run);  
  }
}

void
ags_play_lv2_audio_run_disconnect_connection(AgsConnectable *connectable, GObject *connection)
{
  AgsPlayLv2AudioRun *play_lv2_audio_run;  
  AgsDelayAudioRun *delay_audio_run;

  if(connection == NULL){
    return;
  }
  
  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(connectable);

  g_object_get(play_lv2_audio_run,
	       "delay-audio-run", &delay_audio_run,
	       NULL);

  if(connection == (GObject *) delay_audio_run){
    g_object_disconnect(G_OBJECT(delay_audio_run),
			"any_signal::notation-alloc-input",
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

  /* write dependency */
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
		     G_CALLBACK(ags_play_lv2_audio_run_write_resolve_dependency), G_OBJECT(plugin));

    list = list->next;
  }

  return(node);
}

void
ags_play_lv2_audio_run_resolve_dependency(AgsRecall *recall)
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

  g_list_free(list_start);

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

    list = list->next;
  }

  g_object_set(G_OBJECT(recall),
	       "delay-audio-run", delay_audio_run,
	       "count-beats-audio-run", count_beats_audio_run,
	       NULL);
}

void
ags_play_lv2_audio_run_run_init_pre(AgsRecall *recall)
{
  AgsPlayLv2Audio *play_lv2_audio;
  AgsPlayLv2AudioRun *play_lv2_audio_run;

  AgsLv2Plugin *lv2_plugin;
  
  GObject *output_soundcard;

  LV2_Handle *lv2_handle;
  
  gchar *path;
  float *output, *input;

  guint output_lines, input_lines;
  guint audio_channel;
  guint samplerate;
  guint buffer_size;
  guint port_count;
  uint32_t i;
    
  void (*parent_class_run_init_pre)(AgsRecall *recall);  

  void (*connect_port)(LV2_Handle instance,
		       uint32_t port,
		       void *data_location);
  void (*activate)(LV2_Handle instance);

  pthread_mutex_t *play_lv2_audio_mutex;

  /* get parent class */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  parent_class_run_init_pre = AGS_RECALL_CLASS(ags_play_lv2_audio_run_parent_class)->run_init_pre;

  pthread_mutex_unlock(ags_recall_get_class_mutex());
  
  /* call parent */
  parent_class_run_init_pre(recall);

  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(recall);

  /* get some fields */
  g_object_get(play_lv2_audio_run,
	       "output-soundcard", &output_soundcard,
	       "recall-audio", &play_lv2_audio,
	       NULL);

  /* get presets */
  ags_soundcard_get_presets(AGS_SOUNDCARD(output_soundcard),
			    NULL,
			    &samplerate,
			    &buffer_size,
			    NULL);

  /* recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  play_lv2_audio_mutex = AGS_RECALL(play_lv2_audio)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(play_lv2_audio_mutex);
  
  lv2_plugin = play_lv2_audio->plugin;

  input_lines = play_lv2_audio->input_lines;
  output_lines = play_lv2_audio->output_lines;
  
  connect_port = play_lv2_audio->plugin_descriptor->connect_port;
  activate = play_lv2_audio->plugin_descriptor->activate;

  pthread_mutex_unlock(play_lv2_audio_mutex);

  /* set up buffer */
  input = NULL;
  output = NULL;
  
  if(input_lines > 0){
    input = (float *) malloc(input_lines *
			     buffer_size *
			     sizeof(float));
  }

  output = (float *) malloc(output_lines *
			    buffer_size *
			    sizeof(float));

  play_lv2_audio_run->output = output;
  play_lv2_audio_run->input = input;
  
  /* instantiate lv2 */  
  lv2_handle = (LV2_Handle *) ags_base_plugin_instantiate((AgsBasePlugin *) lv2_plugin,
							  samplerate, buffer_size);

  play_lv2_audio_run->lv2_handle = lv2_handle;
  
#ifdef AGS_DEBUG
  g_message("instantiate LV2 handle");
#endif

  ags_play_lv2_audio_run_load_ports(play_lv2_audio_run);

  /* can't be done in ags_play_lv2_audio_run_run_init_inter since possebility of overlapping buffers */
  pthread_mutex_lock(play_lv2_audio_mutex);

  /* connect audio port */
  for(i = 0; i < input_lines; i++){
#ifdef AGS_DEBUG
    g_message("connect port: %d", play_lv2_audio->input_port[i]);
#endif
    
    connect_port(play_lv2_audio_run->lv2_handle[0],
		 play_lv2_audio->input_port[i],
		 play_lv2_audio_run->input);
  }

  for(i = 0; i < output_lines; i++){
#ifdef AGS_DEBUG
    g_message("connect port: %d", play_lv2_audio->output_port[i]);
#endif
    
    connect_port(play_lv2_audio_run->lv2_handle[0],
		 play_lv2_audio->output_port[i],
		 play_lv2_audio_run->output);
  }

  /* connect event port */
  if(ags_play_lv2_audio_test_flags(play_lv2_audio, AGS_PLAY_LV2_AUDIO_HAS_EVENT_PORT)){
    play_lv2_audio_run->event_port = ags_lv2_plugin_alloc_event_buffer(AGS_PLAY_LV2_AUDIO_DEFAULT_MIDI_LENGHT);
    
    connect_port(play_lv2_audio_run->lv2_handle[0],
		 play_lv2_audio->event_port,
		 play_lv2_audio_run->event_port);
  }
  
  /* connect atom port */
  if(ags_play_lv2_audio_test_flags(play_lv2_audio, AGS_PLAY_LV2_AUDIO_HAS_ATOM_PORT)){
    play_lv2_audio_run->atom_port = ags_lv2_plugin_alloc_atom_sequence(AGS_PLAY_LV2_AUDIO_DEFAULT_MIDI_LENGHT);
    
    connect_port(play_lv2_audio_run->lv2_handle[0],
		 play_lv2_audio->atom_port,
		 play_lv2_audio_run->atom_port);   
  }
  
  /* activate */
  if(activate != NULL){
    activate(play_lv2_audio_run->lv2_handle[0]);
  }

  pthread_mutex_unlock(play_lv2_audio_mutex);

  /* set program */
  if(ags_lv2_plugin_test_flags(lv2_plugin, AGS_LV2_PLUGIN_HAS_PROGRAM_INTERFACE)){
    AgsPort *current_port;

    GList *plugin_port_start, *plugin_port;
    GList *port;
    GList *list;
    
    gchar *specifier, *current_specifier;

    float *port_data;

    guint bank, program;
    guint port_count;

    pthread_mutex_t *base_plugin_mutex;
    
    pthread_mutex_lock(play_lv2_audio_mutex);

    port = g_list_copy(AGS_RECALL(play_lv2_audio)->port);

    bank = play_lv2_audio->bank;
    program = play_lv2_audio->program;
    
    pthread_mutex_unlock(play_lv2_audio_mutex);

    /* get base plugin mutex */
    pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
    base_plugin_mutex = AGS_BASE_PLUGIN(lv2_plugin)->obj_mutex;
  
    pthread_mutex_unlock(ags_base_plugin_get_class_mutex());
    
    /* get plugin port */
    pthread_mutex_lock(base_plugin_mutex);

    plugin_port =
      plugin_port_start = g_list_copy(AGS_BASE_PLUGIN(lv2_plugin)->plugin_port);

    pthread_mutex_unlock(base_plugin_mutex);

    /* create port data */
    port_count = g_list_length(plugin_port_start);
    
    port_data = (float *) malloc(port_count * sizeof(float));

    plugin_port = plugin_port_start;
    
    for(i = 0; i < port_count && plugin_port != NULL; ){
      AgsPluginPort *current_plugin_port;

      pthread_mutex_t *plugin_port_mutex;

      current_plugin_port = AGS_PLUGIN_PORT(plugin_port->data);

      /* get plugin port mutex */
      pthread_mutex_lock(ags_plugin_port_get_class_mutex());
      
      plugin_port_mutex = current_plugin_port->obj_mutex;

      pthread_mutex_unlock(ags_plugin_port_get_class_mutex());

      /* get specifier */
      pthread_mutex_lock(plugin_port_mutex);

      specifier = g_strdup(current_plugin_port->port_name);
 	
      pthread_mutex_unlock(plugin_port_mutex);

      list = ags_port_find_specifier(port, specifier);
      port_data[i] = 0.0;
      
      if(list != NULL){
	GValue value = {0,};
	
	current_port = list->data;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	ags_port_safe_read(current_port,
			   &value);
	port_data[i] = g_value_get_float(&value);

	g_value_unset(&value);
      }

      g_free(specifier);

      /* iterate plugin port */
      plugin_port = plugin_port->next;
    }

    ags_lv2_plugin_change_program(lv2_plugin,
				  play_lv2_audio_run->lv2_handle[0],
				  bank,
				  program);

    /* reset port data */    
    plugin_port = plugin_port_start;

    for(i = 0; i < port_count && plugin_port != NULL;){
      AgsPluginPort *current_plugin_port;

      pthread_mutex_t *plugin_port_mutex;

      current_plugin_port = AGS_PLUGIN_PORT(plugin_port->data);

      /* get plugin port mutex */
      pthread_mutex_lock(ags_plugin_port_get_class_mutex());
      
      plugin_port_mutex = current_plugin_port->obj_mutex;

      pthread_mutex_unlock(ags_plugin_port_get_class_mutex());

      /* get specifier */
      pthread_mutex_lock(plugin_port_mutex);

      specifier = g_strdup(current_plugin_port->port_name);
 	
      pthread_mutex_unlock(plugin_port_mutex);

      list = ags_port_find_specifier(port, specifier);

      if(list != NULL){
	GValue value = {0,};
	
	current_port = list->data;

	g_value_init(&value,
		     G_TYPE_FLOAT);
	g_value_set_float(&value, port_data[i]);
	
	ags_port_safe_write_raw(current_port,
				&value);

	g_value_unset(&value);
      }
      
      /* iterate plugin port */
      plugin_port = plugin_port->next;
    }
    
    g_free(port_data);

    g_list_free(port);
    g_list_free(plugin_port_start);
  }
}

void
ags_play_lv2_audio_run_run_pre(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsChannel *output, *input;
  AgsChannel *channel;
  AgsChannel *selected_channel;
  AgsRecycling *recycling;
  AgsAudioSignal *destination;
  AgsRecallID *recall_id;
  AgsRecyclingContext *recycling_context;    
  AgsPlayLv2Audio *play_lv2_audio;
  AgsPlayLv2AudioRun *play_lv2_audio_run;
  
  AgsLv2Plugin *lv2_plugin;

  GObject *output_soundcard;

  float *output_buffer, *input_buffer;

  guint output_lines, input_lines;
  guint audio_channel;
  guint samplerate;
  guint buffer_size;
  guint format;
  guint copy_mode_in, copy_mode_out;
  uint32_t i;

  void (*parent_class_run_pre)(AgsRecall *recall);    

  void (*run)(LV2_Handle instance,
	      uint32_t sample_count);
  void (*deactivate)(LV2_Handle instance);
  void (*cleanup)(LV2_Handle instance);

  pthread_mutex_t *play_lv2_audio_mutex;

  /* get parent class */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  parent_class_run_pre = AGS_RECALL_CLASS(ags_play_lv2_audio_run_parent_class)->run_pre;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* call parent */
  parent_class_run_pre(recall);

  play_lv2_audio_run = AGS_PLAY_LV2_AUDIO_RUN(recall);

  /* get some fields */
  g_object_get(play_lv2_audio_run,
	       "output-soundcard", &output_soundcard,
	       "audio-channel", &audio_channel,
	       "recall-id", &recall_id,
	       "audio", &audio,
	       "recall-audio", &play_lv2_audio,
	       "destination", &destination,
	       NULL);

  /* get presets */
  ags_soundcard_get_presets(AGS_SOUNDCARD(output_soundcard),
			    NULL,
			    NULL,
			    &buffer_size,
			    NULL);

  /* recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  play_lv2_audio_mutex = AGS_RECALL(play_lv2_audio)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(play_lv2_audio_mutex);

  lv2_plugin = play_lv2_audio->plugin;

  input_lines = play_lv2_audio->input_lines;
  output_lines = play_lv2_audio->output_lines;
  
  pthread_mutex_unlock(play_lv2_audio_mutex);
  
  /* get some fields */
  g_object_get(audio,
	       "output", &output,
	       "input", &input,
	       NULL);
    
  /* get channel */
  selected_channel = ags_channel_nth(output,
				     audio_channel);
  
#if 0
  if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT)){
    selected_channel = ags_channel_nth(input,
				       audio_channel);
  }else{
    selected_channel = ags_channel_nth(output,
				       audio_channel);
  }
#endif
  
  /* recycling */
  g_object_get(selected_channel,
	       "first-recycling", &recycling,
	       NULL);

  ags_recall_unset_behaviour_flags((AgsRecall *) recall, AGS_SOUND_BEHAVIOUR_PERSISTENT);

  if(destination == NULL){
    gdouble delay;
    guint attack;
    guint length;
    
    //TODO:JK: unclear
    attack = 0;
    delay = 0.0;

    /* create new audio signal */
    destination = ags_audio_signal_new((GObject *) output_soundcard,
				       (GObject *) recycling,
				       (GObject *) recall_id);
    
    g_object_set(play_lv2_audio_run,
		 "destination", destination,
		 NULL);
  
    ags_recycling_create_audio_signal_with_defaults(recycling,
						    destination,
						    delay, attack);
    length = 1; // (guint) (2.0 * soundcard->delay[soundcard->tic_counter]) + 1;
    ags_audio_signal_stream_resize(destination,
				   length);

    ags_connectable_connect(AGS_CONNECTABLE(destination));
  
    destination->stream_current = destination->stream;

    ags_recycling_add_audio_signal(recycling,
				   destination);

#ifdef AGS_DEBUG
    g_message("play %x to %x", destination, recall_id);
    g_message("creating destination");
#endif  
  }
  
  /*
   * process data
   */
  /* recycling */
  g_object_get(selected_channel,
	       "first-recycling", &recycling,
	       NULL);

  /* create audio data */
  g_object_get(destination,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);
  
  ags_audio_buffer_util_clear_buffer(destination->stream_current->data, 1,
				     buffer_size, ags_audio_buffer_util_format_from_soundcard(format));

  /* get copy mode and clear buffer */
  copy_mode_in = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT,
						     ags_audio_buffer_util_format_from_soundcard(format));

  copy_mode_out = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						      AGS_AUDIO_BUFFER_UTIL_FLOAT);
  
  if(play_lv2_audio_run->output != NULL){
    ags_audio_buffer_util_clear_float(play_lv2_audio_run->output, output_lines,
				      buffer_size);
  }

  if(play_lv2_audio_run->input != NULL){
    ags_audio_buffer_util_clear_float(play_lv2_audio_run->input, input_lines,
				      buffer_size);
  }

  /* copy data */
  if(play_lv2_audio_run->input != NULL){
    ags_audio_buffer_util_copy_buffer_to_buffer(play_lv2_audio_run->input, (guint) input_lines, 0,
						destination->stream_current->data, 1, 0,
						(guint) buffer_size, copy_mode_in);
  }

  /* process data */
  pthread_mutex_lock(play_lv2_audio_mutex);

  run = play_lv2_audio->plugin_descriptor->run;
    
  pthread_mutex_unlock(play_lv2_audio_mutex);

  if(play_lv2_audio_run->key_on != 0 &&
     play_lv2_audio_run->lv2_handle != NULL){
    run(play_lv2_audio_run->lv2_handle[0],
	(uint32_t) buffer_size);
  }
  
  /* copy data */
  if(play_lv2_audio_run->output != NULL){
    ags_audio_buffer_util_copy_buffer_to_buffer(destination->stream_current->data, 1, 0,
						play_lv2_audio_run->output, (guint) output_lines, 0,
						(guint) buffer_size, copy_mode_out);
  }
}

void
ags_play_lv2_audio_run_alloc_input_callback(AgsDelayAudioRun *delay_audio_run,
					    guint nth_run,
					    gdouble delay, guint attack,
					    AgsPlayLv2AudioRun *play_lv2_audio_run)
{
  AgsAudio *audio;
  AgsChannel *output, *input;
  AgsChannel *channel;
  AgsChannel *selected_channel;
  AgsNotation *notation;
  AgsNote *note;
  AgsPlayLv2Audio *play_lv2_audio;
  AgsDelayAudio *delay_audio;
  
  GObject *output_soundcard;

  snd_seq_event_t *seq_event;
  snd_seq_event_t **event_buffer;
  unsigned long *event_count;

  GList *start_list, *list;
  GList *start_current_position, *current_position;
  GList *start_append_note, *append_note;
  GList *start_remove_note, *remove_note;

  guint audio_start_mapping;
  guint midi_start_mapping, midi_end_mapping;
  guint notation_counter;
  guint note_x0, note_x1;
  guint note_y;  
  guint input_pads;
  guint selected_key;
  guint audio_channel;
  guint i;
  
  pthread_mutex_t *audio_mutex;

  if(delay != 0.0){
    return;
  }

  g_object_get(play_lv2_audio_run,
	       "audio", &audio,
	       "audio-channel", &audio_channel,
	       "output-soundcard", &output_soundcard,
	       "recall-audio", &play_lv2_audio,
	       NULL);

  g_object_get(delay_audio_run,
	       "recall-audio", &delay_audio,
	       NULL);

  /* audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get audio fields */
  pthread_mutex_lock(audio_mutex);

  output = audio->output;
  input = audio->input;

  start_list = g_list_copy(audio->notation);

  pthread_mutex_unlock(audio_mutex);
  
  if(start_list == NULL){
    return;
  }

  /* get channel */
  if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT)){
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

  /* get some fields */
  pthread_mutex_lock(audio_mutex);
  
  notation_counter = play_lv2_audio_run->count_beats_audio_run->notation_counter;

  input_pads = audio->input_pads;

  audio_start_mapping = audio->audio_start_mapping;

  midi_start_mapping = audio->midi_start_mapping;
  midi_end_mapping = audio->midi_end_mapping;
  
  pthread_mutex_unlock(audio_mutex);

  /*  */
  play_lv2_audio_run->timestamp->timer.ags_offset.offset = AGS_NOTATION_DEFAULT_OFFSET * floor(notation_counter / AGS_NOTATION_DEFAULT_OFFSET);

  list = ags_notation_find_near_timestamp(start_list, audio_channel,
					  play_lv2_audio_run->timestamp);
  start_current_position = NULL;
  
  if(list != NULL){
    notation = list->data;

    g_object_get(notation,
		 "note", &start_current_position,
		 NULL);
    
    current_position = start_current_position;
  }
  
  /*
   * feed midi
   */  
  while(current_position != NULL){
    gboolean success;
      
    note = AGS_NOTE(current_position->data);

    g_object_get(note,
		 "x0", &note_x0,
		 "x1", &note_x1,
		 "y", &note_y,
		 NULL);

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

  append_note =
    start_append_note = g_list_reverse(start_append_note);
  
  remove_note =
    start_remove_note = g_list_reverse(start_remove_note);

  /* append */
  while(append_note != NULL){
    gboolean success;

    note = append_note->data;

    g_object_get(note,
		 "y", &note_y,
		 NULL);
  
    /* send key-on */
    if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
      selected_key = input_pads - note_y - 1;
    }else{
      selected_key = note_y;
    }

    /* key on */
    seq_event = (snd_seq_event_t *) malloc(sizeof(snd_seq_event_t));
    memset(seq_event, 0, sizeof(snd_seq_event_t));

    seq_event->type = SND_SEQ_EVENT_NOTEON;

    seq_event->data.note.channel = 0;
    seq_event->data.note.note = 0x7f & (selected_key - audio_start_mapping + midi_start_mapping);
    seq_event->data.note.velocity = 127;

    /* write to port */
    success = FALSE;
    
    if(ags_play_lv2_audio_test_flags(play_lv2_audio, AGS_PLAY_LV2_AUDIO_HAS_ATOM_PORT)){
      success = ags_lv2_plugin_atom_sequence_append_midi(play_lv2_audio_run->atom_port,
							 AGS_PLAY_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
							 seq_event,
							 1);
    }else if(ags_play_lv2_audio_test_flags(play_lv2_audio, AGS_PLAY_LV2_AUDIO_HAS_EVENT_PORT)){
      success = ags_lv2_plugin_event_buffer_append_midi(play_lv2_audio_run->event_port,
							AGS_PLAY_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
							seq_event,
							1);
    }

    if(success){
      play_lv2_audio_run->key_on += 1;
    }
      
    free(seq_event);
    
    /* iterate */
    g_object_unref(append_note->data);

    append_note = append_note->next;
  }

  /* remove */
  while(remove_note != NULL){
    gint match_index;
    gboolean success;
      
    note = remove_note->data;

    g_object_get(note,
		 "y", &note_y,
		 NULL);
  
    /* send key-on */
    if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
      selected_key = input_pads - note_y - 1;
    }else{
      selected_key = note_y;
    }
    
    /* remove key-on */
    if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
      selected_key = input_pads - note_y - 1;
    }else{
      selected_key = note_y;
    }
    
    /* write to port */
    if(ags_play_lv2_audio_test_flags(play_lv2_audio, AGS_PLAY_LV2_AUDIO_HAS_ATOM_PORT)){
      success = ags_lv2_plugin_atom_sequence_remove_midi(play_lv2_audio_run->atom_port,
							 AGS_PLAY_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
							 (0x7f & (selected_key - audio_start_mapping + midi_start_mapping)));
    }else if(ags_play_lv2_audio_test_flags(play_lv2_audio, AGS_PLAY_LV2_AUDIO_HAS_EVENT_PORT)){
      success = ags_lv2_plugin_event_buffer_remove_midi(play_lv2_audio_run->event_port,
							AGS_PLAY_LV2_AUDIO_DEFAULT_MIDI_LENGHT,
							(0x7f & (selected_key - audio_start_mapping + midi_start_mapping)));
    }

    if(success &&
       play_lv2_audio_run->key_on != 0){
      play_lv2_audio_run->key_on -= 1;
    }

    /* iterate */
    g_object_unref(remove_note->data);

    remove_note = remove_note->next;
  }

  g_list_free(start_list);
  g_list_free(start_current_position);
  
  g_list_free(start_append_note);
  g_list_free(start_remove_note);
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
	     "xpath",
  	     g_strdup_printf("xpath=//*[@id='%s']", id));
}

void
ags_play_lv2_audio_run_read_resolve_dependency(AgsFileLookup *file_lookup,
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
 * ags_play_lv2_audio_run_load_ports:
 * @play_lv2_audio_run: the #AgsRecallLv2Run
 *
 * Set up LV2 ports.
 *
 * Since: 2.0.0
 */
void
ags_play_lv2_audio_run_load_ports(AgsPlayLv2AudioRun *play_lv2_audio_run)
{
  AgsPlayLv2Audio *play_lv2_audio;
  AgsPort *current;
  
  AgsLv2Plugin *lv2_plugin;  

  GList *start_port, *port;
  GList *start_list, *list;

  gchar *plugin_name;
  gchar *specifier;
  
  uint32_t port_count;
  uint32_t i, j;
  
  pthread_mutex_t *play_lv2_audio_mutex;

  g_object_get(play_lv2_audio_run,
	       "recall-audio", &play_lv2_audio,
	       NULL);

  /* recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  play_lv2_audio_mutex = AGS_RECALL(play_lv2_audio)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(play_lv2_audio_mutex);

  start_port = g_list_copy(AGS_RECALL(play_lv2_audio)->port);

  lv2_plugin = play_lv2_audio->plugin;

  pthread_mutex_unlock(play_lv2_audio_mutex);

  /* get some fields */
  port_count = g_list_length(start_port);

  g_object_get(lv2_plugin,
	       "plugin-port", &start_list,
	       NULL);

  list = start_list;
  
  for(i = 0; list != NULL; i++){
    guint port_index;

    pthread_mutex_t *plugin_port_mutex;

    g_object_get(list->data,
		 "port-index", &port_index,
		 NULL);
    
    if(ags_plugin_port_test_flags(list->data, AGS_PLUGIN_PORT_CONTROL)){      
      /* get plugin port mutex */
      pthread_mutex_lock(ags_plugin_port_get_class_mutex());
	
      plugin_port_mutex = AGS_PLUGIN_PORT(list->data)->obj_mutex;
	
      pthread_mutex_unlock(ags_plugin_port_get_class_mutex());

      /* get specifier */
      pthread_mutex_lock(plugin_port_mutex);
	
      specifier = g_strdup(AGS_PLUGIN_PORT(list->data)->port_name);

      pthread_mutex_unlock(plugin_port_mutex);

      port = start_port;
      current = NULL;
	    
      while(port != NULL){
	gboolean success;

	pthread_mutex_t *port_mutex;

	pthread_mutex_lock(ags_port_get_class_mutex());
	  
	port_mutex = AGS_PORT(port->data)->obj_mutex;
	  
	pthread_mutex_unlock(ags_port_get_class_mutex());

	/* check success */
	pthread_mutex_lock(port_mutex);
	  
	success = (!g_strcmp0(specifier,
			      AGS_PORT(port->data)->specifier)) ? TRUE: FALSE;

	pthread_mutex_unlock(port_mutex);

	if(success){
	  current = port->data;

	  break;
	}

	/* iterate */
	port = port->next;
      }

#ifdef AGS_DEBUG
      g_message("connect port: %d", AGS_PLUGIN_PORT(list->data)->port_index);
#endif

      if(current != NULL){
	ags_base_plugin_connect_port(AGS_BASE_PLUGIN(lv2_plugin),
				     play_lv2_audio_run->lv2_handle[0],
				     port_index,
				     (float *) &(current->port_value.ags_port_float));
      }
    }

    list = list->next;
  }
}

/**
 * ags_play_lv2_audio_run_new:
 * @audio: the #AgsAudio
 * @delay_audio_run: the #AgsDelayAudioRun dependency
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun dependency
 *
 * Create a new instance of #AgsPlayLv2AudioRun
 *
 * Returns: the new #AgsPlayLv2AudioRun
 *
 * Since: 2.0.0
 */
AgsPlayLv2AudioRun*
ags_play_lv2_audio_run_new(AgsAudio *audio,
			   AgsDelayAudioRun *delay_audio_run,
			   AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AgsPlayLv2AudioRun *play_lv2_audio_run;

  play_lv2_audio_run = (AgsPlayLv2AudioRun *) g_object_new(AGS_TYPE_PLAY_LV2_AUDIO_RUN,
							   "audio", audio,
							   "delay-audio-run", delay_audio_run,
							   "count-beats-audio-run", count_beats_audio_run,
							   NULL);

  return(play_lv2_audio_run);
}
