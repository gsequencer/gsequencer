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

#include <ags/X/machine/ags_synth_input_line.h>

#include <ags/main.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/util/ags_id_generator.h>

#include <ags/plugin/ags_plugin_stock.h>

#include <ags/object/ags_plugin.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>

#include <ags/audio/ags_recall_factory.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_volume_channel.h>
#include <ags/audio/recall/ags_volume_channel_run.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_line.h>
#include <ags/X/ags_line_member.h>

#include <ags/X/machine/ags_synth.h>

void ags_synth_input_line_class_init(AgsSynthInputLineClass *synth_input_line);
void ags_synth_input_line_plugin_interface_init(AgsPluginInterface *plugin);
void ags_synth_input_line_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_synth_input_line_init(AgsSynthInputLine *synth_input_line);
void ags_synth_input_line_connect(AgsConnectable *connectable);
void ags_synth_input_line_disconnect(AgsConnectable *connectable);
gchar* ags_synth_input_line_get_name(AgsPlugin *plugin);
void ags_synth_input_line_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_synth_input_line_get_xml_type(AgsPlugin *plugin);
void ags_synth_input_line_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_synth_input_line_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
void ags_synth_input_line_resolve_line(AgsFileLookup *file_lookup,
				       AgsSynthInputLine *synth_input_line);
xmlNode* ags_synth_input_line_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_synth_input_line_set_channel(AgsLine *line, AgsChannel *channel);
void ags_synth_input_line_map_recall(AgsLine *line,
				     guint output_pad_start);

/**
 * SECTION:ags_synth_input_line
 * @short_description: synth input line
 * @title: AgsSynthInputLine
 * @section_id:
 * @include: ags/X/machine/ags_synth_input_line.h
 *
 * The #AgsSynthInputLine is a composite widget to act as synth input line.
 */

static gpointer ags_synth_input_line_parent_class = NULL;
static AgsConnectableInterface *ags_synth_input_line_parent_connectable_interface;

GType
ags_synth_input_line_get_type()
{
  static GType ags_type_synth_input_line = 0;

  if(!ags_type_synth_input_line){
    static const GTypeInfo ags_synth_input_line_info = {
      sizeof(AgsSynthInputLineClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_synth_input_line_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSynthInputLine),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_synth_input_line_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_synth_input_line_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_synth_input_line_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_synth_input_line = g_type_register_static(AGS_TYPE_LINE,
						       "AgsSynthInputLine\0", &ags_synth_input_line_info,
						       0);

    g_type_add_interface_static(ags_type_synth_input_line,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_synth_input_line,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_synth_input_line);
}

void
ags_synth_input_line_class_init(AgsSynthInputLineClass *synth_input_line)
{
  AgsLineClass *line;

  ags_synth_input_line_parent_class = g_type_class_peek_parent(synth_input_line);

  /* AgsLineClass */
  line = AGS_LINE_CLASS(synth_input_line);

  line->set_channel = ags_synth_input_line_set_channel;
  line->map_recall = ags_synth_input_line_map_recall;
}

void
ags_synth_input_line_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_synth_input_line_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_synth_input_line_connect;
  connectable->disconnect = ags_synth_input_line_disconnect;
}

void
ags_synth_input_line_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_synth_input_line_get_name;
  plugin->set_name = ags_synth_input_line_set_name;
  plugin->get_xml_type = ags_synth_input_line_get_xml_type;
  plugin->set_xml_type = ags_synth_input_line_set_xml_type;
  plugin->read = ags_synth_input_line_read;
  plugin->write = ags_synth_input_line_write;
}

void
ags_synth_input_line_init(AgsSynthInputLine *synth_input_line)
{
  AgsOscillator *oscillator;

  synth_input_line->name = NULL;
  synth_input_line->xml_type = "ags-synth-input-line\0";

  /* oscillator */
  oscillator = ags_oscillator_new();
  synth_input_line->oscillator = oscillator;
  ags_expander_add(AGS_LINE(synth_input_line)->expander,
		   GTK_WIDGET(oscillator),
		   0, 0,
		   1, 1);
}

void
ags_synth_input_line_connect(AgsConnectable *connectable)
{
  AgsSynthInputLine *synth_input_line;

  synth_input_line = AGS_SYNTH_INPUT_LINE(connectable);

  if((AGS_LINE_CONNECTED & (AGS_LINE(synth_input_line)->flags)) != 0){
    return;
  }

  ags_synth_input_line_parent_connectable_interface->connect(connectable);
}

