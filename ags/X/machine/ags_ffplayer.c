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

#include <ags/X/machine/ags_ffplayer.h>
#include <ags/X/machine/ags_ffplayer_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>

#include <ags/X/machine/ags_ffplayer_bridge.h>

#include <math.h>

#include <ags/i18n.h>

void ags_ffplayer_class_init(AgsFFPlayerClass *ffplayer);
void ags_ffplayer_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ffplayer_init(AgsFFPlayer *ffplayer);
void ags_ffplayer_finalize(GObject *gobject);

void ags_ffplayer_connect(AgsConnectable *connectable);
void ags_ffplayer_disconnect(AgsConnectable *connectable);

void ags_ffplayer_realize(GtkWidget *widget);

void ags_ffplayer_resize_audio_channels(AgsMachine *machine,
				     guint audio_channels, guint audio_channels_old,
				     gpointer data);
void ags_ffplayer_resize_pads(AgsMachine *machine, GType type,
			      guint pads, guint pads_old,
			      gpointer data);

void ags_ffplayer_map_recall(AgsMachine *machine);
void ags_ffplayer_output_map_recall(AgsFFPlayer *ffplayer, guint output_pad_start);
void ags_ffplayer_input_map_recall(AgsFFPlayer *ffplayer, guint input_pad_start);

/**
 * SECTION:ags_ffplayer
 * @short_description: ffplayer notation
 * @title: AgsFFPlayer
 * @section_id:
 * @include: ags/X/machine/ags_ffplayer.h
 *
 * The #AgsFFPlayer is a composite widget to act as soundfont2 notation player.
 */

static gpointer ags_ffplayer_parent_class = NULL;
static AgsConnectableInterface *ags_ffplayer_parent_connectable_interface;

GHashTable *ags_ffplayer_sf2_loader_completed = NULL;

GtkStyle *ffplayer_style = NULL;

GType
ags_ffplayer_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_ffplayer = 0;

    static const GTypeInfo ags_ffplayer_info = {
      sizeof(AgsFFPlayerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ffplayer_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFFPlayer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ffplayer_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ffplayer_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ffplayer = g_type_register_static(AGS_TYPE_MACHINE,
					       "AgsFFPlayer", &ags_ffplayer_info,
					       0);
    
    g_type_add_interface_static(ags_type_ffplayer,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_ffplayer);
  }

  return g_define_type_id__volatile;
}

void
ags_ffplayer_class_init(AgsFFPlayerClass *ffplayer)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  AgsMachineClass *machine;

  ags_ffplayer_parent_class = g_type_class_peek_parent(ffplayer);

  /* GObjectClass */
  gobject = (GObjectClass *) ffplayer;

  gobject->finalize = ags_ffplayer_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) ffplayer;

  widget->realize = ags_ffplayer_realize;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) ffplayer;

  machine->map_recall = ags_ffplayer_map_recall;
}

void
ags_ffplayer_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_ffplayer_connectable_parent_interface;

  ags_ffplayer_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_ffplayer_connect;
  connectable->disconnect = ags_ffplayer_disconnect;
}

