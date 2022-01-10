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

#include <ags/X/machine/ags_sf2_synth.h>
#include <ags/X/machine/ags_sf2_synth_callbacks.h>

#include <ags/i18n.h>

void ags_sf2_synth_class_init(AgsSF2SynthClass *sf2_synth);
void ags_sf2_synth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sf2_synth_init(AgsSF2Synth *sf2_synth);
void ags_sf2_synth_finalize(GObject *gobject);

void ags_sf2_synth_connect(AgsConnectable *connectable);
void ags_sf2_synth_disconnect(AgsConnectable *connectable);

void ags_sf2_synth_show(GtkWidget *widget);

void ags_sf2_synth_resize_audio_channels(AgsMachine *machine,
					 guint audio_channels, guint audio_channels_old,
					 gpointer data);
void ags_sf2_synth_resize_pads(AgsMachine *machine, GType channel_type,
			       guint pads, guint pads_old,
			       gpointer data);

void ags_sf2_synth_map_recall(AgsMachine *machine);

void ags_sf2_synth_input_map_recall(AgsSF2Synth *sf2_synth,
				    guint audio_channel_start,
				    guint input_pad_start);
void ags_sf2_synth_output_map_recall(AgsSF2Synth *sf2_synth,
				     guint audio_channel_start,
				     guint output_pad_start);

gint ags_sf2_synth_int_compare_func(gconstpointer a,
				    gconstpointer b);

/**
 * SECTION:ags_sf2_synth
 * @short_description: sf2_synth
 * @title: AgsSF2Synth
 * @section_id:
 * @include: ags/X/machine/ags_sf2_synth.h
 *
 * The #AgsSF2Synth is a composite widget to act as sf2_synth.
 */

static gpointer ags_sf2_synth_parent_class = NULL;
static AgsConnectableInterface *ags_sf2_synth_parent_connectable_interface;

GHashTable *ags_sf2_synth_sf2_loader_completed = NULL;

GType
ags_sf2_synth_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sf2_synth = 0;

    static const GTypeInfo ags_sf2_synth_info = {
      sizeof(AgsSF2SynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sf2_synth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSF2Synth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sf2_synth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_sf2_synth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_sf2_synth = g_type_register_static(AGS_TYPE_MACHINE,
						"AgsSF2Synth", &ags_sf2_synth_info,
						0);
    
    g_type_add_interface_static(ags_type_sf2_synth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sf2_synth);
  }

  return g_define_type_id__volatile;
}

void
ags_sf2_synth_class_init(AgsSF2SynthClass *sf2_synth)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_sf2_synth_parent_class = g_type_class_peek_parent(sf2_synth);

  /* GObjectClass */
  gobject = (GObjectClass *) sf2_synth;

  gobject->finalize = ags_sf2_synth_finalize;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) sf2_synth;

  machine->map_recall = ags_sf2_synth_map_recall;
}

void
ags_sf2_synth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_sf2_synth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_sf2_synth_connect;
  connectable->disconnect = ags_sf2_synth_disconnect;
}

