/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/libags.h>

#include <ags/audio/ags_acceleration.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUTOMATION                (ags_automation_get_type())
#define AGS_AUTOMATION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUTOMATION, AgsAutomation))
#define AGS_AUTOMATION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUTOMATION, AgsAutomationClass))
#define AGS_IS_AUTOMATION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUTOMATION))
#define AGS_IS_AUTOMATION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUTOMATION))
#define AGS_AUTOMATION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_AUTOMATION, AgsAutomationClass))

#define AGS_AUTOMATION_GET_OBJ_MUTEX(obj) (&(((AgsAutomation *) obj)->obj_mutex))

#define AGS_AUTOMATION_DEFAULT_BPM (120.0)

#define AGS_AUTOMATION_TICS_PER_BEAT (1.0)
#define AGS_AUTOMATION_MINIMUM_ACCELERATION_LENGTH (1.0 / 16.0 / 64.0)
#define AGS_AUTOMATION_MAXIMUM_ACCELERATION_LENGTH (16.0)

#define AGS_AUTOMATION_DEFAULT_LENGTH (64 * 16 * 1200 / AGS_AUTOMATION_TICS_PER_BEAT)
#define AGS_AUTOMATION_DEFAULT_JIFFIE (60.0 / AGS_AUTOMATION_DEFAULT_BPM / AGS_AUTOMATION_TICS_PER_BEAT)
#define AGS_AUTOMATION_DEFAULT_DURATION (AGS_AUTOMATION_DEFAULT_LENGTH * AGS_AUTOMATION_DEFAULT_JIFFIE * AGS_USEC_PER_SEC)
#define AGS_AUTOMATION_DEFAULT_OFFSET (64 * (1 / AGS_AUTOMATION_MINIMUM_ACCELERATION_LENGTH))

#define AGS_AUTOMATION_DEFAULT_PRECISION (8)
#define AGS_AUTOMATION_MAXIMUM_STEPS (128)

#define AGS_AUTOMATION_CLIPBOARD_VERSION "1.3.0"
#define AGS_AUTOMATION_CLIPBOARD_TYPE "AgsAutomationClipboardXml"
#define AGS_AUTOMATION_CLIPBOARD_FORMAT "AgsAutomationNativeScale"
#define AGS_AUTOMATION_CLIPBOARD_LEGACY_FORMAT "AgsAutomationNativePiano"

typedef struct _AgsAutomation AgsAutomation;
typedef struct _AgsAutomationClass AgsAutomationClass;

/**
 * AgsAutomationFlags:
 * @AGS_AUTOMATION_BYPASS: ignore any automation data
 * 
 * Enum values to control the behavior or indicate internal state of #AgsAutomation by
 * enable/disable as flags.
 */
typedef enum{
  AGS_AUTOMATION_BYPASS               = 1,
}AgsAutomationFlags;

struct _AgsAutomation
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;

  GObject *audio;
  GType channel_type;
  guint line;

  AgsTimestamp *timestamp;

  gchar *control_name;

  guint steps;
  gdouble upper;
  gdouble lower;
  gdouble default_value;

  AgsFunction *source_function;

  GObject *port;
  
  GList *acceleration;  
  GList *selection;
};

struct _AgsAutomationClass
{
  GObjectClass gobject;
};

GType ags_automation_get_type(void);
GType ags_automation_flags_get_type(void);

GRecMutex* ags_automation_get_obj_mutex(AgsAutomation *automation);

gboolean ags_automation_test_flags(AgsAutomation *automation, guint flags);
void ags_automation_set_flags(AgsAutomation *automation, guint flags);
void ags_automation_unset_flags(AgsAutomation *automation, guint flags);

GList* ags_automation_find_port(GList *automation,
				GObject *port);

GList* ags_automation_find_near_timestamp(GList *automation, guint line,
					  AgsTimestamp *timestamp);
GList* ags_automation_find_near_timestamp_extended(GList *automation, guint line,
						   GType channel_type, gchar *control_name,
						   AgsTimestamp *timestamp);

gint ags_automation_sort_func(gconstpointer a,
			      gconstpointer b);

GList* ags_automation_add(GList *automation,
			  AgsAutomation *new_automation);

GObject* ags_automation_get_audio(AgsAutomation *automation);
void ags_automation_set_audio(AgsAutomation *automation,
			      GObject *audio);

GType ags_automation_get_channel_type(AgsAutomation *automation);
void ags_automation_set_channel_type(AgsAutomation *automation,
				     GType channel_type);

guint ags_automation_get_line(AgsAutomation *automation);
void ags_automation_set_line(AgsAutomation *automation,
			     guint line);

AgsTimestamp* ags_automation_get_timestamp(AgsAutomation *automation);
void ags_automation_set_timestamp(AgsAutomation *automation,
				  AgsTimestamp *timestamp);

gchar* ags_automation_get_control_name(AgsAutomation *automation);
void ags_automation_set_control_name(AgsAutomation *automation,
				     gchar *control_name);

guint ags_automation_get_steps(AgsAutomation *automation);
void ags_automation_set_steps(AgsAutomation *automation,
			      guint steps);

gdouble ags_automation_get_upper(AgsAutomation *automation);
void ags_automation_set_upper(AgsAutomation *automation,
			      gdouble upper);

gdouble ags_automation_get_lower(AgsAutomation *automation);
void ags_automation_set_lower(AgsAutomation *automation,
			      gdouble lower);

gdouble ags_automation_get_default_value(AgsAutomation *automation);
void ags_automation_set_default_value(AgsAutomation *automation,
				      gdouble default_value);

GObject* ags_automation_get_port(AgsAutomation *automation);
void ags_automation_set_port(AgsAutomation *automation,
			     GObject *port);

GList* ags_automation_get_acceleration(AgsAutomation *automation);
void ags_automation_set_acceleration(AgsAutomation *automation,
				     GList *acceleration);

void ags_automation_add_acceleration(AgsAutomation *automation,
				     AgsAcceleration *acceleration,
				     gboolean use_selection_list);
void ags_automation_remove_acceleration(AgsAutomation *automation,
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
void ags_automation_free_all_selection(GList *automation);

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

void ags_automation_insert_from_clipboard(AgsAutomation *automation,
					  xmlNode *automation_node,
					  gboolean reset_x_offset, guint x_offset,
					  gboolean reset_y_offset, gdouble y_offset);

void ags_automation_insert_from_clipboard_extended(AgsAutomation *automation,
						   xmlNode *automation_node,
						   gboolean reset_x_offset, guint x_offset,
						   gboolean reset_y_offset, gdouble y_offset,
						   gboolean match_line, gboolean no_duplicates);

gchar** ags_automation_get_specifier_unique(GList *automation);
gchar** ags_automation_get_specifier_unique_with_channel_type(GList *automation,
							      GType channel_type);
GList* ags_automation_find_specifier(GList *automation,
				     gchar *specifier);
GList* ags_automation_find_channel_type_with_control_name(GList *automation,
							  GType channel_type, gchar *specifier);
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

G_END_DECLS

#endif /*__AGS_AUTOMATION_H__*/
