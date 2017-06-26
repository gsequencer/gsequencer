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

#include <ags/X/editor/ags_machine_radio_button_callbacks.h>

void
ags_machine_radio_button_notify_machine_name_callback(GObject *machine, GParamSpec *pspec,
						      AgsMachineRadioButton *machine_radio_button)
{
  gchar *str;

  str = g_strdup_printf("%s: %s", G_OBJECT_TYPE_NAME(machine), AGS_MACHINE(machine)->machine_name);	
  g_object_set(machine_radio_button,
	       "label", str,
	       NULL);

  g_free(str);
}
