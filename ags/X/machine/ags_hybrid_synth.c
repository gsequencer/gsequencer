/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/X/machine/ags_hybrid_synth.h>
#include <ags/X/machine/ags_hybrid_synth_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>

#include <math.h>

#include <ags/i18n.h>

void ags_hybrid_synth_class_init(AgsHybridSynthClass *hybrid_synth);
void ags_hybrid_synth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_hybrid_synth_init(AgsHybridSynth *hybrid_synth);
void ags_hybrid_synth_finalize(GObject *gobject);

void ags_hybrid_synth_connect(AgsConnectable *connectable);
void ags_hybrid_synth_disconnect(AgsConnectable *connectable);

void ags_hybrid_synth_show(GtkWidget *widget);

void ags_hybrid_synth_resize_audio_channels(AgsMachine *machine,
					    guint audio_channels, guint audio_channels_old,
					    gpointer data);
void ags_hybrid_synth_resize_pads(AgsMachine *machine, GType channel_type,
				  guint pads, guint pads_old,
				  gpointer data);

void ags_hybrid_synth_map_recall(AgsMachine *machine);

void ags_hybrid_synth_input_map_recall(AgsHybridSynth *hybrid_synth,
				       guint audio_channel_start,
				       guint input_pad_start);
void ags_hybrid_synth_output_map_recall(AgsHybridSynth *hybrid_synth,
					guint audio_channel_start,
					guint output_pad_start);

/**
 * SECTION:ags_hybrid_synth
 * @short_description: hybrid synth
 * @title: AgsHybridSynth
 * @section_id:
 * @include: ags/X/machine/ags_hybrid_synth.h
 *
 * The #AgsHybridSynth is a composite widget to act as hybrid synth.
 */

static gpointer ags_hybrid_synth_parent_class = NULL;
static AgsConnectableInterface *ags_hybrid_synth_parent_connectable_interface;

GType
ags_hybrid_synth_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_hybrid_synth = 0;

    static const GTypeInfo ags_hybrid_synth_info = {
      sizeof(AgsHybridSynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_hybrid_synth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsHybridSynth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_hybrid_synth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_hybrid_synth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_hybrid_synth = g_type_register_static(AGS_TYPE_MACHINE,
						   "AgsHybridSynth", &ags_hybrid_synth_info,
						   0);
    
    g_type_add_interface_static(ags_type_hybrid_synth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_hybrid_synth);
  }

  return g_define_type_id__volatile;
}

void
ags_hybrid_synth_class_init(AgsHybridSynthClass *hybrid_synth)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_hybrid_synth_parent_class = g_type_class_peek_parent(hybrid_synth);

  /* GObjectClass */
  gobject = (GObjectClass *) hybrid_synth;

  gobject->finalize = ags_hybrid_synth_finalize;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) hybrid_synth;

  machine->map_recall = ags_hybrid_synth_map_recall;
}

void
ags_hybrid_synth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_hybrid_synth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_hybrid_synth_connect;
  connectable->disconnect = ags_hybrid_synth_disconnect;
}

