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

#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_dynamic_connectable.h>
#include <ags/object/ags_countable.h>
#include <ags/object/ags_tactable.h>
#include <ags/object/ags_seekable.h>
#include <ags/object/ags_plugin.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>

#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_stream_channel_run.h>

#include <ags/audio/task/ags_cancel_audio.h>

#include <math.h>

void ags_count_beats_audio_run_class_init(AgsCountBeatsAudioRunClass *count_beats_audio_run);
void ags_count_beats_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_count_beats_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_count_beats_audio_run_seekable_interface_init(AgsSeekableInterface *seekable);
void ags_count_beats_audio_run_countable_interface_init(AgsCountableInterface *countable);
void ags_count_beats_audio_run_tactable_interface_init(AgsTactableInterface *tactable);
void ags_count_beats_audio_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_count_beats_audio_run_init(AgsCountBeatsAudioRun *count_beats_audio_run);
void ags_count_beats_audio_run_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec);
void ags_count_beats_audio_run_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec);
void ags_count_beats_audio_run_dispose(GObject *gobject);
void ags_count_beats_audio_run_finalize(GObject *gobject);
void ags_count_beats_audio_run_connect(AgsConnectable *connectable);
void ags_count_beats_audio_run_disconnect(AgsConnectable *connectable);
void ags_count_beats_audio_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_count_beats_audio_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_count_beats_audio_run_seek(AgsSeekable *seekable,
				    guint steps,
				    gboolean forward);
guint ags_count_beats_audio_run_get_notation_counter(AgsCountable *countable);
guint ags_count_beats_audio_run_get_sequencer_counter(AgsCountable *countable);
void ags_count_beats_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_count_beats_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_count_beats_audio_run_notify_soundcard_callback(GObject *gobject,
							 GParamSpec *pspec,
							 gpointer user_data);

void ags_count_beats_audio_run_resolve_dependencies(AgsRecall *recall);
AgsRecall* ags_count_beats_audio_run_duplicate(AgsRecall *recall,
					       AgsRecallID *recall_id,
					       guint *n_params, GParameter *parameter);
void ags_count_beats_audio_run_notify_dependency(AgsRecall *recall,
						 guint notify_mode,
						 gint count);
void ags_count_beats_audio_run_run_init_pre(AgsRecall *recall);
void ags_count_beats_audio_run_done(AgsRecall *recall);

gdouble ags_count_beats_audio_run_get_bpm(AgsTactable *tactable);
gdouble ags_count_beats_audio_run_get_tact(AgsTactable *tactable);
void ags_count_beats_audio_run_change_bpm(AgsTactable *tactable, gdouble new_bpm, gdouble old_bpm);
void ags_count_beats_audio_run_change_tact(AgsTactable *tactable, gdouble new_tact, gdouble old_tact);

void ags_count_beats_audio_run_notation_alloc_output_callback(AgsDelayAudioRun *delay_audio_run,
							      guint run_order, gdouble delay, guint attack,
							      AgsCountBeatsAudioRun *count_beats_audio_run);
void ags_count_beats_audio_run_notation_count_callback(AgsDelayAudioRun *delay_audio_run,
						       guint run_order, gdouble delay, guint attack,
						       AgsCountBeatsAudioRun *count_beats_audio_run);

void ags_count_beats_audio_run_sequencer_alloc_output_callback(AgsDelayAudioRun *delay_audio_run,
							       guint run_order, gdouble delay, guint attack,
							       AgsCountBeatsAudioRun *count_beats_audio_run);
void ags_count_beats_audio_run_sequencer_count_callback(AgsDelayAudioRun *delay_audio_run,
							guint run_order, gdouble delay, guint attack,
							AgsCountBeatsAudioRun *count_beats_audio_run);

void ags_count_beats_audio_run_write_resolve_dependency(AgsFileLookup *file_lookup,
							GObject *recall);
void ags_count_beats_audio_run_read_resolve_dependency(AgsFileLookup *file_lookup,
						       GObject *recall);

void ags_count_beats_audio_run_stream_audio_signal_done_callback(AgsRecall *recall,
								 AgsCountBeatsAudioRun *count_beats_audio_run);
void ags_count_beats_audio_run_stop(AgsCountBeatsAudioRun *count_beats_audio_run,
				    gboolean notation);

/**
 * SECTION:ags_count_beats_audio_run
 * @short_description: count beats
 * @title: AgsCountBeatsAudioRun
 * @section_id:
 * @include: ags/audio/recall/ags_count_beats_audio_run.h
 *
 * The #AgsCountBeatsAudioRun class count beats.
 */

enum{
  NOTATION_LOOP,
  NOTATION_START,
  NOTATION_STOP,
  SEQUENCER_LOOP,
  SEQUENCER_START,
  SEQUENCER_STOP,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_DELAY_AUDIO_RUN,
  PROP_NOTATION_COUNTER,
  PROP_SEQUENCER_COUNTER,
};

static gpointer ags_count_beats_audio_run_parent_class = NULL;
static AgsConnectableInterface* ags_count_beats_audio_run_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_count_beats_audio_run_parent_dynamic_connectable_interface;
static AgsPluginInterface *ags_count_beats_audio_run_parent_plugin_interface;

static guint count_beats_audio_run_signals[LAST_SIGNAL];

