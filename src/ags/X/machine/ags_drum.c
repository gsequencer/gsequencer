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

#include <ags/X/machine/ags_drum.h>
#include <ags/X/machine/ags_drum_callbacks.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_portlet.h>
#include <ags/object/ags_plugin.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>
#include <ags/file/ags_file_launch.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recall_factory.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_audio.h>
#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>

#include <ags/audio/task/recall/ags_apply_sequencer_length.h>

#include <ags/widget/ags_led.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_menu_bar.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>

#include <ags/X/machine/ags_drum_input_pad.h>
#include <ags/X/machine/ags_drum_input_line.h>
#include <ags/X/machine/ags_drum_output_pad.h>
#include <ags/X/machine/ags_drum_output_line.h>
#include <ags/X/machine/ags_drum_input_line_callbacks.h>

#include <ags/X/file/ags_gsequencer_file_xml.h>

#include <math.h>

void ags_drum_class_init(AgsDrumClass *drum);
void ags_drum_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_drum_plugin_interface_init(AgsPluginInterface *plugin);
void ags_drum_init(AgsDrum *drum);
void ags_drum_finalize(GObject *gobject);
void ags_drum_connect(AgsConnectable *connectable);
void ags_drum_disconnect(AgsConnectable *connectable);
void ags_drum_show(GtkWidget *widget);
void ags_drum_show_all(GtkWidget *widget);
void ags_drum_map_recall(AgsMachine *machine);
gchar* ags_drum_get_name(AgsPlugin *plugin);
void ags_drum_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_drum_get_xml_type(AgsPlugin *plugin);
void ags_drum_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_drum_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
void ags_drum_launch_task(AgsFileLaunch *file_launch, AgsDrum *drum);
xmlNode* ags_drum_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);
void ags_drum_read_resolve_audio(AgsFileLookup *file_lookup,
				 AgsMachine *machine);

void ags_drum_set_audio_channels(AgsAudio *audio,
				 guint audio_channels, guint audio_channels_old,
				 AgsDrum *drum);
void ags_drum_set_pads(AgsAudio *audio, GType type,
		       guint pads, guint pads_old,
		       AgsDrum *drum);

/**
 * SECTION:ags_drum
 * @short_description: drum sequencer
 * @title: AgsDrum
 * @section_id:
 * @include: ags/X/machine/ags_drum.h
 *
 * The #AgsDrum is a composite widget to act as drum sequencer.
 */

static gpointer ags_drum_parent_class = NULL;

static AgsConnectableInterface *ags_drum_parent_connectable_interface;

GType
ags_drum_get_type(void)
{
  static GType ags_type_drum = 0;

  if(!ags_type_drum){
    static const GTypeInfo ags_drum_info = {
      sizeof(AgsDrumClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_drum_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsDrum),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_drum_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_drum_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_drum_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_drum = g_type_register_static(AGS_TYPE_MACHINE,
					   "AgsDrum\0", &ags_drum_info,
					   0);
    
    g_type_add_interface_static(ags_type_drum,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_drum,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_drum);
}

void
ags_drum_class_init(AgsDrumClass *drum)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  AgsMachineClass *machine;

  ags_drum_parent_class = g_type_class_peek_parent(drum);

  /* GObjectClass */
  gobject = (GObjectClass *) drum;

  gobject->finalize = ags_drum_finalize;

  /* GtkWidget */
  widget = (GtkWidgetClass *) drum;

  widget->show = ags_drum_show;
  widget->show_all = ags_drum_show_all;

  /*  */
  machine = (AgsMachineClass *) drum;

  machine->map_recall = ags_drum_map_recall;
}

void
ags_drum_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_drum_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_drum_connect;
  connectable->disconnect = ags_drum_disconnect;
}

void
ags_drum_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_drum_get_name;
  plugin->set_name = ags_drum_set_name;
  plugin->get_xml_type = ags_drum_get_xml_type;
  plugin->set_xml_type = ags_drum_set_xml_type;
  plugin->read = ags_drum_read;
  plugin->write = ags_drum_write;
}