void
ags_hybrid_synth_init(AgsHybridSynth *hybrid_synth)
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

  g_signal_connect_after((GObject *) hybrid_synth, "parent_set",
			 G_CALLBACK(ags_hybrid_synth_parent_set_callback), (gpointer) hybrid_synth);
  
  audio = AGS_MACHINE(hybrid_synth)->audio;
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

  AGS_MACHINE(hybrid_synth)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
				       AGS_MACHINE_REVERSE_NOTATION);
  AGS_MACHINE(hybrid_synth)->mapping_flags |= AGS_MACHINE_MONO;

  AGS_MACHINE(hybrid_synth)->input_pad_type = G_TYPE_NONE;
  AGS_MACHINE(hybrid_synth)->input_line_type = G_TYPE_NONE;
  AGS_MACHINE(hybrid_synth)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(hybrid_synth)->output_line_type = G_TYPE_NONE;

  /* context menu */
  ags_machine_popup_add_connection_options((AgsMachine *) hybrid_synth,
  					   (AGS_MACHINE_POPUP_MIDI_DIALOG));

  /* audio resize */
  g_signal_connect_after(G_OBJECT(hybrid_synth), "resize-audio-channels",
			 G_CALLBACK(ags_hybrid_synth_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(hybrid_synth), "resize-pads",
			 G_CALLBACK(ags_hybrid_synth_resize_pads), NULL);
  
  /* create widgets */
  hybrid_synth->flags = 0;

  /* mapped IO */
  hybrid_synth->mapped_input_pad = 0;
  hybrid_synth->mapped_output_pad = 0;

  hybrid_synth->playback_play_container = ags_recall_container_new();
  hybrid_synth->playback_recall_container = ags_recall_container_new();

  hybrid_synth->synth_play_container = ags_recall_container_new();
  hybrid_synth->synth_recall_container = ags_recall_container_new();

  hybrid_synth->volume_play_container = ags_recall_container_new();
  hybrid_synth->volume_recall_container = ags_recall_container_new();

  hybrid_synth->envelope_play_container = ags_recall_container_new();
  hybrid_synth->envelope_recall_container = ags_recall_container_new();

  hybrid_synth->buffer_play_container = ags_recall_container_new();
  hybrid_synth->buffer_recall_container = ags_recall_container_new();
 
  /* context menu */
  ags_machine_popup_add_edit_options((AgsMachine *) hybrid_synth,
				     (AGS_MACHINE_POPUP_ENVELOPE));
  
  /* name and xml type */
  hybrid_synth->name = NULL;
  hybrid_synth->xml_type = "ags-hybrid-synth"; 

  /* widget */
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);
  gtk_container_add((GtkContainer *) gtk_bin_get_child((GtkBin *) hybrid_synth),
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
  
  hybrid_synth->synth_0_oscillator = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(hybrid_synth->synth_0_oscillator,
				 "sine");
  gtk_combo_box_text_append_text(hybrid_synth->synth_0_oscillator,
				 "sawtooth");
  gtk_combo_box_text_append_text(hybrid_synth->synth_0_oscillator,
				 "triangle");
  gtk_combo_box_text_append_text(hybrid_synth->synth_0_oscillator,
				 "square");
  gtk_combo_box_text_append_text(hybrid_synth->synth_0_oscillator,
				 "impulse");

  gtk_combo_box_set_active(hybrid_synth->synth_0_oscillator,
			   0);

  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) hybrid_synth->synth_0_oscillator,
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

  hybrid_synth->synth_0_octave = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->synth_0_octave);

  gtk_adjustment_set_lower(adjustment,
			   -6.0);
  gtk_adjustment_set_upper(adjustment,
			   6.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);

//  hybrid_synth->synth_0_octave->scale_max_precision = 12;
  
//  ags_dial_set_scale_precision(hybrid_synth->synth_0_octave,
//			       12);
  ags_dial_set_radius(hybrid_synth->synth_0_octave,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) hybrid_synth->synth_0_octave,
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

  hybrid_synth->synth_0_key = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->synth_0_key);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   2.0);

