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

#include <ags/server/security/ags_authentication_manager.h>

void ags_authentication_manager_class_init(AgsAuthenticationManagerClass *authentication_manager);
void ags_authentication_manager_init (AgsAuthenticationManager *authentication_manager);
void ags_authentication_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_authentication_manager
 * @short_description: Singleton pattern to organize authentication
 * @title: AgsAuthenticationManager
 * @section_id:
 * @include: ags/server/security/ags_authentication_manager.h
 *
 * The #AgsAuthenticationManager manages your authentication.
 */

static gpointer ags_authentication_manager_parent_class = NULL;

AgsAuthenticationManager *ags_authentication_manager = NULL;

GType
ags_authentication_manager_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_authentication_manager = 0;

    static const GTypeInfo ags_authentication_manager_info = {
      sizeof (AgsAuthenticationManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_authentication_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAuthenticationManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_authentication_manager_init,
    };

    ags_type_authentication_manager = g_type_register_static(G_TYPE_OBJECT,
							     "AgsAuthenticationManager",
							     &ags_authentication_manager_info,
							     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_authentication_manager);
  }

  return g_define_type_id__volatile;
}

void
ags_authentication_manager_class_init(AgsAuthenticationManagerClass *authentication_manager)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_authentication_manager_parent_class = g_type_class_peek_parent(authentication_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) authentication_manager;

  gobject->finalize = ags_authentication_manager_finalize;
}

void
ags_authentication_manager_init(AgsAuthenticationManager *authentication_manager)
{
  g_rec_mutex_init(&(authentication_manager->obj_mutex));

  authentication_manager->authentication = NULL;

  authentication_manager->login = g_hash_table_new_full(g_direct_hash,
							g_str_equal,
							g_free,
							g_free);

  authentication_manager->user_uuid = g_hash_table_new_full(g_direct_hash,
							    g_str_equal,
							    g_free,
							    g_object_unref);
}

void
ags_authentication_manager_finalize(GObject *gobject)
{
  AgsAuthenticationManager *authentication_manager;

  authentication_manager = AGS_AUTHENTICATION_MANAGER(gobject);

  if(authentication_manager->authentication != NULL){
    g_list_free_full(authentication_manager->authentication,
		     g_object_unref);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_authentication_manager_parent_class)->finalize(gobject);
}

/**
 * ags_authentication_manager_get_authentication:
 * @authentication_manager: the #AgsAuthenticationManager
 * 
 * Get authentication.
 * 
 * Returns: the #GList-struct containing #GObject implementing #AgsAuthentication
 * 
 * Since: 3.0.0
 */
GList*
ags_authentication_manager_get_authentication(AgsAuthenticationManager *authentication_manager)
{
  GList *authentication;

  GRecMutex *authentication_manager_mutex;
  
  if(!AGS_IS_AUTHENTICATION_MANAGER(authentication_manager)){
    return(NULL);
  }

  /* get authentication manager mutex */
  authentication_manager_mutex = AGS_AUTHENTICATION_MANAGER_GET_OBJ_MUTEX(authentication_manager);

  /* get authentication */
  g_rec_mutex_lock(authentication_manager_mutex);

  authentication = g_list_copy_deep(authentication_manager->authentication,
				    g_object_ref,
				    NULL);

  g_rec_mutex_unlock(authentication_manager_mutex);
  
  return(authentication);
}

/**
 * ags_authentication_manager_add_authentication:
 * @authentication_manager: the #AgsAuthenticationManager
 * @authentication: the #GObject implementing #AgsAuthentication
 * 
 * Add @authentication to @authentication_manager.
 * 
 * Since: 3.0.0
 */
void
ags_authentication_manager_add_authentication(AgsAuthenticationManager *authentication_manager,
					      GObject *authentication)
{
  GRecMutex *authentication_manager_mutex;
  
  if(!AGS_IS_AUTHENTICATION_MANAGER(authentication_manager) ||
     !AGS_IS_AUTHENTICATION(authentication)){
    return;
  }

  /* get authentication manager mutex */
  authentication_manager_mutex = AGS_AUTHENTICATION_MANAGER_GET_OBJ_MUTEX(authentication_manager);

  /* add authentication */
  g_rec_mutex_lock(authentication_manager_mutex);

  if(g_list_find(authentication_manager->authentication, authentication) == NULL){
    authentication_manager->authentication = g_list_prepend(authentication_manager->authentication,
							    authentication);
    g_object_ref(authentication);
  }

  g_rec_mutex_unlock(authentication_manager_mutex);
}

