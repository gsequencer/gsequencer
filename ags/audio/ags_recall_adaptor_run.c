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

#include <ags/audio/ags_recall_adaptor_run.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_dynamic_connectable.h>
#include <ags/object/ags_plugin.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>

void ags_recall_adaptor_run_class_init(AgsRecallAdaptorRunClass *recall_adaptor_run);
void ags_recall_adaptor_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_adaptor_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_recall_adaptor_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_recall_adaptor_run_init(AgsRecallAdaptorRun *recall_adaptor_run);
void ags_recall_adaptor_run_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_recall_adaptor_run_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_recall_adaptor_run_finalize(GObject *gobject);
void ags_recall_adaptor_run_connect(AgsConnectable *connectable);
void ags_recall_adaptor_run_disconnect(AgsConnectable *connectable);
void ags_recall_adaptor_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_recall_adaptor_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable); 
void ags_recall_adaptor_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_recall_adaptor_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

/**
 * SECTION:ags_recall_adaptor_run
 * @short_description: recall midi
 * @title: AgsRecallAdaptorRun
 * @section_id:
 * @include: ags/audio/recall/ags_recall_adaptor_run.h
 *
 * The #AgsRecallAdaptorRun class recall midi.
 */

enum{
  PROP_0,
  PROP_DELAY_AUDIO_RUN,
  PROP_COUNT_BEATS_AUDIO_RUN,
};

static gpointer ags_recall_adaptor_run_parent_class = NULL;
static AgsConnectableInterface* ags_recall_adaptor_run_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_recall_adaptor_run_parent_dynamic_connectable_interface;
static AgsPluginInterface *ags_recall_adaptor_run_parent_plugin_interface;

GType
ags_recall_adaptor_run_get_type()
{
  static GType ags_type_recall_adaptor_run = 0;

  if(!ags_type_recall_adaptor_run){
    static const GTypeInfo ags_recall_adaptor_run_info = {
      sizeof (AgsRecallAdaptorRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_adaptor_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallAdaptorRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_adaptor_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_adaptor_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_adaptor_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_recall_adaptor_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_adaptor_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
							 "AgsRecallAdaptorRun\0",
							 &ags_recall_adaptor_run_info,
							 0);

    g_type_add_interface_static(ags_type_recall_adaptor_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_adaptor_run,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_adaptor_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return (ags_type_recall_adaptor_run);
}

void
ags_recall_adaptor_run_class_init(AgsRecallAdaptorRunClass *recall_adaptor_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_recall_adaptor_run_parent_class = g_type_class_peek_parent(recall_adaptor_run);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_adaptor_run;

  gobject->set_property = ags_recall_adaptor_run_set_property;
  gobject->get_property = ags_recall_adaptor_run_get_property;

  gobject->finalize = ags_recall_adaptor_run_finalize;

  /* properties */

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_adaptor_run;
}

void
ags_recall_adaptor_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_adaptor_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_recall_adaptor_run_connect;
  connectable->disconnect = ags_recall_adaptor_run_disconnect;
}

void
ags_recall_adaptor_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_recall_adaptor_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_recall_adaptor_run_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_recall_adaptor_run_disconnect_dynamic;
}

void
ags_recall_adaptor_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_recall_adaptor_run_parent_plugin_interface = g_type_interface_peek_parent(plugin);
  
  plugin->read = ags_recall_adaptor_run_read;
  plugin->write = ags_recall_adaptor_run_write;
}

void
ags_recall_adaptor_run_init(AgsRecallAdaptorRun *recall_adaptor_run)
{
}

void
ags_recall_adaptor_run_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsRecallAdaptorRun *recall_adaptor_run;

  recall_adaptor_run = AGS_RECALL_ADAPTOR_RUN(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_adaptor_run_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsRecallAdaptorRun *recall_adaptor_run;
  
  recall_adaptor_run = AGS_RECALL_ADAPTOR_RUN(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_adaptor_run_finalize(GObject *gobject)
{
  AgsRecallAdaptorRun *recall_adaptor_run;

  recall_adaptor_run = AGS_RECALL_ADAPTOR_RUN(gobject);

  G_OBJECT_CLASS(ags_recall_adaptor_run_parent_class)->finalize(gobject);
}

void
ags_recall_adaptor_run_connect(AgsConnectable *connectable)
{
  AgsRecallAdaptorRun *recall_adaptor_run;

  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) != 0){
    return;
  }

  /* call parent */
  ags_recall_adaptor_run_parent_connectable_interface->connect(connectable);

  recall_adaptor_run = AGS_RECALL_ADAPTOR_RUN(connectable);
}

void
ags_recall_adaptor_run_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_recall_adaptor_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_recall_adaptor_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsRecallAdaptorRun *recall_adaptor_run;

  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(dynamic_connectable)->flags)) != 0){
    return;
  }

  recall_adaptor_run = AGS_RECALL_ADAPTOR_RUN(dynamic_connectable);

  /* call parent */
  ags_recall_adaptor_run_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);  
}

void
ags_recall_adaptor_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsRecallAdaptorRun *recall_adaptor_run;

  /* call parent */
  ags_recall_adaptor_run_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  recall_adaptor_run = AGS_RECALL_ADAPTOR_RUN(dynamic_connectable);
}

void
ags_recall_adaptor_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  /* read parent */
  ags_recall_adaptor_run_parent_plugin_interface->read(file, node, plugin);

}

xmlNode*
ags_recall_adaptor_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  xmlNode *node, *child;

  /* write parent */
  ags_recall_adaptor_run_parent_plugin_interface->write(file, node, plugin);

  return(node);
}

/**
 * ags_recall_adaptor_run_new:
 *
 * Creates an #AgsRecallAdaptorRun
 *
 * Returns: a new #AgsRecallAdaptorRun
 *
 * Since: 0.7.128
 */
AgsRecallAdaptorRun*
ags_recall_adaptor_run_new()
{
  AgsRecallAdaptorRun *recall_adaptor_run;

  recall_adaptor_run = (AgsRecallAdaptorRun *) g_object_new(AGS_TYPE_RECALL_ADAPTOR_RUN,
							    NULL);

  return(recall_adaptor_run);
}
