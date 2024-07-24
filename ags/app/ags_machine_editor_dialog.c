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

#include <ags/app/ags_machine_editor_dialog.h>
#include <ags/app/ags_machine_editor_dialog_callbacks.h>

#include <ags/app/ags_ui_provider.h>

#include <ags/i18n.h>

void ags_machine_editor_dialog_class_init(AgsMachineEditorDialogClass *machine_editor_dialog);
void ags_machine_editor_dialog_init(AgsMachineEditorDialog *machine_editor_dialog);

void ags_machine_editor_dialog_activate_button_callback(GtkButton *activate_button,
							AgsMachineEditorDialog *machine_editor_dialog);

gboolean ags_machine_editor_dialog_key_pressed_callback(GtkEventControllerKey *event_controller,
							guint keyval,
							guint keycode,
							GdkModifierType state,
							AgsMachineEditorDialog *machine_editor_dialog);
void ags_machine_editor_dialog_key_released_callback(GtkEventControllerKey *event_controller,
						     guint keyval,
						     guint keycode,
						     GdkModifierType state,
						     AgsMachineEditorDialog *machine_editor_dialog);
gboolean ags_machine_editor_dialog_modifiers_callback(GtkEventControllerKey *event_controller,
						      GdkModifierType keyval,
						      AgsMachineEditorDialog *machine_editor_dialog);

/**
 * SECTION:ags_machine_editor_dialog
 * @short_description: edit machine settings
 * @title: AgsMachineEditorDialog
 * @section_id:
 * @include: ags/app/ags_machine_editor_dialog.h
 *
 * #AgsMachineEditorDialog is a dialog widget to edit machine settings.
 */

enum{
  RESPONSE,
  LAST_SIGNAL,
};

static gpointer ags_machine_editor_dialog_parent_class = NULL;

static guint machine_editor_dialog_signals[LAST_SIGNAL];

GType
ags_machine_editor_dialog_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_machine_editor_dialog = 0;

    static const GTypeInfo ags_machine_editor_dialog_info = {
      sizeof (AgsMachineEditorDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_machine_editor_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMachineEditorDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_machine_editor_dialog_init,
    };

    ags_type_machine_editor_dialog = g_type_register_static(GTK_TYPE_WINDOW,
							    "AgsMachineEditorDialog", &ags_machine_editor_dialog_info,
							    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_machine_editor_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_machine_editor_dialog_class_init(AgsMachineEditorDialogClass *machine_editor_dialog)
{
  ags_machine_editor_dialog_parent_class = g_type_class_peek_parent(machine_editor_dialog);

  /* AgsMachineEditorDialog */
  machine_editor_dialog->response = NULL;
  
  /* signals */
  /**
   * AgsMachineEditorDialog::response:
   * @machine_editor_dialog: the #AgsMachineEditorDialog
   *
   * The ::response signal notifies about window interaction.
   *
   * Since: 6.10.0
   */
  machine_editor_dialog_signals[RESPONSE] =
    g_signal_new("response",
		 G_TYPE_FROM_CLASS(machine_editor_dialog),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMachineEditorDialogClass, response),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__INT,
		 G_TYPE_NONE, 1,
		 G_TYPE_INT);
}

void
ags_machine_editor_dialog_init(AgsMachineEditorDialog *machine_editor_dialog)
{
  GtkBox *vbox;
  
  GtkEventController *event_controller;

  machine_editor_dialog->flags = 0;

  gtk_window_set_title(GTK_WINDOW(machine_editor_dialog),
		       i18n("machine editor"));

  gtk_window_set_deletable(GTK_WINDOW(machine_editor_dialog),
			   TRUE);

  gtk_window_set_default_size((GtkWindow *) machine_editor_dialog,
			      800, 600);

  g_signal_connect(machine_editor_dialog, "close-request",
		   G_CALLBACK(ags_machine_editor_dialog_close_request_callback), NULL);

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) machine_editor_dialog,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_machine_editor_dialog_key_pressed_callback), machine_editor_dialog);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_machine_editor_dialog_key_released_callback), machine_editor_dialog);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_machine_editor_dialog_modifiers_callback), machine_editor_dialog);
  
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_window_set_child((GtkWindow *) machine_editor_dialog,
		       (GtkWidget *) vbox);

  machine_editor_dialog->machine_editor = ags_machine_editor_new(NULL);

  /* set parent */
  machine_editor_dialog->machine_editor->parent_dialog = (GtkWidget *) machine_editor_dialog;
  
  gtk_widget_set_hexpand((GtkWidget *) machine_editor_dialog->machine_editor,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) machine_editor_dialog->machine_editor,
			 TRUE);

  gtk_widget_set_halign((GtkWidget *) machine_editor_dialog->machine_editor,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) machine_editor_dialog->machine_editor,
			GTK_ALIGN_FILL);
  
  gtk_box_append(vbox,
		 (GtkWidget *) machine_editor_dialog->machine_editor);
  
  /* buttons */
  machine_editor_dialog->action_area = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
							      AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) machine_editor_dialog->action_area,
			GTK_ALIGN_END);

  gtk_box_append(vbox,
		 (GtkWidget *) machine_editor_dialog->action_area);

  machine_editor_dialog->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("ok"));

  gtk_box_append(machine_editor_dialog->action_area,
		 (GtkWidget *) machine_editor_dialog->activate_button);

  g_signal_connect(machine_editor_dialog->activate_button, "clicked",
		   G_CALLBACK(ags_machine_editor_dialog_activate_button_callback), machine_editor_dialog);
}

