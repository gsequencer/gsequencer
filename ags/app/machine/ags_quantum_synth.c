/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/app/machine/ags_quantum_synth.h>
#include <ags/app/machine/ags_quantum_synth_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_line.h>

#include <math.h>

#include <ags/i18n.h>

void ags_quantum_synth_class_init(AgsQuantumSynthClass *quantum_synth);
void ags_quantum_synth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_quantum_synth_init(AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_finalize(GObject *gobject);

void ags_quantum_synth_connect(AgsConnectable *connectable);
void ags_quantum_synth_disconnect(AgsConnectable *connectable);

void ags_quantum_synth_show(GtkWidget *widget);

void ags_quantum_synth_resize_audio_channels(AgsMachine *machine,
					     guint audio_channels, guint audio_channels_old,
					     gpointer data);
void ags_quantum_synth_resize_pads(AgsMachine *machine, GType channel_type,
				   guint pads, guint pads_old,
				   gpointer data);

void ags_quantum_synth_map_recall(AgsMachine *machine);

void ags_quantum_synth_input_map_recall(AgsQuantumSynth *quantum_synth,
					guint audio_channel_start,
					guint input_pad_start);
void ags_quantum_synth_output_map_recall(AgsQuantumSynth *quantum_synth,
					 guint audio_channel_start,
					 guint output_pad_start);

void ags_quantum_synth_refresh_port(AgsMachine *machine);

/**
 * SECTION:ags_quantum_synth
 * @short_description: quantum synth
 * @title: AgsQuantumSynth
 * @section_id:
 * @include: ags/app/machine/ags_quantum_synth.h
 *
 * The #AgsQuantumSynth is a composite widget to act as quantum synth.
 */

static gpointer ags_quantum_synth_parent_class = NULL;
static AgsConnectableInterface *ags_quantum_synth_parent_connectable_interface;

GType
ags_quantum_synth_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_quantum_synth = 0;

    static const GTypeInfo ags_quantum_synth_info = {
      sizeof(AgsQuantumSynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_quantum_synth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsQuantumSynth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_quantum_synth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_quantum_synth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_quantum_synth = g_type_register_static(AGS_TYPE_MACHINE,
						    "AgsQuantumSynth", &ags_quantum_synth_info,
						    0);
    
    g_type_add_interface_static(ags_type_quantum_synth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_quantum_synth);
  }

  return(g_define_type_id__static);
}

void
ags_quantum_synth_class_init(AgsQuantumSynthClass *quantum_synth)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_quantum_synth_parent_class = g_type_class_peek_parent(quantum_synth);

  /* GObjectClass */
  gobject = (GObjectClass *) quantum_synth;

  gobject->finalize = ags_quantum_synth_finalize;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) quantum_synth;

  machine->map_recall = ags_quantum_synth_map_recall;

  machine->refresh_port = ags_quantum_synth_refresh_port;
}

void
ags_quantum_synth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_quantum_synth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_quantum_synth_connect;
  connectable->disconnect = ags_quantum_synth_disconnect;
}

