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

#include <ags/app/ags_pcm_file_dialog.h>

#include <ags/app/ags_ui_provider.h>

#include <glib/gstdio.h>

#include <ags/i18n.h>

void ags_pcm_file_dialog_class_init(AgsPCMFileDialogClass *pcm_file_dialog);
void ags_pcm_file_dialog_init(AgsPCMFileDialog *pcm_file_dialog);

void ags_pcm_file_dialog_show(GtkWidget *widget);

void ags_pcm_file_dialog_close_request_callback(GtkWindow *window,
						AgsPCMFileDialog *pcm_file_dialog);

void ags_pcm_file_dialog_activate_button_callback(GtkButton *activate_button,
						  AgsPCMFileDialog *pcm_file_dialog);

gboolean ags_pcm_file_dialog_key_pressed_callback(GtkEventControllerKey *event_controller,
						  guint keyval,
						  guint keycode,
						  GdkModifierType state,
						  AgsPCMFileDialog *pcm_file_dialog);
void ags_pcm_file_dialog_key_released_callback(GtkEventControllerKey *event_controller,
					       guint keyval,
					       guint keycode,
					       GdkModifierType state,
					       AgsPCMFileDialog *pcm_file_dialog);
gboolean ags_pcm_file_dialog_modifiers_callback(GtkEventControllerKey *event_controller,
						GdkModifierType keyval,
						AgsPCMFileDialog *pcm_file_dialog);

void ags_pcm_file_dialog_real_response(AgsPCMFileDialog *pcm_file_dialog,
				       gint response_id);

/**
 * SECTION:ags_pcm_file_dialog
 * @short_description: The PCM file chooser dialog
 * @title: AgsPCMFileDialog
 * @section_id:
 * @include: ags/app/ags_pcm_file_dialog.h
 *
 * #AgsPCMFileDialog lets you open PCM audio files and allows you
 * to select audio channel.
 */

enum{
  RESPONSE,
  LAST_SIGNAL,
};

static gpointer ags_pcm_file_dialog_parent_class = NULL;

static guint pcm_file_dialog_signals[LAST_SIGNAL];

GType
ags_pcm_file_dialog_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_pcm_file_dialog = 0;

    static const GTypeInfo ags_pcm_file_dialog_info = {
      sizeof (AgsPCMFileDialogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pcm_file_dialog_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPCMFileDialog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pcm_file_dialog_init,
    };

    ags_type_pcm_file_dialog = g_type_register_static(GTK_TYPE_WINDOW,
						      "AgsPCMFileDialog", &ags_pcm_file_dialog_info,
						      0);
    
    g_once_init_leave(&g_define_type_id__static, ags_type_pcm_file_dialog);
  }

  return(g_define_type_id__static);
}

void
ags_pcm_file_dialog_class_init(AgsPCMFileDialogClass *pcm_file_dialog)
{
  GtkWidgetClass *widget;
  
  ags_pcm_file_dialog_parent_class = g_type_class_peek_parent(pcm_file_dialog);

  widget = (GtkWidgetClass *) pcm_file_dialog;

  widget->show = ags_pcm_file_dialog_show;

  /* AgsFileDialog */
  pcm_file_dialog->response = ags_pcm_file_dialog_real_response;
  
  /* signals */
  /**
   * AgsPCMFileDialog::response:
   * @pcm_file_dialog: the #AgsPCMFileDialog
   * @response_id: the response ID
   *
   * The ::response signal notifies about window interaction.
   *
   * Since: 6.6.0
   */
  pcm_file_dialog_signals[RESPONSE] =
    g_signal_new("response",
		 G_TYPE_FROM_CLASS(pcm_file_dialog),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPCMFileDialogClass, response),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__INT,
		 G_TYPE_NONE, 1,
		 G_TYPE_INT);
}