void
ags_ffplayer_init(AgsFFPlayer *ffplayer)
{
  GtkVBox *vbox;
  GtkAlignment *alignment;
  GtkTable *table;
  GtkHBox *hbox;
  GtkHBox *filename_hbox;
  GtkVBox *piano_vbox;
  GtkLabel *label;
  
  PangoAttrList *attr_list;
  PangoAttribute *attr;

  GtkAllocation allocation;
  
  AgsAudio *audio;
  
  AgsConfig *config;

  gchar *str;
  
  gdouble gui_scale_factor;

  g_signal_connect_after((GObject *) ffplayer, "parent_set",
			 G_CALLBACK(ags_ffplayer_parent_set_callback), (gpointer) ffplayer);

  audio = AGS_MACHINE(ffplayer)->audio;
  ags_audio_set_flags(audio, (AGS_AUDIO_SYNC |
			      AGS_AUDIO_ASYNC |
			      AGS_AUDIO_OUTPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_FILE));
  ags_audio_set_ability_flags(audio, (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION));
  ags_audio_set_behaviour_flags(audio, (AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING |
					AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT));

  config = ags_config_get_instance();
  
  /* scale factor */
  gui_scale_factor = 1.0;
  
  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "gui-scale");

  if(str != NULL){
    gui_scale_factor = g_ascii_strtod(str,
				      NULL);

    g_free(str);
  }

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
  
  AGS_MACHINE(ffplayer)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
				   AGS_MACHINE_REVERSE_NOTATION);
  AGS_MACHINE(ffplayer)->file_input_flags |= AGS_MACHINE_ACCEPT_SOUNDFONT2;

  AGS_MACHINE(ffplayer)->input_pad_type = G_TYPE_NONE;
  AGS_MACHINE(ffplayer)->input_line_type = G_TYPE_NONE;
  AGS_MACHINE(ffplayer)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(ffplayer)->output_line_type = G_TYPE_NONE;

  /* context menu */
  ags_machine_popup_add_connection_options((AgsMachine *) ffplayer,
  					   (AGS_MACHINE_POPUP_MIDI_DIALOG));

  /* audio resize */
  g_signal_connect_after(G_OBJECT(ffplayer), "resize-audio-channels",
			 G_CALLBACK(ags_ffplayer_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(ffplayer), "resize-pads",
			 G_CALLBACK(ags_ffplayer_resize_pads), NULL);

  /* flags */
  ffplayer->flags = 0;

  /* mapped IO */
  ffplayer->mapped_input_pad = 0;
  ffplayer->mapped_output_pad = 0;

  /* context menu */
  ags_machine_popup_add_edit_options((AgsMachine *) ffplayer,
				     (AGS_MACHINE_POPUP_ENVELOPE));
  
  /* name and xml type */
  ffplayer->name = NULL;
  ffplayer->xml_type = "ags-ffplayer";

  /* audio container */
  ffplayer->audio_container = NULL;

  /* create widgets */
  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_container_add((GtkContainer *) (gtk_bin_get_child((GtkBin *) ffplayer)),
		    (GtkWidget *) vbox);

  alignment = (GtkAlignment *) g_object_new(GTK_TYPE_ALIGNMENT,
					    "xalign", 0.0,
					    NULL);
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) alignment,
		     FALSE, FALSE,
		     0);
  
  table = (GtkTable *) gtk_table_new(3, 2, FALSE);
  gtk_container_add((GtkContainer *) alignment,
		    (GtkWidget *) table);
  
  /* preset and instrument */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_table_attach(table,
		   GTK_WIDGET(hbox),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("preset"),
				    "xalign", 0.0,
				    NULL);
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  ffplayer->preset = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(ffplayer->preset),
		     TRUE, FALSE,
		     0);

  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("instrument"),
				    "xalign", 0.0,
				    NULL);
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  ffplayer->instrument = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(ffplayer->instrument),
		     TRUE, FALSE,
		     0);

  /* filename */
  filename_hbox = (GtkHBox *) gtk_hbox_new(FALSE,
					   0);
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(filename_hbox),
		     FALSE, FALSE,
		     0);

#if 0
  ffplayer->filename = (GtkEntry *) gtk_entry_new();
  gtk_box_pack_start((GtkBox *) filename_hbox,
		     (GtkWidget *) ffplayer->filename,
		     FALSE, FALSE,
		     0);
#else
  ffplayer->filename = NULL;
#endif
  
  ffplayer->open = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_OPEN);
  gtk_box_pack_start((GtkBox *) filename_hbox,
		     (GtkWidget *) ffplayer->open,
		     FALSE, FALSE,
		     0);

  ffplayer->sf2_loader = NULL;

  ffplayer->position = -1;

  ffplayer->loading = (GtkLabel *) gtk_label_new(i18n("loading ...  "));
  gtk_box_pack_start((GtkBox *) filename_hbox,
		     (GtkWidget *) ffplayer->loading,
		     FALSE, FALSE,
		     0);
  gtk_widget_set_no_show_all((GtkWidget *) ffplayer->loading,
			     TRUE);
  gtk_widget_hide((GtkWidget *) ffplayer->loading);

  /* piano */
  piano_vbox = (GtkVBox *) gtk_vbox_new(FALSE, 2);
  gtk_table_attach(table, (GtkWidget *) piano_vbox,
		   1, 2,
		   0, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  ffplayer->control_width = (guint) (gui_scale_factor * AGS_FFPLAYER_DEFAULT_CONTROL_WIDTH);
  ffplayer->control_height = (guint) (gui_scale_factor * AGS_FFPLAYER_DEFAULT_CONTROL_HEIGHT);

  ffplayer->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_size_request((GtkWidget *) ffplayer->drawing_area,
			      16 * ffplayer->control_width,
			      ffplayer->control_width * 8 + ffplayer->control_height);
  gtk_widget_set_events((GtkWidget *) ffplayer->drawing_area,
			GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK);
  gtk_box_pack_start((GtkBox *) piano_vbox,
		     (GtkWidget *) ffplayer->drawing_area,
		     FALSE, FALSE,
		     0);

  gtk_widget_get_allocation(GTK_WIDGET(ffplayer->drawing_area), &allocation);
  
  ffplayer->hadjustment = (GtkAdjustment *) gtk_adjustment_new(0.0,
							       0.0,
							       76 * ffplayer->control_width - allocation.width,
							       1.0,
							       (double) ffplayer->control_width,
							       (double) (16 * ffplayer->control_width));
  ffplayer->hscrollbar = (GtkHScrollbar *) gtk_hscrollbar_new(ffplayer->hadjustment);
  gtk_box_pack_start((GtkBox *) piano_vbox,
		     (GtkWidget *) ffplayer->hscrollbar,
		     FALSE, FALSE,
		     0);

  /* effect bridge */
  AGS_MACHINE(ffplayer)->bridge = (GtkContainer *) ags_ffplayer_bridge_new(audio);
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) AGS_MACHINE(ffplayer)->bridge,
		     FALSE, FALSE,
		     0);

  /* dialog */
  ffplayer->open_dialog = NULL;

  /* SF2 loader */
  if(ags_ffplayer_sf2_loader_completed == NULL){
    ags_ffplayer_sf2_loader_completed = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							      NULL,
							      NULL);
  }

  g_hash_table_insert(ags_ffplayer_sf2_loader_completed,
		      ffplayer, ags_ffplayer_sf2_loader_completed_timeout);
  g_timeout_add(1000 / 4, (GSourceFunc) ags_ffplayer_sf2_loader_completed_timeout, (gpointer) ffplayer);
}

