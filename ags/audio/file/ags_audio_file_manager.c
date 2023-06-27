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

#include <ags/audio/file/ags_audio_file_manager.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_audio_file_manager_class_init(AgsAudioFileManagerClass *audio_file_manager);
void ags_audio_file_manager_init(AgsAudioFileManager *audio_file_manager);
void ags_audio_file_manager_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_audio_file_manager_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_audio_file_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_audio_file_manager
 * @short_description: The audio file manager
 * @title: AgsAudioFileManager
 * @section_id:
 * @include: ags/audio_file/ags_audio_file_manager.h
 *
 * #AgsAudioFileManager stores audio files.
 */

enum{
  PROP_0,
  PROP_AUDIO_FILE,
};

static gpointer ags_audio_file_manager_parent_class = NULL;

AgsAudioFileManager *ags_audio_file_manager = NULL;

GType
ags_audio_file_manager_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_audio_file_manager = 0;

    static const GTypeInfo ags_audio_file_manager_info = {
      sizeof (AgsAudioFileManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_file_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioFileManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_file_manager_init,
    };

    ags_type_audio_file_manager = g_type_register_static(G_TYPE_OBJECT,
							 "AgsAudioFileManager", &ags_audio_file_manager_info,
							 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_audio_file_manager);
  }

  return g_define_type_id__volatile;
}

void
ags_audio_file_manager_class_init(AgsAudioFileManagerClass *audio_file_manager)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_audio_file_manager_parent_class = g_type_class_peek_parent(audio_file_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) audio_file_manager;

  gobject->set_property = ags_audio_file_manager_set_property;
  gobject->get_property = ags_audio_file_manager_get_property;

  gobject->finalize = ags_audio_file_manager_finalize;

  /* properties */
  /**
   * AgsAudioFileManager:audio-file:
   *
   * The #GList-struct containing #AgsAudioFile.
   * 
   * Since: 3.4.0
   */
  param_spec = g_param_spec_pointer("audio-file",
				    i18n_pspec("containing audio_file"),
				    i18n_pspec("The audio_file it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_FILE,
				  param_spec);

  /* AgsModel */
}

void
ags_audio_file_manager_init(AgsAudioFileManager *audio_file_manager)
{
  audio_file_manager->audio_file = NULL;
}

