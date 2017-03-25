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

#include <ags/audio/ags_recall.h>

#include <ags/util/ags_destroy_util.h>
#include <ags/util/ags_id_generator.h>
#include <ags/util/ags_list_util.h>

#include <ags/lib/ags_parameter.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_marshal.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_packable.h>
#include <ags/object/ags_dynamic_connectable.h>
#include <ags/object/ags_plugin.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_concurrency_provider.h>
#include <ags/thread/ags_destroy_worker.h>

#include <ags/server/ags_server.h>
#include <ags/server/ags_service_provider.h>
#include <ags/server/ags_registry.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>

#include <ags/plugin/ags_ladspa_manager.h>
#include <ags/plugin/ags_lv2_manager.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_recall_ladspa.h>
#include <ags/audio/ags_recall_lv2.h>
#include <ags/audio/ags_recall_recycling.h>
#include <ags/audio/ags_recall_audio_signal.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libxml/tree.h>

void ags_recall_class_init(AgsRecallClass *recall_class);
void ags_recall_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_packable_interface_init(AgsPackableInterface *packable);
void ags_recall_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
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
void ags_recall_add_to_registry(AgsConnectable *connectable);
void ags_recall_remove_from_registry(AgsConnectable *connectable);
gboolean ags_recall_is_connected(AgsConnectable *connectable);
void ags_recall_connect(AgsConnectable *connectable);
void ags_recall_disconnect(AgsConnectable *connectable);
gboolean ags_recall_pack(AgsPackable *packable, GObject *container);
gboolean ags_recall_unpack(AgsPackable *packable);
void ags_recall_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_recall_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
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
void ags_recall_dispose(GObject *gobject);
void ags_recall_finalize(GObject *gobject);

void ags_recall_real_load_automation(AgsRecall *recall,
				     GList *automation_port);
void ags_recall_real_unload_automation(AgsRecall *recall);

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
				     guint *n_params, GParameter *parameter);

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
  RESOLVE_DEPENDENCIES,
  LOAD_AUTOMATION,
  UNLOAD_AUTOMATION,
  RUN_INIT_PRE,
  RUN_INIT_INTER,
  RUN_INIT_POST,
  AUTOMATE,
  RUN_PRE,
  RUN_INTER,
  RUN_POST,
  STOP_PERSISTENT,
  DONE,
  CANCEL,
  REMOVE,
  DUPLICATE,
  NOTIFY_DEPENDENCY,
  CHILD_ADDED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_SOUNDCARD,
  PROP_CONTAINER,
  PROP_DEPENDENCY,
  PROP_RECALL_ID,
  PROP_PARENT,
  PROP_CHILD,
  PROP_PORT,
};

