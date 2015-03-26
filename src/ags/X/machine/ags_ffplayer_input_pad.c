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

#include <ags/X/machine/ags_ffplayer_input_pad.h>
#include <ags/X/machine/ags_ffplayer_input_pad_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_plugin.h>

#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_line.h>

void ags_ffplayer_input_pad_class_init(AgsFFPlayerInputPadClass *ffplayer_input_pad);
void ags_ffplayer_input_pad_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ffplayer_input_pad_plugin_interface_init(AgsPluginInterface *plugin);
void ags_ffplayer_input_pad_init(AgsFFPlayerInputPad *ffplayer_input_pad);
void ags_ffplayer_input_pad_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_ffplayer_input_pad_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_ffplayer_input_pad_connect(AgsConnectable *connectable);
void ags_ffplayer_input_pad_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_ffplayer_input_pad
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsFFPlayerInputPad
 * @section_id:
 * @include: ags/X/ags_ffplayer_input_pad.h
 *
 * #AgsFFPlayerInputPad is a composite widget to visualize one #AgsChannel. It should be
 * packed by an #AgsFFPlayerInputPad.
 */

enum{
  RESIZE_LINES,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_CHANNEL,
};

static gpointer ags_ffplayer_input_pad_parent_class = NULL;
static guint ffplayer_input_pad_signals[LAST_SIGNAL];

GType
ags_ffplayer_input_pad_get_type(void)
{
  static GType ags_type_ffplayer_input_pad = 0;

  if(!ags_type_ffplayer_input_pad){
    static const GTypeInfo ags_ffplayer_input_pad_info = {
      sizeof(AgsFFPlayerInputPadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ffplayer_input_pad_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFFPlayerInputPad),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ffplayer_input_pad_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ffplayer_input_pad_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_ffplayer_input_pad_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ffplayer_input_pad = g_type_register_static(GTK_TYPE_VBOX,
							 "AgsFFPlayerInputPad\0", &ags_ffplayer_input_pad_info,
							 0);

    g_type_add_interface_static(ags_type_ffplayer_input_pad,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_ffplayer_input_pad,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_ffplayer_input_pad);
}

void
ags_ffplayer_input_pad_class_init(AgsFFPlayerInputPadClass *ffplayer_input_pad)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_ffplayer_input_pad_parent_class = g_type_class_peek_parent(ffplayer_input_pad);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(ffplayer_input_pad);
}

void
ags_ffplayer_input_pad_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_ffplayer_input_pad_connect;
  connectable->disconnect = ags_ffplayer_input_pad_disconnect;
}

void
ags_ffplayer_input_pad_plugin_interface_init(AgsPluginInterface *plugin)
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
ags_ffplayer_input_pad_init(AgsFFPlayerInputPad *ffplayer_input_pad)
{
  //TODO:JK: implement me
}

void
ags_ffplayer_input_pad_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_ffplayer_input_pad_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_ffplayer_input_pad_new:
 * @ffplayer_input_pad: the parent ffplayer_input_pad
 * @channel: the #AgsChannel to visualize
 *
 * Creates an #AgsFFPlayerInputPad
 *
 * Returns: a new #AgsFFPlayerInputPad
 *
 * Since: 0.4
 */
AgsFFPlayerInputPad*
ags_ffplayer_input_pad_new(AgsChannel *channel)
{
  AgsFFPlayerInputPad *ffplayer_input_pad;

  ffplayer_input_pad = (AgsFFPlayerInputPad *) g_object_new(AGS_TYPE_FFPLAYER_INPUT_PAD,
							    "channel\0", channel,
							    NULL);

  return(ffplayer_input_pad);
}
