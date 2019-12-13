/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/server/security/ags_business_group_manager.h>

void ags_business_group_manager_class_init(AgsBusinessGroupManagerClass *business_group_manager);
void ags_business_group_manager_init (AgsBusinessGroupManager *business_group_manager);
void ags_business_group_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_business_group_manager
 * @short_description: Singleton pattern to organize business group
 * @title: AgsBusinessGroupManager
 * @section_id:
 * @include: ags/server/security/ags_business_group_manager.h
 *
 * The #AgsBusinessGroupManager manages your business group.
 */

static gpointer ags_business_group_manager_parent_class = NULL;

AgsBusinessGroupManager *ags_business_group_manager = NULL;

GType
ags_business_group_manager_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_business_group_manager = 0;

    static const GTypeInfo ags_business_group_manager_info = {
      sizeof (AgsBusinessGroupManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_business_group_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsBusinessGroupManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_business_group_manager_init,
    };

    ags_type_business_group_manager = g_type_register_static(G_TYPE_OBJECT,
							     "AgsBusinessGroupManager",
							     &ags_business_group_manager_info,
							     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_business_group_manager);
  }

  return g_define_type_id__volatile;
}

void
ags_business_group_manager_class_init(AgsBusinessGroupManagerClass *business_group_manager)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_business_group_manager_parent_class = g_type_class_peek_parent(business_group_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) business_group_manager;

  gobject->finalize = ags_business_group_manager_finalize;
}

void
ags_business_group_manager_init(AgsBusinessGroupManager *business_group_manager)
{
  g_rec_mutex_init(&(business_group_manager->obj_mutex));

  business_group_manager->business_group = NULL;
}

void
ags_business_group_manager_finalize(GObject *gobject)
{
  AgsBusinessGroupManager *business_group_manager;

  business_group_manager = AGS_BUSINESS_GROUP_MANAGER(gobject);

  if(business_group_manager->business_group != NULL){
    g_list_free_full(business_group_manager->business_group,
		     g_object_unref);
  }

  G_OBJECT_CLASS(ags_business_group_manager_parent_class)->finalize(gobject);
}

/**
 * ags_business_group_manager_get_business_group:
 * @business_group_manager: the #AgsBusinessGroupManager
 * 
 * Get business_group.
 * 
 * Returns: the #GList-struct containing #GObject implementing #AgsBusinessGroup
 * 
 * Since: 3.0.0
 */
GList*
ags_business_group_manager_get_business_group(AgsBusinessGroupManager *business_group_manager)
{
  GList *business_group;

  GRecMutex *business_group_manager_mutex;
  
  if(!AGS_IS_BUSINESS_GROUP_MANAGER(business_group_manager)){
    return(NULL);
  }

  /* get business_group manager mutex */
  business_group_manager_mutex = AGS_BUSINESS_GROUP_MANAGER_GET_OBJ_MUTEX(business_group_manager);

  /* get business_group */
  g_rec_mutex_lock(business_group_manager_mutex);

  business_group = g_list_copy_deep(business_group_manager->business_group,
				    g_object_ref,
				    NULL);

  g_rec_mutex_unlock(business_group_manager_mutex);
  
  return(business_group);
}

/**
 * ags_business_group_manager_add_business_group:
 * @business_group_manager: the #AgsBusinessGroupManager
 * @business_group: the #GObject implementing #AgsBusinessGroup
 * 
 * Add @business_group to @business_group_manager.
 * 
 * Since: 3.0.0
 */
void
ags_business_group_manager_add_business_group(AgsBusinessGroupManager *business_group_manager,
					      GObject *business_group)
{
  GRecMutex *business_group_manager_mutex;
  
  if(!AGS_IS_BUSINESS_GROUP_MANAGER(business_group_manager) ||
     !AGS_IS_BUSINESS_GROUP(business_group)){
    return;
  }

  /* get business_group manager mutex */
  business_group_manager_mutex = AGS_BUSINESS_GROUP_MANAGER_GET_OBJ_MUTEX(business_group_manager);

  /* add business_group */
  g_rec_mutex_lock(business_group_manager_mutex);

  if(g_list_find(business_group_manager->business_group, business_group) == NULL){
    business_group_manager->business_group = g_list_prepend(business_group_manager->business_group,
							    business_group);
    g_object_ref(business_group);
  }

  g_rec_mutex_unlock(business_group_manager_mutex);
}

/**
 * ags_business_group_manager_remove_business_group:
 * @business_group_manager: the #AgsBusinessGroupManager
 * @business_group: the #GObject implementing #AgsBusinessGroup
 * 
 * Remove @business_group from @business_group_manager.
 * 
 * Since: 3.0.0
 */
void
ags_business_group_manager_remove_business_group(AgsBusinessGroupManager *business_group_manager,
						 GObject *business_group)
{
  GRecMutex *business_group_manager_mutex;
  
  if(!AGS_IS_BUSINESS_GROUP_MANAGER(business_group_manager) ||
     !AGS_IS_BUSINESS_GROUP(business_group)){
    return;
  }

  /* get business_group manager mutex */
  business_group_manager_mutex = AGS_BUSINESS_GROUP_MANAGER_GET_OBJ_MUTEX(business_group_manager);

  /* remove business_group */
  g_rec_mutex_lock(business_group_manager_mutex);

  if(g_list_find(business_group_manager->business_group, business_group) != NULL){
    business_group_manager->business_group = g_list_remove(business_group_manager->business_group,
							   business_group);
    g_object_unref(business_group);
  }

  g_rec_mutex_unlock(business_group_manager_mutex);
}

/**
 * ags_business_group_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: the #AgsBusinessGroupManager
 *
 * Since: 3.0.0
 */
AgsBusinessGroupManager*
ags_business_group_manager_get_instance()
{
  static GRecMutex mutex;

  g_mutex_lock(&mutex);

  if(ags_business_group_manager == NULL){
    ags_business_group_manager = ags_business_group_manager_new();
  }
  
  g_mutex_unlock(&mutex);

  return(ags_business_group_manager);
}

/**
 * ags_business_group_manager_new:
 *
 * Creates an #AgsBusinessGroupManager
 *
 * Returns: a new #AgsBusinessGroupManager
 *
 * Since: 3.0.0
 */
AgsBusinessGroupManager*
ags_business_group_manager_new()
{
  AgsBusinessGroupManager *business_group_manager;

  business_group_manager = (AgsBusinessGroupManager *) g_object_new(AGS_TYPE_BUSINESS_GROUP_MANAGER,
								    NULL);

  return(business_group_manager);
}
