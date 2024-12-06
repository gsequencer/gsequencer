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

#include <ags/app/machine/ags_stargazer_synth.h>
#include <ags/app/machine/ags_stargazer_synth_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_line.h>

#include <math.h>

#include <ags/i18n.h>

void ags_stargazer_synth_class_init(AgsStargazerSynthClass *stargazer_synth);
void ags_stargazer_synth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_stargazer_synth_init(AgsStargazerSynth *stargazer_synth);
void ags_stargazer_synth_finalize(GObject *gobject);

void ags_stargazer_synth_connect(AgsConnectable *connectable);
void ags_stargazer_synth_disconnect(AgsConnectable *connectable);

void ags_stargazer_synth_show(GtkWidget *widget);

void ags_stargazer_synth_resize_audio_channels(AgsMachine *machine,
					       guint audio_channels, guint audio_channels_old,
					       gpointer data);
void ags_stargazer_synth_resize_pads(AgsMachine *machine, GType channel_type,
				     guint pads, guint pads_old,
				     gpointer data);

void ags_stargazer_synth_map_recall(AgsMachine *machine);

void ags_stargazer_synth_input_map_recall(AgsStargazerSynth *stargazer_synth,
					  guint audio_channel_start,
					  guint input_pad_start);
void ags_stargazer_synth_output_map_recall(AgsStargazerSynth *stargazer_synth,
					   guint audio_channel_start,
					   guint output_pad_start);

void ags_stargazer_synth_refresh_port(AgsMachine *machine);

/**
 * SECTION:ags_stargazer_synth
 * @short_description: stargazer synth
 * @title: AgsStargazerSynth
 * @section_id:
 * @include: ags/app/machine/ags_stargazer_synth.h
 *
 * The #AgsStargazerSynth is a composite widget to act as stargazer synth.
 */

static gpointer ags_stargazer_synth_parent_class = NULL;
static AgsConnectableInterface *ags_stargazer_synth_parent_connectable_interface;

GType
ags_stargazer_synth_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_stargazer_synth = 0;

    static const GTypeInfo ags_stargazer_synth_info = {
      sizeof(AgsStargazerSynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_stargazer_synth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsStargazerSynth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_stargazer_synth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_stargazer_synth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_stargazer_synth = g_type_register_static(AGS_TYPE_MACHINE,
						      "AgsStargazerSynth", &ags_stargazer_synth_info,
						      0);
    
    g_type_add_interface_static(ags_type_stargazer_synth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_stargazer_synth);
  }

  return(g_define_type_id__static);
}

void
ags_stargazer_synth_class_init(AgsStargazerSynthClass *stargazer_synth)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_stargazer_synth_parent_class = g_type_class_peek_parent(stargazer_synth);

  /* GObjectClass */
  gobject = (GObjectClass *) stargazer_synth;

  gobject->finalize = ags_stargazer_synth_finalize;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) stargazer_synth;

  machine->map_recall = ags_stargazer_synth_map_recall;

  machine->refresh_port = ags_stargazer_synth_refresh_port;
}

void
ags_stargazer_synth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_stargazer_synth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_stargazer_synth_connect;
  connectable->disconnect = ags_stargazer_synth_disconnect;
}

