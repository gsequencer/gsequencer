/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#ifndef __AGS_PERFORMANCE_PREFERENCES_H__
#define __AGS_PERFORMANCE_PREFERENCES_H__

#include <gtk/gtk.h>

#define AGS_TYPE_PERFORMANCE_PREFERENCES                (ags_performance_preferences_get_type())
#define AGS_PERFORMANCE_PREFERENCES(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PERFORMANCE_PREFERENCES, AgsPerformancePreferences))
#define AGS_PERFORMANCE_PREFERENCES_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PERFORMANCE_PREFERENCES, AgsPerformancePreferencesClass))
#define AGS_IS_PERFORMANCE_PREFERENCES(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PERFORMANCE_PREFERENCES))
#define AGS_IS_PERFORMANCE_PREFERENCES_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PERFORMANCE_PREFERENCES))
#define AGS_PERFORMANCE_PREFERENCES_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PERFORMANCE_PREFERENCES, AgsPerformancePreferencesClass))

typedef struct _AgsPerformancePreferences AgsPerformancePreferences;
typedef struct _AgsPerformancePreferencesClass AgsPerformancePreferencesClass;

struct _AgsPerformancePreferences
{
  GtkVBox vbox;

  GtkCheckButton *stream_auto_sense;
  GtkCheckButton *super_threaded;
};

struct _AgsPerformancePreferencesClass
{
  GtkVBoxClass vbox;
};

GType ags_performance_preferences_get_type(void);

AgsPerformancePreferences* ags_performance_preferences_new();

#endif /*__AGS_PERFORMANCE_PREFERENCES_H__*/
