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

#include <ags/X/machine/ags_synth_bulk_output.h>
#include <ags/X/machine/ags_synth_bulk_output_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_plugin.h>

void ags_synth_bulk_output_class_init(AgsSynthBulkOutputClass *synth_bulk_output);
void ags_synth_bulk_output_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_synth_bulk_output_plugin_interface_init(AgsPluginInterface *plugin);
void ags_synth_bulk_output_init(AgsSynthBulkOutput *synth_bulk_output);
void ags_synth_bulk_output_connect(AgsConnectable *connectable);
void ags_synth_bulk_output_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_synth_bulk_output
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsSynthBulkOutput
 * @section_id:
 * @include: ags/X/ags_synth_bulk_output.h
 *
 * #AgsSynthBulkOutput is a composite widget to visualize one #AgsChannel. It should be
 * packed by an #AgsSynthBulkOutput.
 */

static gpointer ags_synth_bulk_output_parent_class = NULL;

GType
ags_synth_bulk_output_get_type(void)
{
  static GType ags_type_synth_bulk_output = 0;

  if(!ags_type_synth_bulk_output){
    static const GTypeInfo ags_synth_bulk_output_info = {
      sizeof(AgsSynthBulkOutputClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_synth_bulk_output_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSynthBulkOutput),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_synth_bulk_output_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_synth_bulk_output_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_synth_bulk_output_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_synth_bulk_output = g_type_register_static(AGS_TYPE_EFFECT_BULK,
							"AgsSynthBulkOutput\0", &ags_synth_bulk_output_info,
							0);

    g_type_add_interface_static(ags_type_synth_bulk_output,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_synth_bulk_output,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_synth_bulk_output);
}

void
ags_synth_bulk_output_class_init(AgsSynthBulkOutputClass *synth_bulk_output)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_synth_bulk_output_parent_class = g_type_class_peek_parent(synth_bulk_output);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(synth_bulk_output);
}

void
ags_synth_bulk_output_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_synth_bulk_output_connect;
  connectable->disconnect = ags_synth_bulk_output_disconnect;
}

void
ags_synth_bulk_output_plugin_interface_init(AgsPluginInterface *plugin)
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
ags_synth_bulk_output_init(AgsSynthBulkOutput *synth_bulk_output)
{
  //TODO:JK: implement me
}

void
ags_synth_bulk_output_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_synth_bulk_output_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_synth_bulk_output_new:
 * @synth_bulk_output: the parent synth_bulk_output
 * @audio: the #AgsAudio to visualize
 * @channel_type: either %AGS_TYPE_INPUT or %AGS_TYPE_OUTPUT
 *
 * Creates an #AgsSynthBulkOutput
 *
 * Returns: a new #AgsSynthBulkOutput
 *
 * Since: 0.4
 */
AgsSynthBulkOutput*
ags_synth_bulk_output_new(AgsAudio *audio,
			  GType channel_type)
{
  AgsSynthBulkOutput *synth_bulk_output;

  synth_bulk_output = (AgsSynthBulkOutput *) g_object_new(AGS_TYPE_SYNTH_BULK_OUTPUT,
							  "audio\0", audio,
							  NULL);

  return(synth_bulk_output);
}
