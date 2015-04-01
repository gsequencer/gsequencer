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

#include <ags/X/machine/ags_matrix_bridge.h>
#include <ags/X/machine/ags_matrix_bridge_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_plugin.h>

#include <ags/audio/ags_input.h>

#include <ags/X/machine/ags_matrix_bulk_input.h>

void ags_matrix_bridge_class_init(AgsMatrixBridgeClass *matrix_bridge);
void ags_matrix_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_matrix_bridge_plugin_interface_init(AgsPluginInterface *plugin);
void ags_matrix_bridge_init(AgsMatrixBridge *matrix_bridge);
void ags_matrix_bridge_connect(AgsConnectable *connectable);
void ags_matrix_bridge_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_matrix_bridge
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsMatrixBridge
 * @section_id:
 * @include: ags/X/ags_matrix_bridge.h
 *
 * #AgsMatrixBridge is a composite widget to visualize all #AgsChannel. It should be
 * packed by an #AgsMachine.
 */

static gpointer ags_matrix_bridge_parent_class = NULL;
static AgsConnectableInterface *ags_matrix_bridge_parent_connectable_interface;

GType
ags_matrix_bridge_get_type(void)
{
  static GType ags_type_matrix_bridge = 0;

  if(!ags_type_matrix_bridge){
    static const GTypeInfo ags_matrix_bridge_info = {
      sizeof(AgsMatrixBridgeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_matrix_bridge_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsMatrixBridge),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_matrix_bridge_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_matrix_bridge_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_matrix_bridge_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_matrix_bridge = g_type_register_static(AGS_TYPE_EFFECT_BRIDGE,
						    "AgsMatrixBridge\0", &ags_matrix_bridge_info,
						    0);

    g_type_add_interface_static(ags_type_matrix_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_matrix_bridge,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_matrix_bridge);
}

void
ags_matrix_bridge_class_init(AgsMatrixBridgeClass *matrix_bridge)
{
  GObjectClass *gobject;

  ags_matrix_bridge_parent_class = g_type_class_peek_parent(matrix_bridge);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(matrix_bridge);
}

void
ags_matrix_bridge_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_matrix_bridge_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_matrix_bridge_connect;
  connectable->disconnect = ags_matrix_bridge_disconnect;
}

void
ags_matrix_bridge_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_xml_type = NULL;
  plugin->set_xml_type = NULL;
  plugin->get_ports = NULL;
  plugin->read = NULL;
  plugin->write = NULL;
  plugin->set_ports = NULL;
}

void
ags_matrix_bridge_init(AgsMatrixBridge *matrix_bridge)
{
  GtkFrame *frame;
  GtkExpander *expander;
  GtkTable *table;

  frame = (GtkFrame *) gtk_frame_new("input bridge\0");
  gtk_box_pack_start(AGS_EFFECT_BRIDGE(matrix_bridge),
		     frame,
		     FALSE, FALSE,
		     0);

  expander = gtk_expander_new("show/hide\0");
  gtk_container_add(frame,
		    expander);

  table = (GtkTable *) gtk_table_new(1, 2, FALSE);
  gtk_container_add(expander,
		    table);

  AGS_EFFECT_BRIDGE(matrix_bridge)->bulk_input = (GtkWidget *) ags_matrix_bulk_input_new(NULL,
											 AGS_TYPE_INPUT);
  gtk_table_attach(table,
		   AGS_EFFECT_BRIDGE(matrix_bridge)->bulk_input,
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_matrix_bridge_connect(AgsConnectable *connectable)
{
  if((AGS_EFFECT_BRIDGE_CONNECTED & (AGS_EFFECT_BRIDGE(connectable)->flags)) != 0){
    return;
  }

  ags_matrix_bridge_parent_connectable_interface->connect(connectable);

  //TODO:JK: implement me
}

void
ags_matrix_bridge_disconnect(AgsConnectable *connectable)
{
  if((AGS_EFFECT_BRIDGE_CONNECTED & (AGS_EFFECT_BRIDGE(connectable)->flags)) == 0){
    return;
  }

  ags_matrix_bridge_parent_connectable_interface->connect(connectable);

  //TODO:JK: implement me
}

/**
 * ags_matrix_bridge_new:
 * @matrix_bridge: the parent matrix_bridge
 * @audio: the #AgsAudio to visualize
 *
 * Creates an #AgsMatrixBridge
 *
 * Returns: a new #AgsMatrixBridge
 *
 * Since: 0.4
 */
AgsMatrixBridge*
ags_matrix_bridge_new(AgsAudio *audio)
{
  AgsMatrixBridge *matrix_bridge;

  matrix_bridge = (AgsMatrixBridge *) g_object_new(AGS_TYPE_MATRIX_BRIDGE,
						   "audio\0", audio,
						   NULL);

  return(matrix_bridge);
}
