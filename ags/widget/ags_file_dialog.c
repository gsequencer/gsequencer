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

#include "ags_file_dialog.h"

#include <glib/gstdio.h>

#include <gdk/gdkkeysyms.h>

#include <ags/i18n.h>

void ags_file_dialog_class_init(AgsFileDialogClass *file_dialog);
void ags_file_dialog_orientable_interface_init(GtkOrientableIface *orientable);
void ags_file_dialog_init(AgsFileDialog *file_dialog);
void ags_file_dialog_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_file_dialog_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_file_dialog_dispose(GObject *gobject);
void ags_file_dialog_finalize(GObject *gobject);

void ags_file_dialog_show(GtkWidget *widget);

void ags_file_dialog_close_request_callback(GtkWindow *window,
					    AgsFileDialog *file_dialog);

void ags_file_dialog_activate_button_callback(GtkButton *activate_button,
					      AgsFileDialog *file_dialog);

gboolean ags_file_dialog_key_pressed_callback(GtkEventControllerKey *event_controller,
					      guint keyval,
					      guint keycode,
					      GdkModifierType state,
					      AgsFileDialog *file_dialog);
void ags_file_dialog_key_released_callback(GtkEventControllerKey *event_controller,
					   guint keyval,
					   guint keycode,
					   GdkModifierType state,
					   AgsFileDialog *file_dialog);
gboolean ags_file_dialog_modifiers_callback(GtkEventControllerKey *event_controller,
					    GdkModifierType keyval,
					    AgsFileDialog *file_dialog);

void ags_file_dialog_real_response(AgsFileDialog *file_dialog,
				   gint response_id);

/**
 * SECTION:ags_file_dialog
 * @short_description: a file dialog widget
 * @title: AgsFileDialog
 * @section_id:
 * @include: ags/widget/ags_file_dialog.h
 *
 * #AgsFileDialog is a dialog widget to open or save files.
 */

enum{
  RESPONSE,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_FILE_WIDGET,
};

static gpointer ags_file_dialog_parent_class = NULL;
static guint file_dialog_signals[LAST_SIGNAL];

GType
ags_file_dialog_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_file_dialog = 0;

    static const GTypeInfo ags_file_dialog_info = {
      sizeof(AgsFileDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_file_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFileDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_file_dialog_init,
    };

    ags_type_file_dialog = g_type_register_static(GTK_TYPE_WINDOW,
						  "AgsFileDialog", &ags_file_dialog_info,
						  0);    

    g_once_init_leave(&g_define_type_id__static, ags_type_file_dialog);
  }

  return(g_define_type_id__static);
}

void
ags_file_dialog_class_init(AgsFileDialogClass *file_dialog)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_file_dialog_parent_class = g_type_class_peek_parent(file_dialog);

  /* GObjectClass */
  gobject = (GObjectClass *) file_dialog;

  gobject->set_property = ags_file_dialog_set_property;
  gobject->get_property = ags_file_dialog_get_property;

  gobject->dispose = ags_file_dialog_dispose;
  gobject->finalize = ags_file_dialog_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) file_dialog;

  widget->show = ags_file_dialog_show;

  /* properties */
  /**
   * AgsFileDialog:file-widget:
   *
   * The file widget.
   * 
   * Since: 6.6.0
   */
  param_spec = g_param_spec_pointer("file-widget",
				    "file widget",
				    "The file widget",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE_WIDGET,
				  param_spec);

  /* AgsFileDialog */
  file_dialog->response = ags_file_dialog_real_response;
  
  /* signals */
  /**
   * AgsFileDialog::response:
   * @file_dialog: the #AgsFileDialog
   *
   * The ::response signal notifies about window interaction.
   *
   * Since: 6.6.0
   */
  file_dialog_signals[RESPONSE] =
    g_signal_new("response",
		 G_TYPE_FROM_CLASS(file_dialog),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFileDialogClass, response),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__INT,
		 G_TYPE_NONE, 1,
		 G_TYPE_INT);
}

