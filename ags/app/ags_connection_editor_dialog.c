/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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
#include <ags/app/ags_connection_editor_dialog_callbacks.h>

#include <ags/app/ags_ui_provider.h>

#include <ags/i18n.h>

void ags_connection_editor_dialog_class_init(AgsConnectionEditorDialogClass *connection_editor_dialog);
void ags_connection_editor_dialog_init(AgsConnectionEditorDialog *connection_editor_dialog);

void ags_connection_editor_dialog_activate_button_callback(GtkButton *activate_button,
							   AgsConnectionEditorDialog *connection_editor_dialog);

gboolean ags_connection_editor_dialog_key_pressed_callback(GtkEventControllerKey *event_controller,
							   guint keyval,
							   guint keycode,
							   GdkModifierType state,
							   AgsConnectionEditorDialog *connection_editor_dialog);
void ags_connection_editor_dialog_key_released_callback(GtkEventControllerKey *event_controller,
							guint keyval,
							guint keycode,
							GdkModifierType state,
							AgsConnectionEditorDialog *connection_editor_dialog);
gboolean ags_connection_editor_dialog_modifiers_callback(GtkEventControllerKey *event_controller,
							 GdkModifierType keyval,
							 AgsConnectionEditorDialog *connection_editor_dialog);

/**
 * SECTION:ags_connection_editor_dialog
 * @short_description: edit connection settings
 * @title: AgsConnectionEditorDialog
 * @section_id:
 * @include: ags/app/ags_connection_editor_dialog.h
 *
 * #AgsConnectionEditorDialog is a dialog widget to edit connection settings.
 */

enum{
  RESPONSE,
  LAST_SIGNAL,
};

static gpointer ags_connection_editor_dialog_parent_class = NULL;

static guint connection_editor_dialog_signals[LAST_SIGNAL];

GType
ags_connection_editor_dialog_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
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

    ags_type_connection_editor_dialog = g_type_register_static(GTK_TYPE_WINDOW,
							       "AgsConnectionEditorDialog", &ags_connection_editor_dialog_info,
							       0);

    g_once_init_leave(&g_define_type_id__static, ags_type_connection_editor_dialog);
  }

  return g_define_type_id__static;
}

void
ags_connection_editor_dialog_class_init(AgsConnectionEditorDialogClass *connection_editor_dialog)
{
  ags_connection_editor_dialog_parent_class = g_type_class_peek_parent(connection_editor_dialog);

  /* AgsConnectionEditorDialog */
  connection_editor_dialog->response = NULL;
  
  /* signals */
  /**
   * AgsConnectionEditorDialog::response:
   * @connection_editor_dialog: the #AgsConnectionEditorDialog
   *
   * The ::response signal notifies about window interaction.
   *
   * Since: 6.10.0
   */
  connection_editor_dialog_signals[RESPONSE] =
    g_signal_new("response",
		 G_TYPE_FROM_CLASS(connection_editor_dialog),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsConnectionEditorDialogClass, response),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__INT,
		 G_TYPE_NONE, 1,
		 G_TYPE_INT);
}

void
ags_connection_editor_dialog_init(AgsConnectionEditorDialog *connection_editor_dialog)
{
  GtkBox *content_area;
  GtkBox *vbox;
  
  GtkEventController *event_controller;

  connection_editor_dialog->flags = 0;

  gtk_window_set_title(GTK_WINDOW(connection_editor_dialog),
		       i18n("connection editor"));
  gtk_window_set_deletable(GTK_WINDOW(connection_editor_dialog),
			   TRUE);

  gtk_window_set_default_size((GtkWindow *) connection_editor_dialog,
			      800, 600);

  g_signal_connect(connection_editor_dialog, "close-request",
		   G_CALLBACK(ags_connection_editor_dialog_close_request_callback), NULL);

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) connection_editor_dialog,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_connection_editor_dialog_key_pressed_callback), connection_editor_dialog);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_connection_editor_dialog_key_released_callback), connection_editor_dialog);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_connection_editor_dialog_modifiers_callback), connection_editor_dialog);
  
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_window_set_child((GtkWindow *) connection_editor_dialog,
		       (GtkWidget *) vbox);

  connection_editor_dialog->connection_editor = ags_connection_editor_new(NULL);

  gtk_widget_set_hexpand((GtkWidget *) connection_editor_dialog->connection_editor,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) connection_editor_dialog->connection_editor,
			 TRUE);

  gtk_widget_set_halign((GtkWidget *) connection_editor_dialog->connection_editor,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) connection_editor_dialog->connection_editor,
			GTK_ALIGN_FILL);

  gtk_box_append(vbox,
		 (GtkWidget *) connection_editor_dialog->connection_editor);
  
  /* buttons */
  connection_editor_dialog->action_area = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
								 AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) connection_editor_dialog->action_area,
			GTK_ALIGN_END);

  gtk_box_append(vbox,
		 (GtkWidget *) connection_editor_dialog->action_area);

  connection_editor_dialog->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("ok"));

  gtk_box_append(connection_editor_dialog->action_area,
		 (GtkWidget *) connection_editor_dialog->activate_button);

  g_signal_connect(connection_editor_dialog->activate_button, "clicked",
		   G_CALLBACK(ags_connection_editor_dialog_activate_button_callback), connection_editor_dialog);
}

void
ags_connection_editor_dialog_activate_button_callback(GtkButton *activate_button,
						      AgsConnectionEditorDialog *connection_editor_dialog)
{
  ags_connection_editor_dialog_response(connection_editor_dialog,
					GTK_RESPONSE_ACCEPT);
}

gboolean
ags_connection_editor_dialog_key_pressed_callback(GtkEventControllerKey *event_controller,
						  guint keyval,
						  guint keycode,
						  GdkModifierType state,
						  AgsConnectionEditorDialog *connection_editor_dialog)
{
  gboolean key_handled;

  key_handled = TRUE;

  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R){
    key_handled = FALSE;
  }
  
  return(key_handled);
}

void
ags_connection_editor_dialog_key_released_callback(GtkEventControllerKey *event_controller,
						   guint keyval,
						   guint keycode,
						   GdkModifierType state,
						   AgsConnectionEditorDialog *connection_editor_dialog)
{
  gboolean key_handled;

  key_handled = TRUE;

  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R){
    key_handled = FALSE;
  }else{
    switch(keyval){
    case GDK_KEY_Escape:
      {
	ags_connection_editor_dialog_response(connection_editor_dialog,
					      GTK_RESPONSE_CLOSE);	
      }
      break;
    }
  }
}

gboolean
ags_connection_editor_dialog_modifiers_callback(GtkEventControllerKey *event_controller,
						GdkModifierType keyval,
						AgsConnectionEditorDialog *connection_editor_dialog)
{
  return(FALSE);
}

/**
 * ags_connection_editor_dialog_response:
 * @connection_editor_dialog: the #AgsConnectionEditorDialog
 * @response: the response
 *
 * Response @connection_editor_dialog due to user action.
 * 
 * Since: 6.10.0
 */
void
ags_connection_editor_dialog_response(AgsConnectionEditorDialog *connection_editor_dialog,
				      gint response)
{
  g_return_if_fail(AGS_IS_CONNECTION_EDITOR_DIALOG(connection_editor_dialog));
  
  g_object_ref((GObject *) connection_editor_dialog);
  g_signal_emit(G_OBJECT(connection_editor_dialog),
		connection_editor_dialog_signals[RESPONSE], 0,
		response);
  g_object_unref((GObject *) connection_editor_dialog);
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
