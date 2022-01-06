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

#include <ags/X/machine/ags_fm_hybrid_synth.h>
#include <ags/X/machine/ags_fm_hybrid_synth_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>

#include <math.h>

#include <ags/i18n.h>

void ags_fm_hybrid_synth_class_init(AgsFMHybridSynthClass *fm_hybrid_synth);
void ags_fm_hybrid_synth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_fm_hybrid_synth_init(AgsFMHybridSynth *fm_hybrid_synth);
void ags_fm_hybrid_synth_finalize(GObject *gobject);

void ags_fm_hybrid_synth_connect(AgsConnectable *connectable);
void ags_fm_hybrid_synth_disconnect(AgsConnectable *connectable);

void ags_fm_hybrid_synth_show(GtkWidget *widget);

void ags_fm_hybrid_synth_resize_audio_channels(AgsMachine *machine,
					       guint audio_channels, guint audio_channels_old,
					       gpointer data);
void ags_fm_hybrid_synth_resize_pads(AgsMachine *machine, GType channel_type,
				     guint pads, guint pads_old,
				     gpointer data);

void ags_fm_hybrid_synth_map_recall(AgsMachine *machine);

void ags_fm_hybrid_synth_input_map_recall(AgsFMHybridSynth *fm_hybrid_synth,
					  guint audio_channel_start,
					  guint input_pad_start);
void ags_fm_hybrid_synth_output_map_recall(AgsFMHybridSynth *fm_hybrid_synth,
					   guint audio_channel_start,
					   guint output_pad_start);

/**
 * SECTION:ags_fm_hybrid_synth
 * @short_description: FM hybrid synth
 * @title: AgsFMHybridSynth
 * @section_id:
 * @include: ags/X/machine/ags_fm_hybrid_synth.h
 *
 * The #AgsFMHybridSynth is a composite widget to act as FM hybrid synth.
 */

static gpointer ags_fm_hybrid_synth_parent_class = NULL;
static AgsConnectableInterface *ags_fm_hybrid_synth_parent_connectable_interface;

GType
ags_fm_hybrid_synth_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fm_hybrid_synth = 0;

    static const GTypeInfo ags_fm_hybrid_synth_info = {
      sizeof(AgsFMHybridSynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fm_hybrid_synth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFMHybridSynth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fm_hybrid_synth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_fm_hybrid_synth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_fm_hybrid_synth = g_type_register_static(AGS_TYPE_MACHINE,
						      "AgsFMHybridSynth", &ags_fm_hybrid_synth_info,
						      0);
    
    g_type_add_interface_static(ags_type_fm_hybrid_synth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fm_hybrid_synth);
  }

  return g_define_type_id__volatile;
}

void
ags_fm_hybrid_synth_class_init(AgsFMHybridSynthClass *fm_hybrid_synth)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_fm_hybrid_synth_parent_class = g_type_class_peek_parent(fm_hybrid_synth);

  /* GObjectClass */
  gobject = (GObjectClass *) fm_hybrid_synth;

  gobject->finalize = ags_fm_hybrid_synth_finalize;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) fm_hybrid_synth;

  machine->map_recall = ags_fm_hybrid_synth_map_recall;
}

void
ags_fm_hybrid_synth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_fm_hybrid_synth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_fm_hybrid_synth_connect;
  connectable->disconnect = ags_fm_hybrid_synth_disconnect;
}