void
ags_ffplayer_finalize(GObject *gobject)
{
  g_hash_table_remove(ags_ffplayer_sf2_loader_completed,
		      gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_ffplayer_parent_class)->finalize(gobject);
}

void
ags_ffplayer_connect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsFFPlayer *ffplayer;

  GList *list;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_ffplayer_parent_connectable_interface->connect(connectable);

  ffplayer = AGS_FFPLAYER(connectable);

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) ffplayer);
  
  g_signal_connect((GObject *) ffplayer, "destroy",
		   G_CALLBACK(ags_ffplayer_destroy_callback), (gpointer) ffplayer);
  
  /* AgsFFPlayer */
  g_signal_connect((GObject *) ffplayer->open, "clicked",
		   G_CALLBACK(ags_ffplayer_open_clicked_callback), (gpointer) ffplayer);

  g_signal_connect_after((GObject *) ffplayer->preset, "changed",
			 G_CALLBACK(ags_ffplayer_preset_changed_callback), (gpointer) ffplayer);

  g_signal_connect_after((GObject *) ffplayer->instrument, "changed",
			 G_CALLBACK(ags_ffplayer_instrument_changed_callback), (gpointer) ffplayer);


  g_signal_connect((GObject *) ffplayer->drawing_area, "draw",
                   G_CALLBACK(ags_ffplayer_draw_callback), (gpointer) ffplayer);

  g_signal_connect((GObject *) ffplayer->drawing_area, "button_press_event",
                   G_CALLBACK(ags_ffplayer_drawing_area_button_press_callback), (gpointer) ffplayer);

  g_signal_connect((GObject *) ffplayer->hadjustment, "value_changed",
		   G_CALLBACK(ags_ffplayer_hscrollbar_value_changed), (gpointer) ffplayer);
}

void
ags_ffplayer_disconnect(AgsConnectable *connectable)
{
  AgsFFPlayer *ffplayer;

  GList *list;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  ags_ffplayer_parent_connectable_interface->disconnect(connectable);

  ffplayer = AGS_FFPLAYER(connectable);

  g_object_disconnect((GObject *) ffplayer,
		      "any_signal::destroy",
		      G_CALLBACK(ags_ffplayer_destroy_callback),
		      (gpointer) ffplayer,
		      NULL);
  
  /* AgsFFPlayer */
  g_object_disconnect((GObject *) ffplayer->open,
		      "any_signal::clicked",
		      G_CALLBACK(ags_ffplayer_open_clicked_callback),
		      (gpointer) ffplayer,
		      NULL);

  g_object_disconnect((GObject *) ffplayer->preset,
		      "any_signal::changed",
		      G_CALLBACK(ags_ffplayer_preset_changed_callback),
		      (gpointer) ffplayer,
		      NULL);

  g_object_disconnect((GObject *) ffplayer->instrument,
		      "any_signal::changed",
		      G_CALLBACK(ags_ffplayer_instrument_changed_callback),
		      (gpointer) ffplayer,
		      NULL);

  g_object_disconnect((GObject *) ffplayer->drawing_area,
		      "any_signal::button_press_event",
		      G_CALLBACK(ags_ffplayer_drawing_area_button_press_callback),
		      (gpointer) ffplayer,
		      NULL);

  g_object_disconnect((GObject *) ffplayer->hadjustment,
		      "any_signal::value_changed",
		      G_CALLBACK(ags_ffplayer_hscrollbar_value_changed),
		      (gpointer) ffplayer,
		      NULL);
}

void
ags_ffplayer_realize(GtkWidget *widget)
{
  AgsFFPlayer *ffplayer;

  ffplayer = (AgsFFPlayer *) widget;
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_ffplayer_parent_class)->realize(widget);

  if(ffplayer_style == NULL){
    ffplayer_style = gtk_style_copy(gtk_widget_get_style((GtkWidget *) ffplayer));
  }
  
  gtk_widget_set_style((GtkWidget *) ffplayer->drawing_area,
		       NULL);

  gtk_widget_set_style((GtkWidget *) ffplayer->hscrollbar,
		       NULL);
}

