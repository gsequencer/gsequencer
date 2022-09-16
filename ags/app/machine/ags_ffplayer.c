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

#include <ags/app/machine/ags_ffplayer.h>
#include <ags/app/machine/ags_ffplayer_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_navigation.h>

#include <ags/app/machine/ags_ffplayer_bridge.h>

#include <math.h>

#include <ags/i18n.h>

void ags_ffplayer_class_init(AgsFFPlayerClass *ffplayer);
void ags_ffplayer_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_ffplayer_init(AgsFFPlayer *ffplayer);
void ags_ffplayer_finalize(GObject *gobject);

void ags_ffplayer_connect(AgsConnectable *connectable);
void ags_ffplayer_disconnect(AgsConnectable *connectable);

void ags_ffplayer_realize(GtkWidget *widget);

void ags_ffplayer_resize_audio_channels(AgsMachine *machine,
				     guint audio_channels, guint audio_channels_old,
				     gpointer data);
void ags_ffplayer_resize_pads(AgsMachine *machine, GType type,
			      guint pads, guint pads_old,
			      gpointer data);

void ags_ffplayer_map_recall(AgsMachine *machine);
void ags_ffplayer_output_map_recall(AgsFFPlayer *ffplayer,
				    guint audio_channel_start,
				    guint output_pad_start);
void ags_ffplayer_input_map_recall(AgsFFPlayer *ffplayer,
				   guint audio_channel_start,
				   guint input_pad_start);

void ags_ffplayer_apply_sf2_synth_launch_callback(AgsTask *task,
						  AgsFFPlayer *ffplayer);

/**
 * SECTION:ags_ffplayer
 * @short_description: ffplayer notation
 * @title: AgsFFPlayer
 * @section_id:
 * @include: ags/app/machine/ags_ffplayer.h
 *
 * The #AgsFFPlayer is a composite widget to act as soundfont2 notation player.
 */

static gpointer ags_ffplayer_parent_class = NULL;
static AgsConnectableInterface *ags_ffplayer_parent_connectable_interface;

GHashTable *ags_ffplayer_sf2_loader_completed = NULL;

GType
ags_ffplayer_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_ffplayer = 0;

    static const GTypeInfo ags_ffplayer_info = {
      sizeof(AgsFFPlayerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ffplayer_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFFPlayer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ffplayer_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ffplayer_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ffplayer = g_type_register_static(AGS_TYPE_MACHINE,
					       "AgsFFPlayer", &ags_ffplayer_info,
					       0);
    
    g_type_add_interface_static(ags_type_ffplayer,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_ffplayer);
  }

  return g_define_type_id__volatile;
}

void
ags_ffplayer_class_init(AgsFFPlayerClass *ffplayer)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  AgsMachineClass *machine;

  ags_ffplayer_parent_class = g_type_class_peek_parent(ffplayer);

  /* GObjectClass */
  gobject = (GObjectClass *) ffplayer;

  gobject->finalize = ags_ffplayer_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) ffplayer;

  widget->realize = ags_ffplayer_realize;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) ffplayer;

  machine->map_recall = ags_ffplayer_map_recall;
}

void
ags_ffplayer_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_ffplayer_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_ffplayer_connect;
  connectable->disconnect = ags_ffplayer_disconnect;
}

