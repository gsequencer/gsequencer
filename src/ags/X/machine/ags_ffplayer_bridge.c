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

#include <ags/X/machine/ags_ffplayer_bridge.h>
#include <ags/X/machine/ags_ffplayer_bridge_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_plugin.h>

#include <ags/X/ags_effect_bulk.h>

#include <ags/X/machine/ags_ffplayer_bulk_input.h>
#include <ags/X/machine/ags_ffplayer_input_pad.h>
#include <ags/X/machine/ags_ffplayer_input_line.h>

void ags_ffplayer_bridge_class_init(AgsFFPlayerBridgeClass *ffplayer_bridge);
void ags_ffplayer_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ffplayer_bridge_plugin_interface_init(AgsPluginInterface *plugin);
void ags_ffplayer_bridge_init(AgsFFPlayerBridge *ffplayer_bridge);
void ags_ffplayer_bridge_connect(AgsConnectable *connectable);
void ags_ffplayer_bridge_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_ffplayer_bridge
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsFFPlayerBridge
 * @section_id:
 * @include: ags/X/ags_ffplayer_bridge.h
 *
 * #AgsFFPlayerBridge is a composite widget to visualize all #AgsChannel. It should be
 * packed by an #AgsMachine.
 */

static gpointer ags_ffplayer_bridge_parent_class = NULL;
static AgsConnectableInterface *ags_ffplayer_bridge_parent_connectable_interface;

GType
ags_ffplayer_bridge_get_type(void)
{
  static GType ags_type_ffplayer_bridge = 0;

  if(!ags_type_ffplayer_bridge){
    static const GTypeInfo ags_ffplayer_bridge_info = {
      sizeof(AgsFFPlayerBridgeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ffplayer_bridge_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFFPlayerBridge),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ffplayer_bridge_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ffplayer_bridge_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_ffplayer_bridge_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ffplayer_bridge = g_type_register_static(AGS_TYPE_EFFECT_BRIDGE,
						      "AgsFFPlayerBridge\0", &ags_ffplayer_bridge_info,
						      0);

    g_type_add_interface_static(ags_type_ffplayer_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_ffplayer_bridge,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_ffplayer_bridge);
}

void
ags_ffplayer_bridge_class_init(AgsFFPlayerBridgeClass *ffplayer_bridge)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_ffplayer_bridge_parent_class = g_type_class_peek_parent(ffplayer_bridge);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(ffplayer_bridge);
}

void
ags_ffplayer_bridge_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_ffplayer_bridge_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_ffplayer_bridge_connect;
  connectable->disconnect = ags_ffplayer_bridge_disconnect;
}

void
ags_ffplayer_bridge_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_xml_type = NULL;
  plugin->set_xml_type = NULL;
  plugin->get_ports = NULL;
  plugin->read = NULL;
  plugin->write = NULL;
  plugin->set_ports = NULL;
}

void
ags_ffplayer_bridge_init(AgsFFPlayerBridge *ffplayer_bridge)
{
  GtkFrame *frame;
  GtkExpander *expander;
  GtkTable *table;

  AGS_EFFECT_BRIDGE(ffplayer_bridge)->input_pad_type = AGS_TYPE_FFPLAYER_INPUT_PAD;
  AGS_EFFECT_BRIDGE(ffplayer_bridge)->input_line_type = AGS_TYPE_FFPLAYER_INPUT_LINE;

  frame = (GtkFrame *) gtk_frame_new("input bridge\0");
  gtk_box_pack_start(AGS_EFFECT_BRIDGE(ffplayer_bridge),
		     frame,
		     FALSE, FALSE,
		     0);

  expander = gtk_expander_new("show/hide\0");
  gtk_container_add(frame,
		    expander);

  table = (GtkTable *) gtk_table_new(1, 2, FALSE);
  gtk_container_add(expander,
		    table);

  AGS_EFFECT_BRIDGE(ffplayer_bridge)->bulk_input = (GtkWidget *) g_object_new(AGS_TYPE_FFPLAYER_BULK_INPUT,
									      NULL);
  gtk_table_attach(table,
		   AGS_EFFECT_BRIDGE(ffplayer_bridge)->bulk_input,
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  AGS_EFFECT_BRIDGE(ffplayer_bridge)->input = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_table_attach(table,
		   AGS_EFFECT_BRIDGE(ffplayer_bridge)->input,
		   1, 2,
		   0, 1,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);
}

void
ags_ffplayer_bridge_connect(AgsConnectable *connectable)
{
  if((AGS_EFFECT_BRIDGE_CONNECTED & (AGS_EFFECT_BRIDGE(connectable)->flags)) != 0){
    return;
  }

  ags_ffplayer_bridge_parent_connectable_interface->connect(connectable);

  //TODO:JK: implement me
}

void
ags_ffplayer_bridge_disconnect(AgsConnectable *connectable)
{
  if((AGS_EFFECT_BRIDGE_CONNECTED & (AGS_EFFECT_BRIDGE(connectable)->flags)) == 0){
    return;
  }

  ags_ffplayer_bridge_parent_connectable_interface->connect(connectable);

  //TODO:JK: implement me
}

/**
 * ags_ffplayer_bridge_new:
 * @ffplayer_bridge: the parent ffplayer_bridge
 * @audio: the #AgsAudio to visualize
 *
 * Creates an #AgsFFPlayerBridge
 *
 * Returns: a new #AgsFFPlayerBridge
 *
 * Since: 0.4
 */
AgsFFPlayerBridge*
ags_ffplayer_bridge_new(AgsAudio *audio)
{
  AgsFFPlayerBridge *ffplayer_bridge;

  ffplayer_bridge = (AgsFFPlayerBridge *) g_object_new(AGS_TYPE_FFPLAYER_BRIDGE,
						       "audio\0", audio,
						       NULL);

  return(ffplayer_bridge);
}