void
ags_sf2_synth_init(AgsSF2Synth *sf2_synth)
{
  GtkBox *sf2_hbox;
  GtkBox *sf2_file_hbox;
  GtkBox *sf2_preset_hbox;
  GtkGrid *synth_grid;
  GtkGrid *chorus_grid;
  GtkBox *hbox;
  GtkTreeView *sf2_bank_tree_view;
  GtkTreeView *sf2_program_tree_view;
  GtkTreeViewColumn *sf2_bank_column;
  GtkTreeViewColumn *sf2_program_column;
  GtkTreeViewColumn *sf2_preset_column;
  GtkScrolledWindow *scrolled_window;
  GtkLabel *label;
    
  GtkCellRenderer *sf2_bank_renderer;
  GtkCellRenderer *sf2_program_renderer;
  GtkCellRenderer *sf2_preset_renderer;
  
  GtkListStore *sf2_bank;
  GtkListStore *sf2_program;

  GtkAdjustment *adjustment;
  
  AgsAudio *audio;
  
  g_signal_connect_after((GObject *) sf2_synth, "parent_set",
			 G_CALLBACK(ags_sf2_synth_parent_set_callback), (gpointer) sf2_synth);
  
  audio = AGS_MACHINE(sf2_synth)->audio;

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
  
  AGS_MACHINE(sf2_synth)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
				    AGS_MACHINE_REVERSE_NOTATION);
  AGS_MACHINE(sf2_synth)->file_input_flags |= AGS_MACHINE_ACCEPT_SOUNDFONT2;

  /* context menu */
  ags_machine_popup_add_connection_options((AgsMachine *) sf2_synth,
  					   (AGS_MACHINE_POPUP_MIDI_DIALOG));

  /* audio resize */
  g_signal_connect_after(G_OBJECT(sf2_synth), "resize-audio-channels",
			 G_CALLBACK(ags_sf2_synth_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(sf2_synth), "resize-pads",
			 G_CALLBACK(ags_sf2_synth_resize_pads), NULL);

  /* flags */
  sf2_synth->flags = 0;

  /* mapped IO */
  sf2_synth->mapped_input_pad = 0;
  sf2_synth->mapped_output_pad = 0;

  sf2_synth->playback_play_container = ags_recall_container_new();
  sf2_synth->playback_recall_container = ags_recall_container_new();

  sf2_synth->notation_play_container = ags_recall_container_new();
  sf2_synth->notation_recall_container = ags_recall_container_new();

  sf2_synth->envelope_play_container = ags_recall_container_new();
  sf2_synth->envelope_recall_container = ags_recall_container_new();

  sf2_synth->buffer_play_container = ags_recall_container_new();
  sf2_synth->buffer_recall_container = ags_recall_container_new();

  /* context menu */
  ags_machine_popup_add_edit_options((AgsMachine *) sf2_synth,
				     (AGS_MACHINE_POPUP_ENVELOPE));
  
  /* name and xml type */
  sf2_synth->name = NULL;
  sf2_synth->xml_type = "ags-sf2-synth";

  /* audio container */
  sf2_synth->audio_container = NULL;

  /* SF2 */
  sf2_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				    0);
  gtk_container_add((GtkContainer *) (gtk_bin_get_child((GtkBin *) sf2_synth)),
		    (GtkWidget *) sf2_hbox);

  /* file */
  sf2_file_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					 0);  
  gtk_box_pack_start(sf2_hbox,
		     (GtkWidget *) sf2_file_hbox,
		     FALSE, FALSE,
		     0);

  sf2_synth->filename = (GtkEntry *) gtk_entry_new();

  gtk_widget_set_valign((GtkWidget *) sf2_synth->filename,
			GTK_ALIGN_START);
  
  gtk_box_pack_start(sf2_file_hbox,
		     (GtkWidget *) sf2_synth->filename,
		     FALSE, FALSE,
		     0);
  
  sf2_synth->open = (GtkButton *) gtk_button_new_with_mnemonic(i18n("_Open"));

  gtk_widget_set_valign((GtkWidget *) sf2_synth->open,
			GTK_ALIGN_START);

  gtk_box_pack_start(sf2_file_hbox,
		     (GtkWidget *) sf2_synth->open,
		     FALSE, FALSE,
		     0);

  sf2_synth->position = -1;

  sf2_synth->sf2_loader_spinner = (GtkSpinner *) gtk_spinner_new();
  gtk_box_pack_start(sf2_file_hbox,
		     (GtkWidget *) sf2_synth->sf2_loader_spinner,
		     FALSE, FALSE,
		     0);
  gtk_widget_set_no_show_all((GtkWidget *) sf2_synth->sf2_loader_spinner,
			     TRUE);
  gtk_widget_hide((GtkWidget *) sf2_synth->sf2_loader_spinner);

  /* preset - bank and program */
  sf2_preset_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					   0);
  gtk_box_pack_start(sf2_hbox,
		     (GtkWidget *) sf2_preset_hbox,
		     FALSE, FALSE,
		     0);

  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL,
								  NULL);
  gtk_widget_set_size_request((GtkWidget *) scrolled_window,
			      AGS_SF2_SYNTH_BANK_WIDTH_REQUEST,
			      AGS_SF2_SYNTH_BANK_HEIGHT_REQUEST);
  gtk_scrolled_window_set_policy(scrolled_window,
				 GTK_POLICY_AUTOMATIC,
				 GTK_POLICY_ALWAYS);
  gtk_box_pack_start(sf2_preset_hbox,
		     (GtkWidget *) scrolled_window,
		     FALSE, FALSE,
		     0);
  
  sf2_synth->bank_tree_view = 
    sf2_bank_tree_view = gtk_tree_view_new();
  gtk_tree_view_set_activate_on_single_click(sf2_bank_tree_view,
					     TRUE);
  gtk_container_add((GtkContainer *) scrolled_window,
		    (GtkWidget *) sf2_bank_tree_view);
    
  gtk_widget_set_size_request((GtkWidget *) sf2_bank_tree_view,
			      AGS_SF2_SYNTH_BANK_WIDTH_REQUEST,
			      AGS_SF2_SYNTH_BANK_HEIGHT_REQUEST);

  sf2_bank_renderer = gtk_cell_renderer_text_new();

  sf2_bank_column = gtk_tree_view_column_new_with_attributes(i18n("bank"),
							     sf2_bank_renderer,
							     "text", 0,
							     NULL);
  gtk_tree_view_append_column(sf2_bank_tree_view,
			      sf2_bank_column);
  
  sf2_bank = gtk_list_store_new(1,
				G_TYPE_INT);

  gtk_tree_view_set_model(sf2_bank_tree_view,
			  GTK_TREE_MODEL(sf2_bank));  
  
  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL,
								  NULL);
  gtk_widget_set_size_request((GtkWidget *) scrolled_window,
			      AGS_SF2_SYNTH_PROGRAM_WIDTH_REQUEST,
			      AGS_SF2_SYNTH_PROGRAM_HEIGHT_REQUEST);
  gtk_scrolled_window_set_policy(scrolled_window,
				 GTK_POLICY_AUTOMATIC,
				 GTK_POLICY_ALWAYS);
  gtk_box_pack_start((GtkBox *) sf2_preset_hbox,
		     (GtkWidget *) scrolled_window,
		     FALSE, FALSE,
		     0);

  sf2_synth->program_tree_view = 
    sf2_program_tree_view = gtk_tree_view_new();
  gtk_tree_view_set_activate_on_single_click(sf2_program_tree_view,
					     TRUE);
  gtk_container_add((GtkContainer *) scrolled_window,
		    (GtkWidget *) sf2_program_tree_view);

  gtk_widget_set_size_request((GtkWidget *) sf2_program_tree_view,
			      AGS_SF2_SYNTH_PROGRAM_WIDTH_REQUEST,
			      AGS_SF2_SYNTH_PROGRAM_HEIGHT_REQUEST);

  sf2_program_renderer = gtk_cell_renderer_text_new();
  sf2_preset_renderer = gtk_cell_renderer_text_new();
  
  sf2_program_column = gtk_tree_view_column_new_with_attributes(i18n("program"),
								sf2_program_renderer,
								"text", 1,
								NULL);
  gtk_tree_view_append_column(sf2_program_tree_view,
			      sf2_program_column);

  sf2_preset_column = gtk_tree_view_column_new_with_attributes(i18n("preset"),
							       sf2_preset_renderer,
								"text", 2,
							       NULL);
  gtk_tree_view_append_column(sf2_program_tree_view,
			      sf2_preset_column);
  
  sf2_program = gtk_list_store_new(3,
				   G_TYPE_INT,
				   G_TYPE_INT,
				   G_TYPE_STRING);

  gtk_tree_view_set_model(sf2_program_tree_view,
			  GTK_TREE_MODEL(sf2_program));


  synth_grid = (GtkGrid *) gtk_grid_new();
  gtk_box_pack_start(sf2_hbox,
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

  sf2_synth->synth_octave = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->synth_octave);

  gtk_adjustment_set_lower(adjustment,
			   -6.0);
  gtk_adjustment_set_upper(adjustment,
			   6.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);