void
ags_fm_hybrid_synth_init(AgsFMHybridSynth *fm_hybrid_synth)
{
  GtkBox *vbox;
  GtkBox *hbox;
  GtkBox *osc_vbox;
  GtkGrid *synth_0_grid;
  GtkGrid *synth_1_grid;
  GtkGrid *misc_grid;
  GtkBox *band_hbox;
  GtkGrid *low_pass_grid;
  GtkGrid *high_pass_grid;
  GtkGrid *chorus_grid;
  GtkFrame *frame;
  GtkLabel *label;

  GtkAdjustment *adjustment;
  
  AgsAudio *audio;

  AgsApplicationContext *application_context;   

  gdouble gui_scale_factor;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  g_signal_connect_after((GObject *) fm_hybrid_synth, "parent_set",
			 G_CALLBACK(ags_fm_hybrid_synth_parent_set_callback), (gpointer) fm_hybrid_synth);
  
  audio = AGS_MACHINE(fm_hybrid_synth)->audio;
  ags_audio_set_flags(audio, (AGS_AUDIO_SYNC |
			      AGS_AUDIO_ASYNC |
			      AGS_AUDIO_OUTPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_SYNTH));
  ags_audio_set_ability_flags(audio, (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION));
  ags_audio_set_behaviour_flags(audio, (AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING |
					AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT));
  g_object_set(audio,
	       "min-audio-channels", 1,
	       "max-audio-channels", 1,
	       "min-output-pads", 1,
	       "min-input-pads", 1,
	       "max-input-pads", 128,
	       "audio-start-mapping", 0,
	       "audio-end-mapping", 128,
	       "midi-start-mapping", 0,
	       "midi-end-mapping", 128,
	       NULL);

  AGS_MACHINE(fm_hybrid_synth)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
					  AGS_MACHINE_REVERSE_NOTATION);
  AGS_MACHINE(fm_hybrid_synth)->mapping_flags |= AGS_MACHINE_MONO;

  AGS_MACHINE(fm_hybrid_synth)->input_pad_type = G_TYPE_NONE;
  AGS_MACHINE(fm_hybrid_synth)->input_line_type = G_TYPE_NONE;
  AGS_MACHINE(fm_hybrid_synth)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(fm_hybrid_synth)->output_line_type = G_TYPE_NONE;

  /* context menu */
  ags_machine_popup_add_connection_options((AgsMachine *) fm_hybrid_synth,
  					   (AGS_MACHINE_POPUP_MIDI_DIALOG));

  /* audio resize */
  g_signal_connect_after(G_OBJECT(fm_hybrid_synth), "resize-audio-channels",
			 G_CALLBACK(ags_fm_hybrid_synth_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(fm_hybrid_synth), "resize-pads",
			 G_CALLBACK(ags_fm_hybrid_synth_resize_pads), NULL);
  
  /* create widgets */
  fm_hybrid_synth->flags = 0;

  /* mapped IO */
  fm_hybrid_synth->mapped_input_pad = 0;
  fm_hybrid_synth->mapped_output_pad = 0;

  fm_hybrid_synth->playback_play_container = ags_recall_container_new();
  fm_hybrid_synth->playback_recall_container = ags_recall_container_new();

  fm_hybrid_synth->fm_synth_play_container = ags_recall_container_new();
  fm_hybrid_synth->fm_synth_recall_container = ags_recall_container_new();

  fm_hybrid_synth->volume_play_container = ags_recall_container_new();
  fm_hybrid_synth->volume_recall_container = ags_recall_container_new();

  fm_hybrid_synth->envelope_play_container = ags_recall_container_new();
  fm_hybrid_synth->envelope_recall_container = ags_recall_container_new();

  fm_hybrid_synth->buffer_play_container = ags_recall_container_new();
  fm_hybrid_synth->buffer_recall_container = ags_recall_container_new();
 
  /* context menu */
  ags_machine_popup_add_edit_options((AgsMachine *) fm_hybrid_synth,
				     (AGS_MACHINE_POPUP_ENVELOPE));
  
  /* name and xml type */
  fm_hybrid_synth->name = NULL;
  fm_hybrid_synth->xml_type = "ags-fm_hybrid-synth"; 

  /* widget */
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);
  gtk_container_add((GtkContainer *) gtk_bin_get_child((GtkBin *) fm_hybrid_synth),
		    (GtkWidget *) vbox);

  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_container_add((GtkContainer *) vbox,
		    (GtkWidget *) hbox);

  /* oscillator */
  osc_vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				    0);
  gtk_container_add((GtkContainer *) hbox,
		    (GtkWidget *) osc_vbox);

  /* grid */
  synth_0_grid = (GtkGrid *) gtk_grid_new();
  gtk_container_add((GtkContainer *) osc_vbox,
		    (GtkWidget *) synth_0_grid);

  /* OSC 1 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  fm_hybrid_synth->synth_0_oscillator = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(fm_hybrid_synth->synth_0_oscillator,
				 "sine");
  gtk_combo_box_text_append_text(fm_hybrid_synth->synth_0_oscillator,
				 "sawtooth");
  gtk_combo_box_text_append_text(fm_hybrid_synth->synth_0_oscillator,
				 "triangle");
  gtk_combo_box_text_append_text(fm_hybrid_synth->synth_0_oscillator,
				 "square");
  gtk_combo_box_text_append_text(fm_hybrid_synth->synth_0_oscillator,
				 "impulse");

  gtk_combo_box_set_active(fm_hybrid_synth->synth_0_oscillator,
			   0);

  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) fm_hybrid_synth->synth_0_oscillator,
		  1, 0,
		  1, 1);

  /* OSC 1 - octave */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - octave"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  2, 0,
		  1, 1);

  fm_hybrid_synth->synth_0_octave = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(fm_hybrid_synth->synth_0_octave);

  gtk_adjustment_set_lower(adjustment,
			   -6.0);
  gtk_adjustment_set_upper(adjustment,
			   6.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);

