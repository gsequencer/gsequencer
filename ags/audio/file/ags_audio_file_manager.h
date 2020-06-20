/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_AUDIO_FILE_MANAGER_H__
#define __AGS_AUDIO_FILE_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUDIO_FILE_MANAGER                (ags_audio_file_manager_get_type())
#define AGS_AUDIO_FILE_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_FILE_MANAGER, AgsAudioFileManager))
#define AGS_AUDIO_FILE_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_AUDIO_FILE_MANAGER, AgsAudioFileManagerClass))
#define AGS_IS_AUDIO_FILE_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUDIO_FILE_MANAGER))
#define AGS_IS_AUDIO_FILE_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUDIO_FILE_MANAGER))
#define AGS_AUDIO_FILE_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_AUDIO_FILE_MANAGER, AgsAudioFileManagerClass))

#define AGS_AUDIO_FILE_MANAGER_GET_OBJ_MUTEX(obj) (&(((AgsAudioFileManager *) obj)->obj_mutex))

typedef struct _AgsAudioFileManager AgsAudioFileManager;
typedef struct _AgsAudioFileManagerClass AgsAudioFileManagerClass;

struct _AgsAudioFileManager
{
  GObject gobject;
  
  GRecMutex obj_mutex;

  GList *audio_file;
};

struct _AgsAudioFileManagerClass
{
  GObjectClass gobject;
};

GType ags_audio_file_manager_get_type();

GRecMutex* ags_audio_file_manager_get_obj_mutex(AgsAudioFileManager *audio_file_manager);

void ags_audio_file_manager_add_audio_file(AgsAudioFileManager *audio_file_manager,
					   GObject *audio_file);
void ags_audio_file_manager_remove_audio_file(AgsAudioFileManager *audio_file_manager,
					      GObject *audio_file);

GObject* ags_audio_file_manager_find_audio_file(AgsAudioFileManager *audio_file_manager,
						gchar *filename);

/* instance */
AgsAudioFileManager* ags_audio_file_manager_get_instance();

AgsAudioFileManager* ags_audio_file_manager_new();

G_END_DECLS

#endif /*__AGS_AUDIO_FILE_MANAGER_H__*/