//  sf2_synth->synth_octave->scale_max_precision = 12;
  
//  ags_dial_set_scale_precision(sf2_synth->synth_octave,
//			       12);
  ags_dial_set_radius(sf2_synth->synth_octave,
		      12);
  
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) sf2_synth->synth_octave,
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

  sf2_synth->synth_key = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->synth_key);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   2.0);

//  sf2_synth->synth_key->scale_max_precision = 12;
//  ags_dial_set_scale_precision(sf2_synth->synth_key,
//			       12);
  ags_dial_set_radius(sf2_synth->synth_key,
		      12);
  
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) sf2_synth->synth_key,
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

  sf2_synth->synth_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->synth_volume);

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

  ags_dial_set_radius(sf2_synth->synth_volume,
		      12);
  
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) sf2_synth->synth_volume,
		  3, 0,
		  1, 1);

  /* pitch */
  label = (GtkLabel *) gtk_label_new(i18n("pitch tuning"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) label,
		  4, 0,
		  1, 1);

  sf2_synth->pitch_tuning = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->pitch_tuning);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);
  gtk_adjustment_set_page_increment(adjustment,
				    10.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(sf2_synth->pitch_tuning,
		      12);
  
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) sf2_synth->pitch_tuning,
		  5, 0,
		  1, 1);

  /* chorus grid */
  chorus_grid = (GtkGrid *) gtk_grid_new();
  gtk_box_pack_start(sf2_hbox,
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

  sf2_synth->chorus_input_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->chorus_input_volume);

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

  ags_dial_set_radius(sf2_synth->chorus_input_volume,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sf2_synth->chorus_input_volume,
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

  sf2_synth->chorus_output_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->chorus_output_volume);

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

  ags_dial_set_radius(sf2_synth->chorus_output_volume,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sf2_synth->chorus_output_volume,
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
  
  sf2_synth->chorus_lfo_oscillator = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(sf2_synth->chorus_lfo_oscillator,
				 "sine");
  gtk_combo_box_text_append_text(sf2_synth->chorus_lfo_oscillator,
				 "sawtooth");
  gtk_combo_box_text_append_text(sf2_synth->chorus_lfo_oscillator,
				 "triangle");
  gtk_combo_box_text_append_text(sf2_synth->chorus_lfo_oscillator,
				 "square");
  gtk_combo_box_text_append_text(sf2_synth->chorus_lfo_oscillator,
				 "impulse");

  gtk_combo_box_set_active(sf2_synth->chorus_lfo_oscillator,
			   0);

  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sf2_synth->chorus_lfo_oscillator,
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

  sf2_synth->chorus_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 10.0, 0.01);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sf2_synth->chorus_lfo_frequency,
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

  sf2_synth->chorus_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->chorus_depth);

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

  ags_dial_set_radius(sf2_synth->chorus_depth,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sf2_synth->chorus_depth,
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

  sf2_synth->chorus_mix = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->chorus_mix);

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

  ags_dial_set_radius(sf2_synth->chorus_mix,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sf2_synth->chorus_mix,
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

  sf2_synth->chorus_delay = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->chorus_delay);

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

  ags_dial_set_radius(sf2_synth->chorus_delay,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sf2_synth->chorus_delay,
		  5, 2,
		  1, 1);

  /* dialog */
  sf2_synth->open_dialog = NULL;

  /* SF2 loader */
  if(ags_sf2_synth_sf2_loader_completed == NULL){
    ags_sf2_synth_sf2_loader_completed = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							       NULL,
							       NULL);
  }

  g_hash_table_insert(ags_sf2_synth_sf2_loader_completed,
		      sf2_synth, ags_sf2_synth_sf2_loader_completed_timeout);
  g_timeout_add(1000 / 4, (GSourceFunc) ags_sf2_synth_sf2_loader_completed_timeout, (gpointer) sf2_synth);
}