void
ags_drum_init(AgsDrum *drum)
{
  GtkVBox *vbox;
  GtkHBox *hbox;
  GtkToggleButton *toggle_button;
  GtkFrame *frame;
  GtkTable *table0, *table1;
  GtkRadioButton *radio_button;

  AgsAudio *audio;

  GList *list;
  guint stream_length;
  int i, j;

  g_signal_connect_after((GObject *) drum, "parent_set\0",
			 G_CALLBACK(ags_drum_parent_set_callback), (gpointer) drum);


  audio = AGS_MACHINE(drum)->audio;
  audio->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
		   AGS_AUDIO_INPUT_HAS_RECYCLING |
		   AGS_AUDIO_INPUT_TAKES_FILE |
		   AGS_AUDIO_SYNC |
		   AGS_AUDIO_ASYNC);

  AGS_MACHINE(drum)->flags |= AGS_MACHINE_IS_SEQUENCER;
  AGS_MACHINE(drum)->input_pad_type = AGS_TYPE_DRUM_INPUT_PAD;
  AGS_MACHINE(drum)->input_line_type = AGS_TYPE_DRUM_INPUT_LINE;
  AGS_MACHINE(drum)->output_pad_type = AGS_TYPE_DRUM_OUTPUT_PAD;
  AGS_MACHINE(drum)->output_line_type = AGS_TYPE_DRUM_OUTPUT_LINE;
  
  g_signal_connect_after(G_OBJECT(audio), "set_audio_channels\0",
			 G_CALLBACK(ags_drum_set_audio_channels), drum);

  g_signal_connect_after(G_OBJECT(audio), "set_pads\0",
			 G_CALLBACK(ags_drum_set_pads), drum);

  drum->flags = 0;

  drum->name = NULL;
  drum->xml_type = "ags-drum\0";

  /* create widgets */
  drum->vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_container_add((GtkContainer*) gtk_bin_get_child((GtkBin *) drum), (GtkWidget *) drum->vbox);

  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) drum->vbox, (GtkWidget *) hbox, FALSE, FALSE, 0);

  drum->input_pad = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  AGS_MACHINE(drum)->input = (GtkContainer *) drum->input_pad;
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) drum->input_pad, FALSE, FALSE, 0);

  drum->output_pad = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  AGS_MACHINE(drum)->output = (GtkContainer *) drum->output_pad;
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) drum->output_pad, FALSE, FALSE, 0);

  drum->selected_pad = NULL;
  drum->selected_edit_button = NULL;

  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) drum->vbox, (GtkWidget *) hbox, FALSE, FALSE, 0);

  frame = (GtkFrame *) gtk_frame_new("kit\0");
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) frame, FALSE, FALSE, 0);

  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_container_add((GtkContainer*) frame, (GtkWidget *) vbox);

  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) gtk_label_new("default\0"), 
		     FALSE, FALSE, 0);
 
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) (drum->open = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_OPEN)),
		     FALSE, FALSE, 0);

  /* sequencer */
  frame = (GtkFrame *) gtk_frame_new("Pattern\0");
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) frame, FALSE, FALSE, 0);

  table0 = (GtkTable *) gtk_table_new(16, 4, FALSE);
  gtk_container_add((GtkContainer*) frame, (GtkWidget *) table0);

  drum->loop_button = (GtkCheckButton *) gtk_check_button_new_with_label("loop\0");
  gtk_table_attach_defaults(table0,
			    (GtkWidget *) drum->loop_button,
			    0, 1, 2, 3);

  AGS_MACHINE(drum)->play = 
    drum->run = (GtkToggleButton *) gtk_toggle_button_new_with_label("run\0");
  gtk_table_attach_defaults(table0,
			    (GtkWidget *) drum->run,
			    1, 2, 0, 3);

  /* bank */
  table1 = (GtkTable *) gtk_table_new(3, 5, TRUE);
  gtk_table_attach_defaults(table0,
			    (GtkWidget *) table1,
			    2, 3, 0, 3);

  drum->selected1 = NULL;

  for(i = 0; i < 3; i++)
    for(j = 0; j < 4; j++){
      drum->index1[i * 4 + j] = (GtkToggleButton *) gtk_toggle_button_new_with_label(g_strdup_printf("%d\0",
												     (4 * i) + (j + 1)));
      
      gtk_table_attach_defaults(table1,
				(GtkWidget *) (drum->index1[i * 4 + j]),
				j, j +1, i, i +1);
    }

  drum->selected1 = drum->index1[0];
  gtk_toggle_button_set_active(drum->index1[0], TRUE);

  drum->selected0 = NULL;

  for(j = 0; j < 4; j++){
    drum->index0[j] = (GtkToggleButton *) gtk_toggle_button_new_with_label(g_strdup_printf("%c\0",
											   'a' + j));
    
    gtk_table_attach_defaults(table1,
			      (GtkWidget *) (drum->index0[j]),
			      j, j +1, 4, 5);
  }

  drum->selected0 = drum->index0[0];
  gtk_toggle_button_set_active(drum->index0[0], TRUE);

  /* duration */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_table_attach(table0,
		   (GtkWidget *) hbox,
		   6, 7, 0, 1,
		   GTK_EXPAND, GTK_EXPAND,
		   0, 0);
  gtk_box_pack_start((GtkBox*) hbox, gtk_label_new("length\0"), FALSE, FALSE, 0);
  drum->length_spin = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0, 64.0, 1.0);
  drum->length_spin->adjustment->value = 16.0;
  gtk_box_pack_start((GtkBox*) hbox, (GtkWidget *) drum->length_spin, FALSE, FALSE, 0);

  /* led */
  drum->active_led = 0;

  drum->led =
    hbox = (GtkHBox *) gtk_hbox_new(FALSE, 16);
  gtk_table_attach(table0,
		   (GtkWidget *) hbox,
		   3, 15,
		   1, 2,
		   0, 0,
		   0, 0);

  for(i = 0; i < 16; i++){
    toggle_button = (GtkToggleButton *) ags_led_new();
    gtk_widget_set_size_request((GtkWidget *) toggle_button, 8, 4);
    gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) toggle_button, FALSE, FALSE, 0);
  }

  /* pattern */
  drum->pattern =
    hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_table_attach(table0,
		   (GtkWidget *) hbox,
		   3, 15,
		   2, 3,
		   0, 0,
		   0, 0);

  for(i = 0; i < 16; i++){
    toggle_button = (GtkToggleButton *) gtk_toggle_button_new();
    gtk_widget_set_size_request((GtkWidget *) toggle_button, 24, 24);
    gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) toggle_button, FALSE, FALSE, 0);
  }

  /* page / offset */
  drum->offset = (GtkVBox*) gtk_vbox_new(FALSE, 0);
  gtk_table_attach_defaults(table0, (GtkWidget *) drum->offset, 15, 16, 0, 3);

  radio_button = (GtkRadioButton *) gtk_radio_button_new_with_label(NULL, "1-16\0");
  gtk_box_pack_start((GtkBox*) drum->offset, (GtkWidget *) radio_button, FALSE, FALSE, 0);

  gtk_box_pack_start((GtkBox*) drum->offset,
		     (GtkWidget *) gtk_radio_button_new_with_label(radio_button->group, "17-32\0"),
		     FALSE, FALSE, 0);

  gtk_box_pack_start((GtkBox*) drum->offset,
		     (GtkWidget *) gtk_radio_button_new_with_label(radio_button->group, "33-48\0"),
		     FALSE, FALSE, 0);

  gtk_box_pack_start((GtkBox*) drum->offset,
		     (GtkWidget *) gtk_radio_button_new_with_label(radio_button->group, "49-64\0"),
		     FALSE, FALSE, 0);
}

