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

#include <ags/X/machine/ags_syncsynth_input_pad.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_plugin.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>

#include <ags/X/ags_window.h>

#include <ags/X/machine/ags_syncsynth.h>

#include <math.h>

void ags_syncsynth_input_pad_class_init(AgsSyncsynthInputPadClass *syncsynth_input_pad);
void ags_syncsynth_input_pad_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_syncsynth_input_pad_plugin_interface_init(AgsPluginInterface *plugin);
void ags_syncsynth_input_pad_init(AgsSyncsynthInputPad *syncsynth_input_pad);
void ags_syncsynth_input_pad_destroy(GtkObject *object);
void ags_syncsynth_input_pad_connect(AgsConnectable *connectable);
void ags_syncsynth_input_pad_disconnect(AgsConnectable *connectable);
gchar* ags_syncsynth_input_pad_get_name(AgsPlugin *plugin);
void ags_syncsynth_input_pad_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_syncsynth_input_pad_get_xml_type(AgsPlugin *plugin);
void ags_syncsynth_input_pad_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_syncsynth_input_pad_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_syncsynth_input_pad_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_syncsynth_input_pad_set_channel(AgsPad *pad, AgsChannel *channel);
void ags_syncsynth_input_pad_resize_lines(AgsPad *pad, GType line_type,
					  guint audio_channels, guint audio_channels_old);

/**
 * SECTION:ags_syncsynth_input_pad
 * @short_description: syncsynth input pad
 * @title: AgsSyncsynthInputPad
 * @section_id:
 * @include: ags/X/machine/ags_syncsynth_input_pad.h
 *
 * The #AgsSyncsynthInputPad is a composite widget to act as syncsynth input pad.
 */

static gpointer ags_syncsynth_input_pad_parent_class = NULL;
static AgsConnectableInterface *ags_syncsynth_input_pad_parent_connectable_interface;

GType
ags_syncsynth_input_pad_get_type()
{
  static GType ags_type_syncsynth_input_pad = 0;

  if(!ags_type_syncsynth_input_pad){
    static const GTypeInfo ags_syncsynth_input_pad_info = {
      sizeof(AgsSyncsynthInputPadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_syncsynth_input_pad_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSyncsynthInputPad),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_syncsynth_input_pad_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_syncsynth_input_pad_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_syncsynth_input_pad_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_syncsynth_input_pad = g_type_register_static(AGS_TYPE_PAD,
							  "AgsSyncsynthInputPad", &ags_syncsynth_input_pad_info,
							  0);

    g_type_add_interface_static(ags_type_syncsynth_input_pad,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_syncsynth_input_pad,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_syncsynth_input_pad);
}

void
ags_syncsynth_input_pad_class_init(AgsSyncsynthInputPadClass *syncsynth_input_pad)
{
  AgsPadClass *pad;

  ags_syncsynth_input_pad_parent_class = g_type_class_peek_parent(syncsynth_input_pad);

  pad = (AgsPadClass *) syncsynth_input_pad;

  pad->set_channel = ags_syncsynth_input_pad_set_channel;
  pad->resize_lines = ags_syncsynth_input_pad_resize_lines;
}

void
ags_syncsynth_input_pad_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_syncsynth_input_pad_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_syncsynth_input_pad_connect;
  connectable->disconnect = ags_syncsynth_input_pad_disconnect;
}

void
ags_syncsynth_input_pad_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_syncsynth_input_pad_get_name;
  plugin->set_name = ags_syncsynth_input_pad_set_name;
  plugin->get_xml_type = ags_syncsynth_input_pad_get_xml_type;
  plugin->set_xml_type = ags_syncsynth_input_pad_set_xml_type;
  plugin->read = ags_syncsynth_input_pad_read;
  plugin->write = ags_syncsynth_input_pad_write;
}

void
ags_syncsynth_input_pad_init(AgsSyncsynthInputPad *syncsynth_input_pad)
{
  syncsynth_input_pad->name = NULL;
  syncsynth_input_pad->xml_type = "ags-syncsynth-input-pad";
}

void
ags_syncsynth_input_pad_connect(AgsConnectable *connectable)
{
  AgsSyncsynthInputPad *syncsynth_input_pad;

  /* AgsSyncsynthInputPad */
  syncsynth_input_pad = AGS_SYNCSYNTH_INPUT_PAD(connectable);

  if((AGS_PAD_CONNECTED & (AGS_PAD(syncsynth_input_pad)->flags)) != 0){
    return;
  }

  ags_syncsynth_input_pad_parent_connectable_interface->connect(connectable);
}

void
ags_syncsynth_input_pad_disconnect(AgsConnectable *connectable)
{
  ags_syncsynth_input_pad_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_syncsynth_input_pad_destroy(GtkObject *object)
{
  /* empty */
}

gchar*
ags_syncsynth_input_pad_get_name(AgsPlugin *plugin)
{
  return(AGS_SYNCSYNTH_INPUT_PAD(plugin)->name);
}

void
ags_syncsynth_input_pad_set_name(AgsPlugin *plugin, gchar *name)
{
  AGS_SYNCSYNTH_INPUT_PAD(plugin)->name = name;
}

gchar*
ags_syncsynth_input_pad_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_SYNCSYNTH_INPUT_PAD(plugin)->xml_type);
}

void
ags_syncsynth_input_pad_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_SYNCSYNTH_INPUT_PAD(plugin)->xml_type = xml_type;
}

void
ags_syncsynth_input_pad_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsSyncsynthInputPad *gobject;
  AgsFileLookup *file_lookup;

  gobject = AGS_SYNCSYNTH_INPUT_PAD(plugin);

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
ags_syncsynth_input_pad_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsSyncsynthInputPad *syncsynth_input_pad;
  xmlNode *node;
  gchar *id;

  syncsynth_input_pad = AGS_SYNCSYNTH_INPUT_PAD(plugin);
  node = NULL;

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-syncsynth-input-pad");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", syncsynth_input_pad,
				   NULL));

  xmlAddChild(parent,
	      node);  

  return(node);
}

void
ags_syncsynth_input_pad_set_channel(AgsPad *pad, AgsChannel *channel)
{
  AGS_PAD_CLASS(ags_syncsynth_input_pad_parent_class)->set_channel(pad, channel);

  /* empty */
}

void
ags_syncsynth_input_pad_resize_lines(AgsPad *pad, GType line_type,
				     guint audio_channels, guint audio_channels_old)
{
  AGS_PAD_CLASS(ags_syncsynth_input_pad_parent_class)->resize_lines(pad, line_type,
								    audio_channels, audio_channels_old);

  /* empty */
}

/**
 * ags_syncsynth_input_pad_new:
 * @channel: the assigned channel
 *
 * Creates an #AgsSyncsynthInputPad
 *
 * Returns: a new #AgsSyncsynthInputPad
 *
 * Since: 0.9.7
 */
AgsSyncsynthInputPad*
ags_syncsynth_input_pad_new(AgsChannel *channel)
{
  AgsSyncsynthInputPad *syncsynth_input_pad;

  syncsynth_input_pad = (AgsSyncsynthInputPad *) g_object_new(AGS_TYPE_SYNCSYNTH_INPUT_PAD,
							      "channel", channel,
							      NULL);
  
  return(syncsynth_input_pad);
}

