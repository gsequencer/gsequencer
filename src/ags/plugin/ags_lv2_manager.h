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

#ifndef __AGS_LV2_MANAGER_H__
#define __AGS_LV2_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_turtle.h>

#include <lv2.h>

#include <math.h>
#include <stdlib.h>

#define AGS_TYPE_LV2_MANAGER                (ags_lv2_manager_get_type())
#define AGS_LV2_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LV2_MANAGER, AgsLv2Manager))
#define AGS_LV2_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LV2_MANAGER, AgsLv2ManagerClass))
#define AGS_IS_LV2_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LV2_MANAGER))
#define AGS_IS_LV2_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LV2_MANAGER))
#define AGS_LV2_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LV2_MANAGER, AgsLv2ManagerClass))

#define AGS_LV2_PLUGIN(ptr) ((AgsLv2Plugin *)(ptr))

#define AGS_LV2_MANAGER_DEFAULT_LOCALE "en-gb\0"

typedef struct _AgsLv2Manager AgsLv2Manager;
typedef struct _AgsLv2ManagerClass AgsLv2ManagerClass;
typedef struct _AgsLv2Plugin AgsLv2Plugin;

typedef enum{
  AGS_LV2_PLUGIN_IS_LOADED    = 1,
}AgsLv2PluginFlags;

struct _AgsLv2Manager
{
  GObject object;

  gchar *locale;
  
  GList *lv2_plugin;
};

struct _AgsLv2ManagerClass
{
  GObjectClass object;
};

struct _AgsLv2Plugin
{
  guint flags;

  AgsTurtle *manifest;

  gchar *filename;
  gchar *path;
  void *plugin_so;
};

GType ags_lv2_manager_get_type(void);

AgsLv2Plugin* ags_lv2_plugin_alloc();
void ags_lv2_plugin_free(AgsLv2Plugin *lv2_plugin);

gchar** ags_lv2_manager_get_filenames();
AgsLv2Plugin* ags_lv2_manager_find_lv2_plugin(gchar *filename);

void ags_lv2_manager_load_file(gchar *filename);
void ags_lv2_manager_load_default_directory();

uint32_t ags_lv2_manager_uri_index(gchar *filename,
				   gchar *uri);
AgsTurtle* ags_lv2_manager_uri_turtle(gchar *filename,
				      gchar *uri);

/*  */
AgsLv2Manager* ags_lv2_manager_get_instance();

AgsLv2Manager* ags_lv2_manager_new(gchar *locale);

#endif /*__AGS_LV2_MANAGER_H__*/
