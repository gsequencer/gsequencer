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

#include <ags/X/machine/ags_oscillator_callbacks.h>

void
ags_oscillator_wave_callback(GtkComboBox *combo, AgsOscillator *oscillator)
{
  ags_oscillator_control_changed(oscillator);
}

void
ags_oscillator_attack_callback(GtkSpinButton *spin_button, AgsOscillator *oscillator)
{
  ags_oscillator_control_changed(oscillator);
}

void
ags_oscillator_frame_count_callback(GtkSpinButton *spin_button, AgsOscillator *oscillator)
{
  ags_oscillator_control_changed(oscillator);
}

void
ags_oscillator_frequency_callback(GtkSpinButton *spin_button, AgsOscillator *oscillator)
{
  ags_oscillator_control_changed(oscillator);
}

void
ags_oscillator_phase_callback(GtkSpinButton *spin_button, AgsOscillator *oscillator)
{
  ags_oscillator_control_changed(oscillator);
}

void
ags_oscillator_volume_callback(GtkSpinButton *spin_button, AgsOscillator *oscillator)
{
  ags_oscillator_control_changed(oscillator);
}