void
ags_pcm_file_dialog_init(AgsPCMFileDialog *pcm_file_dialog)
{
  GtkBox *content_area;
  GtkBox *vbox;
  GtkGrid *grid;

  GtkEventController *event_controller;

  gchar *str;

  gtk_window_set_default_size((GtkWindow *) pcm_file_dialog,
			      800, 600);

  gtk_window_set_hide_on_close((GtkWindow *)pcm_file_dialog,
			       TRUE);

  g_signal_connect(pcm_file_dialog, "close-request",
		   G_CALLBACK(ags_pcm_file_dialog_close_request_callback), pcm_file_dialog);

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) pcm_file_dialog,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_pcm_file_dialog_key_pressed_callback), pcm_file_dialog);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_pcm_file_dialog_key_released_callback), pcm_file_dialog);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_pcm_file_dialog_modifiers_callback), pcm_file_dialog);

  /* file widget */  
  pcm_file_dialog->flags = AGS_PCM_FILE_DIALOG_SHOW_AUDIO_CHANNEL;
  
  content_area = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
					AGS_UI_PROVIDER_DEFAULT_PADDING);
  
  gtk_widget_set_vexpand((GtkWidget *) content_area,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) content_area,
			 TRUE);
  
  gtk_window_set_child((GtkWindow *) pcm_file_dialog,
		       (GtkWidget *) content_area);  

  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_PADDING);
  
  gtk_widget_set_vexpand((GtkWidget *) vbox,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) vbox,
			 TRUE);

  gtk_widget_set_valign((GtkWidget *) vbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) vbox,
			GTK_ALIGN_FILL);

  gtk_box_append(content_area,
		 (GtkWidget *) vbox);

  pcm_file_dialog->file_widget = ags_file_widget_new();

  gtk_widget_set_vexpand((GtkWidget *) pcm_file_dialog->file_widget,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) pcm_file_dialog->file_widget,
			 TRUE);

  gtk_widget_set_valign((GtkWidget *) pcm_file_dialog->file_widget,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) pcm_file_dialog->file_widget,
			GTK_ALIGN_FILL);
  
  gtk_box_append(vbox,
		 (GtkWidget *) pcm_file_dialog->file_widget);

  /* grid */
  grid = (GtkGrid *) gtk_grid_new();

  gtk_widget_set_vexpand((GtkWidget *) grid,
			 FALSE);
  gtk_widget_set_hexpand((GtkWidget *) grid,
			 FALSE);

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_box_append(vbox,
		 (GtkWidget *) grid);

  /* audio channel */
  str = g_strdup_printf("%s: ",
			i18n("audio channel"));
  
  pcm_file_dialog->audio_channel_label = (GtkLabel *) gtk_label_new(str);

  gtk_widget_set_halign((GtkWidget *) pcm_file_dialog->audio_channel_label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) pcm_file_dialog->audio_channel_label,
		  0, 0,
		  1, 1);

  g_free(str);
  
  pcm_file_dialog->audio_channel = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										    256.0,
										    1.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) pcm_file_dialog->audio_channel,
		  1, 0,
		  1, 1);

  /* new channel */
  pcm_file_dialog->new_channel = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("open in new channel"));

  gtk_check_button_set_active(pcm_file_dialog->new_channel,
			      TRUE);

  gtk_grid_attach(grid,
		  (GtkWidget *) pcm_file_dialog->new_channel,
		  0, 1,
		  1, 1);

  /* existing channel */
  pcm_file_dialog->existing_channel = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("override existing links"));

  gtk_check_button_set_active(pcm_file_dialog->existing_channel,
			      TRUE);

  gtk_grid_attach(grid,
		  (GtkWidget *) pcm_file_dialog->existing_channel,
		  0, 2,
		  1, 1);

  /* download link */
  pcm_file_dialog->download_link = (GtkLinkButton *) gtk_link_button_new_with_label("https://gsequencer.com",
										    i18n("download"));

  gtk_grid_attach(grid,
		  (GtkWidget *) pcm_file_dialog->download_link,
		  0, 3,
		  1, 1);
  
  /* button */
  pcm_file_dialog->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("open"));
  gtk_widget_set_halign((GtkWidget *) pcm_file_dialog->activate_button,
			GTK_ALIGN_END);
  gtk_box_append(content_area,
		 (GtkWidget *) pcm_file_dialog->activate_button);

  g_signal_connect(pcm_file_dialog->activate_button, "clicked",
		   G_CALLBACK(ags_pcm_file_dialog_activate_button_callback), pcm_file_dialog);
}

