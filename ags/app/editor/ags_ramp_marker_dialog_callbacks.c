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

#include <ags/app/editor/ags_ramp_marker_dialog_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_tempo_edit.h>

void
ags_ramp_marker_dialog_response_callback(GtkWidget *dialog, gint response,
					 AgsRampMarkerDialog *ramp_marker_dialog)
{
  gboolean hide_dialog;

  hide_dialog = TRUE;
  
  switch(response){
  case GTK_RESPONSE_APPLY:
    {
      hide_dialog = FALSE;
    }
  case GTK_RESPONSE_OK:
    {
      ags_applicable_apply(AGS_APPLICABLE(ramp_marker_dialog));
    }
  case GTK_RESPONSE_CANCEL:
    {
      if(hide_dialog){
	gtk_widget_hide((GtkWidget *) ramp_marker_dialog);
      }
    }
  }
}

void
ags_ramp_marker_dialog_control_name_callback(GtkComboBox *combo_box,
					     AgsRampMarkerDialog *ramp_marker_dialog)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;

  AgsApplicationContext *application_context;
  
  gchar *specifier;
    
  /* application context */
  application_context = ags_application_context_get_instance();
  
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = window->composite_editor;

  if(composite_editor->tempo_edit == NULL){
    return;
  }

  /* specifier */
  specifier = gtk_combo_box_text_get_active_text(ramp_marker_dialog->control_name);
  
  /* reset range */
  gtk_spin_button_set_range(ramp_marker_dialog->ramp_y0,
			    0.0,
			    240.0);
  gtk_spin_button_set_increments(ramp_marker_dialog->ramp_y0,
				 1.0,
				 1.0);
    
  gtk_spin_button_set_range(ramp_marker_dialog->ramp_y1,
			    0.0,
			    240.0);
  gtk_spin_button_set_increments(ramp_marker_dialog->ramp_y1,
				 1.0,
				 1.0);
    
  gtk_spin_button_set_range(ramp_marker_dialog->ramp_step_count,
			    0.0,
			    240.0);
}

void
ags_ramp_marker_dialog_machine_changed_callback(AgsCompositeEditor *composite_editor,
						AgsMachine *machine,
						AgsRampMarkerDialog *ramp_marker_dialog)
{
  ags_applicable_reset(AGS_APPLICABLE(ramp_marker_dialog));
}