void
ags_file_dialog_init(AgsFileDialog *file_dialog)
{  
  GtkEventController *event_controller;
  GtkGrid *grid;
  
  file_dialog->flags = 0;
  
  gtk_window_set_default_size((GtkWindow *) file_dialog,
			      800, 600);

  gtk_window_set_hide_on_close((GtkWindow *) file_dialog,
			       TRUE);

  g_signal_connect(file_dialog, "close-request",
		   G_CALLBACK(ags_file_dialog_close_request_callback), file_dialog);

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) file_dialog,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_file_dialog_key_pressed_callback), file_dialog);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_file_dialog_key_released_callback), file_dialog);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_file_dialog_modifiers_callback), file_dialog);

  file_dialog->vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
					     6);

  gtk_widget_set_valign((GtkWidget *) file_dialog->vbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) file_dialog->vbox,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) file_dialog->vbox,
			 TRUE);  
  gtk_widget_set_hexpand((GtkWidget *) file_dialog->vbox,
			 TRUE);  

  gtk_window_set_child((GtkWindow *) file_dialog,
		       (GtkWidget *) file_dialog->vbox);  

  file_dialog->file_widget = ags_file_widget_new();  

  gtk_widget_set_valign((GtkWidget *) file_dialog->file_widget,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) file_dialog->file_widget,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) file_dialog->file_widget,
			 TRUE);  
  gtk_widget_set_hexpand((GtkWidget *) file_dialog->file_widget,
			 TRUE);  
  
  gtk_box_append(file_dialog->vbox,
		 (GtkWidget *) file_dialog->file_widget);

  /* grid */
  grid = (GtkGrid *) gtk_grid_new();

  gtk_widget_set_vexpand((GtkWidget *) grid,
			 FALSE);
  gtk_widget_set_hexpand((GtkWidget *) grid,
			 FALSE);

  gtk_grid_set_column_spacing(grid,
			      6);
  gtk_grid_set_row_spacing(grid,
			   6);

  gtk_box_append(file_dialog->vbox,
		 (GtkWidget *) grid);

  /* download link */
  file_dialog->download_link = (GtkLinkButton *) gtk_link_button_new_with_label("https://gsequencer.com",
										i18n("download"));

  gtk_grid_attach(grid,
		  (GtkWidget *) file_dialog->download_link,
		  0, 0,
		  1, 1);
  
  /* button */
  file_dialog->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("open"));
  gtk_widget_set_halign((GtkWidget *) file_dialog->activate_button,
			GTK_ALIGN_END);
  gtk_box_append(file_dialog->vbox,
		 (GtkWidget *) file_dialog->activate_button);

  g_signal_connect(file_dialog->activate_button, "clicked",
		   G_CALLBACK(ags_file_dialog_activate_button_callback), file_dialog);
}

