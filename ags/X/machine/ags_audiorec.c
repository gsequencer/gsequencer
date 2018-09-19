/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>

#include <ags/X/thread/ags_gui_thread.h>

void ags_audiorec_class_init(AgsAudiorecClass *audiorec);
void ags_audiorec_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audiorec_plugin_interface_init(AgsPluginInterface *plugin);
void ags_audiorec_init(AgsAudiorec *audiorec);
void ags_audiorec_finalize(GObject *gobject);

void ags_audiorec_connect(AgsConnectable *connectable);
void ags_audiorec_disconnect(AgsConnectable *connectable);

gchar* ags_audiorec_get_name(AgsPlugin *plugin);
void ags_audiorec_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_audiorec_get_xml_type(AgsPlugin *plugin);
void ags_audiorec_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_audiorec_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_audiorec_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_audiorec_show(GtkWidget *widget);

void ags_audiorec_map_recall(AgsMachine *machine);

void ags_audiorec_resize_audio_channels(AgsMachine *machine,
					guint audio_channels, guint audio_channels_old,
					gpointer data);
void ags_audiorec_resize_pads(AgsMachine *machine, GType type,
			      guint pads, guint pads_old,
			      gpointer data);

void ags_audiorec_output_map_recall(AgsAudiorec *audiorec, guint output_pad_start);
void ags_audiorec_input_map_recall(AgsAudiorec *audiorec, guint input_pad_start);

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

GHashTable *ags_audiorec_indicator_queue_draw = NULL;

GType
ags_audiorec_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_audiorec = 0;

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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_audiorec);
  }

  return g_define_type_id__volatile;
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

  machine->map_recall = ags_audiorec_map_recall;
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

  g_signal_connect_after((GObject *) audiorec, "parent_set",
			 G_CALLBACK(ags_audiorec_parent_set_callback), (gpointer) audiorec);

  audio = AGS_MACHINE(audiorec)->audio;
  ags_audio_set_flags(audio, (AGS_AUDIO_SYNC |
			      AGS_AUDIO_OUTPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_RECYCLING));
  ags_audio_set_ability_flags(audio, (AGS_SOUND_ABILITY_WAVE));

  AGS_MACHINE(audiorec)->flags |= (AGS_MACHINE_IS_WAVE_PLAYER);

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

  /* indicator */
  if(ags_audiorec_indicator_queue_draw == NULL){
    ags_audiorec_indicator_queue_draw = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							      NULL,
							      NULL);
  }

  g_hash_table_insert(ags_audiorec_indicator_queue_draw,
		      audiorec, ags_audiorec_indicator_queue_draw_timeout);
  g_timeout_add(1000 / 30, (GSourceFunc) ags_audiorec_indicator_queue_draw_timeout, (gpointer) audiorec);
}

void
ags_audiorec_finalize(GObject *gobject)
{
  AgsAudiorec *audiorec;

  audiorec = gobject;
  
  g_hash_table_remove(ags_audiorec_indicator_queue_draw,
		      audiorec);
  
  /* call parent */
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
  g_signal_connect_after(audiorec->keep_data, "clicked",
			 G_CALLBACK(ags_audiorec_keep_data_callback), audiorec);

  g_signal_connect_after(audiorec->mix_data, "clicked",
			 G_CALLBACK(ags_audiorec_mix_data_callback), audiorec);

  g_signal_connect_after(audiorec->replace_data, "clicked",
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
ags_audiorec_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_audiorec_parent_class)->show(widget);
}

void
ags_audiorec_map_recall(AgsMachine *machine)
{
  AgsAudiorec *audiorec;

  AgsAudio *audio;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  audiorec = AGS_AUDIOREC(machine);

  audio = machine->audio;

  /* ags-play-wave */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-play-wave",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY),
			    0);

  /* ags-capture-wave */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-capture-wave",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY),
			    0);
  
  /* depending on destination */
  ags_audiorec_input_map_recall(audiorec, 0);

  /* depending on destination */
  ags_audiorec_output_map_recall(audiorec, 0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_audiorec_parent_class)->map_recall(machine);  
}