//  hybrid_synth->synth_0_key->scale_max_precision = 12;
//  ags_dial_set_scale_precision(hybrid_synth->synth_0_key,
//			       12);
  ags_dial_set_radius(hybrid_synth->synth_0_key,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) hybrid_synth->synth_0_key,
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

  hybrid_synth->synth_0_phase = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->synth_0_phase);

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

  ags_dial_set_radius(hybrid_synth->synth_0_phase,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) hybrid_synth->synth_0_phase,
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

  hybrid_synth->synth_0_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->synth_0_volume);

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

  ags_dial_set_radius(hybrid_synth->synth_0_volume,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) hybrid_synth->synth_0_volume,
		  5, 1,
		  1, 1);

  /* OSC 1 - sync enabled */
  hybrid_synth->synth_0_sync_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("OSC 1 - sync enabled"));
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) hybrid_synth->synth_0_sync_enabled,
		  6, 0,
		  1, 1);

  /* OSC 1 - sync relative attack factor */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync factor"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  6, 1,
		  1, 1);
  
  hybrid_synth->synth_0_sync_relative_attack_factor = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->synth_0_sync_relative_attack_factor);

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

  ags_dial_set_radius(hybrid_synth->synth_0_sync_relative_attack_factor,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) hybrid_synth->synth_0_sync_relative_attack_factor,
		  7, 1,
		  1, 1);

  /* OSC 1 - sync attack 0 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync attack 0"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  10, 0,
		  1, 1);
  
  hybrid_synth->synth_0_sync_attack_0 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->synth_0_sync_attack_0);

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

  ags_dial_set_radius(hybrid_synth->synth_0_sync_attack_0,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) hybrid_synth->synth_0_sync_attack_0,
		  11, 0,
		  1, 1);

  /* OSC 1 - sync phase 0 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync phase 0"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  12, 0,
		  1, 1);
  
  hybrid_synth->synth_0_sync_phase_0 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->synth_0_sync_phase_0);

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

  ags_dial_set_radius(hybrid_synth->synth_0_sync_phase_0,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) hybrid_synth->synth_0_sync_phase_0,
		  13, 0,
		  1, 1);

  /* OSC 1 - sync attack 1 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync attack 1"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  10, 1,
		  1, 1);
  
  hybrid_synth->synth_0_sync_attack_1 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->synth_0_sync_attack_1);

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

  ags_dial_set_radius(hybrid_synth->synth_0_sync_attack_1,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) hybrid_synth->synth_0_sync_attack_1,
		  11, 1,
		  1, 1);

  /* OSC 1 - sync phase 1 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync phase 1"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  12, 1,
		  1, 1);
  
  hybrid_synth->synth_0_sync_phase_1 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->synth_0_sync_phase_1);

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

  ags_dial_set_radius(hybrid_synth->synth_0_sync_phase_1,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) hybrid_synth->synth_0_sync_phase_1,
		  13, 1,
		  1, 1);

  /* OSC 1 - sync attack 2 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync attack 2"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  10, 2,
		  1, 1);
  
  hybrid_synth->synth_0_sync_attack_2 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->synth_0_sync_attack_2);

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

  ags_dial_set_radius(hybrid_synth->synth_0_sync_attack_2,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) hybrid_synth->synth_0_sync_attack_2,
		  11, 2,
		  1, 1);

  /* OSC 1 - sync phase 2 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync phase 2"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  12, 2,
		  1, 1);
  
  hybrid_synth->synth_0_sync_phase_2 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->synth_0_sync_phase_2);

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

  ags_dial_set_radius(hybrid_synth->synth_0_sync_phase_2,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) hybrid_synth->synth_0_sync_phase_2,
		  13, 2,
		  1, 1);

  /* OSC 1 LFO */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 LFO"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  14, 0,
		  1, 1);
  
  hybrid_synth->synth_0_sync_lfo_oscillator = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(hybrid_synth->synth_0_sync_lfo_oscillator,
				 "sine");
  gtk_combo_box_text_append_text(hybrid_synth->synth_0_sync_lfo_oscillator,
				 "sawtooth");
  gtk_combo_box_text_append_text(hybrid_synth->synth_0_sync_lfo_oscillator,
				 "triangle");
  gtk_combo_box_text_append_text(hybrid_synth->synth_0_sync_lfo_oscillator,
				 "square");
  gtk_combo_box_text_append_text(hybrid_synth->synth_0_sync_lfo_oscillator,
				 "impulse");

  gtk_combo_box_set_active(hybrid_synth->synth_0_sync_lfo_oscillator,
			   0);

  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) hybrid_synth->synth_0_sync_lfo_oscillator,
		  15, 0,
		  1, 1);

  /* OSC 1 LFO frequency */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 LFO frequency"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  14, 1,
		  1, 1);

  hybrid_synth->synth_0_sync_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 10.0, 0.01);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) hybrid_synth->synth_0_sync_lfo_frequency,
		  15, 1,
		  1, 1);
  
  /* grid */
  synth_1_grid = (GtkGrid *) gtk_grid_new();
  gtk_container_add((GtkContainer *) osc_vbox,
		    (GtkWidget *) synth_1_grid);

  /* OSC 2 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  hybrid_synth->synth_1_oscillator = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(hybrid_synth->synth_1_oscillator,
				 "sine");
  gtk_combo_box_text_append_text(hybrid_synth->synth_1_oscillator,
				 "sawtooth");
  gtk_combo_box_text_append_text(hybrid_synth->synth_1_oscillator,
				 "triangle");
  gtk_combo_box_text_append_text(hybrid_synth->synth_1_oscillator,
				 "square");
  gtk_combo_box_text_append_text(hybrid_synth->synth_1_oscillator,
				 "impulse");

  gtk_combo_box_set_active(hybrid_synth->synth_1_oscillator,
			   0);

  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) hybrid_synth->synth_1_oscillator,
		  1, 0,
		  1, 1);
  
  /* OSC 2 - octave */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - octave"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  2, 0,
		  1, 1);

  hybrid_synth->synth_1_octave = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->synth_1_octave);

  gtk_adjustment_set_lower(adjustment,
			   -6.0);
  gtk_adjustment_set_upper(adjustment,
			   6.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);

