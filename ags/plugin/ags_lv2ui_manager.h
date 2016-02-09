/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#ifndef __AGS_LV2UI_MANAGER_H__
#define __AGS_LV2UI_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_turtle.h>

#include <lv2.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>

#include <math.h>
#include <stdlib.h>

#define AGS_TYPE_LV2UI_MANAGER                (ags_lv2ui_manager_get_type())
#define AGS_LV2UI_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LV2UI_MANAGER, AgsLv2uiManager))
#define AGS_LV2UI_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LV2UI_MANAGER, AgsLv2uiManagerClass))
#define AGS_IS_LV2UI_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LV2UI_MANAGER))
#define AGS_IS_LV2UI_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LV2UI_MANAGER))
#define AGS_LV2UI_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LV2UI_MANAGER, AgsLv2uiManagerClass))

#define AGS_LV2UI_PLUGIN(ptr) ((AgsLv2uiPlugin *)(ptr))

typedef struct _AgsLv2uiManager AgsLv2uiManager;
typedef struct _AgsLv2uiManagerClass AgsLv2uiManagerClass;
typedef struct _AgsLv2uiPlugin AgsLv2uiPlugin;

typedef enum{
  AGS_LV2UI_PLUGIN_IS_LOADED    = 1,
}AgsLv2uiPluginFlags;

struct _AgsLv2uiManager
{
  GObject object;

  GList *lv2ui_plugin;
};

struct _AgsLv2uiManagerClass
{
  GObjectClass object;
};

struct _AgsLv2uiPlugin
{
  guint flags;

  AgsTurtle *turtle;

  gchar *filename;
  void *plugin_so;
};

GType ags_lv2ui_manager_get_type(void);

AgsLv2uiPlugin* ags_lv2ui_plugin_alloc();
void ags_lv2ui_plugin_free(AgsLv2uiPlugin *lv2ui_plugin);

gchar** ags_lv2ui_manager_get_filenames();
AgsLv2uiPlugin* ags_lv2ui_manager_find_lv2ui_plugin(gchar *filename);

void ags_lv2ui_manager_load_file(AgsTurtle *turtle,
				 gchar *filename);
void ags_lv2ui_manager_load_default_directory();

uint32_t ags_lv2ui_manager_uri_index(gchar *filename,
				     gchar *uri);

/*  */
AgsLv2uiManager* ags_lv2ui_manager_get_instance();

AgsLv2uiManager* ags_lv2ui_manager_new();

#endif /*__AGS_LV2UI_MANAGER_H__*/
