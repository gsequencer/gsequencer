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

#ifndef __AGS_LV2_PRESET_MANAGER_H__
#define __AGS_LV2_PRESET_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_LV2_PRESET_MANAGER                (ags_lv2_preset_manager_get_type())
#define AGS_LV2_PRESET_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LV2_PRESET_MANAGER, AgsLv2PresetManager))
#define AGS_LV2_PRESET_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LV2_PRESET_MANAGER, AgsLv2PresetManagerClass))
#define AGS_IS_LV2_PRESET_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LV2_PRESET_MANAGER))
#define AGS_IS_LV2_PRESET_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LV2_PRESET_MANAGER))
#define AGS_LV2_PRESET_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LV2_PRESET_MANAGER, AgsLv2PresetManagerClass))

#define AGS_LV2_PRESET_MANAGER_GET_OBJ_MUTEX(obj) (((AgsLv2PresetManager *) obj)->obj_mutex)

typedef struct _AgsLv2PresetManager AgsLv2PresetManager;
typedef struct _AgsLv2PresetManagerClass AgsLv2PresetManagerClass;

struct _AgsLv2PresetManager
{
  GObject gobject;

  pthread_mutex_t *obj_mutex;
  pthread_mutexattr_t *obj_mutexattr;

  GList *lv2_preset;
};

struct _AgsLv2PresetManagerClass
{
  GObjectClass gobject;
};

GType ags_lv2_preset_manager_get_type(void);

pthread_mutex_t* ags_lv2_preset_manager_get_class_mutex();

/*  */
AgsLv2PresetManager* ags_lv2_preset_manager_get_instance();

AgsLv2PresetManager* ags_lv2_preset_manager_new();

#endif /*__AGS_LV2_PRESET_MANAGER_H__*/
