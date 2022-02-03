/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/X/machine/ags_sfz_synth.h>
#include <ags/X/machine/ags_sfz_synth_callbacks.h>

#include <ags/i18n.h>

void ags_sfz_synth_class_init(AgsSFZSynthClass *sfz_synth);
void ags_sfz_synth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sfz_synth_init(AgsSFZSynth *sfz_synth);
void ags_sfz_synth_finalize(GObject *gobject);

void ags_sfz_synth_connect(AgsConnectable *connectable);
void ags_sfz_synth_disconnect(AgsConnectable *connectable);

void ags_sfz_synth_show(GtkWidget *widget);

void ags_sfz_synth_resize_audio_channels(AgsMachine *machine,
					 guint audio_channels, guint audio_channels_old,
					 gpointer data);
void ags_sfz_synth_resize_pads(AgsMachine *machine, GType channel_type,
			       guint pads, guint pads_old,
			       gpointer data);

void ags_sfz_synth_map_recall(AgsMachine *machine);

void ags_sfz_synth_input_map_recall(AgsSFZSynth *sfz_synth,
				    guint audio_channel_start,
				    guint input_pad_start);
void ags_sfz_synth_output_map_recall(AgsSFZSynth *sfz_synth,
				     guint audio_channel_start,
				     guint output_pad_start);

/**
 * SECTION:ags_sfz_synth
 * @short_description: sfz_synth
 * @title: AgsSFZSynth
 * @section_id:
 * @include: ags/X/machine/ags_sfz_synth.h
 *
 * The #AgsSFZSynth is a composite widget to act as sfz_synth.
 */

static gpointer ags_sfz_synth_parent_class = NULL;
static AgsConnectableInterface *ags_sfz_synth_parent_connectable_interface;

GHashTable *ags_sfz_synth_sfz_loader_completed = NULL;

GType
ags_sfz_synth_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sfz_synth = 0;

    static const GTypeInfo ags_sfz_synth_info = {
      sizeof(AgsSFZSynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sfz_synth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSFZSynth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sfz_synth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_sfz_synth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_sfz_synth = g_type_register_static(AGS_TYPE_MACHINE,
						"AgsSFZSynth", &ags_sfz_synth_info,
						0);
    
    g_type_add_interface_static(ags_type_sfz_synth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sfz_synth);
  }

  return g_define_type_id__volatile;
}

void
ags_sfz_synth_class_init(AgsSFZSynthClass *sfz_synth)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_sfz_synth_parent_class = g_type_class_peek_parent(sfz_synth);

  /* GObjectClass */
  gobject = (GObjectClass *) sfz_synth;

  gobject->finalize = ags_sfz_synth_finalize;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) sfz_synth;

  machine->map_recall = ags_sfz_synth_map_recall;
}

void
ags_sfz_synth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_sfz_synth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_sfz_synth_connect;
  connectable->disconnect = ags_sfz_synth_disconnect;
}

