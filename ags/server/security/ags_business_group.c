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

#include <ags/server/security/ags_business_group.h>

void ags_business_group_base_init(AgsBusinessGroupInterface *interface);

/**
 * SECTION:ags_business_group
 * @short_description: base group permissions
 * @title: AgsBusinessGroup
 * @section_id: AgsBusinessGroup
 * @include: ags/server/security/ags_business_group.h
 *
 * The #AgsBusinessGroup interface gives you base group permissions.
 */

GType
ags_business_group_get_type()
{
  static GType ags_type_business_group = 0;

  if(!ags_type_business_group){
    static const GTypeInfo ags_business_group_info = {
      sizeof(AgsBusinessGroupInterface),
      (GBaseInitFunc) ags_business_group_base_init,
      NULL, /* base_finalize */
    };

    ags_type_business_group = g_type_register_static(G_TYPE_INTERFACE,
						     "AgsBusinessGroup\0", &ags_business_group_info,
						     0);
  }

  return(ags_type_business_group);
}

void
ags_business_group_base_init(AgsBusinessGroupInterface *interface)
{
  /* empty */
}

/**
 * ags_business_group_set_business_group_name:
 * @business_group: the #AgsBusinessGroup
 * @security_context: the #AgsSecurityContext
 * @login: the login
 * @security_token: the security token
 * @group_uuid: the group's uuid
 * @business_group_name: the business group name
 * @error: the #GError-struct
 *
 * Set business group name.
 *
 * Since: 1.0.0
 */
void
ags_business_group_set_business_group_name(AgsBusinessGroup *business_group,
					   GObject *security_context,
					   gchar *login,
					   gchar *security_token,
					   gchar *group_uuid,
					   gchar *business_group_name,
					   GError **error)
{
  AgsBusinessGroupInterface *business_group_interface;

  g_return_if_fail(AGS_IS_BUSINESS_GROUP(business_group));
  business_group_interface = AGS_BUSINESS_GROUP_GET_INTERFACE(business_group);
  g_return_if_fail(business_group_interface->set_business_group_name);
  business_group_interface->set_business_group_name(business_group,
						    security_context,
						    login,
						    security_token,
						    group_uuid,
						    business_group_name,
						    error);
}

/**
 * ags_business_group_get_business_group_name:
 * @business_group: the #AgsBusinessGroup
 * @security_context: the #AgsSecurityContext
 * @login: the login
 * @security_token: the security token
 * @group_uuid: the group's uuid
 * @error: the #GError-struct
 *
 * Get business group name.
 *
 * Returns: the business group name as string
 * 
 * Since: 1.0.0
 */
gchar*
ags_business_group_get_business_group_name(AgsBusinessGroup *business_group,
					   GObject *security_context,
					   gchar *login,
					   gchar *security_token,
					   gchar *group_uuid,
					   GError **error)
{
  AgsBusinessGroupInterface *business_group_interface;

  g_return_val_if_fail(AGS_IS_BUSINESS_GROUP(business_group), NULL);
  business_group_interface = AGS_BUSINESS_GROUP_GET_INTERFACE(business_group);
  g_return_val_if_fail(business_group_interface->get_business_group_name, NULL);

  return(business_group_interface->get_business_group_name(business_group,
							   security_context,
							   login,
							   security_token,
							   group_uuid,
							   error));
}

/**
 * ags_business_group_set_user_uuid:
 * @business_group: the #AgsBusinessGroup
 * @security_context: the #AgsSecurityContext
 * @login: the login
 * @security_token: the security token
 * @business_group_name: the business group name
 * @user_uuid: the string array containing uuids of users
 * @error: the #GError-struct
 *
 * Set business group uuids of users as string array.
 *
 * Since: 1.0.0
 */
