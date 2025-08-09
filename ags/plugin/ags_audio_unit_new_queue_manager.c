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

#include <ags/plugin/ags_audio_unit_new_queue_manager.h>
#include <ags/plugin/ags_audio_unit_new_queue.h>

#import <CoreFoundation/CoreFoundation.h>
#import <AVFoundation/AVFoundation.h>

#include <ags/config.h>

void ags_audio_unit_new_queue_manager_class_init(AgsAudioUnitNewQueueManagerClass *audio_unit_new_queue_manager);
void ags_audio_unit_new_queue_manager_init (AgsAudioUnitNewQueueManager *audio_unit_new_queue_manager);
void ags_audio_unit_new_queue_manager_dispose(GObject *gobject);
void ags_audio_unit_new_queue_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_audio_unit_new_queue_manager
 * @short_description: Singleton pattern to organize Audio Unit new queue
 * @title: AgsAudioUnitNewQueueManager
 * @section_id:
 * @include: ags/plugin/ags_audio_unit_new_queue_manager.h
 *
 * The #AgsAudioUnitNewQueueManager provides Audio Unit new queue.
 */

static gpointer ags_audio_unit_new_queue_manager_parent_class = NULL;

AgsAudioUnitNewQueueManager *ags_audio_unit_new_queue_manager = NULL;
gchar **ags_audio_unit_new_queue_default_path = NULL;

GType
ags_audio_unit_new_queue_manager_get_type (void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_audio_unit_new_queue_manager = 0;

    static const GTypeInfo ags_audio_unit_new_queue_manager_info = {
      sizeof (AgsAudioUnitNewQueueManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_unit_new_queue_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioUnitNewQueueManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_unit_new_queue_manager_init,
    };

    ags_type_audio_unit_new_queue_manager = g_type_register_static(G_TYPE_OBJECT,
								   "AgsAudioUnitNewQueueManager",
								   &ags_audio_unit_new_queue_manager_info,
								   0);

    g_once_init_leave(&g_define_type_id__static, ags_type_audio_unit_new_queue_manager);
  }

  return(g_define_type_id__static);
}

void
ags_audio_unit_new_queue_manager_class_init(AgsAudioUnitNewQueueManagerClass *audio_unit_new_queue_manager)
{
  GObjectClass *gobject;

  ags_audio_unit_new_queue_manager_parent_class = g_type_class_peek_parent(audio_unit_new_queue_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) audio_unit_new_queue_manager;

  gobject->dispose = ags_audio_unit_new_queue_manager_dispose;
  gobject->finalize = ags_audio_unit_new_queue_manager_finalize;
}

void
ags_audio_unit_new_queue_manager_init(AgsAudioUnitNewQueueManager *audio_unit_new_queue_manager)
{
  /* audio_unit_new_queue manager mutex */
  g_rec_mutex_init(&(audio_unit_new_queue_manager->obj_mutex));

  /* Audio Unit plugin and path */
  audio_unit_new_queue_manager->new_queue = NULL;
}

void
ags_audio_unit_new_queue_manager_dispose(GObject *gobject)
{
  AgsAudioUnitNewQueueManager *audio_unit_new_queue_manager;

  audio_unit_new_queue_manager = AGS_AUDIO_UNIT_NEW_QUEUE_MANAGER(gobject);

  if(audio_unit_new_queue_manager->new_queue != NULL){
    g_list_free_full(audio_unit_new_queue_manager->new_queue,
		     (GDestroyNotify) ags_audio_unit_new_queue_free);

    audio_unit_new_queue_manager->new_queue = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_audio_unit_new_queue_manager_parent_class)->dispose(gobject);
}

void
ags_audio_unit_new_queue_manager_finalize(GObject *gobject)
{
  AgsAudioUnitNewQueueManager *audio_unit_new_queue_manager;

  audio_unit_new_queue_manager = AGS_AUDIO_UNIT_NEW_QUEUE_MANAGER(gobject);

  g_list_free_full(audio_unit_new_queue_manager->new_queue,
		   (GDestroyNotify) ags_audio_unit_new_queue_free);
  
  if(audio_unit_new_queue_manager == ags_audio_unit_new_queue_manager){
    ags_audio_unit_new_queue_manager = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_audio_unit_new_queue_manager_parent_class)->finalize(gobject);
}

/**
 * ags_audio_unit_new_queue_manager_get_new_queue:
 * @audio_unit_new_queue_manager: the #AgsAudioUnitNewQueueManager
 *
 * Get new queue.
 *
 * Returns: (transfer full): the #GList-struct containing #AgsAudioUnitNewQueue
 *
 * Since: 8.1.2
 */
