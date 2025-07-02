/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/app/machine/ags_raven_synth.h>
#include <ags/app/machine/ags_raven_synth_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_line.h>

#include <math.h>

#include <ags/ags_api_config.h>

#include <ags/i18n.h>

void ags_raven_synth_class_init(AgsRavenSynthClass *raven_synth);
void ags_raven_synth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_raven_synth_init(AgsRavenSynth *raven_synth);
void ags_raven_synth_finalize(GObject *gobject);

void ags_raven_synth_connect(AgsConnectable *connectable);
void ags_raven_synth_disconnect(AgsConnectable *connectable);

void ags_raven_synth_show(GtkWidget *widget);

void ags_raven_synth_resize_audio_channels(AgsMachine *machine,
					   guint audio_channels, guint audio_channels_old,
					   gpointer data);
void ags_raven_synth_resize_pads(AgsMachine *machine, GType channel_type,
				 guint pads, guint pads_old,
				 gpointer data);

void ags_raven_synth_map_recall(AgsMachine *machine);

void ags_raven_synth_input_map_recall(AgsRavenSynth *raven_synth,
				      guint audio_channel_start,
				      guint input_pad_start);
void ags_raven_synth_output_map_recall(AgsRavenSynth *raven_synth,
				       guint audio_channel_start,
				       guint output_pad_start);

void ags_raven_synth_refresh_port(AgsMachine *machine);

/**
 * SECTION:ags_raven_synth
 * @short_description: raven synth
 * @title: AgsRavenSynth
 * @section_id:
 * @include: ags/app/machine/ags_raven_synth.h
 *
 * The #AgsRavenSynth is a composite widget to act as raven synth.
 */

static gpointer ags_raven_synth_parent_class = NULL;
static AgsConnectableInterface *ags_raven_synth_parent_connectable_interface;

GType
ags_raven_synth_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_raven_synth = 0;

    static const GTypeInfo ags_raven_synth_info = {
      sizeof(AgsRavenSynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_raven_synth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsRavenSynth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_raven_synth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_raven_synth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_raven_synth = g_type_register_static(AGS_TYPE_MACHINE,
						  "AgsRavenSynth", &ags_raven_synth_info,
						  0);
    
    g_type_add_interface_static(ags_type_raven_synth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_raven_synth);
  }

  return(g_define_type_id__static);
}

void
ags_raven_synth_class_init(AgsRavenSynthClass *raven_synth)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_raven_synth_parent_class = g_type_class_peek_parent(raven_synth);

  /* GObjectClass */
  gobject = (GObjectClass *) raven_synth;

  gobject->finalize = ags_raven_synth_finalize;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) raven_synth;

  machine->map_recall = ags_raven_synth_map_recall;

  machine->refresh_port = ags_raven_synth_refresh_port;
}

void
ags_raven_synth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_raven_synth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_raven_synth_connect;
  connectable->disconnect = ags_raven_synth_disconnect;
}