void
ags_quantum_synth_init(AgsQuantumSynth *quantum_synth)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  GtkBox *vbox;
  GtkBox *hbox;
  GtkBox *osc_vbox;
  GtkGrid *synth_0_grid;
  GtkGrid *synth_0_seq_grid;
  GtkGrid *synth_1_grid;
  GtkGrid *synth_1_seq_grid;
  GtkGrid *misc_grid;
  GtkBox *pitch_type_hbox;
  GtkBox *effect_vbox;
  GtkFrame *tremolo_frame;
  GtkGrid *tremolo_grid;
  GtkFrame *vibrato_frame;
  GtkBox *ext_hbox;
  GtkGrid *vibrato_grid;
  GtkGrid *chorus_grid;
  GtkLabel *label;

  GtkAdjustment *adjustment;
  
  AgsAudio *audio;

  AgsMachineCounterManager *machine_counter_manager;
  AgsMachineCounter *machine_counter;

  AgsApplicationContext *application_context;   
  
  gchar *machine_name;

  gint position;
  gdouble gui_scale_factor;

  gchar* pitch_type_strv[] = {
    "fluid-interpolate-none",
    "fluid-interpolate-linear",
    "fluid-interpolate-4th-order",
    "fluid-interpolate-7th-order",
    "ags-pitch-2x-alias",    
    "ags-pitch-4x-alias",    
    "ags-pitch-16x-alias",
    NULL,
  };

  gchar* osc_strv[] = {
    "sine",
    "sawtooth",
    "triangle",
    "square",
    "impulse",
    NULL
  };
  
  application_context = ags_application_context_get_instance();
  
  /* machine counter */
  machine_counter_manager = ags_machine_counter_manager_get_instance();

  machine_counter = ags_machine_counter_manager_find_machine_counter(machine_counter_manager,
								     AGS_TYPE_QUANTUM_SYNTH);

  machine_name = NULL;

  if(machine_counter != NULL){
    machine_name = g_strdup_printf("Default %d",
				   machine_counter->counter);
  
    ags_machine_counter_increment(machine_counter);
  }
  
  g_object_set(quantum_synth,
	       "machine-name", machine_name,
	       NULL);

  g_free(machine_name);

  /* machine selector */
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  position = g_list_length(window->machine);
  
  ags_machine_selector_popup_insert_machine(composite_editor->machine_selector,
					    position,
					    (AgsMachine *) quantum_synth);

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
  
  audio = AGS_MACHINE(quantum_synth)->audio;
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

  AGS_MACHINE(quantum_synth)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
					AGS_MACHINE_REVERSE_NOTATION);
  AGS_MACHINE(quantum_synth)->mapping_flags |= AGS_MACHINE_MONO;

  AGS_MACHINE(quantum_synth)->input_pad_type = G_TYPE_NONE;
  AGS_MACHINE(quantum_synth)->input_line_type = G_TYPE_NONE;
  AGS_MACHINE(quantum_synth)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(quantum_synth)->output_line_type = G_TYPE_NONE;

  /* audio resize */
  g_signal_connect_after(G_OBJECT(quantum_synth), "resize-audio-channels",
			 G_CALLBACK(ags_quantum_synth_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(quantum_synth), "resize-pads",
			 G_CALLBACK(ags_quantum_synth_resize_pads), NULL);
  
  /* create widgets */
  quantum_synth->flags = 0;

  /* mapped IO */
  quantum_synth->mapped_input_pad = 0;
  quantum_synth->mapped_output_pad = 0;

  quantum_synth->playback_play_container = ags_recall_container_new();
  quantum_synth->playback_recall_container = ags_recall_container_new();

  quantum_synth->seq_synth_play_container = ags_recall_container_new();
  quantum_synth->seq_synth_recall_container = ags_recall_container_new();

  quantum_synth->volume_play_container = ags_recall_container_new();
  quantum_synth->volume_recall_container = ags_recall_container_new();

  quantum_synth->tremolo_play_container = ags_recall_container_new();
  quantum_synth->tremolo_recall_container = ags_recall_container_new();

  quantum_synth->envelope_play_container = ags_recall_container_new();
  quantum_synth->envelope_recall_container = ags_recall_container_new();

  quantum_synth->buffer_play_container = ags_recall_container_new();
  quantum_synth->buffer_recall_container = ags_recall_container_new();
   
  /* name and xml type */
  quantum_synth->name = NULL;
  quantum_synth->xml_type = "ags-quantum-synth"; 

  /* widget */
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_valign((GtkWidget *) vbox,
			GTK_ALIGN_START);  
  gtk_widget_set_halign((GtkWidget *) vbox,
			GTK_ALIGN_START);

  gtk_widget_set_hexpand((GtkWidget *) vbox,
			 FALSE);

  gtk_box_set_spacing(vbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_frame_set_child(AGS_MACHINE(quantum_synth)->frame,
		      (GtkWidget *) vbox);

  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);

  gtk_box_set_spacing(hbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(vbox,
		 (GtkWidget *) hbox);
  
  /* oscillator */
  osc_vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				    0);

  gtk_box_set_spacing(osc_vbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(hbox,
		 (GtkWidget *) osc_vbox);

  /* grid */
  synth_0_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(synth_0_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(synth_0_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_box_append(osc_vbox,
		 (GtkWidget *) synth_0_grid);

  /* OSC 1 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  quantum_synth->synth_0_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(quantum_synth->synth_0_oscillator,
			     0);

  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) quantum_synth->synth_0_oscillator,
		  1, 0,
		  1, 1);

  /* OSC 1 - octave */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - octave"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  2, 0,
		  1, 1);

  quantum_synth->synth_0_octave = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_octave);

  gtk_adjustment_set_lower(adjustment,
			   -6.0);
  gtk_adjustment_set_upper(adjustment,
			   6.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(quantum_synth->synth_0_octave,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) quantum_synth->synth_0_octave,
		  3, 0,
		  1, 1);

  /* OSC 1 - key */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - key"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  4, 0,
		  1, 1);

  quantum_synth->synth_0_key = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_key);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   2.0);

  ags_dial_set_radius(quantum_synth->synth_0_key,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) quantum_synth->synth_0_key,
		  5, 0,
		  1, 1);
  
  /* OSC 1 - phase */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - phase"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  2, 1,
		  1, 1);

  quantum_synth->synth_0_phase = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_phase);

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

  ags_dial_set_radius(quantum_synth->synth_0_phase,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) quantum_synth->synth_0_phase,
		  3, 1,
		  1, 1);

  /* OSC 1 - volume */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - volume"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  4, 1,
		  1, 1);

  quantum_synth->synth_0_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_volume);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.333);

  ags_dial_set_radius(quantum_synth->synth_0_volume,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) quantum_synth->synth_0_volume,
		  5, 1,
		  1, 1);

  /* seq */
  synth_0_seq_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(synth_0_seq_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(synth_0_seq_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) synth_0_seq_grid,
		  6, 0,
		  8, 4);

  /* synth 0 seq Nr. 0 */
  quantum_synth->synth_0_seq_tuning_0 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_seq_tuning_0);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_0_seq_tuning_0,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) quantum_synth->synth_0_seq_tuning_0,
		  0, 0,
		  1, 1);

  /* synth 0 seq Nr. 1 */
  quantum_synth->synth_0_seq_tuning_1 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_seq_tuning_1);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_0_seq_tuning_1,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) quantum_synth->synth_0_seq_tuning_1,
		  1, 0,
		  1, 1);

  /* synth 0 seq Nr. 2 */
  quantum_synth->synth_0_seq_tuning_2 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_seq_tuning_2);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_0_seq_tuning_2,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) quantum_synth->synth_0_seq_tuning_2,
		  2, 0,
		  1, 1);

  /* synth 0 seq Nr. 3 */
  quantum_synth->synth_0_seq_tuning_3 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_seq_tuning_3);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_0_seq_tuning_3,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) quantum_synth->synth_0_seq_tuning_3,
		  3, 0,
		  1, 1);

  /* synth 0 seq Nr. 4 */
  quantum_synth->synth_0_seq_tuning_4 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_seq_tuning_4);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_0_seq_tuning_4,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) quantum_synth->synth_0_seq_tuning_4,
		  4, 0,
		  1, 1);

  /* synth 0 seq Nr. 5 */
  quantum_synth->synth_0_seq_tuning_5 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_seq_tuning_5);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_0_seq_tuning_5,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) quantum_synth->synth_0_seq_tuning_5,
		  5, 0,
		  1, 1);

  /* synth 0 seq Nr. 6 */
  quantum_synth->synth_0_seq_tuning_6 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_seq_tuning_6);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_0_seq_tuning_6,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) quantum_synth->synth_0_seq_tuning_6,
		  6, 0,
		  1, 1);

  /* synth 0 seq Nr. 7 */
  quantum_synth->synth_0_seq_tuning_7 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_seq_tuning_7);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_0_seq_tuning_7,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) quantum_synth->synth_0_seq_tuning_7,
		  7, 0,
		  1, 1);

  /* synth 0 seq pingpong */
  quantum_synth->synth_0_seq_tuning_pingpong = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("Seq 1 tuning - pingpong"));
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) quantum_synth->synth_0_seq_tuning_pingpong,
		  0, 1,
		  4, 1);

  quantum_synth->synth_0_seq_tuning_lfo_frequency = NULL;

  /* synth 0 LFO frequency */
  quantum_synth->synth_0_seq_tuning_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 16.0, 0.01);
  gtk_spin_button_set_value(quantum_synth->synth_0_seq_tuning_lfo_frequency,
			    8.0);  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) quantum_synth->synth_0_seq_tuning_lfo_frequency,
		  6, 1,
		  2, 1);
  
  /* synth 0 seq Nr. 0 */
  quantum_synth->synth_0_seq_volume_0 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_seq_volume_0);

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

  ags_dial_set_radius(quantum_synth->synth_0_seq_volume_0,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) quantum_synth->synth_0_seq_volume_0,
		  0, 2,
		  1, 1);

  /* synth 0 seq Nr. 1 */
  quantum_synth->synth_0_seq_volume_1 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_seq_volume_1);

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

  ags_dial_set_radius(quantum_synth->synth_0_seq_volume_1,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) quantum_synth->synth_0_seq_volume_1,
		  1, 2,
		  1, 1);

  /* synth 0 seq Nr. 2 */
  quantum_synth->synth_0_seq_volume_2 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_seq_volume_2);

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

  ags_dial_set_radius(quantum_synth->synth_0_seq_volume_2,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) quantum_synth->synth_0_seq_volume_2,
		  2, 2,
		  1, 1);

  /* synth 0 seq Nr. 3 */
  quantum_synth->synth_0_seq_volume_3 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_seq_volume_3);

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

  ags_dial_set_radius(quantum_synth->synth_0_seq_volume_3,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) quantum_synth->synth_0_seq_volume_3,
		  3, 2,
		  1, 1);

  /* synth 0 seq Nr. 4 */
  quantum_synth->synth_0_seq_volume_4 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_seq_volume_4);

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

  ags_dial_set_radius(quantum_synth->synth_0_seq_volume_4,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) quantum_synth->synth_0_seq_volume_4,
		  4, 2,
		  1, 1);

  /* synth 0 seq Nr. 5 */
  quantum_synth->synth_0_seq_volume_5 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_seq_volume_5);

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

  ags_dial_set_radius(quantum_synth->synth_0_seq_volume_5,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) quantum_synth->synth_0_seq_volume_5,
		  5, 2,
		  1, 1);

  /* synth 0 seq Nr. 6 */
  quantum_synth->synth_0_seq_volume_6 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_seq_volume_6);

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

  ags_dial_set_radius(quantum_synth->synth_0_seq_volume_6,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) quantum_synth->synth_0_seq_volume_6,
		  6, 2,
		  1, 1);

  /* synth 0 seq Nr. 7 */
  quantum_synth->synth_0_seq_volume_7 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_seq_volume_7);

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

  ags_dial_set_radius(quantum_synth->synth_0_seq_volume_7,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) quantum_synth->synth_0_seq_volume_7,
		  7, 2,
		  1, 1);

  /* synth 0 seq pingpong */
  quantum_synth->synth_0_seq_volume_pingpong = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("Seq 1 volume - pingpong"));
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) quantum_synth->synth_0_seq_volume_pingpong,
		  0, 3,
		  4, 1);

  quantum_synth->synth_0_seq_volume_lfo_frequency = NULL;

  /* synth 0 LFO frequency */
  quantum_synth->synth_0_seq_volume_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 16.0, 0.01);
  gtk_spin_button_set_value(quantum_synth->synth_0_seq_volume_lfo_frequency,
			    8.0);  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) quantum_synth->synth_0_seq_volume_lfo_frequency,
		  6, 3,
		  2, 1);
  
  /* OSC 1 - sync enabled */
  quantum_synth->synth_0_sync_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("OSC 1 - sync enabled"));
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) quantum_synth->synth_0_sync_enabled,
		  14, 0,
		  2, 1);

  /* OSC 1 - sync relative attack factor */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync factor"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  14, 1,
		  1, 1);
  
  quantum_synth->synth_0_sync_relative_attack_factor = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_sync_relative_attack_factor);

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

  ags_dial_set_radius(quantum_synth->synth_0_sync_relative_attack_factor,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) quantum_synth->synth_0_sync_relative_attack_factor,
		  15, 1,
		  1, 1);

  /* OSC 1 - sync attack 0 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync attack 0"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  18, 0,
		  1, 1);
  
  quantum_synth->synth_0_sync_attack_0 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_sync_attack_0);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0 * M_PI);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_0_sync_attack_0,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) quantum_synth->synth_0_sync_attack_0,
		  19, 0,
		  1, 1);

  /* OSC 1 - sync phase 0 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync phase 0"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  20, 0,
		  1, 1);
  
  quantum_synth->synth_0_sync_phase_0 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_sync_phase_0);

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

  ags_dial_set_radius(quantum_synth->synth_0_sync_phase_0,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) quantum_synth->synth_0_sync_phase_0,
		  21, 0,
		  1, 1);

  /* OSC 1 - sync attack 1 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync attack 1"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  18, 1,
		  1, 1);
  
  quantum_synth->synth_0_sync_attack_1 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_sync_attack_1);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0 * M_PI);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_0_sync_attack_1,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) quantum_synth->synth_0_sync_attack_1,
		  19, 1,
		  1, 1);

  /* OSC 1 - sync phase 1 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync phase 1"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  20, 1,
		  1, 1);
  
  quantum_synth->synth_0_sync_phase_1 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_sync_phase_1);

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

  ags_dial_set_radius(quantum_synth->synth_0_sync_phase_1,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) quantum_synth->synth_0_sync_phase_1,
		  21, 1,
		  1, 1);

  /* OSC 1 - sync attack 2 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync attack 2"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  18, 2,
		  1, 1);
  
  quantum_synth->synth_0_sync_attack_2 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_sync_attack_2);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0 * M_PI);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_0_sync_attack_2,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) quantum_synth->synth_0_sync_attack_2,
		  19, 2,
		  1, 1);

  /* OSC 1 - sync phase 2 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync phase 2"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  20, 2,
		  1, 1);
  
  quantum_synth->synth_0_sync_phase_2 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_sync_phase_2);

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

  ags_dial_set_radius(quantum_synth->synth_0_sync_phase_2,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) quantum_synth->synth_0_sync_phase_2,
		  21, 2,
		  1, 1);

  /* OSC 1 - sync attack 3 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync attack 3"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  18, 3,
		  1, 1);
  
  quantum_synth->synth_0_sync_attack_3 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_sync_attack_3);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0 * M_PI);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_0_sync_attack_3,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) quantum_synth->synth_0_sync_attack_3,
		  19, 3,
		  1, 1);

  /* OSC 1 - sync phase 3 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync phase 3"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  20, 3,
		  1, 1);
  
  quantum_synth->synth_0_sync_phase_3 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_sync_phase_3);

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

  ags_dial_set_radius(quantum_synth->synth_0_sync_phase_3,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) quantum_synth->synth_0_sync_phase_3,
		  21, 3,
		  1, 1);
  
  /* sync OSC 1 LFO */
  label = (GtkLabel *) gtk_label_new(i18n("sync OSC 1 LFO"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  22, 0,
		  1, 1);
  
  quantum_synth->synth_0_sync_lfo_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(quantum_synth->synth_0_sync_lfo_oscillator,
			     0);

  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) quantum_synth->synth_0_sync_lfo_oscillator,
		  23, 0,
		  1, 1);

  /* sync OSC 1 LFO frequency */
  label = (GtkLabel *) gtk_label_new(i18n("sync OSC 1 LFO frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  22, 1,
		  1, 1);

  quantum_synth->synth_0_sync_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 10.0, 0.01);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) quantum_synth->synth_0_sync_lfo_frequency,
		  23, 1,
		  1, 1);

  /* OSC 1 - LFO OSC */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - LFO OSC"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  quantum_synth->synth_0_lfo_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(quantum_synth->synth_0_lfo_oscillator,
			     0);

  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) quantum_synth->synth_0_lfo_oscillator,
		  1, 2,
		  1, 1);

  /* OSC 1 - LFO frequency */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - LFO frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  2, 2,
		  1, 1);

  quantum_synth->synth_0_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 16.0, 0.01);

  gtk_spin_button_set_value(quantum_synth->synth_0_lfo_frequency,
			    AGS_FM_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) quantum_synth->synth_0_lfo_frequency,
		  3, 2,
		  1, 1);

  /* OSC 1 - LFO tuning */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - LFO tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  2, 3,
		  1, 1);

  quantum_synth->synth_0_lfo_tuning = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_lfo_tuning);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);
  gtk_adjustment_set_page_increment(adjustment,
				    10.0);

  gtk_adjustment_set_value(adjustment,
			   AGS_FM_SYNTH_UTIL_DEFAULT_TUNING);

  ags_dial_set_radius(quantum_synth->synth_0_lfo_tuning,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) quantum_synth->synth_0_lfo_tuning,
		  3, 3,
		  1, 1);

  /* OSC 1 - LFO depth */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - LFO depth"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  4, 2,
		  1, 1);

  quantum_synth->synth_0_lfo_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_0_lfo_depth);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   AGS_FM_SYNTH_UTIL_DEFAULT_LFO_DEPTH);

  ags_dial_set_radius(quantum_synth->synth_0_lfo_depth,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) quantum_synth->synth_0_lfo_depth,
		  5, 2,
		  1, 1);
  
  /* grid */
  synth_1_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(synth_1_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(synth_1_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_box_append(osc_vbox,
		 (GtkWidget *) synth_1_grid);

  /* OSC 2 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  quantum_synth->synth_1_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(quantum_synth->synth_1_oscillator,
			     0);

  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) quantum_synth->synth_1_oscillator,
		  1, 0,
		  1, 1);

  /* OSC 2 - octave */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - octave"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  2, 0,
		  1, 1);

  quantum_synth->synth_1_octave = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_octave);

  gtk_adjustment_set_lower(adjustment,
			   -6.0);
  gtk_adjustment_set_upper(adjustment,
			   6.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(quantum_synth->synth_1_octave,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) quantum_synth->synth_1_octave,
		  3, 0,
		  1, 1);

  /* OSC 2 - key */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - key"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  4, 0,
		  1, 1);

  quantum_synth->synth_1_key = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_key);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   2.0);
  ags_dial_set_radius(quantum_synth->synth_1_key,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) quantum_synth->synth_1_key,
		  5, 0,
		  1, 1);
  
  /* OSC 2 - phase */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - phase"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  2, 1,
		  1, 1);

  quantum_synth->synth_1_phase = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_phase);

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

  ags_dial_set_radius(quantum_synth->synth_1_phase,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) quantum_synth->synth_1_phase,
		  3, 1,
		  1, 1);

  /* OSC 2 - volume */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - volume"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  4, 1,
		  1, 1);

  quantum_synth->synth_1_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_volume);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.333);

  ags_dial_set_radius(quantum_synth->synth_1_volume,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) quantum_synth->synth_1_volume,
		  5, 1,
		  1, 1);

  /* seq */
  synth_1_seq_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(synth_1_seq_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(synth_1_seq_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) synth_1_seq_grid,
		  6, 0,
		  8, 4);

  /* synth 1 seq Nr. 0 */
  quantum_synth->synth_1_seq_tuning_0 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_seq_tuning_0);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_1_seq_tuning_0,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) quantum_synth->synth_1_seq_tuning_0,
		  0, 0,
		  1, 1);

  /* synth 1 seq Nr. 1 */
  quantum_synth->synth_1_seq_tuning_1 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_seq_tuning_1);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_1_seq_tuning_1,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) quantum_synth->synth_1_seq_tuning_1,
		  1, 0,
		  1, 1);

  /* synth 1 seq Nr. 2 */
  quantum_synth->synth_1_seq_tuning_2 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_seq_tuning_2);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_1_seq_tuning_2,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) quantum_synth->synth_1_seq_tuning_2,
		  2, 0,
		  1, 1);

  /* synth 1 seq Nr. 3 */
  quantum_synth->synth_1_seq_tuning_3 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_seq_tuning_3);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_1_seq_tuning_3,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) quantum_synth->synth_1_seq_tuning_3,
		  3, 0,
		  1, 1);

  /* synth 1 seq Nr. 4 */
  quantum_synth->synth_1_seq_tuning_4 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_seq_tuning_4);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_1_seq_tuning_4,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) quantum_synth->synth_1_seq_tuning_4,
		  4, 0,
		  1, 1);

  /* synth 1 seq Nr. 5 */
  quantum_synth->synth_1_seq_tuning_5 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_seq_tuning_5);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_1_seq_tuning_5,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) quantum_synth->synth_1_seq_tuning_5,
		  5, 0,
		  1, 1);

  /* synth 1 seq Nr. 6 */
  quantum_synth->synth_1_seq_tuning_6 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_seq_tuning_6);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_1_seq_tuning_6,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) quantum_synth->synth_1_seq_tuning_6,
		  6, 0,
		  1, 1);

  /* synth 1 seq Nr. 7 */
  quantum_synth->synth_1_seq_tuning_7 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_seq_tuning_7);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_1_seq_tuning_7,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) quantum_synth->synth_1_seq_tuning_7,
		  7, 0,
		  1, 1);

  /* synth 1 seq pingpong */
  quantum_synth->synth_1_seq_tuning_pingpong = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("Seq 2 tuning - pingpong"));
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) quantum_synth->synth_1_seq_tuning_pingpong,
		  0, 1,
		  4, 1);
  
  quantum_synth->synth_1_seq_tuning_lfo_frequency = NULL;

  /* synth 1 LFO frequency */
  quantum_synth->synth_1_seq_tuning_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 16.0, 0.01);
  gtk_spin_button_set_value(quantum_synth->synth_1_seq_tuning_lfo_frequency,
			    8.0);  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) quantum_synth->synth_1_seq_tuning_lfo_frequency,
		  6, 1,
		  2, 1);

  /* synth 1 seq Nr. 0 */
  quantum_synth->synth_1_seq_volume_0 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_seq_volume_0);

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

  ags_dial_set_radius(quantum_synth->synth_1_seq_volume_0,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) quantum_synth->synth_1_seq_volume_0,
		  0, 2,
		  1, 1);

  /* synth 1 seq Nr. 1 */
  quantum_synth->synth_1_seq_volume_1 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_seq_volume_1);

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

  ags_dial_set_radius(quantum_synth->synth_1_seq_volume_1,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) quantum_synth->synth_1_seq_volume_1,
		  1, 2,
		  1, 1);

  /* synth 1 seq Nr. 2 */
  quantum_synth->synth_1_seq_volume_2 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_seq_volume_2);

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

  ags_dial_set_radius(quantum_synth->synth_1_seq_volume_2,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) quantum_synth->synth_1_seq_volume_2,
		  2, 2,
		  1, 1);

  /* synth 1 seq Nr. 3 */
  quantum_synth->synth_1_seq_volume_3 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_seq_volume_3);

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

  ags_dial_set_radius(quantum_synth->synth_1_seq_volume_3,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) quantum_synth->synth_1_seq_volume_3,
		  3, 2,
		  1, 1);

  /* synth 1 seq Nr. 4 */
  quantum_synth->synth_1_seq_volume_4 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_seq_volume_4);

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

  ags_dial_set_radius(quantum_synth->synth_1_seq_volume_4,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) quantum_synth->synth_1_seq_volume_4,
		  4, 2,
		  1, 1);

  /* synth 1 seq Nr. 5 */
  quantum_synth->synth_1_seq_volume_5 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_seq_volume_5);

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

  ags_dial_set_radius(quantum_synth->synth_1_seq_volume_5,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) quantum_synth->synth_1_seq_volume_5,
		  5, 2,
		  1, 1);

  /* synth 1 seq Nr. 6 */
  quantum_synth->synth_1_seq_volume_6 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_seq_volume_6);

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

  ags_dial_set_radius(quantum_synth->synth_1_seq_volume_6,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) quantum_synth->synth_1_seq_volume_6,
		  6, 2,
		  1, 1);

  /* synth 1 seq Nr. 7 */
  quantum_synth->synth_1_seq_volume_7 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_seq_volume_7);

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

  ags_dial_set_radius(quantum_synth->synth_1_seq_volume_7,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) quantum_synth->synth_1_seq_volume_7,
		  7, 2,
		  1, 1);

  /* synth 1 seq pingpong */
  quantum_synth->synth_1_seq_volume_pingpong = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("Seq 2 volume - pingpong"));
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) quantum_synth->synth_1_seq_volume_pingpong,
		  0, 3,
		  4, 1);

  /* synth 1 LFO frequency */
  quantum_synth->synth_1_seq_volume_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 16.0, 0.01);
  gtk_spin_button_set_value(quantum_synth->synth_1_seq_volume_lfo_frequency,
			    8.0);  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) quantum_synth->synth_1_seq_volume_lfo_frequency,
		  6, 3,
		  2, 1);

  /* OSC 2 - sync enabled */
  quantum_synth->synth_1_sync_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("OSC 2 - sync enabled"));
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) quantum_synth->synth_1_sync_enabled,
		  14, 0,
		  2, 1);

  /* OSC 2 - sync relative attack factor */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync factor"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  14, 1,
		  1, 1);
  
  quantum_synth->synth_1_sync_relative_attack_factor = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_sync_relative_attack_factor);

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

  ags_dial_set_radius(quantum_synth->synth_1_sync_relative_attack_factor,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) quantum_synth->synth_1_sync_relative_attack_factor,
		  15, 1,
		  1, 1);

  /* OSC 2 - sync attack 0 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync attack 0"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  18, 0,
		  1, 1);
  
  quantum_synth->synth_1_sync_attack_0 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_sync_attack_0);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0 * M_PI);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_1_sync_attack_0,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) quantum_synth->synth_1_sync_attack_0,
		  19, 0,
		  1, 1);

  /* OSC 2 - sync phase 0 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync phase 0"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  20, 0,
		  1, 1);
  
  quantum_synth->synth_1_sync_phase_0 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_sync_phase_0);

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

  ags_dial_set_radius(quantum_synth->synth_1_sync_phase_0,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) quantum_synth->synth_1_sync_phase_0,
		  21, 0,
		  1, 1);

  /* OSC 2 - sync attack 1 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync attack 1"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  18, 1,
		  1, 1);
  
  quantum_synth->synth_1_sync_attack_1 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_sync_attack_1);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0 * M_PI);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_1_sync_attack_1,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) quantum_synth->synth_1_sync_attack_1,
		  19, 1,
		  1, 1);

  /* OSC 2 - sync phase 1 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync phase 1"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  20, 1,
		  1, 1);
  
  quantum_synth->synth_1_sync_phase_1 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_sync_phase_1);

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

  ags_dial_set_radius(quantum_synth->synth_1_sync_phase_1,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) quantum_synth->synth_1_sync_phase_1,
		  21, 1,
		  1, 1);

  /* OSC 2 - sync attack 2 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync attack 2"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  18, 2,
		  1, 1);
  
  quantum_synth->synth_1_sync_attack_2 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_sync_attack_2);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0 * M_PI);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_1_sync_attack_2,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) quantum_synth->synth_1_sync_attack_2,
		  19, 2,
		  1, 1);

  /* OSC 2 - sync phase 2 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync phase 2"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  20, 2,
		  1, 1);
  
  quantum_synth->synth_1_sync_phase_2 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_sync_phase_2);

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

  ags_dial_set_radius(quantum_synth->synth_1_sync_phase_2,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) quantum_synth->synth_1_sync_phase_2,
		  21, 2,
		  1, 1);

  /* OSC 2 - sync attack 3 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync attack 3"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  18, 3,
		  1, 1);
  
  quantum_synth->synth_1_sync_attack_3 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_sync_attack_3);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0 * M_PI);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  ags_dial_set_radius(quantum_synth->synth_1_sync_attack_3,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) quantum_synth->synth_1_sync_attack_3,
		  19, 3,
		  1, 1);

  /* OSC 2 - sync phase 3 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync phase 3"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  20, 3,
		  1, 1);
  
  quantum_synth->synth_1_sync_phase_3 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_sync_phase_3);

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

  ags_dial_set_radius(quantum_synth->synth_1_sync_phase_3,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) quantum_synth->synth_1_sync_phase_3,
		  21, 3,
		  1, 1);

  /* sync OSC 2 LFO */
  label = (GtkLabel *) gtk_label_new(i18n("sync OSC 2 LFO"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  22, 0,
		  1, 1);
  
  quantum_synth->synth_1_sync_lfo_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(quantum_synth->synth_1_sync_lfo_oscillator,
			     0);

  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) quantum_synth->synth_1_sync_lfo_oscillator,
		  23, 0,
		  1, 1);

  /* sync OSC 2 LFO frequency */
  label = (GtkLabel *) gtk_label_new(i18n("sync OSC 2 LFO frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  22, 1,
		  1, 1);

  quantum_synth->synth_1_sync_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 10.0, 0.01);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) quantum_synth->synth_1_sync_lfo_frequency,
		  23, 1,
		  1, 1);

  /* OSC 2 - LFO OSC */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - LFO OSC"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  quantum_synth->synth_1_lfo_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(quantum_synth->synth_1_lfo_oscillator,
			     0);

  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) quantum_synth->synth_1_lfo_oscillator,
		  1, 2,
		  1, 1);

  /* OSC 2 - LFO frequency */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - LFO frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  2, 2,
		  1, 1);

  quantum_synth->synth_1_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 16.0, 0.01);

  gtk_spin_button_set_value(quantum_synth->synth_1_lfo_frequency,
			    AGS_FM_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
    
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) quantum_synth->synth_1_lfo_frequency,
		  3, 2,
		  1, 1);

  /* OSC 2 - LFO tuning */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - LFO tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  2, 3,
		  1, 1);

  quantum_synth->synth_1_lfo_tuning = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_lfo_tuning);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);
  gtk_adjustment_set_page_increment(adjustment,
				    10.0);

  gtk_adjustment_set_value(adjustment,
			   AGS_FM_SYNTH_UTIL_DEFAULT_TUNING);

  ags_dial_set_radius(quantum_synth->synth_1_lfo_tuning,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) quantum_synth->synth_1_lfo_tuning,
		  3, 3,
		  1, 1);

  /* OSC 2 - LFO depth */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - LFO depth"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  4, 2,
		  1, 1);

  quantum_synth->synth_1_lfo_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->synth_1_lfo_depth);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   AGS_FM_SYNTH_UTIL_DEFAULT_LFO_DEPTH);

  ags_dial_set_radius(quantum_synth->synth_1_lfo_depth,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) quantum_synth->synth_1_lfo_depth,
		  5, 2,
		  1, 1);

  /* grid */
  misc_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(misc_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(misc_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_widget_set_valign((GtkWidget *) misc_grid,
			GTK_ALIGN_START);  
  gtk_widget_set_halign((GtkWidget *) misc_grid,
			GTK_ALIGN_START);

  gtk_widget_set_hexpand((GtkWidget *) misc_grid,
			 FALSE);

  gtk_box_append(vbox,
		 (GtkWidget *) misc_grid);

  /* pitch type */
  pitch_type_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					   AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_valign((GtkWidget *) pitch_type_hbox,
			GTK_ALIGN_START);  
  gtk_widget_set_halign((GtkWidget *) pitch_type_hbox,
			GTK_ALIGN_START);

  gtk_widget_set_hexpand((GtkWidget *) pitch_type_hbox,
			 FALSE);

  gtk_grid_attach(misc_grid,
		  (GtkWidget *) pitch_type_hbox,
		  0, 0,
		  2, 1);
  
  label = (GtkLabel *) gtk_label_new(i18n("pitch type"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_box_append(pitch_type_hbox,
		 (GtkWidget *) label);

  quantum_synth->pitch_type = (GtkDropDown *) gtk_drop_down_new_from_strings((const gchar * const *) pitch_type_strv);

  gtk_drop_down_set_selected(quantum_synth->pitch_type,
			     2);

  gtk_box_append(pitch_type_hbox,
		 (GtkWidget *) quantum_synth->pitch_type);
  
  /* pitch */
  label = (GtkLabel *) gtk_label_new(i18n("pitch tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(misc_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  quantum_synth->pitch_tuning = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->pitch_tuning);

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

  ags_dial_set_radius(quantum_synth->pitch_tuning,
		      12);
  
  gtk_grid_attach(misc_grid,
		  (GtkWidget *) quantum_synth->pitch_tuning,
		  1, 1,
		  1, 1);
  
  /* noise */
  label = (GtkLabel *) gtk_label_new(i18n("noise gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(misc_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);

  quantum_synth->noise_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->noise_gain);

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

  ags_dial_set_radius(quantum_synth->noise_gain,
		      12);
  
  gtk_grid_attach(misc_grid,
		  (GtkWidget *) quantum_synth->noise_gain,
		  1, 2,
		  1, 1);

  /* effect control */
  effect_vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				       AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_grid_attach(misc_grid,
		  (GtkWidget *) effect_vbox,
		  3, 0,
		  8, 3);
  
  /* chorus grid */
  chorus_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(chorus_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(chorus_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_widget_set_valign((GtkWidget *) chorus_grid,
			GTK_ALIGN_START);  
  gtk_widget_set_halign((GtkWidget *) chorus_grid,
			GTK_ALIGN_START);

  gtk_widget_set_hexpand((GtkWidget *) chorus_grid,
			 FALSE);

  gtk_box_append(effect_vbox,
		 (GtkWidget *) chorus_grid);
  
  /* chorus input volume */
  label = (GtkLabel *) gtk_label_new(i18n("chorus input volume"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);

  quantum_synth->chorus_input_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->chorus_input_volume);

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

  ags_dial_set_radius(quantum_synth->chorus_input_volume,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) quantum_synth->chorus_input_volume,
		  1, 0,
		  1, 1);

  /* chorus output volume */
  label = (GtkLabel *) gtk_label_new(i18n("chorus output volume"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  quantum_synth->chorus_output_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->chorus_output_volume);

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

  ags_dial_set_radius(quantum_synth->chorus_output_volume,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) quantum_synth->chorus_output_volume,
		  1, 1,
		  1, 1);
  
  /* chorus LFO */
  label = (GtkLabel *) gtk_label_new(i18n("chorus LFO"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) label,
		  2, 0,
		  1, 1);
  
  quantum_synth->chorus_lfo_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(quantum_synth->chorus_lfo_oscillator,
			     0);

  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) quantum_synth->chorus_lfo_oscillator,
		  3, 0,
		  1, 1);

  /* chorus LFO frequency */
  label = (GtkLabel *) gtk_label_new(i18n("chorus LFO frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) label,
		  2, 1,
		  1, 1);

  quantum_synth->chorus_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 10.0, 0.01);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) quantum_synth->chorus_lfo_frequency,
		  3, 1,
		  1, 1);

  /* chorus depth */
  label = (GtkLabel *) gtk_label_new(i18n("chorus depth"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) label,
		  4, 0,
		  1, 1);

  quantum_synth->chorus_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->chorus_depth);

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

  ags_dial_set_radius(quantum_synth->chorus_depth,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) quantum_synth->chorus_depth,
		  5, 0,
		  1, 1);

  /* chorus mix */
  label = (GtkLabel *) gtk_label_new(i18n("chorus mix"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) label,
		  4, 1,
		  1, 1);

  quantum_synth->chorus_mix = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->chorus_mix);

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

  ags_dial_set_radius(quantum_synth->chorus_mix,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) quantum_synth->chorus_mix,
		  5, 1,
		  1, 1);

  /* chorus delay */
  label = (GtkLabel *) gtk_label_new(i18n("chorus delay"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) label,
		  4, 2,
		  1, 1);

  quantum_synth->chorus_delay = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(quantum_synth->chorus_delay);

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

  ags_dial_set_radius(quantum_synth->chorus_delay,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) quantum_synth->chorus_delay,
		  5, 2,
		  1, 1);

  /* ext */
  ext_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				    AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_valign((GtkWidget *) ext_hbox,
			GTK_ALIGN_START);  
  gtk_widget_set_halign((GtkWidget *) ext_hbox,
			GTK_ALIGN_START);

  gtk_widget_set_hexpand((GtkWidget *) ext_hbox,
			 FALSE);

  gtk_box_append(vbox,
		 (GtkWidget *) ext_hbox);

  /* tremolo */
  tremolo_frame = (GtkFrame *) gtk_frame_new(i18n("tremolo"));

  gtk_box_append(ext_hbox,
		 (GtkWidget *) tremolo_frame);

  tremolo_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(tremolo_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(tremolo_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_frame_set_child(tremolo_frame,
		      (GtkWidget *) tremolo_grid);

  quantum_synth->tremolo_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) quantum_synth->tremolo_enabled,
		  0, 0,
		  2, 1);

  label = (GtkLabel *) gtk_label_new(i18n("gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  quantum_synth->tremolo_gain = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(quantum_synth->tremolo_gain,
		      12);

  adjustment = ags_dial_get_adjustment(quantum_synth->tremolo_gain);

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

  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) quantum_synth->tremolo_gain,
		  1, 1,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("lfo-depth"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);

  quantum_synth->tremolo_lfo_depth = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(quantum_synth->tremolo_lfo_depth,
		      12);

  adjustment = ags_dial_get_adjustment(quantum_synth->tremolo_lfo_depth);

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

  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) quantum_synth->tremolo_lfo_depth,
		  1, 2,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("lfo-freq"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);

  quantum_synth->tremolo_lfo_freq = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(quantum_synth->tremolo_lfo_freq,
		      12);

  adjustment = ags_dial_get_adjustment(quantum_synth->tremolo_lfo_freq);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   10.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   6.0);

  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) quantum_synth->tremolo_lfo_freq,
		  1, 3,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);

  quantum_synth->tremolo_tuning = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(quantum_synth->tremolo_tuning,
		      12);

  adjustment = ags_dial_get_adjustment(quantum_synth->tremolo_tuning);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) quantum_synth->tremolo_tuning,
		  1, 4,
		  1, 1);
  
  /* vibrato */
  vibrato_frame = (GtkFrame *) gtk_frame_new(i18n("vibrato"));

  gtk_box_append(ext_hbox,
		 (GtkWidget *) vibrato_frame);

  vibrato_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(vibrato_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(vibrato_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_frame_set_child(vibrato_frame,
		      (GtkWidget *) vibrato_grid);

  quantum_synth->vibrato_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) quantum_synth->vibrato_enabled,
		  0, 0,
		  2, 1);

  label = (GtkLabel *) gtk_label_new(i18n("gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  quantum_synth->vibrato_gain = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(quantum_synth->vibrato_gain,
		      12);

  adjustment = ags_dial_get_adjustment(quantum_synth->vibrato_gain);

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

  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) quantum_synth->vibrato_gain,
		  1, 1,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("lfo-depth"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);

  quantum_synth->vibrato_lfo_depth = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(quantum_synth->vibrato_lfo_depth,
		      12);

  adjustment = ags_dial_get_adjustment(quantum_synth->vibrato_lfo_depth);

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
  
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) quantum_synth->vibrato_lfo_depth,
		  1, 2,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("lfo-freq"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);

  quantum_synth->vibrato_lfo_freq = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(quantum_synth->vibrato_lfo_freq,
		      12);
  
  adjustment = ags_dial_get_adjustment(quantum_synth->vibrato_lfo_freq);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   10.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   8.172);
  
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) quantum_synth->vibrato_lfo_freq,
		  1, 3,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);

  quantum_synth->vibrato_tuning = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(quantum_synth->vibrato_tuning,
		      12);

  adjustment = ags_dial_get_adjustment(quantum_synth->vibrato_tuning);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.0);

  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) quantum_synth->vibrato_tuning,
		  1, 4,
		  1, 1);
}

