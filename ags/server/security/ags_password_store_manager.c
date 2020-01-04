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

#include <ags/server/security/ags_password_store_manager.h>

#include <ags/server/security/ags_auth_security_context.h>

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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_password_store_manager = 0;

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
							     "AgsPasswordStoreManager",
							     &ags_password_store_manager_info,
							     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_password_store_manager);
  }

  return g_define_type_id__volatile;
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
  g_rec_mutex_init(&(password_store_manager->obj_mutex));

  password_store_manager->salt = NULL;
  
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

/**
 * ags_password_store_manager_get_password_store:
 * @password_store_manager: the #AgsPasswordStoreManager
 * 
 * Get password_store.
 * 
 * Returns: (element-type Glib.Object) (transfer container): the #GList-struct containing #GObject implementing #AgsPasswordStore
 * 
 * Since: 3.0.0
 */
GList*
ags_password_store_manager_get_password_store(AgsPasswordStoreManager *password_store_manager)
{
  GList *password_store;

  GRecMutex *password_store_manager_mutex;
  
  if(!AGS_IS_PASSWORD_STORE_MANAGER(password_store_manager)){
    return(NULL);
  }

  /* get password_store manager mutex */
  password_store_manager_mutex = AGS_PASSWORD_STORE_MANAGER_GET_OBJ_MUTEX(password_store_manager);

  /* get password_store */
  g_rec_mutex_lock(password_store_manager_mutex);

  password_store = g_list_copy_deep(password_store_manager->password_store,
				    g_object_ref,
				    NULL);

  g_rec_mutex_unlock(password_store_manager_mutex);
  
  return(password_store);
}

/**
 * ags_password_store_manager_add_password_store:
 * @password_store_manager: the #AgsPasswordStoreManager
 * @password_store: the #GObject implementing #AgsPasswordStore
 * 
 * Add @password_store to @password_store_manager.
 * 
 * Since: 3.0.0
 */
void
ags_password_store_manager_add_password_store(AgsPasswordStoreManager *password_store_manager,
					      GObject *password_store)
{
  GRecMutex *password_store_manager_mutex;
  
  if(!AGS_IS_PASSWORD_STORE_MANAGER(password_store_manager) ||
     !AGS_IS_PASSWORD_STORE(password_store)){
    return;
  }

  /* get password_store manager mutex */
  password_store_manager_mutex = AGS_PASSWORD_STORE_MANAGER_GET_OBJ_MUTEX(password_store_manager);

  /* add password_store */
  g_rec_mutex_lock(password_store_manager_mutex);

  if(g_list_find(password_store_manager->password_store, password_store) == NULL){
    password_store_manager->password_store = g_list_prepend(password_store_manager->password_store,
							    password_store);
    g_object_ref(password_store);
  }

  g_rec_mutex_unlock(password_store_manager_mutex);
}

/**
 * ags_password_store_manager_remove_password_store:
 * @password_store_manager: the #AgsPasswordStoreManager
 * @password_store: the #GObject implementing #AgsPasswordStore
 * 
 * Remove @password_store from @password_store_manager.
 * 
 * Since: 3.0.0
 */
void
ags_password_store_manager_remove_password_store(AgsPasswordStoreManager *password_store_manager,
						 GObject *password_store)
{
  GRecMutex *password_store_manager_mutex;
  
  if(!AGS_IS_PASSWORD_STORE_MANAGER(password_store_manager) ||
     !AGS_IS_PASSWORD_STORE(password_store)){
    return;
  }

  /* get password_store manager mutex */
  password_store_manager_mutex = AGS_PASSWORD_STORE_MANAGER_GET_OBJ_MUTEX(password_store_manager);

  /* remove password_store */
  g_rec_mutex_lock(password_store_manager_mutex);

  if(g_list_find(password_store_manager->password_store, password_store) != NULL){
    password_store_manager->password_store = g_list_remove(password_store_manager->password_store,
							   password_store);
    g_object_unref(password_store);
  }

  g_rec_mutex_unlock(password_store_manager_mutex);
}

/**
 * ags_password_store_manager_check_password:
 * @password_store_manager: the #AgsPasswordStoreManager
 * @user_uuid: the user uuid
 * @password: the password
 * 
 * Check @password to be valid for @user_uuid.
 * 
 * Returns: %TRUE if password matches, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_password_store_manager_check_password(AgsPasswordStoreManager *password_store_manager,
					  gchar *user_uuid,
					  gchar *password)
{
  GList *start_password_store, *password_store;

  gchar *salt;
  gchar *encrypted_password;
  
  gboolean success;

  GError *error;

  GRecMutex *password_store_manager_mutex;
  
  if(!AGS_IS_PASSWORD_STORE_MANAGER(password_store_manager) ||
     user_uuid == NULL ||
     password == NULL){
    return(FALSE);
  }

  /* get password_store manager mutex */
  password_store_manager_mutex = AGS_PASSWORD_STORE_MANAGER_GET_OBJ_MUTEX(password_store_manager);

  /* get salt */
  g_rec_mutex_lock(password_store_manager_mutex);

  salt = g_strdup(password_store_manager->salt);
  
  g_rec_mutex_unlock(password_store_manager_mutex);  

  /* check password */
  password_store =
    start_password_store = ags_password_store_manager_get_password_store(password_store_manager);

  success = FALSE;
  
  while(password_store != NULL){
    gchar *current_password;

    error = NULL;
    current_password = ags_password_store_get_password(AGS_PASSWORD_STORE(password_store->data),
						       ags_auth_security_context_get_instance(),
						       user_uuid,
						       NULL,
						       &error);

    if(error != NULL){
      g_critical("%s", error->message);

      g_error_free(error);
    }
    
    if(salt != NULL){
      error = NULL;
      encrypted_password = ags_password_store_encrypt_password(AGS_PASSWORD_STORE(password_store->data),
							       password,
							       salt,
							       &error);

      if(error != NULL){
	g_critical("%s", error->message);

	g_error_free(error);
      }
    }else{
      encrypted_password = g_strdup(password);
    }
    
    success = (current_password != NULL && !g_strcmp0(encrypted_password, current_password)) ? TRUE: FALSE;
    
    g_free(encrypted_password);
    g_free(current_password);

    if(success){
      break;
    }
    
    password_store = password_store->next;
  }
  
  g_list_free_full(start_password_store,
		   g_object_unref);
  g_free(salt);
  
  return(success);
}

/**
 * ags_password_store_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: the #AgsPasswordStoreManager
 *
 * Since: 3.0.0
 */
AgsPasswordStoreManager*
ags_password_store_manager_get_instance()
{
  static GRecMutex mutex;

  g_mutex_lock(&mutex);

  if(ags_password_store_manager == NULL){
    ags_password_store_manager = ags_password_store_manager_new();
  }
  
  g_mutex_unlock(&mutex);

  return(ags_password_store_manager);
}

/**
 * ags_password_store_manager_new:
 *
 * Creates an #AgsPasswordStoreManager
 *
 * Returns: a new #AgsPasswordStoreManager
 *
 * Since: 3.0.0
 */
AgsPasswordStoreManager*
ags_password_store_manager_new()
{
  AgsPasswordStoreManager *password_store_manager;

  password_store_manager = (AgsPasswordStoreManager *) g_object_new(AGS_TYPE_PASSWORD_STORE_MANAGER,
								    NULL);

  return(password_store_manager);
}
