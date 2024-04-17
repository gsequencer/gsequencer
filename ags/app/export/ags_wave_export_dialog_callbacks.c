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

#include <ags/app/export/ags_wave_export_dialog_callbacks.h>

#include <ags/i18n.h>

void ags_wave_export_dialog_file_open_response_callback(AgsFileDialog *file_dialog,
							gint response,
							AgsWaveExportDialog *wave_export_dialog);

void
ags_wave_export_dialog_file_open_response_callback(AgsFileDialog *file_dialog,
						   gint response,
						   AgsWaveExportDialog *wave_export_dialog)
{
  if(response == GTK_RESPONSE_ACCEPT){
    gchar *filename;

    filename = ags_file_widget_get_filename(ags_file_dialog_get_file_widget(file_dialog));
    
    gtk_editable_set_text(GTK_EDITABLE(wave_export_dialog->filename),
			  filename);
  }
  
  gtk_window_destroy((GtkWindow *) file_dialog);
}

void
ags_wave_export_dialog_file_open_button_callback(GtkWidget *file_open_button,
						 AgsWaveExportDialog *wave_export_dialog)
{
  AgsFileDialog *file_dialog;
  AgsFileWidget *file_widget;

  AgsApplicationContext *application_context;

  gchar *recently_used_filename;
  gchar *bookmark_filename;
  gchar *home_path;
  gchar *sandbox_path;

  file_dialog = ags_file_dialog_new((GtkWindow *) wave_export_dialog,
				    i18n("export to file"));

  file_widget = ags_file_dialog_get_file_widget(file_dialog);

  home_path = ags_file_widget_get_home_path(file_widget);

  sandbox_path = NULL;

#if defined(AGS_MACOS_SANDBOX)
  sandbox_path = g_strdup_printf("%s/Library/%s",
				 home_path,
				 AGS_DEFAULT_BUNDLE_ID);

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_pcm_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_pcm_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif

#if defined(AGS_FLATPAK_SANDBOX)
  if((str = getenv("HOME")) != NULL){
    sandbox_path = g_strdup_printf("%s",
				   str);
  }

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_pcm_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_pcm_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif

#if defined(AGS_SNAP_SANDBOX)
  if((str = getenv("SNAP_USER_DATA")) != NULL){
    sandbox_path = g_strdup_printf("%s",
				   str);
  }

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_pcm_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_pcm_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif
  
#if !defined(AGS_MACOS_SANDBOX) && !defined(AGS_FLATPAK_SANDBOX) && !defined(AGS_SNAP_SANDBOX)
  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_pcm_recently_used.xml",
					   home_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_pcm_bookmark.xml",
				      home_path,
				      AGS_DEFAULT_DIRECTORY);
#endif

  /* recently-used */
  ags_file_widget_set_recently_used_filename(file_widget,
					     recently_used_filename);
  
  ags_file_widget_read_recently_used(file_widget);

  /* bookmark */
  ags_file_widget_set_bookmark_filename(file_widget,
					bookmark_filename);

  ags_file_widget_read_bookmark(file_widget);

#if defined(AGS_MACOS_SANDBOX)
  ags_file_widget_set_flags(file_widget,
			    AGS_FILE_WIDGET_APP_SANDBOX);

  ags_file_widget_set_current_path(file_widget,
				   sandbox_path);
#endif

#if defined(AGS_FLATPAK_SANDBOX)
  ags_file_widget_set_flags(file_widget,
			    AGS_FILE_WIDGET_APP_SANDBOX);

  ags_file_widget_set_current_path(file_widget,
				   sandbox_path);
#endif

#if defined(AGS_SNAP_SANDBOX)
  ags_file_widget_set_flags(file_widget,
			    AGS_FILE_WIDGET_APP_SANDBOX);

  ags_file_widget_set_current_path(file_widget,
				   sandbox_path);
#endif
  
#if !defined(AGS_MACOS_SANDBOX) && !defined(AGS_FLATPAK_SANDBOX) && !defined(AGS_SNAP_SANDBOX)
  ags_file_widget_set_current_path(file_widget,
				   home_path);
#endif

  ags_file_widget_refresh(file_widget);

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_USER_DESKTOP,
			       NULL);

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_DOCUMENTS,
			       NULL);  

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_MUSIC,
			       NULL);

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_USER_HOME,
			       NULL);

  ags_file_widget_set_file_action(file_widget,
				  AGS_FILE_WIDGET_SAVE_AS);

  ags_file_widget_set_default_bundle(file_widget,
				     AGS_DEFAULT_BUNDLE_ID);

  gtk_widget_set_visible((GtkWidget *) file_dialog,
			 TRUE);

  g_signal_connect((GObject *) file_dialog, "response",
		   G_CALLBACK(ags_wave_export_dialog_file_open_response_callback), wave_export_dialog);
}

void
ags_wave_export_dialog_start_tact_callback(GtkSpinButton *spin_button, AgsWaveExportDialog *wave_export_dialog)
{
  ags_wave_export_dialog_update_duration(wave_export_dialog);
}

void
ags_wave_export_dialog_end_tact_callback(GtkSpinButton *spin_button, AgsWaveExportDialog *wave_export_dialog)
{
  ags_wave_export_dialog_update_duration(wave_export_dialog);
}

void
ags_wave_export_dialog_save_as_callback(GtkWidget *widget, AgsWaveExportDialog *wave_export_dialog)
{
  //  ags_applicable_set_update(AGS_APPLICABLE(wave_export_dialog), FALSE);
  ags_connectable_disconnect(AGS_CONNECTABLE(wave_export_dialog));
  ags_applicable_apply(AGS_APPLICABLE(wave_export_dialog));
 
  wave_export_dialog->machine->wave_export_dialog = NULL;
  gtk_window_destroy((GtkWindow *) wave_export_dialog);
}