void
ags_file_dialog_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsFileDialog *file_dialog;

  file_dialog = AGS_FILE_DIALOG(gobject);

  switch(prop_id){
  case PROP_FILE_WIDGET:
    {
      AgsFileWidget *file_widget;

      file_widget = g_value_get_pointer(value);

      if(file_dialog->file_widget == file_widget){
	return;
      }

      file_dialog->file_widget = file_widget;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_file_dialog_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsFileDialog *file_dialog;

  file_dialog = AGS_FILE_DIALOG(gobject);

  switch(prop_id){
  case PROP_FILE_WIDGET:
  {
    g_value_set_pointer(value, file_dialog->file_widget);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_file_dialog_dispose(GObject *gobject)
{  
  /* call parent */
  G_OBJECT_CLASS(ags_file_dialog_parent_class)->dispose(gobject);
}

void
ags_file_dialog_finalize(GObject *gobject)
{  
  /* call parent */
  G_OBJECT_CLASS(ags_file_dialog_parent_class)->finalize(gobject);
}

void
ags_file_dialog_close_request_callback(GtkWindow *window,
				       AgsFileDialog *file_dialog)
{
  ags_file_dialog_response(file_dialog,
			   GTK_RESPONSE_CANCEL);
}

void
ags_file_dialog_activate_button_callback(GtkButton *activate_button,
					 AgsFileDialog *file_dialog)
{
  gchar *filename;
  gchar *basename;
  gchar *dirname;

  filename = ags_file_widget_get_filename(file_dialog->file_widget);

  dirname = g_path_get_dirname(filename);
  basename = g_path_get_basename(filename);

  if(file_dialog->file_widget->file_action == AGS_FILE_WIDGET_SAVE_AS){  
    gboolean writable_location;
    
    writable_location = (g_access(dirname, W_OK) == 0) ? TRUE: FALSE;
    
    if(writable_location &&
       basename != NULL &&
       strlen(basename) > 0 &&
       (!g_strncasecmp(basename, "/", 2)) == FALSE &&
       (!g_strncasecmp(basename, ".", 2)) == FALSE &&
       (!g_strncasecmp(basename, "..", 3)) == FALSE &&
       !g_file_test(filename, G_FILE_TEST_IS_DIR)){
      ags_file_dialog_response(file_dialog,
			       GTK_RESPONSE_ACCEPT);
    }    
  }else{
    GSList *start_filenames, *filenames;

    gboolean readable_location;
    gboolean success;
    
    filenames =
      start_filenames = ags_file_widget_get_filenames(file_dialog->file_widget);
    
    readable_location = (g_access(dirname, R_OK) == 0) ? TRUE: FALSE;
    
    if(readable_location &&
       basename != NULL &&
       strlen(basename) > 0){
      success = FALSE;
      
      if(!g_file_test(filename, G_FILE_TEST_IS_DIR)){
	success = TRUE;
	
	ags_file_dialog_response(file_dialog,
				 GTK_RESPONSE_ACCEPT);
      }

      if(!success){
	while(!success &&
	      filenames != NULL){
	  if(!g_file_test(filenames->data, G_FILE_TEST_IS_DIR)){
	    success = TRUE;
	
	    ags_file_dialog_response(file_dialog,
				     GTK_RESPONSE_ACCEPT);
	  }

	  filenames = filenames->next;
	}
      }
    }

    g_slist_free_full(start_filenames,
		      g_free);
  }

  g_free(filename);
  g_free(dirname);
  g_free(basename);
}

gboolean
ags_file_dialog_key_pressed_callback(GtkEventControllerKey *event_controller,
				     guint keyval,
				     guint keycode,
				     GdkModifierType state,
				     AgsFileDialog *file_dialog)
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
ags_file_dialog_key_released_callback(GtkEventControllerKey *event_controller,
				      guint keyval,
				      guint keycode,
				      GdkModifierType state,
				      AgsFileDialog *file_dialog)
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
	ags_file_dialog_response(file_dialog,
				 GTK_RESPONSE_CANCEL);	
      }
      break;
    }
  }
}

gboolean
ags_file_dialog_modifiers_callback(GtkEventControllerKey *event_controller,
				   GdkModifierType keyval,
				   AgsFileDialog *file_dialog)
{
  return(FALSE);
}

void
ags_file_dialog_show(GtkWidget *widget)
{
  AgsFileDialog *file_dialog;

  file_dialog = AGS_FILE_DIALOG(widget);

  /* hide unneeded */
  if((AGS_FILE_DIALOG_SHOW_DOWNLOAD_LINK & (file_dialog->flags)) == 0){
    gtk_widget_set_visible((GtkWidget *) file_dialog->download_link,
			   FALSE);
  }
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_file_dialog_parent_class)->show(widget);
}

/**
 * ags_file_dialog_test_flags:
 * @file_dialog: the #AgsFileDialog
 * @flags: the flags
 *
 * Test @flags of @file_dialog.
 * 
 * Returns: %TRUE if flags set, otherwise %FALSE
 * 
 * Since: 7.2.8
 */