void
ags_ffplayer_init(AgsFFPlayer *ffplayer)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  GtkBox *vbox;
  GtkGrid *grid;
  GtkBox *hbox;
  GtkBox *filename_hbox;
  GtkBox *piano_vbox;
  GtkBox *synth_generator_vbox;
  GtkBox *aliase_hbox;
  GtkBox *volume_hbox;
  GtkBox *pitch_hbox;
  GtkBox *base_note_hbox;
  GtkBox *key_count_hbox;
  GtkFrame *frame;
  GtkLabel *label;

  GtkAdjustment *adjustment;
  
  GtkAllocation allocation;
  
  AgsAudio *audio;
  AgsSF2SynthGenerator *sf2_synth_generator;
  
  AgsConfig *config;

  AgsMachineCounterManager *machine_counter_manager;
  AgsMachineCounter *machine_counter;

  AgsApplicationContext *application_context;
  
  gchar *machine_name;
  gchar *str;
  
  gint position;
  gdouble gui_scale_factor;
  gdouble page, step;

  machine_counter_manager = ags_machine_counter_manager_get_instance();

  application_context = ags_application_context_get_instance();
  
  /* machine counter */
  machine_counter = ags_machine_counter_manager_find_machine_counter(machine_counter_manager,
								     AGS_TYPE_FFPLAYER);

  machine_name = NULL;

  if(machine_counter != NULL){
    machine_name = g_strdup_printf("Default %d",
				   machine_counter->counter);
  
    ags_machine_counter_increment(machine_counter);
  }
  
  g_object_set(ffplayer,
	       "machine-name", machine_name,
	       NULL);

  g_free(machine_name);

  /* machine selector */
  window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  position = g_list_length(window->machine);
  
  ags_machine_selector_popup_insert_machine(composite_editor->machine_selector,
					    position,
					    ffplayer);

  audio = AGS_MACHINE(ffplayer)->audio;
  ags_audio_set_flags(audio, (AGS_AUDIO_SYNC |
			      AGS_AUDIO_ASYNC |
			      AGS_AUDIO_OUTPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_FILE));
  ags_audio_set_ability_flags(audio, (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_NOTATION));
  ags_audio_set_behaviour_flags(audio, (AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING |
					AGS_SOUND_BEHAVIOUR_DEFAULTS_TO_INPUT));

  config = ags_config_get_instance();
  
  /* scale factor */
  gui_scale_factor = 1.0;
  
  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "gui-scale");

  if(str != NULL){
    gui_scale_factor = g_ascii_strtod(str,
				      NULL);

    g_free(str);
  }

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

  sf2_synth_generator = ags_sf2_synth_generator_new();
  ags_sf2_synth_generator_set_flags(sf2_synth_generator,
				    AGS_SF2_SYNTH_GENERATOR_COMPUTE_INSTRUMENT);
  
  ags_audio_add_sf2_synth_generator(audio,
				    (GObject *) sf2_synth_generator);

  AGS_MACHINE(ffplayer)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
				   AGS_MACHINE_REVERSE_NOTATION);
  AGS_MACHINE(ffplayer)->file_input_flags |= AGS_MACHINE_ACCEPT_SOUNDFONT2;

  AGS_MACHINE(ffplayer)->input_pad_type = G_TYPE_NONE;
  AGS_MACHINE(ffplayer)->input_line_type = G_TYPE_NONE;
  AGS_MACHINE(ffplayer)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(ffplayer)->output_line_type = G_TYPE_NONE;

  /* audio resize */
  g_signal_connect_after(G_OBJECT(ffplayer), "resize-audio-channels",
			 G_CALLBACK(ags_ffplayer_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(ffplayer), "resize-pads",
			 G_CALLBACK(ags_ffplayer_resize_pads), NULL);

  /* flags */
  ffplayer->flags = 0;

  /* mapped IO */
  ffplayer->mapped_input_pad = 0;
  ffplayer->mapped_output_pad = 0;

  ffplayer->playback_play_container = ags_recall_container_new();
  ffplayer->playback_recall_container = ags_recall_container_new();

  ffplayer->notation_play_container = ags_recall_container_new();
  ffplayer->notation_recall_container = ags_recall_container_new();

  ffplayer->two_pass_aliase_play_container = ags_recall_container_new();
  ffplayer->two_pass_aliase_recall_container = ags_recall_container_new();

  ffplayer->volume_play_container = ags_recall_container_new();
  ffplayer->volume_recall_container = ags_recall_container_new();

  ffplayer->envelope_play_container = ags_recall_container_new();
  ffplayer->envelope_recall_container = ags_recall_container_new();

  ffplayer->buffer_play_container = ags_recall_container_new();
  ffplayer->buffer_recall_container = ags_recall_container_new();
  
  /* name and xml type */
  ffplayer->name = NULL;
  ffplayer->xml_type = "ags-ffplayer";

  /* audio container */
  ffplayer->audio_container = NULL;

  /* create widgets */
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);

  gtk_widget_set_valign(vbox,
			GTK_ALIGN_START);  
  gtk_widget_set_halign(vbox,
			GTK_ALIGN_START);

  gtk_widget_set_hexpand(vbox,
			 FALSE);

  gtk_box_set_spacing(vbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_frame_set_child(AGS_MACHINE(ffplayer)->frame,
		      (GtkWidget *) vbox);
  
  grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);
  
  gtk_box_append(vbox,
		 (GtkWidget *) grid);
  
  /* preset and instrument */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);

  gtk_box_set_spacing(hbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_valign((GtkWidget *) hbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) hbox,
			GTK_ALIGN_FILL);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) hbox,
		  0, 1,
		  1, 1);

  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("preset"),
				    "xalign", 0.0,
				    NULL);
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  ffplayer->preset = (GtkComboBox *) gtk_combo_box_text_new();
  gtk_box_append(hbox,
		 (GtkWidget *) ffplayer->preset);

  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("instrument"),
				    "xalign", 0.0,
				    NULL);
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  ffplayer->instrument = (GtkComboBox *) gtk_combo_box_text_new();
  gtk_box_append(hbox,
		 (GtkWidget *) ffplayer->instrument);

  /* filename */
  filename_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					 0);

  gtk_box_set_spacing(filename_hbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(hbox,
		 (GtkWidget *) filename_hbox);

#if 0
  ffplayer->filename = (GtkEntry *) gtk_entry_new();
  gtk_box_append(filename_hbox,
		 (GtkWidget *) ffplayer->filename);
#else
  ffplayer->filename = NULL;
#endif
  
  ffplayer->open = (GtkButton *) gtk_button_new_with_mnemonic(i18n("_Open"));
  gtk_box_append(filename_hbox,
		 (GtkWidget *) ffplayer->open);

  g_atomic_int_set(&(ffplayer->apply_sf2_synth_completed),
		   TRUE);
  
  ffplayer->sf2_loader = NULL;

  ffplayer->load_preset = NULL;
  ffplayer->load_instrument = NULL;

  ffplayer->position = -1;

  ffplayer->sf2_loader_spinner = (GtkSpinner *) gtk_spinner_new();
  gtk_box_append(filename_hbox,
		 (GtkWidget *) ffplayer->sf2_loader_spinner);
  gtk_widget_hide((GtkWidget *) ffplayer->sf2_loader_spinner);

  /* piano */
  piano_vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				      2);

  gtk_box_set_spacing(piano_vbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_valign((GtkWidget *) piano_vbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) piano_vbox,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) piano_vbox,
		  1, 0,
		  1, 3);
  
  ffplayer->control_width = (guint) (gui_scale_factor * AGS_FFPLAYER_DEFAULT_CONTROL_WIDTH);
  ffplayer->control_height = (guint) (gui_scale_factor * AGS_FFPLAYER_DEFAULT_CONTROL_HEIGHT);

  ffplayer->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_size_request((GtkWidget *) ffplayer->drawing_area,
			      16 * ffplayer->control_width,
			      ffplayer->control_width * 8 + ffplayer->control_height);
  gtk_box_append(piano_vbox,
		 (GtkWidget *) ffplayer->drawing_area);

  gtk_widget_get_allocation(GTK_WIDGET(ffplayer->drawing_area), &allocation);
  
  ffplayer->hadjustment = (GtkAdjustment *) gtk_adjustment_new(0.0,
							       0.0,
							       76 * ffplayer->control_width - allocation.width,
							       1.0,
							       (double) ffplayer->control_width,
							       (double) (16 * ffplayer->control_width));
  ffplayer->hscrollbar = (GtkScrollbar *) gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL,
							    ffplayer->hadjustment);
  gtk_box_append(piano_vbox,
		 (GtkWidget *) ffplayer->hscrollbar);

  /* synth generator */
  frame = (GtkFrame *) gtk_frame_new(i18n("synth generator"));

  gtk_widget_set_valign((GtkWidget *) frame,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) frame,
			GTK_ALIGN_FILL);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) frame,
		  2, 0,
		  1, 3);

  synth_generator_vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						0);

  gtk_box_set_spacing(synth_generator_vbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_frame_set_child(frame,
		      (GtkWidget *) synth_generator_vbox);
  
  ffplayer->enable_synth_generator = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));
  gtk_box_append(synth_generator_vbox,
		 (GtkWidget *) ffplayer->enable_synth_generator);

  /* pitch function */
  pitch_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				      0);

  gtk_box_set_spacing(pitch_hbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(synth_generator_vbox,
		 (GtkWidget *) pitch_hbox);

  label = (GtkLabel *) gtk_label_new(i18n("pitch"));
  gtk_box_append(pitch_hbox,
		 (GtkWidget *) label);
  
  ffplayer->pitch_function = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text((GtkComboBoxText *) ffplayer->pitch_function,
				 "ags-fast-pitch");
  
  gtk_combo_box_text_append_text((GtkComboBoxText *) ffplayer->pitch_function,
				 "ags-hq-pitch");

  gtk_combo_box_text_append_text((GtkComboBoxText *) ffplayer->pitch_function,
				 "fluid-no-interpolate");

  gtk_combo_box_text_append_text((GtkComboBoxText *) ffplayer->pitch_function,
				 "fluid-linear-interpolate");

  gtk_combo_box_text_append_text((GtkComboBoxText *) ffplayer->pitch_function,
				 "fluid-4th-order-interpolate");

  gtk_combo_box_text_append_text((GtkComboBoxText *) ffplayer->pitch_function,
				 "fluid-7th-order-interpolate");

  gtk_combo_box_set_active(ffplayer->pitch_function,
			   4);
  
  gtk_box_append(pitch_hbox,
		 (GtkWidget *) ffplayer->pitch_function);
  
  /* base note */
  base_note_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					  0);

  gtk_box_set_spacing(base_note_hbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(synth_generator_vbox,
		 (GtkWidget *) base_note_hbox);

  label = (GtkLabel *) gtk_label_new(i18n("lower"));
  gtk_box_append(base_note_hbox,
		 (GtkWidget *) label);

  ffplayer->lower = (GtkSpinButton *) gtk_spin_button_new_with_range(-70.0,
								     70.0,
								     1.0);
  gtk_spin_button_set_digits(ffplayer->lower,
			     2);
  gtk_spin_button_set_value(ffplayer->lower,
			    -48.0);
  gtk_box_append(base_note_hbox,
		 (GtkWidget *) ffplayer->lower);

  /* key count */
  key_count_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					  0);

  gtk_box_set_spacing(key_count_hbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(synth_generator_vbox,
		 (GtkWidget *) key_count_hbox);

  label = (GtkLabel *) gtk_label_new(i18n("key count"));
  gtk_box_append(key_count_hbox,
		 (GtkWidget *) label);

  ffplayer->key_count = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									 128.0,
									 1.0);
  gtk_spin_button_set_value(ffplayer->key_count,
			    78.0);
  gtk_box_append(key_count_hbox,
		 (GtkWidget *) ffplayer->key_count);

  /* update */
  ffplayer->update = (GtkButton *) gtk_button_new_with_label(i18n("update"));

  gtk_widget_set_valign((GtkWidget *) ffplayer->update,
			(GTK_ALIGN_FILL | GTK_ALIGN_END));
  gtk_widget_set_halign((GtkWidget *) ffplayer->update,
			GTK_ALIGN_FILL);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) ffplayer->update,
		  3, 0,
		  1, 3);
  
  /* aliase */
  frame = (GtkFrame *) gtk_frame_new(i18n("aliase"));

  gtk_widget_set_valign((GtkWidget *) frame,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) frame,
			GTK_ALIGN_FILL);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) frame,
		  4, 0,
		  1, 1);

  aliase_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				       0);

  gtk_widget_set_hexpand(aliase_hbox,
			 FALSE);

  gtk_box_set_spacing(aliase_hbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_frame_set_child(frame,
		      (GtkWidget *) aliase_hbox);
  
  ffplayer->enable_aliase = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));
  gtk_box_append(aliase_hbox,
		 (GtkWidget *) ffplayer->enable_aliase);

  step = 2.0 * M_PI / 100.0;
  page = 2.0 * M_PI / AGS_DIAL_DEFAULT_PRECISION;

  /* aliase a */
  ffplayer->aliase_a_amount = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(ffplayer->aliase_a_amount);

  gtk_adjustment_set_step_increment(adjustment,
				    step);
  gtk_adjustment_set_page_increment(adjustment,
				    page);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   2.0 * M_PI);
  
  gtk_box_append(aliase_hbox,
		 (GtkWidget *) ffplayer->aliase_a_amount);

  ffplayer->aliase_a_phase = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(ffplayer->aliase_a_phase);

  gtk_adjustment_set_step_increment(adjustment,
				    step);
  gtk_adjustment_set_page_increment(adjustment,
				    page);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   2.0 * M_PI);
  
  gtk_box_append(aliase_hbox,
		 (GtkWidget *) ffplayer->aliase_a_phase);

  /* aliase b */
  ffplayer->aliase_b_amount = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(ffplayer->aliase_b_amount);

  gtk_adjustment_set_step_increment(adjustment,
				    step);
  gtk_adjustment_set_page_increment(adjustment,
				    page);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   2.0 * M_PI);
  
  gtk_box_append(aliase_hbox,
		 (GtkWidget *) ffplayer->aliase_b_amount);

  ffplayer->aliase_b_phase = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(ffplayer->aliase_b_phase);

  gtk_adjustment_set_step_increment(adjustment,
				    step);
  gtk_adjustment_set_page_increment(adjustment,
				    page);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   2.0 * M_PI);
  
  gtk_box_append(aliase_hbox,
		 (GtkWidget *) ffplayer->aliase_b_phase);

  /* volume */
  frame = (GtkFrame *) gtk_frame_new(i18n("volume"));

  gtk_widget_set_valign((GtkWidget *) frame,
			GTK_ALIGN_START);
  gtk_widget_set_halign((GtkWidget *) frame,
			GTK_ALIGN_START);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) frame,
		  5, 0,
		  1, 3);

  volume_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				       0);

  gtk_widget_set_hexpand(volume_hbox,
			 FALSE);

  gtk_box_set_spacing(volume_hbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_frame_set_child(frame,
		      (GtkWidget *) volume_hbox);
  
  ffplayer->volume = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
							   0.0,
							   2.0,
							   0.025);
  
  gtk_widget_set_size_request(ffplayer->volume,
			      (gint) (gui_scale_factor * 16.0), (gint) (gui_scale_factor * 100.0));
  
  gtk_box_append(volume_hbox,
		 (GtkWidget *) ffplayer->volume);

  gtk_scale_set_digits(ffplayer->volume,
		       3);

  gtk_range_set_increments(GTK_RANGE(ffplayer->volume),
			   0.025, 0.1);
  gtk_range_set_value(GTK_RANGE(ffplayer->volume),
		      1.0);
  gtk_range_set_inverted(GTK_RANGE(ffplayer->volume),
			 TRUE);  

  /* effect bridge */
  AGS_MACHINE(ffplayer)->bridge = ags_ffplayer_bridge_new(audio);

  AGS_EFFECT_BRIDGE(AGS_MACHINE(ffplayer)->bridge)->parent_machine = ffplayer;
  
  gtk_box_append(vbox,
		 (GtkWidget *) AGS_MACHINE(ffplayer)->bridge);

  /* dialog */
  ffplayer->open_dialog = NULL;

  /* SF2 loader */
  if(ags_ffplayer_sf2_loader_completed == NULL){
    ags_ffplayer_sf2_loader_completed = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							      NULL,
							      NULL);
  }

  g_hash_table_insert(ags_ffplayer_sf2_loader_completed,
		      ffplayer, ags_ffplayer_sf2_loader_completed_timeout);
  g_timeout_add(1000 / 4, (GSourceFunc) ags_ffplayer_sf2_loader_completed_timeout, (gpointer) ffplayer);
}

