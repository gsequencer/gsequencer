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

#ifndef __AGS_SCHEMA_FACTORY_H__
#define __AGS_SCHEMA_FACTORY_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#define AGS_TYPE_SCHEMA_FACTORY                (ags_schema_factory_get_type())
#define AGS_SCHEMA_FACTORY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SCHEMA_FACTORY, AgsSchemaFactory))
#define AGS_SCHEMA_FACTORY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_SCHEMA_FACTORY, AgsSchemaFactoryClass))
#define AGS_IS_SCHEMA_FACTORY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SCHEMA_FACTORY))
#define AGS_IS_SCHEMA_FACTORY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SCHEMA_FACTORY))
#define AGS_SCHEMA_FACTORY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_SCHEMA_FACTORY, AgsSchemaFactoryClass))

typedef struct _AgsSchemaFactory AgsSchemaFactory;
typedef struct _AgsSchemaFactoryClass AgsSchemaFactoryClass;

typedef enum{
  AGS_SCHEMA_FACTORY_PARSE_AS_DTD      = 1,
  AGS_SCHEMA_FACTORY_PARSE_AS_XSD      = 1 << 1,
};

struct _AgsSchemaFactory
{
  GObject object;

  guint flags;
};

struct _AgsSchemaFactoryClass
{
  GObjectClass object;
};

GType ags_schema_factory_get_type();

xmlNode* ags_schema_factory_create_node(AgsSchemaFactory *schema_factory,
					void *node);

AgsSchemaFactory* ags_schema_factory_new();

#endif /*__AGS_SCHEMA_FACTORY_H__*/