void
ags_synth_input_line_disconnect(AgsConnectable *connectable)
{
  ags_synth_input_line_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_synth_input_line_set_channel(AgsLine *line, AgsChannel *channel)
{
  AgsSynthInputLine *synth_input_line;

  AGS_LINE_CLASS(ags_synth_input_line_parent_class)->set_channel(line, channel);

  synth_input_line = AGS_SYNTH_INPUT_LINE(line);

  /* empty */
}

void
ags_synth_input_line_map_recall(AgsLine *line,
				guint output_pad_start)
{
  AgsSynth *synth;
  AgsSynthInputLine *synth_input_line;
  AgsAudio *audio;
  AgsChannel *source;
  guint i;

  if((AGS_LINE_MAPPED_RECALL & (line->flags)) != 0){
    return;
  }

  synth_input_line = AGS_SYNTH_INPUT_LINE(line);

  audio = AGS_AUDIO(line->channel->audio);

  synth = AGS_SYNTH(audio->machine);

  source = line->channel;

  /* empty */
  /* call parent */
  AGS_LINE_CLASS(ags_synth_input_line_parent_class)->map_recall(line,
								output_pad_start);
}

gchar*
ags_synth_input_line_get_name(AgsPlugin *plugin)
{
  return(AGS_SYNTH_INPUT_LINE(plugin)->name);
}

void
ags_synth_input_line_set_name(AgsPlugin *plugin, gchar *name)
{
  AGS_SYNTH_INPUT_LINE(plugin)->name = name;
}

gchar*
ags_synth_input_line_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_SYNTH_INPUT_LINE(plugin)->xml_type);
}

void
ags_synth_input_line_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_SYNTH_INPUT_LINE(plugin)->xml_type = xml_type;
}

void
ags_synth_input_line_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsSynthInputLine *gobject;
  AgsFileLookup *file_lookup;
  xmlNode *child;

  gobject = AGS_SYNTH_INPUT_LINE(plugin);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  gobject->oscillator = ags_oscillator_new();

  file_lookup = (AgsFileLookup *) g_object_new(AGS_TYPE_FILE_LOOKUP,
					       "file\0", file,
					       "node\0", node,
					       "reference\0", gobject,
					       NULL);
  ags_file_add_lookup(file, (GObject *) file_lookup);
  g_signal_connect(G_OBJECT(file_lookup), "resolve\0",
		   G_CALLBACK(ags_synth_input_line_resolve_line), gobject);

  /* child elements */
  child = node->children;

  while(child != NULL){
    if(XML_ELEMENT_NODE == child->type){
      if(!xmlStrncmp(child->name,
		     "ags-oscillator\0",
		     15)){
	ags_file_read_oscillator(file, child, &(gobject->oscillator));
      }
    }

    child = child->next;
  }
}

void
ags_synth_input_line_resolve_line(AgsFileLookup *file_lookup,
				  AgsSynthInputLine *synth_input_line)
{
  ags_expander_add(AGS_LINE(synth_input_line)->expander,
		   GTK_WIDGET(synth_input_line->oscillator),
		   0, 0,
		   1, 1);
}

xmlNode*
ags_synth_input_line_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsSynthInputLine *synth_input_line;
  xmlNode *node;
  gchar *id;

  synth_input_line = AGS_SYNTH_INPUT_LINE(plugin);

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-synth-input-line\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", synth_input_line,
				   NULL));

  ags_file_write_oscillator(file, node, synth_input_line->oscillator);

  xmlAddChild(parent,
	      node);
}

/**
 * ags_synth_input_line_new:
 * @channel: the assigned channel
 *
 * Creates an #AgsSynthInputLine
 *
 * Returns: a new #AgsSynthInputLine
 *
 * Since: 0.4
 */
AgsSynthInputLine*
ags_synth_input_line_new(AgsChannel *channel)
{
  AgsSynthInputLine *synth_input_line;

  synth_input_line = (AgsSynthInputLine *) g_object_new(AGS_TYPE_SYNTH_INPUT_LINE,
							"channel\0", channel,
							NULL);

  return(synth_input_line);
}