void
ags_drum_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_drum_parent_class)->finalize(gobject);
}

void
ags_drum_connect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsDrum *drum;
  AgsDelayAudioRun *play_delay_audio_run;
  AgsRecallHandler *recall_handler;
  GList *list, *list_start;
  int i;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_drum_parent_connectable_interface->connect(connectable);

  drum = AGS_DRUM(connectable);
  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) drum);

  /* AgsDrum */
  g_signal_connect((GObject *) drum->open, "clicked\0",
		   G_CALLBACK(ags_drum_open_callback), (gpointer) drum);

  g_signal_connect((GObject *) drum->loop_button, "clicked\0",
		   G_CALLBACK(ags_drum_loop_button_callback), (gpointer) drum);

  g_signal_connect_after((GObject *) drum->length_spin, "value-changed\0",
			 G_CALLBACK(ags_drum_length_spin_callback), (gpointer) drum);

  for(i = 0; i < 12; i++){
    g_signal_connect(G_OBJECT(drum->index1[i]), "clicked\0",
		     G_CALLBACK(ags_drum_index1_callback), (gpointer) drum);
  }

  for(i = 0; i < 4; i++){
    g_signal_connect(G_OBJECT(drum->index0[i]), "clicked\0",
		     G_CALLBACK(ags_drum_index0_callback), (gpointer) drum);
  }

  /* connect pattern */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) drum->pattern);

  while(list != NULL){
    g_signal_connect(G_OBJECT(list->data), "clicked\0",
		     G_CALLBACK(ags_drum_pad_callback), (gpointer) drum);

    list = list->next;
  }

  g_list_free(list_start);

  /* connect pattern offset range */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) drum->offset);

  while(list != NULL){
    g_signal_connect(G_OBJECT(list->data), "clicked\0",
		     G_CALLBACK(ags_drum_offset_callback), (gpointer) drum);
		   
    list = list->next;
  }

  g_list_free(list_start);

  /* AgsAudio */
  g_signal_connect_after(G_OBJECT(AGS_MACHINE(drum)->audio), "tact\0",
			 G_CALLBACK(ags_drum_tact_callback), drum);

  g_signal_connect_after(G_OBJECT(AGS_MACHINE(drum)->audio), "done\0",
			 G_CALLBACK(ags_drum_done_callback), drum);
}