void
ags_stargazer_synth_init(AgsStargazerSynth *stargazer_synth)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  GtkBox *vbox;
  GtkBox *hbox;
  GtkBox *osc_vbox;
  GtkGrid *synth_0_grid;
  GtkGrid *synth_1_grid;
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

  static const gchar* pitch_type_strv[] = {
    "fluid-interpolate-none",
    "fluid-interpolate-linear",
    "fluid-interpolate-4th-order",
    "fluid-interpolate-7th-order",
    "ags-pitch-2x-alias",    
    "ags-pitch-4x-alias",    
    "ags-pitch-16x-alias",
    NULL,
  };

  application_context = ags_application_context_get_instance();
  
  /* machine counter */
  machine_counter_manager = ags_machine_counter_manager_get_instance();

  machine_counter = ags_machine_counter_manager_find_machine_counter(machine_counter_manager,
								     AGS_TYPE_STARGAZER_SYNTH);

  machine_name = NULL;

  if(machine_counter != NULL){
    machine_name = g_strdup_printf("Default %d",
				   machine_counter->counter);
  
    ags_machine_counter_increment(machine_counter);
  }
  
  g_object_set(stargazer_synth,
	       "machine-name", machine_name,
	       NULL);

  g_free(machine_name);

  /* machine selector */
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  position = g_list_length(window->machine);
  
  ags_machine_selector_popup_insert_machine(composite_editor->machine_selector,
					    position,
					    (AgsMachine *) stargazer_synth);

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
  
  audio = AGS_MACHINE(stargazer_synth)->audio;
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

  AGS_MACHINE(stargazer_synth)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
					  AGS_MACHINE_REVERSE_NOTATION);
  AGS_MACHINE(stargazer_synth)->mapping_flags |= AGS_MACHINE_MONO;

  AGS_MACHINE(stargazer_synth)->input_pad_type = G_TYPE_NONE;
  AGS_MACHINE(stargazer_synth)->input_line_type = G_TYPE_NONE;
  AGS_MACHINE(stargazer_synth)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(stargazer_synth)->output_line_type = G_TYPE_NONE;

  /* audio resize */
  g_signal_connect_after(G_OBJECT(stargazer_synth), "resize-audio-channels",
			 G_CALLBACK(ags_stargazer_synth_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(stargazer_synth), "resize-pads",
			 G_CALLBACK(ags_stargazer_synth_resize_pads), NULL);
  
  /* create widgets */
  stargazer_synth->flags = 0;

  /* mapped IO */
  stargazer_synth->mapped_input_pad = 0;
  stargazer_synth->mapped_output_pad = 0;

  stargazer_synth->playback_play_container = ags_recall_container_new();
  stargazer_synth->playback_recall_container = ags_recall_container_new();

  stargazer_synth->star_synth_play_container = ags_recall_container_new();
  stargazer_synth->star_synth_recall_container = ags_recall_container_new();

  stargazer_synth->volume_play_container = ags_recall_container_new();
  stargazer_synth->volume_recall_container = ags_recall_container_new();

  stargazer_synth->tremolo_play_container = ags_recall_container_new();
  stargazer_synth->tremolo_recall_container = ags_recall_container_new();

  stargazer_synth->envelope_play_container = ags_recall_container_new();
  stargazer_synth->envelope_recall_container = ags_recall_container_new();

  stargazer_synth->buffer_play_container = ags_recall_container_new();
  stargazer_synth->buffer_recall_container = ags_recall_container_new();
   
  /* name and xml type */
  stargazer_synth->name = NULL;
  stargazer_synth->xml_type = "ags-stargazer-synth"; 

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

  gtk_frame_set_child(AGS_MACHINE(stargazer_synth)->frame,
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
  
  stargazer_synth->synth_0_oscillator = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_0_oscillator,
				 "sine");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_0_oscillator,
				 "sawtooth");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_0_oscillator,
				 "triangle");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_0_oscillator,
				 "square");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_0_oscillator,
				 "impulse");

  gtk_combo_box_set_active(stargazer_synth->synth_0_oscillator,
			   0);

  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) stargazer_synth->synth_0_oscillator,
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

  stargazer_synth->synth_0_octave = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_0_octave);

  gtk_adjustment_set_lower(adjustment,
			   -6.0);
  gtk_adjustment_set_upper(adjustment,
			   6.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(stargazer_synth->synth_0_octave,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) stargazer_synth->synth_0_octave,
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

  stargazer_synth->synth_0_key = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_0_key);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   2.0);

  ags_dial_set_radius(stargazer_synth->synth_0_key,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) stargazer_synth->synth_0_key,
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

  stargazer_synth->synth_0_phase = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_0_phase);

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

  ags_dial_set_radius(stargazer_synth->synth_0_phase,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) stargazer_synth->synth_0_phase,
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

  stargazer_synth->synth_0_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_0_volume);

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

  ags_dial_set_radius(stargazer_synth->synth_0_volume,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) stargazer_synth->synth_0_volume,
		  5, 1,
		  1, 1);

  /* OSC 1 - sync enabled */
  stargazer_synth->synth_0_sync_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("OSC 1 - sync enabled"));
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) stargazer_synth->synth_0_sync_enabled,
		  6, 0,
		  1, 1);

  /* OSC 1 - sync relative attack factor */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync factor"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  6, 1,
		  1, 1);
  
  stargazer_synth->synth_0_sync_relative_attack_factor = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_0_sync_relative_attack_factor);

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

  ags_dial_set_radius(stargazer_synth->synth_0_sync_relative_attack_factor,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) stargazer_synth->synth_0_sync_relative_attack_factor,
		  7, 1,
		  1, 1);

  /* OSC 1 - sync attack 0 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync attack 0"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  10, 0,
		  1, 1);
  
  stargazer_synth->synth_0_sync_attack_0 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_0_sync_attack_0);

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

  ags_dial_set_radius(stargazer_synth->synth_0_sync_attack_0,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) stargazer_synth->synth_0_sync_attack_0,
		  11, 0,
		  1, 1);

  /* OSC 1 - sync phase 0 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync phase 0"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  12, 0,
		  1, 1);
  
  stargazer_synth->synth_0_sync_phase_0 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_0_sync_phase_0);

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

  ags_dial_set_radius(stargazer_synth->synth_0_sync_phase_0,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) stargazer_synth->synth_0_sync_phase_0,
		  13, 0,
		  1, 1);

  /* OSC 1 - sync attack 1 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync attack 1"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  10, 1,
		  1, 1);
  
  stargazer_synth->synth_0_sync_attack_1 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_0_sync_attack_1);

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

  ags_dial_set_radius(stargazer_synth->synth_0_sync_attack_1,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) stargazer_synth->synth_0_sync_attack_1,
		  11, 1,
		  1, 1);

  /* OSC 1 - sync phase 1 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync phase 1"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  12, 1,
		  1, 1);
  
  stargazer_synth->synth_0_sync_phase_1 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_0_sync_phase_1);

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

  ags_dial_set_radius(stargazer_synth->synth_0_sync_phase_1,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) stargazer_synth->synth_0_sync_phase_1,
		  13, 1,
		  1, 1);

  /* OSC 1 - sync attack 2 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync attack 2"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  10, 2,
		  1, 1);
  
  stargazer_synth->synth_0_sync_attack_2 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_0_sync_attack_2);

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

  ags_dial_set_radius(stargazer_synth->synth_0_sync_attack_2,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) stargazer_synth->synth_0_sync_attack_2,
		  11, 2,
		  1, 1);

  /* OSC 1 - sync phase 2 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync phase 2"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  12, 2,
		  1, 1);
  
  stargazer_synth->synth_0_sync_phase_2 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_0_sync_phase_2);

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

  ags_dial_set_radius(stargazer_synth->synth_0_sync_phase_2,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) stargazer_synth->synth_0_sync_phase_2,
		  13, 2,
		  1, 1);

  /* OSC 1 - sync attack 3 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync attack 3"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  10, 3,
		  1, 1);
  
  stargazer_synth->synth_0_sync_attack_3 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_0_sync_attack_3);

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

  ags_dial_set_radius(stargazer_synth->synth_0_sync_attack_3,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) stargazer_synth->synth_0_sync_attack_3,
		  11, 3,
		  1, 1);

  /* OSC 1 - sync phase 3 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync phase 3"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  12, 3,
		  1, 1);
  
  stargazer_synth->synth_0_sync_phase_3 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_0_sync_phase_3);

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

  ags_dial_set_radius(stargazer_synth->synth_0_sync_phase_3,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) stargazer_synth->synth_0_sync_phase_3,
		  13, 3,
		  1, 1);
  
  /* sync OSC 1 LFO */
  label = (GtkLabel *) gtk_label_new(i18n("sync OSC 1 LFO"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  14, 0,
		  1, 1);
  
  stargazer_synth->synth_0_sync_lfo_oscillator = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_0_sync_lfo_oscillator,
				 "sine");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_0_sync_lfo_oscillator,
				 "sawtooth");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_0_sync_lfo_oscillator,
				 "triangle");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_0_sync_lfo_oscillator,
				 "square");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_0_sync_lfo_oscillator,
				 "impulse");

  gtk_combo_box_set_active(stargazer_synth->synth_0_sync_lfo_oscillator,
			   0);

  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) stargazer_synth->synth_0_sync_lfo_oscillator,
		  15, 0,
		  1, 1);

  /* sync OSC 1 LFO frequency */
  label = (GtkLabel *) gtk_label_new(i18n("sync OSC 1 LFO frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  14, 1,
		  1, 1);

  stargazer_synth->synth_0_sync_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 10.0, 0.01);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) stargazer_synth->synth_0_sync_lfo_frequency,
		  15, 1,
		  1, 1);

  /* OSC 1 - LFO OSC */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - LFO OSC"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  stargazer_synth->synth_0_lfo_oscillator = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_0_lfo_oscillator,
				 "sine");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_0_lfo_oscillator,
				 "sawtooth");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_0_lfo_oscillator,
				 "triangle");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_0_lfo_oscillator,
				 "square");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_0_lfo_oscillator,
				 "impulse");

  gtk_combo_box_set_active(stargazer_synth->synth_0_lfo_oscillator,
			   0);

  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) stargazer_synth->synth_0_lfo_oscillator,
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

  stargazer_synth->synth_0_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 16.0, 0.01);

  gtk_spin_button_set_value(stargazer_synth->synth_0_lfo_frequency,
			    AGS_FM_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) stargazer_synth->synth_0_lfo_frequency,
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

  stargazer_synth->synth_0_lfo_tuning = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_0_lfo_tuning);

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

  ags_dial_set_radius(stargazer_synth->synth_0_lfo_tuning,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) stargazer_synth->synth_0_lfo_tuning,
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

  stargazer_synth->synth_0_lfo_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_0_lfo_depth);

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

  ags_dial_set_radius(stargazer_synth->synth_0_lfo_depth,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) stargazer_synth->synth_0_lfo_depth,
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
  
  stargazer_synth->synth_1_oscillator = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_1_oscillator,
				 "sine");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_1_oscillator,
				 "sawtooth");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_1_oscillator,
				 "triangle");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_1_oscillator,
				 "square");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_1_oscillator,
				 "impulse");

  gtk_combo_box_set_active(stargazer_synth->synth_1_oscillator,
			   0);

  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) stargazer_synth->synth_1_oscillator,
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

  stargazer_synth->synth_1_octave = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_1_octave);

  gtk_adjustment_set_lower(adjustment,
			   -6.0);
  gtk_adjustment_set_upper(adjustment,
			   6.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(stargazer_synth->synth_1_octave,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) stargazer_synth->synth_1_octave,
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

  stargazer_synth->synth_1_key = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_1_key);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   2.0);
  ags_dial_set_radius(stargazer_synth->synth_1_key,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) stargazer_synth->synth_1_key,
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

  stargazer_synth->synth_1_phase = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_1_phase);

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

  ags_dial_set_radius(stargazer_synth->synth_1_phase,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) stargazer_synth->synth_1_phase,
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

  stargazer_synth->synth_1_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_1_volume);

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

  ags_dial_set_radius(stargazer_synth->synth_1_volume,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) stargazer_synth->synth_1_volume,
		  5, 1,
		  1, 1);

  /* sync OSC 2 LFO */
  label = (GtkLabel *) gtk_label_new(i18n("sync OSC 2 LFO"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  14, 0,
		  1, 1);
  
  stargazer_synth->synth_1_sync_lfo_oscillator = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_1_sync_lfo_oscillator,
				 "sine");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_1_sync_lfo_oscillator,
				 "sawtooth");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_1_sync_lfo_oscillator,
				 "triangle");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_1_sync_lfo_oscillator,
				 "square");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_1_sync_lfo_oscillator,
				 "impulse");

  gtk_combo_box_set_active(stargazer_synth->synth_1_sync_lfo_oscillator,
			   0);

  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) stargazer_synth->synth_1_sync_lfo_oscillator,
		  15, 0,
		  1, 1);

  /* sync OSC 2 LFO frequency */
  label = (GtkLabel *) gtk_label_new(i18n("sync OSC 2 LFO frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  14, 1,
		  1, 1);

  stargazer_synth->synth_1_sync_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 10.0, 0.01);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) stargazer_synth->synth_1_sync_lfo_frequency,
		  15, 1,
		  1, 1);

  /* OSC 2 - LFO OSC */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - LFO OSC"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  stargazer_synth->synth_1_lfo_oscillator = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_1_lfo_oscillator,
				 "sine");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_1_lfo_oscillator,
				 "sawtooth");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_1_lfo_oscillator,
				 "triangle");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_1_lfo_oscillator,
				 "square");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->synth_1_lfo_oscillator,
				 "impulse");

  gtk_combo_box_set_active(stargazer_synth->synth_1_lfo_oscillator,
			   0);

  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) stargazer_synth->synth_1_lfo_oscillator,
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

  stargazer_synth->synth_1_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 16.0, 0.01);

  gtk_spin_button_set_value(stargazer_synth->synth_1_lfo_frequency,
			    AGS_FM_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
    
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) stargazer_synth->synth_1_lfo_frequency,
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

  stargazer_synth->synth_1_lfo_tuning = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_1_lfo_tuning);

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

  ags_dial_set_radius(stargazer_synth->synth_1_lfo_tuning,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) stargazer_synth->synth_1_lfo_tuning,
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

  stargazer_synth->synth_1_lfo_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_1_lfo_depth);

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

  ags_dial_set_radius(stargazer_synth->synth_1_lfo_depth,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) stargazer_synth->synth_1_lfo_depth,
		  5, 2,
		  1, 1);

  /* OSC 2 - sync enabled */
  stargazer_synth->synth_1_sync_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("OSC 2 - sync enabled"));
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) stargazer_synth->synth_1_sync_enabled,
		  6, 0,
		  1, 1);

  /* OSC 2 - sync relative attack factor */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync factor"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  6, 1,
		  1, 1);
  
  stargazer_synth->synth_1_sync_relative_attack_factor = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_1_sync_relative_attack_factor);

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

  ags_dial_set_radius(stargazer_synth->synth_1_sync_relative_attack_factor,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) stargazer_synth->synth_1_sync_relative_attack_factor,
		  7, 1,
		  1, 1);

  /* OSC 2 - sync attack 0 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync attack 0"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  10, 0,
		  1, 1);
  
  stargazer_synth->synth_1_sync_attack_0 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_1_sync_attack_0);

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

  ags_dial_set_radius(stargazer_synth->synth_1_sync_attack_0,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) stargazer_synth->synth_1_sync_attack_0,
		  11, 0,
		  1, 1);

  /* OSC 2 - sync phase 0 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync phase 0"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  12, 0,
		  1, 1);
  
  stargazer_synth->synth_1_sync_phase_0 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_1_sync_phase_0);

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

  ags_dial_set_radius(stargazer_synth->synth_1_sync_phase_0,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) stargazer_synth->synth_1_sync_phase_0,
		  13, 0,
		  1, 1);

  /* OSC 2 - sync attack 1 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync attack 1"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  10, 1,
		  1, 1);
  
  stargazer_synth->synth_1_sync_attack_1 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_1_sync_attack_1);

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

  ags_dial_set_radius(stargazer_synth->synth_1_sync_attack_1,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) stargazer_synth->synth_1_sync_attack_1,
		  11, 1,
		  1, 1);

  /* OSC 2 - sync phase 1 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync phase 1"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  12, 1,
		  1, 1);
  
  stargazer_synth->synth_1_sync_phase_1 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_1_sync_phase_1);

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

  ags_dial_set_radius(stargazer_synth->synth_1_sync_phase_1,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) stargazer_synth->synth_1_sync_phase_1,
		  13, 1,
		  1, 1);

  /* OSC 2 - sync attack 2 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync attack 2"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  10, 2,
		  1, 1);
  
  stargazer_synth->synth_1_sync_attack_2 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_1_sync_attack_2);

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

  ags_dial_set_radius(stargazer_synth->synth_1_sync_attack_2,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) stargazer_synth->synth_1_sync_attack_2,
		  11, 2,
		  1, 1);

  /* OSC 2 - sync phase 2 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync phase 2"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  12, 2,
		  1, 1);
  
  stargazer_synth->synth_1_sync_phase_2 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_1_sync_phase_2);

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

  ags_dial_set_radius(stargazer_synth->synth_1_sync_phase_2,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) stargazer_synth->synth_1_sync_phase_2,
		  13, 2,
		  1, 1);

  /* OSC 2 - sync attack 3 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync attack 3"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  10, 3,
		  1, 1);
  
  stargazer_synth->synth_1_sync_attack_3 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_1_sync_attack_3);

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

  ags_dial_set_radius(stargazer_synth->synth_1_sync_attack_3,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) stargazer_synth->synth_1_sync_attack_3,
		  11, 3,
		  1, 1);

  /* OSC 2 - sync phase 3 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync phase 3"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  12, 3,
		  1, 1);
  
  stargazer_synth->synth_1_sync_phase_3 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->synth_1_sync_phase_3);

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

  ags_dial_set_radius(stargazer_synth->synth_1_sync_phase_3,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) stargazer_synth->synth_1_sync_phase_3,
		  13, 3,
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

  stargazer_synth->pitch_type = (GtkDropDown *) gtk_drop_down_new_from_strings(pitch_type_strv);

  gtk_drop_down_set_selected(stargazer_synth->pitch_type,
			     2);

  gtk_box_append(pitch_type_hbox,
		 (GtkWidget *) stargazer_synth->pitch_type);
  
  /* pitch */
  label = (GtkLabel *) gtk_label_new(i18n("pitch tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(misc_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  stargazer_synth->pitch_tuning = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->pitch_tuning);

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

  ags_dial_set_radius(stargazer_synth->pitch_tuning,
		      12);
  
  gtk_grid_attach(misc_grid,
		  (GtkWidget *) stargazer_synth->pitch_tuning,
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

  stargazer_synth->noise_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->noise_gain);

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

  ags_dial_set_radius(stargazer_synth->noise_gain,
		      12);
  
  gtk_grid_attach(misc_grid,
		  (GtkWidget *) stargazer_synth->noise_gain,
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

  stargazer_synth->chorus_input_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->chorus_input_volume);

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

  ags_dial_set_radius(stargazer_synth->chorus_input_volume,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) stargazer_synth->chorus_input_volume,
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

  stargazer_synth->chorus_output_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->chorus_output_volume);

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

  ags_dial_set_radius(stargazer_synth->chorus_output_volume,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) stargazer_synth->chorus_output_volume,
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
  
  stargazer_synth->chorus_lfo_oscillator = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->chorus_lfo_oscillator,
				 "sine");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->chorus_lfo_oscillator,
				 "sawtooth");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->chorus_lfo_oscillator,
				 "triangle");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->chorus_lfo_oscillator,
				 "square");
  gtk_combo_box_text_append_text((GtkComboBoxText *) stargazer_synth->chorus_lfo_oscillator,
				 "impulse");

  gtk_combo_box_set_active(stargazer_synth->chorus_lfo_oscillator,
			   0);

  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) stargazer_synth->chorus_lfo_oscillator,
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

  stargazer_synth->chorus_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 10.0, 0.01);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) stargazer_synth->chorus_lfo_frequency,
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

  stargazer_synth->chorus_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->chorus_depth);

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

  ags_dial_set_radius(stargazer_synth->chorus_depth,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) stargazer_synth->chorus_depth,
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

  stargazer_synth->chorus_mix = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->chorus_mix);

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

  ags_dial_set_radius(stargazer_synth->chorus_mix,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) stargazer_synth->chorus_mix,
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

  stargazer_synth->chorus_delay = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(stargazer_synth->chorus_delay);

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

  ags_dial_set_radius(stargazer_synth->chorus_delay,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) stargazer_synth->chorus_delay,
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

  stargazer_synth->tremolo_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) stargazer_synth->tremolo_enabled,
		  0, 0,
		  2, 1);

  label = (GtkLabel *) gtk_label_new(i18n("gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  stargazer_synth->tremolo_gain = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(stargazer_synth->tremolo_gain,
		      12);

  adjustment = ags_dial_get_adjustment(stargazer_synth->tremolo_gain);

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
		  (GtkWidget *) stargazer_synth->tremolo_gain,
		  1, 1,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("lfo-depth"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);

  stargazer_synth->tremolo_lfo_depth = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(stargazer_synth->tremolo_lfo_depth,
		      12);

  adjustment = ags_dial_get_adjustment(stargazer_synth->tremolo_lfo_depth);

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
		  (GtkWidget *) stargazer_synth->tremolo_lfo_depth,
		  1, 2,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("lfo-freq"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);

  stargazer_synth->tremolo_lfo_freq = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(stargazer_synth->tremolo_lfo_freq,
		      12);

  adjustment = ags_dial_get_adjustment(stargazer_synth->tremolo_lfo_freq);

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
		  (GtkWidget *) stargazer_synth->tremolo_lfo_freq,
		  1, 3,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);

  stargazer_synth->tremolo_tuning = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(stargazer_synth->tremolo_tuning,
		      12);

  adjustment = ags_dial_get_adjustment(stargazer_synth->tremolo_tuning);

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
		  (GtkWidget *) stargazer_synth->tremolo_tuning,
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

  stargazer_synth->vibrato_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) stargazer_synth->vibrato_enabled,
		  0, 0,
		  2, 1);

  label = (GtkLabel *) gtk_label_new(i18n("gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  stargazer_synth->vibrato_gain = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(stargazer_synth->vibrato_gain,
		      12);

  adjustment = ags_dial_get_adjustment(stargazer_synth->vibrato_gain);

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
		  (GtkWidget *) stargazer_synth->vibrato_gain,
		  1, 1,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("lfo-depth"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);

  stargazer_synth->vibrato_lfo_depth = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(stargazer_synth->vibrato_lfo_depth,
		      12);

  adjustment = ags_dial_get_adjustment(stargazer_synth->vibrato_lfo_depth);

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
		  (GtkWidget *) stargazer_synth->vibrato_lfo_depth,
		  1, 2,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("lfo-freq"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);

  stargazer_synth->vibrato_lfo_freq = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(stargazer_synth->vibrato_lfo_freq,
		      12);
  
  adjustment = ags_dial_get_adjustment(stargazer_synth->vibrato_lfo_freq);

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
		  (GtkWidget *) stargazer_synth->vibrato_lfo_freq,
		  1, 3,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);

  stargazer_synth->vibrato_tuning = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(stargazer_synth->vibrato_tuning,
		      12);

  adjustment = ags_dial_get_adjustment(stargazer_synth->vibrato_tuning);

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
		  (GtkWidget *) stargazer_synth->vibrato_tuning,
		  1, 4,
		  1, 1);
}

