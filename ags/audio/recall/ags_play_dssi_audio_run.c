/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/plugin/ags_dssi_manager.h>
#include <ags/plugin/ags_ladspa_conversion.h>
#include <ags/plugin/ags_dssi_plugin.h>
#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_sound_enums.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/recall/ags_play_dssi_audio.h>
#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ladspa.h>

#include <ags/i18n.h>

void ags_play_dssi_audio_run_class_init(AgsPlayDssiAudioRunClass *play_dssi_audio_run);
void ags_play_dssi_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
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
void ags_play_dssi_audio_run_connect_connection(AgsConnectable *connectable,
						GObject *connection);
void ags_play_dssi_audio_run_disconnect_connection(AgsConnectable *connectable,
						   GObject *connection);

void ags_play_dssi_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_play_dssi_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_play_dssi_audio_run_run_init_pre(AgsRecall *recall);
void ags_play_dssi_audio_run_run_pre(AgsRecall *recall);

void ags_play_dssi_audio_run_resolve_dependency(AgsRecall *recall);

void ags_play_dssi_audio_run_alloc_input_callback(AgsDelayAudioRun *delay_audio_run,
						  guint nth_run,
						  gdouble delay, guint attack,
						  AgsPlayDssiAudioRun *play_dssi_audio_run);

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
  PROP_DESTINATION,
  PROP_NOTATION,
};

static gpointer ags_play_dssi_audio_run_parent_class = NULL;
static AgsConnectableInterface* ags_play_dssi_audio_run_parent_connectable_interface;

GType
ags_play_dssi_audio_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_play_dssi_audio_run;

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

    ags_type_play_dssi_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
							  "AgsPlayDssiAudioRun",
							  &ags_play_dssi_audio_run_info,
							  0);

    g_type_add_interface_static(ags_type_play_dssi_audio_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave (&g_define_type_id__volatile, ags_type_play_dssi_audio_run);
  }

  return g_define_type_id__volatile;
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
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("delay-audio-run",
				   i18n_pspec("assigned AgsDelayAudioRun"),
				   i18n_pspec("the AgsDelayAudioRun which emits ::notation-alloc-input() signal"),
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
   * Since: 3.0.0
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
   * AgsPlayDssiAudioRun:destination:
   *
   * The destination's audio signal.
   * 
   * Since: 3.0.0
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
   * AgsPlayDssiAudioRun:notation:
   *
   * The notation containing the notes.
   * 
   * Since: 3.0.0
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

  recall->resolve_dependency = ags_play_dssi_audio_run_resolve_dependency;
  recall->run_init_pre = ags_play_dssi_audio_run_run_init_pre;
  recall->run_pre = ags_play_dssi_audio_run_run_pre;
}

void
ags_play_dssi_audio_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_play_dssi_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_play_dssi_audio_run_connect;
  connectable->disconnect = ags_play_dssi_audio_run_disconnect;

  connectable->connect_connection = ags_play_dssi_audio_run_connect_connection;
  connectable->disconnect_connection = ags_play_dssi_audio_run_disconnect_connection;
}

