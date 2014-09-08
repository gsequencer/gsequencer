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

#ifndef __AGS_PLUGIN_PREFERENCES_H__
#define __AGS_PLUGIN_PREFERENCES_H__

#include <gtk/gtk.h>

#define AGS_TYPE_PLUGIN_PREFERENCES                (ags_plugin_preferences_get_type())
#define AGS_PLUGIN_PREFERENCES(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLUGIN_PREFERENCES, AgsPluginPreferences))
#define AGS_PLUGIN_PREFERENCES_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLUGIN_PREFERENCES, AgsPluginPreferencesClass))
#define AGS_IS_PLUGIN_PREFERENCES(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PLUGIN_PREFERENCES))
#define AGS_IS_PLUGIN_PREFERENCES_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PLUGIN_PREFERENCES))
#define AGS_PLUGIN_PREFERENCES_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PLUGIN_PREFERENCES, AgsPluginPreferencesClass))

typedef struct _AgsPluginPreferences AgsPluginPreferences;
typedef struct _AgsPluginPreferencesClass AgsPluginPreferencesClass;

struct _AgsPluginPreferences
{
  GtkVBox vbox;

  GtkEntry *ladspa_path;
  GtkCellView *plugin_file;
};

struct _AgsPluginPreferencesClass
{
  GtkVBoxClass vbox;
};

GType ags_plugin_preferences_get_type(void);

AgsPluginPreferences* ags_plugin_preferences_new();

#endif /*__AGS_PLUGIN_PREFERENCES_H__*/
