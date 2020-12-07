/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/X/machine/ags_drum.h>
#include <ags/X/machine/ags_drum_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_menu_bar.h>
#include <ags/X/ags_navigation.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>

#include <ags/X/machine/ags_drum_input_pad.h>
#include <ags/X/machine/ags_drum_input_line.h>
#include <ags/X/machine/ags_drum_output_pad.h>
#include <ags/X/machine/ags_drum_output_line.h>
#include <ags/X/machine/ags_drum_input_line_callbacks.h>

#include <math.h>

#include <ags/i18n.h>

void ags_drum_class_init(AgsDrumClass *drum);
void ags_drum_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_drum_init(AgsDrum *drum);
void ags_drum_finalize(GObject *gobject);

void ags_drum_connect(AgsConnectable *connectable);
void ags_drum_disconnect(AgsConnectable *connectable);

void ags_drum_show(GtkWidget *widget);
void ags_drum_show_all(GtkWidget *widget);

void ags_drum_resize_audio_channels(AgsMachine *machine,
				    guint audio_channels, guint audio_channels_old,
				    gpointer data);
void ags_drum_resize_pads(AgsMachine *machine,
			  GType channel_type,
			  guint pads, guint pads_old,
			  gpointer data);

void ags_drum_map_recall(AgsMachine *machine);

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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_drum = 0;

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

    ags_type_drum = g_type_register_static(AGS_TYPE_MACHINE,
					   "AgsDrum", &ags_drum_info,
					   0);
    
    g_type_add_interface_static(ags_type_drum,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_drum);
  }

  return g_define_type_id__volatile;
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

  gchar *str;
  
  guint stream_length;
  int i, j;

  g_signal_connect_after((GObject *) drum, "parent_set",
			 G_CALLBACK(ags_drum_parent_set_callback), (gpointer) drum);


  audio = AGS_MACHINE(drum)->audio;
  ags_audio_set_flags(audio, (AGS_AUDIO_SYNC |
			      AGS_AUDIO_ASYNC |
			      AGS_AUDIO_OUTPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_FILE));
  ags_audio_set_ability_flags(audio, (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_NOTATION));
  ags_audio_set_behaviour_flags(audio, (AGS_SOUND_BEHAVIOUR_PATTERN_MODE |
					AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT));

  
  g_object_set(audio,
	       "min-audio-channels", 1,
	       "min-output-pads", 1,
	       "min-input-pads", 1,
	       "max-input-pads", 128,
	       "audio-start-mapping", 0,
	       "audio-end-mapping", 128,
	       "midi-start-mapping", 0,
	       "midi-end-mapping", 128,
	       NULL);

  audio->bank_dim[0] = 4;
  audio->bank_dim[1] = 12;
  audio->bank_dim[2] = 64;

  AGS_MACHINE(drum)->flags |= (AGS_MACHINE_IS_SEQUENCER |
			       AGS_MACHINE_TAKES_FILE_INPUT);
  AGS_MACHINE(drum)->file_input_flags |= (AGS_MACHINE_ACCEPT_WAV);
  AGS_MACHINE(drum)->input_pad_type = AGS_TYPE_DRUM_INPUT_PAD;
  AGS_MACHINE(drum)->input_line_type = AGS_TYPE_DRUM_INPUT_LINE;
  AGS_MACHINE(drum)->output_pad_type = AGS_TYPE_DRUM_OUTPUT_PAD;
  AGS_MACHINE(drum)->output_line_type = AGS_TYPE_DRUM_OUTPUT_LINE;

  /* context menu */
  ags_machine_popup_add_edit_options((AgsMachine *) drum,
				     (AGS_MACHINE_POPUP_COPY_PATTERN |
				      AGS_MACHINE_POPUP_ENVELOPE));
  
  ags_machine_popup_add_connection_options((AgsMachine *) drum,
  					   (AGS_MACHINE_POPUP_MIDI_DIALOG));

  /* audio resize */
  g_signal_connect_after(G_OBJECT(drum), "resize-audio-channels",
			 G_CALLBACK(ags_drum_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(drum), "resize-pads",
			 G_CALLBACK(ags_drum_resize_pads), NULL);

  /* flags, name and xml type */
  drum->flags = 0;

  drum->name = NULL;
  drum->xml_type = "ags-drum";

  drum->playback_play_container = ags_recall_container_new();
  drum->playback_recall_container = ags_recall_container_new();

  drum->pattern_play_container = ags_recall_container_new();
  drum->pattern_recall_container = ags_recall_container_new();

  drum->notation_play_container = ags_recall_container_new();
  drum->notation_recall_container = ags_recall_container_new();

  drum->volume_play_container = ags_recall_container_new();
  drum->volume_recall_container = ags_recall_container_new();

  drum->envelope_play_container = ags_recall_container_new();
  drum->envelope_recall_container = ags_recall_container_new();

  drum->peak_play_container = ags_recall_container_new();
  drum->peak_recall_container = ags_recall_container_new();

  drum->buffer_play_container = ags_recall_container_new();
  drum->buffer_recall_container = ags_recall_container_new();

  /* create widgets */
  drum->vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_container_add((GtkContainer*) gtk_bin_get_child((GtkBin *) drum), (GtkWidget *) drum->vbox);

  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) drum->vbox, (GtkWidget *) hbox, FALSE, FALSE, 0);

  /* input pad */
  drum->input_pad = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  AGS_MACHINE(drum)->input = (GtkContainer *) drum->input_pad;
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) drum->input_pad, FALSE, FALSE, 0);

  /* output pad */
  drum->output_pad = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  AGS_MACHINE(drum)->output = (GtkContainer *) drum->output_pad;
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) drum->output_pad, FALSE, FALSE, 0);

  /*  */
  drum->selected_pad = NULL;
  drum->selected_edit_button = NULL;

  /*  */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) drum->vbox,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  frame = (GtkFrame *) gtk_frame_new(i18n("kit"));
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) frame,
		     FALSE, FALSE,
		     0);

  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_container_add((GtkContainer*) frame,
		    (GtkWidget *) vbox);

  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) gtk_label_new(i18n("default")), 
		     FALSE, FALSE,
		     0);
 
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) (drum->open = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_OPEN)),
		     FALSE, FALSE,
		     0);
  drum->open_dialog = NULL;
  
  /* sequencer */
  frame = (GtkFrame *) gtk_frame_new(i18n("pattern"));
  gtk_box_pack_start((GtkBox *) hbox, (GtkWidget *) frame,
		     FALSE, FALSE,
		     0);

  table0 = (GtkTable *) gtk_table_new(8, 4, FALSE);
  gtk_container_add((GtkContainer*) frame,
		    (GtkWidget *) table0);

  drum->loop_button = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("loop"));
  gtk_table_attach_defaults(table0,
			    (GtkWidget *) drum->loop_button,
			    0, 1,
			    2, 3);

  AGS_MACHINE(drum)->play = 
    drum->run = (GtkToggleButton *) gtk_toggle_button_new_with_label(i18n("run"));
  gtk_table_attach_defaults(table0,
			    (GtkWidget *) drum->run,
			    1, 2,
			    0, 3);

  /* bank 1 */
  table1 = (GtkTable *) gtk_table_new(3, 5, TRUE);
  gtk_table_attach_defaults(table0,
			    (GtkWidget *) table1,
			    2, 3,
			    0, 3);

  drum->selected1 = NULL;

  for(i = 0; i < 3; i++){
    for(j = 0; j < 4; j++){
      str = g_strdup_printf("%d",
			    (4 * i) + (j + 1));
      drum->index1[4 * i + j] = (GtkToggleButton *) gtk_toggle_button_new_with_label(str);
      gtk_table_attach_defaults(table1,
				(GtkWidget *) (drum->index1[4 * i + j]),
				j, j + 1,
				i, i + 1);

      g_free(str);
    }
  }

  /* bank 0 */
  drum->selected1 = drum->index1[0];
  gtk_toggle_button_set_active(drum->index1[0], TRUE);

  drum->selected0 = NULL;

  for(j = 0; j < 4; j++){
    str = g_strdup_printf("%c",
			  'a' + j);
    drum->index0[j] = (GtkToggleButton *) gtk_toggle_button_new_with_label(str);
    gtk_table_attach_defaults(table1,
			      (GtkWidget *) (drum->index0[j]),
			      j, j + 1,
			      4, 5);

    g_free(str);
  }

  drum->selected0 = drum->index0[0];
  gtk_toggle_button_set_active(drum->index0[0], TRUE);

  /* duration */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_table_attach(table0,
		   (GtkWidget *) hbox,
		   6, 7,
		   0, 1,
		   GTK_EXPAND, GTK_EXPAND,
		   0, 0);
  gtk_box_pack_start((GtkBox*) hbox, gtk_label_new(i18n("length")), FALSE, FALSE, 0);
  drum->length_spin = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0, 64.0, 1.0);
  gtk_spin_button_set_value(drum->length_spin, 16.0);
  gtk_box_pack_start((GtkBox*) hbox, (GtkWidget *) drum->length_spin, FALSE, FALSE, 0);

  /* pattern box */
  drum->pattern_box = ags_pattern_box_new();
  gtk_table_attach(table0,
		   (GtkWidget *) drum->pattern_box,
		   7, 8,
		   0, 3,
		   GTK_EXPAND, GTK_EXPAND,
		   0, 0);  
}