void
ags_audio_file_manager_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsAudioFileManager *audio_file_manager;

  GRecMutex *audio_file_manager_mutex;

  audio_file_manager = AGS_AUDIO_FILE_MANAGER(gobject);

  /* get audio file manager mutex */
  audio_file_manager_mutex = AGS_AUDIO_FILE_MANAGER_GET_OBJ_MUTEX(audio_file_manager);

  switch(prop_id){
  case PROP_AUDIO_FILE:
  {
    AgsAudioFile *audio_file;
      
    /*  */
    audio_file = (AgsAudioFile *) g_value_get_pointer(value);

    ags_audio_file_manager_add_audio_file(audio_file_manager,
					  (GObject *) audio_file);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_file_manager_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsAudioFileManager *audio_file_manager;

  GRecMutex *audio_file_manager_mutex;

  audio_file_manager = AGS_AUDIO_FILE_MANAGER(gobject);

  /* get audio file manager mutex */
  audio_file_manager_mutex = AGS_AUDIO_FILE_MANAGER_GET_OBJ_MUTEX(audio_file_manager);

  switch(prop_id){
  case PROP_AUDIO_FILE:
  {      
    g_rec_mutex_lock(audio_file_manager_mutex);

    g_value_set_pointer(value,
			g_list_copy_deep(audio_file_manager->audio_file,
					 (GCopyFunc) g_object_ref,
					 NULL));

    g_rec_mutex_unlock(audio_file_manager_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_file_manager_finalize(GObject *gobject)
{
  AgsAudioFileManager *audio_file_manager;

  audio_file_manager = (AgsAudioFileManager *) gobject;
  
  /* call parent */
  G_OBJECT_CLASS(ags_audio_file_manager_parent_class)->finalize(gobject);
}

GRecMutex*
ags_audio_file_manager_get_obj_mutex(AgsAudioFileManager *audio_file_manager)
{
  return(&(audio_file_manager->obj_mutex));
}

/**
 * ags_audio_file_manager_add_audio_file:
 * @audio_file_manager: the #AgsAudioFileManager
 * @audio_file: the #AgsAudioFile
 * 
 * Add @audio_file to @audio_file_manager.
 * 
 * Since: 3.4.0
 */
void
ags_audio_file_manager_add_audio_file(AgsAudioFileManager *audio_file_manager,
				      GObject *audio_file)
{
  GRecMutex *audio_file_manager_mutex;
  
  if(!AGS_IS_AUDIO_FILE_MANAGER(audio_file_manager) ||
     !AGS_IS_AUDIO_FILE(audio_file)){
    return;
  }
  
  /* get audio file manager mutex */
  audio_file_manager_mutex = AGS_AUDIO_FILE_MANAGER_GET_OBJ_MUTEX(audio_file_manager);

  g_rec_mutex_lock(audio_file_manager_mutex);

  if(g_list_find(audio_file_manager->audio_file,
		 audio_file) == NULL){
    g_object_ref(audio_file);
    audio_file_manager->audio_file = g_list_prepend(audio_file_manager->audio_file,
						    audio_file);
  }

  g_rec_mutex_unlock(audio_file_manager_mutex);
}

/**
 * ags_audio_file_manager_remove_audio_file:
 * @audio_file_manager: the #AgsAudioFileManager
 * @audio_file: the #AgsAudioFile
 * 
 * Remove @audio_file from @audio_file_manager.
 * 
 * Since: 3.4.0
 */
void
ags_audio_file_manager_remove_audio_file(AgsAudioFileManager *audio_file_manager,
					 GObject *audio_file)
{
  GRecMutex *audio_file_manager_mutex;
  
  if(!AGS_IS_AUDIO_FILE_MANAGER(audio_file_manager) ||
     !AGS_IS_AUDIO_FILE(audio_file)){
    return;
  }

  /* get audio file manager mutex */
  audio_file_manager_mutex = AGS_AUDIO_FILE_MANAGER_GET_OBJ_MUTEX(audio_file_manager);

  g_rec_mutex_lock(audio_file_manager_mutex);

  if(g_list_find(audio_file_manager->audio_file,
		 audio_file) != NULL){
    g_object_unref(audio_file);
    audio_file_manager->audio_file = g_list_remove(audio_file_manager->audio_file,
						   audio_file);
  }

  g_rec_mutex_unlock(audio_file_manager_mutex);
}

/**
 * ags_audio_file_manager_find_audio_file:
 * @audio_file_manager: the #AgsAudioFileManager
 * @filename: the string identifier
 * 
 * Find audio file by @filename.
 *
 * Returns: (transfer none): the matching #AgsAudioFile if found, otherwise %NULL
 * 
 * Since: 3.4.0
 */
GObject*
ags_audio_file_manager_find_audio_file(AgsAudioFileManager *audio_file_manager,
				       gchar *filename)
{
  AgsAudioFile *audio_file;
  
  GList *start_list, *list;

  gchar *current_filename;

  gboolean success;
  
  GRecMutex *audio_file_manager_mutex;
  
  if(!AGS_IS_AUDIO_FILE_MANAGER(audio_file_manager) ||
     filename == NULL){
    return(NULL);
  }

  g_object_get(audio_file_manager,
	       "audio-file", &start_list,
	       NULL);

  /* get audio file manager mutex */
  audio_file_manager_mutex = AGS_AUDIO_FILE_MANAGER_GET_OBJ_MUTEX(audio_file_manager);

  audio_file = NULL;
  
  list = start_list;

  success = FALSE;
  
  while(list != NULL){
    current_filename = NULL;

    g_object_get(AGS_AUDIO_FILE(list->data),
		 "filename", &current_filename,
		 NULL);

    success = (!g_strcmp0(current_filename,
			  filename)) ? TRUE: FALSE;

    g_free(current_filename);
    
    if(success){
      audio_file = list->data;
      
      break;
    }
    
    list = list->next;
  }

  g_list_free_full(start_list,
		   (GDestroyNotify) g_object_unref);
  
  return(audio_file);
}

/**
 * ags_audio_file_manager_get_instance:
 * 
 * Get ags audio file manager instance.
 *
 * Returns: (transfer none): the #AgsAudioFileManager singleton
 * 
 * Since: 3.4.0
 */
AgsAudioFileManager*
ags_audio_file_manager_get_instance()
{
  static GMutex mutex = {0,};

  g_mutex_lock(&mutex);

  if(ags_audio_file_manager == NULL){
    ags_audio_file_manager = ags_audio_file_manager_new();
  }
  
  g_mutex_unlock(&mutex);

  return(ags_audio_file_manager);
}

/**
 * ags_audio_file_manager_new:
 *
 * Creates an #AgsAudioFileManager
 *
 * Returns: a new #AgsAudioFileManager
 *
 * Since: 3.4.0
 */
AgsAudioFileManager*
ags_audio_file_manager_new()
{
  AgsAudioFileManager *audio_file_manager;

  audio_file_manager = (AgsAudioFileManager *) g_object_new(AGS_TYPE_AUDIO_FILE_MANAGER,
							    NULL);
  
  return(audio_file_manager);
}