void
ags_ffplayer_resize_audio_channels(AgsMachine *machine,
				   guint audio_channels, guint audio_channels_old,
				   gpointer data)
{
  AgsAudio *audio;  
  AgsChannel *start_output, *start_input;
  AgsChannel *channel, *next_pad, *next_channel, *nth_channel;
  
  GList *start_play, *play;

  guint output_pads, input_pads;
  guint i, j;

  audio = machine->audio;

  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "output-pads", &output_pads,
	       "output", &start_output,
	       "input", &start_input,
	       NULL);

  /*  */
  if(audio_channels > audio_channels_old){  
    /* AgsOutput */
    channel = start_output;

    g_object_ref(channel);

    next_channel = NULL;
    
    while(channel != NULL){
      /* get some fields */
      next_pad = ags_channel_next_pad(channel);

      nth_channel = ags_channel_pad_nth(channel,
					audio_channels_old);

      g_object_unref(channel);
      
      channel = nth_channel;
      
      while(channel != next_pad){
	AgsRecycling *recycling;
	AgsAudioSignal *audio_signal;

	GObject *output_soundcard;

	ags_channel_set_ability_flags(channel, (AGS_SOUND_ABILITY_NOTATION));
		
	g_object_get(audio,
		     "output-soundcard", &output_soundcard,
		     NULL);

	/* get recycling */
	g_object_get(channel,
		     "first-recycling", &recycling,
		     NULL);

	/* instantiate template audio signal */
	audio_signal = ags_audio_signal_new(output_soundcard,
					    (GObject *) recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	ags_recycling_add_audio_signal(recycling,
				       audio_signal);

	g_object_unref(output_soundcard);

	g_object_unref(recycling);
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      if(next_pad != NULL){
	g_object_unref(next_pad);
      }
    }

    if(next_channel != NULL){
      g_object_unref(next_channel);
    }

    /* ags-envelope */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-envelope",
			      audio_channels_old, audio_channels, 
			      0, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_PLAY |
			       AGS_RECALL_FACTORY_RECALL | 
			       AGS_RECALL_FACTORY_ADD),
			      0);

    /* AgsInput */
    channel = start_input;

    g_object_ref(channel);

    while(channel != NULL){
      /* get some fields */
      next_pad = ags_channel_next_pad(channel);

      nth_channel = ags_channel_nth(channel,
				    audio_channels_old);

      g_object_unref(channel);

      channel = nth_channel;
      
      next_channel = NULL;

      while(channel != next_pad){
	AgsPort *port;
      
	GList *start_play, *play;
	GList *start_recall, *recall;

	g_object_get(channel,
		     "play", &start_play,
		     "recall", &start_recall,
		     NULL);

	/* play */
	play = ags_recall_find_type(start_play,
				    AGS_TYPE_ENVELOPE_CHANNEL);
      
	if(play != NULL){
	  GValue value = {0};
      
	  g_object_get(play->data,
		       "use-note-length", &port,
		       NULL);

	  g_value_init(&value,
		       G_TYPE_BOOLEAN);
	  g_value_set_boolean(&value,
			      TRUE);

	  ags_port_safe_write(port,
			      &value);
	
	  g_object_unref(port);
	}

	g_list_free(start_play);

	/* recall */
	recall = ags_recall_find_type(start_recall,
				      AGS_TYPE_ENVELOPE_CHANNEL);
      
	if(recall != NULL){
	  GValue value = {0};
      
	  g_object_get(recall->data,
		       "use-note-length", &port,
		       NULL);

	  g_value_init(&value,
		       G_TYPE_BOOLEAN);
	  g_value_set_boolean(&value,
			      TRUE);

	  ags_port_safe_write(port,
			      &value);
	
	  g_object_unref(port);
	}

	g_list_free(start_recall);

	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      if(next_pad != NULL){
	g_object_unref(next_pad);
      }
    }

    if(next_channel != NULL){
      g_object_unref(next_channel);
    }

    if(ags_recall_global_get_rt_safe() ||
       ags_recall_global_get_performance_mode()){
      /* ags-copy */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-copy",
				audio_channels_old, audio_channels,
				0, input_pads,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_RECALL |
				 AGS_RECALL_FACTORY_ADD),
				0);
    }else{
      /* ags-buffer */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-buffer",
				audio_channels_old, audio_channels,
				0, input_pads,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_RECALL |
				 AGS_RECALL_FACTORY_ADD),
				0);
    }
    
    for(i = 0; i < input_pads; i++){
      for(j = audio_channels_old; j < audio_channels; j++){
	AgsPlayChannelRun *play_channel_run;
	AgsStreamChannelRun *stream_channel_run;

	channel = ags_channel_nth(start_input,
				  i * audio_channels + j);
	
	/* ags-play */
	ags_recall_factory_create(audio,
				  NULL, NULL,
				  "ags-play",
				  j, j + 1, 
				  i, i + 1,
				  (AGS_RECALL_FACTORY_INPUT |
				   AGS_RECALL_FACTORY_PLAY |
				   AGS_RECALL_FACTORY_ADD),
				  0);

	/* ags-stream */
	if(!(ags_recall_global_get_rt_safe() ||
	     ags_recall_global_get_performance_mode())){
	  ags_recall_factory_create(audio,
				    NULL, NULL,
				    "ags-stream",
				    j, j + 1, 
				    i, i + 1,
				    (AGS_RECALL_FACTORY_INPUT |
				     AGS_RECALL_FACTORY_PLAY |
				     AGS_RECALL_FACTORY_RECALL | 
				     AGS_RECALL_FACTORY_ADD),
				    0);

	  /* set up dependencies */
	  g_object_get(channel,
		       "play", &start_play,
		       NULL);
    
	  play = ags_recall_find_type(start_play,
				      AGS_TYPE_PLAY_CHANNEL_RUN);
	  play_channel_run = AGS_PLAY_CHANNEL_RUN(play->data);

	  play = ags_recall_find_type(start_play,
				      AGS_TYPE_STREAM_CHANNEL_RUN);
	  stream_channel_run = AGS_STREAM_CHANNEL_RUN(play->data);

	  g_object_set(G_OBJECT(play_channel_run),
		       "stream-channel-run", stream_channel_run,
		       NULL);

	  g_list_free_full(start_play,
			   g_object_unref);
	}else{
	  ags_recall_factory_create(audio,
				    NULL, NULL,
				    "ags-rt-stream",
				    j, j + 1, 
				    i, i + 1,
				    (AGS_RECALL_FACTORY_INPUT |
				     AGS_RECALL_FACTORY_PLAY |
				     AGS_RECALL_FACTORY_RECALL | 
				     AGS_RECALL_FACTORY_ADD),
				    0);
	}

	g_object_unref(channel);
      }
    }
    
    /* AgsOutput */
    /* ags-stream */
    if(!(ags_recall_global_get_rt_safe() ||
	 ags_recall_global_get_performance_mode())){
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-stream",
				audio_channels_old, audio_channels,
				0, audio->output_pads,
				(AGS_RECALL_FACTORY_OUTPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_ADD),
				0);
    }
  }

  if(start_output != NULL){
    g_object_unref(start_output);
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

void
ags_ffplayer_resize_pads(AgsMachine *machine, GType channel_type,
			 guint pads, guint pads_old,
			 gpointer data)
{
  AgsFFPlayer *ffplayer;

  AgsAudio *audio;
  AgsChannel *start_output;
  AgsChannel *channel, *next_channel, *nth_channel;

  guint output_pads, input_pads;
  gboolean grow;
  
  if(pads_old == pads){
    return;
  }
    
  ffplayer = (AgsFFPlayer *) machine;

  audio = machine->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       "input-pads", &input_pads,
	       "output", &start_output,
	       NULL);

  /* check grow */
  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(channel_type == AGS_TYPE_INPUT){
    if(grow){
      /* depending on destination */
      ags_ffplayer_input_map_recall(ffplayer, pads_old);
    }else{
      ffplayer->mapped_input_pad = pads;
    }
  }else if(channel_type == AGS_TYPE_OUTPUT){
    if(grow){
      /* AgsOutput */
      nth_channel = ags_channel_pad_nth(start_output,
					pads_old);

      channel = nth_channel;
      
      next_channel = NULL;
      
      while(channel != NULL){
	AgsRecycling *recycling;
	AgsAudioSignal *audio_signal;

	GObject *output_soundcard;

	ags_channel_set_ability_flags(channel, (AGS_SOUND_ABILITY_NOTATION));

	g_object_get(audio,
		     "output-soundcard", &output_soundcard,
		     NULL);

	/* get recycling */
	g_object_get(channel,
		     "first-recycling", &recycling,
		     NULL);

	/* instantiate template audio signal */
	audio_signal = ags_audio_signal_new(output_soundcard,
					    (GObject *) recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	ags_recycling_add_audio_signal(recycling,
				       audio_signal);

	g_object_unref(output_soundcard);

	g_object_unref(recycling);
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      if(next_channel != NULL){
	g_object_unref(next_channel);
      }
      
      /* depending on destination */
      ags_ffplayer_output_map_recall(ffplayer, pads_old);
    }else{
      ffplayer->mapped_output_pad = pads;
    }
  }else{
    g_critical("unknown channel type");
  }

  if(start_output != NULL){
    g_object_unref(start_output);
  }
}

void
ags_ffplayer_map_recall(AgsMachine *machine)
{
  AgsWindow *window;
  AgsFFPlayer *ffplayer;
  
  AgsAudio *audio;

  AgsDelayAudio *play_delay_audio;
  AgsDelayAudioRun *play_delay_audio_run;
  AgsCountBeatsAudio *play_count_beats_audio;
  AgsCountBeatsAudioRun *play_count_beats_audio_run;
  AgsRecordMidiAudio *recall_record_midi_audio;
  AgsRecordMidiAudioRun *recall_record_midi_audio_run;
  AgsPlayNotationAudio *recall_notation_audio;
  AgsPlayNotationAudioRun *recall_notation_audio_run;

  GList *start_play, *play;
  GList *start_recall, *recall;
  
  GValue value = {0,};

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  ffplayer = AGS_FFPLAYER(machine);
  
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) machine,
						 AGS_TYPE_WINDOW);

  audio = machine->audio;
  
  /* ags-delay */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-delay",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY),
			    0);

  g_object_get(audio,
	       "play", &start_play,
	       NULL);

  play = ags_recall_find_type(start_play,
			      AGS_TYPE_DELAY_AUDIO_RUN);

  if(play != NULL){
    play_delay_audio_run = AGS_DELAY_AUDIO_RUN(play->data);
    //    AGS_RECALL(play_delay_audio_run)->flags |= AGS_RECALL_PERSISTENT;
  }else{
    play_delay_audio_run = NULL;
  }

  g_list_free_full(start_play,
		   g_object_unref);
  
  /* ags-count-beats */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-count-beats",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY),
			    0);
  
  g_object_get(audio,
	       "play", &start_play,
	       NULL);

  play = ags_recall_find_type(start_play,
			      AGS_TYPE_COUNT_BEATS_AUDIO_RUN);

  if(play != NULL){
    play_count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(play->data);

    /* set dependency */  
    g_object_set(G_OBJECT(play_count_beats_audio_run),
		 "delay-audio-run", play_delay_audio_run,
		 NULL);
    ags_seekable_seek(AGS_SEEKABLE(play_count_beats_audio_run),
		      (gint64) 16 * gtk_spin_button_get_value(window->navigation->position_tact),
		      AGS_SEEK_SET);

    /* notation loop */
    g_value_init(&value, G_TYPE_BOOLEAN);
    g_value_set_boolean(&value, gtk_toggle_button_get_active((GtkToggleButton *) window->navigation->loop));
    ags_port_safe_write(AGS_COUNT_BEATS_AUDIO(AGS_RECALL_AUDIO_RUN(play_count_beats_audio_run)->recall_audio)->notation_loop,
			&value);

    g_value_unset(&value);
    g_value_init(&value, G_TYPE_UINT64);

    g_value_set_uint64(&value, 16 * gtk_spin_button_get_value(window->navigation->loop_left_tact));
    ags_port_safe_write(AGS_COUNT_BEATS_AUDIO(AGS_RECALL_AUDIO_RUN(play_count_beats_audio_run)->recall_audio)->notation_loop_start,
			&value);

    g_value_reset(&value);

    g_value_set_uint64(&value, 16 * gtk_spin_button_get_value(window->navigation->loop_right_tact));
    ags_port_safe_write(AGS_COUNT_BEATS_AUDIO(AGS_RECALL_AUDIO_RUN(play_count_beats_audio_run)->recall_audio)->notation_loop_end,
			&value);
  }else{
    play_count_beats_audio_run = NULL;
  }

  g_list_free_full(start_play,
		   g_object_unref);

  /* ags-record-midi */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-record-midi",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY),
			    0);

  g_object_get(audio,
	       "play", &start_play,
	       NULL);

  play = ags_recall_find_type(start_play,
			      AGS_TYPE_RECORD_MIDI_AUDIO_RUN);

  if(play != NULL){
    recall_record_midi_audio_run = AGS_RECORD_MIDI_AUDIO_RUN(play->data);
    
    /* set dependency */
    g_object_set(G_OBJECT(recall_record_midi_audio_run),
		 "delay-audio-run", play_delay_audio_run,
		 NULL);

    /* set dependency */
    g_object_set(G_OBJECT(recall_record_midi_audio_run),
		 "count-beats-audio-run", play_count_beats_audio_run,
		 NULL);
  }  

  g_list_free_full(start_play,
		   g_object_unref);

  /* ags-play-notation */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-play-notation",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY),
			    0);

  g_object_get(audio,
	       "play", &start_play,
	       NULL);

  play = ags_recall_find_type(start_play,
			      AGS_TYPE_PLAY_NOTATION_AUDIO_RUN);

  if(play != NULL){
    recall_notation_audio_run = AGS_PLAY_NOTATION_AUDIO_RUN(play->data);
    
    /* set dependency */
    g_object_set(G_OBJECT(recall_notation_audio_run),
		 "delay-audio-run", play_delay_audio_run,
		 NULL);

    /* set dependency */
    g_object_set(G_OBJECT(recall_notation_audio_run),
		 "count-beats-audio-run", play_count_beats_audio_run,
		 NULL);
  }

  g_list_free_full(start_play,
		   g_object_unref);

  /* depending on destination */
  ags_ffplayer_input_map_recall(ffplayer, 0);

  /* depending on destination */
  ags_ffplayer_output_map_recall(ffplayer, 0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_ffplayer_parent_class)->map_recall(machine);  
}