void
ags_ffplayer_finalize(GObject *gobject)
{
  g_hash_table_remove(ags_ffplayer_sf2_loader_completed,
		      gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_ffplayer_parent_class)->finalize(gobject);
}

void
ags_ffplayer_connect(AgsConnectable *connectable)
{
  AgsFFPlayer *ffplayer;

  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) != 0){
    return;
  }

  ags_ffplayer_parent_connectable_interface->connect(connectable);

  ffplayer = AGS_FFPLAYER(connectable);
  
  g_signal_connect((GObject *) ffplayer, "destroy",
		   G_CALLBACK(ags_ffplayer_destroy_callback), (gpointer) ffplayer);
  
  /* AgsFFPlayer */
  g_signal_connect((GObject *) ffplayer->open, "clicked",
		   G_CALLBACK(ags_ffplayer_open_clicked_callback), (gpointer) ffplayer);

  g_signal_connect_after((GObject *) ffplayer->preset, "changed",
			 G_CALLBACK(ags_ffplayer_preset_changed_callback), (gpointer) ffplayer);

  g_signal_connect_after((GObject *) ffplayer->instrument, "changed",
			 G_CALLBACK(ags_ffplayer_instrument_changed_callback), (gpointer) ffplayer);

  gtk_drawing_area_set_draw_func(ffplayer->drawing_area,
				 ags_ffplayer_draw_callback,
				 ffplayer,
				 NULL);

  g_signal_connect((GObject *) ffplayer->hadjustment, "value_changed",
		   G_CALLBACK(ags_ffplayer_hscrollbar_value_changed), (gpointer) ffplayer);

  g_signal_connect((GObject *) ffplayer->update, "clicked",
		   G_CALLBACK(ags_ffplayer_update_callback), (gpointer) ffplayer);

  g_signal_connect((GObject *) ffplayer->enable_aliase, "toggled",
		   G_CALLBACK(ags_ffplayer_enable_aliase_callback), (gpointer) ffplayer);

  g_signal_connect((GObject *) ffplayer->aliase_a_amount, "value-changed",
		   G_CALLBACK(ags_ffplayer_aliase_a_amount_callback), (gpointer) ffplayer);

  g_signal_connect((GObject *) ffplayer->aliase_a_phase, "value-changed",
		   G_CALLBACK(ags_ffplayer_aliase_a_phase_callback), (gpointer) ffplayer);

  g_signal_connect((GObject *) ffplayer->aliase_b_amount, "value-changed",
		   G_CALLBACK(ags_ffplayer_aliase_b_amount_callback), (gpointer) ffplayer);

  g_signal_connect((GObject *) ffplayer->aliase_b_phase, "value-changed",
		   G_CALLBACK(ags_ffplayer_aliase_b_phase_callback), (gpointer) ffplayer);

  g_signal_connect((GObject *) ffplayer->volume, "value-changed",
		   G_CALLBACK(ags_ffplayer_volume_callback), (gpointer) ffplayer);
}

