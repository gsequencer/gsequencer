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

#include <ags/audio/file/ags_audio_container_manager.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_audio_container_manager_class_init(AgsAudioContainerManagerClass *audio_container_manager);
void ags_audio_container_manager_init(AgsAudioContainerManager *audio_container_manager);
void ags_audio_container_manager_set_property(GObject *gobject,
					      guint prop_id,
					      const GValue *value,
					      GParamSpec *param_spec);
void ags_audio_container_manager_get_property(GObject *gobject,
					      guint prop_id,
					      GValue *value,
					      GParamSpec *param_spec);
void ags_audio_container_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_audio_container_manager
 * @short_description: The audio container manager
 * @title: AgsAudioContainerManager
 * @section_id:
 * @include: ags/audio_container/ags_audio_container_manager.h
 *
 * #AgsAudioContainerManager stores audio containers.
 */

enum{
  PROP_0,
  PROP_AUDIO_CONTAINER,
};

static gpointer ags_audio_container_manager_parent_class = NULL;

AgsAudioContainerManager *ags_audio_container_manager = NULL;

GType
ags_audio_container_manager_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_audio_container_manager = 0;

    static const GTypeInfo ags_audio_container_manager_info = {
      sizeof (AgsAudioContainerManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_container_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioContainerManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_container_manager_init,
    };

    ags_type_audio_container_manager = g_type_register_static(G_TYPE_OBJECT,
							      "AgsAudioContainerManager", &ags_audio_container_manager_info,
							      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_audio_container_manager);
  }

  return g_define_type_id__volatile;
}

void
ags_audio_container_manager_class_init(AgsAudioContainerManagerClass *audio_container_manager)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_audio_container_manager_parent_class = g_type_class_peek_parent(audio_container_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) audio_container_manager;

  gobject->set_property = ags_audio_container_manager_set_property;
  gobject->get_property = ags_audio_container_manager_get_property;

  gobject->finalize = ags_audio_container_manager_finalize;

  /* properties */
  /**
   * AgsAudioContainerManager:audio-container:
   *
   * The #GList-struct containing #AgsAudioContainer.
   * 
   * Since: 3.4.0
   */
  param_spec = g_param_spec_pointer("audio-container",
				    i18n_pspec("containing audio_container"),
				    i18n_pspec("The audio_container it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CONTAINER,
				  param_spec);

  /* AgsModel */
}

void
ags_audio_container_manager_init(AgsAudioContainerManager *audio_container_manager)
{
  /* add audio container manager mutex */
  g_rec_mutex_init(&(audio_container_manager->obj_mutex));
  
  audio_container_manager->audio_container = NULL;
}

