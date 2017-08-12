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

#ifndef __AGS_SYNCSYNTH_CALLBACKS_H__ 
#define __AGS_SYNCSYNTH_CALLBACKS_H__ 

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/X/machine/ags_syncsynth.h>

void ags_syncsynth_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsSyncsynth *syncsynth);

void ags_syncsynth_lower_callback(GtkSpinButton *spin_button, AgsSyncsynth *syncsynth);

void ags_syncsynth_auto_update_callback(GtkToggleButton *toggle, AgsSyncsynth *syncsynth);
void ags_syncsynth_update_callback(GtkButton *button, AgsSyncsynth *syncsynth);

void ags_syncsynth_add_callback(GtkButton *button, AgsSyncsynth *syncsynth);
void ags_syncsynth_remove_callback(GtkButton *button, AgsSyncsynth *syncsynth);

void ags_syncsynth_oscillator_control_changed_callback(AgsOscillator *oscillator,
						       AgsSyncsynth *syncsynth);

#endif /*__AGS_SYNCSYNTH_CALLBACKS_H__ */
