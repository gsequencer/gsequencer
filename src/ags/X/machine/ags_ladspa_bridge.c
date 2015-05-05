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

#include <ags/X/machine/ags_ladspa_bridge.h>
#include <ags/X/machine/ags_ladspa_bridge_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_plugin.h>

void ags_ladspa_bridge_class_init(AgsLadspaBridgeClass *ladspa_bridge);
void ags_ladspa_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ladspa_bridge_plugin_interface_init(AgsPluginInterface *plugin);
void ags_ladspa_bridge_init(AgsLadspaBridge *ladspa_bridge);
void ags_ladspa_bridge_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_ladspa_bridge_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_ladspa_bridge_connect(AgsConnectable *connectable);
void ags_ladspa_bridge_disconnect(AgsConnectable *connectable);
gchar* ags_ladspa_bridge_get_version(AgsPlugin *plugin);
void ags_ladspa_bridge_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_ladspa_bridge_get_build_id(AgsPlugin *plugin);
void ags_ladspa_bridge_set_build_id(AgsPlugin *plugin, gchar *build_id);

/**
 * SECTION:ags_ladspa_bridge
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsLadspaBridge
 * @section_id:
 * @include: ags/X/ags_ladspa_bridge.h
 *
 * #AgsLadspaBridge is a composite widget to visualize all #AgsChannel. It should be
 * packed by an #AgsMachine.
 */

static gpointer ags_ladspa_bridge_parent_class = NULL;

GType
ags_ladspa_bridge_get_type(void)
{
  static GType ags_type_ladspa_bridge = 0;

  if(!ags_type_ladspa_bridge){
    static const GTypeInfo ags_ladspa_bridge_info = {
      sizeof(AgsLadspaBridgeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ladspa_bridge_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsLadspaBridge),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ladspa_bridge_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ladspa_bridge_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_ladspa_bridge_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ladspa_bridge = g_type_register_static(AGS_TYPE_MACHINE,
						    "AgsLadspaBridge\0", &ags_ladspa_bridge_info,
						    0);

    g_type_add_interface_static(ags_type_ladspa_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_ladspa_bridge,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_ladspa_bridge);
}

void
ags_ladspa_bridge_class_init(AgsLadspaBridgeClass *ladspa_bridge)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_ladspa_bridge_parent_class = g_type_class_peek_parent(ladspa_bridge);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(ladspa_bridge);

  gobject->set_property = ags_ladspa_bridge_set_property;
  gobject->get_property = ags_ladspa_bridge_get_property;
}

void
ags_ladspa_bridge_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_ladspa_bridge_connect;
  connectable->disconnect = ags_ladspa_bridge_disconnect;
}

void
ags_ladspa_bridge_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = NULL;
  plugin->set_name = NULL;
  plugin->get_version = ags_ladspa_bridge_get_version;
  plugin->set_version = ags_ladspa_bridge_set_version;
  plugin->get_build_id = ags_ladspa_bridge_get_build_id;
  plugin->set_build_id = ags_ladspa_bridge_set_build_id;
  plugin->get_xml_type = NULL;
  plugin->set_xml_type = NULL;
  plugin->get_ports = NULL;
  plugin->read = NULL;
  plugin->write = NULL;
  plugin->set_ports = NULL;
}

void
ags_ladspa_bridge_init(AgsLadspaBridge *ladspa_bridge)
{
  //TODO:JK: implement me
}

void
ags_ladspa_bridge_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsLadspaBridge *ladspa_bridge;

  ladspa_bridge = AGS_LADSPA_BRIDGE(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ladspa_bridge_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsLadspaBridge *ladspa_bridge;

  ladspa_bridge = AGS_LADSPA_BRIDGE(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ladspa_bridge_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_ladspa_bridge_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

gchar*
ags_ladspa_bridge_get_version(AgsPlugin *plugin)
{
  return(AGS_LADSPA_BRIDGE(plugin)->version);
}

void
ags_ladspa_bridge_set_version(AgsPlugin *plugin, gchar *version)
{
  AgsLadspaBridge *ladspa_bridge;

  ladspa_bridge = AGS_LADSPA_BRIDGE(plugin);

  ladspa_bridge->version = version;
}

gchar*
ags_ladspa_bridge_get_build_id(AgsPlugin *plugin)
{
  return(AGS_LADSPA_BRIDGE(plugin)->build_id);
}

void
ags_ladspa_bridge_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AgsLadspaBridge *ladspa_bridge;

  ladspa_bridge = AGS_LADSPA_BRIDGE(plugin);

  ladspa_bridge->build_id = build_id;
}

/**
 * ags_ladspa_bridge_new:
 * @soundcard: the assigned soundcard.
 *
 * Creates an #AgsLadspaBridge
 *
 * Returns: a new #AgsLadspaBridge
 *
 * Since: 0.4.3
 */
AgsLadspaBridge*
ags_ladspa_bridge_new(GObject *soundcard)
{
  AgsLadspaBridge *ladspa_bridge;
  GValue value = {0,};

  ladspa_bridge = (AgsLadspaBridge *) g_object_new(AGS_TYPE_LADSPA_BRIDGE,
						   NULL);

  if(soundcard != NULL){
    g_value_init(&value, G_TYPE_OBJECT);
    g_value_set_object(&value, soundcard);
    g_object_set_property(G_OBJECT(AGS_MACHINE(ladspa_bridge)->audio),
			  "soundcard\0", &value);
    g_value_unset(&value);
  }

  return(ladspa_bridge);
}
