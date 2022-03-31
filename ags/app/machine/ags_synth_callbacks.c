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

#include <ags/app/machine/ags_synth_callbacks.h>
#include <ags/app/machine/ags_oscillator.h>

#include <ags/app/ags_window.h>

#include <math.h>

void
ags_synth_auto_update_callback(GtkToggleButton *toggle, AgsSynth *synth)
{
  if(gtk_toggle_button_get_active(toggle)){
    ags_synth_set_flags(synth, AGS_SYNTH_AUTO_UPDATE);
  }else{
    ags_synth_unset_flags(synth, AGS_SYNTH_AUTO_UPDATE);
  }
}

void
ags_synth_update_callback(GtkButton *button, AgsSynth *synth)
{
  ags_synth_update(synth);
}

void
ags_synth_lower_callback(GtkSpinButton *spin_button, AgsSynth *synth)
{
  if(ags_synth_test_flags(synth, AGS_SYNTH_AUTO_UPDATE)){
    ags_synth_update(synth);
  }
}
