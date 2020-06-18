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
void ags_ffplayer_output_map_recall(AgsFFPlayer *ffplayer,
				    guint audio_channel_start,
				    guint output_pad_start);
void ags_ffplayer_input_map_recall(AgsFFPlayer *ffplayer,
				   guint audio_channel_start,
				   guint input_pad_start);

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
  GtkVBox *synth_generator_vbox;
  GtkAlignment *alignment;
  GtkTable *table;
  GtkHBox *hbox;
  GtkHBox *filename_hbox;
  GtkVBox *piano_vbox;
  GtkFrame *frame;
  GtkLabel *label;
  
  PangoAttrList *attr_list;
  PangoAttribute *attr;

  GtkAllocation allocation;
  
  AgsAudio *audio;
  AgsSF2SynthGenerator *sf2_synth_generator;
  
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

  sf2_synth_generator = ags_sf2_synth_generator_new();
  ags_sf2_synth_generator_set_flags(sf2_synth_generator,
				    AGS_SF2_SYNTH_GENERATOR_COMPUTE_INSTRUMENT);
  
  ags_audio_add_sf2_synth_generator(audio,
				    (GObject *) sf2_synth_generator);

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

  ffplayer->playback_play_container = ags_recall_container_new();
  ffplayer->playback_recall_container = ags_recall_container_new();

  ffplayer->notation_play_container = ags_recall_container_new();
  ffplayer->notation_recall_container = ags_recall_container_new();

  ffplayer->envelope_play_container = ags_recall_container_new();
  ffplayer->envelope_recall_container = ags_recall_container_new();

  ffplayer->buffer_play_container = ags_recall_container_new();
  ffplayer->buffer_recall_container = ags_recall_container_new();

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
  
  table = (GtkTable *) gtk_table_new(4, 2, FALSE);
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

  ffplayer->load_preset = NULL;
  ffplayer->load_instrument = NULL;

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

  /*  */
  frame = (GtkFrame *) gtk_frame_new("Synth Generator");
  gtk_table_attach(table, (GtkWidget *) frame,
		   2, 3,
		   0, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  synth_generator_vbox = (GtkVBox *) gtk_vbox_new(FALSE,
						  0);
  gtk_container_add((GtkContainer *) frame,
		    (GtkWidget *) synth_generator_vbox);
  
  ffplayer->synth_generator_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));
  gtk_box_pack_start((GtkBox *) synth_generator_vbox,
		     (GtkWidget *) ffplayer->synth_generator_enabled,
		     FALSE, FALSE,
		     0);

  ffplayer->lower = (GtkSpinButton *) gtk_spin_button_new_with_range(-70.0,
								     70.0,
								     1.0);
  gtk_spin_button_set_digits(ffplayer->lower,
			     2);
  gtk_spin_button_set_value(ffplayer->lower,
			    -48.0);
  gtk_box_pack_start((GtkBox *) synth_generator_vbox,
		     (GtkWidget *) ffplayer->lower,
		     FALSE, FALSE,
		     0);
  
  ffplayer->key_count = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									 128.0,
									 1.0);
  gtk_spin_button_set_value(ffplayer->key_count,
			    78.0);
  gtk_box_pack_start((GtkBox *) synth_generator_vbox,
		     (GtkWidget *) ffplayer->key_count,
		     FALSE, FALSE,
		     0);

  ffplayer->update = (GtkButton *) gtk_button_new_with_label(i18n("update"));
  gtk_widget_set_valign(ffplayer->update,
			GTK_ALIGN_END);
  gtk_table_attach(table, (GtkWidget *) ffplayer->update,
		   3, 4,
		   0, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
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
  g_object_disconnect(gobject,
		      "any_signal::resize-audio-channels",
		      G_CALLBACK(ags_ffplayer_resize_audio_channels),
		      NULL,
		      "any_signal::resize-pads",
		      G_CALLBACK(ags_ffplayer_resize_pads),
		      NULL,
		      NULL);

  g_hash_table_remove(ags_ffplayer_sf2_loader_completed,
		      gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_ffplayer_parent_class)->finalize(gobject);
}

void
ags_ffplayer_connect(AgsConnectable *connectable)
{
  AgsFFPlayer *ffplayer;

  GList *list;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_ffplayer_parent_connectable_interface->connect(connectable);

  ffplayer = AGS_FFPLAYER(connectable);
  
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
  AgsFFPlayer *ffplayer;

  ffplayer = (AgsFFPlayer *) machine;

  /*  */
  if(audio_channels > audio_channels_old){  
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_ffplayer_input_map_recall(ffplayer,
				    audio_channels_old,
				    0);

      ags_ffplayer_output_map_recall(ffplayer,
				     audio_channels_old,
				     0);
    }
  }
}