void
ags_ffplayer_disconnect(AgsConnectable *connectable)
{
  AgsFFPlayer *ffplayer;

  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) == 0){
    return;
  }

  ags_ffplayer_parent_connectable_interface->disconnect(connectable);

  ffplayer = AGS_FFPLAYER(connectable);

  g_object_disconnect((GObject *) ffplayer,
		      "any_signal::destroy",
		      G_CALLBACK(ags_ffplayer_destroy_callback),
		      (gpointer) ffplayer,
		      NULL);
  
  /* AgsFFPlayer */
  g_object_disconnect((GObject *) ffplayer->open,
		      "any_signal::clicked",
		      G_CALLBACK(ags_ffplayer_open_clicked_callback),
		      (gpointer) ffplayer,
		      NULL);

  g_object_disconnect((GObject *) ffplayer->preset,
		      "any_signal::changed",
		      G_CALLBACK(ags_ffplayer_preset_changed_callback),
		      (gpointer) ffplayer,
		      NULL);

  g_object_disconnect((GObject *) ffplayer->instrument,
		      "any_signal::changed",
		      G_CALLBACK(ags_ffplayer_instrument_changed_callback),
		      (gpointer) ffplayer,
		      NULL);

  gtk_drawing_area_set_draw_func(ffplayer->drawing_area,
				 NULL,
				 NULL,
				 NULL);

  g_object_disconnect((GObject *) ffplayer->hadjustment,
		      "any_signal::value_changed",
		      G_CALLBACK(ags_ffplayer_hscrollbar_value_changed),
		      (gpointer) ffplayer,
		      NULL);

  g_object_disconnect((GObject *) ffplayer->update,
		      "any_signal::clicked",
		      G_CALLBACK(ags_ffplayer_update_callback),
		      (gpointer) ffplayer,
		      NULL);

  g_object_disconnect((GObject *) ffplayer->enable_aliase,
		      "any_signal::toggled",
		      G_CALLBACK(ags_ffplayer_enable_aliase_callback),
		      (gpointer) ffplayer,
		      NULL);

  g_object_disconnect((GObject *) ffplayer->aliase_a_amount,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_ffplayer_aliase_a_amount_callback),
		      (gpointer) ffplayer,
		      NULL);

  g_object_disconnect((GObject *) ffplayer->aliase_a_phase,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_ffplayer_aliase_a_phase_callback),
		      (gpointer) ffplayer,
		      NULL);

  g_object_disconnect((GObject *) ffplayer->aliase_b_amount,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_ffplayer_aliase_b_amount_callback),
		      (gpointer) ffplayer,
		      NULL);

  g_object_disconnect((GObject *) ffplayer->aliase_b_phase,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_ffplayer_aliase_b_phase_callback),
		      (gpointer) ffplayer,
		      NULL);

  g_object_disconnect((GObject *) ffplayer->volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_ffplayer_volume_callback),
		      (gpointer) ffplayer,
		      NULL);
}