void
ags_raven_synth_init(AgsRavenSynth *raven_synth)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  GtkBox *vbox;
  GtkBox *hbox;
  GtkBox *osc_vbox;
  GtkGrid *synth_0_grid;
  GtkGrid *synth_0_seq_grid;
  GtkGrid *synth_0_effect_grid;
  GtkGrid *synth_1_grid;
  GtkGrid *synth_1_seq_grid;
  GtkGrid *synth_1_effect_grid;
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
								     AGS_TYPE_RAVEN_SYNTH);

  machine_name = NULL;

  if(machine_counter != NULL){
    machine_name = g_strdup_printf("Default %d",
				   machine_counter->counter);
  
    ags_machine_counter_increment(machine_counter);
  }
  
  g_object_set(raven_synth,
	       "machine-name", machine_name,
	       NULL);

  g_free(machine_name);

  /* machine selector */
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  position = g_list_length(window->machine);
  
  ags_machine_selector_popup_insert_machine(composite_editor->machine_selector,
					    position,
					    (AgsMachine *) raven_synth);

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
  
  audio = AGS_MACHINE(raven_synth)->audio;
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

  AGS_MACHINE(raven_synth)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
				      AGS_MACHINE_REVERSE_NOTATION);
  AGS_MACHINE(raven_synth)->mapping_flags |= AGS_MACHINE_MONO;

  AGS_MACHINE(raven_synth)->input_pad_type = G_TYPE_NONE;
  AGS_MACHINE(raven_synth)->input_line_type = G_TYPE_NONE;
  AGS_MACHINE(raven_synth)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(raven_synth)->output_line_type = G_TYPE_NONE;

  /* audio resize */
  g_signal_connect_after(G_OBJECT(raven_synth), "resize-audio-channels",
			 G_CALLBACK(ags_raven_synth_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(raven_synth), "resize-pads",
			 G_CALLBACK(ags_raven_synth_resize_pads), NULL);
  
  /* create widgets */
  raven_synth->flags = 0;

  /* mapped IO */
  raven_synth->mapped_input_pad = 0;
  raven_synth->mapped_output_pad = 0;

  raven_synth->playback_play_container = ags_recall_container_new();
  raven_synth->playback_recall_container = ags_recall_container_new();

  raven_synth->raven_synth_play_container = ags_recall_container_new();
  raven_synth->raven_synth_recall_container = ags_recall_container_new();

  raven_synth->volume_play_container = ags_recall_container_new();
  raven_synth->volume_recall_container = ags_recall_container_new();

  raven_synth->tremolo_play_container = ags_recall_container_new();
  raven_synth->tremolo_recall_container = ags_recall_container_new();

  raven_synth->envelope_play_container = ags_recall_container_new();
  raven_synth->envelope_recall_container = ags_recall_container_new();

  raven_synth->buffer_play_container = ags_recall_container_new();
  raven_synth->buffer_recall_container = ags_recall_container_new();
   
  /* name and xml type */
  raven_synth->name = NULL;
  raven_synth->xml_type = "ags-raven-synth"; 

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

  gtk_frame_set_child(AGS_MACHINE(raven_synth)->frame,
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
  
  raven_synth->synth_0_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(raven_synth->synth_0_oscillator,
			     0);

  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) raven_synth->synth_0_oscillator,
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

  raven_synth->synth_0_octave = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_octave);

  gtk_adjustment_set_lower(adjustment,
			   -6.0);
  gtk_adjustment_set_upper(adjustment,
			   6.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(raven_synth->synth_0_octave,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) raven_synth->synth_0_octave,
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

  raven_synth->synth_0_key = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_key);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   2.0);

  ags_dial_set_radius(raven_synth->synth_0_key,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) raven_synth->synth_0_key,
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

  raven_synth->synth_0_phase = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_phase);

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

  ags_dial_set_radius(raven_synth->synth_0_phase,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) raven_synth->synth_0_phase,
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

  raven_synth->synth_0_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_volume);

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

  ags_dial_set_radius(raven_synth->synth_0_volume,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) raven_synth->synth_0_volume,
		  5, 1,
		  1, 1);

  /* OSC 1 - sync enabled */
  raven_synth->synth_0_sync_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("OSC 1 - sync enabled"));
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) raven_synth->synth_0_sync_enabled,
		  6, 0,
		  1, 1);

  /* OSC 1 - sync relative attack factor 0 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync factor 1"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  10, 0,
		  1, 1);
  
  raven_synth->synth_0_sync_relative_attack_factor_0 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_sync_relative_attack_factor_0);

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

  ags_dial_set_radius(raven_synth->synth_0_sync_relative_attack_factor_0,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) raven_synth->synth_0_sync_relative_attack_factor_0,
		  11, 0,
		  1, 1);

  /* OSC 1 - sync attack 0 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync attack 1"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  12, 0,
		  1, 1);
  
  raven_synth->synth_0_sync_attack_0 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_sync_attack_0);

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

  ags_dial_set_radius(raven_synth->synth_0_sync_attack_0,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) raven_synth->synth_0_sync_attack_0,
		  13, 0,
		  1, 1);

  /* OSC 1 - sync phase 0 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync phase 1"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  14, 0,
		  1, 1);
  
  raven_synth->synth_0_sync_phase_0 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_sync_phase_0);

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

  ags_dial_set_radius(raven_synth->synth_0_sync_phase_0,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) raven_synth->synth_0_sync_phase_0,
		  15, 0,
		  1, 1);

  /* OSC 1 - sync relative attack factor 1 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync factor 2"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  10, 1,
		  1, 1);
  
  raven_synth->synth_0_sync_relative_attack_factor_1 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_sync_relative_attack_factor_1);

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

  ags_dial_set_radius(raven_synth->synth_0_sync_relative_attack_factor_1,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) raven_synth->synth_0_sync_relative_attack_factor_1,
		  11, 1,
		  1, 1);

  /* OSC 1 - sync attack 1 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync attack 2"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  12, 1,
		  1, 1);
  
  raven_synth->synth_0_sync_attack_1 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_sync_attack_1);

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

  ags_dial_set_radius(raven_synth->synth_0_sync_attack_1,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) raven_synth->synth_0_sync_attack_1,
		  13, 1,
		  1, 1);

  /* OSC 1 - sync phase 1 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync phase 2"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  14, 1,
		  1, 1);
  
  raven_synth->synth_0_sync_phase_1 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_sync_phase_1);

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

  ags_dial_set_radius(raven_synth->synth_0_sync_phase_1,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) raven_synth->synth_0_sync_phase_1,
		  15, 1,
		  1, 1);

  /* OSC 1 - sync relative attack factor 2 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync factor 3"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  10, 2,
		  1, 1);
  
  raven_synth->synth_0_sync_relative_attack_factor_2 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_sync_relative_attack_factor_2);

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

  ags_dial_set_radius(raven_synth->synth_0_sync_relative_attack_factor_2,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) raven_synth->synth_0_sync_relative_attack_factor_2,
		  11, 2,
		  1, 1);

  /* OSC 1 - sync attack 2 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync attack 3"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  12, 2,
		  1, 1);
  
  raven_synth->synth_0_sync_attack_2 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_sync_attack_2);

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

  ags_dial_set_radius(raven_synth->synth_0_sync_attack_2,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) raven_synth->synth_0_sync_attack_2,
		  13, 2,
		  1, 1);

  /* OSC 1 - sync phase 2 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync phase 3"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  14, 2,
		  1, 1);
  
  raven_synth->synth_0_sync_phase_2 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_sync_phase_2);

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

  ags_dial_set_radius(raven_synth->synth_0_sync_phase_2,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) raven_synth->synth_0_sync_phase_2,
		  15, 2,
		  1, 1);

  /* OSC 1 - sync relative attack factor 3 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync factor 4"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  10, 3,
		  1, 1);
  
  raven_synth->synth_0_sync_relative_attack_factor_3 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_sync_relative_attack_factor_3);

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

  ags_dial_set_radius(raven_synth->synth_0_sync_relative_attack_factor_3,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) raven_synth->synth_0_sync_relative_attack_factor_3,
		  11, 3,
		  1, 1);

  /* OSC 1 - sync attack 3 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync attack 4"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  12, 3,
		  1, 1);
  
  raven_synth->synth_0_sync_attack_3 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_sync_attack_3);

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

  ags_dial_set_radius(raven_synth->synth_0_sync_attack_3,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) raven_synth->synth_0_sync_attack_3,
		  13, 3,
		  1, 1);

  /* OSC 1 - sync phase 3 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - sync phase 4"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  14, 3,
		  1, 1);
  
  raven_synth->synth_0_sync_phase_3 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_sync_phase_3);

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

  ags_dial_set_radius(raven_synth->synth_0_sync_phase_3,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) raven_synth->synth_0_sync_phase_3,
		  15, 3,
		  1, 1);

  /* OSC 1 - LFO OSC */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 1 - LFO OSC"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  raven_synth->synth_0_lfo_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(raven_synth->synth_0_lfo_oscillator,
			     0);

  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) raven_synth->synth_0_lfo_oscillator,
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

  raven_synth->synth_0_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 16.0, 0.01);

  gtk_spin_button_set_value(raven_synth->synth_0_lfo_frequency,
			    AGS_FM_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) raven_synth->synth_0_lfo_frequency,
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

  raven_synth->synth_0_lfo_tuning = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_lfo_tuning);

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

  ags_dial_set_radius(raven_synth->synth_0_lfo_tuning,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) raven_synth->synth_0_lfo_tuning,
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

  raven_synth->synth_0_lfo_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_lfo_depth);

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

  ags_dial_set_radius(raven_synth->synth_0_lfo_depth,
		      12);
  
  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) raven_synth->synth_0_lfo_depth,
		  5, 2,
		  1, 1);

  /* seq */
  synth_0_seq_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(synth_0_seq_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(synth_0_seq_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) synth_0_seq_grid,
		  0, 6,
		  12, 4);

  /* synth 0 seq Nr. 0 */
  raven_synth->synth_0_seq_tuning_0 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_tuning_0);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_tuning_0,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_tuning_0,
		  0, 0,
		  1, 1);

  /* synth 0 seq Nr. 1 */
  raven_synth->synth_0_seq_tuning_1 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_tuning_1);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_tuning_1,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_tuning_1,
		  1, 0,
		  1, 1);

  /* synth 0 seq Nr. 2 */
  raven_synth->synth_0_seq_tuning_2 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_tuning_2);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_tuning_2,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_tuning_2,
		  2, 0,
		  1, 1);

  /* synth 0 seq Nr. 3 */
  raven_synth->synth_0_seq_tuning_3 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_tuning_3);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_tuning_3,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_tuning_3,
		  3, 0,
		  1, 1);

  /* synth 0 seq Nr. 4 */
  raven_synth->synth_0_seq_tuning_4 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_tuning_4);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_tuning_4,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_tuning_4,
		  4, 0,
		  1, 1);

  /* synth 0 seq Nr. 5 */
  raven_synth->synth_0_seq_tuning_5 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_tuning_5);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_tuning_5,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_tuning_5,
		  5, 0,
		  1, 1);

  /* synth 0 seq Nr. 6 */
  raven_synth->synth_0_seq_tuning_6 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_tuning_6);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_tuning_6,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_tuning_6,
		  6, 0,
		  1, 1);

  /* synth 0 seq Nr. 7 */
  raven_synth->synth_0_seq_tuning_7 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_tuning_7);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_tuning_7,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_tuning_7,
		  7, 0,
		  1, 1);

  /* synth 0 seq Nr. 8 */
  raven_synth->synth_0_seq_tuning_8 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_tuning_8);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_tuning_8,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_tuning_8,
		  8, 0,
		  1, 1);

  /* synth 0 seq Nr. 9 */
  raven_synth->synth_0_seq_tuning_9 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_tuning_9);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_tuning_9,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_tuning_9,
		  9, 0,
		  1, 1);

  /* synth 0 seq Nr. 10 */
  raven_synth->synth_0_seq_tuning_10 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_tuning_10);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_tuning_10,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_tuning_10,
		  10, 0,
		  1, 1);

  /* synth 0 seq Nr. 11 */
  raven_synth->synth_0_seq_tuning_11 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_tuning_11);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_tuning_11,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_tuning_11,
		  11, 0,
		  1, 1);

  /* synth 0 seq Nr. 12 */
  raven_synth->synth_0_seq_tuning_12 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_tuning_12);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_tuning_12,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_tuning_12,
		  12, 0,
		  1, 1);

  /* synth 0 seq Nr. 13 */
  raven_synth->synth_0_seq_tuning_13 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_tuning_13);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_tuning_13,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_tuning_13,
		  13, 0,
		  1, 1);

  /* synth 0 seq Nr. 14 */
  raven_synth->synth_0_seq_tuning_14 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_tuning_14);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_tuning_14,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_tuning_14,
		  14, 0,
		  1, 1);

  /* synth 0 seq Nr. 15 */
  raven_synth->synth_0_seq_tuning_15 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_tuning_15);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_tuning_15,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_tuning_15,
		  15, 0,
		  1, 1);

  /* synth 0 seq pingpong */
  raven_synth->synth_0_seq_tuning_pingpong = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("Seq 1 tuning - pingpong"));
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_tuning_pingpong,
		  0, 1,
		  4, 1);

  /* synth 0 tuning LFO label */
  label = (GtkLabel *) gtk_label_new(i18n("tuning - LFO"));
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) label,
		  4, 1,
		  2, 1);

  /* synth 0 LFO frequency */
  raven_synth->synth_0_seq_tuning_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 16.0, 0.01);
  gtk_spin_button_set_value(raven_synth->synth_0_seq_tuning_lfo_frequency,
			    8.0);  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_tuning_lfo_frequency,
		  6, 1,
		  2, 1);
  
  /* synth 0 seq Nr. 0 */
  raven_synth->synth_0_seq_volume_0 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_volume_0);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_volume_0,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_volume_0,
		  0, 2,
		  1, 1);

  /* synth 0 seq Nr. 1 */
  raven_synth->synth_0_seq_volume_1 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_volume_1);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_volume_1,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_volume_1,
		  1, 2,
		  1, 1);

  /* synth 0 seq Nr. 2 */
  raven_synth->synth_0_seq_volume_2 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_volume_2);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_volume_2,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_volume_2,
		  2, 2,
		  1, 1);

  /* synth 0 seq Nr. 3 */
  raven_synth->synth_0_seq_volume_3 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_volume_3);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_volume_3,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_volume_3,
		  3, 2,
		  1, 1);

  /* synth 0 seq Nr. 4 */
  raven_synth->synth_0_seq_volume_4 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_volume_4);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_volume_4,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_volume_4,
		  4, 2,
		  1, 1);

  /* synth 0 seq Nr. 5 */
  raven_synth->synth_0_seq_volume_5 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_volume_5);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_volume_5,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_volume_5,
		  5, 2,
		  1, 1);

  /* synth 0 seq Nr. 6 */
  raven_synth->synth_0_seq_volume_6 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_volume_6);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_volume_6,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_volume_6,
		  6, 2,
		  1, 1);

  /* synth 0 seq Nr. 7 */
  raven_synth->synth_0_seq_volume_7 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_volume_7);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_volume_7,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_volume_7,
		  7, 2,
		  1, 1);

  /* synth 0 seq Nr. 8 */
  raven_synth->synth_0_seq_volume_8 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_volume_8);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_volume_8,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_volume_8,
		  8, 2,
		  1, 1);

  /* synth 0 seq Nr. 9 */
  raven_synth->synth_0_seq_volume_9 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_volume_9);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_volume_9,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_volume_9,
		  9, 2,
		  1, 1);

  /* synth 0 seq Nr. 10 */
  raven_synth->synth_0_seq_volume_10 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_volume_10);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_volume_10,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_volume_10,
		  10, 2,
		  1, 1);

  /* synth 0 seq Nr. 11 */
  raven_synth->synth_0_seq_volume_11 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_volume_11);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_volume_11,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_volume_11,
		  11, 2,
		  1, 1);

  /* synth 0 seq Nr. 12 */
  raven_synth->synth_0_seq_volume_12 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_volume_12);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_volume_12,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_volume_12,
		  12, 2,
		  1, 1);

  /* synth 0 seq Nr. 13 */
  raven_synth->synth_0_seq_volume_13 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_volume_13);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_volume_13,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_volume_13,
		  13, 2,
		  1, 1);

  /* synth 0 seq Nr. 14 */
  raven_synth->synth_0_seq_volume_14 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_volume_14);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_volume_14,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_volume_14,
		  14, 2,
		  1, 1);

  /* synth 0 seq Nr. 15 */
  raven_synth->synth_0_seq_volume_15 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_0_seq_volume_15);

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

  ags_dial_set_radius(raven_synth->synth_0_seq_volume_15,
		      12);
  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_volume_15,
		  15, 2,
		  1, 1);

  /* synth 0 seq pingpong */
  raven_synth->synth_0_seq_volume_pingpong = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("Seq 1 volume - pingpong"));
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_volume_pingpong,
		  0, 3,
		  4, 1);

  /* synth 0 volume LFO label */
  label = (GtkLabel *) gtk_label_new(i18n("volume - LFO"));
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) label,
		  4, 3,
		  2, 1);
  
  /* synth 0 LFO frequency */
  raven_synth->synth_0_seq_volume_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 16.0, 0.01);
  gtk_spin_button_set_value(raven_synth->synth_0_seq_volume_lfo_frequency,
			    8.0);  
  gtk_grid_attach(synth_0_seq_grid,
		  (GtkWidget *) raven_synth->synth_0_seq_volume_lfo_frequency,
		  6, 3,
		  2, 1);

  /* effect */
  synth_0_effect_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(synth_0_effect_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(synth_0_effect_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_grid_attach(synth_0_grid,
		  (GtkWidget *) synth_0_effect_grid,
		  0, 10,
		  8, 1);

  /* low-pass 0 cut-off frequency */
  label = (GtkLabel *) gtk_label_new(i18n("low-pass 1 - cut-off frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_effect_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  raven_synth->low_pass_0_cut_off_frequency = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->low_pass_0_cut_off_frequency);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   22000.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   2000.0);
  ags_dial_set_radius(raven_synth->low_pass_0_cut_off_frequency,
		      12);
  
  gtk_grid_attach(synth_0_effect_grid,
		  (GtkWidget *) raven_synth->low_pass_0_cut_off_frequency,
		  1, 0,
		  1, 1);

  /* low-pass 0 filter gain */
  label = (GtkLabel *) gtk_label_new(i18n("low-pass 1 - filter gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_effect_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  raven_synth->low_pass_0_filter_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->low_pass_0_filter_gain);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   1.0);
  ags_dial_set_radius(raven_synth->low_pass_0_filter_gain,
		      12);
  
  gtk_grid_attach(synth_0_effect_grid,
		  (GtkWidget *) raven_synth->low_pass_0_filter_gain,
		  1, 1,
		  1, 1);

  /* low-pass 0 no-clip */
  label = (GtkLabel *) gtk_label_new(i18n("low-pass 1 - no clip"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_effect_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  raven_synth->low_pass_0_no_clip = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->low_pass_0_no_clip);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(raven_synth->low_pass_0_no_clip,
		      12);
  
  gtk_grid_attach(synth_0_effect_grid,
		  (GtkWidget *) raven_synth->low_pass_0_no_clip,
		  1, 2,
		  1, 1);

  /* amplifier 0 - amp 0-3 */
  label = (GtkLabel *) gtk_label_new(i18n("amplifier 1 - amp 1-4"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_effect_grid,
		  (GtkWidget *) label,
		  2, 2,
		  1, 1);
  
  /* amplifier 0 - amp 0 */
  raven_synth->amplifier_0_amp_0_gain = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
									      -20.0,
									      20.0,
									      1.0);
  gtk_widget_set_size_request((GtkWidget *) raven_synth->amplifier_0_amp_0_gain,
			      16, 100);

  gtk_range_set_value((GtkRange *) raven_synth->amplifier_0_amp_0_gain,
		      0.0);
  gtk_range_set_inverted((GtkRange *) raven_synth->amplifier_0_amp_0_gain,
			 TRUE);
  
  gtk_grid_attach(synth_0_effect_grid,
		  (GtkWidget *) raven_synth->amplifier_0_amp_0_gain,
		  3, 0,
		  1, 3);

  /* amplifier 0 - amp 1 */
  raven_synth->amplifier_0_amp_1_gain = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
									      -20.0,
									      20.0,
									      1.0);
  gtk_widget_set_size_request((GtkWidget *) raven_synth->amplifier_0_amp_1_gain,
			      16, 100);

  gtk_range_set_value((GtkRange *) raven_synth->amplifier_0_amp_1_gain,
		      0.0);
  gtk_range_set_inverted((GtkRange *) raven_synth->amplifier_0_amp_1_gain,
			 TRUE);

  gtk_grid_attach(synth_0_effect_grid,
		  (GtkWidget *) raven_synth->amplifier_0_amp_1_gain,
		  4, 0,
		  1, 3);

  /* amplifier 0 - amp 2 */
  raven_synth->amplifier_0_amp_2_gain = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
									      -20.0,
									      20.0,
									      1.0);
  gtk_widget_set_size_request((GtkWidget *) raven_synth->amplifier_0_amp_2_gain,
			      16, 100);

  gtk_range_set_value((GtkRange *) raven_synth->amplifier_0_amp_2_gain,
		      0.0);
  gtk_range_set_inverted((GtkRange *) raven_synth->amplifier_0_amp_2_gain,
			 TRUE);

  gtk_grid_attach(synth_0_effect_grid,
		  (GtkWidget *) raven_synth->amplifier_0_amp_2_gain,
		  5, 0,
		  1, 3);

  /* amplifier 0 - amp 3 */
  raven_synth->amplifier_0_amp_3_gain = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
									      -20.0,
									      20.0,
									      1.0);
  gtk_widget_set_size_request((GtkWidget *) raven_synth->amplifier_0_amp_3_gain,
			      16, 100);

  gtk_range_set_value((GtkRange *) raven_synth->amplifier_0_amp_3_gain,
		      0.0);
  gtk_range_set_inverted((GtkRange *) raven_synth->amplifier_0_amp_3_gain,
			 TRUE);

  gtk_grid_attach(synth_0_effect_grid,
		  (GtkWidget *) raven_synth->amplifier_0_amp_3_gain,
		  6, 0,
		  1, 3);
  
  /* amplifier 0 - filter gain */
  label = (GtkLabel *) gtk_label_new(i18n("amplifier 1 - filter gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_0_effect_grid,
		  (GtkWidget *) label,
		  7, 2,
		  1, 1);

  raven_synth->amplifier_0_filter_gain = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
									       -20.0,
									       20.0,
									       1.0);
  gtk_widget_set_size_request((GtkWidget *) raven_synth->amplifier_0_filter_gain,
			      16, 100);

  gtk_range_set_value((GtkRange *) raven_synth->amplifier_0_filter_gain,
		      0.0);
  gtk_range_set_inverted((GtkRange *) raven_synth->amplifier_0_filter_gain,
			 TRUE);

  gtk_grid_attach(synth_0_effect_grid,
		  (GtkWidget *) raven_synth->amplifier_0_filter_gain,
		  8, 0,
		  1, 3);

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
  
  raven_synth->synth_1_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(raven_synth->synth_1_oscillator,
			     0);

  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) raven_synth->synth_1_oscillator,
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

  raven_synth->synth_1_octave = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_octave);

  gtk_adjustment_set_lower(adjustment,
			   -6.0);
  gtk_adjustment_set_upper(adjustment,
			   6.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(raven_synth->synth_1_octave,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) raven_synth->synth_1_octave,
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

  raven_synth->synth_1_key = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_key);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   2.0);
  ags_dial_set_radius(raven_synth->synth_1_key,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) raven_synth->synth_1_key,
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

  raven_synth->synth_1_phase = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_phase);

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

  ags_dial_set_radius(raven_synth->synth_1_phase,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) raven_synth->synth_1_phase,
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

  raven_synth->synth_1_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_volume);

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

  ags_dial_set_radius(raven_synth->synth_1_volume,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) raven_synth->synth_1_volume,
		  5, 1,
		  1, 1);

  /* OSC 2 - LFO OSC */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - LFO OSC"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  raven_synth->synth_1_lfo_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(raven_synth->synth_1_lfo_oscillator,
			     0);

  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) raven_synth->synth_1_lfo_oscillator,
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

  raven_synth->synth_1_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 16.0, 0.01);

  gtk_spin_button_set_value(raven_synth->synth_1_lfo_frequency,
			    AGS_FM_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
    
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) raven_synth->synth_1_lfo_frequency,
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

  raven_synth->synth_1_lfo_tuning = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_lfo_tuning);

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

  ags_dial_set_radius(raven_synth->synth_1_lfo_tuning,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) raven_synth->synth_1_lfo_tuning,
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

  raven_synth->synth_1_lfo_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_lfo_depth);

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

  ags_dial_set_radius(raven_synth->synth_1_lfo_depth,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) raven_synth->synth_1_lfo_depth,
		  5, 2,
		  1, 1);

  /* OSC 2 - sync enabled */
  raven_synth->synth_1_sync_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("OSC 2 - sync enabled"));
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) raven_synth->synth_1_sync_enabled,
		  6, 0,
		  1, 1);

  /* OSC 2 - sync relative attack factor 0 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync factor 1"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  10, 0,
		  1, 1);
  
  raven_synth->synth_1_sync_relative_attack_factor_0 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_sync_relative_attack_factor_0);

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

  ags_dial_set_radius(raven_synth->synth_1_sync_relative_attack_factor_0,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) raven_synth->synth_1_sync_relative_attack_factor_0,
		  11, 0,
		  1, 1);

  /* OSC 2 - sync attack 0 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync attack 1"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  12, 0,
		  1, 1);
  
  raven_synth->synth_1_sync_attack_0 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_sync_attack_0);

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

  ags_dial_set_radius(raven_synth->synth_1_sync_attack_0,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) raven_synth->synth_1_sync_attack_0,
		  13, 0,
		  1, 1);

  /* OSC 2 - sync phase 0 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync phase 1"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  14, 0,
		  1, 1);
  
  raven_synth->synth_1_sync_phase_0 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_sync_phase_0);

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

  ags_dial_set_radius(raven_synth->synth_1_sync_phase_0,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) raven_synth->synth_1_sync_phase_0,
		  15, 0,
		  1, 1);

  /* OSC 2 - sync relative attack factor 1 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync factor 2"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  10, 1,
		  1, 1);
  
  raven_synth->synth_1_sync_relative_attack_factor_1 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_sync_relative_attack_factor_1);

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

  ags_dial_set_radius(raven_synth->synth_1_sync_relative_attack_factor_1,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) raven_synth->synth_1_sync_relative_attack_factor_1,
		  11, 1,
		  1, 1);

  /* OSC 2 - sync attack 1 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync attack 2"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  12, 1,
		  1, 1);
  
  raven_synth->synth_1_sync_attack_1 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_sync_attack_1);

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

  ags_dial_set_radius(raven_synth->synth_1_sync_attack_1,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) raven_synth->synth_1_sync_attack_1,
		  13, 1,
		  1, 1);

  /* OSC 2 - sync phase 1 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync phase 2"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  14, 1,
		  1, 1);
  
  raven_synth->synth_1_sync_phase_1 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_sync_phase_1);

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

  ags_dial_set_radius(raven_synth->synth_1_sync_phase_1,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) raven_synth->synth_1_sync_phase_1,
		  15, 1,
		  1, 1);

  /* OSC 2 - sync relative attack factor 2 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync factor 3"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  10, 2,
		  1, 1);
  
  raven_synth->synth_1_sync_relative_attack_factor_2 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_sync_relative_attack_factor_2);

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

  ags_dial_set_radius(raven_synth->synth_1_sync_relative_attack_factor_2,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) raven_synth->synth_1_sync_relative_attack_factor_2,
		  11, 2,
		  1, 1);

  /* OSC 2 - sync attack 2 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync attack 3"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  12, 2,
		  1, 1);
  
  raven_synth->synth_1_sync_attack_2 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_sync_attack_2);

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

  ags_dial_set_radius(raven_synth->synth_1_sync_attack_2,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) raven_synth->synth_1_sync_attack_2,
		  13, 2,
		  1, 1);

  /* OSC 2 - sync phase 2 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync phase 3"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  14, 2,
		  1, 1);
  
  raven_synth->synth_1_sync_phase_2 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_sync_phase_2);

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

  ags_dial_set_radius(raven_synth->synth_1_sync_phase_2,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) raven_synth->synth_1_sync_phase_2,
		  15, 2,
		  1, 1);

  /* OSC 2 - sync relative attack factor 3 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync factor 4"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  10, 3,
		  1, 1);
  
  raven_synth->synth_1_sync_relative_attack_factor_3 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_sync_relative_attack_factor_3);

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

  ags_dial_set_radius(raven_synth->synth_1_sync_relative_attack_factor_3,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) raven_synth->synth_1_sync_relative_attack_factor_3,
		  11, 3,
		  1, 1);

  /* OSC 2 - sync attack 3 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync attack 4"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  12, 3,
		  1, 1);
  
  raven_synth->synth_1_sync_attack_3 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_sync_attack_3);

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

  ags_dial_set_radius(raven_synth->synth_1_sync_attack_3,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) raven_synth->synth_1_sync_attack_3,
		  13, 3,
		  1, 1);

  /* OSC 2 - sync phase 3 */
  label = (GtkLabel *) gtk_label_new(i18n("OSC 2 - sync phase 4"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) label,
		  14, 3,
		  1, 1);
  
  raven_synth->synth_1_sync_phase_3 = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_sync_phase_3);

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

  ags_dial_set_radius(raven_synth->synth_1_sync_phase_3,
		      12);
  
  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) raven_synth->synth_1_sync_phase_3,
		  15, 3,
		  1, 1);

  /* seq */
  synth_1_seq_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(synth_1_seq_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(synth_1_seq_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) synth_1_seq_grid,
		  0, 4,
		  12, 4);

  /* synth 1 seq Nr. 0 */
  raven_synth->synth_1_seq_tuning_0 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_tuning_0);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_tuning_0,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_tuning_0,
		  0, 0,
		  1, 1);

  /* synth 1 seq Nr. 1 */
  raven_synth->synth_1_seq_tuning_1 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_tuning_1);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_tuning_1,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_tuning_1,
		  1, 0,
		  1, 1);

  /* synth 1 seq Nr. 2 */
  raven_synth->synth_1_seq_tuning_2 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_tuning_2);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_tuning_2,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_tuning_2,
		  2, 0,
		  1, 1);

  /* synth 1 seq Nr. 3 */
  raven_synth->synth_1_seq_tuning_3 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_tuning_3);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_tuning_3,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_tuning_3,
		  3, 0,
		  1, 1);

  /* synth 1 seq Nr. 4 */
  raven_synth->synth_1_seq_tuning_4 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_tuning_4);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_tuning_4,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_tuning_4,
		  4, 0,
		  1, 1);

  /* synth 1 seq Nr. 5 */
  raven_synth->synth_1_seq_tuning_5 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_tuning_5);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_tuning_5,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_tuning_5,
		  5, 0,
		  1, 1);

  /* synth 1 seq Nr. 6 */
  raven_synth->synth_1_seq_tuning_6 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_tuning_6);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_tuning_6,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_tuning_6,
		  6, 0,
		  1, 1);

  /* synth 1 seq Nr. 7 */
  raven_synth->synth_1_seq_tuning_7 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_tuning_7);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_tuning_7,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_tuning_7,
		  7, 0,
		  1, 1);

  /* synth 1 seq Nr. 8 */
  raven_synth->synth_1_seq_tuning_8 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_tuning_8);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_tuning_8,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_tuning_8,
		  8, 0,
		  1, 1);

  /* synth 1 seq Nr. 9 */
  raven_synth->synth_1_seq_tuning_9 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_tuning_9);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_tuning_9,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_tuning_9,
		  9, 0,
		  1, 1);

  /* synth 1 seq Nr. 10 */
  raven_synth->synth_1_seq_tuning_10 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_tuning_10);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_tuning_10,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_tuning_10,
		  10, 0,
		  1, 1);

  /* synth 1 seq Nr. 11 */
  raven_synth->synth_1_seq_tuning_11 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_tuning_11);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_tuning_11,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_tuning_11,
		  11, 0,
		  1, 1);

  /* synth 1 seq Nr. 12 */
  raven_synth->synth_1_seq_tuning_12 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_tuning_12);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_tuning_12,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_tuning_12,
		  12, 0,
		  1, 1);

  /* synth 1 seq Nr. 13 */
  raven_synth->synth_1_seq_tuning_13 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_tuning_13);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_tuning_13,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_tuning_13,
		  13, 0,
		  1, 1);

  /* synth 1 seq Nr. 14 */
  raven_synth->synth_1_seq_tuning_14 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_tuning_14);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_tuning_14,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_tuning_14,
		  14, 0,
		  1, 1);

  /* synth 1 seq Nr. 15 */
  raven_synth->synth_1_seq_tuning_15 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_tuning_15);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_tuning_15,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_tuning_15,
		  15, 0,
		  1, 1);

  /* synth 1 seq pingpong */
  raven_synth->synth_1_seq_tuning_pingpong = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("Seq 2 tuning - pingpong"));
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_tuning_pingpong,
		  0, 1,
		  4, 1);

  /* synth 1 tuning LFO label */
  label = (GtkLabel *) gtk_label_new(i18n("tuning - LFO"));
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) label,
		  4, 1,
		  2, 1);

  /* synth 1 LFO frequency */
  raven_synth->synth_1_seq_tuning_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 16.0, 0.01);
  gtk_spin_button_set_value(raven_synth->synth_1_seq_tuning_lfo_frequency,
			    8.0);  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_tuning_lfo_frequency,
		  6, 1,
		  2, 1);
  
  /* synth 1 seq Nr. 0 */
  raven_synth->synth_1_seq_volume_0 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_volume_0);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_volume_0,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_volume_0,
		  0, 2,
		  1, 1);

  /* synth 1 seq Nr. 1 */
  raven_synth->synth_1_seq_volume_1 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_volume_1);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_volume_1,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_volume_1,
		  1, 2,
		  1, 1);

  /* synth 1 seq Nr. 2 */
  raven_synth->synth_1_seq_volume_2 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_volume_2);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_volume_2,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_volume_2,
		  2, 2,
		  1, 1);

  /* synth 1 seq Nr. 3 */
  raven_synth->synth_1_seq_volume_3 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_volume_3);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_volume_3,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_volume_3,
		  3, 2,
		  1, 1);

  /* synth 1 seq Nr. 4 */
  raven_synth->synth_1_seq_volume_4 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_volume_4);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_volume_4,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_volume_4,
		  4, 2,
		  1, 1);

  /* synth 1 seq Nr. 5 */
  raven_synth->synth_1_seq_volume_5 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_volume_5);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_volume_5,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_volume_5,
		  5, 2,
		  1, 1);

  /* synth 1 seq Nr. 6 */
  raven_synth->synth_1_seq_volume_6 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_volume_6);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_volume_6,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_volume_6,
		  6, 2,
		  1, 1);

  /* synth 1 seq Nr. 7 */
  raven_synth->synth_1_seq_volume_7 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_volume_7);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_volume_7,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_volume_7,
		  7, 2,
		  1, 1);

  /* synth 1 seq Nr. 8 */
  raven_synth->synth_1_seq_volume_8 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_volume_8);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_volume_8,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_volume_8,
		  8, 2,
		  1, 1);

  /* synth 1 seq Nr. 9 */
  raven_synth->synth_1_seq_volume_9 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_volume_9);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_volume_9,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_volume_9,
		  9, 2,
		  1, 1);

  /* synth 1 seq Nr. 10 */
  raven_synth->synth_1_seq_volume_10 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_volume_10);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_volume_10,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_volume_10,
		  10, 2,
		  1, 1);

  /* synth 1 seq Nr. 11 */
  raven_synth->synth_1_seq_volume_11 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_volume_11);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_volume_11,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_volume_11,
		  11, 2,
		  1, 1);

  /* synth 1 seq Nr. 12 */
  raven_synth->synth_1_seq_volume_12 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_volume_12);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_volume_12,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_volume_12,
		  12, 2,
		  1, 1);

  /* synth 1 seq Nr. 13 */
  raven_synth->synth_1_seq_volume_13 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_volume_13);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_volume_13,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_volume_13,
		  13, 2,
		  1, 1);

  /* synth 1 seq Nr. 14 */
  raven_synth->synth_1_seq_volume_14 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_volume_14);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_volume_14,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_volume_14,
		  14, 2,
		  1, 1);

  /* synth 1 seq Nr. 15 */
  raven_synth->synth_1_seq_volume_15 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(raven_synth->synth_1_seq_volume_15);

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

  ags_dial_set_radius(raven_synth->synth_1_seq_volume_15,
		      12);
  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_volume_15,
		  15, 2,
		  1, 1);

  /* synth 1 seq pingpong */
  raven_synth->synth_1_seq_volume_pingpong = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("Seq 2 volume - pingpong"));
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_volume_pingpong,
		  0, 3,
		  4, 1);

  /* synth 1 volume LFO label */
  label = (GtkLabel *) gtk_label_new(i18n("volume - LFO"));
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) label,
		  4, 3,
		  2, 1);
  
  /* synth 1 LFO frequency */
  raven_synth->synth_1_seq_volume_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 16.0, 0.01);
  gtk_spin_button_set_value(raven_synth->synth_1_seq_volume_lfo_frequency,
			    8.0);  
  gtk_grid_attach(synth_1_seq_grid,
		  (GtkWidget *) raven_synth->synth_1_seq_volume_lfo_frequency,
		  6, 3,
		  2, 1);

  /* effect */
  synth_1_effect_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(synth_1_effect_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(synth_1_effect_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_grid_attach(synth_1_grid,
		  (GtkWidget *) synth_1_effect_grid,
		  0, 10,
		  8, 1);

  /* low-pass 1 cut-off frequency */
  label = (GtkLabel *) gtk_label_new(i18n("low-pass 2 - cut-off frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_effect_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  raven_synth->low_pass_1_cut_off_frequency = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->low_pass_1_cut_off_frequency);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   22000.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   2000.0);
  ags_dial_set_radius(raven_synth->low_pass_1_cut_off_frequency,
		      12);
  
  gtk_grid_attach(synth_1_effect_grid,
		  (GtkWidget *) raven_synth->low_pass_1_cut_off_frequency,
		  1, 0,
		  1, 1);

  /* low-pass 1 filter gain */
  label = (GtkLabel *) gtk_label_new(i18n("low-pass 2 - filter gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_effect_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  raven_synth->low_pass_1_filter_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->low_pass_1_filter_gain);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   1.0);
  ags_dial_set_radius(raven_synth->low_pass_1_filter_gain,
		      12);
  
  gtk_grid_attach(synth_1_effect_grid,
		  (GtkWidget *) raven_synth->low_pass_1_filter_gain,
		  1, 1,
		  1, 1);

  /* low-pass 1 no-clip */
  label = (GtkLabel *) gtk_label_new(i18n("low-pass 2 - no clip"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_effect_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  raven_synth->low_pass_1_no_clip = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->low_pass_1_no_clip);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(raven_synth->low_pass_1_no_clip,
		      12);
  
  gtk_grid_attach(synth_1_effect_grid,
		  (GtkWidget *) raven_synth->low_pass_1_no_clip,
		  1, 2,
		  1, 1);

  /* amplifier 1 - amp 0-3 */
  label = (GtkLabel *) gtk_label_new(i18n("amplifier 2 - amp 1-4"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_effect_grid,
		  (GtkWidget *) label,
		  2, 2,
		  1, 1);
  
  /* amplifier 1 - amp 0 */
  raven_synth->amplifier_1_amp_0_gain = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
									      -20.0,
									      20.0,
									      1.0);
  gtk_widget_set_size_request((GtkWidget *) raven_synth->amplifier_1_amp_0_gain,
			      16, 100);

  gtk_range_set_value((GtkRange *) raven_synth->amplifier_1_amp_0_gain,
		      0.0);
  gtk_range_set_inverted((GtkRange *) raven_synth->amplifier_1_amp_0_gain,
			 TRUE);
  
  gtk_grid_attach(synth_1_effect_grid,
		  (GtkWidget *) raven_synth->amplifier_1_amp_0_gain,
		  3, 0,
		  1, 3);

  /* amplifier 1 - amp 1 */
  raven_synth->amplifier_1_amp_1_gain = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
									      -20.0,
									      20.0,
									      1.0);
  gtk_widget_set_size_request((GtkWidget *) raven_synth->amplifier_1_amp_1_gain,
			      16, 100);

  gtk_range_set_value((GtkRange *) raven_synth->amplifier_1_amp_1_gain,
		      0.0);
  gtk_range_set_inverted((GtkRange *) raven_synth->amplifier_1_amp_1_gain,
			 TRUE);

  gtk_grid_attach(synth_1_effect_grid,
		  (GtkWidget *) raven_synth->amplifier_1_amp_1_gain,
		  4, 0,
		  1, 3);

  /* amplifier 1 - amp 2 */
  raven_synth->amplifier_1_amp_2_gain = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
									      -20.0,
									      20.0,
									      1.0);
  gtk_widget_set_size_request((GtkWidget *) raven_synth->amplifier_1_amp_2_gain,
			      16, 100);

  gtk_range_set_value((GtkRange *) raven_synth->amplifier_1_amp_2_gain,
		      0.0);
  gtk_range_set_inverted((GtkRange *) raven_synth->amplifier_1_amp_2_gain,
			 TRUE);

  gtk_grid_attach(synth_1_effect_grid,
		  (GtkWidget *) raven_synth->amplifier_1_amp_2_gain,
		  5, 0,
		  1, 3);

  /* amplifier 1 - amp 3 */
  raven_synth->amplifier_1_amp_3_gain = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
									      -20.0,
									      20.0,
									      1.0);
  gtk_widget_set_size_request((GtkWidget *) raven_synth->amplifier_1_amp_3_gain,
			      16, 100);

  gtk_range_set_value((GtkRange *) raven_synth->amplifier_1_amp_3_gain,
		      0.0);
  gtk_range_set_inverted((GtkRange *) raven_synth->amplifier_1_amp_3_gain,
			 TRUE);

  gtk_grid_attach(synth_1_effect_grid,
		  (GtkWidget *) raven_synth->amplifier_1_amp_3_gain,
		  6, 0,
		  1, 3);
  
  /* amplifier 1 - filter gain */
  label = (GtkLabel *) gtk_label_new(i18n("amplifier 2 - filter gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_1_effect_grid,
		  (GtkWidget *) label,
		  7, 2,
		  1, 1);

  raven_synth->amplifier_1_filter_gain = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
									       -20.0,
									       20.0,
									       1.0);
  gtk_widget_set_size_request((GtkWidget *) raven_synth->amplifier_1_filter_gain,
			      16, 100);

  gtk_range_set_value((GtkRange *) raven_synth->amplifier_1_filter_gain,
		      0.0);
  gtk_range_set_inverted((GtkRange *) raven_synth->amplifier_1_filter_gain,
			 TRUE);

  gtk_grid_attach(synth_1_effect_grid,
		  (GtkWidget *) raven_synth->amplifier_1_filter_gain,
		  8, 0,
		  1, 3);
  
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

  raven_synth->pitch_type = (GtkDropDown *) gtk_drop_down_new_from_strings((const gchar * const *) pitch_type_strv);

  gtk_drop_down_set_selected(raven_synth->pitch_type,
			     2);

  gtk_box_append(pitch_type_hbox,
		 (GtkWidget *) raven_synth->pitch_type);
  
  /* pitch */
  label = (GtkLabel *) gtk_label_new(i18n("pitch tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(misc_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  raven_synth->pitch_tuning = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->pitch_tuning);

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

  ags_dial_set_radius(raven_synth->pitch_tuning,
		      12);
  
  gtk_grid_attach(misc_grid,
		  (GtkWidget *) raven_synth->pitch_tuning,
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

  raven_synth->noise_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->noise_gain);

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

  ags_dial_set_radius(raven_synth->noise_gain,
		      12);
  
  gtk_grid_attach(misc_grid,
		  (GtkWidget *) raven_synth->noise_gain,
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

  raven_synth->chorus_input_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->chorus_input_volume);

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

  ags_dial_set_radius(raven_synth->chorus_input_volume,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) raven_synth->chorus_input_volume,
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

  raven_synth->chorus_output_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->chorus_output_volume);

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

  ags_dial_set_radius(raven_synth->chorus_output_volume,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) raven_synth->chorus_output_volume,
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
  
  raven_synth->chorus_lfo_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(raven_synth->chorus_lfo_oscillator,
			     0);

  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) raven_synth->chorus_lfo_oscillator,
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

  raven_synth->chorus_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 10.0, 0.01);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) raven_synth->chorus_lfo_frequency,
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

  raven_synth->chorus_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->chorus_depth);

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

  ags_dial_set_radius(raven_synth->chorus_depth,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) raven_synth->chorus_depth,
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

  raven_synth->chorus_mix = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->chorus_mix);

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

  ags_dial_set_radius(raven_synth->chorus_mix,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) raven_synth->chorus_mix,
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

  raven_synth->chorus_delay = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(raven_synth->chorus_delay);

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

  ags_dial_set_radius(raven_synth->chorus_delay,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) raven_synth->chorus_delay,
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

  raven_synth->tremolo_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) raven_synth->tremolo_enabled,
		  0, 0,
		  2, 1);

  label = (GtkLabel *) gtk_label_new(i18n("gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  raven_synth->tremolo_gain = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(raven_synth->tremolo_gain,
		      12);

  adjustment = ags_dial_get_adjustment(raven_synth->tremolo_gain);

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
		  (GtkWidget *) raven_synth->tremolo_gain,
		  1, 1,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("lfo-depth"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);

  raven_synth->tremolo_lfo_depth = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(raven_synth->tremolo_lfo_depth,
		      12);

  adjustment = ags_dial_get_adjustment(raven_synth->tremolo_lfo_depth);

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
		  (GtkWidget *) raven_synth->tremolo_lfo_depth,
		  1, 2,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("lfo-freq"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);

  raven_synth->tremolo_lfo_freq = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(raven_synth->tremolo_lfo_freq,
		      12);

  adjustment = ags_dial_get_adjustment(raven_synth->tremolo_lfo_freq);

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
		  (GtkWidget *) raven_synth->tremolo_lfo_freq,
		  1, 3,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);

  raven_synth->tremolo_tuning = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(raven_synth->tremolo_tuning,
		      12);

  adjustment = ags_dial_get_adjustment(raven_synth->tremolo_tuning);

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
		  (GtkWidget *) raven_synth->tremolo_tuning,
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

  raven_synth->vibrato_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) raven_synth->vibrato_enabled,
		  0, 0,
		  2, 1);

  label = (GtkLabel *) gtk_label_new(i18n("gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  raven_synth->vibrato_gain = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(raven_synth->vibrato_gain,
		      12);

  adjustment = ags_dial_get_adjustment(raven_synth->vibrato_gain);

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
		  (GtkWidget *) raven_synth->vibrato_gain,
		  1, 1,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("lfo-depth"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);

  raven_synth->vibrato_lfo_depth = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(raven_synth->vibrato_lfo_depth,
		      12);

  adjustment = ags_dial_get_adjustment(raven_synth->vibrato_lfo_depth);

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
		  (GtkWidget *) raven_synth->vibrato_lfo_depth,
		  1, 2,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("lfo-freq"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);

  raven_synth->vibrato_lfo_freq = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(raven_synth->vibrato_lfo_freq,
		      12);
  
  adjustment = ags_dial_get_adjustment(raven_synth->vibrato_lfo_freq);

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
		  (GtkWidget *) raven_synth->vibrato_lfo_freq,
		  1, 3,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);

  raven_synth->vibrato_tuning = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(raven_synth->vibrato_tuning,
		      12);

  adjustment = ags_dial_get_adjustment(raven_synth->vibrato_tuning);

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
		  (GtkWidget *) raven_synth->vibrato_tuning,
		  1, 4,
		  1, 1);
}