//  hybrid_synth->synth_1_octave->scale_max_precision = 12;
//  ags_dial_set_scale_precision(hybrid_synth->synth_1_octave,
//			       12);
  ags_dial_set_radius(hybrid_synth->synth_1_octave,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) hybrid_synth->synth_1_octave,
		  3, 0,
		  1, 1);

  /* OSC 2 - key */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - key"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  4, 0,
		  1, 1);

  hybrid_synth->synth_1_key = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->synth_1_key);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   2.0);

//  hybrid_synth->synth_1_key->scale_max_precision = 12;
//  ags_dial_set_scale_precision(hybrid_synth->synth_1_key,
//			       12);
  ags_dial_set_radius(hybrid_synth->synth_1_key,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) hybrid_synth->synth_1_key,
		  5, 0,
		  1, 1);
  
  /* OSC 2 - phase */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - phase"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  2, 1,
		  1, 1);

  hybrid_synth->synth_1_phase = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->synth_1_phase);

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

  ags_dial_set_radius(hybrid_synth->synth_1_phase,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) hybrid_synth->synth_1_phase,
		  3, 1,
		  1, 1);

  /* OSC 2 - volume */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - volume"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  4, 1,
		  1, 1);

  hybrid_synth->synth_1_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->synth_1_volume);

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

  ags_dial_set_radius(hybrid_synth->synth_1_volume,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) hybrid_synth->synth_1_volume,
		  5, 1,
		  1, 1);

  /* OSC 2 - sync enabled */
  hybrid_synth->synth_1_sync_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("OSC 2 - sync enabled"));
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) hybrid_synth->synth_1_sync_enabled,
		  6, 0,
		  1, 1);

  /* OSC 2 - sync relative attack factor */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync factor"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  6, 1,
		  1, 1);
  
  hybrid_synth->synth_1_sync_relative_attack_factor = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->synth_1_sync_relative_attack_factor);

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

  ags_dial_set_radius(hybrid_synth->synth_1_sync_relative_attack_factor,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) hybrid_synth->synth_1_sync_relative_attack_factor,
		  7, 1,
		  1, 1);

  /* OSC 2 - sync attack 0 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync attack 0"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  10, 0,
		  1, 1);
  
  hybrid_synth->synth_1_sync_attack_0 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->synth_1_sync_attack_0);

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

  ags_dial_set_radius(hybrid_synth->synth_1_sync_attack_0,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) hybrid_synth->synth_1_sync_attack_0,
		  11, 0,
		  1, 1);

  /* OSC 2 - sync phase 0 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync phase 0"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  12, 0,
		  1, 1);
  
  hybrid_synth->synth_1_sync_phase_0 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->synth_1_sync_phase_0);

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

  ags_dial_set_radius(hybrid_synth->synth_1_sync_phase_0,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) hybrid_synth->synth_1_sync_phase_0,
		  13, 0,
		  1, 1);

  /* OSC 2 - sync attack 1 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync attack 1"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  10, 1,
		  1, 1);
  
  hybrid_synth->synth_1_sync_attack_1 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->synth_1_sync_attack_1);

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

  ags_dial_set_radius(hybrid_synth->synth_1_sync_attack_1,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) hybrid_synth->synth_1_sync_attack_1,
		  11, 1,
		  1, 1);

  /* OSC 2 - sync phase 1 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync phase 1"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  12, 1,
		  1, 1);
  
  hybrid_synth->synth_1_sync_phase_1 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->synth_1_sync_phase_1);

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

  ags_dial_set_radius(hybrid_synth->synth_1_sync_phase_1,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) hybrid_synth->synth_1_sync_phase_1,
		  13, 1,
		  1, 1);

  /* OSC 2 - sync attack 2 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync attack 2"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  10, 2,
		  1, 1);
  
  hybrid_synth->synth_1_sync_attack_2 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->synth_1_sync_attack_2);

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

  ags_dial_set_radius(hybrid_synth->synth_1_sync_attack_2,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) hybrid_synth->synth_1_sync_attack_2,
		  11, 2,
		  1, 1);

  /* OSC 2 - sync phase 2 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync phase 2"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  12, 2,
		  1, 1);
  
  hybrid_synth->synth_1_sync_phase_2 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->synth_1_sync_phase_2);

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

  ags_dial_set_radius(hybrid_synth->synth_1_sync_phase_2,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) hybrid_synth->synth_1_sync_phase_2,
		  13, 2,
		  1, 1);

  /* OSC 2 LFO */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 LFO"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  14, 0,
		  1, 1);
  
  hybrid_synth->synth_1_sync_lfo_oscillator = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(hybrid_synth->synth_1_sync_lfo_oscillator,
				 "sine");
  gtk_combo_box_text_append_text(hybrid_synth->synth_1_sync_lfo_oscillator,
				 "sawtooth");
  gtk_combo_box_text_append_text(hybrid_synth->synth_1_sync_lfo_oscillator,
				 "triangle");
  gtk_combo_box_text_append_text(hybrid_synth->synth_1_sync_lfo_oscillator,
				 "square");
  gtk_combo_box_text_append_text(hybrid_synth->synth_1_sync_lfo_oscillator,
				 "impulse");

  gtk_combo_box_set_active(hybrid_synth->synth_1_sync_lfo_oscillator,
			   0);

  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) hybrid_synth->synth_1_sync_lfo_oscillator,
		  15, 0,
		  1, 1);

  /* OSC 2 LFO frequency */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 LFO frequency"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  14, 1,
		  1, 1);

  hybrid_synth->synth_1_sync_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 10.0, 0.01);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) hybrid_synth->synth_1_sync_lfo_frequency,
		  15, 1,
		  1, 1);

  /* grid */
  misc_grid = (GtkGrid *) gtk_grid_new();
  gtk_container_add((GtkContainer *) vbox,
		    (GtkWidget *) misc_grid);

  /* sequencer */
  hybrid_synth->sequencer_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("sequencer enabled"));
  gtk_grid_attach(misc_grid,
		  (GtkWidget *) hybrid_synth->sequencer_enabled,
		  0, 0,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("sequencer sign"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(misc_grid,
		  (GtkWidget *) label,
		  2, 0,
		  1, 1);
  
  hybrid_synth->sequencer_sign = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(hybrid_synth->sequencer_sign,
				 "A");
  gtk_combo_box_text_append_text(hybrid_synth->sequencer_sign,
				 "A#");
  gtk_combo_box_text_append_text(hybrid_synth->sequencer_sign,
				 "H");
  gtk_combo_box_text_append_text(hybrid_synth->sequencer_sign,
				 "C");
  gtk_combo_box_text_append_text(hybrid_synth->sequencer_sign,
				 "C#");
  gtk_combo_box_text_append_text(hybrid_synth->sequencer_sign,
				 "D");
  gtk_combo_box_text_append_text(hybrid_synth->sequencer_sign,
				 "D#");
  gtk_combo_box_text_append_text(hybrid_synth->sequencer_sign,
				 "E");
  gtk_combo_box_text_append_text(hybrid_synth->sequencer_sign,
				 "F");
  gtk_combo_box_text_append_text(hybrid_synth->sequencer_sign,
				 "F#");
  gtk_combo_box_text_append_text(hybrid_synth->sequencer_sign,
				 "G");
  gtk_combo_box_text_append_text(hybrid_synth->sequencer_sign,
				 "G#");

  gtk_combo_box_set_active(hybrid_synth->sequencer_sign,
			   0);

  gtk_grid_attach(misc_grid,
		  (GtkWidget *) hybrid_synth->sequencer_sign,
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

  hybrid_synth->pitch_tuning = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->pitch_tuning);

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

  ags_dial_set_radius(hybrid_synth->pitch_tuning,
		      12);
  
  gtk_grid_attach(misc_grid,
		  (GtkWidget *) hybrid_synth->pitch_tuning,
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

  hybrid_synth->noise_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->noise_gain);

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

  ags_dial_set_radius(hybrid_synth->noise_gain,
		      12);
  
  gtk_grid_attach(misc_grid,
		  (GtkWidget *) hybrid_synth->noise_gain,
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

  hybrid_synth->low_pass_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("low pass enabled"));
  gtk_grid_attach(low_pass_grid,
		  (GtkWidget *) hybrid_synth->low_pass_enabled,
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

  hybrid_synth->low_pass_q_lin = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->low_pass_q_lin);

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

  ags_dial_set_radius(hybrid_synth->low_pass_q_lin,
		      12);
  
  gtk_grid_attach(low_pass_grid,
		  (GtkWidget *) hybrid_synth->low_pass_q_lin,
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

  hybrid_synth->low_pass_filter_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->low_pass_filter_gain);

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

  ags_dial_set_radius(hybrid_synth->low_pass_filter_gain,
		      12);
  
  gtk_grid_attach(low_pass_grid,
		  (GtkWidget *) hybrid_synth->low_pass_filter_gain,
		  1, 2,
		  1, 1);

  /* high pass grid */
  high_pass_grid = (GtkGrid *) gtk_grid_new();
  gtk_container_add((GtkContainer *) band_hbox,
		    (GtkWidget *) high_pass_grid);

  hybrid_synth->high_pass_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("high pass enabled"));
  gtk_grid_attach(high_pass_grid,
		  (GtkWidget *) hybrid_synth->high_pass_enabled,
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

  hybrid_synth->high_pass_q_lin = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->high_pass_q_lin);

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

  ags_dial_set_radius(hybrid_synth->high_pass_q_lin,
		      12);
  
  gtk_grid_attach(high_pass_grid,
		  (GtkWidget *) hybrid_synth->high_pass_q_lin,
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

  hybrid_synth->high_pass_filter_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->high_pass_filter_gain);

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

  ags_dial_set_radius(hybrid_synth->high_pass_filter_gain,
		      12);
  
  gtk_grid_attach(high_pass_grid,
		  (GtkWidget *) hybrid_synth->high_pass_filter_gain,
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

  hybrid_synth->chorus_input_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->chorus_input_volume);

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

  ags_dial_set_radius(hybrid_synth->chorus_input_volume,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) hybrid_synth->chorus_input_volume,
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

  hybrid_synth->chorus_output_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->chorus_output_volume);

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

  ags_dial_set_radius(hybrid_synth->chorus_output_volume,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) hybrid_synth->chorus_output_volume,
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
  
  hybrid_synth->chorus_lfo_oscillator = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(hybrid_synth->chorus_lfo_oscillator,
				 "sine");
  gtk_combo_box_text_append_text(hybrid_synth->chorus_lfo_oscillator,
				 "sawtooth");
  gtk_combo_box_text_append_text(hybrid_synth->chorus_lfo_oscillator,
				 "triangle");
  gtk_combo_box_text_append_text(hybrid_synth->chorus_lfo_oscillator,
				 "square");
  gtk_combo_box_text_append_text(hybrid_synth->chorus_lfo_oscillator,
				 "impulse");

  gtk_combo_box_set_active(hybrid_synth->chorus_lfo_oscillator,
			   0);

  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) hybrid_synth->chorus_lfo_oscillator,
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

  hybrid_synth->chorus_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 10.0, 0.01);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) hybrid_synth->chorus_lfo_frequency,
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

  hybrid_synth->chorus_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->chorus_depth);

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

  ags_dial_set_radius(hybrid_synth->chorus_depth,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) hybrid_synth->chorus_depth,
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

  hybrid_synth->chorus_mix = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->chorus_mix);

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

  ags_dial_set_radius(hybrid_synth->chorus_mix,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) hybrid_synth->chorus_mix,
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

  hybrid_synth->chorus_delay = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(hybrid_synth->chorus_delay);

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

  ags_dial_set_radius(hybrid_synth->chorus_delay,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) hybrid_synth->chorus_delay,
		  5, 2,
		  1, 1);
}

