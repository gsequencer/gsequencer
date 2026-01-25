/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#include <ags/app/machine/ags_modular_synth.h>
#include <ags/app/machine/ags_modular_synth_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_line.h>

#include <math.h>

#include <ags/ags_api_config.h>

#include <ags/i18n.h>

void ags_modular_synth_class_init(AgsModularSynthClass *modular_synth);
void ags_modular_synth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_modular_synth_init(AgsModularSynth *modular_synth);
void ags_modular_synth_finalize(GObject *gobject);

void ags_modular_synth_connect(AgsConnectable *connectable);
void ags_modular_synth_disconnect(AgsConnectable *connectable);

void ags_modular_synth_show(GtkWidget *widget);

void ags_modular_synth_resize_audio_channels(AgsMachine *machine,
					     guint audio_channels, guint audio_channels_old,
					     gpointer data);
void ags_modular_synth_resize_pads(AgsMachine *machine, GType channel_type,
				   guint pads, guint pads_old,
				   gpointer data);

void ags_modular_synth_map_recall(AgsMachine *machine);

void ags_modular_synth_input_map_recall(AgsModularSynth *modular_synth,
					guint audio_channel_start,
					guint input_pad_start);
void ags_modular_synth_output_map_recall(AgsModularSynth *modular_synth,
					 guint audio_channel_start,
					 guint output_pad_start);

void ags_modular_synth_refresh_port(AgsMachine *machine);

/**
 * SECTION:ags_modular_synth
 * @short_description: modular synth
 * @title: AgsModularSynth
 * @section_id:
 * @include: ags/app/machine/ags_modular_synth.h
 *
 * The #AgsModularSynth is a composite widget to act as modular synth.
 */

static gpointer ags_modular_synth_parent_class = NULL;
static AgsConnectableInterface *ags_modular_synth_parent_connectable_interface;

GType
ags_modular_synth_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_modular_synth = 0;

    static const GTypeInfo ags_modular_synth_info = {
      sizeof(AgsModularSynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_modular_synth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsModularSynth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_modular_synth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_modular_synth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_modular_synth = g_type_register_static(AGS_TYPE_MACHINE,
						    "AgsModularSynth", &ags_modular_synth_info,
						    0);
    
    g_type_add_interface_static(ags_type_modular_synth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_modular_synth);
  }

  return(g_define_type_id__static);
}

void
ags_modular_synth_class_init(AgsModularSynthClass *modular_synth)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_modular_synth_parent_class = g_type_class_peek_parent(modular_synth);

  /* GObjectClass */
  gobject = (GObjectClass *) modular_synth;

  gobject->finalize = ags_modular_synth_finalize;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) modular_synth;

  machine->map_recall = ags_modular_synth_map_recall;

  machine->refresh_port = ags_modular_synth_refresh_port;
}

void
ags_modular_synth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_modular_synth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_modular_synth_connect;
  connectable->disconnect = ags_modular_synth_disconnect;
}

