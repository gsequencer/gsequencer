/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/GSequencer/machine/ags_fm_synth_callbacks.h>
#include <ags/GSequencer/machine/ags_fm_oscillator.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/GSequencer/ags_window.h>

#include <math.h>

void
ags_fm_synth_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsFMSynth *fm_synth)
{
  AgsWindow *window;

  gchar *str;

  if(old_parent != NULL){
    return;
  }

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);

  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_FM_SYNTH)->counter);

  g_object_set(AGS_MACHINE(fm_synth),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_FM_SYNTH);

  g_free(str);
}

void
ags_fm_synth_auto_update_callback(GtkToggleButton *toggle, AgsFMSynth *fm_synth)
{
  if(gtk_toggle_button_get_active(toggle)){
    ags_fm_synth_set_flags(fm_synth, AGS_FM_SYNTH_AUTO_UPDATE);
  }else{
    ags_fm_synth_unset_flags(fm_synth, AGS_FM_SYNTH_AUTO_UPDATE);
  }
}

void
ags_fm_synth_update_callback(GtkButton *button, AgsFMSynth *fm_synth)
{
  ags_fm_synth_update(fm_synth);
}

void
ags_fm_synth_lower_callback(GtkSpinButton *spin_button, AgsFMSynth *fm_synth)
{
  if(ags_fm_synth_test_flags(fm_synth, AGS_FM_SYNTH_AUTO_UPDATE)){
    ags_fm_synth_update(fm_synth);
  }
}
