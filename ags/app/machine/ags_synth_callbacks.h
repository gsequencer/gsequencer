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

#ifndef __AGS_SYNTH_CALLBACKS_H__ 
#define __AGS_SYNTH_CALLBACKS_H__ 

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/machine/ags_synth.h>

G_BEGIN_DECLS

void ags_synth_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsSynth *synth);

void ags_synth_auto_update_callback(GtkToggleButton *toggle, AgsSynth *synth);
void ags_synth_update_callback(GtkButton *button, AgsSynth *synth);

void ags_synth_lower_callback(GtkSpinButton *spin_button, AgsSynth *synth);

G_END_DECLS

#endif /*__AGS_SYNTH_CALLBACKS_H__ */