void
ags_pcm_file_dialog_show(GtkWidget *widget)
{
  AgsPCMFileDialog *pcm_file_dialog;

  pcm_file_dialog = AGS_PCM_FILE_DIALOG(widget);

  /* hide unneeded */
  if((AGS_PCM_FILE_DIALOG_SHOW_AUDIO_CHANNEL & (pcm_file_dialog->flags)) == 0){
    gtk_widget_set_visible((GtkWidget *) pcm_file_dialog->audio_channel_label,
			   FALSE);
    gtk_widget_set_visible((GtkWidget *) pcm_file_dialog->audio_channel,
			   FALSE);
  }

  if((AGS_PCM_FILE_DIALOG_SHOW_NEW_CHANNEL & (pcm_file_dialog->flags)) == 0){
    gtk_widget_set_visible((GtkWidget *) pcm_file_dialog->new_channel,
			   FALSE);
  }
  
  if((AGS_PCM_FILE_DIALOG_SHOW_EXISTING_CHANNEL & (pcm_file_dialog->flags)) == 0){
    gtk_widget_set_visible((GtkWidget *) pcm_file_dialog->existing_channel,
			   FALSE);
  }
  
  if((AGS_PCM_FILE_DIALOG_SHOW_DOWNLOAD_LINK & (pcm_file_dialog->flags)) == 0){
    gtk_widget_set_visible((GtkWidget *) pcm_file_dialog->download_link,
			   FALSE);
  }
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_pcm_file_dialog_parent_class)->show(widget);
}

/**
 * ags_pcm_file_dialog_test_flags:
 * @pcm_file_dialog: the #AgsPCMFileDialog
 * @flags: the flags
 *
 * Test @flags of @pcm_file_dialog.
 * 
 * Returns: %TRUE if flags set, otherwise %FALSE
 * 
 * Since: 6.6.0
 */
gboolean
ags_pcm_file_dialog_test_flags(AgsPCMFileDialog *pcm_file_dialog,
			       guint flags)
{
  gboolean success;

  if(!AGS_IS_PCM_FILE_DIALOG(pcm_file_dialog)){
    return(FALSE);
  }
  
  success = ((flags & (pcm_file_dialog->flags)) != 0) ? TRUE: FALSE;
  
  return(success);
}

/**
 * ags_pcm_file_dialog_set_flags:
 * @pcm_file_dialog: the #AgsPCMFileDialog
 * @flags: the flags
 *
 * Set @flags of @pcm_file_dialog.
 * 
 * Since: 6.6.0
 */
void
ags_pcm_file_dialog_set_flags(AgsPCMFileDialog *pcm_file_dialog,
			      guint flags)
{
  if(!AGS_IS_PCM_FILE_DIALOG(pcm_file_dialog)){
    return;
  }
  
  pcm_file_dialog->flags |= flags;
}

/**
 * ags_pcm_file_dialog_unset_flags:
 * @pcm_file_dialog: the #AgsPCMFileDialog
 * @flags: the flags
 *
 * Set @flags of @pcm_file_dialog.
 * 
 * Since: 6.6.0
 */
void
ags_pcm_file_dialog_unset_flags(AgsPCMFileDialog *pcm_file_dialog,
				guint flags)
{
  if(!AGS_IS_PCM_FILE_DIALOG(pcm_file_dialog)){
    return;
  }

  pcm_file_dialog->flags &= (~flags);
}