void
ags_machine_editor_dialog_activate_button_callback(GtkButton *activate_button,
						   AgsMachineEditorDialog *machine_editor_dialog)
{
  ags_machine_editor_dialog_response(machine_editor_dialog,
				     GTK_RESPONSE_ACCEPT);
}

gboolean
ags_machine_editor_dialog_key_pressed_callback(GtkEventControllerKey *event_controller,
					       guint keyval,
					       guint keycode,
					       GdkModifierType state,
					       AgsMachineEditorDialog *machine_editor_dialog)
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
ags_machine_editor_dialog_key_released_callback(GtkEventControllerKey *event_controller,
						guint keyval,
						guint keycode,
						GdkModifierType state,
						AgsMachineEditorDialog *machine_editor_dialog)
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
	ags_machine_editor_dialog_response(machine_editor_dialog,
					   GTK_RESPONSE_CLOSE);	
      }
      break;
    }
  }
}

gboolean
ags_machine_editor_dialog_modifiers_callback(GtkEventControllerKey *event_controller,
					     GdkModifierType keyval,
					     AgsMachineEditorDialog *machine_editor_dialog)
{
  return(FALSE);
}

/**
 * ags_machine_editor_dialog_response:
 * @machine_editor_dialog: the #AgsMachineEditorDialog
 * @response: the response
 *
 * Response @machine_editor_dialog due to user action.
 * 
 * Since: 6.10.0
 */
void
ags_machine_editor_dialog_response(AgsMachineEditorDialog *machine_editor_dialog,
				   gint response)
{
  g_return_if_fail(AGS_IS_MACHINE_EDITOR_DIALOG(machine_editor_dialog));
  
  g_object_ref((GObject *) machine_editor_dialog);
  g_signal_emit(G_OBJECT(machine_editor_dialog),
		machine_editor_dialog_signals[RESPONSE], 0,
		response);
  g_object_unref((GObject *) machine_editor_dialog);
}

/**
 * ags_machine_editor_dialog_new:
 * @title: the title
 * @transient_for: the transient for
 *
 * Creates an #AgsMachineEditorDialog
 *
 * Returns: a new #AgsMachineEditorDialog
 *
 * Since: 4.0.0
 */
AgsMachineEditorDialog*
ags_machine_editor_dialog_new(gchar *title,
			      GtkWindow *transient_for)
{
  AgsMachineEditorDialog *machine_editor_dialog;

  machine_editor_dialog = (AgsMachineEditorDialog *) g_object_new(AGS_TYPE_MACHINE_EDITOR_DIALOG,
								  "title", title,
								  "transient-for", transient_for,
								  NULL);

  return(machine_editor_dialog);
}
