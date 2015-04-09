/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/X/machine/ags_mixer.h>
#include <ags/X/machine/ags_mixer_callbacks.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>
#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_plugin.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>
#include <ags/file/ags_file_gui.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_recall_factory.h>
#include <ags/audio/ags_recall.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>

#include <ags/X/machine/ags_mixer_input_pad.h>
#include <ags/X/machine/ags_mixer_input_line.h>

void ags_mixer_class_init(AgsMixerClass *mixer);
void ags_mixer_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_mixer_plugin_interface_init(AgsPluginInterface *plugin);
void ags_mixer_init(AgsMixer *mixer);
void ags_mixer_finalize(GObject *gobject);
void ags_mixer_connect(AgsConnectable *connectable);
void ags_mixer_disconnect(AgsConnectable *connectable);
void ags_mixer_show(GtkWidget *widget);
void ags_mixer_map_recall(AgsMachine *machine);
gchar* ags_mixer_get_name(AgsPlugin *plugin);
void ags_mixer_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_mixer_get_xml_type(AgsPlugin *plugin);
void ags_mixer_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_mixer_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_mixer_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);
void ags_mixer_read_resolve_audio(AgsFileLookup *file_lookup,
				  AgsMachine *machine);

void ags_mixer_set_audio_channels(AgsAudio *audio,
				  guint audio_channels, guint audio_channels_old,
				  gpointer data);
void ags_mixer_set_pads(AgsAudio *audio, GType type,
			guint pads, guint pads_old,
			gpointer data);

/**
 * SECTION:ags_mixer
 * @short_description: mixer
 * @title: AgsMixer
 * @section_id:
 * @include: ags/X/machine/ags_mixer.h
 *
 * The #AgsMixer is a composite widget to act as mixer.
 */

static gpointer ags_mixer_parent_class = NULL;

static AgsConnectableInterface *ags_mixer_parent_connectable_interface;

extern const char *AGS_MIX_VOLUME;

GType
ags_mixer_get_type(void)
{
  static GType ags_type_mixer = 0;

  if(!ags_type_mixer){
    static const GTypeInfo ags_mixer_info = {
      sizeof(AgsMixerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_mixer_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsMixer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_mixer_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_mixer_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_mixer_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_mixer = g_type_register_static(AGS_TYPE_MACHINE,
					    "AgsMixer\0", &ags_mixer_info,
					    0);
    
    g_type_add_interface_static(ags_type_mixer,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_mixer,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_mixer);
}

void
ags_mixer_class_init(AgsMixerClass *mixer)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  AgsMachineClass *machine;

  ags_mixer_parent_class = g_type_class_peek_parent(mixer);

  /* GObjectClass */
  gobject = (GObjectClass *) mixer;

  gobject->finalize = ags_mixer_finalize;

  /* GtkWidget */
  widget = (GtkWidgetClass *) mixer;
  //  widget->show = ags_mixer_show;

  /* AgsMachine */
  machine = (AgsMachineClass *) mixer;

  machine->map_recall = ags_mixer_map_recall;
}

void
ags_mixer_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_mixer_connectable_parent_interface;

  ags_mixer_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_mixer_connect;
  connectable->disconnect = ags_mixer_disconnect;
}

void
ags_mixer_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_mixer_get_name;
  plugin->set_name = ags_mixer_set_name;
  plugin->get_xml_type = ags_mixer_get_xml_type;
  plugin->set_xml_type = ags_mixer_set_xml_type;
  plugin->read = ags_mixer_read;
  plugin->write = ags_mixer_write;
}

void
ags_mixer_init(AgsMixer *mixer)
{
  g_signal_connect_after((GObject *) mixer, "parent_set\0",
			 G_CALLBACK(ags_mixer_parent_set_callback), (gpointer) mixer);

  AGS_MACHINE(mixer)->audio->flags |= (AGS_AUDIO_ASYNC);

  AGS_MACHINE(mixer)->input_pad_type = AGS_TYPE_MIXER_INPUT_PAD;
  AGS_MACHINE(mixer)->input_line_type = AGS_TYPE_MIXER_INPUT_LINE;
  AGS_MACHINE(mixer)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(mixer)->output_line_type = G_TYPE_NONE;
  
  /* AgsAudio */
  g_signal_connect_after(G_OBJECT(mixer->machine.audio), "set_audio_channels\0",
			 G_CALLBACK(ags_mixer_set_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(mixer->machine.audio), "set_pads\0",
			 G_CALLBACK(ags_mixer_set_pads), NULL);

  /*  */
  mixer->name = NULL;
  mixer->xml_type = "ags-mixer\0";

  mixer->input_pad = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  AGS_MACHINE(mixer)->input = (GtkContainer *) mixer->input_pad;
  gtk_container_add((GtkContainer*) (gtk_container_get_children((GtkContainer *) mixer))->data, (GtkWidget *) mixer->input_pad);
}

void
ags_mixer_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_mixer_parent_class)->finalize(gobject);
}

