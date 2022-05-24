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

#include <ags/app/export/ags_midi_export_wizard_callbacks.h>

#include <ags/app/ags_ui_provider.h>

void
ags_midi_export_wizard_response_callback(GtkWidget *wizard, gint response, gpointer data)
{
  AgsMidiExportWizard *midi_export_wizard;

  AgsApplicationContext *application_context;

  midi_export_wizard = (AgsMidiExportWizard *) wizard;

  application_context = ags_application_context_get_instance();
  
  switch(response){
  case GTK_RESPONSE_CANCEL:
    {
      if(ags_midi_export_wizard_test_flags(midi_export_wizard, AGS_MIDI_EXPORT_WIZARD_SHOW_FILE_CHOOSER)){
	/* show/hide */
	ags_midi_export_wizard_unset_flags(midi_export_wizard,
					   AGS_MIDI_EXPORT_WIZARD_SHOW_FILE_CHOOSER);
	
	ags_midi_export_wizard_set_flags(midi_export_wizard,
					 AGS_MIDI_EXPORT_WIZARD_SHOW_MACHINE_COLLECTION);
      }
    }
    break;
  case GTK_RESPONSE_OK:
    {
      if(ags_midi_export_wizard_test_flags(midi_export_wizard, AGS_MIDI_EXPORT_WIZARD_SHOW_MACHINE_COLLECTION)){
	/* show/hide */
	ags_midi_export_wizard_unset_flags(midi_export_wizard,
					   AGS_MIDI_EXPORT_WIZARD_SHOW_MACHINE_COLLECTION);
	
	ags_midi_export_wizard_set_flags(midi_export_wizard,
					 AGS_MIDI_EXPORT_WIZARD_SHOW_FILE_CHOOSER);
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
      ags_ui_provider_set_midi_export_wizard(AGS_UI_PROVIDER(application_context),
					     NULL);
      
      gtk_window_destroy(wizard);
    }
    break;
  default:
    g_warning("unknown response");
  }
}