void
ags_modular_synth_init(AgsModularSynth *modular_synth)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  GtkBox *vbox;
  GtkBox *hbox;
  GtkGrid *modular_synth_grid;
  GtkGrid *env_0_grid;  
  GtkGrid *env_1_grid;  
  GtkGrid *lfo_0_grid;  
  GtkGrid *lfo_1_grid;  
  GtkGrid *noise_grid;  
  GtkGrid *modulation_grid;  
  GtkGrid *osc_0_grid;  
  GtkGrid *osc_1_grid;  
  GtkGrid *effect_grid;  
  GtkGrid *low_pass_grid;  
  GtkGrid *amplifier_grid;  
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
								     AGS_TYPE_MODULAR_SYNTH);

  machine_name = NULL;

  if(machine_counter != NULL){
    machine_name = g_strdup_printf("Default %d",
				   machine_counter->counter);
  
    ags_machine_counter_increment(machine_counter);
  }
  
  g_object_set(modular_synth,
	       "machine-name", machine_name,
	       NULL);

  g_free(machine_name);

  /* machine selector */
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  position = g_list_length(window->machine);
  
  ags_machine_selector_popup_insert_machine(composite_editor->machine_selector,
					    position,
					    (AgsMachine *) modular_synth);

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
  
  audio = AGS_MACHINE(modular_synth)->audio;
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

  AGS_MACHINE(modular_synth)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
					AGS_MACHINE_REVERSE_NOTATION);
  AGS_MACHINE(modular_synth)->mapping_flags |= AGS_MACHINE_MONO;

  AGS_MACHINE(modular_synth)->input_pad_type = G_TYPE_NONE;
  AGS_MACHINE(modular_synth)->input_line_type = G_TYPE_NONE;
  AGS_MACHINE(modular_synth)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(modular_synth)->output_line_type = G_TYPE_NONE;

  /* audio resize */
  g_signal_connect_after(G_OBJECT(modular_synth), "resize-audio-channels",
			 G_CALLBACK(ags_modular_synth_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(modular_synth), "resize-pads",
			 G_CALLBACK(ags_modular_synth_resize_pads), NULL);
  
  /* create widgets */
  modular_synth->flags = 0;

  /* mapped IO */
  modular_synth->mapped_input_pad = 0;
  modular_synth->mapped_output_pad = 0;

  modular_synth->playback_play_container = ags_recall_container_new();
  modular_synth->playback_recall_container = ags_recall_container_new();

  modular_synth->modular_synth_play_container = ags_recall_container_new();
  modular_synth->modular_synth_recall_container = ags_recall_container_new();

  modular_synth->volume_play_container = ags_recall_container_new();
  modular_synth->volume_recall_container = ags_recall_container_new();

  modular_synth->envelope_play_container = ags_recall_container_new();
  modular_synth->envelope_recall_container = ags_recall_container_new();

  modular_synth->buffer_play_container = ags_recall_container_new();
  modular_synth->buffer_recall_container = ags_recall_container_new();
   
  /* name and xml type */
  modular_synth->name = NULL;
  modular_synth->xml_type = "ags-modular-synth"; 

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

  gtk_frame_set_child(AGS_MACHINE(modular_synth)->frame,
		      (GtkWidget *) vbox);

  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);

  gtk_box_set_spacing(hbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  /* modular synth */
  modular_synth_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(modular_synth_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(modular_synth_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_box_append(hbox,
		 (GtkWidget *) modular_synth_grid);

  /* env-0 */
  env_0_grid = (GtkGrid *) gtk_grid_new();
  gtk_widget_set_halign((GtkWidget *) env_0_grid,
			GTK_ALIGN_START);
  gtk_grid_attach(modular_synth_grid,
		  (GtkWidget *) env_0_grid,
		  0, 0,
		  1, 1);

  /* attack */
  label = (GtkLabel *) gtk_label_new(i18n("env-0 - attack"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  modular_synth->env_0_attack = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->env_0_attack);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   1.0);
  ags_dial_set_radius(modular_synth->env_0_attack,
		      12);
  
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) modular_synth->env_0_attack,
		  1, 0,
		  1, 1);

  /* decay */
  label = (GtkLabel *) gtk_label_new(i18n("env-0 - decay"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  modular_synth->env_0_decay = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->env_0_decay);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   1.0);
  ags_dial_set_radius(modular_synth->env_0_decay,
		      12);
  
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) modular_synth->env_0_decay,
		  1, 1,
		  1, 1);

  /* sustain */
  label = (GtkLabel *) gtk_label_new(i18n("env-0 - sustain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  modular_synth->env_0_sustain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->env_0_sustain);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   1.0);
  ags_dial_set_radius(modular_synth->env_0_sustain,
		      12);
  
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) modular_synth->env_0_sustain,
		  1, 2,
		  1, 1);

  /* release */
  label = (GtkLabel *) gtk_label_new(i18n("env-0 - release"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  modular_synth->env_0_release = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->env_0_release);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   1.0);
  ags_dial_set_radius(modular_synth->env_0_release,
		      12);
  
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) modular_synth->env_0_release,
		  1, 3,
		  1, 1);

  /* gain */
  label = (GtkLabel *) gtk_label_new(i18n("env-0 - gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);
  
  modular_synth->env_0_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->env_0_gain);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   1.0);
  ags_dial_set_radius(modular_synth->env_0_gain,
		      12);
  
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) modular_synth->env_0_gain,
		  1, 4,
		  1, 1);

  /* frequency */
  label = (GtkLabel *) gtk_label_new(i18n("env-0 - frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) label,
		  0, 5,
		  1, 1);
  
  modular_synth->env_0_frequency = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->env_0_frequency);

  gtk_adjustment_set_lower(adjustment,
			   0.01);
  gtk_adjustment_set_upper(adjustment,
			   16.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   6.0);
  ags_dial_set_radius(modular_synth->env_0_frequency,
		      12);
  
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) modular_synth->env_0_frequency,
		  1, 5,
		  1, 1);

  /* env-1 */
  env_1_grid = (GtkGrid *) gtk_grid_new();
  gtk_widget_set_halign((GtkWidget *) env_1_grid,
			GTK_ALIGN_START);
  gtk_grid_attach(modular_synth_grid,
		  (GtkWidget *) env_1_grid,
		  1, 0,
		  1, 1);

  /* attack */
  label = (GtkLabel *) gtk_label_new(i18n("env-1 - attack"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  modular_synth->env_1_attack = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->env_1_attack);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   1.0);
  ags_dial_set_radius(modular_synth->env_1_attack,
		      12);
  
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) modular_synth->env_1_attack,
		  1, 0,
		  1, 1);

  /* decay */
  label = (GtkLabel *) gtk_label_new(i18n("env-1 - decay"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  modular_synth->env_1_decay = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->env_1_decay);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   1.0);
  ags_dial_set_radius(modular_synth->env_1_decay,
		      12);
  
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) modular_synth->env_1_decay,
		  1, 1,
		  1, 1);

  /* sustain */
  label = (GtkLabel *) gtk_label_new(i18n("env-1 - sustain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  modular_synth->env_1_sustain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->env_1_sustain);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   1.0);
  ags_dial_set_radius(modular_synth->env_1_sustain,
		      12);
  
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) modular_synth->env_1_sustain,
		  1, 2,
		  1, 1);

  /* release */
  label = (GtkLabel *) gtk_label_new(i18n("env-1 - release"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  modular_synth->env_1_release = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->env_1_release);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   1.0);
  ags_dial_set_radius(modular_synth->env_1_release,
		      12);
  
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) modular_synth->env_1_release,
		  1, 3,
		  1, 1);

  /* gain */
  label = (GtkLabel *) gtk_label_new(i18n("env-1 - gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);
  
  modular_synth->env_1_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->env_1_gain);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   1.0);
  ags_dial_set_radius(modular_synth->env_1_gain,
		      12);
  
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) modular_synth->env_1_gain,
		  1, 4,
		  1, 1);

  /* frequency */
  label = (GtkLabel *) gtk_label_new(i18n("env-1 - frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) label,
		  0, 5,
		  1, 1);
  
  modular_synth->env_1_frequency = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->env_1_frequency);

  gtk_adjustment_set_lower(adjustment,
			   0.01);
  gtk_adjustment_set_upper(adjustment,
			   16.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   6.0);
  ags_dial_set_radius(modular_synth->env_1_frequency,
		      12);
  
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) modular_synth->env_1_frequency,
		  1, 5,
		  1, 1);

  /* lfo-0 */
  lfo_0_grid = (GtkGrid *) gtk_grid_new();
  gtk_widget_set_halign((GtkWidget *) lfo_0_grid,
			GTK_ALIGN_START);
  gtk_grid_attach(modular_synth_grid,
		  (GtkWidget *) lfo_0_grid,
		  0, 1,
		  1, 1);

  /* oscillator */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-0 - oscillator"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_0_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  modular_synth->lfo_0_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(modular_synth->lfo_0_oscillator,
			     0);

  gtk_grid_attach(lfo_0_grid,
		  (GtkWidget *) modular_synth->lfo_0_oscillator,
		  1, 0,
		  1, 1);

  /* frequency */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-0 - frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_0_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  modular_synth->lfo_0_frequency = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->lfo_0_frequency);

  gtk_adjustment_set_lower(adjustment,
			   0.01);
  gtk_adjustment_set_upper(adjustment,
			   16.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   6.0);
  ags_dial_set_radius(modular_synth->lfo_0_frequency,
		      12);
  
  gtk_grid_attach(lfo_0_grid,
		  (GtkWidget *) modular_synth->lfo_0_frequency,
		  1, 1,
		  1, 1);

  /* depth */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-0 - depth"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_0_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  modular_synth->lfo_0_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->lfo_0_depth);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   1.0);
  ags_dial_set_radius(modular_synth->lfo_0_depth,
		      12);
  
  gtk_grid_attach(lfo_0_grid,
		  (GtkWidget *) modular_synth->lfo_0_depth,
		  1, 2,
		  1, 1);

  /* tuning */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-0 - tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_0_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  modular_synth->lfo_0_tuning = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->lfo_0_tuning);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(modular_synth->lfo_0_tuning,
		      12);
  
  gtk_grid_attach(lfo_0_grid,
		  (GtkWidget *) modular_synth->lfo_0_tuning,
		  1, 3,
		  1, 1);

  /* lfo-1 */
  lfo_1_grid = (GtkGrid *) gtk_grid_new();
  gtk_widget_set_halign((GtkWidget *) lfo_1_grid,
			GTK_ALIGN_START);
  gtk_grid_attach(modular_synth_grid,
		  (GtkWidget *) lfo_1_grid,
		  1, 1,
		  1, 1);

  /* oscillator */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-1 - oscillator"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_1_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  modular_synth->lfo_1_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(modular_synth->lfo_1_oscillator,
			     0);

  gtk_grid_attach(lfo_1_grid,
		  (GtkWidget *) modular_synth->lfo_1_oscillator,
		  1, 0,
		  1, 1);

  /* frequency */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-1 - frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_1_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  modular_synth->lfo_1_frequency = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->lfo_1_frequency);

  gtk_adjustment_set_lower(adjustment,
			   0.01);
  gtk_adjustment_set_upper(adjustment,
			   16.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   6.0);
  ags_dial_set_radius(modular_synth->lfo_1_frequency,
		      12);
  
  gtk_grid_attach(lfo_1_grid,
		  (GtkWidget *) modular_synth->lfo_1_frequency,
		  1, 1,
		  1, 1);

  /* depth */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-1 - depth"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_1_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  modular_synth->lfo_1_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->lfo_1_depth);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   1.0);
  ags_dial_set_radius(modular_synth->lfo_1_depth,
		      12);
  
  gtk_grid_attach(lfo_1_grid,
		  (GtkWidget *) modular_synth->lfo_1_depth,
		  1, 2,
		  1, 1);

  /* tuning */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-1 - tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_1_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  modular_synth->lfo_1_tuning = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->lfo_1_tuning);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(modular_synth->lfo_1_tuning,
		      12);
  
  gtk_grid_attach(lfo_1_grid,
		  (GtkWidget *) modular_synth->lfo_1_tuning,
		  1, 3,
		  1, 1);

  /* noise */
  noise_grid = (GtkGrid *) gtk_grid_new();
  gtk_widget_set_halign((GtkWidget *) noise_grid,
			GTK_ALIGN_START);
  gtk_grid_attach(modular_synth_grid,
		  (GtkWidget *) noise_grid,
		  0, 2,
		  2, 1);

  /* frequency */
  label = (GtkLabel *) gtk_label_new(i18n("noise - frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(noise_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  modular_synth->noise_frequency = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->noise_frequency);

  gtk_adjustment_set_lower(adjustment,
			   220.0);
  gtk_adjustment_set_upper(adjustment,
			   1760.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(modular_synth->noise_frequency,
		      12);

  gtk_grid_attach(noise_grid,
		  (GtkWidget *) modular_synth->noise_frequency,
		  1, 0,
		  1, 1);
  
  /* gain */
  label = (GtkLabel *) gtk_label_new(i18n("noise - gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(noise_grid,
		  (GtkWidget *) label,
		  2, 0,
		  1, 1);
  
  modular_synth->noise_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->noise_gain);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(modular_synth->noise_gain,
		      12);

  gtk_grid_attach(noise_grid,
		  (GtkWidget *) modular_synth->noise_gain,
		  3, 0,
		  1, 1);

  /* modulation */
  modulation_grid = (GtkGrid *) gtk_grid_new();
  gtk_widget_set_halign((GtkWidget *) modulation_grid,
			GTK_ALIGN_START);
  gtk_grid_attach(modular_synth_grid,
		  (GtkWidget *) modulation_grid,
		  2, 0,
		  1, 3);

  /* modulation matrix */
  modular_synth->modulation_matrix = ags_modulation_matrix_new();
  gtk_grid_attach(modulation_grid,
		  (GtkWidget *) modular_synth->modulation_matrix,
		  0, 0,
		  1, 1);

  /* osc-0 */
  osc_0_grid = (GtkGrid *) gtk_grid_new();
  gtk_widget_set_halign((GtkWidget *) osc_0_grid,
	 		GTK_ALIGN_START);
  gtk_grid_attach(modular_synth_grid,
		  (GtkWidget *) osc_0_grid,
		  3, 0,
		  1, 1);

  /* oscillator */
  label = (GtkLabel *) gtk_label_new(i18n("osc-0 - oscillator"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  modular_synth->osc_0_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(modular_synth->osc_0_oscillator,
			     0);

  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) modular_synth->osc_0_oscillator,
		  1, 0,
		  1, 1);

  /* octave */
  label = (GtkLabel *) gtk_label_new(i18n("osc-0 - octave"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  modular_synth->osc_0_octave = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->osc_0_octave);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(modular_synth->osc_0_octave,
		      12);

  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) modular_synth->osc_0_octave,
		  1, 1,
		  1, 1);

  /* key */
  label = (GtkLabel *) gtk_label_new(i18n("osc-0 - key"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  modular_synth->osc_0_key = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->osc_0_key);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(modular_synth->osc_0_key,
		      12);

  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) modular_synth->osc_0_key,
		  1, 2,
		  1, 1);

  /* phase */
  label = (GtkLabel *) gtk_label_new(i18n("osc-0 - phase"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  modular_synth->osc_0_phase = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->osc_0_phase);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   2.0 * M_PI);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(modular_synth->osc_0_phase,
		      12);

  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) modular_synth->osc_0_phase,
		  1, 3,
		  1, 1);

  /* volume */
  label = (GtkLabel *) gtk_label_new(i18n("osc-0 - volume"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);
  
  modular_synth->osc_0_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->osc_0_volume);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(modular_synth->osc_0_volume,
		      12);

  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) modular_synth->osc_0_volume,
		  1, 4,
		  1, 4);

  /* osc-1 */
  osc_1_grid = (GtkGrid *) gtk_grid_new();
  gtk_widget_set_halign((GtkWidget *) osc_1_grid,
	 		GTK_ALIGN_START);
  gtk_grid_attach(modular_synth_grid,
		  (GtkWidget *) osc_1_grid,
		  3, 1,
		  1, 1);

  /* oscillator */
  label = (GtkLabel *) gtk_label_new(i18n("osc-1 - oscillator"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  modular_synth->osc_1_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(modular_synth->osc_1_oscillator,
			     0);

  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) modular_synth->osc_1_oscillator,
		  1, 0,
		  1, 1);

  /* octave */
  label = (GtkLabel *) gtk_label_new(i18n("osc-1 - octave"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  modular_synth->osc_1_octave = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->osc_1_octave);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(modular_synth->osc_1_octave,
		      12);

  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) modular_synth->osc_1_octave,
		  1, 1,
		  1, 1);

  /* key */
  label = (GtkLabel *) gtk_label_new(i18n("osc-1 - key"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  modular_synth->osc_1_key = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->osc_1_key);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(modular_synth->osc_1_key,
		      12);

  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) modular_synth->osc_1_key,
		  1, 2,
		  1, 1);

  /* phase */
  label = (GtkLabel *) gtk_label_new(i18n("osc-1 - phase"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  modular_synth->osc_1_phase = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->osc_1_phase);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   2.0 * M_PI);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(modular_synth->osc_1_phase,
		      12);

  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) modular_synth->osc_1_phase,
		  1, 3,
		  1, 1);

  /* volume */
  label = (GtkLabel *) gtk_label_new(i18n("osc-1 - volume"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);
  
  modular_synth->osc_1_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->osc_1_volume);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(modular_synth->osc_1_volume,
		      12);

  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) modular_synth->osc_1_volume,
		  1, 4,
		  1, 1);

  /* effect */
  effect_grid = (GtkGrid *) gtk_grid_new();

  gtk_widget_set_valign((GtkWidget *) effect_grid,
	 		GTK_ALIGN_START);
  gtk_widget_set_halign((GtkWidget *) effect_grid,
	 		GTK_ALIGN_START);

  gtk_widget_set_vexpand((GtkWidget *) effect_grid,
			 FALSE);
  
  gtk_grid_attach(modular_synth_grid,
		  (GtkWidget *) effect_grid,
		  4, 0,
		  1, 1);

  /* pitch tuning */
  label = (GtkLabel *) gtk_label_new(i18n("pitch tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(effect_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  modular_synth->pitch_tuning = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->pitch_tuning);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(modular_synth->pitch_tuning,
		      12);

  gtk_grid_attach(effect_grid,
		  (GtkWidget *) modular_synth->pitch_tuning,
		  1, 0,
		  1, 1);

  /* volume */
  label = (GtkLabel *) gtk_label_new(i18n("volume"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(effect_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  modular_synth->volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->volume);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(modular_synth->volume,
		      12);

  gtk_grid_attach(effect_grid,
		  (GtkWidget *) modular_synth->volume,
		  1, 1,
		  1, 1);

  /* low-pass */
  low_pass_grid = (GtkGrid *) gtk_grid_new();

  gtk_widget_set_valign((GtkWidget *) low_pass_grid,
	 		GTK_ALIGN_START);
  gtk_widget_set_halign((GtkWidget *) low_pass_grid,
	 		GTK_ALIGN_START);

  gtk_widget_set_vexpand((GtkWidget *) low_pass_grid,
			 FALSE);
  
  gtk_grid_attach(modular_synth_grid,
		  (GtkWidget *) low_pass_grid,
		  0, 4,
		  2, 1);

  /* low-pass-0 cut-off frequency */
  label = (GtkLabel *) gtk_label_new(i18n("cut-off frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(low_pass_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  modular_synth->low_pass_0_cut_off_frequency = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->low_pass_0_cut_off_frequency);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   22000.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(modular_synth->low_pass_0_cut_off_frequency,
		      12);

  gtk_grid_attach(low_pass_grid,
		  (GtkWidget *) modular_synth->low_pass_0_cut_off_frequency,
		  1, 0,
		  1, 1);

  /* low-pass-0 filter gain */
  label = (GtkLabel *) gtk_label_new(i18n("filter gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(low_pass_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  modular_synth->low_pass_0_filter_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->low_pass_0_filter_gain);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(modular_synth->low_pass_0_filter_gain,
		      12);

  gtk_grid_attach(low_pass_grid,
		  (GtkWidget *) modular_synth->low_pass_0_filter_gain,
		  1, 1,
		  1, 1);

  /* low-pass-0 no-clip */
  label = (GtkLabel *) gtk_label_new(i18n("no-clip"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(low_pass_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  modular_synth->low_pass_0_no_clip = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->low_pass_0_no_clip);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   22000.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(modular_synth->low_pass_0_no_clip,
		      12);

  gtk_grid_attach(low_pass_grid,
		  (GtkWidget *) modular_synth->low_pass_0_no_clip,
		  1, 2,
		  1, 1);

  /* amplifier */
  amplifier_grid = (GtkGrid *) gtk_grid_new();

  gtk_widget_set_valign((GtkWidget *) amplifier_grid,
	 		GTK_ALIGN_START);
  gtk_widget_set_halign((GtkWidget *) amplifier_grid,
	 		GTK_ALIGN_START);

  gtk_widget_set_vexpand((GtkWidget *) amplifier_grid,
			 FALSE);
  
  gtk_grid_attach(modular_synth_grid,
		  (GtkWidget *) amplifier_grid,
		  1, 4,
		  2, 1);

  /* amplifier-0 amp-0 gain */
  label = (GtkLabel *) gtk_label_new(i18n("amp-0 gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(amplifier_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  modular_synth->amplifier_0_amp_0_gain = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
										-20.0,
										20.0,
										1.0);
  gtk_widget_set_size_request((GtkWidget *) modular_synth->amplifier_0_amp_0_gain,
			      16, 100);

  gtk_range_set_value((GtkRange *) modular_synth->amplifier_0_amp_0_gain,
		      0.0);
  gtk_range_set_inverted((GtkRange *) modular_synth->amplifier_0_amp_0_gain,
			 TRUE);

  gtk_grid_attach(amplifier_grid,
		  (GtkWidget *) modular_synth->amplifier_0_amp_0_gain,
		  1, 0,
		  1, 1);

  /* amplifier-0 amp-1 gain */
  label = (GtkLabel *) gtk_label_new(i18n("amp-1 gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(amplifier_grid,
		  (GtkWidget *) label,
		  2, 0,
		  1, 1);
  
  modular_synth->amplifier_0_amp_1_gain = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
										-20.0,
										20.0,
										1.0);
  gtk_widget_set_size_request((GtkWidget *) modular_synth->amplifier_0_amp_1_gain,
			      16, 100);

  gtk_range_set_value((GtkRange *) modular_synth->amplifier_0_amp_1_gain,
		      0.0);
  gtk_range_set_inverted((GtkRange *) modular_synth->amplifier_0_amp_1_gain,
			 TRUE);
  
  gtk_grid_attach(amplifier_grid,
		  (GtkWidget *) modular_synth->amplifier_0_amp_1_gain,
		  3, 0,
		  1, 1);

  /* amplifier-0 amp-2 gain */
  label = (GtkLabel *) gtk_label_new(i18n("amp-2 gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(amplifier_grid,
		  (GtkWidget *) label,
		  4, 0,
		  1, 1);
  
  modular_synth->amplifier_0_amp_2_gain = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
										-20.0,
										20.0,
										1.0);
  gtk_widget_set_size_request((GtkWidget *) modular_synth->amplifier_0_amp_2_gain,
			      16, 100);

  gtk_range_set_value((GtkRange *) modular_synth->amplifier_0_amp_2_gain,
		      0.0);
  gtk_range_set_inverted((GtkRange *) modular_synth->amplifier_0_amp_2_gain,
			 TRUE);
  
  gtk_grid_attach(amplifier_grid,
		  (GtkWidget *) modular_synth->amplifier_0_amp_2_gain,
		  5, 0,
		  1, 1);

  /* amplifier-0 amp-3 gain */
  label = (GtkLabel *) gtk_label_new(i18n("amp-3 gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(amplifier_grid,
		  (GtkWidget *) label,
		  6, 0,
		  1, 1);
  
  modular_synth->amplifier_0_amp_3_gain = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
										-20.0,
										20.0,
										1.0);
  gtk_widget_set_size_request((GtkWidget *) modular_synth->amplifier_0_amp_3_gain,
			      16, 100);

  gtk_range_set_value((GtkRange *) modular_synth->amplifier_0_amp_3_gain,
		      0.0);
  gtk_range_set_inverted((GtkRange *) modular_synth->amplifier_0_amp_3_gain,
			 TRUE);
  
  gtk_grid_attach(amplifier_grid,
		  (GtkWidget *) modular_synth->amplifier_0_amp_3_gain,
		  7, 0,
		  1, 1);

  /* amplifier-0 filter gain */
  label = (GtkLabel *) gtk_label_new(i18n("filter gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(amplifier_grid,
		  (GtkWidget *) label,
		  8, 0,
		  1, 1);
  
  modular_synth->amplifier_0_filter_gain = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
										 -20.0,
										 20.0,
										 1.0);
  gtk_widget_set_size_request((GtkWidget *) modular_synth->amplifier_0_filter_gain,
			      16, 100);

  gtk_range_set_value((GtkRange *) modular_synth->amplifier_0_filter_gain,
		      0.0);
  gtk_range_set_inverted((GtkRange *) modular_synth->amplifier_0_filter_gain,
			 TRUE);

  gtk_grid_attach(amplifier_grid,
		  (GtkWidget *) modular_synth->amplifier_0_filter_gain,
		  9, 0,
		  1, 1);

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

  gtk_grid_attach(modular_synth_grid,
		  (GtkWidget *) chorus_grid,
		  0, 5,
		  3, 1);
  
  /* chorus input volume */
  label = (GtkLabel *) gtk_label_new(i18n("chorus input volume"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);

  modular_synth->chorus_input_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->chorus_input_volume);

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

  ags_dial_set_radius(modular_synth->chorus_input_volume,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) modular_synth->chorus_input_volume,
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

  modular_synth->chorus_output_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->chorus_output_volume);

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

  ags_dial_set_radius(modular_synth->chorus_output_volume,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) modular_synth->chorus_output_volume,
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
  
  modular_synth->chorus_lfo_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(modular_synth->chorus_lfo_oscillator,
			     0);

  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) modular_synth->chorus_lfo_oscillator,
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

  modular_synth->chorus_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 10.0, 0.01);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) modular_synth->chorus_lfo_frequency,
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

  modular_synth->chorus_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->chorus_depth);

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

  ags_dial_set_radius(modular_synth->chorus_depth,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) modular_synth->chorus_depth,
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

  modular_synth->chorus_mix = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->chorus_mix);

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

  ags_dial_set_radius(modular_synth->chorus_mix,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) modular_synth->chorus_mix,
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

  modular_synth->chorus_delay = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(modular_synth->chorus_delay);

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

  ags_dial_set_radius(modular_synth->chorus_delay,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) modular_synth->chorus_delay,
		  5, 2,
		  1, 1);
}