void
ags_play_dssi_audio_run_init(AgsPlayDssiAudioRun *play_dssi_audio_run)
{
  guint i;
  
  ags_recall_set_ability_flags((AgsRecall *) play_dssi_audio_run, (AGS_SOUND_ABILITY_SEQUENCER |
								   AGS_SOUND_ABILITY_NOTATION |
								   AGS_SOUND_ABILITY_WAVE |
								   AGS_SOUND_ABILITY_MIDI));

  AGS_RECALL(play_dssi_audio_run)->name = "ags-play-dssi";
  AGS_RECALL(play_dssi_audio_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_dssi_audio_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_dssi_audio_run)->xml_type = "ags-play-dssi-audio-run";
  AGS_RECALL(play_dssi_audio_run)->port = NULL;

  play_dssi_audio_run->audio_channels = 0;
  
  play_dssi_audio_run->ladspa_handle = NULL;

  play_dssi_audio_run->port_data = NULL;
  
  play_dssi_audio_run->input = NULL;
  play_dssi_audio_run->output = NULL;

  play_dssi_audio_run->delta_time = 0;
  
  play_dssi_audio_run->event_buffer = (snd_seq_event_t **) malloc(AGS_PLAY_DSSI_AUDIO_RUN_DEFAULT_MIDI_LENGHT * sizeof(snd_seq_event_t *));

  for(i = 0; i < AGS_PLAY_DSSI_AUDIO_RUN_DEFAULT_MIDI_LENGHT; i++){
    play_dssi_audio_run->event_buffer[i] = NULL;
  }
  
  play_dssi_audio_run->event_count = (unsigned long *) malloc(AGS_PLAY_DSSI_AUDIO_RUN_DEFAULT_MIDI_LENGHT * sizeof(unsigned long));

  for(i = 0; i < AGS_PLAY_DSSI_AUDIO_RUN_DEFAULT_MIDI_LENGHT; i++){
    play_dssi_audio_run->event_count[i] = 0;
  }

  play_dssi_audio_run->key_on = 0;
  
  play_dssi_audio_run->delay_audio_run = NULL;
  play_dssi_audio_run->count_beats_audio_run = NULL;

  play_dssi_audio_run->destination = NULL;

  play_dssi_audio_run->notation = NULL;

  play_dssi_audio_run->timestamp = ags_timestamp_new();
  g_object_ref(play_dssi_audio_run->timestamp);
  
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

  GRecMutex *recall_mutex;

  play_dssi_audio_run = AGS_PLAY_DSSI_AUDIO_RUN(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(gobject);

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      AgsDelayAudioRun *delay_audio_run, *old_delay_audio_run;

      gboolean is_template;

      delay_audio_run = g_value_get_object(value);
      old_delay_audio_run = NULL;

      g_rec_mutex_lock(recall_mutex);

      if(delay_audio_run == play_dssi_audio_run->delay_audio_run){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }
      
      if(play_dssi_audio_run->delay_audio_run != NULL){
	old_delay_audio_run = play_dssi_audio_run->delay_audio_run;

	g_object_unref(G_OBJECT(play_dssi_audio_run->delay_audio_run));
      }

      if(delay_audio_run != NULL){
	g_object_ref(delay_audio_run);
      }

      g_rec_mutex_unlock(recall_mutex);

      /* check template */
      if(delay_audio_run != NULL &&
	 ags_recall_test_flags((AgsRecall *) play_dssi_audio_run, AGS_RECALL_TEMPLATE)){
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
	  list = ags_recall_dependency_find_dependency(AGS_RECALL(play_dssi_audio_run)->recall_dependency,
						       old_delay_audio_run);

	  if(list != NULL){
	    recall_dependency = list->data;
	  }
	  
	  ags_recall_remove_recall_dependency(AGS_RECALL(play_dssi_audio_run),
					      (GObject *) recall_dependency);
	}
      }else{
	if(ags_connectable_is_connected(AGS_CONNECTABLE(play_dssi_audio_run))){
	  ags_connectable_disconnect_connection(AGS_CONNECTABLE(play_dssi_audio_run),
						(GObject *) old_delay_audio_run);
	}
      }

      /* new - dependency/connection */
      g_rec_mutex_lock(recall_mutex);

      play_dssi_audio_run->delay_audio_run = delay_audio_run;

      g_rec_mutex_unlock(recall_mutex);

      if(delay_audio_run != NULL){
	if(is_template){
	  ags_recall_add_recall_dependency(AGS_RECALL(play_dssi_audio_run),
					   ags_recall_dependency_new((GObject *) delay_audio_run));
	}else{
	  if(ags_connectable_is_connected(AGS_CONNECTABLE(play_dssi_audio_run))){
	    ags_connectable_connect_connection(AGS_CONNECTABLE(play_dssi_audio_run),
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
      
      g_rec_mutex_lock(recall_mutex);

      if(count_beats_audio_run == play_dssi_audio_run->count_beats_audio_run){
	g_rec_mutex_unlock(recall_mutex);
	
	return;
      }

      if((AGS_RECALL_TEMPLATE & (AGS_RECALL(play_dssi_audio_run)->flags)) != 0){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(play_dssi_audio_run->count_beats_audio_run != NULL){
	g_object_unref(G_OBJECT(play_dssi_audio_run->count_beats_audio_run));
      }

      if(count_beats_audio_run != NULL){
	g_object_ref(count_beats_audio_run);
      }

      play_dssi_audio_run->count_beats_audio_run = count_beats_audio_run;

      g_rec_mutex_unlock(recall_mutex);

      /* check template */
      if(count_beats_audio_run != NULL &&
	 ags_recall_test_flags((AgsRecall *) play_dssi_audio_run, AGS_RECALL_TEMPLATE)){
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
	  list = ags_recall_dependency_find_dependency(AGS_RECALL(play_dssi_audio_run)->recall_dependency,
						       old_count_beats_audio_run);

	  if(list != NULL){
	    recall_dependency = list->data;
	  }
	  
	  ags_recall_remove_recall_dependency(AGS_RECALL(play_dssi_audio_run),
					      (GObject *) recall_dependency);
	}
      }

      /* dependency - add */
      if(is_template &&
	 count_beats_audio_run != NULL){
	ags_recall_add_recall_dependency(AGS_RECALL(play_dssi_audio_run),
					 ags_recall_dependency_new((GObject *) count_beats_audio_run));
      }
    }
    break;
  case PROP_DESTINATION:
    {
      AgsAudioSignal *destination;

      destination = (AgsAudioSignal *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(play_dssi_audio_run->destination == (GObject *) destination){
	g_rec_mutex_unlock(recall_mutex);
      
	return;
      }

      if(play_dssi_audio_run->destination != NULL){
	g_object_unref(play_dssi_audio_run->destination);
      }

      if(destination != NULL){
	g_object_ref(destination);
      }

      play_dssi_audio_run->destination = (GObject *) destination;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_NOTATION:
    {
      AgsNotation *notation;

      notation = (AgsNotation *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(play_dssi_audio_run->notation == notation){
	g_rec_mutex_unlock(recall_mutex);
      
	return;
      }

      if(play_dssi_audio_run->notation != NULL){
	g_object_unref(play_dssi_audio_run->notation);
      }

      if(notation != NULL){
	g_object_ref(notation);
      }

      play_dssi_audio_run->notation = notation;

      g_rec_mutex_unlock(recall_mutex);
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

  GRecMutex *recall_mutex;
  
  play_dssi_audio_run = AGS_PLAY_DSSI_AUDIO_RUN(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(gobject);

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, G_OBJECT(play_dssi_audio_run->delay_audio_run));

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, G_OBJECT(play_dssi_audio_run->count_beats_audio_run));

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_DESTINATION:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, play_dssi_audio_run->destination);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_NOTATION:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, play_dssi_audio_run->notation);

      g_rec_mutex_unlock(recall_mutex);
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

  g_free(play_dssi_audio_run->port_data);

  g_free(play_dssi_audio_run->input);
  g_free(play_dssi_audio_run->output);

  //FIXME:JK: memory leak
  g_free(play_dssi_audio_run->event_buffer);
  g_free(play_dssi_audio_run->event_count);
  
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
  AgsPlayDssiAudioRun *play_dssi_audio_run;
  AgsDelayAudioRun *delay_audio_run;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  /* call parent */
  ags_play_dssi_audio_run_parent_connectable_interface->connect(connectable);

  play_dssi_audio_run = AGS_PLAY_DSSI_AUDIO_RUN(connectable);
  
  g_object_get(play_dssi_audio_run,
	       "delay-audio-run", &delay_audio_run,
	       NULL);

  ags_connectable_connect_connection(connectable, (GObject *) delay_audio_run);

  if(delay_audio_run != NULL){
    g_object_unref(delay_audio_run);
  }
}

void
ags_play_dssi_audio_run_disconnect(AgsConnectable *connectable)
{
  AgsPlayDssiAudioRun *play_dssi_audio_run;  
  AgsDelayAudioRun *delay_audio_run;
  
  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  play_dssi_audio_run = AGS_PLAY_DSSI_AUDIO_RUN(connectable);
  
  g_object_get(play_dssi_audio_run,
	       "delay-audio-run", &delay_audio_run,
	       NULL);

  ags_connectable_disconnect_connection(connectable, (GObject *) delay_audio_run);

  /* call parent */
  ags_play_dssi_audio_run_parent_connectable_interface->disconnect(connectable);

  if(delay_audio_run != NULL){
    g_object_unref(delay_audio_run);
  }
}

void
ags_play_dssi_audio_run_connect_connection(AgsConnectable *connectable, GObject *connection)
{
  AgsPlayDssiAudioRun *play_dssi_audio_run;  
  AgsDelayAudioRun *delay_audio_run;

  if(connection == NULL){
    return;
  }

  play_dssi_audio_run = AGS_PLAY_DSSI_AUDIO_RUN(connectable);

  g_object_get(play_dssi_audio_run,
	       "delay-audio-run", &delay_audio_run,
	       NULL);

  if(connection == (GObject *) delay_audio_run){
    g_signal_connect(G_OBJECT(delay_audio_run), "notation-alloc-input",
		     G_CALLBACK(ags_play_dssi_audio_run_alloc_input_callback), play_dssi_audio_run);  
  }
  
  if(delay_audio_run != NULL){
    g_object_unref(delay_audio_run);
  }
}

void
ags_play_dssi_audio_run_disconnect_connection(AgsConnectable *connectable, GObject *connection)
{
  AgsPlayDssiAudioRun *play_dssi_audio_run;  
  AgsDelayAudioRun *delay_audio_run;

  if(connection == NULL){
    return;
  }
  
  play_dssi_audio_run = AGS_PLAY_DSSI_AUDIO_RUN(connectable);

  g_object_get(play_dssi_audio_run,
	       "delay-audio-run", &delay_audio_run,
	       NULL);

  if(connection == (GObject *) delay_audio_run){
    g_object_disconnect(G_OBJECT(delay_audio_run),
			"any_signal::notation-alloc-input",
			G_CALLBACK(ags_play_dssi_audio_run_alloc_input_callback),
			play_dssi_audio_run,
			NULL);
  }

  if(delay_audio_run != NULL){
    g_object_unref(delay_audio_run);
  }
}

void
ags_play_dssi_audio_run_resolve_dependency(AgsRecall *recall)
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

  g_object_unref(recall_id);

  g_object_unref(recall_container);
}

void
ags_play_dssi_audio_run_run_init_pre(AgsRecall *recall)
{  
  AgsPlayDssiAudio *play_dssi_audio;
  AgsPlayDssiAudioRun *play_dssi_audio_run;

  AgsDssiPlugin *dssi_plugin;

  GObject *output_soundcard;

  LADSPA_Handle *ladspa_handle;

  LADSPA_Data *output, *input;
  LADSPA_Data *port_data;  

  guint port_count;
  guint output_lines, input_lines;
  guint samplerate;
  guint buffer_size;
  unsigned long i, i_stop;
    
  void (*parent_class_run_init_pre)(AgsRecall *recall);  

  LADSPA_Handle (*instantiate)(const struct _LADSPA_Descriptor * Descriptor,
                               unsigned long SampleRate);
  void (*activate)(LADSPA_Handle Instance);

  GRecMutex *play_dssi_audio_mutex;

  /* get parent class */
  parent_class_run_init_pre = AGS_RECALL_CLASS(ags_play_dssi_audio_run_parent_class)->run_init_pre;

  /* call parent */
  parent_class_run_init_pre(recall);

  /* get some fields */
  play_dssi_audio_run = AGS_PLAY_DSSI_AUDIO_RUN(recall);

  g_object_get(recall,
	       "output-soundcard", &output_soundcard,
	       "recall-audio", &play_dssi_audio,
	       NULL);

  /* get presets */
  ags_soundcard_get_presets(AGS_SOUNDCARD(output_soundcard),
			    NULL,
			    &samplerate,
			    &buffer_size,
			    NULL);

  /* recall mutex */
  play_dssi_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(play_dssi_audio);

  /* get some fields */
  g_rec_mutex_lock(play_dssi_audio_mutex);

  dssi_plugin = play_dssi_audio->plugin;

  output_lines = play_dssi_audio->output_lines;
  input_lines = play_dssi_audio->input_lines;

  port_count = play_dssi_audio->plugin_descriptor->LADSPA_Plugin->PortCount;

  instantiate = play_dssi_audio->plugin_descriptor->LADSPA_Plugin->instantiate;
  activate = play_dssi_audio->plugin_descriptor->LADSPA_Plugin->activate;
  
  g_rec_mutex_unlock(play_dssi_audio_mutex);

  play_dssi_audio_run->port_data = (LADSPA_Data *) malloc(port_count * sizeof(LADSPA_Data));

  output = NULL;
  input = NULL;
    
  if(input_lines > 0){
    input = (LADSPA_Data *) malloc(input_lines *
				   buffer_size *
				   sizeof(LADSPA_Data));
  }

  output = (LADSPA_Data *) malloc(output_lines *
				  buffer_size *
				  sizeof(LADSPA_Data));

  play_dssi_audio_run->output = output;
  play_dssi_audio_run->input = input;
  
  if(input_lines < output_lines){
    i_stop = output_lines;
  }else{
    i_stop = input_lines;
  }
  
  ladspa_handle = NULL;

  if(i_stop > 0){
    ladspa_handle = (LADSPA_Handle *) malloc(i_stop *
					     sizeof(LADSPA_Handle));
  }
    
  play_dssi_audio_run->audio_channels = i_stop;

  
  /* instantiate dssi */
  g_rec_mutex_lock(play_dssi_audio_mutex);

  for(i = 0; i < i_stop; i++){
    /* instantiate dssi */
    ladspa_handle[i] = instantiate(play_dssi_audio->plugin_descriptor->LADSPA_Plugin,
				   (unsigned long) samplerate);

#ifdef AGS_DEBUG
    g_message("instantiate DSSI handle %d %d",
	      play_dssi_audio->bank,
	      play_dssi_audio->program);
#endif
  }

  g_rec_mutex_unlock(play_dssi_audio_mutex);

  port_data = NULL;
  
  if(port_count > 0){    
    port_data = (LADSPA_Data *) malloc(port_count * sizeof(LADSPA_Data));
  }

  /*  */  
  play_dssi_audio_run->ladspa_handle = ladspa_handle;

  play_dssi_audio_run->port_data = port_data;

  /*  */
  ags_play_dssi_audio_run_load_ports(play_dssi_audio_run);
  
  for(i = 0; i < i_stop; i++){
    if(activate != NULL){
      activate(play_dssi_audio_run->ladspa_handle[i]);
    }

#ifdef AGS_DEBUG
    g_message("instantiate DSSI handle");
#endif

  }

  g_object_unref(output_soundcard);

  g_object_unref(play_dssi_audio);
}

void
ags_play_dssi_audio_run_run_pre(AgsRecall *recall)
{  
  AgsPlayDssiAudio *play_dssi_audio;
  AgsPlayDssiAudioRun *play_dssi_audio_run;
  AgsAudio *audio;
  AgsChannel *start_output, *start_input;
  AgsChannel *channel;
  AgsChannel *selected_channel;
  AgsRecycling *recycling;
  AgsRecyclingContext *recycling_context;
  AgsAudioSignal *destination;
  AgsPort *current_port;
  AgsRecallID *recall_id;
  
  AgsDssiPlugin *dssi_plugin;
  
  GObject *output_soundcard;

  GList *list_start, *list;
  GList *port;
  
  snd_seq_event_t *seq_event;
  snd_seq_event_t **event_buffer;
  unsigned long *event_count;

  gchar *specifier, *current_specifier;
  
  LADSPA_Data port_data;

  guint bank, program;
  guint output_lines, input_lines;
  guint port_count;
  guint audio_channel;
  guint samplerate;
  guint buffer_size;
  guint format;
  guint copy_mode_in, copy_mode_out;
  unsigned long i, i_stop;

  void (*parent_class_run_pre)(AgsRecall *recall);  

  void (*select_program)(LADSPA_Handle Instance,
			 unsigned long Bank,
			 unsigned long Program);
  void (*run_synth)(LADSPA_Handle Instance,
		    unsigned long SampleCount,
		    snd_seq_event_t *Events,
		    unsigned long EventCount);
  void (*run)(LADSPA_Handle Instance,
	      unsigned long SampleCount);
  
  GRecMutex *audio_mutex;
  GRecMutex *play_dssi_audio_mutex;
  GRecMutex *port_mutex;
  
  /* get parent class */
  parent_class_run_pre = AGS_RECALL_CLASS(ags_play_dssi_audio_run_parent_class)->run_pre;

  /* call parent */
  parent_class_run_pre(recall);

  /* get some fields */
  play_dssi_audio_run = AGS_PLAY_DSSI_AUDIO_RUN(recall);

  g_object_get(recall,
	       "output-soundcard", &output_soundcard,
	       "audio-channel", &audio_channel,
	       "recall-id", &recall_id,
	       "audio", &audio,
	       "recall-audio", &play_dssi_audio,
	       "destination", &destination,
	       NULL);

  /* get presets */
  ags_soundcard_get_presets(AGS_SOUNDCARD(output_soundcard),
			    NULL,
			    NULL,
			    &buffer_size,
			    NULL);

  /* recall mutex */
  play_dssi_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(play_dssi_audio);

  /* get some fields */
  g_rec_mutex_lock(play_dssi_audio_mutex);

  dssi_plugin = play_dssi_audio->plugin;

  input_lines = play_dssi_audio->input_lines;
  output_lines = play_dssi_audio->output_lines;
  
  g_rec_mutex_unlock(play_dssi_audio_mutex);

  /* audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);
  
  /* get some fields */
  g_rec_mutex_lock(audio_mutex);

  start_output = audio->output;

  if(start_output != NULL){
    g_object_ref(start_output);
  }
  
  start_input = audio->input;

  if(start_input != NULL){
    g_object_ref(start_input);
  }

  g_rec_mutex_unlock(audio_mutex);

  /* get channel */
  selected_channel = ags_channel_nth(start_output,
				     audio_channel);
  
#if 0
  if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT)){
    selected_channel = ags_channel_nth(start_input,
				       audio_channel);
  }else{
    selected_channel = ags_channel_nth(start_output,
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

    g_object_set(play_dssi_audio_run,
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

  g_object_get(destination,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);
  
  /* select program */
  copy_mode_in = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT,
						     ags_audio_buffer_util_format_from_soundcard(format));


  if(play_dssi_audio_run->input != NULL){
    ags_audio_buffer_util_clear_float(play_dssi_audio_run->input, input_lines,
				      buffer_size);
  }

  /* copy data */
  if(play_dssi_audio_run->input != NULL){
    ags_audio_buffer_util_copy_buffer_to_buffer(play_dssi_audio_run->input, (guint) input_lines, 0,
						destination->stream_current->data, 1, 0,
						(guint) buffer_size, copy_mode_in);
  }
  
  if(play_dssi_audio_run->output != NULL){
    ags_audio_buffer_util_clear_float(play_dssi_audio_run->output, output_lines,
				      buffer_size);
  }

  /* select program */
  g_rec_mutex_lock(play_dssi_audio_mutex);

  port_count = play_dssi_audio->plugin_descriptor->LADSPA_Plugin->PortCount;

  select_program = play_dssi_audio->plugin_descriptor->select_program;
  
  g_rec_mutex_unlock(play_dssi_audio_mutex);

  /* retrieve port data */
  g_object_get(play_dssi_audio,
	       "port", &list_start,
	       NULL);

  for(i = 0; i < port_count; i++){
    g_rec_mutex_lock(play_dssi_audio_mutex);

    specifier = g_strdup(play_dssi_audio->plugin_descriptor->LADSPA_Plugin->PortNames[i]);

    g_rec_mutex_unlock(play_dssi_audio_mutex);

    list = list_start;
    
    while(list != NULL){
      gboolean success;
      
      current_port = list->data;

      /* get port mutex */
      port_mutex = AGS_PORT_GET_OBJ_MUTEX(current_port);

      /* check specifier */
      g_rec_mutex_lock(port_mutex);

      current_specifier = g_strdup(current_port->specifier);
      
      g_rec_mutex_unlock(port_mutex);
      
      success = (!g_strcmp0(specifier,
			    current_specifier)) ? TRUE: FALSE;
      g_free(current_specifier);
	
      if(success){
	GValue value = {0,};
	  
	g_value_init(&value,
		     G_TYPE_FLOAT);
	ags_port_safe_read(current_port,
			   &value);
	
	play_dssi_audio_run->port_data[i] = g_value_get_float(&value);

	g_value_unset(&value);
	
	break;
      }

      list = list->next;
    }

    g_free(specifier);
  }

  g_object_get(play_dssi_audio,
	       "bank", &bank,
	       "program", &program,
	       NULL);

  if(select_program != NULL){    
    if(input_lines < output_lines){
      i_stop = output_lines;
    }else{
      i_stop = input_lines;
    }
    
    for(i = 0; i < i_stop; i++){
      select_program(play_dssi_audio_run->ladspa_handle[i],
		     (unsigned long) bank,
		     (unsigned long) program);

      //      g_message("b p %u %u", bank, program);
    }
  }

  /* reset port data */    
  for(i = 0; i < port_count; i++){
    g_rec_mutex_lock(play_dssi_audio_mutex);

    specifier = g_strdup(play_dssi_audio->plugin_descriptor->LADSPA_Plugin->PortNames[i]);

    g_rec_mutex_unlock(play_dssi_audio_mutex);

    list = list_start;
    current_port = NULL;
    
    while(list != NULL){
      gboolean success;
      
      current_port = list->data;

      /* get port mutex */
      port_mutex = AGS_PORT_GET_OBJ_MUTEX(current_port);

      /* check specifier */
      g_rec_mutex_lock(port_mutex);

      current_specifier = g_strdup(current_port->specifier);
      
      g_rec_mutex_unlock(port_mutex);
      
      success = (!g_strcmp0(specifier,
			    current_specifier)) ? TRUE: FALSE;
      g_free(current_specifier);

      if(success){
	break;
      }

      list = list->next;
    }

    g_free(specifier);

    if(list != NULL){
      GValue value = {0,};
      
      g_value_init(&value,
		   G_TYPE_FLOAT);
      port_data = play_dssi_audio_run->port_data[i];

      g_value_set_float(&value,
			port_data);
      ags_port_safe_write(current_port,
			  &value);

      g_value_unset(&value);
    }
  }

  g_list_free_full(list_start,
		   g_object_unref);

  /* process data */
  g_rec_mutex_lock(play_dssi_audio_mutex);

  run_synth = play_dssi_audio->plugin_descriptor->run_synth;
  run = play_dssi_audio->plugin_descriptor->LADSPA_Plugin->run;
  
  g_rec_mutex_unlock(play_dssi_audio_mutex);

  if(play_dssi_audio_run->event_buffer != NULL &&
     play_dssi_audio_run->key_on != 0){
    event_buffer = play_dssi_audio_run->event_buffer;
    event_count = play_dssi_audio_run->event_count;
    
    while(event_buffer[0] != NULL){
      seq_event = event_buffer[0];

      if(run_synth != NULL){
	if(event_buffer[0]->type == SND_SEQ_EVENT_NOTEON){
	  run_synth(play_dssi_audio_run->ladspa_handle[0],
		    (unsigned long) (output_lines * buffer_size),
		    event_buffer[0],
		    event_count[0]);
	}
      }else if(run != NULL){
	run(play_dssi_audio_run->ladspa_handle[0],
	    (unsigned long) buffer_size);
      }
      
      event_buffer++;
      event_count++;
    }
  }

  /* create audio data */
  ags_audio_buffer_util_clear_buffer(destination->stream_current->data, 1,
				     buffer_size, ags_audio_buffer_util_format_from_soundcard(format));

  /* get copy mode and clear buffer */
  copy_mode_out = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						      AGS_AUDIO_BUFFER_UTIL_FLOAT);

  /* copy data */
  if(play_dssi_audio_run->output != NULL){
    ags_audio_buffer_util_copy_buffer_to_buffer(destination->stream_current->data, 1, 0,
						play_dssi_audio_run->output, (guint) output_lines, 0,
						(guint) buffer_size, copy_mode_out);
  }

  /* unref */
  g_object_unref(output_soundcard);

  g_object_unref(recall_id);

  g_object_unref(audio);

  g_object_unref(play_dssi_audio);

  g_object_unref(destination);

  if(start_output != NULL){
    g_object_unref(start_output);
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }

  if(selected_channel != NULL){
    g_object_unref(selected_channel);
  }

  if(recycling != NULL){
    g_object_unref(recycling);
  }
}

void
ags_play_dssi_audio_run_alloc_input_callback(AgsDelayAudioRun *delay_audio_run,
					     guint nth_run,
					     gdouble delay, guint attack,
					     AgsPlayDssiAudioRun *play_dssi_audio_run)
{
  AgsAudio *audio;
  AgsNotation *notation;
  AgsNote *note;
  AgsPlayDssiAudio *play_dssi_audio;
  AgsDelayAudio *delay_audio;
  
  GObject *output_soundcard;

  snd_seq_event_t *seq_event;
  snd_seq_event_t **event_buffer;
  unsigned long *event_count;

  GList *start_list, *list;
  GList *start_current_position, *current_position;
  GList *start_feed_note, *feed_note;
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
  
  GRecMutex *audio_mutex;

  if(delay != 0.0){
    return;
  }

  g_object_get(play_dssi_audio_run,
	       "audio", &audio,
	       "audio-channel", &audio_channel,
	       "output-soundcard", &output_soundcard,
	       "recall-audio", &play_dssi_audio,
	       NULL);

  g_object_get(delay_audio_run,
	       "recall-audio", &delay_audio,
	       NULL);

  /* audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get audio fields */
  g_rec_mutex_lock(audio_mutex);

  start_list = g_list_copy_deep(audio->notation,
				(GCopyFunc) g_object_ref,
				NULL);

  g_rec_mutex_unlock(audio_mutex);
  
  if(start_list == NULL){
    g_object_unref(audio);
    
    g_object_unref(output_soundcard);

    g_object_unref(play_dssi_audio);

    g_object_unref(delay_audio);

    return;
  }
  
  /* get notation */
  notation = NULL;
  current_position = NULL;
  
  start_feed_note = NULL;
  start_append_note = NULL;
  start_remove_note = NULL;

  /* get some fields */
  g_rec_mutex_lock(audio_mutex);
  
  notation_counter = play_dssi_audio_run->count_beats_audio_run->notation_counter;

  input_pads = audio->input_pads;

  audio_start_mapping = audio->audio_start_mapping;

  midi_start_mapping = audio->midi_start_mapping;
  midi_end_mapping = audio->midi_end_mapping;
  
  g_rec_mutex_unlock(audio_mutex);

  /*  */
  play_dssi_audio_run->timestamp->timer.ags_offset.offset = AGS_NOTATION_DEFAULT_OFFSET * floor(notation_counter / AGS_NOTATION_DEFAULT_OFFSET);

  list = ags_notation_find_near_timestamp(start_list, audio_channel,
					  play_dssi_audio_run->timestamp);

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
    gboolean do_feed;
    gboolean success;
      
    note = AGS_NOTE(current_position->data);

    do_feed = ags_note_test_flags(note,
				  AGS_NOTE_FEED);
    g_object_get(note,
		 "x0", &note_x0,
		 "x1", &note_x1,
		 "y", &note_y,
		 NULL);

    if(note_x0 == notation_counter){
      start_append_note = g_list_prepend(start_append_note,
					 note);
      g_object_ref(note);
    }else if((do_feed &&
	      (note_x1 == notation_counter ||
	       note_x1 + 1 == notation_counter)) ||
	     (!do_feed &&
	      note_x0 <= notation_counter &&
	      note_x1 > notation_counter)){
      start_feed_note = g_list_prepend(start_feed_note,
				       note);
      g_object_ref(note);
    }else if((do_feed &&
	      note_x1 + 1 == notation_counter) ||
	     (!do_feed &&
	      note_x1 == notation_counter)){
      start_remove_note = g_list_prepend(start_remove_note,
					 note);
      g_object_ref(note);
    }else if(note_x0 > notation_counter){
      break;
    }
    
    /* iterate */
    current_position = current_position->next;
  }

  feed_note =
    start_feed_note = g_list_reverse(start_feed_note);

  append_note =
    start_append_note = g_list_reverse(start_append_note);
  
  remove_note =
    start_remove_note = g_list_reverse(start_remove_note);
  
  /* feed/append */
  if(start_feed_note != NULL){
    if(start_append_note != NULL){
      start_feed_note = g_list_concat(start_feed_note,
				      start_append_note);
    }
  }else{
    start_feed_note = start_append_note;
  }

  feed_note = start_feed_note;
  
  for(i = 0; feed_note != NULL && i < AGS_PLAY_DSSI_AUDIO_RUN_DEFAULT_MIDI_LENGHT; i++){
    note = feed_note->data;

    g_object_get(note,
		 "y", &note_y,
		 NULL);
    
    /* send key-on */
    if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
      selected_key = input_pads - note_y - 1;
    }else{
      selected_key = note_y;
    }

    if(play_dssi_audio_run->event_buffer[i] == NULL){
      play_dssi_audio_run->event_buffer[i] = (snd_seq_event_t *) g_malloc(sizeof(snd_seq_event_t));
    }

    seq_event = play_dssi_audio_run->event_buffer[i];
    
    play_dssi_audio_run->event_count[i] = 1;
    
    /* key on */
    memset(seq_event, 0, sizeof(snd_seq_event_t));
    
    seq_event->type = SND_SEQ_EVENT_NOTEON;

    seq_event->data.note.channel = 0;
    seq_event->data.note.note = 0x7f & (selected_key - audio_start_mapping + midi_start_mapping);
    seq_event->data.note.velocity = 127;

    play_dssi_audio_run->key_on += 1;

    /* iterate */
    feed_note = feed_note->next;
  }

  for(; remove_note != NULL && i < AGS_PLAY_DSSI_AUDIO_RUN_DEFAULT_MIDI_LENGHT; i++){
    g_free(play_dssi_audio_run->event_buffer[i]);
    
    play_dssi_audio_run->event_buffer[i] = NULL;
    play_dssi_audio_run->event_count[i] = 0;
    
    play_dssi_audio_run->key_on -= 1;

    /* iterate */
    remove_note = remove_note->next;
  }
  
  g_list_free_full(start_list,
		   g_object_unref);
  g_list_free_full(start_current_position,
		   g_object_unref);
  
  g_list_free_full(start_feed_note,
		   g_object_unref);
  g_list_free_full(start_remove_note,
		   g_object_unref);

  /* unref */
  g_object_unref(audio);
    
  g_object_unref(output_soundcard);

  g_object_unref(play_dssi_audio);

  g_object_unref(delay_audio);
}  

