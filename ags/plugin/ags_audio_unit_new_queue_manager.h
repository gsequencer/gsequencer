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

#ifndef __AGS_AUDIO_UNIT_NEW_QUEUE_MANAGER_H__
#define __AGS_AUDIO_UNIT_NEW_QUEUE_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/plugin/ags_audio_unit_new_queue.h>

G_BEGIN_DECLS

#define AGS_TYPE_AUDIO_UNIT_NEW_QUEUE_MANAGER                (ags_audio_unit_new_queue_manager_get_type())
#define AGS_AUDIO_UNIT_NEW_QUEUE_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_UNIT_NEW_QUEUE_MANAGER, AgsAudioUnitNewQueueManager))
#define AGS_AUDIO_UNIT_NEW_QUEUE_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUDIO_UNIT_NEW_QUEUE_MANAGER, AgsAudioUnitNewQueueManagerClass))
#define AGS_IS_AUDIO_UNIT_NEW_QUEUE_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUDIO_UNIT_NEW_QUEUE_MANAGER))
#define AGS_IS_AUDIO_UNIT_NEW_QUEUE_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUDIO_UNIT_NEW_QUEUE_MANAGER))
#define AGS_AUDIO_UNIT_NEW_QUEUE_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_AUDIO_UNIT_NEW_QUEUE_MANAGER, AgsAudioUnitNewQueueManagerClass))

#define AGS_AUDIO_UNIT_NEW_QUEUE_MANAGER_GET_OBJ_MUTEX(obj) (&(((AgsAudioUnitNewQueueManager *) obj)->obj_mutex))

typedef struct _AgsAudioUnitNewQueueManager AgsAudioUnitNewQueueManager;
typedef struct _AgsAudioUnitNewQueueManagerClass AgsAudioUnitNewQueueManagerClass;

struct _AgsAudioUnitNewQueueManager
{
  GObject gobject;

  GRecMutex obj_mutex;

  GList *new_queue;
};

struct _AgsAudioUnitNewQueueManagerClass
{
  GObjectClass gobject;
};
  
GType ags_audio_unit_new_queue_manager_get_type(void);

GList* ags_audio_unit_new_queue_manager_get_new_queue(AgsAudioUnitNewQueueManager *audio_unit_new_queue_manager);

/*  */
AgsAudioUnitNewQueueManager* ags_audio_unit_new_queue_manager_get_instance();

AgsAudioUnitNewQueueManager* ags_audio_unit_new_queue_manager_new();

G_END_DECLS

#endif /*__AGS_AUDIO_UNIT_NEW_QUEUE_MANAGER_H__*/