void
ags_modular_synth_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_modular_synth_parent_class)->finalize(gobject);
}

void
ags_modular_synth_connect(AgsConnectable *connectable)
{
  AgsModularSynth *modular_synth;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  ags_modular_synth_parent_connectable_interface->connect(connectable);
  
  /* AgsModularSynth */
  modular_synth = AGS_MODULAR_SYNTH(connectable);

  /* env-0 */
  g_signal_connect_after(modular_synth->env_0_attack, "value-changed",
			 G_CALLBACK(ags_modular_synth_env_0_attack_callback), modular_synth);

  g_signal_connect_after(modular_synth->env_0_decay, "value-changed",
			 G_CALLBACK(ags_modular_synth_env_0_decay_callback), modular_synth);

  g_signal_connect_after(modular_synth->env_0_sustain, "value-changed",
			 G_CALLBACK(ags_modular_synth_env_0_sustain_callback), modular_synth);

  g_signal_connect_after(modular_synth->env_0_release, "value-changed",
			 G_CALLBACK(ags_modular_synth_env_0_release_callback), modular_synth);

  g_signal_connect_after(modular_synth->env_0_gain, "value-changed",
			 G_CALLBACK(ags_modular_synth_env_0_gain_callback), modular_synth);

  g_signal_connect_after(modular_synth->env_0_frequency, "value-changed",
			 G_CALLBACK(ags_modular_synth_env_0_frequency_callback), modular_synth);

  /* env-1 */
  g_signal_connect_after(modular_synth->env_1_attack, "value-changed",
			 G_CALLBACK(ags_modular_synth_env_1_attack_callback), modular_synth);

  g_signal_connect_after(modular_synth->env_1_decay, "value-changed",
			 G_CALLBACK(ags_modular_synth_env_1_decay_callback), modular_synth);

  g_signal_connect_after(modular_synth->env_1_sustain, "value-changed",
			 G_CALLBACK(ags_modular_synth_env_1_sustain_callback), modular_synth);

  g_signal_connect_after(modular_synth->env_1_release, "value-changed",
			 G_CALLBACK(ags_modular_synth_env_1_release_callback), modular_synth);

  g_signal_connect_after(modular_synth->env_1_gain, "value-changed",
			 G_CALLBACK(ags_modular_synth_env_1_gain_callback), modular_synth);

  g_signal_connect_after(modular_synth->env_1_frequency, "value-changed",
			 G_CALLBACK(ags_modular_synth_env_1_frequency_callback), modular_synth);

  /* LFO-0 */
  g_signal_connect_after(modular_synth->lfo_0_oscillator, "notify::selected",
			 G_CALLBACK(ags_modular_synth_lfo_0_oscillator_callback), modular_synth);
  
  g_signal_connect_after(modular_synth->lfo_0_frequency, "value-changed",
			 G_CALLBACK(ags_modular_synth_lfo_0_frequency_callback), modular_synth);
  
  g_signal_connect_after(modular_synth->lfo_0_depth, "value-changed",
			 G_CALLBACK(ags_modular_synth_lfo_0_depth_callback), modular_synth);
  
  g_signal_connect_after(modular_synth->lfo_0_tuning, "value-changed",
			 G_CALLBACK(ags_modular_synth_lfo_0_tuning_callback), modular_synth);

  /* LFO-1 */
  g_signal_connect_after(modular_synth->lfo_1_oscillator, "notify::selected",
			 G_CALLBACK(ags_modular_synth_lfo_1_oscillator_callback), modular_synth);
  
  g_signal_connect_after(modular_synth->lfo_1_frequency, "value-changed",
			 G_CALLBACK(ags_modular_synth_lfo_1_frequency_callback), modular_synth);
  
  g_signal_connect_after(modular_synth->lfo_1_depth, "value-changed",
			 G_CALLBACK(ags_modular_synth_lfo_1_depth_callback), modular_synth);
  
  g_signal_connect_after(modular_synth->lfo_1_tuning, "value-changed",
			 G_CALLBACK(ags_modular_synth_lfo_1_tuning_callback), modular_synth);

  /* noise */
  g_signal_connect_after(modular_synth->noise_frequency, "value-changed",
			 G_CALLBACK(ags_modular_synth_noise_frequency_callback), modular_synth);

  g_signal_connect_after(modular_synth->noise_gain, "value-changed",
			 G_CALLBACK(ags_modular_synth_noise_gain_callback), modular_synth);

  /* modulation matrix */
  ags_connectable_connect(AGS_CONNECTABLE(modular_synth->modulation_matrix));

  g_signal_connect_after(modular_synth->modulation_matrix, "toggled",
			 G_CALLBACK(ags_modular_synth_modulation_matrix_callback), modular_synth);

  /* osc-0 */
  g_signal_connect_after(modular_synth->osc_0_oscillator, "notify::selected",
			 G_CALLBACK(ags_modular_synth_osc_0_oscillator_callback), modular_synth);
  
  g_signal_connect_after(modular_synth->osc_0_octave, "value-changed",
			 G_CALLBACK(ags_modular_synth_osc_0_octave_callback), modular_synth);
  
  g_signal_connect_after(modular_synth->osc_0_key, "value-changed",
			 G_CALLBACK(ags_modular_synth_osc_0_key_callback), modular_synth);
  
  g_signal_connect_after(modular_synth->osc_0_phase, "value-changed",
			 G_CALLBACK(ags_modular_synth_osc_0_phase_callback), modular_synth);
  
  g_signal_connect_after(modular_synth->osc_0_volume, "value-changed",
			 G_CALLBACK(ags_modular_synth_osc_0_volume_callback), modular_synth);

  /* osc-1 */
  g_signal_connect_after(modular_synth->osc_1_oscillator, "notify::selected",
			 G_CALLBACK(ags_modular_synth_osc_1_oscillator_callback), modular_synth);
  
  g_signal_connect_after(modular_synth->osc_1_octave, "value-changed",
			 G_CALLBACK(ags_modular_synth_osc_1_octave_callback), modular_synth);
  
  g_signal_connect_after(modular_synth->osc_1_key, "value-changed",
			 G_CALLBACK(ags_modular_synth_osc_1_key_callback), modular_synth);
  
  g_signal_connect_after(modular_synth->osc_1_phase, "value-changed",
			 G_CALLBACK(ags_modular_synth_osc_1_phase_callback), modular_synth);
  
  g_signal_connect_after(modular_synth->osc_1_volume, "value-changed",
			 G_CALLBACK(ags_modular_synth_osc_1_volume_callback), modular_synth);

  /* volume */
  g_signal_connect_after(modular_synth->volume, "value-changed",
			 G_CALLBACK(ags_modular_synth_volume_callback), modular_synth);

  /* pitch */
  g_signal_connect_after(modular_synth->pitch_tuning, "value-changed",
			 G_CALLBACK(ags_modular_synth_pitch_tuning_callback), modular_synth);

  /* low-pass 0 */
  g_signal_connect_after(modular_synth->low_pass_0_cut_off_frequency, "value-changed",
			 G_CALLBACK(ags_modular_synth_low_pass_0_cut_off_frequency_callback), modular_synth);

  g_signal_connect_after(modular_synth->low_pass_0_filter_gain, "value-changed",
			 G_CALLBACK(ags_modular_synth_low_pass_0_filter_gain_callback), modular_synth);

  g_signal_connect_after(modular_synth->low_pass_0_no_clip, "value-changed",
			 G_CALLBACK(ags_modular_synth_low_pass_0_no_clip_callback), modular_synth);

  /* amplifier 0 */  
  g_signal_connect_after(modular_synth->amplifier_0_amp_0_gain, "value-changed",
			 G_CALLBACK(ags_modular_synth_amplifier_0_amp_0_gain_callback), modular_synth);

  g_signal_connect_after(modular_synth->amplifier_0_amp_1_gain, "value-changed",
			 G_CALLBACK(ags_modular_synth_amplifier_0_amp_1_gain_callback), modular_synth);

  g_signal_connect_after(modular_synth->amplifier_0_amp_2_gain, "value-changed",
			 G_CALLBACK(ags_modular_synth_amplifier_0_amp_2_gain_callback), modular_synth);

  g_signal_connect_after(modular_synth->amplifier_0_amp_3_gain, "value-changed",
			 G_CALLBACK(ags_modular_synth_amplifier_0_amp_3_gain_callback), modular_synth);

  g_signal_connect_after(modular_synth->amplifier_0_filter_gain, "value-changed",
			 G_CALLBACK(ags_modular_synth_amplifier_0_filter_gain_callback), modular_synth);

  /* chorus */
  g_signal_connect_after(modular_synth->chorus_input_volume, "value-changed",
			 G_CALLBACK(ags_modular_synth_chorus_input_volume_callback), modular_synth);
  
  g_signal_connect_after(modular_synth->chorus_output_volume, "value-changed",
			 G_CALLBACK(ags_modular_synth_chorus_output_volume_callback), modular_synth);
  
  g_signal_connect_after(modular_synth->chorus_lfo_oscillator, "notify::selected",
			 G_CALLBACK(ags_modular_synth_chorus_lfo_oscillator_callback), modular_synth);
  
  g_signal_connect_after(modular_synth->chorus_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_modular_synth_chorus_lfo_frequency_callback), modular_synth);
  
  g_signal_connect_after(modular_synth->chorus_depth, "value-changed",
			 G_CALLBACK(ags_modular_synth_chorus_depth_callback), modular_synth);
  
  g_signal_connect_after(modular_synth->chorus_mix, "value-changed",
			 G_CALLBACK(ags_modular_synth_chorus_mix_callback), modular_synth);
  
  g_signal_connect_after(modular_synth->chorus_delay, "value-changed",
			 G_CALLBACK(ags_modular_synth_chorus_delay_callback), modular_synth);

  g_signal_connect_after(modular_synth->chorus_input_volume, "value-changed",
			 G_CALLBACK(ags_modular_synth_chorus_input_volume_callback), modular_synth);
  
  g_signal_connect_after(modular_synth->chorus_output_volume, "value-changed",
			 G_CALLBACK(ags_modular_synth_chorus_output_volume_callback), modular_synth);
}

