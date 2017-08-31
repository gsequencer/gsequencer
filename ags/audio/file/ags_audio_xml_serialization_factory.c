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

#include <ags/audio/file/ags_audio_xml_serialization_factory.h>

#include <ags/object/ags_connectable.h>

void ags_audio_xml_serialization_factory_class_init(AgsAudioXmlSerializationFactoryClass *audio_xml_serialization_factory_class);
void ags_audio_xml_serialization_factory_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_xml_serialization_factory_init(AgsAudioXmlSerializationFactory *audio_xml_serialization_factory);

/**
 * SECTION:ags_audio_xml_serialization_factory
 * @short_description: Factory pattern
 * @title: AgsAudioXmlSerializationFactory
 * @section_id:
 * @include: ags/audio/file/ags_audio_xml_serialization_factory.h
 *
 * #AgsAudioXmlSerializationFactory allocates or instantiates specified #xmlNode-struct.
 */

static gpointer ags_audio_xml_serialization_factory_parent_class = NULL;

GType
ags_audio_xml_serialization_factory_get_type (void)
{
  static GType ags_type_audio_xml_serialization_factory = 0;

  if(!ags_type_audio_xml_serialization_factory){
    static const GTypeInfo ags_audio_xml_serialization_factory_info = {
      sizeof (AgsAudioXmlSerializationFactoryClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_xml_serialization_factory_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioXmlSerializationFactory),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_xml_serialization_factory_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_xml_serialization_factory_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_xml_serialization_factory = g_type_register_static(G_TYPE_OBJECT,
								      "AgsAudioXmlSerializationFactory",
								      &ags_audio_xml_serialization_factory_info,
								      0);

    g_type_add_interface_static(ags_type_audio_xml_serialization_factory,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_audio_xml_serialization_factory);
}

void
ags_audio_xml_serialization_factory_class_init(AgsAudioXmlSerializationFactoryClass *audio_xml_serialization_factory)
{
  ags_audio_xml_serialization_factory_parent_class = g_type_class_peek_parent(audio_xml_serialization_factory);
}

void
ags_audio_xml_serialization_factory_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;
  connectable->connect = NULL;
  connectable->disconnect = NULL;
}

void
ags_audio_xml_serialization_factory_init(AgsAudioXmlSerializationFactory *audio_xml_serialization_factory)
{
}

gpointer
ags_audio_xml_serialization_factory_create(AgsAudioXmlSerializationFactory *audio_xml_serialization_factory,
					   xmlNode *node)
{
  gpointer ret_val;

  ret_val = NULL;

  //TODO:JK: implement me

  return(ret_val);
}

/**
 * ags_audio_xml_serialization_factory_new:
 *
 * Creates a #AgsAudioXmlSerializationFactory
 *
 * Returns: a new #AgsAudioXmlSerializationFactory
 * 
 * Since: 1.0.0
 */
AgsAudioXmlSerializationFactory*
ags_audio_xml_serialization_factory_new()
{
  AgsAudioXmlSerializationFactory *audio_xml_serialization_factory;

  audio_xml_serialization_factory = (AgsAudioXmlSerializationFactory *) g_object_new(AGS_TYPE_AUDIO_XML_SERIALIZATION_FACTORY,
										     NULL);

  return(audio_xml_serialization_factory);
}
