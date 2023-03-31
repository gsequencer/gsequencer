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

#include <ags/app/import/ags_midi_import_wizard_callbacks.h>

#include <ags/app/ags_ui_provider.h>

#include <ags/app/import/ags_track_collection.h>

#include <libxml/tree.h>

void
ags_midi_import_wizard_response_callback(GtkWidget *wizard, gint response, gpointer data)
{
  AgsMidiImportWizard *midi_import_wizard;

  AgsApplicationContext *application_context;
  
  midi_import_wizard = (AgsMidiImportWizard *) wizard;

  application_context = ags_application_context_get_instance();
  
  switch(response){
  case GTK_RESPONSE_CANCEL:
    {
      if(ags_midi_import_wizard_test_flags(midi_import_wizard, AGS_MIDI_IMPORT_WIZARD_SHOW_TRACK_COLLECTION)){
	/* show/hide */
	ags_midi_import_wizard_unset_flags(midi_import_wizard,
					   AGS_MIDI_IMPORT_WIZARD_SHOW_TRACK_COLLECTION);
	
	ags_midi_import_wizard_set_flags(midi_import_wizard,
					 AGS_MIDI_IMPORT_WIZARD_SHOW_FILE_CHOOSER);
      }
    }
    break;
  case GTK_RESPONSE_OK:
    {      
      if(ags_midi_import_wizard_test_flags(midi_import_wizard, AGS_MIDI_IMPORT_WIZARD_SHOW_FILE_CHOOSER)){
	AgsMidiParser *midi_parser;

	GFile *file;
	
	xmlDoc *midi_doc;

	gchar *filename;

	/* show/hide */
	ags_midi_import_wizard_unset_flags(midi_import_wizard,
					   AGS_MIDI_IMPORT_WIZARD_SHOW_FILE_CHOOSER);

	ags_midi_import_wizard_set_flags(midi_import_wizard,
					 AGS_MIDI_IMPORT_WIZARD_SHOW_TRACK_COLLECTION);

	/* parse */
	file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(midi_import_wizard->file_chooser));

	filename = g_file_get_path(file);

	midi_parser = ags_midi_parser_new_from_filename(filename);
	midi_doc = ags_midi_parser_parse_full(midi_parser);
	//	xmlSaveFormatFileEnc("-", midi_doc, "UTF-8", 1);
	
	g_object_set(midi_import_wizard->track_collection,
		     "midi-document", midi_doc,
		      NULL);
	ags_track_collection_parse((AgsTrackCollection *) midi_import_wizard->track_collection);

	g_object_unref(file);
      }
    }
    break;
  case GTK_RESPONSE_ACCEPT:
    {
      ags_applicable_apply(AGS_APPLICABLE(wizard));
    }
  case GTK_RESPONSE_DELETE_EVENT:
  case GTK_RESPONSE_CLOSE:
  case GTK_RESPONSE_REJECT:
    {
      ags_ui_provider_set_midi_import_wizard(AGS_UI_PROVIDER(application_context),
					     NULL);
      
      gtk_window_destroy((GtkWindow *) wizard);
    }
    break;
  default:
    g_warning("unknown response");
  }
}
