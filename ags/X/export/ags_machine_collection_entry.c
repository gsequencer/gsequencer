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

#include <ags/X/export/ags_machine_collection_entry.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/X/ags_machine.h>

#include <math.h>

void ags_machine_collection_entry_class_init(AgsMachineCollectionEntryClass *machine_collection_entry);
void ags_machine_collection_entry_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_machine_collection_entry_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_machine_collection_entry_init(AgsMachineCollectionEntry *machine_collection_entry);
void ags_machine_collection_entry_set_property(GObject *gobject,
					       guint prop_id,
					       const GValue *value,
					       GParamSpec *param_spec);
void ags_machine_collection_entry_get_property(GObject *gobject,
					       guint prop_id,
					       GValue *value,
					       GParamSpec *param_spec);
void ags_machine_collection_entry_connect(AgsConnectable *connectable);
void ags_machine_collection_entry_disconnect(AgsConnectable *connectable);
void ags_machine_collection_entry_set_update(AgsApplicable *applicable, gboolean update);
void ags_machine_collection_entry_apply(AgsApplicable *applicable);
void ags_machine_collection_entry_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_machine_collection_entry
 * @short_description: pack pad editors.
 * @title: AgsMachineCollectionEntry
 * @section_id:
 * @include: ags/X/ags_machine_collection_entry.h
 *
 * #AgsMachineCollectionEntry is a wizard to export midi files and do machine mapping.
 */

enum{
  PROP_0,
  PROP_MACHINE,
};

GType
ags_machine_collection_entry_get_type(void)
{
  static GType ags_type_machine_collection_entry = 0;

  if(!ags_type_machine_collection_entry){
    static const GTypeInfo ags_machine_collection_entry_info = {
      sizeof (AgsMachineCollectionEntryClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_machine_collection_entry_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMachineCollectionEntry),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_machine_collection_entry_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_machine_collection_entry_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_machine_collection_entry_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_machine_collection_entry = g_type_register_static(GTK_TYPE_TABLE,
							       "AgsMachineCollectionEntry\0", &ags_machine_collection_entry_info,
							       0);

    g_type_add_interface_static(ags_type_machine_collection_entry,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_machine_collection_entry,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }
  
  return(ags_type_machine_collection_entry);
}

void
ags_machine_collection_entry_class_init(AgsMachineCollectionEntryClass *machine_collection_entry)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  /* GObjectClass */
  gobject = (GObjectClass *) machine_collection_entry;

  gobject->set_property = ags_machine_collection_entry_set_property;
  gobject->get_property = ags_machine_collection_entry_get_property;

  /* properties */
  /**
   * AgsMachineCollectionEntry:machine:
   *
   * The machines as xmlNode to parse.
   * 
   * Since: 0.8.0
   */
  param_spec = g_param_spec_object("machine\0",
				   "assigned machine\0",
				   "The machine which this machine entry is assigned with\0",
				   AGS_TYPE_MACHINE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MACHINE,
				  param_spec);
}

void
ags_machine_collection_entry_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_machine_collection_entry_connect;
  connectable->disconnect = ags_machine_collection_entry_disconnect;
}

void
ags_machine_collection_entry_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_machine_collection_entry_set_update;
  applicable->apply = ags_machine_collection_entry_apply;
  applicable->reset = ags_machine_collection_entry_reset;
}

void
ags_machine_collection_entry_init(AgsMachineCollectionEntry *machine_collection_entry)
{
}

void
ags_machine_collection_entry_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec)
{
  AgsMachineCollectionEntry *machine_collection_entry;

  machine_collection_entry = AGS_MACHINE_COLLECTION_ENTRY(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    {
      GtkWidget *machine;

      machine = (GtkWidget *) g_value_get_object(value);

      if(machine_collection_entry->machine == machine){
	return;
      }

      if(machine_collection_entry->machine != NULL){
	g_object_unref(machine_collection_entry->machine);
      }
      
      if(machine != NULL){
	g_object_ref(machine);
      }

      machine_collection_entry->machine = machine;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_collection_entry_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec)
{
  AgsMachineCollectionEntry *machine_collection_entry;

  machine_collection_entry = AGS_MACHINE_COLLECTION_ENTRY(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    {
      g_value_set_object(value, machine_collection_entry->machine);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_collection_entry_connect(AgsConnectable *connectable)
{
  AgsMachineCollectionEntry *machine_collection_entry;

  machine_collection_entry = AGS_MACHINE_COLLECTION_ENTRY(connectable);
}

void
ags_machine_collection_entry_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_machine_collection_entry_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsMachineCollectionEntry *machine_collection_entry;

  machine_collection_entry = AGS_MACHINE_COLLECTION_ENTRY(applicable);
}

void
ags_machine_collection_entry_apply(AgsApplicable *applicable)
{
  AgsMachineCollectionEntry *machine_collection_entry;

  machine_collection_entry = AGS_MACHINE_COLLECTION_ENTRY(applicable);
}

void
ags_machine_collection_entry_reset(AgsApplicable *applicable)
{
  AgsMachineCollectionEntry *machine_collection_entry;

  machine_collection_entry = AGS_MACHINE_COLLECTION_ENTRY(applicable);
}

/**
 * ags_machine_collection_entry_new:
 *
 * Creates an #AgsMachineCollectionEntry
 *
 * Returns: a new #AgsMachineCollectionEntry
 *
 * Since: 0.8.0
 */
AgsMachineCollectionEntry*
ags_machine_collection_entry_new()
{
  AgsMachineCollectionEntry *machine_collection_entry;

  machine_collection_entry = (AgsMachineCollectionEntry *) g_object_new(AGS_TYPE_MACHINE_COLLECTION_ENTRY,
									NULL);
  
  return(machine_collection_entry);
}
