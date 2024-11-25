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

#include <ags/app/ags_quit_dialog.h>
#include <ags/app/ags_quit_dialog_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <ags/app/file/ags_simple_file.h>

#include <ags/app/machine/ags_audiorec.h>

#include <glib.h>
#include <glib/gstdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_quit_dialog_class_init(AgsQuitDialogClass *quit_dialog);
void ags_quit_dialog_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_quit_dialog_init(AgsQuitDialog *quit_dialog);
void ags_quit_dialog_finalize(GObject *gobject);

gboolean ags_quit_dialog_is_connected(AgsConnectable *connectable);
void ags_quit_dialog_connect(AgsConnectable *connectable);
void ags_quit_dialog_disconnect(AgsConnectable *connectable);

void ags_quit_dialog_close_request_callback(GtkWindow *window,
					    AgsQuitDialog *quit_dialog);

gboolean ags_quit_dialog_key_pressed_callback(GtkEventControllerKey *event_controller,
					      guint keyval,
					      guint keycode,
					      GdkModifierType state,
					      AgsQuitDialog *quit_dialog);
void ags_quit_dialog_key_released_callback(GtkEventControllerKey *event_controller,
					   guint keyval,
					   guint keycode,
					   GdkModifierType state,
					   AgsQuitDialog *quit_dialog);
gboolean ags_quit_dialog_modifiers_callback(GtkEventControllerKey *event_controller,
					    GdkModifierType keyval,
					    AgsQuitDialog *quit_dialog);

void ags_quit_dialog_real_response(AgsQuitDialog *quit_dialog,
				   gint response);

/**
 * SECTION:ags_quit_dialog
 * @short_description: The quit dialog
 * @title: AgsQuitDialog
 * @section_id:
 * @include: ags/app/ags_quit_dialog.h
 *
 * #AgsQuitDialog is shown as terminating GSequencer.
 */

static gpointer ags_quit_dialog_parent_class = NULL;

enum{
  RESPONSE,
  LAST_SIGNAL,
};

static guint quit_dialog_signals[LAST_SIGNAL];

GType
ags_quit_dialog_get_type()
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_quit_dialog = 0;

    static const GTypeInfo ags_quit_dialog_info = {
      sizeof (AgsQuitDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_quit_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsQuitDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_quit_dialog_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_quit_dialog_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_quit_dialog = g_type_register_static(GTK_TYPE_WINDOW,
						  "AgsQuitDialog", &ags_quit_dialog_info,
						  0);
    
    g_type_add_interface_static(ags_type_quit_dialog,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_quit_dialog);
  }

  return g_define_type_id__volatile;
}

void
ags_quit_dialog_class_init(AgsQuitDialogClass *quit_dialog)
{
  GObjectClass *gobject;

  ags_quit_dialog_parent_class = g_type_class_peek_parent(quit_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) quit_dialog;

  gobject->finalize = ags_quit_dialog_finalize;

  /* AgsQuitDialog */
  quit_dialog->response = ags_quit_dialog_real_response;
  
  /* signals */
  /**
   * AgsQuitDialog::response:
   * @quit_dialog: the #AgsQuitDialog
   *
   * The ::response signal notifies about window interaction.
   *
   * Since: 6.16.18
   */
  quit_dialog_signals[RESPONSE] =
    g_signal_new("response",
		 G_TYPE_FROM_CLASS(quit_dialog),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsQuitDialogClass, response),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__INT,
		 G_TYPE_NONE, 1,
		 G_TYPE_INT);
}

