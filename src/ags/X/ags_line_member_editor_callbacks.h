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

#ifndef __AGS_LINE_MEMBER_EDITOR_CALLBACKS_H__
#define __AGS_LINE_MEMBER_EDITOR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/X/ags_line_member_editor.h>

void ags_line_member_editor_add_callback(GtkWidget *button,
					 AgsLineMemberEditor *line_member_editor);
void ags_line_member_editor_remove_callback(GtkWidget *button,
					    AgsLineMemberEditor *line_member_editor);

void ags_line_member_editor_ladspa_browser_response_callback(GtkDialog *dialog,
							     gint response,
							     AgsLineMemberEditor *line_member_editor);

#endif /*__AGS_LINE_MEMBER_EDITOR_CALLBACKS_H__*/