void
ags_stargazer_synth_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_stargazer_synth_parent_class)->finalize(gobject);
}

void
ags_stargazer_synth_connect(AgsConnectable *connectable)
{
  AgsStargazerSynth *stargazer_synth;
  
  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) != 0){
    return;
  }

  ags_stargazer_synth_parent_connectable_interface->connect(connectable);
  
  /* AgsStargazerSynth */
  stargazer_synth = AGS_STARGAZER_SYNTH(connectable);

  g_signal_connect_after(stargazer_synth->synth_0_oscillator, "changed",
			 G_CALLBACK(ags_stargazer_synth_synth_0_oscillator_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_0_octave, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_0_octave_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_0_key, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_0_key_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_0_phase, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_0_phase_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_0_volume, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_0_volume_callback), stargazer_synth);
    
  g_signal_connect_after(stargazer_synth->synth_0_sync_enabled, "toggled",
			 G_CALLBACK(ags_stargazer_synth_synth_0_sync_enabled_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_0_sync_relative_attack_factor, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_0_sync_relative_attack_factor_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_0_sync_attack_0, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_0_sync_attack_0_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_0_sync_phase_0, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_0_sync_phase_0_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_0_sync_attack_1, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_0_sync_attack_1_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_0_sync_phase_1, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_0_sync_phase_1_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_0_sync_attack_2, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_0_sync_attack_2_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_0_sync_phase_2, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_0_sync_phase_2_callback), stargazer_synth);

  g_signal_connect_after(stargazer_synth->synth_0_sync_attack_3, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_0_sync_attack_3_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_0_sync_phase_3, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_0_sync_phase_3_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_0_sync_lfo_oscillator, "changed",
			 G_CALLBACK(ags_stargazer_synth_synth_0_sync_lfo_oscillator_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_0_sync_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_0_sync_lfo_frequency_callback), stargazer_synth);

  g_signal_connect_after(stargazer_synth->synth_0_lfo_oscillator, "changed",
			 G_CALLBACK(ags_stargazer_synth_synth_0_lfo_oscillator_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_0_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_0_lfo_frequency_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_0_lfo_depth, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_0_lfo_depth_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_0_lfo_tuning, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_0_lfo_tuning_callback), stargazer_synth);

  g_signal_connect_after(stargazer_synth->synth_1_oscillator, "changed",
			 G_CALLBACK(ags_stargazer_synth_synth_1_oscillator_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_1_octave, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_1_octave_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_1_key, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_1_key_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_1_phase, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_1_phase_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_1_volume, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_1_volume_callback), stargazer_synth);
    
  g_signal_connect_after(stargazer_synth->synth_1_sync_enabled, "toggled",
			 G_CALLBACK(ags_stargazer_synth_synth_1_sync_enabled_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_1_sync_relative_attack_factor, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_1_sync_relative_attack_factor_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_1_sync_attack_0, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_1_sync_attack_0_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_1_sync_phase_0, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_1_sync_phase_0_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_1_sync_attack_1, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_1_sync_attack_1_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_1_sync_phase_1, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_1_sync_phase_1_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_1_sync_attack_2, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_1_sync_attack_2_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_1_sync_phase_2, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_1_sync_phase_2_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_1_sync_attack_3, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_1_sync_attack_3_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_1_sync_phase_3, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_1_sync_phase_3_callback), stargazer_synth);

  g_signal_connect_after(stargazer_synth->synth_1_sync_lfo_oscillator, "changed",
			 G_CALLBACK(ags_stargazer_synth_synth_1_sync_lfo_oscillator_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_1_sync_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_1_sync_lfo_frequency_callback), stargazer_synth);

  g_signal_connect_after(stargazer_synth->synth_1_lfo_oscillator, "changed",
			 G_CALLBACK(ags_stargazer_synth_synth_1_lfo_oscillator_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_1_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_1_lfo_frequency_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_1_lfo_depth, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_1_lfo_depth_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->synth_1_lfo_tuning, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_synth_1_lfo_tuning_callback), stargazer_synth);

  g_signal_connect((GObject *) stargazer_synth->pitch_type, "notify::selected",
		   G_CALLBACK(ags_stargazer_synth_pitch_type_callback), (gpointer) stargazer_synth);

  g_signal_connect_after(stargazer_synth->pitch_tuning, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_pitch_tuning_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->noise_gain, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_noise_gain_callback), stargazer_synth);
    
  //  g_signal_connect_after(stargazer_synth->chorus_enabled, "toggled",
  //			 G_CALLBACK(ags_stargazer_synth_chorus_enabled_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->chorus_input_volume, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_chorus_input_volume_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->chorus_output_volume, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_chorus_output_volume_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->chorus_lfo_oscillator, "changed",
			 G_CALLBACK(ags_stargazer_synth_chorus_lfo_oscillator_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->chorus_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_chorus_lfo_frequency_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->chorus_depth, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_chorus_depth_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->chorus_mix, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_chorus_mix_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->chorus_delay, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_chorus_delay_callback), stargazer_synth);

  g_signal_connect_after(stargazer_synth->chorus_input_volume, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_chorus_input_volume_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->chorus_output_volume, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_chorus_output_volume_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->chorus_lfo_oscillator, "changed",
			 G_CALLBACK(ags_stargazer_synth_chorus_lfo_oscillator_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->chorus_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_chorus_lfo_frequency_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->chorus_depth, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_chorus_depth_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->chorus_mix, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_chorus_mix_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->chorus_delay, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_chorus_delay_callback), stargazer_synth);

  g_signal_connect_after(stargazer_synth->tremolo_enabled, "toggled",
			 G_CALLBACK(ags_stargazer_synth_tremolo_enabled_callback), stargazer_synth);

  g_signal_connect_after(stargazer_synth->tremolo_gain, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_tremolo_gain_callback), stargazer_synth);
  
  g_signal_connect_after(stargazer_synth->tremolo_lfo_depth, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_tremolo_lfo_depth_callback), stargazer_synth);

  g_signal_connect_after(stargazer_synth->tremolo_lfo_freq, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_tremolo_lfo_freq_callback), stargazer_synth);

  g_signal_connect_after(stargazer_synth->tremolo_tuning, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_tremolo_tuning_callback), stargazer_synth);

  g_signal_connect_after(stargazer_synth->vibrato_enabled, "toggled",
			 G_CALLBACK(ags_stargazer_synth_vibrato_enabled_callback), stargazer_synth);

  g_signal_connect_after(stargazer_synth->vibrato_gain, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_vibrato_gain_callback), stargazer_synth);

  g_signal_connect_after(stargazer_synth->vibrato_lfo_depth, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_vibrato_lfo_depth_callback), stargazer_synth);

  g_signal_connect_after(stargazer_synth->vibrato_lfo_freq, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_vibrato_lfo_freq_callback), stargazer_synth);

  g_signal_connect_after(stargazer_synth->vibrato_tuning, "value-changed",
			 G_CALLBACK(ags_stargazer_synth_vibrato_tuning_callback), stargazer_synth);
}

