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

#include <ags/X/machine/ags_audiorec.h>
#include <ags/X/machine/ags_audiorec_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_window.h>

void ags_audiorec_class_init(AgsAudiorecClass *audiorec);
void ags_audiorec_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audiorec_plugin_interface_init(AgsPluginInterface *plugin);
void ags_audiorec_init(AgsAudiorec *audiorec);
void ags_audiorec_connect(AgsConnectable *connectable);
void ags_audiorec_disconnect(AgsConnectable *connectable);
void ags_audiorec_finalize(GObject *gobject);
void ags_audiorec_show(GtkWidget *widget);
gchar* ags_audiorec_get_name(AgsPlugin *plugin);
void ags_audiorec_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_audiorec_get_xml_type(AgsPlugin *plugin);
void ags_audiorec_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_audiorec_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_audiorec_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_audiorec_resize_audio_channels(AgsMachine *machine,
					guint audio_channels, guint audio_channels_old,
					gpointer data);
void ags_audiorec_resize_pads(AgsMachine *machine, GType type,
			      guint pads, guint pads_old,
			      gpointer data);

/**
 * SECTION:ags_audiorec
 * @short_description: record audio data
 * @title: AgsAudiorec
 * @section_id:
 * @include: ags/X/machine/ags_audiorec.h
 *
 * The #AgsAudiorec is a composite widget to act as capture source.
 */

static gpointer ags_audiorec_parent_class = NULL;
static AgsConnectableInterface *ags_audiorec_parent_connectable_interface;

GType
ags_audiorec_get_type(void)
{
  static GType ags_type_audiorec = 0;

  if(!ags_type_audiorec){
    static const GTypeInfo ags_audiorec_info = {
      sizeof(AgsAudiorecClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audiorec_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsAudiorec),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audiorec_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audiorec_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_audiorec_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audiorec = g_type_register_static(AGS_TYPE_MACHINE,
					       "AgsAudiorec", &ags_audiorec_info,
					       0);
    
    g_type_add_interface_static(ags_type_audiorec,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_audiorec,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_audiorec);
}

void
ags_audiorec_class_init(AgsAudiorecClass *audiorec)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  AgsMachineClass *machine;

  ags_audiorec_parent_class = g_type_class_peek_parent(audiorec);

  /* GObjectClass */
  gobject = (GObjectClass *) audiorec;

  gobject->finalize = ags_audiorec_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) audiorec;

  widget->show = ags_audiorec_show;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) audiorec;
}

void
ags_audiorec_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_audiorec_connectable_parent_interface;

  ags_audiorec_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_audiorec_connect;
}

void
ags_audiorec_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_audiorec_get_name;
  plugin->set_name = ags_audiorec_set_name;
  plugin->get_xml_type = ags_audiorec_get_xml_type;
  plugin->set_xml_type = ags_audiorec_set_xml_type;
  plugin->read = ags_audiorec_read;
  plugin->write = ags_audiorec_write;
}

