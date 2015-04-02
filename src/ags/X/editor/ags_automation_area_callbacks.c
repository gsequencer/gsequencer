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

#include <ags/X/editor/ags_automation_area_callbacks.h>

#include <ags/X/ags_automation_editor_callbacks.h>

gboolean
ags_automation_area_expose_event(GtkWidget *widget, GdkEventExpose *event,
				 AgsAutomationArea *automation_area)
{
  AgsAutomationEditor *automation_editor;
  cairo_t *cr;

  g_message("debug: c");
  
  automation_editor = gtk_widget_get_ancestor(automation_area,
					      AGS_TYPE_AUTOMATION_EDITOR);

  cr = gdk_cairo_create(widget->window);
  cairo_push_group(cr);

  ags_automation_area_draw_strip(automation_area,
				 cr);
  ags_automation_area_draw_scale(automation_area,
				 cr);
  ags_automation_area_draw_automation(automation_area,
				      cr);

  cairo_pop_group_to_source(cr);
  cairo_paint(cr);

  return(TRUE);
}

gboolean
ags_automation_area_configure_event(GtkWidget *widget, GdkEventConfigure *event,
				    AgsAutomationArea *automation_area)
{
  //TODO:JK: implement me

  return(FALSE);
}

gboolean
ags_automation_area_button_press_event(GtkWidget *widget, GdkEventButton *event,
				       AgsAutomationArea *automation_area)
{
  //TODO:JK: implement me

  return(TRUE);
}

gboolean
ags_automation_area_button_release_event(GtkWidget *widget, GdkEventButton *event,
					 AgsAutomationArea *automation_area)
{
  //TODO:JK: implement me

  return(FALSE);
}

gboolean
ags_automation_area_motion_notify_event(GtkWidget *widget, GdkEventMotion *event,
					AgsAutomationArea *automation_area)
{
  //TODO:JK: implement me

  return(FALSE);
}

