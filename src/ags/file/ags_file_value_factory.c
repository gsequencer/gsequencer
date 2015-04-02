/* AGS - Advanced GTK Sequencer
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

#include <ags/file/ags_file_value_factory.h>

#include <ags-lib/object/ags_serializeable.h>

void ags_file_value_factory_class_init(AgsFileValueFactoryClass *file_value_factory);
void ags_file_value_factory_serializeable_interface_init(AgsSerializeableInterface *serializeable);
void ags_file_value_factory_init(AgsFileValueFactory *file_value_factory);
void ags_file_value_factory_finalize(GObject *gobject);
void ags_file_value_factory_serialize(AgsSerializeable *serializeable);

static gpointer ags_file_value_factory_parent_class = NULL;

GType
ags_file_value_factory_get_type(void)
{
  static GType ags_type_file_value_factory = 0;

  if(!ags_type_file_value_factory){
    static const GTypeInfo ags_file_value_factory_info = {
      sizeof (AgsFileValueFactoryClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_file_value_factory_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFileValueFactory),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_file_value_factory_init,
    };

    static const GInterfaceInfo ags_serializeable_interface_info = {
      (GInterfaceInitFunc) ags_file_value_factory_serializeable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_file_value_factory = g_type_register_static(G_TYPE_OBJECT,
							 "AgsFileValueFactory\0",
							 &ags_file_value_factory_info,
							 0);

    g_type_add_interface_static(ags_type_file_value_factory,
				AGS_TYPE_SERIALIZEABLE,
				&ags_serializeable_interface_info);
  }

  return(ags_type_file_value_factory);
}

void
ags_file_value_factory_class_init(AgsFileClass *file_value_factory)
{
  GObjectClass *gobject;

  ags_file_value_factory_parent_class = g_type_class_peek_parent(file_value_factory);

  /* GObjectClass */
  gobject = (GObjectClass *) file_value_factory;

  gobject->finalize = ags_file_value_factory_finalize;
}

void
ags_file_value_factory_serializeable_interface_init(AgsSerializeableInterface *serializeable)
{
  file_value_factory->serialize = ags_file_value_factory_serialize;
}

void
ags_file_value_factory_init(AgsFileValueFactory *file_value_factory)
{
  //TODO:JK: implement me
}

void
ags_file_value_factory_finalize(GObject *gobject)
{
  //TODO:JK: implement me
}

void
ags_file_value_factory_serialize(AgsSerializeable *serializeable)
{
  //TODO:JK: implement me
}

AgsFileValueFactory*
ags_file_value_factory_new()
{
  AgsFileValueFactory *file_value_factory;

  file_value_factory = g_object_new(AGS_TYPE_FILE_VALUE_FACTORY,
				    NULL);

  return(file_value_factory);
}

