/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/export/ags_midi_export_wizard_callbacks.h>

#include <ags/object/ags_applicable.h>

#include <ags/X/ags_window.h>

void
ags_midi_export_wizard_response_callback(GtkWidget *wizard, gint response, gpointer data)
{
  AgsMidiExportWizard *midi_export_wizard;

  midi_export_wizard = (AgsMidiExportWizard *) wizard;
  
  switch(response){
  case GTK_RESPONSE_REJECT:
    {
      if((AGS_MIDI_EXPORT_WIZARD_SHOW_FILE_CHOOSER & (midi_export_wizard->flags)) != 0){
	/* show/hide */
	gtk_widget_hide(midi_export_wizard->file_chooser->parent);

	gtk_widget_show(midi_export_wizard->machine_collection->parent);
	gtk_widget_show_all(midi_export_wizard->machine_collection);

	midi_export_wizard->flags |= AGS_MIDI_EXPORT_WIZARD_SHOW_MACHINE_COLLECTION;
	midi_export_wizard->flags &= (~AGS_MIDI_EXPORT_WIZARD_SHOW_FILE_CHOOSER);
      }
    }
    break;
  case GTK_RESPONSE_ACCEPT:
    {
      if((AGS_MIDI_EXPORT_WIZARD_SHOW_MACHINE_COLLECTION  & (midi_export_wizard->flags)) != 0){
	/* show/hide */
	midi_export_wizard->flags |= AGS_MIDI_EXPORT_WIZARD_SHOW_FILE_CHOOSER;
	midi_export_wizard->flags &= (~AGS_MIDI_EXPORT_WIZARD_SHOW_MACHINE_COLLECTION);

	gtk_widget_hide(midi_export_wizard->machine_collection->parent);

	gtk_widget_show(midi_export_wizard->file_chooser->parent);
	gtk_widget_show_all(midi_export_wizard->file_chooser);
      }
    }
    break;
  case GTK_RESPONSE_OK:
    {
      ags_applicable_apply(AGS_APPLICABLE(wizard));
    }
  case GTK_RESPONSE_DELETE_EVENT:
  case GTK_RESPONSE_CLOSE:
  case GTK_RESPONSE_CANCEL:
    {
      AGS_WINDOW(midi_export_wizard->main_window)->midi_export_wizard = NULL;
      gtk_widget_destroy(wizard);
    }
    break;
  default:
    g_warning("unknown response");
  }
}