void
ags_business_group_set_user_uuid(AgsBusinessGroup *business_group,
				 GObject *security_context,
				 gchar *login,
				 gchar *security_token,
				 gchar *business_group_name,
				 gchar** user_uuid,
				 GError **error)
{
  AgsBusinessGroupInterface *business_group_interface;

  g_return_if_fail(AGS_IS_BUSINESS_GROUP(business_group));
  business_group_interface = AGS_BUSINESS_GROUP_GET_INTERFACE(business_group);
  g_return_if_fail(business_group_interface->set_user_uuid);
  business_group_interface->set_user_uuid(business_group,
					  security_context,
					  login,
					  security_token,
					  business_group_name,
					  user_uuid,
					  error);
}

/**
 * ags_business_group_get_user_uuid:
 * @business_group: the #AgsBusinessGroup
 * @security_context: the #AgsSecurityContext
 * @login: the login
 * @security_token: the security token
 * @business_group_name: the business group name
 * @error: the #GError-struct
 *
 * Get business group name.
 *
 * Returns: the business group's uuids of users as string array
 * 
 * Since: 1.0.0
 */
gchar**
ags_business_group_get_user_uuid(AgsBusinessGroup *business_group,
				 GObject *security_context,
				 gchar *login,
				 gchar *security_token,
				 gchar *business_group_name,
				 GError **error)
{
  AgsBusinessGroupInterface *business_group_interface;

  g_return_val_if_fail(AGS_IS_BUSINESS_GROUP(business_group), NULL);
  business_group_interface = AGS_BUSINESS_GROUP_GET_INTERFACE(business_group);
  g_return_val_if_fail(business_group_interface->get_user_uuid, NULL);

  return(business_group_interface->get_user_uuid(business_group,
						 security_context,
						 login,
						 security_token,
						 business_group_name,
						 error));
}

/**
 * ags_business_group_set_context_path_with_read_permission:
 * @business_group: the #AgsBusinessGroup
 * @login: the login
 * @security_token: the security token
 * @business_group_name: the business group name
 * @context_path: the string array containing context paths
 * @error: the #GError-struct
 *
 * Set business group context paths as string array.
 *
 * Since: 1.0.0
 */
void
ags_business_group_set_context_path_with_read_permission(AgsBusinessGroup *business_group,
							 gchar *login,
							 gchar *security_token,
							 gchar *business_group_name,
							 gchar **context_path,
							 GError **error)
{
  AgsBusinessGroupInterface *business_group_interface;

  g_return_if_fail(AGS_IS_BUSINESS_GROUP(business_group));
  business_group_interface = AGS_BUSINESS_GROUP_GET_INTERFACE(business_group);
  g_return_if_fail(business_group_interface->set_context_path_with_read_permission);
  business_group_interface->set_context_path_with_read_permission(business_group,
								  login,
								  security_token,
								  business_group_name,
								  context_path,
								  error);
}

/**
 * ags_business_group_get_context_path_with_read_permission:
 * @business_group: the #AgsBusinessGroup
 * @login: the login
 * @security_token: the security token
 * @business_group_name: the business group name
 * @error: the #GError-struct
 *
 * Get business group name.
 *
 * Returns: the business group's context paths as string array
 * 
 * Since: 1.0.0
 */
gchar**
ags_business_group_get_context_path_with_read_permission(AgsBusinessGroup *business_group,
							 gchar *login,
							 gchar *security_token,
							 gchar *business_group_name,
							 GError **error)
{
  AgsBusinessGroupInterface *business_group_interface;

  g_return_val_if_fail(AGS_IS_BUSINESS_GROUP(business_group), NULL);
  business_group_interface = AGS_BUSINESS_GROUP_GET_INTERFACE(business_group);
  g_return_val_if_fail(business_group_interface->get_context_path_with_read_permission, NULL);

  return(business_group_interface->get_context_path_with_read_permission(business_group,
									 login,
									 security_token,
									 business_group_name,
									 error));
}

/**
 * ags_business_group_set_context_path_with_write_permission:
 * @business_group: the #AgsBusinessGroup
 * @login: the login
 * @security_token: the security token
 * @business_group_name: the business group name
 * @context_path: the string array containing context paths
 * @error: the #GError-struct
 *
 * Set business group context paths as string array.
 *
 * Since: 1.0.0
 */
