/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/thread/ags_condition_manager.h>

#include <ags/object/ags_connectable.h>

void ags_condition_manager_class_init(AgsConditionManagerClass *condition_manager);
void ags_condition_manager_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_condition_manager_init(AgsConditionManager *condition_manager);
void ags_condition_manager_connect(AgsConnectable *connectable);
void ags_condition_manager_disconnect(AgsConnectable *connectable);
void ags_condition_manager_finalize(GObject *gobject);

void ags_condition_manager_destroy_data(gpointer data);

/**
 * SECTION:ags_condition_manager
 * @short_description: condition manager
 * @title: AgsConditionManager
 * @section_id:
 * @include: ags/thread/ags_condition_manager.h
 *
 * The #AgsConditionManager keeps your condition in a hash table where you can lookup your
 * conditions.
 */

static gpointer ags_condition_manager_parent_class = NULL;

AgsConditionManager *ags_condition_manager = NULL;

GType
ags_condition_manager_get_type()
{
  static GType ags_type_condition_manager = 0;

  if(!ags_type_condition_manager){
    const GTypeInfo ags_condition_manager_info = {
      sizeof (AgsConditionManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_condition_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsConditionManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_condition_manager_init,
    };

    const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_condition_manager_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_condition_manager = g_type_register_static(G_TYPE_OBJECT,
							"AgsConditionManager\0",
							&ags_condition_manager_info,
							0);

    g_type_add_interface_static(ags_type_condition_manager,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return(ags_type_condition_manager);
}

void
ags_condition_manager_class_init(AgsConditionManagerClass *condition_manager)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_condition_manager_parent_class = g_type_class_peek_parent(condition_manager);

  /* GObject */
  gobject = (GObjectClass *) condition_manager;

  gobject->finalize = ags_condition_manager_finalize;
}

void
ags_condition_manager_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_condition_manager_connect;
  connectable->disconnect = ags_condition_manager_disconnect;
}

void
ags_condition_manager_init(AgsConditionManager *condition_manager)
{
  condition_manager->lock_object = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							 NULL,
							 (GDestroyNotify) ags_condition_manager_destroy_data);
}

void
ags_condition_manager_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_condition_manager_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_condition_manager_finalize(GObject *gobject)
{
  AgsConditionManager *condition_manager;

  condition_manager = AGS_CONDITION_MANAGER(gobject);

  g_hash_table_destroy(condition_manager->lock_object);
}

void
ags_condition_manager_destroy_data(gpointer data)
{
  pthread_cond_destroy((pthread_cond_t *) data);
}

/**
 * ags_condition_manager_insert:
 * @condition_manager: the #AgsConditionManager
 * @lock_object: the object as key
 * @condition: the condition to insert
 *
 * Inserts a condition into hash map associated with @lock_object.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 0.7.122.8
 */
gboolean
ags_condition_manager_insert(AgsConditionManager *condition_manager,
			     GObject *lock_object, pthread_cond_t *cond)
{
  if(condition_manager == NULL ||
     lock_object == NULL ||
     condition == NULL){
    return(FALSE);
  }

  g_hash_table_insert(condition_manager->lock_object,
		      lock_object, cond);

  return(TRUE);
}

/**
 * ags_condition_manager_remove:
 * @condition_manager: the #AgsConditionManager
 * @lock_object: the object to remove
 * 
 * Removes a lock associated with @lock_object.3
 *
 * Returns: %TRUE as successfully removed, otherwise %FALSE
 *
 * Since: 0.7.122.8
 */
gboolean
ags_condition_manager_remove(AgsConditionManager *condition_manager,
			     GObject *lock_object)
{
  pthread_cond_t *cond;

  cond = g_hash_table_lookup(condition_manager->lock_object,
			     lock_object);

  if(cond == NULL){
    return(FALSE);
  }

  g_hash_table_remove(condition_manager->lock_object,
		      lock_object);

  free(cond);
  
  return(TRUE);
}

/**
 * ags_condition_manager_lookup:
 * @condition_manager: the #AgsConditionManager
 * @lock_object: the object to lookup
 *
 * Lookup a condition associated with @lock_object in @condition_manager
 *
 * Returns: the condition on success, else %NULL
 *
 * Since: 0.7.122.8
 */
pthread_cond_t*
ags_condition_manager_lookup(AgsConditionManager *condition_manager,
			     GObject *lock_object)
{
  pthread_cond_t *cond;

  cond = (pthread_cond_t *) g_hash_table_lookup(condition_manager->lock_object,
						lock_object);

  return(cond);
}

/**
 * ags_condition_manager_get_instance:
 * 
 * Singleton function to optain the condition manager instance.
 *
 * Returns: an instance of #AgsConditionManager
 *
 * Since: 0.7.122.8
 */
AgsConditionManager*
ags_condition_manager_get_instance()
{
  if(ags_condition_manager == NULL){
    ags_condition_manager = ags_condition_manager_new();
  }

  return(ags_condition_manager);
}

/**
 * ags_condition_manager_new:
 *
 * Instantiate a condition manager. 
 *
 * Returns: a new #AgsConditionManager
 *
 * Since: 0.7.122.8
 */
AgsConditionManager*
ags_condition_manager_new()
{
  AgsConditionManager *condition_manager;

  condition_manager = (AgsConditionManager *) g_object_new(AGS_TYPE_CONDITION_MANAGER,
							   NULL);

  return(condition_manager);
}