void
ags_drum_finalize(GObject *gobject)
{
  g_object_disconnect(gobject,
		      "any_signal::resize-audio-channels",
		      G_CALLBACK(ags_drum_resize_audio_channels),
		      NULL,
		      "any_signal::resize-pads",
		      G_CALLBACK(ags_drum_resize_pads),
		      NULL,
		      NULL);

  /* call parent */
  G_OBJECT_CLASS(ags_drum_parent_class)->finalize(gobject);
}

void
ags_drum_connect(AgsConnectable *connectable)
{
  AgsDrum *drum;

  int i;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  drum = AGS_DRUM(connectable);

  /* call parent */
  ags_drum_parent_connectable_interface->connect(connectable);

  /* GtkObject */
  g_signal_connect((GObject *) drum, "destroy",
		   G_CALLBACK(ags_drum_destroy_callback), (gpointer) drum);
  
  /* AgsDrum */
  g_signal_connect((GObject *) drum->open, "clicked",
		   G_CALLBACK(ags_drum_open_callback), (gpointer) drum);

  g_signal_connect((GObject *) drum->loop_button, "clicked",
		   G_CALLBACK(ags_drum_loop_button_callback), (gpointer) drum);

  g_signal_connect_after((GObject *) drum->length_spin, "value-changed",
			 G_CALLBACK(ags_drum_length_spin_callback), (gpointer) drum);

  for(i = 0; i < 12; i++){
    g_signal_connect(G_OBJECT(drum->index1[i]), "clicked",
		     G_CALLBACK(ags_drum_index1_callback), (gpointer) drum);
  }

  for(i = 0; i < 4; i++){
    g_signal_connect(G_OBJECT(drum->index0[i]), "clicked",
		     G_CALLBACK(ags_drum_index0_callback), (gpointer) drum);
  }

  ags_connectable_connect(AGS_CONNECTABLE(drum->pattern_box));

  /* AgsMachine */
  g_signal_connect_after(G_OBJECT(drum), "stop",
			 G_CALLBACK(ags_drum_stop_callback), NULL);
}

