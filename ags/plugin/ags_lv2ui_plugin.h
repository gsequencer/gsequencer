/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_LV2UI_PLUGIN_H__
#define __AGS_LV2UI_PLUGIN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/plugin/ags_base_plugin.h>
#include <ags/lib/ags_turtle.h>

#include <lv2.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>

#define AGS_TYPE_LV2UI_PLUGIN                (ags_lv2ui_plugin_get_type())
#define AGS_LV2UI_PLUGIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LV2UI_PLUGIN, AgsLv2uiPlugin))
#define AGS_LV2UI_PLUGIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LV2UI_PLUGIN, AgsLv2uiPluginClass))
#define AGS_IS_LV2UI_PLUGIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LV2UI_PLUGIN))
#define AGS_IS_LV2UI_PLUGIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LV2UI_PLUGIN))
#define AGS_LV2UI_PLUGIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LV2UI_PLUGIN, AgsLv2uiPluginClass))

typedef struct _AgsLv2uiPlugin AgsLv2uiPlugin;
typedef struct _AgsLv2uiPluginClass AgsLv2uiPluginClass;

typedef enum{
  AGS_LV2UI_PLUGIN_IS_SYNTHESIZER  = 1,
  AGS_LV2UI_GTK2                   = 1 <<  1,
  AGS_LV2UI_GTK3                   = 1 <<  2,
  AGS_LV2UI_QT4                    = 1 <<  3,
  AGS_LV2UI_QT5                    = 1 <<  4,
}AgsLv2uiFlags;

struct _AgsLv2uiPlugin
{
  AgsBasePlugin base_plugin;

  guint flags;

  gchar *gui_uri;

  AgsTurtle *manifest;
  AgsTurtle *gui_turtle;

  GObject *lv2_plugin;
};

struct _AgsLv2uiPluginClass
{
  AgsBasePluginClass base_plugin;
};

GType ags_lv2ui_plugin_get_type(void);

GList* ags_lv2ui_plugin_find_gui_uri(GList *lv2ui_plugin,
				     gchar *gui_uri);

AgsLv2uiPlugin* ags_lv2ui_plugin_new(AgsTurtle *gui_turtle, gchar *filename, gchar *effect, gchar *gui_uri, guint effect_index);

#endif /*__AGS_LV2UI_PLUGIN_H__*/