void
ags_ffplayer_input_map_recall(AgsFFPlayer *ffplayer, guint input_pad_start)
{
  AgsAudio *audio;
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel, *nth_channel;

  GList *start_play, *play;

  guint input_pads;
  guint audio_channels;
  guint i, j;

  if(ffplayer->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(ffplayer)->audio;

  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       "input", &start_input,
	       NULL);

  /* ags-envelope */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-envelope",
			    0, audio_channels,
			    input_pad_start, input_pads,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL | 
			     AGS_RECALL_FACTORY_ADD),
			    0);

  nth_channel = ags_channel_pad_nth(start_input,
				    input_pad_start);

  channel = nth_channel;

  next_channel = NULL;
  
  while(channel != NULL){
    AgsPort *port;
      
    GList *start_play, *play;
    GList *start_recall, *recall;

    g_object_get(channel,
		 "play", &start_play,
		 "recall", &start_recall,
		 NULL);

    /* play */
    play = ags_recall_find_type(start_play,
				AGS_TYPE_ENVELOPE_CHANNEL);
      
    if(play != NULL){
      GValue value = {0};
      
      g_object_get(play->data,
		   "use-note-length", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_BOOLEAN);
      g_value_set_boolean(&value,
			  TRUE);

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_play);

    /* recall */
    recall = ags_recall_find_type(start_recall,
				  AGS_TYPE_ENVELOPE_CHANNEL);
      
    if(recall != NULL){
      GValue value = {0};
      
      g_object_get(recall->data,
		   "use-note-length", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_BOOLEAN);
      g_value_set_boolean(&value,
			  TRUE);

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_recall);

    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }

  if(next_channel != NULL){
    g_object_unref(next_channel);
  }
  
  /* remap for input */
  if(ags_recall_global_get_rt_safe() ||
     ags_recall_global_get_performance_mode()){
    /* ags-copy */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-copy",
			      0, audio_channels,
			      input_pad_start, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_RECALL |
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }else{
    /* ags-buffer */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-buffer",
			      0, audio_channels,
			      input_pad_start, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_RECALL |
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }
  
  for(i = input_pad_start; i < input_pads; i++){
    for(j = 0; j < audio_channels; j++){
      AgsPlayChannelRun *play_channel_run;
      AgsStreamChannelRun *stream_channel_run;

      channel = ags_channel_nth(start_input,
				i * audio_channels + j);
      /* ags-play */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-play",
				j, j + 1, 
				i, i + 1,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_ADD),
				0);

      /* ags-feed */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-feed",
				j, j + 1, 
				i, i + 1,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_RECALL | 
				 AGS_RECALL_FACTORY_ADD),
				0);

      /* ags-stream */
      if(!(ags_recall_global_get_rt_safe() ||
	   ags_recall_global_get_performance_mode())){
	ags_recall_factory_create(audio,
				  NULL, NULL,
				  "ags-stream",
				  j, j + 1, 
				  i, i + 1,
				  (AGS_RECALL_FACTORY_INPUT |
				   AGS_RECALL_FACTORY_PLAY |
				   AGS_RECALL_FACTORY_RECALL | 
				   AGS_RECALL_FACTORY_ADD),
				  0);

	/* set up dependencies */
	g_object_get(channel,
		     "play", &start_play,
		     NULL);
    
	play = ags_recall_find_type(start_play,
				    AGS_TYPE_PLAY_CHANNEL_RUN);
	play_channel_run = AGS_PLAY_CHANNEL_RUN(play->data);

	play = ags_recall_find_type(start_play,
				    AGS_TYPE_STREAM_CHANNEL_RUN);
	stream_channel_run = AGS_STREAM_CHANNEL_RUN(play->data);

	g_object_set(G_OBJECT(play_channel_run),
		     "stream-channel-run", stream_channel_run,
		     NULL);

	g_list_free_full(start_play,
			 g_object_unref);
      }else{
	ags_recall_factory_create(audio,
				  NULL, NULL,
				  "ags-rt-stream",
				  j, j + 1, 
				  i, i + 1,
				  (AGS_RECALL_FACTORY_INPUT |
				   AGS_RECALL_FACTORY_PLAY |
				   AGS_RECALL_FACTORY_RECALL | 
				   AGS_RECALL_FACTORY_ADD),
				  0);
      }

      g_object_unref(channel);
    }
  }
  
  ffplayer->mapped_input_pad = input_pads;

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

