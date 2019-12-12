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

#ifndef __AGS_XML_BUSINESS_GROUP_H__
#define __AGS_XML_BUSINESS_GROUP_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

G_BEGIN_DECLS

#define AGS_TYPE_XML_BUSINESS_GROUP                (ags_xml_business_group_get_type())
#define AGS_XML_BUSINESS_GROUP(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_XML_BUSINESS_GROUP, AgsXmlBusinessGroup))
#define AGS_XML_BUSINESS_GROUP_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_XML_BUSINESS_GROUP, AgsXmlBusinessGroupClass))
#define AGS_IS_XML_BUSINESS_GROUP(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_XML_BUSINESS_GROUP))
#define AGS_IS_XML_BUSINESS_GROUP_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_XML_BUSINESS_GROUP))
#define AGS_XML_BUSINESS_GROUP_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_XML_BUSINESS_GROUP, AgsXmlBusinessGroupClass))

#define AGS_XML_BUSINESS_GROUP_GET_OBJ_MUTEX(obj) (&(((AgsXmlBusinessGroup *) obj)->obj_mutex))

typedef struct _AgsXmlBusinessGroup AgsXmlBusinessGroup;
typedef struct _AgsXmlBusinessGroupClass AgsXmlBusinessGroupClass;

struct _AgsXmlBusinessGroup
{
  GObject gobject;

  GRecMutex obj_mutex;

  gchar *filename;
  gchar *encoding;
  gchar *dtd;

  xmlDoc *doc;
  xmlNode *root_node;
};

struct _AgsXmlBusinessGroupClass
{
  GObjectClass gobject;
};

GType ags_xml_business_group_get_type();

void ags_xml_business_group_open_filename(AgsXmlBusinessGroup *xml_business_group,
					  gchar *filename);

AgsXmlBusinessGroup* ags_xml_business_group_new();

G_END_DECLS

#endif /*__AGS_XML_BUSINESS_GROUP_H__*/