void
ags_sfz_synth_init(AgsSFZSynth *sfz_synth)
{
  GtkBox *sfz_hbox;
  GtkBox *sfz_file_hbox;
  GtkBox *sfz_opcode_hbox;
  GtkBox *effect_vbox;
  GtkTreeView *sfz_opcode_tree_view;
  GtkTreeViewColumn *sfz_opcode_column;
  GtkTreeViewColumn *sfz_opcode_value_column;
  GtkGrid *synth_grid;
  GtkGrid *chorus_grid;
  GtkScrolledWindow *scrolled_window;
  GtkLabel *label;

  GtkCellRenderer *sfz_opcode_renderer;
  GtkCellRenderer *sfz_opcode_value_renderer;

  GtkListStore *sfz_opcode;

  GtkAdjustment *adjustment;

  AgsAudio *audio;
  
  g_signal_connect_after((GObject *) sfz_synth, "parent_set",
			 G_CALLBACK(ags_sfz_synth_parent_set_callback), (gpointer) sfz_synth);

  audio = AGS_MACHINE(sfz_synth)->audio;

  ags_audio_set_flags(audio, (AGS_AUDIO_SYNC |
			      AGS_AUDIO_ASYNC |
			      AGS_AUDIO_OUTPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_RECYCLING));
  ags_audio_set_ability_flags(audio, (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION));
  ags_audio_set_behaviour_flags(audio, (AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING |
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
  
  AGS_MACHINE(sfz_synth)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
				    AGS_MACHINE_REVERSE_NOTATION);
  AGS_MACHINE(sfz_synth)->file_input_flags |= AGS_MACHINE_ACCEPT_SOUNDFONT2;

  /* context menu */
  ags_machine_popup_add_connection_options((AgsMachine *) sfz_synth,
  					   (AGS_MACHINE_POPUP_MIDI_DIALOG));

  /* audio resize */
  g_signal_connect_after(G_OBJECT(sfz_synth), "resize-audio-channels",
			 G_CALLBACK(ags_sfz_synth_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(sfz_synth), "resize-pads",
			 G_CALLBACK(ags_sfz_synth_resize_pads), NULL);

  /* flags */
  sfz_synth->flags = 0;

  /* mapped IO */
  sfz_synth->mapped_output_audio_channel = 0;
  sfz_synth->mapped_input_audio_channel = 0;

  sfz_synth->mapped_input_pad = 0;
  sfz_synth->mapped_output_pad = 0;

  sfz_synth->playback_play_container = ags_recall_container_new();
  sfz_synth->playback_recall_container = ags_recall_container_new();

  sfz_synth->sfz_synth_play_container = ags_recall_container_new();
  sfz_synth->sfz_synth_recall_container = ags_recall_container_new();

  sfz_synth->envelope_play_container = ags_recall_container_new();
  sfz_synth->envelope_recall_container = ags_recall_container_new();

  sfz_synth->buffer_play_container = ags_recall_container_new();
  sfz_synth->buffer_recall_container = ags_recall_container_new();

  /* context menu */
  ags_machine_popup_add_edit_options((AgsMachine *) sfz_synth,
				     (AGS_MACHINE_POPUP_ENVELOPE));
  
  /* name and xml type */
  sfz_synth->name = NULL;
  sfz_synth->xml_type = "ags-sfz-synth";

  /* SFZ */
  sfz_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				    0);
  gtk_container_add((GtkContainer *) (gtk_bin_get_child((GtkBin *) sfz_synth)),
		    (GtkWidget *) sfz_hbox);

  /* file */
  sfz_file_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					 0);  
  gtk_box_pack_start(sfz_hbox,
		     (GtkWidget *) sfz_file_hbox,
		     FALSE, FALSE,
		     0);

  sfz_synth->filename = (GtkEntry *) gtk_entry_new();

  gtk_widget_set_valign((GtkWidget *) sfz_synth->filename,
			GTK_ALIGN_START);
  
  gtk_box_pack_start(sfz_file_hbox,
		     (GtkWidget *) sfz_synth->filename,
		     FALSE, FALSE,
		     0);
  
  sfz_synth->open = (GtkButton *) gtk_button_new_with_mnemonic(i18n("_Open"));

  gtk_widget_set_valign((GtkWidget *) sfz_synth->open,
			GTK_ALIGN_START);

  gtk_box_pack_start(sfz_file_hbox,
		     (GtkWidget *) sfz_synth->open,
		     FALSE, FALSE,
		     0);

  sfz_synth->sfz_loader = NULL;
  
  sfz_synth->position = -1;

  sfz_synth->sfz_loader_spinner = (GtkSpinner *) gtk_spinner_new();
  gtk_box_pack_start(sfz_file_hbox,
		     (GtkWidget *) sfz_synth->sfz_loader_spinner,
		     FALSE, FALSE,
		     0);
  gtk_widget_set_no_show_all((GtkWidget *) sfz_synth->sfz_loader_spinner,
			     TRUE);
  gtk_widget_hide((GtkWidget *) sfz_synth->sfz_loader_spinner);

  /* opcode */
  sfz_opcode_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					   0);
  gtk_box_pack_start(sfz_hbox,
		     (GtkWidget *) sfz_opcode_hbox,
		     FALSE, FALSE,
		     0);

  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL,
								  NULL);
  gtk_widget_set_size_request((GtkWidget *) scrolled_window,
			      AGS_SFZ_SYNTH_OPCODE_WIDTH_REQUEST,
			      AGS_SFZ_SYNTH_OPCODE_HEIGHT_REQUEST);
  gtk_scrolled_window_set_policy(scrolled_window,
				 GTK_POLICY_AUTOMATIC,
				 GTK_POLICY_ALWAYS);
  gtk_box_pack_start(sfz_opcode_hbox,
		     (GtkWidget *) scrolled_window,
		     FALSE, FALSE,
		     0);
  
  sfz_synth->opcode_tree_view = 
    sfz_opcode_tree_view = gtk_tree_view_new();
  gtk_tree_view_set_activate_on_single_click(sfz_opcode_tree_view,
					     TRUE);
  gtk_container_add((GtkContainer *) scrolled_window,
		    (GtkWidget *) sfz_opcode_tree_view);
    
  gtk_widget_set_size_request((GtkWidget *) sfz_opcode_tree_view,
			      AGS_SFZ_SYNTH_OPCODE_WIDTH_REQUEST,
			      AGS_SFZ_SYNTH_OPCODE_HEIGHT_REQUEST);

  sfz_opcode_renderer = gtk_cell_renderer_text_new();

  sfz_opcode_column = gtk_tree_view_column_new_with_attributes(i18n("opcode"),
							       sfz_opcode_renderer,
							       "text", 0,
							       NULL);
  gtk_tree_view_append_column(sfz_opcode_tree_view,
			      sfz_opcode_column);

  sfz_opcode_value_renderer = gtk_cell_renderer_text_new();

  sfz_opcode_value_column = gtk_tree_view_column_new_with_attributes(i18n("value"),
								     sfz_opcode_value_renderer,
								     "text", 1,
								     NULL);
  gtk_tree_view_append_column(sfz_opcode_tree_view,
			      sfz_opcode_value_column);
  
  sfz_opcode = gtk_list_store_new(2,
				  G_TYPE_STRING,
				  G_TYPE_STRING);

  gtk_tree_view_set_model(sfz_opcode_tree_view,
			  GTK_TREE_MODEL(sfz_opcode));  

  /* effect control */
  effect_vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				       0);
  gtk_container_add((GtkContainer *) sfz_hbox,
		    (GtkWidget *) effect_vbox);

  /*  */
  synth_grid = (GtkGrid *) gtk_grid_new();
  gtk_box_pack_start(effect_vbox,
		     (GtkWidget *) synth_grid,
		     FALSE, FALSE,
		     0);

  /* WAV 1 - octave */
  label = (GtkLabel *) gtk_label_new(i18n("WAV 1 - octave"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);

  sfz_synth->synth_octave = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->synth_octave);

  gtk_adjustment_set_lower(adjustment,
			   -6.0);
  gtk_adjustment_set_upper(adjustment,
			   6.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);