void
ags_ffplayer_output_map_recall(AgsFFPlayer *ffplayer, guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads, input_pads;
  guint audio_channels;

  if(ffplayer->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(ffplayer)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);
  
  /* remap for input */
  if(ags_recall_global_get_rt_safe() ||
     ags_recall_global_get_performance_mode()){
    /* ags-copy */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-copy",
			      0, audio_channels, 
			      0, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_RECALL |
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }else{
    /* ags-buffer */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-buffer",
			      0, audio_channels, 
			      0, input_pads,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_RECALL |
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }

  if(!(ags_recall_global_get_rt_safe() ||
       ags_recall_global_get_performance_mode())){
    /* ags-stream */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-stream",
			      0, audio_channels,
			      output_pad_start, output_pads,
			      (AGS_RECALL_FACTORY_OUTPUT |
			       AGS_RECALL_FACTORY_PLAY |
			       AGS_RECALL_FACTORY_ADD),
			      0);
  }
  
  ffplayer->mapped_output_pad = output_pads;
}

void
ags_ffplayer_open_filename(AgsFFPlayer *ffplayer,
			   gchar *filename)
{
  AgsSF2Loader *sf2_loader;

  if(!AGS_IS_FFPLAYER(ffplayer) ||
     filename == NULL){
    return;
  }
  
  ffplayer->sf2_loader = 
    sf2_loader = ags_sf2_loader_new(AGS_MACHINE(ffplayer)->audio,
				    filename,
				    NULL,
				    NULL);

  ags_sf2_loader_start(sf2_loader);
}

