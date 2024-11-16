/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#ifndef __AGS_LINE_MEMBER_CALLBACKS_H__
#define __AGS_LINE_MEMBER_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_line_member.h>

G_BEGIN_DECLS

void ags_line_member_dial_changed_callback(GtkWidget *dial, AgsLineMember *line_member);
void ags_line_member_scale_changed_callback(GtkWidget *scale, AgsLineMember *line_member);
void ags_line_member_spin_button_changed_callback(GtkWidget *spin_button, AgsLineMember *line_member);
void ags_line_member_check_button_toggled_callback(GObject *gobject,
						   GParamSpec *pspec,
						   AgsLineMember *line_member);
void ags_line_member_toggle_button_toggled_callback(GObject *gobject,
						    GParamSpec *pspec,
						    AgsLineMember *line_member);
void ags_line_member_button_clicked_callback(GtkWidget *button, AgsLineMember *line_member);

G_END_DECLS

#endif /*__AGS_LINE_MEMBER_CALLBACKS_H__*/
