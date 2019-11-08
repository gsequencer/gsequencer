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

#ifndef __AGS_FREQUENCY_MAP_MANAGER_H__
#define __AGS_FREQUENCY_MAP_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_frequency_map.h>

G_BEGIN_DECLS

#define AGS_TYPE_FREQUENCY_MAP_MANAGER                (ags_frequency_map_manager_get_type())
#define AGS_FREQUENCY_MAP_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FREQUENCY_MAP_MANAGER, AgsFrequencyMapManager))
#define AGS_FREQUENCY_MAP_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FREQUENCY_MAP_MANAGER, AgsFrequencyMapManagerClass))
#define AGS_IS_FREQUENCY_MAP_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FREQUENCY_MAP_MANAGER))
#define AGS_IS_FREQUENCY_MAP_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FREQUENCY_MAP_MANAGER))
#define AGS_FREQUENCY_MAP_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FREQUENCY_MAP_MANAGER, AgsFrequencyMapManagerClass))

#define AGS_FREQUENCY_MAP_MANAGER_GET_OBJ_MUTEX(obj) (&(((AgsFrequencyMapManager *) obj)->obj_mutex))

#define AGS_FREQUENCY_MAP_MANAGER_EQUINOX (22000)

typedef struct _AgsFrequencyMapManager AgsFrequencyMapManager;
typedef struct _AgsFrequencyMapManagerClass AgsFrequencyMapManagerClass;

/**
 * AgsFrequencyMapManagerFlags:
 * @AGS_FREQUENCY_MAP_MANAGER_PRESERVE_FREQUENCY_MAP: if set preserve frequency maps, otherwise destroy 
 *
 * Enum values to control the behavior or indicate internal state of #AgsFrequencyMapManager by
 * enable/disable as flags.
 */
typedef enum{
  AGS_FREQUENCY_MAP_MANAGER_PRESERVE_FREQUENCY_MAP       = 1,
}AgsFrequencyMapManagerFlags;

struct _AgsFrequencyMapManager
{
  GObject gobject;

  guint flags;
  
  GRecMutex obj_mutex;

  GList *frequency_map;
  GList *factorized_frequency_map;
};

struct _AgsFrequencyMapManagerClass
{
  GObjectClass gobject;
};

GType ags_frequency_map_manager_get_type(void);

gboolean ags_frequency_map_manager_test_flags(AgsFrequencyMapManager *frequency_map_manager, guint flags);
void ags_frequency_map_manager_set_flags(AgsFrequencyMapManager *frequency_map_manager, guint flags);
void ags_frequency_map_manager_unset_flags(AgsFrequencyMapManager *frequency_map_manager, guint flags);

void ags_frequency_map_manager_add_frequency_map(AgsFrequencyMapManager *frequency_map_manager,
						 AgsFrequencyMap *frequency_map);
void ags_frequency_map_manager_add_factorized_frequency_map(AgsFrequencyMapManager *frequency_map_manager,
							    AgsFrequencyMap *frequency_map);

AgsFrequencyMap* ags_frequency_map_manager_find_frequency_map(AgsFrequencyMapManager *frequency_map_manager,
							      guint samplerate, guint buffer_size,
							      gdouble freq);
AgsFrequencyMap* ags_frequency_map_manager_find_factorized_frequency_map(AgsFrequencyMapManager *frequency_map_manager,
									 guint samplerate, guint buffer_size);

void ags_frequency_map_manager_load_default(AgsFrequencyMapManager *frequency_map_manager);

/*  */
AgsFrequencyMapManager* ags_frequency_map_manager_get_instance();

AgsFrequencyMapManager* ags_frequency_map_manager_new();

G_END_DECLS

#endif /*__AGS_FREQUENCY_MAP_MANAGER_H__*/