void
ags_ffplayer_load_preset(AgsFFPlayer *ffplayer)
{
  AgsAudioContainer *audio_container;
  
  gchar **preset;

  if(!AGS_IS_FFPLAYER(ffplayer) ||
     ffplayer->audio_container == NULL ||
     ffplayer->audio_container->sound_container == NULL){
    return;
  }

  audio_container = ffplayer->audio_container;

  /* select first preset */
  preset = ags_ipatch_sf2_reader_get_preset_all(AGS_IPATCH_SF2_READER(AGS_IPATCH(audio_container->sound_container)->reader));

  /* fill ffplayer->preset */
  while(preset != NULL && preset[0] != NULL){
    gtk_combo_box_text_append_text(ffplayer->preset,
				   preset[0]);
    
    preset++;
  }

  ags_ffplayer_load_instrument(ffplayer);
}

void
ags_ffplayer_load_instrument(AgsFFPlayer *ffplayer)
{
  AgsAudioContainer *audio_container;
  
  gchar **instrument;

  gint position;
  
  if(!AGS_IS_FFPLAYER(ffplayer) ||
     ffplayer->audio_container == NULL ||
     ffplayer->audio_container->sound_container == NULL){
    return;
  }

  audio_container = ffplayer->audio_container;
  
  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(ffplayer->instrument))));

  position = gtk_combo_box_get_active(GTK_COMBO_BOX(ffplayer->preset));

  if(position != -1){
    instrument = ags_ipatch_sf2_reader_get_instrument_by_preset_index(AGS_IPATCH_SF2_READER(AGS_IPATCH(audio_container->sound_container)->reader),
								      position);
  }else{
    instrument = NULL;
  }

  /* fill ffplayer->instrument */
  while(instrument != NULL && instrument[0] != NULL){
    gtk_combo_box_text_append_text(ffplayer->instrument,
				   instrument[0]);

    instrument++;
  }
}

