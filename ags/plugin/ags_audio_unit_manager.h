/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#ifndef __AGS_AUDIO_UNIT_MANAGER_H__
#define __AGS_AUDIO_UNIT_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/plugin/ags_audio_unit_plugin.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUDIO_UNIT_MANAGER                (ags_audio_unit_manager_get_type())
#define AGS_AUDIO_UNIT_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_UNIT_MANAGER, AgsAudioUnitManager))
#define AGS_AUDIO_UNIT_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUDIO_UNIT_MANAGER, AgsAudioUnitManagerClass))
#define AGS_IS_AUDIO_UNIT_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUDIO_UNIT_MANAGER))
#define AGS_IS_AUDIO_UNIT_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUDIO_UNIT_MANAGER))
#define AGS_AUDIO_UNIT_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_AUDIO_UNIT_MANAGER, AgsAudioUnitManagerClass))

#define AGS_AUDIO_UNIT_MANAGER_GET_OBJ_MUTEX(obj) (&(((AgsAudioUnitManager *) obj)->obj_mutex))

typedef struct _AgsAudioUnitManager AgsAudioUnitManager;
typedef struct _AgsAudioUnitManagerClass AgsAudioUnitManagerClass;

struct _AgsAudioUnitManager
{
  GObject gobject;

  GRecMutex obj_mutex;
  
  GList *audio_unit_plugin_blacklist;
  GList *audio_unit_plugin;

  GList *audio_unit_new_queue;
};

struct _AgsAudioUnitManagerClass
{
  GObjectClass gobject;
};
  
GType ags_audio_unit_manager_get_type(void);

gchar** ags_audio_unit_manager_get_default_path();
void ags_audio_unit_manager_set_default_path(gchar** default_path);

gchar** ags_audio_unit_manager_get_filenames(AgsAudioUnitManager *audio_unit_manager);
AgsAudioUnitPlugin* ags_audio_unit_manager_find_audio_unit_plugin(AgsAudioUnitManager *audio_unit_manager,
								   gchar *filename, gchar *effect);
AgsAudioUnitPlugin* ags_audio_unit_manager_find_audio_unit_plugin_with_fallback(AgsAudioUnitManager *audio_unit_manager,
										 gchar *filename, gchar *effect);

void ags_audio_unit_manager_load_blacklist(AgsAudioUnitManager *audio_unit_manager,
					   gchar *blacklist_filename);

void ags_audio_unit_manager_load_component(AgsAudioUnitManager *audio_unit_manager,
					   gpointer theComponent);
void ags_audio_unit_manager_load_shared(AgsAudioUnitManager *audio_unit_manager);

GList* ags_audio_unit_manager_get_audio_unit_plugin(AgsAudioUnitManager *audio_unit_manager);

/*  */
AgsAudioUnitManager* ags_audio_unit_manager_get_instance();

AgsAudioUnitManager* ags_audio_unit_manager_new();

G_END_DECLS

#endif /*__AGS_AUDIO_UNIT_MANAGER_H__*/