void
ags_drum_disconnect(AgsConnectable *connectable)
{
  AgsDrum *drum;

  int i;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  drum = AGS_DRUM(connectable);

  /* GtkObject */
  g_object_disconnect((GObject *) drum,
		      "any_signal::destroy",
		      G_CALLBACK(ags_drum_destroy_callback),
		      (gpointer) drum,
		      NULL);
  
  /* AgsDrum */
  g_object_disconnect((GObject *) drum->open,
		      "any_signal::clicked",
		      G_CALLBACK(ags_drum_open_callback),
		      (gpointer) drum,
		      NULL);

  g_object_disconnect((GObject *) drum->loop_button,
		      "any_signal::clicked",
		      G_CALLBACK(ags_drum_loop_button_callback),
		      (gpointer) drum,
		      NULL);

  g_object_disconnect((GObject *) drum->length_spin,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_drum_length_spin_callback),
		      (gpointer) drum,
		      NULL);

  for(i = 0; i < 12; i++){
    g_object_disconnect(G_OBJECT(drum->index1[i]),
			"any_signal::clicked",
			G_CALLBACK(ags_drum_index1_callback),
			(gpointer) drum,
			NULL);
  }

  for(i = 0; i < 4; i++){
    g_object_disconnect(G_OBJECT(drum->index0[i]),
			"any_signal::clicked",
			G_CALLBACK(ags_drum_index0_callback),
			(gpointer) drum,
			NULL);
  }

  ags_connectable_disconnect(AGS_CONNECTABLE(drum->pattern_box));

  /* AgsAudio */
  g_object_disconnect(G_OBJECT(drum),
		      "any_signal::stop",
		      G_CALLBACK(ags_drum_stop_callback),
		      NULL,
		      NULL);

  /* call parent */
  ags_drum_parent_connectable_interface->disconnect(connectable);
}

