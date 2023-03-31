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

#include <ags/app/ags_export_window_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_navigation.h>

#include <glib/gstdio.h>

void ags_export_window_replace_files_response_callback(GtkDialog *dialog,
						       gint response,
						       AgsExportWindow *export_window);

void
ags_export_window_add_export_soundcard_callback(GtkWidget *button,
						AgsExportWindow *export_window)
{
  AgsExportSoundcard *export_soundcard;

  export_soundcard = ags_export_soundcard_new();
  
  ags_export_window_add_export_soundcard(export_window,
					 export_soundcard);
}

void
ags_export_window_remove_export_soundcard_callback(GtkWidget *button,
						   AgsExportWindow *export_window)
{
  AgsExportSoundcard *export_soundcard;

  export_soundcard = (AgsExportSoundcard *) gtk_widget_get_ancestor(button,
								    AGS_TYPE_EXPORT_SOUNDCARD);
  
  ags_export_window_remove_export_soundcard(export_window,
					    export_soundcard);
}

void
ags_export_window_tact_callback(GtkWidget *spin_button,
				AgsExportWindow *export_window)
{
  AgsWindow *window;

  AgsApplicationContext *application_context;

  GObject *default_soundcard;
  
  gchar *str;
  
  gdouble delay_factor;
  gdouble delay;

  /* retrieve window */
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* get some properties */
  delay_factor = ags_soundcard_get_delay_factor(AGS_SOUNDCARD(default_soundcard));
  delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(default_soundcard));

  /* update duration */
  str = ags_time_get_uptime_from_offset(gtk_spin_button_get_value(export_window->tact) * 16.0,
					gtk_spin_button_get_value(window->navigation->bpm),
					delay,
					delay_factor);
  gtk_label_set_text(export_window->duration,
		     str);
  g_free(str);
}

void
ags_export_window_replace_files_response_callback(GtkDialog *dialog,
						  gint response,
						  AgsExportWindow *export_window)
{
  GList *start_remove_filename, *remove_filename;

  remove_filename =
    start_remove_filename = export_window->remove_filename;

  export_window->remove_filename = NULL;

  if(response == GTK_RESPONSE_ACCEPT ||
     response == GTK_RESPONSE_OK){
    /* remove files */
    while(remove_filename != NULL){
      g_remove(remove_filename->data);

      remove_filename = remove_filename->next;
    }

    ags_export_window_start_export(export_window);
  }  
  
  g_list_free_full(start_remove_filename,
		   (GDestroyNotify) g_free);
  
  gtk_window_destroy((GtkWindow *) dialog);
}

void
ags_export_window_export_callback(GtkWidget *toggle_button,
				  AgsExportWindow *export_window)
{
  if(gtk_toggle_button_get_active((GtkToggleButton *) toggle_button)){
    GList *start_export_soundcard, *export_soundcard;
    GList *all_filename;
    GList *start_remove_filename;
    
    gchar *filename;

    gboolean file_exists;

    export_soundcard =
      start_export_soundcard = ags_export_window_get_export_soundcard(export_window);
    
    all_filename = NULL;
    start_remove_filename = NULL;
    
    file_exists = FALSE;

    while(export_soundcard != NULL){
      GtkEntryBuffer *entry_buffer;

      entry_buffer = gtk_entry_get_buffer(AGS_EXPORT_SOUNDCARD(export_soundcard->data)->filename);

      filename = gtk_entry_buffer_get_text(entry_buffer);
      all_filename = g_list_prepend(all_filename,
				    filename);
      
      /* test filename */
      if(filename == NULL ||
	 strlen(filename) == 0){
	export_soundcard = export_soundcard->next;
	  
	continue;
      }
      
      if(g_file_test(filename,
		     G_FILE_TEST_EXISTS)){      
	if(g_file_test(filename,
		       (G_FILE_TEST_IS_DIR | G_FILE_TEST_IS_SYMLINK))){
	  export_soundcard = export_soundcard->next;
	  
	  continue;
	}

	start_remove_filename = g_list_prepend(start_remove_filename,
					       filename);
	file_exists = TRUE;
      }
      
      export_soundcard = export_soundcard->next;
    }

    if(file_exists){
      GtkDialog *dialog;

      dialog = (GtkDialog *) gtk_message_dialog_new((GtkWindow *) export_window,
						    GTK_DIALOG_MODAL,
						    GTK_MESSAGE_QUESTION,
						    GTK_BUTTONS_OK_CANCEL,
						    "Replace existing file(s)?");

      export_window->remove_filename = start_remove_filename;
      
      g_signal_connect((GObject *) dialog, "response",
		       G_CALLBACK(ags_export_window_replace_files_response_callback), export_window);
    }else{
      ags_export_window_start_export(export_window);
    }
  }else{
    ags_export_window_stop_export(export_window);
  }
}

void
ags_export_window_update_ui_callback(AgsApplicationContext *application_context,
				     AgsExportWindow *export_window)
{
  if(ags_export_window_test_flags(export_window,
				  AGS_EXPORT_WINDOW_HAS_STOP_TIMEOUT)){
    if(g_atomic_int_get(&(export_window->do_stop))){
      g_atomic_int_set(&(export_window->do_stop),
		       FALSE);
    
      ags_export_window_stop_export(export_window);
      gtk_toggle_button_set_active(export_window->export,
				   FALSE);
    }
  }
}

void
ags_export_window_stop_callback(AgsThread *thread,
				AgsExportWindow *export_window)
{
  g_atomic_int_set(&(export_window->do_stop),
		   TRUE);
}