void
ags_stargazer_synth_disconnect(AgsConnectable *connectable)
{
  AgsStargazerSynth *stargazer_synth;
  
  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) == 0){
    return;
  }

  ags_stargazer_synth_parent_connectable_interface->disconnect(connectable);

  /* AgsStargazerSynth */
  stargazer_synth = AGS_STARGAZER_SYNTH(connectable);

  g_object_disconnect(stargazer_synth->synth_0_oscillator,
		      "any_signal::changed",
		      G_CALLBACK(ags_stargazer_synth_synth_0_oscillator_callback),
		      stargazer_synth,
		      NULL);
  
  g_object_disconnect(stargazer_synth->synth_0_octave,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_synth_0_octave_callback),
		      stargazer_synth,
		      NULL);
  
  g_object_disconnect(stargazer_synth->synth_0_key,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_synth_0_key_callback),
		      stargazer_synth,
		      NULL);
  
  g_object_disconnect(stargazer_synth->synth_0_phase,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_synth_0_phase_callback),
		      stargazer_synth,
		      NULL);
  
  g_object_disconnect(stargazer_synth->synth_0_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_synth_0_volume_callback),
		      stargazer_synth,
		      NULL);
    
  g_object_disconnect(stargazer_synth->synth_0_lfo_oscillator,
		      "any_signal::changed",
		      G_CALLBACK(ags_stargazer_synth_synth_0_lfo_oscillator_callback),
		      stargazer_synth,
		      NULL);
  
  g_object_disconnect(stargazer_synth->synth_0_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_synth_0_lfo_frequency_callback),
		      stargazer_synth,
		      NULL);
  
  g_object_disconnect(stargazer_synth->synth_0_lfo_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_synth_0_lfo_depth_callback),
		      stargazer_synth,
		      NULL);
  
  g_object_disconnect(stargazer_synth->synth_0_lfo_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_synth_0_lfo_tuning_callback),
		      stargazer_synth,
		      NULL);

  g_object_disconnect(stargazer_synth->synth_1_oscillator,
		      "any_signal::changed",
		      G_CALLBACK(ags_stargazer_synth_synth_1_oscillator_callback),
		      stargazer_synth,
		      NULL);
  
  g_object_disconnect(stargazer_synth->synth_1_octave,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_synth_1_octave_callback),
		      stargazer_synth,
		      NULL);
  
  g_object_disconnect(stargazer_synth->synth_1_key,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_synth_1_key_callback),
		      stargazer_synth,
		      NULL);
  
  g_object_disconnect(stargazer_synth->synth_1_phase,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_synth_1_phase_callback),
		      stargazer_synth,
		      NULL);
  
  g_object_disconnect(stargazer_synth->synth_1_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_synth_1_volume_callback),
		      stargazer_synth,
		      NULL);
    
  g_object_disconnect(stargazer_synth->synth_1_lfo_oscillator,
		      "any_signal::changed",
		      G_CALLBACK(ags_stargazer_synth_synth_1_lfo_oscillator_callback),
		      stargazer_synth,
		      NULL);
  
  g_object_disconnect(stargazer_synth->synth_1_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_synth_1_lfo_frequency_callback),
		      stargazer_synth,
		      NULL);
  
  g_object_disconnect(stargazer_synth->synth_1_lfo_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_synth_1_lfo_depth_callback),
		      stargazer_synth,
		      NULL);
  
  g_object_disconnect(stargazer_synth->synth_1_lfo_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_synth_1_lfo_tuning_callback),
		      stargazer_synth,
		      NULL);

  g_object_disconnect((GObject *) stargazer_synth->pitch_type,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_stargazer_synth_pitch_type_callback),
		      (gpointer) stargazer_synth,
		      NULL);
  
  g_object_disconnect(stargazer_synth->pitch_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_pitch_tuning_callback),
		      stargazer_synth,
		      NULL);
  
  g_object_disconnect(stargazer_synth->noise_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_noise_gain_callback),
		      stargazer_synth,
		      NULL);
    
  //  g_object_disconnect(stargazer_synth->chorus_enabled,
  //		      "any_signal::toggled",
  //			 G_CALLBACK(ags_stargazer_synth_chorus_enabled_callback),
  //		      stargazer_synth,
  //		      NULL);
  
  g_object_disconnect(stargazer_synth->chorus_input_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_chorus_input_volume_callback),
		      stargazer_synth,
		      NULL);
  
  g_object_disconnect(stargazer_synth->chorus_output_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_chorus_output_volume_callback),
		      stargazer_synth,
		      NULL);
  
  g_object_disconnect(stargazer_synth->chorus_lfo_oscillator,
		      "any_signal::changed",
		      G_CALLBACK(ags_stargazer_synth_chorus_lfo_oscillator_callback),
		      stargazer_synth,
		      NULL);
  
  g_object_disconnect(stargazer_synth->chorus_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_chorus_lfo_frequency_callback),
		      stargazer_synth,
		      NULL);
  
  g_object_disconnect(stargazer_synth->chorus_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_chorus_depth_callback),
		      stargazer_synth,
		      NULL);
  
  g_object_disconnect(stargazer_synth->chorus_mix,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_chorus_mix_callback),
		      stargazer_synth,
		      NULL);
  
  g_object_disconnect(stargazer_synth->chorus_delay,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_chorus_delay_callback),
		      stargazer_synth,
		      NULL);

  g_object_disconnect((GObject *) stargazer_synth->tremolo_enabled,
		      "any_signal::toggled",
		      G_CALLBACK(ags_stargazer_synth_tremolo_enabled_callback),
		      stargazer_synth,
		      NULL);

  g_object_disconnect((GObject *) stargazer_synth->tremolo_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_tremolo_gain_callback),
		      (gpointer) stargazer_synth,
		      NULL);

  g_object_disconnect((GObject *) stargazer_synth->tremolo_lfo_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_tremolo_lfo_depth_callback),
		      (gpointer) stargazer_synth,
		      NULL);

  g_object_disconnect((GObject *) stargazer_synth->tremolo_lfo_freq,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_tremolo_lfo_freq_callback),
		      (gpointer) stargazer_synth,
		      NULL);

  g_object_disconnect((GObject *) stargazer_synth->tremolo_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_tremolo_tuning_callback),
		      (gpointer) stargazer_synth,
		      NULL);

  g_object_disconnect((GObject *) stargazer_synth->vibrato_enabled,
		      "any_signal::toggled",
		      G_CALLBACK(ags_stargazer_synth_vibrato_enabled_callback),
		      stargazer_synth,
		      NULL);

  g_object_disconnect((GObject *) stargazer_synth->vibrato_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_vibrato_gain_callback),
		      (gpointer) stargazer_synth,
		      NULL);

  g_object_disconnect((GObject *) stargazer_synth->vibrato_lfo_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_vibrato_lfo_depth_callback),
		      (gpointer) stargazer_synth,
		      NULL);

  g_object_disconnect((GObject *) stargazer_synth->vibrato_lfo_freq,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_stargazer_synth_vibrato_lfo_freq_callback),
		      (gpointer) stargazer_synth,
		      NULL);
}