void
ags_drum_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_drum_parent_class)->show(widget);

  ags_pattern_box_set_pattern(AGS_DRUM(widget)->pattern_box);
}

void
ags_drum_show_all(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_drum_parent_class)->show_all(widget);

  ags_pattern_box_set_pattern(AGS_DRUM(widget)->pattern_box);
}

void
ags_drum_resize_audio_channels(AgsMachine *machine,
			       guint audio_channels, guint audio_channels_old,
			       gpointer data)
{
  //empty
}

void
ags_drum_resize_pads(AgsMachine *machine,
		     GType channel_type,
		     guint pads, guint pads_old,
		     gpointer data)
{
  AgsDrum *drum;
  AgsDrumInputPad *drum_input_pad;
  
  GList *start_list;

  drum = AGS_DRUM(machine);
  
  if(g_type_is_a(channel_type, AGS_TYPE_INPUT)){
    if(pads > pads_old){
      /* reset edit button */
      if(pads_old == 0){
	GtkToggleButton *selected_edit_button;

	start_list = gtk_container_get_children((GtkContainer *) drum->input_pad);

	drum_input_pad = NULL;

	if(start_list != NULL){
	  drum_input_pad = AGS_DRUM_INPUT_PAD(start_list->data);
	}
	
	drum->selected_pad = drum_input_pad;
	AGS_MACHINE(drum)->selected_input_pad = (GtkWidget *) drum_input_pad;

	if(drum->selected_pad != NULL){
	  drum->selected_edit_button = drum->selected_pad->edit;
	  gtk_toggle_button_set_active((GtkToggleButton *) drum->selected_edit_button, TRUE);
	}

	g_list_free(start_list);
      }
    }else{
      /* destroy AgsPad's */
      if(pads == 0){
	drum->selected_pad = NULL;
	drum->selected_edit_button = NULL;
      }else{
	guint pad;
	
	drum_input_pad = AGS_DRUM_INPUT_PAD(gtk_widget_get_ancestor(GTK_WIDGET(drum->selected_edit_button),
								    AGS_TYPE_PAD));

	g_object_get(AGS_PAD(drum_input_pad)->channel,
		     "pad", &pad,
		     NULL);
	
	if(pad > pads){
	  start_list = gtk_container_get_children((GtkContainer *) drum->input_pad);

	  drum_input_pad = NULL;

	  if(start_list != NULL){
	    drum_input_pad = AGS_DRUM_INPUT_PAD(start_list->data);
	  }
	  
	  drum->selected_pad = drum_input_pad;
	  AGS_MACHINE(drum)->selected_input_pad = (GtkWidget *) drum_input_pad;
	  
	  if(drum->selected_pad != NULL){
	    drum->selected_edit_button = drum->selected_pad->edit;
	    gtk_toggle_button_set_active((GtkToggleButton *) drum->selected_edit_button, TRUE);
	  }
	  
	  g_list_free(start_list);
	}
      }
    }
  }else{
    if(pads > pads_old){
      //empty
    }
  }
}

