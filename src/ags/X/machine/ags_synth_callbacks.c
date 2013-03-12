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
  synth->machine.name = g_strdup_printf("Default %d", window->counter->synth);
  window->counter->synth++;
}

void
ags_synth_lower_callback(GtkSpinButton *spin_button, AgsSynth *synth)
{
}

void
ags_synth_auto_update_callback(GtkToggleButton *toggle, AgsSynth *synth)
{
  GList *list;

  list = gtk_container_get_children((GtkContainer *) synth->oscillator->menu);

  if(toggle->active)
    while(list != NULL){
      ags_oscillator_connect(AGS_CONNECTABLE(list->data));

      list = list->next;
    }
  else
    while(list != NULL){
      ags_oscillator_disconnect(AGS_CONNECTABLE(list->data));

      list = list->next;
    }
}

void
ags_synth_update_callback(GtkButton *button, AgsSynth *synth)
{
  ags_synth_update(synth);
}