void
ags_modular_synth_disconnect(AgsConnectable *connectable)
{
  AgsModularSynth *modular_synth;
  
  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  ags_modular_synth_parent_connectable_interface->disconnect(connectable);

  /* AgsModularSynth */
  modular_synth = AGS_MODULAR_SYNTH(connectable);

  /* env-0 */
  g_object_disconnect(modular_synth->env_0_attack,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_env_0_attack_callback),
		      modular_synth,
		      NULL);

  g_object_disconnect(modular_synth->env_0_decay,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_env_0_decay_callback),
		      modular_synth,
		      NULL);

  g_object_disconnect(modular_synth->env_0_sustain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_env_0_sustain_callback),
		      modular_synth,
		      NULL);

  g_object_disconnect(modular_synth->env_0_release,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_env_0_release_callback),
		      modular_synth,
		      NULL);

  g_object_disconnect(modular_synth->env_0_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_env_0_gain_callback),
		      modular_synth,
		      NULL);

  g_object_disconnect(modular_synth->env_0_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_env_0_frequency_callback),
		      modular_synth,
		      NULL);

  /* env-1 */
  g_object_disconnect(modular_synth->env_1_attack,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_env_1_attack_callback),
		      modular_synth,
		      NULL);

  g_object_disconnect(modular_synth->env_1_decay,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_env_1_decay_callback),
		      modular_synth,
		      NULL);

  g_object_disconnect(modular_synth->env_1_sustain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_env_1_sustain_callback),
		      modular_synth,
		      NULL);

  g_object_disconnect(modular_synth->env_1_release,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_env_1_release_callback),
		      modular_synth,
		      NULL);

  g_object_disconnect(modular_synth->env_1_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_env_1_gain_callback),
		      modular_synth,
		      NULL);

  g_object_disconnect(modular_synth->env_1_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_env_1_frequency_callback),
		      modular_synth,
		      NULL);

  /* LFO-0 */
  g_object_disconnect(modular_synth->lfo_0_oscillator,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_modular_synth_lfo_0_oscillator_callback),
		      modular_synth,
		      NULL);
  
  g_object_disconnect(modular_synth->lfo_0_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_lfo_0_frequency_callback),
		      modular_synth,
		      NULL);
  
  g_object_disconnect(modular_synth->lfo_0_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_lfo_0_depth_callback),
		      modular_synth,
		      NULL);
  
  g_object_disconnect(modular_synth->lfo_0_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_lfo_0_tuning_callback),
		      modular_synth,
		      NULL);

  /* LFO-1 */
  g_object_disconnect(modular_synth->lfo_1_oscillator,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_modular_synth_lfo_1_oscillator_callback),
		      modular_synth,
		      NULL);
  
  g_object_disconnect(modular_synth->lfo_1_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_lfo_1_frequency_callback),
		      modular_synth,
		      NULL);
  
  g_object_disconnect(modular_synth->lfo_1_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_lfo_1_depth_callback),
		      modular_synth,
		      NULL);
  
  g_object_disconnect(modular_synth->lfo_1_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_lfo_1_tuning_callback),
		      modular_synth,
		      NULL);

  /* noise */
  g_object_disconnect(modular_synth->noise_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_noise_frequency_callback),
		      modular_synth,
		      NULL);

  g_object_disconnect(modular_synth->noise_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_noise_gain_callback),
		      modular_synth,
		      NULL);

  /* modulation matrix */
  ags_connectable_disconnect(AGS_CONNECTABLE(modular_synth->modulation_matrix));

  g_object_disconnect(modular_synth->modulation_matrix,
		      "any_signal::toggled",
		      G_CALLBACK(ags_modular_synth_modulation_matrix_callback),
		      modular_synth,
		      NULL);

  /* osc-0 */
  g_object_disconnect(modular_synth->osc_0_oscillator,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_modular_synth_osc_0_oscillator_callback),
		      modular_synth,
		      NULL);
  
  g_object_disconnect(modular_synth->osc_0_octave,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_osc_0_octave_callback),
		      modular_synth,
		      NULL);
  
  g_object_disconnect(modular_synth->osc_0_key,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_osc_0_key_callback),
		      modular_synth,
		      NULL);
  
  g_object_disconnect(modular_synth->osc_0_phase,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_osc_0_phase_callback),
		      modular_synth,
		      NULL);
  
  g_object_disconnect(modular_synth->osc_0_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_osc_0_volume_callback),
		      modular_synth,
		      NULL);

  /* osc-1 */
  g_object_disconnect(modular_synth->osc_1_oscillator,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_modular_synth_osc_1_oscillator_callback),
		      modular_synth,
		      NULL);
  
  g_object_disconnect(modular_synth->osc_1_octave,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_osc_1_octave_callback),
		      modular_synth,
		      NULL);
  
  g_object_disconnect(modular_synth->osc_1_key,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_osc_1_key_callback),
		      modular_synth,
		      NULL);
  
  g_object_disconnect(modular_synth->osc_1_phase,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_osc_1_phase_callback),
		      modular_synth,
		      NULL);
  
  g_object_disconnect(modular_synth->osc_1_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_osc_1_volume_callback),
		      modular_synth,
		      NULL);

  /* volume */
  g_object_disconnect(modular_synth->volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_volume_callback),
		      modular_synth,
		      NULL);

  /* pitch */
  g_object_disconnect(modular_synth->pitch_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_pitch_tuning_callback),
		      modular_synth,
		      NULL);

  /* low-pass 0 */
  g_object_disconnect(modular_synth->low_pass_0_cut_off_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_low_pass_0_cut_off_frequency_callback),
		      modular_synth,
		      NULL);

  g_object_disconnect(modular_synth->low_pass_0_filter_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_low_pass_0_filter_gain_callback),
		      modular_synth,
		      NULL);

  g_object_disconnect(modular_synth->low_pass_0_no_clip,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_low_pass_0_no_clip_callback),
		      modular_synth,
		      NULL);

  /* amplifier 0 */  
  g_object_disconnect(modular_synth->amplifier_0_amp_0_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_amplifier_0_amp_0_gain_callback),
		      modular_synth,
		      NULL);

  g_object_disconnect(modular_synth->amplifier_0_amp_1_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_amplifier_0_amp_1_gain_callback),
		      modular_synth,
		      NULL);

  g_object_disconnect(modular_synth->amplifier_0_amp_2_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_amplifier_0_amp_2_gain_callback),
		      modular_synth,
		      NULL);

  g_object_disconnect(modular_synth->amplifier_0_amp_3_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_amplifier_0_amp_3_gain_callback),
		      modular_synth,
		      NULL);

  g_object_disconnect(modular_synth->amplifier_0_filter_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_amplifier_0_filter_gain_callback),
		      modular_synth,
		      NULL);

  /* chorus */
  g_object_disconnect(modular_synth->chorus_input_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_chorus_input_volume_callback),
		      modular_synth,
		      NULL);
  
  g_object_disconnect(modular_synth->chorus_output_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_chorus_output_volume_callback),
		      modular_synth,
		      NULL);
  
  g_object_disconnect(modular_synth->chorus_lfo_oscillator,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_modular_synth_chorus_lfo_oscillator_callback),
		      modular_synth,
		      NULL);
  
  g_object_disconnect(modular_synth->chorus_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_chorus_lfo_frequency_callback),
		      modular_synth,
		      NULL);
  
  g_object_disconnect(modular_synth->chorus_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_chorus_depth_callback),
		      modular_synth,
		      NULL);
  
  g_object_disconnect(modular_synth->chorus_mix,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_chorus_mix_callback),
		      modular_synth,
		      NULL);
  
  g_object_disconnect(modular_synth->chorus_delay,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_chorus_delay_callback),
		      modular_synth,
		      NULL);

  g_object_disconnect(modular_synth->chorus_input_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_modular_synth_chorus_input_volume_callback),
		      modular_synth,
		      NULL);
  
  g_object_disconnect(modular_synth->chorus_output_volume,
		      "any_signal::value-changed",
			 G_CALLBACK(ags_modular_synth_chorus_output_volume_callback),
		      modular_synth,
		      NULL);
}