void
ags_raven_synth_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_raven_synth_parent_class)->finalize(gobject);
}

void
ags_raven_synth_connect(AgsConnectable *connectable)
{
  AgsRavenSynth *raven_synth;
  
  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) != 0){
    return;
  }

  ags_raven_synth_parent_connectable_interface->connect(connectable);
  
  /* AgsRavenSynth */
  raven_synth = AGS_RAVEN_SYNTH(connectable);

  g_signal_connect_after(raven_synth->synth_0_oscillator, "notify::selected",
			 G_CALLBACK(ags_raven_synth_synth_0_oscillator_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_octave, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_octave_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_key, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_key_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_phase, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_phase_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_volume, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_volume_callback), raven_synth);

  g_signal_connect_after(raven_synth->synth_0_lfo_oscillator, "notify::selected",
			 G_CALLBACK(ags_raven_synth_synth_0_lfo_oscillator_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_lfo_frequency_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_lfo_depth, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_lfo_depth_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_lfo_tuning, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_lfo_tuning_callback), raven_synth);
    
  g_signal_connect_after(raven_synth->synth_0_sync_enabled, "toggled",
			 G_CALLBACK(ags_raven_synth_synth_0_sync_enabled_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_sync_relative_attack_factor_0, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_sync_relative_attack_factor_0_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_sync_attack_0, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_sync_attack_0_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_sync_phase_0, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_sync_phase_0_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_sync_relative_attack_factor_1, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_sync_relative_attack_factor_1_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_sync_attack_1, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_sync_attack_1_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_sync_phase_1, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_sync_phase_1_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_sync_relative_attack_factor_2, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_sync_relative_attack_factor_2_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_sync_attack_2, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_sync_attack_2_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_sync_phase_2, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_sync_phase_2_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_sync_relative_attack_factor_3, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_sync_relative_attack_factor_3_callback), raven_synth);

  g_signal_connect_after(raven_synth->synth_0_sync_attack_3, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_sync_attack_3_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_sync_phase_3, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_sync_phase_3_callback), raven_synth);

  /* seq tuning */
  g_signal_connect_after(raven_synth->synth_0_seq_tuning_0, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_0_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_tuning_1, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_1_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_tuning_2, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_2_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_tuning_3, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_3_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_tuning_4, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_4_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_tuning_5, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_5_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_tuning_6, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_6_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_tuning_7, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_7_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_tuning_8, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_8_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_tuning_9, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_9_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_tuning_10, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_10_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_tuning_11, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_11_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_tuning_12, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_12_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_tuning_13, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_13_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_tuning_14, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_14_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_tuning_15, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_15_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_tuning_pingpong, "toggled",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_pingpong_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_tuning_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_lfo_frequency_callback), raven_synth);

  /* seq volume */
  g_signal_connect_after(raven_synth->synth_0_seq_volume_0, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_volume_0_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_volume_1, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_volume_1_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_volume_2, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_volume_2_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_volume_3, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_volume_3_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_volume_4, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_volume_4_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_volume_5, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_volume_5_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_volume_6, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_volume_6_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_volume_7, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_volume_7_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_volume_8, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_volume_8_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_volume_9, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_volume_9_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_volume_10, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_volume_10_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_volume_11, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_volume_11_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_volume_12, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_volume_12_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_volume_13, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_volume_13_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_volume_14, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_volume_14_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_volume_15, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_volume_15_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_volume_pingpong, "toggled",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_volume_pingpong_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_0_seq_volume_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_0_seq_volume_lfo_frequency_callback), raven_synth);

  /* low-pass 0 */
  g_signal_connect_after(raven_synth->low_pass_0_cut_off_frequency, "value-changed",
			 G_CALLBACK(ags_raven_synth_low_pass_0_cut_off_frequency_callback), raven_synth);

  g_signal_connect_after(raven_synth->low_pass_0_filter_gain, "value-changed",
			 G_CALLBACK(ags_raven_synth_low_pass_0_filter_gain_callback), raven_synth);

  g_signal_connect_after(raven_synth->low_pass_0_no_clip, "value-changed",
			 G_CALLBACK(ags_raven_synth_low_pass_0_no_clip_callback), raven_synth);

  /* amplifier 0 */  
  g_signal_connect_after(raven_synth->amplifier_0_amp_0_gain, "value-changed",
			 G_CALLBACK(ags_raven_synth_amplifier_0_amp_0_gain_callback), raven_synth);

  g_signal_connect_after(raven_synth->amplifier_0_amp_1_gain, "value-changed",
			 G_CALLBACK(ags_raven_synth_amplifier_0_amp_1_gain_callback), raven_synth);

  g_signal_connect_after(raven_synth->amplifier_0_amp_2_gain, "value-changed",
			 G_CALLBACK(ags_raven_synth_amplifier_0_amp_2_gain_callback), raven_synth);

  g_signal_connect_after(raven_synth->amplifier_0_amp_3_gain, "value-changed",
			 G_CALLBACK(ags_raven_synth_amplifier_0_amp_3_gain_callback), raven_synth);

  g_signal_connect_after(raven_synth->amplifier_0_filter_gain, "value-changed",
			 G_CALLBACK(ags_raven_synth_amplifier_0_filter_gain_callback), raven_synth);
  
  /* synth 1 */  
  g_signal_connect_after(raven_synth->synth_1_oscillator, "notify::selected",
			 G_CALLBACK(ags_raven_synth_synth_1_oscillator_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_octave, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_octave_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_key, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_key_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_phase, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_phase_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_volume, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_volume_callback), raven_synth);

  g_signal_connect_after(raven_synth->synth_1_lfo_oscillator, "notify::selected",
			 G_CALLBACK(ags_raven_synth_synth_1_lfo_oscillator_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_lfo_frequency_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_lfo_depth, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_lfo_depth_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_lfo_tuning, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_lfo_tuning_callback), raven_synth);
    
  g_signal_connect_after(raven_synth->synth_1_sync_enabled, "toggled",
			 G_CALLBACK(ags_raven_synth_synth_1_sync_enabled_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_sync_relative_attack_factor_0, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_sync_relative_attack_factor_0_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_sync_attack_0, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_sync_attack_0_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_sync_phase_0, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_sync_phase_0_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_sync_relative_attack_factor_1, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_sync_relative_attack_factor_1_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_sync_attack_1, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_sync_attack_1_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_sync_phase_1, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_sync_phase_1_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_sync_relative_attack_factor_2, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_sync_relative_attack_factor_2_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_sync_attack_2, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_sync_attack_2_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_sync_phase_2, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_sync_phase_2_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_sync_relative_attack_factor_3, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_sync_relative_attack_factor_3_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_sync_attack_3, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_sync_attack_3_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_sync_phase_3, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_sync_phase_3_callback), raven_synth);

  /* seq tuning */
  g_signal_connect_after(raven_synth->synth_1_seq_tuning_0, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_0_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_tuning_1, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_1_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_tuning_2, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_2_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_tuning_3, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_3_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_tuning_4, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_4_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_tuning_5, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_5_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_tuning_6, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_6_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_tuning_7, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_7_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_tuning_8, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_8_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_tuning_9, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_9_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_tuning_10, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_10_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_tuning_11, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_11_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_tuning_12, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_12_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_tuning_13, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_13_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_tuning_14, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_14_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_tuning_15, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_15_callback), raven_synth);
    
  g_signal_connect_after(raven_synth->synth_1_seq_tuning_pingpong, "toggled",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_pingpong_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_tuning_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_lfo_frequency_callback), raven_synth);

  /* seq volume */
  g_signal_connect_after(raven_synth->synth_1_seq_volume_0, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_volume_0_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_volume_1, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_volume_1_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_volume_2, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_volume_2_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_volume_3, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_volume_3_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_volume_4, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_volume_4_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_volume_5, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_volume_5_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_volume_6, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_volume_6_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_volume_7, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_volume_7_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_volume_8, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_volume_8_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_volume_9, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_volume_9_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_volume_10, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_volume_10_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_volume_11, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_volume_11_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_volume_12, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_volume_12_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_volume_13, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_volume_13_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_volume_14, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_volume_14_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_volume_15, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_volume_15_callback), raven_synth);
    
  g_signal_connect_after(raven_synth->synth_1_seq_volume_pingpong, "toggled",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_volume_pingpong_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->synth_1_seq_volume_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_raven_synth_synth_1_seq_volume_lfo_frequency_callback), raven_synth);

  /* low-pass 1 */
  g_signal_connect_after(raven_synth->low_pass_1_cut_off_frequency, "value-changed",
			 G_CALLBACK(ags_raven_synth_low_pass_1_cut_off_frequency_callback), raven_synth);

  g_signal_connect_after(raven_synth->low_pass_1_filter_gain, "value-changed",
			 G_CALLBACK(ags_raven_synth_low_pass_1_filter_gain_callback), raven_synth);

  g_signal_connect_after(raven_synth->low_pass_1_no_clip, "value-changed",
			 G_CALLBACK(ags_raven_synth_low_pass_1_no_clip_callback), raven_synth);

  /* amplifier 1 */  
  g_signal_connect_after(raven_synth->amplifier_1_amp_0_gain, "value-changed",
			 G_CALLBACK(ags_raven_synth_amplifier_1_amp_0_gain_callback), raven_synth);

  g_signal_connect_after(raven_synth->amplifier_1_amp_1_gain, "value-changed",
			 G_CALLBACK(ags_raven_synth_amplifier_1_amp_1_gain_callback), raven_synth);

  g_signal_connect_after(raven_synth->amplifier_1_amp_2_gain, "value-changed",
			 G_CALLBACK(ags_raven_synth_amplifier_1_amp_2_gain_callback), raven_synth);

  g_signal_connect_after(raven_synth->amplifier_1_amp_3_gain, "value-changed",
			 G_CALLBACK(ags_raven_synth_amplifier_1_amp_3_gain_callback), raven_synth);

  g_signal_connect_after(raven_synth->amplifier_1_filter_gain, "value-changed",
			 G_CALLBACK(ags_raven_synth_amplifier_1_filter_gain_callback), raven_synth);

  /* pitch */
  g_signal_connect((GObject *) raven_synth->pitch_type, "notify::selected",
		   G_CALLBACK(ags_raven_synth_pitch_type_callback), (gpointer) raven_synth);

  g_signal_connect_after(raven_synth->pitch_tuning, "value-changed",
			 G_CALLBACK(ags_raven_synth_pitch_tuning_callback), raven_synth);

  /* noise */
  g_signal_connect_after(raven_synth->noise_gain, "value-changed",
			 G_CALLBACK(ags_raven_synth_noise_gain_callback), raven_synth);
    
  //  g_signal_connect_after(raven_synth->chorus_enabled, "toggled",
  //			 G_CALLBACK(ags_raven_synth_chorus_enabled_callback), raven_synth);

  /* chorus */
  g_signal_connect_after(raven_synth->chorus_input_volume, "value-changed",
			 G_CALLBACK(ags_raven_synth_chorus_input_volume_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->chorus_output_volume, "value-changed",
			 G_CALLBACK(ags_raven_synth_chorus_output_volume_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->chorus_lfo_oscillator, "notify::selected",
			 G_CALLBACK(ags_raven_synth_chorus_lfo_oscillator_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->chorus_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_raven_synth_chorus_lfo_frequency_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->chorus_depth, "value-changed",
			 G_CALLBACK(ags_raven_synth_chorus_depth_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->chorus_mix, "value-changed",
			 G_CALLBACK(ags_raven_synth_chorus_mix_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->chorus_delay, "value-changed",
			 G_CALLBACK(ags_raven_synth_chorus_delay_callback), raven_synth);

  g_signal_connect_after(raven_synth->chorus_input_volume, "value-changed",
			 G_CALLBACK(ags_raven_synth_chorus_input_volume_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->chorus_output_volume, "value-changed",
			 G_CALLBACK(ags_raven_synth_chorus_output_volume_callback), raven_synth);

  g_signal_connect_after(raven_synth->tremolo_enabled, "toggled",
			 G_CALLBACK(ags_raven_synth_tremolo_enabled_callback), raven_synth);

  g_signal_connect_after(raven_synth->tremolo_gain, "value-changed",
			 G_CALLBACK(ags_raven_synth_tremolo_gain_callback), raven_synth);
  
  g_signal_connect_after(raven_synth->tremolo_lfo_depth, "value-changed",
			 G_CALLBACK(ags_raven_synth_tremolo_lfo_depth_callback), raven_synth);

  g_signal_connect_after(raven_synth->tremolo_lfo_freq, "value-changed",
			 G_CALLBACK(ags_raven_synth_tremolo_lfo_freq_callback), raven_synth);

  g_signal_connect_after(raven_synth->tremolo_tuning, "value-changed",
			 G_CALLBACK(ags_raven_synth_tremolo_tuning_callback), raven_synth);

  g_signal_connect_after(raven_synth->vibrato_enabled, "toggled",
			 G_CALLBACK(ags_raven_synth_vibrato_enabled_callback), raven_synth);

  g_signal_connect_after(raven_synth->vibrato_gain, "value-changed",
			 G_CALLBACK(ags_raven_synth_vibrato_gain_callback), raven_synth);

  g_signal_connect_after(raven_synth->vibrato_lfo_depth, "value-changed",
			 G_CALLBACK(ags_raven_synth_vibrato_lfo_depth_callback), raven_synth);

  g_signal_connect_after(raven_synth->vibrato_lfo_freq, "value-changed",
			 G_CALLBACK(ags_raven_synth_vibrato_lfo_freq_callback), raven_synth);

  g_signal_connect_after(raven_synth->vibrato_tuning, "value-changed",
			 G_CALLBACK(ags_raven_synth_vibrato_tuning_callback), raven_synth);
}

void
ags_raven_synth_disconnect(AgsConnectable *connectable)
{
  AgsRavenSynth *raven_synth;
  
  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) == 0){
    return;
  }

  ags_raven_synth_parent_connectable_interface->disconnect(connectable);

  /* AgsRavenSynth */
  raven_synth = AGS_RAVEN_SYNTH(connectable);

  /* synth 0 */
  g_object_disconnect(raven_synth->synth_0_oscillator,
		      "any_signal::changed",
		      G_CALLBACK(ags_raven_synth_synth_0_oscillator_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_octave,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_octave_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_key,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_key_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_phase,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_phase_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_volume_callback),
		      raven_synth,
		      NULL);
  
  /* LFO */
  g_object_disconnect(raven_synth->synth_0_lfo_oscillator,
		      "any_signal::changed",
		      G_CALLBACK(ags_raven_synth_synth_0_lfo_oscillator_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_lfo_frequency_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_lfo_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_lfo_depth_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_lfo_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_lfo_tuning_callback),
		      raven_synth,
		      NULL);

  /* sync */
  g_object_disconnect(raven_synth->synth_0_sync_enabled,
		      "any_signal::toggled",
		      G_CALLBACK(ags_raven_synth_synth_0_sync_enabled_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_sync_relative_attack_factor_0,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_sync_relative_attack_factor_0_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_sync_attack_0,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_sync_attack_0_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_sync_phase_0,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_sync_phase_0_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_sync_relative_attack_factor_1,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_sync_relative_attack_factor_1_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_sync_attack_1,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_sync_attack_1_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_sync_phase_1,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_sync_phase_1_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_sync_relative_attack_factor_2,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_sync_relative_attack_factor_2_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_sync_attack_2,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_sync_attack_2_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_sync_phase_2,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_sync_phase_2_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_sync_relative_attack_factor_3,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_sync_relative_attack_factor_3_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_sync_attack_3,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_sync_attack_3_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_sync_phase_3,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_sync_phase_3_callback),
		      raven_synth,
		      NULL);

  /* seq tuning */
  g_object_disconnect(raven_synth->synth_0_seq_tuning_0,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_0_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_tuning_1,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_1_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_tuning_2,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_2_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_tuning_3,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_3_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_tuning_4,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_4_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_tuning_5,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_5_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_tuning_6,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_6_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_tuning_7,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_7_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_tuning_8,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_8_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_tuning_9,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_9_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_tuning_10,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_10_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_tuning_11,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_11_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_tuning_12,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_12_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_tuning_13,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_13_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_tuning_14,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_14_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_tuning_15,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_15_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_tuning_pingpong,
		      "any_signal::toggled",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_pingpong_callback),
		      raven_synth,
		      NULL);

  g_object_disconnect(raven_synth->synth_0_seq_tuning_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_tuning_lfo_frequency_callback),
		      raven_synth,
		      NULL);

  /* seq volume */
  g_object_disconnect(raven_synth->synth_0_seq_volume_0,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_volume_0_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_volume_1,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_volume_1_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_volume_2,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_volume_2_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_volume_3,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_volume_3_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_volume_4,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_volume_4_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_volume_5,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_volume_5_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_volume_6,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_volume_6_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_volume_7,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_volume_7_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_volume_8,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_volume_8_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_volume_9,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_volume_9_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_volume_10,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_volume_10_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_volume_11,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_volume_11_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_volume_12,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_volume_12_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_volume_13,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_volume_13_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_volume_14,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_volume_14_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_volume_15,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_volume_15_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_0_seq_volume_pingpong,
		      "any_signal::toggled",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_volume_pingpong_callback),
		      raven_synth,
		      NULL);

  g_object_disconnect(raven_synth->synth_0_seq_volume_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_0_seq_volume_lfo_frequency_callback),
		      raven_synth,
		      NULL);

  /* low-pass 0 */  
  g_object_disconnect(raven_synth->low_pass_0_cut_off_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_low_pass_0_cut_off_frequency_callback),
		      raven_synth,
		      NULL);

  g_object_disconnect(raven_synth->low_pass_0_filter_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_low_pass_0_filter_gain_callback),
		      raven_synth,
		      NULL);

  g_object_disconnect(raven_synth->low_pass_0_no_clip,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_low_pass_0_no_clip_callback),
		      raven_synth,
		      NULL);

  /* amplifier 0 */  
  g_object_disconnect(raven_synth->amplifier_0_amp_0_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_amplifier_0_amp_0_gain_callback),
		      raven_synth,
		      NULL);

  g_object_disconnect(raven_synth->amplifier_0_amp_1_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_amplifier_0_amp_1_gain_callback),
		      raven_synth,
		      NULL);

  g_object_disconnect(raven_synth->amplifier_0_amp_2_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_amplifier_0_amp_2_gain_callback),
		      raven_synth,
		      NULL);

  g_object_disconnect(raven_synth->amplifier_0_amp_3_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_amplifier_0_amp_3_gain_callback),
		      raven_synth,
		      NULL);

  g_object_disconnect(raven_synth->amplifier_0_filter_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_amplifier_0_filter_gain_callback),
		      raven_synth,
		      NULL);

  /* synth 1 */
  g_object_disconnect(raven_synth->synth_1_oscillator,
		      "any_signal::changed",
		      G_CALLBACK(ags_raven_synth_synth_1_oscillator_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_octave,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_octave_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_key,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_key_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_phase,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_phase_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_volume_callback),
		      raven_synth,
		      NULL);

  /* sync */
  g_object_disconnect(raven_synth->synth_1_sync_enabled,
		      "any_signal::toggled",
		      G_CALLBACK(ags_raven_synth_synth_1_sync_enabled_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_sync_relative_attack_factor_0,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_sync_relative_attack_factor_0_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_sync_attack_0,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_sync_attack_0_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_sync_phase_0,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_sync_phase_0_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_sync_relative_attack_factor_1,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_sync_relative_attack_factor_1_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_sync_attack_1,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_sync_attack_1_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_sync_phase_1,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_sync_phase_1_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_sync_relative_attack_factor_2,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_sync_relative_attack_factor_2_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_sync_attack_2,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_sync_attack_2_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_sync_phase_2,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_sync_phase_2_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_sync_relative_attack_factor_3,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_sync_relative_attack_factor_3_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_sync_attack_3,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_sync_attack_3_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_sync_phase_3,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_sync_phase_3_callback),
		      raven_synth,
		      NULL);
  
  /* LFO */
  g_object_disconnect(raven_synth->synth_1_lfo_oscillator,
		      "any_signal::changed",
		      G_CALLBACK(ags_raven_synth_synth_1_lfo_oscillator_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_lfo_frequency_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_lfo_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_lfo_depth_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_lfo_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_lfo_tuning_callback),
		      raven_synth,
		      NULL);

  /* seq tuning */
  g_object_disconnect(raven_synth->synth_1_seq_tuning_0,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_0_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_tuning_1,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_1_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_tuning_2,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_2_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_tuning_3,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_3_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_tuning_4,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_4_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_tuning_5,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_5_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_tuning_6,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_6_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_tuning_7,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_7_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_tuning_8,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_8_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_tuning_9,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_9_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_tuning_10,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_10_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_tuning_11,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_11_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_tuning_12,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_12_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_tuning_13,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_13_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_tuning_14,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_14_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_tuning_15,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_15_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_tuning_pingpong,
		      "any_signal::toggled",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_pingpong_callback),
		      raven_synth,
		      NULL);

  g_object_disconnect(raven_synth->synth_1_seq_tuning_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_tuning_lfo_frequency_callback),
		      raven_synth,
		      NULL);

  /* seq volume */
  g_object_disconnect(raven_synth->synth_1_seq_volume_0,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_volume_0_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_volume_1,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_volume_1_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_volume_2,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_volume_2_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_volume_3,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_volume_3_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_volume_4,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_volume_4_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_volume_5,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_volume_5_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_volume_6,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_volume_6_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_volume_7,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_volume_7_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_volume_8,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_volume_8_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_volume_9,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_volume_9_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_volume_10,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_volume_10_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_volume_11,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_volume_11_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_volume_12,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_volume_12_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_volume_13,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_volume_13_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_volume_14,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_volume_14_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_volume_15,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_volume_15_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->synth_1_seq_volume_pingpong,
		      "any_signal::toggled",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_volume_pingpong_callback),
		      raven_synth,
		      NULL);

  g_object_disconnect(raven_synth->synth_1_seq_volume_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_synth_1_seq_volume_lfo_frequency_callback),
		      raven_synth,
		      NULL);

  /* low-pass 1 */  
  g_object_disconnect(raven_synth->low_pass_1_cut_off_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_low_pass_1_cut_off_frequency_callback),
		      raven_synth,
		      NULL);

  g_object_disconnect(raven_synth->low_pass_1_filter_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_low_pass_1_filter_gain_callback),
		      raven_synth,
		      NULL);

  g_object_disconnect(raven_synth->low_pass_1_no_clip,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_low_pass_1_no_clip_callback),
		      raven_synth,
		      NULL);

  /* amplifier 1 */  
  g_object_disconnect(raven_synth->amplifier_1_amp_0_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_amplifier_1_amp_0_gain_callback),
		      raven_synth,
		      NULL);

  g_object_disconnect(raven_synth->amplifier_1_amp_1_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_amplifier_1_amp_1_gain_callback),
		      raven_synth,
		      NULL);

  g_object_disconnect(raven_synth->amplifier_1_amp_2_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_amplifier_1_amp_2_gain_callback),
		      raven_synth,
		      NULL);

  g_object_disconnect(raven_synth->amplifier_1_amp_3_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_amplifier_1_amp_3_gain_callback),
		      raven_synth,
		      NULL);

  g_object_disconnect(raven_synth->amplifier_1_filter_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_amplifier_1_filter_gain_callback),
		      raven_synth,
		      NULL);

  /* pitch */
  g_object_disconnect((GObject *) raven_synth->pitch_type,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_raven_synth_pitch_type_callback),
		      (gpointer) raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->pitch_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_pitch_tuning_callback),
		      raven_synth,
		      NULL);

  /* noise */
  g_object_disconnect(raven_synth->noise_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_noise_gain_callback),
		      raven_synth,
		      NULL);

  /* chorus */
  //  g_object_disconnect(raven_synth->chorus_enabled,
  //		      "any_signal::toggled",
  //			 G_CALLBACK(ags_raven_synth_chorus_enabled_callback),
  //		      raven_synth,
  //		      NULL);
  
  g_object_disconnect(raven_synth->chorus_input_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_chorus_input_volume_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->chorus_output_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_chorus_output_volume_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->chorus_lfo_oscillator,
		      "any_signal::changed",
		      G_CALLBACK(ags_raven_synth_chorus_lfo_oscillator_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->chorus_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_chorus_lfo_frequency_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->chorus_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_chorus_depth_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->chorus_mix,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_chorus_mix_callback),
		      raven_synth,
		      NULL);
  
  g_object_disconnect(raven_synth->chorus_delay,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_chorus_delay_callback),
		      raven_synth,
		      NULL);

  /* tremolo */
  g_object_disconnect((GObject *) raven_synth->tremolo_enabled,
		      "any_signal::toggled",
		      G_CALLBACK(ags_raven_synth_tremolo_enabled_callback),
		      raven_synth,
		      NULL);

  g_object_disconnect((GObject *) raven_synth->tremolo_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_tremolo_gain_callback),
		      (gpointer) raven_synth,
		      NULL);

  g_object_disconnect((GObject *) raven_synth->tremolo_lfo_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_tremolo_lfo_depth_callback),
		      (gpointer) raven_synth,
		      NULL);

  g_object_disconnect((GObject *) raven_synth->tremolo_lfo_freq,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_tremolo_lfo_freq_callback),
		      (gpointer) raven_synth,
		      NULL);

  g_object_disconnect((GObject *) raven_synth->tremolo_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_tremolo_tuning_callback),
		      (gpointer) raven_synth,
		      NULL);

  /* vibrato */
  g_object_disconnect((GObject *) raven_synth->vibrato_enabled,
		      "any_signal::toggled",
		      G_CALLBACK(ags_raven_synth_vibrato_enabled_callback),
		      raven_synth,
		      NULL);

  g_object_disconnect((GObject *) raven_synth->vibrato_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_vibrato_gain_callback),
		      (gpointer) raven_synth,
		      NULL);

  g_object_disconnect((GObject *) raven_synth->vibrato_lfo_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_vibrato_lfo_depth_callback),
		      (gpointer) raven_synth,
		      NULL);

  g_object_disconnect((GObject *) raven_synth->vibrato_lfo_freq,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_raven_synth_vibrato_lfo_freq_callback),
		      (gpointer) raven_synth,
		      NULL);
}