void
ags_drum_disconnect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsDrum *drum;

  ags_drum_parent_connectable_interface->disconnect(connectable);

  drum = AGS_DRUM(connectable);

  window = AGS_WINDOW(gtk_widget_get_ancestor((GtkWidget *) drum, AGS_TYPE_WINDOW));

  //TODO:JK: implement me
}

void
ags_drum_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_drum_parent_class)->show(widget);

  ags_drum_set_pattern(AGS_DRUM(widget));
}

void
ags_drum_show_all(GtkWidget *widget)
{
  AgsDrum *drum;
  
  GTK_WIDGET_CLASS(ags_drum_parent_class)->show_all(widget);

  drum = AGS_DRUM(widget);
  
  ags_drum_set_pattern(drum);
}

void
ags_drum_map_recall(AgsMachine *machine)
{
  AgsAudio *audio;

  AgsDelayAudio *play_delay_audio;
  AgsDelayAudioRun *play_delay_audio_run;
  AgsCountBeatsAudio *play_count_beats_audio;
  AgsCountBeatsAudioRun *play_count_beats_audio_run;
  AgsCopyPatternAudio *recall_copy_pattern_audio;
  AgsCopyPatternAudioRun *recall_copy_pattern_audio_run;

  GList *list;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  audio = machine->audio;

  /* ags-delay */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-delay\0",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY),
			    0);

  list = ags_recall_find_type(audio->play, AGS_TYPE_DELAY_AUDIO_RUN);

  if(list != NULL){
    play_delay_audio_run = AGS_DELAY_AUDIO_RUN(list->data);
    //    AGS_RECALL(play_delay_audio_run)->flags |= AGS_RECALL_PERSISTENT;
  }
  
  /* ags-count-beats */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-count-beats\0",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY),
			    0);
  
  list = ags_recall_find_type(audio->play, AGS_TYPE_COUNT_BEATS_AUDIO_RUN);

  if(list != NULL){
    play_count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(list->data);

    /* set dependency */  
    g_object_set(G_OBJECT(play_count_beats_audio_run),
		 "delay-audio-run\0", play_delay_audio_run,
		 NULL);
  }

  /* ags-copy-pattern */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-copy-pattern\0",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_RECALL),
			    0);

  list = ags_recall_find_type(audio->recall, AGS_TYPE_COPY_PATTERN_AUDIO_RUN);

  if(list != NULL){
    recall_copy_pattern_audio_run = AGS_COPY_PATTERN_AUDIO_RUN(list->data);

    /* set dependency */
    g_object_set(G_OBJECT(recall_copy_pattern_audio_run),
		 "delay-audio-run\0", play_delay_audio_run,
		 "count-beats-audio-run\0", play_count_beats_audio_run,
		 NULL);

  }

  AGS_MACHINE_CLASS(ags_drum_parent_class)->map_recall(machine);
}

gchar*
ags_drum_get_name(AgsPlugin *plugin)
{
  return(AGS_DRUM(plugin)->name);
}