/**
 * ags_pcm_file_dialog_get_file_widget:
 * @pcm_file_dialog: the #AgsPCMFileDialog
 *
 * Get file widget of @pcm_file_dialog.
 * 
 * Returns: the #AgsFileWidget or %NULL
 * 
 * Since: 6.6.0
 */
AgsFileWidget*
ags_pcm_file_dialog_get_file_widget(AgsPCMFileDialog *pcm_file_dialog)
{
  g_return_val_if_fail(AGS_IS_PCM_FILE_DIALOG(pcm_file_dialog), NULL);

  return(pcm_file_dialog->file_widget);
}

/**
 * ags_pcm_file_dialog_get_audio_channel:
 * @pcm_file_dialog: the #AgsPCMFileDialog
 *
 * Get audio channel of @pcm_file_dialog.
 * 
 * Returns: the #GtkSpinButton or %NULL
 * 
 * Since: 6.6.0
 */
GtkSpinButton*
ags_pcm_file_dialog_get_audio_channel(AgsPCMFileDialog *pcm_file_dialog)
{
  g_return_val_if_fail(AGS_IS_PCM_FILE_DIALOG(pcm_file_dialog), NULL);

  return(pcm_file_dialog->audio_channel);
}

/**
 * ags_pcm_file_dialog_get_new_channel:
 * @pcm_file_dialog: the #AgsPCMFileDialog
 *
 * Get new channel of @pcm_file_dialog.
 * 
 * Returns: the #GtkCheckButton or %NULL
 * 
 * Since: 6.6.0
 */
GtkCheckButton*
ags_pcm_file_dialog_get_new_channel(AgsPCMFileDialog *pcm_file_dialog)
{
  g_return_val_if_fail(AGS_IS_PCM_FILE_DIALOG(pcm_file_dialog), NULL);

  return(pcm_file_dialog->new_channel);
}

/**
 * ags_pcm_file_dialog_get_existing_channel:
 * @pcm_file_dialog: the #AgsPCMFileDialog
 *
 * Get existing channel of @pcm_file_dialog.
 * 
 * Returns: the #GtkCheckButton or %NULL
 * 
 * Since: 6.6.0
 */
GtkCheckButton*
ags_pcm_file_dialog_get_existing_channel(AgsPCMFileDialog *pcm_file_dialog)
{
  g_return_val_if_fail(AGS_IS_PCM_FILE_DIALOG(pcm_file_dialog), NULL);

  return(pcm_file_dialog->existing_channel);
}

/**
 * ags_pcm_file_dialog_get_download_link:
 * @pcm_file_dialog: the #AgsPCMFileDialog
 *
 * Get download link of @pcm_file_dialog.
 * 
 * Returns: the #GtkLinkButton or %NULL
 * 
 * Since: 7.2.8
 */
GtkLinkButton*
ags_pcm_file_dialog_get_download_link(AgsPCMFileDialog *pcm_file_dialog)
{
  g_return_val_if_fail(AGS_IS_PCM_FILE_DIALOG(pcm_file_dialog), NULL);

  return(pcm_file_dialog->download_link);
}

void
ags_pcm_file_dialog_close_request_callback(GtkWindow *window,
					   AgsPCMFileDialog *pcm_file_dialog)
{
  ags_pcm_file_dialog_response(pcm_file_dialog,
			       GTK_RESPONSE_CANCEL);
}

