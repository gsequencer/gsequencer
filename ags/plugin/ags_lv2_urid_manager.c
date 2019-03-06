/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/plugin/ags_lv2_urid_manager.h>

#include <ags/object/ags_connectable.h>

void ags_lv2_urid_manager_class_init(AgsLv2UridManagerClass *lv2_urid_manager);
void ags_lv2_urid_manager_init(AgsLv2UridManager *lv2_urid_manager);
void ags_lv2_urid_manager_finalize(GObject *gobject);

void ags_lv2_urid_manager_destroy_data(gpointer data);
gboolean ags_lv2_urid_manager_finder(gpointer key, gpointer value, gpointer user_data);

/**
 * SECTION:ags_lv2_urid_manager
 * @short_description: LV2 urid manager
 * @title: AgsLv2UridManager
 * @section_id:
 * @include: ags/plugin/ags_lv2_urid_manager.h
 *
 * The #AgsLv2UridManager keeps your urid in a hash table where you can lookup your
 * ids.
 */

static gpointer ags_lv2_urid_manager_parent_class = NULL;

static pthread_mutex_t ags_lv2_urid_manager_class_mutex = PTHREAD_MUTEX_INITIALIZER;

AgsLv2UridManager *ags_lv2_urid_manager = NULL;

GType
ags_lv2_urid_manager_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_lv2_urid_manager = 0;

    const GTypeInfo ags_lv2_urid_manager_info = {
      sizeof (AgsLv2UridManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lv2_urid_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLv2UridManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2_urid_manager_init,
    };

    ags_type_lv2_urid_manager = g_type_register_static(G_TYPE_OBJECT,
						       "AgsLv2UridManager",
						       &ags_lv2_urid_manager_info,
						       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_lv2_urid_manager);
  }

  return g_define_type_id__volatile;
}

void
ags_lv2_urid_manager_class_init(AgsLv2UridManagerClass *lv2_urid_manager)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_lv2_urid_manager_parent_class = g_type_class_peek_parent(lv2_urid_manager);

  /* GObject */
  gobject = (GObjectClass *) lv2_urid_manager;

  gobject->finalize = ags_lv2_urid_manager_finalize;
}

void
ags_lv2_urid_manager_init(AgsLv2UridManager *lv2_urid_manager)
{
  /* lv2 urid manager mutex */
  lv2_urid_manager->obj_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(lv2_urid_manager->obj_mutexattr);
  pthread_mutexattr_settype(lv2_urid_manager->obj_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(lv2_urid_manager->obj_mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif

  lv2_urid_manager->obj_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(lv2_urid_manager->obj_mutex,
		     lv2_urid_manager->obj_mutexattr);

  lv2_urid_manager->id_counter = 1;

  lv2_urid_manager->urid = g_hash_table_new_full(g_str_hash, g_str_equal,
						 NULL,
						 (GDestroyNotify) ags_lv2_urid_manager_destroy_data);
}

void
ags_lv2_urid_manager_finalize(GObject *gobject)
{
  AgsLv2UridManager *lv2_urid_manager;

  lv2_urid_manager = AGS_LV2_URID_MANAGER(gobject);

  g_hash_table_destroy(lv2_urid_manager->urid);

  if(lv2_urid_manager == ags_lv2_urid_manager){
    ags_lv2_urid_manager = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_lv2_urid_manager_parent_class)->finalize(gobject);
}

/**
 * ags_lv2_urid_manager_get_class_mutex:
 * 
 * Get class mutex.
 * 
 * Returns: the class mutex of #AgsLv2UridManager
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_lv2_urid_manager_get_class_mutex()
{
  return(&ags_lv2_urid_manager_class_mutex);
}

void
ags_lv2_urid_manager_destroy_data(gpointer data)
{
  /* empty */
}

gboolean
ags_lv2_urid_manager_finder(gpointer key, gpointer value, gpointer user_data)
{
  if((uint32_t) g_value_get_uint((GValue *) value) == (uint32_t) g_value_get_uint((GValue *) user_data)){
    return(TRUE);
  }

  return(FALSE);
}

/**
 * ags_lv2_urid_manager_insert:
 * @lv2_urid_manager: the #AgsLv2UridManager
 * @uri: the object as key
 * @id: the id to insert
 *
 * Inserts a id into hash  associated with @urid.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_lv2_urid_manager_insert(AgsLv2UridManager *lv2_urid_manager,
			    gchar *uri, GValue *id)
{
  pthread_mutex_t *lv2_urid_manager_mutex;

  if(lv2_urid_manager == NULL ||
     uri == NULL ||
     id == NULL){
    return(FALSE);
  }

  /* get lv2 uri map manager mutex */
  pthread_mutex_lock(ags_lv2_urid_manager_get_class_mutex());
  
  lv2_urid_manager_mutex = lv2_urid_manager->obj_mutex;
  
  pthread_mutex_unlock(ags_lv2_urid_manager_get_class_mutex());

  /*  */
  pthread_mutex_lock(lv2_urid_manager_mutex);

  g_hash_table_insert(lv2_urid_manager->urid,
		      uri, id);

  pthread_mutex_unlock(lv2_urid_manager_mutex);

  return(TRUE);
}

