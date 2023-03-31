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

#include <ags/app/ags_connection_editor_dialog.h>

#include <ags/app/ags_ui_provider.h>

#include <ags/i18n.h>

void ags_connection_editor_dialog_class_init(AgsConnectionEditorDialogClass *connection_editor_dialog);
void ags_connection_editor_dialog_init(AgsConnectionEditorDialog *connection_editor_dialog);

/**
 * SECTION:ags_connection_editor_dialog
 * @short_description: edit connection settings
 * @title: AgsConnectionEditorDialog
 * @section_id:
 * @include: ags/app/ags_connection_editor_dialog.h
 *
 * #AgsConnectionEditorDialog is a dialog widget to edit connection settings.
 */

static gpointer ags_connection_editor_dialog_parent_class = NULL;

GType
ags_connection_editor_dialog_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_connection_editor_dialog = 0;

    static const GTypeInfo ags_connection_editor_dialog_info = {
      sizeof (AgsConnectionEditorDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_connection_editor_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsConnectionEditorDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_connection_editor_dialog_init,
    };

    ags_type_connection_editor_dialog = g_type_register_static(GTK_TYPE_DIALOG,
							       "AgsConnectionEditorDialog", &ags_connection_editor_dialog_info,
							       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_connection_editor_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_connection_editor_dialog_class_init(AgsConnectionEditorDialogClass *connection_editor_dialog)
{
  ags_connection_editor_dialog_parent_class = g_type_class_peek_parent(connection_editor_dialog);
}

void
ags_connection_editor_dialog_init(AgsConnectionEditorDialog *connection_editor_dialog)
{
  GtkBox *content_area;
  
  connection_editor_dialog->connection_editor = ags_connection_editor_new(NULL);

  gtk_widget_set_hexpand((GtkWidget *) connection_editor_dialog->connection_editor,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) connection_editor_dialog->connection_editor,
			 TRUE);

  gtk_widget_set_halign((GtkWidget *) connection_editor_dialog->connection_editor,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) connection_editor_dialog->connection_editor,
			GTK_ALIGN_FILL);

  content_area = (GtkBox *) gtk_dialog_get_content_area((GtkDialog *) connection_editor_dialog);
  gtk_box_append(content_area,
		 (GtkWidget *) connection_editor_dialog->connection_editor);

  gtk_window_set_default_size((GtkWindow *) connection_editor_dialog,
			      800, 600);
  
  gtk_dialog_add_buttons((GtkDialog *) connection_editor_dialog,
			 i18n("_OK"),
			 GTK_RESPONSE_ACCEPT,
			 i18n("_Cancel"),
			 GTK_RESPONSE_REJECT,
			 NULL);
}

/**
 * ags_connection_editor_dialog_new:
 * @title: the title
 * @transient_for: the transient for
 *
 * Creates an #AgsConnectionEditorDialog
 *
 * Returns: a new #AgsConnectionEditorDialog
 *
 * Since: 4.0.0
 */
AgsConnectionEditorDialog*
ags_connection_editor_dialog_new(gchar *title,
				 GtkWindow *transient_for)
{
  AgsConnectionEditorDialog *connection_editor_dialog;

  connection_editor_dialog = (AgsConnectionEditorDialog *) g_object_new(AGS_TYPE_CONNECTION_EDITOR_DIALOG,
									"title", title,
									"transient-for", transient_for,
									NULL);

  return(connection_editor_dialog);
}
