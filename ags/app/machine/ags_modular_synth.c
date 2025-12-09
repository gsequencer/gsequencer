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

  /* lfo-0 */
  lfo_0_grid = (GtkGrid *) gtk_grid_new();
  gtk_widget_set_halign((GtkWidget *) lfo_0_grid,
			GTK_ALIGN_START);
  gtk_grid_attach(modular_synth_grid,
		  (GtkWidget *) lfo_0_grid,
		  0, 1,
		  1, 1);

  /* lfo-1 */
  lfo_1_grid = (GtkGrid *) gtk_grid_new();
  gtk_widget_set_halign((GtkWidget *) lfo_1_grid,
			GTK_ALIGN_START);
  gtk_grid_attach(modular_synth_grid,
		  (GtkWidget *) lfo_1_grid,
		  1, 1,
		  1, 1);

  //TODO:JK: implement me
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

  //TODO:JK: implement me
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

  //TODO:JK: implement me
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

  modular_synth = (AgsModularSynth *) machine;
  
  start_play = ags_audio_get_play(machine->audio);
  start_recall = ags_audio_get_recall(machine->audio);

  recall =
    start_recall = g_list_concat(start_play, start_recall);

  machine->flags |= AGS_MACHINE_NO_UPDATE;

  if((recall = ags_recall_find_type(recall, AGS_TYPE_FX_MODULAR_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    //TODO:JK: implement me
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
