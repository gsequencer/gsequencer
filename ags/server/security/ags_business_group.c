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

#include <ags/server/security/ags_business_group.h>

void ags_business_group_base_init(AgsBusinessGroupInterface *ginterface);

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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_business_group = 0;

    static const GTypeInfo ags_business_group_info = {
      sizeof(AgsBusinessGroupInterface),
      (GBaseInitFunc) ags_business_group_base_init,
      NULL, /* base_finalize */
    };

    ags_type_business_group = g_type_register_static(G_TYPE_INTERFACE,
						     "AgsBusinessGroup", &ags_business_group_info,
						     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_business_group);
  }

  return g_define_type_id__volatile;
}

void
ags_business_group_base_init(AgsBusinessGroupInterface *ginterface)
{
  /* empty */
}

/**
 * ags_business_group_get_group_uuid:
 * @business_group: the #AgsBusinessGroup
 * @security_context: the #AgsSecurityContext
 * @user_uuid: the users unique identifier
 * @security_token: the security token
 * @error: the #GError-struct
 *
 * Get group UUID as string vector.
 * 
 * Returns: the group UUIDs as %NULL terminated string array
 * 
 * Since: 3.0.0
 */
gchar**
ags_business_group_get_group_uuid(AgsBusinessGroup *business_group,
				  GObject *security_context,
				  gchar *user_uuid,
				  gchar *security_token,
				  GError **error)
{
  AgsBusinessGroupInterface *business_group_interface;

  g_return_val_if_fail(AGS_IS_BUSINESS_GROUP(business_group), NULL);
  business_group_interface = AGS_BUSINESS_GROUP_GET_INTERFACE(business_group);
  g_return_val_if_fail(business_group_interface->get_group_uuid, NULL);

  return(business_group_interface->get_group_uuid(business_group,
						  security_context,
						  user_uuid,
						  security_token,
						  error));
}

/**
 * ags_business_group_set_group_name:
 * @business_group: the #AgsBusinessGroup
 * @security_context: the #AgsSecurityContext
 * @user_uuid: the user's UUID
 * @security_token: the security token
 * @group_uuid: the group's UUID
 * @group_name: the business group's name to set
 * @error: the #GError-struct
 *
 * Set business group name.
 *
 * Since: 3.0.0
 */
void
ags_business_group_set_group_name(AgsBusinessGroup *business_group,
				  GObject *security_context,
				  gchar *user_uuid,
				  gchar *security_token,
				  gchar *group_uuid,
				  gchar *group_name,
				  GError **error)
{
  AgsBusinessGroupInterface *business_group_interface;

  g_return_if_fail(AGS_IS_BUSINESS_GROUP(business_group));
  business_group_interface = AGS_BUSINESS_GROUP_GET_INTERFACE(business_group);
  g_return_if_fail(business_group_interface->set_group_name);
  business_group_interface->set_group_name(business_group,
					   security_context,
					   user_uuid,
					   security_token,
					   group_uuid,
					   group_name,
					   error);
}

/**
 * ags_business_group_get_group_name:
 * @business_group: the #AgsBusinessGroup
 * @security_context: the #AgsSecurityContext
 * @user_uuid: the user's UUID
 * @security_token: the security token
 * @group_uuid: the group's UUID
 * @error: the #GError-struct
 *
 * Get business group name.
 *
 * Returns: the business group's name as string
 * 
 * Since: 3.0.0
 */
gchar*
ags_business_group_get_group_name(AgsBusinessGroup *business_group,
				  GObject *security_context,
				  gchar *user_uuid,
				  gchar *security_token,
				  gchar *group_uuid,
				  GError **error)
{
  AgsBusinessGroupInterface *business_group_interface;

  g_return_val_if_fail(AGS_IS_BUSINESS_GROUP(business_group), NULL);
  business_group_interface = AGS_BUSINESS_GROUP_GET_INTERFACE(business_group);
  g_return_val_if_fail(business_group_interface->get_group_name, NULL);

  return(business_group_interface->get_group_name(business_group,
						  security_context,
						  user_uuid,
						  security_token,
						  group_uuid,
						  error));
}

/**
 * ags_business_group_set_user:
 * @business_group: the #AgsBusinessGroup
 * @security_context: the #AgsSecurityContext
 * @user_uuid: the user's UUID
 * @security_token: the security token
 * @group_uuid: the business group's UUID
 * @user: the string array containing user names
 * @error: the #GError-struct
 *
 * Set business group of user names as %NULL terminated string array.
 *
 * Since: 3.0.0
 */
void
ags_business_group_set_user(AgsBusinessGroup *business_group,
			    GObject *security_context,
			    gchar *user_uuid,
			    gchar *security_token,
			    gchar *group_uuid,
			    gchar **user,
			    GError **error)
{
  AgsBusinessGroupInterface *business_group_interface;

  g_return_if_fail(AGS_IS_BUSINESS_GROUP(business_group));
  business_group_interface = AGS_BUSINESS_GROUP_GET_INTERFACE(business_group);
  g_return_if_fail(business_group_interface->set_user);
  business_group_interface->set_user(business_group,
				     security_context,
				     user_uuid,
				     security_token,
				     group_uuid,
				     user,
				     error);
}

/**
 * ags_business_group_get_user:
 * @business_group: the #AgsBusinessGroup
 * @security_context: the #AgsSecurityContext
 * @user_uuid: the user's UUID
 * @security_token: the security token
 * @group_uuid: the business group's UUID
 * @error: the #GError-struct
 *
 * Get business group name as %NULL terminated string array.
 *
 * Returns: the business group's user names as string vector
 * 
 * Since: 3.0.0
 */
gchar**
ags_business_group_get_user(AgsBusinessGroup *business_group,
			    GObject *security_context,
			    gchar *user_uuid,
			    gchar *security_token,
			    gchar *group_uuid,
			    GError **error)
{
  AgsBusinessGroupInterface *business_group_interface;

  g_return_val_if_fail(AGS_IS_BUSINESS_GROUP(business_group), NULL);
  business_group_interface = AGS_BUSINESS_GROUP_GET_INTERFACE(business_group);
  g_return_val_if_fail(business_group_interface->get_user, NULL);

  return(business_group_interface->get_user(business_group,
					    security_context,
					    user_uuid,
					    security_token,
					    group_uuid,
					    error));
}
