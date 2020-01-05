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

#include <ags/server/security/ags_auth_security_context.h>

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

  authentication_manager->session_timeout = AGS_AUTHENTICATION_MANAGER_DEFAULT_SESSION_TIMEOUT;
  
  authentication_manager->login = g_hash_table_new_full(g_str_hash,
							g_str_equal,
							g_free,
							ags_login_info_unref);
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

  if(authentication_manager->login != NULL){
    g_hash_table_destroy(authentication_manager->login);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_authentication_manager_parent_class)->finalize(gobject);
}

/**
 * ags_login_info_alloc:
 * 
 * Alloc #AgsLoginInfo-struct.
 * 
 * Returns: the newly allocated #AgsLoginInfo-struct
 * 
 * Since: 3.0.0
 */
AgsLoginInfo*
ags_login_info_alloc()
{
  AgsLoginInfo *login_info;

  login_info = (AgsLoginInfo *) malloc(sizeof(AgsLoginInfo));

  login_info->ref_count = 1;
  login_info->active_session_count = 0;

  login_info->user_uuid = NULL;

  login_info->security_context = NULL;

  return(login_info);
}

/**
 * ags_login_info_free:
 * @login_info: the #AgsLoginInfo-struct
 * 
 * Free @login_info.
 * 
 * Since: 3.0.0
 */
void
ags_login_info_free(AgsLoginInfo *login_info)
{
  if(login_info == NULL){
    return;
  }

  g_free(login_info->user_uuid);

  if(login_info->security_context != NULL){
    g_object_unref(login_info->security_context);
  }
}

/**
 * ags_login_info_ref:
 * @login_info: the #AgsLoginInfo-struct
 * 
 * Increase ref-count of @login_info.
 * 
 * Since: 3.0.0
 */
void
ags_login_info_ref(AgsLoginInfo *login_info)
{
  if(login_info == NULL){
    return;
  }

  login_info->ref_count += 1;
}

/**
 * ags_login_info_unref:
 * @login_info: the #AgsLoginInfo-struct
 * 
 * Decrease ref-count of @login_info and free it if ref-count drops to 0.
 * 
 * Since: 3.0.0
 */
void
ags_login_info_unref(AgsLoginInfo *login_info)
{
  if(login_info == NULL){
    return;
  }

  login_info->ref_count -= 1;

  if(login_info->ref_count <= 0){
    ags_login_info_free(login_info);
  }
}

/**
 * ags_authentication_manager_get_authentication:
 * @authentication_manager: the #AgsAuthenticationManager
 * 
 * Get authentication.
 * 
 * Returns: (element-type GObject) (transfer full): the #GList-struct containing #GObject implementing #AgsAuthentication
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
 * ags_authentication_manager_get_session_timeout:
 * @authentication_manager: the #AgsAuthenticationManager
 * 
 * Get session timeout.
 * 
 * Returns: the session timeout
 * 
 * Since: 3.0.0
 */
gint64
ags_authentication_manager_get_session_timeout(AgsAuthenticationManager *authentication_manager)
{
  gint64 session_timeout;

  GRecMutex *authentication_manager_mutex;

  if(!AGS_IS_AUTHENTICATION_MANAGER(authentication_manager)){
    return(0);
  }

  /* get authentication manager mutex */
  authentication_manager_mutex = AGS_AUTHENTICATION_MANAGER_GET_OBJ_MUTEX(authentication_manager);

  /* get sesssion timeout */
  g_rec_mutex_lock(authentication_manager_mutex);

  session_timeout = authentication_manager->session_timeout;
  
  g_rec_mutex_unlock(authentication_manager_mutex);

  return(session_timeout);
}

/**
 * ags_authentication_manager_lookup_login:
 * @authentication_manager: the #AgsAuthenticationManager
 * @login: the login
 * 
 * Lookup @login.
 * 
 * Returns: the user #AgsLoginInfo-struct or %NULL
 * 
 * Since: 3.0.0
 */
