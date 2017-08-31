/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_XML_SERIALIZATION_FACTORY_H__
#define __AGS_XML_SERIALIZATION_FACTORY_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#define AGS_TYPE_XML_SERIALIZATION_FACTORY                (ags_xml_serialization_factory_get_type())
#define AGS_XML_SERIALIZATION_FACTORY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_XML_SERIALIZATION_FACTORY, AgsXmlSerializationFactory))
#define AGS_XML_SERIALIZATION_FACTORY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_XML_SERIALIZATION_FACTORY, AgsXmlSerializationFactoryClass))
#define AGS_IS_XML_SERIALIZATION_FACTORY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_XML_SERIALIZATION_FACTORY))
#define AGS_IS_XML_SERIALIZATION_FACTORY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_XML_SERIALIZATION_FACTORY))
#define AGS_XML_SERIALIZATION_FACTORY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_XML_SERIALIZATION_FACTORY, AgsXmlSerializationFactoryClass))

typedef struct _AgsXmlSerializationFactory AgsXmlSerializationFactory;
typedef struct _AgsXmlSerializationFactoryClass AgsXmlSerializationFactoryClass;

struct _AgsXmlSerializationFactory
{
  GObject object;
};

struct _AgsXmlSerializationFactoryClass
{
  GObjectClass object;
};

GType ags_xml_serialization_factory_get_type();

gpointer ags_xml_serialization_factory_create(AgsXmlSerializationFactory *xml_serialization_factory,
					      xmlNode *node);

/*  */
AgsXmlSerializationFactory* ags_xml_serialization_factory_get_instance();
AgsXmlSerializationFactory* ags_xml_serialization_factory_new();

#endif /*__AGS_XML_SERIALIZATION_FACTORY_H__*/