GType
ags_count_beats_audio_run_get_type()
{
  static GType ags_type_count_beats_audio_run = 0;

  if(!ags_type_count_beats_audio_run){
    static const GTypeInfo ags_count_beats_audio_run_info = {
      sizeof (AgsCountBeatsAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_count_beats_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCountBeatsAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_count_beats_audio_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_count_beats_audio_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_count_beats_audio_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_seekable_interface_info = {
      (GInterfaceInitFunc) ags_count_beats_audio_run_seekable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_countable_interface_info = {
      (GInterfaceInitFunc) ags_count_beats_audio_run_countable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_tactable_interface_info = {
      (GInterfaceInitFunc) ags_count_beats_audio_run_tactable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_count_beats_audio_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_count_beats_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
							    "AgsCountBeatsAudioRun\0",
							    &ags_count_beats_audio_run_info,
							    0);
    
    g_type_add_interface_static(ags_type_count_beats_audio_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_count_beats_audio_run,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);

    g_type_add_interface_static(ags_type_count_beats_audio_run,
				AGS_TYPE_COUNTABLE,
				&ags_countable_interface_info);

    g_type_add_interface_static(ags_type_count_beats_audio_run,
				AGS_TYPE_SEEKABLE,
				&ags_seekable_interface_info);

    g_type_add_interface_static(ags_type_count_beats_audio_run,
				AGS_TYPE_TACTABLE,
				&ags_tactable_interface_info);

    g_type_add_interface_static(ags_type_count_beats_audio_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_count_beats_audio_run);
}

void
ags_count_beats_audio_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_count_beats_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_count_beats_audio_run_connect;
  connectable->disconnect = ags_count_beats_audio_run_disconnect;
}

void
ags_count_beats_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_count_beats_audio_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_count_beats_audio_run_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_count_beats_audio_run_disconnect_dynamic;
}

void
ags_count_beats_audio_run_seekable_interface_init(AgsSeekableInterface *seekable)
{
  seekable->seek = ags_count_beats_audio_run_seek;
}

void
ags_count_beats_audio_run_countable_interface_init(AgsCountableInterface *countable)
{
  countable->get_notation_counter = ags_count_beats_audio_run_get_notation_counter;
  countable->get_sequencer_counter = ags_count_beats_audio_run_get_sequencer_counter;
}

void
ags_count_beats_audio_run_tactable_interface_init(AgsTactableInterface *tactable)
{
  tactable->get_bpm = ags_count_beats_audio_run_get_bpm;
  tactable->get_tact = ags_count_beats_audio_run_get_tact;
  tactable->get_sequencer_duration = NULL;
  tactable->get_notation_duration = NULL;

  tactable->change_sequencer_duration = NULL;
  tactable->change_notation_duration = NULL;
  tactable->change_bpm = ags_count_beats_audio_run_change_bpm;
  tactable->change_tact = ags_count_beats_audio_run_change_tact;
}

void
ags_count_beats_audio_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_count_beats_audio_run_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->read = ags_count_beats_audio_run_read;
  plugin->write = ags_count_beats_audio_run_write;
}

void
ags_count_beats_audio_run_class_init(AgsCountBeatsAudioRunClass *count_beats_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_count_beats_audio_run_parent_class = g_type_class_peek_parent(count_beats_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) count_beats_audio_run;

  gobject->set_property = ags_count_beats_audio_run_set_property;
  gobject->get_property = ags_count_beats_audio_run_get_property;

  gobject->dispose = ags_count_beats_audio_run_dispose;
  gobject->finalize = ags_count_beats_audio_run_finalize;

  /* properties */
  /**
   * AgsCountBeatsAudioRun:delay-audio-run:
   *
   * The assigned #AgsDelayAudioRun dependency.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("delay-audio-run\0",
				   "assigned AgsDelayAudioRun\0",
				   "The AgsDelayAudioRun which emits sequencer_alloc_output sequencer_count signal\0",
				   AGS_TYPE_DELAY_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY_AUDIO_RUN,
				  param_spec);
  
  /**
   * AgsCountBeatsAudioRun:notation-counter:
   *
   * The notation counter.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_uint("notation-counter\0",
				 "notation counter indicates offset\0",
				 "The notation counter indicates the offset in the notation\0",
				 0,
				 65535, //FIXME:JK: figure out how many beats this can really have
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION_COUNTER,
				  param_spec);

  /**
   * AgsCountBeatsAudioRun:sequencer-counter:
   *
   * The sequencer counter.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_uint("sequencer-counter\0",
				 "sequencer counter indicates offset\0",
				 "The sequenecer counter indicates the offset in the sequencer\0",
				 0,
				 65535, //FIXME:JK: figure out how many beats this can really have
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEQUENCER_COUNTER,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) count_beats_audio_run;

  recall->resolve_dependencies = ags_count_beats_audio_run_resolve_dependencies;
  recall->duplicate = ags_count_beats_audio_run_duplicate;
  recall->notify_dependency = ags_count_beats_audio_run_notify_dependency;
  recall->run_init_pre = ags_count_beats_audio_run_run_init_pre;
  recall->done = ags_count_beats_audio_run_done;

  /* AgsCountBeatsAudioRunClass */
  count_beats_audio_run->notation_start = NULL;
  count_beats_audio_run->notation_loop = NULL;
  count_beats_audio_run->notation_stop = NULL;
  count_beats_audio_run->sequencer_start = NULL;
  count_beats_audio_run->sequencer_loop = NULL;
  count_beats_audio_run->sequencer_stop = NULL;

  /* signals */
  /**
   * AgsCountBeatsAudioRun::notation-start:
   * @count_beats_audio_run: the object
   * @run_order: the nth run
   *
   * The ::notation-start signal is emited while starting notation playback.
   */
  count_beats_audio_run_signals[NOTATION_START] =
    g_signal_new("notation-start\0",
		 G_TYPE_FROM_CLASS(count_beats_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCountBeatsAudioRunClass, notation_start),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsCountBeatsAudioRun::notation-loop:
   * @count_beats_audio_run: the object
   * @run_order: the nth run
   *
   * The ::notation-loop signal is emited while looping notation playback.
   */
  count_beats_audio_run_signals[NOTATION_LOOP] =
    g_signal_new("notation-loop\0",
		 G_TYPE_FROM_CLASS(count_beats_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCountBeatsAudioRunClass, notation_loop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsCountBeatsAudioRun::notation-stop:
   * @count_beats_audio_run: the object
   * @run_order: the nth run
   *
   * The ::notation-loop signal is emited while stoping notation playback.
   */
  count_beats_audio_run_signals[NOTATION_STOP] =
    g_signal_new("notation-stop\0",
		 G_TYPE_FROM_CLASS(count_beats_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCountBeatsAudioRunClass, notation_stop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsCountBeatsAudioRun::sequencer-start:
   * @count_beats_audio_run: the object
   * @run_order: the nth run
   *
   * The ::sequencer-start signal is emited while starting sequencer playback.
   */
  count_beats_audio_run_signals[SEQUENCER_START] =
    g_signal_new("sequencer-start\0",
		 G_TYPE_FROM_CLASS(count_beats_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCountBeatsAudioRunClass, sequencer_start),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsCountBeatsAudioRun::sequencer-loop:
   * @count_beats_audio_run: the object
   * @run_order: the nth run
   *
   * The ::sequencer-loop signal is emited while looping sequencer playback.
   */
  count_beats_audio_run_signals[SEQUENCER_LOOP] =
    g_signal_new("sequencer-loop\0",
		 G_TYPE_FROM_CLASS(count_beats_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCountBeatsAudioRunClass, sequencer_loop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsCountBeatsAudioRun::sequencer-stop:
   * @count_beats_audio_run: the object
   * @run_order: the nth run
   *
   * The ::sequencer-loop signal is emited while stoping sequencer playback.
   */
  count_beats_audio_run_signals[SEQUENCER_STOP] =
    g_signal_new("sequencer-stop\0",
		 G_TYPE_FROM_CLASS(count_beats_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCountBeatsAudioRunClass, sequencer_stop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);
}

void
ags_count_beats_audio_run_init(AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AGS_RECALL(count_beats_audio_run)->name = "ags-count-beats\0";
  AGS_RECALL(count_beats_audio_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(count_beats_audio_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(count_beats_audio_run)->xml_type = "ags-count-beats-audio-run\0";
  AGS_RECALL(count_beats_audio_run)->port = NULL;

  count_beats_audio_run->notation_counter = 0;
  count_beats_audio_run->sequencer_counter = 0;

  count_beats_audio_run->recall_ref = 0;

  count_beats_audio_run->hide_ref = 0;
  count_beats_audio_run->notation_hide_ref_counter = 0;
  count_beats_audio_run->sequencer_hide_ref_counter = 0;

  count_beats_audio_run->delay_audio_run = NULL;

  g_signal_connect_after(count_beats_audio_run, "notify::soundcard",
			 G_CALLBACK(ags_count_beats_audio_run_notify_soundcard_callback), NULL);
}

void
ags_count_beats_audio_run_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;

  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(gobject);

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      AgsDelayAudioRun *delay_audio_run;
      gboolean is_template;

      delay_audio_run = (AgsDelayAudioRun *) g_value_get_object(value);

      if(count_beats_audio_run->delay_audio_run == delay_audio_run){
	return;
      }

      if(delay_audio_run != NULL &&
	 (AGS_RECALL_TEMPLATE & (AGS_RECALL(delay_audio_run)->flags)) != 0){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(count_beats_audio_run->delay_audio_run != NULL){
	if(is_template){
	  ags_recall_remove_dependency(AGS_RECALL(count_beats_audio_run),
				       (AgsRecall *) count_beats_audio_run->delay_audio_run);
	}else{
	  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(count_beats_audio_run)->flags)) != 0){
	    g_object_disconnect(G_OBJECT(count_beats_audio_run->delay_audio_run),
				"sequencer-alloc-output\0",
				G_CALLBACK(ags_count_beats_audio_run_sequencer_alloc_output_callback),
				count_beats_audio_run,
				"sequencer-count\0",
				G_CALLBACK(ags_count_beats_audio_run_sequencer_count_callback),
				count_beats_audio_run,
				"notation-alloc-output\0",
				G_CALLBACK(ags_count_beats_audio_run_notation_alloc_output_callback),
				count_beats_audio_run,
				"notation-count\0",
				G_CALLBACK(ags_count_beats_audio_run_notation_count_callback),
				count_beats_audio_run,
				NULL);
	  }
	}
	
	g_object_unref(G_OBJECT(count_beats_audio_run->delay_audio_run));
      }

      if(delay_audio_run != NULL){
	g_object_ref(G_OBJECT(delay_audio_run));

	if(is_template){
	  ags_recall_add_dependency(AGS_RECALL(count_beats_audio_run),
				    ags_recall_dependency_new((GObject *) delay_audio_run));
	}else{
	  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(count_beats_audio_run)->flags)) != 0){
	    g_signal_connect(G_OBJECT(delay_audio_run), "sequencer-alloc-output\0",
			     G_CALLBACK(ags_count_beats_audio_run_sequencer_alloc_output_callback), count_beats_audio_run);

	    g_signal_connect(G_OBJECT(delay_audio_run), "sequencer-count\0",
			     G_CALLBACK(ags_count_beats_audio_run_sequencer_count_callback), count_beats_audio_run);

	    g_signal_connect(G_OBJECT(delay_audio_run), "notation-alloc-output\0",
			     G_CALLBACK(ags_count_beats_audio_run_notation_alloc_output_callback), count_beats_audio_run);

	    g_signal_connect(G_OBJECT(delay_audio_run), "notation-count\0",
			     G_CALLBACK(ags_count_beats_audio_run_notation_count_callback), count_beats_audio_run);
	  }
	}
      }

      count_beats_audio_run->delay_audio_run = delay_audio_run;
    }
    break;
  case PROP_NOTATION_COUNTER:
    {
      guint counter;

      counter = g_value_get_uint(value);

      count_beats_audio_run->notation_counter = counter;
    }
    break;
  case PROP_SEQUENCER_COUNTER:
    {
      guint counter;

      counter = g_value_get_uint(value);

      count_beats_audio_run->sequencer_counter = counter;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_count_beats_audio_run_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;
  
  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(gobject);

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      g_value_set_object(value, count_beats_audio_run->delay_audio_run);
    }
    break;
  case PROP_NOTATION_COUNTER:
    {
      g_value_set_uint(value, count_beats_audio_run->notation_counter);
    }
    break;
  case PROP_SEQUENCER_COUNTER:
    {
      g_value_set_uint(value, count_beats_audio_run->sequencer_counter);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_count_beats_audio_run_dispose(GObject *gobject)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;

  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(gobject);

  /* delay audio run */
  if(count_beats_audio_run->delay_audio_run != NULL){
    g_object_unref(count_beats_audio_run->delay_audio_run);

    count_beats_audio_run->delay_audio_run = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_count_beats_audio_run_parent_class)->dispose(gobject);
}

void
ags_count_beats_audio_run_finalize(GObject *gobject)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;

  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(gobject);

  /* delay audio run */
  if(count_beats_audio_run->delay_audio_run != NULL){
    g_object_unref(count_beats_audio_run->delay_audio_run);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_count_beats_audio_run_parent_class)->finalize(gobject);
}

void
ags_count_beats_audio_run_connect(AgsConnectable *connectable)
{  
  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) != 0){
    return;
  }

  /* call parent */
  ags_count_beats_audio_run_parent_connectable_interface->connect(connectable);
}

void
ags_count_beats_audio_run_disconnect(AgsConnectable *connectable)
{
  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) == 0){
    return;
  }

  /* call parent */
  ags_count_beats_audio_run_parent_connectable_interface->connect(connectable);
}

void
ags_count_beats_audio_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;

  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(dynamic_connectable)->flags)) != 0){
    return;
  }

  ags_count_beats_audio_run_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* AgsCountBeats */
  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(dynamic_connectable);

  g_signal_connect(G_OBJECT(count_beats_audio_run->delay_audio_run), "sequencer-alloc-output\0",
		   G_CALLBACK(ags_count_beats_audio_run_sequencer_alloc_output_callback), count_beats_audio_run);

  g_signal_connect(G_OBJECT(count_beats_audio_run->delay_audio_run), "sequencer-count\0",
		   G_CALLBACK(ags_count_beats_audio_run_sequencer_count_callback), count_beats_audio_run);

  g_signal_connect(G_OBJECT(count_beats_audio_run->delay_audio_run), "notation-alloc-output\0",
		   G_CALLBACK(ags_count_beats_audio_run_notation_alloc_output_callback), count_beats_audio_run);

  g_signal_connect(G_OBJECT(count_beats_audio_run->delay_audio_run), "notation-count\0",
		   G_CALLBACK(ags_count_beats_audio_run_notation_count_callback), count_beats_audio_run);
}