void
ags_drum_map_recall(AgsMachine *machine)
{
  AgsNavigation *navigation;
  AgsDrum *drum;
  
  AgsAudio *audio;

  AgsApplicationContext *application_context;
  
  GList *start_recall, *recall;

  gint position;
  
  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  application_context = ags_application_context_get_instance();

  navigation = ags_ui_provider_get_navigation(AGS_UI_PROVIDER(application_context));
  
  drum = AGS_DRUM(machine);
  
  audio = machine->audio;

  position = 0;
  
  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       drum->playback_play_container, drum->playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-pattern */
  start_recall = ags_fx_factory_create(audio,
				       drum->pattern_play_container, drum->pattern_recall_container,
				       "ags-fx-pattern",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-notation */
  start_recall = ags_fx_factory_create(audio,
				       drum->notation_play_container, drum->notation_recall_container,
				       "ags-fx-notation",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  recall = start_recall;

  while((recall = ags_recall_template_find_type(recall, AGS_TYPE_FX_NOTATION_AUDIO)) != NULL){
    AgsPort *port;

    GValue value = G_VALUE_INIT;
    
    ags_fx_notation_audio_set_pattern_mode(recall->data,
					   TRUE);
    
    /* loop */
    port = NULL;
    
    g_object_get(recall->data,
		 "loop", &port,
		 NULL);

    g_value_init(&value,
		 G_TYPE_BOOLEAN);

    g_value_set_boolean(&value,
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(navigation->loop)));

    ags_port_safe_write(port,
			&value);

    if(port != NULL){
      g_object_unref(port);
    }
    
    /* loop start */
    port = NULL;
    
    g_object_get(recall->data,
		 "loop-start", &port,
		 NULL);

    g_value_unset(&value);
    g_value_init(&value,
		 G_TYPE_UINT64);

    g_value_set_uint64(&value,
		       16 * gtk_spin_button_get_value_as_int(navigation->loop_left_tact));

    ags_port_safe_write(port,
			&value);

    if(port != NULL){
      g_object_unref(port);
    }
    
    /* loop end */
    port = NULL;
    
    g_object_get(recall->data,
		 "loop-end", &port,
		 NULL);

    g_value_unset(&value);
    g_value_init(&value,
		 G_TYPE_UINT64);

    g_value_set_uint64(&value,
		       16 * gtk_spin_button_get_value_as_int(navigation->loop_right_tact));

    ags_port_safe_write(port,
			&value);

    if(port != NULL){
      g_object_unref(port);
    }

    /* iterate */
    recall = recall->next;
  }
  
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-volume */
  start_recall = ags_fx_factory_create(audio,
				       drum->volume_play_container, drum->volume_recall_container,
				       "ags-fx-volume",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-envelope */
  start_recall = ags_fx_factory_create(audio,
				       drum->envelope_play_container, drum->envelope_recall_container,
				       "ags-fx-envelope",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-peak */
  start_recall = ags_fx_factory_create(audio,
				       drum->peak_play_container, drum->peak_recall_container,
				       "ags-fx-peak",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-buffer */
  start_recall = ags_fx_factory_create(audio,
				       drum->buffer_play_container, drum->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
  
  /* call parent */
  AGS_MACHINE_CLASS(ags_drum_parent_class)->map_recall(machine);
}

/**
 * ags_drum_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsDrum
 *
 * Returns: the new #AgsDrum
 *
 * Since: 3.0.0
 */
AgsDrum*
ags_drum_new(GObject *soundcard)
{
  AgsDrum *drum;

  drum = (AgsDrum *) g_object_new(AGS_TYPE_DRUM,
				  NULL);

  g_object_set(AGS_MACHINE(drum)->audio,
	       "output-soundcard", soundcard,
	       NULL);

  return(drum);
}
