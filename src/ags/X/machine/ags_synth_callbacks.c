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

#include <ags/X/machine/ags_synth_callbacks.h>
#include <ags/X/machine/ags_oscillator.h>

#include <ags/object/ags_connectable.h>

#include <ags/X/ags_window.h>

#include <math.h>

void
ags_synth_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsSynth *synth)
{
  AgsWindow *window;

  if(old_parent != NULL)
    return;

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);

  AGS_MACHINE(synth)->name = g_strdup_printf("Default %d\0",
					     ags_window_find_machine_counter(window, AGS_TYPE_SYNTH)->counter);
  ags_window_increment_machine_counter(window,
				       AGS_TYPE_SYNTH);
}

void
ags_synth_lower_callback(GtkSpinButton *spin_button, AgsSynth *synth)
{
}

void
ags_synth_auto_update_callback(GtkToggleButton *toggle, AgsSynth *synth)
{
  if(gtk_toggle_button_get_active(toggle)){
    synth->flags |= AGS_SYNTH_AUTO_UPDATE;
  }else{
    synth->flags &= (~AGS_SYNTH_AUTO_UPDATE);
  }
}

void
ags_synth_update_callback(GtkButton *button, AgsSynth *synth)
{
  ags_synth_update(synth);
}
