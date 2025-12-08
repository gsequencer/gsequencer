/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/app/machine/ags_modulation_matrix.h>
#include <ags/app/machine/ags_modulation_matrix_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_line.h>

#include <math.h>

#include <ags/ags_api_config.h>

#include <ags/i18n.h>

void ags_modulation_matrix_class_init(AgsModulationMatrixClass *modulation_matrix);
void ags_modulation_matrix_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_modulation_matrix_init(AgsModulationMatrix *modulation_matrix);
void ags_modulation_matrix_finalize(GObject *gobject);

gboolean ags_modulation_matrix_is_connected(AgsConnectable *connectable);
void ags_modulation_matrix_connect(AgsConnectable *connectable);
void ags_modulation_matrix_disconnect(AgsConnectable *connectable);

void ags_modulation_matrix_show(GtkWidget *widget);
/**
 * SECTION:ags_modulation_matrix
 * @short_description: modular synth
 * @title: AgsModulationMatrix
 * @section_id:
 * @include: ags/app/machine/ags_modulation_matrix.h
 *
 * The #AgsModulationMatrix is a composite widget to act as modular synth.
 */

static gpointer ags_modulation_matrix_parent_class = NULL;
static AgsConnectableInterface *ags_modulation_matrix_parent_connectable_interface;

GType
ags_modulation_matrix_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_modulation_matrix = 0;

    static const GTypeInfo ags_modulation_matrix_info = {
      sizeof(AgsModulationMatrixClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_modulation_matrix_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsModulationMatrix),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_modulation_matrix_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_modulation_matrix_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_modulation_matrix = g_type_register_static(GTK_TYPE_BOX,
							"AgsModulationMatrix", &ags_modulation_matrix_info,
							0);
    
    g_type_add_interface_static(ags_type_modulation_matrix,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_modulation_matrix);
  }

  return(g_define_type_id__static);
}

void
ags_modulation_matrix_class_init(AgsModulationMatrixClass *modulation_matrix)
{
  GObjectClass *gobject;

  ags_modulation_matrix_parent_class = g_type_class_peek_parent(modulation_matrix);

  /* GObjectClass */
  gobject = (GObjectClass *) modulation_matrix;

  gobject->finalize = ags_modulation_matrix_finalize;
}

void
ags_modulation_matrix_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_modulation_matrix_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_connected = ags_modulation_matrix_is_connected;

  connectable->connect = ags_modulation_matrix_connect;
  connectable->disconnect = ags_modulation_matrix_disconnect;
}

void
ags_modulation_matrix_init(AgsModulationMatrix *modulation_matrix)
{
  //TODO:JK: implement me
}

void
ags_modulation_matrix_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_modulation_matrix_parent_class)->finalize(gobject);
}

gboolean
ags_modulation_matrix_is_connected(AgsConnectable *connectable)
{
  AgsModulationMatrix *modulation_matrix;

  gboolean is_connected;
  
  /* AgsModulationMatrix */
  modulation_matrix = AGS_MODULATION_MATRIX(connectable);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (modulation_matrix->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_modulation_matrix_connect(AgsConnectable *connectable)
{
  AgsModulationMatrix *modulation_matrix;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  ags_modulation_matrix_parent_connectable_interface->connect(connectable);
  
  /* AgsModulationMatrix */
  modulation_matrix = AGS_MODULATION_MATRIX(connectable);

  //TODO:JK: implement me
}

void
ags_modulation_matrix_disconnect(AgsConnectable *connectable)
{
  AgsModulationMatrix *modulation_matrix;
  
  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  ags_modulation_matrix_parent_connectable_interface->disconnect(connectable);

  /* AgsModulationMatrix */
  modulation_matrix = AGS_MODULATION_MATRIX(connectable);

  //TODO:JK: implement me
}

/**
 * ags_modulation_matrix_new:
 *
 * Create a new instance of #AgsModulationMatrix
 *
 * Returns: the new #AgsModulationMatrix
 *
 * Since: 8.2.0
 */
AgsModulationMatrix*
ags_modulation_matrix_new()
{
  AgsModulationMatrix *modulation_matrix;

  modulation_matrix = (AgsModulationMatrix *) g_object_new(AGS_TYPE_MODULATION_MATRIX,
							   NULL);

  return(modulation_matrix);
}