void
ags_sf2_synth_finalize(GObject *gobject)
{
  g_hash_table_remove(ags_sf2_synth_sf2_loader_completed,
		      gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_sf2_synth_parent_class)->finalize(gobject);
}

void
ags_sf2_synth_connect(AgsConnectable *connectable)
{
  AgsSF2Synth *sf2_synth;
  
  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_sf2_synth_parent_connectable_interface->connect(connectable);

  /* AgsSF2Synth */
  sf2_synth = AGS_SF2_SYNTH(connectable);

  g_signal_connect((GObject *) sf2_synth, "destroy",
		   G_CALLBACK(ags_sf2_synth_destroy_callback), (gpointer) sf2_synth);
  

  g_signal_connect((GObject *) sf2_synth->open, "clicked",
		   G_CALLBACK(ags_sf2_synth_open_clicked_callback), (gpointer) sf2_synth);

  g_signal_connect((GObject *) sf2_synth->bank_tree_view, "row-activated",
		   G_CALLBACK(ags_sf2_synth_bank_tree_view_callback), (gpointer) sf2_synth);

  g_signal_connect((GObject *) sf2_synth->program_tree_view, "row-activated",
		   G_CALLBACK(ags_sf2_synth_program_tree_view_callback), (gpointer) sf2_synth);
  
  //TODO:JK: implement me
}