void
ags_audiorec_init(AgsAudiorec *audiorec)
{
  GtkHBox *hbox;
  GtkHBox *vbox;
  GtkHBox *filename_hbox;
  GtkHBox *radio_hbox;
  GtkFrame *frame;
  GtkLabel *label;

  AgsAudio *audio;

  audio = AGS_MACHINE(audiorec)->audio;
  audio->flags |= (AGS_AUDIO_SYNC |
		   AGS_AUDIO_OUTPUT_HAS_RECYCLING |
		   AGS_AUDIO_INPUT_HAS_RECYCLING);

  /* audio resize */
  g_signal_connect_after(G_OBJECT(audiorec), "resize-audio-channels",
			 G_CALLBACK(ags_audiorec_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(audiorec), "resize-pads",
			 G_CALLBACK(ags_audiorec_resize_pads), NULL);
  
  /* mapped IO */
  audiorec->mapped_input_pad = 0;
  audiorec->mapped_output_pad = 0;

  /* name and xml type */
  audiorec->name = NULL;
  audiorec->xml_type = "ags-audiorec";

  /* context menu */
  ags_machine_popup_add_connection_options((AgsMachine *) audiorec,
					   (AGS_MACHINE_POPUP_CONNECTION_EDITOR));

  AGS_MACHINE(audiorec)->connection_flags |= AGS_MACHINE_SHOW_AUDIO_INPUT_CONNECTION;
  
  /* hbox */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_container_add((GtkContainer *) (gtk_bin_get_child((GtkBin *) audiorec)),
		    (GtkWidget *) hbox);

  /* frame - filename and open */
  frame = (GtkFrame *) gtk_frame_new("file");
  gtk_box_pack_start(hbox,
		     frame,
		     FALSE, FALSE,
		     0);

  vbox = (GtkHBox *) gtk_vbox_new(FALSE,
				  0);
  gtk_container_add((GtkContainer *) frame,
		    (GtkWidget *) vbox);

  /* filename */
  filename_hbox = (GtkHBox *) gtk_hbox_new(FALSE,
					   0);
  gtk_box_pack_start(vbox,
		     filename_hbox,
		     FALSE, FALSE,
		     0);
  
  label = gtk_label_new("filename: ");
  gtk_box_pack_start(filename_hbox,
		     label,
		     FALSE, FALSE,
		     0);

  audiorec->filename = (GtkEntry *) gtk_entry_new();
  gtk_box_pack_start(filename_hbox,
		     audiorec->filename,
		     FALSE, FALSE,
		     0);

  audiorec->open = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_OPEN);
  gtk_box_pack_start(filename_hbox,
		     audiorec->open,
		     FALSE, FALSE,
		     0);
  
  /* radio */
  radio_hbox = (GtkHBox *) gtk_hbox_new(FALSE,
					0);
  gtk_box_pack_start(vbox,
		     radio_hbox,
		     FALSE, FALSE,
		     0);

  audiorec->keep_data = gtk_radio_button_new_with_label_from_widget(NULL,
								    "keep");
  gtk_box_pack_start(radio_hbox,
		     audiorec->keep_data,
		     FALSE, FALSE,
		     0);

  audiorec->replace_data = gtk_radio_button_new_with_label_from_widget(audiorec->keep_data,
								       "replace");
  gtk_box_pack_start(radio_hbox,
		     audiorec->replace_data,
		     FALSE, FALSE,
		     0);
  
  audiorec->mix_data = gtk_radio_button_new_with_label_from_widget(audiorec->keep_data,
								   "mix");
  gtk_box_pack_start(radio_hbox,
		     audiorec->mix_data,
		     FALSE, FALSE,
		     0);
  
  /* frame - hindicator */
  frame = (GtkFrame *) gtk_frame_new("input");
  gtk_box_pack_start(hbox,
		     frame,
		     FALSE, FALSE,
		     0);

  audiorec->hindicator_vbox = (GtkHBox *) gtk_vbox_new(FALSE,
						       0);
  gtk_container_add((GtkContainer *) frame,
		    (GtkWidget *) audiorec->hindicator_vbox);

  /* dialog */
  audiorec->open_dialog = NULL;
}

void
ags_audiorec_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_audiorec_parent_class)->finalize(gobject);
}

void
ags_audiorec_connect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsAudiorec *audiorec;

  GList *list;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_audiorec_parent_connectable_interface->connect(connectable);

  audiorec = AGS_AUDIOREC(connectable);

  /* filename */
  g_signal_connect(audiorec->open, "clicked",
		   G_CALLBACK(ags_audiorec_open_callback), audiorec);

  /* mode */
  g_signal_connect(audiorec->keep_data, "clicked",
		   G_CALLBACK(ags_audiorec_keep_data_callback), audiorec);

  g_signal_connect(audiorec->mix_data, "clicked",
		   G_CALLBACK(ags_audiorec_mix_data_callback), audiorec);

  g_signal_connect(audiorec->replace_data, "clicked",
		   G_CALLBACK(ags_audiorec_replace_data_callback), audiorec);
}

void
ags_audiorec_disconnect(AgsConnectable *connectable)
{
  AgsAudiorec *audiorec;

  GList *list;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  ags_audiorec_parent_connectable_interface->disconnect(connectable);

  audiorec = AGS_AUDIOREC(connectable);

  /* filename */
  g_object_disconnect(audiorec->open,
		      "any_signal::clicked",
		      G_CALLBACK(ags_audiorec_open_callback),
		      audiorec,
		      NULL);

  /* mode */
  g_object_disconnect(audiorec->keep_data,
		      "any_signal::clicked",
		      G_CALLBACK(ags_audiorec_keep_data_callback),
		      audiorec,
		      NULL);

  g_object_disconnect(audiorec->replace_data,
		      "any_signal::clicked",
		      G_CALLBACK(ags_audiorec_replace_data_callback),
		      audiorec,
		      NULL);

  g_object_disconnect(audiorec->mix_data,
		      "any_signal::clicked",
		      G_CALLBACK(ags_audiorec_mix_data_callback),
		      audiorec,
		      NULL);
}

