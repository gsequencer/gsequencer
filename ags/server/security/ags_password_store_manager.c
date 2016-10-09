/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/server/security/ags_password_store_manager.h>

void ags_password_store_manager_class_init(AgsPasswordStoreManagerClass *password_store_manager);
void ags_password_store_manager_init (AgsPasswordStoreManager *password_store_manager);
void ags_password_store_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_password_store_manager
 * @short_description: Singleton pattern to organize password stores
 * @title: AgsPasswordStoreManager
 * @section_id:
 * @include: ags/server/security/ags_password_store_manager.h
 *
 * The #AgsPasswordStoreManager manages your password stores.
 */

static gpointer ags_password_store_manager_parent_class = NULL;

AgsPasswordStoreManager *ags_password_store_manager = NULL;

GType
ags_password_store_manager_get_type (void)
{
  static GType ags_type_password_store_manager = 0;

  if(!ags_type_password_store_manager){
    static const GTypeInfo ags_password_store_manager_info = {
      sizeof (AgsPasswordStoreManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_password_store_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPasswordStoreManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_password_store_manager_init,
    };

    ags_type_password_store_manager = g_type_register_static(G_TYPE_OBJECT,
							     "AgsPasswordStoreManager\0",
							     &ags_password_store_manager_info,
							     0);
  }

  return (ags_type_password_store_manager);
}

void
ags_password_store_manager_class_init(AgsPasswordStoreManagerClass *password_store_manager)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_password_store_manager_parent_class = g_type_class_peek_parent(password_store_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) password_store_manager;

  gobject->finalize = ags_password_store_manager_finalize;
}

void
ags_password_store_manager_init(AgsPasswordStoreManager *password_store_manager)
{
  password_store_manager->password_store = NULL;
}

void
ags_password_store_manager_finalize(GObject *gobject)
{
  AgsPasswordStoreManager *password_store_manager;

  password_store_manager = AGS_PASSWORD_STORE_MANAGER(gobject);

  if(password_store_manager->password_store != NULL){
    g_list_free_full(password_store_manager->password_store,
		     g_object_unref);
  }

  G_OBJECT_CLASS(ags_password_store_manager_parent_class)->finalize(gobject);
}

GList*
ags_password_store_manager_get_password_store(AgsPasswordStoreManager *password_store_manager)
{
  if(password_store_manager == NULL){
    password_store_manager = ags_password_store_manager_get_instance();
  }
  
  return(password_store_manager->password_store);
}

void
ags_password_store_manager_add_password_store(AgsPasswordStoreManager *password_store_manager,
					      GObject *password_store)
{
  if(password_store_manager == NULL){
    password_store_manager = ags_password_store_manager_get_instance();
  }

  password_store_manager->password_store = g_list_prepend(password_store_manager->password_store,
							  password_store);
}

void
ags_password_store_manager_remove_password_store(AgsPasswordStoreManager *password_store_manager,
						 GObject *password_store)
{
  if(password_store_manager == NULL){
    password_store_manager = ags_password_store_manager_get_instance();
  }

  password_store_manager->password_store = g_list_remove(password_store_manager->password_store,
							 password_store);
}

gboolean
ags_password_store_manager_check_password(AgsPasswordStoreManager *password_store_manager,
					  gchar *login,
					  gchar *password)
{
  //TODO:JK: implement me
  
  return(FALSE);
}

/**
 * ags_password_store_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: the #AgsPasswordStoreManager
 *
 * Since: 1.0.0
 */
AgsPasswordStoreManager*
ags_password_store_manager_get_instance()
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&(mutex));

  if(ags_password_store_manager == NULL){
    ags_password_store_manager = ags_password_store_manager_new();

    pthread_mutex_unlock(&(mutex));
  }else{
    pthread_mutex_unlock(&(mutex));
  }

  return(ags_password_store_manager);
}

/**
 * ags_password_store_manager_new:
 *
 * Creates an #AgsPasswordStoreManager
 *
 * Returns: a new #AgsPasswordStoreManager
 *
 * Since: 1.0.0
 */
AgsPasswordStoreManager*
ags_password_store_manager_new()
{
  AgsPasswordStoreManager *password_store_manager;

  password_store_manager = (AgsPasswordStoreManager *) g_object_new(AGS_TYPE_PASSWORD_STORE_MANAGER,
								    NULL);

  return(password_store_manager);
}
