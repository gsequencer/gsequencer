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

#include <ags/app/machine/ags_abyss_synth.h>
#include <ags/app/machine/ags_abyss_synth_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_line.h>

#include <math.h>

#include <ags/ags_api_config.h>

#include <ags/i18n.h>

static void ags_abyss_synth_class_init(AgsAbyssSynthClass *abyss_synth);
static void ags_abyss_synth_connectable_interface_init(AgsConnectableInterface *connectable);
static void ags_abyss_synth_init(AgsAbyssSynth *abyss_synth);
static void ags_abyss_synth_finalize(GObject *gobject);

static void ags_abyss_synth_connect(AgsConnectable *connectable);
static void ags_abyss_synth_disconnect(AgsConnectable *connectable);

static void ags_abyss_synth_show(GtkWidget *widget);

static void ags_abyss_synth_resize_audio_channels(AgsMachine *machine,
						  guint audio_channels, guint audio_channels_old,
						  gpointer data);
static void ags_abyss_synth_resize_pads(AgsMachine *machine, GType channel_type,
					guint pads, guint pads_old,
					gpointer data);

static void ags_abyss_synth_map_recall(AgsMachine *machine);

static void ags_abyss_synth_input_map_recall(AgsAbyssSynth *abyss_synth,
					     guint audio_channel_start,
					     guint input_pad_start);
static void ags_abyss_synth_output_map_recall(AgsAbyssSynth *abyss_synth,
					      guint audio_channel_start,
					      guint output_pad_start);

static void ags_abyss_synth_refresh_port(AgsMachine *machine);

/**
 * SECTION:ags_abyss_synth
 * @short_description: abyss synth
 * @title: AgsAbyssSynth
 * @section_id:
 * @include: ags/app/machine/ags_abyss_synth.h
 *
 * The #AgsAbyssSynth is a composite widget to act as abyss synth.
 */

static gpointer ags_abyss_synth_parent_class = NULL;
static AgsConnectableInterface *ags_abyss_synth_parent_connectable_interface;

GType
ags_abyss_synth_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_abyss_synth = 0;

    static const GTypeInfo ags_abyss_synth_info = {
      sizeof(AgsAbyssSynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_abyss_synth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsAbyssSynth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_abyss_synth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_abyss_synth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_abyss_synth = g_type_register_static(AGS_TYPE_MACHINE,
						  "AgsAbyssSynth", &ags_abyss_synth_info,
						  0);
    
    g_type_add_interface_static(ags_type_abyss_synth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_abyss_synth);
  }

  return(g_define_type_id__static);
}

void
ags_abyss_synth_class_init(AgsAbyssSynthClass *abyss_synth)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  AgsMachineClass *machine;

  ags_abyss_synth_parent_class = g_type_class_peek_parent(abyss_synth);

  /* GObjectClass */
  gobject = (GObjectClass *) abyss_synth;

  gobject->finalize = ags_abyss_synth_finalize;

  /* GtkWidget */
  widget = (GtkWidgetClass *) abyss_synth;

  gtk_widget_class_set_css_name(widget,
				"ags-abyss-synth");
  
  /* AgsMachineClass */
  machine = (AgsMachineClass *) abyss_synth;

  machine->map_recall = ags_abyss_synth_map_recall;

  machine->refresh_port = ags_abyss_synth_refresh_port;
}

void
ags_abyss_synth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_abyss_synth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_abyss_synth_connect;
  connectable->disconnect = ags_abyss_synth_disconnect;
}