void
ags_count_beats_audio_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;

  ags_count_beats_audio_run_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  /* AgsCountBeats */
  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(dynamic_connectable);

  g_object_disconnect(G_OBJECT(count_beats_audio_run->delay_audio_run),
		      "sequencer-alloc-output\0",
		      G_CALLBACK(ags_count_beats_audio_run_sequencer_alloc_output_callback),
		      count_beats_audio_run,
		      "sequencer-count\0",
		      G_CALLBACK(ags_count_beats_audio_run_sequencer_count_callback),
		      count_beats_audio_run,
		      "notation-alloc-output\0",
		      G_CALLBACK(ags_count_beats_audio_run_notation_alloc_output_callback),
		      count_beats_audio_run,
		      "notation-count\0",
		      G_CALLBACK(ags_count_beats_audio_run_notation_count_callback),
		      count_beats_audio_run,
		      NULL);
}

void
ags_count_beats_audio_run_seek(AgsSeekable *seekable,
			       guint steps,
			       gboolean move_forward)
{
  AgsDelayAudio *delay_audio;
  AgsDelayAudioRun *delay_audio_run;
  AgsCountBeatsAudioRun *count_beats_audio_run;

  guint seq_steps;
  
  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(seekable);
  delay_audio_run = count_beats_audio_run->delay_audio_run;
  delay_audio = (AgsDelayAudio *) AGS_RECALL_AUDIO_RUN(delay_audio_run)->recall_audio;
  
  if(delay_audio->sequencer_duration->port_value.ags_port_double != 0.0){
    seq_steps = (steps % (guint) delay_audio->sequencer_duration->port_value.ags_port_double);
  }else{
    seq_steps = (steps % 16);
  }
  
  if(move_forward){
    count_beats_audio_run->notation_counter += steps;

    if(count_beats_audio_run->sequencer_counter + seq_steps < (guint) delay_audio->sequencer_duration->port_value.ags_port_double){
      count_beats_audio_run->sequencer_counter += seq_steps;
    }else{
      count_beats_audio_run->sequencer_counter = (guint) (seq_steps + count_beats_audio_run->sequencer_counter) % (guint) (delay_audio->sequencer_duration->port_value.ags_port_double);
    }
  }else{
    count_beats_audio_run->notation_counter -= steps;

    if(count_beats_audio_run->sequencer_counter - seq_steps >= 0){
      count_beats_audio_run->sequencer_counter -= seq_steps;
    }else{
      count_beats_audio_run->sequencer_counter = (guint) (delay_audio->sequencer_duration->port_value.ags_port_double) - (seq_steps - count_beats_audio_run->sequencer_counter);
    }
  }
}