void
ags_audio_container_manager_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec)
{
  AgsAudioContainerManager *audio_container_manager;

  GRecMutex *audio_container_manager_mutex;

  audio_container_manager = AGS_AUDIO_CONTAINER_MANAGER(gobject);

  /* get audio container manager mutex */
  audio_container_manager_mutex = AGS_AUDIO_CONTAINER_MANAGER_GET_OBJ_MUTEX(audio_container_manager);

  switch(prop_id){
  case PROP_AUDIO_CONTAINER:
  {
    AgsAudioContainer *audio_container;
      
    /*  */
    audio_container = (AgsAudioContainer *) g_value_get_pointer(value);

    ags_audio_container_manager_add_audio_container(audio_container_manager,
						    (GObject *) audio_container);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_container_manager_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec)
{
  AgsAudioContainerManager *audio_container_manager;

  GRecMutex *audio_container_manager_mutex;

  audio_container_manager = AGS_AUDIO_CONTAINER_MANAGER(gobject);

  /* get audio container manager mutex */
  audio_container_manager_mutex = AGS_AUDIO_CONTAINER_MANAGER_GET_OBJ_MUTEX(audio_container_manager);

  switch(prop_id){
  case PROP_AUDIO_CONTAINER:
  {      
    g_rec_mutex_lock(audio_container_manager_mutex);
    
    g_value_set_pointer(value,
			g_list_copy_deep(audio_container_manager->audio_container,
					 (GCopyFunc) g_object_ref,
					 NULL));

    g_rec_mutex_unlock(audio_container_manager_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_container_manager_finalize(GObject *gobject)
{
  AgsAudioContainerManager *audio_container_manager;

  audio_container_manager = (AgsAudioContainerManager *) gobject;
  
  /* call parent */
  G_OBJECT_CLASS(ags_audio_container_manager_parent_class)->finalize(gobject);
}

/**
 * ags_audio_container_manager_add_audio_container:
 * @audio_container_manager: the #AgsAudioContainerManager
 * @audio_container: the #AgsAudioContainer
 * 
 * Add @audio_container to @audio_container_manager.
 * 
 * Since: 3.4.0
 */
void
ags_audio_container_manager_add_audio_container(AgsAudioContainerManager *audio_container_manager,
						GObject *audio_container)
{
  GRecMutex *audio_container_manager_mutex;

  if(!AGS_IS_AUDIO_CONTAINER_MANAGER(audio_container_manager) ||
     !AGS_IS_AUDIO_CONTAINER(audio_container)){
    return;
  }

  /* get audio container manager mutex */
  audio_container_manager_mutex = AGS_AUDIO_CONTAINER_MANAGER_GET_OBJ_MUTEX(audio_container_manager);

  g_rec_mutex_lock(audio_container_manager_mutex);
    
  if(g_list_find(audio_container_manager->audio_container,
		 audio_container) == NULL){
    g_object_ref(audio_container);
    audio_container_manager->audio_container = g_list_prepend(audio_container_manager->audio_container,
							      audio_container);
  }

  g_rec_mutex_unlock(audio_container_manager_mutex);
}

/**
 * ags_audio_container_manager_remove_audio_container:
 * @audio_container_manager: the #AgsAudioContainerManager
 * @audio_container: the #AgsAudioContainer
 * 
 * Remove @audio_container from @audio_container_manager.
 * 
 * Since: 3.4.0
 */
void
ags_audio_container_manager_remove_audio_container(AgsAudioContainerManager *audio_container_manager,
						   GObject *audio_container)
{
  GRecMutex *audio_container_manager_mutex;

  if(!AGS_IS_AUDIO_CONTAINER_MANAGER(audio_container_manager) ||
     !AGS_IS_AUDIO_CONTAINER(audio_container)){
    return;
  }

  /* get audio container manager mutex */
  audio_container_manager_mutex = AGS_AUDIO_CONTAINER_MANAGER_GET_OBJ_MUTEX(audio_container_manager);

  g_rec_mutex_lock(audio_container_manager_mutex);
    
  if(g_list_find(audio_container_manager->audio_container,
		 audio_container) != NULL){
    g_object_unref(audio_container);
    audio_container_manager->audio_container = g_list_remove(audio_container_manager->audio_container,
							     audio_container);
  }

  g_rec_mutex_unlock(audio_container_manager_mutex);
}

/**
 * ags_audio_container_manager_find_audio_container:
 * @audio_container_manager: the #AgsAudioContainerManager
 * @filename: the string identifier
 * 
 * Find audio container by @filename.
 *
 * Returns: (transfer none): the matching #AgsAudioContainer if found, otherwise %NULL
 * 
 * Since: 3.4.0
 */
GObject*
ags_audio_container_manager_find_audio_container(AgsAudioContainerManager *audio_container_manager,
						 gchar *filename)
{
  AgsAudioContainer *audio_container;
  
  GList *start_list, *list;

  gchar *current_filename;

  gboolean success;
  
  if(!AGS_IS_AUDIO_CONTAINER_MANAGER(audio_container_manager) ||
     filename == NULL){
    return(NULL);
  }

  g_object_get(audio_container_manager,
	       "audio-container", &start_list,
	       NULL);
    
  audio_container = NULL;
  
  list = start_list;
  
  success = FALSE;
  
  while(list != NULL){
    current_filename = NULL;

    g_object_get(AGS_AUDIO_CONTAINER(list->data),
		 "filename", &current_filename,
		 NULL);

    success = (!g_strcmp0(current_filename,
			  filename)) ? TRUE: FALSE;

    g_free(current_filename);
    
    if(success){
      audio_container = list->data;
      
      break;
    }
    
    list = list->next;
  }

  g_list_free_full(start_list,
		   (GDestroyNotify) g_object_unref);
  
  return(audio_container);
}

/**
 * ags_audio_container_manager_get_instance:
 * 
 * Get ags audio container manager instance.
 *
 * Returns: (transfer none): the #AgsAudioContainerManager singleton
 * 
 * Since: 3.4.0
 */
AgsAudioContainerManager*
ags_audio_container_manager_get_instance()
{
  static GMutex mutex = {0,};

  g_mutex_lock(&mutex);

  if(ags_audio_container_manager == NULL){
    ags_audio_container_manager = ags_audio_container_manager_new();
  }
  
  g_mutex_unlock(&mutex);

  return(ags_audio_container_manager);
}

/**
 * ags_audio_container_manager_new:
 *
 * Creates an #AgsAudioContainerManager
 *
 * Returns: a new #AgsAudioContainerManager
 *
 * Since: 3.4.0
 */
AgsAudioContainerManager*
ags_audio_container_manager_new()
{
  AgsAudioContainerManager *audio_container_manager;

  audio_container_manager = (AgsAudioContainerManager *) g_object_new(AGS_TYPE_AUDIO_CONTAINER_MANAGER,
								      NULL);
  
  return(audio_container_manager);
}