void
ags_audiorec_output_map_recall(AgsAudiorec *audiorec, guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;
  guint audio_channels;
  
  if(audiorec->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(audiorec)->audio;

  g_object_get(audio,
	       "output-pads", &output_pads,
	       "audio-channels", &audio_channels,
	       NULL);

  /* ags-peak */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-peak",
			    0, audio_channels, 
			    output_pad_start, output_pads,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  /* ags-play-wave */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-play-wave",
			    0, audio_channels,
			    output_pad_start, output_pads,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_ADD),
			    0);

  /* ags-capture-wave */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-capture-wave",
			    0, audio_channels,
			    output_pad_start, output_pads,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_ADD),
			    0);
  
  audiorec->mapped_output_pad = output_pads;
}

void
ags_audiorec_input_map_recall(AgsAudiorec *audiorec, guint input_pad_start)
{
  AgsAudio *audio;
  AgsChannel *source, *current;

  GList *list;

  guint input_pads;
  guint audio_channels;

  if(audiorec->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(audiorec)->audio;

  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);
}

void
ags_audiorec_resize_audio_channels(AgsMachine *machine,
				   guint audio_channels, guint audio_channels_old,
				   gpointer data)
{
  AgsAudiorec *audiorec;

  AgsAudio *audio;
  AgsChannel *output;
  AgsChannel *channel, *next_pad;
  AgsRecycling *first_recycling;
  AgsAudioSignal *audio_signal;  

  GObject *output_soundcard;
  
  guint output_pads, input_pads;
  
  audiorec = AGS_AUDIOREC(machine);

  audio = AGS_MACHINE(audiorec)->audio;

  g_object_get(audio,
	       "input-pads", &input_pads,
	       "output-pads", &output_pads,
	       "output", &output,
	       NULL);

  if(audio_channels > audio_channels_old){
    AgsHIndicator *hindicator;
	
    guint i;

    /* AgsOutput */
    channel = output;

    while(channel != NULL){
      /* get some fields */
      g_object_get(channel,
		   "next-pad", &next_pad,
		   NULL);

      channel = ags_channel_pad_nth(channel,
				    audio_channels_old);

      while(channel != next_pad){
	ags_channel_set_ability_flags(channel, (AGS_SOUND_ABILITY_WAVE));
	
	/* get some fields */
	g_object_get(channel,
		     "output-soundcard", &output_soundcard,
		     "first-recycling", &first_recycling,
		     NULL);

	/* audio signal */
	audio_signal = ags_audio_signal_new(output_soundcard,
					    (GObject *) first_recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	ags_audio_signal_stream_resize(audio_signal,
				       3);
	ags_recycling_add_audio_signal(first_recycling,
				       audio_signal);
	
	/* iterate */
	g_object_get(channel,
		     "next", &channel,
		     NULL);
      }
    }

    if(input_pads > 0){      
    }
    
    if(output_pads > 0){
      /* AgsOutput */
      /* ags-play-wave */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-play-wave",
				audio_channels_old, audio_channels, 
				0, output_pads,
				(AGS_RECALL_FACTORY_OUTPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_ADD),
				0);

      /* ags-capture-wave */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-capture-wave",
				audio_channels_old, audio_channels, 
				0, output_pads,
				(AGS_RECALL_FACTORY_OUTPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_ADD),
				0);
      
      /* ags-peak */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-peak",
				audio_channels, audio_channels_old, 
				0, output_pads,
				(AGS_RECALL_FACTORY_OUTPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_ADD),
				0);
    }
    
    /* widgets */
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

  AgsAudio *audio;
  AgsChannel *output;
  AgsChannel *channel;
  AgsRecycling *first_recycling;
  AgsAudioSignal *audio_signal;

  GObject *output_soundcard;
  
  guint audio_channels;

  audiorec = AGS_AUDIOREC(machine);

  audio = machine->audio;

  if(type == AGS_TYPE_INPUT){
    if(pads > pads_old){
      AgsHIndicator *hindicator;

      guint audio_channels;
      guint i;

      /* get some fields */
      g_object_get(audio,
		   "output", &output,
		   "audio-channels", &audio_channels,
		   NULL);

      /* AgsOutput */
      channel = ags_channel_pad_nth(output,
				    pads_old);

      while(channel != NULL){
	ags_channel_set_ability_flags(channel, (AGS_SOUND_ABILITY_WAVE));

	/* get some fields */
	g_object_get(channel,
		     "output-soundcard", &output_soundcard,
		     "first-recycling", &first_recycling,
		     NULL);

	/* audio signal */
	audio_signal = ags_audio_signal_new(output_soundcard,
					    (GObject *) first_recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	ags_audio_signal_stream_resize(audio_signal,
				       3);
	ags_recycling_add_audio_signal(first_recycling,
				       audio_signal);
	
	/* iterate */
	g_object_get(channel,
		     "next", &channel,
		     NULL);
      }

      for(i = 0; i < audio_channels; i++){
	hindicator = ags_hindicator_new();
	gtk_box_pack_start(audiorec->hindicator_vbox,
			   hindicator,
			   FALSE, FALSE,
			   8);
      }

      gtk_widget_show_all(audiorec->hindicator_vbox);

      /* depending on destination */
      ags_audiorec_input_map_recall(audiorec,
				    pads_old);
    }else{
      GList *list, *list_start;

      list_start = 
	list = gtk_container_get_children(audiorec->hindicator_vbox);

      while(list != NULL){
	gtk_widget_destroy(list->data);
      
	list = list->next;
      }

      g_list_free(list_start);

      audiorec->mapped_input_pad = pads;
    }
  }else{
    if(pads > pads_old){
      /* depending on destination */
      ags_audiorec_output_map_recall(audiorec,
				     pads_old);
    }else{
      audiorec->mapped_output_pad = pads;
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
  AgsWaveLoader *wave_loader;
  
  if(!AGS_IS_AUDIOREC(audiorec) ||
     filename == NULL){
    return;
  }

  wave_loader = ags_wave_loader_new(AGS_MACHINE(audiorec)->audio,
				    filename,
				    TRUE);

  ags_wave_loader_start(wave_loader);
}

/**
 * ags_audiorec_indicator_queue_draw_timeout:
 * @widget: the widget
 *
 * Queue draw widget
 *
 * Returns: %TRUE if proceed with redraw, otherwise %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_audiorec_indicator_queue_draw_timeout(AgsAudiorec *audiorec)
{
  if(g_hash_table_lookup(ags_audiorec_indicator_queue_draw,
			 audiorec) != NULL){
    AgsAudio *audio;
    AgsChannel *channel;
    
    GList *list, *list_start;

    guint i;

    audio = AGS_MACHINE(audiorec)->audio;
    g_object_get(audio,
		 "output", &channel,
		 NULL);
    
    list_start = 
      list = gtk_container_get_children((GtkContainer *) audiorec->hindicator_vbox);
    
    /* check members */
    for(i = 0; list != NULL; i++){
      GtkAdjustment *adjustment;
      GtkWidget *child;

      AgsPort *current;

      GList *start_port;
      
      gdouble average_peak;
      gdouble peak;
	
      GValue value = {0,};
	
      child = list->data;
      
      average_peak = 0.0;
      
      start_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(channel,
										  "./peak[0]",
										  TRUE);

      current = NULL;

      if(start_port != NULL){
	current = start_port->data;
      }
      
      /* recall port - read value */
      g_value_init(&value, G_TYPE_FLOAT);
      ags_port_safe_read(current,
			 &value);
      
      peak = g_value_get_float(&value);
      g_value_unset(&value);

      /* calculate peak */
      average_peak += ((1.0 / (1.0 / peak)) * 10.0);
      
      /* apply */
      g_object_get(child,
		   "adjustment", &adjustment,
		   NULL);
	
      gtk_adjustment_set_value(adjustment,
			       average_peak);

      /* queue draw */
      gtk_widget_queue_draw(child);

      /* iterate */
      list = list->next;

      g_object_get(channel,
		   "next", &channel,
		   NULL);
    }

    g_list_free(list_start);
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_audiorec_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsAudiorec
 *
 * Returns: the new #AgsAudiorec
 *
 * Since: 2.0.0
 */
AgsAudiorec*
ags_audiorec_new(GObject *soundcard)
{
  AgsAudiorec *audiorec;
  GValue value = {0,};

  audiorec = (AgsAudiorec *) g_object_new(AGS_TYPE_AUDIOREC,
					  NULL);

  g_object_set(G_OBJECT(AGS_MACHINE(audiorec)->audio),
	       "output-soundcard", soundcard,
	       NULL);

  return(audiorec);
}