guint
ags_count_beats_audio_run_get_notation_counter(AgsCountable *countable)
{
  return(AGS_COUNT_BEATS_AUDIO_RUN(countable)->notation_counter);
}

guint
ags_count_beats_audio_run_get_sequencer_counter(AgsCountable *countable)
{
  return(AGS_COUNT_BEATS_AUDIO_RUN(countable)->sequencer_counter);
}

void
ags_count_beats_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;
  AgsFileLookup *file_lookup;
  xmlNode *iter;

  /* read parent */
  ags_count_beats_audio_run_parent_plugin_interface->read(file, node, plugin);

  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(plugin);

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
							   "reference\0", count_beats_audio_run,
							   NULL);
	      ags_file_add_lookup(file, (GObject *) file_lookup);
	      g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
			       G_CALLBACK(ags_count_beats_audio_run_read_resolve_dependency), count_beats_audio_run);
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
ags_count_beats_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsFileLookup *file_lookup;
  xmlNode *node, *child;
  xmlNode *dependency_node;
  GList *list;
  gchar *id;

  /* write parent */
  node = ags_count_beats_audio_run_parent_plugin_interface->write(file, parent, plugin);

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
						 "reference\0", G_OBJECT(plugin),
						 NULL);
    ags_file_add_lookup(file, (GObject *) file_lookup);
    g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		     G_CALLBACK(ags_count_beats_audio_run_write_resolve_dependency), G_OBJECT(plugin));

    list = list->next;
  }

  return(node);
}

void
ags_count_beats_audio_run_notify_soundcard_callback(GObject *gobject,
						    GParamSpec *pspec,
						    gpointer user_data)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;

  GObject *soundcard;
  
  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(gobject);

  soundcard = AGS_RECALL(count_beats_audio_run)->soundcard;

  /* bpm */
  count_beats_audio_run->bpm = ags_soundcard_get_bpm(AGS_SOUNDCARD(soundcard));

  /* tact */
  count_beats_audio_run->tact = AGS_SOUNDCARD_DEFAULT_TACT;
}

