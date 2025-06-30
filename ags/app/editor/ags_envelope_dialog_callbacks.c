/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/app/editor/ags_envelope_dialog_callbacks.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

gboolean
ags_envelope_dialog_close_request_callback(GtkWindow *window, gpointer user_data)
{
  ags_envelope_dialog_response((AgsEnvelopeDialog *) window,
			       GTK_RESPONSE_CLOSE);	

  return(FALSE);
}

int
ags_envelope_dialog_apply_callback(GtkWidget *widget, AgsEnvelopeDialog *envelope_dialog)
{
  ags_applicable_apply(AGS_APPLICABLE(envelope_dialog));

  //TODO:JK: remove me
  //  ags_applicable_reset(AGS_APPLICABLE(envelope_dialog));

  return(0);
}

int
ags_envelope_dialog_ok_callback(GtkWidget *widget, AgsEnvelopeDialog *envelope_dialog)
{
  //  ags_applicable_set_update(AGS_APPLICABLE(envelope_dialog), FALSE);
  ags_connectable_disconnect(AGS_CONNECTABLE(envelope_dialog));
  ags_applicable_apply(AGS_APPLICABLE(envelope_dialog));
 
  envelope_dialog->machine->envelope_dialog = NULL;
  gtk_window_destroy((GtkWindow *) envelope_dialog);

  return(0);
}

int
ags_envelope_dialog_cancel_callback(GtkWidget *widget, AgsEnvelopeDialog *envelope_dialog)
{
  envelope_dialog->machine->envelope_dialog = NULL;
  gtk_window_destroy((GtkWindow *) envelope_dialog);

  return(0);
}
