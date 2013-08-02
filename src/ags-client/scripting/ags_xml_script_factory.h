/* AGS Client - Advanced GTK Sequencer Client
 * Copyright (C) 2013 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_XML_SCRIPT_FACTORY_H__
#define __AGS_XML_SCRIPT_FACTORY_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#include <ags-client/scripting/ags_schema_factory.h>
#include <ags-client/scripting/mapping/ags_script_object.h>

#define AGS_TYPE_XML_SCRIPT_FACTORY                (ags_xml_script_factory_get_type())
#define AGS_XML_SCRIPT_FACTORY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_XML_SCRIPT_FACTORY, AgsXmlScriptFactory))
#define AGS_XML_SCRIPT_FACTORY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_XML_SCRIPT_FACTORY, AgsXmlScriptFactoryClass))
#define AGS_IS_XML_SCRIPT_FACTORY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_XML_SCRIPT_FACTORY))
#define AGS_IS_XML_SCRIPT_FACTORY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_XML_SCRIPT_FACTORY))
#define AGS_XML_SCRIPT_FACTORY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_XML_SCRIPT_FACTORY, AgsXmlScriptFactoryClass))

#define AGS_XML_SCRIPT_FACTORY_DEFAULT_SCHEMA "ags_script.xsd"

typedef struct _AgsXmlScriptFactory AgsXmlScriptFactory;
typedef struct _AgsXmlScriptFactoryClass AgsXmlScriptFactoryClass;

#define AGS_XML_SCRIPT_FACTORY_ERROR (ags_xml_script_factory_error_quark())

typedef enum{
  AGS_XML_SCRIPT_FACTORY_INVALID_PROTOTYPE,
}AgsXmlScriptFactoryError;

struct _AgsXmlScriptFactory
{
  GObject object;

  AgsSchemaFactory *schema_factory;
  xmlDoc *schema;

  GList *prototype;
};

struct _AgsXmlScriptFactoryClass
{
  GObjectClass object;

  void (*create_prototype)(AgsXmlScriptFactory *xml_script_factory);
};

GType ags_xml_script_factory_get_type();

void ags_xml_script_factory_create_prototype(AgsXmlScriptFactory *xml_script_factory);

xmlNode* ags_xml_script_factory_map(AgsXmlScriptFactory *xml_script_factory,
				    AgsScriptObject *script_object,
				    GError **error);

AgsXmlScriptFactory* ags_xml_script_factory_new();

#endif /*__AGS_XML_SCRIPT_FACTORY_H__*/