//  fm_hybrid_synth->synth_0_octave->scale_max_precision = 12;
  
//  ags_dial_set_scale_precision(fm_hybrid_synth->synth_0_octave,
//			       12);
  ags_dial_set_radius(fm_hybrid_synth->synth_0_octave,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) fm_hybrid_synth->synth_0_octave,
		  3, 0,
		  1, 1);

  /* OSC 1 - key */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - key"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  4, 0,
		  1, 1);

  fm_hybrid_synth->synth_0_key = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(fm_hybrid_synth->synth_0_key);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   2.0);

//  fm_hybrid_synth->synth_0_key->scale_max_precision = 12;
//  ags_dial_set_scale_precision(fm_hybrid_synth->synth_0_key,
//			       12);
  ags_dial_set_radius(fm_hybrid_synth->synth_0_key,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) fm_hybrid_synth->synth_0_key,
		  5, 0,
		  1, 1);
  
  /* OSC 1 - phase */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - phase"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  2, 1,
		  1, 1);

  fm_hybrid_synth->synth_0_phase = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(fm_hybrid_synth->synth_0_phase);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   2.0 * M_PI);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(fm_hybrid_synth->synth_0_phase,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) fm_hybrid_synth->synth_0_phase,
		  3, 1,
		  1, 1);

  /* OSC 1 - volume */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - volume"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  4, 1,
		  1, 1);

  fm_hybrid_synth->synth_0_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(fm_hybrid_synth->synth_0_volume);

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

  ags_dial_set_radius(fm_hybrid_synth->synth_0_volume,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) fm_hybrid_synth->synth_0_volume,
		  5, 1,
		  1, 1);

  //TODO:JK: implement me
  
  /* grid */
  misc_grid = (GtkGrid *) gtk_grid_new();
  gtk_container_add((GtkContainer *) vbox,
		    (GtkWidget *) misc_grid);

  /* sequencer */
  fm_hybrid_synth->sequencer_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("sequencer enabled"));
  gtk_grid_attach(misc_grid,
		  (GtkWidget *) fm_hybrid_synth->sequencer_enabled,
		  0, 0,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("sequencer sign"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(misc_grid,
		  (GtkWidget *) label,
		  2, 0,
		  1, 1);
  
  fm_hybrid_synth->sequencer_sign = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(fm_hybrid_synth->sequencer_sign,
				 "A");
  gtk_combo_box_text_append_text(fm_hybrid_synth->sequencer_sign,
				 "A#");
  gtk_combo_box_text_append_text(fm_hybrid_synth->sequencer_sign,
				 "H");
  gtk_combo_box_text_append_text(fm_hybrid_synth->sequencer_sign,
				 "C");
  gtk_combo_box_text_append_text(fm_hybrid_synth->sequencer_sign,
				 "C#");
  gtk_combo_box_text_append_text(fm_hybrid_synth->sequencer_sign,
				 "D");
  gtk_combo_box_text_append_text(fm_hybrid_synth->sequencer_sign,
				 "D#");
  gtk_combo_box_text_append_text(fm_hybrid_synth->sequencer_sign,
				 "E");
  gtk_combo_box_text_append_text(fm_hybrid_synth->sequencer_sign,
				 "F");
  gtk_combo_box_text_append_text(fm_hybrid_synth->sequencer_sign,
				 "F#");
  gtk_combo_box_text_append_text(fm_hybrid_synth->sequencer_sign,
				 "G");
  gtk_combo_box_text_append_text(fm_hybrid_synth->sequencer_sign,
				 "G#");

  gtk_combo_box_set_active(fm_hybrid_synth->sequencer_sign,
			   0);

  gtk_grid_attach(misc_grid,
		  (GtkWidget *) fm_hybrid_synth->sequencer_sign,
		  3, 0,
		  1, 1);

  /* pitch */
  label = (GtkLabel *) gtk_label_new(i18n("pitch tuning"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(misc_grid,
		  (GtkWidget *) label,
		  4, 0,
		  1, 1);

  fm_hybrid_synth->pitch_tuning = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(fm_hybrid_synth->pitch_tuning);

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

  ags_dial_set_radius(fm_hybrid_synth->pitch_tuning,
		      12);
  
  gtk_grid_attach(misc_grid,
		  (GtkWidget *) fm_hybrid_synth->pitch_tuning,
		  5, 0,
		  1, 1);
  
  /* noise */
  label = (GtkLabel *) gtk_label_new(i18n("noise gain"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(misc_grid,
		  (GtkWidget *) label,
		  6, 0,
		  1, 1);

  fm_hybrid_synth->noise_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(fm_hybrid_synth->noise_gain);

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

  ags_dial_set_radius(fm_hybrid_synth->noise_gain,
		      12);
  
  gtk_grid_attach(misc_grid,
		  (GtkWidget *) fm_hybrid_synth->noise_gain,
		  7, 0,
		  1, 1);

  /* band */
  band_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				     0);
  gtk_container_add((GtkContainer *) vbox,
		    (GtkWidget *) band_hbox);

  /* low pass grid */
  low_pass_grid = (GtkGrid *) gtk_grid_new();
  gtk_container_add((GtkContainer *) band_hbox,
		    (GtkWidget *) low_pass_grid);

  fm_hybrid_synth->low_pass_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("low pass enabled"));
  gtk_grid_attach(low_pass_grid,
		  (GtkWidget *) fm_hybrid_synth->low_pass_enabled,
		  0, 0,
		  1, 1);

  /* low pass q-lin */
  label = (GtkLabel *) gtk_label_new(i18n("low pass q-lin"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(low_pass_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  fm_hybrid_synth->low_pass_q_lin = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(fm_hybrid_synth->low_pass_q_lin);

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

  ags_dial_set_radius(fm_hybrid_synth->low_pass_q_lin,
		      12);
  
  gtk_grid_attach(low_pass_grid,
		  (GtkWidget *) fm_hybrid_synth->low_pass_q_lin,
		  1, 1,
		  1, 1);

  /* low pass filter gain */
  label = (GtkLabel *) gtk_label_new(i18n("low pass filter gain"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(low_pass_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);

  fm_hybrid_synth->low_pass_filter_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(fm_hybrid_synth->low_pass_filter_gain);

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

  ags_dial_set_radius(fm_hybrid_synth->low_pass_filter_gain,
		      12);
  
  gtk_grid_attach(low_pass_grid,
		  (GtkWidget *) fm_hybrid_synth->low_pass_filter_gain,
		  1, 2,
		  1, 1);

  /* high pass grid */
  high_pass_grid = (GtkGrid *) gtk_grid_new();
  gtk_container_add((GtkContainer *) band_hbox,
		    (GtkWidget *) high_pass_grid);

  fm_hybrid_synth->high_pass_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("high pass enabled"));
  gtk_grid_attach(high_pass_grid,
		  (GtkWidget *) fm_hybrid_synth->high_pass_enabled,
		  0, 0,
		  1, 1);

  /* high pass q-lin */
  label = (GtkLabel *) gtk_label_new(i18n("high pass q-lin"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(high_pass_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  fm_hybrid_synth->high_pass_q_lin = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(fm_hybrid_synth->high_pass_q_lin);

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

  ags_dial_set_radius(fm_hybrid_synth->high_pass_q_lin,
		      12);
  
  gtk_grid_attach(high_pass_grid,
		  (GtkWidget *) fm_hybrid_synth->high_pass_q_lin,
		  1, 1,
		  1, 1);

  /* high pass filter gain */
  label = (GtkLabel *) gtk_label_new(i18n("high pass filter gain"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(high_pass_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);

  fm_hybrid_synth->high_pass_filter_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(fm_hybrid_synth->high_pass_filter_gain);

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

  ags_dial_set_radius(fm_hybrid_synth->high_pass_filter_gain,
		      12);
  
  gtk_grid_attach(high_pass_grid,
		  (GtkWidget *) fm_hybrid_synth->high_pass_filter_gain,
		  1, 2,
		  1, 1);

  /* chorus grid */
  chorus_grid = (GtkGrid *) gtk_grid_new();
  gtk_container_add((GtkContainer *) vbox,
		    (GtkWidget *) chorus_grid);

  /* chorus input volume */
  label = (GtkLabel *) gtk_label_new(i18n("chorus input volume"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);

  fm_hybrid_synth->chorus_input_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(fm_hybrid_synth->chorus_input_volume);

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

  ags_dial_set_radius(fm_hybrid_synth->chorus_input_volume,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) fm_hybrid_synth->chorus_input_volume,
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

  fm_hybrid_synth->chorus_output_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(fm_hybrid_synth->chorus_output_volume);

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

  ags_dial_set_radius(fm_hybrid_synth->chorus_output_volume,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) fm_hybrid_synth->chorus_output_volume,
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
  
  fm_hybrid_synth->chorus_lfo_oscillator = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(fm_hybrid_synth->chorus_lfo_oscillator,
				 "sine");
  gtk_combo_box_text_append_text(fm_hybrid_synth->chorus_lfo_oscillator,
				 "sawtooth");
  gtk_combo_box_text_append_text(fm_hybrid_synth->chorus_lfo_oscillator,
				 "triangle");
  gtk_combo_box_text_append_text(fm_hybrid_synth->chorus_lfo_oscillator,
				 "square");
  gtk_combo_box_text_append_text(fm_hybrid_synth->chorus_lfo_oscillator,
				 "impulse");

  gtk_combo_box_set_active(fm_hybrid_synth->chorus_lfo_oscillator,
			   0);

  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) fm_hybrid_synth->chorus_lfo_oscillator,
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

  fm_hybrid_synth->chorus_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 10.0, 0.01);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) fm_hybrid_synth->chorus_lfo_frequency,
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

  fm_hybrid_synth->chorus_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(fm_hybrid_synth->chorus_depth);

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

  ags_dial_set_radius(fm_hybrid_synth->chorus_depth,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) fm_hybrid_synth->chorus_depth,
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

  fm_hybrid_synth->chorus_mix = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(fm_hybrid_synth->chorus_mix);

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

  ags_dial_set_radius(fm_hybrid_synth->chorus_mix,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) fm_hybrid_synth->chorus_mix,
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

  fm_hybrid_synth->chorus_delay = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(fm_hybrid_synth->chorus_delay);

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

  ags_dial_set_radius(fm_hybrid_synth->chorus_delay,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) fm_hybrid_synth->chorus_delay,
		  5, 2,
		  1, 1);

  //TODO:JK: implement me
}

void
ags_fm_hybrid_synth_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_fm_hybrid_synth_parent_class)->finalize(gobject);
}

void
ags_fm_hybrid_synth_connect(AgsConnectable *connectable)
{
  AgsFMHybridSynth *fm_hybrid_synth;
  
  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_fm_hybrid_synth_parent_connectable_interface->connect(connectable);

  /* AgsFMHybridSynth */
  fm_hybrid_synth = AGS_FM_HYBRID_SYNTH(connectable);

  g_signal_connect_after(fm_hybrid_synth->synth_0_oscillator, "changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_0_oscillator_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->synth_0_octave, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_0_octave_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->synth_0_key, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_0_key_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->synth_0_phase, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_0_phase_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->synth_0_volume, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_0_volume_callback), fm_hybrid_synth);
    
  g_signal_connect_after(fm_hybrid_synth->synth_0_lfo_oscillator, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_0_lfo_oscillator_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->synth_0_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_0_lfo_frequency_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->synth_0_lfo_depth, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_0_lfo_depth_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->synth_0_lfo_tuning, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_0_lfo_tuning_callback), fm_hybrid_synth);

  g_signal_connect_after(fm_hybrid_synth->synth_1_oscillator, "changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_1_oscillator_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->synth_1_octave, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_1_octave_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->synth_1_key, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_1_key_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->synth_1_phase, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_1_phase_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->synth_1_volume, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_1_volume_callback), fm_hybrid_synth);
    
  g_signal_connect_after(fm_hybrid_synth->synth_1_lfo_oscillator, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_1_lfo_oscillator_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->synth_1_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_1_lfo_frequency_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->synth_1_lfo_depth, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_1_lfo_depth_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->synth_1_lfo_tuning, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_1_lfo_tuning_callback), fm_hybrid_synth);

  g_signal_connect_after(fm_hybrid_synth->synth_2_oscillator, "changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_2_oscillator_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->synth_2_octave, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_2_octave_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->synth_2_key, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_2_key_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->synth_2_phase, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_2_phase_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->synth_2_volume, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_2_volume_callback), fm_hybrid_synth);
    
  g_signal_connect_after(fm_hybrid_synth->synth_2_lfo_oscillator, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_2_lfo_oscillator_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->synth_2_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_2_lfo_frequency_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->synth_2_lfo_depth, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_2_lfo_depth_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->synth_2_lfo_tuning, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_synth_2_lfo_tuning_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->sequencer_enabled, "clicked",
			 G_CALLBACK(ags_fm_hybrid_synth_sequencer_enabled_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->sequencer_sign, "changed",
			 G_CALLBACK(ags_fm_hybrid_synth_sequencer_sign_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->pitch_tuning, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_pitch_tuning_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->noise_gain, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_noise_gain_callback), fm_hybrid_synth);
    
//  g_signal_connect_after(fm_hybrid_synth->chorus_enabled, "clicked",
//			 G_CALLBACK(ags_fm_hybrid_synth_chorus_enabled_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->chorus_input_volume, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_chorus_input_volume_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->chorus_output_volume, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_chorus_output_volume_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->chorus_lfo_oscillator, "changed",
			 G_CALLBACK(ags_fm_hybrid_synth_chorus_lfo_oscillator_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->chorus_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_chorus_lfo_frequency_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->chorus_depth, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_chorus_depth_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->chorus_mix, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_chorus_mix_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->chorus_delay, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_chorus_delay_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->low_pass_enabled, "clicked",
			 G_CALLBACK(ags_fm_hybrid_synth_low_pass_enabled_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->low_pass_q_lin, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_low_pass_q_lin_callback), fm_hybrid_synth);
    
  g_signal_connect_after(fm_hybrid_synth->low_pass_filter_gain, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_low_pass_filter_gain_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->high_pass_enabled, "clicked",
			 G_CALLBACK(ags_fm_hybrid_synth_high_pass_enabled_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->high_pass_q_lin, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_high_pass_q_lin_callback), fm_hybrid_synth);
  
  g_signal_connect_after(fm_hybrid_synth->high_pass_filter_gain, "value-changed",
			 G_CALLBACK(ags_fm_hybrid_synth_high_pass_filter_gain_callback), fm_hybrid_synth);
}

void
ags_fm_hybrid_synth_disconnect(AgsConnectable *connectable)
{
  AgsFMHybridSynth *fm_hybrid_synth;
  
  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  ags_fm_hybrid_synth_parent_connectable_interface->disconnect(connectable);

  /* AgsFMHybridSynth */
  fm_hybrid_synth = AGS_FM_HYBRID_SYNTH(connectable);

  //TODO:JK: implement me
}

void
ags_fm_hybrid_synth_resize_audio_channels(AgsMachine *machine,
					  guint audio_channels, guint audio_channels_old,
					  gpointer data)
{
  AgsFMHybridSynth *fm_hybrid_synth;

  fm_hybrid_synth = (AgsFMHybridSynth *) machine;

  if(audio_channels > audio_channels_old){    
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_fm_hybrid_synth_input_map_recall(fm_hybrid_synth,
					   audio_channels_old,
					   0);
      
      ags_fm_hybrid_synth_output_map_recall(fm_hybrid_synth,
					    audio_channels_old,
					    0);
    }
  }
}

void
ags_fm_hybrid_synth_resize_pads(AgsMachine *machine, GType type,
				guint pads, guint pads_old,
				gpointer data)
{
  AgsFMHybridSynth *fm_hybrid_synth;

  gboolean grow;

  fm_hybrid_synth = (AgsFMHybridSynth *) machine;
  
  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(type == AGS_TYPE_INPUT){
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	/* depending on destination */
	ags_fm_hybrid_synth_input_map_recall(fm_hybrid_synth,
					     0,
					     pads_old);
      }
    }else{
      fm_hybrid_synth->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_fm_hybrid_synth_output_map_recall(fm_hybrid_synth,
					      0,
					      pads_old);
      }
    }else{
      fm_hybrid_synth->mapped_output_pad = pads;
    }
  }
}

void
ags_fm_hybrid_synth_map_recall(AgsMachine *machine)
{
  AgsFMHybridSynth *fm_hybrid_synth;
  
  AgsAudio *audio;

  GList *start_recall, *recall;

  gint position;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  fm_hybrid_synth = AGS_FM_HYBRID_SYNTH(machine);

  audio = machine->audio;
  
  position = 0;
  
  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       fm_hybrid_synth->playback_play_container, fm_hybrid_synth->playback_recall_container,
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

  /* ags-fx-synth */
  start_recall = ags_fx_factory_create(audio,
				       fm_hybrid_synth->fm_synth_play_container, fm_hybrid_synth->fm_synth_recall_container,
				       "ags-fx-fm-synth",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-volume */
  start_recall = ags_fx_factory_create(audio,
				       fm_hybrid_synth->volume_play_container, fm_hybrid_synth->volume_recall_container,
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
				       fm_hybrid_synth->envelope_play_container, fm_hybrid_synth->envelope_recall_container,
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
				       fm_hybrid_synth->buffer_play_container, fm_hybrid_synth->buffer_recall_container,
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
  ags_fm_hybrid_synth_input_map_recall(fm_hybrid_synth,
				       0,
				       0);

  /* depending on destination */
  ags_fm_hybrid_synth_output_map_recall(fm_hybrid_synth,
					0,
					0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_fm_hybrid_synth_parent_class)->map_recall(machine);  
}

void
ags_fm_hybrid_synth_input_map_recall(AgsFMHybridSynth *fm_hybrid_synth,
				     guint audio_channel_start,
				     guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(fm_hybrid_synth->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(fm_hybrid_synth)->audio;

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
				       fm_hybrid_synth->playback_play_container, fm_hybrid_synth->playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-synth */
  start_recall = ags_fx_factory_create(audio,
				       fm_hybrid_synth->fm_synth_play_container, fm_hybrid_synth->fm_synth_recall_container,
				       "ags-fx-fm-synth",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-volume */
  start_recall = ags_fx_factory_create(audio,
				       fm_hybrid_synth->volume_play_container, fm_hybrid_synth->volume_recall_container,
				       "ags-fx-volume",
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
				       fm_hybrid_synth->envelope_play_container, fm_hybrid_synth->envelope_recall_container,
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
				       fm_hybrid_synth->buffer_play_container, fm_hybrid_synth->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  fm_hybrid_synth->mapped_input_pad = input_pads;
}

void
ags_fm_hybrid_synth_output_map_recall(AgsFMHybridSynth *fm_hybrid_synth,
				      guint audio_channel_start,
				      guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;

  if(fm_hybrid_synth->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(fm_hybrid_synth)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);
  
  fm_hybrid_synth->mapped_output_pad = output_pads;
}

/**
 * ags_fm_hybrid_synth_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsFMHybridSynth
 *
 * Returns: the new #AgsFMHybridSynth
 *
 * Since: 3.15.0
 */
AgsFMHybridSynth*
ags_fm_hybrid_synth_new(GObject *soundcard)
{
  AgsFMHybridSynth *fm_hybrid_synth;

  fm_hybrid_synth = (AgsFMHybridSynth *) g_object_new(AGS_TYPE_FM_HYBRID_SYNTH,
						      NULL);

  g_object_set(AGS_MACHINE(fm_hybrid_synth)->audio,
	       "output-soundcard", soundcard,
	       NULL);

  return(fm_hybrid_synth);
}