void
ags_drum_set_name(AgsPlugin *plugin, gchar *name)
{
  AGS_DRUM(plugin)->name = name;
}

gchar*
ags_drum_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_DRUM(plugin)->xml_type);
}

void
ags_drum_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_DRUM(plugin)->xml_type = xml_type;
}

void
ags_drum_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsDrum *gobject;
  AgsFileLookup *file_lookup;
  AgsFileLaunch *file_launch;
  GList *list;

  gobject = AGS_DRUM(plugin);

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
			     G_CALLBACK(ags_drum_read_resolve_audio), gobject);
      
      break;
    }

    list = list->next;
  }

  /*  */
  file_launch = g_object_new(AGS_TYPE_FILE_LAUNCH,
			     "node\0", node,
			     "file\0", file,
			     NULL);
  g_signal_connect(G_OBJECT(file_launch), "start\0",
		   G_CALLBACK(ags_drum_launch_task), gobject);
  ags_file_add_launch(file,
		      file_launch);
}

void
ags_drum_read_resolve_audio(AgsFileLookup *file_lookup,
			    AgsMachine *machine)
{
  AgsDrum *drum;
  GList *pad, *pad_start, *line, *line_start;

  drum = AGS_DRUM(machine);

  g_signal_connect_after(G_OBJECT(machine->audio), "set_audio_channels\0",
			 G_CALLBACK(ags_drum_set_audio_channels), drum);

  g_signal_connect_after(G_OBJECT(machine->audio), "set_pads\0",
			 G_CALLBACK(ags_drum_set_pads), drum);

  pad_start = 
    pad = gtk_container_get_children(machine->input);

  while(pad != NULL){
    line_start = 
      line = gtk_container_get_children(AGS_PAD(pad->data)->expander_set);

    while(line != NULL){
      /* AgsAudio */
      g_signal_connect_after(G_OBJECT(machine->audio), "set_pads\0",
			     G_CALLBACK(ags_drum_input_line_audio_set_pads_callback), AGS_DRUM_INPUT_LINE(line->data));

      line = line->next;
    }

    g_list_free(line_start);
    pad = pad->next;
  }
}

void
ags_drum_launch_task(AgsFileLaunch *file_launch, AgsDrum *drum)
{
  xmlNode *node;
  guint64 length, index;

  node = file_launch->node;

  /* length */
  length = (gdouble) g_ascii_strtod(xmlGetProp(node,
					       "length\0"),
				    NULL);
  gtk_spin_button_set_value(drum->length_spin,
			    length);

  /* loop */
  if(!g_strcmp0(xmlGetProp(node,
			   "loop\0"),
		AGS_FILE_TRUE)){
    gtk_toggle_button_set_active(drum->loop_button,
				 TRUE);
  }

  /* index */
  index = g_ascii_strtoull(xmlGetProp(node,
				      "bank-index-0\0"),
			   NULL,
			   10);

  if(index != 0){
    gtk_toggle_button_set_active(drum->index0[0],
				 FALSE);
    gtk_toggle_button_set_active(drum->index0[index],
				 TRUE);
    drum->selected0 = drum->index0[index];
  }

  index = g_ascii_strtoull(xmlGetProp(node,
				      "bank-index-1\0"),
			   NULL,
			   10);

  if(index != 0){
    gtk_toggle_button_set_active(drum->index1[0],
				 FALSE);
    gtk_toggle_button_set_active(drum->index1[index],
				 TRUE);
    drum->selected1 = drum->index1[index];
  }
}

xmlNode*
ags_drum_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsDrum *drum;
  xmlNode *node;
  GList *list;
  gchar *id;
  guint i;

  drum = AGS_DRUM(plugin);

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-drum\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", drum,
				   NULL));

  xmlNewProp(node,
	     "length\0",
	     g_strdup_printf("%d\0", (gint) gtk_spin_button_get_value(drum->length_spin)));

  for(i = 0; drum->selected0 != drum->index0[i]; i++);

  xmlNewProp(node,
	     "bank-index-0\0",
	     g_strdup_printf("%d\0", i));

  for(i = 0; drum->selected1 != drum->index1[i]; i++);

  xmlNewProp(node,
	     "bank-index-1\0",
	     g_strdup_printf("%d\0", i));

  xmlNewProp(node,
	     "loop\0",
	     g_strdup_printf("%s\0", ((gtk_toggle_button_get_active(drum->loop_button)) ? AGS_FILE_TRUE: AGS_FILE_FALSE)));

  xmlAddChild(parent,
	      node);

  return(node);
}