void
ags_ffplayer_realize(GtkWidget *widget)
{
  AgsFFPlayer *ffplayer;

  ffplayer = (AgsFFPlayer *) widget;
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_ffplayer_parent_class)->realize(widget);
}

void
ags_ffplayer_resize_audio_channels(AgsMachine *machine,
				   guint audio_channels, guint audio_channels_old,
				   gpointer data)
{
  AgsFFPlayer *ffplayer;

  ffplayer = (AgsFFPlayer *) machine;

  /*  */
  if(audio_channels > audio_channels_old){  
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_ffplayer_input_map_recall(ffplayer,
				    audio_channels_old,
				    0);

      ags_ffplayer_output_map_recall(ffplayer,
				     audio_channels_old,
				     0);
    }
  }
}

void
ags_ffplayer_resize_pads(AgsMachine *machine, GType channel_type,
			 guint pads, guint pads_old,
			 gpointer data)
{
  AgsFFPlayer *ffplayer;

  gboolean grow;
  
  if(pads_old == pads){
    return;
  }
    
  ffplayer = (AgsFFPlayer *) machine;

  /* check grow */
  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(g_type_is_a(channel_type, AGS_TYPE_INPUT)){
    if(grow){
      /* depending on destination */
      ags_ffplayer_input_map_recall(ffplayer,
				    0,
				    pads_old);
    }else{
      ffplayer->mapped_input_pad = pads;
    }
  }else if(g_type_is_a(channel_type, AGS_TYPE_OUTPUT)){
    if(grow){      
      /* depending on destination */
      ags_ffplayer_output_map_recall(ffplayer,
				     0,
				     pads_old);
    }else{
      ffplayer->mapped_output_pad = pads;
    }
  }else{
    g_critical("unknown channel type");
  }
}

