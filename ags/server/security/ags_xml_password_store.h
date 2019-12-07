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

#ifndef __AGS_XML_PASSWORD_STORE_H__
#define __AGS_XML_PASSWORD_STORE_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

G_BEGIN_DECLS

#define AGS_TYPE_XML_PASSWORD_STORE                (ags_xml_password_store_get_type())
#define AGS_XML_PASSWORD_STORE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_XML_PASSWORD_STORE, AgsXmlPasswordStore))
#define AGS_XML_PASSWORD_STORE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_XML_PASSWORD_STORE, AgsXmlPasswordStoreClass))
#define AGS_IS_XML_PASSWORD_STORE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_XML_PASSWORD_STORE))
#define AGS_IS_XML_PASSWORD_STORE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_XML_PASSWORD_STORE))
#define AGS_XML_PASSWORD_STORE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_XML_PASSWORD_STORE, AgsXmlPasswordStoreClass))

#define AGS_XML_PASSWORD_STORE_GET_OBJ_MUTEX(obj) (&(((AgsXmlPasswordStore *) obj)->obj_mutex))

typedef struct _AgsXmlPasswordStore AgsXmlPasswordStore;
typedef struct _AgsXmlPasswordStoreClass AgsXmlPasswordStoreClass;

struct _AgsXmlPasswordStore
{
  GObject gobject;

  GRecMutex obj_mutex;

  gchar *filename;
  gchar *encoding;
  gchar *dtd;

  xmlDoc *doc;
  xmlNode *root_node;
};

struct _AgsXmlPasswordStoreClass
{
  GObjectClass gobject;
};

GType ags_xml_password_store_get_type();

void ags_xml_password_store_open_filename(AgsXmlPasswordStore *xml_password_store,
					  gchar *filename);

xmlNode* ags_xml_password_store_find_login(AgsXmlPasswordStore *xml_password_store,
					   gchar *login);

AgsXmlPasswordStore* ags_xml_password_store_new();

G_END_DECLS

#endif /*__AGS_XML_PASSWORD_STORE_H__*/