void
ags_raven_synth_resize_audio_channels(AgsMachine *machine,
				      guint audio_channels, guint audio_channels_old,
				      gpointer data)
{
  AgsRavenSynth *raven_synth;

  raven_synth = (AgsRavenSynth *) machine;

  if(audio_channels > audio_channels_old){    
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_raven_synth_input_map_recall(raven_synth,
				       audio_channels_old,
				       0);
      
      ags_raven_synth_output_map_recall(raven_synth,
					audio_channels_old,
					0);
    }
  }
}

void
ags_raven_synth_resize_pads(AgsMachine *machine, GType type,
			    guint pads, guint pads_old,
			    gpointer data)
{
  AgsRavenSynth *raven_synth;

  gboolean grow;

  raven_synth = (AgsRavenSynth *) machine;
  
  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(type == AGS_TYPE_INPUT){
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	/* depending on destination */
	ags_raven_synth_input_map_recall(raven_synth,
					 0,
					 pads_old);
      }
    }else{
      raven_synth->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_raven_synth_output_map_recall(raven_synth,
					  0,
					  pads_old);
      }
    }else{
      raven_synth->mapped_output_pad = pads;
    }
  }
}

void
ags_raven_synth_map_recall(AgsMachine *machine)
{
  AgsRavenSynth *raven_synth;
  
  AgsAudio *audio;

  GList *start_recall, *recall;

  gint position;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  raven_synth = AGS_RAVEN_SYNTH(machine);

  audio = machine->audio;
  
  position = 0;

  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       raven_synth->playback_play_container, raven_synth->playback_recall_container,
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

  /* ags-fx-raven-synth */
  start_recall = ags_fx_factory_create(audio,
				       raven_synth->raven_synth_play_container, raven_synth->raven_synth_recall_container,
				       "ags-fx-raven-synth",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);
  
  recall = start_recall;

  while(recall != NULL){
    if(AGS_IS_RECALL_AUDIO(recall->data) ||
       AGS_IS_RECALL_CHANNEL(recall->data)){
#if defined(AGS_OSXAPI)
      ags_recall_set_flags(recall->data,
			   (AGS_RECALL_MIDI2 | AGS_RECALL_MIDI2_CONTROL_CHANGE));
#else
      ags_recall_set_flags(recall->data,
			   (AGS_RECALL_MIDI1 | AGS_RECALL_MIDI1_CONTROL_CHANGE));
#endif
    }

    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-tremolo */
  start_recall = ags_fx_factory_create(audio,
				       raven_synth->tremolo_play_container, raven_synth->tremolo_recall_container,
				       "ags-fx-tremolo",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);
  
  recall = start_recall;

  while(recall != NULL){
    if(AGS_IS_RECALL_AUDIO(recall->data) ||
       AGS_IS_RECALL_CHANNEL(recall->data)){
#if defined(AGS_OSXAPI)
      ags_recall_set_flags(recall->data,
			   (AGS_RECALL_MIDI2 | AGS_RECALL_MIDI2_CONTROL_CHANGE));
#else
      ags_recall_set_flags(recall->data,
			   (AGS_RECALL_MIDI1 | AGS_RECALL_MIDI1_CONTROL_CHANGE));
#endif
    }

    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-volume */
  start_recall = ags_fx_factory_create(audio,
				       raven_synth->volume_play_container, raven_synth->volume_recall_container,
				       "ags-fx-volume",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);
  
  recall = start_recall;

  while(recall != NULL){
    if(AGS_IS_RECALL_AUDIO(recall->data) ||
       AGS_IS_RECALL_CHANNEL(recall->data)){
#if defined(AGS_OSXAPI)
      ags_recall_set_flags(recall->data,
			   (AGS_RECALL_MIDI2 | AGS_RECALL_MIDI2_CONTROL_CHANGE));
#else
      ags_recall_set_flags(recall->data,
			   (AGS_RECALL_MIDI1 | AGS_RECALL_MIDI1_CONTROL_CHANGE));
#endif
    }

    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
  

  /* ags-fx-envelope */
  start_recall = ags_fx_factory_create(audio,
				       raven_synth->envelope_play_container, raven_synth->envelope_recall_container,
				       "ags-fx-envelope",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);
  
  recall = start_recall;

  while(recall != NULL){
    if(AGS_IS_RECALL_AUDIO(recall->data) ||
       AGS_IS_RECALL_CHANNEL(recall->data)){
#if defined(AGS_OSXAPI)
      ags_recall_set_flags(recall->data,
			   (AGS_RECALL_MIDI2 | AGS_RECALL_MIDI2_CONTROL_CHANGE));
#else
      ags_recall_set_flags(recall->data,
			   (AGS_RECALL_MIDI1 | AGS_RECALL_MIDI1_CONTROL_CHANGE));
#endif
    }

    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
  
  /* ags-fx-buffer */
  start_recall = ags_fx_factory_create(audio,
				       raven_synth->buffer_play_container, raven_synth->buffer_recall_container,
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
  ags_raven_synth_input_map_recall(raven_synth,
				   0,
				   0);

  /* depending on destination */
  ags_raven_synth_output_map_recall(raven_synth,
				    0,
				    0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_raven_synth_parent_class)->map_recall(machine);  
}

void
ags_raven_synth_input_map_recall(AgsRavenSynth *raven_synth,
				 guint audio_channel_start,
				 guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall, *recall;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(raven_synth->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

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
				       raven_synth->playback_play_container, raven_synth->playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-raven-synth */
  start_recall = ags_fx_factory_create(audio,
				       raven_synth->raven_synth_play_container, raven_synth->raven_synth_recall_container,
				       "ags-fx-raven-synth",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);
  
  recall = start_recall;

  while(recall != NULL){
    if(AGS_IS_RECALL_AUDIO(recall->data) ||
       AGS_IS_RECALL_CHANNEL(recall->data)){
#if defined(AGS_OSXAPI)
      ags_recall_set_flags(recall->data,
			   (AGS_RECALL_MIDI2 | AGS_RECALL_MIDI2_CONTROL_CHANGE));
#else
      ags_recall_set_flags(recall->data,
			   (AGS_RECALL_MIDI1 | AGS_RECALL_MIDI1_CONTROL_CHANGE));
#endif
    }

    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-tremolo */
  start_recall = ags_fx_factory_create(audio,
				       raven_synth->tremolo_play_container, raven_synth->tremolo_recall_container,
				       "ags-fx-tremolo",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);
  
  recall = start_recall;

  while(recall != NULL){
    if(AGS_IS_RECALL_AUDIO(recall->data) ||
       AGS_IS_RECALL_CHANNEL(recall->data)){
#if defined(AGS_OSXAPI)
      ags_recall_set_flags(recall->data,
			   (AGS_RECALL_MIDI2 | AGS_RECALL_MIDI2_CONTROL_CHANGE));
#else
      ags_recall_set_flags(recall->data,
			   (AGS_RECALL_MIDI1 | AGS_RECALL_MIDI1_CONTROL_CHANGE));
#endif
    }

    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-volume */
  start_recall = ags_fx_factory_create(audio,
				       raven_synth->volume_play_container, raven_synth->volume_recall_container,
				       "ags-fx-volume",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);
  
  recall = start_recall;

  while(recall != NULL){
    if(AGS_IS_RECALL_AUDIO(recall->data) ||
       AGS_IS_RECALL_CHANNEL(recall->data)){
#if defined(AGS_OSXAPI)
      ags_recall_set_flags(recall->data,
			   (AGS_RECALL_MIDI2 | AGS_RECALL_MIDI2_CONTROL_CHANGE));
#else
      ags_recall_set_flags(recall->data,
			   (AGS_RECALL_MIDI1 | AGS_RECALL_MIDI1_CONTROL_CHANGE));
#endif
    }

    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-envelope */
  start_recall = ags_fx_factory_create(audio,
				       raven_synth->envelope_play_container, raven_synth->envelope_recall_container,
				       "ags-fx-envelope",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);
  
  recall = start_recall;

  while(recall != NULL){
    if(AGS_IS_RECALL_AUDIO(recall->data) ||
       AGS_IS_RECALL_CHANNEL(recall->data)){
#if defined(AGS_OSXAPI)
      ags_recall_set_flags(recall->data,
			   (AGS_RECALL_MIDI2 | AGS_RECALL_MIDI2_CONTROL_CHANGE));
#else
      ags_recall_set_flags(recall->data,
			   (AGS_RECALL_MIDI1 | AGS_RECALL_MIDI1_CONTROL_CHANGE));
#endif
    }

    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
  
  /* ags-fx-buffer */
  start_recall = ags_fx_factory_create(audio,
				       raven_synth->buffer_play_container, raven_synth->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  raven_synth->mapped_input_pad = input_pads;
}

void
ags_raven_synth_output_map_recall(AgsRavenSynth *raven_synth,
				  guint audio_channel_start,
				  guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;

  if(raven_synth->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(raven_synth)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);
  
  raven_synth->mapped_output_pad = output_pads;
}

void
ags_raven_synth_refresh_port(AgsMachine *machine)
{
  AgsRavenSynth *raven_synth;
  
  GList *start_play, *start_recall, *recall;

  raven_synth = (AgsRavenSynth *) machine;
  
  start_play = ags_audio_get_play(machine->audio);
  start_recall = ags_audio_get_recall(machine->audio);

  recall =
    start_recall = g_list_concat(start_play, start_recall);

  machine->flags |= AGS_MACHINE_NO_UPDATE;

  if((recall = ags_recall_find_type(recall, AGS_TYPE_FX_RAVEN_SYNTH_AUDIO)) != NULL){
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

      gtk_drop_down_set_selected(raven_synth->synth_0_oscillator,
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

      ags_dial_set_value(raven_synth->synth_0_octave,
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

      ags_dial_set_value(raven_synth->synth_0_key,
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

      ags_dial_set_value(raven_synth->synth_0_phase,
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

      ags_dial_set_value(raven_synth->synth_0_volume,
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

      gtk_drop_down_set_selected(raven_synth->synth_0_lfo_oscillator,
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

      gtk_spin_button_set_value(raven_synth->synth_0_lfo_frequency,
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

      ags_dial_set_value(raven_synth->synth_0_lfo_depth,
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

      ags_dial_set_value(raven_synth->synth_0_lfo_tuning,
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
	gtk_check_button_set_active(raven_synth->synth_0_sync_enabled,
				    TRUE);
      }else{
	gtk_check_button_set_active(raven_synth->synth_0_sync_enabled,
				    FALSE);
      }

      g_object_unref(port);
    }

    /* synth-0 sync relative attack factor 0 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-sync-relative-attack-factor-0", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_0_sync_relative_attack_factor_0,
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

      ags_dial_set_value(raven_synth->synth_0_sync_attack_0,
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

      ags_dial_set_value(raven_synth->synth_0_sync_phase_0,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 sync relative attack factor 1 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-sync-relative-attack-factor-1", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_0_sync_relative_attack_factor_1,
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

      ags_dial_set_value(raven_synth->synth_0_sync_attack_1,
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

      ags_dial_set_value(raven_synth->synth_0_sync_phase_1,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 sync relative attack factor 2 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-sync-relative-attack-factor-2", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_0_sync_relative_attack_factor_2,
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

      ags_dial_set_value(raven_synth->synth_0_sync_attack_2,
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

      ags_dial_set_value(raven_synth->synth_0_sync_phase_2,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 sync attack 3 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-sync-attack-3", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_0_sync_attack_3,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 sync phase 3 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-sync-phase-3", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_0_sync_phase_3,
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

      ags_dial_set_value(raven_synth->synth_0_seq_tuning_0,
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

      ags_dial_set_value(raven_synth->synth_0_seq_tuning_1,
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

      ags_dial_set_value(raven_synth->synth_0_seq_tuning_2,
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

      ags_dial_set_value(raven_synth->synth_0_seq_tuning_3,
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

      ags_dial_set_value(raven_synth->synth_0_seq_tuning_4,
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

      ags_dial_set_value(raven_synth->synth_0_seq_tuning_5,
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

      ags_dial_set_value(raven_synth->synth_0_seq_tuning_6,
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

      ags_dial_set_value(raven_synth->synth_0_seq_tuning_7,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq tuning 8 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-tuning-8", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_0_seq_tuning_8,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq tuning 9 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-tuning-9", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_0_seq_tuning_9,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq tuning 10 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-tuning-10", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_0_seq_tuning_10,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq tuning 11 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-tuning-11", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_0_seq_tuning_11,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq tuning 12 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-tuning-12", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_0_seq_tuning_12,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq tuning 13 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-tuning-13", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_0_seq_tuning_13,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq tuning 14 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-tuning-14", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_0_seq_tuning_14,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq tuning 15 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-tuning-15", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_0_seq_tuning_15,
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
	gtk_check_button_set_active(raven_synth->synth_0_seq_tuning_pingpong,
				    TRUE);
      }else{
	gtk_check_button_set_active(raven_synth->synth_0_seq_tuning_pingpong,
				    FALSE);
      }

      g_object_unref(port);
    }

    /* synth-0 seq tuning lfo frequency */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-tuning-lfo-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_spin_button_set_value(raven_synth->synth_0_seq_tuning_lfo_frequency,
				(gdouble) g_value_get_float(&value));

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

      ags_dial_set_value(raven_synth->synth_0_seq_volume_0,
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

      ags_dial_set_value(raven_synth->synth_0_seq_volume_1,
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

      ags_dial_set_value(raven_synth->synth_0_seq_volume_2,
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

      ags_dial_set_value(raven_synth->synth_0_seq_volume_3,
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

      ags_dial_set_value(raven_synth->synth_0_seq_volume_4,
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

      ags_dial_set_value(raven_synth->synth_0_seq_volume_5,
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

      ags_dial_set_value(raven_synth->synth_0_seq_volume_6,
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

      ags_dial_set_value(raven_synth->synth_0_seq_volume_7,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq volume 8 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-volume-8", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_0_seq_volume_8,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq volume 9 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-volume-9", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_0_seq_volume_9,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq volume 10 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-volume-10", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_0_seq_volume_10,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq volume 11 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-volume-11", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_0_seq_volume_11,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq volume 12 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-volume-12", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_0_seq_volume_12,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq volume 13 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-volume-13", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_0_seq_volume_13,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq volume 14 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-volume-14", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_0_seq_volume_14,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 seq volume 15 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-volume-15", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_0_seq_volume_15,
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
	gtk_check_button_set_active(raven_synth->synth_0_seq_volume_pingpong,
				    TRUE);
      }else{
	gtk_check_button_set_active(raven_synth->synth_0_seq_volume_pingpong,
				    FALSE);
      }

      g_object_unref(port);
    }

    /* synth-0 seq volume lfo frequency */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-seq-volume-lfo-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_spin_button_set_value(raven_synth->synth_0_seq_volume_lfo_frequency,
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

      gtk_drop_down_set_selected(raven_synth->synth_1_oscillator,
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

      ags_dial_set_value(raven_synth->synth_1_octave,
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

      ags_dial_set_value(raven_synth->synth_1_key,
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

      ags_dial_set_value(raven_synth->synth_1_phase,
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

      ags_dial_set_value(raven_synth->synth_1_volume,
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

      gtk_drop_down_set_selected(raven_synth->synth_1_lfo_oscillator,
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

      gtk_spin_button_set_value(raven_synth->synth_1_lfo_frequency,
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

      ags_dial_set_value(raven_synth->synth_1_lfo_depth,
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

      ags_dial_set_value(raven_synth->synth_1_lfo_tuning,
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
	gtk_check_button_set_active(raven_synth->synth_1_sync_enabled,
				    TRUE);
      }else{
	gtk_check_button_set_active(raven_synth->synth_1_sync_enabled,
				    FALSE);
      }

      g_object_unref(port);
    }

    /* synth-1 sync relative attack factor 0 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-sync-relative-attack-factor-0", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_1_sync_relative_attack_factor_0,
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

      ags_dial_set_value(raven_synth->synth_1_sync_attack_0,
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

      ags_dial_set_value(raven_synth->synth_1_sync_phase_0,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 sync relative attack factor 1 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-sync-relative-attack-factor-1", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_1_sync_relative_attack_factor_1,
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

      ags_dial_set_value(raven_synth->synth_1_sync_attack_1,
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

      ags_dial_set_value(raven_synth->synth_1_sync_phase_1,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 sync relative attack factor 2 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-sync-relative-attack-factor-2", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_1_sync_relative_attack_factor_2,
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

      ags_dial_set_value(raven_synth->synth_1_sync_attack_2,
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

      ags_dial_set_value(raven_synth->synth_1_sync_phase_2,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 sync relative attack factor 3 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-sync-relative-attack-factor-3", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_1_sync_relative_attack_factor_3,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 sync attack 3 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-sync-attack-3", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_1_sync_attack_3,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 sync phase 3 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-sync-phase-3", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_1_sync_phase_3,
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

      ags_dial_set_value(raven_synth->synth_1_seq_tuning_0,
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

      ags_dial_set_value(raven_synth->synth_1_seq_tuning_1,
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

      ags_dial_set_value(raven_synth->synth_1_seq_tuning_2,
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

      ags_dial_set_value(raven_synth->synth_1_seq_tuning_3,
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

      ags_dial_set_value(raven_synth->synth_1_seq_tuning_4,
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

      ags_dial_set_value(raven_synth->synth_1_seq_tuning_5,
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

      ags_dial_set_value(raven_synth->synth_1_seq_tuning_6,
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

      ags_dial_set_value(raven_synth->synth_1_seq_tuning_7,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq tuning 8 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-tuning-8", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_1_seq_tuning_8,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq tuning 9 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-tuning-9", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_1_seq_tuning_9,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq tuning 10 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-tuning-10", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_1_seq_tuning_10,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq tuning 11 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-tuning-11", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_1_seq_tuning_11,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq tuning 12 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-tuning-12", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_1_seq_tuning_12,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq tuning 13 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-tuning-13", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_1_seq_tuning_13,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq tuning 14 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-tuning-14", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_1_seq_tuning_14,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq tuning 15 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-tuning-15", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_1_seq_tuning_15,
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
	gtk_check_button_set_active(raven_synth->synth_1_seq_tuning_pingpong,
				    TRUE);
      }else{
	gtk_check_button_set_active(raven_synth->synth_1_seq_tuning_pingpong,
				    FALSE);
      }

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
	gtk_check_button_set_active(raven_synth->synth_1_seq_tuning_pingpong,
				    TRUE);
      }else{
	gtk_check_button_set_active(raven_synth->synth_1_seq_tuning_pingpong,
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

      ags_dial_set_value(raven_synth->synth_1_seq_volume_0,
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

      ags_dial_set_value(raven_synth->synth_1_seq_volume_1,
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

      ags_dial_set_value(raven_synth->synth_1_seq_volume_2,
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

      ags_dial_set_value(raven_synth->synth_1_seq_volume_3,
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

      ags_dial_set_value(raven_synth->synth_1_seq_volume_4,
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

      ags_dial_set_value(raven_synth->synth_1_seq_volume_5,
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

      ags_dial_set_value(raven_synth->synth_1_seq_volume_6,
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

      ags_dial_set_value(raven_synth->synth_1_seq_volume_7,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq volume 8 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-volume-8", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_1_seq_volume_8,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq volume 9 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-volume-9", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_1_seq_volume_9,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq volume 10 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-volume-10", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_1_seq_volume_10,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq volume 11 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-volume-11", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_1_seq_volume_11,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq volume 12 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-volume-12", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_1_seq_volume_12,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq volume 13 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-volume-13", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_1_seq_volume_13,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq volume 14 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-volume-14", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_1_seq_volume_14,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-1 seq volume 15 */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-volume-15", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->synth_1_seq_volume_15,
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
	gtk_check_button_set_active(raven_synth->synth_1_seq_volume_pingpong,
				    TRUE);
      }else{
	gtk_check_button_set_active(raven_synth->synth_1_seq_volume_pingpong,
				    FALSE);
      }

      g_object_unref(port);
    }

    /* synth-1 seq volume lfo frequency */
    port = NULL;

    g_object_get(recall->data,
		 "synth-1-seq-volume-lfo-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_spin_button_set_value(raven_synth->synth_1_seq_volume_lfo_frequency,
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

      gtk_drop_down_set_selected(raven_synth->pitch_type,
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

      ags_dial_set_value(raven_synth->pitch_tuning,
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

      ags_dial_set_value(raven_synth->noise_gain,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }
    
    /* low-pass 0 cut off frequency */
    port = NULL;

    g_object_get(recall->data,
		 "low-pass-0-cut-off-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->low_pass_0_cut_off_frequency,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }
    
    /* low-pass 0 filter gain */
    port = NULL;

    g_object_get(recall->data,
		 "low-pass-0-filter-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->low_pass_0_filter_gain,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }
    
    /* low-pass 0 no clip */
    port = NULL;

    g_object_get(recall->data,
		 "low-pass-0-no-clip", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->low_pass_0_no_clip,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* amplifier 0 amp 0 gain */
    port = NULL;

    g_object_get(recall->data,
		 "amplifier-0-amp-0-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_range_set_value((GtkRange *) raven_synth->amplifier_0_amp_0_gain,
			  (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* amplifier 0 amp 1 gain */
    port = NULL;

    g_object_get(recall->data,
		 "amplifier-0-amp-1-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_range_set_value((GtkRange *) raven_synth->amplifier_0_amp_1_gain,
			  (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* amplifier 0 amp 2 gain */
    port = NULL;

    g_object_get(recall->data,
		 "amplifier-0-amp-2-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_range_set_value((GtkRange *) raven_synth->amplifier_0_amp_2_gain,
			  (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* amplifier 0 amp 3 gain */
    port = NULL;

    g_object_get(recall->data,
		 "amplifier-0-amp-3-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_range_set_value((GtkRange *) raven_synth->amplifier_0_amp_3_gain,
			  (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* amplifier 0 filter gain */
    port = NULL;

    g_object_get(recall->data,
		 "amplifier-0-filter-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_range_set_value((GtkRange *) raven_synth->amplifier_0_filter_gain,
			  (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }
    
    /* low-pass 1 cut off frequency */
    port = NULL;

    g_object_get(recall->data,
		 "low-pass-1-cut-off-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->low_pass_1_cut_off_frequency,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }
    
    /* low-pass 1 filter gain */
    port = NULL;

    g_object_get(recall->data,
		 "low-pass-1-filter-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->low_pass_1_filter_gain,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }
    
    /* low-pass 1 no clip */
    port = NULL;

    g_object_get(recall->data,
		 "low-pass-1-no-clip", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(raven_synth->low_pass_1_no_clip,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* amplifier 1 amp 0 gain */
    port = NULL;

    g_object_get(recall->data,
		 "amplifier-1-amp-0-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_range_set_value((GtkRange *) raven_synth->amplifier_1_amp_0_gain,
			  (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* amplifier 1 amp 1 gain */
    port = NULL;

    g_object_get(recall->data,
		 "amplifier-1-amp-1-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_range_set_value((GtkRange *) raven_synth->amplifier_1_amp_1_gain,
			  (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* amplifier 1 amp 2 gain */
    port = NULL;

    g_object_get(recall->data,
		 "amplifier-1-amp-2-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_range_set_value((GtkRange *) raven_synth->amplifier_1_amp_2_gain,
			  (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* amplifier 1 amp 3 gain */
    port = NULL;

    g_object_get(recall->data,
		 "amplifier-1-amp-3-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_range_set_value((GtkRange *) raven_synth->amplifier_1_amp_3_gain,
			  (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* amplifier 1 filter gain */
    port = NULL;

    g_object_get(recall->data,
		 "amplifier-1-filter-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_range_set_value((GtkRange *) raven_synth->amplifier_1_filter_gain,
			  (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* chorus enabled */
#if 0
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
	gtk_check_button_set_active(raven_synth->chorus_enabled,
				    TRUE);
      }else{
	gtk_check_button_set_active(raven_synth->chorus_enabled,
				    FALSE);
      }

      g_object_unref(port);
    }
#endif
    
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

      gtk_drop_down_set_selected(raven_synth->chorus_lfo_oscillator,
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

      gtk_spin_button_set_value(raven_synth->chorus_lfo_frequency,
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

      ags_dial_set_value(raven_synth->chorus_depth,
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

      ags_dial_set_value(raven_synth->chorus_mix,
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

      ags_dial_set_value(raven_synth->chorus_delay,
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

      ags_dial_set_value(raven_synth->vibrato_gain,
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

      ags_dial_set_value(raven_synth->vibrato_lfo_depth,
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

      ags_dial_set_value(raven_synth->vibrato_lfo_freq,
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

      ags_dial_set_value(raven_synth->vibrato_tuning,
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

      ags_dial_set_value(raven_synth->tremolo_gain,
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

      ags_dial_set_value(raven_synth->tremolo_lfo_depth,
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

      ags_dial_set_value(raven_synth->tremolo_lfo_freq,
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

      ags_dial_set_value(raven_synth->tremolo_tuning,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }
  }
  
  machine->flags &= (~AGS_MACHINE_NO_UPDATE);
}

/**
 * ags_raven_synth_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsRavenSynth
 *
 * Returns: the new #AgsRavenSynth
 *
 * Since: 7.7.0
 */
AgsRavenSynth*
ags_raven_synth_new(GObject *soundcard)
{
  AgsRavenSynth *raven_synth;

  raven_synth = (AgsRavenSynth *) g_object_new(AGS_TYPE_RAVEN_SYNTH,
					       NULL);

  g_object_set(AGS_MACHINE(raven_synth)->audio,
	       "output-soundcard", soundcard,
	       NULL);

  return(raven_synth);
}
