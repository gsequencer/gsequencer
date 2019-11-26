/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/X/machine/ags_fm_synth_input_pad.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_window.h>

#include <ags/X/machine/ags_fm_synth.h>

#include <math.h>

void ags_fm_synth_input_pad_class_init(AgsFMSynthInputPadClass *fm_synth_input_pad);
void ags_fm_synth_input_pad_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_fm_synth_input_pad_plugin_interface_init(AgsPluginInterface *plugin);
void ags_fm_synth_input_pad_init(AgsFMSynthInputPad *fm_synth_input_pad);

void ags_fm_synth_input_pad_connect(AgsConnectable *connectable);
void ags_fm_synth_input_pad_disconnect(AgsConnectable *connectable);

gchar* ags_fm_synth_input_pad_get_name(AgsPlugin *plugin);
void ags_fm_synth_input_pad_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_fm_synth_input_pad_get_xml_type(AgsPlugin *plugin);
void ags_fm_synth_input_pad_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_fm_synth_input_pad_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_fm_synth_input_pad_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_fm_synth_input_pad_set_channel(AgsPad *pad, AgsChannel *channel);
void ags_fm_synth_input_pad_resize_lines(AgsPad *pad, GType line_type,
					 guint audio_channels, guint audio_channels_old);

/**
 * SECTION:ags_fm_synth_input_pad
 * @short_description: fm synth input pad
 * @title: AgsFMSynthInputPad
 * @section_id:
 * @include: ags/X/machine/ags_fm_synth_input_pad.h
 *
 * The #AgsFMSynthInputPad is a composite widget to act as fm synth input pad.
 */

static gpointer ags_fm_synth_input_pad_parent_class = NULL;
static AgsConnectableInterface *ags_fm_synth_input_pad_parent_connectable_interface;

GType
ags_fm_synth_input_pad_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fm_synth_input_pad = 0;

    static const GTypeInfo ags_fm_synth_input_pad_info = {
      sizeof(AgsFMSynthInputPadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fm_synth_input_pad_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFMSynthInputPad),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fm_synth_input_pad_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_fm_synth_input_pad_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_fm_synth_input_pad_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_fm_synth_input_pad = g_type_register_static(AGS_TYPE_PAD,
							 "AgsFMSynthInputPad", &ags_fm_synth_input_pad_info,
							 0);

    g_type_add_interface_static(ags_type_fm_synth_input_pad,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_fm_synth_input_pad,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fm_synth_input_pad);
  }

  return g_define_type_id__volatile;
}

void
ags_fm_synth_input_pad_class_init(AgsFMSynthInputPadClass *fm_synth_input_pad)
{
  AgsPadClass *pad;

  ags_fm_synth_input_pad_parent_class = g_type_class_peek_parent(fm_synth_input_pad);

  pad = (AgsPadClass *) fm_synth_input_pad;

  pad->set_channel = ags_fm_synth_input_pad_set_channel;
  pad->resize_lines = ags_fm_synth_input_pad_resize_lines;
}

void
ags_fm_synth_input_pad_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_fm_synth_input_pad_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_fm_synth_input_pad_connect;
  connectable->disconnect = ags_fm_synth_input_pad_disconnect;
}

void
ags_fm_synth_input_pad_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_fm_synth_input_pad_get_name;
  plugin->set_name = ags_fm_synth_input_pad_set_name;
  plugin->get_xml_type = ags_fm_synth_input_pad_get_xml_type;
  plugin->set_xml_type = ags_fm_synth_input_pad_set_xml_type;
  plugin->read = ags_fm_synth_input_pad_read;
  plugin->write = ags_fm_synth_input_pad_write;
}

void
ags_fm_synth_input_pad_init(AgsFMSynthInputPad *fm_synth_input_pad)
{
  fm_synth_input_pad->name = NULL;
  fm_synth_input_pad->xml_type = "ags-fm_synth-input-pad";
}

void
ags_fm_synth_input_pad_connect(AgsConnectable *connectable)
{
  AgsFMSynthInputPad *fm_synth_input_pad;

  /* AgsFMSynthInputPad */
  fm_synth_input_pad = AGS_FM_SYNTH_INPUT_PAD(connectable);

  if((AGS_PAD_CONNECTED & (AGS_PAD(fm_synth_input_pad)->flags)) != 0){
    return;
  }

  ags_fm_synth_input_pad_parent_connectable_interface->connect(connectable);
}

void
ags_fm_synth_input_pad_disconnect(AgsConnectable *connectable)
{
  ags_fm_synth_input_pad_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

gchar*
ags_fm_synth_input_pad_get_name(AgsPlugin *plugin)
{
  return(AGS_FM_SYNTH_INPUT_PAD(plugin)->name);
}

void
ags_fm_synth_input_pad_set_name(AgsPlugin *plugin, gchar *name)
{
  AGS_FM_SYNTH_INPUT_PAD(plugin)->name = name;
}

gchar*
ags_fm_synth_input_pad_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_FM_SYNTH_INPUT_PAD(plugin)->xml_type);
}

void
ags_fm_synth_input_pad_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_FM_SYNTH_INPUT_PAD(plugin)->xml_type = xml_type;
}

void
ags_fm_synth_input_pad_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsFMSynthInputPad *gobject;
  AgsFileLookup *file_lookup;

  gobject = AGS_FM_SYNTH_INPUT_PAD(plugin);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference", gobject,
				   NULL));
}

xmlNode*
ags_fm_synth_input_pad_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsFMSynthInputPad *fm_synth_input_pad;
  xmlNode *node;
  gchar *id;

  fm_synth_input_pad = AGS_FM_SYNTH_INPUT_PAD(plugin);
  node = NULL;

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-fm-synth-input-pad");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", fm_synth_input_pad,
				   NULL));

  xmlAddChild(parent,
	      node);  

  return(node);
}

void
ags_fm_synth_input_pad_set_channel(AgsPad *pad, AgsChannel *channel)
{
  AGS_PAD_CLASS(ags_fm_synth_input_pad_parent_class)->set_channel(pad, channel);

  /* empty */
}

void
ags_fm_synth_input_pad_resize_lines(AgsPad *pad, GType line_type,
				    guint audio_channels, guint audio_channels_old)
{
  AGS_PAD_CLASS(ags_fm_synth_input_pad_parent_class)->resize_lines(pad, line_type,
								   audio_channels, audio_channels_old);

  /* empty */
}

/**
 * ags_fm_synth_input_pad_new:
 * @channel: the assigned #AgsChannel
 *
 * Create a new instance of #AgsFMSynthInputPad
 *
 * Returns: the new #AgsFMSynthInputPad
 *
 * Since: 2.3.0
 */
AgsFMSynthInputPad*
ags_fm_synth_input_pad_new(AgsChannel *channel)
{
  AgsFMSynthInputPad *fm_synth_input_pad;

  fm_synth_input_pad = (AgsFMSynthInputPad *) g_object_new(AGS_TYPE_FM_SYNTH_INPUT_PAD,
							   "channel", channel,
							   NULL);
  
  return(fm_synth_input_pad);
}
