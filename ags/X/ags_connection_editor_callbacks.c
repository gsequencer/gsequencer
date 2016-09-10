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

#include <ags/X/ags_connection_editor_callbacks.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

int
ags_connection_editor_switch_page_callback(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, AgsConnectionEditor *connection_editor)
{
  /* empty */
}

int
ags_connection_editor_apply_callback(GtkWidget *widget, AgsConnectionEditor *connection_editor)
{
  ags_applicable_apply(AGS_APPLICABLE(connection_editor));

  //TODO:JK: remove me
  //  ags_applicable_reset(AGS_APPLICABLE(connection_editor));

  return(0);
}

int
ags_connection_editor_ok_callback(GtkWidget *widget, AgsConnectionEditor *connection_editor)
{
  //  ags_applicable_set_update(AGS_APPLICABLE(connection_editor), FALSE);
  ags_connectable_disconnect(AGS_CONNECTABLE(connection_editor));
  ags_applicable_apply(AGS_APPLICABLE(connection_editor));

  gtk_widget_destroy((GtkWidget *) connection_editor);

  return(0);
}

int
ags_connection_editor_cancel_callback(GtkWidget *widget, AgsConnectionEditor *connection_editor)
{
  gtk_widget_destroy((GtkWidget *) connection_editor);

  return(0);
}