AgsLoginInfo*
ags_authentication_manager_lookup_login(AgsAuthenticationManager *authentication_manager,
					gchar *login)
{
  AgsLoginInfo *login_info;
  
  GRecMutex *authentication_manager_mutex;
  
  if(!AGS_IS_AUTHENTICATION_MANAGER(authentication_manager) ||
     login == NULL){
    return(NULL);
  }

  /* get authentication manager mutex */
  authentication_manager_mutex = AGS_AUTHENTICATION_MANAGER_GET_OBJ_MUTEX(authentication_manager);
  
  /* lookup login */
  g_rec_mutex_lock(authentication_manager_mutex);

  login_info = g_hash_table_lookup(authentication_manager->login,
				   login);

  if(login_info != NULL){
    ags_login_info_ref(login_info);
  }
  
  g_rec_mutex_unlock(authentication_manager_mutex);
  
  return(login_info);
}

/**
 * ags_authentication_manager_insert_login:
 * @authentication_manager: the #AgsAuthenticationManager
 * @login: the login
 * @login_info: the #AgsLoginInfo-struct
 * 
 * Insert @login as key and @login_info as its value.
 * 
 * Since: 3.0.0
 */
void
ags_authentication_manager_insert_login(AgsAuthenticationManager *authentication_manager,
					gchar *login,
					AgsLoginInfo *login_info)
{
  GRecMutex *authentication_manager_mutex;
  
  if(!AGS_IS_AUTHENTICATION_MANAGER(authentication_manager) ||
     login == NULL ||
     login_info == NULL){
    return;
  }

  /* get authentication manager mutex */
  authentication_manager_mutex = AGS_AUTHENTICATION_MANAGER_GET_OBJ_MUTEX(authentication_manager);

  /* insert login and user uuid */
  g_rec_mutex_lock(authentication_manager_mutex);

  ags_login_info_ref(login_info);
  
  g_hash_table_insert(authentication_manager->login,
		      g_strdup(login),
		      login_info);
  
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
 * ags_authentication_manager_login:
 * @authentication_manager: the #AgsAuthenticationManager
 * @authentication_module: the authentication module
 * @login: the login
 * @password: the password
 * @user_uuid: (out) (transfer full): return location of user UUID
 * @security_token: (out) (transfer full): return location of security token
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
  AgsSecurityContext *current_security_context;
  
  GList *start_authentication, *authentication;

  gchar *current_security_token, *current_user_uuid;

  gboolean success;
  
  GError *error;

  if(!AGS_IS_AUTHENTICATION_MANAGER(authentication_manager) ||
     login == NULL ||
     password == NULL){
    return(FALSE);
  }
  
  authentication =
    start_authentication = ags_authentication_manager_get_authentication(authentication_manager);

  success = FALSE;
  
  if(user_uuid != NULL){
    user_uuid[0] = NULL;
  }

  if(security_token != NULL){
    security_token[0] = NULL;
  }

  current_user_uuid = NULL;
  current_security_token = NULL;
  
  while(authentication != NULL){
    gchar **strv;
    
    strv = ags_authentication_get_authentication_module(AGS_AUTHENTICATION(authentication->data));
    
    if(g_strv_contains(strv,
		       authentication_module)){
      error = NULL;
      success = ags_authentication_login(AGS_AUTHENTICATION(authentication->data),
					 login,
					 password,
					 &current_user_uuid,
					 &current_security_token,
					 &error);
      
      if(error != NULL){
	g_warning("%s", error->message);

	g_error_free(error);
      }
      
      if(success){
	if(user_uuid != NULL){
	  user_uuid[0] = current_user_uuid;
	}

	if(security_token != NULL){
	  security_token[0] = current_security_token;
	}

	break;
      }
    }
    
    authentication = authentication->next;
  }

  g_list_free_full(start_authentication,
		   g_object_unref);
  
  return(success);
}

