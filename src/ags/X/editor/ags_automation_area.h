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

#ifndef __AGS_AUTOMATION_AREA_H__
#define __AGS_AUTOMATION_AREA_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_automation.h>

#define AGS_TYPE_AUTOMATION_AREA                (ags_automation_area_get_type())
#define AGS_AUTOMATION_AREA(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUTOMATION_AREA, AgsAutomationArea))
#define AGS_AUTOMATION_AREA_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUTOMATION_AREA, AgsAutomationAreaClass))
#define AGS_IS_AUTOMATION_AREA(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUTOMATION_AREA))
#define AGS_IS_AUTOMATION_AREA_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUTOMATION_AREA))
#define AGS_AUTOMATION_AREA_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_AUTOMATION_AREA, AgsAutomationAreaClass))

#define AGS_AUTOMATION_AREA_MAX_CONTROLS 1200

typedef struct _AgsAutomationArea AgsAutomationArea;
typedef struct _AgsAutomationAreaClass AgsAutomationAreaClass;

struct _AgsAutomationArea
{
  GtkDrawingArea drawing_area;

  AgsAutomation *automation;
};

struct _AgsAutomationAreaClass
{
  GtkDrawingAreaClass drawing_area;
};

GType ags_automation_area_get_type(void);

void ags_automation_area_draw_strip(AgsAutomationArea *automation_area, cairo_t *cr,
				    AgsChannel *channel, gchar *control_name);
void ags_automation_area_draw_scale(AgsAutomationArea *automation_area, cairo_t *cr,
				    gdouble lower, gdouble upper);
void ags_automation_area_draw_automation(AgsAutomationArea *automation_area, cairo_t *cr,
					 gdouble x0, gdouble x1);
void ags_automation_area_draw_surface(AgsAutomationArea *automation_area, cairo_t *cr,
				      gdouble x0, gdouble y0,
				      gdouble x1, gdouble y1);

AgsAutomationArea* ags_automation_area_new();

#endif /*__AGS_AUTOMATION_AREA_H__*/
