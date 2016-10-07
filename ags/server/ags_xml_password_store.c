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

#include <ags/server/ags_xml_password_store.h>

#include <ags/server/ags_password_store.h>

void ags_xml_password_store_class_init(AgsXmlPasswordStoreClass *xml_password_store);
void ags_xml_password_store_password_store_interface_init(AgsPasswordStoreInterface *password_store);
void ags_xml_password_store_init(AgsXmlPasswordStore *xml_password_store);
void ags_xml_password_store_finalize(GObject *gobject);
gchar* ags_xml_password_store_get_login_name(AgsPasswordStore *password_store,
					     gchar *user,
					     gchar *security_token,
					     guint security_context,
					     GError **error);
void ags_xml_password_store_set_login_name(AgsPasswordStore *password_store,
					   gchar *user,
					   gchar *security_token,
					   guint security_context,
					   gchar *login_name,
					   GError **error);
gchar* ags_xml_password_store_get_password(AgsPasswordStore *password_store,
					   gchar *user,
					   gchar *security_token,
					   guint security_context,
					   GError **error);
void ags_xml_password_store_set_password(AgsPasswordStore *password_store,
					 gchar *user,
					 gchar *security_token,
					 guint security_context,
					 gchar *password,
					 GError **error);
char* ags_xml_password_store_encrypt_password(AgsPasswordStore *password_store,
					      gchar *password,
					      GError **error);
gchar* ags_xml_password_store_decrypt_password(AgsPasswordStore *password_store,
					       char *data,
					       GError **error);

/**
 * SECTION:ags_xml_password_store
 * @short_description: password store by XML file
 * @title: AgsXmlPasswordStore
 * @section_id:
 * @include: ags/server/ags_xml_password_store.h
 *
 * The #AgsXmlPasswordStore is an object to keep passwords.
 */

static gpointer ags_xml_password_store_parent_class = NULL;

GType
ags_xml_password_store_get_type()
{
  static GType ags_type_xml_password_store = 0;

  if(!ags_type_xml_password_store){
    static const GTypeInfo ags_xml_password_store_info = {
      sizeof (AgsXmlPasswordStoreClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_xml_password_store_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsXmlPasswordStore),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_xml_password_store_init,
    };

    static const GInterfaceInfo ags_password_store_interface_info = {
      (GInterfaceInitFunc) ags_xml_password_store_password_store_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_xml_password_store = g_type_register_static(G_TYPE_OBJECT,
							 "AgsXmlPasswordStore\0",
							 &ags_xml_password_store_info,
							 0);

    g_type_add_interface_static(ags_type_xml_password_store,
				AGS_TYPE_PASSWORD_STORE,
				&ags_password_store_interface_info);
  }

  return (ags_type_xml_password_store);
}

void
ags_xml_password_store_class_init(AgsXmlPasswordStoreClass *xml_password_store)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_xml_password_store_parent_class = g_type_class_peek_parent(xml_password_store);

  /* GObjectClass */
  gobject = (GObjectClass *) xml_password_store;

  gobject->finalize = ags_xml_password_store_finalize;
}

void
ags_xml_password_store_password_store_interface_init(AgsPasswordStoreInterface *password_store)
{
  password_store->get_login_name = ags_xml_password_store_get_login_name;
  password_store->set_login_name = ags_xml_password_store_set_login_name;

  password_store->get_password = ags_xml_password_store_get_password;
  password_store->set_password = ags_xml_password_store_set_password;

  password_store->encrypt_password = ags_xml_password_store_encrypt_password;
  password_store->decrypt_password = ags_xml_password_store_decrypt_password;
}

void
ags_xml_password_store_init(AgsXmlPasswordStore *xml_password_store)
{
  xml_password_store->filename = NULL;
  xml_password_store->encoding = NULL;
  xml_password_store->dtd = NULL;

  xml_password_store->doc = NULL;
  xml_password_store->root_node = NULL;
}

void
ags_xml_password_store_finalize(GObject *gobject)
{
  AgsXmlPasswordStore *xml_password_store;

  xml_password_store = AGS_XML_PASSWORD_STORE(gobject);

  G_OBJECT_CLASS(ags_xml_password_store_parent_class)->finalize(gobject);
}

gchar*
ags_xml_password_store_get_login_name(AgsPasswordStore *password_store,
				      gchar *user,
				      gchar *security_token,
				      guint security_context,
				      GError **error)
{
  //TODO:JK: implement me
}

void
ags_xml_password_store_set_login_name(AgsPasswordStore *password_store,
				      gchar *user,
				      gchar *security_token,
				      guint security_context,
				      gchar *login_name,
				      GError **error)
{
  //TODO:JK: implement me
}

gchar*
ags_xml_password_store_get_password(AgsPasswordStore *password_store,
				    gchar *user,
				    gchar *security_token,
				    guint security_context,
				    GError **error)
{
  //TODO:JK: implement me
}

void
ags_xml_password_store_set_password(AgsPasswordStore *password_store,
				    gchar *user,
				    gchar *security_token,
				    guint security_context,
				    gchar *password,
				    GError **error)
{
  //TODO:JK: implement me
}

char*
ags_xml_password_store_encrypt_password(AgsPasswordStore *password_store,
					gchar *password,
					GError **error)
{
  //TODO:JK: implement me
}

gchar*
ags_xml_password_store_decrypt_password(AgsPasswordStore *password_store,
					char *data,
					GError **error)
{
  //TODO:JK: implement me
}

AgsXmlPasswordStore*
ags_xml_password_store_new()
{
  AgsXmlPasswordStore *xml_password_store;

  xml_password_store = (AgsXmlPasswordStore *) g_object_new(AGS_TYPE_XML_PASSWORD_STORE,
							    NULL);

  return(xml_password_store);
}
