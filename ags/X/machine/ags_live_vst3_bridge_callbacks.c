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

#include <ags/X/machine/ags_live_vst3_bridge_callbacks.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_bulk.h>
#include <ags/X/ags_bulk_member.h>

void
ags_live_vst3_bridge_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsLiveVst3Bridge *live_vst3_bridge)
{
  AgsWindow *window;

  gchar *str;

  if(old_parent != NULL){
    return;
  }

  window = AGS_WINDOW(gtk_widget_get_toplevel(widget));

  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_LIVE_VST3_BRIDGE)->counter);

  g_object_set(AGS_MACHINE(live_vst3_bridge),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_LIVE_VST3_BRIDGE);

  g_free(str);
}

void
ags_live_vst3_bridge_program_changed_callback(GtkComboBox *combo_box, AgsLiveVst3Bridge *live_vst3_bridge)
{
  //TODO:JK: implement me
}
