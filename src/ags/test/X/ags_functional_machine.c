/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/test/X/ags_functional_machine_util.h>

#include <ags/test/X/ags_functional_menu_bar_util.h>

gboolean
ags_functional_machine_util_context_menu_activate_path(AgsMachine *machine,
						       gchar *path)
{
  GtkMenu *context_menu;

  gboolean success;

  if(machine == NULL){
    return(FALSE);
  }

  gtk_test_widget_click(GTK_WIDGET(machine),
			2,
			GDK_BUTTON2_MASK);
  context_menu = machine->popup;
  
  success = ags_functional_menu_bar_util_activate_menu_path(context_menu,
							    path);

  return(success);
}

