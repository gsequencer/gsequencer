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

#ifndef __AGS_AUTOMATION_AREA_CALLBACKS_H__
#define __AGS_AUTOMATION_AREA_CALLBACKS_H__

#include <ags/X/editor/ags_automation_area.h>

gboolean ags_automation_area_expose_event(GtkWidget *widget, GdkEventExpose *event,
					  AgsAutomationArea *automation_area);
gboolean ags_automation_area_configure_event(GtkWidget *widget, GdkEventConfigure *event,
					     AgsAutomationArea *automation_area);
gboolean ags_automation_area_button_press_event(GtkWidget *widget, GdkEventButton *event,
						AgsAutomationArea *automation_area);
gboolean ags_automation_area_button_release_event(GtkWidget *widget, GdkEventButton *event,
						  AgsAutomationArea *automation_area);
gboolean ags_automation_area_motion_notify_event(GtkWidget *widget, GdkEventMotion *event,
						 AgsAutomationArea *automation_area);

#endif /*__AGS_AUTOMATION_AREA_CALLBACKS_H__*/
