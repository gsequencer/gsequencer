/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_AUTOMATION_H__
#define __AGS_AUTOMATION_H__

#include <glib.h>
#include <glib-object.h>
#include <libxml/tree.h>

#include <ags/lib/ags_function.h>

#include <ags/audio/ags_acceleration.h>

#define AGS_TYPE_AUTOMATION                (ags_automation_get_type())
#define AGS_AUTOMATION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUTOMATION, AgsAutomation))
#define AGS_AUTOMATION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUTOMATION, AgsAutomationClass))
#define AGS_IS_AUTOMATION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUTOMATION))
#define AGS_IS_AUTOMATION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUTOMATION))
#define AGS_AUTOMATION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_AUTOMATION, AgsAutomationClass))

#define AGS_AUTOMATION_DEFAULT_BPM (120.0)

#define AGS_AUTOMATION_TICS_PER_BEAT (1.0)
#define AGS_AUTOMATION_MINIMUM_ACCELERATION_LENGTH (1.0 / 16.0 / 64.0)

#define AGS_AUTOMATION_DEFAULT_LENGTH (64 * 16 * 1200 / AGS_AUTOMATION_TICS_PER_BEAT)
#define AGS_AUTOMATION_DEFAULT_JIFFIE (60.0 / AGS_AUTOMATION_DEFAULT_BPM / AGS_AUTOMATION_TICS_PER_BEAT)
#define AGS_AUTOMATION_DEFAULT_DURATION (AGS_AUTOMATION_DEFAULT_LENGTH * AGS_AUTOMATION_DEFAULT_JIFFIE * AGS_MICROSECONDS_PER_SECOND)

#define AGS_AUTOMATION_DEFAULT_PRECISION (8)
#define AGS_AUTOMATION_MAXIMUM_STEPS (128)

typedef struct _AgsAutomation AgsAutomation;
typedef struct _AgsAutomationClass AgsAutomationClass;

typedef enum{
  AGS_AUTOMATION_CONNECTED            = 1,
  AGS_AUTOMATION_BYPASS               = 1 <<  1,
}AgsAutomationFlags;

struct _AgsAutomation
{
  GObject gobject;

  guint flags;

  GObject *timestamp;

  GObject *audio;
  guint line;
  GType channel_type;
  gchar *control_name;

  guint steps;
  gdouble upper;
  gdouble lower;
  gdouble default_value;

  AgsFunction *source_function;
  
  GList *acceleration;

  gdouble loop_start;
  gdouble loop_end;
  gdouble offset;
  
  GList *selection;

  GObject *port;

  GList *current_accelerations;
  GList *next_accelerations;
};

struct _AgsAutomationClass
{
  GObjectClass gobject;
};

GType ags_automation_get_type(void);

GList* ags_automation_find_port(GList *automation,
				GObject *port);

GList* ags_automation_find_near_timestamp(GList *automation, guint line,
					  GObject *timestamp);

void ags_automation_add_acceleration(AgsAutomation *automation,
				     AgsAcceleration *acceleration,
				     gboolean use_selection_list);

gboolean ags_automation_remove_acceleration_at_position(AgsAutomation *automation,
							guint x, gdouble y);

GList* ags_automation_get_selection(AgsAutomation *automation);

gboolean ags_automation_is_acceleration_selected(AgsAutomation *automation, AgsAcceleration *acceleration);

AgsAcceleration* ags_automation_find_point(AgsAutomation *automation,
					   guint x, gdouble y,
					   gboolean use_selection_list);
GList* ags_automation_find_region(AgsAutomation *automation,
				  guint x0, gdouble y0,
				  guint x1, gdouble y1,
				  gboolean use_selection_list);

void ags_automation_free_selection(AgsAutomation *automation);

void ags_automation_add_point_to_selection(AgsAutomation *automation,
					   guint x, gdouble y,
					   gboolean replace_current_selection);
void ags_automation_remove_point_from_selection(AgsAutomation *automation,
						guint x, gdouble y);

void ags_automation_add_region_to_selection(AgsAutomation *automation,
					    guint x0, gdouble y0,
					    guint x1, gdouble y1,
					    gboolean replace_current_selection);
void ags_automation_remove_region_from_selection(AgsAutomation *automation,
						 guint x0, gdouble y0,
						 guint x1, gdouble y1);

void ags_automation_add_all_to_selection(AgsAutomation *automation);

xmlNode* ags_automation_copy_selection(AgsAutomation *automation);
xmlNode* ags_automation_cut_selection(AgsAutomation *automation);
void ags_automation_merge_clipboard(xmlNode *audio_node,
				    xmlNode *automation_node);

void ags_automation_insert_from_clipboard(AgsAutomation *automation,
					  xmlNodePtr content,
					  gboolean reset_x_offset, guint x_offset,
					  gboolean reset_y_offset, gdouble y_offset);

GList* ags_automation_get_current(AgsAutomation *automation);

gchar** ags_automation_get_specifier_unique(GList *automation);
GList* ags_automation_find_specifier(GList *automation,
				     gchar *specifier);
GList* ags_automation_find_specifier_with_type_and_line(GList *automation,
							gchar *specifier,
							GType channel_type,
							guint line);

guint ags_automation_get_value(AgsAutomation *automation,
			       guint x, guint x_end,
			       gboolean use_prev_on_failure,
			       GValue *value);

AgsAutomation* ags_automation_new(GObject *audio,
				  guint line,
				  GType channel_type,
				  gchar *control_name);

#endif /*__AGS_AUTOMATION_H__*/
