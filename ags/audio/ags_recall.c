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

#include <ags/audio/ags_recall.h>

#include <ags/libags.h>

#include <ags/plugin/ags_ladspa_manager.h>
#include <ags/plugin/ags_dssi_manager.h>
#include <ags/plugin/ags_lv2_manager.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_recycling_context.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_dependency.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_recall_ladspa.h>
#include <ags/audio/ags_recall_dssi.h>
#include <ags/audio/ags_recall_lv2.h>
#include <ags/audio/ags_recall_recycling.h>
#include <ags/audio/ags_recall_audio_signal.h>

#include <ags/i18n.h>

#include <libxml/tree.h>

#include <string.h>

void ags_recall_class_init(AgsRecallClass *recall_class);
void ags_recall_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_plugin_interface_init(AgsPluginInterface *plugin);
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

gchar* ags_recall_get_name(AgsPlugin *plugin);
void ags_recall_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_recall_get_version(AgsPlugin *plugin);
void ags_recall_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_recall_get_build_id(AgsPlugin *plugin);
void ags_recall_set_build_id(AgsPlugin *plugin, gchar *build_id);
gchar* ags_recall_get_xml_type(AgsPlugin *plugin);
void ags_recall_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
GList* ags_recall_get_ports(AgsPlugin *plugin);
void ags_recall_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_recall_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_recall_real_run_init_pre(AgsRecall *recall);
void ags_recall_real_run_init_inter(AgsRecall *recall);
void ags_recall_real_run_init_post(AgsRecall *recall);

void ags_recall_real_run_automate(AgsRecall *recall);
void ags_recall_real_run_pre(AgsRecall *recall);
void ags_recall_real_run_inter(AgsRecall *recall);
void ags_recall_real_run_post(AgsRecall *recall);

void ags_recall_real_stop_persistent(AgsRecall *recall);
void ags_recall_real_done(AgsRecall *recall);

void ags_recall_real_cancel(AgsRecall *recall);
void ags_recall_real_remove(AgsRecall *recall);

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
 * #AgsRecall acts as effect processor.
 */

enum{
  LOAD_AUTOMATION,
  UNLOAD_AUTOMATION,
  RESOLVE_DEPENDENCY,
  CHECK_RT_DATA,  
  RUN_INIT_PRE,
  RUN_INIT_INTER,
  RUN_INIT_POST,
  FEED_INPUT_QUEUE,
  AUTOMATE,
  RUN_PRE,
  RUN_INTER,
  RUN_POST,
  DO_FEEDBACK,
  FEED_OUTPUT_QUEUE,
  STOP_PERSISTENT,
  CANCEL,
  DONE,
  DUPLICATE,
  NOTIFY_DEPENDENCY,
  CHILD_ADDED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_RECALL_CONTAINER,
  PROP_OUTPUT_SOUNDCARD,
  PROP_OUTPUT_SOUNDCARD_CHANNEL,
  PROP_INPUT_SOUNDCARD,
  PROP_INPUT_SOUNDCARD_CHANNEL,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_PORT,
  PROP_AUTOMATION_PORT,
  PROP_RECALL_ID,
  PROP_RECALL_DEPENDENCY,
  PROP_PARENT,
  PROP_CHILD,
  PROP_CHILD_TYPE,
};

static gpointer ags_recall_parent_class = NULL;
static guint recall_signals[LAST_SIGNAL];

