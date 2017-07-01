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

#include <ags/X/editor/ags_crop_note_dialog_callbacks.h>

#include <ags/object/ags_applicable.h>

void
ags_crop_note_dialog_response_callback(GtkWidget *dialog, gint response,
				       AgsCropNoteDialog *crop_note_dialog)
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
      ags_applicable_apply(AGS_APPLICABLE(crop_note_dialog));
    }
  case GTK_RESPONSE_CANCEL:
    {
      if(hide_dialog){
	gtk_widget_hide(crop_note_dialog);
      }
    }
  }
}

void
ags_crop_note_dialog_absolute_callback(GtkWidget *button,
				       AgsCropNoteDialog *crop_note_dialog)
{
  if(gtk_toggle_button_get_active(crop_note_dialog->absolute)){
    gtk_spin_button_set_range(crop_note_dialog->crop_note,
			      0.0,
			      AGS_CROP_NOTE_DIALOG_MAX_WIDTH);
  }else{
    gtk_spin_button_set_range(crop_note_dialog->crop_note,
			      -1.0 * AGS_CROP_NOTE_DIALOG_MAX_WIDTH,
			      AGS_CROP_NOTE_DIALOG_MAX_WIDTH);
  }
}