GList*
ags_audio_unit_new_queue_manager_get_new_queue(AgsAudioUnitNewQueueManager *audio_unit_new_queue_manager)
{
  GList *new_queue;
  
  GRecMutex *audio_unit_new_queue_manager_mutex;

  if(!AGS_IS_AUDIO_UNIT_NEW_QUEUE_MANAGER(audio_unit_new_queue_manager)){
    return(NULL);
  }
  
  /* get audio_unit_new_queue manager mutex */
  audio_unit_new_queue_manager_mutex = AGS_AUDIO_UNIT_NEW_QUEUE_MANAGER_GET_OBJ_MUTEX(audio_unit_new_queue_manager);
  
  g_rec_mutex_lock(audio_unit_new_queue_manager_mutex);

  new_queue = g_list_copy(audio_unit_new_queue_manager->new_queue);
  
  g_rec_mutex_unlock(audio_unit_new_queue_manager_mutex);

  return(new_queue);
}

/**
 * ags_audio_unit_new_queue_manager_fetch_audio_unit:
 * @audio_unit_new_queue_manager: the #AgsAudioUnitNewQueueManager
 * @plugin_name: the plugin name
 *
 * Fetch Audio Unit by @plugin_name.
 *
 * Returns: (transfer full): the AVAudioUnit
 *
 * Since: 8.1.2
 */
gpointer
ags_audio_unit_new_queue_manager_fetch_audio_unit(AgsAudioUnitNewQueueManager *audio_unit_new_queue_manager,
						  gchar *plugin_name)
{
  GList *new_queue;
  
  gpointer audio_unit;
  
  GRecMutex *audio_unit_new_queue_manager_mutex;

  if(!AGS_IS_AUDIO_UNIT_NEW_QUEUE_MANAGER(audio_unit_new_queue_manager) ||
     plugin_name == NULL){
    return(NULL);
  }
  
  /* get audio_unit_new_queue manager mutex */
  audio_unit_new_queue_manager_mutex = AGS_AUDIO_UNIT_NEW_QUEUE_MANAGER_GET_OBJ_MUTEX(audio_unit_new_queue_manager);
  
  audio_unit = NULL;

  g_rec_mutex_lock(audio_unit_new_queue_manager_mutex);

  new_queue = audio_unit_new_queue_manager->new_queue;

  while(new_queue != NULL){
    AgsAudioUnitNewQueue *current;

    current = (AgsAudioUnitNewQueue *) new_queue->data;
    
    if(current != NULL &&
       !(current->in_use) &&
       current->plugin_name != NULL &&
       !g_strcmp0(current->plugin_name,
		  plugin_name)){
      current->in_use = TRUE;
      
      audio_unit = current->audio_unit;

      audio_unit_new_queue_manager->new_queue = g_list_remove(audio_unit_new_queue_manager->new_queue,
							      current);
      ags_audio_unit_new_queue_free(current);
      
      break;
    }

    new_queue = new_queue->next;
  }
  
  g_rec_mutex_unlock(audio_unit_new_queue_manager_mutex);

  return(audio_unit);
}

/**
 * ags_audio_unit_new_queue_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: (transfer none): the #AgsAudioUnitNewQueueManager
 *
 * Since: 8.1.2
 */
AgsAudioUnitNewQueueManager*
ags_audio_unit_new_queue_manager_get_instance()
{
  static GMutex mutex;

  g_mutex_lock(&(mutex));

  if(ags_audio_unit_new_queue_manager == NULL){
    ags_audio_unit_new_queue_manager = ags_audio_unit_new_queue_manager_new();
  }

  g_mutex_unlock(&(mutex));

  return(ags_audio_unit_new_queue_manager);
}

/**
 * ags_audio_unit_new_queue_manager_new:
 *
 * Create a new instance of #AgsAudioUnitNewQueueManager
 *
 * Returns: the new #AgsAudioUnitNewQueueManager
 *
 * Since: 8.1.2
 */
AgsAudioUnitNewQueueManager*
ags_audio_unit_new_queue_manager_new()
{
  AgsAudioUnitNewQueueManager *audio_unit_new_queue_manager;

  audio_unit_new_queue_manager = (AgsAudioUnitNewQueueManager *) g_object_new(AGS_TYPE_AUDIO_UNIT_NEW_QUEUE_MANAGER,
									      NULL);

  return(audio_unit_new_queue_manager);
}