void
ags_stargazer_synth_resize_audio_channels(AgsMachine *machine,
					  guint audio_channels, guint audio_channels_old,
					  gpointer data)
{
  AgsStargazerSynth *stargazer_synth;

  stargazer_synth = (AgsStargazerSynth *) machine;

  if(audio_channels > audio_channels_old){    
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_stargazer_synth_input_map_recall(stargazer_synth,
					   audio_channels_old,
					   0);
      
      ags_stargazer_synth_output_map_recall(stargazer_synth,
					    audio_channels_old,
					    0);
    }
  }
}

void
ags_stargazer_synth_resize_pads(AgsMachine *machine, GType type,
				guint pads, guint pads_old,
				gpointer data)
{
  AgsStargazerSynth *stargazer_synth;

  gboolean grow;

  stargazer_synth = (AgsStargazerSynth *) machine;
  
  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(type == AGS_TYPE_INPUT){
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	/* depending on destination */
	ags_stargazer_synth_input_map_recall(stargazer_synth,
					     0,
					     pads_old);
      }
    }else{
      stargazer_synth->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_stargazer_synth_output_map_recall(stargazer_synth,
					      0,
					      pads_old);
      }
    }else{
      stargazer_synth->mapped_output_pad = pads;
    }
  }
}

void
ags_stargazer_synth_map_recall(AgsMachine *machine)
{
  AgsStargazerSynth *stargazer_synth;
  
  AgsAudio *audio;

  GList *start_recall, *recall;

  gint position;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  stargazer_synth = AGS_STARGAZER_SYNTH(machine);

  audio = machine->audio;
  
  position = 0;

  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       stargazer_synth->playback_play_container, stargazer_synth->playback_recall_container,
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
				       stargazer_synth->star_synth_play_container, stargazer_synth->star_synth_recall_container,
				       "ags-fx-star-synth",
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
				       stargazer_synth->tremolo_play_container, stargazer_synth->tremolo_recall_container,
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
				       stargazer_synth->volume_play_container, stargazer_synth->volume_recall_container,
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
				       stargazer_synth->envelope_play_container, stargazer_synth->envelope_recall_container,
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
				       stargazer_synth->buffer_play_container, stargazer_synth->buffer_recall_container,
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
  ags_stargazer_synth_input_map_recall(stargazer_synth,
				       0,
				       0);

  /* depending on destination */
  ags_stargazer_synth_output_map_recall(stargazer_synth,
					0,
					0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_stargazer_synth_parent_class)->map_recall(machine);  
}