void
ags_audiorec_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_audiorec_parent_class)->show(widget);
}

gchar*
ags_audiorec_get_name(AgsPlugin *plugin)
{
  return(AGS_AUDIOREC(plugin)->name);
}

void
ags_audiorec_set_name(AgsPlugin *plugin, gchar *name)
{
  AGS_AUDIOREC(plugin)->name = name;
}

gchar*
ags_audiorec_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_AUDIOREC(plugin)->xml_type);
}

void
ags_audiorec_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_AUDIOREC(plugin)->xml_type = xml_type;
}

void
ags_audiorec_resize_audio_channels(AgsMachine *machine,
				   guint audio_channels, guint audio_channels_old,
				   gpointer data)
{
  AgsAudiorec *audiorec;

  guint pads;
  
  audiorec = AGS_AUDIOREC(machine);

  pads = machine->audio->input_pads;

  if(audio_channels > audio_channels_old &&
     pads > 0){
    AgsHIndicator *hindicator;
	
    guint i;

    for(i = audio_channels_old; i < audio_channels; i++){
      hindicator = ags_hindicator_new();
      gtk_box_pack_start(audiorec->hindicator_vbox,
			 hindicator,
			 FALSE, FALSE,
			 8);
    }

    gtk_widget_show_all(audiorec->hindicator_vbox);
  }else{
    GList *list, *list_start;

    list_start =
      list = gtk_container_get_children(audiorec->hindicator_vbox);

    list = g_list_nth(list_start,
		      audio_channels);
    
    while(list != NULL){
      gtk_widget_destroy(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);
  }
}

void
ags_audiorec_resize_pads(AgsMachine *machine, GType type,
			 guint pads, guint pads_old,
			 gpointer data)
{
  AgsAudiorec *audiorec;

  audiorec = AGS_AUDIOREC(machine);

  if(type == AGS_TYPE_INPUT){
    if(pads == 1 &&
       pads_old == 0){
      AgsHIndicator *hindicator;

      guint audio_channels;
      guint i;

      audio_channels = machine->audio->audio_channels;

      for(i = 0; i < audio_channels; i++){
	hindicator = ags_hindicator_new();
	gtk_box_pack_start(audiorec->hindicator_vbox,
			   hindicator,
			   FALSE, FALSE,
			   8);
      }

      gtk_widget_show_all(audiorec->hindicator_vbox);
    }else if(pads == 0){
      GList *list, *list_start;

      list_start = 
	list = gtk_container_get_children(audiorec->hindicator_vbox);

      while(list != NULL){
	gtk_widget_destroy(list->data);
      
	list = list->next;
      }

      g_list_free(list_start);
    }
  }
}

void
ags_audiorec_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsAudiorec *gobject;
  
  gobject = AGS_AUDIOREC(plugin);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference", gobject,
				   NULL));

  //TODO:JK: implement me
}

xmlNode*
ags_audiorec_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsAudiorec *audiorec;

  xmlNode *node;

  gchar *id;

  audiorec = AGS_AUDIOREC(plugin);

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-audiorec");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", audiorec,
				   NULL));

  //TODO:JK: implement me
  
  xmlAddChild(parent,
	      node);

  return(node);
}

void
ags_audiorec_open_filename(AgsAudiorec *audiorec,
			   gchar *filename)
{
  //TODO:JK: implement me
}

/**
 * ags_audiorec_new:
 * @soundcard: the assigned soundcard.
 *
 * Creates an #AgsAudiorec
 *
 * Returns: a new #AgsAudiorec
 *
 * Since: 1.2.0
 */
AgsAudiorec*
ags_audiorec_new(GObject *soundcard)
{
  AgsAudiorec *audiorec;
  GValue value = {0,};

  audiorec = (AgsAudiorec *) g_object_new(AGS_TYPE_AUDIOREC,
					  NULL);

  if(soundcard != NULL){
    g_value_init(&value, G_TYPE_OBJECT);
    g_value_set_object(&value, soundcard);
    g_object_set_property(G_OBJECT(audiorec->machine.audio),
			  "soundcard", &value);
    g_value_unset(&value);
  }

  return(audiorec);
}
