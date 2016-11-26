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

#ifndef __AGS_LV2UI_MANAGER_H__
#define __AGS_LV2UI_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_turtle.h>

#include <ags/plugin/ags_lv2ui_plugin.h>

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

typedef struct _AgsLv2uiManager AgsLv2uiManager;
typedef struct _AgsLv2uiManagerClass AgsLv2uiManagerClass;

struct _AgsLv2uiManager
{
  GObject gobject;

  GList *lv2ui_plugin_blacklist;
  GList *lv2ui_plugin;
};

struct _AgsLv2uiManagerClass
{
  GObjectClass gobject;
};

GType ags_lv2ui_manager_get_type(void);

gchar** ags_lv2ui_manager_get_filenames(AgsLv2uiManager *lv2ui_manager);
AgsLv2uiPlugin* ags_lv2ui_manager_find_lv2ui_plugin(AgsLv2uiManager *lv2ui_manager,
						    gchar *filename,
						    gchar *effect);

void ags_lv2ui_manager_load_blacklist(AgsLv2uiManager *lv2ui_manager,
				      gchar *blacklist_filename);

void ags_lv2ui_manager_load_file(AgsLv2uiManager *lv2ui_manager,
				 AgsTurtle *manifest,
				 AgsTurtle *turtle,
				 gchar *lv2ui_path,
				 gchar *filename);
void ags_lv2ui_manager_load_default_directory(AgsLv2uiManager *lv2ui_manager);

/*  */
AgsLv2uiManager* ags_lv2ui_manager_get_instance();

AgsLv2uiManager* ags_lv2ui_manager_new();

#endif /*__AGS_LV2UI_MANAGER_H__*/
