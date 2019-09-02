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

#ifndef __AGS_PITCH_SAMPLER_FILE_CALLBACKS_H__ 
#define __AGS_PITCH_SAMPLER_FILE_CALLBACKS_H__ 

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/X/machine/ags_pitch_sampler_file.h>

void ags_pitch_sampler_file_open_callback(GtkWidget *button, AgsPitchSamplerFile *pitch_sampler_file);

void ags_pitch_sampler_file_base_key_callback(GtkSpinButton *spin_button, AgsPitchSamplerFile *pitch_sampler_file);

void ags_pitch_sampler_file_loop_start_callback(GtkSpinButton *spin_button, AgsPitchSamplerFile *pitch_sampler_file);
void ags_pitch_sampler_file_loop_end_callback(GtkSpinButton *spin_button, AgsPitchSamplerFile *pitch_sampler_file);

#endif /*__AGS_PITCH_SAMPLER_FILE_CALLBACKS_H__ */
