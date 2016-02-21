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

#include <ags/audio/recall/ags_perform_automation_audio_run.h>
#include <ags/audio/recall/ags_perform_automation_audio.h>

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

#include <ags/thread/ags_timestamp_thread.h>

#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>

void ags_perform_automation_audio_run_class_init(AgsPerformAutomationAudioRunClass *perform_automation_audio_run);
void ags_perform_automation_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_perform_automation_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_perform_automation_audio_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_perform_automation_audio_run_init(AgsPerformAutomationAudioRun *perform_automation_audio_run);
void ags_perform_automation_audio_run_set_property(GObject *gobject,
						   guint prop_id,
						   const GValue *value,
						   GParamSpec *param_spec);
void ags_perform_automation_audio_run_get_property(GObject *gobject,
						   guint prop_id,
						   GValue *value,
						   GParamSpec *param_spec);
void ags_perform_automation_audio_run_finalize(GObject *gobject);
void ags_perform_automation_audio_run_connect(AgsConnectable *connectable);
void ags_perform_automation_audio_run_disconnect(AgsConnectable *connectable);
void ags_perform_automation_audio_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_perform_automation_audio_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable); 
void ags_perform_automation_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_perform_automation_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

AgsRecall* ags_perform_automation_audio_run_duplicate(AgsRecall *recall,
						      AgsRecallID *recall_id,
						      guint *n_params, GParameter *parameter);

/**
 * SECTION:ags_perform_automation_audio_run
 * @short_description: play notation
 * @title: AgsPerformAutomationAudioRun
 * @section_id:
 * @include: ags/audio/recall/ags_perform_automation_audio_run.h
 *
 * The #AgsPerformAutomationAudioRun class play notation.
 */

enum{
  PROP_0,
};

static gpointer ags_perform_automation_audio_run_parent_class = NULL;
static AgsConnectableInterface* ags_perform_automation_audio_run_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_perform_automation_audio_run_parent_dynamic_connectable_interface;
static AgsPluginInterface *ags_perform_automation_audio_run_parent_plugin_interface;

GType
ags_perform_automation_audio_run_get_type()
{
  static GType ags_type_perform_automation_audio_run = 0;

  if(!ags_type_perform_automation_audio_run){
    static const GTypeInfo ags_perform_automation_audio_run_info = {
      sizeof (AgsPerformAutomationAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_perform_automation_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPerformAutomationAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_perform_automation_audio_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_perform_automation_audio_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_perform_automation_audio_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_perform_automation_audio_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_perform_automation_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
								   "AgsPerformAutomationAudioRun\0",
								   &ags_perform_automation_audio_run_info,
								   0);

    g_type_add_interface_static(ags_type_perform_automation_audio_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_perform_automation_audio_run,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);

    g_type_add_interface_static(ags_type_perform_automation_audio_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return (ags_type_perform_automation_audio_run);
}

void
ags_perform_automation_audio_run_class_init(AgsPerformAutomationAudioRunClass *perform_automation_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_perform_automation_audio_run_parent_class = g_type_class_peek_parent(perform_automation_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) perform_automation_audio_run;

  gobject->set_property = ags_perform_automation_audio_run_set_property;
  gobject->get_property = ags_perform_automation_audio_run_get_property;

  gobject->finalize = ags_perform_automation_audio_run_finalize;

  /* properties */

  /* AgsRecallClass */
  recall = (AgsRecallClass *) perform_automation_audio_run;

  recall->duplicate = ags_perform_automation_audio_run_duplicate;
}

void
ags_perform_automation_audio_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_perform_automation_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_perform_automation_audio_run_connect;
  connectable->disconnect = ags_perform_automation_audio_run_disconnect;
}

void
ags_perform_automation_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_perform_automation_audio_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_perform_automation_audio_run_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_perform_automation_audio_run_disconnect_dynamic;
}

void
ags_perform_automation_audio_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_perform_automation_audio_run_parent_plugin_interface = g_type_interface_peek_parent(plugin);
  
  plugin->read = ags_perform_automation_audio_run_read;
  plugin->write = ags_perform_automation_audio_run_write;
}