void
ags_count_beats_audio_run_resolve_dependencies(AgsRecall *recall)
{
  AgsRecall *template;
  AgsRecallDependency *recall_dependency;
  AgsRecallID *recall_id;

  AgsDelayAudioRun *delay_audio_run;

  GList *list;

  guint i, i_stop;

  if(recall->soundcard == NULL){
    return;
  }

  template = AGS_RECALL(ags_recall_find_template(AGS_RECALL_CONTAINER(recall->container)->recall_audio_run)->data);

  list = template->dependencies;
  recall_id = recall->recall_id;

  delay_audio_run = NULL;

  i_stop = 1;

  for(i = 0; i < i_stop && list != NULL;){
    recall_dependency = AGS_RECALL_DEPENDENCY(list->data);

    if(AGS_IS_DELAY_AUDIO_RUN(recall_dependency->dependency)){
      AgsDelayAudio *delay_audio;

      guint note_offset;

      delay_audio_run = (AgsDelayAudioRun *) ags_recall_dependency_resolve(recall_dependency, recall_id);
    
      delay_audio = (AgsDelayAudio *) AGS_RECALL_AUDIO_RUN(delay_audio_run)->recall_audio;

      //NOTE:JK: not really thread-safe but might be initialized by a task
      note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(recall->soundcard));
      
      AGS_COUNT_BEATS_AUDIO_RUN(recall)->sequencer_counter = note_offset % (guint) delay_audio->sequencer_duration->port_value.ags_port_double;
      AGS_COUNT_BEATS_AUDIO_RUN(recall)->notation_counter = note_offset;

      i++;
    }

    list = list->next;
  }

  g_object_set(G_OBJECT(recall),
	       "delay-audio-run\0", delay_audio_run,
	       NULL);
}

AgsRecall*
ags_count_beats_audio_run_duplicate(AgsRecall *recall,
				    AgsRecallID *recall_id,
				    guint *n_params, GParameter *parameter)
{
  AgsCountBeatsAudioRun *copy;

  copy = AGS_COUNT_BEATS_AUDIO_RUN(AGS_RECALL_CLASS(ags_count_beats_audio_run_parent_class)->duplicate(recall,
												       recall_id,
												       n_params, parameter));

  //  g_message("ags_count_beats_audio_run_duplicate\n\0");

  return((AgsRecall *) copy);
}

void
ags_count_beats_audio_run_notify_dependency(AgsRecall *recall,
					    guint notify_mode,
					    gint count)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;

  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(recall);

  switch(notify_mode){
  case AGS_RECALL_NOTIFY_RUN:
    count_beats_audio_run->hide_ref += count;
    //    g_message("count_beats_audio_run->hide_ref: %u\n\0", count_beats_audio_run->hide_ref);
    break;
  case AGS_RECALL_NOTIFY_AUDIO:
    break;
  case AGS_RECALL_NOTIFY_AUDIO_RUN:
    break;
  case AGS_RECALL_NOTIFY_CHANNEL:
    break;
  case AGS_RECALL_NOTIFY_CHANNEL_RUN:
    count_beats_audio_run->recall_ref += count;

    break;
  default:
    g_message("ags_count_beats_audio_run.c - ags_count_beats_audio_run_notify: unknown notify\0");
  }
}

void
ags_count_beats_audio_run_run_init_pre(AgsRecall *recall)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;

  AGS_RECALL_CLASS(ags_count_beats_audio_run_parent_class)->run_init_pre(recall);

  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(recall);

  count_beats_audio_run->first_run = TRUE;

  count_beats_audio_run->notation_hide_ref_counter = 0;
  count_beats_audio_run->sequencer_hide_ref_counter = 0;
  
  //  count_beats_audio_run->sequencer_counter = 0;
  //  count_beats_audio_run->notation_counter = 0;
}

void
ags_count_beats_audio_run_done(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsRecallID *recall_id;
  
  AgsCountBeatsAudio *count_beats_audio;
  AgsCountBeatsAudioRun *count_beats_audio_run;
  AgsDelayAudioRun *delay_audio_run;
  
  AgsCancelAudio *cancel_audio;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsThread *async_queue;

  AgsApplicationContext *application_context;

  GObject *soundcard;
  
  gboolean sequencer, notation;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *soundcard_mutex;
  
  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(recall);
  count_beats_audio = AGS_COUNT_BEATS_AUDIO(AGS_RECALL_AUDIO_RUN(count_beats_audio_run)->recall_audio);

  delay_audio_run = count_beats_audio_run->delay_audio_run;
  
  audio = AGS_RECALL_AUDIO(count_beats_audio)->audio;
  recall_id = AGS_RECALL(count_beats_audio_run)->recall_id;
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  soundcard = AGS_RECALL_AUDIO(count_beats_audio)->audio->soundcard;

  /* lookup soundcard mutex */
  pthread_mutex_lock(application_mutex);
  
  soundcard_mutex = ags_mutex_manager_lookup(mutex_manager,
					     soundcard);
  
  pthread_mutex_unlock(application_mutex);
  
  /* get application_context */
  pthread_mutex_lock(soundcard_mutex);
  
  application_context = ags_soundcard_get_application_context(AGS_SOUNDCARD(soundcard));

  pthread_mutex_unlock(soundcard_mutex);
  
  /* get main loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get async queue */
  async_queue = (AgsThread *) ags_thread_find_type(main_loop,
						   AGS_TYPE_TASK_THREAD);

  if((AGS_RECALL_ID_SEQUENCER & (recall_id->flags)) != 0){
    sequencer = TRUE;
  }else{
    sequencer = FALSE;
  }

  notation = FALSE;
  
  if((AGS_RECALL_ID_NOTATION & (recall->recall_id->flags)) != 0){
    //  notation = TRUE;
  }
  
  /* create cancel task */
  cancel_audio = ags_cancel_audio_new(AGS_RECALL_AUDIO(count_beats_audio)->audio,
				      FALSE, sequencer, notation);
  
  /* append AgsCancelAudio */
  ags_task_thread_append_task((AgsTaskThread *) async_queue,
			      (AgsTask *) cancel_audio);  

  /* call parent */
  AGS_RECALL_CLASS(ags_count_beats_audio_run_parent_class)->done(recall);

  ags_recall_done((AgsRecall *) delay_audio_run);
  ags_audio_done(audio,
		 recall_id);

}

/**
 * ags_count_beats_audio_run_notation_start:
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun
 * @run_order: the nth run order
 *
 * Emitted as notation starts playback.
 *
 * Since: 0.4
 */
void
ags_count_beats_audio_run_notation_start(AgsCountBeatsAudioRun *count_beats_audio_run,
					 guint run_order)
{
  g_return_if_fail(AGS_IS_COUNT_BEATS_AUDIO_RUN(count_beats_audio_run));

  g_object_ref(G_OBJECT(count_beats_audio_run));
  g_signal_emit(G_OBJECT(count_beats_audio_run),
		count_beats_audio_run_signals[NOTATION_START], 0,
		run_order);
  g_object_unref(G_OBJECT(count_beats_audio_run));
}

