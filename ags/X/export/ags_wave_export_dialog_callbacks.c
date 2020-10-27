/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/X/export/ags_wave_export_dialog_callbacks.h>

void
ags_wave_export_dialog_file_chooser_button_callback(GtkWidget *file_chooser_button,
						    AgsWaveExportDialog *wave_export_dialog)
{
  GtkFileChooserDialog *file_chooser;
  
  file_chooser = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new("Export to file ...",
								      GTK_WINDOW(wave_export_dialog),
								      GTK_FILE_CHOOSER_ACTION_SAVE,
								      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
								      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
								      NULL);
  if(gtk_dialog_run(GTK_DIALOG(file_chooser)) == GTK_RESPONSE_ACCEPT){
    char *filename;

    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
    gtk_entry_set_text(wave_export_dialog->filename,
		       filename);
  }
  
  gtk_widget_destroy((GtkWidget *) file_chooser);
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
  gtk_widget_destroy((GtkWidget *) wave_export_dialog);

  return(0);
}

int
ags_wave_export_dialog_cancel_callback(GtkWidget *widget, AgsWaveExportDialog *wave_export_dialog)
{
  wave_export_dialog->machine->wave_export_dialog = NULL;
  gtk_widget_destroy((GtkWidget *) wave_export_dialog);

  return(0);
}

gboolean
ags_wave_export_dialog_delete_event(GtkWidget *widget, GdkEventAny *event,
				    AgsWaveExportDialog *wave_export_dialog)
{
  wave_export_dialog->machine->wave_export_dialog = NULL;

  return(TRUE);
}
