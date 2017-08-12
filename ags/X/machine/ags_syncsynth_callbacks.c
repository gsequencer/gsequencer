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

#include <ags/X/machine/ags_syncsynth_callbacks.h>
#include <ags/X/machine/ags_oscillator.h>

#include <ags/object/ags_connectable.h>

#include <ags/X/ags_window.h>

#include <math.h>

void
ags_syncsynth_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsSyncsynth *syncsynth)
{
  AgsWindow *window;

  gchar *str;

  if(old_parent != NULL){
    return;
  }

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);

  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_SYNCSYNTH)->counter);

  g_object_set(AGS_MACHINE(syncsynth),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_SYNCSYNTH);

  g_free(str);
}

void
ags_syncsynth_lower_callback(GtkSpinButton *spin_button, AgsSyncsynth *syncsynth)
{
}

void
ags_syncsynth_auto_update_callback(GtkToggleButton *toggle, AgsSyncsynth *syncsynth)
{
  if(gtk_toggle_button_get_active(toggle)){
    syncsynth->flags |= AGS_SYNCSYNTH_AUTO_UPDATE;
  }else{
    syncsynth->flags &= (~AGS_SYNCSYNTH_AUTO_UPDATE);
  }
}

void
ags_syncsynth_update_callback(GtkButton *button, AgsSyncsynth *syncsynth)
{
  ags_syncsynth_update(syncsynth);
}
