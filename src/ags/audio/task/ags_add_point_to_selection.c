/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/audio/task/ags_add_point_to_selection.h>

#include <ags/object/ags_connectable.h>

void ags_add_point_to_selection_class_init(AgsAddPointToSelectionClass *add_point_to_selection);
void ags_add_point_to_selection_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_add_point_to_selection_init(AgsAddPointToSelection *add_point_to_selection);
void ags_add_point_to_selection_connect(AgsConnectable *connectable);
void ags_add_point_to_selection_disconnect(AgsConnectable *connectable);
void ags_add_point_to_selection_finalize(GObject *gobject);

void ags_add_point_to_selection_launch(AgsTask *task);

static gpointer ags_add_point_to_selection_parent_class = NULL;
static AgsConnectableInterface *ags_add_point_to_selection_parent_connectable_interface;

GType
ags_add_point_to_selection_get_type()
{
  static GType ags_type_add_point_to_selection = 0;

  if(!ags_type_add_point_to_selection){
    static const GTypeInfo ags_add_point_to_selection_info = {
      sizeof (AgsAddPointToSelectionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_add_point_to_selection_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAddPointToSelection),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_add_point_to_selection_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_add_point_to_selection_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_add_point_to_selection = g_type_register_static(AGS_TYPE_TASK,
							     "AgsAddPointToSelection",
							     &ags_add_point_to_selection_info,
							     0);

    g_type_add_interface_static(ags_type_add_point_to_selection,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_add_point_to_selection);
}

void
ags_add_point_to_selection_class_init(AgsAddPointToSelectionClass *add_point_to_selection)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_add_point_to_selection_parent_class = g_type_class_peek_parent(add_point_to_selection);

  /* gobject */
  gobject = (GObjectClass *) add_point_to_selection;

  gobject->finalize = ags_add_point_to_selection_finalize;

  /* task */
  task = (AgsTaskClass *) add_point_to_selection;

  task->launch = ags_add_point_to_selection_launch;
}

void
ags_add_point_to_selection_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_add_point_to_selection_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_add_point_to_selection_connect;
  connectable->disconnect = ags_add_point_to_selection_disconnect;
}

void
ags_add_point_to_selection_init(AgsAddPointToSelection *add_point_to_selection)
{
  add_point_to_selection->notation = NULL;
  add_point_to_selection->x = 0;
  add_point_to_selection->y = 0;
  add_point_to_selection->replace_current_selection = TRUE;
}

void
ags_add_point_to_selection_connect(AgsConnectable *connectable)
{
  ags_add_point_to_selection_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_add_point_to_selection_disconnect(AgsConnectable *connectable)
{
  ags_add_point_to_selection_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_add_point_to_selection_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_add_point_to_selection_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_add_point_to_selection_launch(AgsTask *task)
{
  AgsAddPointToSelection *add_point_to_selection;

  add_point_to_selection = AGS_ADD_POINT_TO_SELECTION(task);

  /* add note */
  ags_notation_add_point_to_selection(add_point_to_selection->notation,
				      add_point_to_selection->x, add_point_to_selection->y,
				      add_point_to_selection->replace_current_selection);
}

AgsAddPointToSelection*
ags_add_point_to_selection_new(AgsNotation *notation,
			       guint x, guint y,
			       gboolean replace_current_selection)
{
  AgsAddPointToSelection *add_point_to_selection;

  add_point_to_selection = (AgsAddPointToSelection *) g_object_new(AGS_TYPE_ADD_POINT_TO_SELECTION,
								   NULL);

  add_point_to_selection->notation = notation;
  add_point_to_selection->x = x;
  add_point_to_selection->y = y;
  add_point_to_selection->replace_current_selection = replace_current_selection;

  return(add_point_to_selection);
}