static pthread_mutex_t ags_recall_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_recall_get_type (void)
{
  static GType ags_type_recall = 0;

  if(!ags_type_recall){
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

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_recall_plugin_interface_init,
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

    g_type_add_interface_static(ags_type_recall,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_recall);
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
   * AgsRecall:recall-container:
   *
   * The #AgsRecallContainer packed into.
   * 
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * AgsChannel:output-soundcard-channel:
   *
   * The output soundcard channel.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_int("output-soundcard-channel",
				 i18n_pspec("output soundcard channel"),
				 i18n_pspec("The output soundcard channel"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT_SOUNDCARD_CHANNEL,
				  param_spec);

  /**
   * AgsRecall:input-soundcard:
   *
   * The assigned soundcard.
   * 
   * Since: 2.0.0
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
   * AgsChannel:input-soundcard-channel:
   *
   * The input soundcard channel.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_int("input-soundcard-channel",
				 i18n_pspec("input soundcard channel"),
				 i18n_pspec("The input soundcard channel"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT_SOUNDCARD_CHANNEL,
				  param_spec);

  /**
   * AgsChannel:samplerate:
   *
   * The samplerate.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_uint("samplerate",
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
   * AgsChannel:buffer-size:
   *
   * The buffer size.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_uint("buffer-size",
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
   * AgsChannel:format:
   *
   * The format.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_uint("format",
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
   * AgsRecall:recall-dependency:
   *
   * The assigned #AgsRecallDependency.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("recall-dependency",
				    i18n_pspec("recall dependency"),
				    i18n_pspec("The recall dependency that can be added"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_DEPENDENCY,
				  param_spec);

  /**
   * AgsRecall:port:
   *
   * The assigned #AgsPort
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("port",
				    i18n_pspec("port of recall"),
				    i18n_pspec("The port of recall"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT,
				  param_spec);

  /**
   * AgsRecall:automation-port:
   *
   * The #AgsPort doing automation.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("autamation-port",
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
   * Since: 2.0.0
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
   * AgsRecall:recall-dependency:
   *
   * The  #AgsRecall dependency.
   * 
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * AgsRecall:child:
   *
   * The child #AgsRecall.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("child",
				    i18n_pspec("child of recall"),
				    i18n_pspec("The child that can be added"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHILD,
				  param_spec);

  /**
   * AgsRecall:child-type:
   *
   * The type of child #AgsRecall.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_gtype("child-type",
				  i18n_pspec("child type"),
				  i18n_pspec("The type of child that can be added"),
				  AGS_TYPE_RECALL,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHILD_TYPE,
				  param_spec);
  
  /* AgsRecallClass */
  recall->load_automation = NULL;
  recall->unload_automation = NULL;

  recall->resolve_dependency = NULL;
  recall->check_rt_data = NULL;
  
  recall->run_init_pre = ags_recall_real_run_init_pre;
  recall->run_init_inter = ags_recall_real_run_init_inter;
  recall->run_init_post = ags_recall_real_run_init_post;

  recall->feed_input_queue = NULL;
  recall->automate = NULL;

  recall->run_pre = ags_recall_real_run_pre;
  recall->run_inter = ags_recall_real_run_inter;
  recall->run_post = ags_recall_real_run_post;

  recall->do_feedback = NULL;
  recall->feed_output_queue = NULL;

  recall->stop_persistent = ags_recall_real_stop_persistent;
  recall->cancel = ags_recall_real_cancel;
  recall->done = ags_recall_real_done;

  recall->duplicate = ags_recall_real_duplicate;

  recall->notify_dependency = NULL;

  recall->child_added = NULL;

  /* signals */
  /**
   * AgsRecall::load-automation:
   * @recall: the #AgsRecall to initialize
   *
   * The ::load-automation signal notifies about loading automation.
   *
   * Since: 2.0.0
   */
  recall_signals[LOAD_AUTOMATION] =
    g_signal_new("load-automation",
		 G_TYPE_FROM_CLASS(recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecallClass, load_automation),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__POINTER,
		 G_TYPE_NONE, 1,
		 G_TYPE_POINTER);

  /**
   * AgsRecall::unload-automation:
   * @recall: the #AgsRecall to initialize
   *
   * The ::unload-automation signal notifies about unloading automation.
   *
   * Since: 2.0.0
   */
  recall_signals[UNLOAD_AUTOMATION] =
    g_signal_new("unload-automation",
		 G_TYPE_FROM_CLASS(recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecallClass, unload_automation),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsRecall::resolve-dependency:
   * @recall: the #AgsRecall to resolve
   *
   * The ::resolve-dependency signal notifies about resolving
   * dependency.
   *
   * Since: 2.0.0
   */
  recall_signals[RESOLVE_DEPENDENCY] =
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
   * Since: 2.0.0
   */
  recall_signals[CHECK_RT_DATA] =
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
   * Since: 2.0.0
   */
  recall_signals[RUN_INIT_PRE] =
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
   * Since: 2.0.0
   */
  recall_signals[RUN_INIT_INTER] =
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
   * Since: 2.0.0
   */
  recall_signals[RUN_INIT_POST] =
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
   * Since: 2.0.0
   */
  recall_signals[FEED_INPUT_QUEUE] =
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
   * Since: 2.0.0
   */
  recall_signals[AUTOMATE] =
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
   * Since: 2.0.0
   */
  recall_signals[RUN_PRE] =
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
   * Since: 2.0.0
   */
  recall_signals[RUN_INTER] =
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
   * Since: 2.0.0
   */
  recall_signals[RUN_POST] =
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
   * Since: 2.0.0
   */
  recall_signals[DO_FEEDBACK] =
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
   * Since: 2.0.0
   */
  recall_signals[FEED_OUTPUT_QUEUE] =
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
   * Since: 2.0.0
   */
  recall_signals[STOP_PERSISTENT] =
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
   * Since: 2.0.0
   */
  recall_signals[CANCEL] =
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
   * Since: 2.0.0
   */
  recall_signals[DONE] =
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
   * Since: 2.0.0
   */
  recall_signals[DUPLICATE] =
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
   * @count: the reference count
   *
   * The ::notify-dependency signal notifies about dependency
   * added.
   *
   * Since: 2.0.0
   */
  recall_signals[NOTIFY_DEPENDENCY] =
    g_signal_new("notify-dependency",
		 G_TYPE_FROM_CLASS(recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsRecallClass, notify_dependency),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_INT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT, G_TYPE_INT);

  /**
   * AgsRecall::child-added:
   * @recall: the #AgsRecall to add the child
   * @child: the #AgsRecall to add
   *
   * The ::child-added signal notifies about children
   * added.
   *
   * Since: 2.0.0
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
ags_recall_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_recall_get_name;
  plugin->set_name = ags_recall_set_name;
  plugin->get_version = ags_recall_get_version;
  plugin->set_version = ags_recall_set_version;
  plugin->get_build_id = ags_recall_get_build_id;
  plugin->set_build_id = ags_recall_set_build_id;
  plugin->get_xml_type = ags_recall_get_xml_type;
  plugin->set_xml_type = ags_recall_set_xml_type;
  plugin->get_ports = ags_recall_get_ports;
  plugin->read = ags_recall_read;
  plugin->write = ags_recall_write;
  plugin->set_ports = NULL;
}

void
ags_recall_init(AgsRecall *recall)
{  
  AgsMutexManager *mutex_manager;

  AgsConfig *config;

  gchar *str;
  gchar *str0, *str1;

  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  recall->flags = 0;
  recall->ability_flags = 0;
  recall->behaviour_flags = 0;
  recall->sound_scope = -1;
  recall->staging_flags = 0;
  recall->state_flags = 0;

  /* add recall mutex */
  recall->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  recall->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);
    
  /* uuid */
  recall->uuid = ags_uuid_alloc();
  ags_uuid_generate(recall->uuid);

  /* version and build id */
  recall->version = NULL;
  recall->build_id = NULL;

  /* effect and name */
  recall->effect = NULL;
  recall->name = NULL;

  /* xml type  */
  recall->xml_type = NULL;

  /* config */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  config = ags_config_get_instance();

  /* base init */
  recall->recall_container = NULL;

  recall->output_soundcard = NULL;
  recall->output_soundcard_channel = 0;

  recall->input_soundcard = NULL;
  recall->input_soundcard_channel = 0;

  /* presets */
  recall->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  recall->buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  recall->format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  /* read config */
  pthread_mutex_lock(application_mutex);

  /* samplerate */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "samplerate");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "samplerate");
  }
  
  if(str != NULL){
    recall->samplerate = g_ascii_strtoull(str,
					  NULL,
					  10);

    free(str);
  }

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
    recall->buffer_size = g_ascii_strtoull(str,
					   NULL,
					   10);

    free(str);
  }
  
  /* format */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "format");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "format");
  }
  
  if(str != NULL){
    recall->format = g_ascii_strtoull(str,
				      NULL,
				      10);

    free(str);
  }

  pthread_mutex_unlock(application_mutex);

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

  pthread_mutex_t *recall_mutex;

  recall = AGS_RECALL(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = recall->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_RECALL_CONTAINER:
    {
      AgsRecallContainer *recall_container;

      recall_container = (AgsRecallContainer *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);
      
      if(recall->recall_container == recall_container){
	pthread_mutex_unlock(recall_mutex);
	
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

      pthread_mutex_unlock(recall_mutex);
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
      pthread_mutex_lock(recall_mutex);

      recall->output_soundcard_channel = g_value_get_int(value);

      pthread_mutex_unlock(recall_mutex);
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
      pthread_mutex_lock(recall_mutex);

      recall->input_soundcard_channel = g_value_get_int(value);

      pthread_mutex_unlock(recall_mutex);
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
      guint format;

      format = g_value_get_uint(value);

      ags_recall_set_format(recall,
			    format);
    }
    break;
  case PROP_PORT:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_pointer(value);

      pthread_mutex_lock(recall_mutex);

      if(!AGS_IS_PORT(port) ||
	 g_list_find(recall->port, port) != NULL){
	pthread_mutex_unlock(recall_mutex);
	
	return;
      }

      g_object_ref(port);
      recall->port = g_list_prepend(recall->port,
				    port);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_AUTOMATION_PORT:
    {
      AgsPort *automation_port;

      automation_port = (AgsPort *) g_value_get_pointer(value);

      pthread_mutex_lock(recall_mutex);

      if(!AGS_IS_PORT(automation_port) ||
	 g_list_find(recall->automation_port, automation_port) != NULL){
	pthread_mutex_unlock(recall_mutex);
	
	return;
      }

      g_object_ref(automation_port);
      recall->port = g_list_prepend(recall->automation_port,
				    automation_port);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RECALL_ID:
    {
      AgsRecallID *recall_id;

      recall_id = (AgsRecallID *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(recall->recall_id == recall_id){
	pthread_mutex_unlock(recall_mutex);
	
	return;
      }

      pthread_mutex_unlock(recall_mutex);
	
      ags_recall_set_recall_id(recall, recall_id);
    }
    break;
  case PROP_RECALL_DEPENDENCY:
    {
      AgsRecallDependency *recall_dependency;

      recall_dependency = (AgsRecallDependency *) g_value_get_pointer(value);

      pthread_mutex_lock(recall_mutex);
      
      if(!AGS_IS_RECALL_DEPENDENCY(recall_dependency) ||
	 g_list_find(recall->recall_dependency, recall_dependency) != NULL){
	pthread_mutex_unlock(recall_mutex);
	
	return;
      }
      
      ags_recall_add_recall_dependency(recall, recall_dependency);
    }
    break;
  case PROP_PARENT:
    {
      AgsRecall *parent;
      
      parent = (AgsRecall *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(recall->parent == parent){
	pthread_mutex_unlock(recall_mutex);
	
	return;
      }

      if(recall->parent != NULL){
	g_object_unref(recall->parent);
      }

      if(parent != NULL){
	g_object_ref(parent);
      }
      
      recall->parent = parent;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_CHILD_TYPE:
    {
      recall->child_type = g_value_get_gtype(value);
    }
    break;
  case PROP_CHILD:
    {
      AgsRecall *child;
      
      child = (AgsRecall *) g_value_get_pointer(value);

      pthread_mutex_lock(recall_mutex);

      if(child == NULL ||
	 g_list_find(recall->children, child) != NULL){
	pthread_mutex_unlock(recall_mutex);
	
	return;
      }

      pthread_mutex_unlock(recall_mutex);
      
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

  pthread_mutex_t *recall_mutex;

  recall = AGS_RECALL(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = recall->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_RECALL_CONTAINER:
    {
      pthread_mutex_lock(recall_mutex);
      
      g_value_set_object(value, recall->recall_container);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_OUTPUT_SOUNDCARD:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, recall->output_soundcard);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_OUTPUT_SOUNDCARD_CHANNEL:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_int(value, recall->output_soundcard_channel);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_INPUT_SOUNDCARD:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value,
			 recall->input_soundcard);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_INPUT_SOUNDCARD_CHANNEL:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_int(value, recall->input_soundcard_channel);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_uint(value, recall->samplerate);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_uint(value, recall->buffer_size);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_uint(value, recall->format);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_PORT:
    {
      pthread_mutex_lock(recall_mutex);
      
      g_value_set_pointer(value, g_list_copy(recall->port));

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_AUTOMATION_PORT:
    {
      pthread_mutex_lock(recall_mutex);
      
      g_value_set_pointer(value, g_list_copy(recall->automation_port));

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RECALL_ID:
    {
      pthread_mutex_lock(recall_mutex);
      
      g_value_set_object(value, recall->recall_id);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RECALL_DEPENDENCY:
    {
      pthread_mutex_lock(recall_mutex);
      
      g_value_set_pointer(value, g_list_copy(recall->recall_dependency));

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_PARENT:
    {
      pthread_mutex_lock(recall_mutex);
      
      g_value_set_object(value, recall->parent);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_CHILD_TYPE:
    {
      pthread_mutex_lock(recall_mutex);
      
      g_value_set_gtype(value,
			recall->child_type);

      pthread_mutex_unlock(recall_mutex);
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

  GList *list_start, *list;

  recall = AGS_RECALL(gobject);

  ags_connectable_disconnect(AGS_CONNECTABLE(recall));
  
  /* recall container */
  if(recall->recall_container != NULL){
    g_object_unref(recall->recall_container);
    
    recall->recall_container = NULL;
  }

  /* output/input soundcard */
  if(recall->output_soundcard != NULL){
    g_object_unref(recall->output_soundcard);

    recall->output_soundcard = NULL;
  }

  if(recall->input_soundcard != NULL){
    g_object_unref(recall->input_soundcard);

    recall->input_soundcard = NULL;
  }
  
  /* port */
  if(recall->port != NULL){
    g_list_free_full(recall->port,
		     g_object_unref);

    recall->port = NULL;
  }
  
  /* automation port */
  if(recall->automation_port != NULL){
    g_list_free_full(recall->automation_port,
		     g_object_unref);

    recall->automation_port = NULL;
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
      list_start = g_list_copy(recall->children);

    while(list != NULL){      
      g_object_run_dispose(G_OBJECT(list->data));

      list = list->next;
    }

    g_list_free(list_start);
    g_list_free_full(recall->children,
		     g_object_unref);

    recall->children = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_parent_class)->dispose(gobject);
}

void
ags_recall_finalize(GObject *gobject)
{
  AgsRecall *recall;
  
  guint i;
  
  recall = AGS_RECALL(gobject);

#ifdef AGS_DEBUG
  g_message("finalize %s\n", G_OBJECT_TYPE_NAME(gobject));
#endif

  pthread_mutex_destroy(recall->obj_mutex);
  free(recall->obj_mutex);

  pthread_mutexattr_destroy(recall->obj_mutexattr);
  free(recall->obj_mutexattr);

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
  
  /* output/input soundcard */
  if(recall->output_soundcard != NULL){
    g_object_unref(recall->output_soundcard);
  }

  if(recall->input_soundcard != NULL){
    g_object_unref(recall->input_soundcard);
  }

  /* port */
  g_list_free_full(recall->port,
		   g_object_unref);

  /* automation port */
  g_list_free_full(recall->automation_port,
		   g_object_unref);

  /* recall id */
  if(recall->recall_id != NULL){
    g_object_unref(recall->recall_id);
  }

  /* recall dependency */
  g_list_free_full(recall->recall_dependency,
		   g_object_unref);

  /* recall handler */
  g_list_free_full(recall->recall_handler,
		   ags_recall_handler_free);

  /* parent */
  if(recall->parent != NULL){
    g_object_unref(recall->parent);
  }

  /* children */
  if(recall->child_parameter_name != NULL){
    g_strfreev(recall->child_parameter_name);
  }

  if(recall->child_value != NULL){
    for(i = 0; i < recall->n_child_params; i++){
      g_value_unset(&(recall->child_value[i]));
    }
    
    g_free(recall->child_value);
  }

  g_list_free_full(recall->children,
		   g_object_unref);
  
  /* call parent */
  G_OBJECT_CLASS(ags_recall_parent_class)->finalize(gobject);
}

AgsUUID*
ags_recall_get_uuid(AgsConnectable *connectable)
{
  AgsRecall *recall;
  
  AgsUUID *ptr;

  pthread_mutex_t *recall_mutex;

  recall = AGS_RECALL(connectable);

  /* get recall signal mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = recall->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get UUID */
  pthread_mutex_lock(recall_mutex);

  ptr = recall->uuid;

  pthread_mutex_unlock(recall_mutex);
  
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

  pthread_mutex_t *recall_mutex;

  recall = AGS_RECALL(connectable);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = recall->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* check is added */
  pthread_mutex_lock(recall_mutex);

  is_ready = (((AGS_RECALL_ADDED_TO_REGISTRY & (recall->flags)) != 0) ? TRUE: FALSE);

  pthread_mutex_unlock(recall_mutex);
  
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

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  recall = AGS_RECALL(connectable);

  ags_recall_set_flags(recall, AGS_RECALL_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(&(entry->entry),
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

  pthread_mutex_t *recall_mutex;

  recall = AGS_RECALL(connectable);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = recall->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* check is connected */
  pthread_mutex_lock(recall_mutex);

  is_connected = (((AGS_RECALL_CONNECTED & (recall->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(recall_mutex);
  
  return(is_connected);
}

void
ags_recall_connect(AgsConnectable *connectable)
{
  AgsRecall *recall;

  GList *list_start, *list;

  pthread_mutex_t *recall_mutex;

  if(ags_connectable_is_connect(connectable)){
    return;
  }

  recall = AGS_RECALL(connectable);

  ags_recall_set_flags(recall, AGS_RECALL_CONNECTED);  
  
  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = recall->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* connect children */
  pthread_mutex_lock(recall_mutex);

  list =
    list_start = g_list_copy(recall->children);

  pthread_mutex_unlock(recall_mutex);

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free(list_start);
  
  /* recall handler */
  pthread_mutex_lock(recall_mutex);

  list =
    list_start = g_list_copy(recall->recall_handler);

  pthread_mutex_unlock(recall_mutex);

  while(list != NULL){
    AgsRecallHandler *recall_handler;
    
    recall_handler = AGS_RECALL_HANDLER(list->data);
    g_signal_connect_after(G_OBJECT(recall), recall_handler->signal_name,
			   G_CALLBACK(recall_handler->callback), recall_handler->data);
    
    list = list->next;
  }

  g_list_free(list_start);

  /* load automation */
  ags_recall_load_automation(recall);
}

void
ags_recall_disconnect(AgsConnectable *connectable)
{
  AgsRecall *recall;

  GList *list_start, *list;

  pthread_mutex_t *recall_mutex;

  if(!ags_connectable_is_connect(connectable)){
    return;
  }

  recall = AGS_RECALL(connectable);

  ags_recall_unset_flags(recall, AGS_RECALL_CONNECTED);    

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = recall->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* connect children */
  pthread_mutex_lock(recall_mutex);

  list =
    list_start = g_list_copy(recall->children);

  pthread_mutex_unlock(recall_mutex);

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free(list_start);
  
  /* recall handler */
  pthread_mutex_lock(recall_mutex);

  list =
    list_start = g_list_copy(recall->recall_handler);

  pthread_mutex_unlock(recall_mutex);

  while(list != NULL){
    AgsRecallHandler *recall_handler;

    gchar *signal_name;
    
    recall_handler = AGS_RECALL_HANDLER(list->data);

    signal_name = g_strdup_printf("any_signal::%s",
				  recall_handler->signal_name);
    g_object_disconnect(G_OBJECT(recall),
			signal_name,
			G_CALLBACK(recall_handler->callback),
			recall_handler->data,
			NULL);

    g_free(signal_name);

    list = list->next;
  }

  g_list_free(list_start);

  /* unload automation */
  ags_recall_unload_automation(recall);
}

gchar*
ags_recall_get_name(AgsPlugin *plugin)
{
  return(AGS_RECALL(plugin)->name);
}

void
ags_recall_set_name(AgsPlugin *plugin, gchar *name)
{
  AGS_RECALL(plugin)->name = name;
}

gchar*
ags_recall_get_version(AgsPlugin *plugin)
{
  return(AGS_RECALL(plugin)->version);
}

void
ags_recall_set_version(AgsPlugin *plugin, gchar *version)
{
  AGS_RECALL(plugin)->version = version;
}

gchar*
ags_recall_get_build_id(AgsPlugin *plugin)
{
  return(AGS_RECALL(plugin)->build_id);
}

void
ags_recall_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AGS_RECALL(plugin)->build_id = build_id;
}

gchar*
ags_recall_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_RECALL(plugin)->xml_type);
}

void
ags_recall_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_RECALL(plugin)->xml_type = xml_type;
}

GList*
ags_recall_get_ports(AgsPlugin *plugin)
{
  return(AGS_RECALL(plugin)->port);
}

void
ags_recall_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsRecall *recall;

  recall = AGS_RECALL(plugin);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference", recall,
				   NULL));
}

xmlNode*
ags_recall_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsRecall *recall;
  xmlNode *node;
  gchar *id;

  recall = AGS_RECALL(plugin);

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    AGS_RECALL(plugin)->xml_type);
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", recall,
				   NULL));

  xmlAddChild(parent,
	      node);

  return(node);
}

/**
 * ags_recall_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_recall_get_class_mutex()
{
  return(&ags_recall_class_mutex);
}

/**
 * ags_recall_set_flags:
 * @recall: the #AgsRecall
 * @flags: flags
 *
 * Set flags.
 * 
 * Since: 2.0.0
 */
void
ags_recall_set_flags(AgsRecall *recall, guint flags)
{
  if(!AGS_IS_RECALL(recall)){
    return;
  }

  /* set flags */
  recall->flags |= flags;
}

/**
 * ags_recall_set_ability_flags:
 * @recall: the #AgsRecall
 * @ability_flags: ability flags
 *
 * Set ability flags recursively.
 * 
 * Since: 2.0.0
 */
void
ags_recall_set_ability_flags(AgsRecall *recall, guint ability_flags)
{
  GList *child;

  if(!AGS_IS_RECALL(recall)){
    return;
  }

  recall->ability_flags |= ability_flags;

  /* apply recursivly */
  child = recall->children;

  while(child != NULL){
    ags_recall_set_ability_flags(AGS_RECALL(child->data), ability_flags);

    child = child->next;
  }
}

/**
 * ags_recall_unset_ability_flags:
 * @recall: the #AgsRecall
 * @ability_flags: ability flags
 *
 * Unset ability flags recursively.
 * 
 * Since: 2.0.0
 */
void
ags_recall_unset_ability_flags(AgsRecall *recall, guint ability_flags)
{
  GList *child;

  if(!AGS_IS_RECALL(recall)){
    return;
  }

  recall->ability_flags &= (~ability_flags);

  /* apply recursivly */
  child = recall->children;

  while(child != NULL){
    ags_recall_set_ability_flags(AGS_RECALL(child->data), ability_flags);

    child = child->next;
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
 * Since: 2.0.0
 */
gboolean
ags_recall_check_ability_flags(AgsRecall *recall, guint ability_flags)
{
  if(!AGS_IS_RECALL(recall)){
    return(FALSE);
  }

  if((AGS_SOUND_ABILITY_PLAYBACK & (ability_flags)) != 0 &&
     (AGS_SOUND_ABILITY_PLAYBACK & (recall->ability_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_ABILITY_NOTATION & (ability_flags)) != 0 &&
     (AGS_SOUND_ABILITY_NOTATION & (recall->ability_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_ABILITY_SEQUENCER & (ability_flags)) != 0 &&
     (AGS_SOUND_ABILITY_SEQUENCER & (recall->ability_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_ABILITY_WAVE & (ability_flags)) != 0 &&
     (AGS_SOUND_ABILITY_WAVE & (recall->ability_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_ABILITY_MIDI & (ability_flags)) != 0 &&
     (AGS_SOUND_ABILITY_MIDI & (recall->ability_flags)) == 0){
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
 * Since: 2.0.0
 */
gboolean
ags_recall_match_ability_flags_to_scope(AgsRecall *recall, gint sound_scope)
{
  if(!AGS_IS_RECALL(recall)){
    return(FALSE);
  }

  switch(sound_scope){
  case AGS_SOUND_SCOPE_PLAYBACK:
    {
      if((AGS_SOUND_ABILITY_PLAYBACK & (recall->ability_flags)) != 0){
	return(TRUE);
      }else{
	return(FALSE);
      }
    }
  case AGS_SOUND_SCOPE_NOTATION:
    {
      if((AGS_SOUND_ABILITY_NOTATION & (recall->ability_flags)) != 0){
	return(TRUE);
      }else{
	return(FALSE);
      }
    }
  case AGS_SOUND_SCOPE_SEQUENCER:
    {
      if((AGS_SOUND_ABILITY_SEQUENCER & (recall->ability_flags)) != 0){
	return(TRUE);
      }else{
	return(FALSE);
      }
    }
  case AGS_SOUND_SCOPE_WAVE:
    {
      if((AGS_SOUND_ABILITY_WAVE & (recall->ability_flags)) != 0){
	return(TRUE);
      }else{
	return(FALSE);
      }
    }
  case AGS_SOUND_SCOPE_MIDI:
    {
      if((AGS_SOUND_ABILITY_MIDI & (recall->ability_flags)) != 0){
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
 * ags_recall_set_behaviour_flags:
 * @recall: the #AgsRecall
 * @behaviour_flags: the behaviour flags
 * 
 * Set behaviour flags of @recall.
 * 
 * Since: 2.0.0
 */
void
ags_recall_set_behaviour_flags(AgsRecall *recall, guint behaviour_flags)
{
  if(!AGS_IS_RECALL(recall)){
    return;
  }

  recall->behaviour_flags |= behaviour_flags;
}

/**
 * ags_recall_unset_behaviour_flags:
 * @recall: the #AgsRecall
 * @behaviour_flags: the behaviour flags
 * 
 * Unset behaviour flags of @recall.
 * 
 * Since: 2.0.0
 */
void
ags_recall_unset_behaviour_flags(AgsRecall *recall, guint behaviour_flags)
{
  if(!AGS_IS_RECALL(recall)){
    return;
  }

  recall->behaviour_flags &= (~behaviour_flags);
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
 * Since: 2.0.0
 */
gboolean
ags_recall_check_behaviour_flags(AgsRecall *recall, guint behaviour_flags)
{
  if(!AGS_IS_RECALL(recall)){
    return(FALSE);
  }

  if((AGS_SOUND_BEHAVIOUR_PATTERN_MODE & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_PATTERN_MODE & (recall->behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_BULK_MODE & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_BULK_MODE & (recall->behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING & (recall->behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_OUTPUT & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_OUTPUT & (recall->behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT & (recall->behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUTPUT & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_CHAINED_TO_OUPUT & (recall->behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_CHAINED_TO_INPUT & (recall->behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_PERSISTENT & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_PERSISTENT & (recall->behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_PERSISTENT_PLAYBACK & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_PERSISTENT_PLAYBACK & (recall->behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_PERSISTENT_NOTATION & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_PERSISTENT_NOTATION & (recall->behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_PERSISTENT_SEQUENCER & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_PERSISTENT_SEQUENCER & (recall->behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_PERSISTENT_WAVE & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_PERSISTENT_WAVE & (recall->behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_PERSISTENT_MIDI & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_PERSISTENT_MIDI & (recall->behaviour_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_BEHAVIOUR_PROPAGATE_DONE & (behaviour_flags)) != 0 &&
     (AGS_SOUND_BEHAVIOUR_PROPAGATE_DONE & (recall->behaviour_flags)) == 0){
    return(FALSE);
  }

  return(TRUE);
}

/**
 * ags_recall_set_scope:
 * @recall: the #AgsRecall
 * @sound_scope: the sound scope
 * 
 * Set @sound_scope for @recall.
 * 
 * Since: 2.0.0
 */
void
ags_recall_set_sound_scope(AgsRecall *recall, gint sound_scope)
{
  if(!AGS_IS_RECALL(recall) &&
     ags_recall_check_scope(recall,
			    -1)){
    return;
  }

  recall->sound_scope = sound_scope;
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
 * Since: 2.0.0
 */
gboolean
ags_recall_check_sound_scope(AgsRecall *recall, gint sound_scope)
{
  if(!AGS_IS_RECALL(recall)){
    return(FALSE);
  }

  if(sound_scope < 0){
    switch(recall->sound_scope){
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
       sound_scope == recall->sound_scope){
      return(TRUE);
    }else{
      return(FALSE);
    }
  }
}

/**
 * ags_recall_set_staging_flags:
 * @recall: the #AgsRecall
 * @staging_flags: staging flags to set
 * 
 * Set staging flags.
 * 
 * Since: 2.0.0
 */
void
ags_recall_set_staging_flags(AgsRecall *recall, guint staging_flags)
{
  if(!AGS_IS_RECALL(recall)){
    return;
  }
  
  if((AGS_SOUND_STAGING_FINI & (recall->staging_flags)) == 0){
    if((AGS_SOUND_STAGING_CHECK_RT_DATA & (staging_flags)) != 0 &&
       (AGS_SOUND_STAGING_CHECK_RT_DATA & (recall->staging_flags)) == 0){    
      ags_recall_check_rt_data(recall);
    }

    if((AGS_SOUND_STAGING_RUN_INIT_PRE & (staging_flags)) != 0 &&
       (AGS_SOUND_STAGING_RUN_INIT_PRE & (recall->staging_flags)) == 0){
      ags_recall_run_init_pre(recall);
    }

    if((AGS_SOUND_STAGING_RUN_INIT_INTER & (staging_flags)) != 0 &&
       (AGS_SOUND_STAGING_RUN_INIT_INTER & (recall->staging_flags)) == 0){
      ags_recall_run_init_inter(recall);
    }

    if((AGS_SOUND_STAGING_RUN_INIT_POST & (staging_flags)) != 0 &&
       (AGS_SOUND_STAGING_RUN_INIT_POST & (recall->staging_flags)) == 0){
      ags_recall_run_init_post(recall);
    }
  
    if((AGS_SOUND_STAGING_FEED_INPUT_QUEUE & (staging_flags)) != 0 &&
       (AGS_SOUND_STAGING_FEED_INPUT_QUEUE & (recall->staging_flags)) == 0){
      ags_recall_run_init_feed_input_queue(recall);
    }

    if((AGS_SOUND_STAGING_AUTOMATE & (staging_flags)) != 0 &&
       (AGS_SOUND_STAGING_AUTOMATE & (recall->staging_flags)) == 0){
      ags_recall_run_init_automate(recall);
    }

    if((AGS_SOUND_STAGING_RUN_PRE & (staging_flags)) != 0 &&
       (AGS_SOUND_STAGING_RUN_PRE & (recall->staging_flags)) == 0){
      ags_recall_run_pre(recall);
    }

    if((AGS_SOUND_STAGING_RUN_INTER & (staging_flags)) != 0 &&
       (AGS_SOUND_STAGING_RUN_INTER & (recall->staging_flags)) == 0){
      ags_recall_run_inter(recall);
    }

    if((AGS_SOUND_STAGING_RUN_POST & (staging_flags)) != 0 &&
       (AGS_SOUND_STAGING_RUN_POST & (recall->staging_flags)) == 0){
      ags_recall_run_post(recall);
    }

    if((AGS_SOUND_STAGING_DO_FEEDBACK & (staging_flags)) != 0 &&
       (AGS_SOUND_STAGING_DO_FEEDBACK & (recall->staging_flags)) == 0){
      ags_recall_run_init_do_feedback(recall);
    }

    if((AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE & (staging_flags)) != 0 &&
       (AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE & (recall->staging_flags)) == 0){
      ags_recall_run_init_feed_output_queue(recall);
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
     (AGS_SOUND_STAGING_CANCEL & (recall->staging_flags)) == 0){
    ags_recall_set_state_flags(recall,
			       AGS_SOUND_STATE_IS_TERMINATING);

    ags_recall_run_init_cancel(recall);
  }
  
  if((AGS_SOUND_STAGING_DONE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_DONE & (recall->staging_flags)) == 0){
    ags_recall_run_init_done(recall);
  }

  if((AGS_SOUND_STAGING_REMOVE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_REMOVE & (recall->staging_flags)) == 0){
    ags_recall_run_init_remove(recall);
  }
  
  recall->staging_flags |= staging_flags;
}

/**
 * ags_recall_unset_staging_flags:
 * @recall: the #AgsRecall
 * @staging_flags: staging flags to unset
 * 
 * Unset staging flags.
 * 
 * Since: 2.0.0
 */
void
ags_recall_unset_staging_flags(AgsRecall *recall, guint staging_flags)
{
  if(!AGS_IS_RECALL(recall)){
    return;
  }

  recall->staging_flags &= (~staging_flags);
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
 * Since: 2.0.0
 */
gboolean
ags_recall_check_staging_flags(AgsRecall *recall, guint staging_flags)
{
  if(!AGS_IS_RECALL(recall)){
    return(FALSE);
  }

  if((AGS_SOUND_STAGING_CHECK_RT & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_CHECK_RT & (recall->staging_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STAGING_RUN_INIT_PRE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_INIT_PRE & (recall->staging_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STAGING_RUN_INIT_INTER & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_INIT_INTER & (recall->staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_RUN_INIT_POST & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_INIT_POST & (recall->staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_FEED_INPUT_QUEUE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_FEED_INPUT_QUEUE & (recall->staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_AUTOMATE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_AUTOMATE & (recall->staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_RUN_PRE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_PRE & (recall->staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_RUN_INTER & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_INTER & (recall->staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_RUN_POST & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_RUN_POST & (recall->staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_DO_FEEDBACK & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_DO_FEEDBACK & (recall->staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_FEED_OUTPUT_QUEUE & (recall->staging_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STAGING_FINI & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_FINI & (recall->staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_CANCEL & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_CANCEL & (recall->staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_DONE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_DONE & (recall->staging_flags)) == 0){
    return(FALSE);
  }
  
  if((AGS_SOUND_STAGING_REMOVE & (staging_flags)) != 0 &&
     (AGS_SOUND_STAGING_REMOVE & (recall->staging_flags)) == 0){
    return(FALSE);
  }
  
  return(TRUE);
}

/**
 * ags_recall_set_state_flags:
 * @recall: the #AgsRecall
 * @state_flags: state flags to set
 * 
 * Set state flags.
 * 
 * Since: 2.0.0
 */
void
ags_recall_set_state_flags(AgsRecall *recall, guint state_flags)
{
  if(!AGS_IS_RECALL(recall)){
    return;
  }

  recall->state_flags |= state_flags;
}

/**
 * ags_recall_unset_state_flags:
 * @recall: the #AgsRecall
 * @state_flags: state flags to unset
 * 
 * Unset state flags.
 * 
 * Since: 2.0.0
 */
void
ags_recall_unset_state_flags(AgsRecall *recall, guint state_flags)
{
  if(!AGS_IS_RECALL(recall)){
    return;
  }

  recall->state_flags &= (~state_flags);
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
 * Since: 2.0.0
 */
gboolean
ags_recall_check_state_flags(AgsRecall *recall, guint state_flags)
{
  if(!AGS_IS_RECALL(recall)){
    return(FALSE);
  }

  if((AGS_SOUND_STATE_IS_WAITING & (state_flags)) != 0 &&
     (AGS_SOUND_STATE_IS_WAITING & (recall->state_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STATE_IS_ACTIVE & (state_flags)) != 0 &&
     (AGS_SOUND_STATE_IS_ACTIVE & (recall->state_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STATE_IS_PROCESSING & (state_flags)) != 0 &&
     (AGS_SOUND_STATE_IS_PROCESSING & (recall->state_flags)) == 0){
    return(FALSE);
  }

  if((AGS_SOUND_STATE_IS_TERMINATING & (state_flags)) != 0 &&
     (AGS_SOUND_STATE_IS_TERMINATING & (recall->state_flags)) == 0){
    return(FALSE);
  }
  
  return(TRUE);
}

/**
 * ags_recall_load_automation:
 * @recall: an #AgsRecall
 * @automation_port: a #GList-struct containing #AgsPort
 *
 * A signal indicating that additional automation will be loaded from
 * @automation_port.
 * 
 * Since: 1.0.0
 */
void
ags_recall_load_automation(AgsRecall *recall,
			   GList *automation_port)
{
  g_return_if_fail(AGS_IS_RECALL(recall));
  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[LOAD_AUTOMATION], 0,
		automation_port);
  g_object_unref(G_OBJECT(recall));
}

/**
 * ags_recall_unload_automation:
 * @recall: an #AgsRecall
 *
 * A signal indicating that the automation will be unloaded.
 * 
 * Since: 1.0.0
 */
void
ags_recall_unload_automation(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));  
  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[UNLOAD_AUTOMATION], 0);
  g_object_unref(G_OBJECT(recall));
}

/**
 * ags_recall_resolve_dependency:
 * @recall: an #AgsRecall
 *
 * A signal indicating that the inheriting object should resolve
 * it's dependency.
 * 
 * Since: 1.0.0
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
		recall_signals[RESOLVE_DEPENDENCY], 0);
  g_object_unref(G_OBJECT(recall));
}

/**
 * ags_recall_child_added:
 * @parent: an #AgsRecall
 * @child: another #AgsRecall
 *
 * A signal indicating that the a child has been added.
 * 
 * Since: 1.0.0
 */
void
ags_recall_child_added(AgsRecall *parent, AgsRecall *child)
{
  g_return_if_fail(AGS_IS_RECALL(parent));
  g_object_ref(G_OBJECT(parent));
  g_signal_emit(G_OBJECT(parent),
		recall_signals[CHILD_ADDED], 0,
		child);
  g_object_unref(G_OBJECT(parent));
}

void
ags_recall_real_run_init_pre(AgsRecall *recall)
{
  GList *list, *list_next;

  list = recall->children;

  while(list != NULL){
    list_next = list->next;

    if((AGS_RECALL_TEMPLATE & (AGS_RECALL(list->data)->flags)) != 0){
      g_warning("running on template");
    }

    ags_recall_run_init_pre(AGS_RECALL(list->data));

    list = list_next;
  }
}

/**
 * ags_recall_run_init_pre:
 * @recall: an #AgsRecall
 *
 * Prepare for run, this is the pre stage within the preparation.
 * 
 * Since: 1.0.0
 */
void
ags_recall_run_init_pre(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[RUN_INIT_PRE], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_run_init_inter(AgsRecall *recall)
{
  GList *list, *list_next;

  list = recall->children;

  while(list != NULL){
    list_next = list->next;
    
    if((AGS_RECALL_TEMPLATE & (AGS_RECALL(list->data)->flags)) != 0){
      g_warning("running on template");
    }

    ags_recall_run_init_inter(AGS_RECALL(list->data));

    list = list_next;
  }
}

/**
 * ags_recall_run_init_inter:
 * @recall: an #AgsRecall
 *
 * Prepare for run, this is the inter stage within the preparation.
 * 
 * Since: 1.0.0
 */
void
ags_recall_run_init_inter(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[RUN_INIT_INTER], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_run_init_post(AgsRecall *recall)
{
  GList *list, *list_next;

  list = recall->children;

  while(list != NULL){
    list_next = list->next;
    
    if((AGS_RECALL_TEMPLATE & (AGS_RECALL(list->data)->flags)) != 0){
      g_warning("running on template");
    }

    ags_recall_run_init_post(AGS_RECALL(list->data));

    list = list_next;
  }

  recall->flags |= (AGS_RECALL_INITIAL_RUN |
		    AGS_RECALL_RUN_INITIALIZED);
}

/**
 * ags_recall_run_init_post:
 * @recall: an #AgsRecall
 *
 * Prepare for run, this is the post stage within the preparation.
 * 
 * Since: 1.0.0
 */
void
ags_recall_run_init_post(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[RUN_INIT_POST], 0);
  g_object_unref(G_OBJECT(recall));
}

/**
 * ags_recall_check_rt_stream:
 * @recall: an #AgsRecall
 *
 * Prepare for run, this is the pre stage within the preparation.
 * 
 * Since: 1.4.0
 */
void
ags_recall_check_rt_stream(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[CHECK_RT_STREAM], 0);
  g_object_unref(G_OBJECT(recall));
}

/**
 * ags_recall_automate:
 * @recall: an #AgsRecall
 *
 * This is the automate port of @recall.
 * 
 * Since: 1.0.0
 */
void
ags_recall_automate(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[AUTOMATE], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_run_pre(AgsRecall *recall)
{
  GList *list, *list_start;

  /* lock ports */
  ags_recall_lock_port(recall);

  /* run */
  list_start = 
    list = ags_list_util_copy_and_ref(recall->children);

  while(list != NULL){
    if((AGS_RECALL_TEMPLATE & (AGS_RECALL(list->data)->flags)) != 0){
      g_warning("running on template");
      list = list->next;
      continue;
    }

    g_object_ref(list->data);
    AGS_RECALL_GET_CLASS(AGS_RECALL(list->data))->run_pre(AGS_RECALL(list->data));
    g_object_unref(list->data);
    g_object_unref(list->data);
    
    list = list->next;
  }

  g_list_free(list_start);

  /* unlock ports */
  ags_recall_unlock_port(recall);
}

/**
 * ags_recall_run_pre:
 * @recall: an #AgsRecall
 *
 * This is the pre stage within a run.
 * 
 * Since: 1.0.0
 */
void
ags_recall_run_pre(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[RUN_PRE], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_run_inter(AgsRecall *recall)
{
  GList *list, *list_start;

  /* lock port */
  ags_recall_lock_port(recall);

  /* run */
  list_start = 
    list = ags_list_util_copy_and_ref(recall->children);

  while(list != NULL){
    if((AGS_RECALL_TEMPLATE & (AGS_RECALL(list->data)->flags)) != 0){
      g_warning("running on template");
      list = list->next;
      continue;
    }

    g_object_ref(list->data);
    AGS_RECALL_GET_CLASS(AGS_RECALL(list->data))->run_inter(AGS_RECALL(list->data));
    g_object_unref(list->data);
    g_object_unref(list->data);
    
    list = list->next;
  }

  g_list_free(list_start);

  /* unlock port */
  ags_recall_unlock_port(recall);
}

/**
 * ags_recall_run_inter:
 * @recall: an #AgsRecall
 *
 * This is the inter stage within a run.
 * 
 * Since: 1.0.0
 */
void
ags_recall_run_inter(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[RUN_INTER], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_run_post(AgsRecall *recall)
{
  GList *list, *list_start;

  /* lock port */
  ags_recall_lock_port(recall);

  /* run */
  list_start = 
    list = ags_list_util_copy_and_ref(recall->children);
  
  while(list != NULL){
    if((AGS_RECALL_TEMPLATE & (AGS_RECALL(list->data)->flags)) != 0){
      g_warning("running on template");
      list = list->next;
      continue;
    }

    g_object_ref(list->data);
    AGS_RECALL_GET_CLASS(AGS_RECALL(list->data))->run_post(AGS_RECALL(list->data));
    g_object_unref(list->data);
    g_object_unref(list->data);
    
    list = list->next;
  }

  g_list_free(list_start);
  
  if((AGS_RECALL_INITIAL_RUN & (recall->flags)) != 0){
    recall->flags &= (~AGS_RECALL_INITIAL_RUN);
  }

  /* unlock port */
  ags_recall_unlock_port(recall);
}

/**
 * ags_recall_run_post:
 * @recall: an #AgsRecall
 *
 * This is the post stage within a run.
 * 
 * Since: 1.0.0
 */
void
ags_recall_run_post(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[RUN_POST], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_stop_persistent(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[STOP_PERSISTENT], 0);
  g_object_unref(G_OBJECT(recall));
}

/**
 * ags_recall_stop_persistent:
 * @recall: an #AgsRecall
 *
 * Unsets the %AGS_RECALL_PERSISTENT flag set and invokes ags_recall_done().
 * 
 * Since: 1.0.0
 */
void
ags_recall_stop_persistent(AgsRecall *recall)
{
  if((AGS_RECALL_DONE & (recall->flags)) != 0){
    return;
  }

  recall->flags &= (~(AGS_RECALL_PERSISTENT |
		      AGS_RECALL_PERSISTENT_PLAYBACK |
		      AGS_RECALL_PERSISTENT_SEQUENCER |
		      AGS_RECALL_PERSISTENT_NOTATION));

  ags_recall_done(recall);
}

void
ags_recall_real_done(AgsRecall *recall)
{
  if((AGS_RECALL_DONE & (recall->flags)) != 0){
    return;
  }

  recall->flags |= AGS_RECALL_DONE;
  
  ags_recall_remove(recall);
}

/**
 * ags_recall_done:
 * @recall: an #AgsRecall
 *
 * The #AgsRecall doesn't want to run anymore, it has been done its
 * work.
 * 
 * Since: 1.0.0
 */
void
ags_recall_done(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));
  
  if((AGS_RECALL_PERSISTENT & (recall->flags)) != 0 ||
     (AGS_RECALL_TEMPLATE & (recall->flags)) != 0 ||
     ((AGS_RECALL_PERSISTENT_PLAYBACK & (recall->flags)) != 0 && (AGS_RECALL_PLAYBACK & (recall->flags)) != 0) ||
     ((AGS_RECALL_PERSISTENT_SEQUENCER & (recall->flags)) != 0 && (AGS_RECALL_SEQUENCER & (recall->flags)) != 0) ||
     ((AGS_RECALL_PERSISTENT_NOTATION & (recall->flags)) != 0 && (AGS_RECALL_NOTATION & (recall->flags)) != 0)){
    return;
  }

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[DONE], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_cancel(AgsRecall *recall)
{
  GList *list, *list_next;

  if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0){
    return;
  }

  /* call cancel for children */
  list = recall->children;

  while(list != NULL){
    list_next = list->next;
    
    ags_recall_cancel(AGS_RECALL(list->data));
    
    list = list_next;
  }

  if((AGS_RECALL_PERSISTENT & (recall->flags)) != 0 ||
     (AGS_RECALL_PERSISTENT_PLAYBACK & (recall->flags)) != 0){
    ags_recall_stop_persistent(recall);
  }else{
    ags_recall_done(recall);
  }
}

/**
 * ags_recall_cancel:
 * @recall: an #AgsRecall
 *
 * The #AgsRecall doesn't want to run anymore, it aborts further execution.
 * 
 * Since: 1.0.0
 */
void
ags_recall_cancel(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[CANCEL], 0);
  g_object_unref(G_OBJECT(recall));
}

void
ags_recall_real_remove(AgsRecall *recall)
{
  AgsMutexManager *mutex_manager;
  AgsDestroyWorker *destroy_worker;
  
  AgsApplicationContext *application_context;

  AgsRecall *parent;

  GList *worker;
  
  pthread_mutex_t *application_mutex;
  
  application_context = ags_application_context_get_instance();

  mutex_manager = ags_mutex_manager_get_instance();
  
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get destroy worker */
  pthread_mutex_lock(application_mutex);
  
  worker = ags_concurrency_provider_get_worker(AGS_CONCURRENCY_PROVIDER(application_context));
  worker = ags_list_util_find_type(worker,
				   AGS_TYPE_DESTROY_WORKER);

  if(worker != NULL){
    destroy_worker = worker->data;
  }else{
    destroy_worker = NULL;
  }
  
  pthread_mutex_unlock(application_mutex);
  
  /* dispose and unref */
  g_object_ref(recall);

  if(recall->parent == NULL){
#if 0
    if(destroy_worker != NULL){
      ags_destroy_worker_add(destroy_worker,
			     recall, g_object_run_dispose);
    }else{
      g_object_run_dispose(recall);
    }
#endif
    
    return;
  }else{
    parent = AGS_RECALL(recall->parent);

    ags_recall_remove_child(parent,
    			    recall);

    if(destroy_worker != NULL){
      ags_destroy_worker_add(destroy_worker,
			     recall, ags_destroy_util_dispose_and_unref);
    }else{
      g_object_run_dispose(recall);
      g_object_unref(recall);
    }
  }

  /* propagate done */
  if(parent != NULL &&
     (AGS_RECALL_PROPAGATE_DONE & (parent->flags)) != 0 &&
     (AGS_RECALL_PERSISTENT & (parent->flags)) == 0 &&
     parent->children == NULL){
    ags_recall_done(parent);
  }
}

/**
 * ags_recall_remove:
 * @recall: an #AgsRecall
 *
 * The #AgsRecall will be removed immediately.
 * 
 * Since: 1.0.0
 */
void
ags_recall_remove(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[REMOVE], 0);
  g_object_unref(G_OBJECT(recall));
  //  g_object_unref(recall);
}

/**
 * ags_recall_is_done:
 * @recalls: a #GList-struct of #AgsRecall
 * @recycling_context: an #AgsRecyclingContext
 *
 * Check if recall is over.
 * 
 * Returns: %TRUE if recall is done, otherwise %FALSE
 *
 * Since: 1.0.0
 */
gboolean
ags_recall_is_done(GList *recalls, GObject *recycling_context)
{
  AgsRecall *recall;

  if(recalls == NULL ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return(FALSE);
  }

  while(recalls != NULL){
    recall = AGS_RECALL(recalls->data);

    if((AGS_RECALL_TEMPLATE & (recall->flags)) == 0 &&
       !AGS_IS_RECALL_AUDIO(recall) &&
       !AGS_IS_RECALL_CHANNEL(recall) &&
       recall->recall_id != NULL &&
       recall->recall_id->recycling_context == (AgsRecyclingContext *) recycling_context){
      if((AGS_RECALL_DONE & (recall->flags)) == 0){
	recall->flags &= (~AGS_RECALL_RUN_INITIALIZED);
	//	g_message("done: %s", G_OBJECT_TYPE_NAME(recall));
	return(FALSE);
      }
    }

    recalls = recalls->next;
  }

  return(TRUE);
}

AgsRecall*
ags_recall_real_duplicate(AgsRecall *recall,
			  AgsRecallID *recall_id,
			  guint *n_params, gchar **parameter_name, GValue *value)
{
  AgsRecall *copy;
  AgsRecallClass *recall_class, *copy_class;
  AgsRecallContainer *recall_container;
  AgsRecallHandler *recall_handler, *recall_handler_copy;

  GList *list, *child;

  guint local_n_params;

  local_n_params = 0;
  
  if(n_params == NULL){
    n_params = &local_n_params;
  }
  
  if(n_params[0] == 0){
    parameter_name = (gchar **) malloc(5 * sizeof(gchar *));
    value = g_new0(GValue,
		   4);
  }else{
    parameter_name = (gchar **) realloc(parameter_name,
					(n_params[0] + 5) * sizeof(gchar *));
    value = g_renew(GValue,
		    n_params[0] + 4);
  }

  parameter_name[n_params[0]] = "output-soundcard";
  value[n_params[0]] = {0,};
  g_value_set_object(&(value[n_params[0]]), recall->output_soundcard);

  parameter_name[n_params[0] + 1] = "input-soundcard";
  value[n_params[1]] = {0,};
  g_value_set_object(&(value[n_params[0] + 1]), recall->input_soundcard);
    
  parameter_name[n_params[0] + 2] = "recall-id";
  value[n_params[2]] = {0,};
  g_value_set_object(&(value[n_params[0] + 2]), recall_id);
    
  parameter_name[n_params[0] + 3] = "recall-container";
  value[n_params[3]] = {0,};
  g_value_set_object(&(value[n_params[0] + 3]), recall->container);

  parameter_name[n_params[0] + 4] = NULL;

  n_params[0] += 4;
  
  parameter = ags_parameter_grow(G_OBJECT_TYPE(recall),
				 parameter, n_params,
				 NULL);

  copy = g_object_new_with_properties(G_OBJECT_TYPE(recall),
				      *n_params, parameter_name, value);
  g_free(parameter);
  
  ags_recall_set_flags(copy,
		       (recall->flags & (~ (AGS_RECALL_ADDED_TO_REGISTRY |
					    AGS_RECALL_CONNECTED |
					    AGS_RECALL_TEMPLATE))));

  copy->child_type = recall->child_type;

  /* duplicate handlers */
  list = recall->handlers;
  
  while(list != NULL){
    recall_handler = AGS_RECALL_HANDLER(list->data);

    recall_handler_copy = ags_recall_handler_alloc(recall_handler->signal_name,
						   recall_handler->callback,
						   recall_handler->data);
    ags_recall_add_handler(copy, recall_handler_copy);

    list = list->next;
  }

  return(copy);
}

/**
 * ags_recall_duplicate:
 * @recall: an #AgsRecall
 * @recall_id: an #AgsRecallID
 *
 * Should duplicate an #AgsRecall, so it can pass the runs. Mainly used for
 * creating duplicates from templates, see %AGS_RECALL_TEMPLATE.
 * 
 * Returns: the duplicated #AgsRecall
 *
 * Since: 1.0.0
 */
AgsRecall*
ags_recall_duplicate(AgsRecall *recall, AgsRecallID *recall_id) /*, guint n_params, GParameter *parameter */
{
  AgsRecall *copy;
  GParameter *params;
  guint n_params;

  g_return_val_if_fail(AGS_IS_RECALL(recall), NULL);

  params = NULL;
  n_params = 0;

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[DUPLICATE], 0,
		recall_id, &n_params, params,
		&copy);
  g_object_unref(G_OBJECT(recall));

  return(copy);
}

/**
 * ags_recall_set_recall_id:
 * @recall: an #AgsRecall
 * @recall_id: the #AgsRecallID to set
 *
 * Sets the recall id recursively.
 * 
 * Since: 1.0.0
 */
void
ags_recall_set_recall_id(AgsRecall *recall, AgsRecallID *recall_id)
{
  GList *list;

  list = recall->children;

  while(list != NULL){
    if((AGS_RECALL_TEMPLATE & (AGS_RECALL(list->data)->flags)) != 0){
      g_warning("running on template");
    }

    ags_recall_set_recall_id(AGS_RECALL(list->data), recall_id);

    list = list->next;
  }

  recall->recall_id = recall_id;
}

/**
 * ags_recall_notify_dependency:
 * @recall: an #AgsRecall
 * @flags: see AgsRecallNotifyDependencyMode
 * @count: how many dependencies
 *
 * Notifies a recall that an other depends on it.
 * 
 * Since: 1.0.0
 */
void
ags_recall_notify_dependency(AgsRecall *recall, guint flags, gint count)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[NOTIFY_DEPENDENCY], 0,
		flags, count);
  g_object_unref(G_OBJECT(recall));
}

/**
 * ags_recall_add_recall_dependency:
 * @recall: the #AgsRecall
 * @recall_dependency: an #AgsRecallDependency
 *
 * Associate a new dependency for this recall.
 * 
 * Since: 2.0.0
 */
void
ags_recall_add_recall_dependency(AgsRecall *recall, AgsRecallDependency *recall_dependency)
{
  pthread_mutex_t *recall_mutex;
  
  if(!AGS_IS_RECALL(recall) ||
     !AGS_IS_RECALL_DEPENDENCY(recall_dependency)){
    return;
  }

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = recall->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* add recall dependency */
  pthread_mutex_lock(recall_mutex);
  
  g_object_ref(recall_dependency);  
  recall->recall_dependency = g_list_prepend(recall->recall_dependency,
					     recall_dependency);

  pthread_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_remove_recall_dependency:
 * @recall: the #AgsRecall
 * @recall_dependency: an #AgsRecallDependency
 *
 * Remove a prior associated dependency.
 * 
 * Since: 2.0.0
 */
void
ags_recall_remove_recall_dependency(AgsRecall *recall, AgsRecallDependency *recall_dependency)
{
  pthread_mutex_t *recall_mutex;

  if(!AGS_IS_RECALL(recall) ||
     !AGS_IS_RECALL_DEPENDENCY(recall_dependency)){
    return;
  }

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = recall->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* remove recall dependency */
  pthread_mutex_lock(recall_mutex);

  recall->recall_dependency = g_list_remove(recall->recall_dependency,
					    recall_dependency);
  g_object_unref(G_OBJECT(recall_dependency));

  pthread_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_remove_child:
 * @recall: an #AgsRecall
 * @child: an #AgsRecall
 *
 * An #AgsRecall may have children.
 * 
 * Since: 1.0.0
 */
void
ags_recall_remove_child(AgsRecall *recall, AgsRecall *child)
{
  if(recall == NULL ||
     child == NULL ||
     child->parent != recall){
    return;
  }

  if((AGS_RECALL_CONNECTED & (recall->flags)) != 0){
    ags_connectable_disconnect(AGS_CONNECTABLE(child));
  }
  
  pthread_mutex_lock(recall->children_mutex);

  recall->children = g_list_remove(recall->children,
				   child);

  pthread_mutex_unlock(recall->children_mutex);
  
  child->parent = NULL;

  g_object_unref(recall);
  g_object_unref(child);
}

/**
 * ags_recall_handler_free:
 * @recall_handler: the #AgsRecallHandler-struct
 *
 * Free @recall_hanlder. 
 * 
 * Since: 2.0.0
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
 * @callback: the #GCallback function
 * @data: the data to pass the callback
 *
 * Allocates #AgsRecallHandler-struct.
 * 
 * Returns: the newly allocated #AgsRecallHandler-struct
 * 
 * Since: 1.0.0
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
 * ags_recall_add_handler:
 * @recall: the #AgsRecall to connect
 * @recall_handler: the signal specs
 *
 * Connect callback to @recall specified by @recall_handler.
 * 
 * Since: 1.0.0
 */
void
ags_recall_add_handler(AgsRecall *recall,
		       AgsRecallHandler *recall_handler)
{
  recall->handlers = g_list_prepend(recall->handlers,
				    recall_handler);
}

/**
 * ags_recall_remove_handler:
 * @recall: the #AgsRecall to connect
 * @recall_handler: the signal specs
 *
 * Remove a #AgsRecallHandler-struct from @recall.
 * 
 * Since: 1.0.0
 */
void
ags_recall_remove_handler(AgsRecall *recall,
			  AgsRecallHandler *recall_handler)
{
  recall->handlers = g_list_remove(recall->handlers,
				   recall_handler);
}

/**
 * ags_recall_add_child:
 * @parent: an #AgsRecall
 * @child: an #AgsRecall
 *
 * An #AgsRecall may have children.
 * 
 * Since: 1.0.0
 */
void
ags_recall_add_child(AgsRecall *parent, AgsRecall *child)
{
  guint inheritated_flags_mask;
  
  if(child == NULL ||
     parent == NULL ||
     child->parent == parent){
    return;
  }

  g_object_ref(child);

  inheritated_flags_mask = (AGS_RECALL_PLAYBACK |
			    AGS_RECALL_SEQUENCER |
			    AGS_RECALL_NOTATION |
			    AGS_RECALL_PROPAGATE_DONE |
			    AGS_RECALL_INITIAL_RUN);

  if(!AGS_IS_RECALL_AUDIO_SIGNAL(child)){
    inheritated_flags_mask |= (AGS_RECALL_PERSISTENT |
			       AGS_RECALL_PERSISTENT_PLAYBACK |
			       AGS_RECALL_PERSISTENT_SEQUENCER |
			       AGS_RECALL_PERSISTENT_NOTATION);
  }

  /* unref old */
  if(child->parent != NULL){
    child->flags &= (~inheritated_flags_mask);

    pthread_mutex_lock(child->parent->children_mutex);
    
    child->parent->children = g_list_remove(child->parent->children, child);

    pthread_mutex_unlock(child->parent->children_mutex);

    g_object_unref(child->parent);
    g_object_unref(child);
    g_object_set(G_OBJECT(child),
		 "recall_id", NULL,
		 NULL);
  }

  /* ref new */
  if(parent != NULL){
    g_object_ref(parent);
    g_object_ref(child);

    child->flags |= (inheritated_flags_mask & (parent->flags));

    pthread_mutex_lock(parent->children_mutex);
    
    parent->children = g_list_prepend(parent->children,
				      child);

    pthread_mutex_unlock(parent->children_mutex);

    g_object_set(G_OBJECT(child),
		 "output-soundcard", parent->soundcard,
		 "recall_id", parent->recall_id,
		 NULL);
    g_signal_connect(G_OBJECT(child), "done",
    		     G_CALLBACK(ags_recall_child_done), parent);
  }
  
  child->parent = parent;

  if(parent != NULL){
    ags_recall_child_added(parent,
			   child);
  }

  ags_connectable_connect(AGS_CONNECTABLE(child));

  if(AGS_IS_RECALL(parent)){
    guint staging_flags;
    pthread_mutex_t *parent_mutex;

    staging_flags = (AGS_SOUND_STAGING_CHECK_RT_DATA |
		     AGS_SOUND_STAGING_RUN_INIT_PRE |
		     AGS_SOUND_STAGING_RUN_INIT_INTER |
		     AGS_SOUND_STAGING_RUN_INIT_POST);

    /* get recall mutex */
    pthread_mutex_lock(ags_recall_get_class_mutex());
  
    parent_mutex = parent->obj_mutex;
  
    pthread_mutex_unlock(ags_recall_get_class_mutex());
    
    /* get mask */
    pthread_mutex_lock(parent_mutex);

    staging_flags = (staging_flags & (parent->staging_flags));
    
    pthread_mutex_unlock(parent_mutex);

    /* set staging flags */
    ags_recall_set_staging_flags(child,
				 staging_flags);
  }

  g_object_unref(child);
}

/**
 * ags_recall_get_by_effect:
 * @list: a #GList with recalls
 * @filename: the filename containing @effect or %NULL
 * @effect: the effect name
 *
 * Finds all matching effect and filename.
 *
 * Returns: a GList, or %NULL if not found
 *
 * Since: 1.0.0
 */
GList*
ags_recall_get_by_effect(GList *recall, gchar *filename, gchar *effect)
{
  AgsRecall *current;
  GList *list;

  list = NULL;

  while(recall != NULL){
    current = AGS_RECALL(recall->data);
    
    if(filename != NULL){
      if(AGS_IS_RECALL_LADSPA(current) &&
	 !g_strcmp0(AGS_RECALL_LADSPA(current)->filename, filename) &&
	 !g_strcmp0(AGS_RECALL_LADSPA(current)->effect, effect)){
	list = g_list_prepend(list,
			      current);
      }else if(AGS_IS_RECALL_DSSI(current) &&
	       !g_strcmp0(AGS_RECALL_DSSI(current)->filename, filename) &&
	       !g_strcmp0(AGS_RECALL_DSSI(current)->effect, effect)){
	list = g_list_prepend(list,
			      current);
      }else if(AGS_IS_RECALL_LV2(current) &&
	       !g_strcmp0(AGS_RECALL_LV2(current)->filename, filename) &&
	       !g_strcmp0(AGS_RECALL_LV2(current)->effect, effect)){
	list = g_list_prepend(list,
			      current);
      }else if(!g_strcmp0(current->effect, effect)){
	list = g_list_prepend(list,
			      current);
      }
    }
    
    recall = recall->next;
  }
  
  list = g_list_reverse(list);
  
  return(list);
}

/**
 * ags_recall_find_recall_id_with_effect:
 * @list: a #GList with recalls
 * @recall_id: an #AgsRecallId, may be %NULL
 * @filename: the filename containing @effect or %NULL
 * @effect: the effect name
 *
 * Finds next matching effect name. Intended to be used as
 * iteration function.
 *
 * Returns: a GList, or %NULL if not found
 *
 * Since: 1.0.0
 */
GList*
ags_recall_find_recall_id_with_effect(GList *list, AgsRecallID *recall_id, gchar *filename, gchar *effect)
{
  AgsRecall *recall;

  while(list != NULL){
    recall = AGS_RECALL(list->data);

    if(filename == NULL){
      if(((recall_id != NULL &&
	   recall->recall_id != NULL &&
	   recall_id->recycling_context == recall->recall_id->recycling_context) ||
	  (recall_id == NULL &&
	   recall->recall_id == NULL)) &&
	 !g_strcmp0(recall->effect, effect)){
	return(list);
      }
    }else{
      if(AGS_IS_RECALL_LADSPA(recall) &&
	 ((recall_id != NULL &&
	   recall->recall_id != NULL &&
	   recall_id->recycling_context == recall->recall_id->recycling_context) ||
	  (recall_id == NULL &&
	   recall->recall_id == NULL)) &&
	 !g_strcmp0(AGS_RECALL_LADSPA(recall)->filename, filename) &&
	 !g_strcmp0(AGS_RECALL_LADSPA(recall)->effect, effect)){
	return(list);
      }else if(AGS_IS_RECALL_DSSI(recall) &&
	 ((recall_id != NULL &&
	   recall->recall_id != NULL &&
	   recall_id->recycling_context == recall->recall_id->recycling_context) ||
	  (recall_id == NULL &&
	   recall->recall_id == NULL)) &&
	 !g_strcmp0(AGS_RECALL_DSSI(recall)->filename, filename) &&
	 !g_strcmp0(AGS_RECALL_DSSI(recall)->effect, effect)){
	return(list);
      }else if(AGS_IS_RECALL_LV2(recall) &&
	       ((recall_id != NULL &&
		 recall->recall_id != NULL &&
		 recall_id->recycling_context == recall->recall_id->recycling_context) ||
		(recall_id == NULL &&
		 recall->recall_id == NULL)) &&
	       !g_strcmp0(AGS_RECALL_LV2(recall)->filename, filename) &&
	       !g_strcmp0(AGS_RECALL_LV2(recall)->effect, effect)){
	return(list);
      }
    }
  
    list = list->next;
  }

  return(NULL);
}


/**
 * ags_recall_find_type:
 * @recall_i: a #GList containing recalls
 * @type: a #GType
 * 
 * Finds next matching recall for type. Intended to be used as
 * iteration function.
 *
 * Returns: a #GList containing recalls, or %NULL if not found
 *
 * Since: 1.0.0
 */
GList*
ags_recall_find_type(GList *recall_i, GType gtype)
{
  AgsRecall *recall;

  while(recall_i != NULL){
    recall = AGS_RECALL(recall_i->data);

    if(G_OBJECT_TYPE(recall) == gtype){
      break;
    }

    recall_i = recall_i->next;
  }

  return(recall_i);
}

/**
 * ags_recall_find_template:
 * @recall_i: a #GList containing recalls
 *
 * Finds next template, see #AGS_RECALL_TEMPLATE flag. Intended to be used as
 * iteration function.
 *
 * Returns: a #GList containing recalls, or %NULL if not found
 *
 * Since: 1.0.0
 */
GList*
ags_recall_find_template(GList *recall_i)
{
  AgsRecall *recall;

  while(recall_i != NULL){
    recall = AGS_RECALL(recall_i->data);

    if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0){
      return(recall_i);
    }

    recall_i = recall_i->next;
  }

  return(NULL);
}

/**
 * ags_recall_template_find_type:
 * @recall_i: a #GList containing recalls
 * @type: a #GType
 * 
 * Finds next matching recall for type which is a template, see #AGS_RECALL_TEMPLATE flag.
 * Intended to be used as iteration function.
 *
 * Returns: a #GList containing recalls, or %NULL if not found
 *
 * Since: 1.0.0
 */
GList*
ags_recall_template_find_type(GList *recall_i, GType gtype)
{
  AgsRecall *recall;

  while(recall_i != NULL){
    recall = (AgsRecall *) recall_i->data;

    if(AGS_IS_RECALL(recall) &&
       (AGS_RECALL_TEMPLATE & (recall->flags)) != 0 &&
       G_OBJECT_TYPE(recall) == gtype){
      break;
    }

    recall_i = recall_i->next;
  }

  return(recall_i);
}

/**
 * ags_recall_template_find_all_type:
 * @recall_i: a #GList containing recalls
 * @...: a #GType
 * 
 * Finds next matching recall for type which is a template, see #AGS_RECALL_TEMPLATE flag.
 * Intended to be used as iteration function.
 *
 * Returns: a #GList containing recalls, or %NULL if not found
 *
 * Since: 1.0.0
 */
GList*
ags_recall_template_find_all_type(GList *recall_i, ...)
{
  AgsRecall *recall;
  
  GType *recall_types, *offset;
  GType current;

  guint i;
  
  va_list ap;

  va_start(ap,
	   recall_i);

  recall_types = (GType *) malloc(sizeof(GType));

  i = 0;
  
  while(TRUE){
    current = va_arg(ap, GType);

    if(current == G_TYPE_NONE){
      break;
    }
    
    recall_types = (GType *) realloc(recall_types,
				     (i + 2) * sizeof(GType));
    recall_types[i] = current;
    
    i++;
  }

  recall_types[i] = G_TYPE_NONE;
  
  va_end(ap);
  
  while(recall_i != NULL){
    recall = (AgsRecall *) recall_i->data;

    if(AGS_IS_RECALL(recall)){
      offset = recall_types;
    
      while(*offset != G_TYPE_NONE){      
	if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0 &&
	   g_type_is_a(G_OBJECT_TYPE(recall), *offset)){
	  free(recall_types);
	
	  return(recall_i);
	}

	offset++;
      }
    }
    
    recall_i = recall_i->next;
  }

  free(recall_types);
	
  return(NULL);
}

/**
 * ags_recall_find_type_with_recycling_context:
 * @recall_i: a #GList containing recalls
 * @type: a #GType
 * @recycling_context: an #AgsRecyclingContext
 * 
 * Finds next matching recall for type which has @recycling_context, see #AgsRecallId for further
 * details about #AgsRecyclingContext. Intended to be used as iteration function.
 *
 * Returns: a #GList containing recalls, or %NULL if not found
 *
 * Since: 1.0.0
 */
GList*
ags_recall_find_type_with_recycling_context(GList *recall_i, GType gtype, GObject *recycling_context)
{
  AgsRecall *recall;

  while(recall_i != NULL){
    recall = AGS_RECALL(recall_i->data);

    if(g_type_is_a(G_OBJECT_TYPE(recall), gtype) &&
       recall->recall_id != NULL &&
       recall->recall_id->recycling_context == (AgsRecyclingContext *) recycling_context){
      return(recall_i);
    }

    recall_i = recall_i->next;
  }

  return(NULL);
}

/**
 * ags_recall_find_recycling_context:
 * @recall_i: a #GList containing recalls
 * @recycling_context: an #AgsRecyclingContext
 * 
 * Finds next matching recall which has @recycling_context, see #AgsRecallId for further
 * details about #AgsRecyclingContext. Intended to be used as iteration function.
 *
 * Returns: a #GList containing recalls, or %NULL if not found
 *
 * Since: 1.0.0
 */
GList*
ags_recall_find_recycling_context(GList *recall_i, GObject *recycling_context)
{
  AgsRecall *recall;

#ifdef AGS_DEBUG
  g_message("ags_recall_find_recycling_context: recycling_context = %llx\n", recycling_context);
#endif

  while(recall_i != NULL){
    recall = AGS_RECALL(recall_i->data);

#ifdef AGS_DEBUG
    if(recall->recall_id != NULL){
      g_message("ags_recall_find_recycling_context: recall_id->recycling_contianer = %llx\n", (long long unsigned int) recall->recall_id->recycling_context);
    }
#endif

    if(recall->recall_id != NULL &&
       recall->recall_id->recycling_context == (AgsRecyclingContext *) recycling_context){
      return(recall_i);
    }

    recall_i = recall_i->next;
  }

  return(NULL);
}

/**
 * ags_recall_find_provider:
 * @recall: a #GList-struct containing recalls
 * @provider: a #GObject
 * 
 * Finds next matching recall for type which has @provider. The @provider may be either an #AgsChannel
 * or an #AgsAudio object. This function tries to find the corresponding #AgsRecallChannel and #AgsRecallAudio
 * objects of a #AgsRecall to find. If these recalls contains the @provider, the function will return.
 *
 * Returns: a #GList-struct containing recalls, or %NULL if not found
 *
 * Since: 1.0.0
 */
GList*
ags_recall_find_provider(GList *recall, GObject *provider)
{
  while(recall != NULL){
    AgsRecall *current;

    current = AGS_RECALL(recall->data);

    if(AGS_IS_AUDIO(provider)){
      if(AGS_IS_RECALL_AUDIO(current)){
	if(((GObject *) AGS_RECALL_AUDIO(current)->audio) == provider){
	  return(recall);
	}
      }else if(AGS_IS_RECALL_AUDIO_RUN(current)){
	AgsRecallAudio *recall_audio;

	recall_audio = AGS_RECALL_AUDIO_RUN(current)->recall_audio;

	if(recall_audio != NULL &&
	   ((GObject *) recall_audio->audio) == provider){
	  return(recall);
	}
      }
    }else if(AGS_IS_CHANNEL(provider)){
      if(AGS_IS_RECALL_CHANNEL(current)){
	if(((GObject *) AGS_RECALL_CHANNEL(current)->source) == provider)
	  return(recall);
      }else if(AGS_IS_RECALL_CHANNEL_RUN(current)){
	if(((GObject *) AGS_RECALL_CHANNEL_RUN(current)->source) == provider){
	  return(recall);
	}
      }
    }else if(AGS_IS_RECYCLING(provider)){
      if(AGS_IS_RECALL_RECYCLING(current)){
	if(((GObject *) AGS_RECALL_RECYCLING(current)->source) == provider){
	  return(recall);
	}
      }
    }else if(AGS_IS_AUDIO_SIGNAL(provider)){
      if(AGS_IS_RECALL_AUDIO_SIGNAL(current)){
	if(((GObject *) AGS_RECALL_AUDIO_SIGNAL(current)->source) == provider){
	  return(recall);
	}
      }
    }

    recall = recall->next;
  }

  return(NULL);
}

/**
 * ags_recall_find_provider:
 * @recall: a #GList containing recalls
 * @provider: a #GObject
 * 
 * Finds provider eg. #AgsAudio or #AgsChannel within #GList containig #AgsRecall.
 *
 * Returns: a #GList containing recalls, or %NULL if not found
 * 
 * Since: 1.0.0
 */
GList*
ags_recall_template_find_provider(GList *recall, GObject *provider)
{
  GList *list;

  list = recall;

  while((list = (ags_recall_find_provider(list, provider))) != NULL){
    if((AGS_RECALL_TEMPLATE & (AGS_RECALL(list->data)->flags)) != 0){
      return(list);
    }

    list = list->next;
  }

  return(NULL);
}

/**
 * ags_recall_find_provider_with_recycling_context:
 * @recall_i: a #GList containing recalls
 * @provider: a #GObject
 * @recycling_context: an #AgsRecyclingContext
 * 
 * Like ags_recall_template_find_provider() but given additionally @recycling_context as search parameter.
 *
 * Returns: a #GList containing recalls, or %NULL if not found
 * 
 * Since: 1.0.0
 */
GList*
ags_recall_find_provider_with_recycling_context(GList *recall_i, GObject *provider, GObject *recycling_context)
{
  AgsRecall *recall;

  while((recall_i = ags_recall_find_provider(recall_i, provider)) != NULL){
    recall = AGS_RECALL(recall_i->data);
    
    if(recall->recall_id != NULL &&
       recall->recall_id->recycling_context == (AgsRecyclingContext *) recycling_context){
      return(recall_i);
    }

    recall_i = recall_i->next;
  }

  return(NULL);
}

void
ags_recall_child_done(AgsRecall *child,
		      AgsRecall *parent)
{
  if(child == NULL ||
     parent == NULL){
    return;
  }
  
  ags_recall_remove_child(parent,
			  child);
  g_object_run_dispose(child);
}

/**
 * ags_recall_lock_port:
 * @recall: the #AgsRecall
 *
 * Unlocks the ports.
 *
 * Since: 1.0.0
 */
void
ags_recall_lock_port(AgsRecall *recall)
{
  GList *port;

  if(recall == NULL){
    return;
  }
  
  port = recall->port;

  while(port != NULL){
    pthread_mutex_lock(AGS_PORT(port->data)->mutex);

    port = port->next;
  }
}

/**
 * ags_recall_unlock_port:
 * @recall: the #AgsRecall
 *
 * Unlocks the ports.
 *
 * Since: 1.0.0
 */
void
ags_recall_unlock_port(AgsRecall *recall)
{
  GList *port;

  if(recall == NULL){
    return;
  }
  
  port = recall->port;

  while(port != NULL){
    pthread_mutex_unlock(AGS_PORT(port->data)->mutex);

    port = port->next;
  }
}

/**
 * ags_recall_new:
 *
 * Creates an #AgsRecall.
 *
 * Returns: a new #AgsRecall.
 * 
 * Since: 1.0.0
 */
AgsRecall*
ags_recall_new()
{
  AgsRecall *recall;

  recall = (AgsRecall *) g_object_new(AGS_TYPE_RECALL,
				      NULL);

  return(recall);
}
