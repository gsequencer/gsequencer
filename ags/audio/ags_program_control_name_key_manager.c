/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/audio/ags_program_control_name_key_manager.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_program_control_name_key_manager_class_init(AgsProgramControlNameKeyManagerClass *program_control_name_key_manager);
void ags_program_control_name_key_manager_init(AgsProgramControlNameKeyManager *program_control_name_key_manager);
void ags_program_control_name_key_manager_set_property(GObject *gobject,
						       guint prop_id,
						       const GValue *value,
						       GParamSpec *param_spec);
void ags_program_control_name_key_manager_get_property(GObject *gobject,
						       guint prop_id,
						       GValue *value,
						       GParamSpec *param_spec);
void ags_program_control_name_key_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_program_control_name_key_manager
 * @short_description: The audio file manager
 * @title: AgsProgramControlNameKeyManager
 * @section_id:
 * @include: ags/program_control_name_key/ags_program_control_name_key_manager.h
 *
 * #AgsProgramControlNameKeyManager stores audio files.
 */

enum{
  PROP_0,
  PROP_CONTROL_NAME_KEY,
};

static gpointer ags_program_control_name_key_manager_parent_class = NULL;

AgsProgramControlNameKeyManager *ags_program_control_name_key_manager = NULL;

GType
ags_program_control_name_key_manager_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_program_control_name_key_manager = 0;

    static const GTypeInfo ags_program_control_name_key_manager_info = {
      sizeof (AgsProgramControlNameKeyManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_program_control_name_key_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsProgramControlNameKeyManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_program_control_name_key_manager_init,
    };

    ags_type_program_control_name_key_manager = g_type_register_static(G_TYPE_OBJECT,
								       "AgsProgramControlNameKeyManager", &ags_program_control_name_key_manager_info,
								       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_program_control_name_key_manager);
  }

  return g_define_type_id__volatile;
}

void
ags_program_control_name_key_manager_class_init(AgsProgramControlNameKeyManagerClass *program_control_name_key_manager)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_program_control_name_key_manager_parent_class = g_type_class_peek_parent(program_control_name_key_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) program_control_name_key_manager;

  gobject->set_property = ags_program_control_name_key_manager_set_property;
  gobject->get_property = ags_program_control_name_key_manager_get_property;

  gobject->finalize = ags_program_control_name_key_manager_finalize;

  /* properties */
  /**
   * AgsProgramControlNameKeyManager:control-name-key:
   *
   * The #GHashTable-struct containing gpointer of key.
   * 
   * Since: 5.4.0
   */
  param_spec = g_param_spec_pointer("control-name-key",
				    i18n_pspec("containing control name key"),
				    i18n_pspec("The control name key it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONTROL_NAME_KEY,
				  param_spec);

  /* AgsModel */
}

void
ags_program_control_name_key_manager_init(AgsProgramControlNameKeyManager *program_control_name_key_manager)
{
  /* add program control name key manager mutex */
  g_rec_mutex_init(&(program_control_name_key_manager->obj_mutex));

  program_control_name_key_manager->control_name_key = g_hash_table_new_full(g_str_hash,
									     g_str_equal,
									     g_free,
									     NULL);
}

