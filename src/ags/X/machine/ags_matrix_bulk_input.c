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

#include <ags/X/machine/ags_matrix_bulk_input.h>
#include <ags/X/machine/ags_matrix_bulk_input_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_plugin.h>

void ags_matrix_bulk_input_class_init(AgsMatrixBulkInputClass *matrix_bulk_input);
void ags_matrix_bulk_input_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_matrix_bulk_input_plugin_interface_init(AgsPluginInterface *plugin);
void ags_matrix_bulk_input_init(AgsMatrixBulkInput *matrix_bulk_input);
void ags_matrix_bulk_input_connect(AgsConnectable *connectable);
void ags_matrix_bulk_input_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_matrix_bulk_input
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsMatrixBulkInput
 * @section_id:
 * @include: ags/X/ags_matrix_bulk_input.h
 *
 * #AgsMatrixBulkInput is a composite widget to visualize one #AgsChannel. It should be
 * packed by an #AgsMatrixBulkInput.
 */

static gpointer ags_matrix_bulk_input_parent_class = NULL;
static AgsConnectableInterface *ags_matrix_bulk_input_parent_connectable_interface;

GType
ags_matrix_bulk_input_get_type(void)
{
  static GType ags_type_matrix_bulk_input = 0;

  if(!ags_type_matrix_bulk_input){
    static const GTypeInfo ags_matrix_bulk_input_info = {
      sizeof(AgsMatrixBulkInputClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_matrix_bulk_input_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsMatrixBulkInput),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_matrix_bulk_input_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_matrix_bulk_input_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_matrix_bulk_input_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_matrix_bulk_input = g_type_register_static(AGS_TYPE_EFFECT_BULK,
							"AgsMatrixBulkInput\0", &ags_matrix_bulk_input_info,
							0);

    g_type_add_interface_static(ags_type_matrix_bulk_input,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_matrix_bulk_input,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_matrix_bulk_input);
}

void
ags_matrix_bulk_input_class_init(AgsMatrixBulkInputClass *matrix_bulk_input)
{
  GObjectClass *gobject;

  ags_matrix_bulk_input_parent_class = g_type_class_peek_parent(matrix_bulk_input);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(matrix_bulk_input);
}

void
ags_matrix_bulk_input_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_matrix_bulk_input_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_matrix_bulk_input_connect;
  connectable->disconnect = ags_matrix_bulk_input_disconnect;
}

void
ags_matrix_bulk_input_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = NULL;
  plugin->set_name = NULL;
  plugin->get_xml_type = NULL;
  plugin->set_xml_type = NULL;
  plugin->get_ports = NULL;
  plugin->read = NULL;
  plugin->write = NULL;
  plugin->set_ports = NULL;
}

void
ags_matrix_bulk_input_init(AgsMatrixBulkInput *matrix_bulk_input)
{
  //TODO:JK: implement me
}

void
ags_matrix_bulk_input_connect(AgsConnectable *connectable)
{
  if((AGS_EFFECT_BULK_CONNECTED & (AGS_EFFECT_BULK(connectable)->flags)) != 0){
    return;
  }

  ags_matrix_bulk_input_parent_connectable_interface->connect(connectable);

  //TODO:JK: implement me
}

void
ags_matrix_bulk_input_disconnect(AgsConnectable *connectable)
{
  if((AGS_EFFECT_BULK_CONNECTED & (AGS_EFFECT_BULK(connectable)->flags)) == 0){
    return;
  }

  ags_matrix_bulk_input_parent_connectable_interface->disconnect(connectable);

  //TODO:JK: implement me
}


/**
 * ags_matrix_bulk_input_new:
 * @matrix_bulk_input: the parent matrix_bulk_input
 * @audio: the #AgsAudio to visualize
 * @channel_type: either %AGS_TYPE_INPUT or %AGS_TYPE_OUTPUT
 *
 * Creates an #AgsMatrixBulkInput
 *
 * Returns: a new #AgsMatrixBulkInput
 *
 * Since: 0.4
 */
AgsMatrixBulkInput*
ags_matrix_bulk_input_new(AgsAudio *audio,
			  GType channel_type)
{
  AgsMatrixBulkInput *matrix_bulk_input;

  matrix_bulk_input = (AgsMatrixBulkInput *) g_object_new(AGS_TYPE_MATRIX_BULK_INPUT,
							  "audio\0", audio,
							  "channel-type\0", channel_type,
							  NULL);

  return(matrix_bulk_input);
}
