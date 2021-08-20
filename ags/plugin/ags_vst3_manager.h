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

#ifndef __AGS_VST3_MANAGER_H__
#define __AGS_VST3_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/plugin/ags_vst3_plugin.h>

G_BEGIN_DECLS

#define AGS_TYPE_VST3_MANAGER                (ags_vst3_manager_get_type())
#define AGS_VST3_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_VST3_MANAGER, AgsVst3Manager))
#define AGS_VST3_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_VST3_MANAGER, AgsVst3ManagerClass))
#define AGS_IS_VST3_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_VST3_MANAGER))
#define AGS_IS_VST3_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_VST3_MANAGER))
#define AGS_VST3_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_VST3_MANAGER, AgsVst3ManagerClass))

#define AGS_VST3_MANAGER_GET_OBJ_MUTEX(obj) (&(((AgsVst3Manager *) obj)->obj_mutex))

typedef struct _AgsVst3Manager AgsVst3Manager;
typedef struct _AgsVst3ManagerClass AgsVst3ManagerClass;

struct _AgsVst3Manager
{
  GObject gobject;

  GRecMutex obj_mutex;
  
  GList *vst3_plugin_blacklist;
  GList *vst3_plugin;
};

struct _AgsVst3ManagerClass
{
  GObjectClass gobject;
};

GType ags_vst3_manager_get_type(void);

gchar** ags_vst3_manager_get_default_path();
void ags_vst3_manager_set_default_path(gchar** default_path);

gchar** ags_vst3_manager_get_filenames(AgsVst3Manager *vst3_manager);
AgsVst3Plugin* ags_vst3_manager_find_vst3_plugin(AgsVst3Manager *vst3_manager,
						 gchar *filename, gchar *effect);
AgsVst3Plugin* ags_vst3_manager_find_vst3_plugin_with_fallback(AgsVst3Manager *vst3_manager,
							       gchar *filename, gchar *effect);

void ags_vst3_manager_load_blacklist(AgsVst3Manager *vst3_manager,
				     gchar *blacklist_filename);

void ags_vst3_manager_load_file(AgsVst3Manager *vst3_manager,
				gchar *vst3_path,
				gchar *filename);
void ags_vst3_manager_load_default_directory(AgsVst3Manager *vst3_manager);

GList* ags_vst3_manager_get_vst3_plugin(AgsVst3Manager *vst3_manager);

/*  */
AgsVst3Manager* ags_vst3_manager_get_instance();

AgsVst3Manager* ags_vst3_manager_new();

G_END_DECLS

#endif /*__AGS_VST3_MANAGER_H__*/