void
ags_perform_automation_audio_run_init(AgsPerformAutomationAudioRun *perform_automation_audio_run)
{
  AGS_RECALL(perform_automation_audio_run)->name = "ags-perform-notation\0";
  AGS_RECALL(perform_automation_audio_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(perform_automation_audio_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(perform_automation_audio_run)->xml_type = "ags-perform-notation-audio-run\0";
  AGS_RECALL(perform_automation_audio_run)->port = NULL;
}

void
ags_perform_automation_audio_run_set_property(GObject *gobject,
					      guint prop_id,
					      const GValue *value,
					      GParamSpec *param_spec)
{
  AgsPerformAutomationAudioRun *perform_automation_audio_run;

  perform_automation_audio_run = AGS_PERFORM_AUTOMATION_AUDIO_RUN(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_perform_automation_audio_run_get_property(GObject *gobject,
					      guint prop_id,
					      GValue *value,
					      GParamSpec *param_spec)
{
  AgsPerformAutomationAudioRun *perform_automation_audio_run;
  
  perform_automation_audio_run = AGS_PERFORM_AUTOMATION_AUDIO_RUN(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_perform_automation_audio_run_finalize(GObject *gobject)
{
  AgsPerformAutomationAudioRun *perform_automation_audio_run;

  perform_automation_audio_run = AGS_PERFORM_AUTOMATION_AUDIO_RUN(gobject);

  G_OBJECT_CLASS(ags_perform_automation_audio_run_parent_class)->finalize(gobject);
}

void
ags_perform_automation_audio_run_connect(AgsConnectable *connectable)
{
  AgsPerformAutomationAudioRun *perform_automation_audio_run;

  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) != 0){
    return;
  }

  /* call parent */
  ags_perform_automation_audio_run_parent_connectable_interface->connect(connectable);

  perform_automation_audio_run = AGS_PERFORM_AUTOMATION_AUDIO_RUN(connectable);
}

void
ags_perform_automation_audio_run_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_perform_automation_audio_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_perform_automation_audio_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsPerformAutomationAudioRun *perform_automation_audio_run;

  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(dynamic_connectable)->flags)) != 0){
    return;
  }

  perform_automation_audio_run = AGS_PERFORM_AUTOMATION_AUDIO_RUN(dynamic_connectable);

  /* call parent */
  ags_perform_automation_audio_run_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);
}

void
ags_perform_automation_audio_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsPerformAutomationAudioRun *perform_automation_audio_run;

  /* call parent */
  ags_perform_automation_audio_run_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  perform_automation_audio_run = AGS_PERFORM_AUTOMATION_AUDIO_RUN(dynamic_connectable);
}

void
ags_perform_automation_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsFileLookup *file_lookup;
  xmlNode *iter;

  /* read parent */
  ags_perform_automation_audio_run_parent_plugin_interface->read(file, node, plugin);
}

xmlNode*
ags_perform_automation_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsFileLookup *file_lookup;
  xmlNode *node, *child;
  xmlNode *dependency_node;
  GList *list;
  gchar *id;

  /* write parent */
  node = ags_perform_automation_audio_run_parent_plugin_interface->write(file, parent, plugin);

  return(node);
}

AgsRecall*
ags_perform_automation_audio_run_duplicate(AgsRecall *recall,
					   AgsRecallID *recall_id,
					   guint *n_params, GParameter *parameter)
{
  AgsPerformAutomationAudioRun *copy, *perform_automation_audio_run;

  copy = AGS_PERFORM_AUTOMATION_AUDIO_RUN(AGS_RECALL_CLASS(ags_perform_automation_audio_run_parent_class)->duplicate(recall,
														     recall_id,
														     n_params, parameter));

  return((AgsRecall *) copy);
}

/**
 * ags_perform_automation_audio_run_new:
 *
 * Creates an #AgsPerformAutomationAudioRun
 *
 * Returns: a new #AgsPerformAutomationAudioRun
 *
 * Since: 0.7.2
 */
AgsPerformAutomationAudioRun*
ags_perform_automation_audio_run_new()
{
  AgsPerformAutomationAudioRun *perform_automation_audio_run;

  perform_automation_audio_run = (AgsPerformAutomationAudioRun *) g_object_new(AGS_TYPE_PERFORM_AUTOMATION_AUDIO_RUN,
									       NULL);

  return(perform_automation_audio_run);
}