void
ags_sf2_synth_disconnect(AgsConnectable *connectable)
{
  AgsSF2Synth *sf2_synth;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  ags_sf2_synth_parent_connectable_interface->disconnect(connectable);

  /* AgsSF2Synth */
  sf2_synth = AGS_SF2_SYNTH(connectable);

  g_object_disconnect((GObject *) sf2_synth,
		      "any_signal::destroy",
		      G_CALLBACK(ags_sf2_synth_destroy_callback),
		      (gpointer) sf2_synth,
		      NULL);  

  g_object_disconnect((GObject *) sf2_synth->open,
		      "any_signal::clicked",
		      G_CALLBACK(ags_sf2_synth_open_clicked_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->program_tree_view,
		      "any_signal::row-activated",
		      G_CALLBACK(ags_sf2_synth_program_tree_view_callback),
		      (gpointer) sf2_synth,
		      NULL);

  //TODO:JK: implement me
}

void
ags_sf2_synth_resize_audio_channels(AgsMachine *machine,
				    guint audio_channels, guint audio_channels_old,
				    gpointer data)
{
  AgsSF2Synth *sf2_synth;

  sf2_synth = (AgsSF2Synth *) machine;

  if(audio_channels > audio_channels_old){    
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_sf2_synth_input_map_recall(sf2_synth,
				     audio_channels_old,
				     0);
      
      ags_sf2_synth_output_map_recall(sf2_synth,
				      audio_channels_old,
				      0);
    }
  }
}

void
ags_sf2_synth_resize_pads(AgsMachine *machine, GType type,
			  guint pads, guint pads_old,
			  gpointer data)
{
  AgsSF2Synth *sf2_synth;

  gboolean grow;

  sf2_synth = (AgsSF2Synth *) machine;
  
  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(type == AGS_TYPE_INPUT){
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	/* depending on destination */
	ags_sf2_synth_input_map_recall(sf2_synth,
				       0,
				       pads_old);
      }
    }else{
      sf2_synth->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_sf2_synth_output_map_recall(sf2_synth,
					0,
					pads_old);
      }
    }else{
      sf2_synth->mapped_output_pad = pads;
    }
  }
}

