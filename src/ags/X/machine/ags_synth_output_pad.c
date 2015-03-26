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

#include <ags/X/machine/ags_synth_output_pad.h>
#include <ags/X/machine/ags_synth_output_pad_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_plugin.h>

#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_line.h>

void ags_synth_output_pad_class_init(AgsSynthOutputPadClass *synth_output_pad);
void ags_synth_output_pad_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_synth_output_pad_plugin_interface_init(AgsPluginInterface *plugin);
void ags_synth_output_pad_init(AgsSynthOutputPad *synth_output_pad);
void ags_synth_output_pad_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_synth_output_pad_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_synth_output_pad_connect(AgsConnectable *connectable);
void ags_synth_output_pad_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_synth_output_pad
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsSynthOutputPad
 * @section_id:
 * @include: ags/X/ags_synth_output_pad.h
 *
 * #AgsSynthOutputPad is a composite widget to visualize one #AgsChannel. It should be
 * packed by an #AgsSynthOutputPad.
 */

enum{
  RESIZE_LINES,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_CHANNEL,
};

static gpointer ags_synth_output_pad_parent_class = NULL;
static guint synth_output_pad_signals[LAST_SIGNAL];

GType
ags_synth_output_pad_get_type(void)
{
  static GType ags_type_synth_output_pad = 0;

  if(!ags_type_synth_output_pad){
    static const GTypeInfo ags_synth_output_pad_info = {
      sizeof(AgsSynthOutputPadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_synth_output_pad_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSynthOutputPad),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_synth_output_pad_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_synth_output_pad_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_synth_output_pad_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_synth_output_pad = g_type_register_static(GTK_TYPE_VBOX,
						       "AgsSynthOutputPad\0", &ags_synth_output_pad_info,
						       0);

    g_type_add_interface_static(ags_type_synth_output_pad,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_synth_output_pad,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_synth_output_pad);
}

void
ags_synth_output_pad_class_init(AgsSynthOutputPadClass *synth_output_pad)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_synth_output_pad_parent_class = g_type_class_peek_parent(synth_output_pad);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(synth_output_pad);
}

void
ags_synth_output_pad_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_synth_output_pad_connect;
  connectable->disconnect = ags_synth_output_pad_disconnect;
}

void
ags_synth_output_pad_plugin_interface_init(AgsPluginInterface *plugin)
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
ags_synth_output_pad_init(AgsSynthOutputPad *synth_output_pad)
{
  //TODO:JK: implement me
}

void
ags_synth_output_pad_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_synth_output_pad_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_synth_output_pad_new:
 * @synth_output_pad: the parent synth_output_pad
 * @channel: the #AgsChannel to visualize
 *
 * Creates an #AgsSynthOutputPad
 *
 * Returns: a new #AgsSynthOutputPad
 *
 * Since: 0.4
 */
AgsSynthOutputPad*
ags_synth_output_pad_new(AgsChannel *channel)
{
  AgsSynthOutputPad *synth_output_pad;

  synth_output_pad = (AgsSynthOutputPad *) g_object_new(AGS_TYPE_SYNTH_OUTPUT_PAD,
							"channel\0", channel,
							NULL);

  return(synth_output_pad);
}