/**
 * ags_authentication_manager_logout:
 * @authentication_manager: the #AgsAuthenticationManager
 * @security_context: the #AgsSecurityContext
 * @login: the login
 * @security_token: the security token
 * 
 * Logout.
 *
 * Returns: %TRUE if logout was successful, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_authentication_manager_logout(AgsAuthenticationManager *authentication_manager,
				  GObject *security_context,
				  gchar *login,
				  gchar *security_token)
{
  AgsSecurityContext *current_security_context;
  
  AgsLoginInfo *login_info;

  GList *start_authentication, *authentication;

  gchar *user_uuid;

  gboolean is_session_active;
  
  GRecMutex *authentication_manager_mutex;

  if(!AGS_IS_AUTHENTICATION_MANAGER(authentication_manager) ||
     login == NULL ||
     security_token == NULL){
    return(FALSE);
  }

  authentication_manager_mutex = AGS_AUTHENTICATION_MANAGER_GET_OBJ_MUTEX(authentication_manager);

  login_info = ags_authentication_manager_lookup_login(authentication_manager,
						       login);

  user_uuid = NULL;
  is_session_active = FALSE;
  
  if(login_info != NULL){  
    g_rec_mutex_lock(authentication_manager_mutex);

    user_uuid = g_strdup(login_info->user_uuid);

    g_rec_mutex_unlock(authentication_manager_mutex);
  }else{
    return(FALSE);
  }
  
  authentication =
    start_authentication = ags_authentication_manager_get_authentication(authentication_manager);
  
  while(authentication != NULL){
    GError *error;

    error = NULL;
    is_session_active = ags_authentication_is_session_active(AGS_AUTHENTICATION(authentication->data),
							     security_context,
							     user_uuid,
							     security_token,
							     &error);

    if(error != NULL){
      g_warning("%s", error->message);

      g_error_free(error);
    }
    
    if(is_session_active){
      error = NULL;
      ags_authentication_logout(AGS_AUTHENTICATION(authentication->data),
				security_context,
				login,
				security_token,
				&error);

      if(error != NULL){
	g_warning("%s", error->message);
	
	g_error_free(error);
      }
      
      break;
    }
    
    authentication = authentication->next;
  }

  g_list_free_full(start_authentication,
		   g_object_unref);

  ags_login_info_unref(login_info);
  
  g_free(user_uuid);
  
  return(is_session_active);
}

/**
 * ags_authentication_manager_get_digest:
 * @authentication_manager: the #AgsAuthenticationManager
 * @authentication_module: the authentication module
 * @realm: the realm
 * @login: the login
 * @security_token: the security token
 * 
 * Get digest of @login.
 * 
 * Returns: (transfer full): the digest as string, or %NULL if not available
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

  if(!AGS_IS_AUTHENTICATION_MANAGER(authentication_manager) ||
     login == NULL ||
     security_token == NULL){
    return(NULL);
  }

  current_digest = NULL;
  
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

      break;
    }
    
    authentication = authentication->next;
  }

  g_list_free_full(start_authentication,
		   g_object_unref);

  return(current_digest);
}

/**
 * ags_authentication_manager_is_session_active:
 * @authentication_manager: the #AgsAuthenticationManager
 * @security_context: the security context
 * @user_uuid: the user's UUID
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
					     gchar *user_uuid,
					     gchar *security_token)
{
  GList *start_authentication, *authentication;  

  gboolean is_session_active;
  
  if(!AGS_IS_AUTHENTICATION_MANAGER(authentication_manager) ||
     user_uuid == NULL){
    return(FALSE);
  }

  is_session_active = FALSE;
  
  authentication =
    start_authentication = ags_authentication_manager_get_authentication(authentication_manager);
  
  while(authentication != NULL){
    GError *error;

    error = NULL;
    is_session_active = ags_authentication_is_session_active(AGS_AUTHENTICATION(authentication->data),
							     security_context,
							     user_uuid,
							     security_token,
							     &error);

    if(error != NULL){
      g_warning("%s", error->message);

      g_error_free(error);
    }
    
    if(is_session_active){
      break;
    }
    
    authentication = authentication->next;
  }

  g_list_free_full(start_authentication,
		   g_object_unref);
  
  return(is_session_active);
}

/**
 * ags_authentication_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: (transfer none): the #AgsAuthenticationManager
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
