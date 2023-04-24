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

#include <ags/app/machine/ags_ffplayer_bridge.h>
#include <ags/app/machine/ags_ffplayer_bridge_callbacks.h>

#include <ags/app/ags_effect_bulk.h>

#include <ags/app/machine/ags_ffplayer_bulk_input.h>
#include <ags/app/machine/ags_ffplayer_input_pad.h>
#include <ags/app/machine/ags_ffplayer_input_line.h>

void ags_ffplayer_bridge_class_init(AgsFFPlayerBridgeClass *ffplayer_bridge);
void ags_ffplayer_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ffplayer_bridge_init(AgsFFPlayerBridge *ffplayer_bridge);

void ags_ffplayer_bridge_connect(AgsConnectable *connectable);
void ags_ffplayer_bridge_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_ffplayer_bridge
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsFFPlayerBridge
 * @section_id:
 * @include: ags/app/machine/ags_ffplayer_bridge.h
 *
 * #AgsFFPlayerBridge is a composite widget to visualize all #AgsChannel. It should be
 * packed by an #AgsMachine.
 */

static gpointer ags_ffplayer_bridge_parent_class = NULL;
static AgsConnectableInterface *ags_ffplayer_bridge_parent_connectable_interface;

GType
ags_ffplayer_bridge_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_ffplayer_bridge = 0;

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

    ags_type_ffplayer_bridge = g_type_register_static(AGS_TYPE_EFFECT_BRIDGE,
						      "AgsFFPlayerBridge", &ags_ffplayer_bridge_info,
						      0);

    g_type_add_interface_static(ags_type_ffplayer_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_ffplayer_bridge);
  }

  return g_define_type_id__volatile;
}

void
ags_ffplayer_bridge_class_init(AgsFFPlayerBridgeClass *ffplayer_bridge)
{
  ags_ffplayer_bridge_parent_class = g_type_class_peek_parent(ffplayer_bridge);
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
ags_ffplayer_bridge_init(AgsFFPlayerBridge *ffplayer_bridge)
{
  GtkFrame *frame;
  GtkExpander *expander;
  GtkGrid *grid;

  AGS_EFFECT_BRIDGE(ffplayer_bridge)->input_pad_type = AGS_TYPE_FFPLAYER_INPUT_PAD;
  AGS_EFFECT_BRIDGE(ffplayer_bridge)->input_line_type = AGS_TYPE_FFPLAYER_INPUT_LINE;

  frame = (GtkFrame *) gtk_frame_new("input bridge");
  gtk_grid_attach((GtkGrid *) AGS_EFFECT_BRIDGE(ffplayer_bridge),
		  (GtkWidget *) frame,
		  0, 0,
		  1, 1);

  expander = (GtkExpander *) gtk_expander_new("show/hide");
  gtk_frame_set_child(frame,
		      (GtkWidget *) expander);

  grid = (GtkGrid *) gtk_grid_new();
  gtk_expander_set_child(expander,
			 (GtkWidget *) grid);

  AGS_EFFECT_BRIDGE(ffplayer_bridge)->bulk_input = (GtkWidget *) g_object_new(AGS_TYPE_FFPLAYER_BULK_INPUT,
									      NULL);
  
  AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(ffplayer_bridge)->bulk_input)->parent_bridge = (GtkWidget *) ffplayer_bridge;
  
  gtk_widget_set_valign((GtkWidget *) AGS_EFFECT_BRIDGE(ffplayer_bridge)->bulk_input,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) AGS_EFFECT_BRIDGE(ffplayer_bridge)->bulk_input,
			GTK_ALIGN_FILL);
  
  AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(ffplayer_bridge)->bulk_input)->parent_bridge = ffplayer_bridge;
  gtk_grid_attach(grid,
		  (GtkWidget *) AGS_EFFECT_BRIDGE(ffplayer_bridge)->bulk_input,
		  0, 0,
		  1, 1);
  
  AGS_EFFECT_BRIDGE(ffplayer_bridge)->input = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
								     0);

  gtk_widget_set_valign((GtkWidget *) AGS_EFFECT_BRIDGE(ffplayer_bridge)->input,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) AGS_EFFECT_BRIDGE(ffplayer_bridge)->input,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) AGS_EFFECT_BRIDGE(ffplayer_bridge)->input,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) AGS_EFFECT_BRIDGE(ffplayer_bridge)->input,
			 TRUE);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) AGS_EFFECT_BRIDGE(ffplayer_bridge)->input,
		  1, 0,
		  1, 1);
}

void
ags_ffplayer_bridge_connect(AgsConnectable *connectable)
{
  if((AGS_CONNECTABLE_CONNECTED & (AGS_EFFECT_BRIDGE(connectable)->connectable_flags)) != 0){
    return;
  }

  ags_ffplayer_bridge_parent_connectable_interface->connect(connectable);

  //TODO:JK: implement me
}

void
ags_ffplayer_bridge_disconnect(AgsConnectable *connectable)
{
  if((AGS_CONNECTABLE_CONNECTED & (AGS_EFFECT_BRIDGE(connectable)->connectable_flags)) == 0){
    return;
  }

  ags_ffplayer_bridge_parent_connectable_interface->disconnect(connectable);

  //TODO:JK: implement me
}

/**
 * ags_ffplayer_bridge_new:
 * @audio: the #AgsAudio to visualize
 *
 * Creates an #AgsFFPlayerBridge
 *
 * Returns: a new #AgsFFPlayerBridge
 *
 * Since: 3.0.0
 */
AgsFFPlayerBridge*
ags_ffplayer_bridge_new(AgsAudio *audio)
{
  AgsFFPlayerBridge *ffplayer_bridge;

  ffplayer_bridge = (AgsFFPlayerBridge *) g_object_new(AGS_TYPE_FFPLAYER_BRIDGE,
						       "audio", audio,
						       NULL);

  return(ffplayer_bridge);
}
