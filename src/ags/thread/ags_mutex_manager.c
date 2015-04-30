/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/thread/ags_mutex_manager.h>

#include <ags/object/ags_connectable.h>

void ags_mutex_manager_class_init(AgsMutexManagerClass *mutex_manager);
void ags_mutex_manager_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_mutex_manager_init(AgsMutexManager *mutex_manager);
void ags_mutex_manager_connect(AgsConnectable *connectable);
void ags_mutex_manager_disconnect(AgsConnectable *connectable);
void ags_mutex_manager_finalize(GObject *gobject);

void ags_mutex_manager_destroy_data(gpointer data);

static gpointer ags_mutex_manager_parent_class = NULL;

AgsMutexManager *ags_mutex_manager = NULL;

GType
ags_mutex_manager_get_type()
{
  static GType ags_type_mutex_manager = 0;

  if(!ags_type_mutex_manager){
    const GTypeInfo ags_mutex_manager_info = {
      sizeof (AgsMutexManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_mutex_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMutexManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_mutex_manager_init,
    };

    const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_mutex_manager_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_mutex_manager = g_type_register_static(G_TYPE_OBJECT,
						    "AgsMutexManager\0",
						    &ags_mutex_manager_info,
						    0);

    g_type_add_interface_static(ags_type_mutex_manager,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return(ags_type_mutex_manager);
}

void
ags_mutex_manager_class_init(AgsMutexManagerClass *mutex_manager)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_mutex_manager_parent_class = g_type_class_peek_parent(mutex_manager);

  /* GObject */
  gobject = (GObjectClass *) mutex_manager;

  gobject->finalize = ags_mutex_manager_finalize;
}

void
ags_mutex_manager_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_mutex_manager_connect;
  connectable->disconnect = ags_mutex_manager_disconnect;
}

void
ags_mutex_manager_init(AgsMutexManager *mutex_manager)
{
  mutex_manager->lock_object = g_hash_table_new_full(g_direct_hash, g_direct_equal,
						     NULL,
						     (GDestroyNotify) ags_mutex_manager_destroy_data);
}

void
ags_mutex_manager_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_mutex_manager_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_mutex_manager_finalize(GObject *gobject)
{
  AgsMutexManager *mutex_manager;

  mutex_manager = AGS_MUTEX_MANAGER(gobject);

  g_hash_table_destroy(mutex_manager->lock_object);
}

void
ags_mutex_manager_destroy_data(gpointer data)
{
  pthread_mutex_destroy((pthread_mutex_t *) data);
}

gboolean
ags_mutex_manager_insert(AgsMutexManager *mutex_manager,
			 GObject *lock_object, pthread_mutex_t *mutex)
{
  return(g_hash_table_insert(mutex_manager->lock_object,
			     lock_object, mutex));
}

gboolean
ags_mutex_manager_remove(AgsMutexManager *mutex_manager,
			 GObject *lock_object)
{
  pthread_mutex_t *mutex;

  mutex = g_hash_table_lookup(mutex_manager->lock_object,
			      lock_object);

  if(mutex == NULL ||
     pthread_mutex_trylock(mutex) != 0){
    return(FALSE);
  }

  g_hash_table_remove(mutex_manager->lock_object,
		      lock_object);

  pthread_mutex_unlock(mutex);
  
  return(TRUE);
}

pthread_mutex_t*
ags_mutex_manager_lookup(AgsMutexManager *mutex_manager,
			 GObject *lock_object)
{
  pthread_mutex_t *mutex;

  mutex = (pthread_mutex_t *) g_hash_table_lookup(mutex_manager->lock_object,
						  lock_object);

  return(mutex);
}

AgsMutexManager*
ags_mutex_manager_get_instance()
{
  if(ags_mutex_manager == NULL){
    ags_mutex_manager = ags_mutex_manager_new();
  }

  return(ags_mutex_manager);
}

AgsMutexManager*
ags_mutex_manager_new()
{
  AgsMutexManager *mutex_manager;

  mutex_manager = (AgsMutexManager *) g_object_new(AGS_TYPE_MUTEX_MANAGER,
						   NULL);

  return(mutex_manager);
}

