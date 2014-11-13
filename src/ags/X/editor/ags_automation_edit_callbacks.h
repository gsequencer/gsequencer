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

#ifndef __AGS_AUTOMATION_EDIT_CALLBACKS_H__
#define __AGS_AUTOMATION_EDIT_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/editor/ags_automation_edit.h>

gboolean ags_automation_edit_drawing_area_expose_event(GtkWidget *widget, GdkEventExpose *event, AgsAutomationEdit *automation_edit);
gboolean ags_automation_edit_drawing_area_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsAutomationEdit *automation_edit);
gboolean ags_automation_edit_drawing_area_button_press_event(GtkWidget *widget, GdkEventButton *event, AgsAutomationEdit *automation_edit);
gboolean ags_automation_edit_drawing_area_button_release_event(GtkWidget *widget, GdkEventButton *event, AgsAutomationEdit *automation_edit);
gboolean ags_automation_edit_drawing_area_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, AgsAutomationEdit *automation_edit);

void ags_automation_edit_vscrollbar_value_changed(GtkRange *range, AgsAutomationEdit *automation_edit);
void ags_automation_edit_hscrollbar_value_changed(GtkRange *range, AgsAutomationEdit *automation_edit);

#endif /*__AGS_AUTOMATION_EDIT_CALLBACKS_H__*/
