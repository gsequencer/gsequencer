/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/audio/ags_recall.h>

#include <ags/plugin/ags_ladspa_manager.h>
#include <ags/plugin/ags_dssi_manager.h>
#include <ags/plugin/ags_lv2_manager.h>

#include <ags/audio/ags_sound_enums.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recycling_context.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_dependency.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_recall_recycling.h>
#include <ags/audio/ags_recall_audio_signal.h>

#include <libxml/tree.h>

#include <string.h>

#include <ags/i18n.h>

void ags_recall_class_init(AgsRecallClass *recall_class);
void ags_recall_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_init(AgsRecall *recall);
void ags_recall_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_recall_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_recall_dispose(GObject *gobject);
void ags_recall_finalize(GObject *gobject);

AgsUUID* ags_recall_get_uuid(AgsConnectable *connectable);
gboolean ags_recall_has_resource(AgsConnectable *connectable);
gboolean ags_recall_is_ready(AgsConnectable *connectable);
void ags_recall_add_to_registry(AgsConnectable *connectable);
void ags_recall_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_recall_list_resource(AgsConnectable *connectable);
xmlNode* ags_recall_xml_compose(AgsConnectable *connectable);
void ags_recall_xml_parse(AgsConnectable *connectable,
			  xmlNode *node);
gboolean ags_recall_is_connected(AgsConnectable *connectable);
void ags_recall_connect(AgsConnectable *connectable);
void ags_recall_disconnect(AgsConnectable *connectable);

void ags_recall_real_resolve_dependency(AgsRecall *recall);
void ags_recall_real_check_rt_data(AgsRecall *recall);

void ags_recall_real_run_init_pre(AgsRecall *recall);
void ags_recall_real_run_init_inter(AgsRecall *recall);
void ags_recall_real_run_init_post(AgsRecall *recall);

void ags_recall_real_feed_input_queue(AgsRecall *recall);
void ags_recall_real_automate(AgsRecall *recall);

void ags_recall_real_run_pre(AgsRecall *recall);
void ags_recall_real_run_inter(AgsRecall *recall);
void ags_recall_real_run_post(AgsRecall *recall);

void ags_recall_real_do_feedback(AgsRecall *recall);
void ags_recall_real_feed_output_queue(AgsRecall *recall);

void ags_recall_real_stop_persistent(AgsRecall *recall);
void ags_recall_real_cancel(AgsRecall *recall);
void ags_recall_real_done(AgsRecall *recall);

AgsRecall* ags_recall_real_duplicate(AgsRecall *reall,
				     AgsRecallID *recall_id,
				     guint *n_params, gchar **parameter_name, GValue *value);

void ags_recall_child_done(AgsRecall *child,
			   AgsRecall *parent);

/**
 * SECTION:ags_recall
 * @short_description: The recall base class
 * @title: AgsRecall
 * @section_id: 
 * @include: ags/audio/ags_recall.h
 *
 * #AgsRecall acts as effect processor. It depends on staging program from #AgsAudioThread and #AgsChannelThread
 * what signals are invoked.
 *
 * ags-fx staging does actually only run duplicate and intialization of a recall:
 * 
 * * play-duplicate
 * * play-run-init-pre, play-run-init-inter and play-run-init-post
 *
 * And the actuall audio processing:
 *
 * * play-automate
 * * play-run-inter
 *
 * Termination of audio processing usually follows:
 *
 * * play-done
 * * play-cancel
 * 
 * You usually create recalls of the same audio processing effect for different contices. For those
 * contices exists derived recalls you inherit from.
 *
 * * #AgsRecallAudio
 * * #AgsRecallAudioRun
 * * #AgsRecallChannel
 * * #AgsRecallChannelRun
 * * #AgsRecallRecycling
 * * #AgsREcallAudioSignal
 *
 * You add the derived #AgsRecallAudio and #AgsRecallAudioRun to #AgsAudio by calling ags_audio_add_recall().
 * Analogously, you do this for #AgsRecallChannel and #AgsRecallChannelRun by calling ags_channel_add_recall().
 *
 * Make sure you set property `child-type` of #AgsRecallChannelRun to #AgsRecallRecycling and of
 * #AgsRecallRecycling to #AgsRecallAudioSignal derived type.
 *
 * Further #AgsRecall is packed by #AgsRecallContainer.
 */

enum{
  PLAY_RESOLVE_DEPENDENCY,
  PLAY_CHECK_RT_DATA,  
  PLAY_RUN_INIT_PRE,
  PLAY_RUN_INIT_INTER,
  PLAY_RUN_INIT_POST,
  PLAY_FEED_INPUT_QUEUE,
  PLAY_AUTOMATE,
  PLAY_RUN_PRE,
  PLAY_RUN_INTER,
  PLAY_RUN_POST,
  PLAY_DO_FEEDBACK,
  PLAY_FEED_OUTPUT_QUEUE,
  PLAY_STOP_PERSISTENT,
  PLAY_CANCEL,
  PLAY_DONE,
  PLAY_DUPLICATE,
  PLAY_NOTIFY_DEPENDENCY,
  CHILD_ADDED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_EFFECT,
  PROP_EFFECT_INDEX,
  PROP_RECALL_CONTAINER,
  PROP_OUTPUT_SOUNDCARD,
  PROP_OUTPUT_SOUNDCARD_CHANNEL,
  PROP_INPUT_SOUNDCARD,
  PROP_INPUT_SOUNDCARD_CHANNEL,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_PAD,
  PROP_AUDIO_CHANNEL,
  PROP_LINE,
  PROP_PORT,
  PROP_AUTOMATION_PORT,
  PROP_RECALL_ID,
  PROP_RECALL_DEPENDENCY,
  PROP_PARENT,
  PROP_CHILD_TYPE,
  PROP_CHILD,
};

static gpointer ags_recall_parent_class = NULL;
static guint recall_signals[LAST_SIGNAL];

static gboolean ags_recall_global_children_lock_free = FALSE;
static gboolean ags_recall_global_omit_event = TRUE;
static gboolean ags_recall_global_performance_mode = FALSE;
static gboolean ags_recall_global_rt_safe = FALSE;

GType
ags_recall_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_recall = 0;

    static const GTypeInfo ags_recall_info = {
      sizeof (AgsRecallClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecall),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall = g_type_register_static(G_TYPE_OBJECT,
					     "AgsRecall",
					     &ags_recall_info,
					     0);

    g_type_add_interface_static(ags_type_recall,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_recall);
  }

  return g_define_type_id__volatile;
}