void
ags_quantum_synth_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_quantum_synth_parent_class)->finalize(gobject);
}

void
ags_quantum_synth_connect(AgsConnectable *connectable)
{
  AgsQuantumSynth *quantum_synth;
  
  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) != 0){
    return;
  }

  ags_quantum_synth_parent_connectable_interface->connect(connectable);
  
  /* AgsQuantumSynth */
  quantum_synth = AGS_QUANTUM_SYNTH(connectable);

  /* synth 0 */
  g_signal_connect_after(quantum_synth->synth_0_oscillator, "notify::selected",
			 G_CALLBACK(ags_quantum_synth_synth_0_oscillator_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_octave, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_octave_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_key, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_key_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_phase, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_phase_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_volume, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_volume_callback), quantum_synth);

  /* seq tuning */
  g_signal_connect_after(quantum_synth->synth_0_seq_tuning_0, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_seq_tuning_0_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_seq_tuning_1, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_seq_tuning_1_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_seq_tuning_2, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_seq_tuning_2_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_seq_tuning_3, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_seq_tuning_3_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_seq_tuning_4, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_seq_tuning_4_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_seq_tuning_5, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_seq_tuning_5_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_seq_tuning_6, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_seq_tuning_6_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_seq_tuning_7, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_seq_tuning_7_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_seq_tuning_pingpong, "toggled",
			 G_CALLBACK(ags_quantum_synth_synth_0_seq_tuning_pingpong_callback), quantum_synth);

  /* seq volume */
  g_signal_connect_after(quantum_synth->synth_0_seq_volume_0, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_seq_volume_0_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_seq_volume_1, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_seq_volume_1_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_seq_volume_2, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_seq_volume_2_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_seq_volume_3, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_seq_volume_3_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_seq_volume_4, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_seq_volume_4_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_seq_volume_5, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_seq_volume_5_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_seq_volume_6, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_seq_volume_6_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_seq_volume_7, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_seq_volume_7_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_seq_volume_pingpong, "toggled",
			 G_CALLBACK(ags_quantum_synth_synth_0_seq_volume_pingpong_callback), quantum_synth);
  
  /* sync */
  g_signal_connect_after(quantum_synth->synth_0_sync_enabled, "toggled",
			 G_CALLBACK(ags_quantum_synth_synth_0_sync_enabled_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_sync_relative_attack_factor, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_sync_relative_attack_factor_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_sync_attack_0, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_sync_attack_0_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_sync_phase_0, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_sync_phase_0_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_sync_attack_1, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_sync_attack_1_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_sync_phase_1, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_sync_phase_1_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_sync_attack_2, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_sync_attack_2_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_sync_phase_2, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_sync_phase_2_callback), quantum_synth);

  g_signal_connect_after(quantum_synth->synth_0_sync_attack_3, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_sync_attack_3_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_sync_phase_3, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_sync_phase_3_callback), quantum_synth);

  /* LFO */
  g_signal_connect_after(quantum_synth->synth_0_sync_lfo_oscillator, "notify::selected",
			 G_CALLBACK(ags_quantum_synth_synth_0_sync_lfo_oscillator_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_sync_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_sync_lfo_frequency_callback), quantum_synth);

  g_signal_connect_after(quantum_synth->synth_0_lfo_oscillator, "notify::selected",
			 G_CALLBACK(ags_quantum_synth_synth_0_lfo_oscillator_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_lfo_frequency_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_lfo_depth, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_lfo_depth_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_0_lfo_tuning, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_0_lfo_tuning_callback), quantum_synth);

  /* synth 1 */
  g_signal_connect_after(quantum_synth->synth_1_oscillator, "notify::selected",
			 G_CALLBACK(ags_quantum_synth_synth_1_oscillator_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_octave, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_octave_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_key, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_key_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_phase, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_phase_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_volume, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_volume_callback), quantum_synth);

  /* seq tuning */
  g_signal_connect_after(quantum_synth->synth_1_seq_tuning_0, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_seq_tuning_0_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_seq_tuning_1, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_seq_tuning_1_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_seq_tuning_2, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_seq_tuning_2_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_seq_tuning_3, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_seq_tuning_3_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_seq_tuning_4, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_seq_tuning_4_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_seq_tuning_5, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_seq_tuning_5_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_seq_tuning_6, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_seq_tuning_6_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_seq_tuning_7, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_seq_tuning_7_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_seq_tuning_pingpong, "toggled",
			 G_CALLBACK(ags_quantum_synth_synth_1_seq_tuning_pingpong_callback), quantum_synth);

  /* seq volume */
  g_signal_connect_after(quantum_synth->synth_1_seq_volume_0, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_seq_volume_0_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_seq_volume_1, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_seq_volume_1_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_seq_volume_2, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_seq_volume_2_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_seq_volume_3, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_seq_volume_3_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_seq_volume_4, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_seq_volume_4_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_seq_volume_5, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_seq_volume_5_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_seq_volume_6, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_seq_volume_6_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_seq_volume_7, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_seq_volume_7_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_seq_volume_pingpong, "toggled",
			 G_CALLBACK(ags_quantum_synth_synth_1_seq_volume_pingpong_callback), quantum_synth);
  
  /* sync */  
  g_signal_connect_after(quantum_synth->synth_1_sync_enabled, "toggled",
			 G_CALLBACK(ags_quantum_synth_synth_1_sync_enabled_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_sync_relative_attack_factor, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_sync_relative_attack_factor_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_sync_attack_0, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_sync_attack_0_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_sync_phase_0, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_sync_phase_0_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_sync_attack_1, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_sync_attack_1_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_sync_phase_1, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_sync_phase_1_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_sync_attack_2, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_sync_attack_2_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_sync_phase_2, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_sync_phase_2_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_sync_attack_3, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_sync_attack_3_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_sync_phase_3, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_sync_phase_3_callback), quantum_synth);

  /* LFO */
  g_signal_connect_after(quantum_synth->synth_1_sync_lfo_oscillator, "notify::selected",
			 G_CALLBACK(ags_quantum_synth_synth_1_sync_lfo_oscillator_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_sync_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_sync_lfo_frequency_callback), quantum_synth);

  g_signal_connect_after(quantum_synth->synth_1_lfo_oscillator, "notify::selected",
			 G_CALLBACK(ags_quantum_synth_synth_1_lfo_oscillator_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_lfo_frequency_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_lfo_depth, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_lfo_depth_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->synth_1_lfo_tuning, "value-changed",
			 G_CALLBACK(ags_quantum_synth_synth_1_lfo_tuning_callback), quantum_synth);

  /* pitch */
  g_signal_connect((GObject *) quantum_synth->pitch_type, "notify::selected",
		   G_CALLBACK(ags_quantum_synth_pitch_type_callback), (gpointer) quantum_synth);

  g_signal_connect_after(quantum_synth->pitch_tuning, "value-changed",
			 G_CALLBACK(ags_quantum_synth_pitch_tuning_callback), quantum_synth);

  /* noise */
  g_signal_connect_after(quantum_synth->noise_gain, "value-changed",
			 G_CALLBACK(ags_quantum_synth_noise_gain_callback), quantum_synth);

  /* chorus */
  //  g_signal_connect_after(quantum_synth->chorus_enabled, "toggled",
  //			 G_CALLBACK(ags_quantum_synth_chorus_enabled_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->chorus_input_volume, "value-changed",
			 G_CALLBACK(ags_quantum_synth_chorus_input_volume_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->chorus_output_volume, "value-changed",
			 G_CALLBACK(ags_quantum_synth_chorus_output_volume_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->chorus_lfo_oscillator, "notify::selected",
			 G_CALLBACK(ags_quantum_synth_chorus_lfo_oscillator_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->chorus_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_quantum_synth_chorus_lfo_frequency_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->chorus_depth, "value-changed",
			 G_CALLBACK(ags_quantum_synth_chorus_depth_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->chorus_mix, "value-changed",
			 G_CALLBACK(ags_quantum_synth_chorus_mix_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->chorus_delay, "value-changed",
			 G_CALLBACK(ags_quantum_synth_chorus_delay_callback), quantum_synth);

  g_signal_connect_after(quantum_synth->chorus_input_volume, "value-changed",
			 G_CALLBACK(ags_quantum_synth_chorus_input_volume_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->chorus_output_volume, "value-changed",
			 G_CALLBACK(ags_quantum_synth_chorus_output_volume_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->chorus_lfo_oscillator, "notify::selected",
			 G_CALLBACK(ags_quantum_synth_chorus_lfo_oscillator_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->chorus_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_quantum_synth_chorus_lfo_frequency_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->chorus_depth, "value-changed",
			 G_CALLBACK(ags_quantum_synth_chorus_depth_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->chorus_mix, "value-changed",
			 G_CALLBACK(ags_quantum_synth_chorus_mix_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->chorus_delay, "value-changed",
			 G_CALLBACK(ags_quantum_synth_chorus_delay_callback), quantum_synth);

  /* tremolo */
  g_signal_connect_after(quantum_synth->tremolo_enabled, "toggled",
			 G_CALLBACK(ags_quantum_synth_tremolo_enabled_callback), quantum_synth);

  g_signal_connect_after(quantum_synth->tremolo_gain, "value-changed",
			 G_CALLBACK(ags_quantum_synth_tremolo_gain_callback), quantum_synth);
  
  g_signal_connect_after(quantum_synth->tremolo_lfo_depth, "value-changed",
			 G_CALLBACK(ags_quantum_synth_tremolo_lfo_depth_callback), quantum_synth);

  g_signal_connect_after(quantum_synth->tremolo_lfo_freq, "value-changed",
			 G_CALLBACK(ags_quantum_synth_tremolo_lfo_freq_callback), quantum_synth);

  g_signal_connect_after(quantum_synth->tremolo_tuning, "value-changed",
			 G_CALLBACK(ags_quantum_synth_tremolo_tuning_callback), quantum_synth);

  /* vibrato */
  g_signal_connect_after(quantum_synth->vibrato_enabled, "toggled",
			 G_CALLBACK(ags_quantum_synth_vibrato_enabled_callback), quantum_synth);

  g_signal_connect_after(quantum_synth->vibrato_gain, "value-changed",
			 G_CALLBACK(ags_quantum_synth_vibrato_gain_callback), quantum_synth);

  g_signal_connect_after(quantum_synth->vibrato_lfo_depth, "value-changed",
			 G_CALLBACK(ags_quantum_synth_vibrato_lfo_depth_callback), quantum_synth);

  g_signal_connect_after(quantum_synth->vibrato_lfo_freq, "value-changed",
			 G_CALLBACK(ags_quantum_synth_vibrato_lfo_freq_callback), quantum_synth);

  g_signal_connect_after(quantum_synth->vibrato_tuning, "value-changed",
			 G_CALLBACK(ags_quantum_synth_vibrato_tuning_callback), quantum_synth);
}

void
ags_quantum_synth_disconnect(AgsConnectable *connectable)
{
  AgsQuantumSynth *quantum_synth;
  
  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) == 0){
    return;
  }

  ags_quantum_synth_parent_connectable_interface->disconnect(connectable);

  /* AgsQuantumSynth */
  quantum_synth = AGS_QUANTUM_SYNTH(connectable);

  /* synth 0 */
  g_object_disconnect(quantum_synth->synth_0_oscillator,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_quantum_synth_synth_0_oscillator_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_octave,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_octave_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_key,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_key_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_phase,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_phase_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_volume_callback),
		      quantum_synth,
		      NULL);

  /* seq tuning */
  g_object_disconnect(quantum_synth->synth_0_seq_tuning_0,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_seq_tuning_0_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_seq_tuning_1,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_seq_tuning_1_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_seq_tuning_2,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_seq_tuning_2_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_seq_tuning_3,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_seq_tuning_3_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_seq_tuning_4,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_seq_tuning_4_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_seq_tuning_5,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_seq_tuning_5_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_seq_tuning_6,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_seq_tuning_6_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_seq_tuning_7,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_seq_tuning_7_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_seq_tuning_pingpong,
		      "any_signal::toggled",
		      G_CALLBACK(ags_quantum_synth_synth_0_seq_tuning_pingpong_callback),
		      quantum_synth,
		      NULL);

  /* seq volume */
  g_object_disconnect(quantum_synth->synth_0_seq_volume_0,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_seq_volume_0_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_seq_volume_1,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_seq_volume_1_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_seq_volume_2,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_seq_volume_2_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_seq_volume_3,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_seq_volume_3_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_seq_volume_4,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_seq_volume_4_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_seq_volume_5,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_seq_volume_5_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_seq_volume_6,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_seq_volume_6_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_seq_volume_7,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_seq_volume_7_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_seq_volume_pingpong,
		      "any_signal::toggled",
		      G_CALLBACK(ags_quantum_synth_synth_0_seq_volume_pingpong_callback),
		      quantum_synth,
		      NULL);
  
  /* sync */
  g_object_disconnect(quantum_synth->synth_0_sync_enabled,
		      "any_signal::toggled",
		      G_CALLBACK(ags_quantum_synth_synth_0_sync_enabled_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_sync_relative_attack_factor,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_sync_relative_attack_factor_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_sync_attack_0,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_sync_attack_0_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_sync_phase_0,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_sync_phase_0_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_sync_attack_1,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_sync_attack_1_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_sync_phase_1,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_sync_phase_1_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_sync_attack_2,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_sync_attack_2_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_sync_phase_2,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_sync_phase_2_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_sync_attack_3,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_sync_attack_3_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_sync_phase_3,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_sync_phase_3_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_sync_lfo_oscillator,
		      "any_signal::changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_sync_lfo_oscillator_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_sync_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_sync_lfo_frequency_callback),
		      quantum_synth,
		      NULL);

  /* LFO */
  g_object_disconnect(quantum_synth->synth_0_lfo_oscillator,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_quantum_synth_synth_0_lfo_oscillator_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_lfo_frequency_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_lfo_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_lfo_depth_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_0_lfo_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_0_lfo_tuning_callback),
		      quantum_synth,
		      NULL);

  /* synth 1 */
  g_object_disconnect(quantum_synth->synth_1_oscillator,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_quantum_synth_synth_1_oscillator_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_octave,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_octave_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_key,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_key_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_phase,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_phase_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_volume_callback),
		      quantum_synth,
		      NULL);

  /* seq tuning */
  g_object_disconnect(quantum_synth->synth_1_seq_tuning_0,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_seq_tuning_0_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_seq_tuning_1,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_seq_tuning_1_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_seq_tuning_2,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_seq_tuning_2_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_seq_tuning_3,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_seq_tuning_3_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_seq_tuning_4,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_seq_tuning_4_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_seq_tuning_5,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_seq_tuning_5_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_seq_tuning_6,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_seq_tuning_6_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_seq_tuning_7,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_seq_tuning_7_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_seq_tuning_pingpong,
		      "any_signal::toggled",
		      G_CALLBACK(ags_quantum_synth_synth_1_seq_tuning_pingpong_callback),
		      quantum_synth,
		      NULL);

  /* seq volume */
  g_object_disconnect(quantum_synth->synth_1_seq_volume_0,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_seq_volume_0_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_seq_volume_1,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_seq_volume_1_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_seq_volume_2,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_seq_volume_2_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_seq_volume_3,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_seq_volume_3_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_seq_volume_4,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_seq_volume_4_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_seq_volume_5,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_seq_volume_5_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_seq_volume_6,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_seq_volume_6_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_seq_volume_7,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_seq_volume_7_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_seq_volume_pingpong,
		      "any_signal::toggled",
		      G_CALLBACK(ags_quantum_synth_synth_1_seq_volume_pingpong_callback),
		      quantum_synth,
		      NULL);
  
  /* sync */
  g_object_disconnect(quantum_synth->synth_1_sync_enabled,
		      "any_signal::toggled",
		      G_CALLBACK(ags_quantum_synth_synth_1_sync_enabled_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_sync_relative_attack_factor,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_sync_relative_attack_factor_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_sync_attack_0,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_sync_attack_0_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_sync_phase_0,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_sync_phase_0_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_sync_attack_1,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_sync_attack_1_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_sync_phase_1,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_sync_phase_1_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_sync_attack_2,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_sync_attack_2_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_sync_phase_2,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_sync_phase_2_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_sync_attack_3,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_sync_attack_3_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_sync_phase_3,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_sync_phase_3_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_sync_lfo_oscillator,
		      "any_signal::changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_sync_lfo_oscillator_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_sync_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_sync_lfo_frequency_callback),
		      quantum_synth,
		      NULL);

  /* LFO */
  g_object_disconnect(quantum_synth->synth_1_lfo_oscillator,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_quantum_synth_synth_1_lfo_oscillator_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_lfo_frequency_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_lfo_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_lfo_depth_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->synth_1_lfo_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_synth_1_lfo_tuning_callback),
		      quantum_synth,
		      NULL);

  /* pitch */
  g_object_disconnect((GObject *) quantum_synth->pitch_type,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_quantum_synth_pitch_type_callback),
		      (gpointer) quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->pitch_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_pitch_tuning_callback),
		      quantum_synth,
		      NULL);

  /* noise */
  g_object_disconnect(quantum_synth->noise_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_noise_gain_callback),
		      quantum_synth,
		      NULL);

  /* chorus */
  //  g_object_disconnect(quantum_synth->chorus_enabled,
  //		      "any_signal::toggled",
  //			 G_CALLBACK(ags_quantum_synth_chorus_enabled_callback),
  //		      quantum_synth,
  //		      NULL);
  
  g_object_disconnect(quantum_synth->chorus_input_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_chorus_input_volume_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->chorus_output_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_chorus_output_volume_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->chorus_lfo_oscillator,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_quantum_synth_chorus_lfo_oscillator_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->chorus_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_chorus_lfo_frequency_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->chorus_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_chorus_depth_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->chorus_mix,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_chorus_mix_callback),
		      quantum_synth,
		      NULL);
  
  g_object_disconnect(quantum_synth->chorus_delay,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_chorus_delay_callback),
		      quantum_synth,
		      NULL);

  /* tremolo */
  g_object_disconnect((GObject *) quantum_synth->tremolo_enabled,
		      "any_signal::toggled",
		      G_CALLBACK(ags_quantum_synth_tremolo_enabled_callback),
		      quantum_synth,
		      NULL);

  g_object_disconnect((GObject *) quantum_synth->tremolo_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_tremolo_gain_callback),
		      (gpointer) quantum_synth,
		      NULL);

  g_object_disconnect((GObject *) quantum_synth->tremolo_lfo_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_tremolo_lfo_depth_callback),
		      (gpointer) quantum_synth,
		      NULL);

  g_object_disconnect((GObject *) quantum_synth->tremolo_lfo_freq,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_tremolo_lfo_freq_callback),
		      (gpointer) quantum_synth,
		      NULL);

  g_object_disconnect((GObject *) quantum_synth->tremolo_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_tremolo_tuning_callback),
		      (gpointer) quantum_synth,
		      NULL);

  /* vibrato */
  g_object_disconnect((GObject *) quantum_synth->vibrato_enabled,
		      "any_signal::toggled",
		      G_CALLBACK(ags_quantum_synth_vibrato_enabled_callback),
		      quantum_synth,
		      NULL);

  g_object_disconnect((GObject *) quantum_synth->vibrato_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_vibrato_gain_callback),
		      (gpointer) quantum_synth,
		      NULL);

  g_object_disconnect((GObject *) quantum_synth->vibrato_lfo_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_vibrato_lfo_depth_callback),
		      (gpointer) quantum_synth,
		      NULL);

  g_object_disconnect((GObject *) quantum_synth->vibrato_lfo_freq,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_quantum_synth_vibrato_lfo_freq_callback),
		      (gpointer) quantum_synth,
		      NULL);
}

