/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/app/ags_midi_dialog_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/app/ags_window.h>

int
ags_midi_dialog_apply_callback(GtkWidget *widget, AgsMidiDialog *midi_dialog)
{
  ags_applicable_apply(AGS_APPLICABLE(midi_dialog));

  //TODO:JK: remove me
  //  ags_applicable_reset(AGS_APPLICABLE(midi_dialog));

  return(0);
}

int
ags_midi_dialog_ok_callback(GtkWidget *widget, AgsMidiDialog *midi_dialog)
{
  //  ags_applicable_set_update(AGS_APPLICABLE(midi_dialog), FALSE);
  ags_connectable_disconnect(AGS_CONNECTABLE(midi_dialog));
  ags_applicable_apply(AGS_APPLICABLE(midi_dialog));
 
  midi_dialog->machine->midi_dialog = NULL;
  gtk_widget_destroy((GtkWidget *) midi_dialog);

  return(0);
}

int
ags_midi_dialog_cancel_callback(GtkWidget *widget, AgsMidiDialog *midi_dialog)
{
  midi_dialog->machine->midi_dialog = NULL;
  gtk_widget_destroy((GtkWidget *) midi_dialog);

  return(0);
}


gboolean
ags_midi_dialog_delete_event(GtkWidget *widget, GdkEventAny *event,
			     AgsMidiDialog *midi_dialog)
{
  midi_dialog->machine->midi_dialog = NULL;

  return(TRUE);
}
