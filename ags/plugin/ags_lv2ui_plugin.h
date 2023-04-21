/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/libags.h>

#include <ags/plugin/ags_base_plugin.h>

#include <lv2.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>

G_BEGIN_DECLS

#define AGS_TYPE_LV2UI_PLUGIN                (ags_lv2ui_plugin_get_type())
#define AGS_LV2UI_PLUGIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LV2UI_PLUGIN, AgsLv2uiPlugin))
#define AGS_LV2UI_PLUGIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LV2UI_PLUGIN, AgsLv2uiPluginClass))
#define AGS_IS_LV2UI_PLUGIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LV2UI_PLUGIN))
#define AGS_IS_LV2UI_PLUGIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LV2UI_PLUGIN))
#define AGS_LV2UI_PLUGIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LV2UI_PLUGIN, AgsLv2uiPluginClass))

#define AGS_LV2UI_PLUGIN_DESCRIPTOR(ptr) ((LV2UI_Descriptor *)(ptr))

typedef struct _AgsLv2uiPlugin AgsLv2uiPlugin;
typedef struct _AgsLv2uiPluginClass AgsLv2uiPluginClass;

/**
 * AgsLv2uiPluginFlags:
 * @AGS_LV2UI_PLUGIN_IS_SYNTHESIZER: is synthesizer
 * @AGS_LV2UI_PLUGIN_GTK2: has Gtk+-2.0 UI
 * @AGS_LV2UI_PLUGIN_GTK3: has Gtk3 UI
 * @AGS_LV2UI_PLUGIN_QT4: has Qt4 UI
 * @AGS_LV2UI_PLUGIN_QT5: has Qt5 UI
 * 
 * Enum values to control the behavior or indicate internal state of #AgsLv2uiPlugin by
 * enable/disable as flags.
 */
typedef enum{
  AGS_LV2UI_PLUGIN_IS_SYNTHESIZER  = 1,
  AGS_LV2UI_PLUGIN_GTK2            = 1 <<  1,
  AGS_LV2UI_PLUGIN_GTK3            = 1 <<  2,
  AGS_LV2UI_PLUGIN_QT4             = 1 <<  3,
  AGS_LV2UI_PLUGIN_QT5             = 1 <<  4,
}AgsLv2uiPluginFlags;

struct _AgsLv2uiPlugin
{
  AgsBasePlugin base_plugin;

  guint flags;

  gchar *gui_uri;

  AgsTurtle *manifest;
  AgsTurtle *gui_turtle;

  GObject *lv2_plugin;

  LV2_Feature **feature;
};

struct _AgsLv2uiPluginClass
{
  AgsBasePluginClass base_plugin;
};

GType ags_lv2ui_plugin_get_type(void);
GType ags_lv2ui_plugin_flags_get_type();

gboolean ags_lv2ui_plugin_test_flags(AgsLv2uiPlugin *lv2ui_plugin, AgsLv2uiPluginFlags flags);
void ags_lv2ui_plugin_set_flags(AgsLv2uiPlugin *lv2ui_plugin, AgsLv2uiPluginFlags flags);
void ags_lv2ui_plugin_unset_flags(AgsLv2uiPlugin *lv2ui_plugin, AgsLv2uiPluginFlags flags);

GList* ags_lv2ui_plugin_find_gui_uri(GList *lv2ui_plugin,
				     gchar *gui_uri);

AgsLv2uiPlugin* ags_lv2ui_plugin_new(AgsTurtle *gui_turtle,
				     gchar *filename,
				     gchar *effect,
				     gchar *gui_uri,
				     guint effect_index);

G_END_DECLS

#endif /*__AGS_LV2UI_PLUGIN_H__*/