/**
 * ags_ffplayer_sf2_loader_completed_timeout:
 * @ffplayer: the #AgsFFPlayer
 *
 * Queue draw widget
 *
 * Returns: %TRUE if proceed poll completed, otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_ffplayer_sf2_loader_completed_timeout(AgsFFPlayer *ffplayer)
{
  if(g_hash_table_lookup(ags_ffplayer_sf2_loader_completed,
			 ffplayer) != NULL){
    if(ffplayer->sf2_loader != NULL){
      if(ags_sf2_loader_test_flags(ffplayer->sf2_loader, AGS_SF2_LOADER_HAS_COMPLETED)){
	/* reassign audio container */
	ffplayer->audio_container = ffplayer->sf2_loader->audio_container;
	ffplayer->sf2_loader->audio_container = NULL;

	/* clear preset and instrument */
	gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(ffplayer->preset))));
	gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(ffplayer->instrument))));

	/* level select */
	if(ffplayer->audio_container->sound_container != NULL){
	  ags_sound_container_select_level_by_index(AGS_SOUND_CONTAINER(ffplayer->audio_container->sound_container), 0);
	  AGS_IPATCH(ffplayer->audio_container->sound_container)->nesting_level += 1;
    
	  ags_ffplayer_load_preset(ffplayer);
	}

	/* cleanup */	
	g_object_run_dispose((GObject *) ffplayer->sf2_loader);
	g_object_unref(ffplayer->sf2_loader);

	ffplayer->sf2_loader = NULL;

	ffplayer->position = -1;
	gtk_widget_hide((GtkWidget *) ffplayer->loading);

      }else{
	if(ffplayer->position == -1){
	  ffplayer->position = 0;

	  gtk_widget_show((GtkWidget *) ffplayer->loading);
	}

	switch(ffplayer->position){
	case 0:
	  {
	    ffplayer->position = 1;
	    
	    gtk_label_set_label(ffplayer->loading,
				"loading ...  ");
	  }
	  break;
	case 1:
	  {
	    ffplayer->position = 2;

	    gtk_label_set_label(ffplayer->loading,
				"loading  ... ");
	  }
	  break;
	case 2:
	  {
	    ffplayer->position = 0;

	    gtk_label_set_label(ffplayer->loading,
				"loading   ...");
	  }
	  break;
	}
      }
    }
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_ffplayer_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsFFPlayer
 *
 * Returns: the new #AgsFFPlayer
 *
 * Since: 3.0.0
 */
AgsFFPlayer*
ags_ffplayer_new(GObject *output_soundcard)
{
  AgsFFPlayer *ffplayer;

  ffplayer = (AgsFFPlayer *) g_object_new(AGS_TYPE_FFPLAYER,
					  NULL);

  if(output_soundcard != NULL){
    g_object_set(AGS_MACHINE(ffplayer)->audio,
		 "output-soundcard", output_soundcard,
		 NULL);
  }

  return(ffplayer);
}
