/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#ifndef __AGS_PORT_SELECTION_CALLBACKS_H__
#define __AGS_PORT_SELECTION_CALLBACKS_H__

#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_port_editor.h>

#include <ags/X/editor/ags_port_selection.h>

void ags_port_selection_ports_toggled_callback(GtkWidget *menu_item,
					       AgsPortSelection *selection);

#endif /*__AGS_PORT_SELECTION_CALLBACKS_H__*/
