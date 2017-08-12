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

#include <ags/X/machine/ags_synth_input_line_callbacks.h>
#include <ags/X/machine/ags_synth.h>

void
ags_synth_input_line_oscillator_control_changed_callback(AgsOscillator *oscillator,
							 AgsSynthInputLine *synth_input_line)
{
  AgsSynth *synth;

  synth = (AgsSynth *) gtk_widget_get_ancestor((GtkWidget *) synth_input_line,
					       AGS_TYPE_SYNTH);

  if((AGS_SYNTH_AUTO_UPDATE & (synth->flags)) != 0){
    ags_synth_update(synth);
  }
}
