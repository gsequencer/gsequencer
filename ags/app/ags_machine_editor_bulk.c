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

#include <ags/app/ags_machine_editor_bulk.h>
#include <ags/app/ags_machine_editor_bulk_callbacks.h>

#include <ags/app/ags_machine_editor.h>

#include <ags/i18n.h>

void ags_machine_editor_bulk_class_init(AgsMachineEditorBulkClass *machine_editor_bulk);
void ags_machine_editor_bulk_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_machine_editor_bulk_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_machine_editor_bulk_init(AgsMachineEditorBulk *machine_editor_bulk);

void ags_machine_editor_bulk_connect(AgsConnectable *connectable);
void ags_machine_editor_bulk_disconnect(AgsConnectable *connectable);

void ags_machine_editor_bulk_set_update(AgsApplicable *applicable, gboolean update);
void ags_machine_editor_bulk_apply(AgsApplicable *applicable);
void ags_machine_editor_bulk_reset(AgsApplicable *applicable);

static gpointer ags_machine_editor_bulk_parent_class = NULL;

/**
 * SECTION:ags_machine_editor_bulk
 * @short_description: Edit audio related aspects
 * @title: AgsMachineEditorBulk
 * @section_id:
 * @include: ags/app/ags_machine_editor_bulk.h
 *
 * #AgsMachineEditorBulk is a composite widget to edit all aspects of #AgsAudio.
 * It consists of multiple child editor_bulks.
 */

GType
ags_machine_editor_bulk_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_machine_editor_bulk = 0;

    static const GTypeInfo ags_machine_editor_bulk_info = {
      sizeof (AgsMachineEditorBulkClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_machine_editor_bulk_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMachineEditorBulk),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_machine_editor_bulk_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_machine_editor_bulk_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_machine_editor_bulk_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_machine_editor_bulk = g_type_register_static(GTK_TYPE_BOX,
							  "AgsMachineEditorBulk", &ags_machine_editor_bulk_info,
							  0);

    g_type_add_interface_static(ags_type_machine_editor_bulk,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_machine_editor_bulk,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_machine_editor_bulk);
  }

  return g_define_type_id__volatile;
}

void
ags_machine_editor_bulk_class_init(AgsMachineEditorBulkClass *machine_editor_bulk)
{
  GObjectClass *gobject;

  ags_machine_editor_bulk_parent_class = g_type_class_peek_parent(machine_editor_bulk);

  /* GObjectClass */
  gobject = (GObjectClass *) machine_editor_bulk;
}

void
ags_machine_editor_bulk_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_machine_editor_bulk_connect;
  connectable->disconnect = ags_machine_editor_bulk_disconnect;
}

void
ags_machine_editor_bulk_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_machine_editor_bulk_set_update;
  applicable->apply = ags_machine_editor_bulk_apply;
  applicable->reset = ags_machine_editor_bulk_reset;
}

void
ags_machine_editor_bulk_init(AgsMachineEditorBulk *machine_editor_bulk)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(machine_editor_bulk),
				 GTK_ORIENTATION_VERTICAL);
  
  machine_editor_bulk->connectable_flags = 0;
}

void
ags_machine_editor_bulk_connect(AgsConnectable *connectable)
{
  AgsMachineEditorBulk *machine_editor_bulk;

  machine_editor_bulk = AGS_MACHINE_EDITOR_BULK(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (machine_editor_bulk->connectable_flags)) != 0){
    return;
  }

  machine_editor_bulk->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  //TODO:JK: implement me
}

void
ags_machine_editor_bulk_disconnect(AgsConnectable *connectable)
{
  AgsMachineEditorBulk *machine_editor_bulk;

  machine_editor_bulk = AGS_MACHINE_EDITOR_BULK(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (machine_editor_bulk->connectable_flags)) == 0){
    return;
  }
  
  machine_editor_bulk->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  //TODO:JK: implement me
}

void
ags_machine_editor_bulk_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsMachineEditorBulk *machine_editor_bulk;

  machine_editor_bulk = AGS_MACHINE_EDITOR_BULK(applicable);

  //TODO:JK: implement me
}

void
ags_machine_editor_bulk_apply(AgsApplicable *applicable)
{
  AgsMachineEditorBulk *machine_editor_bulk;

  machine_editor_bulk = AGS_MACHINE_EDITOR_BULK(applicable);
}

void
ags_machine_editor_bulk_reset(AgsApplicable *applicable)
{
  AgsMachineEditorBulk *machine_editor_bulk;

  machine_editor_bulk = AGS_MACHINE_EDITOR_BULK(applicable);  

  //TODO:JK: implement me
}

/**
 * ags_machine_editor_bulk_new:
 * @channel_type: the channel type
 *
 * Creates an #AgsMachineEditorBulk
 *
 * Returns: a new #AgsMachineEditorBulk
 *
 * Since: 4.0.0
 */
AgsMachineEditorBulk*
ags_machine_editor_bulk_new(AgsChannel *channel)
{
  AgsMachineEditorBulk *machine_editor_bulk;

  machine_editor_bulk = (AgsMachineEditorBulk *) g_object_new(AGS_TYPE_MACHINE_EDITOR_BULK,
							      "channel", channel,
							      NULL);

  return(machine_editor_bulk);
}
