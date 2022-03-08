/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

void ags_wave_export_dialog_file_chooser_response_callback(GtkDialog *file_chooser,
							   gint response,
							   AgsWaveExportDialog *wave_export_dialog);

void
ags_wave_export_dialog_file_chooser_response_callback(GtkDialog *file_chooser,
						      gint response,
						      AgsWaveExportDialog *wave_export_dialog)
{
  if(response == GTK_RESPONSE_ACCEPT){
    GFile *file;
    
    char *filename;

    file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(file_chooser));

    filename = g_file_get_path(file);
    
    gtk_editable_set_text(GTK_EDITABLE(wave_export_dialog->filename),
			  filename);
  }
  
  gtk_window_destroy((GtkWindow *) file_chooser);
}

void
ags_wave_export_dialog_file_chooser_button_callback(GtkWidget *file_chooser_button,
						    AgsWaveExportDialog *wave_export_dialog)
{
  GtkFileChooserDialog *file_chooser;
  
  file_chooser = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new(i18n("Export to file ..."),
								      GTK_WINDOW(wave_export_dialog),
								      GTK_FILE_CHOOSER_ACTION_SAVE,
								      i18n("_Cancel"), GTK_RESPONSE_CANCEL,
								      i18n("_OK"), GTK_RESPONSE_ACCEPT,
								      NULL);

  gtk_widget_show(file_chooser);
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

int
ags_wave_export_dialog_apply_callback(GtkWidget *widget, AgsWaveExportDialog *wave_export_dialog)
{
  ags_applicable_apply(AGS_APPLICABLE(wave_export_dialog));

  //TODO:JK: remove me
  //  ags_applicable_reset(AGS_APPLICABLE(wave_export_dialog));

  return(0);
}

int
ags_wave_export_dialog_ok_callback(GtkWidget *widget, AgsWaveExportDialog *wave_export_dialog)
{
  //  ags_applicable_set_update(AGS_APPLICABLE(wave_export_dialog), FALSE);
  ags_connectable_disconnect(AGS_CONNECTABLE(wave_export_dialog));
  ags_applicable_apply(AGS_APPLICABLE(wave_export_dialog));
 
  wave_export_dialog->machine->wave_export_dialog = NULL;
  gtk_window_destroy((GtkWindow *) wave_export_dialog);

  return(0);
}

int
ags_wave_export_dialog_cancel_callback(GtkWidget *widget, AgsWaveExportDialog *wave_export_dialog)
{
  wave_export_dialog->machine->wave_export_dialog = NULL;
  gtk_window_destroy((GtkWindow *) wave_export_dialog);

  return(0);
}