void
ags_mixer_connect(AgsConnectable *connectable)
{
  AgsMixer *mixer;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_mixer_parent_connectable_interface->connect(connectable);

  /* AgsMixer */
  mixer = AGS_MIXER(connectable);

  //  g_signal_connect((GObject *) mixer, "destroy\0",
  //		   G_CALLBACK(ags_mixer_destroy_callback), (gpointer) mixer);
}

void
ags_mixer_disconnect(AgsConnectable *connectable)
{
  AgsMixer *mixer;

  ags_mixer_parent_connectable_interface->disconnect(connectable);

  /* AgsMixer */
  mixer = AGS_MIXER(connectable);

  //TODO:JK: implement me
}

void
ags_mixer_show(GtkWidget *widget)
{
}

void
ags_mixer_map_recall(AgsMachine *machine)
{
  AGS_MACHINE_CLASS(ags_mixer_parent_class)->map_recall(machine);
  
  /* empty */
}

gchar*
ags_mixer_get_name(AgsPlugin *plugin)
{
  return(AGS_MIXER(plugin)->name);
}

void
ags_mixer_set_name(AgsPlugin *plugin, gchar *name)
{
  AGS_MIXER(plugin)->name = name;
}

gchar*
ags_mixer_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_MIXER(plugin)->xml_type);
}

void
ags_mixer_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_MIXER(plugin)->xml_type = xml_type;
}

void
ags_mixer_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsMixer *gobject;
  AgsFileLookup *file_lookup;
  GList *list;
  guint64 index;

  gobject = AGS_MIXER(plugin);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  list = file->lookup;

  while((file_lookup = ags_file_lookup_find_by_node(list,
						    node->parent)) != NULL){
    if(g_signal_handler_find(list->data,
			     G_SIGNAL_MATCH_FUNC,
			     0,
			     0,
			     NULL,
			     ags_file_read_machine_resolve_audio,
			     NULL) != 0){
      g_signal_connect_after(G_OBJECT(file_lookup), "resolve\0",
			     G_CALLBACK(ags_mixer_read_resolve_audio), gobject);
      
      break;
    }

    list = list->next;
  }
}

void
ags_mixer_read_resolve_audio(AgsFileLookup *file_lookup,
			     AgsMachine *machine)
{
  AgsMixer *mixer;
  GList *pad, *pad_start, *line, *line_start;

  mixer = AGS_MIXER(machine);

  g_signal_connect_after(G_OBJECT(machine->audio), "set_audio_channels\0",
			 G_CALLBACK(ags_mixer_set_audio_channels), mixer);

  g_signal_connect_after(G_OBJECT(machine->audio), "set_pads\0",
			 G_CALLBACK(ags_mixer_set_pads), mixer);
}

xmlNode*
ags_mixer_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsMixer *mixer;
  xmlNode *node;
  gchar *id;
  guint i;

  mixer = AGS_MIXER(plugin);

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-mixer\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", mixer,
				   NULL));

  xmlAddChild(parent,
	      node);

  return(node);
}

void
ags_mixer_set_audio_channels(AgsAudio *audio,
			     guint audio_channels, guint audio_channels_old,
			     gpointer data)
{
  /* empty */
}

void
ags_mixer_set_pads(AgsAudio *audio, GType type,
		   guint pads, guint pads_old,
		   gpointer data)
{
  /* empty */
}

/**
 * ags_mixer_new:
 * @soundcard: the assigned soundcard.
 *
 * Creates an #AgsMixer
 *
 * Returns: a new #AgsMixer
 *
 * Since: 0.3
 */
AgsMixer*
ags_mixer_new(GObject *soundcard)
{
  AgsMixer *mixer;

  mixer = (AgsMixer *) g_object_new(AGS_TYPE_MIXER,
				    NULL);

  g_object_set(G_OBJECT(AGS_MACHINE(mixer)->audio),
	       "soundcard\0", soundcard,
	       NULL);

  return(mixer);
}
