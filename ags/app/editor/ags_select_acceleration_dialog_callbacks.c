/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/app/editor/ags_select_acceleration_dialog_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>

#include <ags/i18n.h>

void
ags_select_acceleration_dialog_response_callback(GtkWidget *dialog, gint response,
						 AgsSelectAccelerationDialog *select_acceleration_dialog)
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
      ags_applicable_apply(AGS_APPLICABLE(select_acceleration_dialog));
    }
  case GTK_RESPONSE_CANCEL:
    {
      if(hide_dialog){
	gtk_widget_hide((GtkWidget *) select_acceleration_dialog);
      }
    }
  }
}

void
ags_select_acceleration_dialog_add_callback(GtkWidget *button,
					    AgsSelectAccelerationDialog *select_acceleration_dialog)
{
}

void
ags_select_acceleration_dialog_machine_changed_callback(AgsCompositeEditor *composite_editor,
							AgsMachine *machine,
							AgsSelectAccelerationDialog *select_acceleration_dialog)
{
  ags_applicable_reset(AGS_APPLICABLE(select_acceleration_dialog));
}