/**
 * ags_count_beats_audio_run_notation_loop:
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun
 * @run_order: the nth run order
 *
 * Emitted as notation loops playback.
 *
 * Since: 0.4
 */
void
ags_count_beats_audio_run_notation_loop(AgsCountBeatsAudioRun *count_beats_audio_run,
					guint run_order)
{
  g_return_if_fail(AGS_IS_COUNT_BEATS_AUDIO_RUN(count_beats_audio_run));

  g_object_ref(G_OBJECT(count_beats_audio_run));
  g_signal_emit(G_OBJECT(count_beats_audio_run),
		count_beats_audio_run_signals[NOTATION_LOOP], 0,
		run_order);
  g_object_unref(G_OBJECT(count_beats_audio_run));
}

/**
 * ags_count_beats_audio_run_notation_stop:
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun
 * @run_order: the nth run order
 *
 * Emitted as notation stops playback.
 *
 * Since: 0.4
 */
void
ags_count_beats_audio_run_notation_stop(AgsCountBeatsAudioRun *count_beats_audio_run,
					guint run_order)
{
  g_return_if_fail(AGS_IS_COUNT_BEATS_AUDIO_RUN(count_beats_audio_run));

  g_object_ref(G_OBJECT(count_beats_audio_run));
  g_signal_emit(G_OBJECT(count_beats_audio_run),
		count_beats_audio_run_signals[NOTATION_STOP], 0,
		run_order);
  g_object_unref(G_OBJECT(count_beats_audio_run));
}

/**
 * ags_count_beats_audio_run_sequencer_start:
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun
 * @run_order: the nth run order
 *
 * Emitted as sequencer starts playback.
 *
 * Since: 0.4
 */
void
ags_count_beats_audio_run_sequencer_start(AgsCountBeatsAudioRun *count_beats_audio_run,
					  guint run_order)
{
  g_return_if_fail(AGS_IS_COUNT_BEATS_AUDIO_RUN(count_beats_audio_run));

  g_object_ref(G_OBJECT(count_beats_audio_run));
  g_signal_emit(G_OBJECT(count_beats_audio_run),
		count_beats_audio_run_signals[SEQUENCER_START], 0,
		run_order);
  g_object_unref(G_OBJECT(count_beats_audio_run));
}

/**
 * ags_count_beats_audio_run_sequencer_loop:
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun
 * @run_order: the nth run order
 *
 * Emitted as sequencer loops playback.
 *
 * Since: 0.4
 */
void
ags_count_beats_audio_run_sequencer_loop(AgsCountBeatsAudioRun *count_beats_audio_run,
					 guint run_order)
{
  g_return_if_fail(AGS_IS_COUNT_BEATS_AUDIO_RUN(count_beats_audio_run));

  g_object_ref(G_OBJECT(count_beats_audio_run));
  g_signal_emit(G_OBJECT(count_beats_audio_run),
		count_beats_audio_run_signals[SEQUENCER_LOOP], 0,
		run_order);
  g_object_unref(G_OBJECT(count_beats_audio_run));
}

/**
 * ags_count_beats_audio_run_sequencer_stop:
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun
 * @run_order: the nth run order
 *
 * Emitted as sequencer stops playback.
 *
 * Since: 0.4
 */
void
ags_count_beats_audio_run_sequencer_stop(AgsCountBeatsAudioRun *count_beats_audio_run,
					 guint run_order)
{
  g_return_if_fail(AGS_IS_COUNT_BEATS_AUDIO_RUN(count_beats_audio_run));

  g_object_ref(G_OBJECT(count_beats_audio_run));
  g_signal_emit(G_OBJECT(count_beats_audio_run),
		count_beats_audio_run_signals[SEQUENCER_STOP], 0,
		run_order);
  g_object_unref(G_OBJECT(count_beats_audio_run));
}

void
ags_count_beats_audio_run_notation_alloc_output_callback(AgsDelayAudioRun *delay_audio_run,
							 guint run_order, gdouble delay, guint attack,
							 AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AgsCountBeatsAudio *count_beats_audio;

  gboolean loop;
  
  GValue loop_value = {0,};  

  if((AGS_RECALL_ID_NOTATION & (AGS_RECALL(count_beats_audio_run)->recall_id->flags)) == 0){
    return;
  }
  
  count_beats_audio = AGS_COUNT_BEATS_AUDIO(AGS_RECALL_AUDIO_RUN(count_beats_audio_run)->recall_audio);

  /* loop */
  g_value_init(&loop_value, G_TYPE_BOOLEAN);
  ags_port_safe_read(count_beats_audio->notation_loop, &loop_value);

  loop = g_value_get_boolean(&loop_value);
  g_value_unset(&loop_value);

  /* start */
  if(count_beats_audio_run->first_run){
    //    g_message("ags_count_beats_audio_run_sequencer_alloc_output_callback: start\n\0");
    ags_count_beats_audio_run_notation_start(count_beats_audio_run,
					     run_order);
  }

  /* loop */
  if(!count_beats_audio_run->first_run &&
     count_beats_audio_run->notation_counter == 0){
    /* emit notation signals */
    if(loop){
      //	g_message("ags_count_beats_audio_run_notation_alloc_output_callback: loop\n\0");

      ags_count_beats_audio_run_notation_loop(count_beats_audio_run,
					      run_order);
    }
  }
}

void
ags_count_beats_audio_run_sequencer_alloc_output_callback(AgsDelayAudioRun *delay_audio_run,
							  guint run_order,
							  gdouble delay, guint attack,
							  AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AgsCountBeatsAudio *count_beats_audio;

  gboolean loop;

  GValue value = {0,};
  
  if((AGS_RECALL_ID_SEQUENCER & (AGS_RECALL(count_beats_audio_run)->recall_id->flags)) == 0){
    return;
  }

  count_beats_audio = AGS_COUNT_BEATS_AUDIO(AGS_RECALL_AUDIO_RUN(count_beats_audio_run)->recall_audio);

  /* loop */
  g_value_init(&value, G_TYPE_BOOLEAN);
  ags_port_safe_read(count_beats_audio->sequencer_loop, &value);

  loop = g_value_get_boolean(&value);
  g_value_unset(&value);

  /* start */
  if(count_beats_audio_run->first_run){
    //    g_message("ags_count_beats_audio_run_sequencer_alloc_output_callback: start\n\0");
    ags_count_beats_audio_run_sequencer_start(count_beats_audio_run,
					      run_order);
  }

  /* loop */
  if(!count_beats_audio_run->first_run &&
     count_beats_audio_run->sequencer_counter == 0){
    /* emit sequencer signals */
    if(loop){
      //      g_message("ags_count_beats_audio_run_sequencer_alloc_output_callback: loop\n\0");
      
      ags_count_beats_audio_run_sequencer_loop(count_beats_audio_run,
					       run_order);
    }
  }
}

