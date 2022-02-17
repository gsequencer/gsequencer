/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_PITCH_SAMPLER_CALLBACKS_H__ 
#define __AGS_PITCH_SAMPLER_CALLBACKS_H__ 

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/GSequencer/machine/ags_pitch_sampler.h>

G_BEGIN_DECLS

void ags_pitch_sampler_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsPitchSampler *pitch_sampler);

void ags_pitch_sampler_open_callback(GtkButton *button, AgsPitchSampler *pitch_sampler);

void ags_pitch_sampler_file_control_changed_callback(AgsPitchSamplerFile *pitch_sampler_file,
						     AgsPitchSampler *pitch_sampler);

void ags_pitch_sampler_update_callback(GtkButton *button, AgsPitchSampler *pitch_sampler);

void ags_pitch_sampler_enable_lfo_callback(GtkToggleButton *toggle, AgsPitchSampler *pitch_sampler);

void ags_pitch_sampler_lfo_freq_callback(GtkSpinButton *spin_button, AgsPitchSampler *pitch_sampler);
void ags_pitch_sampler_lfo_phase_callback(GtkSpinButton *spin_button, AgsPitchSampler *pitch_sampler);
void ags_pitch_sampler_lfo_depth_callback(GtkSpinButton *spin_button, AgsPitchSampler *pitch_sampler);
void ags_pitch_sampler_lfo_tuning_callback(GtkSpinButton *spin_button, AgsPitchSampler *pitch_sampler);

void ags_pitch_sampler_enable_aliase_callback(GtkWidget *widget, AgsPitchSampler *pitch_sampler);

void ags_pitch_sampler_aliase_a_amount_callback(AgsDial *dial, AgsPitchSampler *pitch_sampler);
void ags_pitch_sampler_aliase_a_phase_callback(AgsDial *dial, AgsPitchSampler *pitch_sampler);

void ags_pitch_sampler_aliase_b_amount_callback(AgsDial *dial, AgsPitchSampler *pitch_sampler);
void ags_pitch_sampler_aliase_b_phase_callback(AgsDial *dial, AgsPitchSampler *pitch_sampler);

void ags_pitch_sampler_volume_callback(GtkRange *range, AgsPitchSampler *pitch_sampler);

G_END_DECLS

#endif /*__AGS_PITCH_SAMPLER_CALLBACKS_H__ */