void
ags_stargazer_synth_input_map_recall(AgsStargazerSynth *stargazer_synth,
				     guint audio_channel_start,
				     guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(stargazer_synth->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(stargazer_synth)->audio;

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
				       stargazer_synth->playback_play_container, stargazer_synth->playback_recall_container,
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
				       stargazer_synth->star_synth_play_container, stargazer_synth->star_synth_recall_container,
				       "ags-fx-star-synth",
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
				       stargazer_synth->volume_play_container, stargazer_synth->volume_recall_container,
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
				       stargazer_synth->envelope_play_container, stargazer_synth->envelope_recall_container,
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
				       stargazer_synth->buffer_play_container, stargazer_synth->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  stargazer_synth->mapped_input_pad = input_pads;
}

void
ags_stargazer_synth_output_map_recall(AgsStargazerSynth *stargazer_synth,
				      guint audio_channel_start,
				      guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;

  if(stargazer_synth->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(stargazer_synth)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);
  
  stargazer_synth->mapped_output_pad = output_pads;
}

void
ags_stargazer_synth_refresh_port(AgsMachine *machine)
{
  AgsStargazerSynth *stargazer_synth;
  
  GList *start_play, *start_recall, *recall;

  stargazer_synth = (AgsStargazerSynth *) machine;
  
  start_play = ags_audio_get_play(machine->audio);
  start_recall = ags_audio_get_recall(machine->audio);

  recall =
    start_recall = g_list_concat(start_play, start_recall);

  machine->flags |= AGS_MACHINE_NO_UPDATE;

  if((recall = ags_recall_find_type(recall, AGS_TYPE_FX_STAR_SYNTH_AUDIO)) != NULL){
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

      gtk_combo_box_set_active(stargazer_synth->synth_0_oscillator,
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

      ags_dial_set_value(stargazer_synth->synth_0_octave,
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

      ags_dial_set_value(stargazer_synth->synth_0_key,
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

      ags_dial_set_value(stargazer_synth->synth_0_phase,
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

      ags_dial_set_value(stargazer_synth->synth_0_volume,
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

      gtk_combo_box_set_active(stargazer_synth->synth_0_lfo_oscillator,
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

      gtk_spin_button_set_value(stargazer_synth->synth_0_lfo_frequency,
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

      ags_dial_set_value(stargazer_synth->synth_0_lfo_depth,
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

      ags_dial_set_value(stargazer_synth->synth_0_lfo_tuning,
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
	gtk_check_button_set_active(stargazer_synth->synth_0_sync_enabled,
				    TRUE);
      }else{
	gtk_check_button_set_active(stargazer_synth->synth_0_sync_enabled,
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

      ags_dial_set_value(stargazer_synth->synth_0_sync_relative_attack_factor,
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

      ags_dial_set_value(stargazer_synth->synth_0_sync_attack_0,
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

      ags_dial_set_value(stargazer_synth->synth_0_sync_phase_0,
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

      ags_dial_set_value(stargazer_synth->synth_0_sync_attack_1,
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

      ags_dial_set_value(stargazer_synth->synth_0_sync_phase_1,
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

      ags_dial_set_value(stargazer_synth->synth_0_sync_attack_2,
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

      ags_dial_set_value(stargazer_synth->synth_0_sync_phase_2,
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

      gtk_combo_box_set_active(stargazer_synth->synth_0_sync_lfo_oscillator,
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

      gtk_spin_button_set_value(stargazer_synth->synth_0_sync_lfo_frequency,
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

      gtk_combo_box_set_active(stargazer_synth->synth_1_oscillator,
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

      ags_dial_set_value(stargazer_synth->synth_1_octave,
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

      ags_dial_set_value(stargazer_synth->synth_1_key,
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

      ags_dial_set_value(stargazer_synth->synth_1_phase,
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

      ags_dial_set_value(stargazer_synth->synth_1_volume,
			 (gdouble) g_value_get_float(&value));

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

      gtk_combo_box_set_active(stargazer_synth->synth_1_lfo_oscillator,
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

      gtk_spin_button_set_value(stargazer_synth->synth_1_lfo_frequency,
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

      ags_dial_set_value(stargazer_synth->synth_1_lfo_depth,
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

      ags_dial_set_value(stargazer_synth->synth_1_lfo_tuning,
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
	gtk_check_button_set_active(stargazer_synth->synth_1_sync_enabled,
				    TRUE);
      }else{
	gtk_check_button_set_active(stargazer_synth->synth_1_sync_enabled,
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

      ags_dial_set_value(stargazer_synth->synth_1_sync_relative_attack_factor,
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

      ags_dial_set_value(stargazer_synth->synth_1_sync_attack_0,
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

      ags_dial_set_value(stargazer_synth->synth_1_sync_phase_0,
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

      ags_dial_set_value(stargazer_synth->synth_1_sync_attack_1,
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

      ags_dial_set_value(stargazer_synth->synth_1_sync_phase_1,
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

      ags_dial_set_value(stargazer_synth->synth_1_sync_attack_2,
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

      ags_dial_set_value(stargazer_synth->synth_1_sync_phase_2,
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

      gtk_combo_box_set_active(stargazer_synth->synth_1_sync_lfo_oscillator,
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

      gtk_spin_button_set_value(stargazer_synth->synth_1_sync_lfo_frequency,
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

      gtk_drop_down_set_selected(stargazer_synth->pitch_type,
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

      ags_dial_set_value(stargazer_synth->pitch_tuning,
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

      ags_dial_set_value(stargazer_synth->noise_gain,
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
	gtk_check_button_set_active(stargazer_synth->chorus_enabled,
				    TRUE);
      }else{
	gtk_check_button_set_active(stargazer_synth->chorus_enabled,
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

      gtk_combo_box_set_active(stargazer_synth->chorus_lfo_oscillator,
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

      gtk_spin_button_set_value(stargazer_synth->chorus_lfo_frequency,
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

      ags_dial_set_value(stargazer_synth->chorus_depth,
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

      ags_dial_set_value(stargazer_synth->chorus_mix,
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

      ags_dial_set_value(stargazer_synth->chorus_delay,
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

      ags_dial_set_value(stargazer_synth->vibrato_gain,
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

      ags_dial_set_value(stargazer_synth->vibrato_lfo_depth,
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

      ags_dial_set_value(stargazer_synth->vibrato_lfo_freq,
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

      ags_dial_set_value(stargazer_synth->vibrato_tuning,
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

      ags_dial_set_value(stargazer_synth->tremolo_gain,
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

      ags_dial_set_value(stargazer_synth->tremolo_lfo_depth,
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

      ags_dial_set_value(stargazer_synth->tremolo_lfo_freq,
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

      ags_dial_set_value(stargazer_synth->tremolo_tuning,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }
  }
  
  machine->flags &= (~AGS_MACHINE_NO_UPDATE);
}

/**
 * ags_stargazer_synth_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsStargazerSynth
 *
 * Returns: the new #AgsStargazerSynth
 *
 * Since: 3.14.0
 */
AgsStargazerSynth*
ags_stargazer_synth_new(GObject *soundcard)
{
  AgsStargazerSynth *stargazer_synth;

  stargazer_synth = (AgsStargazerSynth *) g_object_new(AGS_TYPE_STARGAZER_SYNTH,
						       NULL);

  g_object_set(AGS_MACHINE(stargazer_synth)->audio,
	       "output-soundcard", soundcard,
	       NULL);

  return(stargazer_synth);
}
