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

#ifndef __AGS_AUDIO_CONTAINER_MANAGER_H__
#define __AGS_AUDIO_CONTAINER_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUDIO_CONTAINER_MANAGER                (ags_audio_container_manager_get_type())
#define AGS_AUDIO_CONTAINER_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_CONTAINER_MANAGER, AgsAudioContainerManager))
#define AGS_AUDIO_CONTAINER_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_AUDIO_CONTAINER_MANAGER, AgsAudioContainerManagerClass))
#define AGS_IS_AUDIO_CONTAINER_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUDIO_CONTAINER_MANAGER))
#define AGS_IS_AUDIO_CONTAINER_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUDIO_CONTAINER_MANAGER))
#define AGS_AUDIO_CONTAINER_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_AUDIO_CONTAINER_MANAGER, AgsAudioContainerManagerClass))

typedef struct _AgsAudioContainerManager AgsAudioContainerManager;
typedef struct _AgsAudioContainerManagerClass AgsAudioContainerManagerClass;

struct _AgsAudioContainerManager
{
  GObject gobject;
  
  GList *audio_container;
};

struct _AgsAudioContainerManagerClass
{
  GObjectClass gobject;
};

GType ags_audio_container_manager_get_type();

void ags_audio_container_manager_add_audio_container(AgsAudioContainerManager *audio_container_manager,
						     GObject *audio_container);
void ags_audio_container_manager_remove_audio_container(AgsAudioContainerManager *audio_container_manager,
							GObject *audio_container);

GObject* ags_audio_container_manager_find_audio_container(AgsAudioContainerManager *audio_container_manager,
							  gchar *filename);

/* instance */
AgsAudioContainerManager* ags_audio_container_manager_get_instance();

AgsAudioContainerManager* ags_audio_container_manager_new();

G_END_DECLS

#endif /*__AGS_AUDIO_CONTAINER_MANAGER_H__*/