void
ags_abyss_synth_init(AgsAbyssSynth *abyss_synth)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  GtkBox *vbox;
  GtkBox *hbox;
  GtkGrid *abyss_synth_grid;
  GtkNotebook *sends_notebook;
  GtkBox *env_box;
  GtkGrid *env_0_grid;
  GtkGrid *env_1_grid;
  GtkGrid *env_2_grid;
  GtkGrid *env_3_grid;
  GtkBox *lfo_box;
  GtkGrid *lfo_0_grid;
  GtkGrid *lfo_1_grid;
  GtkGrid *lfo_2_grid;
  GtkGrid *lfo_3_grid;
  GtkBox *seq_box;
  GtkGrid *seq_0_grid;
  GtkGrid *seq_1_grid;
  GtkGrid *seq_2_grid;
  GtkGrid *seq_3_grid;  
  GtkBox *pink_noise_box;
  GtkGrid *pink_noise_0_grid;  
  GtkGrid *pink_noise_1_grid;  
  GtkGrid *modulation_grid;  
  GtkBox *osc_box;
  GtkGrid *osc_0_grid;
  GtkGrid *osc_1_grid;
  GtkGrid *osc_2_grid;
  GtkGrid *osc_3_grid;
  GtkBox *ring_box;
  GtkGrid *ring_0_grid;
  GtkGrid *ring_1_grid;
  GtkGrid *effect_grid;  
  GtkGrid *low_pass_0_grid;  
  GtkGrid *low_pass_1_grid;  
  GtkGrid *amplifier_grid;  
  GtkGrid *chorus_grid;
  GtkLabel *label;

  GtkAdjustment *adjustment;
  
  AgsAudio *audio;

  AgsMachineCounterManager *machine_counter_manager;
  AgsMachineCounter *machine_counter;

  AgsApplicationContext *application_context;   

  GStrvBuilder *strv_builder;

  gchar **label_x, **label_y;
  
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
								     AGS_TYPE_ABYSS_SYNTH);

  machine_name = NULL;

  if(machine_counter != NULL){
    machine_name = g_strdup_printf("Default %d",
				   machine_counter->counter);
  
    ags_machine_counter_increment(machine_counter);
  }
  
  g_object_set(abyss_synth,
	       "machine-name", machine_name,
	       NULL);

  g_free(machine_name);

  /* machine selector */
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  position = g_list_length(window->machine);
  
  ags_machine_selector_popup_insert_machine(composite_editor->machine_selector,
					    position,
					    (AgsMachine *) abyss_synth);

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
  
  audio = AGS_MACHINE(abyss_synth)->audio;
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

  AGS_MACHINE(abyss_synth)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
				      AGS_MACHINE_REVERSE_NOTATION);
  AGS_MACHINE(abyss_synth)->mapping_flags |= AGS_MACHINE_MONO;

  AGS_MACHINE(abyss_synth)->input_pad_type = G_TYPE_NONE;
  AGS_MACHINE(abyss_synth)->input_line_type = G_TYPE_NONE;
  AGS_MACHINE(abyss_synth)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(abyss_synth)->output_line_type = G_TYPE_NONE;

  /* audio resize */
  g_signal_connect_after(G_OBJECT(abyss_synth), "resize-audio-channels",
			 G_CALLBACK(ags_abyss_synth_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(abyss_synth), "resize-pads",
			 G_CALLBACK(ags_abyss_synth_resize_pads), NULL);
  
  /* create widgets */
  abyss_synth->flags = 0;

  /* mapped IO */
  abyss_synth->mapped_input_pad = 0;
  abyss_synth->mapped_output_pad = 0;

  abyss_synth->playback_play_container = ags_recall_container_new();
  abyss_synth->playback_recall_container = ags_recall_container_new();

  abyss_synth->abyss_synth_play_container = ags_recall_container_new();
  abyss_synth->abyss_synth_recall_container = ags_recall_container_new();

  abyss_synth->volume_play_container = ags_recall_container_new();
  abyss_synth->volume_recall_container = ags_recall_container_new();

  abyss_synth->envelope_play_container = ags_recall_container_new();
  abyss_synth->envelope_recall_container = ags_recall_container_new();

  abyss_synth->buffer_play_container = ags_recall_container_new();
  abyss_synth->buffer_recall_container = ags_recall_container_new();
   
  /* name and xml type */
  abyss_synth->name = NULL;
  abyss_synth->xml_type = "ags-abyss-synth"; 

  /* widget */
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_valign((GtkWidget *) vbox,
			GTK_ALIGN_START);  
  gtk_widget_set_halign((GtkWidget *) vbox,
			GTK_ALIGN_START);

  gtk_widget_set_hexpand((GtkWidget *) vbox,
			 FALSE);

  gtk_widget_set_margin_top((GtkWidget *) vbox,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN);
  gtk_widget_set_margin_bottom((GtkWidget *) vbox,
			       AGS_UI_PROVIDER_DEFAULT_MARGIN);

  gtk_widget_set_margin_start((GtkWidget *) vbox,
			      AGS_UI_PROVIDER_DEFAULT_MARGIN);
  gtk_widget_set_margin_end((GtkWidget *) vbox,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN);

  gtk_frame_set_child(AGS_MACHINE(abyss_synth)->frame,
		      (GtkWidget *) vbox);

  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  /* abyss synth */
  abyss_synth_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(abyss_synth_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(abyss_synth_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_box_append(hbox,
		 (GtkWidget *) abyss_synth_grid);

  /* sends notebook */
  sends_notebook = (GtkNotebook *) gtk_notebook_new();
  
  gtk_grid_attach(abyss_synth_grid,
		  (GtkWidget *) sends_notebook,
		  0, 0,
		  2, 1);
    
  /* env box */
  env_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				   AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_notebook_append_page(sends_notebook,
			   (GtkWidget *) env_box,
			   gtk_label_new(i18n("envelope")));
  
  /* env-0 */
  env_0_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(env_0_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(env_0_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) env_0_grid,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) env_0_grid,
			GTK_ALIGN_START);

  gtk_box_append(env_box,
		 (GtkWidget *) env_0_grid);

  /* attack */
  label = (GtkLabel *) gtk_label_new(i18n("env-1 - attack"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  abyss_synth->env_0_attack = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_0_attack);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->env_0_attack,
		      12);
  
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) abyss_synth->env_0_attack,
		  1, 0,
		  1, 1);

  /* decay */
  label = (GtkLabel *) gtk_label_new(i18n("env-1 - decay"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  abyss_synth->env_0_decay = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_0_decay);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->env_0_decay,
		      12);
  
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) abyss_synth->env_0_decay,
		  1, 1,
		  1, 1);

  /* sustain */
  label = (GtkLabel *) gtk_label_new(i18n("env-1 - sustain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  abyss_synth->env_0_sustain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_0_sustain);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->env_0_sustain,
		      12);
  
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) abyss_synth->env_0_sustain,
		  1, 2,
		  1, 1);

  /* release */
  label = (GtkLabel *) gtk_label_new(i18n("env-1 - release"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  abyss_synth->env_0_release = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_0_release);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->env_0_release,
		      12);
  
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) abyss_synth->env_0_release,
		  1, 3,
		  1, 1);

  /* gain */
  label = (GtkLabel *) gtk_label_new(i18n("env-1 - gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);
  
  abyss_synth->env_0_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_0_gain);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->env_0_gain,
		      12);
  
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) abyss_synth->env_0_gain,
		  1, 4,
		  1, 1);

  /* frequency */
  label = (GtkLabel *) gtk_label_new(i18n("env-1 - frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) label,
		  0, 5,
		  1, 1);
  
  abyss_synth->env_0_frequency = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_0_frequency);

  gtk_adjustment_set_lower(adjustment,
			   0.01);
  gtk_adjustment_set_upper(adjustment,
			   16.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   6.0);
  ags_dial_set_radius(abyss_synth->env_0_frequency,
		      12);
  
  gtk_grid_attach(env_0_grid,
		  (GtkWidget *) abyss_synth->env_0_frequency,
		  1, 5,
		  1, 1);

  /* env-1 */
  env_1_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(env_1_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(env_1_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) env_1_grid,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) env_1_grid,
			GTK_ALIGN_START);

  gtk_box_append(env_box,
		 (GtkWidget *) env_1_grid);

  /* attack */
  label = (GtkLabel *) gtk_label_new(i18n("env-2 - attack"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  abyss_synth->env_1_attack = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_1_attack);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->env_1_attack,
		      12);
  
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) abyss_synth->env_1_attack,
		  1, 0,
		  1, 1);

  /* decay */
  label = (GtkLabel *) gtk_label_new(i18n("env-2 - decay"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  abyss_synth->env_1_decay = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_1_decay);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->env_1_decay,
		      12);
  
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) abyss_synth->env_1_decay,
		  1, 1,
		  1, 1);

  /* sustain */
  label = (GtkLabel *) gtk_label_new(i18n("env-2 - sustain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  abyss_synth->env_1_sustain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_1_sustain);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->env_1_sustain,
		      12);
  
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) abyss_synth->env_1_sustain,
		  1, 2,
		  1, 1);

  /* release */
  label = (GtkLabel *) gtk_label_new(i18n("env-2 - release"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  abyss_synth->env_1_release = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_1_release);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->env_1_release,
		      12);
  
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) abyss_synth->env_1_release,
		  1, 3,
		  1, 1);

  /* gain */
  label = (GtkLabel *) gtk_label_new(i18n("env-2 - gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);
  
  abyss_synth->env_1_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_1_gain);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->env_1_gain,
		      12);
  
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) abyss_synth->env_1_gain,
		  1, 4,
		  1, 1);

  /* frequency */
  label = (GtkLabel *) gtk_label_new(i18n("env-2 - frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) label,
		  0, 5,
		  1, 1);
  
  abyss_synth->env_1_frequency = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_1_frequency);

  gtk_adjustment_set_lower(adjustment,
			   0.01);
  gtk_adjustment_set_upper(adjustment,
			   16.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   6.0);
  ags_dial_set_radius(abyss_synth->env_1_frequency,
		      12);
  
  gtk_grid_attach(env_1_grid,
		  (GtkWidget *) abyss_synth->env_1_frequency,
		  1, 5,
		  1, 1);

  /* env-2 */
  env_2_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(env_2_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(env_2_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) env_2_grid,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) env_2_grid,
			GTK_ALIGN_START);

  gtk_box_append(env_box,
		 (GtkWidget *) env_2_grid);

  /* attack */
  label = (GtkLabel *) gtk_label_new(i18n("env-3 - attack"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_2_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  abyss_synth->env_2_attack = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_2_attack);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->env_2_attack,
		      12);
  
  gtk_grid_attach(env_2_grid,
		  (GtkWidget *) abyss_synth->env_2_attack,
		  1, 0,
		  1, 1);

  /* decay */
  label = (GtkLabel *) gtk_label_new(i18n("env-3 - decay"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_2_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  abyss_synth->env_2_decay = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_2_decay);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->env_2_decay,
		      12);
  
  gtk_grid_attach(env_2_grid,
		  (GtkWidget *) abyss_synth->env_2_decay,
		  1, 1,
		  1, 1);

  /* sustain */
  label = (GtkLabel *) gtk_label_new(i18n("env-3 - sustain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_2_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  abyss_synth->env_2_sustain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_2_sustain);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->env_2_sustain,
		      12);
  
  gtk_grid_attach(env_2_grid,
		  (GtkWidget *) abyss_synth->env_2_sustain,
		  1, 2,
		  1, 1);

  /* release */
  label = (GtkLabel *) gtk_label_new(i18n("env-3 - release"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_2_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  abyss_synth->env_2_release = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_2_release);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->env_2_release,
		      12);
  
  gtk_grid_attach(env_2_grid,
		  (GtkWidget *) abyss_synth->env_2_release,
		  1, 3,
		  1, 1);

  /* gain */
  label = (GtkLabel *) gtk_label_new(i18n("env-3 - gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_2_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);
  
  abyss_synth->env_2_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_2_gain);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->env_2_gain,
		      12);
  
  gtk_grid_attach(env_2_grid,
		  (GtkWidget *) abyss_synth->env_2_gain,
		  1, 4,
		  1, 1);

  /* frequency */
  label = (GtkLabel *) gtk_label_new(i18n("env-3 - frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_2_grid,
		  (GtkWidget *) label,
		  0, 5,
		  1, 1);
  
  abyss_synth->env_2_frequency = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_2_frequency);

  gtk_adjustment_set_lower(adjustment,
			   0.01);
  gtk_adjustment_set_upper(adjustment,
			   16.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   6.0);
  ags_dial_set_radius(abyss_synth->env_2_frequency,
		      12);
  
  gtk_grid_attach(env_2_grid,
		  (GtkWidget *) abyss_synth->env_2_frequency,
		  1, 5,
		  1, 1);

  /* env-3 */
  env_3_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(env_3_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(env_3_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) env_3_grid,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) env_3_grid,
			GTK_ALIGN_START);

  gtk_box_append(env_box,
		 (GtkWidget *) env_3_grid);

  /* attack */
  label = (GtkLabel *) gtk_label_new(i18n("env-4 - attack"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_3_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  abyss_synth->env_3_attack = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_3_attack);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->env_3_attack,
		      12);
  
  gtk_grid_attach(env_3_grid,
		  (GtkWidget *) abyss_synth->env_3_attack,
		  1, 0,
		  1, 1);

  /* decay */
  label = (GtkLabel *) gtk_label_new(i18n("env-4 - decay"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_3_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  abyss_synth->env_3_decay = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_3_decay);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->env_3_decay,
		      12);
  
  gtk_grid_attach(env_3_grid,
		  (GtkWidget *) abyss_synth->env_3_decay,
		  1, 1,
		  1, 1);

  /* sustain */
  label = (GtkLabel *) gtk_label_new(i18n("env-4 - sustain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_3_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  abyss_synth->env_3_sustain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_3_sustain);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->env_3_sustain,
		      12);
  
  gtk_grid_attach(env_3_grid,
		  (GtkWidget *) abyss_synth->env_3_sustain,
		  1, 2,
		  1, 1);

  /* release */
  label = (GtkLabel *) gtk_label_new(i18n("env-4 - release"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_3_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  abyss_synth->env_3_release = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_3_release);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->env_3_release,
		      12);
  
  gtk_grid_attach(env_3_grid,
		  (GtkWidget *) abyss_synth->env_3_release,
		  1, 3,
		  1, 1);

  /* gain */
  label = (GtkLabel *) gtk_label_new(i18n("env-4 - gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_3_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);
  
  abyss_synth->env_3_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_3_gain);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->env_3_gain,
		      12);
  
  gtk_grid_attach(env_3_grid,
		  (GtkWidget *) abyss_synth->env_3_gain,
		  1, 4,
		  1, 1);

  /* frequency */
  label = (GtkLabel *) gtk_label_new(i18n("env-4 - frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(env_3_grid,
		  (GtkWidget *) label,
		  0, 5,
		  1, 1);
  
  abyss_synth->env_3_frequency = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->env_3_frequency);

  gtk_adjustment_set_lower(adjustment,
			   0.01);
  gtk_adjustment_set_upper(adjustment,
			   16.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   6.0);
  ags_dial_set_radius(abyss_synth->env_3_frequency,
		      12);
  
  gtk_grid_attach(env_3_grid,
		  (GtkWidget *) abyss_synth->env_3_frequency,
		  1, 5,
		  1, 1);
    
  /* lfo box */
  lfo_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				   AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_notebook_append_page(sends_notebook,
			   (GtkWidget *) lfo_box,
			   gtk_label_new(i18n("LFO")));

  /* lfo-0 */
  lfo_0_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(lfo_0_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(lfo_0_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) lfo_0_grid,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) lfo_0_grid,
			GTK_ALIGN_START);

  gtk_box_append(lfo_box,
		 (GtkWidget *) lfo_0_grid);

  /* oscillator */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-1 - oscillator"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_0_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  abyss_synth->lfo_0_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(abyss_synth->lfo_0_oscillator,
			     0);

  gtk_grid_attach(lfo_0_grid,
		  (GtkWidget *) abyss_synth->lfo_0_oscillator,
		  1, 0,
		  1, 1);

  /* frequency */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-1 - frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_0_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  abyss_synth->lfo_0_frequency = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->lfo_0_frequency);

  gtk_adjustment_set_lower(adjustment,
			   0.01);
  gtk_adjustment_set_upper(adjustment,
			   16.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
  ags_dial_set_radius(abyss_synth->lfo_0_frequency,
		      12);
  
  gtk_grid_attach(lfo_0_grid,
		  (GtkWidget *) abyss_synth->lfo_0_frequency,
		  1, 1,
		  1, 1);

  /* depth */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-1 - depth"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_0_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  abyss_synth->lfo_0_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->lfo_0_depth);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   1.0);
  ags_dial_set_radius(abyss_synth->lfo_0_depth,
		      12);
  
  gtk_grid_attach(lfo_0_grid,
		  (GtkWidget *) abyss_synth->lfo_0_depth,
		  1, 2,
		  1, 1);

  /* tuning */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-1 - tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_0_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  abyss_synth->lfo_0_tuning = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->lfo_0_tuning);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->lfo_0_tuning,
		      12);
  
  gtk_grid_attach(lfo_0_grid,
		  (GtkWidget *) abyss_synth->lfo_0_tuning,
		  1, 3,
		  1, 1);

  /* lfo-1 */
  lfo_1_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(lfo_1_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(lfo_1_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) lfo_1_grid,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) lfo_1_grid,
			GTK_ALIGN_START);

  gtk_box_append(lfo_box,
		 (GtkWidget *) lfo_1_grid);

  /* oscillator */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-2 - oscillator"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_1_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  abyss_synth->lfo_1_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(abyss_synth->lfo_1_oscillator,
			     0);

  gtk_grid_attach(lfo_1_grid,
		  (GtkWidget *) abyss_synth->lfo_1_oscillator,
		  1, 0,
		  1, 1);

  /* frequency */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-2 - frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_1_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  abyss_synth->lfo_1_frequency = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->lfo_1_frequency);

  gtk_adjustment_set_lower(adjustment,
			   0.01);
  gtk_adjustment_set_upper(adjustment,
			   16.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
  ags_dial_set_radius(abyss_synth->lfo_1_frequency,
		      12);
  
  gtk_grid_attach(lfo_1_grid,
		  (GtkWidget *) abyss_synth->lfo_1_frequency,
		  1, 1,
		  1, 1);

  /* depth */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-2 - depth"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_1_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  abyss_synth->lfo_1_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->lfo_1_depth);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   1.0);
  ags_dial_set_radius(abyss_synth->lfo_1_depth,
		      12);
  
  gtk_grid_attach(lfo_1_grid,
		  (GtkWidget *) abyss_synth->lfo_1_depth,
		  1, 2,
		  1, 1);

  /* tuning */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-2 - tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_1_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  abyss_synth->lfo_1_tuning = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->lfo_1_tuning);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->lfo_1_tuning,
		      12);
  
  gtk_grid_attach(lfo_1_grid,
		  (GtkWidget *) abyss_synth->lfo_1_tuning,
		  1, 3,
		  1, 1);

  /* lfo-2 */
  lfo_2_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(lfo_2_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(lfo_2_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) lfo_2_grid,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) lfo_2_grid,
			GTK_ALIGN_START);

  gtk_box_append(lfo_box,
		 (GtkWidget *) lfo_2_grid);

  /* oscillator */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-3 - oscillator"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_2_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  abyss_synth->lfo_2_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(abyss_synth->lfo_2_oscillator,
			     0);

  gtk_grid_attach(lfo_2_grid,
		  (GtkWidget *) abyss_synth->lfo_2_oscillator,
		  1, 0,
		  1, 1);

  /* frequency */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-3 - frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_2_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  abyss_synth->lfo_2_frequency = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->lfo_2_frequency);

  gtk_adjustment_set_lower(adjustment,
			   0.01);
  gtk_adjustment_set_upper(adjustment,
			   16.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
  ags_dial_set_radius(abyss_synth->lfo_2_frequency,
		      12);
  
  gtk_grid_attach(lfo_2_grid,
		  (GtkWidget *) abyss_synth->lfo_2_frequency,
		  1, 1,
		  1, 1);

  /* depth */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-3 - depth"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_2_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  abyss_synth->lfo_2_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->lfo_2_depth);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   1.0);
  ags_dial_set_radius(abyss_synth->lfo_2_depth,
		      12);
  
  gtk_grid_attach(lfo_2_grid,
		  (GtkWidget *) abyss_synth->lfo_2_depth,
		  1, 2,
		  1, 1);

  /* tuning */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-3 - tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_2_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  abyss_synth->lfo_2_tuning = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->lfo_2_tuning);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->lfo_2_tuning,
		      12);
  
  gtk_grid_attach(lfo_2_grid,
		  (GtkWidget *) abyss_synth->lfo_2_tuning,
		  1, 3,
		  1, 1);

  /* lfo-3 */
  lfo_3_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(lfo_3_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(lfo_3_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) lfo_3_grid,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) lfo_3_grid,
			GTK_ALIGN_START);

  gtk_box_append(lfo_box,
		 (GtkWidget *) lfo_3_grid);

  /* oscillator */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-4 - oscillator"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_3_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  abyss_synth->lfo_3_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(abyss_synth->lfo_3_oscillator,
			     0);

  gtk_grid_attach(lfo_3_grid,
		  (GtkWidget *) abyss_synth->lfo_3_oscillator,
		  1, 0,
		  1, 1);

  /* frequency */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-4 - frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_3_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  abyss_synth->lfo_3_frequency = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->lfo_3_frequency);

  gtk_adjustment_set_lower(adjustment,
			   0.01);
  gtk_adjustment_set_upper(adjustment,
			   16.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
  ags_dial_set_radius(abyss_synth->lfo_3_frequency,
		      12);
  
  gtk_grid_attach(lfo_3_grid,
		  (GtkWidget *) abyss_synth->lfo_3_frequency,
		  1, 1,
		  1, 1);

  /* depth */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-4 - depth"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_3_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  abyss_synth->lfo_3_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->lfo_3_depth);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   1.0);
  ags_dial_set_radius(abyss_synth->lfo_3_depth,
		      12);
  
  gtk_grid_attach(lfo_3_grid,
		  (GtkWidget *) abyss_synth->lfo_3_depth,
		  1, 2,
		  1, 1);

  /* tuning */
  label = (GtkLabel *) gtk_label_new(i18n("lfo-4 - tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(lfo_3_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  abyss_synth->lfo_3_tuning = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->lfo_3_tuning);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->lfo_3_tuning,
		      12);
  
  gtk_grid_attach(lfo_3_grid,
		  (GtkWidget *) abyss_synth->lfo_3_tuning,
		  1, 3,
		  1, 1);
    
  /* seq box */
  seq_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				   AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_notebook_append_page(sends_notebook,
			   (GtkWidget *) seq_box,
			   gtk_label_new(i18n("sequencer")));

  /* seq-0 */
  seq_0_grid = (GtkGrid *) gtk_grid_new();
  gtk_widget_set_halign((GtkWidget *) seq_0_grid,
			GTK_ALIGN_START);
  gtk_box_append(seq_box,
		 (GtkWidget *) seq_0_grid);

  /* seq 0 modulation Nr. 0 */
  abyss_synth->seq_0_modulation_0 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_0_modulation_0);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_0_modulation_0,
		      12);
  
  gtk_grid_attach(seq_0_grid,
		  (GtkWidget *) abyss_synth->seq_0_modulation_0,
		  0, 0,
		  1, 1);

  /* seq 0 modulation Nr. 1 */
  abyss_synth->seq_0_modulation_1 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_0_modulation_1);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_0_modulation_1,
		      12);
  
  gtk_grid_attach(seq_0_grid,
		  (GtkWidget *) abyss_synth->seq_0_modulation_1,
		  1, 0,
		  1, 1);

  /* seq 0 modulation Nr. 2 */
  abyss_synth->seq_0_modulation_2 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_0_modulation_2);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_0_modulation_2,
		      12);
  
  gtk_grid_attach(seq_0_grid,
		  (GtkWidget *) abyss_synth->seq_0_modulation_2,
		  2, 0,
		  1, 1);

  /* seq 0 modulation Nr. 3 */
  abyss_synth->seq_0_modulation_3 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_0_modulation_3);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_0_modulation_3,
		      12);
  
  gtk_grid_attach(seq_0_grid,
		  (GtkWidget *) abyss_synth->seq_0_modulation_3,
		  3, 0,
		  1, 1);

  /* seq 0 modulation Nr. 4 */
  abyss_synth->seq_0_modulation_4 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_0_modulation_4);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_0_modulation_4,
		      12);
  
  gtk_grid_attach(seq_0_grid,
		  (GtkWidget *) abyss_synth->seq_0_modulation_4,
		  4, 0,
		  1, 1);

  /* seq 0 modulation Nr. 5 */
  abyss_synth->seq_0_modulation_5 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_0_modulation_5);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_0_modulation_5,
		      12);
  
  gtk_grid_attach(seq_0_grid,
		  (GtkWidget *) abyss_synth->seq_0_modulation_5,
		  5, 0,
		  1, 1);

  /* seq 0 modulation Nr. 6 */
  abyss_synth->seq_0_modulation_6 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_0_modulation_6);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_0_modulation_6,
		      12);
  
  gtk_grid_attach(seq_0_grid,
		  (GtkWidget *) abyss_synth->seq_0_modulation_6,
		  6, 0,
		  1, 1);

  /* seq 0 modulation Nr. 7 */
  abyss_synth->seq_0_modulation_7 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_0_modulation_7);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_0_modulation_7,
		      12);
  
  gtk_grid_attach(seq_0_grid,
		  (GtkWidget *) abyss_synth->seq_0_modulation_7,
		  7, 0,
		  1, 1);

  /* seq 0 modulation Nr. 8 */
  abyss_synth->seq_0_modulation_8 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_0_modulation_8);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_0_modulation_8,
		      12);
  
  gtk_grid_attach(seq_0_grid,
		  (GtkWidget *) abyss_synth->seq_0_modulation_8,
		  8, 0,
		  1, 1);

  /* seq 0 modulation Nr. 9 */
  abyss_synth->seq_0_modulation_9 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_0_modulation_9);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_0_modulation_9,
		      12);
  
  gtk_grid_attach(seq_0_grid,
		  (GtkWidget *) abyss_synth->seq_0_modulation_9,
		  9, 0,
		  1, 1);

  /* seq 0 modulation Nr. 10 */
  abyss_synth->seq_0_modulation_10 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_0_modulation_10);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_0_modulation_10,
		      12);
  
  gtk_grid_attach(seq_0_grid,
		  (GtkWidget *) abyss_synth->seq_0_modulation_10,
		  10, 0,
		  1, 1);

  /* seq 0 modulation Nr. 11 */
  abyss_synth->seq_0_modulation_11 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_0_modulation_11);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_0_modulation_11,
		      12);
  
  gtk_grid_attach(seq_0_grid,
		  (GtkWidget *) abyss_synth->seq_0_modulation_11,
		  11, 0,
		  1, 1);

  /* seq 0 modulation Nr. 12 */
  abyss_synth->seq_0_modulation_12 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_0_modulation_12);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_0_modulation_12,
		      12);
  
  gtk_grid_attach(seq_0_grid,
		  (GtkWidget *) abyss_synth->seq_0_modulation_12,
		  12, 0,
		  1, 1);

  /* seq 0 modulation Nr. 13 */
  abyss_synth->seq_0_modulation_13 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_0_modulation_13);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_0_modulation_13,
		      12);
  
  gtk_grid_attach(seq_0_grid,
		  (GtkWidget *) abyss_synth->seq_0_modulation_13,
		  13, 0,
		  1, 1);

  /* seq 0 modulation Nr. 14 */
  abyss_synth->seq_0_modulation_14 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_0_modulation_14);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_0_modulation_14,
		      12);
  
  gtk_grid_attach(seq_0_grid,
		  (GtkWidget *) abyss_synth->seq_0_modulation_14,
		  14, 0,
		  1, 1);

  /* seq 0 modulation Nr. 15 */
  abyss_synth->seq_0_modulation_15 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_0_modulation_15);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_0_modulation_15,
		      12);
  
  gtk_grid_attach(seq_0_grid,
		  (GtkWidget *) abyss_synth->seq_0_modulation_15,
		  15, 0,
		  1, 1);
  
  /* seq 0 pingpong */
  abyss_synth->seq_0_pingpong = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("Seq 1 - pingpong"));
  gtk_grid_attach(seq_0_grid,
		  (GtkWidget *) abyss_synth->seq_0_pingpong,
		  0, 1,
		  4, 1);
  
  /* seq 0 LFO label */
  label = (GtkLabel *) gtk_label_new(i18n("Seq 1 - LFO"));
  gtk_grid_attach(seq_0_grid,
		  (GtkWidget *) label,
		  4, 1,
		  2, 1);
  
  /* seq 0 LFO frequency */
  abyss_synth->seq_0_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01,
										      16.0,
										      0.01);
  gtk_spin_button_set_value(abyss_synth->seq_0_lfo_frequency,
			    6.0);  
  gtk_grid_attach(seq_0_grid,
		  (GtkWidget *) abyss_synth->seq_0_lfo_frequency,
		  6, 1,
		  2, 1);

  /* seq-1 */
  seq_1_grid = (GtkGrid *) gtk_grid_new();
  gtk_widget_set_halign((GtkWidget *) seq_1_grid,
			GTK_ALIGN_START);
  gtk_box_append(seq_box,
		 (GtkWidget *) seq_1_grid);

  /* seq 1 modulation Nr. 0 */
  abyss_synth->seq_1_modulation_0 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_1_modulation_0);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_1_modulation_0,
		      12);
  
  gtk_grid_attach(seq_1_grid,
		  (GtkWidget *) abyss_synth->seq_1_modulation_0,
		  0, 0,
		  1, 1);

  /* seq 1 modulation Nr. 1 */
  abyss_synth->seq_1_modulation_1 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_1_modulation_1);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_1_modulation_1,
		      12);
  
  gtk_grid_attach(seq_1_grid,
		  (GtkWidget *) abyss_synth->seq_1_modulation_1,
		  1, 0,
		  1, 1);

  /* seq 1 modulation Nr. 2 */
  abyss_synth->seq_1_modulation_2 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_1_modulation_2);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_1_modulation_2,
		      12);
  
  gtk_grid_attach(seq_1_grid,
		  (GtkWidget *) abyss_synth->seq_1_modulation_2,
		  2, 0,
		  1, 1);

  /* seq 1 modulation Nr. 3 */
  abyss_synth->seq_1_modulation_3 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_1_modulation_3);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_1_modulation_3,
		      12);
  
  gtk_grid_attach(seq_1_grid,
		  (GtkWidget *) abyss_synth->seq_1_modulation_3,
		  3, 0,
		  1, 1);

  /* seq 1 modulation Nr. 4 */
  abyss_synth->seq_1_modulation_4 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_1_modulation_4);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_1_modulation_4,
		      12);
  
  gtk_grid_attach(seq_1_grid,
		  (GtkWidget *) abyss_synth->seq_1_modulation_4,
		  4, 0,
		  1, 1);

  /* seq 1 modulation Nr. 5 */
  abyss_synth->seq_1_modulation_5 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_1_modulation_5);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_1_modulation_5,
		      12);
  
  gtk_grid_attach(seq_1_grid,
		  (GtkWidget *) abyss_synth->seq_1_modulation_5,
		  5, 0,
		  1, 1);

  /* seq 1 modulation Nr. 6 */
  abyss_synth->seq_1_modulation_6 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_1_modulation_6);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_1_modulation_6,
		      12);
  
  gtk_grid_attach(seq_1_grid,
		  (GtkWidget *) abyss_synth->seq_1_modulation_6,
		  6, 0,
		  1, 1);

  /* seq 1 modulation Nr. 7 */
  abyss_synth->seq_1_modulation_7 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_1_modulation_7);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_1_modulation_7,
		      12);
  
  gtk_grid_attach(seq_1_grid,
		  (GtkWidget *) abyss_synth->seq_1_modulation_7,
		  7, 0,
		  1, 1);

  /* seq 1 modulation Nr. 8 */
  abyss_synth->seq_1_modulation_8 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_1_modulation_8);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_1_modulation_8,
		      12);
  
  gtk_grid_attach(seq_1_grid,
		  (GtkWidget *) abyss_synth->seq_1_modulation_8,
		  8, 0,
		  1, 1);

  /* seq 1 modulation Nr. 9 */
  abyss_synth->seq_1_modulation_9 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_1_modulation_9);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_1_modulation_9,
		      12);
  
  gtk_grid_attach(seq_1_grid,
		  (GtkWidget *) abyss_synth->seq_1_modulation_9,
		  9, 0,
		  1, 1);

  /* seq 1 modulation Nr. 10 */
  abyss_synth->seq_1_modulation_10 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_1_modulation_10);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_1_modulation_10,
		      12);
  
  gtk_grid_attach(seq_1_grid,
		  (GtkWidget *) abyss_synth->seq_1_modulation_10,
		  10, 0,
		  1, 1);

  /* seq 1 modulation Nr. 11 */
  abyss_synth->seq_1_modulation_11 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_1_modulation_11);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_1_modulation_11,
		      12);
  
  gtk_grid_attach(seq_1_grid,
		  (GtkWidget *) abyss_synth->seq_1_modulation_11,
		  11, 0,
		  1, 1);

  /* seq 1 modulation Nr. 12 */
  abyss_synth->seq_1_modulation_12 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_1_modulation_12);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_1_modulation_12,
		      12);
  
  gtk_grid_attach(seq_1_grid,
		  (GtkWidget *) abyss_synth->seq_1_modulation_12,
		  12, 0,
		  1, 1);

  /* seq 1 modulation Nr. 13 */
  abyss_synth->seq_1_modulation_13 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_1_modulation_13);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_1_modulation_13,
		      12);
  
  gtk_grid_attach(seq_1_grid,
		  (GtkWidget *) abyss_synth->seq_1_modulation_13,
		  13, 0,
		  1, 1);

  /* seq 1 modulation Nr. 14 */
  abyss_synth->seq_1_modulation_14 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_1_modulation_14);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_1_modulation_14,
		      12);
  
  gtk_grid_attach(seq_1_grid,
		  (GtkWidget *) abyss_synth->seq_1_modulation_14,
		  14, 0,
		  1, 1);

  /* seq 1 modulation Nr. 15 */
  abyss_synth->seq_1_modulation_15 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_1_modulation_15);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_1_modulation_15,
		      12);
  
  gtk_grid_attach(seq_1_grid,
		  (GtkWidget *) abyss_synth->seq_1_modulation_15,
		  15, 0,
		  1, 1);
  
  /* seq 1 pingpong */
  abyss_synth->seq_1_pingpong = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("Seq 2 - pingpong"));
  gtk_grid_attach(seq_1_grid,
		  (GtkWidget *) abyss_synth->seq_1_pingpong,
		  0, 1,
		  4, 1);
  
  /* seq 1 LFO label */
  label = (GtkLabel *) gtk_label_new(i18n("Seq 2 - LFO"));
  gtk_grid_attach(seq_1_grid,
		  (GtkWidget *) label,
		  4, 1,
		  2, 1);
  
  /* seq 1 LFO frequency */
  abyss_synth->seq_1_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01,
										      16.0,
										      0.01);
  gtk_spin_button_set_value(abyss_synth->seq_1_lfo_frequency,
			    6.0);  
  gtk_grid_attach(seq_1_grid,
		  (GtkWidget *) abyss_synth->seq_1_lfo_frequency,
		  6, 1,
		  2, 1);

  /* seq-2 */
  seq_2_grid = (GtkGrid *) gtk_grid_new();
  gtk_widget_set_halign((GtkWidget *) seq_2_grid,
			GTK_ALIGN_START);
  gtk_box_append(seq_box,
		 (GtkWidget *) seq_2_grid);

  /* seq 2 modulation Nr. 0 */
  abyss_synth->seq_2_modulation_0 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_2_modulation_0);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_2_modulation_0,
		      12);
  
  gtk_grid_attach(seq_2_grid,
		  (GtkWidget *) abyss_synth->seq_2_modulation_0,
		  0, 0,
		  1, 1);

  /* seq 2 modulation Nr. 1 */
  abyss_synth->seq_2_modulation_1 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_2_modulation_1);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_2_modulation_1,
		      12);
  
  gtk_grid_attach(seq_2_grid,
		  (GtkWidget *) abyss_synth->seq_2_modulation_1,
		  1, 0,
		  1, 1);

  /* seq 2 modulation Nr. 2 */
  abyss_synth->seq_2_modulation_2 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_2_modulation_2);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_2_modulation_2,
		      12);
  
  gtk_grid_attach(seq_2_grid,
		  (GtkWidget *) abyss_synth->seq_2_modulation_2,
		  2, 0,
		  1, 1);

  /* seq 2 modulation Nr. 3 */
  abyss_synth->seq_2_modulation_3 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_2_modulation_3);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_2_modulation_3,
		      12);
  
  gtk_grid_attach(seq_2_grid,
		  (GtkWidget *) abyss_synth->seq_2_modulation_3,
		  3, 0,
		  1, 1);

  /* seq 2 modulation Nr. 4 */
  abyss_synth->seq_2_modulation_4 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_2_modulation_4);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_2_modulation_4,
		      12);
  
  gtk_grid_attach(seq_2_grid,
		  (GtkWidget *) abyss_synth->seq_2_modulation_4,
		  4, 0,
		  1, 1);

  /* seq 2 modulation Nr. 5 */
  abyss_synth->seq_2_modulation_5 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_2_modulation_5);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_2_modulation_5,
		      12);
  
  gtk_grid_attach(seq_2_grid,
		  (GtkWidget *) abyss_synth->seq_2_modulation_5,
		  5, 0,
		  1, 1);

  /* seq 2 modulation Nr. 6 */
  abyss_synth->seq_2_modulation_6 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_2_modulation_6);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_2_modulation_6,
		      12);
  
  gtk_grid_attach(seq_2_grid,
		  (GtkWidget *) abyss_synth->seq_2_modulation_6,
		  6, 0,
		  1, 1);

  /* seq 2 modulation Nr. 7 */
  abyss_synth->seq_2_modulation_7 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_2_modulation_7);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_2_modulation_7,
		      12);
  
  gtk_grid_attach(seq_2_grid,
		  (GtkWidget *) abyss_synth->seq_2_modulation_7,
		  7, 0,
		  1, 1);

  /* seq 2 modulation Nr. 8 */
  abyss_synth->seq_2_modulation_8 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_2_modulation_8);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_2_modulation_8,
		      12);
  
  gtk_grid_attach(seq_2_grid,
		  (GtkWidget *) abyss_synth->seq_2_modulation_8,
		  8, 0,
		  1, 1);

  /* seq 2 modulation Nr. 9 */
  abyss_synth->seq_2_modulation_9 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_2_modulation_9);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_2_modulation_9,
		      12);
  
  gtk_grid_attach(seq_2_grid,
		  (GtkWidget *) abyss_synth->seq_2_modulation_9,
		  9, 0,
		  1, 1);

  /* seq 2 modulation Nr. 10 */
  abyss_synth->seq_2_modulation_10 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_2_modulation_10);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_2_modulation_10,
		      12);
  
  gtk_grid_attach(seq_2_grid,
		  (GtkWidget *) abyss_synth->seq_2_modulation_10,
		  10, 0,
		  1, 1);

  /* seq 2 modulation Nr. 11 */
  abyss_synth->seq_2_modulation_11 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_2_modulation_11);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_2_modulation_11,
		      12);
  
  gtk_grid_attach(seq_2_grid,
		  (GtkWidget *) abyss_synth->seq_2_modulation_11,
		  11, 0,
		  1, 1);

  /* seq 2 modulation Nr. 12 */
  abyss_synth->seq_2_modulation_12 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_2_modulation_12);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_2_modulation_12,
		      12);
  
  gtk_grid_attach(seq_2_grid,
		  (GtkWidget *) abyss_synth->seq_2_modulation_12,
		  12, 0,
		  1, 1);

  /* seq 2 modulation Nr. 13 */
  abyss_synth->seq_2_modulation_13 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_2_modulation_13);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_2_modulation_13,
		      12);
  
  gtk_grid_attach(seq_2_grid,
		  (GtkWidget *) abyss_synth->seq_2_modulation_13,
		  13, 0,
		  1, 1);

  /* seq 2 modulation Nr. 14 */
  abyss_synth->seq_2_modulation_14 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_2_modulation_14);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_2_modulation_14,
		      12);
  
  gtk_grid_attach(seq_2_grid,
		  (GtkWidget *) abyss_synth->seq_2_modulation_14,
		  14, 0,
		  1, 1);

  /* seq 2 modulation Nr. 15 */
  abyss_synth->seq_2_modulation_15 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_2_modulation_15);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_2_modulation_15,
		      12);
  
  gtk_grid_attach(seq_2_grid,
		  (GtkWidget *) abyss_synth->seq_2_modulation_15,
		  15, 0,
		  1, 1);
  
  /* seq 2 pingpong */
  abyss_synth->seq_2_pingpong = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("Seq 3 - pingpong"));
  gtk_grid_attach(seq_2_grid,
		  (GtkWidget *) abyss_synth->seq_2_pingpong,
		  0, 1,
		  4, 1);
  
  /* seq 2 LFO label */
  label = (GtkLabel *) gtk_label_new(i18n("Seq 3 - LFO"));
  gtk_grid_attach(seq_2_grid,
		  (GtkWidget *) label,
		  4, 1,
		  2, 1);
  
  /* seq 2 LFO frequency */
  abyss_synth->seq_2_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01,
										      16.0,
										      0.01);
  gtk_spin_button_set_value(abyss_synth->seq_2_lfo_frequency,
			    6.0);  
  gtk_grid_attach(seq_2_grid,
		  (GtkWidget *) abyss_synth->seq_2_lfo_frequency,
		  6, 1,
		  2, 1);

  /* seq-3 */
  seq_3_grid = (GtkGrid *) gtk_grid_new();
  gtk_widget_set_halign((GtkWidget *) seq_3_grid,
			GTK_ALIGN_START);
  gtk_box_append(seq_box,
		 (GtkWidget *) seq_3_grid);

  /* seq 3 modulation Nr. 0 */
  abyss_synth->seq_3_modulation_0 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_3_modulation_0);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_3_modulation_0,
		      12);
  
  gtk_grid_attach(seq_3_grid,
		  (GtkWidget *) abyss_synth->seq_3_modulation_0,
		  0, 0,
		  1, 1);

  /* seq 3 modulation Nr. 1 */
  abyss_synth->seq_3_modulation_1 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_3_modulation_1);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_3_modulation_1,
		      12);
  
  gtk_grid_attach(seq_3_grid,
		  (GtkWidget *) abyss_synth->seq_3_modulation_1,
		  1, 0,
		  1, 1);

  /* seq 3 modulation Nr. 2 */
  abyss_synth->seq_3_modulation_2 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_3_modulation_2);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_3_modulation_2,
		      12);
  
  gtk_grid_attach(seq_3_grid,
		  (GtkWidget *) abyss_synth->seq_3_modulation_2,
		  2, 0,
		  1, 1);

  /* seq 3 modulation Nr. 3 */
  abyss_synth->seq_3_modulation_3 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_3_modulation_3);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_3_modulation_3,
		      12);
  
  gtk_grid_attach(seq_3_grid,
		  (GtkWidget *) abyss_synth->seq_3_modulation_3,
		  3, 0,
		  1, 1);

  /* seq 3 modulation Nr. 4 */
  abyss_synth->seq_3_modulation_4 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_3_modulation_4);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_3_modulation_4,
		      12);
  
  gtk_grid_attach(seq_3_grid,
		  (GtkWidget *) abyss_synth->seq_3_modulation_4,
		  4, 0,
		  1, 1);

  /* seq 3 modulation Nr. 5 */
  abyss_synth->seq_3_modulation_5 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_3_modulation_5);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_3_modulation_5,
		      12);
  
  gtk_grid_attach(seq_3_grid,
		  (GtkWidget *) abyss_synth->seq_3_modulation_5,
		  5, 0,
		  1, 1);

  /* seq 3 modulation Nr. 6 */
  abyss_synth->seq_3_modulation_6 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_3_modulation_6);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_3_modulation_6,
		      12);
  
  gtk_grid_attach(seq_3_grid,
		  (GtkWidget *) abyss_synth->seq_3_modulation_6,
		  6, 0,
		  1, 1);

  /* seq 3 modulation Nr. 7 */
  abyss_synth->seq_3_modulation_7 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_3_modulation_7);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_3_modulation_7,
		      12);
  
  gtk_grid_attach(seq_3_grid,
		  (GtkWidget *) abyss_synth->seq_3_modulation_7,
		  7, 0,
		  1, 1);

  /* seq 3 modulation Nr. 8 */
  abyss_synth->seq_3_modulation_8 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_3_modulation_8);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_3_modulation_8,
		      12);
  
  gtk_grid_attach(seq_3_grid,
		  (GtkWidget *) abyss_synth->seq_3_modulation_8,
		  8, 0,
		  1, 1);

  /* seq 3 modulation Nr. 9 */
  abyss_synth->seq_3_modulation_9 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_3_modulation_9);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_3_modulation_9,
		      12);
  
  gtk_grid_attach(seq_3_grid,
		  (GtkWidget *) abyss_synth->seq_3_modulation_9,
		  9, 0,
		  1, 1);

  /* seq 3 modulation Nr. 10 */
  abyss_synth->seq_3_modulation_10 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_3_modulation_10);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_3_modulation_10,
		      12);
  
  gtk_grid_attach(seq_3_grid,
		  (GtkWidget *) abyss_synth->seq_3_modulation_10,
		  10, 0,
		  1, 1);

  /* seq 3 modulation Nr. 11 */
  abyss_synth->seq_3_modulation_11 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_3_modulation_11);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_3_modulation_11,
		      12);
  
  gtk_grid_attach(seq_3_grid,
		  (GtkWidget *) abyss_synth->seq_3_modulation_11,
		  11, 0,
		  1, 1);

  /* seq 3 modulation Nr. 12 */
  abyss_synth->seq_3_modulation_12 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_3_modulation_12);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_3_modulation_12,
		      12);
  
  gtk_grid_attach(seq_3_grid,
		  (GtkWidget *) abyss_synth->seq_3_modulation_12,
		  12, 0,
		  1, 1);

  /* seq 3 modulation Nr. 13 */
  abyss_synth->seq_3_modulation_13 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_3_modulation_13);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_3_modulation_13,
		      12);
  
  gtk_grid_attach(seq_3_grid,
		  (GtkWidget *) abyss_synth->seq_3_modulation_13,
		  13, 0,
		  1, 1);

  /* seq 3 modulation Nr. 14 */
  abyss_synth->seq_3_modulation_14 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_3_modulation_14);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_3_modulation_14,
		      12);
  
  gtk_grid_attach(seq_3_grid,
		  (GtkWidget *) abyss_synth->seq_3_modulation_14,
		  14, 0,
		  1, 1);

  /* seq 3 modulation Nr. 15 */
  abyss_synth->seq_3_modulation_15 = (AgsDial *) ags_dial_new();
  
  adjustment = ags_dial_get_adjustment(abyss_synth->seq_3_modulation_15);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   1.0);

  ags_dial_set_radius(abyss_synth->seq_3_modulation_15,
		      12);
  
  gtk_grid_attach(seq_3_grid,
		  (GtkWidget *) abyss_synth->seq_3_modulation_15,
		  15, 0,
		  1, 1);
  
  /* seq 3 pingpong */
  abyss_synth->seq_3_pingpong = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("Seq 4 - pingpong"));
  gtk_grid_attach(seq_3_grid,
		  (GtkWidget *) abyss_synth->seq_3_pingpong,
		  0, 1,
		  4, 1);
  
  /* seq 3 LFO label */
  label = (GtkLabel *) gtk_label_new(i18n("Seq 4 - LFO"));
  gtk_grid_attach(seq_3_grid,
		  (GtkWidget *) label,
		  4, 1,
		  2, 1);
  
  /* seq 3 LFO frequency */
  abyss_synth->seq_3_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01,
										      16.0,
										      0.01);
  gtk_spin_button_set_value(abyss_synth->seq_3_lfo_frequency,
			    6.0);  
  gtk_grid_attach(seq_3_grid,
		  (GtkWidget *) abyss_synth->seq_3_lfo_frequency,
		  6, 1,
		  2, 1);
  
  /* pink noise box */
  pink_noise_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
					  AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_notebook_append_page(sends_notebook,
			   (GtkWidget *) pink_noise_box,
			   gtk_label_new(i18n("pink noise")));
  
  /* pink noise 0 */
  pink_noise_0_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(pink_noise_0_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(pink_noise_0_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) pink_noise_0_grid,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) pink_noise_0_grid,
			GTK_ALIGN_START);

  gtk_box_append(pink_noise_box,
		 (GtkWidget *) pink_noise_0_grid);

  /* frequency */
  label = (GtkLabel *) gtk_label_new(i18n("noise 1 - frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(pink_noise_0_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  abyss_synth->pink_noise_0_frequency = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->pink_noise_0_frequency);

  gtk_adjustment_set_lower(adjustment,
			   220.0);
  gtk_adjustment_set_upper(adjustment,
			   1760.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   220.0);
  ags_dial_set_radius(abyss_synth->pink_noise_0_frequency,
		      12);

  gtk_grid_attach(pink_noise_0_grid,
		  (GtkWidget *) abyss_synth->pink_noise_0_frequency,
		  1, 0,
		  1, 1);
  
  /* gain */
  label = (GtkLabel *) gtk_label_new(i18n("noise 1 - gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(pink_noise_0_grid,
		  (GtkWidget *) label,
		  2, 0,
		  1, 1);
  
  abyss_synth->pink_noise_0_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->pink_noise_0_gain);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->pink_noise_0_gain,
		      12);

  gtk_grid_attach(pink_noise_0_grid,
		  (GtkWidget *) abyss_synth->pink_noise_0_gain,
		  3, 0,
		  1, 1);

  /* pink noise 1 */
  pink_noise_1_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(pink_noise_1_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(pink_noise_1_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) pink_noise_1_grid,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) pink_noise_1_grid,
			GTK_ALIGN_START);

  gtk_box_append(pink_noise_box,
		 (GtkWidget *) pink_noise_1_grid);

  /* frequency */
  label = (GtkLabel *) gtk_label_new(i18n("noise 2 - frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(pink_noise_1_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  abyss_synth->pink_noise_1_frequency = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->pink_noise_1_frequency);

  gtk_adjustment_set_lower(adjustment,
			   220.0);
  gtk_adjustment_set_upper(adjustment,
			   1760.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   220.0);
  ags_dial_set_radius(abyss_synth->pink_noise_1_frequency,
		      12);

  gtk_grid_attach(pink_noise_1_grid,
		  (GtkWidget *) abyss_synth->pink_noise_1_frequency,
		  1, 0,
		  1, 1);
  
  /* gain */
  label = (GtkLabel *) gtk_label_new(i18n("noise 2 - gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(pink_noise_1_grid,
		  (GtkWidget *) label,
		  2, 0,
		  1, 1);
  
  abyss_synth->pink_noise_1_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->pink_noise_1_gain);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->pink_noise_1_gain,
		      12);

  gtk_grid_attach(pink_noise_1_grid,
		  (GtkWidget *) abyss_synth->pink_noise_1_gain,
		  3, 0,
		  1, 1);

  /* modulation */
  modulation_grid = (GtkGrid *) gtk_grid_new();
  
  gtk_grid_set_column_spacing(modulation_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(modulation_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_widget_set_halign((GtkWidget *) modulation_grid,
	 		GTK_ALIGN_START);
  
  gtk_grid_attach(abyss_synth_grid,
		  (GtkWidget *) modulation_grid,
		  2, 0,
		  1, 1);

  /* modulation matrix */
  abyss_synth->modulation_matrix = ags_modulation_matrix_new();

  strv_builder = g_strv_builder_new();

  g_strv_builder_add_many(strv_builder,
			  "osc-1 - frequency",
			  "osc-1 - phase",
			  "osc-1 - volume",
			  "osc-2 - frequency",
			  "osc-2 - phase",
			  "osc-2 - volume",
			  "osc-3 - frequency",
			  "osc-3 - phase",
			  "osc-3 - volume",
			  "osc-4 - frequency",
			  "osc-4 - phase",
			  "osc-4 - volume",
			  "ring-1 - tuning",
			  "ring-1 - drive",
			  "ring-1 - mix",
			  "ring-1 - gain",
			  "ring-2 - tuning",
			  "ring-2 - drive",
			  "ring-2 - mix",
			  "ring-2 - gain",
			  "pitch tuning",
			  "volume",
			  NULL);

  label_x = g_strv_builder_end(strv_builder);

  g_strv_builder_add_many(strv_builder,
			  "env-1",
			  "env-2",
			  "env-3",
			  "env-4",
			  "lfo-1",
			  "lfo-2",
			  "lfo-3",
			  "lfo-4",
			  "seq-1",
			  "seq-2",
			  "seq-3",
			  "seq-4",
			  "noise-1",
			  "noise-2",
			  NULL);

  label_y = g_strv_builder_end(strv_builder);

  g_strv_builder_unref(strv_builder);

  ags_modulation_matrix_set_dim(abyss_synth->modulation_matrix,
				22, 14);

  ags_modulation_matrix_set_label(abyss_synth->modulation_matrix,
				  label_x, label_y);

  gtk_grid_attach(modulation_grid,
		  (GtkWidget *) abyss_synth->modulation_matrix,
		  0, 0,
		  1, 1);

  /* osc box */
  osc_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				   AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_grid_attach(abyss_synth_grid,
		  (GtkWidget *) osc_box,
		  3, 0,
		  1, 1);
  
  /* osc-0 */
  osc_0_grid = (GtkGrid *) gtk_grid_new();
  
  gtk_grid_set_column_spacing(osc_0_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(osc_0_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_widget_set_halign((GtkWidget *) osc_0_grid,
	 		GTK_ALIGN_START);

  gtk_box_append(osc_box,
		 (GtkWidget *) osc_0_grid);

  /* oscillator */
  label = (GtkLabel *) gtk_label_new(i18n("osc-1 - oscillator"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  abyss_synth->osc_0_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(abyss_synth->osc_0_oscillator,
			     0);

  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) abyss_synth->osc_0_oscillator,
		  1, 0,
		  1, 1);

  /* octave */
  label = (GtkLabel *) gtk_label_new(i18n("osc-1 - octave"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  abyss_synth->osc_0_octave = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->osc_0_octave);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->osc_0_octave,
		      12);

  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) abyss_synth->osc_0_octave,
		  1, 1,
		  1, 1);

  /* key */
  label = (GtkLabel *) gtk_label_new(i18n("osc-1 - key"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  abyss_synth->osc_0_key = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->osc_0_key);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->osc_0_key,
		      12);

  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) abyss_synth->osc_0_key,
		  1, 2,
		  1, 1);

  /* phase */
  label = (GtkLabel *) gtk_label_new(i18n("osc-1 - phase"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  abyss_synth->osc_0_phase = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->osc_0_phase);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   2.0 * M_PI);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->osc_0_phase,
		      12);

  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) abyss_synth->osc_0_phase,
		  1, 3,
		  1, 1);

  /* volume */
  label = (GtkLabel *) gtk_label_new(i18n("osc-1 - volume"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);
  
  abyss_synth->osc_0_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->osc_0_volume);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->osc_0_volume,
		      12);

  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) abyss_synth->osc_0_volume,
		  1, 4,
		  1, 1);

  /* to low-pass */
  abyss_synth->osc_0_low_pass_0 = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("osc-1 - low-pass-1"));
  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) abyss_synth->osc_0_low_pass_0,
		  0, 5,
		  2, 1);

  abyss_synth->osc_0_low_pass_1 = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("osc-1 - low-pass-2"));
  gtk_check_button_set_group(abyss_synth->osc_0_low_pass_1,
			     abyss_synth->osc_0_low_pass_0);
  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) abyss_synth->osc_0_low_pass_1,
		  0, 6,
		  2, 1);

  abyss_synth->osc_0_no_low_pass = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("osc-1 - no low-pass"));
  gtk_check_button_set_group(abyss_synth->osc_0_no_low_pass,
			     abyss_synth->osc_0_low_pass_0);
  gtk_grid_attach(osc_0_grid,
		  (GtkWidget *) abyss_synth->osc_0_no_low_pass,
		  0, 7,
		  2, 1);

  gtk_check_button_set_active(abyss_synth->osc_0_no_low_pass,
			      TRUE);

  /* osc-1 */
  osc_1_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(osc_1_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(osc_1_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_widget_set_halign((GtkWidget *) osc_1_grid,
	 		GTK_ALIGN_START);

  gtk_box_append(osc_box,
		 (GtkWidget *) osc_1_grid);

  /* oscillator */
  label = (GtkLabel *) gtk_label_new(i18n("osc-2 - oscillator"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  abyss_synth->osc_1_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(abyss_synth->osc_1_oscillator,
			     0);

  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) abyss_synth->osc_1_oscillator,
		  1, 0,
		  1, 1);

  /* octave */
  label = (GtkLabel *) gtk_label_new(i18n("osc-2 - octave"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  abyss_synth->osc_1_octave = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->osc_1_octave);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->osc_1_octave,
		      12);

  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) abyss_synth->osc_1_octave,
		  1, 1,
		  1, 1);

  /* key */
  label = (GtkLabel *) gtk_label_new(i18n("osc-2 - key"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  abyss_synth->osc_1_key = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->osc_1_key);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->osc_1_key,
		      12);

  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) abyss_synth->osc_1_key,
		  1, 2,
		  1, 1);

  /* phase */
  label = (GtkLabel *) gtk_label_new(i18n("osc-2 - phase"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  abyss_synth->osc_1_phase = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->osc_1_phase);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   2.0 * M_PI);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->osc_1_phase,
		      12);

  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) abyss_synth->osc_1_phase,
		  1, 3,
		  1, 1);

  /* volume */
  label = (GtkLabel *) gtk_label_new(i18n("osc-2 - volume"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);
  
  abyss_synth->osc_1_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->osc_1_volume);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->osc_1_volume,
		      12);

  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) abyss_synth->osc_1_volume,
		  1, 4,
		  1, 1);

  /* to low-pass */
  abyss_synth->osc_1_low_pass_0 = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("osc-2 - low-pass-1"));
  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) abyss_synth->osc_1_low_pass_0,
		  0, 5,
		  2, 1);

  abyss_synth->osc_1_low_pass_1 = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("osc-2 - low-pass-2"));
  gtk_check_button_set_group(abyss_synth->osc_1_low_pass_1,
			     abyss_synth->osc_1_low_pass_0);
  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) abyss_synth->osc_1_low_pass_1,
		  0, 6,
		  2, 1);

  abyss_synth->osc_1_no_low_pass = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("osc-2 - no low-pass"));
  gtk_check_button_set_group(abyss_synth->osc_1_no_low_pass,
			     abyss_synth->osc_1_low_pass_0);
  gtk_grid_attach(osc_1_grid,
		  (GtkWidget *) abyss_synth->osc_1_no_low_pass,
		  0, 7,
		  2, 1);

  gtk_check_button_set_active(abyss_synth->osc_1_no_low_pass,
			      TRUE);

  /* osc-2 */
  osc_2_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(osc_2_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(osc_2_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_widget_set_halign((GtkWidget *) osc_2_grid,
	 		GTK_ALIGN_START);

  gtk_box_append(osc_box,
		 (GtkWidget *) osc_2_grid);

  /* oscillator */
  label = (GtkLabel *) gtk_label_new(i18n("osc-3 - oscillator"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_2_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  abyss_synth->osc_2_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(abyss_synth->osc_2_oscillator,
			     0);

  gtk_grid_attach(osc_2_grid,
		  (GtkWidget *) abyss_synth->osc_2_oscillator,
		  1, 0,
		  1, 1);

  /* octave */
  label = (GtkLabel *) gtk_label_new(i18n("osc-3 - octave"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_2_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  abyss_synth->osc_2_octave = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->osc_2_octave);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->osc_2_octave,
		      12);

  gtk_grid_attach(osc_2_grid,
		  (GtkWidget *) abyss_synth->osc_2_octave,
		  1, 1,
		  1, 1);

  /* key */
  label = (GtkLabel *) gtk_label_new(i18n("osc-3 - key"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_2_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  abyss_synth->osc_2_key = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->osc_2_key);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->osc_2_key,
		      12);

  gtk_grid_attach(osc_2_grid,
		  (GtkWidget *) abyss_synth->osc_2_key,
		  1, 2,
		  1, 1);

  /* phase */
  label = (GtkLabel *) gtk_label_new(i18n("osc-3 - phase"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_2_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  abyss_synth->osc_2_phase = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->osc_2_phase);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   2.0 * M_PI);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->osc_2_phase,
		      12);

  gtk_grid_attach(osc_2_grid,
		  (GtkWidget *) abyss_synth->osc_2_phase,
		  1, 3,
		  1, 1);

  /* volume */
  label = (GtkLabel *) gtk_label_new(i18n("osc-3 - volume"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_2_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);
  
  abyss_synth->osc_2_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->osc_2_volume);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->osc_2_volume,
		      12);

  gtk_grid_attach(osc_2_grid,
		  (GtkWidget *) abyss_synth->osc_2_volume,
		  1, 4,
		  1, 1);

  /* to low-pass */
  abyss_synth->osc_2_low_pass_0 = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("osc-3 - low-pass-1"));
  gtk_grid_attach(osc_2_grid,
		  (GtkWidget *) abyss_synth->osc_2_low_pass_0,
		  0, 5,
		  2, 1);

  abyss_synth->osc_2_low_pass_1 = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("osc-3 - low-pass-2"));
  gtk_check_button_set_group(abyss_synth->osc_2_low_pass_1,
			     abyss_synth->osc_2_low_pass_0);
  gtk_grid_attach(osc_2_grid,
		  (GtkWidget *) abyss_synth->osc_2_low_pass_1,
		  0, 6,
		  2, 1);

  abyss_synth->osc_2_no_low_pass = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("osc-3 - no low-pass"));
  gtk_check_button_set_group(abyss_synth->osc_2_no_low_pass,
			     abyss_synth->osc_2_low_pass_0);
  gtk_grid_attach(osc_2_grid,
		  (GtkWidget *) abyss_synth->osc_2_no_low_pass,
		  0, 7,
		  2, 1);

  gtk_check_button_set_active(abyss_synth->osc_2_no_low_pass,
			      TRUE);

  /* osc-3 */
  osc_3_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(osc_3_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(osc_3_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_widget_set_halign((GtkWidget *) osc_3_grid,
	 		GTK_ALIGN_START);
  
  gtk_box_append(osc_box,
		 (GtkWidget *) osc_3_grid);

  /* oscillator */
  label = (GtkLabel *) gtk_label_new(i18n("osc-4 - oscillator"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_3_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  abyss_synth->osc_3_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(abyss_synth->osc_3_oscillator,
			     0);

  gtk_grid_attach(osc_3_grid,
		  (GtkWidget *) abyss_synth->osc_3_oscillator,
		  1, 0,
		  1, 1);

  /* octave */
  label = (GtkLabel *) gtk_label_new(i18n("osc-4 - octave"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_3_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  abyss_synth->osc_3_octave = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->osc_3_octave);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->osc_3_octave,
		      12);

  gtk_grid_attach(osc_3_grid,
		  (GtkWidget *) abyss_synth->osc_3_octave,
		  1, 1,
		  1, 1);

  /* key */
  label = (GtkLabel *) gtk_label_new(i18n("osc-4 - key"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_3_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  abyss_synth->osc_3_key = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->osc_3_key);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->osc_3_key,
		      12);

  gtk_grid_attach(osc_3_grid,
		  (GtkWidget *) abyss_synth->osc_3_key,
		  1, 2,
		  1, 1);

  /* phase */
  label = (GtkLabel *) gtk_label_new(i18n("osc-4 - phase"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_3_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  abyss_synth->osc_3_phase = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->osc_3_phase);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   2.0 * M_PI);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->osc_3_phase,
		      12);

  gtk_grid_attach(osc_3_grid,
		  (GtkWidget *) abyss_synth->osc_3_phase,
		  1, 3,
		  1, 1);

  /* volume */
  label = (GtkLabel *) gtk_label_new(i18n("osc-4 - volume"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(osc_3_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);
  
  abyss_synth->osc_3_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->osc_3_volume);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->osc_3_volume,
		      12);

  gtk_grid_attach(osc_3_grid,
		  (GtkWidget *) abyss_synth->osc_3_volume,
		  1, 4,
		  1, 1);

  /* to low-pass */
  abyss_synth->osc_3_low_pass_0 = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("osc-4 - low-pass-1"));
  gtk_grid_attach(osc_3_grid,
		  (GtkWidget *) abyss_synth->osc_3_low_pass_0,
		  0, 5,
		  2, 1);

  abyss_synth->osc_3_low_pass_1 = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("osc-4 - low-pass-2"));
  gtk_check_button_set_group(abyss_synth->osc_3_low_pass_1,
			     abyss_synth->osc_3_low_pass_0);
  gtk_grid_attach(osc_3_grid,
		  (GtkWidget *) abyss_synth->osc_3_low_pass_1,
		  0, 6,
		  2, 1);

  abyss_synth->osc_3_no_low_pass = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("osc-4 - no low-pass"));
  gtk_check_button_set_group(abyss_synth->osc_3_no_low_pass,
			     abyss_synth->osc_3_low_pass_0);
  gtk_grid_attach(osc_3_grid,
		  (GtkWidget *) abyss_synth->osc_3_no_low_pass,
		  0, 7,
		  2, 1);

  gtk_check_button_set_active(abyss_synth->osc_3_no_low_pass,
			      TRUE);

  /* ring box */
  ring_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				   AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_grid_attach(abyss_synth_grid,
		  (GtkWidget *) ring_box,
		  4, 0,
		  1, 1);
  
  /* ring 0 */
  ring_0_grid = (GtkGrid *) gtk_grid_new();
  
  gtk_grid_set_column_spacing(ring_0_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(ring_0_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_widget_set_halign((GtkWidget *) ring_0_grid,
	 		GTK_ALIGN_START);
  
  gtk_box_append(ring_box,
		 (GtkWidget *) ring_0_grid);

  /* enabled */
  abyss_synth->ring_0_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("ring-1 - enabled"));
  gtk_grid_attach(ring_0_grid,
		  (GtkWidget *) abyss_synth->ring_0_enabled,
		  0, 0,
		  2, 1);
  
  /* tuning */
  label = (GtkLabel *) gtk_label_new(i18n("ring-1 - tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(ring_0_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  abyss_synth->ring_0_tuning = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->ring_0_tuning);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->ring_0_tuning,
		      12);

  gtk_grid_attach(ring_0_grid,
		  (GtkWidget *) abyss_synth->ring_0_tuning,
		  1, 1,
		  1, 1);

  /* drive */
  label = (GtkLabel *) gtk_label_new(i18n("ring-1 - drive"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(ring_0_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  abyss_synth->ring_0_drive = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->ring_0_drive);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->ring_0_drive,
		      12);

  gtk_grid_attach(ring_0_grid,
		  (GtkWidget *) abyss_synth->ring_0_drive,
		  1, 2,
		  1, 1);

  /* mix */
  label = (GtkLabel *) gtk_label_new(i18n("ring-1 - mix"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(ring_0_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  abyss_synth->ring_0_mix = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->ring_0_mix);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->ring_0_mix,
		      12);

  gtk_grid_attach(ring_0_grid,
		  (GtkWidget *) abyss_synth->ring_0_mix,
		  1, 3,
		  1, 1);

  /* gain */
  label = (GtkLabel *) gtk_label_new(i18n("ring-1 - gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(ring_0_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);
  
  abyss_synth->ring_0_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->ring_0_gain);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->ring_0_gain,
		      12);

  gtk_grid_attach(ring_0_grid,
		  (GtkWidget *) abyss_synth->ring_0_gain,
		  1, 4,
		  1, 1);
  
  /* ring 1 */
  ring_1_grid = (GtkGrid *) gtk_grid_new();
  
  gtk_grid_set_column_spacing(ring_1_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(ring_1_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_widget_set_halign((GtkWidget *) ring_1_grid,
	 		GTK_ALIGN_START);
  
  gtk_box_append(ring_box,
		 (GtkWidget *) ring_1_grid);

  /* enabled */
  abyss_synth->ring_1_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("ring-2 - enabled"));
  gtk_grid_attach(ring_1_grid,
		  (GtkWidget *) abyss_synth->ring_1_enabled,
		  0, 0,
		  2, 1);
  
  /* tuning */
  label = (GtkLabel *) gtk_label_new(i18n("ring-2 - tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(ring_1_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  abyss_synth->ring_1_tuning = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->ring_1_tuning);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->ring_1_tuning,
		      12);

  gtk_grid_attach(ring_1_grid,
		  (GtkWidget *) abyss_synth->ring_1_tuning,
		  1, 1,
		  1, 1);

  /* drive */
  label = (GtkLabel *) gtk_label_new(i18n("ring-2 - drive"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(ring_1_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  abyss_synth->ring_1_drive = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->ring_1_drive);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->ring_1_drive,
		      12);

  gtk_grid_attach(ring_1_grid,
		  (GtkWidget *) abyss_synth->ring_1_drive,
		  1, 2,
		  1, 1);

  /* mix */
  label = (GtkLabel *) gtk_label_new(i18n("ring-2 - mix"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(ring_1_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);
  
  abyss_synth->ring_1_mix = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->ring_1_mix);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->ring_1_mix,
		      12);

  gtk_grid_attach(ring_1_grid,
		  (GtkWidget *) abyss_synth->ring_1_mix,
		  1, 3,
		  1, 1);

  /* gain */
  label = (GtkLabel *) gtk_label_new(i18n("ring-2 - gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(ring_1_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);
  
  abyss_synth->ring_1_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->ring_1_gain);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->ring_1_gain,
		      12);

  gtk_grid_attach(ring_1_grid,
		  (GtkWidget *) abyss_synth->ring_1_gain,
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
  
  gtk_grid_attach(abyss_synth_grid,
		  (GtkWidget *) effect_grid,
		  5, 0,
		  1, 1);

  /* pitch tuning */
  label = (GtkLabel *) gtk_label_new(i18n("pitch tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(effect_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  abyss_synth->pitch_tuning = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->pitch_tuning);

  gtk_adjustment_set_lower(adjustment,
			   -1200.0);
  gtk_adjustment_set_upper(adjustment,
			   1200.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->pitch_tuning,
		      12);

  gtk_grid_attach(effect_grid,
		  (GtkWidget *) abyss_synth->pitch_tuning,
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
  
  abyss_synth->volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->volume);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.333);
  ags_dial_set_radius(abyss_synth->volume,
		      12);

  gtk_grid_attach(effect_grid,
		  (GtkWidget *) abyss_synth->volume,
		  1, 1,
		  1, 1);

  /* low-pass 0 */
  low_pass_0_grid = (GtkGrid *) gtk_grid_new();

  gtk_widget_set_valign((GtkWidget *) low_pass_0_grid,
	 		GTK_ALIGN_START);
  gtk_widget_set_halign((GtkWidget *) low_pass_0_grid,
	 		GTK_ALIGN_START);

  gtk_widget_set_vexpand((GtkWidget *) low_pass_0_grid,
			 FALSE);
  
  gtk_grid_attach(abyss_synth_grid,
		  (GtkWidget *) low_pass_0_grid,
		  0, 4,
		  1, 1);

  /* low-pass-0 cut-off frequency */
  label = (GtkLabel *) gtk_label_new(i18n("cut-off frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(low_pass_0_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  abyss_synth->low_pass_0_cut_off_frequency = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->low_pass_0_cut_off_frequency);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   22000.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   2000.0);
  ags_dial_set_radius(abyss_synth->low_pass_0_cut_off_frequency,
		      12);

  gtk_grid_attach(low_pass_0_grid,
		  (GtkWidget *) abyss_synth->low_pass_0_cut_off_frequency,
		  1, 0,
		  1, 1);

  /* low-pass-0 filter gain */
  label = (GtkLabel *) gtk_label_new(i18n("filter gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(low_pass_0_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  abyss_synth->low_pass_0_filter_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->low_pass_0_filter_gain);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   1.0);
  ags_dial_set_radius(abyss_synth->low_pass_0_filter_gain,
		      12);

  gtk_grid_attach(low_pass_0_grid,
		  (GtkWidget *) abyss_synth->low_pass_0_filter_gain,
		  1, 1,
		  1, 1);

  /* low-pass-0 no-clip */
  label = (GtkLabel *) gtk_label_new(i18n("no-clip"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(low_pass_0_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  abyss_synth->low_pass_0_no_clip = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->low_pass_0_no_clip);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   22000.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->low_pass_0_no_clip,
		      12);

  gtk_grid_attach(low_pass_0_grid,
		  (GtkWidget *) abyss_synth->low_pass_0_no_clip,
		  1, 2,
		  1, 1);

  /* low-pass 1 */
  low_pass_1_grid = (GtkGrid *) gtk_grid_new();

  gtk_widget_set_valign((GtkWidget *) low_pass_1_grid,
	 		GTK_ALIGN_START);
  gtk_widget_set_halign((GtkWidget *) low_pass_1_grid,
	 		GTK_ALIGN_START);

  gtk_widget_set_vexpand((GtkWidget *) low_pass_1_grid,
			 FALSE);
  
  gtk_grid_attach(abyss_synth_grid,
		  (GtkWidget *) low_pass_1_grid,
		  1, 4,
		  1, 1);

  /* low-pass-0 cut-off frequency */
  label = (GtkLabel *) gtk_label_new(i18n("cut-off frequency"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(low_pass_1_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  abyss_synth->low_pass_1_cut_off_frequency = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->low_pass_1_cut_off_frequency);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   22000.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   2000.0);
  ags_dial_set_radius(abyss_synth->low_pass_1_cut_off_frequency,
		      12);

  gtk_grid_attach(low_pass_1_grid,
		  (GtkWidget *) abyss_synth->low_pass_1_cut_off_frequency,
		  1, 0,
		  1, 1);

  /* low-pass-0 filter gain */
  label = (GtkLabel *) gtk_label_new(i18n("filter gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(low_pass_1_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);
  
  abyss_synth->low_pass_1_filter_gain = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->low_pass_1_filter_gain);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   1.0);
  ags_dial_set_radius(abyss_synth->low_pass_1_filter_gain,
		      12);

  gtk_grid_attach(low_pass_1_grid,
		  (GtkWidget *) abyss_synth->low_pass_1_filter_gain,
		  1, 1,
		  1, 1);

  /* low-pass-0 no-clip */
  label = (GtkLabel *) gtk_label_new(i18n("no-clip"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(low_pass_1_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);
  
  abyss_synth->low_pass_1_no_clip = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->low_pass_1_no_clip);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   22000.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);

  gtk_adjustment_set_value(adjustment,
			   0.0);
  ags_dial_set_radius(abyss_synth->low_pass_1_no_clip,
		      12);

  gtk_grid_attach(low_pass_1_grid,
		  (GtkWidget *) abyss_synth->low_pass_1_no_clip,
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
  
  gtk_grid_attach(abyss_synth_grid,
		  (GtkWidget *) amplifier_grid,
		  2, 4,
		  2, 1);

  /* amplifier-0 amp-0 gain */
  label = (GtkLabel *) gtk_label_new(i18n("amp-0 gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(amplifier_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);
  
  abyss_synth->amplifier_0_amp_0_gain = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
									      -20.0,
									      20.0,
									      1.0);
  gtk_widget_set_size_request((GtkWidget *) abyss_synth->amplifier_0_amp_0_gain,
			      16, 100);

  gtk_range_set_value((GtkRange *) abyss_synth->amplifier_0_amp_0_gain,
		      0.0);
  gtk_range_set_inverted((GtkRange *) abyss_synth->amplifier_0_amp_0_gain,
			 TRUE);

  gtk_grid_attach(amplifier_grid,
		  (GtkWidget *) abyss_synth->amplifier_0_amp_0_gain,
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
  
  abyss_synth->amplifier_0_amp_1_gain = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
									      -20.0,
									      20.0,
									      1.0);
  gtk_widget_set_size_request((GtkWidget *) abyss_synth->amplifier_0_amp_1_gain,
			      16, 100);

  gtk_range_set_value((GtkRange *) abyss_synth->amplifier_0_amp_1_gain,
		      0.0);
  gtk_range_set_inverted((GtkRange *) abyss_synth->amplifier_0_amp_1_gain,
			 TRUE);
  
  gtk_grid_attach(amplifier_grid,
		  (GtkWidget *) abyss_synth->amplifier_0_amp_1_gain,
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
  
  abyss_synth->amplifier_0_amp_2_gain = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
									      -20.0,
									      20.0,
									      1.0);
  gtk_widget_set_size_request((GtkWidget *) abyss_synth->amplifier_0_amp_2_gain,
			      16, 100);

  gtk_range_set_value((GtkRange *) abyss_synth->amplifier_0_amp_2_gain,
		      0.0);
  gtk_range_set_inverted((GtkRange *) abyss_synth->amplifier_0_amp_2_gain,
			 TRUE);
  
  gtk_grid_attach(amplifier_grid,
		  (GtkWidget *) abyss_synth->amplifier_0_amp_2_gain,
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
  
  abyss_synth->amplifier_0_amp_3_gain = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
									      -20.0,
									      20.0,
									      1.0);
  gtk_widget_set_size_request((GtkWidget *) abyss_synth->amplifier_0_amp_3_gain,
			      16, 100);

  gtk_range_set_value((GtkRange *) abyss_synth->amplifier_0_amp_3_gain,
		      0.0);
  gtk_range_set_inverted((GtkRange *) abyss_synth->amplifier_0_amp_3_gain,
			 TRUE);
  
  gtk_grid_attach(amplifier_grid,
		  (GtkWidget *) abyss_synth->amplifier_0_amp_3_gain,
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
  
  abyss_synth->amplifier_0_filter_gain = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
									       -20.0,
									       20.0,
									       1.0);
  gtk_widget_set_size_request((GtkWidget *) abyss_synth->amplifier_0_filter_gain,
			      16, 100);

  gtk_range_set_value((GtkRange *) abyss_synth->amplifier_0_filter_gain,
		      1.0);
  gtk_range_set_inverted((GtkRange *) abyss_synth->amplifier_0_filter_gain,
			 TRUE);

  gtk_grid_attach(amplifier_grid,
		  (GtkWidget *) abyss_synth->amplifier_0_filter_gain,
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

  gtk_grid_attach(abyss_synth_grid,
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

  abyss_synth->chorus_input_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->chorus_input_volume);

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

  ags_dial_set_radius(abyss_synth->chorus_input_volume,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) abyss_synth->chorus_input_volume,
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

  abyss_synth->chorus_output_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->chorus_output_volume);

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

  ags_dial_set_radius(abyss_synth->chorus_output_volume,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) abyss_synth->chorus_output_volume,
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
  
  abyss_synth->chorus_lfo_oscillator = (GtkDropDown *) gtk_drop_down_new_from_strings((const char * const *) osc_strv);

  gtk_drop_down_set_selected(abyss_synth->chorus_lfo_oscillator,
			     0);

  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) abyss_synth->chorus_lfo_oscillator,
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

  abyss_synth->chorus_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 10.0, 0.01);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) abyss_synth->chorus_lfo_frequency,
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

  abyss_synth->chorus_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->chorus_depth);

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

  ags_dial_set_radius(abyss_synth->chorus_depth,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) abyss_synth->chorus_depth,
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

  abyss_synth->chorus_mix = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->chorus_mix);

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

  ags_dial_set_radius(abyss_synth->chorus_mix,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) abyss_synth->chorus_mix,
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

  abyss_synth->chorus_delay = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(abyss_synth->chorus_delay);

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

  ags_dial_set_radius(abyss_synth->chorus_delay,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) abyss_synth->chorus_delay,
		  5, 2,
		  1, 1);
}

void
ags_abyss_synth_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_abyss_synth_parent_class)->finalize(gobject);
}

void
ags_abyss_synth_connect(AgsConnectable *connectable)
{
  AgsAbyssSynth *abyss_synth;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  ags_abyss_synth_parent_connectable_interface->connect(connectable);
  
  /* AgsAbyssSynth */
  abyss_synth = AGS_ABYSS_SYNTH(connectable);

  /* env-0 */
  g_signal_connect_after(abyss_synth->env_0_attack, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_0_attack_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->env_0_decay, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_0_decay_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->env_0_sustain, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_0_sustain_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->env_0_release, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_0_release_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->env_0_gain, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_0_gain_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->env_0_frequency, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_0_frequency_callback), abyss_synth);

  /* env-1 */
  g_signal_connect_after(abyss_synth->env_1_attack, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_1_attack_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->env_1_decay, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_1_decay_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->env_1_sustain, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_1_sustain_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->env_1_release, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_1_release_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->env_1_gain, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_1_gain_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->env_1_frequency, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_1_frequency_callback), abyss_synth);

  /* env-2 */
  g_signal_connect_after(abyss_synth->env_2_attack, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_2_attack_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->env_2_decay, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_2_decay_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->env_2_sustain, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_2_sustain_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->env_2_release, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_2_release_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->env_2_gain, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_2_gain_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->env_2_frequency, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_2_frequency_callback), abyss_synth);

  /* env-3 */
  g_signal_connect_after(abyss_synth->env_2_attack, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_2_attack_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->env_2_decay, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_2_decay_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->env_2_sustain, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_2_sustain_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->env_2_release, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_2_release_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->env_2_gain, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_2_gain_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->env_2_frequency, "value-changed",
			 G_CALLBACK(ags_abyss_synth_env_2_frequency_callback), abyss_synth);

  /* LFO-0 */
  g_signal_connect_after(abyss_synth->lfo_0_oscillator, "notify::selected",
			 G_CALLBACK(ags_abyss_synth_lfo_0_oscillator_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->lfo_0_frequency, "value-changed",
			 G_CALLBACK(ags_abyss_synth_lfo_0_frequency_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->lfo_0_depth, "value-changed",
			 G_CALLBACK(ags_abyss_synth_lfo_0_depth_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->lfo_0_tuning, "value-changed",
			 G_CALLBACK(ags_abyss_synth_lfo_0_tuning_callback), abyss_synth);

  /* LFO-1 */
  g_signal_connect_after(abyss_synth->lfo_1_oscillator, "notify::selected",
			 G_CALLBACK(ags_abyss_synth_lfo_1_oscillator_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->lfo_1_frequency, "value-changed",
			 G_CALLBACK(ags_abyss_synth_lfo_1_frequency_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->lfo_1_depth, "value-changed",
			 G_CALLBACK(ags_abyss_synth_lfo_1_depth_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->lfo_1_tuning, "value-changed",
			 G_CALLBACK(ags_abyss_synth_lfo_1_tuning_callback), abyss_synth);

  /* LFO-2 */
  g_signal_connect_after(abyss_synth->lfo_2_oscillator, "notify::selected",
			 G_CALLBACK(ags_abyss_synth_lfo_2_oscillator_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->lfo_2_frequency, "value-changed",
			 G_CALLBACK(ags_abyss_synth_lfo_2_frequency_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->lfo_2_depth, "value-changed",
			 G_CALLBACK(ags_abyss_synth_lfo_2_depth_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->lfo_2_tuning, "value-changed",
			 G_CALLBACK(ags_abyss_synth_lfo_2_tuning_callback), abyss_synth);

  /* LFO-3 */
  g_signal_connect_after(abyss_synth->lfo_3_oscillator, "notify::selected",
			 G_CALLBACK(ags_abyss_synth_lfo_3_oscillator_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->lfo_3_frequency, "value-changed",
			 G_CALLBACK(ags_abyss_synth_lfo_3_frequency_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->lfo_3_depth, "value-changed",
			 G_CALLBACK(ags_abyss_synth_lfo_3_depth_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->lfo_3_tuning, "value-changed",
			 G_CALLBACK(ags_abyss_synth_lfo_3_tuning_callback), abyss_synth);

  /* pink noise 0 */
  g_signal_connect_after(abyss_synth->pink_noise_0_frequency, "value-changed",
			 G_CALLBACK(ags_abyss_synth_pink_noise_0_frequency_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->pink_noise_0_gain, "value-changed",
			 G_CALLBACK(ags_abyss_synth_pink_noise_0_gain_callback), abyss_synth);

  /* pink noise 1 */
  g_signal_connect_after(abyss_synth->pink_noise_1_frequency, "value-changed",
			 G_CALLBACK(ags_abyss_synth_pink_noise_1_frequency_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->pink_noise_1_gain, "value-changed",
			 G_CALLBACK(ags_abyss_synth_pink_noise_1_gain_callback), abyss_synth);

  /* seq-0 modulation */
  g_signal_connect_after(abyss_synth->seq_0_modulation_0, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_0_modulation_0_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_0_modulation_1, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_0_modulation_1_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_0_modulation_2, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_0_modulation_2_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_0_modulation_3, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_0_modulation_3_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_0_modulation_4, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_0_modulation_4_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_0_modulation_5, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_0_modulation_5_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_0_modulation_6, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_0_modulation_6_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_0_modulation_7, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_0_modulation_7_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_0_modulation_8, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_0_modulation_8_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_0_modulation_9, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_0_modulation_9_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_0_modulation_10, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_0_modulation_10_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_0_modulation_11, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_0_modulation_11_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_0_modulation_12, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_0_modulation_12_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_0_modulation_13, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_0_modulation_13_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_0_modulation_14, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_0_modulation_14_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_0_modulation_15, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_0_modulation_15_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_0_pingpong, "toggled",
			 G_CALLBACK(ags_abyss_synth_seq_0_pingpong_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_0_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_0_lfo_frequency_callback), abyss_synth);

  /* seq-1 modulation */
  g_signal_connect_after(abyss_synth->seq_1_modulation_0, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_1_modulation_0_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_1_modulation_1, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_1_modulation_1_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_1_modulation_2, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_1_modulation_2_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_1_modulation_3, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_1_modulation_3_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_1_modulation_4, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_1_modulation_4_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_1_modulation_5, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_1_modulation_5_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_1_modulation_6, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_1_modulation_6_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_1_modulation_7, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_1_modulation_7_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_1_modulation_8, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_1_modulation_8_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_1_modulation_9, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_1_modulation_9_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_1_modulation_10, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_1_modulation_10_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_1_modulation_11, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_1_modulation_11_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_1_modulation_12, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_1_modulation_12_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_1_modulation_13, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_1_modulation_13_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_1_modulation_14, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_1_modulation_14_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_1_modulation_15, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_1_modulation_15_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_1_pingpong, "toggled",
			 G_CALLBACK(ags_abyss_synth_seq_1_pingpong_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_1_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_1_lfo_frequency_callback), abyss_synth);

  /* seq-2 modulation */
  g_signal_connect_after(abyss_synth->seq_2_modulation_0, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_2_modulation_0_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_2_modulation_1, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_2_modulation_1_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_2_modulation_2, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_2_modulation_2_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_2_modulation_3, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_2_modulation_3_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_2_modulation_4, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_2_modulation_4_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_2_modulation_5, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_2_modulation_5_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_2_modulation_6, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_2_modulation_6_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_2_modulation_7, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_2_modulation_7_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_2_modulation_8, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_2_modulation_8_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_2_modulation_9, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_2_modulation_9_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_2_modulation_10, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_2_modulation_10_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_2_modulation_11, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_2_modulation_11_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_2_modulation_12, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_2_modulation_12_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_2_modulation_13, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_2_modulation_13_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_2_modulation_14, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_2_modulation_14_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_2_modulation_15, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_2_modulation_15_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_2_pingpong, "toggled",
			 G_CALLBACK(ags_abyss_synth_seq_2_pingpong_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_2_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_2_lfo_frequency_callback), abyss_synth);

  /* seq-3 modulation */
  g_signal_connect_after(abyss_synth->seq_3_modulation_0, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_3_modulation_0_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_3_modulation_1, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_3_modulation_1_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_3_modulation_2, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_3_modulation_2_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_3_modulation_3, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_3_modulation_3_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_3_modulation_4, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_3_modulation_4_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_3_modulation_5, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_3_modulation_5_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_3_modulation_6, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_3_modulation_6_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_3_modulation_7, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_3_modulation_7_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_3_modulation_8, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_3_modulation_8_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_3_modulation_9, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_3_modulation_9_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_3_modulation_10, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_3_modulation_10_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_3_modulation_11, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_3_modulation_11_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_3_modulation_12, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_3_modulation_12_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_3_modulation_13, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_3_modulation_13_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_3_modulation_14, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_3_modulation_14_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_3_modulation_15, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_3_modulation_15_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_3_pingpong, "toggled",
			 G_CALLBACK(ags_abyss_synth_seq_3_pingpong_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->seq_3_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_abyss_synth_seq_3_lfo_frequency_callback), abyss_synth);
  
  /* modulation matrix */
  ags_connectable_connect(AGS_CONNECTABLE(abyss_synth->modulation_matrix));

  g_signal_connect_after(abyss_synth->modulation_matrix, "toggled",
			 G_CALLBACK(ags_abyss_synth_modulation_matrix_callback), abyss_synth);

  /* osc-0 */
  g_signal_connect_after(abyss_synth->osc_0_oscillator, "notify::selected",
			 G_CALLBACK(ags_abyss_synth_osc_0_oscillator_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_0_octave, "value-changed",
			 G_CALLBACK(ags_abyss_synth_osc_0_octave_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_0_key, "value-changed",
			 G_CALLBACK(ags_abyss_synth_osc_0_key_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_0_phase, "value-changed",
			 G_CALLBACK(ags_abyss_synth_osc_0_phase_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_0_volume, "value-changed",
			 G_CALLBACK(ags_abyss_synth_osc_0_volume_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_0_low_pass_0, "toggled",
			 G_CALLBACK(ags_abyss_synth_osc_0_low_pass_0_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_0_low_pass_1, "toggled",
			 G_CALLBACK(ags_abyss_synth_osc_0_low_pass_1_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_0_no_low_pass, "toggled",
			 G_CALLBACK(ags_abyss_synth_osc_0_no_low_pass_callback), abyss_synth);

  /* osc-1 */
  g_signal_connect_after(abyss_synth->osc_1_oscillator, "notify::selected",
			 G_CALLBACK(ags_abyss_synth_osc_1_oscillator_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_1_octave, "value-changed",
			 G_CALLBACK(ags_abyss_synth_osc_1_octave_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_1_key, "value-changed",
			 G_CALLBACK(ags_abyss_synth_osc_1_key_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_1_phase, "value-changed",
			 G_CALLBACK(ags_abyss_synth_osc_1_phase_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_1_volume, "value-changed",
			 G_CALLBACK(ags_abyss_synth_osc_1_volume_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_1_low_pass_0, "toggled",
			 G_CALLBACK(ags_abyss_synth_osc_1_low_pass_0_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_1_low_pass_1, "toggled",
			 G_CALLBACK(ags_abyss_synth_osc_1_low_pass_1_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_1_no_low_pass, "toggled",
			 G_CALLBACK(ags_abyss_synth_osc_1_no_low_pass_callback), abyss_synth);

  /* osc-2 */
  g_signal_connect_after(abyss_synth->osc_2_oscillator, "notify::selected",
			 G_CALLBACK(ags_abyss_synth_osc_2_oscillator_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_2_octave, "value-changed",
			 G_CALLBACK(ags_abyss_synth_osc_2_octave_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_2_key, "value-changed",
			 G_CALLBACK(ags_abyss_synth_osc_2_key_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_2_phase, "value-changed",
			 G_CALLBACK(ags_abyss_synth_osc_2_phase_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_2_volume, "value-changed",
			 G_CALLBACK(ags_abyss_synth_osc_2_volume_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_2_low_pass_0, "toggled",
			 G_CALLBACK(ags_abyss_synth_osc_2_low_pass_0_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_2_low_pass_1, "toggled",
			 G_CALLBACK(ags_abyss_synth_osc_2_low_pass_1_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_2_no_low_pass, "toggled",
			 G_CALLBACK(ags_abyss_synth_osc_2_no_low_pass_callback), abyss_synth);

  /* osc-3 */
  g_signal_connect_after(abyss_synth->osc_3_oscillator, "notify::selected",
			 G_CALLBACK(ags_abyss_synth_osc_3_oscillator_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_3_octave, "value-changed",
			 G_CALLBACK(ags_abyss_synth_osc_3_octave_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_3_key, "value-changed",
			 G_CALLBACK(ags_abyss_synth_osc_3_key_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_3_phase, "value-changed",
			 G_CALLBACK(ags_abyss_synth_osc_3_phase_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_3_volume, "value-changed",
			 G_CALLBACK(ags_abyss_synth_osc_3_volume_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_3_low_pass_0, "toggled",
			 G_CALLBACK(ags_abyss_synth_osc_3_low_pass_0_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_3_low_pass_1, "toggled",
			 G_CALLBACK(ags_abyss_synth_osc_3_low_pass_1_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->osc_3_no_low_pass, "toggled",
			 G_CALLBACK(ags_abyss_synth_osc_3_no_low_pass_callback), abyss_synth);

  /* ring 0 */
  g_signal_connect_after(abyss_synth->ring_0_enabled, "toggled",
			 G_CALLBACK(ags_abyss_synth_ring_0_enabled_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->ring_0_tuning, "value-changed",
			 G_CALLBACK(ags_abyss_synth_ring_0_tuning_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->ring_0_drive, "value-changed",
			 G_CALLBACK(ags_abyss_synth_ring_0_drive_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->ring_0_mix, "value-changed",
			 G_CALLBACK(ags_abyss_synth_ring_0_mix_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->ring_0_gain, "value-changed",
			 G_CALLBACK(ags_abyss_synth_ring_0_gain_callback), abyss_synth);

  /* ring 1 */
  g_signal_connect_after(abyss_synth->ring_1_enabled, "toggled",
			 G_CALLBACK(ags_abyss_synth_ring_1_enabled_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->ring_1_tuning, "value-changed",
			 G_CALLBACK(ags_abyss_synth_ring_1_tuning_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->ring_1_drive, "value-changed",
			 G_CALLBACK(ags_abyss_synth_ring_1_drive_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->ring_1_mix, "value-changed",
			 G_CALLBACK(ags_abyss_synth_ring_1_mix_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->ring_1_gain, "value-changed",
			 G_CALLBACK(ags_abyss_synth_ring_1_gain_callback), abyss_synth);  

  /* volume */
  g_signal_connect_after(abyss_synth->volume, "value-changed",
			 G_CALLBACK(ags_abyss_synth_volume_callback), abyss_synth);

  /* pitch */
  g_signal_connect_after(abyss_synth->pitch_tuning, "value-changed",
			 G_CALLBACK(ags_abyss_synth_pitch_tuning_callback), abyss_synth);

  /* low-pass 0 */
  g_signal_connect_after(abyss_synth->low_pass_0_cut_off_frequency, "value-changed",
			 G_CALLBACK(ags_abyss_synth_low_pass_0_cut_off_frequency_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->low_pass_0_filter_gain, "value-changed",
			 G_CALLBACK(ags_abyss_synth_low_pass_0_filter_gain_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->low_pass_0_no_clip, "value-changed",
			 G_CALLBACK(ags_abyss_synth_low_pass_0_no_clip_callback), abyss_synth);

  /* low-pass 1 */
  g_signal_connect_after(abyss_synth->low_pass_1_cut_off_frequency, "value-changed",
			 G_CALLBACK(ags_abyss_synth_low_pass_1_cut_off_frequency_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->low_pass_1_filter_gain, "value-changed",
			 G_CALLBACK(ags_abyss_synth_low_pass_1_filter_gain_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->low_pass_1_no_clip, "value-changed",
			 G_CALLBACK(ags_abyss_synth_low_pass_1_no_clip_callback), abyss_synth);

  /* amplifier 0 */  
  g_signal_connect_after(abyss_synth->amplifier_0_amp_0_gain, "value-changed",
			 G_CALLBACK(ags_abyss_synth_amplifier_0_amp_0_gain_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->amplifier_0_amp_1_gain, "value-changed",
			 G_CALLBACK(ags_abyss_synth_amplifier_0_amp_1_gain_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->amplifier_0_amp_2_gain, "value-changed",
			 G_CALLBACK(ags_abyss_synth_amplifier_0_amp_2_gain_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->amplifier_0_amp_3_gain, "value-changed",
			 G_CALLBACK(ags_abyss_synth_amplifier_0_amp_3_gain_callback), abyss_synth);

  g_signal_connect_after(abyss_synth->amplifier_0_filter_gain, "value-changed",
			 G_CALLBACK(ags_abyss_synth_amplifier_0_filter_gain_callback), abyss_synth);

  /* chorus */
  g_signal_connect_after(abyss_synth->chorus_input_volume, "value-changed",
			 G_CALLBACK(ags_abyss_synth_chorus_input_volume_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->chorus_output_volume, "value-changed",
			 G_CALLBACK(ags_abyss_synth_chorus_output_volume_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->chorus_lfo_oscillator, "notify::selected",
			 G_CALLBACK(ags_abyss_synth_chorus_lfo_oscillator_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->chorus_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_abyss_synth_chorus_lfo_frequency_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->chorus_depth, "value-changed",
			 G_CALLBACK(ags_abyss_synth_chorus_depth_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->chorus_mix, "value-changed",
			 G_CALLBACK(ags_abyss_synth_chorus_mix_callback), abyss_synth);
  
  g_signal_connect_after(abyss_synth->chorus_delay, "value-changed",
			 G_CALLBACK(ags_abyss_synth_chorus_delay_callback), abyss_synth);
}

void
ags_abyss_synth_disconnect(AgsConnectable *connectable)
{
  AgsAbyssSynth *abyss_synth;
  
  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  ags_abyss_synth_parent_connectable_interface->disconnect(connectable);

  /* AgsAbyssSynth */
  abyss_synth = AGS_ABYSS_SYNTH(connectable);

  /* env-0 */
  g_object_disconnect(abyss_synth->env_0_attack,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_0_attack_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->env_0_decay,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_0_decay_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->env_0_sustain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_0_sustain_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->env_0_release,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_0_release_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->env_0_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_0_gain_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->env_0_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_0_frequency_callback),
		      abyss_synth,
		      NULL);

  /* env-1 */
  g_object_disconnect(abyss_synth->env_1_attack,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_1_attack_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->env_1_decay,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_1_decay_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->env_1_sustain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_1_sustain_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->env_1_release,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_1_release_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->env_1_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_1_gain_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->env_1_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_1_frequency_callback),
		      abyss_synth,
		      NULL);

  /* env-2 */
  g_object_disconnect(abyss_synth->env_2_attack,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_2_attack_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->env_2_decay,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_2_decay_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->env_2_sustain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_2_sustain_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->env_2_release,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_2_release_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->env_2_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_2_gain_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->env_2_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_2_frequency_callback),
		      abyss_synth,
		      NULL);

  /* env-3 */
  g_object_disconnect(abyss_synth->env_3_attack,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_3_attack_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->env_3_decay,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_3_decay_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->env_3_sustain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_3_sustain_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->env_3_release,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_3_release_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->env_3_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_3_gain_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->env_3_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_env_3_frequency_callback),
		      abyss_synth,
		      NULL);

  /* LFO-0 */
  g_object_disconnect(abyss_synth->lfo_0_oscillator,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_abyss_synth_lfo_0_oscillator_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->lfo_0_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_lfo_0_frequency_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->lfo_0_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_lfo_0_depth_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->lfo_0_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_lfo_0_tuning_callback),
		      abyss_synth,
		      NULL);

  /* LFO-1 */
  g_object_disconnect(abyss_synth->lfo_1_oscillator,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_abyss_synth_lfo_1_oscillator_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->lfo_1_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_lfo_1_frequency_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->lfo_1_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_lfo_1_depth_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->lfo_1_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_lfo_1_tuning_callback),
		      abyss_synth,
		      NULL);

  /* LFO-2 */
  g_object_disconnect(abyss_synth->lfo_2_oscillator,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_abyss_synth_lfo_2_oscillator_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->lfo_2_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_lfo_2_frequency_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->lfo_2_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_lfo_2_depth_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->lfo_2_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_lfo_2_tuning_callback),
		      abyss_synth,
		      NULL);

  /* LFO-3 */
  g_object_disconnect(abyss_synth->lfo_3_oscillator,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_abyss_synth_lfo_3_oscillator_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->lfo_3_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_lfo_3_frequency_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->lfo_3_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_lfo_3_depth_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->lfo_3_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_lfo_3_tuning_callback),
		      abyss_synth,
		      NULL);

  /* seq 0 */
  g_object_disconnect(abyss_synth->seq_0_modulation_0,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_0_modulation_0_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_0_modulation_1,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_0_modulation_1_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_0_modulation_2,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_0_modulation_2_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_0_modulation_3,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_0_modulation_3_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_0_modulation_4,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_0_modulation_4_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_0_modulation_5,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_0_modulation_5_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_0_modulation_6,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_0_modulation_6_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_0_modulation_7,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_0_modulation_7_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_0_modulation_8,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_0_modulation_8_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_0_modulation_9,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_0_modulation_9_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_0_modulation_10,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_0_modulation_10_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_0_modulation_11,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_0_modulation_11_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_0_modulation_12,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_0_modulation_12_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_0_modulation_13,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_0_modulation_13_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_0_modulation_14,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_0_modulation_14_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_0_modulation_15,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_0_modulation_15_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_0_pingpong,
		      "any_signal::toggled",
		      G_CALLBACK(ags_abyss_synth_seq_0_pingpong_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->seq_0_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_0_lfo_frequency_callback),
		      abyss_synth,
		      NULL);

  /* seq 1 */
  g_object_disconnect(abyss_synth->seq_1_modulation_0,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_1_modulation_0_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_1_modulation_1,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_1_modulation_1_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_1_modulation_2,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_1_modulation_2_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_1_modulation_3,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_1_modulation_3_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_1_modulation_4,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_1_modulation_4_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_1_modulation_5,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_1_modulation_5_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_1_modulation_6,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_1_modulation_6_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_1_modulation_7,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_1_modulation_7_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_1_modulation_8,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_1_modulation_8_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_1_modulation_9,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_1_modulation_9_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_1_modulation_10,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_1_modulation_10_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_1_modulation_11,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_1_modulation_11_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_1_modulation_12,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_1_modulation_12_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_1_modulation_13,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_1_modulation_13_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_1_modulation_14,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_1_modulation_14_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_1_modulation_15,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_1_modulation_15_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_1_pingpong,
		      "any_signal::toggled",
		      G_CALLBACK(ags_abyss_synth_seq_1_pingpong_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->seq_1_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_1_lfo_frequency_callback),
		      abyss_synth,
		      NULL);

  /* seq 2 */
  g_object_disconnect(abyss_synth->seq_2_modulation_0,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_2_modulation_0_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_2_modulation_1,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_2_modulation_1_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_2_modulation_2,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_2_modulation_2_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_2_modulation_3,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_2_modulation_3_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_2_modulation_4,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_2_modulation_4_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_2_modulation_5,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_2_modulation_5_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_2_modulation_6,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_2_modulation_6_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_2_modulation_7,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_2_modulation_7_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_2_modulation_8,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_2_modulation_8_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_2_modulation_9,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_2_modulation_9_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_2_modulation_10,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_2_modulation_10_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_2_modulation_11,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_2_modulation_11_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_2_modulation_12,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_2_modulation_12_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_2_modulation_13,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_2_modulation_13_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_2_modulation_14,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_2_modulation_14_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_2_modulation_15,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_2_modulation_15_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_2_pingpong,
		      "any_signal::toggled",
		      G_CALLBACK(ags_abyss_synth_seq_2_pingpong_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->seq_2_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_2_lfo_frequency_callback),
		      abyss_synth,
		      NULL);

  /* seq 3 */
  g_object_disconnect(abyss_synth->seq_3_modulation_0,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_3_modulation_0_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_3_modulation_1,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_3_modulation_1_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_3_modulation_2,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_3_modulation_2_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_3_modulation_3,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_3_modulation_3_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_3_modulation_4,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_3_modulation_4_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_3_modulation_5,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_3_modulation_5_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_3_modulation_6,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_3_modulation_6_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_3_modulation_7,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_3_modulation_7_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_3_modulation_8,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_3_modulation_8_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_3_modulation_9,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_3_modulation_9_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_3_modulation_10,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_3_modulation_10_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_3_modulation_11,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_3_modulation_11_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_3_modulation_12,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_3_modulation_12_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_3_modulation_13,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_3_modulation_13_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_3_modulation_14,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_3_modulation_14_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_3_modulation_15,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_3_modulation_15_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->seq_3_pingpong,
		      "any_signal::toggled",
		      G_CALLBACK(ags_abyss_synth_seq_3_pingpong_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->seq_3_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_seq_3_lfo_frequency_callback),
		      abyss_synth,
		      NULL);
  
  /* pink noise 0 */
  g_object_disconnect(abyss_synth->pink_noise_0_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_pink_noise_0_frequency_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->pink_noise_0_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_pink_noise_0_gain_callback),
		      abyss_synth,
		      NULL);

  /* pink noise 1 */
  g_object_disconnect(abyss_synth->pink_noise_1_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_pink_noise_1_frequency_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->pink_noise_1_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_pink_noise_1_gain_callback),
		      abyss_synth,
		      NULL);

  /* modulation matrix */
  ags_connectable_disconnect(AGS_CONNECTABLE(abyss_synth->modulation_matrix));

  g_object_disconnect(abyss_synth->modulation_matrix,
		      "any_signal::toggled",
		      G_CALLBACK(ags_abyss_synth_modulation_matrix_callback),
		      abyss_synth,
		      NULL);

  /* osc-0 */
  g_object_disconnect(abyss_synth->osc_0_oscillator,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_abyss_synth_osc_0_oscillator_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->osc_0_octave,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_osc_0_octave_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->osc_0_key,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_osc_0_key_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->osc_0_phase,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_osc_0_phase_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->osc_0_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_osc_0_volume_callback),
		      abyss_synth,
		      NULL);

  /* osc-1 */
  g_object_disconnect(abyss_synth->osc_1_oscillator,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_abyss_synth_osc_1_oscillator_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->osc_1_octave,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_osc_1_octave_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->osc_1_key,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_osc_1_key_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->osc_1_phase,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_osc_1_phase_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->osc_1_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_osc_1_volume_callback),
		      abyss_synth,
		      NULL);

  /* osc-2 */
  g_object_disconnect(abyss_synth->osc_2_oscillator,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_abyss_synth_osc_2_oscillator_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->osc_2_octave,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_osc_2_octave_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->osc_2_key,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_osc_2_key_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->osc_2_phase,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_osc_2_phase_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->osc_2_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_osc_2_volume_callback),
		      abyss_synth,
		      NULL);

  /* osc-3 */
  g_object_disconnect(abyss_synth->osc_3_oscillator,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_abyss_synth_osc_3_oscillator_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->osc_3_octave,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_osc_3_octave_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->osc_3_key,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_osc_3_key_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->osc_3_phase,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_osc_3_phase_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->osc_3_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_osc_3_volume_callback),
		      abyss_synth,
		      NULL);
  
  /* ring 0 */
  g_object_disconnect(abyss_synth->ring_0_enabled,
		      "any_signal::toggled",
		      G_CALLBACK(ags_abyss_synth_ring_0_enabled_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->ring_0_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_ring_0_tuning_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->ring_0_drive,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_ring_0_drive_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->ring_0_mix,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_ring_0_mix_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->ring_0_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_ring_0_gain_callback),
		      abyss_synth,
		      NULL);

  /* ring 1 */
  g_object_disconnect(abyss_synth->ring_1_enabled,
		      "any_signal::toggled",
		      G_CALLBACK(ags_abyss_synth_ring_1_enabled_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->ring_1_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_ring_1_tuning_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->ring_1_drive,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_ring_1_drive_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->ring_1_mix,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_ring_1_mix_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->ring_1_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_ring_1_gain_callback),
		      abyss_synth,
		      NULL);

  /* volume */
  g_object_disconnect(abyss_synth->volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_volume_callback),
		      abyss_synth,
		      NULL);

  /* pitch */
  g_object_disconnect(abyss_synth->pitch_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_pitch_tuning_callback),
		      abyss_synth,
		      NULL);

  /* low-pass 0 */
  g_object_disconnect(abyss_synth->low_pass_0_cut_off_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_low_pass_0_cut_off_frequency_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->low_pass_0_filter_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_low_pass_0_filter_gain_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->low_pass_0_no_clip,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_low_pass_0_no_clip_callback),
		      abyss_synth,
		      NULL);

  /* low-pass 1 */
  g_object_disconnect(abyss_synth->low_pass_1_cut_off_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_low_pass_1_cut_off_frequency_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->low_pass_1_filter_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_low_pass_1_filter_gain_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->low_pass_1_no_clip,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_low_pass_1_no_clip_callback),
		      abyss_synth,
		      NULL);

  /* amplifier 0 */  
  g_object_disconnect(abyss_synth->amplifier_0_amp_0_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_amplifier_0_amp_0_gain_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->amplifier_0_amp_1_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_amplifier_0_amp_1_gain_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->amplifier_0_amp_2_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_amplifier_0_amp_2_gain_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->amplifier_0_amp_3_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_amplifier_0_amp_3_gain_callback),
		      abyss_synth,
		      NULL);

  g_object_disconnect(abyss_synth->amplifier_0_filter_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_amplifier_0_filter_gain_callback),
		      abyss_synth,
		      NULL);

  /* chorus */
  g_object_disconnect(abyss_synth->chorus_input_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_chorus_input_volume_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->chorus_output_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_chorus_output_volume_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->chorus_lfo_oscillator,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_abyss_synth_chorus_lfo_oscillator_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->chorus_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_chorus_lfo_frequency_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->chorus_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_chorus_depth_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->chorus_mix,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_chorus_mix_callback),
		      abyss_synth,
		      NULL);
  
  g_object_disconnect(abyss_synth->chorus_delay,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_abyss_synth_chorus_delay_callback),
		      abyss_synth,
		      NULL);
}

void
ags_abyss_synth_resize_audio_channels(AgsMachine *machine,
				      guint audio_channels, guint audio_channels_old,
				      gpointer data)
{
  AgsAbyssSynth *abyss_synth;

  abyss_synth = (AgsAbyssSynth *) machine;

  if(audio_channels > audio_channels_old){    
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_abyss_synth_input_map_recall(abyss_synth,
				       audio_channels_old,
				       0);
      
      ags_abyss_synth_output_map_recall(abyss_synth,
					audio_channels_old,
					0);
    }
  }
}

void
ags_abyss_synth_resize_pads(AgsMachine *machine, GType type,
			    guint pads, guint pads_old,
			    gpointer data)
{
  AgsAbyssSynth *abyss_synth;

  gboolean grow;

  abyss_synth = (AgsAbyssSynth *) machine;
  
  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(type == AGS_TYPE_INPUT){
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	/* depending on destination */
	ags_abyss_synth_input_map_recall(abyss_synth,
					 0,
					 pads_old);
      }
    }else{
      abyss_synth->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_abyss_synth_output_map_recall(abyss_synth,
					  0,
					  pads_old);
      }
    }else{
      abyss_synth->mapped_output_pad = pads;
    }
  }
}

void
ags_abyss_synth_map_recall(AgsMachine *machine)
{
  AgsAbyssSynth *abyss_synth;
  
  AgsAudio *audio;

  GList *start_recall, *recall;

  gint position;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  abyss_synth = AGS_ABYSS_SYNTH(machine);

  audio = machine->audio;
  
  position = 0;

  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       abyss_synth->playback_play_container, abyss_synth->playback_recall_container,
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

  /* ags-fx-abyss-synth */
  start_recall = ags_fx_factory_create(audio,
				       abyss_synth->abyss_synth_play_container, abyss_synth->abyss_synth_recall_container,
				       "ags-fx-abyss-synth",
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
				       abyss_synth->volume_play_container, abyss_synth->volume_recall_container,
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
				       abyss_synth->envelope_play_container, abyss_synth->envelope_recall_container,
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
				       abyss_synth->buffer_play_container, abyss_synth->buffer_recall_container,
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
  ags_abyss_synth_input_map_recall(abyss_synth,
				   0,
				   0);

  /* depending on destination */
  ags_abyss_synth_output_map_recall(abyss_synth,
				    0,
				    0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_abyss_synth_parent_class)->map_recall(machine);  
}

void
ags_abyss_synth_input_map_recall(AgsAbyssSynth *abyss_synth,
				 guint audio_channel_start,
				 guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall, *recall;

  gint position;
  guint input_pads;
  guint audio_channels;

  if(abyss_synth->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

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
				       abyss_synth->playback_play_container, abyss_synth->playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-abyss-synth */
  start_recall = ags_fx_factory_create(audio,
				       abyss_synth->abyss_synth_play_container, abyss_synth->abyss_synth_recall_container,
				       "ags-fx-abyss-synth",
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
				       abyss_synth->volume_play_container, abyss_synth->volume_recall_container,
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
				       abyss_synth->envelope_play_container, abyss_synth->envelope_recall_container,
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
				       abyss_synth->buffer_play_container, abyss_synth->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       audio_channel_start, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  abyss_synth->mapped_input_pad = input_pads;
}

void
ags_abyss_synth_output_map_recall(AgsAbyssSynth *abyss_synth,
				  guint audio_channel_start,
				  guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;

  if(abyss_synth->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(abyss_synth)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);
  
  abyss_synth->mapped_output_pad = output_pads;
}

void
ags_abyss_synth_refresh_port(AgsMachine *machine)
{
  AgsAbyssSynth *abyss_synth;
  
  GList *start_play, *start_recall, *recall;

  guint nth_sends;
  guint i, j;

  const gchar const * sends_strv[] = {
    "synth-0-env-0-sends",
    "synth-0-env-1-sends",
    "synth-0-env-2-sends",
    "synth-0-env-3-sends",
    "synth-0-lfo-0-sends",
    "synth-0-lfo-1-sends",
    "synth-0-lfo-2-sends",
    "synth-0-lfo-3-sends",
    "synth-0-noise-sends",
    NULL
  };
  
  abyss_synth = (AgsAbyssSynth *) machine;
  
  start_play = ags_audio_get_play(machine->audio);
  start_recall = ags_audio_get_recall(machine->audio);

  recall =
    start_recall = g_list_concat(start_play, start_recall);

  machine->flags |= AGS_MACHINE_NO_UPDATE;

  if((recall = ags_recall_find_type(recall, AGS_TYPE_FX_ABYSS_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    //TODO:JK: implement me
  }
  
  machine->flags &= (~AGS_MACHINE_NO_UPDATE);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_abyss_synth_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsAbyssSynth
 *
 * Returns: the new #AgsAbyssSynth
 *
 * Since: 9.4.0
 */
AgsAbyssSynth*
ags_abyss_synth_new(GObject *soundcard)
{
  AgsAbyssSynth *abyss_synth;

  abyss_synth = (AgsAbyssSynth *) g_object_new(AGS_TYPE_ABYSS_SYNTH,
					       NULL);

  g_object_set(AGS_MACHINE(abyss_synth)->audio,
	       "output-soundcard", soundcard,
	       NULL);

  return(abyss_synth);
}