//  sfz_synth->synth_octave->scale_max_precision = 12;
  
//  ags_dial_set_scale_precision(sfz_synth->synth_octave,
//			       12);
  ags_dial_set_radius(sfz_synth->synth_octave,
		      12);
  
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) sfz_synth->synth_octave,
		  1, 0,
		  1, 1);

  /* WAV 1 - key */
  label = (GtkLabel *) gtk_label_new(i18n("WAV 1 - key"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  sfz_synth->synth_key = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->synth_key);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   2.0);

//  sfz_synth->synth_key->scale_max_precision = 12;
//  ags_dial_set_scale_precision(sfz_synth->synth_key,
//			       12);
  ags_dial_set_radius(sfz_synth->synth_key,
		      12);
  
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) sfz_synth->synth_key,
		  1, 1,
		  1, 1);

  /* WAV 1 - volume */
  label = (GtkLabel *) gtk_label_new(i18n("WAV 1 - volume"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) label,
		  2, 0,
		  1, 1);

  sfz_synth->synth_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->synth_volume);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.5);

  ags_dial_set_radius(sfz_synth->synth_volume,
		      12);
  
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) sfz_synth->synth_volume,
		  3, 0,
		  1, 1);

  /* chorus grid */
  chorus_grid = (GtkGrid *) gtk_grid_new();
  gtk_box_pack_start(effect_vbox,
		     (GtkWidget *) chorus_grid,
		     FALSE, FALSE,
		     0);
  
  /* chorus input volume */
  label = (GtkLabel *) gtk_label_new(i18n("chorus input volume"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);

  sfz_synth->chorus_input_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->chorus_input_volume);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(sfz_synth->chorus_input_volume,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sfz_synth->chorus_input_volume,
		  1, 0,
		  1, 1);

  /* chorus output volume */
  label = (GtkLabel *) gtk_label_new(i18n("chorus output volume"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  sfz_synth->chorus_output_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->chorus_output_volume);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(sfz_synth->chorus_output_volume,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sfz_synth->chorus_output_volume,
		  1, 1,
		  1, 1);
  
  /* chorus LFO */
  label = (GtkLabel *) gtk_label_new(i18n("chorus LFO"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) label,
		  2, 0,
		  1, 1);
  
  sfz_synth->chorus_lfo_oscillator = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(sfz_synth->chorus_lfo_oscillator,
				 "sine");
  gtk_combo_box_text_append_text(sfz_synth->chorus_lfo_oscillator,
				 "sawtooth");
  gtk_combo_box_text_append_text(sfz_synth->chorus_lfo_oscillator,
				 "triangle");
  gtk_combo_box_text_append_text(sfz_synth->chorus_lfo_oscillator,
				 "square");
  gtk_combo_box_text_append_text(sfz_synth->chorus_lfo_oscillator,
				 "impulse");

  gtk_combo_box_set_active(sfz_synth->chorus_lfo_oscillator,
			   0);

  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sfz_synth->chorus_lfo_oscillator,
		  3, 0,
		  1, 1);

  /* chorus LFO frequency */
  label = (GtkLabel *) gtk_label_new(i18n("chorus LFO frequency"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) label,
		  2, 1,
		  1, 1);

  sfz_synth->chorus_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 10.0, 0.01);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sfz_synth->chorus_lfo_frequency,
		  3, 1,
		  1, 1);

  /* chorus depth */
  label = (GtkLabel *) gtk_label_new(i18n("chorus depth"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) label,
		  4, 0,
		  1, 1);

  sfz_synth->chorus_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->chorus_depth);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(sfz_synth->chorus_depth,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sfz_synth->chorus_depth,
		  5, 0,
		  1, 1);

  /* chorus mix */
  label = (GtkLabel *) gtk_label_new(i18n("chorus mix"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) label,
		  4, 1,
		  1, 1);

  sfz_synth->chorus_mix = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->chorus_mix);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.5);

  ags_dial_set_radius(sfz_synth->chorus_mix,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sfz_synth->chorus_mix,
		  5, 1,
		  1, 1);

  /* chorus delay */
  label = (GtkLabel *) gtk_label_new(i18n("chorus delay"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) label,
		  4, 2,
		  1, 1);

  sfz_synth->chorus_delay = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->chorus_delay);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(sfz_synth->chorus_delay,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sfz_synth->chorus_delay,
		  5, 2,
		  1, 1);

  /* dialog */
  sfz_synth->open_dialog = NULL;

  /* SFZ loader */
  if(ags_sfz_synth_sfz_loader_completed == NULL){
    ags_sfz_synth_sfz_loader_completed = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							       NULL,
							       NULL);
  }

  g_hash_table_insert(ags_sfz_synth_sfz_loader_completed,
		      sfz_synth, ags_sfz_synth_sfz_loader_completed_timeout);
  g_timeout_add(1000 / 4, (GSourceFunc) ags_sfz_synth_sfz_loader_completed_timeout, (gpointer) sfz_synth);
}