/**
 * ags_authentication_manager_remove_authentication:
 * @authentication_manager: the #AgsAuthenticationManager
 * @authentication: the #GObject implementing #AgsAuthentication
 * 
 * Remove @authentication from @authentication_manager.
 * 
 * Since: 3.0.0
 */
void
ags_authentication_manager_remove_authentication(AgsAuthenticationManager *authentication_manager,
						 GObject *authentication)
{
  GRecMutex *authentication_manager_mutex;
  
  if(!AGS_IS_AUTHENTICATION_MANAGER(authentication_manager) ||
     !AGS_IS_AUTHENTICATION(authentication)){
    return;
  }

  /* get authentication manager mutex */
  authentication_manager_mutex = AGS_AUTHENTICATION_MANAGER_GET_OBJ_MUTEX(authentication_manager);

  /* remove authentication */
  g_rec_mutex_lock(authentication_manager_mutex);

  if(g_list_find(authentication_manager->authentication, authentication) != NULL){
    authentication_manager->authentication = g_list_remove(authentication_manager->authentication,
							   authentication);
    g_object_unref(authentication);
  }

  g_rec_mutex_unlock(authentication_manager_mutex);
}

/**
 * ags_authentication_manager_lookup_login:
 * @authentication_manager: the #AgsAuthenticationManager
 * @login: the login
 * 
 * Lookup @login.
 * 
 * Returns: the user UUID or %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_authentication_manager_lookup_login(AgsAuthenticationManager *authentication_manager,
					gchar *login)
{
  gchar *user_uuid;
  
  GRecMutex *authentication_manager_mutex;
  
  if(!AGS_IS_AUTHENTICATION_MANAGER(authentication_manager) ||
     login == NULL){
    return(NULL);
  }

  /* get authentication manager mutex */
  authentication_manager_mutex = AGS_AUTHENTICATION_MANAGER_GET_OBJ_MUTEX(authentication_manager);
  
  /* lookup login */
  g_rec_mutex_lock(authentication_manager_mutex);

  user_uuid = g_hash_table_lookup(authentication_manager->login,
				  login);

  if(user_uuid != NULL){
    user_uuid = g_strdup(user_uuid);
  }
  
  g_rec_mutex_unlock(authentication_manager_mutex);
  
  return(user_uuid);
}

/**
 * ags_authentication_manager_insert_login:
 * @authentication_manager: the #AgsAuthenticationManager
 * @login: the login
 * @user_uuid: the user UUID
 * 
 * Insert @login as key and @user_uuid as its value.
 * 
 * Since: 3.0.0
 */
void
ags_authentication_manager_insert_login(AgsAuthenticationManager *authentication_manager,
					gchar *login,
					gchar *user_uuid)
{
  GRecMutex *authentication_manager_mutex;
  
  if(!AGS_IS_AUTHENTICATION_MANAGER(authentication_manager) ||
     login == NULL ||
     user_uuid == NULL){
    return;
  }

  /* get authentication manager mutex */
  authentication_manager_mutex = AGS_AUTHENTICATION_MANAGER_GET_OBJ_MUTEX(authentication_manager);

  /* insert login and user uuid */
  g_rec_mutex_lock(authentication_manager_mutex);

  g_hash_table_insert(authentication_manager->login,
		      g_strdup(login),
		      g_strdup(user_uuid));
  
  g_rec_mutex_unlock(authentication_manager_mutex);
}

/**
 * ags_authentication_manager_remove_login:
 * @authentication_manager: the #AgsAuthenticationManager
 * @login: the login
 * 
 * Remove @login.
 * 
 * Since: 3.0.0
 */
void
ags_authentication_manager_remove_login(AgsAuthenticationManager *authentication_manager,
					gchar *login)
{
  GRecMutex *authentication_manager_mutex;
  
  if(!AGS_IS_AUTHENTICATION_MANAGER(authentication_manager) ||
     login == NULL){
    return;
  }

  /* get authentication manager mutex */
  authentication_manager_mutex = AGS_AUTHENTICATION_MANAGER_GET_OBJ_MUTEX(authentication_manager);
  
  /* remove login */
  g_rec_mutex_lock(authentication_manager_mutex);

  g_hash_table_remove(authentication_manager->login,
		      login);
  
  g_rec_mutex_unlock(authentication_manager_mutex);
}

