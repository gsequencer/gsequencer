/* This file is part of GSequencer.
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

#ifndef __AGS_LINE_MEMBER_CALLBACKS_H__
#define __AGS_LINE_MEMBER_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/X/ags_line_member.h>

int ags_line_member_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsLineMember *line_member);

void ags_line_member_dial_changed_callback(GtkWidget *dial, AgsLineMember *line_member);
void ags_line_member_vscale_changed_callback(GtkWidget *vscale, AgsLineMember *line_member);
void ags_line_member_hscale_changed_callback(GtkWidget *hscale, AgsLineMember *line_member);
void ags_line_member_spin_button_changed_callback(GtkWidget *spin_button, AgsLineMember *line_member);
void ags_line_member_check_button_clicked_callback(GtkWidget *check_button, AgsLineMember *line_member);
void ags_line_member_toggle_button_clicked_callback(GtkWidget *toggle_button, AgsLineMember *line_member);
void ags_line_member_button_clicked_callback(GtkWidget *button, AgsLineMember *line_member);

void ags_line_member_port_safe_write_callback(AgsPort *port, GValue *value,
					      AgsLineMember *line_member);

#endif /*__AGS_LINE_MEMBER_CALLBACKS_H__*/