void
ags_ffplayer_resize_pads(AgsMachine *machine, GType channel_type,
			 guint pads, guint pads_old,
			 gpointer data)
{
  AgsFFPlayer *ffplayer;

  gboolean grow;
  
  if(pads_old == pads){
    return;
  }
    
  ffplayer = (AgsFFPlayer *) machine;

  /* check grow */
  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(g_type_is_a(channel_type, AGS_TYPE_INPUT)){
    if(grow){
      /* depending on destination */
      ags_ffplayer_input_map_recall(ffplayer,
				    0,
				    pads_old);
    }else{
      ffplayer->mapped_input_pad = pads;
    }
  }else if(g_type_is_a(channel_type, AGS_TYPE_OUTPUT)){
    if(grow){      
      /* depending on destination */
      ags_ffplayer_output_map_recall(ffplayer,
				     0,
				     pads_old);
    }else{
      ffplayer->mapped_output_pad = pads;
    }
  }else{
    g_critical("unknown channel type");
  }
}

void
ags_ffplayer_map_recall(AgsMachine *machine)
{
  AgsFFPlayer *ffplayer;
  
  AgsAudio *audio;

  GList *start_recall, *recall;
  gint position;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  ffplayer = AGS_FFPLAYER(machine);

  audio = machine->audio;
  
  position = 0;
  
  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       ffplayer->playback_play_container, ffplayer->playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-notation */
  start_recall = ags_fx_factory_create(audio,
				       ffplayer->notation_play_container, ffplayer->notation_recall_container,
				       "ags-fx-notation",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-envelope */
  start_recall = ags_fx_factory_create(audio,
				       ffplayer->envelope_play_container, ffplayer->envelope_recall_container,
				       "ags-fx-envelope",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-buffer */
  start_recall = ags_fx_factory_create(audio,
				       ffplayer->buffer_play_container, ffplayer->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* depending on destination */
  ags_ffplayer_input_map_recall(ffplayer,
				0,
				0);

  /* depending on destination */
  ags_ffplayer_output_map_recall(ffplayer,
				 0,
				 0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_ffplayer_parent_class)->map_recall(machine);  
}

void
ags_ffplayer_input_map_recall(AgsFFPlayer *ffplayer,
			      guint audio_channel_start,
			      guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(ffplayer->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(ffplayer)->audio;

  position = 0;

  input_pads = 0;
  audio_channels = 0;
  
  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);

  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       ffplayer->playback_play_container, ffplayer->playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-notation */
  start_recall = ags_fx_factory_create(audio,
				       ffplayer->notation_play_container, ffplayer->notation_recall_container,
				       "ags-fx-notation",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-envelope */
  start_recall = ags_fx_factory_create(audio,
				       ffplayer->envelope_play_container, ffplayer->envelope_recall_container,
				       "ags-fx-envelope",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-buffer */
  start_recall = ags_fx_factory_create(audio,
				       ffplayer->buffer_play_container, ffplayer->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  ffplayer->mapped_input_pad = input_pads;
}

void
ags_ffplayer_output_map_recall(AgsFFPlayer *ffplayer,
			       guint audio_channel_start,
			       guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;

  if(ffplayer->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(ffplayer)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);
  
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

void
ags_ffplayer_update(AgsFFPlayer *ffplayer)
{
  //TODO:JK: implement me
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

	  if(ffplayer->load_preset != NULL){
	    GtkTreeModel *model;
	    GtkTreeIter iter;

	    gchar *value;

	    /* preset */
	    model = gtk_combo_box_get_model(GTK_COMBO_BOX(ffplayer->preset));

	    if(gtk_tree_model_get_iter_first(model, &iter)){
	      do{
		gtk_tree_model_get(model, &iter,
				   0, &value,
				   -1);

		if(!g_strcmp0(ffplayer->load_preset,
			      value)){
		  gtk_combo_box_set_active_iter((GtkComboBox *) ffplayer->preset,
						&iter);
		  break;
		}
	      }while(gtk_tree_model_iter_next(model,
					      &iter));
	    }

	    g_free(ffplayer->load_preset);

	    ffplayer->load_preset = NULL;
	  }

	  if(ffplayer->load_instrument != NULL){
	    GtkTreeModel *model;
	    GtkTreeIter iter;

	    gchar *value;

	    /* instrument */
	    model = gtk_combo_box_get_model(GTK_COMBO_BOX(ffplayer->instrument));

	    if(gtk_tree_model_get_iter_first(model, &iter)){
	      do{
		gtk_tree_model_get(model, &iter,
				   0, &value,
				   -1);

		if(!g_strcmp0(ffplayer->load_instrument,
			      value)){
		  gtk_combo_box_set_active_iter((GtkComboBox *) ffplayer->instrument,
						&iter);
		  break;
		}
	      }while(gtk_tree_model_iter_next(model,
					      &iter));
	    }

	    g_free(ffplayer->load_instrument);

	    ffplayer->load_instrument = NULL;
	  }
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