/**
 * ags_authentication_manager_lookup_user_uuid:
 * @authentication_manager: the #AgsAuthenticationManager
 * @user_uuid: the user UUID
 * 
 * Lookup @user_uuid.
 * 
 * Returns: the #AgsSecurityContext or %NULL
 * 
 * Since: 3.0.0
 */
AgsSecurityContext*
ags_authentication_manager_lookup_user_uuid(AgsAuthenticationManager *authentication_manager,
					    gchar *user_uuid)
{
  AgsSecurityContext *security_context;
  
  GRecMutex *authentication_manager_mutex;
  
  if(!AGS_IS_AUTHENTICATION_MANAGER(authentication_manager) ||
     user_uuid == NULL){
    return(NULL);
  }

  /* get authentication manager mutex */
  authentication_manager_mutex = AGS_AUTHENTICATION_MANAGER_GET_OBJ_MUTEX(authentication_manager);
  
  /* lookup login */
  g_rec_mutex_lock(authentication_manager_mutex);

  security_context = g_hash_table_lookup(authentication_manager->user_uuid,
					 user_uuid);

  if(security_context != NULL){
    g_object_ref(security_context);
  }
  
  g_rec_mutex_unlock(authentication_manager_mutex);
  
  return(security_context);
}

/**
 * ags_authentication_manager_insert_uuid:
 * @authentication_manager: the #AgsAuthenticationManager
 * @user_uuid: the user UUID
 * @security_context: the #AgsSecurityContext
 * 
 * Insert @user_uuid as key and @security_context as its value.
 * 
 * Since: 3.0.0
 */
void
ags_authentication_manager_insert_uuid(AgsAuthenticationManager *authentication_manager,
				       gchar *user_uuid, AgsSecurityContext *security_context)
{
  GRecMutex *authentication_manager_mutex;
  
  if(!AGS_IS_AUTHENTICATION_MANAGER(authentication_manager) ||
     user_uuid == NULL ||
     !AGS_IS_SECURITY_CONTEXT(security_context)){
    return;
  }

  /* get authentication manager mutex */
  authentication_manager_mutex = AGS_AUTHENTICATION_MANAGER_GET_OBJ_MUTEX(authentication_manager);

  /* insert login and user uuid */
  g_rec_mutex_lock(authentication_manager_mutex);

  g_object_ref(security_context);
  
  g_hash_table_insert(authentication_manager->user_uuid,
		      g_strdup(user_uuid),
		      security_context);
  
  g_rec_mutex_unlock(authentication_manager_mutex);
}

/**
 * ags_authentication_manager_remove_uuid:
 * @authentication_manager: the #AgsAuthenticationManager
 * @user_uuid: user UUID
 * 
 * Remove @user_uuid.
 * 
 * Since: 3.0.0
 */
void
ags_authentication_manager_remove_uuid(AgsAuthenticationManager *authentication_manager,
				       gchar *user_uuid)
{
  GRecMutex *authentication_manager_mutex;
  
  if(!AGS_IS_AUTHENTICATION_MANAGER(authentication_manager) ||
     user_uuid == NULL){
    return;
  }

  /* get authentication manager mutex */
  authentication_manager_mutex = AGS_AUTHENTICATION_MANAGER_GET_OBJ_MUTEX(authentication_manager);
  
  /* remove user UUID */
  g_rec_mutex_lock(authentication_manager_mutex);

  g_hash_table_remove(authentication_manager->user_uuid,
		      user_uuid);
  
  g_rec_mutex_unlock(authentication_manager_mutex);
}