void
ags_hybrid_synth_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_hybrid_synth_parent_class)->finalize(gobject);
}

void
ags_hybrid_synth_connect(AgsConnectable *connectable)
{
  AgsHybridSynth *hybrid_synth;
  
  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_hybrid_synth_parent_connectable_interface->connect(connectable);

  /* AgsHybridSynth */
  hybrid_synth = AGS_HYBRID_SYNTH(connectable);

  g_signal_connect_after(hybrid_synth->synth_0_oscillator, "changed",
			 G_CALLBACK(ags_hybrid_synth_synth_0_oscillator_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_0_octave, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_0_octave_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_0_key, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_0_key_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_0_phase, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_0_phase_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_0_volume, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_0_volume_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_0_sync_enabled, "clicked",
			 G_CALLBACK(ags_hybrid_synth_synth_0_sync_enabled_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_0_sync_relative_attack_factor, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_0_sync_relative_attack_factor_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_0_sync_attack_0, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_0_sync_attack_0_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_0_sync_phase_0, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_0_sync_phase_0_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_0_sync_attack_1, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_0_sync_attack_1_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_0_sync_phase_1, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_0_sync_phase_1_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_0_sync_attack_2, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_0_sync_attack_2_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_0_sync_phase_2, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_0_sync_phase_2_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_0_sync_lfo_oscillator, "changed",
			 G_CALLBACK(ags_hybrid_synth_synth_0_sync_lfo_oscillator_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_0_sync_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_0_sync_lfo_frequency_callback), hybrid_synth);

  g_signal_connect_after(hybrid_synth->synth_1_oscillator, "changed",
			 G_CALLBACK(ags_hybrid_synth_synth_1_oscillator_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_1_octave, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_1_octave_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_1_key, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_1_key_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_1_phase, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_1_phase_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_1_volume, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_1_volume_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_1_sync_enabled, "clicked",
			 G_CALLBACK(ags_hybrid_synth_synth_1_sync_enabled_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_1_sync_relative_attack_factor, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_1_sync_relative_attack_factor_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_1_sync_attack_0, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_1_sync_attack_0_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_1_sync_phase_0, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_1_sync_phase_0_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_1_sync_attack_1, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_1_sync_attack_1_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_1_sync_phase_1, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_1_sync_phase_1_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_1_sync_attack_2, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_1_sync_attack_2_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_1_sync_phase_2, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_1_sync_phase_2_callback), hybrid_synth);

  g_signal_connect_after(hybrid_synth->synth_1_sync_lfo_oscillator, "changed",
			 G_CALLBACK(ags_hybrid_synth_synth_1_sync_lfo_oscillator_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->synth_1_sync_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_synth_1_sync_lfo_frequency_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->sequencer_enabled, "clicked",
			 G_CALLBACK(ags_hybrid_synth_sequencer_enabled_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->sequencer_sign, "changed",
			 G_CALLBACK(ags_hybrid_synth_sequencer_sign_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->pitch_tuning, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_pitch_tuning_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->noise_gain, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_noise_gain_callback), hybrid_synth);
    
//  g_signal_connect_after(hybrid_synth->chorus_enabled, "clicked",
//			 G_CALLBACK(ags_hybrid_synth_chorus_enabled_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->chorus_input_volume, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_chorus_input_volume_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->chorus_output_volume, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_chorus_output_volume_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->chorus_lfo_oscillator, "changed",
			 G_CALLBACK(ags_hybrid_synth_chorus_lfo_oscillator_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->chorus_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_chorus_lfo_frequency_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->chorus_depth, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_chorus_depth_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->chorus_mix, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_chorus_mix_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->chorus_delay, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_chorus_delay_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->low_pass_enabled, "clicked",
			 G_CALLBACK(ags_hybrid_synth_low_pass_enabled_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->low_pass_q_lin, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_low_pass_q_lin_callback), hybrid_synth);
    
  g_signal_connect_after(hybrid_synth->low_pass_filter_gain, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_low_pass_filter_gain_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->high_pass_enabled, "clicked",
			 G_CALLBACK(ags_hybrid_synth_high_pass_enabled_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->high_pass_q_lin, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_high_pass_q_lin_callback), hybrid_synth);
  
  g_signal_connect_after(hybrid_synth->high_pass_filter_gain, "value-changed",
			 G_CALLBACK(ags_hybrid_synth_high_pass_filter_gain_callback), hybrid_synth);
}

void
ags_hybrid_synth_disconnect(AgsConnectable *connectable)
{
  AgsHybridSynth *hybrid_synth;
  
  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  ags_hybrid_synth_parent_connectable_interface->disconnect(connectable);

  /* AgsHybridSynth */
  hybrid_synth = AGS_HYBRID_SYNTH(connectable);

  //TODO:JK: implement me
}

void
ags_hybrid_synth_resize_audio_channels(AgsMachine *machine,
				       guint audio_channels, guint audio_channels_old,
				       gpointer data)
{
  AgsHybridSynth *hybrid_synth;

  hybrid_synth = (AgsHybridSynth *) machine;

  if(audio_channels > audio_channels_old){    
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_hybrid_synth_input_map_recall(hybrid_synth,
					audio_channels_old,
					0);
      
      ags_hybrid_synth_output_map_recall(hybrid_synth,
					 audio_channels_old,
					 0);
    }
  }
}

void
ags_hybrid_synth_resize_pads(AgsMachine *machine, GType type,
			     guint pads, guint pads_old,
			     gpointer data)
{
  AgsHybridSynth *hybrid_synth;

  gboolean grow;

  hybrid_synth = (AgsHybridSynth *) machine;
  
  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(type == AGS_TYPE_INPUT){
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	/* depending on destination */
	ags_hybrid_synth_input_map_recall(hybrid_synth,
					  0,
					  pads_old);
      }
    }else{
      hybrid_synth->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_hybrid_synth_output_map_recall(hybrid_synth,
					   0,
					   pads_old);
      }
    }else{
      hybrid_synth->mapped_output_pad = pads;
    }
  }
}

void
ags_hybrid_synth_map_recall(AgsMachine *machine)
{
  AgsHybridSynth *hybrid_synth;
  
  AgsAudio *audio;

  GList *start_recall, *recall;

  gint position;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  hybrid_synth = AGS_HYBRID_SYNTH(machine);

  audio = machine->audio;
  
  position = 0;
  
  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       hybrid_synth->playback_play_container, hybrid_synth->playback_recall_container,
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
				       hybrid_synth->synth_play_container, hybrid_synth->synth_recall_container,
				       "ags-fx-synth",
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
				       hybrid_synth->volume_play_container, hybrid_synth->volume_recall_container,
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
				       hybrid_synth->envelope_play_container, hybrid_synth->envelope_recall_container,
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
				       hybrid_synth->buffer_play_container, hybrid_synth->buffer_recall_container,
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
  ags_hybrid_synth_input_map_recall(hybrid_synth,
				    0,
				    0);

  /* depending on destination */
  ags_hybrid_synth_output_map_recall(hybrid_synth,
				     0,
				     0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_hybrid_synth_parent_class)->map_recall(machine);  
}

void
ags_hybrid_synth_input_map_recall(AgsHybridSynth *hybrid_synth,
				  guint audio_channel_start,
				  guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(hybrid_synth->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(hybrid_synth)->audio;

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
				       hybrid_synth->playback_play_container, hybrid_synth->playback_recall_container,
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
				       hybrid_synth->synth_play_container, hybrid_synth->synth_recall_container,
				       "ags-fx-synth",
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
				       hybrid_synth->volume_play_container, hybrid_synth->volume_recall_container,
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
				       hybrid_synth->envelope_play_container, hybrid_synth->envelope_recall_container,
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
				       hybrid_synth->buffer_play_container, hybrid_synth->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  hybrid_synth->mapped_input_pad = input_pads;
}

void
ags_hybrid_synth_output_map_recall(AgsHybridSynth *hybrid_synth,
				   guint audio_channel_start,
				   guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;

  if(hybrid_synth->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(hybrid_synth)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);
  
  hybrid_synth->mapped_output_pad = output_pads;
}

/**
 * ags_hybrid_synth_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsHybridSynth
 *
 * Returns: the new #AgsHybridSynth
 *
 * Since: 3.14.0
 */
AgsHybridSynth*
ags_hybrid_synth_new(GObject *soundcard)
{
  AgsHybridSynth *hybrid_synth;

  hybrid_synth = (AgsHybridSynth *) g_object_new(AGS_TYPE_HYBRID_SYNTH,
						 NULL);

  g_object_set(AGS_MACHINE(hybrid_synth)->audio,
	       "output-soundcard", soundcard,
	       NULL);

  return(hybrid_synth);
}
