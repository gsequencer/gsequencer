/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_OSCILLATOR_CALLBACKS_H__ 
#define __AGS_OSCILLATOR_CALLBACKS_H__ 

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/machine/ags_oscillator.h>

void ags_oscillator_wave_callback(GtkComboBox *combo, AgsOscillator *oscillator);
void ags_oscillator_attack_callback(GtkSpinButton *spin_button, AgsOscillator *oscillator);
void ags_oscillator_frame_count_callback(GtkSpinButton *spin_button, AgsOscillator *oscillator);
void ags_oscillator_frequency_callback(GtkSpinButton *spin_button, AgsOscillator *oscillator);
void ags_oscillator_phase_callback(GtkSpinButton *spin_button, AgsOscillator *oscillator);
void ags_oscillator_volume_callback(GtkSpinButton *spin_button, AgsOscillator *oscillator);

#endif /*__AGS_OSCILLATOR_CALLBACKS_H__ */
