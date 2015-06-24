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

#ifndef __AGS_PAD_CALLBACKS_H__
#define __AGS_PAD_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_pad.h>

int ags_pad_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsPad *pad);
int ags_pad_destroy_callback(GtkObject *object, AgsPad *pad);
int ags_pad_show_callback(GtkWidget *widget, AgsPad *pad);

int ags_pad_option_changed_callback(GtkWidget *widget, AgsPad *pad);

int ags_pad_group_clicked_callback(GtkWidget *widget, AgsPad *pad);
int ags_pad_mute_clicked_callback(GtkWidget *widget, AgsPad *pad);
int ags_pad_solo_clicked_callback(GtkWidget *widget, AgsPad *pad);

#endif /*__AGS_PAD_CALLBACKS_H__*/