/**
 * ags_authentication_manager_login:
 * @authentication_manager: the #AgsAuthenticationManager
 * @authentication_module: the authentication module
 * @login: the login
 * @password: the password
 * @user_uuid: return location of user UUID
 * @security_token: return location of security token
 * 
 * Login.
 *
 * Returns: %TRUE if login was successful, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_authentication_manager_login(AgsAuthenticationManager *authentication_manager,
				 gchar *authentication_module,
				 gchar *login,
				 gchar *password,
				 gchar **user_uuid,
				 gchar **security_token)
{
  GList *start_authentication, *authentication;

  gchar *current_token, *current_uuid;
  
  authentication =
    start_authentication = ags_authentication_manager_get_authentication(authentication_manager);

  current_uuid = NULL;
  current_token = NULL;
  
  while(authentication != NULL){
    GError *error;

    error = NULL;
    
    if(g_strv_contains(ags_authentication_get_authentication_module(AGS_AUTHENTICATION(authentication->data)),
		       authentication_module) &&
       ags_authentication_login(AGS_AUTHENTICATION(authentication->data),
				login, password,
				&current_uuid, &current_token,
				&error)){
      if(user_uuid != NULL){
	*user_uuid = current_uuid;
      }

      if(security_token != NULL){
	*security_token = current_token;
      }

      g_list_free_full(start_authentication,
		       g_object_unref);
      
      return(TRUE);
    }

    if(error != NULL){
      g_warning("%s", error->message);

      g_error_free(error);
    }
    
    authentication = authentication->next;
  }

  g_list_free_full(start_authentication,
		   g_object_unref);
  
  return(FALSE);
}

/**
 * ags_authentication_manager_get_digest:
 * @authentication_manager: the #AgsAuthenticationManager
 * @authentication_module: the authentication module
 * @realm: the realm
 * @login: the login
 * 
 * Get digest of @login.
 * 
 * Returns: the digest as string, or %NULL if not available
 * 
 * Since: 3.0.0
 */
gchar*
ags_authentication_manager_get_digest(AgsAuthenticationManager *authentication_manager,
				      gchar *authentication_module,
				      gchar *realm,
				      gchar *login,
				      gchar *security_token)
{
  GList *start_authentication, *authentication;

  gchar *current_digest;
  
  authentication =
    start_authentication = ags_authentication_manager_get_authentication(authentication_manager);

  while(authentication != NULL){
    if(g_strv_contains(ags_authentication_get_authentication_module(AGS_AUTHENTICATION(authentication->data)),
		       authentication_module)){
      GError *error;

      error = NULL;
    
      current_digest = ags_authentication_get_digest(AGS_AUTHENTICATION(authentication->data),
						     realm,
						     login,
						     security_token,
						     &error);

      if(error != NULL){
	g_warning("%s", error->message);

	g_error_free(error);
      }

      g_list_free_full(start_authentication,
		       g_object_unref);
      
      return(current_digest);
    }
    
    authentication = authentication->next;
  }

  g_list_free_full(start_authentication,
		   g_object_unref);

  return(NULL);
}

/**
 * ags_authentication_manager_is_session_active:
 * @authentication_manager: the #AgsAuthenticationManager
 * @security_context: the security context
 * @login: the login
 * @security_token: the security token
 * 
 * Check if session is active.
 *
 * Returns: %TRUE if active, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_authentication_manager_is_session_active(AgsAuthenticationManager *authentication_manager,
					     GObject *security_context,
					     gchar *login,
					     gchar *security_token)
{
  GList *start_authentication, *authentication;
  
  authentication =
    start_authentication = ags_authentication_manager_get_authentication(authentication_manager);
  
  while(authentication != NULL){
    GError *error;

    error = NULL;
    
    if(ags_authentication_is_session_active(AGS_AUTHENTICATION(authentication->data),
					    security_context,
					    login,
					    security_token,
					    &error)){
      g_list_free_full(start_authentication,
		       g_object_unref);

      return(TRUE);
    }

    if(error != NULL){
      g_warning("%s", error->message);

      g_error_free(error);
    }
    
    authentication = authentication->next;
  }

  g_list_free_full(start_authentication,
		   g_object_unref);
  
  return(FALSE);
}

/**
 * ags_authentication_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: the #AgsAuthenticationManager
 *
 * Since: 3.0.0
 */
AgsAuthenticationManager*
ags_authentication_manager_get_instance()
{
  static GMutex mutex;

  g_mutex_lock(&mutex);

  if(ags_authentication_manager == NULL){
    ags_authentication_manager = ags_authentication_manager_new();
  }
  
  g_mutex_unlock(&mutex);

  return(ags_authentication_manager);
}

/**
 * ags_authentication_manager_new:
 *
 * Creates an #AgsAuthenticationManager
 *
 * Returns: a new #AgsAuthenticationManager
 *
 * Since: 3.0.0
 */
AgsAuthenticationManager*
ags_authentication_manager_new()
{
  AgsAuthenticationManager *authentication_manager;

  authentication_manager = (AgsAuthenticationManager *) g_object_new(AGS_TYPE_AUTHENTICATION_MANAGER,
								     NULL);

  return(authentication_manager);
}