void
ags_sfz_synth_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_sfz_synth_parent_class)->finalize(gobject);
}

void
ags_sfz_synth_connect(AgsConnectable *connectable)
{
  AgsSFZSynth *sfz_synth;
  
  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_sfz_synth_parent_connectable_interface->connect(connectable);

  /* AgsSFZSynth */
  sfz_synth = AGS_SFZ_SYNTH(connectable);

  g_signal_connect((GObject *) sfz_synth, "destroy",
		   G_CALLBACK(ags_sfz_synth_destroy_callback), (gpointer) sfz_synth);  

  g_signal_connect((GObject *) sfz_synth->open, "clicked",
		   G_CALLBACK(ags_sfz_synth_open_clicked_callback), (gpointer) sfz_synth);

  g_signal_connect_after(sfz_synth->synth_octave, "value-changed",
			 G_CALLBACK(ags_sfz_synth_synth_octave_callback), sfz_synth);
  
  g_signal_connect_after(sfz_synth->synth_key, "value-changed",
			 G_CALLBACK(ags_sfz_synth_synth_key_callback), sfz_synth);
  
  g_signal_connect_after(sfz_synth->synth_volume, "value-changed",
			 G_CALLBACK(ags_sfz_synth_synth_volume_callback), sfz_synth);

  //  g_signal_connect_after(sfz_synth->chorus_enabled, "clicked",
//			 G_CALLBACK(ags_sfz_synth_chorus_enabled_callback), sfz_synth);
  
  g_signal_connect_after(sfz_synth->chorus_input_volume, "value-changed",
			 G_CALLBACK(ags_sfz_synth_chorus_input_volume_callback), sfz_synth);
  
  g_signal_connect_after(sfz_synth->chorus_output_volume, "value-changed",
			 G_CALLBACK(ags_sfz_synth_chorus_output_volume_callback), sfz_synth);
  
  g_signal_connect_after(sfz_synth->chorus_lfo_oscillator, "changed",
			 G_CALLBACK(ags_sfz_synth_chorus_lfo_oscillator_callback), sfz_synth);
  
  g_signal_connect_after(sfz_synth->chorus_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_sfz_synth_chorus_lfo_frequency_callback), sfz_synth);
  
  g_signal_connect_after(sfz_synth->chorus_depth, "value-changed",
			 G_CALLBACK(ags_sfz_synth_chorus_depth_callback), sfz_synth);
  
  g_signal_connect_after(sfz_synth->chorus_mix, "value-changed",
			 G_CALLBACK(ags_sfz_synth_chorus_mix_callback), sfz_synth);
  
  g_signal_connect_after(sfz_synth->chorus_delay, "value-changed",
			 G_CALLBACK(ags_sfz_synth_chorus_delay_callback), sfz_synth);
}

