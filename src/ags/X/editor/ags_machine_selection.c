/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/X/editor/ags_machine_selection.h>

#include <ags-lib/object/ags_connectable.h>

void ags_machine_selection_class_init(AgsMachineSelectionClass *machine_selection);
void ags_machine_selection_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_machine_selection_init(AgsMachineSelection *machine_selection);
void ags_machine_selection_connect(AgsConnectable *connectable);
void ags_machine_selection_disconnect(AgsConnectable *connectable);
void ags_machine_selection_show(GtkWidget *widget);

static gpointer ags_machine_selection_parent_class = NULL;

GType
ags_machine_selection_get_type(void)
{
  static GType ags_type_machine_selection = 0;

  if(!ags_type_machine_selection){
    static const GTypeInfo ags_machine_selection_info = {
      sizeof (AgsMachineSelectionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_machine_selection_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMachineSelection),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_machine_selection_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_machine_selection_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_machine_selection = g_type_register_static(GTK_TYPE_DIALOG,
							"AgsMachineSelection\0", &ags_machine_selection_info,
							0);
    
    g_type_add_interface_static(ags_type_machine_selection,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_machine_selection);
}

void
ags_machine_selection_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_machine_selection_connect;
  connectable->disconnect = ags_machine_selection_disconnect;
}

void
ags_machine_selection_class_init(AgsMachineSelectionClass *machine_selection)
{
  GObjectClass *gobject;

  ags_machine_selection_parent_class = g_type_class_peek_parent(machine_selection);

  /* GObjectClass */
  gobject = (GObjectClass *) machine_selection;
}

void
ags_machine_selection_init(AgsMachineSelection *machine_selection)
{
}

void
ags_machine_selection_connect(AgsConnectable *connectable)
{
}

void
ags_machine_selection_disconnect(AgsConnectable *connectable)
{
}

AgsMachineSelection*
ags_machine_selection_new()
{
  AgsMachineSelection *machine_selection;

  machine_selection = (AgsMachineSelection *) g_object_new(AGS_TYPE_MACHINE_SELECTION,
					   NULL);

  return(machine_selection);
}
