/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/ags_export_soundcard_callbacks.h>

#include <ags/X/ags_export_window.h>

void
ags_export_soundcard_backend_callback(GtkWidget *combo_box,
				      AgsExportSoundcard *export_soundcard)
{
  //TODO:JK: implement me
}

void
ags_export_soundcard_file_chooser_button_callback(GtkWidget *file_chooser_button,
						  AgsExportSoundcard *export_soundcard)
{
  AgsExportWindow *export_window;
  GtkFileChooserDialog *file_chooser;
  
  export_window = (AgsExportWindow *) gtk_widget_get_ancestor(export_soundcard,
							      AGS_TYPE_EXPORT_WINDOW);
  
  file_chooser = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new("Export to file ...\0",
								      GTK_WINDOW(export_window),
								      GTK_FILE_CHOOSER_ACTION_SAVE,
								      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
								      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
								      NULL);
  if(gtk_dialog_run(GTK_DIALOG(file_chooser)) == GTK_RESPONSE_ACCEPT){
    char *filename;

    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
    gtk_entry_set_text(export_soundcard->filename,
		       filename);
  }
  
  gtk_widget_destroy((GtkWidget *) file_chooser);
}