/**
 * ags_play_dssi_audio_run_load_ports:
 * @play_dssi_audio_run: the #AgsPlayDssiAudioRun
 *
 * Set up DSSI ports.
 *
 * Since: 3.0.0
 */
void
ags_play_dssi_audio_run_load_ports(AgsPlayDssiAudioRun *play_dssi_audio_run)
{
  AgsPlayDssiAudio *play_dssi_audio;
  AgsPort *current_port;

  AgsDssiPlugin *dssi_plugin;

  GList *list_start, *list;

  gchar *plugin_name;
  gchar *specifier;
  gchar *path;
  
  guint output_lines, input_lines;
  unsigned long port_count;
  unsigned long i, j, j_stop;

  DSSI_Descriptor *plugin_descriptor;
  LADSPA_PortDescriptor *port_descriptor;
  LADSPA_PortDescriptor current_port_descriptor;
  
  void (*connect_port)(LADSPA_Handle Instance,
		       unsigned long Port,
		       LADSPA_Data * DataLocation);

  GRecMutex *play_dssi_audio_mutex;
  GRecMutex *base_plugin_mutex;
  
  if(!AGS_IS_PLAY_DSSI_AUDIO_RUN(play_dssi_audio_run)){
    return;
  }

  g_object_get(play_dssi_audio_run,
	       "recall-audio", &play_dssi_audio,
	       NULL);

  /* recall mutex */
  play_dssi_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(play_dssi_audio);

  /* get some fields */
  g_rec_mutex_lock(play_dssi_audio_mutex);

  list_start = g_list_copy(AGS_RECALL(play_dssi_audio)->port);

  dssi_plugin = play_dssi_audio->plugin;

  plugin_descriptor = play_dssi_audio->plugin_descriptor;

  input_lines = play_dssi_audio->input_lines;
  output_lines = play_dssi_audio->output_lines;

  g_rec_mutex_unlock(play_dssi_audio_mutex);
    
  /* base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(dssi_plugin);

  /* get some fields */
  g_rec_mutex_lock(base_plugin_mutex);

  port_count = plugin_descriptor->LADSPA_Plugin->PortCount;

  port_descriptor = plugin_descriptor->LADSPA_Plugin->PortDescriptors;
  connect_port = plugin_descriptor->LADSPA_Plugin->connect_port;

  g_rec_mutex_unlock(base_plugin_mutex);

  if(input_lines < output_lines){
    j_stop = output_lines;
  }else{
    j_stop = input_lines;
  }

  for(i = 0; i < port_count; i++){
    AgsPluginPort *plugin_port;
    
    GRecMutex *port_mutex;

    g_rec_mutex_lock(base_plugin_mutex);

    current_port_descriptor = port_descriptor[i];
    
    g_rec_mutex_unlock(base_plugin_mutex);

    if(LADSPA_IS_PORT_CONTROL(current_port_descriptor)){
      if(LADSPA_IS_PORT_INPUT(current_port_descriptor) ||
	 LADSPA_IS_PORT_OUTPUT(current_port_descriptor)){
	LADSPA_Data *port_pointer;
	
	g_rec_mutex_lock(base_plugin_mutex);

	specifier = g_strdup(plugin_descriptor->LADSPA_Plugin->PortNames[i]);

	g_rec_mutex_unlock(base_plugin_mutex);

	list = ags_port_find_specifier(list_start, specifier);
	g_free(specifier);

	if(list != NULL){
	  current_port = list->data;
	  
	  /* get port mutex */
	  port_mutex = AGS_PORT_GET_OBJ_MUTEX(current_port);

	  /* get port pointer */
	  g_rec_mutex_lock(port_mutex);
	    
	  port_pointer = (LADSPA_Data *) &(current_port->port_value.ags_port_ladspa);

	  g_rec_mutex_unlock(port_mutex);

	  for(j = 0; j < j_stop; j++){
#ifdef AGS_DEBUG
	    g_message("connecting port[%d]: %d/%d - %f", j, i, port_count, current->port_value.ags_port_ladspa);
#endif	  
	    connect_port(play_dssi_audio_run->ladspa_handle[j],
			 (unsigned long) i,
			 port_pointer);
	  }
	}
      }
    }
  }

  g_list_free(list_start);
  
  /* connect audio port */
  for(j = 0; j < input_lines; j++){
    connect_port(play_dssi_audio_run->ladspa_handle[j],
		 (unsigned long) (play_dssi_audio->input_port[j]),
		 &(play_dssi_audio_run->input[j]));
  }
  
  for(j = 0; j < play_dssi_audio->output_lines; j++){
    connect_port(play_dssi_audio_run->ladspa_handle[j],
		 (unsigned long) (play_dssi_audio->output_port[j]),
		 &(play_dssi_audio_run->output[j]));
  }

  g_object_unref(play_dssi_audio);
}

/**
 * ags_play_dssi_audio_run_new:
 * @audio: the #AgsAudio
 * @delay_audio_run: the #AgsDelayAudioRun dependency
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun dependency
 *
 * Create a new instance of #AgsPlayDssiAudioRun
 *
 * Returns: the new #AgsPlayDssiAudioRun
 *
 * Since: 3.0.0
 */
AgsPlayDssiAudioRun*
ags_play_dssi_audio_run_new(AgsAudio *audio,
			    AgsDelayAudioRun *delay_audio_run,
			    AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AgsPlayDssiAudioRun *play_dssi_audio_run;

  play_dssi_audio_run = (AgsPlayDssiAudioRun *) g_object_new(AGS_TYPE_PLAY_DSSI_AUDIO_RUN,
							     "audio", audio,
							     "delay-audio-run", delay_audio_run,
							     "count-beats-audio-run", count_beats_audio_run,
							     NULL);

  return(play_dssi_audio_run);
}