void
ags_modular_synth_resize_audio_channels(AgsMachine *machine,
					guint audio_channels, guint audio_channels_old,
					gpointer data)
{
  AgsModularSynth *modular_synth;

  modular_synth = (AgsModularSynth *) machine;

  if(audio_channels > audio_channels_old){    
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_modular_synth_input_map_recall(modular_synth,
					 audio_channels_old,
					 0);
      
      ags_modular_synth_output_map_recall(modular_synth,
					  audio_channels_old,
					  0);
    }
  }
}

void
ags_modular_synth_resize_pads(AgsMachine *machine, GType type,
			      guint pads, guint pads_old,
			      gpointer data)
{
  AgsModularSynth *modular_synth;

  gboolean grow;

  modular_synth = (AgsModularSynth *) machine;
  
  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(type == AGS_TYPE_INPUT){
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	/* depending on destination */
	ags_modular_synth_input_map_recall(modular_synth,
					   0,
					   pads_old);
      }
    }else{
      modular_synth->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_modular_synth_output_map_recall(modular_synth,
					    0,
					    pads_old);
      }
    }else{
      modular_synth->mapped_output_pad = pads;
    }
  }
}

void
ags_modular_synth_map_recall(AgsMachine *machine)
{
  AgsModularSynth *modular_synth;
  
  AgsAudio *audio;

  GList *start_recall, *recall;

  gint position;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  modular_synth = AGS_MODULAR_SYNTH(machine);

  audio = machine->audio;
  
  position = 0;

  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       modular_synth->playback_play_container, modular_synth->playback_recall_container,
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

  /* ags-fx-modular-synth */
  start_recall = ags_fx_factory_create(audio,
				       modular_synth->modular_synth_play_container, modular_synth->modular_synth_recall_container,
				       "ags-fx-modular-synth",
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
				       modular_synth->volume_play_container, modular_synth->volume_recall_container,
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
				       modular_synth->envelope_play_container, modular_synth->envelope_recall_container,
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
				       modular_synth->buffer_play_container, modular_synth->buffer_recall_container,
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
  ags_modular_synth_input_map_recall(modular_synth,
				     0,
				     0);

  /* depending on destination */
  ags_modular_synth_output_map_recall(modular_synth,
				      0,
				      0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_modular_synth_parent_class)->map_recall(machine);  
}

void
ags_modular_synth_input_map_recall(AgsModularSynth *modular_synth,
				   guint audio_channel_start,
				   guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall, *recall;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(modular_synth->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

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
				       modular_synth->playback_play_container, modular_synth->playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-modular-synth */
  start_recall = ags_fx_factory_create(audio,
				       modular_synth->modular_synth_play_container, modular_synth->modular_synth_recall_container,
				       "ags-fx-modular-synth",
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
				       modular_synth->volume_play_container, modular_synth->volume_recall_container,
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
				       modular_synth->envelope_play_container, modular_synth->envelope_recall_container,
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
				       modular_synth->buffer_play_container, modular_synth->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  modular_synth->mapped_input_pad = input_pads;
}

void
ags_modular_synth_output_map_recall(AgsModularSynth *modular_synth,
				    guint audio_channel_start,
				    guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;

  if(modular_synth->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(modular_synth)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);
  
  modular_synth->mapped_output_pad = output_pads;
}

void
ags_modular_synth_refresh_port(AgsMachine *machine)
{
  AgsModularSynth *modular_synth;
  
  GList *start_play, *start_recall, *recall;

  guint nth_sends;
  guint i, j;

  const gchar const * sends_strv[] = {
    "synth-0-env-0-sends",
    "synth-0-env-1-sends",
    "synth-0-lfo-0-sends",
    "synth-0-lfo-1-sends",
    "synth-0-noise-sends",
    NULL
  };
  
  modular_synth = (AgsModularSynth *) machine;
  
  start_play = ags_audio_get_play(machine->audio);
  start_recall = ags_audio_get_recall(machine->audio);

  recall =
    start_recall = g_list_concat(start_play, start_recall);

  machine->flags |= AGS_MACHINE_NO_UPDATE;

  if((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    gint64 sends[AGS_MODULAR_SYNTH_SENDS_COUNT];
    
    /* synth-0 OSC-0 oscillator */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-osc-0-oscillator", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_drop_down_set_selected(modular_synth->osc_0_oscillator,
				 (gint) g_value_get_float(&value));


      g_object_unref(port);
    }

    /* synth-0-osc-0 octave */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-osc-0-octave", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->osc_0_octave,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0-osc-0 key */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-osc-0-key", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->osc_0_key,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0-osc-0 phase */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-osc-0-phase", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->osc_0_phase,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0-osc-0 volume */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-osc-0-volume", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->osc_0_volume,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 OSC-1 oscillator */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-osc-1-oscillator", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_drop_down_set_selected(modular_synth->osc_1_oscillator,
				 (gint) g_value_get_float(&value));


      g_object_unref(port);
    }

    /* synth-0-osc-1 octave */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-osc-1-octave", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->osc_1_octave,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0-osc-1 key */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-osc-1-key", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->osc_1_key,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0-osc-1 phase */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-osc-1-phase", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->osc_1_phase,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0-osc-1 volume */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-osc-1-volume", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->osc_1_volume,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* modulation matrix */    
    for(nth_sends = 0; nth_sends < 5; nth_sends++){
      memset(&(sends[0]), 0, AGS_MODULAR_SYNTH_SENDS_COUNT * sizeof(gint64));

      port = NULL;

      g_object_get(recall->data,
		   sends_strv[nth_sends], &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_POINTER);

	g_value_set_pointer(&value,
			    (gpointer) &(sends));
      
	ags_port_safe_read(port,
			   &value);

	g_object_unref(port);
      }

      for(i = 0, j = 0; i < AGS_MODULAR_SYNTH_SENDS_COUNT; i++){
	gboolean sends_is_enabled;
	gboolean matrix_is_enabled;

	sends_is_enabled = (((1L << i) & (sends[j])) != 0) ? TRUE: FALSE;
	matrix_is_enabled = ags_modulation_matrix_get_enabled(modular_synth->modulation_matrix,
							      i, nth_sends);
						     
	if(sends_is_enabled){
	  if(!matrix_is_enabled){
	    ags_modulation_matrix_set_enabled(modular_synth->modulation_matrix,
					      i, nth_sends,
					      TRUE);

	    ags_modulation_matrix_toggled(modular_synth->modulation_matrix,
					  i, nth_sends);
	  }
	
	  j++;
	}else{
	  if(matrix_is_enabled){
	    ags_modulation_matrix_set_enabled(modular_synth->modulation_matrix,
					      i, nth_sends,
					      FALSE);

	    ags_modulation_matrix_toggled(modular_synth->modulation_matrix,
					  i, nth_sends);
	  }
	}
      }
    }

    gtk_widget_queue_draw((GtkWidget *) modular_synth->modulation_matrix->drawing_area);
    
    /* synth-0 env-0 attack */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-env-0-attack", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->env_0_attack,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 env-0 decay */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-env-0-decay", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->env_0_decay,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 env-0 sustain */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-env-0-sustain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->env_0_sustain,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 env-0 release */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-env-0-release", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->env_0_release,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 env-0 gain */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-env-0-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->env_0_gain,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 env-0 frequency */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-env-0-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->env_0_frequency,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 env-1 attack */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-env-1-attack", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->env_1_attack,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 env-1 decay */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-env-1-decay", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->env_1_decay,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 env-1 sustain */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-env-1-sustain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->env_1_sustain,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 env-1 release */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-env-1-release", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->env_1_release,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 env-1 gain */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-env-1-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->env_1_gain,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 env-1 frequency */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-env-1-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->env_1_frequency,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 lfo-0 oscillator */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-lfo-0-oscillator", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_drop_down_set_selected(modular_synth->lfo_0_oscillator,
				 (gint) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 lfo-0 frequency */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-lfo-0-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->lfo_0_frequency,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 lfo-0 depth */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-lfo-0-depth", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->lfo_0_depth,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 lfo-0 tuning */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-lfo-0-tuning", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->lfo_0_tuning,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 lfo-1 oscillator */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-lfo-1-oscillator", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      gtk_drop_down_set_selected(modular_synth->lfo_1_oscillator,
				 (gint) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 lfo-1 frequency */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-lfo-1-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->lfo_1_frequency,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 lfo-1 depth */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-lfo-1-depth", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->lfo_1_depth,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 lfo-1 tuning */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-lfo-1-tuning", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->lfo_1_tuning,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* noise frequency */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-noise-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->noise_frequency,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* noise gain */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-noise-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->noise_gain,
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

      ags_dial_set_value(modular_synth->volume,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth-0 pitch tuning */
    port = NULL;

    g_object_get(recall->data,
		 "synth-0-pitch-tuning", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(modular_synth->pitch_tuning,
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

      ags_dial_set_value(modular_synth->low_pass_0_cut_off_frequency,
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

      ags_dial_set_value(modular_synth->low_pass_0_filter_gain,
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

      ags_dial_set_value(modular_synth->low_pass_0_no_clip,
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

      gtk_range_set_value((GtkRange *) modular_synth->amplifier_0_amp_0_gain,
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

      gtk_range_set_value((GtkRange *) modular_synth->amplifier_0_amp_1_gain,
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

      gtk_range_set_value((GtkRange *) modular_synth->amplifier_0_amp_2_gain,
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

      gtk_range_set_value((GtkRange *) modular_synth->amplifier_0_amp_3_gain,
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

      gtk_range_set_value((GtkRange *) modular_synth->amplifier_0_filter_gain,
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
	gtk_check_button_set_active(modular_synth->chorus_enabled,
				    TRUE);
      }else{
	gtk_check_button_set_active(modular_synth->chorus_enabled,
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

      gtk_drop_down_set_selected(modular_synth->chorus_lfo_oscillator,
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

      gtk_spin_button_set_value(modular_synth->chorus_lfo_frequency,
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

      ags_dial_set_value(modular_synth->chorus_depth,
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

      ags_dial_set_value(modular_synth->chorus_mix,
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

      ags_dial_set_value(modular_synth->chorus_delay,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }
  }
  
  machine->flags &= (~AGS_MACHINE_NO_UPDATE);
}

/**
 * ags_modular_synth_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsModularSynth
 *
 * Returns: the new #AgsModularSynth
 *
 * Since: 8.2.0
 */
AgsModularSynth*
ags_modular_synth_new(GObject *soundcard)
{
  AgsModularSynth *modular_synth;

  modular_synth = (AgsModularSynth *) g_object_new(AGS_TYPE_MODULAR_SYNTH,
						   NULL);

  g_object_set(AGS_MACHINE(modular_synth)->audio,
	       "output-soundcard", soundcard,
	       NULL);

  return(modular_synth);
}
