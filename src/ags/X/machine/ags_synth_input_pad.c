/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/X/machine/ags_synth_input_pad.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_plugin.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>

#include <ags/X/ags_window.h>

#include <ags/X/machine/ags_synth.h>

#include <math.h>

void ags_synth_input_pad_class_init(AgsSynthInputPadClass *synth_input_pad);
void ags_synth_input_pad_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_synth_input_pad_plugin_interface_init(AgsPluginInterface *plugin);
void ags_synth_input_pad_init(AgsSynthInputPad *synth_input_pad);
void ags_synth_input_pad_destroy(GtkObject *object);
void ags_synth_input_pad_connect(AgsConnectable *connectable);
void ags_synth_input_pad_disconnect(AgsConnectable *connectable);
gchar* ags_synth_input_pad_get_name(AgsPlugin *plugin);
void ags_synth_input_pad_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_synth_input_pad_get_xml_type(AgsPlugin *plugin);
void ags_synth_input_pad_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_synth_input_pad_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_synth_input_pad_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_synth_input_pad_set_channel(AgsPad *pad, AgsChannel *channel);
void ags_synth_input_pad_resize_lines(AgsPad *pad, GType line_type,
				      guint audio_channels, guint audio_channels_old);

/**
 * SECTION:ags_synth_input_pad
 * @short_description: synth input pad
 * @title: AgsSynthInputPad
 * @section_id:
 * @include: ags/X/machine/ags_synth_input_pad.h
 *
 * The #AgsSynthInputPad is a composite widget to act as synth input pad.
 */

static gpointer ags_synth_input_pad_parent_class = NULL;
static AgsConnectableInterface *ags_synth_input_pad_parent_connectable_interface;

GType
ags_synth_input_pad_get_type()
{
  static GType ags_type_synth_input_pad = 0;

  if(!ags_type_synth_input_pad){
    static const GTypeInfo ags_synth_input_pad_info = {
      sizeof(AgsSynthInputPadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_synth_input_pad_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSynthInputPad),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_synth_input_pad_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_synth_input_pad_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_synth_input_pad_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_synth_input_pad = g_type_register_static(AGS_TYPE_PAD,
						      "AgsSynthInputPad\0", &ags_synth_input_pad_info,
						      0);

    g_type_add_interface_static(ags_type_synth_input_pad,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_synth_input_pad,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_synth_input_pad);
}

void
ags_synth_input_pad_class_init(AgsSynthInputPadClass *synth_input_pad)
{
  AgsPadClass *pad;

  ags_synth_input_pad_parent_class = g_type_class_peek_parent(synth_input_pad);

  pad = (AgsPadClass *) synth_input_pad;

  pad->set_channel = ags_synth_input_pad_set_channel;
  pad->resize_lines = ags_synth_input_pad_resize_lines;
}

void
ags_synth_input_pad_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_synth_input_pad_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_synth_input_pad_connect;
  connectable->disconnect = ags_synth_input_pad_disconnect;
}

void
ags_synth_input_pad_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_synth_input_pad_get_name;
  plugin->set_name = ags_synth_input_pad_set_name;
  plugin->get_xml_type = ags_synth_input_pad_get_xml_type;
  plugin->set_xml_type = ags_synth_input_pad_set_xml_type;
  plugin->read = ags_synth_input_pad_read;
  plugin->write = ags_synth_input_pad_write;
}

void
ags_synth_input_pad_init(AgsSynthInputPad *synth_input_pad)
{
  synth_input_pad->name = NULL;
  synth_input_pad->xml_type = "ags-synth-input-pad\0";
}

void
ags_synth_input_pad_connect(AgsConnectable *connectable)
{
  AgsSynthInputPad *synth_input_pad;

  /* AgsSynthInputPad */
  synth_input_pad = AGS_SYNTH_INPUT_PAD(connectable);

  if((AGS_PAD_CONNECTED & (AGS_PAD(synth_input_pad)->flags)) != 0){
    return;
  }

  ags_synth_input_pad_parent_connectable_interface->connect(connectable);
}

void
ags_synth_input_pad_disconnect(AgsConnectable *connectable)
{
  ags_synth_input_pad_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_synth_input_pad_destroy(GtkObject *object)
{
  /* empty */
}

gchar*
ags_synth_input_pad_get_name(AgsPlugin *plugin)
{
  return(AGS_SYNTH_INPUT_PAD(plugin)->name);
}

void
ags_synth_input_pad_set_name(AgsPlugin *plugin, gchar *name)
{
  AGS_SYNTH_INPUT_PAD(plugin)->name = name;
}

gchar*
ags_synth_input_pad_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_SYNTH_INPUT_PAD(plugin)->xml_type);
}

void
ags_synth_input_pad_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_SYNTH_INPUT_PAD(plugin)->xml_type = xml_type;
}

void
ags_synth_input_pad_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsSynthInputPad *gobject;
  AgsFileLookup *file_lookup;

  gobject = AGS_SYNTH_INPUT_PAD(plugin);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));
}

xmlNode*
ags_synth_input_pad_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsSynthInputPad *synth_input_pad;
  xmlNode *node;
  gchar *id;

  synth_input_pad = AGS_SYNTH_INPUT_PAD(plugin);
  node = NULL;

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-synth-input-pad\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", synth_input_pad,
				   NULL));

  xmlAddChild(parent,
	      node);  

  return(node);
}

void
ags_synth_input_pad_set_channel(AgsPad *pad, AgsChannel *channel)
{
  AGS_PAD_CLASS(ags_synth_input_pad_parent_class)->set_channel(pad, channel);

  /* empty */
}

void
ags_synth_input_pad_resize_lines(AgsPad *pad, GType line_type,
				 guint audio_channels, guint audio_channels_old)
{
  AGS_PAD_CLASS(ags_synth_input_pad_parent_class)->resize_lines(pad, line_type,
								audio_channels, audio_channels_old);

  /* empty */
}

/**
 * ags_synth_input_pad_new:
 * @channel: the assigned channel
 *
 * Creates an #AgsSynthInputPad
 *
 * Returns: a new #AgsSynthInputPad
 *
 * Since: 0.4
 */
AgsSynthInputPad*
ags_synth_input_pad_new(AgsChannel *channel)
{
  AgsSynthInputPad *synth_input_pad;

  synth_input_pad = (AgsSynthInputPad *) g_object_new(AGS_TYPE_SYNTH_INPUT_PAD,
						      "channel\0", channel,
						      NULL);
  
  return(synth_input_pad);
}