void
ags_sf2_synth_map_recall(AgsMachine *machine)
{
  AgsSF2Synth *sf2_synth;
  
  AgsAudio *audio;

  GList *start_recall;

  gint position;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  sf2_synth = AGS_SF2_SYNTH(machine);

  audio = machine->audio;
  
  position = 0;
  
  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       sf2_synth->playback_play_container, sf2_synth->playback_recall_container,
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

  /* ags-fx-notation */
  start_recall = ags_fx_factory_create(audio,
				       sf2_synth->notation_play_container, sf2_synth->notation_recall_container,
				       "ags-fx-sf2-synth",
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
				       sf2_synth->envelope_play_container, sf2_synth->envelope_recall_container,
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
				       sf2_synth->buffer_play_container, sf2_synth->buffer_recall_container,
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
  ags_sf2_synth_input_map_recall(sf2_synth,
				 0,
				 0);

  /* depending on destination */
  ags_sf2_synth_output_map_recall(sf2_synth,
				  0,
				  0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_sf2_synth_parent_class)->map_recall(machine);  
}

void
ags_sf2_synth_input_map_recall(AgsSF2Synth *sf2_synth,
			       guint audio_channel_start,
			       guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(sf2_synth->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

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
				       sf2_synth->playback_play_container, sf2_synth->playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-notation */
  start_recall = ags_fx_factory_create(audio,
				       sf2_synth->notation_play_container, sf2_synth->notation_recall_container,
				       "ags-fx-sf2-synth",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-envelope */
  start_recall = ags_fx_factory_create(audio,
				       sf2_synth->envelope_play_container, sf2_synth->envelope_recall_container,
				       "ags-fx-envelope",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-buffer */
  start_recall = ags_fx_factory_create(audio,
				       sf2_synth->buffer_play_container, sf2_synth->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  sf2_synth->mapped_input_pad = input_pads;
}

void
ags_sf2_synth_output_map_recall(AgsSF2Synth *sf2_synth,
				guint audio_channel_start,
				guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;

  if(sf2_synth->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);
  
  sf2_synth->mapped_output_pad = output_pads;
}

gint
ags_sf2_synth_int_compare_func(gconstpointer a,
			       gconstpointer b)
{
  if(GPOINTER_TO_INT(a) < GPOINTER_TO_INT(b)){
    return(-1);
  }else if(GPOINTER_TO_INT(a) == GPOINTER_TO_INT(b)){
    return(0);
  }else{
    return(1);
  }
}

/**
 * ags_sf2_synth_open_filename:
 * @sf2_synth: the #AgsSF2Synth
 * @filename: the filename
 * 
 * Open @filename.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_open_filename(AgsSF2Synth *sf2_synth,
			    gchar *filename)
{
  AgsSF2Loader *sf2_loader;

  if(!AGS_IS_SF2_SYNTH(sf2_synth) ||
     filename == NULL){
    return;
  }
  
  sf2_synth->sf2_loader = 
    sf2_loader = ags_sf2_loader_new(AGS_MACHINE(sf2_synth)->audio,
				    filename,
				    NULL,
				    NULL);

  ags_sf2_loader_start(sf2_loader);
}

void
ags_sf2_synth_load_bank(AgsSF2Synth *sf2_synth,
			gint bank)
{
  GtkListStore *program_list_store;
	
  AgsIpatch *ipatch;

  IpatchSF2 *sf2;
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;
	
  IpatchIter preset_iter;

  GError *error;

  program_list_store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(sf2_synth->program_tree_view)));

  gtk_list_store_clear(program_list_store);

  ipatch = sf2_synth->audio_container->sound_container;

  error = NULL;
  sf2 = (IpatchSF2 *) ipatch_convert_object_to_type((GObject *) ipatch->handle->file,
						    IPATCH_TYPE_SF2,
						    &error);

  if(error != NULL){
    g_error_free(error);
  }
	
  ipatch_list = ipatch_container_get_children((IpatchContainer *) sf2,
					      IPATCH_TYPE_SF2_PRESET);

  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &preset_iter);
    
    if(ipatch_iter_first(&preset_iter) != NULL){
      GList *start_list_bank, *list_bank;
      GList *start_list_program, *list_program;
      GList *start_list_name, *list_name;

      GtkTreeIter tree_iter;

      guint i;
      guint j;

      start_list_bank = NULL;
      start_list_program = NULL;
      start_list_name = NULL;

      i = 0;
      j = 0;
      
      do{
	gchar *current_name;
	
	int current_bank, current_program;
	      
	ipatch_item = ipatch_iter_get(&preset_iter);

	ipatch_sf2_preset_get_midi_locale((IpatchSF2Preset *) ipatch_item,
					  &current_bank,
					  &current_program);

	if(g_list_find(start_list_program, GINT_TO_POINTER(current_program)) == NULL){
	  current_name = ipatch_sf2_preset_get_name((IpatchSF2Preset *) ipatch_item);
	  
	  start_list_program = g_list_insert_sorted(start_list_program,
						    GINT_TO_POINTER(current_program),
						    ags_sf2_synth_int_compare_func);

	  start_list_bank = g_list_insert(start_list_bank,
					  GINT_TO_POINTER(current_bank),
					  g_list_index(start_list_program,
						       GINT_TO_POINTER(current_program)));

	  start_list_name = g_list_insert(start_list_name,
					  current_name,
					  g_list_index(start_list_program,
						       GINT_TO_POINTER(current_program)));
	  i++;
	  
	  if(current_bank == bank){	    
	    j++;	    
	  }
	}
      }while(ipatch_iter_next(&preset_iter) != NULL);

      list_bank = start_list_bank;
      list_program = start_list_program;
      list_name = start_list_name;

      while(list_program != NULL){
	gchar *current_name;
	
	int current_bank, current_program;

	current_bank = GPOINTER_TO_INT(list_bank->data);
	current_program = GPOINTER_TO_INT(list_program->data);
	current_name = list_name->data;
	
	if(current_bank == bank){
	  gtk_list_store_append(program_list_store,
				&tree_iter);

	  gtk_list_store_set(program_list_store, &tree_iter,
			     0, bank,
			     1, current_program,
			     2, current_name,
			     -1);
	}

	list_bank = list_bank->next;
	list_program = list_program->next;
	list_name = list_name->next;
      }
      
      ags_sf2_synth_load_midi_locale(sf2_synth,
				     bank,
				     GPOINTER_TO_INT(start_list_program->data));

      g_list_free(start_list_bank);
      g_list_free(start_list_program);
      g_list_free(start_list_name);
    }
  }
}

void
ags_sf2_synth_load_midi_locale(AgsSF2Synth *sf2_synth,
			       gint bank,
			       gint program)
{
  AgsChannel *start_input;
  AgsChannel *input, *next_input;
  AgsIpatch *ipatch;

  IpatchSF2 *sf2;
  IpatchSF2Preset *sf2_preset;
  IpatchSF2Inst *sf2_instrument;
  IpatchSF2Sample *sf2_sample;
  IpatchItem *pzone;
  IpatchItem *izone;
  IpatchList *pzone_list;
  IpatchList *izone_list;
  
  IpatchIter pzone_iter, izone_iter;

  guint audio_channels;
  
  GError *error;

  start_input = NULL;

  audio_channels = 0;
  
  g_object_get(AGS_MACHINE(sf2_synth)->audio,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);
  
  ipatch = (AgsIpatch *) sf2_synth->audio_container->sound_container;

  error = NULL;
  sf2 = (IpatchSF2 *) ipatch_convert_object_to_type((GObject *) ipatch->handle->file,
						    IPATCH_TYPE_SF2,
						    &error);

  if(error != NULL){
    g_error_free(error);
  }

  sf2_preset = ipatch_sf2_find_preset(sf2,
				      NULL,
				      bank,
				      program,
				      NULL);
  
  if(sf2_preset != NULL){
    pzone_list = ipatch_sf2_preset_get_zones(sf2_preset);

    ipatch_list_init_iter(pzone_list, &pzone_iter);
    
    if(ipatch_iter_first(&pzone_iter) != NULL){      
      do{
	IpatchRange *note_range;

	guint i, j;
	
	pzone = ipatch_iter_get(&pzone_iter);

	g_object_get(pzone,
		     "note-range", &note_range,
		     NULL);
	
	sf2_instrument = (IpatchItem *) ipatch_sf2_pzone_get_inst((IpatchSF2PZone *) ipatch_iter_get(&pzone_iter));
	
	izone_list = ipatch_sf2_inst_get_zones(sf2_instrument);

	i = 0;
	
	if(izone_list != NULL){
	  ipatch_list_init_iter(izone_list, &izone_iter);

	  if(ipatch_iter_first(&izone_iter) != NULL){      
	    do{
	      gdouble *buffer;
	      
	      guint frame_count;
	      guint loop_start, loop_end;
	      
	      izone = ipatch_iter_get(&izone_iter);

	      sf2_sample = ipatch_sf2_izone_get_sample((IpatchSF2IZone *) izone);

	      g_object_get(sf2_sample,
			   "sample-size", &frame_count,
			   "loop-start", &loop_start,
			   "loop-end", &loop_end,
			   NULL);

	      buffer = (gdouble *) g_malloc(frame_count * sizeof(gdouble));
	      
	      error = NULL;
	      ipatch_sample_read_transform(IPATCH_SAMPLE(sf2_sample),
					   0,
					   frame_count,
					   buffer,
					   IPATCH_SAMPLE_DOUBLE | IPATCH_SAMPLE_MONO,
					   IPATCH_SAMPLE_MAP_CHANNEL(0, 0),
					   &error);

	      if(error != NULL){
		g_error_free(error);
	      }

	      input = ags_channel_pad_nth(start_input,
					  note_range->low + i);

	      for(j = 0; j < audio_channels; j++){
		AgsAudioSignal *audio_signal;

		
		//TODO:JK: implement me
		
		/* iterate */
		next_input = ags_channel_next(input);

		if(input != NULL){
		  g_object_unref(input);
		}
		
		input = next_input;
	      }
	      
	      if(input != NULL){
		g_object_unref(input);
	      }

	      i++;
	    }while(ipatch_iter_next(&izone_iter) != NULL);
	  }
	}
      }while(ipatch_iter_next(&pzone_iter) != NULL);
    }
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

