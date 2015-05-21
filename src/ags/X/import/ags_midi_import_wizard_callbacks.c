/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/import/ags_midi_import_wizard_callbacks.h>

#include <ags/object/ags_applicable.h>

void
ags_midi_import_wizard_response_callback(GtkWidget *wizard, gint response, gpointer data)
{
  AgsMidiImportWizard *midi_import_wizard;

  midi_import_wizard = (AgsMidiImportWizard *) wizard;
  
  switch(response){
  case GTK_RESPONSE_REJECT:
    {
      if((AGS_MIDI_IMPORT_WIZARD_SHOW_TRACK_COLLECTION  & (midi_import_wizard->flags)) != 0){
	midi_import_wizard->flags |= AGS_MIDI_IMPORT_WIZARD_SHOW_FILE_CHOOSER;
	midi_import_wizard->flags &= (~AGS_MIDI_IMPORT_WIZARD_SHOW_TRACK_COLLECTION);

	gtk_widget_hide(midi_import_wizard->track_collection->parent);

	gtk_widget_show(midi_import_wizard->file_chooser->parent);
	gtk_widget_show_all(midi_import_wizard->file_chooser);
      }
    }
    break;
  case GTK_RESPONSE_ACCEPT:
    {
      if((AGS_MIDI_IMPORT_WIZARD_SHOW_FILE_CHOOSER & (midi_import_wizard->flags)) != 0){
	midi_import_wizard->flags |= AGS_MIDI_IMPORT_WIZARD_SHOW_TRACK_COLLECTION;
	midi_import_wizard->flags &= (~AGS_MIDI_IMPORT_WIZARD_SHOW_FILE_CHOOSER);

	gtk_widget_hide(midi_import_wizard->file_chooser->parent);

	gtk_widget_show(midi_import_wizard->track_collection->parent);
	gtk_widget_show_all(midi_import_wizard->track_collection);
      }
    }
    break;
  case GTK_RESPONSE_OK:
    {
      ags_applicable_apply(AGS_APPLICABLE(wizard));
    }
  case GTK_RESPONSE_CANCEL:
    {
      gtk_widget_hide(wizard);
    }
    break;
  default:
    g_warning("unknown response\0");
  }
}