void
ags_quantum_synth_resize_audio_channels(AgsMachine *machine,
					guint audio_channels, guint audio_channels_old,
					gpointer data)
{
  AgsQuantumSynth *quantum_synth;

  quantum_synth = (AgsQuantumSynth *) machine;

  if(audio_channels > audio_channels_old){    
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_quantum_synth_input_map_recall(quantum_synth,
					 audio_channels_old,
					 0);
      
      ags_quantum_synth_output_map_recall(quantum_synth,
					  audio_channels_old,
					  0);
    }
  }
}

void
ags_quantum_synth_resize_pads(AgsMachine *machine, GType type,
			      guint pads, guint pads_old,
			      gpointer data)
{
  AgsQuantumSynth *quantum_synth;

  gboolean grow;

  quantum_synth = (AgsQuantumSynth *) machine;
  
  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(type == AGS_TYPE_INPUT){
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	/* depending on destination */
	ags_quantum_synth_input_map_recall(quantum_synth,
					   0,
					   pads_old);
      }
    }else{
      quantum_synth->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_quantum_synth_output_map_recall(quantum_synth,
					    0,
					    pads_old);
      }
    }else{
      quantum_synth->mapped_output_pad = pads;
    }
  }
}

void
ags_quantum_synth_map_recall(AgsMachine *machine)
{
  AgsQuantumSynth *quantum_synth;
  
  AgsAudio *audio;

  GList *start_recall, *recall;

  gint position;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  quantum_synth = AGS_QUANTUM_SYNTH(machine);

  audio = machine->audio;
  
  position = 0;

  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       quantum_synth->playback_play_container, quantum_synth->playback_recall_container,
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
				       quantum_synth->seq_synth_play_container, quantum_synth->seq_synth_recall_container,
				       "ags-fx-seq-synth",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-tremolo */
  start_recall = ags_fx_factory_create(audio,
				       quantum_synth->tremolo_play_container, quantum_synth->tremolo_recall_container,
				       "ags-fx-tremolo",
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
				       quantum_synth->volume_play_container, quantum_synth->volume_recall_container,
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
				       quantum_synth->envelope_play_container, quantum_synth->envelope_recall_container,
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
				       quantum_synth->buffer_play_container, quantum_synth->buffer_recall_container,
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
  ags_quantum_synth_input_map_recall(quantum_synth,
				     0,
				     0);

  /* depending on destination */
  ags_quantum_synth_output_map_recall(quantum_synth,
				      0,
				      0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_quantum_synth_parent_class)->map_recall(machine);  
}

void
ags_quantum_synth_input_map_recall(AgsQuantumSynth *quantum_synth,
				   guint audio_channel_start,
				   guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(quantum_synth->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(quantum_synth)->audio;

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
				       quantum_synth->playback_play_container, quantum_synth->playback_recall_container,
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
				       quantum_synth->seq_synth_play_container, quantum_synth->seq_synth_recall_container,
				       "ags-fx-seq-synth",
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
				       quantum_synth->volume_play_container, quantum_synth->volume_recall_container,
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
				       quantum_synth->envelope_play_container, quantum_synth->envelope_recall_container,
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
				       quantum_synth->buffer_play_container, quantum_synth->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  quantum_synth->mapped_input_pad = input_pads;
}

void
ags_quantum_synth_output_map_recall(AgsQuantumSynth *quantum_synth,
				    guint audio_channel_start,
				    guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;

  if(quantum_synth->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(quantum_synth)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);
  
  quantum_synth->mapped_output_pad = output_pads;
}

void
ags_quantum_synth_refresh_port(AgsMachine *machine)
{
  AgsQuantumSynth *quantum_synth;
  
  GList *start_play, *start_recall, *recall;

  quantum_synth = (AgsQuantumSynth *) machine;
  
  start_play = ags_audio_get_play(machine->audio);
  start_recall = ags_audio_get_recall(machine->audio);

  recall =
    start_recall = g_list_concat(start_play, start_recall);

  machine->flags |= AGS_MACHINE_NO_UPDATE;

  if((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SEQ_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    /* synth-0 oscillator */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-oscillator", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_drop_down_set_selected(quantum_synth->synth_0_oscillator,
				 (gint) g_value_get_float(&value));


      g_object_unref(port);
    }

    /* synth-0 octave */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-octave", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_octave,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 key */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-key", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_key,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 phase */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-phase", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_phase,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 volume */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-volume", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_volume,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq tuning 0 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-tuning-0", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_seq_tuning_0,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq tuning 1 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-tuning-1", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_seq_tuning_1,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq tuning 2 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-tuning-2", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_seq_tuning_2,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq tuning 3 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-tuning-3", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_seq_tuning_3,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq tuning 4 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-tuning-4", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_seq_tuning_4,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq tuning 5 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-tuning-5", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_seq_tuning_5,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq tuning 6 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-tuning-6", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_seq_tuning_6,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq tuning 7 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-tuning-7", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_seq_tuning_7,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq tuning pingpong */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-tuning-pingpong", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      if(g_value_get_float(&value) != 0.0){
	gtk_check_button_set_active(quantum_synth->synth_0_seq_tuning_pingpong,
				    TRUE);
      }else{
	gtk_check_button_set_active(quantum_synth->synth_0_seq_tuning_pingpong,
				    FALSE);
      }

      g_object_unref(port);
    }

    /* synth-0 seq volume 0 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-volume-0", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_seq_volume_0,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq volume 1 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-volume-1", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_seq_volume_1,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq volume 2 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-volume-2", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_seq_volume_2,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq volume 3 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-volume-3", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_seq_volume_3,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq volume 4 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-volume-4", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_seq_volume_4,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq volume 5 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-volume-5", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_seq_volume_5,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq volume 6 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-volume-6", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_seq_volume_6,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq volume 7 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-volume-7", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_seq_volume_7,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq volume pingpong */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-volume-pingpong", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      if(g_value_get_float(&value) != 0.0){
	gtk_check_button_set_active(quantum_synth->synth_0_seq_volume_pingpong,
				    TRUE);
      }else{
	gtk_check_button_set_active(quantum_synth->synth_0_seq_volume_pingpong,
				    FALSE);
      }

      g_object_unref(port);
    }

    /* synth-0 volume */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-volume", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_volume,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 lfo oscillator */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-lfo-oscillator", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_drop_down_set_selected(quantum_synth->synth_0_lfo_oscillator,
				 (gint) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 lfo frequency */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-lfo-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_spin_button_set_value(quantum_synth->synth_0_lfo_frequency,
				(gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 lfo depth */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-lfo-depth", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_lfo_depth,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 lfo tuning */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-lfo-tuning", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_lfo_tuning,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 sync enabled */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-sync-enabled", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      if(g_value_get_float(&value) != 0.0){
	gtk_check_button_set_active(quantum_synth->synth_0_sync_enabled,
				    TRUE);
      }else{
	gtk_check_button_set_active(quantum_synth->synth_0_sync_enabled,
				    FALSE);
      }

      g_object_unref(port);
    }

    /* synth-0 sync relative attack factor */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-sync-relative-attack-factor", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_sync_relative_attack_factor,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 sync attack 0 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-sync-attack-0", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_sync_attack_0,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 sync phase 0 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-sync-phase-0", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_sync_phase_0,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 sync attack 1 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-sync-attack-1", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_sync_attack_1,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 sync phase 1 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-sync-phase-1", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_sync_phase_1,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 sync attack 2 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-sync-attack-2", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_sync_attack_2,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 sync phase 2 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-sync-phase-2", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_0_sync_phase_2,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 sync lfo oscillator */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-sync-lfo-oscillator", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_drop_down_set_selected(quantum_synth->synth_0_sync_lfo_oscillator,
				 (gint) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 sync lfo frequency */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-sync-lfo-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_spin_button_set_value(quantum_synth->synth_0_sync_lfo_frequency,
				(gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 oscillator */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-oscillator", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_drop_down_set_selected(quantum_synth->synth_1_oscillator,
				 (gint) g_value_get_float(&value));


      g_object_unref(port);
    }

    /* synth-1 octave */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-octave", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_octave,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 key */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-key", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_key,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 phase */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-phase", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_phase,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 volume */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-volume", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_volume,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq tuning 0 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-tuning-0", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_seq_tuning_0,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq tuning 1 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-tuning-1", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_seq_tuning_1,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq tuning 2 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-tuning-2", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_seq_tuning_2,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq tuning 3 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-tuning-3", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_seq_tuning_3,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq tuning 4 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-tuning-4", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_seq_tuning_4,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq tuning 5 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-tuning-5", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_seq_tuning_5,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq tuning 6 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-tuning-6", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_seq_tuning_6,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq tuning 7 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-tuning-7", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_seq_tuning_7,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq tuning pingpong */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-tuning-pingpong", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      if(g_value_get_float(&value) != 0.0){
	gtk_check_button_set_active(quantum_synth->synth_1_seq_tuning_pingpong,
				    TRUE);
      }else{
	gtk_check_button_set_active(quantum_synth->synth_1_seq_tuning_pingpong,
				    FALSE);
      }

      g_object_unref(port);
    }

    /* synth-1 seq volume 0 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-volume-0", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_seq_volume_0,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq volume 1 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-volume-1", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_seq_volume_1,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq volume 2 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-volume-2", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_seq_volume_2,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq volume 3 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-volume-3", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_seq_volume_3,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq volume 4 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-volume-4", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_seq_volume_4,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq volume 5 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-volume-5", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_seq_volume_5,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq volume 6 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-volume-6", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_seq_volume_6,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq volume 7 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-volume-7", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_seq_volume_7,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq volume pingpong */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-volume-pingpong", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      if(g_value_get_float(&value) != 0.0){
	gtk_check_button_set_active(quantum_synth->synth_1_seq_volume_pingpong,
				    TRUE);
      }else{
	gtk_check_button_set_active(quantum_synth->synth_1_seq_volume_pingpong,
				    FALSE);
      }

      g_object_unref(port);
    }
    
    /* synth-1 lfo oscillator */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-lfo-oscillator", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_drop_down_set_selected(quantum_synth->synth_1_lfo_oscillator,
				 (gint) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 lfo frequency */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-lfo-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_spin_button_set_value(quantum_synth->synth_1_lfo_frequency,
				(gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 lfo depth */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-lfo-depth", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_lfo_depth,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 lfo tuning */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-lfo-tuning", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_lfo_tuning,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 sync enabled */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-sync-enabled", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      if(g_value_get_float(&value) != 0.0){
	gtk_check_button_set_active(quantum_synth->synth_1_sync_enabled,
				    TRUE);
      }else{
	gtk_check_button_set_active(quantum_synth->synth_1_sync_enabled,
				    FALSE);
      }

      g_object_unref(port);
    }

    /* synth-1 sync relative attack factor */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-sync-relative-attack-factor", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_sync_relative_attack_factor,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 sync attack 0 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-sync-attack-0", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_sync_attack_0,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 sync phase 0 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-sync-phase-0", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_sync_phase_0,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 sync attack 1 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-sync-attack-1", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_sync_attack_1,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 sync phase 1 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-sync-phase-1", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_sync_phase_1,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 sync attack 2 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-sync-attack-2", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_sync_attack_2,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 sync phase 2 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-sync-phase-2", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->synth_1_sync_phase_2,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 sync lfo oscillator */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-sync-lfo-oscillator", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_drop_down_set_selected(quantum_synth->synth_1_sync_lfo_oscillator,
				 (gint) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 sync lfo frequency */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-sync-lfo-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_spin_button_set_value(quantum_synth->synth_1_sync_lfo_frequency,
				(gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* pitch type */
    port = NULL;

    g_object_get(recall->data,
		 "pitch-type", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_drop_down_set_selected(quantum_synth->pitch_type,
				 (guint) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* pitch tuning */
    port = NULL;

    g_object_get(recall->data,
		 "pitch-tuning", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->pitch_tuning,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }
    
    /* noise gain */
    port = NULL;

    g_object_get(recall->data,
		 "noise-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->noise_gain,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* chorus enabled */
    port = NULL;

    g_object_get(recall->data,
		 "chorus-enabled", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      if(g_value_get_float(&value) != 0.0){
	gtk_check_button_set_active(quantum_synth->chorus_enabled,
				    TRUE);
      }else{
	gtk_check_button_set_active(quantum_synth->chorus_enabled,
				    FALSE);
      }

      g_object_unref(port);
    }
    
    /* chorus LFO oscillator */
    port = NULL;

    g_object_get(recall->data,
		 "chorus-lfo-oscillator", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_drop_down_set_selected(quantum_synth->chorus_lfo_oscillator,
				 (gint) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* chorus LFO frequency */
    port = NULL;

    g_object_get(recall->data,
		 "chorus-lfo-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_spin_button_set_value(quantum_synth->chorus_lfo_frequency,
				(gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* chorus depth */
    port = NULL;

    g_object_get(recall->data,
		 "chorus-depth", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->chorus_depth,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* chorus mix */
    port = NULL;

    g_object_get(recall->data,
		 "chorus-mix", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->chorus_mix,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* chorus delay */
    port = NULL;

    g_object_get(recall->data,
		 "chorus-delay", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->chorus_delay,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* vibrato gain */
    port = NULL;

    g_object_get(recall->data,
		 "vibrato-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->vibrato_gain,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* vibrato LFO depth */
    port = NULL;

    g_object_get(recall->data,
		 "vibrato-lfo-depth", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->vibrato_lfo_depth,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* vibrato LFO freq */
    port = NULL;

    g_object_get(recall->data,
		 "vibrato-lfo-freq", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->vibrato_lfo_freq,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* vibrato tuning */
    port = NULL;

    g_object_get(recall->data,
		 "vibrato-tuning", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->vibrato_tuning,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }
  }

  recall = start_recall;
  
  if((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TREMOLO_AUDIO)) != NULL){
    AgsPort *port;

    /* tremolo gain */
    port = NULL;

    g_object_get(recall->data,
		 "tremolo-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->tremolo_gain,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* tremolo LFO depth */
    port = NULL;

    g_object_get(recall->data,
		 "tremolo-lfo-depth", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->tremolo_lfo_depth,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* tremolo LFO freq */
    port = NULL;

    g_object_get(recall->data,
		 "tremolo-lfo-freq", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->tremolo_lfo_freq,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* tremolo tuning */
    port = NULL;

    g_object_get(recall->data,
		 "tremolo-tuning", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(quantum_synth->tremolo_tuning,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }
  }
  
  machine->flags &= (~AGS_MACHINE_NO_UPDATE);
}

/**
 * ags_quantum_synth_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsQuantumSynth
 *
 * Returns: the new #AgsQuantumSynth
 *
 * Since: 3.14.0
 */
AgsQuantumSynth*
ags_quantum_synth_new(GObject *soundcard)
{
  AgsQuantumSynth *quantum_synth;

  quantum_synth = (AgsQuantumSynth *) g_object_new(AGS_TYPE_QUANTUM_SYNTH,
						   NULL);

  g_object_set(AGS_MACHINE(quantum_synth)->audio,
	       "output-soundcard", soundcard,
	       NULL);

  return(quantum_synth);
}