void
ags_quit_dialog_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;

  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_quit_dialog_is_connected;  
  connectable->connect = ags_quit_dialog_connect;
  connectable->disconnect = ags_quit_dialog_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_quit_dialog_init(AgsQuitDialog *quit_dialog)
{
  GtkBox *vbox;  
  GtkBox *inner_vbox;  
  GtkBox *hbox;
  
  GtkEventController *event_controller;

  quit_dialog->connectable_flags = 0;
  
  g_object_set(quit_dialog,
	       "title", i18n("Exit GSequencer"),
	       NULL);

  gtk_window_set_default_size((GtkWindow *) quit_dialog,
			      AGS_UI_PROVIDER_DEFAULT_QUIT_DIALOG_WIDTH, AGS_UI_PROVIDER_DEFAULT_QUIT_DIALOG_HEIGHT);

  g_signal_connect(quit_dialog, "close-request",
		   G_CALLBACK(ags_quit_dialog_close_request_callback), quit_dialog);

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) quit_dialog,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_quit_dialog_key_pressed_callback), quit_dialog);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_quit_dialog_key_released_callback), quit_dialog);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_quit_dialog_modifiers_callback), quit_dialog);

  /* vbox */
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_valign((GtkWidget *) vbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_vexpand((GtkWidget *) vbox,
			 TRUE);
  
  gtk_box_set_spacing((GtkBox *) vbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_window_set_child((GtkWindow *) quit_dialog,
		       (GtkWidget *) vbox);

  /* content */
  quit_dialog->current_question = AGS_QUIT_DIALOG_QUESTION_SAVE_FILE;

  quit_dialog->scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();

  gtk_widget_set_valign((GtkWidget *) quit_dialog->scrolled_window,
			GTK_ALIGN_FILL);
  gtk_widget_set_vexpand((GtkWidget *) quit_dialog->scrolled_window,
			 TRUE);

  gtk_box_append(vbox,
		 (GtkWidget *) quit_dialog->scrolled_window);

  /* inner vbox */
  inner_vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_valign((GtkWidget *) inner_vbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_vexpand((GtkWidget *) inner_vbox,
			 TRUE);
  
  gtk_box_set_spacing((GtkBox *) inner_vbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_scrolled_window_set_child(quit_dialog->scrolled_window,
				(GtkWidget *) inner_vbox);

  /* question */
  quit_dialog->save_file_question = (GtkLabel *) gtk_label_new(i18n("Do you want to save before quit?"));
  gtk_label_set_xalign(quit_dialog->save_file_question,
		       0.0);
  gtk_box_append(inner_vbox,
		 (GtkWidget *) quit_dialog->save_file_question);

  quit_dialog->export_wave_question = (GtkLabel *) gtk_label_new(i18n("Do you want to fast export before quit?"));
  gtk_label_set_xalign(quit_dialog->export_wave_question,
		       0.0);
  gtk_box_append(inner_vbox,
		 (GtkWidget *) quit_dialog->export_wave_question);

  gtk_widget_set_visible((GtkWidget *) quit_dialog->export_wave_question,
			 FALSE);

  /* filename hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_valign((GtkWidget *) hbox,
			GTK_ALIGN_START);
  gtk_widget_set_vexpand((GtkWidget *) hbox,
			 FALSE);
  
  gtk_box_set_spacing((GtkBox *) hbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(inner_vbox,
		 (GtkWidget *) hbox);
  
  quit_dialog->export_wave = (GtkLabel *) gtk_label_new(i18n("Current filename of export wave:"));
  gtk_box_append(hbox,
		 (GtkWidget *) quit_dialog->export_wave);

  gtk_widget_set_visible((GtkWidget *) quit_dialog->export_wave,
			 FALSE);

  quit_dialog->export_wave_filename = (GtkLabel *) gtk_label_new("(null)");
  gtk_box_append(hbox,
		 (GtkWidget *) quit_dialog->export_wave_filename);

  gtk_widget_set_visible((GtkWidget *) quit_dialog->export_wave_filename,
			 FALSE);

  quit_dialog->accept_all = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("accept all"));
  gtk_box_append(inner_vbox,
		 (GtkWidget *) quit_dialog->accept_all);

  quit_dialog->nth_wave_export_machine = 0;

  quit_dialog->wave_export_machine = NULL;

  /* button hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_valign((GtkWidget *) hbox,
			GTK_ALIGN_END);
  gtk_widget_set_halign((GtkWidget *) hbox,
			GTK_ALIGN_END);

  gtk_widget_set_vexpand((GtkWidget *) hbox,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) hbox,
			 TRUE);
  
  gtk_box_set_spacing((GtkBox *) hbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(vbox,
		 (GtkWidget *) hbox);
  
  quit_dialog->accept = (GtkButton *) gtk_button_new_with_label(i18n("yes"));
  gtk_box_append(hbox,
		 (GtkWidget *) quit_dialog->accept);

  quit_dialog->reject = (GtkButton *) gtk_button_new_with_label(i18n("no"));
  gtk_box_append(hbox,
		 (GtkWidget *) quit_dialog->reject);

  quit_dialog->cancel = (GtkButton *) gtk_button_new_with_label(i18n("cancel"));
  gtk_box_append(hbox,
		 (GtkWidget *) quit_dialog->cancel);
}

void
ags_quit_dialog_finalize(GObject *gobject)
{
  AgsQuitDialog *quit_dialog;

  quit_dialog = (AgsQuitDialog *) gobject;
  
  G_OBJECT_CLASS(ags_quit_dialog_parent_class)->finalize(gobject);
}

gboolean
ags_quit_dialog_is_connected(AgsConnectable *connectable)
{
  AgsQuitDialog *quit_dialog;
  
  gboolean is_connected;
  
  quit_dialog = AGS_QUIT_DIALOG(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (quit_dialog->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_quit_dialog_connect(AgsConnectable *connectable)
{
  AgsQuitDialog *quit_dialog;

  quit_dialog = AGS_QUIT_DIALOG(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  quit_dialog->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  g_signal_connect(quit_dialog->accept, "clicked",
		   G_CALLBACK(ags_quit_dialog_accept_callback), quit_dialog);

  g_signal_connect(quit_dialog->reject, "clicked",
		   G_CALLBACK(ags_quit_dialog_reject_callback), quit_dialog);

  g_signal_connect(quit_dialog->cancel, "clicked",
		   G_CALLBACK(ags_quit_dialog_cancel_callback), quit_dialog);
}

void
ags_quit_dialog_disconnect(AgsConnectable *connectable)
{
  AgsQuitDialog *quit_dialog;

  quit_dialog = AGS_QUIT_DIALOG(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  quit_dialog->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  g_object_disconnect(quit_dialog->accept,
		      "any_signal::clicked",
		      G_CALLBACK(ags_quit_dialog_accept_callback),
		      quit_dialog,
		      NULL);

  g_object_disconnect(quit_dialog->reject,
		      "any_signal::clicked",
		      G_CALLBACK(ags_quit_dialog_reject_callback),
		      quit_dialog,
		      NULL);

  g_object_disconnect(quit_dialog->cancel,
		      "any_signal::clicked",
		      G_CALLBACK(ags_quit_dialog_cancel_callback),
		      quit_dialog,
		      NULL);
}

void
ags_quit_dialog_close_request_callback(GtkWindow *window,
				       AgsQuitDialog *quit_dialog)
{
  ags_quit_dialog_response(quit_dialog,
			   GTK_RESPONSE_CANCEL);
}

gboolean
ags_quit_dialog_key_pressed_callback(GtkEventControllerKey *event_controller,
				     guint keyval,
				     guint keycode,
				     GdkModifierType state,
				     AgsQuitDialog *quit_dialog)
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
ags_quit_dialog_key_released_callback(GtkEventControllerKey *event_controller,
				      guint keyval,
				      guint keycode,
				      GdkModifierType state,
				      AgsQuitDialog *quit_dialog)
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
	ags_quit_dialog_response(quit_dialog,
				 GTK_RESPONSE_CANCEL);	
      }
      break;
    }
  }
}

gboolean
ags_quit_dialog_modifiers_callback(GtkEventControllerKey *event_controller,
				   GdkModifierType keyval,
				   AgsQuitDialog *quit_dialog)
{
  return(FALSE);
}

/**
 * ags_quit_dialog_save_file:
 * @quit_dialog: the #AgsQuitDialog
 * 
 * Save file @quit_dialog.
 * 
 * Since: 3.16.18
 */
void
ags_quit_dialog_save_file(AgsQuitDialog *quit_dialog)
{
  AgsWindow *window;
    
  AgsApplicationContext *application_context;

  AgsSimpleFile *simple_file;
  
  GList *start_list, *list;
    
  GError *error;

  application_context = ags_application_context_get_instance();
  
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  simple_file = (AgsSimpleFile *) g_object_new(AGS_TYPE_SIMPLE_FILE,
					       "filename", window->loaded_filename,
					       NULL);
      
  error = NULL;
  ags_simple_file_rw_open(simple_file,
			  TRUE,
			  &error);
  
  if(error != NULL){
    g_message("%s", error->message);

    g_error_free(error);
  }
    
  ags_simple_file_write(simple_file);
  ags_simple_file_close(simple_file);
  
  g_object_unref(G_OBJECT(simple_file));
}

/**
 * ags_quit_dialog_fast_export:
 * @quit_dialog: the #AgsQuitDialog
 * @machine: the #AgsMachine
 * 
 * Fast export @quit_dialog.
 * 
 * Since: 3.5.0
 */
void
ags_quit_dialog_fast_export(AgsQuitDialog *quit_dialog,
			    AgsMachine *machine)
{
  GList *start_wave, *end_wave;
    
  guint64 start_frame, end_frame;
  guint buffer_size;
  
  if(!AGS_IS_QUIT_DIALOG(quit_dialog) ||
     !AGS_IS_MACHINE(machine)){
    return;
  }

  /* get some fields */
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  start_wave = NULL;
  
  g_object_get(machine->audio,
	       "wave", &start_wave,
	       "buffer-size", &buffer_size,	       
	       NULL);

  start_frame = 0;
  end_frame = 0;
  
  end_wave = g_list_last(start_wave);  

  if(end_wave != NULL){
    GList *end_buffer;

    guint64 x;
    
    end_buffer = g_list_last(AGS_WAVE(end_wave->data)->buffer);

    x = 0;
    
    if(end_buffer != NULL){
      g_object_get(AGS_BUFFER(end_buffer->data),
		   "x", &x,
		   NULL);

      end_frame = x + buffer_size;
    }else{
      x = ags_timestamp_get_ags_offset(AGS_WAVE(end_wave->data)->timestamp);

      end_frame = x + buffer_size;
    }
  }

  if(AGS_IS_AUDIOREC(machine)){
    gchar *filename;

    filename = g_strdup(gtk_editable_get_text(GTK_EDITABLE(AGS_AUDIOREC(machine)->filename)));
  
    ags_audiorec_fast_export((AgsAudiorec *) machine,
			     filename,
			     start_frame, end_frame);
  }
}

void
ags_quit_dialog_real_response(AgsQuitDialog *quit_dialog,
			      gint response)
{
  switch(response){
  case GTK_RESPONSE_OK:
  case GTK_RESPONSE_CANCEL:
    {
      gtk_widget_set_visible((GtkWidget *) quit_dialog,
			     FALSE);
    }
  }
}

/**
 * ags_quit_dialog_response:
 * @quit_dialog: the #AgsQuitDialog
 * @response: the response
 *
 * Response @quit_dialog due to user action.
 * 
 * Since: 6.16.18
 */
void
ags_quit_dialog_response(AgsQuitDialog *quit_dialog,
			 gint response)
{
  g_return_if_fail(AGS_IS_QUIT_DIALOG(quit_dialog));
  
  g_object_ref((GObject *) quit_dialog);
  g_signal_emit(G_OBJECT(quit_dialog),
		quit_dialog_signals[RESPONSE], 0,
		response);
  g_object_unref((GObject *) quit_dialog);
}

/**
 * ags_quit_dialog_new:
 * @transient_for: the #GtkWindow
 * 
 * Create a new instance of #AgsQuitDialog
 * 
 * Returns: the #AgsQuitDialog
 * 
 * Since: 3.5.0
 */
AgsQuitDialog*
ags_quit_dialog_new(GtkWindow *transient_for)
{
  AgsQuitDialog *quit_dialog;

  quit_dialog = (AgsQuitDialog *) g_object_new(AGS_TYPE_QUIT_DIALOG,
					       "transient-for", transient_for,
					       NULL);

  return(quit_dialog);
}
