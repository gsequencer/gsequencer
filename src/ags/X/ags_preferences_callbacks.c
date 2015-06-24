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

#include <ags/X/ags_preferences_callbacks.h>

#include <ags/object/ags_applicable.h>

#include <ags/X/ags_window.h>

void
ags_preferences_response_callback(GtkDialog *dialog, gint response_id, gpointer user_data)
{
  gboolean apply;

  apply = FALSE;

  switch(response_id){
  case GTK_RESPONSE_APPLY:
    {
      apply = TRUE;
    }
  case GTK_RESPONSE_OK:
    {
      ags_applicable_apply(AGS_APPLICABLE(dialog));
      gtk_dialog_run((GtkDialog *) gtk_message_dialog_new((GtkWindow *) dialog,
							  GTK_DIALOG_MODAL,
							  GTK_MESSAGE_WARNING,
							  GTK_BUTTONS_OK,
							  "Restart Advanced Gtk+ Sequencer now!\0"));
      
      if(apply){
	ags_applicable_reset(AGS_APPLICABLE(dialog));

	break;
      }
    }
  case GTK_RESPONSE_CANCEL:
    {
      AGS_PREFERENCES(dialog)->flags |= AGS_PREFERENCES_SHUTDOWN;
      AGS_WINDOW(AGS_PREFERENCES(dialog)->window)->preferences = NULL;
      gtk_widget_destroy(GTK_WIDGET(dialog));
    }
  }
}