void
ags_program_control_name_key_manager_set_property(GObject *gobject,
						  guint prop_id,
						  const GValue *value,
						  GParamSpec *param_spec)
{
  AgsProgramControlNameKeyManager *program_control_name_key_manager;

  GRecMutex *program_control_name_key_manager_mutex;

  program_control_name_key_manager = AGS_PROGRAM_CONTROL_NAME_KEY_MANAGER(gobject);

  /* get program control name key manager mutex */
  program_control_name_key_manager_mutex = AGS_PROGRAM_CONTROL_NAME_KEY_MANAGER_GET_OBJ_MUTEX(program_control_name_key_manager);

  switch(prop_id){
  case PROP_CONTROL_NAME_KEY:
    {
      gpointer ptr;

      ptr = g_value_get_pointer(value);
    
      g_rec_mutex_lock(program_control_name_key_manager_mutex);

      program_control_name_key_manager->control_name_key = ptr;
    
      g_rec_mutex_unlock(program_control_name_key_manager_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_program_control_name_key_manager_get_property(GObject *gobject,
						  guint prop_id,
						  GValue *value,
						  GParamSpec *param_spec)
{
  AgsProgramControlNameKeyManager *program_control_name_key_manager;

  GRecMutex *program_control_name_key_manager_mutex;

  program_control_name_key_manager = AGS_PROGRAM_CONTROL_NAME_KEY_MANAGER(gobject);

  /* get program control name key manager mutex */
  program_control_name_key_manager_mutex = AGS_PROGRAM_CONTROL_NAME_KEY_MANAGER_GET_OBJ_MUTEX(program_control_name_key_manager);

  switch(prop_id){
  case PROP_CONTROL_NAME_KEY:
    {      
      g_rec_mutex_lock(program_control_name_key_manager_mutex);

      g_value_set_pointer(value,
			  program_control_name_key_manager->control_name_key);
    
      g_rec_mutex_unlock(program_control_name_key_manager_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_program_control_name_key_manager_finalize(GObject *gobject)
{
  AgsProgramControlNameKeyManager *program_control_name_key_manager;

  program_control_name_key_manager = (AgsProgramControlNameKeyManager *) gobject;
  
  /* call parent */
  G_OBJECT_CLASS(ags_program_control_name_key_manager_parent_class)->finalize(gobject);
}

GRecMutex*
ags_program_control_name_key_manager_get_obj_mutex(AgsProgramControlNameKeyManager *program_control_name_key_manager)
{
  return(&(program_control_name_key_manager->obj_mutex));
}

gpointer
ags_program_control_name_key_manager_find_program(AgsProgramControlNameKeyManager *program_control_name_key_manager,
						  gchar *control_name)
{
  gpointer ptr;

  GRecMutex *program_control_name_key_manager_mutex;

  /* get program control name key manager mutex */
  program_control_name_key_manager_mutex = AGS_PROGRAM_CONTROL_NAME_KEY_MANAGER_GET_OBJ_MUTEX(program_control_name_key_manager);

  g_rec_mutex_lock(program_control_name_key_manager_mutex);
  
  ptr = g_hash_table_lookup(program_control_name_key_manager->control_name_key,
			    control_name);

  if(ptr == NULL){
    ptr = g_strdup(control_name);

    g_hash_table_insert(program_control_name_key_manager->control_name_key,
			ptr,
			ptr);
  }
    
  g_rec_mutex_unlock(program_control_name_key_manager_mutex);

  return(ptr);
}

/**
 * ags_program_control_name_key_manager_get_instance:
 * 
 * Get ags audio file manager instance.
 *
 * Returns: (transfer none): the #AgsProgramControlNameKeyManager singleton
 * 
 * Since: 5.4.0
 */
AgsProgramControlNameKeyManager*
ags_program_control_name_key_manager_get_instance()
{
  static GMutex mutex = {0,};

  g_mutex_lock(&mutex);

  if(ags_program_control_name_key_manager == NULL){
    ags_program_control_name_key_manager = ags_program_control_name_key_manager_new();
  }
  
  g_mutex_unlock(&mutex);

  return(ags_program_control_name_key_manager);
}

/**
 * ags_program_control_name_key_manager_new:
 *
 * Creates an #AgsProgramControlNameKeyManager
 *
 * Returns: a new #AgsProgramControlNameKeyManager
 *
 * Since: 5.4.0
 */
AgsProgramControlNameKeyManager*
ags_program_control_name_key_manager_new()
{
  AgsProgramControlNameKeyManager *program_control_name_key_manager;

  program_control_name_key_manager = (AgsProgramControlNameKeyManager *) g_object_new(AGS_TYPE_PROGRAM_CONTROL_NAME_KEY_MANAGER,
										      NULL);
  
  return(program_control_name_key_manager);
}
