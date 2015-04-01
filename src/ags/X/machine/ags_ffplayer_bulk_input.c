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

#include <ags/X/machine/ags_ffplayer_bulk_input.h>
#include <ags/X/machine/ags_ffplayer_bulk_input_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_plugin.h>

void ags_ffplayer_bulk_input_class_init(AgsFFPlayerBulkInputClass *ffplayer_bulk_input);
void ags_ffplayer_bulk_input_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ffplayer_bulk_input_plugin_interface_init(AgsPluginInterface *plugin);
void ags_ffplayer_bulk_input_init(AgsFFPlayerBulkInput *ffplayer_bulk_input);
void ags_ffplayer_bulk_input_connect(AgsConnectable *connectable);
void ags_ffplayer_bulk_input_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_ffplayer_bulk_input
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsFFPlayerBulkInput
 * @section_id:
 * @include: ags/X/ags_ffplayer_bulk_input.h
 *
 * #AgsFFPlayerBulkInput is a composite widget to visualize one #AgsChannel. It should be
 * packed by an #AgsFFPlayerBulkInput.
 */

static gpointer ags_ffplayer_bulk_input_parent_class = NULL;
static AgsConnectableInterface *ags_ffplayer_bulk_input_parent_connectable_interface;

GType
ags_ffplayer_bulk_input_get_type(void)
{
  static GType ags_type_ffplayer_bulk_input = 0;

  if(!ags_type_ffplayer_bulk_input){
    static const GTypeInfo ags_ffplayer_bulk_input_info = {
      sizeof(AgsFFPlayerBulkInputClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ffplayer_bulk_input_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFFPlayerBulkInput),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ffplayer_bulk_input_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ffplayer_bulk_input_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_ffplayer_bulk_input_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ffplayer_bulk_input = g_type_register_static(AGS_TYPE_EFFECT_BULK,
							  "AgsFFPlayerBulkInput\0", &ags_ffplayer_bulk_input_info,
							  0);

    g_type_add_interface_static(ags_type_ffplayer_bulk_input,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_ffplayer_bulk_input,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_ffplayer_bulk_input);
}

void
ags_ffplayer_bulk_input_class_init(AgsFFPlayerBulkInputClass *ffplayer_bulk_input)
{
  GObjectClass *gobject;

  ags_ffplayer_bulk_input_parent_class = g_type_class_peek_parent(ffplayer_bulk_input);

  /* GObjectClass */
  gobject = (GObjectClass *) ffplayer_bulk_input;
}

void
ags_ffplayer_bulk_input_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_ffplayer_bulk_input_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_ffplayer_bulk_input_connect;
  connectable->disconnect = ags_ffplayer_bulk_input_disconnect;
}

void
ags_ffplayer_bulk_input_plugin_interface_init(AgsPluginInterface *plugin)
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
ags_ffplayer_bulk_input_init(AgsFFPlayerBulkInput *ffplayer_bulk_input)
{
  //TODO:JK: implement me
}

void
ags_ffplayer_bulk_input_connect(AgsConnectable *connectable)
{
  if((AGS_EFFECT_BULK_CONNECTED & (AGS_EFFECT_BULK(connectable)->flags)) != 0){
    return;
  }

  ags_ffplayer_bulk_input_parent_connectable_interface->connect(connectable);

  //TODO:JK: implement me
}

void
ags_ffplayer_bulk_input_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_ffplayer_bulk_input_new:
 * @ffplayer_bulk_input: the parent ffplayer_bulk_input
 * @audio: the #AgsAudio to visualize
 * @channel_type: either %AGS_TYPE_INPUT or %AGS_TYPE_OUTPUT
 *
 * Creates an #AgsFFPlayerBulkInput
 *
 * Returns: a new #AgsFFPlayerBulkInput
 *
 * Since: 0.4
 */
AgsFFPlayerBulkInput*
ags_ffplayer_bulk_input_new(AgsAudio *audio,
			    GType channel_type)
{
  AgsFFPlayerBulkInput *ffplayer_bulk_input;

  ffplayer_bulk_input = (AgsFFPlayerBulkInput *) g_object_new(AGS_TYPE_FFPLAYER_BULK_INPUT,
							      "audio\0", audio,
							      "channel-type\0", channel_type,
							      NULL);

  return(ffplayer_bulk_input);
}
