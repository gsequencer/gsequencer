/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/ags_machine_editor_collection.h>
#include <ags/app/ags_machine_editor_collection_callbacks.h>

#include <ags/i18n.h>

void ags_machine_editor_collection_class_init(AgsMachineEditorCollectionClass *machine_editor_collection);
void ags_machine_editor_collection_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_machine_editor_collection_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_machine_editor_collection_init(AgsMachineEditorCollection *machine_editor_collection);
void ags_machine_editor_collection_set_property(GObject *gobject,
						guint prop_id,
						const GValue *value,
						GParamSpec *param_spec);
void ags_machine_editor_collection_get_property(GObject *gobject,
						guint prop_id,
						GValue *value,
						GParamSpec *param_spec);

void ags_machine_editor_collection_connect(AgsConnectable *connectable);
void ags_machine_editor_collection_disconnect(AgsConnectable *connectable);

void ags_machine_editor_collection_set_update(AgsApplicable *applicable, gboolean update);
void ags_machine_editor_collection_apply(AgsApplicable *applicable);
void ags_machine_editor_collection_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_machine_editor_collection
 * @short_description: Edit audio related aspects
 * @title: AgsMachineEditorCollection
 * @section_id:
 * @include: ags/app/ags_machine_editor_collection.h
 *
 * #AgsMachineEditorCollection is a composite widget to edit all aspects of #AgsAudio.
 * It consists of multiple child editor_collections.
 */

enum{
  PROP_0,
  PROP_CHANNEL_TYPE,
};

GType
ags_machine_editor_collection_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_machine_editor_collection = 0;

    static const GTypeInfo ags_machine_editor_collection_info = {
      sizeof (AgsMachineEditorCollectionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_machine_editor_collection_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMachineEditorCollection),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_machine_editor_collection_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_machine_editor_collection_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_machine_editor_collection_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_machine_editor_collection = g_type_register_static(GTK_TYPE_DIALOG,
								"AgsMachineEditorCollection", &ags_machine_editor_collection_info,
								0);

    g_type_add_interface_static(ags_type_machine_editor_collection,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_machine_editor_collection,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_machine_editor_collection);
  }

  return g_define_type_id__volatile;
}

void
ags_machine_editor_collection_class_init(AgsMachineEditorCollectionClass *machine_editor_collection)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  /* GObjectClass */
  gobject = (GObjectClass *) machine_editor_collection;

  gobject->set_property = ags_machine_editor_collection_set_property;
  gobject->get_property = ags_machine_editor_collection_get_property;

  /* properties */
  /**
   * AgsMachineEditorCollection:channel-type:
   *
   * The channel type.
   * 
   * Since: 4.0.0
   */
  param_spec = g_param_spec_gtype("channel-type",
				  i18n_pspec("channel type"),
				  i18n_pspec("The channel type"),
				  AGS_TYPE_CHANNEL,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL_TYPE,
				  param_spec);
}

void
ags_machine_editor_collection_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_machine_editor_collection_connect;
  connectable->disconnect = ags_machine_editor_collection_disconnect;
}

void
ags_machine_editor_collection_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_machine_editor_collection_set_update;
  applicable->apply = ags_machine_editor_collection_apply;
  applicable->reset = ags_machine_editor_collection_reset;
}

void
ags_machine_editor_collection_init(AgsMachineEditorCollection *machine_editor_collection)
{
  machine_editor_collection->connectable_flags = 0;
}

void
ags_machine_editor_collection_set_property(GObject *gobject,
					   guint prop_id,
					   const GValue *value,
					   GParamSpec *param_spec)
{
  AgsMachineEditorCollection *machine_editor_collection;

  machine_editor_collection = AGS_MACHINE_EDITOR_COLLECTION(gobject);

  switch(prop_id){
  case PROP_CHANNEL_TYPE:
    {
      machine_editor_collection->channel_type = g_value_get_gtype(value);  
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_editor_collection_get_property(GObject *gobject,
					   guint prop_id,
					   GValue *value,
					   GParamSpec *param_spec)
{
  AgsMachineEditorCollection *machine_editor_collection;

  machine_editor_collection = AGS_MACHINE_EDITOR_COLLECTION(gobject);

  switch(prop_id){
  case PROP_CHANNEL_TYPE:
    g_value_set_gtype(value, machine_editor_collection->channel_type);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_editor_collection_connect(AgsConnectable *connectable)
{
  AgsMachineEditorCollection *machine_editor_collection;

  machine_editor_collection = AGS_MACHINE_EDITOR_COLLECTION(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (machine_editor_collection->connectable_flags)) != 0){
    return;
  }

  machine_editor_collection->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  //TODO:JK: implement me
}

void
ags_machine_editor_collection_disconnect(AgsConnectable *connectable)
{
  AgsMachineEditorCollection *machine_editor_collection;

  machine_editor_collection = AGS_MACHINE_EDITOR_COLLECTION(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (machine_editor_collection->connectable_flags)) == 0){
    return;
  }
  
  machine_editor_collection->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  //TODO:JK: implement me
}

void
ags_machine_editor_collection_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsMachineEditorCollection *machine_editor_collection;

  machine_editor_collection = AGS_MACHINE_EDITOR_COLLECTION(applicable);

  //TODO:JK: implement me
}

void
ags_machine_editor_collection_apply(AgsApplicable *applicable)
{
  AgsMachineEditorCollection *machine_editor_collection;

  machine_editor_collection = AGS_MACHINE_EDITOR_COLLECTION(applicable);

  //TODO:JK: implement me
}

void
ags_machine_editor_collection_reset(AgsApplicable *applicable)
{
  AgsMachineEditorCollection *machine_editor_collection;

  machine_editor_collection = AGS_MACHINE_EDITOR_COLLECTION(applicable);

  //TODO:JK: implement me
}

/**
 * ags_machine_editor_collection_new:
 * @channel_type: the channel type
 *
 * Creates an #AgsMachineEditorCollection
 *
 * Returns: a new #AgsMachineEditorCollection
 *
 * Since: 4.0.0
 */
AgsMachineEditorCollection*
ags_machine_editor_collection_new(GType channel_type)
{
  AgsMachineEditorCollection *machine_editor_collection;

  machine_editor_collection = (AgsMachineEditorCollection *) g_object_new(AGS_TYPE_MACHINE_EDITOR_COLLECTION,
									  "channel-type", channel_type,
									  NULL);

  return(machine_editor_collection);
}
