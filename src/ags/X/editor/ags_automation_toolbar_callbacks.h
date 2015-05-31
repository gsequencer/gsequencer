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

#ifndef __AGS_AUTOMATION_TOOLBAR_CALLBACKS_H__
#define __AGS_AUTOMATION_TOOLBAR_CALLBACKS_H__

#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_automation_editor.h>

#include <ags/X/editor/ags_automation_toolbar.h>

void ags_automation_toolbar_machine_changed_callback(AgsAutomationEditor *automation_editor,
						     AgsMachine *machine,
						     AgsAutomationToolbar *toolbar);

void ags_automation_toolbar_port_changed_callback(GtkComboBox *combo_box,
						  AgsAutomationToolbar *automation_toolbar);

#endif /*__AGS_AUTOMATION_TOOLBAR_CALLBACKS_H__*/