void
ags_business_group_set_context_path_with_write_permission(AgsBusinessGroup *business_group,
							  gchar *login,
							  gchar *security_token,
							  gchar *business_group_name,
							  gchar **context_path,
							  GError **error)
{
  AgsBusinessGroupInterface *business_group_interface;

  g_return_if_fail(AGS_IS_BUSINESS_GROUP(business_group));
  business_group_interface = AGS_BUSINESS_GROUP_GET_INTERFACE(business_group);
  g_return_if_fail(business_group_interface->set_context_path_with_write_permission);
  business_group_interface->set_context_path_with_write_permission(business_group,
								   login,
								   security_token,
								   business_group_name,
								   context_path,
								   error);
}

/**
 * ags_business_group_get_context_path_with_write_permission:
 * @business_group: the #AgsBusinessGroup
 * @login: the login
 * @security_token: the security token
 * @business_group_name: the business group name
 * @error: the #GError-struct
 *
 * Get business group name.
 *
 * Returns: the business group's context paths as string array
 * 
 * Since: 1.0.0
 */
gchar**
ags_business_group_get_context_path_with_write_permission(AgsBusinessGroup *business_group,
							  gchar *login,
							  gchar *security_token,
							  gchar *business_group_name,
							  GError **error)
{
  AgsBusinessGroupInterface *business_group_interface;

  g_return_val_if_fail(AGS_IS_BUSINESS_GROUP(business_group), NULL);
  business_group_interface = AGS_BUSINESS_GROUP_GET_INTERFACE(business_group);
  g_return_val_if_fail(business_group_interface->get_context_path_with_write_permission, NULL);

  return(business_group_interface->get_context_path_with_write_permission(business_group,
									  login,
									  security_token,
									  business_group_name,
									  error));
}

/**
 * ags_business_group_set_context_path_with_execute_permission:
 * @business_group: the #AgsBusinessGroup
 * @login: the login
 * @security_token: the security token
 * @business_group_name: the business group name
 * @context_path: the string array containing context paths
 * @error: the #GError-struct
 *
 * Set business group context paths as string array.
 *
 * Since: 1.0.0
 */
void
ags_business_group_set_context_path_with_execute_permission(AgsBusinessGroup *business_group,
							    gchar *login,
							    gchar *security_token,
							    gchar *business_group_name,
							    gchar **context_path,
							    GError **error)
{
  AgsBusinessGroupInterface *business_group_interface;

  g_return_if_fail(AGS_IS_BUSINESS_GROUP(business_group));
  business_group_interface = AGS_BUSINESS_GROUP_GET_INTERFACE(business_group);
  g_return_if_fail(business_group_interface->set_context_path_with_execute_permission);
  business_group_interface->set_context_path_with_execute_permission(business_group,
								     login,
								     security_token,
								     business_group_name,
								     context_path,
								     error);
}

/**
 * ags_business_group_get_context_path_with_execute_permission:
 * @business_group: the #AgsBusinessGroup
 * @login: the login
 * @security_token: the security token
 * @business_group_name: the business group name
 * @error: the #GError-struct
 *
 * Get business group name.
 *
 * Returns: the business group's context paths as string array
 * 
 * Since: 1.0.0
 */
gchar**
ags_business_group_get_context_path_with_execute_permission(AgsBusinessGroup *business_group,
							    gchar *login,
							    gchar *security_token,
							    gchar *business_group_name,
							    GError **error)
{
  AgsBusinessGroupInterface *business_group_interface;

  g_return_val_if_fail(AGS_IS_BUSINESS_GROUP(business_group), NULL);
  business_group_interface = AGS_BUSINESS_GROUP_GET_INTERFACE(business_group);
  g_return_val_if_fail(business_group_interface->get_context_path_with_execute_permission, NULL);

  return(business_group_interface->get_context_path_with_execute_permission(business_group,
									    login,
									    security_token,
									    business_group_name,
									    error));
}