void
ags_count_beats_audio_run_notation_count_callback(AgsDelayAudioRun *delay_audio_run,
						  guint run_order,
						  gdouble delay, guint attack,
						  AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AgsCountBeatsAudio *count_beats_audio;

  gdouble loop_end;
  gboolean loop;

  GValue loop_value = {0,};
  GValue loop_end_value = {0,};  

  if((guint) floor(delay) != 0){
    return;
  }

  if((AGS_RECALL_ID_NOTATION & (AGS_RECALL(count_beats_audio_run)->recall_id->flags)) == 0){
    return;
  }
  
  count_beats_audio = AGS_COUNT_BEATS_AUDIO(AGS_RECALL_AUDIO_RUN(count_beats_audio_run)->recall_audio);

  /* loop */
  g_value_init(&loop_value, G_TYPE_BOOLEAN);
  ags_port_safe_read(count_beats_audio->notation_loop, &loop_value);

  loop = g_value_get_boolean(&loop_value);
  g_value_unset(&loop_value);

  /* loop end */
  g_value_init(&loop_end_value, G_TYPE_DOUBLE);
  ags_port_safe_read(count_beats_audio->notation_loop_end, &loop_end_value);

  loop_end = g_value_get_double(&loop_end_value);
  g_value_unset(&loop_end_value);

  /* tact signal */
  //  g_message("notation %d\0", count_beats_audio_run->notation_counter);
  ags_audio_tact(AGS_RECALL_AUDIO(count_beats_audio)->audio,
		 AGS_RECALL(count_beats_audio_run)->recall_id);
  
  /* 
   * Block counter for sequencer and notation counter
   */
  if(loop){
    if(count_beats_audio_run->notation_counter >= (guint) loop_end - 1.0){
      count_beats_audio_run->notation_counter = 0;
    }else{
      count_beats_audio_run->notation_counter += 1;
    }
  }else{
    count_beats_audio_run->notation_counter += 1;
  }
}

void
ags_count_beats_audio_run_sequencer_count_callback(AgsDelayAudioRun *delay_audio_run,
						   guint run_order, gdouble delay, guint attack,
						   AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AgsCountBeatsAudio *count_beats_audio;

  gdouble loop_end;
  gboolean loop;

  GValue loop_value = {0,};  
  GValue loop_end_value = {0,};

  if((guint) floor(delay) != 0){
    return;
  }

  if((AGS_RECALL_ID_SEQUENCER & (AGS_RECALL(count_beats_audio_run)->recall_id->flags)) == 0){
    return;
  }

  count_beats_audio = AGS_COUNT_BEATS_AUDIO(AGS_RECALL_AUDIO_RUN(count_beats_audio_run)->recall_audio);

  /* loop */
  g_value_init(&loop_value, G_TYPE_BOOLEAN);
  ags_port_safe_read(count_beats_audio->sequencer_loop, &loop_value);

  loop = g_value_get_boolean(&loop_value);
  g_value_unset(&loop_value);

  /* loop end */
  g_value_init(&loop_end_value, G_TYPE_DOUBLE);
  ags_port_safe_read(count_beats_audio->sequencer_loop_end, &loop_end_value);

  loop_end = g_value_get_double(&loop_end_value);
  g_value_unset(&loop_end_value);

  /* tact signal */
  ags_audio_tact(AGS_RECALL_AUDIO(count_beats_audio)->audio,
		 AGS_RECALL(count_beats_audio_run)->recall_id);
  
  /* 
   * Block counter for sequencer and notation counter
   */
  //    g_message("sequencer: tic\0");
  if(count_beats_audio_run->first_run){
    count_beats_audio_run->first_run = FALSE;
  }

  if(loop){
    if(count_beats_audio_run->sequencer_counter >= (guint) loop_end - 1.0){
      count_beats_audio_run->sequencer_counter = 0;
    }else{
      count_beats_audio_run->sequencer_counter += 1;
    }
  }else{      
    if(count_beats_audio_run->sequencer_counter >= (guint) loop_end - 1.0){
      AgsAudio *audio;

      AgsMutexManager *mutex_manager;

      GList *playback;

      pthread_mutex_t *application_mutex;
      pthread_mutex_t *audio_mutex;
      
      audio = AGS_RECALL_AUDIO_RUN(count_beats_audio_run)->recall_audio->audio;

      mutex_manager = ags_mutex_manager_get_instance();
      application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
      /* lookup audio mutex */
      pthread_mutex_lock(application_mutex);
  
      audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					     audio);
  
      pthread_mutex_unlock(application_mutex);

      /* reset sequencer counter */
      count_beats_audio_run->sequencer_counter = 0;

      /* get playback */
      pthread_mutex_lock(audio_mutex);
      
      playback = AGS_PLAYBACK_DOMAIN(audio->playback_domain)->playback;

      pthread_mutex_unlock(audio_mutex);
      
      /* emit stop signals */
      ags_count_beats_audio_run_sequencer_stop(count_beats_audio_run,
					       FALSE);

      /* set done flag in soundcard play */
      while(playback != NULL){
	AgsChannel *channel;
	AgsRecyclingContext *recycling_context;
	
	pthread_mutex_lock(audio_mutex);

	channel = audio->output;
	
	if(AGS_PLAYBACK(playback->data)->recall_id[1] != NULL){
	  recycling_context = AGS_PLAYBACK(playback->data)->recall_id[1]->recycling_context;
	}else{
	  recycling_context = NULL;
	}

	pthread_mutex_unlock(audio_mutex);

	if(recycling_context == AGS_RECALL(count_beats_audio_run)->recall_id->recycling_context){
	  AgsStreamChannelRun *stream_channel_run;
	  
	  GList *list;
	  GList *recall_recycling_list, *recall_audio_signal_list;
	  
	  gboolean found;

	  pthread_mutex_t *channel_mutex;

	  //	    AGS_PLAYBACK(playback->data)->flags |= AGS_PLAYBACK_DONE;

	  /* lookup channel mutex */
	  pthread_mutex_lock(application_mutex);
	  
	  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						   channel);
	  
	  pthread_mutex_unlock(application_mutex);

	  /* check if to stop audio processing */
	  found = FALSE;

	  pthread_mutex_lock(channel_mutex);
	  
	  list = channel->play;
	  list = ags_recall_find_type_with_recycling_context(list,
							     AGS_TYPE_STREAM_CHANNEL_RUN,
							     (GObject *) recycling_context);

	  if(list != NULL){
	    stream_channel_run = AGS_STREAM_CHANNEL_RUN(list->data);

	    recall_recycling_list = AGS_RECALL(stream_channel_run)->children;
  
	    while(recall_recycling_list != NULL){
	      recall_audio_signal_list = AGS_RECALL(recall_recycling_list->data)->children;

	      while(recall_audio_signal_list != NULL){
		found = TRUE;

		g_signal_connect_after(G_OBJECT(recall_audio_signal_list->data), "done\0",
				       G_CALLBACK(ags_count_beats_audio_run_stream_audio_signal_done_callback), AGS_RECALL(count_beats_audio_run));

		recall_audio_signal_list = recall_audio_signal_list->next;
	      }

	      recall_recycling_list = recall_recycling_list->next;
	    }
	  }

	  pthread_mutex_unlock(channel_mutex);

	  /* stop audio processing*/
	  if(!found){
	    ags_count_beats_audio_run_stop(count_beats_audio_run,
					   FALSE);
	  }

	  break;
	}

	/* iterate playback */
	pthread_mutex_lock(audio_mutex);
	
      	playback = playback->next;

	pthread_mutex_unlock(audio_mutex);
      }

      return;
    }

    count_beats_audio_run->sequencer_counter += 1;
  }
}