GType
ags_recall_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_RECALL_TEMPLATE, "AGS_RECALL_TEMPLATE", "recall-template" },
      { AGS_RECALL_DEFAULT_TEMPLATE, "AGS_RECALL_DEFAULT_TEMPLATE", "recall-default-template" },
      { AGS_RECALL_HAS_OUTPUT_PORT, "AGS_RECALL_HAS_OUTPUT_PORT", "recall-has-output-port" },
      { AGS_RECALL_BYPASS, "AGS_RECALL_BYPASS", "recall-bypass" },
      { AGS_RECALL_INITIAL_RUN, "AGS_RECALL_INITIAL_RUN", "recall-initial-run" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsRecallFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

GType
ags_recall_notify_dependency_mode_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_RECALL_NOTIFY_RUN, "AGS_RECALL_NOTIFY_RUN", "recall-notify-run" },
      { AGS_RECALL_NOTIFY_AUDIO, "AGS_RECALL_NOTIFY_AUDIO", "recall-notify-audio" },
      { AGS_RECALL_NOTIFY_AUDIO_RUN, "AGS_RECALL_NOTIFY_AUDIO_RUN", "recall-notify-audio-run" },
      { AGS_RECALL_NOTIFY_CHANNEL, "AGS_RECALL_NOTIFY_CHANNEL", "recall-notify-channel" },
      { AGS_RECALL_NOTIFY_CHANNEL_RUN, "AGS_RECALL_NOTIFY_CHANNEL_RUN", "recall-notify-channel-run" },
      { AGS_RECALL_NOTIFY_RECALL, "AGS_RECALL_NOTIFY_RECALL", "recall-notify-recall" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsRecallNotifyDependencyMode"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

void
ags_recall_class_init(AgsRecallClass *recall)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_recall_parent_class = g_type_class_peek_parent(recall);

  /* GObjectClass */
  gobject = (GObjectClass *) recall;

  gobject->set_property = ags_recall_set_property;
  gobject->get_property = ags_recall_get_property;

  gobject->dispose = ags_recall_dispose;
  gobject->finalize = ags_recall_finalize;

  /* properties */
  /**
   * AgsRecall:filename:
   *
   * The plugin's filename.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("filename",
				   i18n_pspec("the object file"),
				   i18n_pspec("The filename as string of object file"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsRecall:effect:
   *
   * The plugin's effect.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("effect",
				   i18n_pspec("the effect"),
				   i18n_pspec("The effect's string representation"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EFFECT,
				  param_spec);

  /**
   * AgsRecall:effect-index:
   *
   * The effect's index.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("effect-index",
				 i18n_pspec("index of effect"),
				 i18n_pspec("The numerical index of effect"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EFFECT_INDEX,
				  param_spec);
  
  /**
   * AgsRecall:recall-container:
   *
   * The #AgsRecallContainer packed into.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("recall-container",
				   i18n_pspec("container of recall"),
				   i18n_pspec("The container which this recall is packed into"),
				   AGS_TYPE_RECALL_CONTAINER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_CONTAINER,
				  param_spec);

  /**
   * AgsRecall:output-soundcard:
   *
   * The assigned soundcard.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("output-soundcard",
				   i18n_pspec("output soundcard"),
				   i18n_pspec("The output soundcard which this recall is packed into"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT_SOUNDCARD,
				  param_spec);

  /**
   * AgsRecall:output-soundcard-channel:
   *
   * The output soundcard channel.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_int("output-soundcard-channel",
				i18n_pspec("output soundcard channel"),
				i18n_pspec("The output soundcard channel"),
				-1,
				G_MAXINT32,
				-1,
				G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT_SOUNDCARD_CHANNEL,
				  param_spec);

  /**
   * AgsRecall:input-soundcard:
   *
   * The assigned soundcard.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("input-soundcard",
				   i18n_pspec("input soundcard"),
				   i18n_pspec("The input soundcard which this recall is packed into"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT_SOUNDCARD,
				  param_spec);

  /**
   * AgsRecall:input-soundcard-channel:
   *
   * The input soundcard channel.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_int("input-soundcard-channel",
				i18n_pspec("input soundcard channel"),
				i18n_pspec("The input soundcard channel"),
				-1,
				G_MAXINT32,
				-1,
				G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT_SOUNDCARD_CHANNEL,
				  param_spec);

  /**
   * AgsRecall:samplerate:
   *
   * The samplerate.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("samplerate"),
				 i18n_pspec("The samplerate"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsRecall:buffer-size:
   *
   * The buffer size.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("buffer-size",
				 i18n_pspec("buffer size"),
				 i18n_pspec("The buffer size"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsRecall:format:
   *
   * The format.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("format",
				 i18n_pspec("format"),
				 i18n_pspec("The format"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsRecall:pad:
   *
   * The nth pad.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("pad",
				  i18n_pspec("nth pad"),
				  i18n_pspec("The nth pad"),
				  0,
				  G_MAXUINT32,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PAD,
				  param_spec);

  /**
   * AgsRecall:audio-channel:
   *
   * The nth audio channel.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("audio-channel",
				  i18n_pspec("nth audio channel"),
				  i18n_pspec("The nth audio channel"),
				  0,
				  G_MAXUINT32,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);

  /**
   * AgsRecall:line:
   *
   * The nth line.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("line",
				  i18n_pspec("nth line"),
				  i18n_pspec("The nth line"),
				  0,
				  G_MAXUINT32,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LINE,
				  param_spec);
  
  /**
   * AgsRecall:port: (type GList(AgsPort)) (transfer full)
   *
   * The assigned #AgsPort
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("port",
				    i18n_pspec("port of recall"),
				    i18n_pspec("The port of recall"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT,
				  param_spec);

  /**
   * AgsRecall:automation-port: (type GList(AgsPort)) (transfer full)
   *
   * The #AgsPort doing automation.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("automation-port",
				    i18n_pspec("automation port"),
				    i18n_pspec("The port doing automation"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUTOMATION_PORT,
				  param_spec);

  /**
   * AgsRecall:recall-id:
   *
   * The #AgsRecallID running in.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("recall-id",
				   i18n_pspec("run id of recall"),
				   i18n_pspec("The recall id of the recall"),
				   AGS_TYPE_RECALL_ID,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_ID,
				  param_spec);
  
  /**
   * AgsRecall:recall-dependency: (type GList(AgsRecallDependency)) (transfer full)
   *
   * The #AgsRecallDependency.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("recall-dependency",
				    i18n_pspec("recall dependency"),
				    i18n_pspec("The assigned recall dependency"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_DEPENDENCY,
				  param_spec);

  /**
   * AgsRecall:parent:
   *
   * The parent #AgsRecall.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("parent",
				   i18n_pspec("parent recall of this recall"),
				   i18n_pspec("The recall should be the parent instance of this recall"),
				   AGS_TYPE_RECALL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PARENT,
				  param_spec);

  /**
   * AgsRecall:child-type:
   *
   * The type of child #AgsRecall.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_gtype("child-type",
				  i18n_pspec("child type"),
				  i18n_pspec("The type of child that can be added"),
				  G_TYPE_NONE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHILD_TYPE,
				  param_spec);
  
  /**
   * AgsRecall:child: (type GList(AgsRecall)) (transfer full)
   *
   * The child #AgsRecall.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("child",
				    i18n_pspec("child of recall"),
				    i18n_pspec("The child that can be added"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHILD,
				  param_spec);
  
  /* AgsRecallClass */
  recall->resolve_dependency = ags_recall_real_resolve_dependency;
  recall->check_rt_data = ags_recall_real_check_rt_data;
  
  recall->run_init_pre = ags_recall_real_run_init_pre;
  recall->run_init_inter = ags_recall_real_run_init_inter;
  recall->run_init_post = ags_recall_real_run_init_post;

  recall->feed_input_queue = ags_recall_real_feed_input_queue;
  recall->automate = ags_recall_real_automate;

  recall->run_pre = ags_recall_real_run_pre;
  recall->run_inter = ags_recall_real_run_inter;
  recall->run_post = ags_recall_real_run_post;

  recall->do_feedback = ags_recall_real_do_feedback;
  recall->feed_output_queue = ags_recall_real_feed_output_queue;

  recall->stop_persistent = ags_recall_real_stop_persistent;
  recall->cancel = ags_recall_real_cancel;
  recall->done = ags_recall_real_done;

  recall->duplicate = ags_recall_real_duplicate;

  recall->notify_dependency = NULL;

  recall->child_added = NULL;

  /* signals */
  /**
   * AgsRecall::resolve-dependency:
   * @recall: the #AgsRecall to resolve
   *
   * The ::resolve-dependency signal notifies about resolving
   * dependency.
   *
   * Since: 3.0.0
   */
  recall_signals[PLAY_RESOLVE_DEPENDENCY] =
    g_signal_new("resolve-dependency",
		 G_TYPE_FROM_CLASS(recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecallClass, resolve_dependency),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsRecall::check-rt-data:
   * @recall: the #AgsRecall to initialize
   *
   * The ::check-rt-data signal notifies about initializing
   * stage 0.
   *
   * Since: 3.0.0
   */
  recall_signals[PLAY_CHECK_RT_DATA] =
    g_signal_new("check-rt-data",
		 G_TYPE_FROM_CLASS(recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecallClass, check_rt_data),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsRecall::run-init-pre:
   * @recall: the #AgsRecall to initialize
   *
   * The ::run-init-pre signal notifies about initializing
   * stage 0.
   *
   * Since: 3.0.0
   */
  recall_signals[PLAY_RUN_INIT_PRE] =
    g_signal_new("run-init-pre",
		 G_TYPE_FROM_CLASS(recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecallClass, run_init_pre),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsRecall::run-init-inter:
   * @recall: the #AgsRecall to initialize
   *
   * The ::run-init-inter signal notifies about initializing
   * stage 1.
   *
   * Since: 3.0.0
   */
  recall_signals[PLAY_RUN_INIT_INTER] =
    g_signal_new("run-init-inter",
		 G_TYPE_FROM_CLASS(recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecallClass, run_init_inter),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsRecall::run-init-post:
   * @recall: the #AgsRecall to initialize
   *
   * The ::run-init-post signal notifies about initializing
   * stage 2.
   *
   * Since: 3.0.0
   */
  recall_signals[PLAY_RUN_INIT_POST] =
    g_signal_new("run-init-post",
		 G_TYPE_FROM_CLASS(recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecallClass, run_init_post),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsRecall::feed-input-queue:
   * @recall: the #AgsRecall to play 
   *
   * The ::feed-input-queue signal notifies about running
   * feed input queue.
   *
   * Since: 3.0.0
   */
  recall_signals[PLAY_FEED_INPUT_QUEUE] =
    g_signal_new("feed-input-queue",
		 G_TYPE_FROM_CLASS(recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecallClass, feed_input_queue),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsRecall::automate:
   * @recall: the #AgsRecall to play 
   *
   * The ::automate signal notifies about running
   * automation and is normally called during ::run-pre.
   *
   * Since: 3.0.0
   */
  recall_signals[PLAY_AUTOMATE] =
    g_signal_new("automate",
		 G_TYPE_FROM_CLASS(recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecallClass, automate),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsRecall::run-pre:
   * @recall: the #AgsRecall to play 
   *
   * The ::run-pre signal notifies about running
   * stage 0.
   *
   * Since: 3.0.0
   */
  recall_signals[PLAY_RUN_PRE] =
    g_signal_new("run-pre",
		 G_TYPE_FROM_CLASS(recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecallClass, run_pre),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsRecall::run-inter:
   * @recall: the #AgsRecall to play
   *
   * The ::run-inter signal notifies about running
   * stage 1.
   *
   * Since: 3.0.0
   */
  recall_signals[PLAY_RUN_INTER] =
    g_signal_new("run-inter",
		 G_TYPE_FROM_CLASS(recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecallClass, run_inter),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsRecall::run-post:
   * @recall: the #AgsRecall to play
   *
   * The ::run-post signal notifies about running
   * stage 2.
   *
   * Since: 3.0.0
   */
  recall_signals[PLAY_RUN_POST] =
    g_signal_new("run-post",
		 G_TYPE_FROM_CLASS(recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecallClass, run_post),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsRecall::do-feedback:
   * @recall: the #AgsRecall to play
   *
   * The ::do-feedback signal notifies about running
   * stage 2.
   *
   * Since: 3.0.0
   */
  recall_signals[PLAY_DO_FEEDBACK] =
    g_signal_new("do-feedback",
		 G_TYPE_FROM_CLASS(recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecallClass, do_feedback),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsRecall::feed-output-queue:
   * @recall: the #AgsRecall to play 
   *
   * The ::feed-output-queue signal notifies about running
   * feed output queue.
   *
   * Since: 3.0.0
   */
  recall_signals[PLAY_FEED_OUTPUT_QUEUE] =
    g_signal_new("feed-output-queue",
		 G_TYPE_FROM_CLASS(recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecallClass, feed_output_queue),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
  
  /**
   * AgsRecall::stop-persistent:
   * @recall: the #AgsRecall stop playback
   *
   * The ::stop-persistent signal notifies about definitively
   * stopping playback.
   *
   * Since: 3.0.0
   */
  recall_signals[PLAY_STOP_PERSISTENT] =
    g_signal_new("stop_persistent",
		 G_TYPE_FROM_CLASS(recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecallClass, stop_persistent),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsRecall::cancel:
   * @recall: the #AgsRecall to cancel playback
   *
   * The ::cancel signal notifies about cancelling playback.
   *
   * Since: 3.0.0
   */
  recall_signals[PLAY_CANCEL] =
    g_signal_new("cancel",
		 G_TYPE_FROM_CLASS(recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecallClass, cancel),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsRecall::done:
   * @recall: the #AgsRecall to finish playback
   *
   * The ::done signal notifies about stopping playback.
   *
   * Since: 3.0.0
   */
  recall_signals[PLAY_DONE] =
    g_signal_new("done",
		 G_TYPE_FROM_CLASS(recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecallClass, done),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsRecall::duplicate:
   * @recall: the #AgsRecall to duplicate
   * @recall_id: the assigned #AgsRecallID
   * @n_params: pointer to array length
   * @parameter_name: parameter name string vector
   * @value: the #GValue-struct array
   *
   * The ::duplicate signal notifies about instantiating.
   *
   * Returns: (transfer full): the new #AgsRecall instance
   * 
   * Since: 3.0.0
   */
  recall_signals[PLAY_DUPLICATE] =
    g_signal_new("duplicate",
		 G_TYPE_FROM_CLASS(recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecallClass, duplicate),
		 NULL, NULL,
		 ags_cclosure_marshal_OBJECT__OBJECT_POINTER_POINTER_POINTER,
		 G_TYPE_OBJECT, 4,
		 G_TYPE_OBJECT,
		 G_TYPE_POINTER, G_TYPE_POINTER, G_TYPE_POINTER);

  /**
   * AgsRecall::notify-dependency:
   * @recall: the #AgsRecall to notify
   * @dependency: the kind of dependency
   * @increase: if %TRUE increase dependency count, else if %FALSE decrease
   *
   * The ::notify-dependency signal notifies about dependency
   * added.
   *
   * Since: 3.0.0
   */
  recall_signals[PLAY_NOTIFY_DEPENDENCY] =
    g_signal_new("notify-dependency",
		 G_TYPE_FROM_CLASS(recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecallClass, notify_dependency),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_BOOLEAN,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT, G_TYPE_BOOLEAN);

  /**
   * AgsRecall::child-added:
   * @recall: the #AgsRecall to add the child
   * @child: the #AgsRecall to add
   *
   * The ::child-added signal notifies about children
   * added.
   *
   * Since: 3.0.0
   */
  recall_signals[CHILD_ADDED] =
    g_signal_new("child-added",
		 G_TYPE_FROM_CLASS(recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecallClass, child_added),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_recall_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_recall_get_uuid;
  connectable->has_resource = ags_recall_has_resource;

  connectable->is_ready = ags_recall_is_ready;
  connectable->add_to_registry = ags_recall_add_to_registry;
  connectable->remove_from_registry = ags_recall_remove_from_registry;

  connectable->list_resource = ags_recall_list_resource;
  connectable->xml_compose = ags_recall_xml_compose;
  connectable->xml_parse = ags_recall_xml_parse;

  connectable->is_connected = ags_recall_is_connected;  
  connectable->connect = ags_recall_connect;
  connectable->disconnect = ags_recall_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_recall_init(AgsRecall *recall)
{  
  recall->flags = 0;
  recall->connectable_flags = 0;
  recall->ability_flags = 0;
  recall->behaviour_flags = 0;
  recall->sound_scope = -1;
  recall->staging_flags = 0;
  recall->state_flags = 0;

  /* add recall mutex */
  g_rec_mutex_init(&(recall->obj_mutex));
    
  /* uuid */
#if 0
  recall->uuid = ags_uuid_alloc();
  ags_uuid_generate(recall->uuid);
#else
  recall->uuid = NULL;
#endif
  
  /* version and build id */
  recall->version = NULL;
  recall->build_id = NULL;

  /* name */
  recall->name = NULL;

  /* filename and effect */
  recall->filename = NULL;
  recall->effect = NULL;
  recall->effect_index = 0;

  /* xml type  */
  recall->xml_type = NULL;

  /* base init */
  recall->recall_container = NULL;

  recall->output_soundcard = NULL;
  recall->output_soundcard_channel = 0;

  recall->input_soundcard = NULL;
  recall->input_soundcard_channel = 0;

#if 0
  /* config */
  config = ags_config_get_instance();

  /* presets */
  recall->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  recall->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  recall->format = ags_soundcard_helper_config_get_format(config);
#else
  /* presets */
  recall->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  recall->buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  recall->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
#endif
  
  recall->pad = 0;
  recall->audio_channel = 0;

  recall->line = 0;
  
  /* port and automation port */
  recall->port = NULL;
  recall->automation_port = NULL;

  /* recall id */
  recall->recall_id = NULL;

  /* recall dependency */
  recall->recall_dependency = NULL;

  /* recall handler */
  recall->recall_handler = NULL;

  /* nested recall */
  recall->parent = NULL;

  recall->child_type = G_TYPE_NONE;

  recall->n_child_params = 0;
  recall->child_parameter_name = NULL;
  recall->child_value = NULL;

  recall->children = NULL;
}

void
ags_recall_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsRecall *recall;

  GRecMutex *recall_mutex;

  recall = AGS_RECALL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);

      g_rec_mutex_lock(recall_mutex);

      if(filename == recall->filename){
	g_rec_mutex_unlock(recall_mutex);
	
	return;
      }

      if(recall->filename != NULL){
	g_free(recall->filename);
      }

      recall->filename = g_strdup(filename);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;
      
      effect = g_value_get_string(value);

      g_rec_mutex_lock(recall_mutex);

      if(effect == recall->effect){
	g_rec_mutex_unlock(recall_mutex);
	
	return;
      }

      if(recall->effect != NULL){
	g_free(recall->effect);
      }

      recall->effect = g_strdup(effect);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_EFFECT_INDEX:
    {
      g_rec_mutex_lock(recall_mutex);

      recall->effect_index = g_value_get_uint(value);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_RECALL_CONTAINER:
    {
      AgsRecallContainer *recall_container;

      recall_container = (AgsRecallContainer *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);
      
      if(recall->recall_container == (GObject *) recall_container){
	g_rec_mutex_unlock(recall_mutex);
	
	return;
      }
      
      if(recall->recall_container != NULL){
	g_object_unref(G_OBJECT(recall->recall_container));

	recall->recall_container = NULL;
      }

      if(recall_container != NULL){
	g_object_ref(recall_container);	
      }

      recall->recall_container = (GObject *) recall_container;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_OUTPUT_SOUNDCARD:
    {
      GObject *output_soundcard;

      output_soundcard = (GObject *) g_value_get_object(value);

      ags_recall_set_output_soundcard(recall,
				      output_soundcard);
    }
    break;
  case PROP_OUTPUT_SOUNDCARD_CHANNEL:
    {
      g_rec_mutex_lock(recall_mutex);

      recall->output_soundcard_channel = g_value_get_int(value);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_INPUT_SOUNDCARD:
    {
      GObject *input_soundcard;

      input_soundcard = (GObject *) g_value_get_object(value);

      ags_recall_set_input_soundcard(recall,
				     input_soundcard);
    }
    break;
  case PROP_INPUT_SOUNDCARD_CHANNEL:
    {
      g_rec_mutex_lock(recall_mutex);

      recall->input_soundcard_channel = g_value_get_int(value);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      ags_recall_set_samplerate(recall,
				samplerate);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      ags_recall_set_buffer_size(recall,
				 buffer_size);
    }
    break;
  case PROP_FORMAT:
    {
      AgsSoundcardFormat format;

      format = g_value_get_uint(value);

      ags_recall_set_format(recall,
			    format);
    }
    break;
  case PROP_PAD:
    {
      g_rec_mutex_lock(recall_mutex);

      recall->pad = g_value_get_uint(value);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      g_rec_mutex_lock(recall_mutex);

      recall->audio_channel = g_value_get_uint(value);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LINE:
    {
      g_rec_mutex_lock(recall_mutex);

      recall->line = g_value_get_uint(value);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PORT:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_pointer(value);

      g_rec_mutex_lock(recall_mutex);

      if(!AGS_IS_PORT(port) ||
	 g_list_find(recall->port, port) != NULL){
	g_rec_mutex_unlock(recall_mutex);
	
	return;
      }

      g_object_ref(port);
      recall->port = g_list_prepend(recall->port,
				    port);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_AUTOMATION_PORT:
    {
      AgsPort *automation_port;

      automation_port = (AgsPort *) g_value_get_pointer(value);

      g_rec_mutex_lock(recall_mutex);

      if(!AGS_IS_PORT(automation_port) ||
	 g_list_find(recall->automation_port, automation_port) != NULL){
	g_rec_mutex_unlock(recall_mutex);
	
	return;
      }

      g_object_ref(automation_port);
      recall->port = g_list_prepend(recall->automation_port,
				    automation_port);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RECALL_ID:
    {
      AgsRecallID *recall_id;

      recall_id = (AgsRecallID *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(recall->recall_id == recall_id){	
	g_rec_mutex_unlock(recall_mutex);
	
	return;
      }

      g_rec_mutex_unlock(recall_mutex);
	
      ags_recall_set_recall_id(recall, recall_id);
    }
    break;
  case PROP_RECALL_DEPENDENCY:
    {
      AgsRecallDependency *recall_dependency;

      recall_dependency = (AgsRecallDependency *) g_value_get_pointer(value);

      g_rec_mutex_lock(recall_mutex);
      
      if(!AGS_IS_RECALL_DEPENDENCY(recall_dependency) ||
	 g_list_find(recall->recall_dependency, recall_dependency) != NULL){
	g_rec_mutex_unlock(recall_mutex);
	
	return;
      }
      
      g_rec_mutex_unlock(recall_mutex);
      
      ags_recall_add_recall_dependency(recall, recall_dependency);
    }
    break;
  case PROP_PARENT:
    {
      AgsRecall *parent;
      
      parent = (AgsRecall *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(recall->parent == parent){
	g_rec_mutex_unlock(recall_mutex);
	
	return;
      }

      if(recall->parent != NULL){
	g_object_unref(recall->parent);
      }

      if(parent != NULL){
	g_object_ref(parent);
      }
      
      recall->parent = parent;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_CHILD_TYPE:
    {
      g_rec_mutex_lock(recall_mutex);

      recall->child_type = g_value_get_gtype(value);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_CHILD:
    {
      AgsRecall *child;
      
      child = (AgsRecall *) g_value_get_pointer(value);

      g_rec_mutex_lock(recall_mutex);

      if(!AGS_IS_RECALL(child) ||
	 g_list_find(recall->children, child) != NULL){
	g_rec_mutex_unlock(recall_mutex);
	
	return;
      }

      g_rec_mutex_unlock(recall_mutex);
      
      ags_recall_add_child(recall, child);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsRecall *recall;

  GRecMutex *recall_mutex;

  recall = AGS_RECALL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_rec_mutex_lock(recall_mutex);
      
      g_value_set_string(value, recall->filename);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_EFFECT:
    {
      g_rec_mutex_lock(recall_mutex);
      
      g_value_set_string(value, recall->effect);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_EFFECT_INDEX:
    {
      g_rec_mutex_lock(recall_mutex);
      
      g_value_set_uint(value, recall->effect_index);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RECALL_CONTAINER:
    {
      g_rec_mutex_lock(recall_mutex);
      
      g_value_set_object(value, recall->recall_container);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_OUTPUT_SOUNDCARD:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, recall->output_soundcard);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_OUTPUT_SOUNDCARD_CHANNEL:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_int(value, recall->output_soundcard_channel);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_INPUT_SOUNDCARD:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value,
			 recall->input_soundcard);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_INPUT_SOUNDCARD_CHANNEL:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_int(value, recall->input_soundcard_channel);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_uint(value, recall->samplerate);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_uint(value, recall->buffer_size);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_uint(value, recall->format);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_PAD:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_uint(value, recall->pad);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_uint(value, recall->audio_channel);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_LINE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_uint(value, recall->line);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_PORT:
    {
      g_rec_mutex_lock(recall_mutex);
      
      g_value_set_pointer(value, g_list_copy_deep(recall->port,
						  (GCopyFunc) g_object_ref,
						  NULL));

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_AUTOMATION_PORT:
    {
      g_rec_mutex_lock(recall_mutex);
      
      g_value_set_pointer(value, g_list_copy_deep(recall->automation_port,
						  (GCopyFunc) g_object_ref,
						  NULL));

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RECALL_ID:
    {
      g_rec_mutex_lock(recall_mutex);
      
      g_value_set_object(value, recall->recall_id);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RECALL_DEPENDENCY:
    {
      g_rec_mutex_lock(recall_mutex);
      
      g_value_set_pointer(value, g_list_copy_deep(recall->recall_dependency,
						  (GCopyFunc) g_object_ref,
						  NULL));

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_PARENT:
    {
      g_rec_mutex_lock(recall_mutex);
      
      g_value_set_object(value, recall->parent);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_CHILD:
    {
      g_rec_mutex_lock(recall_mutex);
      
      g_value_set_pointer(value, g_list_copy_deep(recall->children,
						  (GCopyFunc) g_object_ref,
						  NULL));

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_CHILD_TYPE:
    {
      g_rec_mutex_lock(recall_mutex);
      
      g_value_set_gtype(value,
			recall->child_type);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_dispose(GObject *gobject)
{
  AgsRecall *recall;

  GList *start_list, *list;

  recall = AGS_RECALL(gobject);

  ags_connectable_disconnect(AGS_CONNECTABLE(recall));
  
  /* recall container */
  if(recall->recall_container != NULL){
    ags_recall_container_remove(recall->recall_container,
				recall);
  }

  /* soundcard */
  if(recall->output_soundcard != NULL){    
    gpointer tmp;

    tmp = recall->output_soundcard;

    recall->output_soundcard = NULL;

    g_object_unref(tmp);
  }

  if(recall->input_soundcard != NULL){    
    gpointer tmp;

    tmp = recall->input_soundcard;

    recall->input_soundcard = NULL;

    g_object_unref(tmp);
  }
  
  /* port */
  if(recall->port != NULL){
    start_list = recall->port;

    recall->port = NULL;

    g_list_free_full(start_list,
		     (GDestroyNotify) g_object_unref);
  }
  
  /* automation port */
  if(recall->automation_port != NULL){
    start_list = recall->automation_port;

    recall->automation_port = NULL;

    g_list_free_full(start_list,
		     g_object_unref);
  }

  /* recall id */
  if(recall->recall_id != NULL){
    g_object_unref(recall->recall_id);

    recall->recall_id = NULL;
  }

  /* recall dependency */
  if(recall->recall_dependency != NULL){
    g_list_free_full(recall->recall_dependency,
		     g_object_unref);

    recall->recall_dependency = NULL;
  }
  
  /* parent */
  if(recall->parent != NULL){
    ags_recall_remove_child(recall->parent,
			    recall);

    recall->parent = NULL;
  }
    
  /* children */
  if(recall->children != NULL){
    list =
      start_list = recall->children;

    recall->children = NULL;

    while(list != NULL){      
      g_object_run_dispose(G_OBJECT(list->data));

      list = list->next;
    }

    g_list_free_full(start_list,
		     g_object_unref);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_parent_class)->dispose(gobject);
}

void
ags_recall_finalize(GObject *gobject)
{
  AgsRecall *recall;

  GList *start_list, *list;
  
  guint i;
  
  recall = AGS_RECALL(gobject);

#ifdef AGS_DEBUG
  g_message("finalize %s", G_OBJECT_TYPE_NAME(gobject));
#endif

  ags_uuid_free(recall->uuid);
  
  //TODO:JK: check removal
#if 0
  guint *ids;
  guint i, n_ids;
  
  ids = g_signal_list_ids(AGS_TYPE_RECALL,
			  &n_ids);
  
  for(i = 0; i < n_ids; i++){
    g_signal_handlers_disconnect_matched(gobject,
					 G_SIGNAL_MATCH_ID,
					 ids[i],
					 0,
					 NULL,
					 NULL,
					 NULL);
  }

  g_free(ids);
#endif

  g_free(recall->filename);
  g_free(recall->effect);
  
  /* recall container */
  if(recall->recall_container != NULL){
    ags_recall_container_remove(recall->recall_container,
				recall);
  }

  /* soundcard */
  if(recall->output_soundcard != NULL){    
    gpointer tmp;

    tmp = recall->output_soundcard;

    recall->output_soundcard = NULL;

    g_object_unref(tmp);
  }

  if(recall->input_soundcard != NULL){    
    gpointer tmp;

    tmp = recall->input_soundcard;

    recall->input_soundcard = NULL;

    g_object_unref(tmp);
  }
  
  /* port */
  if(recall->port != NULL){
    start_list = recall->port;

    recall->port = NULL;

    g_list_free_full(start_list,
		     (GDestroyNotify) g_object_unref);
  }
  
  /* automation port */
  if(recall->automation_port != NULL){
    start_list = recall->automation_port;

    recall->automation_port = NULL;

    g_list_free_full(start_list,
		     g_object_unref);
  }

  /* recall id */
  if(recall->recall_id != NULL){
    g_object_unref(recall->recall_id);

    recall->recall_id = NULL;
  }

  /* recall dependency */
  if(recall->recall_dependency != NULL){
    g_list_free_full(recall->recall_dependency,
		     g_object_unref);

    recall->recall_dependency = NULL;
  }
  
  /* parent */
  if(recall->parent != NULL){
    ags_recall_remove_child(recall->parent,
			    recall);

    recall->parent = NULL;
  }

  /* recall handler */
  g_list_free_full(recall->recall_handler,
		   (GDestroyNotify) ags_recall_handler_free);
    
  /* children */
  if(recall->child_parameter_name != NULL){
    g_strfreev(recall->child_parameter_name);

    recall->child_parameter_name = NULL;
  }

  if(recall->child_value != NULL){
    for(i = 0; i < recall->n_child_params; i++){
      g_value_unset(&(recall->child_value[i]));
    }
    
    g_free(recall->child_value);

    recall->child_value = NULL;
  }

  if(recall->children != NULL){
    list =
      start_list = recall->children;

    recall->children = NULL;

    while(list != NULL){      
      g_object_run_dispose(G_OBJECT(list->data));

      list = list->next;
    }

    g_list_free_full(start_list,
		     g_object_unref);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_recall_parent_class)->finalize(gobject);
}

AgsUUID*
ags_recall_get_uuid(AgsConnectable *connectable)
{
  AgsRecall *recall;
  
  AgsUUID *ptr;

  GRecMutex *recall_mutex;

  recall = AGS_RECALL(connectable);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* get UUID */
  g_rec_mutex_lock(recall_mutex);

  ptr = recall->uuid;

  g_rec_mutex_unlock(recall_mutex);
  
  return(ptr);
}

gboolean
ags_recall_has_resource(AgsConnectable *connectable)
{
  return(TRUE);
}

gboolean
ags_recall_is_ready(AgsConnectable *connectable)
{
  AgsRecall *recall;
  
  gboolean is_ready;
  
  GRecMutex *recall_mutex;

  recall = AGS_RECALL(connectable);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* check is ready */
  g_rec_mutex_lock(recall_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (recall->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(recall_mutex);
  
  return(is_ready);
}

void
ags_recall_add_to_registry(AgsConnectable *connectable)
{
  AgsRecall *recall;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  GList *list;

  GRecMutex *recall_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }

  application_context = ags_application_context_get_instance();
  
  recall = AGS_RECALL(connectable);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  g_rec_mutex_lock(recall_mutex);

  recall->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;

  g_rec_mutex_unlock(recall_mutex);

  registry = (AgsRegistry *) ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(entry->entry,
		       (gpointer) recall);
    ags_registry_add_entry(registry,
			   entry);
  }

  //TODO:JK: implement me
}

void
ags_recall_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_recall_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_recall_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_recall_xml_parse(AgsConnectable *connectable,
		     xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_recall_is_connected(AgsConnectable *connectable)
{
  AgsRecall *recall;
  
  gboolean is_connected;
  
  GRecMutex *recall_mutex;

  recall = AGS_RECALL(connectable);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* check is connected */
  g_rec_mutex_lock(recall_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (recall->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(recall_mutex);
  
  return(is_connected);
}

void
ags_recall_connect(AgsConnectable *connectable)
{
  AgsRecall *recall;

  GList *list_start, *list, *next;

  gboolean children_lock_free;
  
  GRecMutex *recall_mutex;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  recall = AGS_RECALL(connectable);
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  g_rec_mutex_lock(recall_mutex);

  recall->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  g_rec_mutex_unlock(recall_mutex);

  children_lock_free = ags_recall_global_get_children_lock_free();

  /* connect children */
  if(!children_lock_free){
    g_rec_mutex_lock(recall_mutex);

    list =
      list_start = g_list_copy(recall->children);

    g_rec_mutex_unlock(recall_mutex);
  }else{
    list =
      list_start = recall->children;
  }
  
  while(list != NULL){
    next = list->next;
    
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = next;
  }

  if(!children_lock_free){
    g_list_free(list_start);
  }
  
  /* recall handler */
  g_rec_mutex_lock(recall_mutex);

  list =
    list_start = g_list_copy(recall->recall_handler);

  g_rec_mutex_unlock(recall_mutex);

  while(list != NULL){
    AgsRecallHandler *recall_handler;
    
    recall_handler = AGS_RECALL_HANDLER(list->data);
    g_signal_connect_after(G_OBJECT(recall), recall_handler->signal_name,
			   G_CALLBACK(recall_handler->callback), recall_handler->data);
    
    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_recall_disconnect(AgsConnectable *connectable)
{
  AgsRecall *recall;

  GList *list_start, *list, *next;

  gboolean children_lock_free;
  
  GRecMutex *recall_mutex;

  recall = AGS_RECALL(connectable);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  g_rec_mutex_lock(recall_mutex);

  recall->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  g_rec_mutex_unlock(recall_mutex);

  children_lock_free = ags_recall_global_get_children_lock_free();

  /* connect children */
  if(!children_lock_free){
    g_rec_mutex_lock(recall_mutex);

    list =
      list_start = g_list_copy(recall->children);

    g_rec_mutex_unlock(recall_mutex);
  }else{
    list = 
      list_start = recall->children;
  }
  
  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  if(!children_lock_free){
    g_list_free(list_start);
  }
  
  /* recall handler */
  g_rec_mutex_lock(recall_mutex);

  list =
    list_start = g_list_copy(recall->recall_handler);

  g_rec_mutex_unlock(recall_mutex);

  while(list != NULL){
    AgsRecallHandler *recall_handler;

    gchar *signal_name;

    next = list->next;
    
    recall_handler = AGS_RECALL_HANDLER(list->data);

    signal_name = g_strdup_printf("any_signal::%s",
				  recall_handler->signal_name);
    g_object_disconnect(G_OBJECT(recall),
			signal_name,
			G_CALLBACK(recall_handler->callback),
			recall_handler->data,
			NULL);

    g_free(signal_name);

    list = next;
  }

  g_list_free(list_start);
}

/**
 * ags_recall_global_set_omit_event:
 * @omit_event: %TRUE if omit event, otherwise %FALSE
 * 
 * Set global config value omit event.
 * 
 * Since: 3.0.0
 */
void
ags_recall_global_set_omit_event(gboolean omit_event)
{
  ags_recall_global_omit_event = omit_event;
}

/**
 * ags_recall_global_get_children_lock_free:
 * 
 * Get global config value lock free children.
 *
 * Returns: if %TRUE does lock free children, else not
 * 
 * Since: 3.0.0
 */
gboolean
ags_recall_global_get_children_lock_free()
{
  gboolean children_lock_free;

  children_lock_free = ags_recall_global_children_lock_free;
  
  return(children_lock_free);
}

/**
 * ags_recall_global_get_omit_event:
 * 
 * Get global config value omit event.
 *
 * Returns: if %TRUE does omit events, else not
 * 
 * Since: 3.0.0
 */
gboolean
ags_recall_global_get_omit_event()
{
  gboolean omit_event;

  omit_event = ags_recall_global_omit_event;
  
  return(omit_event);
}

/**
 * ags_recall_global_get_performance_mode:
 * 
 * Get global config value performance mode.
 *
 * Returns: if %TRUE does performance mode, else not
 * 
 * Since: 3.0.0
 */
gboolean
ags_recall_global_get_performance_mode()
{
  gboolean performance_mode;

  performance_mode = ags_recall_global_performance_mode;
  
  return(performance_mode);
}

/**
 * ags_recall_global_get_rt_safe:
 * 
 * Get global config value rt-safe.
 *
 * Returns: if %TRUE does rt-safe strategy, else not
 * 
 * Since: 3.0.0
 */
gboolean
ags_recall_global_get_rt_safe()
{
  gboolean rt_safe;

  rt_safe = ags_recall_global_rt_safe;
  
  return(rt_safe);
}

/**
 * ags_recall_get_obj_mutex:
 * @recall: the #AgsRecall
 * 
 * Get object mutex.
 * 
 * Returns: (type gpointer) (transfer none): the #GRecMutex to lock @recall
 * 
 * Since: 3.1.0
 */
GRecMutex*
ags_recall_get_obj_mutex(AgsRecall *recall)
{
  if(!AGS_IS_RECALL(recall)){
    return(NULL);
  }

  return(AGS_RECALL_GET_OBJ_MUTEX(recall));
}

/**
 * ags_recall_test_flags:
 * @recall: the #AgsRecall
 * @flags: the flags
 *
 * Test @flags to be set on @recall.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_recall_test_flags(AgsRecall *recall, AgsRecallFlags flags)
{
  gboolean retval;  
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return(FALSE);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* test */
  g_rec_mutex_lock(recall_mutex);

  retval = ((flags & (recall->flags)) != 0) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(recall_mutex);

  return(retval);
}

/**
 * ags_recall_set_flags:
 * @recall: the #AgsRecall
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 3.0.0
 */
void
ags_recall_set_flags(AgsRecall *recall, AgsRecallFlags flags)
{
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* set flags */
  g_rec_mutex_lock(recall_mutex);

  recall->flags |= flags;

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_unset_flags:
 * @recall: the #AgsRecall
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 3.0.0
 */
void
ags_recall_unset_flags(AgsRecall *recall, AgsRecallFlags flags)
{
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* set flags */
  g_rec_mutex_lock(recall_mutex);

  recall->flags &= (~flags);

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_test_ability_flags:
 * @recall: the #AgsRecall
 * @ability_flags: the ability flags
 *
 * Test @ability_flags to be set on @recall.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_recall_test_ability_flags(AgsRecall *recall, AgsSoundAbilityFlags ability_flags)
{
  gboolean retval;  
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return(FALSE);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* test */
  g_rec_mutex_lock(recall_mutex);

  retval = ((ability_flags & (recall->ability_flags)) != 0) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(recall_mutex);

  return(retval);
}

/**
 * ags_recall_set_ability_flags:
 * @recall: the #AgsRecall
 * @ability_flags: ability flags
 *
 * Set ability flags recursively.
 * 
 * Since: 3.0.0
 */
void
ags_recall_set_ability_flags(AgsRecall *recall, AgsSoundAbilityFlags ability_flags)
{
  GList *child_start, *child, *next;

  gboolean children_lock_free;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  children_lock_free = ags_recall_global_get_children_lock_free();

  /* set ability flags */
  g_rec_mutex_lock(recall_mutex);

  recall->ability_flags |= ability_flags;

  /* apply recursivly */
  if(!children_lock_free){
    child =
      child_start = g_list_copy(recall->children);
  }else{
    child =
      child_start = recall->children;
  }
  
  g_rec_mutex_unlock(recall_mutex);

  while(child != NULL){
    next = child->next;
    
    ags_recall_set_ability_flags(AGS_RECALL(child->data), ability_flags);

    child = next;
  }

  if(!children_lock_free){
    g_list_free(child_start);
  }
}

/**
 * ags_recall_unset_ability_flags:
 * @recall: the #AgsRecall
 * @ability_flags: ability flags
 *
 * Unset ability flags recursively.
 * 
 * Since: 3.0.0
 */
void
ags_recall_unset_ability_flags(AgsRecall *recall, AgsSoundAbilityFlags ability_flags)
{
  GList *child_start, *child, *next;

  gboolean children_lock_free;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  children_lock_free = ags_recall_global_get_children_lock_free();

  /* unset ability flags */
  g_rec_mutex_lock(recall_mutex);

  recall->ability_flags &= (~ability_flags);

  /* apply recursivly */
  if(!children_lock_free){
    child =
      child_start = g_list_copy(recall->children);
  }else{
    child = 
      child_start = recall->children;
  }
  
  g_rec_mutex_unlock(recall_mutex);

  while(child != NULL){
    next = child->next;
    
    ags_recall_set_ability_flags(AGS_RECALL(child->data), ability_flags);

    child = next;
  }

  if(!children_lock_free){
    g_list_free(child_start);
  }
}

/**
 * ags_recall_check_ability_flags:
 * @recall: the #AgsRecall
 * @ability_flags: the ability flags
 * 
 * Check if @ability_flags is set for @recall.
 * 
 * Returns: %TRUE flags are set, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_recall_check_ability_flags(AgsRecall *recall, AgsSoundAbilityFlags ability_flags)
{
  AgsSoundAbilityFlags recall_ability_flags;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return(FALSE);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* get ability flags */
  g_rec_mutex_lock(recall_mutex);

  recall_ability_flags = recall->ability_flags;

  g_rec_mutex_unlock(recall_mutex);

  if((AGS_SOUND_ABILITY_PLAYBACK & (ability_flags)) != 0 &&
     (AGS_SOUND_ABILITY_PLAYBACK & (recall_ability_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_ABILITY_NOTATION & (ability_flags)) != 0 &&
     (AGS_SOUND_ABILITY_NOTATION & (recall_ability_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_ABILITY_SEQUENCER & (ability_flags)) != 0 &&
     (AGS_SOUND_ABILITY_SEQUENCER & (recall_ability_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_ABILITY_WAVE & (ability_flags)) != 0 &&
     (AGS_SOUND_ABILITY_WAVE & (recall_ability_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_ABILITY_MIDI & (ability_flags)) != 0 &&
     (AGS_SOUND_ABILITY_MIDI & (recall_ability_flags)) == 0){
    return(FALSE);
  }

  return(TRUE);
}

/**
 * ags_recall_match_ability_flags_to_scope:
 * @recall: the #AgsRecall
 * @sound_scope: the sound scope
 * 
 * Check if @sound_scope related ability flag is set.
 * 
 * Returns: %TRUE if sound scope is available, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_recall_match_ability_flags_to_scope(AgsRecall *recall, gint sound_scope)
{
  AgsSoundAbilityFlags recall_ability_flags;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return(FALSE);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* get ability flags */
  g_rec_mutex_lock(recall_mutex);

  recall_ability_flags = recall->ability_flags;

  g_rec_mutex_unlock(recall_mutex);

  switch(sound_scope){
  case AGS_SOUND_SCOPE_PLAYBACK:
    {
      if((AGS_SOUND_ABILITY_PLAYBACK & (recall_ability_flags)) != 0){
	return(TRUE);
      }else{
	return(FALSE);
      }
    }
  case AGS_SOUND_SCOPE_NOTATION:
    {
      if((AGS_SOUND_ABILITY_NOTATION & (recall_ability_flags)) != 0){
	return(TRUE);
      }else{
	return(FALSE);
      }
    }
  case AGS_SOUND_SCOPE_SEQUENCER:
    {
      if((AGS_SOUND_ABILITY_SEQUENCER & (recall_ability_flags)) != 0){
	return(TRUE);
      }else{
	return(FALSE);
      }
    }
  case AGS_SOUND_SCOPE_WAVE:
    {
      if((AGS_SOUND_ABILITY_WAVE & (recall_ability_flags)) != 0){
	return(TRUE);
      }else{
	return(FALSE);
      }
    }
  case AGS_SOUND_SCOPE_MIDI:
    {
      if((AGS_SOUND_ABILITY_MIDI & (recall_ability_flags)) != 0){
	return(TRUE);
      }else{
	return(FALSE);
      }
    }
  default:
    return(FALSE);
  }
}

/**
 * ags_recall_test_behaviour_flags:
 * @recall: the #AgsRecall
 * @behaviour_flags: the behaviour flags
 *
 * Test @behaviour_flags to be set on @recall.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_recall_test_behaviour_flags(AgsRecall *recall, AgsSoundBehaviourFlags behaviour_flags)
{
  gboolean retval;  
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return(FALSE);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* test */
  g_rec_mutex_lock(recall_mutex);

  retval = ((behaviour_flags & (recall->behaviour_flags)) != 0) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(recall_mutex);
  
  return(retval);
}

/**
 * ags_recall_set_behaviour_flags:
 * @recall: the #AgsRecall
 * @behaviour_flags: the behaviour flags
 * 
 * Set behaviour flags of @recall.
 * 
 * Since: 3.0.0
 */
void
ags_recall_set_behaviour_flags(AgsRecall *recall, AgsSoundBehaviourFlags behaviour_flags)
{
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* set behaviour flags */
  g_rec_mutex_lock(recall_mutex);

  recall->behaviour_flags |= behaviour_flags;

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_unset_behaviour_flags:
 * @recall: the #AgsRecall
 * @behaviour_flags: the behaviour flags
 * 
 * Unset behaviour flags of @recall.
 * 
 * Since: 3.0.0
 */
void
ags_recall_unset_behaviour_flags(AgsRecall *recall, AgsSoundBehaviourFlags behaviour_flags)
{
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* unset behaviour flags */
  g_rec_mutex_lock(recall_mutex);

  recall->behaviour_flags &= (~behaviour_flags);

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_check_behaviour_flags:
 * @recall: the #AgsRecall
 * @behaviour_flags: the behaviour flags
 * 
 * Check if @behaviour_flags is set for @recall.
 * 
 * Returns: %TRUE flags are set, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_recall_check_behaviour_flags(AgsRecall *recall, AgsSoundBehaviourFlags behaviour_flags)
{
  AgsSoundBehaviourFlags recall_behaviour_flags;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return(FALSE);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* get behaviour flags */
  g_rec_mutex_lock(recall_mutex);

  recall_behaviour_flags = recall->behaviour_flags;

  g_rec_mutex_unlock(recall_mutex);

  if((AGS_SOUND_BEHAVIOUR_PATTERN_MODE & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_PATTERN_MODE & (recall_behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_BULK_MODE & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_BULK_MODE & (recall_behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING & (recall_behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_OUTPUT & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_OUTPUT & (recall_behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT & (recall_behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT & (recall_behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT & (recall_behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_PERSISTENT & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_PERSISTENT & (recall_behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_PERSISTENT_PLAYBACK & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_PERSISTENT_PLAYBACK & (recall_behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_PERSISTENT_NOTATION & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_PERSISTENT_NOTATION & (recall_behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_PERSISTENT_SEQUENCER & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_PERSISTENT_SEQUENCER & (recall_behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_PERSISTENT_WAVE & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_PERSISTENT_WAVE & (recall_behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_PERSISTENT_MIDI & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_PERSISTENT_MIDI & (recall_behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_PROPAGATE_DONE & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_PROPAGATE_DONE & (recall_behaviour_flags)) == 0){
    return(FALSE);
  }

  return(TRUE);
}

/**
 * ags_recall_set_sound_scope:
 * @recall: the #AgsRecall
 * @sound_scope: the sound scope
 * 
 * Set @sound_scope for @recall.
 * 
 * Since: 3.0.0
 */
void
ags_recall_set_sound_scope(AgsRecall *recall, gint sound_scope)
{
  GList *start_child, *child, *next;

  gboolean children_lock_free;  
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall) &&
     ags_recall_check_sound_scope(recall,
				  -1)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  children_lock_free = ags_recall_global_get_children_lock_free();

  /* set sound scope */
  g_rec_mutex_lock(recall_mutex);

  recall->sound_scope = sound_scope;

  /* apply recursivly */
  if(!children_lock_free){
    child =
      start_child = g_list_copy(recall->children);
  }else{
    child =
      start_child = recall->children;
  }

  g_rec_mutex_unlock(recall_mutex);

  while(child != NULL){
    next = child->next;
    
    ags_recall_set_sound_scope(AGS_RECALL(child->data), sound_scope);

    child = next;
  }

  if(!children_lock_free){
    g_list_free(start_child);
  }  
}

/**
 * ags_recall_get_sound_scope:
 * @recall: the #AgsRecall
 * 
 * Get sound scope for @recall.
 * 
 * Returns: the used sound scope
 * 
 * Since: 3.0.0
 */
gint
ags_recall_get_sound_scope(AgsRecall *recall)
{
  gint sound_scope;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return(-1);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* set sound scope */
  g_rec_mutex_lock(recall_mutex);

  sound_scope = recall->sound_scope;

  g_rec_mutex_unlock(recall_mutex);
  
  return(sound_scope);
}

/**
 * ags_recall_check_sound_scope:
 * @recall: the #AgsRecall
 * @sound_scope: the sound scope to check or -1 to check all
 * 
 * Check if @sound_scope is set for @recall.
 * 
 * Returns: %TRUE if sound scope matches, otherwise  %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_recall_check_sound_scope(AgsRecall *recall, gint sound_scope)
{
  gint recall_sound_scope;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return(FALSE);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* get sound scope */
  g_rec_mutex_lock(recall_mutex);

  recall_sound_scope = recall->sound_scope;

  g_rec_mutex_unlock(recall_mutex);

  if(sound_scope < 0){
    switch(recall_sound_scope){
    case AGS_SOUND_SCOPE_PLAYBACK:
    case AGS_SOUND_SCOPE_NOTATION:
    case AGS_SOUND_SCOPE_SEQUENCER:
    case AGS_SOUND_SCOPE_WAVE:
    case AGS_SOUND_SCOPE_MIDI:
      return(TRUE);      
    default:
      return(FALSE);
    }
  }else{
    if(sound_scope < AGS_SOUND_SCOPE_LAST &&
       sound_scope == recall_sound_scope){
      return(TRUE);
    }else{
      return(FALSE);
    }
  }
}

/**
 * ags_recall_test_staging_flags:
 * @recall: the #AgsRecall
 * @staging_flags: the staging flags
 *
 * Test @staging_flags to be set on @recall.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_recall_test_staging_flags(AgsRecall *recall,
			      AgsSoundStagingFlags staging_flags)
{
  gboolean retval;  
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return(FALSE);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* test */
  g_rec_mutex_lock(recall_mutex);

  retval = ((staging_flags & (recall->staging_flags)) != 0) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(recall_mutex);

  return(retval);
}

/**
 * ags_recall_set_staging_flags:
 * @recall: the #AgsRecall
 * @staging_flags: staging flags to set
 * 
 * Set staging flags.
 * 
 * Since: 3.0.0
 */
void
ags_recall_set_staging_flags(AgsRecall *recall, AgsSoundStagingFlags staging_flags)
{
  AgsSoundStagingFlags recall_staging_flags;
  AgsSoundStateFlags recall_state_flags;
  
  gboolean omit_event;

  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  omit_event = ags_recall_global_get_omit_event();

  /* get staging flags */
  g_rec_mutex_lock(recall_mutex);

  recall_staging_flags = recall->staging_flags;
  recall_state_flags = recall->state_flags;
  
  g_rec_mutex_unlock(recall_mutex);
  
  /* invoke appropriate staging */
  if((AGS_SOUND_STAGING_FINI & (recall_staging_flags)) == 0 &&
     (AGS_SOUND_STATE_IS_TERMINATING & (recall_state_flags)) == 0){
    if((AGS_SOUND_STAGING_CHECK_RT_DATA & (staging_flags)) != 0 &&
       (AGS_SOUND_STAGING_CHECK_RT_DATA & (recall_staging_flags)) == 0){    
      if(omit_event){
	AGS_RECALL_GET_CLASS(recall)->check_rt_data(recall);
      }else{
	ags_recall_check_rt_data(recall);
      }
    }

    if((AGS_SOUND_STAGING_RUN_INIT_PRE & (staging_flags)) != 0 &&
       (AGS_SOUND_STAGING_RUN_INIT_PRE & (recall_staging_flags)) == 0){
      if(omit_event){
	AGS_RECALL_GET_CLASS(recall)->run_init_pre(recall);
      }else{
	ags_recall_run_init_pre(recall);
      }
    }

    if((AGS_SOUND_STAGING_RUN_INIT_INTER & (staging_flags)) != 0 &&
       (AGS_SOUND_STAGING_RUN_INIT_INTER & (recall_staging_flags)) == 0){
      if(omit_event){
	AGS_RECALL_GET_CLASS(recall)->run_init_inter(recall);
      }else{
	ags_recall_run_init_inter(recall);
      }
    }

    if((AGS_SOUND_STAGING_RUN_INIT_POST & (staging_flags)) != 0 &&
       (AGS_SOUND_STAGING_RUN_INIT_POST & (recall_staging_flags)) == 0){
      if(omit_event){
	AGS_RECALL_GET_CLASS(recall)->run_init_post(recall);
      }else{
	ags_recall_run_init_post(recall);
      }
    }
  
    if((AGS_SOUND_STAGING_FEED_INPUT_QUEUE & (staging_flags)) != 0 &&
       (AGS_SOUND_STAGING_FEED_INPUT_QUEUE & (recall_staging_flags)) == 0){
      if(omit_event){
	AGS_RECALL_GET_CLASS(recall)->feed_input_queue(recall);
      }else{
	ags_recall_feed_input_queue(recall);
      }
    }

    if((AGS_SOUND_STAGING_AUTOMATE & (staging_flags)) != 0 &&
       (AGS_SOUND_STAGING_AUTOMATE & (recall_staging_flags)) == 0){
      if(omit_event){
	AGS_RECALL_GET_CLASS(recall)->automate(recall);
      }else{
	ags_recall_automate(recall);
      }
    }

    if((AGS_SOUND_STAGING_RUN_PRE & (staging_flags)) != 0 &&
       (AGS_SOUND_STAGING_RUN_PRE & (recall_staging_flags)) == 0){
      if(omit_event){
	AGS_RECALL_GET_CLASS(recall)->run_pre(recall);
      }else{
	ags_recall_run_pre(recall);
      }
    }

    if((AGS_SOUND_STAGING_RUN_INTER & (staging_flags)) != 0 &&
       (AGS_SOUND_STAGING_RUN_INTER & (recall_staging_flags)) == 0){
      if(omit_event){
	AGS_RECALL_GET_CLASS(recall)->run_inter(recall);
      }else{
	ags_recall_run_inter(recall);
      }
    }

    if((AGS_SOUND_STAGING_RUN_POST & (staging_flags)) != 0 &&
       (AGS_SOUND_STAGING_RUN_POST & (recall_staging_flags)) == 0){
      if(omit_event){
	AGS_RECALL_GET_CLASS(recall)->run_post(recall);
      }else{
	ags_recall_run_post(recall);
      }
    }

    if((AGS_SOUND_STAGING_DO_FEEDBACK & (staging_flags)) != 0 &&
       (AGS_SOUND_STAGING_DO_FEEDBACK & (recall_staging_flags)) == 0){
      if(omit_event){
	AGS_RECALL_GET_CLASS(recall)->do_feedback(recall);
      }else{
	ags_recall_do_feedback(recall);
      }
    }

    if((AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE & (staging_flags)) != 0 &&
       (AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE & (recall_staging_flags)) == 0){
      if(omit_event){
	AGS_RECALL_GET_CLASS(recall)->feed_output_queue(recall);
      }else{
	ags_recall_feed_output_queue(recall);
      }
    }
  }

  if((AGS_SOUND_STAGING_FINI & (staging_flags)) != 0){
    ags_recall_unset_staging_flags(recall,
				   (AGS_SOUND_STAGING_FEED_INPUT_QUEUE |
				    AGS_SOUND_STAGING_AUTOMATE |
				    AGS_SOUND_STAGING_RUN_PRE |
				    AGS_SOUND_STAGING_RUN_INTER |
				    AGS_SOUND_STAGING_RUN_POST |
				    AGS_SOUND_STAGING_DO_FEEDBACK |
				    AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE));    
  }

  if((AGS_SOUND_STAGING_CANCEL & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_CANCEL & (recall_staging_flags)) == 0){
    ags_recall_set_state_flags(recall,
			       AGS_SOUND_STATE_IS_TERMINATING);

    ags_recall_cancel(recall);
  }
  
  if((AGS_SOUND_STAGING_DONE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_DONE & (recall_staging_flags)) == 0){
    ags_recall_done(recall);
  }

#if 0
  if((AGS_SOUND_STAGING_REMOVE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_REMOVE & (recall_staging_flags)) == 0){
    ags_recall_remove(recall);
  }
#endif
  
  /* apply flags */
  g_rec_mutex_lock(recall_mutex);

  recall->staging_flags |= staging_flags;

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_unset_staging_flags:
 * @recall: the #AgsRecall
 * @staging_flags: staging flags to unset
 * 
 * Unset staging flags.
 * 
 * Since: 3.0.0
 */
void
ags_recall_unset_staging_flags(AgsRecall *recall, AgsSoundStagingFlags staging_flags)
{
  GList *list_start, *list, *next;

  gboolean children_lock_free;  

  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  children_lock_free = ags_recall_global_get_children_lock_free();

  /* unset staging flags */
  g_rec_mutex_lock(recall_mutex);

  recall->staging_flags &= (~staging_flags);

  if(!children_lock_free){
    list =
      list_start = g_list_copy_deep(recall->children,
				    (GCopyFunc) g_object_ref,
				    NULL);
  }else{
    list = 
      list_start = recall->children;
  }

  g_rec_mutex_unlock(recall_mutex);

  while(list != NULL){
    next = list->next;
    
    ags_recall_unset_staging_flags(AGS_RECALL(list->data), staging_flags);

    list = next;
  }

  if(!children_lock_free){
    g_list_free_full(list_start,
		     g_object_unref);
  }
}

/**
 * ags_recall_check_staging_flags:
 * @recall: the #AgsRecall
 * @staging_flags: staging flags to check
 * 
 * Check the occurence of @staging_flags in @recall.
 * 
 * Returns: %TRUE if all flags matched, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_recall_check_staging_flags(AgsRecall *recall, AgsSoundStagingFlags staging_flags)
{
  AgsSoundStagingFlags recall_staging_flags;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return(FALSE);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* get staging flags */
  g_rec_mutex_lock(recall_mutex);

  recall_staging_flags = recall->staging_flags;

  g_rec_mutex_unlock(recall_mutex);

  /* check staging flags */
  if((AGS_SOUND_STAGING_CHECK_RT_DATA & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_CHECK_RT_DATA & (recall_staging_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STAGING_RUN_INIT_PRE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (recall_staging_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STAGING_RUN_INIT_INTER & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_INIT_INTER & (recall_staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_RUN_INIT_POST & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_INIT_POST & (recall_staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_FEED_INPUT_QUEUE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_FEED_INPUT_QUEUE & (recall_staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_AUTOMATE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_AUTOMATE & (recall_staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_RUN_PRE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_PRE & (recall_staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_RUN_INTER & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_INTER & (recall_staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_RUN_POST & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_POST & (recall_staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_DO_FEEDBACK & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_DO_FEEDBACK & (recall_staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE & (recall_staging_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STAGING_FINI & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_FINI & (recall_staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_CANCEL & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_CANCEL & (recall_staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_DONE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_DONE & (recall_staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_REMOVE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_REMOVE & (recall_staging_flags)) == 0){
    return(FALSE);
  }
  
  return(TRUE);
}

/**
 * ags_recall_test_state_flags:
 * @recall: the #AgsRecall
 * @state_flags: the state flags
 *
 * Test @state_flags to be set on @recall.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_recall_test_state_flags(AgsRecall *recall,
			    AgsSoundStateFlags state_flags)
{
  gboolean retval;  
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return(FALSE);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* test */
  g_rec_mutex_lock(recall_mutex);

  retval = ((state_flags & (recall->state_flags)) != 0) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(recall_mutex);

  return(retval);
}

/**
 * ags_recall_set_state_flags:
 * @recall: the #AgsRecall
 * @state_flags: state flags to set
 * 
 * Set state flags.
 * 
 * Since: 3.0.0
 */
void
ags_recall_set_state_flags(AgsRecall *recall, AgsSoundStateFlags state_flags)
{  
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* set state flags */
  g_rec_mutex_lock(recall_mutex);

  recall->state_flags |= state_flags;

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_unset_state_flags:
 * @recall: the #AgsRecall
 * @state_flags: state flags to unset
 * 
 * Unset state flags.
 * 
 * Since: 3.0.0
 */
void
ags_recall_unset_state_flags(AgsRecall *recall, AgsSoundStateFlags state_flags)
{
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* unset state flags */
  g_rec_mutex_lock(recall_mutex);

  recall->state_flags &= (~state_flags);

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_check_state_flags:
 * @recall: the #AgsRecall
 * @state_flags: state flags to check
 * 
 * Check the occurence of @state_flags in @recall.
 * 
 * Returns: %TRUE if all flags matched, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_recall_check_state_flags(AgsRecall *recall, AgsSoundStateFlags state_flags)
{
  AgsSoundStateFlags recall_state_flags;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return(FALSE);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* get state flags */
  g_rec_mutex_lock(recall_mutex);

  recall_state_flags = recall->state_flags;
  
  g_rec_mutex_unlock(recall_mutex);

  /* check state flags */
  if((AGS_SOUND_STATE_IS_WAITING & (state_flags)) != 0 &&
     (AGS_SOUND_STATE_IS_WAITING & (recall_state_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STATE_IS_ACTIVE & (state_flags)) != 0 &&
     (AGS_SOUND_STATE_IS_ACTIVE & (recall_state_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STATE_IS_PROCESSING & (state_flags)) != 0 &&
     (AGS_SOUND_STATE_IS_PROCESSING & (recall_state_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STATE_IS_TERMINATING & (state_flags)) != 0 &&
     (AGS_SOUND_STATE_IS_TERMINATING & (recall_state_flags)) == 0){
    return(FALSE);
  }
  
  return(TRUE);
}

/**
 * ags_recall_get_filename:
 * @recall: the #AgsRecall
 * 
 * Get filename.
 * 
 * Returns: the filename
 * 
 * Since: 3.1.0
 */
gchar*
ags_recall_get_filename(AgsRecall *recall)
{
  gchar *filename;

  if(!AGS_IS_RECALL(recall)){
    return(NULL);
  }

  g_object_get(recall,
	       "filename", &filename,
	       NULL);

  return(filename);
}

/**
 * ags_recall_set_filename:
 * @recall: the #AgsRecall
 * @filename: the filename
 * 
 * Set filename.
 * 
 * Since: 3.1.0
 */
void
ags_recall_set_filename(AgsRecall *recall,
			gchar *filename)
{
  if(!AGS_IS_RECALL(recall)){
    return;
  }

  g_object_set(recall,
	       "filename", filename,
	       NULL);
}

/**
 * ags_recall_get_effect:
 * @recall: the #AgsRecall
 * 
 * Get effect.
 * 
 * Returns: the effect
 * 
 * Since: 3.1.0
 */
gchar*
ags_recall_get_effect(AgsRecall *recall)
{
  gchar *effect;

  if(!AGS_IS_RECALL(recall)){
    return(NULL);
  }

  g_object_get(recall,
	       "effect", &effect,
	       NULL);

  return(effect);
}

/**
 * ags_recall_set_effect:
 * @recall: the #AgsRecall
 * @effect: the effect
 * 
 * Set effect.
 * 
 * Since: 3.1.0
 */
void
ags_recall_set_effect(AgsRecall *recall,
		      gchar *effect)
{
  if(!AGS_IS_RECALL(recall)){
    return;
  }

  g_object_set(recall,
	       "effect", effect,
	       NULL);
}

/**
 * ags_recall_get_effect_index:
 * @recall: the #AgsRecall
 * 
 * Get effect index.
 * 
 * Returns: the effect index
 * 
 * Since: 3.1.0
 */
guint
ags_recall_get_effect_index(AgsRecall *recall)
{
  guint effect_index;
  
  if(!AGS_IS_RECALL(recall)){
    return(0);
  }

  g_object_get(recall,
	       "effect-index", &effect_index,
	       NULL);

  return(effect_index);
}

/**
 * ags_recall_set_effect_index:
 * @recall: the #AgsRecall
 * @effect_index: the effect index
 * 
 * Set effect index.
 * 
 * Since: 3.1.0
 */
void
ags_recall_set_effect_index(AgsRecall *recall,
			    guint effect_index)
{
  if(!AGS_IS_RECALL(recall)){
    return;
  }

  g_object_set(recall,
	       "effect-index", effect_index,
	       NULL);
}

/**
 * ags_recall_get_recall_container:
 * @recall: the #AgsRecall
 * 
 * Get recall container of @recall.
 * 
 * Returns:  (transfer full): the #AgsRecallContainer
 * 
 * Since: 3.1.0
 */
GObject*
ags_recall_get_recall_container(AgsRecall *recall)
{
  GObject *recall_container;
  
  if(!AGS_IS_RECALL(recall)){
    return(NULL);
  }

  g_object_get(recall,
	       "recall-container", &recall_container,
	       NULL);

  return(recall_container);
}

/**
 * ags_recall_set_recall_container:
 * @recall: the #AgsRecall
 * @recall_container: the #AgsRecallContainer
 * 
 * Set @recall_container of @recall.
 * 
 * Since: 3.1.0
 */
void
ags_recall_set_recall_container(AgsRecall *recall,
				GObject *recall_container)
{
  if(!AGS_IS_RECALL(recall)){
    return;
  }

  g_object_set(recall,
	       "recall-container", recall_container,
	       NULL);
}

/**
 * ags_recall_get_recall_id:
 * @recall: the #AgsRecall
 * 
 * Get recall id of @recall.
 * 
 * Returns:  (transfer full): the #AgsRecallID
 * 
 * Since: 3.1.0
 */
AgsRecallID*
ags_recall_get_recall_id(AgsRecall *recall)
{
  AgsRecallID *recall_id;
  
  if(!AGS_IS_RECALL(recall)){
    return(NULL);
  }

  g_object_get(recall,
	       "recall-id", &recall_id,
	       NULL);

  return(recall_id);
}

/**
 * ags_recall_set_recall_id:
 * @recall: the #AgsRecall
 * @recall_id: the #AgsRecallID to set
 *
 * Set @recall_id of @recall and all its children.
 * 
 * Since: 3.0.0
 */
void
ags_recall_set_recall_id(AgsRecall *recall,
			 AgsRecallID *recall_id)
{
  GList *list_start, *list, *next;

  gboolean children_lock_free;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return;
  }
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  children_lock_free = ags_recall_global_get_children_lock_free();

  /* set recall id - children */
  g_rec_mutex_lock(recall_mutex);

  if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0){
    g_warning("set recall id on template");
  }

  if(!children_lock_free){
    list =
      list_start = g_list_copy(recall->children);
  }else{
    list =
      list_start = recall->children;
  }
  
  g_rec_mutex_unlock(recall_mutex);

  while(list != NULL){
    next = list->next;
    
    ags_recall_set_recall_id(AGS_RECALL(list->data), recall_id);

    list = next;
  }

  if(!children_lock_free){
    g_list_free(list_start);
  }
  
  /* set recall id */
  g_rec_mutex_lock(recall_mutex);

  recall->recall_id = recall_id;
  g_object_ref(recall_id);
  
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_get_recall_dependency:
 * @recall: the #AgsRecall
 * 
 * Get recall dependency.
 * 
 * Returns: (element-type AgsAudio.RecallDependency) (transfer full): the #GList-struct containig #AgsRecallDependency
 * 
 * Since: 3.1.0
 */
GList*
ags_recall_get_recall_dependency(AgsRecall *recall)
{
  GList *recall_dependency;

  if(!AGS_IS_RECALL(recall)){
    return(NULL);
  }

  g_object_get(recall,
	       "recall-dependency", &recall_dependency,
	       NULL);

  return(recall_dependency);
}

/**
 * ags_recall_set_recall_dependency:
 * @recall: the #AgsRecall
 * @recall_dependency: (element-type AgsAudio.RecallDependency) (transfer full): the #GList-struct containing #AgsRecallDependency
 * 
 * Set recall dependency by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_recall_set_recall_dependency(AgsRecall *recall,
				 GList *recall_dependency)
{
  GList *start_recall_dependency;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);
    
  g_rec_mutex_lock(recall_mutex);

  start_recall_dependency = recall->recall_dependency;
  recall->recall_dependency = recall_dependency;
  
  g_rec_mutex_unlock(recall_mutex);

  g_list_free_full(start_recall_dependency,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_recall_add_recall_dependency:
 * @recall: the #AgsRecall
 * @recall_dependency: the #AgsRecallDependency
 *
 * Associate a new dependency for this recall.
 * 
 * Since: 3.0.0
 */
void
ags_recall_add_recall_dependency(AgsRecall *recall, AgsRecallDependency *recall_dependency)
{
  GRecMutex *recall_mutex;
  
  if(!AGS_IS_RECALL(recall) ||
     !AGS_IS_RECALL_DEPENDENCY(recall_dependency)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* add recall dependency */
  g_rec_mutex_lock(recall_mutex);
  
  g_object_ref(recall_dependency);  
  recall->recall_dependency = g_list_prepend(recall->recall_dependency,
					     recall_dependency);

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_remove_recall_dependency:
 * @recall: the #AgsRecall
 * @recall_dependency: the #AgsRecallDependency
 *
 * Remove a prior associated dependency.
 * 
 * Since: 3.0.0
 */
void
ags_recall_remove_recall_dependency(AgsRecall *recall, AgsRecallDependency *recall_dependency)
{
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall) ||
     !AGS_IS_RECALL_DEPENDENCY(recall_dependency)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* remove recall dependency */
  g_rec_mutex_lock(recall_mutex);

  if(g_list_find(recall->recall_dependency,
		 recall_dependency) != NULL){
    recall->recall_dependency = g_list_remove(recall->recall_dependency,
					      recall_dependency);
    g_object_unref(G_OBJECT(recall_dependency));
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_get_port:
 * @recall: the #AgsRecall
 * 
 * Get port.
 * 
 * Returns: (element-type AgsAudio.Port) (transfer full): the #GList-struct containig #AgsPort
 * 
 * Since: 3.7.18
 */
GList*
ags_recall_get_port(AgsRecall *recall)
{
  GList *port;

  if(!AGS_IS_RECALL(recall)){
    return(NULL);
  }

  g_object_get(recall,
	       "port", &port,
	       NULL);

  return(port);
}

/**
 * ags_recall_set_port:
 * @recall: the #AgsRecall
 * @port: (element-type AgsAudio.Port) (transfer full): the #GList-struct containing #AgsPort
 * 
 * Set port by replacing existing.
 * 
 * Since: 3.7.18
 */
void
ags_recall_set_port(AgsRecall *recall,
		    GList *port)
{
  GList *start_port;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);
    
  g_rec_mutex_lock(recall_mutex);

  start_port = recall->port;
  recall->port = port;
  
  g_rec_mutex_unlock(recall_mutex);

  g_list_free_full(start_port,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_recall_add_port:
 * @recall: the #AgsRecall
 * @port: the #AgsPort
 * 
 * Add @port to @recall.
 * 
 * Since: 3.3.0
 */
void
ags_recall_add_port(AgsRecall *recall,
		    AgsPort *port)
{
  GRecMutex *recall_mutex;
  
  if(!AGS_IS_RECALL(recall) ||
     !AGS_IS_PORT(port)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* add port */
  g_rec_mutex_lock(recall_mutex);

  if(g_list_find(recall->port, port) == NULL){
    g_object_ref(port);
    recall->port = g_list_prepend(recall->port,
				  port);
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_remove_port:
 * @recall: the #AgsRecall
 * @port: the #AgsPort
 * 
 * Remove @port from @recall.
 * 
 * Since: 3.3.0
 */
void
ags_recall_remove_port(AgsRecall *recall,
		       AgsPort *port)
{
  GRecMutex *recall_mutex;
  
  if(!AGS_IS_RECALL(recall) ||
     !AGS_IS_PORT(port)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* remove port */
  g_rec_mutex_lock(recall_mutex);

  if(g_list_find(recall->port, port) != NULL){
    g_object_unref(port);
    recall->port = g_list_remove(recall->port,
				 port);
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_get_children:
 * @recall: the #AgsRecall
 * 
 * Get recall children.
 * 
 * Returns: (element-type AgsAudio.Recall) (transfer full): the #GList-struct containig #AgsRecall
 * 
 * Since: 3.1.0
 */
GList*
ags_recall_get_children(AgsRecall *recall)
{
  GList *children;

  if(!AGS_IS_RECALL(recall)){
    return(NULL);
  }

  g_object_get(recall,
	       "child", &children,
	       NULL);

  return(children);
}

/**
 * ags_recall_set_children:
 * @recall: the #AgsRecall
 * @children: (element-type AgsAudio.Recall) (transfer full): the #GList-struct containing #AgsRecall
 * 
 * Set recall children by replacing existing.
 * 
 * Since: 3.1.0
 */
void
ags_recall_set_children(AgsRecall *recall,
			GList *children)
{
  GList *start_children;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);
    
  g_rec_mutex_lock(recall_mutex);

  start_children = recall->children;
  recall->children = children;
  
  g_rec_mutex_unlock(recall_mutex);

  g_list_free_full(start_children,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_recall_add_child:
 * @recall: the #AgsRecall
 * @child: the child #AgsRecall
 *
 * Add @child to @recall.
 * 
 * Since: 3.0.0
 */
void
ags_recall_add_child(AgsRecall *recall, AgsRecall *child)
{
  AgsRecall *old_parent;
  AgsRecallID *recall_id;

  GObject *output_soundcard;
  GObject *input_soundcard;

  gint output_soundcard_channel;
  gint input_soundcard_channel;
  guint samplerate;
  guint buffer_size;
  AgsSoundcardFormat format;
  AgsSoundAbilityFlags recall_ability_flags;
  AgsSoundBehaviourFlags recall_behaviour_flags;
  gint recall_sound_scope;
  AgsSoundStagingFlags staging_flags;
  
  GRecMutex *recall_mutex, *child_mutex;
  
  if(!AGS_IS_RECALL(child) ||
     !AGS_IS_RECALL(recall)){
    return;
  }

  /* get recall mutex - recall and child */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);
  child_mutex = AGS_RECALL_GET_OBJ_MUTEX(child);

  /* check if already set */
  g_rec_mutex_lock(child_mutex);

  if(child->parent == recall){
    g_rec_mutex_unlock(child_mutex);
    
    return;
  }

  old_parent = child->parent;
  
  g_rec_mutex_unlock(child_mutex);

  /*  */
  g_object_ref(recall);
  g_object_ref(child);

  /* remove old */
  if(old_parent != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(child));
    ags_recall_remove_child(old_parent,
			    child);
  }

  /* add child */
  g_rec_mutex_lock(recall_mutex);

  recall_ability_flags = recall->ability_flags;
  recall_behaviour_flags = recall->behaviour_flags;
  recall_sound_scope = recall->sound_scope;

  output_soundcard = recall->output_soundcard;
  output_soundcard_channel = recall->output_soundcard_channel;

  input_soundcard = recall->input_soundcard;
  input_soundcard_channel = recall->input_soundcard_channel;

  samplerate = recall->samplerate;
  buffer_size = recall->buffer_size;
  format = recall->format;
  
  recall->children = g_list_prepend(recall->children,
				    child);
  
  g_rec_mutex_unlock(recall_mutex);

  g_object_get(recall,
	       "recall-id", &recall_id,
	       NULL);
  
  /* ref new */
  ags_recall_set_ability_flags(child, recall_ability_flags);
  ags_recall_set_behaviour_flags(child, recall_behaviour_flags);
  ags_recall_set_sound_scope(child, recall_sound_scope);

  g_rec_mutex_lock(child_mutex);

  child->parent = recall;

  g_rec_mutex_unlock(child_mutex);

  g_object_set(G_OBJECT(child),
	       "output-soundcard", output_soundcard,
	       "output-soundcard-channel", output_soundcard_channel,
	       "input-soundcard", input_soundcard,
	       "input-soundcard-channel", input_soundcard_channel,
	       "samplerate", samplerate,
	       "buffer-size", buffer_size,
	       "format", format,
	       "recall-id", recall_id,
	       NULL);

  g_signal_connect_after(G_OBJECT(child), "done",
			 G_CALLBACK(ags_recall_child_done), recall);

  ags_recall_child_added(recall,
			 child);

  if(ags_connectable_is_connected(AGS_CONNECTABLE(recall))){
    ags_connectable_connect(AGS_CONNECTABLE(child));
  }
  
  /* get mask */
  if(recall_id != NULL){
    staging_flags = (AGS_SOUND_STAGING_CHECK_RT_DATA |
		     AGS_SOUND_STAGING_RUN_INIT_PRE |
		     AGS_SOUND_STAGING_RUN_INIT_INTER |
		     AGS_SOUND_STAGING_RUN_INIT_POST);

    //FIXME:JK: this doesn't work
#if 0  
    g_message("staging + 0x%x", staging_flags);
  
    g_rec_mutex_lock(recall_mutex);

    staging_flags = (staging_flags & (recall->staging_flags));
  
    g_rec_mutex_unlock(recall_mutex);

    g_message("staging - 0x%x", staging_flags);
#endif
  
    /* set staging flags */
    ags_recall_set_staging_flags(child,
				 staging_flags);

    g_object_unref(recall_id);
  }
}

/**
 * ags_recall_remove_child:
 * @recall: the #AgsRecall
 * @child: the child #AgsRecall
 *
 * Remove @child from @recall.
 * 
 * Since: 3.0.0
 */
void
ags_recall_remove_child(AgsRecall *recall, AgsRecall *child)
{
  GRecMutex *recall_mutex, *child_mutex;
  
  if(!AGS_IS_RECALL(child) ||
     !AGS_IS_RECALL(recall)){
    return;
  }

  /* get recall mutex - recall and child */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);
  child_mutex = AGS_RECALL_GET_OBJ_MUTEX(child);

  /* check if not set */
  g_rec_mutex_lock(child_mutex);

  if(child->parent != recall){
    g_rec_mutex_unlock(child_mutex);
    
    return;
  }

  g_rec_mutex_unlock(child_mutex);

  /* remove from recall */
  g_rec_mutex_lock(recall_mutex);

  if(g_list_find(recall->children,
		 child) != NULL){
    recall->children = g_list_remove(recall->children,
				     child);
    g_object_unref(child);
  }
    
  g_rec_mutex_unlock(recall_mutex);

  /* unref recall */
  child->parent = NULL;

  g_object_unref(recall);
}

/**
 * ags_recall_handler_free:
 * @recall_handler: (type gpointer) (transfer none): the #AgsRecallHandler-struct
 *
 * Free @recall_hanlder. 
 * 
 * Since: 3.0.0
 */
void
ags_recall_handler_free(AgsRecallHandler *recall_handler)
{
  if(recall_handler == NULL){
    return;
  }

  g_free(recall_handler->signal_name);

  free(recall_handler);
}

/**
 * ags_recall_handler_alloc:
 * @signal_name: signal's name to connect
 * @callback: (scope call): the #GCallback function
 * @data: the data to pass the callback
 *
 * Allocates #AgsRecallHandler-struct.
 * 
 * Returns: (type gpointer) (transfer none): the newly allocated #AgsRecallHandler-struct
 * 
 * Since: 3.0.0
 */
AgsRecallHandler*
ags_recall_handler_alloc(const gchar *signal_name,
			 GCallback callback,
			 GObject *data)
{
  AgsRecallHandler *recall_handler;

  recall_handler = (AgsRecallHandler *) malloc(sizeof(AgsRecallHandler));
  
  recall_handler->signal_name = g_strdup(signal_name);
  recall_handler->callback = callback;
  recall_handler->data = data;

  return(recall_handler);
}

/**
 * ags_recall_add_recall_handler:
 * @recall: the #AgsRecall to connect
 * @recall_handler: (type gpointer) (transfer none): the signal specs
 *
 * Connect callback to @recall specified by @recall_handler.
 * 
 * Since: 3.0.0
 */
void
ags_recall_add_recall_handler(AgsRecall *recall,
			      AgsRecallHandler *recall_handler)
{
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall) ||
     recall_handler == NULL){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* add handler */
  g_rec_mutex_lock(recall_mutex);

  recall->recall_handler = g_list_prepend(recall->recall_handler,
					  recall_handler);

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_remove_recall_handler:
 * @recall: the #AgsRecall to connect
 * @recall_handler: (type gpointer) (transfer none): the signal specs
 *
 * Remove a #AgsRecallHandler-struct from @recall.
 * 
 * Since: 3.0.0
 */
void
ags_recall_remove_recall_handler(AgsRecall *recall,
				 AgsRecallHandler *recall_handler)
{
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall) ||
     recall_handler == NULL){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);
  
  /* remove handler */
  g_rec_mutex_lock(recall_mutex);

  recall->recall_handler = g_list_remove(recall->recall_handler,
					 recall_handler);

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_get_output_soundcard:
 * @recall: the #AgsRecall
 *
 * Get the output soundcard object of @recall.
 *
 * Returns: (transfer full): the output soundcard
 * 
 * Since: 3.1.0
 */
GObject*
ags_recall_get_output_soundcard(AgsRecall *recall)
{
  GObject *output_soundcard;
  
  if(!AGS_IS_RECALL(recall)){
    return(NULL);
  }

  g_object_get(recall,
	       "output-soundcard", &output_soundcard,
	       NULL);

  return(output_soundcard);
}

/**
 * ags_recall_set_output_soundcard:
 * @recall: the #AgsRecall
 * @output_soundcard: the #GObject implementing #AgsSoundcard
 * 
 * Set output soundcard of @recall.
 * 
 * Since: 3.0.0
 */
void
ags_recall_set_output_soundcard(AgsRecall *recall, GObject *output_soundcard)
{
  guint samplerate;
  guint buffer_size;
  AgsSoundcardFormat format;

  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  /* unref of old soundcard */
  g_rec_mutex_lock(recall_mutex);

  if(recall->output_soundcard != NULL){
    g_signal_handlers_disconnect_by_data(recall->output_soundcard,
					 recall);

    g_object_unref(recall->output_soundcard);
  }

  /* ref and set output soundcard */
  if(output_soundcard != NULL){
    g_object_ref(output_soundcard);
    
    ags_soundcard_get_presets(AGS_SOUNDCARD(output_soundcard),
			      NULL,
			      &samplerate,
			      &buffer_size,
			      &format);
  }

  recall->output_soundcard = output_soundcard;
  
  g_rec_mutex_unlock(recall_mutex);

  g_object_set(recall,
	       "samplerate", samplerate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

/**
 * ags_recall_get_input_soundcard:
 * @recall: the #AgsRecall
 *
 * Get the input soundcard object of @recall.
 *
 * Returns: (transfer full): the input soundcard
 * 
 * Since: 3.1.0
 */
GObject*
ags_recall_get_input_soundcard(AgsRecall *recall)
{
  GObject *input_soundcard;
  
  if(!AGS_IS_RECALL(recall)){
    return(NULL);
  }

  g_object_get(recall,
	       "input-soundcard", &input_soundcard,
	       NULL);

  return(input_soundcard);
}

/**
 * ags_recall_set_input_soundcard:
 * @recall: the #AgsRecall
 * @input_soundcard: the #GObject implementing #AgsSoundcard
 * 
 * Set input soundcard of @recall.
 * 
 * Since: 3.0.0
 */
void
ags_recall_set_input_soundcard(AgsRecall *recall, GObject *input_soundcard)
{
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* unref of old soundcard */
  g_rec_mutex_lock(recall_mutex);

  if(recall->input_soundcard != NULL){
    g_signal_handlers_disconnect_by_data(recall->input_soundcard,
					 recall);

    g_object_unref(recall->input_soundcard);
  }

  /* ref and set input soundcard */
  if(input_soundcard != NULL){
    g_object_ref(input_soundcard);
  }

  recall->input_soundcard = input_soundcard;
  
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_get_samplerate:
 * @recall: the #AgsRecall
 *
 * Gets samplerate.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.1.0
 */
guint
ags_recall_get_samplerate(AgsRecall *recall)
{
  guint samplerate;
  
  if(!AGS_IS_RECALL(recall)){
    return(0);
  }

  g_object_get(recall,
	       "samplerate", &samplerate,
	       NULL);

  return(samplerate);
}

/**
 * ags_recall_set_samplerate:
 * @recall: the #AgsRecall
 * @samplerate: the samplerate
 * 
 * Set samplerate of @recall.
 * 
 * Since: 3.0.0
 */
void
ags_recall_set_samplerate(AgsRecall *recall, guint samplerate)
{
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* set samplerate */
  g_rec_mutex_lock(recall_mutex);

  recall->samplerate = samplerate;
  
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_get_buffer_size:
 * @recall: the #AgsRecall
 *
 * Gets buffer size.
 * 
 * Returns: the buffer size
 * 
 * Since: 3.1.0
 */
guint
ags_recall_get_buffer_size(AgsRecall *recall)
{
  guint buffer_size;
  
  if(!AGS_IS_RECALL(recall)){
    return(0);
  }

  g_object_get(recall,
	       "buffer-size", &buffer_size,
	       NULL);

  return(buffer_size);
}

/**
 * ags_recall_set_buffer_size:
 * @recall: the #AgsRecall
 * @buffer_size: the buffer size
 * 
 * Set buffer size of @recall.
 * 
 * Since: 3.0.0
 */
void
ags_recall_set_buffer_size(AgsRecall *recall, guint buffer_size)
{
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* set buffer size */
  g_rec_mutex_lock(recall_mutex);

  recall->buffer_size = buffer_size;
  
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_get_format:
 * @recall: the #AgsRecall
 *
 * Gets format.
 * 
 * Returns: the format
 * 
 * Since: 3.1.0
 */
AgsSoundcardFormat
ags_recall_get_format(AgsRecall *recall)
{
  AgsSoundcardFormat format;
  
  if(!AGS_IS_RECALL(recall)){
    return(0);
  }

  g_object_get(recall,
	       "format", &format,
	       NULL);

  return(format);
}

/**
 * ags_recall_set_format:
 * @recall: the #AgsRecall
 * @format: the format
 * 
 * Set format of @recall.
 * 
 * Since: 3.0.0
 */
void
ags_recall_set_format(AgsRecall *recall, AgsSoundcardFormat format)
{
  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL(recall)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* set format */
  g_rec_mutex_lock(recall_mutex);

  recall->format = format;
  
  g_rec_mutex_unlock(recall_mutex);
}

void
ags_recall_real_resolve_dependency(AgsRecall *recall)
{
  GList *list_start, *list, *next;

  gboolean children_lock_free;
  
  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  children_lock_free = ags_recall_global_get_children_lock_free();

  /* resolve dependency */
  g_rec_mutex_lock(recall_mutex);

  if((AGS_RECALL_TEMPLATE & (AGS_RECALL(recall)->flags)) != 0){
    g_warning("running on template");
  }

  if(!children_lock_free){
    list =
      list_start = g_list_copy(recall->children);
  }else{
    list =
      list_start = recall->children;
  }
  
  g_rec_mutex_unlock(recall_mutex);

  while(list != NULL){
    next = list->next;
    
    ags_recall_resolve_dependency(AGS_RECALL(list->data));

    list = next;
  }

  if(!children_lock_free){
    g_list_free(list_start);
  }
}

/**
 * ags_recall_resolve_dependency:
 * @recall: the #AgsRecall
 *
 * A signal indicating that the inheriting object should resolve
 * it's dependency.
 * 
 * Since: 3.0.0
 */
void
ags_recall_resolve_dependency(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

#ifdef AGS_DEBUG
  g_message("resolving %s", G_OBJECT_TYPE_NAME(recall));
#endif
  
  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[PLAY_RESOLVE_DEPENDENCY], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_check_rt_data(AgsRecall *recall)
{
  GList *list_start, *list, *next;

  gboolean children_lock_free;
  
  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  children_lock_free = ags_recall_global_get_children_lock_free();

  /* check rt data */
  g_rec_mutex_lock(recall_mutex);
  
  recall->staging_flags |= AGS_SOUND_STAGING_CHECK_RT_DATA;

  if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0){
    g_warning("running on template");
  }

  if(!children_lock_free){
    list =
      list_start = g_list_copy(recall->children);
  }else{
    list = 
      list_start = recall->children;
  }
  
  g_rec_mutex_unlock(recall_mutex);

  while(list != NULL){
    next = list->next;
    
    ags_recall_check_rt_data(AGS_RECALL(list->data));

    list = next;
  }

  if(!children_lock_free){
    g_list_free(list_start);  
  }
  
  /* set is waiting */
  g_rec_mutex_lock(recall_mutex);

  recall->state_flags |= (AGS_SOUND_STATE_IS_WAITING);

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_check_rt_data:
 * @recall: the #AgsRecall
 *
 * Prepare for run, this is the pre stage within the preparation.
 * 
 * Since: 3.0.0
 */
void
ags_recall_check_rt_data(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));
  g_return_if_fail(!ags_recall_test_state_flags(recall, AGS_SOUND_STATE_IS_TERMINATING));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[PLAY_CHECK_RT_DATA], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_run_init_pre(AgsRecall *recall)
{
  GList *list_start, *list, *next;

  gboolean children_lock_free;
  gboolean omit_event;
  
  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  children_lock_free = ags_recall_global_get_children_lock_free();
  omit_event = ags_recall_global_get_omit_event();

  /* run init pre */
  g_rec_mutex_lock(recall_mutex);

  if((AGS_SOUND_STAGING_RUN_INIT_PRE & (recall->staging_flags)) != 0){
    g_rec_mutex_unlock(recall_mutex);

    return;
  }

  recall->flags |= AGS_RECALL_INITIAL_RUN;
  recall->staging_flags |= AGS_SOUND_STAGING_RUN_INIT_PRE;

  if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0){
    g_warning("running on template");
  }
  
  if(!children_lock_free){
    list =
      list_start = g_list_copy_deep(recall->children,
				    (GCopyFunc) g_object_ref,
				    NULL);
  }else{
    list = 
      list_start = recall->children;
  }
  
  g_rec_mutex_unlock(recall_mutex);

  while(list != NULL){
    next = list->next;
    
    if(omit_event){
      AGS_RECALL_GET_CLASS(AGS_RECALL(list->data))->run_init_pre(AGS_RECALL(list->data));
    }else{
      ags_recall_run_init_pre(AGS_RECALL(list->data));
    }
    
    list = next;
  }

  if(!children_lock_free){
    g_list_free_full(list_start,
		     g_object_unref);
  }
}

/**
 * ags_recall_run_init_pre:
 * @recall: the #AgsRecall
 *
 * Prepare for run, this is the pre stage within the preparation.
 * 
 * Since: 3.0.0
 */
void
ags_recall_run_init_pre(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));
  g_return_if_fail(!ags_recall_test_state_flags(recall, AGS_SOUND_STATE_IS_TERMINATING));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[PLAY_RUN_INIT_PRE], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_run_init_inter(AgsRecall *recall)
{
  GList *list_start, *list, *next;

  gboolean children_lock_free;
  gboolean omit_event;
  
  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  children_lock_free = ags_recall_global_get_children_lock_free();
  omit_event = ags_recall_global_get_omit_event();

  /* run init inter */
  g_rec_mutex_lock(recall_mutex);

  if((AGS_SOUND_STAGING_RUN_INIT_INTER & (recall->staging_flags)) != 0){
    g_rec_mutex_unlock(recall_mutex);

    return;
  }

  recall->staging_flags |= AGS_SOUND_STAGING_RUN_INIT_INTER;

  if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0){
    g_warning("running on template");
  }

  if(!children_lock_free){
    list =
      list_start = g_list_copy_deep(recall->children,
				    (GCopyFunc) g_object_ref,
				    NULL);
  }else{
    list = 
      list_start = recall->children;
  }
  
  g_rec_mutex_unlock(recall_mutex);

  while(list != NULL){
    next = list->next;
    
    if(omit_event){
      AGS_RECALL_GET_CLASS(AGS_RECALL(list->data))->run_init_inter(AGS_RECALL(list->data));
    }else{
      ags_recall_run_init_inter(AGS_RECALL(list->data));
    }
    
    list = next;
  }

  if(!children_lock_free){
    g_list_free_full(list_start,
		     g_object_unref);
  }
}

/**
 * ags_recall_run_init_inter:
 * @recall: the #AgsRecall
 *
 * Prepare for run, this is the inter stage within the preparation.
 * 
 * Since: 3.0.0
 */
void
ags_recall_run_init_inter(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));
  g_return_if_fail(!ags_recall_test_state_flags(recall, AGS_SOUND_STATE_IS_TERMINATING));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[PLAY_RUN_INIT_INTER], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_run_init_post(AgsRecall *recall)
{
  GList *list_start, *list, *next;

  gboolean children_lock_free;
  gboolean omit_event;
  
  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  children_lock_free = ags_recall_global_get_children_lock_free();
  omit_event = ags_recall_global_get_omit_event();

  /* run init post */
  g_rec_mutex_lock(recall_mutex);

  if((AGS_SOUND_STAGING_RUN_INIT_POST & (recall->staging_flags)) != 0){
    g_rec_mutex_unlock(recall_mutex);

    return;
  }

  recall->staging_flags |= AGS_SOUND_STAGING_RUN_INIT_POST;

  if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0){
    g_warning("running on template");
  }

  if(!children_lock_free){
    list =
      list_start = g_list_copy_deep(recall->children,
				    (GCopyFunc) g_object_ref,
				    NULL);
  }else{
    list = 
      list_start = recall->children;
  }
  
  g_rec_mutex_unlock(recall_mutex);

  while(list != NULL){
    next = list->next;
    
    if(omit_event){
      AGS_RECALL_GET_CLASS(AGS_RECALL(list->data))->run_init_post(AGS_RECALL(list->data));
    }else{
      ags_recall_run_init_post(AGS_RECALL(list->data));
    }
    
    list = next;
  }

  if(!children_lock_free){
    g_list_free_full(list_start,
		     g_object_unref);
  }
  
  /* set active */
  g_rec_mutex_lock(recall_mutex);

  recall->state_flags &= (~AGS_SOUND_STATE_IS_WAITING);
  recall->state_flags |= (AGS_SOUND_STATE_IS_ACTIVE);

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_run_init_post:
 * @recall: the #AgsRecall
 *
 * Prepare for run, this is the post stage within the preparation.
 * 
 * Since: 3.0.0
 */
void
ags_recall_run_init_post(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));
  g_return_if_fail(!ags_recall_test_state_flags(recall, AGS_SOUND_STATE_IS_TERMINATING));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[PLAY_RUN_INIT_POST], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_feed_input_queue(AgsRecall *recall)
{
  GList *list_start, *list, *next;

  GRecMutex *recall_mutex;

  gboolean children_lock_free;  
  gboolean omit_event;
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  children_lock_free = ags_recall_global_get_children_lock_free();
  omit_event = ags_recall_global_get_omit_event();

  /* feed input queue */
  g_rec_mutex_lock(recall_mutex);

  if((AGS_SOUND_STAGING_FEED_INPUT_QUEUE & (recall->staging_flags)) != 0){
    g_rec_mutex_unlock(recall_mutex);

    return;
  }

  recall->staging_flags |= AGS_SOUND_STAGING_FEED_INPUT_QUEUE;

  if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0){
    g_warning("running on template");
  }

  if(!children_lock_free){
    list =
      list_start = g_list_copy_deep(recall->children,
				    (GCopyFunc) g_object_ref,
				    NULL);
  }else{
    list = 
      list_start = recall->children;
  }
  
  g_rec_mutex_unlock(recall_mutex);

  while(list != NULL){
    next = list->next;
    
    if(omit_event){
      AGS_RECALL_GET_CLASS(AGS_RECALL(list->data))->feed_input_queue(AGS_RECALL(list->data));
    }else{
      ags_recall_feed_input_queue(AGS_RECALL(list->data));
    }    

    list = next;
  }

  if(!children_lock_free){
    g_list_free_full(list_start,
		     g_object_unref);
  }
}

/**
 * ags_recall_feed_input_queue:
 * @recall: the #AgsRecall
 *
 * Feed input queue of @recall.
 * 
 * Since: 3.0.0
 */
void
ags_recall_feed_input_queue(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));
  g_return_if_fail(!ags_recall_test_state_flags(recall, AGS_SOUND_STATE_IS_TERMINATING));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[PLAY_FEED_INPUT_QUEUE], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_automate(AgsRecall *recall)
{
  GList *list_start, *list, *next;

  GRecMutex *recall_mutex;

  gboolean children_lock_free;  
  gboolean omit_event;
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  children_lock_free = ags_recall_global_get_children_lock_free();
  omit_event = ags_recall_global_get_omit_event();

  /* automate */
  g_rec_mutex_lock(recall_mutex);

  if((AGS_SOUND_STAGING_AUTOMATE & (recall->staging_flags)) != 0){
    g_rec_mutex_unlock(recall_mutex);

    return;
  }

  recall->staging_flags |= AGS_SOUND_STAGING_AUTOMATE;

  if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0){
    g_warning("running on template");
  }

  list =
    list_start = g_list_copy_deep(recall->children,
				  (GCopyFunc) g_object_ref,
				  NULL);
  
  g_rec_mutex_unlock(recall_mutex);

  while(list != NULL){
    next = list->next;
    
    if(omit_event){
      AGS_RECALL_GET_CLASS(AGS_RECALL(list->data))->automate(AGS_RECALL(list->data));
    }else{
      ags_recall_automate(AGS_RECALL(list->data));
    }    

    list = next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
}

/**
 * ags_recall_automate:
 * @recall: the #AgsRecall
 *
 * Automate port of @recall.
 * 
 * Since: 3.0.0
 */
void
ags_recall_automate(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[PLAY_AUTOMATE], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_run_pre(AgsRecall *recall)
{
  GList *list_start, *list, *next;

  GRecMutex *recall_mutex;

  gboolean children_lock_free;  
  gboolean omit_event;

#if 0
  if(AGS_IS_RECALL_AUDIO_SIGNAL(recall)){
    g_message("%s::run-pre()", G_OBJECT_TYPE_NAME(recall));
  }
#endif
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  children_lock_free = ags_recall_global_get_children_lock_free();
  omit_event = ags_recall_global_get_omit_event();

  /* run pre */
  g_rec_mutex_lock(recall_mutex);

  if((AGS_SOUND_STAGING_RUN_PRE & (recall->staging_flags)) != 0){
    g_rec_mutex_unlock(recall_mutex);

    return;
  }

  recall->staging_flags |= AGS_SOUND_STAGING_RUN_PRE;

  if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0){
    g_warning("running on template");
  }

  if(!children_lock_free){
    list =
      list_start = g_list_copy_deep(recall->children,
				    (GCopyFunc) g_object_ref,
				    NULL);
  }else{
    list = 
      list_start = recall->children;
  }
  
  g_rec_mutex_unlock(recall_mutex);

  while(list != NULL){
    next = list->next;
    
    if(omit_event){
      AGS_RECALL_GET_CLASS(AGS_RECALL(list->data))->run_pre(AGS_RECALL(list->data));
    }else{
      ags_recall_run_pre(AGS_RECALL(list->data));
    }    

    list = next;
  }

  if(!children_lock_free){
    g_list_free_full(list_start,
		     g_object_unref);
  }
}

/**
 * ags_recall_run_pre:
 * @recall: the #AgsRecall
 *
 * This is the pre stage within a run.
 * 
 * Since: 3.0.0
 */
void
ags_recall_run_pre(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));
  g_return_if_fail(!ags_recall_test_state_flags(recall, AGS_SOUND_STATE_IS_TERMINATING));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[PLAY_RUN_PRE], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_run_inter(AgsRecall *recall)
{
  GList *list_start, *list, *next;

  GRecMutex *recall_mutex;

  gboolean children_lock_free;  
  gboolean omit_event;

#if 0
  if(AGS_IS_RECALL_AUDIO_SIGNAL(recall)){
    g_message("%s::run-inter()", G_OBJECT_TYPE_NAME(recall));
  }
#endif
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  children_lock_free = ags_recall_global_get_children_lock_free();
  omit_event = ags_recall_global_get_omit_event();

  /* run inter */
  g_rec_mutex_lock(recall_mutex);

  if((AGS_SOUND_STAGING_RUN_INTER & (recall->staging_flags)) != 0){
    g_rec_mutex_unlock(recall_mutex);

    return;
  }

  recall->staging_flags |= AGS_SOUND_STAGING_RUN_INTER;

  if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0){
    g_warning("running on template");
  }

  if(!children_lock_free){
    list =
      list_start = g_list_copy_deep(recall->children,
				    (GCopyFunc) g_object_ref,
				    NULL);
  }else{
    list = 
      list_start = recall->children;
  }
  
  g_rec_mutex_unlock(recall_mutex);

  while(list != NULL){
    next = list->next;
    
    if(omit_event){
      AGS_RECALL_GET_CLASS(AGS_RECALL(list->data))->run_inter(AGS_RECALL(list->data));
    }else{
      ags_recall_run_inter(AGS_RECALL(list->data));
    }    

    list = next;
  }

  if(!children_lock_free){
    g_list_free_full(list_start,
		     g_object_unref);
  }
}

/**
 * ags_recall_run_inter:
 * @recall: the #AgsRecall
 *
 * This is the inter stage within a run.
 * 
 * Since: 3.0.0
 */
void
ags_recall_run_inter(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));
  g_return_if_fail(!ags_recall_test_state_flags(recall, AGS_SOUND_STATE_IS_TERMINATING));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[PLAY_RUN_INTER], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_run_post(AgsRecall *recall)
{
  GList *list_start, *list, *next;

  GRecMutex *recall_mutex;

  gboolean children_lock_free;  
  gboolean omit_event;

#if 0
  if(AGS_IS_RECALL_AUDIO_SIGNAL(recall)){
    g_message("%s::run-post()", G_OBJECT_TYPE_NAME(recall));
  }
#endif
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  children_lock_free = ags_recall_global_get_children_lock_free();
  omit_event = ags_recall_global_get_omit_event();

  /* run post */
  g_rec_mutex_lock(recall_mutex);

  if((AGS_SOUND_STAGING_RUN_POST & (recall->staging_flags)) != 0){
    g_rec_mutex_unlock(recall_mutex);

    return;
  }

  recall->staging_flags |= AGS_SOUND_STAGING_RUN_POST;
  recall->flags &= (~AGS_RECALL_INITIAL_RUN);

  if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0){
    g_warning("running on template");
  }

  if(!children_lock_free){
    list =
      list_start = g_list_copy_deep(recall->children,
				    (GCopyFunc) g_object_ref,
				    NULL);
  }else{
    list = 
      list_start = recall->children;
  }
  
  g_rec_mutex_unlock(recall_mutex);

  while(list != NULL){
    next = list->next;
    
    if(omit_event){
      AGS_RECALL_GET_CLASS(AGS_RECALL(list->data))->run_post(AGS_RECALL(list->data));
    }else{
      ags_recall_run_post(AGS_RECALL(list->data));
    }    

    list = next;
  }

  if(!children_lock_free){
    g_list_free_full(list_start,
		     g_object_unref);
  }
}

/**
 * ags_recall_run_post:
 * @recall: the #AgsRecall
 *
 * This is the post stage within a run.
 * 
 * Since: 3.0.0
 */
void
ags_recall_run_post(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));
  g_return_if_fail(!ags_recall_test_state_flags(recall, AGS_SOUND_STATE_IS_TERMINATING));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[PLAY_RUN_POST], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_do_feedback(AgsRecall *recall)
{
  GList *list_start, *list, *next;

  GRecMutex *recall_mutex;

  gboolean children_lock_free;  
  gboolean omit_event;
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  children_lock_free = ags_recall_global_get_children_lock_free();
  omit_event = ags_recall_global_get_omit_event();

  /* do feedback */
  g_rec_mutex_lock(recall_mutex);

  if((AGS_SOUND_STAGING_DO_FEEDBACK & (recall->staging_flags)) != 0){
    g_rec_mutex_unlock(recall_mutex);

    return;
  }

  recall->staging_flags |= AGS_SOUND_STAGING_DO_FEEDBACK;

  if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0){
    g_warning("running on template");
  }

  if(!children_lock_free){
    list =
      list_start = g_list_copy_deep(recall->children,
				    (GCopyFunc) g_object_ref,
				    NULL);
  }else{
    list = 
      list_start = recall->children;
  }
  
  g_rec_mutex_unlock(recall_mutex);

  while(list != NULL){
    next = list->next;
    
    if(omit_event){
      AGS_RECALL_GET_CLASS(AGS_RECALL(list->data))->do_feedback(AGS_RECALL(list->data));
    }else{
      ags_recall_do_feedback(AGS_RECALL(list->data));
    }    

    list = next;
  }

  if(!children_lock_free){
    g_list_free_full(list_start,
		     g_object_unref);
  }
}

/**
 * ags_recall_do_feedback:
 * @recall: the #AgsRecall
 *
 * Do feedback of @recall.
 * 
 * Since: 3.0.0
 */
void
ags_recall_do_feedback(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));
  g_return_if_fail(!ags_recall_test_state_flags(recall, AGS_SOUND_STATE_IS_TERMINATING));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[PLAY_DO_FEEDBACK], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_feed_output_queue(AgsRecall *recall)
{
  GList *list_start, *list, *next;

  GRecMutex *recall_mutex;

  gboolean children_lock_free;  
  gboolean omit_event;
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  children_lock_free = ags_recall_global_get_children_lock_free();
  omit_event = ags_recall_global_get_omit_event();

  /* feed output queue */
  g_rec_mutex_lock(recall_mutex);

  if((AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE & (recall->staging_flags)) != 0){
    g_rec_mutex_unlock(recall_mutex);

    return;
  }

  recall->staging_flags |= AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE;

  if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0){
    g_warning("running on template");
  }

  if(!children_lock_free){
    list =
      list_start = g_list_copy_deep(recall->children,
				    (GCopyFunc) g_object_ref,
				    NULL);
  }else{
    list = 
      list_start = recall->children;
  }
  
  g_rec_mutex_unlock(recall_mutex);

  while(list != NULL){
    next = list->next;
    
    if(omit_event){
      AGS_RECALL_GET_CLASS(AGS_RECALL(list->data))->feed_output_queue(AGS_RECALL(list->data));
    }else{
      ags_recall_feed_output_queue(AGS_RECALL(list->data));
    }    

    list = next;
  }

  if(!children_lock_free){
    g_list_free_full(list_start,
		     g_object_unref);
  }
}

/**
 * ags_recall_feed_output_queue:
 * @recall: the #AgsRecall
 *
 * Feed output queue of @recall.
 * 
 * Since: 3.0.0
 */
void
ags_recall_feed_output_queue(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));
  g_return_if_fail(!ags_recall_test_state_flags(recall, AGS_SOUND_STATE_IS_TERMINATING));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[PLAY_FEED_OUTPUT_QUEUE], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_stop_persistent(AgsRecall *recall)
{
  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* check state and staging */
  g_rec_mutex_lock(recall_mutex);

  if((AGS_SOUND_STATE_IS_TERMINATING & (recall->state_flags)) != 0 ||
     (AGS_SOUND_STAGING_DONE & (recall->staging_flags)) != 0){
    g_rec_mutex_unlock(recall_mutex);
    
    return;
  }

  recall->behaviour_flags &= (~(AGS_SOUND_BEHAVIOUR_PERSISTENT |
				AGS_SOUND_BEHAVIOUR_PERSISTENT_PLAYBACK |
				AGS_SOUND_BEHAVIOUR_PERSISTENT_NOTATION |
				AGS_SOUND_BEHAVIOUR_PERSISTENT_SEQUENCER |
				AGS_SOUND_BEHAVIOUR_PERSISTENT_WAVE |
				AGS_SOUND_BEHAVIOUR_PERSISTENT_MIDI));

  g_rec_mutex_unlock(recall_mutex);

  /* emit done */
  ags_recall_done(recall);
}

/**
 * ags_recall_stop_persistent:
 * @recall: the #AgsRecall
 *
 * Unsets the %AGS_SOUND_BEHAVIOUR_PERSISTENT and related behaviour flags and
 * invokes ags_recall_done().
 * 
 * Since: 3.0.0
 */
void
ags_recall_stop_persistent(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[PLAY_STOP_PERSISTENT], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_cancel(AgsRecall *recall)
{
  GList *list_start, *list;

  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* cancel */
  g_rec_mutex_lock(recall_mutex);

  recall->staging_flags |= AGS_SOUND_STAGING_CANCEL;

  if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0){
    g_warning("running on template");
  }

  list =
    list_start = g_list_copy(recall->children);

  g_rec_mutex_unlock(recall_mutex);

  while(list != NULL){
    ags_recall_cancel(AGS_RECALL(list->data));
    
    list = list->next;
  }
  
  g_list_free(list_start);  

  /* stop any recall */
  ags_recall_stop_persistent(recall);
}

/**
 * ags_recall_cancel:
 * @recall: the #AgsRecall
 *
 * The #AgsRecall doesn't want to run anymore, it aborts further execution.
 * 
 * Since: 3.0.0
 */
void
ags_recall_cancel(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[PLAY_CANCEL], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_done(AgsRecall *recall)
{
  GList *list_start, *list;

  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

#ifdef AGS_DEBUG  
  if(AGS_IS_RECALL_AUDIO_SIGNAL(recall)){
    g_message("done - %s", G_OBJECT_TYPE_NAME(recall));
  }
#endif
  
  /* do feedback */
  g_rec_mutex_lock(recall_mutex);

  recall->staging_flags |= AGS_SOUND_STAGING_DONE;

  if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0){
    g_warning("running on template");
  }
  
  list =
    list_start = g_list_copy(recall->children);

  g_rec_mutex_unlock(recall_mutex);
  
  while(list != NULL){
    ags_recall_done(AGS_RECALL(list->data));

    list = list->next;
  }

  g_list_free(list_start);  
}

/**
 * ags_recall_done:
 * @recall: the #AgsRecall
 *
 * The #AgsRecall doesn't want to run anymore, it has been done its
 * work.
 * 
 * Since: 3.0.0
 */
void
ags_recall_done(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));
  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[PLAY_DONE], 0);
  g_object_unref(G_OBJECT(recall));
}

AgsRecall*
ags_recall_real_duplicate(AgsRecall *recall,
			  AgsRecallID *recall_id,
			  guint *n_params, gchar **parameter_name, GValue *value)
{
  AgsRecall *copy_recall;
  AgsRecallClass *recall_class, *copy_class;
  AgsRecallContainer *recall_container;

  GObject *output_soundcard;
  GObject *input_soundcard;

  GType child_type;
  
  GList *list, *child;

  AgsRecallFlags recall_flags;
  AgsSoundAbilityFlags ability_flags;
  AgsSoundBehaviourFlags behaviour_flags;
  gint output_soundcard_channel;
  gint input_soundcard_channel;
  guint local_n_params;
  guint i;
  
  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);
  
  /* get some fields */
  g_rec_mutex_lock(recall_mutex);

  recall_flags = recall->flags;
  ability_flags = recall->ability_flags;
  behaviour_flags = recall->behaviour_flags;
  
  recall_container = (AgsRecallContainer *) recall->recall_container;
  
  output_soundcard = recall->output_soundcard;
  output_soundcard_channel = recall->output_soundcard_channel;

  input_soundcard = recall->input_soundcard;
  input_soundcard_channel = recall->input_soundcard_channel;

  child_type = recall->child_type;
  
  g_rec_mutex_unlock(recall_mutex);

  /* grow parameter name and value */
  local_n_params = 0;
  
  if(n_params == NULL){
    n_params = &local_n_params;
  }
  
  if(n_params[0] == 0){
    parameter_name = (gchar **) malloc(8 * sizeof(gchar *));
    value = g_new0(GValue,
		   7);
  }else{
    parameter_name = (gchar **) realloc(parameter_name,
					(n_params[0] + 8) * sizeof(gchar *));
    value = g_renew(GValue,
		    value,
		    n_params[0] + 7);
  }

  /* set parameter name and value */
  parameter_name[n_params[0]] = "output-soundcard";
  memset(&(value[n_params[0]]), 0, sizeof(GValue));
  g_value_init(&(value[n_params[0]]),
	       G_TYPE_OBJECT);
  g_value_set_object(&(value[n_params[0]]), output_soundcard);

  parameter_name[n_params[0] + 1] = "output-soundcard-channel";
  memset(&(value[n_params[0] + 1]), 0, sizeof(GValue));
  g_value_init(&(value[n_params[0] + 1]),
	       G_TYPE_INT);
  g_value_set_int(&(value[n_params[0] + 1]), output_soundcard_channel);

  parameter_name[n_params[0] + 2] = "input-soundcard";
  memset(&(value[n_params[0] + 2]), 0, sizeof(GValue));
  g_value_init(&(value[n_params[0] + 2]),
	       G_TYPE_OBJECT);
  g_value_set_object(&(value[n_params[0] + 2]), input_soundcard);
    
  parameter_name[n_params[0] + 3] = "input-soundcard-channel";
  memset(&(value[n_params[0] + 3]), 0, sizeof(GValue));
  g_value_init(&(value[n_params[0] + 3]),
	       G_TYPE_INT);
  g_value_set_int(&(value[n_params[0] + 3]), input_soundcard_channel);

  parameter_name[n_params[0] + 4] = "recall-id";
  memset(&(value[n_params[0] + 4]), 0, sizeof(GValue));
  g_value_init(&(value[n_params[0] + 4]),
	       G_TYPE_OBJECT);
  g_value_set_object(&(value[n_params[0] + 4]), recall_id);
    
  parameter_name[n_params[0] + 5] = "recall-container";
  memset(&(value[n_params[0] + 5]), 0, sizeof(GValue));
  g_value_init(&(value[n_params[0] + 5]),
	       G_TYPE_OBJECT);
  g_value_set_object(&(value[n_params[0] + 5]), recall_container);

  parameter_name[n_params[0] + 6] = "child-type";
  memset(&(value[n_params[0] + 6]), 0, sizeof(GValue));
  g_value_init(&(value[n_params[0] + 6]),
	       G_TYPE_GTYPE);
  g_value_set_gtype(&(value[n_params[0] + 6]), child_type);

  parameter_name[n_params[0] + 7] = NULL;

  n_params[0] += 7;
  
#if HAVE_GLIB_2_54    
  copy_recall = g_object_new_with_properties(G_OBJECT_TYPE(recall),
					     n_params[0], parameter_name, value);
#else
  copy_recall = g_object_new(G_OBJECT_TYPE(recall),
			     NULL);

  {
    guint i;

    for(i = 0; i < n_params[0]; i++){
      g_object_set_property((GObject *) copy_recall,
			    parameter_name[i], &(value[i]));
    }
  }
#endif
  
  /* free parameter name and value */
  g_free(parameter_name);

  for(i = 0; i < n_params[0]; i++){
    g_value_unset(&(value[i]));
  }
  
  g_free(value);

  /* apply flags */
  ags_recall_set_flags(copy_recall,
		       (recall_flags & (~AGS_RECALL_TEMPLATE)));

  ags_recall_set_ability_flags(copy_recall, ability_flags);
  ags_recall_set_behaviour_flags(copy_recall, behaviour_flags);
  //  ags_recall_set_sound_scope(copy_recall, sound_scope);
  
  /* duplicate handlers */
  g_rec_mutex_lock(recall_mutex);

  list = recall->recall_handler;
  
  while(list != NULL){
    AgsRecallHandler *recall_handler, *copy_recall_handler;
    
    recall_handler = AGS_RECALL_HANDLER(list->data);

    copy_recall_handler = ags_recall_handler_alloc(recall_handler->signal_name,
						   recall_handler->callback,
						   recall_handler->data);
    ags_recall_add_recall_handler(copy_recall,
				  copy_recall_handler);

    list = list->next;
  }

  g_rec_mutex_unlock(recall_mutex);

  /* recall container */
  if(recall_container != NULL){
    if(AGS_IS_RECALL_AUDIO(copy_recall)){
      g_object_set(recall_container,
		   "recall-audio", copy_recall,
		   NULL);
    }else if(AGS_IS_RECALL_AUDIO_RUN(copy_recall)){
      g_object_set(recall_container,
		   "recall-audio-run", copy_recall,
		   NULL);
    }else if(AGS_IS_RECALL_CHANNEL(copy_recall)){
      g_object_set(recall_container,
		   "recall-channel", copy_recall,
		   NULL);
    }else if(AGS_IS_RECALL_CHANNEL_RUN(copy_recall)){
      g_object_set(recall_container,
		   "recall-channel-run", copy_recall,
		   NULL);
    }
  }
  
  return(copy_recall);
}

/**
 * ags_recall_duplicate:
 * @recall: the template #AgsRecAll
 * @recall_id: the #AgsRecallID
 * @n_params: guint pointer to parameter count
 * @parameter_name: string vector containing parameter names
 * @value: the #GValue-struct array
 *
 * Should duplicate an #AgsRecall, so it can pass the run stages. Mainly used for
 * creating duplicates of templates, see %AGS_RECALL_TEMPLATE.
 * 
 * Returns: (transfer full): the duplicated #AgsRecall
 *
 * Since: 3.0.0
 */
AgsRecall*
ags_recall_duplicate(AgsRecall *recall,
		     AgsRecallID *recall_id,
		     guint *n_params, gchar **parameter_name, GValue *value)
{
  AgsRecall *recall_copy;

  g_return_val_if_fail(AGS_IS_RECALL(recall), NULL);
  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[PLAY_DUPLICATE], 0,
		recall_id,
		n_params, parameter_name, value,
		&recall_copy);
  g_object_unref(G_OBJECT(recall));

  return(recall_copy);
}

/**
 * ags_recall_notify_dependency:
 * @recall: the #AgsRecall
 * @dependency: the dependency to notify for, see #AgsRecallNotifyDependencyMode-enum
 * @increase: if %TRUE increase, else if %FALSE decrease
 *
 * Notifies a recall that an other depends on it.
 * 
 * Since: 3.0.0
 */
void
ags_recall_notify_dependency(AgsRecall *recall, guint dependency, gboolean increase)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[PLAY_NOTIFY_DEPENDENCY], 0,
		dependency, increase);
  g_object_unref(G_OBJECT(recall));
}

/**
 * ags_recall_child_added:
 * @recall: the #AgsRecall
 * @child: the child #AgsRecall
 *
 * A signal indicating that the a child has been added.
 * 
 * Since: 3.0.0
 */
void
ags_recall_child_added(AgsRecall *recall, AgsRecall *child)
{
  g_return_if_fail(AGS_IS_RECALL(recall));
  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[CHILD_ADDED], 0,
		child);
  g_object_unref(G_OBJECT(recall));
}

/**
 * ags_recall_is_done:
 * @recall: (element-type AgsAudio.Recall) (transfer none): the #GList-struct containing #AgsRecall
 * @recycling_context: the #AgsRecyclingContext
 *
 * Check if recall is over.
 * 
 * Returns: %TRUE if recall is done, otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_recall_is_done(GList *recall, GObject *recycling_context)
{
  AgsRecall *current_recall;
  AgsRecallID *current_recall_id;
  AgsRecyclingContext *current_recycling_context;
  
  AgsRecallFlags current_recall_flags;
  AgsSoundStagingFlags current_staging_flags;
  
  GRecMutex *current_recall_mutex;
  GRecMutex *current_recall_id_mutex;

  if(recall == NULL ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return(FALSE);
  }

  while(recall != NULL){
    current_recall = AGS_RECALL(recall->data);

    /* get recall mutex */
    current_recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(current_recall);

    /* get some fields */
    g_rec_mutex_lock(current_recall_mutex);

    current_recall_flags = current_recall->flags;
    current_staging_flags = current_recall->staging_flags;

    current_recall_id = current_recall->recall_id;

    g_rec_mutex_unlock(current_recall_mutex);

    if(current_recall_id != NULL){
      /* get recall id mutex */
      current_recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(current_recall_id);

      /* get some fields */
      g_rec_mutex_lock(current_recall_id_mutex);

      current_recycling_context = current_recall_id->recycling_context;

      g_rec_mutex_unlock(current_recall_id_mutex);
    }else{
      current_recycling_context = NULL;
    } 
    
    if((AGS_RECALL_TEMPLATE & (current_recall_flags)) == 0 &&
       !AGS_IS_RECALL_AUDIO(current_recall) &&
       !AGS_IS_RECALL_CHANNEL(current_recall) &&
       current_recycling_context == (AgsRecyclingContext *) recycling_context){
      if((AGS_SOUND_STAGING_DONE & (current_staging_flags)) == 0){
	//FIXME:JK: replacement
	//	current_recall->flags &= (~AGS_RECALL_RUN_INITIALIZED);
	//	g_message("done: %s", G_OBJECT_TYPE_NAME(recall));
	
	return(FALSE);
      }
    }

    /* iterate */
    recall = recall->next;
  }

  return(TRUE);
}

/**
 * ags_recall_get_by_effect:
 * @recall: (element-type AgsAudio.Recall) (transfer none): the #GList-struct containing #AgsRecall
 * @filename: the filename containing @effect or %NULL
 * @effect: the effect name
 *
 * Finds all recalls matching @filename and @effect.
 *
 * Returns: (element-type AgsAudio.Recall) (transfer full): a #GList-struct containing #AgsRecall, or %NULL if not found
 *
 * Since: 3.0.0
 */
GList*
ags_recall_get_by_effect(GList *recall, gchar *filename, gchar *effect)
{
  GList *list;

  gchar *current_filename, *current_effect;
  
  if(recall == NULL ||
     effect == NULL){
    return(NULL);
  }
  
  list = NULL;

  while(recall != NULL){
    /* get some fields */
    current_filename = NULL;
    current_effect = NULL;
    
    g_object_get(recall->data,
		 "filename", &current_filename,
		 "effect", &current_effect,
		 NULL);

    /* check filename and effect */
    if(filename == NULL){
      if(current_filename == NULL &&
	 current_effect != NULL &&
	 !g_strcmp0(current_effect, effect)){
	g_object_ref(recall->data);
	list = g_list_prepend(list,
			      recall->data);
      }
    }else{
      if(current_filename != NULL &&
	 !g_strcmp0(current_filename, filename) &&
	 current_effect != NULL &&
	 !g_strcmp0(current_effect, effect)){
	g_object_ref(recall->data);
	list = g_list_prepend(list,
			      recall->data);
      }
    }    

    g_free(current_filename);
    g_free(current_effect);

    /* iterate */
    recall = recall->next;
  }
  
  list = g_list_reverse(list);
  
  return(list);
}

/**
 * ags_recall_find_recall_id_with_effect:
 * @recall: (element-type AgsAudio.Recall) (transfer none): the #GList-struct containing #AgsRecall
 * @recall_id: the #AgsRecallID, may be %NULL
 * @filename: the filename or %NULL
 * @effect: the effect name
 *
 * Finds next matching effect name. Intended to be used as
 * iteration function.
 *
 * Returns: (element-type AgsAudio.Recall) (transfer none): next matching #GList-struct, or %NULL if not found
 *
 * Since: 3.0.0
 */
GList*
ags_recall_find_recall_id_with_effect(GList *recall, AgsRecallID *recall_id, gchar *filename, gchar *effect)
{
  AgsRecall *current_recall;
  AgsRecallID *current_recall_id;
  AgsRecyclingContext *recycling_context;
  AgsRecyclingContext *current_recycling_context;

  gchar *current_filename, *current_effect;
  
  GRecMutex *current_recall_mutex;
  GRecMutex *recall_id_mutex;
  GRecMutex *current_recall_id_mutex;

  if(recall == NULL ||
     effect == NULL){
    return(NULL);
  }
  
  /* get recycling context */
  recycling_context = NULL;
    
  if(recall_id != NULL){
    /* get recall id mutex */
    recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(recall_id);

    /* recycling context */
    g_rec_mutex_lock(recall_id_mutex);

    recycling_context = recall_id->recycling_context;
      
    g_rec_mutex_unlock(recall_id_mutex);
  }
  
  while(recall != NULL){
    current_recall = AGS_RECALL(recall->data);

    /* get recall mutex */
    current_recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(current_recall);

    /* get some fields */
    g_rec_mutex_lock(current_recall_mutex);

    current_filename = current_recall->filename;
    current_effect = current_recall->effect;
    
    current_recall_id = current_recall->recall_id;
    
    g_rec_mutex_unlock(current_recall_mutex);

    /* get recycling context */
    current_recycling_context = NULL;
    
    if(current_recall_id != NULL){
      /* get recall id mutex */
      current_recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(current_recall_id);

      /* recycling context */
      g_rec_mutex_lock(current_recall_id_mutex);

      current_recycling_context = current_recall_id->recycling_context;
      
      g_rec_mutex_unlock(current_recall_id_mutex);
    }

    /* check recall id, filename and effect */
    if(filename == NULL){
      if(current_filename == NULL &&
	 current_effect != NULL &&
	 !g_strcmp0(current_effect, effect)){
	if(recall_id == NULL){
	  if(current_recall_id == NULL){
	    return(recall);
	  }
	}else{
	  if(recycling_context == current_recycling_context){
	    return(recall);
	  }
	}
      }
    }else{
      if(current_filename != NULL &&
	 !g_strcmp0(current_filename, filename) &&
	 current_effect != NULL &&
	 !g_strcmp0(current_effect, effect)){
	if(recall_id == NULL){
	  if(current_recall_id == NULL){
	    return(recall);
	  }
	}else{
	  if(recycling_context == current_recycling_context){
	    return(recall);
	  }
	}
      }
    }    

    /* iterate */
    recall = recall->next;
  }

  return(NULL);
}

/**
 * ags_recall_find_type:
 * @recall: (element-type AgsAudio.Recall) (transfer none): the #GList-struct containing #AgsRecall
 * @type: the #GType
 * 
 * Finds next matching recall for type. Intended to be used as
 * iteration function.
 *
 * Returns: (element-type AgsAudio.Recall) (transfer none): next matching #GList-struct, or %NULL if not found
 *
 * Since: 3.0.0
 */
GList*
ags_recall_find_type(GList *recall, GType gtype)
{
  AgsRecall *current_recall;

  while(recall != NULL){
    GType current_gtype;
    
    current_recall = AGS_RECALL(recall->data);

    current_gtype = G_OBJECT_TYPE(current_recall);
    
    if(g_type_is_a(current_gtype, gtype)){
      break;
    }

    /* iterate */
    recall = recall->next;
  }

  return(recall);
}

/**
 * ags_recall_find_template:
 * @recall: (element-type AgsAudio.Recall) (transfer none): the #GList-struct containing #AgsRecall
 *
 * Finds next template, see #AGS_RECALL_TEMPLATE flag. Intended to be used as
 * iteration function.
 *
 * Returns: (element-type AgsAudio.Recall) (transfer none): next matching #GList-struct, or %NULL if not found
 *
 * Since: 3.0.0
 */
GList*
ags_recall_find_template(GList *recall)
{
  AgsRecall *current_recall;

  AgsRecallFlags current_recall_flags;
  
  GRecMutex *current_recall_mutex;
  
  while(recall != NULL){
    current_recall = AGS_RECALL(recall->data);

    /* get recall mutex */
    current_recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(current_recall);

    /* get some fields */
    g_rec_mutex_lock(current_recall_mutex);

    current_recall_flags = current_recall->flags;

    g_rec_mutex_unlock(current_recall_mutex);

    /* check recall flags */
    if((AGS_RECALL_TEMPLATE & (current_recall_flags)) != 0){
      return(recall);
    }

    /* iterate */
    recall = recall->next;
  }

  return(NULL);
}

/**
 * ags_recall_template_find_type:
 * @recall: (element-type AgsAudio.Recall) (transfer none): the #GList-struct containing #AgsRecall
 * @type: a #GType
 * 
 * Finds next matching recall for type which is a template, see #AGS_RECALL_TEMPLATE flag.
 * Intended to be used as iteration function.
 *
 * Returns: (element-type AgsAudio.Recall) (transfer none): next matching #GList-struct, or %NULL if not found
 *
 * Since: 3.0.0
 */
GList*
ags_recall_template_find_type(GList *recall, GType gtype)
{
  AgsRecall *current_recall;

  while(recall != NULL){
    current_recall = (AgsRecall *) recall->data;

    if(AGS_IS_RECALL(current_recall) &&
       (AGS_RECALL_TEMPLATE & (current_recall->flags)) != 0 &&
       g_type_is_a(G_OBJECT_TYPE(current_recall), gtype)){
      break;
    }

    recall = recall->next;
  }

  return(recall);
}

/**
 * ags_recall_template_find_all_type:
 * @recall: (element-type AgsAudio.Recall) (transfer none): the #GList-struct containing #AgsRecall
 * @...: a #GType
 * 
 * Finds next matching recall for type which is a template, see #AGS_RECALL_TEMPLATE flag.
 * Intended to be used as iteration function.
 *
 * Returns: (element-type AgsAudio.Recall) (transfer none): next matching #GList-struct, or %NULL if not found
 *
 * Since: 3.0.0
 */
GList*
ags_recall_template_find_all_type(GList *recall, ...)
{
  AgsRecall *current_recall;
  
  GType *recall_type, *offset;
  GType current;

  AgsRecallFlags current_recall_flags;
  guint i;
  
  va_list ap;

  GRecMutex *current_recall_mutex;

  /* read all types */
  va_start(ap,
	   recall);

  recall_type = (GType *) malloc(sizeof(GType));

  i = 0;
  
  while(TRUE){
    current = va_arg(ap, GType);

    if(current == G_TYPE_NONE){
      break;
    }
    
    recall_type = (GType *) realloc(recall_type,
				    (i + 2) * sizeof(GType));
    recall_type[i] = current;
    
    i++;
  }

  recall_type[i] = G_TYPE_NONE;
  
  va_end(ap);

  /* find all types */
  while(recall != NULL){
    current_recall = (AgsRecall *) recall->data;
    
    if(AGS_IS_RECALL(current_recall)){
      /* get recall mutex */
      current_recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(current_recall);

      /* get some fields */
      g_rec_mutex_lock(current_recall_mutex);

      current_recall_flags = current_recall->flags;

      g_rec_mutex_unlock(current_recall_mutex);

      /**/
      if((AGS_RECALL_TEMPLATE & (current_recall_flags)) != 0){
	offset = recall_type;
    
	while(offset[0] != G_TYPE_NONE){
	  if(g_type_is_a(G_OBJECT_TYPE(current_recall), offset[0])){
	    free(recall_type);
	
	    return(recall);
	  }

	  offset++;
	}
      }
    }

    /* iterate */
    recall = recall->next;
  }

  free(recall_type);
	
  return(NULL);
}

/**
 * ags_recall_find_type_with_recycling_context:
 * @recall: (element-type AgsAudio.Recall) (transfer none): the #GList-struct containing #AgsRecall
 * @type: the #GType
 * @recycling_context: the #AgsRecyclingContext
 * 
 * Finds next matching recall for type which has @recycling_context, see #AgsRecallId for further
 * details about #AgsRecyclingContext. Intended to be used as iteration function.
 *
 * Returns: (element-type AgsAudio.Recall) (transfer none): next matching #GList-struct, or %NULL if not found
 *
 * Since: 3.0.0
 */
GList*
ags_recall_find_type_with_recycling_context(GList *recall, GType gtype, GObject *recycling_context)
{
  AgsRecall *current_recall;
  AgsRecallID *current_recall_id;
  AgsRecyclingContext *current_recycling_context;

  GRecMutex *current_recall_mutex;
  GRecMutex *current_recall_id_mutex;
  
  while(recall != NULL){
    current_recall = AGS_RECALL(recall->data);

    /* get recall mutex */
    current_recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(current_recall);

    /* get some fields */
    g_rec_mutex_lock(current_recall_mutex);

    current_recall_id = current_recall->recall_id;
    
    g_rec_mutex_unlock(current_recall_mutex);

    /* get recycling context */
    current_recycling_context = NULL;
    
    if(current_recall_id != NULL){
      /* get recall id mutex */
      current_recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(current_recall_id);

      /* recycling context */
      g_rec_mutex_lock(current_recall_id_mutex);

      current_recycling_context = current_recall_id->recycling_context;
      
      g_rec_mutex_unlock(current_recall_id_mutex);
    }

    if(g_type_is_a(G_OBJECT_TYPE(current_recall), gtype) &&
       current_recycling_context == (AgsRecyclingContext *) recycling_context){
      return(recall);
    }

    /* iterate */
    recall = recall->next;
  }

  return(NULL);
}

/**
 * ags_recall_find_recycling_context:
 * @recall: (element-type AgsAudio.Recall) (transfer none): the #GList-struct containing #AgsRecall
 * @recycling_context: the #AgsRecyclingContext
 * 
 * Finds next matching recall which has @recycling_context, see #AgsRecallId for further
 * details about #AgsRecyclingContext. Intended to be used as iteration function.
 *
 * Returns: (element-type AgsAudio.Recall) (transfer none): next matching #GList-struct, or %NULL if not found
 *
 * Since: 3.0.0
 */
GList*
ags_recall_find_recycling_context(GList *recall, GObject *recycling_context)
{
  AgsRecall *current_recall;
  AgsRecallID *current_recall_id;
  AgsRecyclingContext *current_recycling_context;

  while(recall != NULL){
    current_recall = AGS_RECALL(recall->data);

    /* get some fields */
    current_recall_id = NULL;
    current_recycling_context = NULL;    
    
    g_object_get(current_recall,
		 "recall-id", &current_recall_id,
		 NULL);

    /* get recycling context */
    if(current_recall_id != NULL){
      g_object_get(current_recall_id,
		   "recycling-context", &current_recycling_context,
		   NULL);
    }

    if(current_recall_id != NULL){
      g_object_unref(current_recall_id);
    }

    if(current_recycling_context != NULL){
      g_object_unref(current_recycling_context);
    }
    
    /* check recycling context */
    if(current_recycling_context == (AgsRecyclingContext *) recycling_context){
      return(recall);
    }

    /* iterate */
    recall = recall->next;
  }

  return(NULL);
}

/**
 * ags_recall_find_provider:
 * @recall: (element-type AgsAudio.Recall) (transfer none): the #GList-struct containing #AgsRecall
 * @provider: the #GObject, either #AgsAudio, #AgsChannel, #AgsRecycling or #AgsAudioSignal
 * 
 * Finds next matching recall for type which has @provider. The @provider may be either an #AgsChannel
 * or an #AgsAudio object. This function tries to find the corresponding #AgsRecallChannel and #AgsRecallAudio
 * objects of a #AgsRecall to find. If these recalls contains the @provider, the function will return.
 *
 * Returns: (element-type AgsAudio.Recall) (transfer none): next matching #GList-struct, or %NULL if not found
 *
 * Since: 3.0.0
 */
GList*
ags_recall_find_provider(GList *recall, GObject *provider)
{
  AgsAudio *current_audio;
  AgsChannel *current_channel;
  AgsRecycling *current_recycling;
  AgsAudioSignal *current_audio_signal;
  AgsRecall *current_recall;

  gboolean success;
  
  while(recall != NULL){
    current_recall = AGS_RECALL(recall->data);

    if(AGS_IS_AUDIO(provider)){
      if(AGS_IS_RECALL_AUDIO(current_recall)){
	g_object_get(current_recall,
		     "audio", &current_audio,
		     NULL);

	success = ((GObject *) current_audio == provider) ? TRUE: FALSE;

	if(current_audio != NULL){
	  g_object_unref(current_audio);
	}
	
	if(success){
	  return(recall);
	}
      }else if(AGS_IS_RECALL_AUDIO_RUN(current_recall)){
	g_object_get(current_recall,
		     "audio", &current_audio,
		     NULL);

	success = ((GObject *) current_audio == provider) ? TRUE: FALSE;

	if(current_audio != NULL){
	  g_object_unref(current_audio);
	}
	
	if(success){
	  return(recall);
	}
      }
    }else if(AGS_IS_CHANNEL(provider)){
      if(AGS_IS_RECALL_CHANNEL(current_recall)){
	g_object_get(current_recall,
		     "source", &current_channel,
		     NULL);

	success = ((GObject *) current_channel == provider) ? TRUE: FALSE;

	if(current_channel != NULL){
	  g_object_unref(current_channel);
	}
	
	if(success){
	  return(recall);
	}
      }else if(AGS_IS_RECALL_CHANNEL_RUN(current_recall)){
	g_object_get(current_recall,
		     "source", &current_channel,
		     NULL);

	success = ((GObject *) current_channel == provider) ? TRUE: FALSE;

	if(current_channel != NULL){
	  g_object_unref(current_channel);
	}
	
	if(success){
	  return(recall);
	}
      }
    }else if(AGS_IS_RECYCLING(provider)){
      if(AGS_IS_RECALL_RECYCLING(current_recall)){
	g_object_get(current_recall,
		     "source", &current_recycling,
		     NULL);

	success = ((GObject *) current_recycling == provider) ? TRUE: FALSE;

	if(current_recycling != NULL){
	  g_object_unref(current_recycling);
	}
	
	if(success){
	  return(recall);
	}
      }
    }else if(AGS_IS_AUDIO_SIGNAL(provider)){
      if(AGS_IS_RECALL_AUDIO_SIGNAL(current_recall)){
	g_object_get(current_recall,
		     "source", &current_audio_signal,
		     NULL);

	success = ((GObject *) current_audio_signal == provider) ? TRUE: FALSE;

	if(current_audio_signal != NULL){
	  g_object_unref(current_audio_signal);
	}
	
	if(success){
	  return(recall);
	}
      }
    }

    /* iterate */
    recall = recall->next;
  }

  return(NULL);
}

/**
 * ags_recall_template_find_provider:
 * @recall: (element-type AgsAudio.Recall) (transfer none): the #GList-struct containing #AgsRecall
 * @provider: the #GObject as provider
 * 
 * Finds provider eg. #AgsAudio or #AgsChannel within @recall containig #AgsRecall.
 *
 * Returns: (element-type AgsAudio.Recall) (transfer none): next matching #GList-struct, or %NULL if not found
 * 
 * Since: 3.0.0
 */
GList*
ags_recall_template_find_provider(GList *recall, GObject *provider)
{
  AgsRecall *current_recall;

  AgsRecallFlags current_recall_flags;
  
  GRecMutex *current_recall_mutex;

  while((recall = (ags_recall_find_provider(recall, provider))) != NULL){
    current_recall = AGS_RECALL(recall->data);  

    /* get recall mutex */
    current_recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(current_recall);

    /* get some fields */
    g_rec_mutex_lock(current_recall_mutex);

    current_recall_flags = current_recall->flags;
    
    g_rec_mutex_unlock(current_recall_mutex);

    /* check template */
    if((AGS_RECALL_TEMPLATE & (current_recall_flags)) != 0){
      return(recall);
    }

    /* iterate */
    recall = recall->next;
  }

  return(NULL);
}

/**
 * ags_recall_find_provider_with_recycling_context:
 * @recall: (element-type AgsAudio.Recall) (transfer none): the #GList-struct containing #AgsRecall
 * @provider: the #GObject as provider
 * @recycling_context: the #AgsRecyclingContext
 * 
 * Like ags_recall_template_find_provider() but given additionally @recycling_context as search parameter.
 *
 * Returns: (element-type AgsAudio.Recall) (transfer none): next matching #GList-struct, or %NULL if not found
 * 
 * Since: 3.0.0
 */
GList*
ags_recall_find_provider_with_recycling_context(GList *recall, GObject *provider, GObject *recycling_context)
{
  AgsRecall *current_recall;
  AgsRecallID *current_recall_id;
  AgsRecyclingContext *current_recycling_context;

  GRecMutex *current_recall_mutex;
  GRecMutex *current_recall_id_mutex;
  
  while((recall = ags_recall_find_provider(recall, provider)) != NULL){
    current_recall = AGS_RECALL(recall->data);
    
    /* get recall mutex */
    current_recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(current_recall);

    /* get some fields */
    g_rec_mutex_lock(current_recall_mutex);

    current_recall_id = current_recall->recall_id;
    
    g_rec_mutex_unlock(current_recall_mutex);

    /* get recycling context */
    current_recycling_context = NULL;
    
    if(current_recall_id != NULL){
      /* get recall id mutex */
      current_recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(current_recall_id);

      /* recycling context */
      g_rec_mutex_lock(current_recall_id_mutex);

      current_recycling_context = current_recall_id->recycling_context;
      
      g_rec_mutex_unlock(current_recall_id_mutex);
    }

    if(current_recycling_context == (AgsRecyclingContext *) recycling_context){
      return(recall);
    }

    /* iterate */
    recall = recall->next;
  }

  return(NULL);
}

void
ags_recall_child_done(AgsRecall *child,
		      AgsRecall *parent)
{
  GList *children;

  AgsSoundBehaviourFlags parent_behaviour_flags;
  
  /* remove child */
  ags_connectable_disconnect(AGS_CONNECTABLE(child));
  ags_recall_remove_child(parent,
			  child);

  if(TRUE){
    AgsDestroyWorker *destroy_worker;
    
    destroy_worker = ags_destroy_worker_get_instance();
    ags_destroy_worker_add(destroy_worker,
			   child, ags_destroy_util_dispose_and_unref);
  }else{
    g_object_run_dispose((GObject *) child);
    g_object_unref((GObject *) child);
  }
  
  g_object_get(parent,
	       "child", &children,
	       NULL);
  
  if(ags_recall_test_behaviour_flags(parent, AGS_SOUND_BEHAVIOUR_PROPAGATE_DONE) &&
     !ags_recall_test_behaviour_flags(parent, AGS_SOUND_BEHAVIOUR_PERSISTENT) &&
     children == NULL){
    ags_recall_done(parent);
  }

  g_list_free_full(children,
		   g_object_unref);
}

/**
 * ags_recall_lock_port:
 * @recall: the #AgsRecall
 *
 * Locks the ports.
 *
 * Since: 3.0.0
 */
void
ags_recall_lock_port(AgsRecall *recall)
{
  AgsPort *port;
  
  GList *list_start, *list;

  GRecMutex *recall_mutex;
  GRecMutex *port_mutex;

  if(!AGS_IS_RECALL(recall)){
    return;
  }  

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* get some fields */
  g_rec_mutex_lock(recall_mutex);

  list =
    list_start = g_list_copy(recall->port);

  g_rec_mutex_unlock(recall_mutex);

  while(list != NULL){
    port = list->data;
    
    /* get port mutex */
    port_mutex = AGS_PORT_GET_OBJ_MUTEX(port);

    /* lock port mutex */
    g_rec_mutex_lock(port_mutex);

    /* iterate */
    list = list->next;
  }

  g_list_free(list_start);
}

/**
 * ags_recall_unlock_port:
 * @recall: the #AgsRecall
 *
 * Unlocks the ports.
 *
 * Since: 3.0.0
 */
void
ags_recall_unlock_port(AgsRecall *recall)
{
  AgsPort *port;
  
  GList *list_start, *list;

  GRecMutex *recall_mutex;
  GRecMutex *port_mutex;

  if(!AGS_IS_RECALL(recall)){
    return;
  }  

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* get some fields */
  g_rec_mutex_lock(recall_mutex);

  list =
    list_start = g_list_copy(recall->port);

  g_rec_mutex_unlock(recall_mutex);

  while(list != NULL){
    port = list->data;

    /* get port mutex */
    port_mutex = AGS_PORT_GET_OBJ_MUTEX(port);

    /* lock port mutex */
    g_rec_mutex_unlock(port_mutex);

    /* iterate */
    list = list->next;
  }

  g_list_free(list_start);
}

/**
 * ags_recall_new:
 *
 * Instantiate #AgsRecall.
 *
 * Returns: the new instance of #AgsRecall.
 * 
 * Since: 3.0.0
 */
AgsRecall*
ags_recall_new()
{
  AgsRecall *recall;

  recall = (AgsRecall *) g_object_new(AGS_TYPE_RECALL,
				      NULL);

  return(recall);
}
