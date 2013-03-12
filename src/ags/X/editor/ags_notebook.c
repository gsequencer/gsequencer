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

#include <ags/X/editor/ags_notebook.h>
#include <ags/X/editor/ags_notebook_callbacks.h>

#include <ags/X/ags_editor.h>

void ags_notebook_class_init(AgsNotebookClass *notebook);
void ags_notebook_init(AgsNotebook *notebook);
void ags_notebook_connect(AgsNotebook *notebook);
void ags_notebook_destroy(GtkObject *object);
void ags_notebook_show(GtkWidget *widget);
void ags_notebook_paint(AgsNotebook *notebook);

GtkStyle *notebook_style;

GType
ags_notebook_get_type(void)
{
  static GType notebook_type = 0;

  if (!notebook_type){
    static const GtkTypeInfo notebook_info = {
      "AgsNotebook",
      sizeof(AgsNotebook), /* base_init */
      sizeof(AgsNotebookClass), /* base_finalize */
      (GtkClassInitFunc) ags_notebook_class_init,
      (GtkObjectInitFunc) ags_notebook_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      (GtkClassInitFunc) NULL,
    };

    notebook_type = gtk_type_unique (GTK_TYPE_NOTEBOOK, &notebook_info);
  }

  return (notebook_type);
}

void
ags_notebook_class_init(AgsNotebookClass *notebook)
{
}

void
ags_notebook_init(AgsNotebook *notebook)
{
  gtk_widget_set_style((GtkWidget *) notebook, notebook_style);
}

void
ags_notebook_connect(AgsNotebook *notebook)
{
  AgsEditor *editor;

  editor = (AgsEditor *) gtk_widget_get_ancestor((GtkWidget *) notebook, AGS_TYPE_EDITOR);

  g_signal_connect((GObject *) notebook, "destroy",
		   G_CALLBACK(ags_notebook_destroy_callback), (gpointer) notebook);

  g_signal_connect((GObject *) notebook, "show",
		   G_CALLBACK(ags_notebook_show_callback), (gpointer) notebook);

  g_signal_connect((GObject *) editor, "change_machine",
		   G_CALLBACK(ags_notebook_change_machine_callback), notebook);
}

void
ags_notebook_destroy(GtkObject *object)
{
}

void
ags_notebook_show(GtkWidget *widget)
{
}

AgsNotebook*
ags_notebook_new()
{
  AgsNotebook *notebook;

  notebook = (AgsNotebook *) g_object_new(AGS_TYPE_NOTEBOOK, NULL);

  return(notebook);
}