void
ags_ffplayer_map_recall(AgsMachine *machine)
{
  AgsNavigation *navigation;
  AgsFFPlayer *ffplayer;
  
  AgsAudio *audio;

  AgsApplicationContext *application_context;

  GList *start_recall, *recall;

  gint position;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  application_context = ags_application_context_get_instance();

  navigation = (AgsNavigation *) ags_ui_provider_get_navigation(AGS_UI_PROVIDER(application_context));

  ffplayer = AGS_FFPLAYER(machine);

  audio = machine->audio;
  
  position = 0;
  
  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       ffplayer->playback_play_container, ffplayer->playback_recall_container,
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
				       ffplayer->notation_play_container, ffplayer->notation_recall_container,
				       "ags-fx-notation",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-two-pass-aliase */
  start_recall = ags_fx_factory_create(audio,
				       ffplayer->two_pass_aliase_play_container, ffplayer->two_pass_aliase_recall_container,
				       "ags-fx-two-pass-aliase",
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
				       ffplayer->volume_play_container, ffplayer->volume_recall_container,
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
				       ffplayer->envelope_play_container, ffplayer->envelope_recall_container,
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
				       ffplayer->buffer_play_container, ffplayer->buffer_recall_container,
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
  ags_ffplayer_input_map_recall(ffplayer,
				0,
				0);

  /* depending on destination */
  ags_ffplayer_output_map_recall(ffplayer,
				 0,
				 0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_ffplayer_parent_class)->map_recall(machine);  
}

void
ags_ffplayer_input_map_recall(AgsFFPlayer *ffplayer,
			      guint audio_channel_start,
			      guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  gint position;
  guint input_pads;
  guint audio_channels;
  guint i;
  guint j;

  audio = AGS_MACHINE(ffplayer)->audio;

  position = 0;

  input_pads = 0;
  audio_channels = 0;
  
  /* get some fields */
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);
  
  for(i = 0; i < input_pads; i++){
    for(j = 0; j < audio_channels; j++){
      AgsMachineInputLine *input_line;

      input_line = g_list_nth_data(AGS_MACHINE(ffplayer)->machine_input_line,
				   (i * audio_channels) + j);

      if(input_line != NULL &&
	 input_line->mapped_recall == FALSE){
	/* ags-fx-playback */
	start_recall = ags_fx_factory_create(audio,
					     ffplayer->playback_play_container, ffplayer->playback_recall_container,
					     "ags-fx-playback",
					     NULL,
					     NULL,
					     j, j + 1,
					     i, i + 1,
					     position,
					     (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

	g_list_free_full(start_recall,
			 (GDestroyNotify) g_object_unref);

	/* ags-fx-notation */
	start_recall = ags_fx_factory_create(audio,
					     ffplayer->notation_play_container, ffplayer->notation_recall_container,
					     "ags-fx-notation",
					     NULL,
					     NULL,
					     j, j + 1,
					     i, i + 1,
					     position,
					     (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

	g_list_free_full(start_recall,
			 (GDestroyNotify) g_object_unref);

	/* ags-fx-two-pass-aliase */
	start_recall = ags_fx_factory_create(audio,
					     ffplayer->two_pass_aliase_play_container, ffplayer->two_pass_aliase_recall_container,
					     "ags-fx-two-pass-aliase",
					     NULL,
					     NULL,
					     j, j + 1,
					     i, i + 1,
					     position,
					     (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

	g_list_free_full(start_recall,
			 (GDestroyNotify) g_object_unref);

	/* ags-fx-volume */
	start_recall = ags_fx_factory_create(audio,
					     ffplayer->volume_play_container, ffplayer->volume_recall_container,
					     "ags-fx-volume",
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
					     ffplayer->envelope_play_container, ffplayer->envelope_recall_container,
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
					     ffplayer->buffer_play_container, ffplayer->buffer_recall_container,
					     "ags-fx-buffer",
					     NULL,
					     NULL,
					     j, j + 1,
					     i, i + 1,
					     position,
					     (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);
  
	g_list_free_full(start_recall,
			 (GDestroyNotify) g_object_unref);

	/* now input line is mapped */
	input_line->mapped_recall = TRUE;	
      }
    }
  }

  ffplayer->mapped_input_pad = input_pads;
}

void
ags_ffplayer_output_map_recall(AgsFFPlayer *ffplayer,
			       guint audio_channel_start,
			       guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;

  if(ffplayer->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(ffplayer)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);
  
  ffplayer->mapped_output_pad = output_pads;
}

void
ags_ffplayer_apply_sf2_synth_launch_callback(AgsTask *task,
					     AgsFFPlayer *ffplayer)
{
  g_atomic_int_set(&(ffplayer->apply_sf2_synth_completed),
		   TRUE);
}

void
ags_ffplayer_open_filename(AgsFFPlayer *ffplayer,
			   gchar *filename)
{
  AgsSF2Loader *sf2_loader;

  if(!AGS_IS_FFPLAYER(ffplayer) ||
     filename == NULL){
    return;
  }
  
  ffplayer->sf2_loader = 
    sf2_loader = ags_sf2_loader_new(AGS_MACHINE(ffplayer)->audio,
				    filename,
				    NULL,
				    NULL);
//  ags_sf2_loader_set_flags(sf2_loader,
//			   AGS_SF2_LOADER_RUN_APPLY_SYNTH);
  
  ags_sf2_loader_start(sf2_loader);
}

void
ags_ffplayer_load_preset(AgsFFPlayer *ffplayer)
{
  AgsAudioContainer *audio_container;
  
  gchar **preset;

  if(!AGS_IS_FFPLAYER(ffplayer) ||
     ffplayer->audio_container == NULL ||
     ffplayer->audio_container->sound_container == NULL){
    return;
  }

  audio_container = ffplayer->audio_container;

  /* select first preset */
  preset = ags_ipatch_sf2_reader_get_preset_all(AGS_IPATCH_SF2_READER(AGS_IPATCH(audio_container->sound_container)->reader));

  /* fill ffplayer->preset */
  while(preset != NULL && preset[0] != NULL){
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(ffplayer->preset),
				   preset[0]);
    
    preset++;
  }

  ags_ffplayer_load_instrument(ffplayer);
}

void
ags_ffplayer_load_instrument(AgsFFPlayer *ffplayer)
{
  AgsAudioContainer *audio_container;
  
  gchar **instrument;

  gint position;
  
  if(!AGS_IS_FFPLAYER(ffplayer) ||
     ffplayer->audio_container == NULL ||
     ffplayer->audio_container->sound_container == NULL){
    return;
  }

  audio_container = ffplayer->audio_container;
  
  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(ffplayer->instrument))));

  position = gtk_combo_box_get_active(GTK_COMBO_BOX(ffplayer->preset));

  if(position != -1){
    instrument = ags_ipatch_sf2_reader_get_instrument_by_preset_index(AGS_IPATCH_SF2_READER(AGS_IPATCH(audio_container->sound_container)->reader),
								      position);
  }else{
    instrument = NULL;
  }

  /* fill ffplayer->instrument */
  while(instrument != NULL && instrument[0] != NULL){
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(ffplayer->instrument),
				   instrument[0]);

    instrument++;
  }
}

/**
 * ags_ffplayer_update:
 * @ffplayer: the #AgsFFPlayer
 * 
 * Update @ffplayer.
 * 
 * Since: 3.4.0
 */
void
ags_ffplayer_update(AgsFFPlayer *ffplayer)
{
  AgsAudio *audio;
  AgsChannel *start_input;
  
  AgsAudioContainer *audio_container;

  AgsResizeAudio *resize_audio;
  AgsApplySF2Synth *apply_sf2_synth;
  AgsOpenSf2Instrument *open_sf2_instrument;

  AgsApplicationContext *application_context;

  gchar *preset_str;
  gchar *instrument_str;
  gchar *str;
  gchar *pitch_type;
  
  gdouble lower;
  gdouble key_count;
  guint audio_channels;
  guint output_pads;

  if(!AGS_IS_FFPLAYER(ffplayer)){
    return;
  }

  application_context = ags_application_context_get_instance();

  audio_container = ffplayer->audio_container;

  if(audio_container == NULL){
    return;
  }

  audio = AGS_MACHINE(ffplayer)->audio;

  start_input = NULL;

  g_object_get(audio,
	       "input", &start_input,
	       NULL);

  /*  */  
  preset_str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(ffplayer->preset));
  
  instrument_str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(ffplayer->instrument));

  lower = gtk_spin_button_get_value(ffplayer->lower);
  key_count = gtk_spin_button_get_value(ffplayer->key_count);

  audio_channels = AGS_MACHINE(ffplayer)->audio_channels;
  
  output_pads = AGS_MACHINE(ffplayer)->output_pads;

  /* pitch type */
  pitch_type = "ags-fluid-4th-order";

  str = gtk_combo_box_text_get_active_text(ffplayer->pitch_function);

  if(!g_ascii_strncasecmp(str,
			  "ags-fast-pitch",
			  15)){
    pitch_type = "ags-fast-pitch";
  }else if(!g_ascii_strncasecmp(str,
				"ags-hq-pitch",
				13)){
    pitch_type = "ags-hq-pitch";
  }else if(!g_ascii_strncasecmp(str,
				"fluid-no-interpolate",
				21)){
    pitch_type = "ags-fluid-none";
  }else if(!g_ascii_strncasecmp(str,
				"fluid-linear-interpolate",
				25)){
    pitch_type = "ags-fluid-linear";
  }else if(!g_ascii_strncasecmp(str,
				"fluid-4th-order-interpolate",
				28)){
    pitch_type = "ags-fluid-4th-order";
  }else if(!g_ascii_strncasecmp(str,
				"fluid-7th-order-interpolate",
				28)){
    pitch_type = "ags-fluid-7th-order";
  }
  
  /* open sf2 instrument */
  if(gtk_check_button_get_active(ffplayer->enable_synth_generator)){
    GList *start_sf2_synth_generator;
    GList *start_sound_resource;

    guint requested_frame_count;
    
    resize_audio = ags_resize_audio_new(audio,
					output_pads,
					key_count,
					audio_channels);
      
    /* append task */
    ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				  (AgsTask *) resize_audio);
    
    start_sf2_synth_generator = NULL;

    g_object_get(audio,
		 "sf2-synth-generator", &start_sf2_synth_generator,
		 NULL);

    requested_frame_count = 0;
    
    start_sound_resource = ags_audio_container_find_sound_resource(audio_container,
								   preset_str,
								   instrument_str,
								   NULL);

    if(start_sound_resource != NULL){
      ags_sound_resource_info(AGS_SOUND_RESOURCE(start_sound_resource->data),
			      &requested_frame_count,
			      NULL, NULL);
    }

    if(start_sf2_synth_generator != NULL){      
      ags_sf2_synth_util_load_instrument(AGS_SF2_SYNTH_GENERATOR(start_sf2_synth_generator->data)->sf2_synth_util,
					 preset_str,
					 instrument_str);
    
      g_object_set(start_sf2_synth_generator->data,
		   "filename", audio_container->filename,
		   "preset", preset_str,
		   "instrument", instrument_str,
		   "frame-count", requested_frame_count,
		   "pitch-type", pitch_type,
		   NULL);
      
      apply_sf2_synth = ags_apply_sf2_synth_new(start_sf2_synth_generator->data,
						start_input,
						lower, (guint) key_count);

      g_atomic_int_set(&(ffplayer->apply_sf2_synth_completed),
		       FALSE);

      g_signal_connect_after(apply_sf2_synth, "launch",
			     G_CALLBACK(ags_ffplayer_apply_sf2_synth_launch_callback), ffplayer);
      
      g_object_set(apply_sf2_synth,
		   "requested-frame-count", requested_frame_count,
		   NULL);
      
      /* append task */
      ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				    (AgsTask *) apply_sf2_synth);
    }

    g_list_free_full(start_sound_resource,
		     (GDestroyNotify) g_object_unref);

    g_list_free_full(start_sf2_synth_generator,
		     (GDestroyNotify) g_object_unref);
  }else{
    open_sf2_instrument = ags_open_sf2_instrument_new(audio,
						      AGS_IPATCH(audio_container->sound_container),
						      NULL,
						      NULL,
						      NULL,
						      0);
    
    /* append task */
    ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				  (AgsTask *) open_sf2_instrument);
  }  

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