void
ags_drum_set_audio_channels(AgsAudio *audio,
			    guint audio_channels, guint audio_channels_old,
			    AgsDrum *drum)
{
  //empty
}

void
ags_drum_set_pads(AgsAudio *audio, GType type,
		  guint pads, guint pads_old,
		  AgsDrum *drum)
{
  AgsChannel *channel;
  GList *list, *list_next;
  guint i, j;

  if(type == AGS_TYPE_INPUT){
    AgsDrumInputPad *drum_input_pad;

    if(pads_old < pads){
      /* reset edit button */
      if(pads_old == 0){
	GtkToggleButton *selected_edit_button;

	drum->selected_pad = AGS_DRUM_INPUT_PAD(gtk_container_get_children((GtkContainer *) drum->input_pad)->data);
	drum->selected_edit_button = drum->selected_pad->edit;
	gtk_toggle_button_set_active((GtkToggleButton *) drum->selected_edit_button, TRUE);
      }
    }else{
      /* destroy AgsPad's */
      if(pads == 0){
	drum->selected_pad = NULL;
	drum->selected_edit_button = NULL;
      }else{
	drum_input_pad = AGS_DRUM_INPUT_PAD(gtk_widget_get_ancestor(GTK_WIDGET(drum->selected_edit_button), AGS_TYPE_PAD));

	if(drum_input_pad->pad.channel->pad > pads){
	  drum->selected_pad = AGS_DRUM_INPUT_PAD(gtk_container_get_children((GtkContainer *) drum->input_pad)->data);
	  drum->selected_edit_button = drum->selected_pad->edit;
	  gtk_toggle_button_set_active((GtkToggleButton *) drum->selected_edit_button, TRUE);
	}
      }
    }
  }  
}

void
ags_drum_set_pattern(AgsDrum *drum)
{
  AgsLine *selected_line;
  GList *list, *line;
  guint index0, index1, offset;
  gboolean set_active;
  guint i;

  if(drum->selected_pad == NULL){
    g_message("no selected pad\n\0");
    return;
  }

  for(i = 0; i < 4 && drum->selected0 != drum->index0[i]; i++);

  index0 = i;

  for(i = 0; i < 12 && drum->selected1 != drum->index1[i]; i++);
  
  index1 = i;

  list = gtk_container_get_children((GtkContainer *) drum->offset);

  for(i = 0; i < 4 && ! GTK_TOGGLE_BUTTON(list->data)->active; i++)
    list = list->next;

  offset = i * 16;

  list = gtk_container_get_children((GtkContainer *) drum->pattern);

  drum->flags |= AGS_DRUM_BLOCK_PATTERN;

  for(i = 0; i < 16; i++){
    set_active = TRUE;

    line = gtk_container_get_children(GTK_CONTAINER(AGS_PAD(drum->selected_pad)->expander_set));

    while((line = ags_line_find_next_grouped(line)) != NULL){
      selected_line = AGS_LINE(line->data);

      if(!ags_pattern_get_bit((AgsPattern *) selected_line->channel->pattern->data, index0, index1, offset + i)){
	set_active = FALSE;
	break;
      }

      line = line->next;
    }

    gtk_toggle_button_set_active((GtkToggleButton *) list->data, set_active);

    list = list->next;
  }

  drum->flags &= (~AGS_DRUM_BLOCK_PATTERN);
}

/**
 * ags_drum_new:
 * @soundcard: the assigned soundcard.
 *
 * Creates an #AgsDrum
 *
 * Returns: a new #AgsDrum
 *
 * Since: 0.3
 */
AgsDrum*
ags_drum_new(GObject *soundcard)
{
  AgsDrum *drum;
  GValue value = {0,};

  drum = (AgsDrum *) g_object_new(AGS_TYPE_DRUM,
				  NULL);

    if(soundcard != NULL){
    g_value_init(&value, G_TYPE_OBJECT);
    g_value_set_object(&value, soundcard);
    g_object_set_property(G_OBJECT(AGS_MACHINE(drum)->audio),
			  "soundcard\0", &value);
    g_value_unset(&value);
  }

  return(drum);
}
