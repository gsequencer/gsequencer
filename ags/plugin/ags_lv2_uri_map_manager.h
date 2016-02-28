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

#ifndef __AGS_LV2_URI_MAP_MANAGER_H__
#define __AGS_LV2_URI_MAP_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <lv2.h>
#include <lv2/lv2plug.in/ns/ext/uri-map/uri-map.h>

#define AGS_TYPE_LV2_URI_MAP_MANAGER                (ags_lv2_uri_map_manager_get_type())
#define AGS_LV2_URI_MAP_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LV2_URI_MAP_MANAGER, AgsLv2UriMapManager))
#define AGS_LV2_URI_MAP_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LV2_URI_MAP_MANAGER, AgsLv2UriMapManagerClass))
#define AGS_IS_LV2_URI_MAP_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LV2_URI_MAP_MANAGER))
#define AGS_IS_LV2_URI_MAP_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LV2_URI_MAP_MANAGER))
#define AGS_LV2_URI_MAP_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LV2_URI_MAP_MANAGER, AgsLv2UriMapManagerClass))

typedef struct _AgsLv2UriMapManager AgsLv2UriMapManager;
typedef struct _AgsLv2UriMapManagerClass AgsLv2UriMapManagerClass;

struct _AgsLv2UriMapManager
{
  GObject gobject;

  uint32_t id_counter;
  
  GHashTable *uri_map;  
};

struct _AgsLv2UriMapManagerClass
{
  GObjectClass gobject;
};

GType ags_lv2_uri_map_manager_get_type(void);

gboolean ags_lv2_uri_map_manager_insert(AgsLv2UriMapManager *lv2_uri_map_manager,
					gchar *uri, GValue *id);
gboolean ags_lv2_uri_map_manager_remove(AgsLv2UriMapManager *lv2_uri_map_manager,
					gchar *uri);

GValue* ags_lv2_uri_map_manager_lookup(AgsLv2UriMapManager *lv2_uri_map_manager,
				       gchar *uri);

void ags_lv2_uri_map_manager_load_default(AgsLv2UriMapManager *lv2_uri_map_manager);

uint32_t ags_lv2_uri_map_manager_uri_to_id(LV2_URI_Map_Callback_Data callback_data,
					   char *map,
					   char *uri);

AgsLv2UriMapManager* ags_lv2_uri_map_manager_get_instance();
AgsLv2UriMapManager* ags_lv2_uri_map_manager_new();

#endif /*__AGS_LV2_URI_MAP_MANAGER_H__*/