/**
 * ags_lv2_urid_manager_remove:
 * @lv2_urid_manager: the #AgsLv2UridManager
 * @uri: the object to remove
 * 
 * Removes an entry associated with @urid.
 *
 * Returns: %TRUE as successfully removed, otherwise %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_lv2_urid_manager_remove(AgsLv2UridManager *lv2_urid_manager,
			    gchar *uri)
{
  GValue *id;

  pthread_mutex_t *lv2_urid_manager_mutex;

  /* get lv2 uri map manager mutex */
  pthread_mutex_lock(ags_lv2_urid_manager_get_class_mutex());
  
  lv2_urid_manager_mutex = lv2_urid_manager->obj_mutex;
  
  pthread_mutex_unlock(ags_lv2_urid_manager_get_class_mutex());

  /*  */
  pthread_mutex_lock(lv2_urid_manager_mutex);

  id = g_hash_table_lookup(lv2_urid_manager->urid,
			   uri);

  if(id != NULL){
    g_hash_table_remove(lv2_urid_manager->urid,
			uri);
  }

  pthread_mutex_unlock(lv2_urid_manager_mutex);
  
  return(TRUE);
}

/**
 * ags_lv2_urid_manager_lookup:
 * @lv2_urid_manager: the #AgsLv2UridManager
 * @uri: the object to lookup
 *
 * Lookup a id associated with @urid in @lv2_urid_manager
 *
 * Returns: the id on success, else the newly created id
 *
 * Since: 2.0.0
 */
GValue*
ags_lv2_urid_manager_lookup(AgsLv2UridManager *lv2_urid_manager,
			    gchar *uri)
{
  GValue *value;

  pthread_mutex_t *lv2_urid_manager_mutex;

  /* get lv2 uri map manager mutex */
  pthread_mutex_lock(ags_lv2_urid_manager_get_class_mutex());
  
  lv2_urid_manager_mutex = lv2_urid_manager->obj_mutex;
  
  pthread_mutex_unlock(ags_lv2_urid_manager_get_class_mutex());

  /*  */
  pthread_mutex_lock(lv2_urid_manager_mutex);

  value = (GValue *) g_hash_table_lookup(lv2_urid_manager->urid,
					 uri);
  
  pthread_mutex_unlock(lv2_urid_manager_mutex);

  if(value == NULL){
    g_message("new uri %s", uri);
    
    value = g_new0(GValue,
		   1);
    g_value_init(value,
		 G_TYPE_ULONG);

    g_value_set_ulong(value,
		      lv2_urid_manager->id_counter);
    ags_lv2_urid_manager_insert(lv2_urid_manager,
				uri,
				value);

    lv2_urid_manager->id_counter++;
  }
  
  pthread_mutex_unlock(lv2_urid_manager_mutex);
    
  return(value);
}

void
ags_lv2_urid_manager_load_default(AgsLv2UridManager *lv2_urid_manager)
{
  GValue *value;
  gchar **str;
  
  static const gchar *default_uri[] = {
    NULL,
  };

  str = default_uri;

  while(str[0] != NULL){
    value = g_new0(GValue,
		   1);
    g_value_init(value,
		 G_TYPE_ULONG);

    g_value_set_ulong(value,
		      lv2_urid_manager->id_counter);
    ags_lv2_urid_manager_insert(lv2_urid_manager,
				*str,
				value);

    lv2_urid_manager->id_counter++;
    str++;
  }
}

uint32_t
ags_lv2_urid_manager_map(LV2_URID_Map_Handle handle,
			 char *uri)
{
  GValue *value;
  uint32_t id;

  value = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
				      uri);
  id = g_value_get_ulong(value);
  
  return(id);
}

const char*
ags_lv2_urid_manager_unmap(LV2_URID_Map_Handle handle,
			   uint32_t urid)
{
  AgsLv2UridManager *lv2_urid_manager;

  GList *key, *key_start;
  
  gpointer data, tmp;
  
  lv2_urid_manager = ags_lv2_urid_manager_get_instance();
  
  key_start = 
    key = g_hash_table_get_keys(lv2_urid_manager->urid);

  data = NULL;

  while(key != NULL){
    tmp = (gpointer) g_hash_table_lookup(lv2_urid_manager->urid,
					 key->data);

    if(urid == g_value_get_ulong((GValue *) tmp)){
      ags_lv2_urid_manager_remove(lv2_urid_manager,
				  key->data);
      data = tmp;

      break;
    }

    key = key->next;
  }
  
  g_list_free(key_start);
  
  return(data);
}

/**
 * ags_lv2_urid_manager_get_instance:
 * 
 * Singleton function to optain the id manager instance.
 *
 * Returns: an instance of #AgsLv2UridManager
 *
 * Since: 2.0.0
 */
AgsLv2UridManager*
ags_lv2_urid_manager_get_instance()
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&mutex);

  if(ags_lv2_urid_manager == NULL){
    ags_lv2_urid_manager = ags_lv2_urid_manager_new();
    
    //    ags_lv2_urid_manager_load_default(ags_lv2_urid_manager);
  }

  pthread_mutex_unlock(&mutex);

  return(ags_lv2_urid_manager);
}

/**
 * ags_lv2_urid_manager_new:
 *
 * Instantiate a id manager. 
 *
 * Returns: a new #AgsLv2UridManager
 *
 * Since: 2.0.0
 */
AgsLv2UridManager*
ags_lv2_urid_manager_new()
{
  AgsLv2UridManager *lv2_urid_manager;

  lv2_urid_manager = (AgsLv2UridManager *) g_object_new(AGS_TYPE_LV2_URID_MANAGER,
							NULL);

  return(lv2_urid_manager);
}
