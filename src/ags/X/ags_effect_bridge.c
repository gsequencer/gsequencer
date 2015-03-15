/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_bridge_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_plugin.h>

#include <ags/X/ags_machine.h>

void ags_effect_bridge_class_init(AgsEffectBridgeClass *effect_bridge);
void ags_effect_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_effect_bridge_plugin_interface_init(AgsPluginInterface *plugin);
void ags_effect_bridge_init(AgsEffectBridge *effect_bridge);
void ags_effect_bridge_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_effect_bridge_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_effect_bridge_connect(AgsConnectable *connectable);
void ags_effect_bridge_disconnect(AgsConnectable *connectable);
gchar* ags_effect_bridge_get_version(AgsPlugin *plugin);
void ags_effect_bridge_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_effect_bridge_get_build_id(AgsPlugin *plugin);
void ags_effect_bridge_set_build_id(AgsPlugin *plugin, gchar *build_id);

/**
 * SECTION:ags_effect_bridge
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsEffectBridge
 * @section_id:
 * @include: ags/X/ags_effect_bridge.h
 *
 * #AgsEffectBridge is a composite widget to visualize all #AgsChannel. It should be
 * packed by an #AgsMachine.
 */

static gpointer ags_effect_bridge_parent_class = NULL;

GType
ags_effect_bridge_get_type(void)
{
  static GType ags_type_effect_bridge = 0;

  if(!ags_type_effect_bridge){
    static const GTypeInfo ags_effect_bridge_info = {
      sizeof(AgsEffectBridgeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_effect_bridge_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsEffectBridge),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_effect_bridge_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_effect_bridge_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_effect_bridge_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_effect_bridge = g_type_register_static(GTK_TYPE_VBOX,
						    "AgsEffectBridge\0", &ags_effect_bridge_info,
						    0);

    g_type_add_interface_static(ags_type_effect_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_effect_bridge,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_effect_bridge);
}

void
ags_effect_bridge_class_init(AgsEffectBridgeClass *effect_bridge)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_effect_bridge_parent_class = g_type_class_peek_parent(effect_bridge);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(effect_bridge);

  gobject->set_property = ags_effect_bridge_set_property;
  gobject->get_property = ags_effect_bridge_get_property;
}

void
ags_effect_bridge_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_effect_bridge_connect;
  connectable->disconnect = ags_effect_bridge_disconnect;
}

void
ags_effect_bridge_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = NULL;
  plugin->set_name = NULL;
  plugin->get_version = ags_effect_bridge_get_version;
  plugin->set_version = ags_effect_bridge_set_version;
  plugin->get_build_id = ags_effect_bridge_get_build_id;
  plugin->set_build_id = ags_effect_bridge_set_build_id;
  plugin->get_xml_type = NULL;
  plugin->set_xml_type = NULL;
  plugin->get_ports = NULL;
  plugin->read = NULL;
  plugin->write = NULL;
  plugin->set_ports = NULL;
}

void
ags_effect_bridge_init(AgsEffectBridge *effect_bridge)
{
  //TODO:JK: implement me
}

void
ags_effect_bridge_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsEffectBridge *effect_bridge;

  effect_bridge = AGS_EFFECT_BRIDGE(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_effect_bridge_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsEffectBridge *effect_bridge;

  effect_bridge = AGS_EFFECT_BRIDGE(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_effect_bridge_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_effect_bridge_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

gchar*
ags_effect_bridge_get_version(AgsPlugin *plugin)
{
  return(AGS_EFFECT_BRIDGE(plugin)->version);
}

void
ags_effect_bridge_set_version(AgsPlugin *plugin, gchar *version)
{
  AgsEffectBridge *effect_bridge;

  effect_bridge = AGS_EFFECT_BRIDGE(plugin);

  effect_bridge->version = version;
}

gchar*
ags_effect_bridge_get_build_id(AgsPlugin *plugin)
{
  return(AGS_EFFECT_BRIDGE(plugin)->build_id);
}

void
ags_effect_bridge_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AgsEffectBridge *effect_bridge;

  effect_bridge = AGS_EFFECT_BRIDGE(plugin);

  effect_bridge->build_id = build_id;
}

/**
 * ags_effect_bridge_new:
 * @effect_bridge: the parent effect_bridge
 * @audio: the #AgsAudio to visualize
 *
 * Creates an #AgsEffectBridge
 *
 * Returns: a new #AgsEffectBridge
 *
 * Since: 0.4
 */
AgsEffectBridge*
ags_effect_bridge_new(AgsAudio *audio)
{
  AgsEffectBridge *effect_bridge;

  effect_bridge = (AgsEffectBridge *) g_object_new(AGS_TYPE_EFFECT_BRIDGE,
						   "audio\0", audio,
						   NULL);

  return(effect_bridge);
}