void
ags_count_beats_audio_run_read_resolve_dependency(AgsFileLookup *file_lookup,
						  GObject *recall)
{
  AgsFileIdRef *id_ref;
  gchar *xpath;

  xpath = (gchar *) xmlGetProp(file_lookup->node,
			       "xpath\0");

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_xpath(file_lookup->file, xpath);

  if(AGS_IS_DELAY_AUDIO_RUN(id_ref->ref)){
    g_object_set(G_OBJECT(recall),
		 "delay-audio-run\0", AGS_DELAY_AUDIO_RUN(id_ref->ref),
		 NULL);
  }
}

void
ags_count_beats_audio_run_write_resolve_dependency(AgsFileLookup *file_lookup,
						   GObject *recall)
{
  AgsFileIdRef *id_ref;
  gchar *id;

  id_ref = (AgsFileIdRef *) ags_file_find_id_ref_by_reference(file_lookup->file,
							      AGS_COUNT_BEATS_AUDIO_RUN(file_lookup->ref)->delay_audio_run);

  id = xmlGetProp(id_ref->node, AGS_FILE_ID_PROP);

  xmlNewProp(file_lookup->node,
	     "xpath\0",
  	     g_strdup_printf("xpath=//*[@id='%s']\0", id));
}

void
ags_count_beats_audio_run_stream_audio_signal_done_callback(AgsRecall *recall,
							    AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AgsCountBeatsAudio *count_beats_audio;
  GValue loop_sequencer = {0,};
  GValue loop_end_sequencer = {0,};

  count_beats_audio = AGS_COUNT_BEATS_AUDIO(AGS_RECALL_AUDIO_RUN(count_beats_audio_run)->recall_audio);

  /* check for loop or notation */
  g_value_init(&loop_sequencer,
	       G_TYPE_BOOLEAN);
  ags_port_safe_read(count_beats_audio->sequencer_loop,
		     &loop_sequencer);
  
  if((AGS_RECALL_ID_SEQUENCER & (recall->recall_id->flags)) != 0){
    if(g_value_get_boolean(&loop_sequencer)){
      g_value_unset(&loop_sequencer);
      return;
    }else{
      g_value_unset(&loop_sequencer);
    }

    g_value_init(&loop_end_sequencer, G_TYPE_DOUBLE);
    ags_port_safe_read(count_beats_audio->sequencer_loop_end, &loop_end_sequencer);
  
    if(count_beats_audio_run->sequencer_counter < (guint) g_value_get_double(&loop_end_sequencer) - 1.0){
      g_value_unset(&loop_end_sequencer);
      return;
    }else{
      g_value_unset(&loop_end_sequencer);
    }
  
    /* you're done */
    ags_recall_done((AgsRecall *) count_beats_audio_run);
  }
}

void
ags_count_beats_audio_run_stop(AgsCountBeatsAudioRun *count_beats_audio_run,
			       gboolean notation)
{
  if(!notation){
    if((AGS_RECALL_ID_SEQUENCER & (AGS_RECALL(count_beats_audio_run)->recall_id->flags)) != 0){
      //TODO:JK: enhance me
      ags_recall_done((AgsRecall *) count_beats_audio_run);
    }
  }else{
    //    ags_count_beats_audio_run_done(count_beats_audio_run);
  }
} 

gdouble
ags_count_beats_audio_run_get_bpm(AgsTactable *tactable)
{
  return(AGS_COUNT_BEATS_AUDIO_RUN(tactable)->bpm);
}

gdouble
ags_count_beats_audio_run_get_tact(AgsTactable *tactable)
{
  return(AGS_COUNT_BEATS_AUDIO_RUN(tactable)->tact);
}

void
ags_count_beats_audio_run_change_bpm(AgsTactable *tactable, gdouble new_bpm, gdouble old_bpm)
{
  /* empty */
}

void
ags_count_beats_audio_run_change_tact(AgsTactable *tactable, gdouble new_tact, gdouble old_tact)
{
  /* empty */
}

/**
 * ags_count_beats_audio_run_new:
 * @delay_audio_run: the #AgsDelayAudioRun dependency
 *
 * Creates an #AgsCountBeatsAudioRun
 *
 * Returns: a new #AgsCountBeatsAudioRun
 *
 * Since: 0.4
 */
AgsCountBeatsAudioRun*
ags_count_beats_audio_run_new(AgsDelayAudioRun *delay_audio_run)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;

  count_beats_audio_run = (AgsCountBeatsAudioRun *) g_object_new(AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
								 "delay_audio_run\0", delay_audio_run,
								 NULL);
  
  return(count_beats_audio_run);
}
