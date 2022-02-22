/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/app/machine/ags_matrix_bridge.h>
#include <ags/app/machine/ags_matrix_bridge_callbacks.h>

#include <ags/app/machine/ags_matrix_bulk_input.h>

void ags_matrix_bridge_class_init(AgsMatrixBridgeClass *matrix_bridge);
void ags_matrix_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_matrix_bridge_init(AgsMatrixBridge *matrix_bridge);
void ags_matrix_bridge_finalize(GObject *gobject);

void ags_matrix_bridge_connect(AgsConnectable *connectable);
void ags_matrix_bridge_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_matrix_bridge
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsMatrixBridge
 * @section_id:
 * @include: ags/app/machine/ags_matrix_bridge.h
 *
 * #AgsMatrixBridge is a composite widget to visualize all #AgsChannel. It should be
 * packed by an #AgsMachine.
 */

static gpointer ags_matrix_bridge_parent_class = NULL;
static AgsConnectableInterface *ags_matrix_bridge_parent_connectable_interface;

GType
ags_matrix_bridge_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_matrix_bridge = 0;

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

    ags_type_matrix_bridge = g_type_register_static(AGS_TYPE_EFFECT_BRIDGE,
						    "AgsMatrixBridge", &ags_matrix_bridge_info,
						    0);

    g_type_add_interface_static(ags_type_matrix_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_matrix_bridge);
  }

  return g_define_type_id__volatile;
}

void
ags_matrix_bridge_class_init(AgsMatrixBridgeClass *matrix_bridge)
{
  GObjectClass *gobject;

  ags_matrix_bridge_parent_class = g_type_class_peek_parent(matrix_bridge);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(matrix_bridge);

  gobject->finalize = ags_matrix_bridge_finalize;
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
ags_matrix_bridge_init(AgsMatrixBridge *matrix_bridge)
{
  GtkFrame *frame;
  GtkExpander *expander;
  GtkGrid *grid;

  frame = (GtkFrame *) gtk_frame_new("input bridge");
  gtk_box_pack_start((GtkBox *) AGS_EFFECT_BRIDGE(matrix_bridge),
		     (GtkWidget *) frame,
		     FALSE, FALSE,
		     0);

  expander = (GtkExpander *) gtk_expander_new("show/hide");
  gtk_container_add((GtkContainer *) frame,
		    (GtkWidget *) expander);

  grid = (GtkGrid *) gtk_grid_new();
  gtk_container_add((GtkContainer *) expander,
		    (GtkWidget *)  grid);

  AGS_EFFECT_BRIDGE(matrix_bridge)->bulk_input = (GtkWidget *) ags_matrix_bulk_input_new(NULL,
											 AGS_TYPE_INPUT);

  gtk_widget_set_valign((GtkWidget *) AGS_EFFECT_BRIDGE(matrix_bridge)->bulk_input,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) AGS_EFFECT_BRIDGE(matrix_bridge)->bulk_input,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) AGS_EFFECT_BRIDGE(matrix_bridge)->bulk_input,
		  0, 0,
		  1, 1);
}

void
ags_matrix_bridge_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_matrix_bridge_parent_class)->finalize(gobject);
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
 * @audio: the #AgsAudio to visualize
 *
 * Create a new instance of #AgsMatrixBridge
 *
 * Returns: the new #AgsMatrixBridge
 *
 * Since: 3.0.0
 */
AgsMatrixBridge*
ags_matrix_bridge_new(AgsAudio *audio)
{
  AgsMatrixBridge *matrix_bridge;

  matrix_bridge = (AgsMatrixBridge *) g_object_new(AGS_TYPE_MATRIX_BRIDGE,
						   "audio", audio,
						   NULL);

  return(matrix_bridge);
}
