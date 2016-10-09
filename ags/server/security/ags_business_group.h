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

#ifndef __AGS_BUSINESS_GROUP_H__
#define __AGS_BUSINESS_GROUP_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_BUSINESS_GROUP                    (ags_business_group_get_type())
#define AGS_BUSINESS_GROUP(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_BUSINESS_GROUP, AgsBusinessGroup))
#define AGS_BUSINESS_GROUP_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_BUSINESS_GROUP, AgsBusinessGroupInterface))
#define AGS_IS_BUSINESS_GROUP(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_BUSINESS_GROUP))
#define AGS_IS_BUSINESS_GROUP_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_BUSINESS_GROUP))
#define AGS_BUSINESS_GROUP_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_BUSINESS_GROUP, AgsBusinessGroupInterface))

typedef struct _AgsBusinessGroup AgsBusinessGroup;
typedef struct _AgsBusinessGroupInterface AgsBusinessGroupInterface;

struct _AgsBusinessGroupInterface
{
  GTypeInterface interface;
  
  void (*set_business_group_name)(AgsBusinessGroup *business_group,
				  GObject *security_context,
				  gchar *login,
				  gchar *security_token,
				  gchar *group_uuid,
				  gchar *business_group_name,
				  GError **error);
  gchar* (*get_business_group_name)(AgsBusinessGroup *business_group,
				    GObject *security_context,
				    gchar *login,
				    gchar *security_token,
				    gchar *group_uuid,
				    GError **error);
  
  void (*set_user_uuid)(AgsBusinessGroup *business_group,
			GObject *security_context,
			gchar *login,
			gchar *security_token,
			gchar *business_group_name,
			gchar** user_uuid,
			GError **error);
  gchar** (*get_user_uuid)(AgsBusinessGroup *business_group,
			   GObject *security_context,
			   gchar *login,
			   gchar *security_token,
			   gchar *business_group_name,
			   GError **error);
  
  void (*set_context_path_with_read_permission)(AgsBusinessGroup *business_group,
						gchar *login,
						gchar *security_token,
						gchar *business_group_name,
						gchar **context_path,
						GError **error);
  gchar** (*get_context_path_with_read_permission)(AgsBusinessGroup *business_group,
						   gchar *login,
						   gchar *security_token,
						   gchar *business_group_name,
						   GError **error);
  
  void (*set_context_path_with_write_permission)(AgsBusinessGroup *business_group,
						 gchar *login,
						 gchar *security_token,
						 gchar *business_group_name,
						 gchar **context_path,
						 GError **error);
  gchar** (*get_context_path_with_write_permission)(AgsBusinessGroup *business_group,
						    gchar *login,
						    gchar *security_token,
						    gchar *business_group_name,
						    GError **error);
  
  void (*set_context_path_with_execute_permission)(AgsBusinessGroup *business_group,
						   gchar *login,
						   gchar *security_token,
						   gchar *business_group_name,
						   gchar **context_path,
						   GError **error);
  gchar** (*get_context_path_with_execute_permission)(AgsBusinessGroup *business_group,
						      gchar *login,
						      gchar *security_token,
						      gchar *business_group_name,
						      GError **error);
};

GType ags_business_group_get_type();

void ags_business_group_set_business_group_name(AgsBusinessGroup *business_group,
						GObject *security_context,
						gchar *login,
						gchar *security_token,
						gchar *group_uuid,
						gchar *business_group_name,
						GError **error);
gchar* ags_business_group_get_business_group_name(AgsBusinessGroup *business_group,
						  GObject *security_context,
						  gchar *login,
						  gchar *security_token,
						  gchar *group_uuid,
						  GError **error);
  
void ags_business_group_set_user_uuid(AgsBusinessGroup *business_group,
				      GObject *security_context,
				      gchar *login,
				      gchar *security_token,
				      gchar *business_group_name,
				      gchar** user_uuid,
				      GError **error);
gchar** ags_business_group_get_user_uuid(AgsBusinessGroup *business_group,
					 GObject *security_context,
					 gchar *login,
					 gchar *security_token,
					 gchar *business_group_name,
					 GError **error);
  
void ags_business_group_set_context_path_with_read_permission(AgsBusinessGroup *business_group,
							      gchar *login,
							      gchar *security_token,
							      gchar *business_group_name,
							      gchar **context_path,
							      GError **error);
gchar** ags_business_group_get_context_path_with_read_permission(AgsBusinessGroup *business_group,
								 gchar *login,
								 gchar *security_token,
								 gchar *business_group_name,
								 GError **error);

void ags_business_group_set_context_path_with_write_permission(AgsBusinessGroup *business_group,
							       gchar *login,
							       gchar *security_token,
							       gchar *business_group_name,
							       gchar **context_path,
							       GError **error);
gchar** ags_business_group_get_context_path_with_write_permission(AgsBusinessGroup *business_group,
								  gchar *login,
								  gchar *security_token,
								  gchar *business_group_name,
								  GError **error);
  
void ags_business_group_set_context_path_with_execute_permission(AgsBusinessGroup *business_group,
								 gchar *login,
								 gchar *security_token,
								 gchar *business_group_name,
								 gchar **context_path,
								 GError **error);
gchar** ags_business_group_get_context_path_with_execute_permission(AgsBusinessGroup *business_group,
								    gchar *login,
								    gchar *security_token,
								    gchar *business_group_name,
								    GError **error);

#endif /*__AGS_BUSINESS_GROUP_H__*/