void
ags_pcm_file_dialog_activate_button_callback(GtkButton *activate_button,
					     AgsPCMFileDialog *pcm_file_dialog)
{
  gchar *filename;
  gchar *basename;
  gchar *dirname;

  filename = ags_file_widget_get_filename(pcm_file_dialog->file_widget);

  dirname = g_path_get_dirname(filename);
  basename = g_path_get_basename(filename);

  if(pcm_file_dialog->file_widget->file_action == AGS_FILE_WIDGET_SAVE_AS){  
    gboolean writable_location;
    
    writable_location = (g_access(dirname, W_OK) == 0) ? TRUE: FALSE;
    
    if(writable_location &&
       basename != NULL &&
       strlen(basename) > 0 &&
       (!g_strncasecmp(basename, "/", 2)) == FALSE &&
       (!g_strncasecmp(basename, ".", 2)) == FALSE &&
       (!g_strncasecmp(basename, "..", 3)) == FALSE &&
       !g_file_test(filename, G_FILE_TEST_IS_DIR)){
      ags_pcm_file_dialog_response(pcm_file_dialog,
				   GTK_RESPONSE_ACCEPT);
    }    
  }else{
    GSList *start_filenames, *filenames;

    gboolean readable_location;
    gboolean success;

    filenames =
      start_filenames = ags_file_widget_get_filenames(pcm_file_dialog->file_widget);

    readable_location = (g_access(dirname, R_OK) == 0) ? TRUE: FALSE;
    
    if(readable_location &&
       basename != NULL &&
       strlen(basename) > 0){
      success = FALSE;
      
      if(!g_file_test(filename, G_FILE_TEST_IS_DIR)){
	success = TRUE;
		
	ags_pcm_file_dialog_response(pcm_file_dialog,
				     GTK_RESPONSE_ACCEPT);
      }

      if(!success){
	while(!success &&
	      filenames != NULL){
	  if(!g_file_test(filenames->data, G_FILE_TEST_IS_DIR)){
	    success = TRUE;
	
	    ags_pcm_file_dialog_response(pcm_file_dialog,
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
ags_pcm_file_dialog_key_pressed_callback(GtkEventControllerKey *event_controller,
					 guint keyval,
					 guint keycode,
					 GdkModifierType state,
					 AgsPCMFileDialog *pcm_file_dialog)
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
ags_pcm_file_dialog_key_released_callback(GtkEventControllerKey *event_controller,
					  guint keyval,
					  guint keycode,
					  GdkModifierType state,
					  AgsPCMFileDialog *pcm_file_dialog)
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
	ags_pcm_file_dialog_response(pcm_file_dialog,
				     GTK_RESPONSE_CANCEL);	
      }
      break;
    }
  }
}

gboolean
ags_pcm_file_dialog_modifiers_callback(GtkEventControllerKey *event_controller,
				       GdkModifierType keyval,
				       AgsPCMFileDialog *pcm_file_dialog)
{
  return(FALSE);
}

void
ags_pcm_file_dialog_real_response(AgsPCMFileDialog *pcm_file_dialog,
				  gint response)
{  
  gtk_widget_set_visible((GtkWidget *) pcm_file_dialog,
			 FALSE);
}

/**
 * ags_pcm_file_dialog_response:
 * @pcm_file_dialog: the #AgsPCMFileDialog
 * @response: the response
 *
 * Response @pcm_file_dialog due to user action.
 * 
 * Since: 6.6.0
 */
void
ags_pcm_file_dialog_response(AgsPCMFileDialog *pcm_file_dialog,
			     gint response)
{
  g_return_if_fail(AGS_IS_PCM_FILE_DIALOG(pcm_file_dialog));
  
  g_object_ref((GObject *) pcm_file_dialog);
  g_signal_emit(G_OBJECT(pcm_file_dialog),
		pcm_file_dialog_signals[RESPONSE], 0,
		response);
  g_object_unref((GObject *) pcm_file_dialog);
}

/**
 * ags_pcm_file_dialog_new:
 * @transient_for: the transient for window
 * @title: the title
 * 
 * Create a new instance of #AgsPCMFileDialog
 * 
 * Returns: the #AgsPCMFileDialog
 * 
 * Since: 4.0.0
 */
AgsPCMFileDialog*
ags_pcm_file_dialog_new(GtkWindow *transient_for,
			gchar *title)
{
  AgsPCMFileDialog *pcm_file_dialog;

  pcm_file_dialog = (AgsPCMFileDialog *) g_object_new(AGS_TYPE_PCM_FILE_DIALOG,
						      "transient-for", transient_for,
						      "title", title,
						      NULL);

  return(pcm_file_dialog);
}
