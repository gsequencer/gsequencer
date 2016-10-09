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
  static GType ags_type_authentication_manager = 0;

  if(!ags_type_authentication_manager){
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
							     "AgsAuthenticationManager\0",
							     &ags_authentication_manager_info,
							     0);
  }

  return (ags_type_authentication_manager);
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
  authentication_manager->authentication = NULL;
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

  G_OBJECT_CLASS(ags_authentication_manager_parent_class)->finalize(gobject);
}

GList*
ags_authentication_manager_get_authentication(AgsAuthenticationManager *authentication_manager)
{
  if(authentication_manager == NULL){
    authentication_manager = ags_authentication_manager_get_instance();
  }
  
  return(authentication_manager->authentication);
}

void
ags_authentication_manager_add_authentication(AgsAuthenticationManager *authentication_manager,
					      GObject *authentication)
{
  if(authentication_manager == NULL){
    authentication_manager = ags_authentication_manager_get_instance();
  }

  authentication_manager->authentication = g_list_prepend(authentication_manager->authentication,
							  authentication);
}

void
ags_authentication_manager_remove_authentication(AgsAuthenticationManager *authentication_manager,
						 GObject *authentication)
{
  if(authentication_manager == NULL){
    authentication_manager = ags_authentication_manager_get_instance();
  }

  authentication_manager->authentication = g_list_remove(authentication_manager->authentication,
							 authentication);
}

gboolean
ags_authentication_manager_login(AgsAuthenticationManager *authentication_manager,
				 gchar *login,
				 gchar *password,
				 gchar **user_uuid,
				 gchar **security_token)
{
  //TODO:JK: implement me
  
  return(FALSE);
}

gboolean
ags_authentication_manager_check_authentication(AgsAuthenticationManager *authentication_manager,
						gchar *user_uuid,
						gchar *security_token)
{
  //TODO:JK: implement me
  
  return(FALSE);
}

/**
 * ags_authentication_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: the #AgsAuthenticationManager
 *
 * Since: 1.0.0
 */
AgsAuthenticationManager*
ags_authentication_manager_get_instance()
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&(mutex));

  if(ags_authentication_manager == NULL){
    ags_authentication_manager = ags_authentication_manager_new();

    pthread_mutex_unlock(&(mutex));
  }else{
    pthread_mutex_unlock(&(mutex));
  }

  return(ags_authentication_manager);
}

/**
 * ags_authentication_manager_new:
 *
 * Creates an #AgsAuthenticationManager
 *
 * Returns: a new #AgsAuthenticationManager
 *
 * Since: 1.0.0
 */
AgsAuthenticationManager*
ags_authentication_manager_new()
{
  AgsAuthenticationManager *authentication_manager;

  authentication_manager = (AgsAuthenticationManager *) g_object_new(AGS_TYPE_AUTHENTICATION_MANAGER,
								     NULL);

  return(authentication_manager);
}
