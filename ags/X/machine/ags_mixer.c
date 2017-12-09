/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/machine/ags_mixer.h>
#include <ags/X/machine/ags_mixer_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>

#include <ags/X/file/ags_gui_file_xml.h>

#include <ags/X/machine/ags_mixer_input_pad.h>
#include <ags/X/machine/ags_mixer_input_line.h>

void ags_mixer_class_init(AgsMixerClass *mixer);
void ags_mixer_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_mixer_plugin_interface_init(AgsPluginInterface *plugin);
void ags_mixer_init(AgsMixer *mixer);
void ags_mixer_finalize(GObject *gobject);
void ags_mixer_connect(AgsConnectable *connectable);
void ags_mixer_disconnect(AgsConnectable *connectable);
void ags_mixer_map_recall(AgsMachine *machine);
gchar* ags_mixer_get_name(AgsPlugin *plugin);
void ags_mixer_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_mixer_get_xml_type(AgsPlugin *plugin);
void ags_mixer_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_mixer_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_mixer_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

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
					    "AgsMixer", &ags_mixer_info,
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
  AgsMachineClass *machine;

  ags_mixer_parent_class = g_type_class_peek_parent(mixer);

  /* GObjectClass */
  gobject = (GObjectClass *) mixer;

  gobject->finalize = ags_mixer_finalize;

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
  g_signal_connect_after((GObject *) mixer, "parent_set",
			 G_CALLBACK(ags_mixer_parent_set_callback), (gpointer) mixer);

  AGS_MACHINE(mixer)->audio->flags |= (AGS_AUDIO_ASYNC);

  AGS_MACHINE(mixer)->input_pad_type = AGS_TYPE_MIXER_INPUT_PAD;
  AGS_MACHINE(mixer)->input_line_type = AGS_TYPE_MIXER_INPUT_LINE;
  AGS_MACHINE(mixer)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(mixer)->output_line_type = G_TYPE_NONE;
  
  /*  */
  mixer->name = NULL;
  mixer->xml_type = "ags-mixer";

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
  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_mixer_parent_connectable_interface->connect(connectable);
}

void
ags_mixer_disconnect(AgsConnectable *connectable)
{
  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  ags_mixer_parent_connectable_interface->disconnect(connectable);
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
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference", gobject,
				   NULL));
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
		    "ags-mixer");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", mixer,
				   NULL));

  xmlAddChild(parent,
	      node);

  return(node);
}

/**
 * ags_mixer_new:
 * @soundcard: the assigned soundcard.
 *
 * Creates an #AgsMixer
 *
 * Returns: a new #AgsMixer
 *
 * Since: 1.0.0
 */
AgsMixer*
ags_mixer_new(GObject *soundcard)
{
  AgsMixer *mixer;

  mixer = (AgsMixer *) g_object_new(AGS_TYPE_MIXER,
				    NULL);

  g_object_set(G_OBJECT(AGS_MACHINE(mixer)->audio),
	       "soundcard", soundcard,
	       NULL);

  return(mixer);
}