gboolean
ags_file_dialog_test_flags(AgsFileDialog *file_dialog,
			   guint flags)
{
  gboolean success;

  if(!AGS_IS_FILE_DIALOG(file_dialog)){
    return(FALSE);
  }
  
  success = ((flags & (file_dialog->flags)) != 0) ? TRUE: FALSE;
  
  return(success);
}

/**
 * ags_file_dialog_set_flags:
 * @file_dialog: the #AgsFileDialog
 * @flags: the flags
 *
 * Set @flags of @file_dialog.
 * 
 * Since: 7.2.8
 */
void
ags_file_dialog_set_flags(AgsFileDialog *file_dialog,
			  guint flags)
{
  if(!AGS_IS_FILE_DIALOG(file_dialog)){
    return;
  }
  
  file_dialog->flags |= flags;
}

/**
 * ags_file_dialog_unset_flags:
 * @file_dialog: the #AgsFileDialog
 * @flags: the flags
 *
 * Set @flags of @file_dialog.
 * 
 * Since: 7.2.8
 */
void
ags_file_dialog_unset_flags(AgsFileDialog *file_dialog,
			    guint flags)
{
  if(!AGS_IS_FILE_DIALOG(file_dialog)){
    return;
  }

  file_dialog->flags &= (~flags);
}

/**
 * ags_file_dialog_get_file_widget:
 * @file_dialog: the #AgsFileDialog
 *
 * Get widget of @file_dialog.
 *
 * Returns: (transfer none): the #AgsFileWidget
 * 
 * Since: 6.6.0
 */
AgsFileWidget*
ags_file_dialog_get_file_widget(AgsFileDialog *file_dialog)
{
  g_return_val_if_fail(AGS_IS_FILE_DIALOG(file_dialog), NULL);

  return(file_dialog->file_widget);
}

/**
 * ags_file_dialog_get_download_link:
 * @file_dialog: the #AgsFileDialog
 *
 * Get download link of @file_dialog.
 * 
 * Returns: (transfer none): the #GtkLinkButton or %NULL
 * 
 * Since: 7.2.8
 */
GtkLinkButton*
ags_file_dialog_get_download_link(AgsFileDialog *file_dialog)
{
  g_return_val_if_fail(AGS_IS_FILE_DIALOG(file_dialog), NULL);

  return(file_dialog->download_link);
}

void
ags_file_dialog_real_response(AgsFileDialog *file_dialog,
			      gint response_id)
{  
  gtk_widget_set_visible((GtkWidget *) file_dialog,
			 FALSE);
}

/**
 * ags_file_dialog_response:
 * @file_dialog: the #AgsFileDialog
 * @response_id: the response id
 *
 * Response @file_dialog due to user action.
 * 
 * Since: 6.6.0
 */
void
ags_file_dialog_response(AgsFileDialog *file_dialog,
			 gint response_id)
{
  g_return_if_fail(AGS_IS_FILE_DIALOG(file_dialog));
  
  g_object_ref((GObject *) file_dialog);
  g_signal_emit(G_OBJECT(file_dialog),
		file_dialog_signals[RESPONSE], 0,
		response_id);
  g_object_unref((GObject *) file_dialog);
}

/**
 * ags_file_dialog_new:
 * @transient_for: the #GtkWidget transient-for
 * @title: the window title
 *
 * Creates a new instance of #AgsFileDialog.
 *
 * Returns: the new #AgsFileDialog
 *
 * Since: 6.6.0
 */
AgsFileDialog*
ags_file_dialog_new(GtkWidget *transient_for,
		    gchar *title)
{
  AgsFileDialog *file_dialog;

  file_dialog = (AgsFileDialog *) g_object_new(AGS_TYPE_FILE_DIALOG,
					       "transient-for", transient_for,
					       "title", title,
					       NULL);
  
  return(file_dialog);
}