/**
 * ags_ffplayer_sf2_loader_completed_timeout:
 * @ffplayer: the #AgsFFPlayer
 *
 * Queue draw widget
 *
 * Returns: %TRUE if proceed poll completed, otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_ffplayer_sf2_loader_completed_timeout(AgsFFPlayer *ffplayer)
{
  if(g_hash_table_lookup(ags_ffplayer_sf2_loader_completed,
			 ffplayer) != NULL){
    gchar *load_preset, *load_instrument;
    
    load_preset = ffplayer->load_preset;
    load_instrument = ffplayer->load_instrument;
      
    if(ffplayer->sf2_loader != NULL){
      if(ags_sf2_loader_test_flags(ffplayer->sf2_loader, AGS_SF2_LOADER_HAS_COMPLETED)){
	ffplayer->audio_container = ffplayer->sf2_loader->audio_container;
	ffplayer->sf2_loader->audio_container = NULL;

	/* clear preset and instrument */
	gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(ffplayer->preset))));
	gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(ffplayer->instrument))));    

	ffplayer->position = -1;

	gtk_spinner_stop(ffplayer->sf2_loader_spinner);
	gtk_widget_hide((GtkWidget *) ffplayer->sf2_loader_spinner);

	/* cleanup */	
	g_object_run_dispose((GObject *) ffplayer->sf2_loader);
	g_object_unref(ffplayer->sf2_loader);

	ffplayer->sf2_loader = NULL;
	
	ags_ffplayer_load_preset(ffplayer);
    
	return(TRUE);
      }else{
	if(ffplayer->position == -1){
	  ffplayer->position = 0;

	  gtk_widget_show((GtkWidget *) ffplayer->sf2_loader_spinner);
	  gtk_spinner_start(ffplayer->sf2_loader_spinner);
	}
    
	return(TRUE);
      }      
    }
    
    if(load_preset != NULL){
      /* level select */
      if(ffplayer->audio_container != NULL &&
	 ffplayer->audio_container->sound_container != NULL){
	GtkTreeModel *model;
	GtkTreeIter iter;
	
	gchar *value;
	
	/* preset */
	model = gtk_combo_box_get_model(GTK_COMBO_BOX(ffplayer->preset));
	
	if(gtk_tree_model_get_iter_first(model, &iter)){
	  do{
	    gtk_tree_model_get(model, &iter,
			       0, &value,
			       -1);
	    
	    if(!g_strcmp0(load_preset,
			  value)){
	      gtk_combo_box_set_active_iter((GtkComboBox *) ffplayer->preset,
					    &iter);
	      ags_ffplayer_load_instrument(ffplayer);
	      
	      break;
	    }
	  }while(gtk_tree_model_iter_next(model,
					  &iter));
	}
      }
      
      g_free(load_preset);

      ffplayer->load_preset = NULL;
    
      return(TRUE);
    }
    
    if(load_instrument != NULL){
      /* level select */
      if(ffplayer->audio_container != NULL &&
	 ffplayer->audio_container->sound_container != NULL){
	GtkTreeModel *model;
	GtkTreeIter iter;

	gchar *value;

	/* instrument */
	model = gtk_combo_box_get_model(GTK_COMBO_BOX(ffplayer->instrument));

	if(gtk_tree_model_get_iter_first(model, &iter)){
	  do{
	    gtk_tree_model_get(model, &iter,
			       0, &value,
			       -1);

	    if(!g_strcmp0(load_instrument,
			  value)){
	      gtk_combo_box_set_active_iter((GtkComboBox *) ffplayer->instrument,
					    &iter);
		  
	      break;
	    }
	  }while(gtk_tree_model_iter_next(model,
					  &iter));
	}
      }

      g_free(load_instrument);

      ffplayer->load_instrument = NULL;
    
      return(TRUE);
    }    

    if(g_atomic_int_get(&(ffplayer->apply_sf2_synth_completed))){
      if(ffplayer->position == 0){
	ffplayer->position = -1;

	gtk_spinner_stop(ffplayer->sf2_loader_spinner);
	gtk_widget_hide((GtkWidget *) ffplayer->sf2_loader_spinner);

#if 0
	gtk_widget_set_sensitive(ffplayer->filename,
				 TRUE);
#endif
	
	gtk_widget_set_sensitive(ffplayer->preset,
				 TRUE);
	gtk_widget_set_sensitive(ffplayer->instrument,
				 TRUE);

	gtk_widget_set_sensitive(ffplayer->open,
				 TRUE);

	gtk_widget_set_sensitive(ffplayer->update,
				 TRUE);
      }
    }else{
      if(ffplayer->position == -1){
	ffplayer->position = 0;
	    
	gtk_widget_show((GtkWidget *) ffplayer->sf2_loader_spinner);
	gtk_spinner_start(ffplayer->sf2_loader_spinner);

#if 0
	gtk_widget_set_sensitive(ffplayer->filename,
				 FALSE);
#endif
	
	gtk_widget_set_sensitive(ffplayer->preset,
				 FALSE);
	gtk_widget_set_sensitive(ffplayer->instrument,
				 FALSE);
	
	gtk_widget_set_sensitive(ffplayer->open,
				 FALSE);

	gtk_widget_set_sensitive(ffplayer->update,
				 FALSE);
      }
    }
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_ffplayer_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsFFPlayer
 *
 * Returns: the new #AgsFFPlayer
 *
 * Since: 3.0.0
 */
AgsFFPlayer*
ags_ffplayer_new(GObject *output_soundcard)
{
  AgsFFPlayer *ffplayer;

  ffplayer = (AgsFFPlayer *) g_object_new(AGS_TYPE_FFPLAYER,
					  NULL);

  if(output_soundcard != NULL){
    g_object_set(AGS_MACHINE(ffplayer)->audio,
		 "output-soundcard", output_soundcard,
		 NULL);
  }

  return(ffplayer);
}
