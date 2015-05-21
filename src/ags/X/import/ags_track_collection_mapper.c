/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#include <ags/X/import/ags_track_collection_mapper.h>
#include <ags/X/import/ags_track_collection_mapper_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>

#include <ags/X/ags_window.h>

void ags_track_collection_mapper_class_init(AgsTracCollectionkMapperClass *track_collection_mapper);
void ags_track_collection_mapper_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_track_collection_mapper_init(AgsTracCollectionkMapper *track_collection_mapper);
void ags_track_collection_mapper_connect(AgsConnectable *connectable);
void ags_track_collection_mapper_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_track_collection_mapper
 * @short_description: pack pad editors.
 * @title: AgsTracCollectionkMapper
 * @section_id:
 * @include: ags/X/ags_track_collection_mapper.h
 *
 * #AgsTracCollectionkMapper is a wizard to import midi files and do track mapping..
 */

GType
ags_track_collection_mapper_get_type(void)
{
  static GType ags_type_track_collection_mapper = 0;

  if(!ags_type_track_collection_mapper){
    static const GTypeInfo ags_track_collection_mapper_info = {
      sizeof (AgsTracCollectionkMapperClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_track_collection_mapper_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTracCollectionkMapper),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_track_collection_mapper_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_track_collection_mapper_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_track_collection_mapper = g_type_register_static(GTK_TYPE_DIALOG,
							 "AgsTracCollectionkMapper\0", &ags_track_collection_mapper_info,
							 0);

    g_type_add_interface_static(ags_type_track_collection_mapper,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return(ags_type_track_collection_mapper);
}

void
ags_track_collection_mapper_class_init(AgsTracCollectionkMapperClass *track_collection_mapper)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  /* GObjectClass */
  gobject = (GObjectClass *) track_collection_mapper;
}

void
ags_track_collection_mapper_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_track_collection_mapper_connect;
  connectable->disconnect = ags_track_collection_mapper_disconnect;
}

void
ags_track_collection_mapper_init(AgsTracCollectionkMapper *track_collection_mapper)
{
}

void
ags_track_collection_mapper_connect(AgsConnectable *connectable)
{
  AgsTracCollectionkMapper *track_collection_mapper;

  track_collection_mapper = AGS_TRACK_COLLECTION_MAPPER(connectable);
}

void
ags_track_collection_mapper_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

/**
 * ags_track_collection_mapper_new:
 *
 * Creates an #AgsTracCollectionkMapper
 *
 * Returns: a new #AgsTracCollectionkMapper
 *
 * Since: 0.4.3
 */
AgsTracCollectionkMapper*
ags_track_collection_mapper_new()
{
  AgsTracCollectionkMapper *track_collection_mapper;

  track_collection_mapper = (AgsTracCollectionkMapper *) g_object_new(AGS_TYPE_TRACK_COLLECTION_MAPPER,
								      NULL);
  
  return(track_collection_mapper);
}