static gpointer ags_recall_parent_class = NULL;
static guint recall_signals[LAST_SIGNAL];

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

    static const GInterfaceInfo ags_packable_interface_info = {
      (GInterfaceInitFunc) ags_recall_packable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_recall_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall = g_type_register_static(G_TYPE_OBJECT,
					     "AgsRecall\0",
					     &ags_recall_info,
					     0);

    g_type_add_interface_static(ags_type_recall,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall,
				AGS_TYPE_PACKABLE,
				&ags_packable_interface_info);

    g_type_add_interface_static(ags_type_recall,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);

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
   * AgsRecall:soundcard:
   *
   * The assigned soundcard.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("soundcard\0",
				   "soundcard of recall\0",
				   "The soundcard which this recall is packed into\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /**
   * AgsRecall:recall-container:
   *
   * The recall container packed into.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("recall-container\0",
				   "container of recall\0",
				   "The container which this recall is packed into\0",
				   AGS_TYPE_RECALL_CONTAINER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONTAINER,
				  param_spec);

  /**
   * AgsRecall:dependency:
   *
   * The recall depending on.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("dependency\0",
				   "dependency of recall\0",
				   "The dependency that can be added\0",
				   AGS_TYPE_RECALL_DEPENDENCY,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEPENDENCY,
				  param_spec);

  /**
   * AgsRecall:recall-id:
   *
   * The recall id running in.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("recall-id\0",
				   "run id of recall\0",
				   "The recall id of the recall\0",
				   AGS_TYPE_RECALL_ID,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_ID,
				  param_spec);
  
  /**
   * AgsRecall:audio-channel:
   *
   * The parent recall.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("parent\0",
				   "parent recall of this recall\0",
				   "The recall should be the parent instance of this recall\0",
				   AGS_TYPE_RECALL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PARENT,
				  param_spec);
  
  /**
   * AgsRecall:audio-channel:
   *
   * The child recall.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("child\0",
				   "child of recall\0",
				   "The child that can be added\0",
				   AGS_TYPE_RECALL,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHILD,
				  param_spec);

  /**
   * AgsRecall:port:
   *
   * The assigned #AgsPort
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_object("port\0",
				   "port of recall\0",
				   "The port of recall\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT,
				  param_spec);
  
  /* AgsRecallClass */
  recall->resolve_dependencies = NULL;

  recall->load_automation = NULL;
  recall->unload_automation = NULL;
  
  recall->run_init_pre = ags_recall_real_run_init_pre;
  recall->run_init_inter = ags_recall_real_run_init_inter;
  recall->run_init_post = ags_recall_real_run_init_post;

  recall->automate = NULL;
  recall->run_pre = ags_recall_real_run_pre;
  recall->run_inter = ags_recall_real_run_inter;
  recall->run_post = ags_recall_real_run_post;

  recall->stop_persistent = ags_recall_real_stop_persistent;
  recall->done = ags_recall_real_done;

  recall->cancel = ags_recall_real_cancel;
  recall->remove = ags_recall_real_remove;

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
   * Since: 0.7.42
   */
  recall_signals[LOAD_AUTOMATION] =
    g_signal_new("load-automation\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, load_automation),
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
   * Since: 0.7.42
   */
  recall_signals[UNLOAD_AUTOMATION] =
    g_signal_new("unload-automation\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, unload_automation),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsRecall::resolve-dependencies:
   * @recall: the #AgsRecall to resolve
   *
   * The ::resolve-dependencies signal notifies about resolving
   * dependencies.
   *
   * Since: 0.4.2
   */
  recall_signals[RESOLVE_DEPENDENCIES] =
    g_signal_new("resolve-dependencies\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, resolve_dependencies),
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
   * Since: 0.4.2
   */
  recall_signals[RUN_INIT_PRE] =
    g_signal_new("run-init-pre\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, run_init_pre),
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
   * Since: 0.4.2
   */
  recall_signals[RUN_INIT_INTER] =
    g_signal_new("run-init-inter\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, run_init_inter),
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
   * Since: 0.4.2
   */
  recall_signals[RUN_INIT_POST] =
    g_signal_new("run-init-post\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, run_init_post),
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
   * Since: 0.7.42
   */
  recall_signals[AUTOMATE] =
    g_signal_new("automate\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, automate),
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
   * Since: 0.4.2
   */
  recall_signals[RUN_PRE] =
    g_signal_new("run-pre\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, run_pre),
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
   * Since: 0.4.2
   */
  recall_signals[RUN_INTER] =
    g_signal_new("run-inter\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, run_inter),
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
   * Since: 0.4.2
   */
  recall_signals[RUN_POST] =
    g_signal_new("run-post\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, run_post),
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
   * Since: 0.4.2
   */
  recall_signals[STOP_PERSISTENT] =
    g_signal_new("stop_persistent\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, stop_persistent),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsRecall::done:
   * @recall: the #AgsRecall to finish playback
   *
   * The ::done signal notifies about stopping playback.
   *
   * Since: 0.4.2
   */
  recall_signals[DONE] =
    g_signal_new("done\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, done),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsRecall::cancel:
   * @recall: the #AgsRecall to cancel playback
   *
   * The ::cancel signal notifies about cancelling playback.
   *
   * Since: 0.4.2
   */
  recall_signals[CANCEL] =
    g_signal_new("cancel\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, cancel),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsRecall::remove:
   * @recall: the #AgsRecall to remove of audio loop
   *
   * The ::remove signal notifies about removing.
   *
   * Since: 0.4.2
   */
  recall_signals[REMOVE] =
    g_signal_new("remove\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, remove),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsRecall::duplicate:
   * @recall: the #AgsRecall to duplicate
   * @recall_id: the assigned #AgsRecallID
   * @n_params: pointer to array length
   * @parameter: parameter array
   *
   * The ::duplicate signal notifies about instantiating.
   *
   * Since: 0.4.2
   */
  recall_signals[DUPLICATE] =
    g_signal_new("duplicate\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, duplicate),
		 NULL, NULL,
		 g_cclosure_user_marshal_OBJECT__OBJECT_POINTER_POINTER,
		 G_TYPE_OBJECT, 3,
		 G_TYPE_OBJECT,
		 G_TYPE_POINTER, G_TYPE_POINTER);

  /**
   * AgsRecall::notify-dependency:
   * @recall: the #AgsRecall to notify
   * @dependency: the kind of dependency
   * @count: the reference count
   *
   * The ::notify-dependency signal notifies about dependencies
   * added.
   *
   * Since: 0.4.2
   */
  recall_signals[NOTIFY_DEPENDENCY] =
    g_signal_new("notify-dependency\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, notify_dependency),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_INT,
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
   * Since: 0.4.2
   */
  recall_signals[CHILD_ADDED] =
    g_signal_new("child-added\0",
		 G_TYPE_FROM_CLASS (recall),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecallClass, child_added),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_recall_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->add_to_registry = ags_recall_add_to_registry;
  connectable->remove_from_registry = ags_recall_remove_from_registry;

  connectable->is_ready = NULL;
  connectable->is_connected = ags_recall_is_connected;
  connectable->connect = ags_recall_connect;
  connectable->disconnect = ags_recall_disconnect;
}

void
ags_recall_packable_interface_init(AgsPackableInterface *packable)
{
  packable->pack = ags_recall_pack;
  packable->unpack = ags_recall_unpack;
}

void
ags_recall_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  dynamic_connectable->connect_dynamic = ags_recall_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_recall_disconnect_dynamic;
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
  pthread_mutexattr_t *attr;

  recall->flags = 0;

  /* soundcard */
  recall->soundcard = NULL;

  /* container */
  recall->container = NULL;

  /* version and build id */
  recall->version = NULL;
  recall->build_id = NULL;

  /* effect and name */
  recall->effect = NULL;
  recall->name = NULL;

  /* xml type  */
  recall->xml_type = NULL;

  /* dependency */
  recall->dependencies = NULL;

  /* recall id */
  recall->recall_id = NULL;

  /* nested recall */
  attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  recall->children_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(recall->children_mutex,
		     attr);

  recall->parent = NULL;
  recall->children = NULL;

  recall->child_type = G_TYPE_NONE;
  recall->child_parameters = NULL;
  recall->n_params = 0;

  /* port */
  recall->port = NULL;
  recall->automation_port = NULL;

  /* handlers */
  recall->handlers = NULL;
}

void
ags_recall_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsRecall *recall;

  recall = AGS_RECALL(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;
      GList *current;
      
      soundcard = g_value_get_object(value);

      if(soundcard == recall->soundcard){
	return;
      }

      if(recall->soundcard != NULL){
	g_object_unref(recall->soundcard);
      }
      
      if(soundcard != NULL){
	g_object_ref(G_OBJECT(soundcard));
      }
      
      recall->soundcard = (GObject *) soundcard;

      current = recall->children;

      while(current != NULL){
	g_object_set(G_OBJECT(current->data),
		     "soundcard\0", soundcard,
		     NULL);

	current = current->next;
      }
    }
    break;
  case PROP_CONTAINER:
    {
      AgsRecallContainer *container;

      container = (AgsRecallContainer *) g_value_get_object(value);

      if((AgsRecallContainer *) recall->container == container ||
	 !(AGS_IS_RECALL_AUDIO(recall) ||
	   AGS_IS_RECALL_AUDIO_RUN(recall) ||
	   AGS_IS_RECALL_CHANNEL(recall) ||
	   AGS_IS_RECALL_CHANNEL_RUN(recall)))
	return;

      if(recall->container != NULL){
	ags_packable_unpack(AGS_PACKABLE(recall));

	g_object_unref(G_OBJECT(recall->container));
      }

      if(container != NULL){
	g_object_ref(G_OBJECT(container));

	ags_packable_pack(AGS_PACKABLE(recall), G_OBJECT(container));

	if(AGS_IS_RECALL_AUDIO(recall)){
	  g_object_set(G_OBJECT(container),
		       "recall_audio\0", recall,
		       NULL);
	}else if(AGS_IS_RECALL_AUDIO_RUN(recall)){
	  g_object_set(G_OBJECT(container),
		       "recall_audio_run\0", recall,
		       NULL);
	}else if(AGS_IS_RECALL_CHANNEL(recall)){
	  g_object_set(G_OBJECT(container),
		       "recall_channel\0", recall,
		       NULL);
	}else if(AGS_IS_RECALL_CHANNEL_RUN(recall)){
	  g_object_set(G_OBJECT(container),
		       "recall_channel_run\0", recall,
		       NULL);
	}
      }

      recall->container = (GObject *) container;
    }
    break;
  case PROP_DEPENDENCY:
    {
      AgsRecallDependency *recall_dependency;

      recall_dependency = (AgsRecallDependency *) g_value_get_object(value);

      ags_recall_add_dependency(recall, recall_dependency);
    }
    break;
  case PROP_RECALL_ID:
    {
      AgsRecallID *recall_id;

      recall_id = (AgsRecallID *) g_value_get_object(value);

      if(recall->recall_id == recall_id){
	return;
      }

      if(recall->recall_id != NULL){
	g_object_unref(G_OBJECT(recall->recall_id));
      }

      if(recall_id != NULL){
	g_object_ref(G_OBJECT(recall_id));
      }
	
      ags_recall_set_recall_id(recall, recall_id);
    }
    break;
  case PROP_PARENT:
    {
      AgsRecall *parent;
      
      parent = (AgsRecall *) g_value_get_object(value);

      if(recall->parent == parent){
	return;
      }

      if(recall->parent != NULL){
	g_object_unref(recall->parent);
      }

      if(parent != NULL){
	g_object_ref(parent);
      }
      
      recall->parent = parent;
    }
    break;
  case PROP_CHILD:
    {
      AgsRecall *child;

      gboolean child_added;
      
      child = (AgsRecall *) g_value_get_object(value);

      pthread_mutex_lock(recall->children_mutex);

      child_added = (g_list_find(recall->children, child) != NULL) ? TRUE: FALSE;
      
      pthread_mutex_unlock(recall->children_mutex);

      if(child == NULL ||
	 child_added){
	return;
      }
      
      ags_recall_add_child(recall, child);
    }
    break;
  case PROP_PORT:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == NULL ||
	 g_list_find(recall->port, port) != NULL){
	return;
      }

      g_object_ref(port);
      recall->port = g_list_prepend(recall->port,
				    port);
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

  recall = AGS_RECALL(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, recall->soundcard);
    }
    break;
  case PROP_CONTAINER:
    {
      g_value_set_object(value, recall->container);
    }
    break;
  case PROP_RECALL_ID:
    {
      g_value_set_object(value, recall->recall_id);
    }
    break;
  case PROP_PARENT:
    {
      g_value_set_object(value, recall->parent);
    }
    break;
  case PROP_PORT:
    {
      g_value_set_object(value, g_list_copy(recall->port));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_add_to_registry(AgsConnectable *connectable)
{
  AgsApplicationContext *application_context;
  AgsServer *server;
  AgsRecall *recall;
  AgsRegistryEntry *entry;
  GList *list;
  
  recall = AGS_RECALL(connectable);

  application_context = ags_soundcard_get_application_context(AGS_SOUNDCARD(recall->soundcard));

  server = ags_service_provider_get_server(AGS_SERVICE_PROVIDER(application_context));

  if(server != NULL){
    entry = ags_registry_entry_alloc(server->registry);
    g_value_set_object(&(entry->entry),
		       (gpointer) recall);
    ags_registry_add(server->registry,
		     entry);
  }
}

void
ags_recall_remove_from_registry(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

gboolean
ags_recall_is_connected(AgsConnectable *connectable)
{
  AgsRecall *recall;

  recall = AGS_RECALL(connectable);

  if((AGS_RECALL_CONNECTED & (recall->flags)) != 0){
    return(TRUE);
  }else{
    return(FALSE);
  }
}

void
ags_recall_connect(AgsConnectable *connectable)
{
  AgsRecall *recall;
  AgsRecallHandler *recall_handler;
  GList *list;

  recall = AGS_RECALL(connectable);

  if((AGS_RECALL_CONNECTED & (recall->flags)) != 0){
    return;
  }
  
  list = recall->children;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* handlers */
  list = recall->handlers;

  while(list != NULL){
    recall_handler = AGS_RECALL_HANDLER(list->data);
    recall_handler->handler = g_signal_connect_after(G_OBJECT(recall), recall_handler->signal_name,
						     G_CALLBACK(recall_handler->callback), recall_handler->data);

    list = list->next;
  }

  recall->flags |= AGS_RECALL_CONNECTED;
}

void
ags_recall_disconnect(AgsConnectable *connectable)
{
  AgsRecall *recall;
  GList *list;

  recall = AGS_RECALL(connectable);

  list = recall->children;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  recall->flags &= (~AGS_RECALL_CONNECTED);

  ags_recall_unload_automation(recall);
}

gboolean
ags_recall_pack(AgsPackable *packable, GObject *container)
{
  AgsRecall *recall;

  recall = AGS_RECALL(packable);

  if(recall == NULL ||
     recall->container == container ||
     (container != NULL && !AGS_IS_RECALL_CONTAINER(container)))
    return(TRUE);

#ifdef AGS_DEBUG
  g_message("===== packing: %s\0", G_OBJECT_TYPE_NAME(recall));
#endif

  return(FALSE);
}

gboolean
ags_recall_unpack(AgsPackable *packable)
{
  AgsRecall *recall;

  recall = AGS_RECALL(packable);

  if(recall == NULL ||
     recall->container == NULL){
    return(TRUE);
  }

  /* unset link */
  recall->container = NULL;

  return(FALSE);
}

void
ags_recall_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsRecall *recall;
  AgsRecallHandler *recall_handler;
  GList *list;

  recall = AGS_RECALL(dynamic_connectable);

  if((AGS_RECALL_DYNAMIC_CONNECTED & (recall->flags)) != 0){
    return;
  }

#ifdef AGS_DEBUG
      g_message("dynamic connect: %s\0", G_OBJECT_TYPE_NAME(recall));
#endif

  /* connect children */
  list = recall->children;

  while(list != NULL){
    ags_dynamic_connectable_connect_dynamic(AGS_DYNAMIC_CONNECTABLE(list->data));

    list = list->next;
  }

  /* connect handlers */
  list = recall->handlers;

  while(list != NULL){
    recall_handler = AGS_RECALL_HANDLER(list->data);
    recall_handler->handler = g_signal_connect_after(G_OBJECT(recall), recall_handler->signal_name,
						     G_CALLBACK(recall_handler->callback), recall_handler->data);

    list = list->next;
  }

  //TODO:JK: fixme
  recall->flags |= AGS_RECALL_DYNAMIC_CONNECTED;
}

void
ags_recall_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsRecall *recall;
  AgsRecallHandler *recall_handler;
  GList *list;

  recall = AGS_RECALL(dynamic_connectable);

  if((AGS_RECALL_DYNAMIC_CONNECTED & (recall->flags)) == 0){
    return;
  }

  /* disconnect children */
  list = recall->children;

  while(list != NULL){
    ags_dynamic_connectable_disconnect_dynamic(AGS_DYNAMIC_CONNECTABLE(list->data));

    list = list->next;
  }

  /* disconnect handlers */
  list = recall->handlers;

  while(list != NULL){
    recall_handler = AGS_RECALL_HANDLER(list->data);

    g_signal_handler_disconnect(G_OBJECT(recall), recall_handler->handler);

    list = list->next;
  }

  recall->flags &= (~AGS_RECALL_DYNAMIC_CONNECTED);
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
				   "application-context\0", file->application_context,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", recall,
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
				   "application-context\0", file->application_context,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", recall,
				   NULL));

  xmlAddChild(parent,
	      node);

  return(node);
}

void
ags_recall_dispose(GObject *gobject)
{
  AgsRecall *recall;

  GList *list, *list_next;

  recall = AGS_RECALL(gobject);

  /* soundcard */
  if(recall->soundcard != NULL){
    g_object_unref(recall->soundcard);

    recall->soundcard = NULL;
  }

  /* dependency */
  if(recall->dependencies != NULL){
    g_list_free_full(recall->dependencies,
		     g_object_unref);

    recall->dependencies = NULL;
  }
  
  /* recall id */
  if(recall->recall_id != NULL){
    g_object_unref(recall->recall_id);

    recall->recall_id = NULL;
  }
  
  /* children */
  if(recall->children != NULL){
    list = recall->children;

    while(list != NULL){
      list_next = list->next;
      
      g_object_run_dispose(G_OBJECT(list->data));

      list = list_next;
    }
    
    g_list_free_full(recall->children,
		     g_object_unref);

    recall->children = NULL;
  }
  
  if(recall->container != NULL){
    ags_packable_unpack(AGS_PACKABLE(recall));

    recall->container = NULL;
  }

  /* port */
  if(recall->port != NULL){
    g_list_free_full(recall->port,
		     g_object_unref);

    recall->port = NULL;
  }
  
  /* parent */
  if(recall->parent != NULL){
    ags_recall_remove_child(recall->parent,
			    recall);

    recall->parent = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_parent_class)->dispose(gobject);
}

void
ags_recall_finalize(GObject *gobject)
{
  AgsRecall *recall;

  guint *ids;
  guint i, n_ids;
  
  recall = AGS_RECALL(gobject);

#ifdef AGS_DEBUG
  g_message("finalize %s\n\0", G_OBJECT_TYPE_NAME(gobject));
#endif

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

  /* soundcard */
  if(recall->soundcard != NULL){
    g_object_unref(recall->soundcard);
  }

  //  if(recall->name != NULL){
    //    g_free(recall->name);
  //  }

  /* dependency */
  g_list_free_full(recall->dependencies,
		   g_object_unref);

  /* recall id */
  if(recall->recall_id != NULL){
    g_object_unref(recall->recall_id);
  }
  
  /* children */
  g_list_free_full(recall->children,
		   g_object_unref);

  pthread_mutex_destroy(recall->children_mutex);
  free(recall->children_mutex);
  
  if(recall->container != NULL){
    ags_packable_unpack(AGS_PACKABLE(recall));
  }

  
  if(recall->child_parameters != NULL){
    g_free(recall->child_parameters);
  }

  /* port */
  g_list_free_full(recall->port,
		   g_object_unref);

  /* parent */
  if(recall->parent != NULL){
    ags_recall_remove_child(recall->parent,
			    recall);
    //TODO:JK: implement me
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_parent_class)->finalize(gobject);
}

/**
 * ags_recall_set_flags:
 * @recall: an #AgsRecall
 * @flags: the flags mask
 *
 * Set flags recursivly.
 * 
 * Since: 0.4
 */
void
ags_recall_set_flags(AgsRecall *recall, guint flags)
{
  GList *child;
  guint inheritated_flags_mask;

  /* set flags */
  recall->flags |= flags;

  /* set recursivly - prepare mask */
  inheritated_flags_mask = (AGS_RECALL_PLAYBACK |
			    AGS_RECALL_SEQUENCER |
			    AGS_RECALL_NOTATION |
			    AGS_RECALL_PROPAGATE_DONE |
			    AGS_RECALL_INITIAL_RUN);

  if(!AGS_IS_RECALL_RECYCLING(recall)){
    inheritated_flags_mask |= (AGS_RECALL_PERSISTENT |
			       AGS_RECALL_PERSISTENT_PLAYBACK |
			       AGS_RECALL_PERSISTENT_SEQUENCER |
			       AGS_RECALL_PERSISTENT_NOTATION);
  }

  /* apply recursivly */
  child = recall->children;

  while(child != NULL){
    ags_recall_set_flags(AGS_RECALL(child->data), (inheritated_flags_mask & (flags)));

    child = child->next;
  }
}

/**
 * ags_recall_load_automation:
 * @recall: an #AgsRecall
 * @automation_port: a #GList-struct containing #AgsPort
 *
 * A signal indicating that additional automation will be loaded from
 * @automation_port.
 * 
 * Since: 0.7.30
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
 * Since: 0.7.30
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
 * ags_recall_resolve_dependencies:
 * @recall: an #AgsRecall
 *
 * A signal indicating that the inheriting object should resolve
 * it's dependencies.
 * 
 * Since: 0.4
 */
void
ags_recall_resolve_dependencies(AgsRecall *recall)
{
  g_return_if_fail(AGS_IS_RECALL(recall));

#ifdef AGS_DEBUG
  g_message("resolving %s\0", G_OBJECT_TYPE_NAME(recall));
#endif
  
  g_object_ref(G_OBJECT(recall));
  g_signal_emit(G_OBJECT(recall),
		recall_signals[RESOLVE_DEPENDENCIES], 0);
  g_object_unref(G_OBJECT(recall));
}

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
      g_warning("running on template\0");
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
 * Since: 0.4
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
      g_warning("running on template\0");
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
 * Since: 0.4
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
      g_warning("running on template\0");
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
 * Since: 0.4
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
 * ags_recall_automate:
 * @recall: an #AgsRecall
 *
 * This is the automate port of @recall.
 * 
 * Since: 0.7.6
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
      g_warning("running on template\0");
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
 * Since: 0.4
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
      g_warning("running on template\0");
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
 * Since: 0.4
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
      g_warning("running on template\0");
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
 * Since: 0.4
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
 * Since: 0.4
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
 * Since: 0.4
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
 * Since: 0.4
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
    if(destroy_worker != NULL){
      ags_destroy_worker_add(destroy_worker,
			     recall, ags_destroy_util_dispose_and_unref);
    }else{
      g_object_run_dispose(recall);
      g_object_unref(recall);
    }
    
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
 * Since: 0.4
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
 * @recalls: an #AgsRecall
 * @recycling_context: an #AgsRecyclingContext
 *
 * Check if recall is over.
 * 
 * Returns: %TRUE if recall is done, otherwise %FALSE
 *
 * Since: 0.4
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
	//	g_message("done: %s\0", G_OBJECT_TYPE_NAME(recall));
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
			  guint *n_params, GParameter *parameter)
{
  AgsRecall *copy;
  AgsRecallClass *recall_class, *copy_class;
  AgsRecallContainer *recall_container;
  AgsRecallHandler *recall_handler, *recall_handler_copy;
  GList *list, *child;

  parameter = ags_parameter_grow(G_OBJECT_TYPE(recall),
				 parameter, n_params,
				 "soundcard\0", recall->soundcard,
				 "recall_id\0", recall_id,
				 "recall_container\0", recall->container,
				 NULL);

  copy = g_object_newv(G_OBJECT_TYPE(recall), *n_params, parameter);
  g_free(parameter);
  
  ags_recall_set_flags(copy,
		       (recall->flags & (~ (AGS_RECALL_TEMPLATE |
					    AGS_RECALL_RUN_INITIALIZED |
					    AGS_RECALL_CONNECTED |
					    AGS_RECALL_DYNAMIC_CONNECTED))));

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
 * Since: 0.4
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
 * Since: 0.4
 */
void
ags_recall_set_recall_id(AgsRecall *recall, AgsRecallID *recall_id)
{
  GList *list;

  list = recall->children;

  while(list != NULL){
    if((AGS_RECALL_TEMPLATE & (AGS_RECALL(list->data)->flags)) != 0){
      g_warning("running on template\0");
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
 * Since: 0.4
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
 * ags_recall_add_dependency:
 * @recall: an #AgsRecall
 * @recall_dependency: an #AgsRecallDependency
 *
 * Associate a new dependency for this recall.
 * 
 * Since: 0.4
 */
void
ags_recall_add_dependency(AgsRecall *recall, AgsRecallDependency *recall_dependency)
{
  if(recall == NULL ||
     recall_dependency == NULL)
    return;

  g_object_ref(recall_dependency);
  
  recall->dependencies = g_list_prepend(recall->dependencies,
					recall_dependency);
  
  ags_connectable_connect(AGS_CONNECTABLE(recall_dependency));
}

/**
 * ags_recall_remove_dependency:
 * @recall: an #AgsRecall
 * @dependency: an #AgsRecall
 *
 * Remove a prior associated dependency.
 * 
 * Since: 0.4
 */
void
ags_recall_remove_dependency(AgsRecall *recall, AgsRecall *dependency)
{
  AgsRecallDependency *recall_dependency;
  GList *dependencies;

  if(recall == NULL ||
     dependency == NULL)
    return;

  dependencies = ags_recall_dependency_find_dependency(recall->dependencies, (GObject *) dependency);

  if(dependencies == NULL)
    return;

  recall_dependency = AGS_RECALL_DEPENDENCY(dependencies->data);
  recall->dependencies = g_list_delete_link(recall->dependencies, dependencies);
  g_object_unref(G_OBJECT(recall_dependency));
}

/**
 * ags_recall_get_dependencies:
 * @recall: an #AgsRecall
 *
 * Retrieve dependencies.
 *
 * Returns: a #GList with all dependencies.
 * 
 * Since: 0.4
 */
//FIXME:JK: duplicate the list
GList*
ags_recall_get_dependencies(AgsRecall *recall)
{
  return(recall->dependencies);
}

/**
 * ags_recall_remove_child:
 * @recall: an #AgsRecall
 * @child: an #AgsRecall
 *
 * An #AgsRecall may have children.
 * 
 * Since: 0.4
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

  if((AGS_RECALL_DYNAMIC_CONNECTED & (recall->flags)) != 0){
    ags_dynamic_connectable_disconnect_dynamic(AGS_DYNAMIC_CONNECTABLE(child));
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
 * ags_recall_add_child:
 * @parent: an #AgsRecall
 * @child: an #AgsRecall
 *
 * An #AgsRecall may have children.
 * 
 * Since: 0.4
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
		 "recall_id\0", NULL,
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
		 "soundcard\0", parent->soundcard,
		 "recall_id\0", parent->recall_id,
		 NULL);
    g_signal_connect(G_OBJECT(child), "done\0",
    		     G_CALLBACK(ags_recall_child_done), parent);
  }
  
  child->parent = parent;

  if(parent != NULL){
    ags_recall_child_added(parent,
			   child);
  }

  ags_connectable_connect(AGS_CONNECTABLE(child));

  if(parent != NULL &&
     (AGS_RECALL_DYNAMIC_CONNECTED & (parent->flags)) != 0 &&
     (AGS_RECALL_DYNAMIC_CONNECTED & (child->flags)) == 0){
    ags_dynamic_connectable_connect_dynamic(AGS_DYNAMIC_CONNECTABLE(child));

    ags_recall_run_init_pre(AGS_RECALL(child));
    ags_recall_run_init_inter(AGS_RECALL(child));
    ags_recall_run_init_post(AGS_RECALL(child));
  }

  g_object_unref(child);
}

/**
 * ags_recall_get_children:
 * @recall: an #AgsRecall
 *
 * Retrieve children.
 *
 * Returns: a GList with all children.
 * 
 * Since: 0.4
 */
GList*
ags_recall_get_children(AgsRecall *recall)
{
  return(g_list_copy(recall->children));
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
 * Since: 0.4.3
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
      }else if(AGS_IS_RECALL_LV2(current) &&
	       !g_strcmp0(AGS_RECALL_LV2(current)->filename, filename) &&
	       !g_strcmp0(AGS_RECALL_LV2(current)->effect, effect)){
	list = g_list_prepend(list,
			      current);
      }else  if(!g_strcmp0(current->effect, effect)){
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
 * Since: 0.4.3
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
 * Since: 0.4
 */
GList*
ags_recall_find_type(GList *recall_i, GType type)
{
  AgsRecall *recall;

  while(recall_i != NULL){
    recall = AGS_RECALL(recall_i->data);

    if(G_OBJECT_TYPE(recall) == type){
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
 * Since: 0.4
 */
GList*
ags_recall_find_template(GList *recall_i)
{
  AgsRecall *recall;

  while(recall_i != NULL){
    recall = AGS_RECALL(recall_i->data);

    if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0)
      return(recall_i);

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
 * Since: 0.4
 */
GList*
ags_recall_template_find_type(GList *recall_i, GType type)
{
  AgsRecall *recall;

  while(recall_i != NULL){
    recall = (AgsRecall *) recall_i->data;

    if(AGS_IS_RECALL(recall) &&
       (AGS_RECALL_TEMPLATE & (recall->flags)) != 0 &&
       G_OBJECT_TYPE(recall) == type)
      break;

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
 * Since: 0.4.3
 */
GList*
ags_recall_template_find_all_type(GList *recall_i, ...)
{
  AgsRecall *recall;
  
  GList *start, *list;
  GType current;
  va_list ap;

  list = NULL;
  va_start(ap,
	   recall_i);

  while((current = va_arg(ap, GType)) != G_TYPE_NONE){
    //FIXME:JK: rather gulong_to_pointer
    list = g_list_prepend(list,
			  current);
  }
  
  va_end(ap);

  start = list;
  
  while(recall_i != NULL){
    recall = (AgsRecall *) recall_i->data;

    if(AGS_IS_RECALL(recall)){
      list = start;
    
      while(list != NULL){
	//FIXME:JK: rather gpointer_to_ulong
	current = list->data;
      
	if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0 &&
	   g_type_is_a(G_OBJECT_TYPE(recall), current)){
	  g_list_free(start);
	
	  return(recall_i);
	}

	list = list->next;
      }
    }
    
    recall_i = recall_i->next;
  }

  g_list_free(start);
	
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
 * Since: 0.4
 */
GList*
ags_recall_find_type_with_recycling_context(GList *recall_i, GType type, GObject *recycling_context)
{
  AgsRecall *recall;

  while(recall_i != NULL){
    recall = AGS_RECALL(recall_i->data);

    if(g_type_is_a(G_OBJECT_TYPE(recall), type) &&
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
 * Since: 0.4
 */
GList*
ags_recall_find_recycling_context(GList *recall_i, GObject *recycling_context)
{
  AgsRecall *recall;

#ifdef AGS_DEBUG
  g_message("ags_recall_find_recycling_context: recycling_context = %llx\n\0", recycling_context);
#endif

  while(recall_i != NULL){
    recall = AGS_RECALL(recall_i->data);

    if(recall->recall_id != NULL)
#ifdef AGS_DEBUG
      g_message("ags_recall_find_recycling_context: recall_id->recycling_contianer = %llx\n\0", (long long unsigned int) recall->recall_id->recycling_context);
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
 * @recall_i: a #GList containing recalls
 * @provider: a #GObject
 * 
 * Finds next matching recall for type which has @provider. The @provider may be either an #AgsChannel
 * or an #AgsAudio object. This function tries to find the corresponding #AgsRecallChannel and #AgsRecallAudio
 * objects of a #AgsRecall to find. If these recalls contains the @provider, the function will return.
 *
 * Returns: a #GList containing recalls, or %NULL if not found
 *
 * Since: 0.4
 */
GList*
ags_recall_find_provider(GList *recall_i, GObject *provider)
{
  AgsRecall *recall;

  while(recall_i != NULL){
    recall = AGS_RECALL(recall_i->data);

    if(AGS_IS_AUDIO(provider)){
      if(AGS_IS_RECALL_AUDIO(recall)){
	if(((GObject *) AGS_RECALL_AUDIO(recall)->audio) == provider)
	  return(recall_i);
      }else if(AGS_IS_RECALL_AUDIO_RUN(recall)){
	AgsRecallAudio *recall_audio;

	recall_audio = AGS_RECALL_AUDIO_RUN(recall)->recall_audio;

	if(recall_audio != NULL &&
	   ((GObject *) recall_audio->audio) == provider){
	  return(recall_i);
	}
      }
    }else if(AGS_IS_CHANNEL(provider)){
      if(AGS_IS_RECALL_CHANNEL(recall)){
	if(((GObject *) AGS_RECALL_CHANNEL(recall)->source) == provider)
	  return(recall_i);
      }else if(AGS_IS_RECALL_CHANNEL_RUN(recall)){
	if(((GObject *) AGS_RECALL_CHANNEL_RUN(recall)->source) == provider){
	  return(recall_i);
	}
      }
    }else if(AGS_IS_RECYCLING(provider)){
      if(AGS_IS_RECALL_RECYCLING(recall)){
	if(((GObject *) AGS_RECALL_RECYCLING(recall)->source) == provider){
	  return(recall_i);
	}
      }
    }else if(AGS_IS_AUDIO_SIGNAL(provider)){
      if(AGS_IS_RECALL_AUDIO_SIGNAL(recall)){
	if(((GObject *) AGS_RECALL_AUDIO_SIGNAL(recall)->source) == provider){
	  return(recall_i);
	}
      }
    }

    recall_i = recall_i->next;
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
 * Since: 0.4
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
 * Since: 0.4
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
ags_recall_run_init(AgsRecall *recall, guint stage)
{
  if(stage == 0){
    ags_recall_run_init_pre(recall);
  }else if(stage == 1){
    ags_recall_run_init_inter(recall);
  }else{
    ags_recall_run_init_post(recall);
  }
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
 * Since: 0.4
 */
AgsRecallHandler*
ags_recall_handler_alloc(const gchar *signal_name,
			 GCallback callback,
			 GObject *data)
{
  AgsRecallHandler *recall_handler;

  recall_handler = (AgsRecallHandler *) malloc(sizeof(AgsRecallHandler));
  
  recall_handler->signal_name = signal_name;
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
 * Since: 0.4
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
 * Since: 0.4
 */
void
ags_recall_remove_handler(AgsRecall *recall,
			  AgsRecallHandler *recall_handler)
{
  recall->handlers = g_list_remove(recall->handlers,
				   recall_handler);
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
 * Since: 0.7.10
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
 * Since: 0.7.10
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
 * Since: 0.3
 */
AgsRecall*
ags_recall_new()
{
  AgsRecall *recall;

  recall = (AgsRecall *) g_object_new(AGS_TYPE_RECALL, NULL);

  return(recall);
}
