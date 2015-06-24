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

#include <ags/X/machine/ags_drum_output_pad_callbacks.h>

#include <ags/X/machine/ags_drum.h>

void
ags_drum_output_pad_play_done(AgsRecall *recall, AgsRecallID *recall_id, AgsDrumOutputPad *drum_output_pad)
{
  AgsDrum *drum;

  drum = (AgsDrum *) gtk_widget_get_ancestor((GtkWidget *) drum_output_pad, AGS_TYPE_DRUM);

  /*
  drum->play_ref++;

  if(drum->play_ref == drum->machine.audio->output_lines){
    drum->play_ref = 0;
    drum->machine.audio->devout_play->flags |= AGS_DEVOUT_PLAY_DONE;
    gtk_toggle_button_set_active(drum->run, FALSE);
  }
  */
}

void
ags_drum_output_pad_play_cancel(AgsRecall *recall, AgsRecallID *recall_id, AgsDrumOutputPad *drum_output_pad)
{
}