void
ags_sfz_synth_disconnect(AgsConnectable *connectable)
{
  AgsSFZSynth *sfz_synth;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  ags_sfz_synth_parent_connectable_interface->disconnect(connectable);

  /* AgsSFZSynth */
  sfz_synth = AGS_SFZ_SYNTH(connectable);

  g_object_disconnect((GObject *) sfz_synth,
		      "any_signal::destroy",
		      G_CALLBACK(ags_sfz_synth_destroy_callback),
		      (gpointer) sfz_synth,
		      NULL);  

  g_object_disconnect((GObject *) sfz_synth->open,
		      "any_signal::clicked",
		      G_CALLBACK(ags_sfz_synth_open_clicked_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->synth_octave,
		      "any_signal::clicked",
		      G_CALLBACK(ags_sfz_synth_synth_octave_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->synth_key,
		      "any_signal::clicked",
		      G_CALLBACK(ags_sfz_synth_synth_key_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->synth_volume,
		      "any_signal::clicked",
		      G_CALLBACK(ags_sfz_synth_synth_volume_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->chorus_input_volume,
		      "any_signal::clicked",
		      G_CALLBACK(ags_sfz_synth_chorus_input_volume_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->chorus_output_volume,
		      "any_signal::clicked",
		      G_CALLBACK(ags_sfz_synth_chorus_output_volume_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->chorus_input_volume,
		      "any_signal::clicked",
		      G_CALLBACK(ags_sfz_synth_chorus_input_volume_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->chorus_lfo_oscillator,
		      "any_signal::clicked",
		      G_CALLBACK(ags_sfz_synth_chorus_lfo_oscillator_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->chorus_lfo_frequency,
		      "any_signal::clicked",
		      G_CALLBACK(ags_sfz_synth_chorus_lfo_frequency_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->chorus_depth,
		      "any_signal::clicked",
		      G_CALLBACK(ags_sfz_synth_chorus_depth_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->chorus_mix,
		      "any_signal::clicked",
		      G_CALLBACK(ags_sfz_synth_chorus_mix_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->chorus_delay,
		      "any_signal::clicked",
		      G_CALLBACK(ags_sfz_synth_chorus_delay_callback),
		      (gpointer) sfz_synth,
		      NULL);
}

void
ags_sfz_synth_resize_audio_channels(AgsMachine *machine,
				    guint audio_channels, guint audio_channels_old,
				    gpointer data)
{
  AgsSFZSynth *sfz_synth;

  sfz_synth = (AgsSFZSynth *) machine;

  if(audio_channels > audio_channels_old){    
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_sfz_synth_input_map_recall(sfz_synth,
				     audio_channels_old,
				     0);
      
      ags_sfz_synth_output_map_recall(sfz_synth,
				      audio_channels_old,
				      0);
    }
  }
}

void
ags_sfz_synth_resize_pads(AgsMachine *machine, GType type,
			  guint pads, guint pads_old,
			  gpointer data)
{
  AgsSFZSynth *sfz_synth;

  gboolean grow;

  sfz_synth = (AgsSFZSynth *) machine;
  
  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(type == AGS_TYPE_INPUT){
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	/* depending on destination */
	ags_sfz_synth_input_map_recall(sfz_synth,
				       0,
				       pads_old);
      }
    }else{
      sfz_synth->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_sfz_synth_output_map_recall(sfz_synth,
					0,
					pads_old);
      }
    }else{
      sfz_synth->mapped_output_pad = pads;
    }
  }
}

void
ags_sfz_synth_map_recall(AgsMachine *machine)
{
  AgsSFZSynth *sfz_synth;
  
  AgsAudio *audio;

  GList *start_recall;

  gint position;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  sfz_synth = AGS_SFZ_SYNTH(machine);

  audio = machine->audio;
  
  position = 0;
  
  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       sfz_synth->playback_play_container, sfz_synth->playback_recall_container,
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

  /* ags-fx-sfz-synth */
  start_recall = ags_fx_factory_create(audio,
				       sfz_synth->sfz_synth_play_container, sfz_synth->sfz_synth_recall_container,
				       "ags-fx-sfz-synth",
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
				       sfz_synth->envelope_play_container, sfz_synth->envelope_recall_container,
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

  /* ags-fx-buffer */
  start_recall = ags_fx_factory_create(audio,
				       sfz_synth->buffer_play_container, sfz_synth->buffer_recall_container,
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

  /* depending on destination */
  ags_sfz_synth_input_map_recall(sfz_synth,
				 0,
				 0);

  /* depending on destination */
  ags_sfz_synth_output_map_recall(sfz_synth,
				  0,
				  0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_sfz_synth_parent_class)->map_recall(machine);  
}

void
ags_sfz_synth_input_map_recall(AgsSFZSynth *sfz_synth,
			       guint audio_channel_start,
			       guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  guint input_pads;
  guint audio_channels;
  gint position;
  guint i;
  guint j;

  audio = AGS_MACHINE(sfz_synth)->audio;

  position = 0;

  /* get some fields */
  input_pads = AGS_MACHINE(sfz_synth)->input_pads;
  audio_channels = AGS_MACHINE(sfz_synth)->audio_channels;    
  
  for(i = 0; i < input_pads; i++){
    for(j = 0; j < audio_channels; j++){
      AgsMachineInputLine* input_line;

      input_line = g_list_nth_data(AGS_MACHINE(sfz_synth)->machine_input_line,
				   (i * audio_channels) + j);

      if(input_line != NULL &&
	 input_line->mapped_recall == FALSE){
	/* ags-fx-playback */
	start_recall = ags_fx_factory_create(audio,
					     sfz_synth->playback_play_container, sfz_synth->playback_recall_container,
					     "ags-fx-playback",
					     NULL,
					     NULL,
					     j, j + 1,
					     i, i + 1,
					     position,
					     (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

	g_list_free_full(start_recall,
			 (GDestroyNotify) g_object_unref);

	/* ags-fx-sfz-synth */
	start_recall = ags_fx_factory_create(audio,
					     sfz_synth->sfz_synth_play_container, sfz_synth->sfz_synth_recall_container,
					     "ags-fx-sfz-synth",
					     NULL,
					     NULL,
					     j, j + 1,
					     i, i + 1,
					     position,
					     (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

	g_list_free_full(start_recall,
			 (GDestroyNotify) g_object_unref);

	/* ags-fx-envelope */
	start_recall = ags_fx_factory_create(audio,
					     sfz_synth->envelope_play_container, sfz_synth->envelope_recall_container,
					     "ags-fx-envelope",
					     NULL,
					     NULL,
					     j, j + 1,
					     i, i + 1,
					     position,
					     (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

	g_list_free_full(start_recall,
			 (GDestroyNotify) g_object_unref);

	/* ags-fx-buffer */
	start_recall = ags_fx_factory_create(audio,
					     sfz_synth->buffer_play_container, sfz_synth->buffer_recall_container,
					     "ags-fx-buffer",
					     NULL,
					     NULL,
					     j, j + 1,
					     i, i + 1,
					     position,
					     (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

	g_list_free_full(start_recall,
			 (GDestroyNotify) g_object_unref);
      }
    }
  }
  
  sfz_synth->mapped_input_audio_channel = audio_channels;
  sfz_synth->mapped_input_pad = input_pads;
}

void
ags_sfz_synth_output_map_recall(AgsSFZSynth *sfz_synth,
				guint audio_channel_start,
				guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;
  guint audio_channels;

  if(sfz_synth->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(sfz_synth)->audio;
  
  /* get some fields */
  output_pads = AGS_MACHINE(sfz_synth)->output_pads;
  audio_channels = AGS_MACHINE(sfz_synth)->audio_channels;    
  
  sfz_synth->mapped_output_audio_channel = audio_channels;
  sfz_synth->mapped_output_pad = output_pads;
}

/**
 * ags_sfz_synth_open_filename:
 * @sfz_synth: the #AgsSFZSynth
 * @filename: the filename
 * 
 * Open @filename.
 * 
 * Since: 3.17.0
 */
void
ags_sfz_synth_open_filename(AgsSFZSynth *sfz_synth,
			    gchar *filename)
{
  AgsSFZLoader *sfz_loader;

  if(!AGS_IS_SFZ_SYNTH(sfz_synth) ||
     filename == NULL){
    return;
  }
  
  sfz_synth->sfz_loader = 
    sfz_loader = ags_sfz_loader_new(AGS_MACHINE(sfz_synth)->audio,
				    filename,
				    TRUE);

  ags_sfz_loader_start(sfz_loader);
}

/**
 * ags_sfz_synth_sfz_loader_completed_timeout:
 * @sfz_synth: the #AgsSFZSynth
 *
 * Queue draw widget
 *
 * Returns: %TRUE if proceed poll completed, otherwise %FALSE
 *
 * Since: 3.4.0
 */
gboolean
ags_sfz_synth_sfz_loader_completed_timeout(AgsSFZSynth *sfz_synth)
{
  if(g_hash_table_lookup(ags_sfz_synth_sfz_loader_completed,
			 sfz_synth) != NULL){
    //TODO:JK: implement me
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_sfz_synth_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsSFZSynth
 *
 * Returns: the new #AgsSFZSynth
 *
 * Since: 3.4.0
 */
AgsSFZSynth*
ags_sfz_synth_new(GObject *soundcard)
{
  AgsSFZSynth *sfz_synth;

  sfz_synth = (AgsSFZSynth *) g_object_new(AGS_TYPE_SFZ_SYNTH,
					   NULL);

  g_object_set(AGS_MACHINE(sfz_synth)->audio,
	       "output-soundcard", soundcard,
	       NULL);

  return(sfz_synth);
}