/**
 * ags_sf2_synth_sf2_loader_completed_timeout:
 * @sf2_synth: the #AgsSF2Synth
 *
 * Queue draw widget
 *
 * Returns: %TRUE if proceed poll completed, otherwise %FALSE
 *
 * Since: 3.4.0
 */
gboolean
ags_sf2_synth_sf2_loader_completed_timeout(AgsSF2Synth *sf2_synth)
{
  if(g_hash_table_lookup(ags_sf2_synth_sf2_loader_completed,
			 sf2_synth) != NULL){
    if(sf2_synth->sf2_loader != NULL){
      if(ags_sf2_loader_test_flags(sf2_synth->sf2_loader, AGS_SF2_LOADER_HAS_COMPLETED)){
	GtkListStore *bank_list_store;
	
	AgsIpatch *ipatch;

	IpatchSF2 *sf2;
	IpatchItem *ipatch_item;
	IpatchList *ipatch_list;
	
	IpatchIter preset_iter;

	GError *error;
	
	/* reassign audio container */
	sf2_synth->audio_container = sf2_synth->sf2_loader->audio_container;
	sf2_synth->sf2_loader->audio_container = NULL;

	bank_list_store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(sf2_synth->bank_tree_view)));

	gtk_list_store_clear(bank_list_store);
	gtk_list_store_clear(GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(sf2_synth->program_tree_view))));

	ipatch = (AgsIpatch *) sf2_synth->audio_container->sound_container;

	error = NULL;
	sf2 = (IpatchSF2 *) ipatch_convert_object_to_type((GObject *) ipatch->handle->file,
							  IPATCH_TYPE_SF2,
							  &error);

	if(error != NULL){
	  g_error_free(error);
	}
	
	ipatch_list = ipatch_container_get_children((IpatchContainer *) sf2,
						    IPATCH_TYPE_SF2_PRESET);

	if(ipatch_list != NULL){
	  ipatch_list_init_iter(ipatch_list, &preset_iter);
    
	  if(ipatch_iter_first(&preset_iter) != NULL){
	    GList *start_list, *list;

	    GtkTreeIter tree_iter;

	    start_list = NULL;
	    
	    do{	      
	      int bank, program;
	      
	      ipatch_item = ipatch_iter_get(&preset_iter);

	      ipatch_sf2_preset_get_midi_locale((IpatchSF2Preset *) ipatch_item,
						&bank,
						&program);

	      if(g_list_find(start_list, GINT_TO_POINTER(bank)) == NULL){
		start_list = g_list_insert_sorted(start_list,
						  GINT_TO_POINTER(bank),
						  ags_sf2_synth_int_compare_func);
	      }
	    }while(ipatch_iter_next(&preset_iter) != NULL);

	    list = start_list;

	    while(list != NULL){
	      gtk_list_store_append(bank_list_store,
				    &tree_iter);

	      gtk_list_store_set(bank_list_store, &tree_iter,
				 0, GPOINTER_TO_INT(list->data),
				 -1);

	      list = list->next;
	    }

	    ags_sf2_synth_load_bank(sf2_synth,
				    GPOINTER_TO_INT(start_list->data));

	    g_list_free(start_list);
	  }
	}

	/* cleanup */	
	g_object_run_dispose((GObject *) sf2_synth->sf2_loader);
	g_object_unref(sf2_synth->sf2_loader);

	sf2_synth->sf2_loader = NULL;

	sf2_synth->position = -1;

	gtk_spinner_stop(sf2_synth->sf2_loader_spinner);
	gtk_widget_hide((GtkWidget *) sf2_synth->sf2_loader_spinner);
      }else{
	if(sf2_synth->position == -1){
	  sf2_synth->position = 0;

	  gtk_widget_show((GtkWidget *) sf2_synth->sf2_loader_spinner);
	  gtk_spinner_start(sf2_synth->sf2_loader_spinner);
	}
      }
    }
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_sf2_synth_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsSF2Synth
 *
 * Returns: the new #AgsSF2Synth
 *
 * Since: 3.4.0
 */
AgsSF2Synth*
ags_sf2_synth_new(GObject *soundcard)
{
  AgsSF2Synth *sf2_synth;

  sf2_synth = (AgsSF2Synth *) g_object_new(AGS_TYPE_SF2_SYNTH,
					   NULL);

  g_object_set(AGS_MACHINE(sf2_synth)->audio,
	       "output-soundcard", soundcard,
	       NULL);

  return(sf2_synth);
}
