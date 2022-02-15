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

#include <ags/X/machine/ags_pitch_sampler.h>
#include <ags/X/machine/ags_pitch_sampler_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_navigation.h>

#include <math.h>

#include <ags/i18n.h>

void ags_pitch_sampler_class_init(AgsPitchSamplerClass *pitch_sampler);
void ags_pitch_sampler_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pitch_sampler_init(AgsPitchSampler *pitch_sampler);
void ags_pitch_sampler_finalize(GObject *gobject);

void ags_pitch_sampler_connect(AgsConnectable *connectable);
void ags_pitch_sampler_disconnect(AgsConnectable *connectable);

void ags_pitch_sampler_resize_audio_channels(AgsMachine *machine,
					     guint audio_channels, guint audio_channels_old,
					     gpointer data);
void ags_pitch_sampler_resize_pads(AgsMachine *machine, GType type,
				   guint pads, guint pads_old,
				   gpointer data);

void ags_pitch_sampler_map_recall(AgsMachine *machine);
void ags_pitch_sampler_output_map_recall(AgsPitchSampler *pitch_sampler,
					 guint audio_channel_start,
					 guint output_pad_start);
void ags_pitch_sampler_input_map_recall(AgsPitchSampler *pitch_sampler,
					guint audio_channel_start,
					guint input_pad_start);

/**
 * SECTION:ags_pitch_sampler
 * @short_description: pitch_sampler notation
 * @title: AgsPitchSampler
 * @section_id:
 * @include: ags/X/machine/ags_pitch_sampler.h
 *
 * The #AgsPitchSampler is a composite widget to act as soundfont2 notation player.
 */

static gpointer ags_pitch_sampler_parent_class = NULL;
static AgsConnectableInterface *ags_pitch_sampler_parent_connectable_interface;

GHashTable *ags_pitch_sampler_sfz_loader_completed = NULL;

GType
ags_pitch_sampler_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_pitch_sampler = 0;

    static const GTypeInfo ags_pitch_sampler_info = {
      sizeof(AgsPitchSamplerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pitch_sampler_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsPitchSampler),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pitch_sampler_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_pitch_sampler_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_pitch_sampler = g_type_register_static(AGS_TYPE_MACHINE,
						    "AgsPitchSampler", &ags_pitch_sampler_info,
						    0);
    
    g_type_add_interface_static(ags_type_pitch_sampler,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_pitch_sampler);
  }

  return g_define_type_id__volatile;
}

void
ags_pitch_sampler_class_init(AgsPitchSamplerClass *pitch_sampler)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_pitch_sampler_parent_class = g_type_class_peek_parent(pitch_sampler);

  /* GObjectClass */
  gobject = (GObjectClass *) pitch_sampler;

  gobject->finalize = ags_pitch_sampler_finalize;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) pitch_sampler;

  machine->map_recall = ags_pitch_sampler_map_recall;
}

void
ags_pitch_sampler_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_pitch_sampler_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_pitch_sampler_connect;
  connectable->disconnect = ags_pitch_sampler_disconnect;
}

void
ags_pitch_sampler_init(AgsPitchSampler *pitch_sampler)
{
  GtkExpander *expander;
  GtkBox *vbox;
  AgsPitchSamplerFile *file;
  GtkBox *hbox;
  GtkBox *control_vbox;
  GtkBox *filename_hbox;
  GtkBox *synth_generator_vbox;
  GtkBox *pitch_hbox;
  GtkBox *base_note_hbox;
  GtkBox *key_count_hbox;
  GtkBox *aliase_hbox;
  GtkGrid *lfo_grid;
  GtkBox *volume_hbox;
  GtkFrame *frame;
  GtkLabel *label;

  GtkAdjustment *adjustment;
  
  AgsAudio *audio;
  AgsSFZSynthGenerator *sfz_synth_generator;

  AgsApplicationContext *application_context;   

  gdouble gui_scale_factor;
  gdouble page, step;

  application_context = ags_application_context_get_instance();

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  g_signal_connect_after((GObject *) pitch_sampler, "parent_set",
			 G_CALLBACK(ags_pitch_sampler_parent_set_callback), (gpointer) pitch_sampler);

  audio = AGS_MACHINE(pitch_sampler)->audio;
  ags_audio_set_flags(audio, (AGS_AUDIO_SYNC |
			      AGS_AUDIO_ASYNC |
			      AGS_AUDIO_OUTPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_FILE));
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

  sfz_synth_generator = ags_sfz_synth_generator_new();
  
  ags_audio_add_sfz_synth_generator(audio,
				    (GObject *) sfz_synth_generator);
  
  AGS_MACHINE(pitch_sampler)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
					AGS_MACHINE_REVERSE_NOTATION);
  AGS_MACHINE(pitch_sampler)->file_input_flags |= AGS_MACHINE_ACCEPT_SFZ;

  AGS_MACHINE(pitch_sampler)->input_pad_type = G_TYPE_NONE;
  AGS_MACHINE(pitch_sampler)->input_line_type = G_TYPE_NONE;
  AGS_MACHINE(pitch_sampler)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(pitch_sampler)->output_line_type = G_TYPE_NONE;

  /* context menu */
  ags_machine_popup_add_connection_options((AgsMachine *) pitch_sampler,
  					   (AGS_MACHINE_POPUP_MIDI_DIALOG));

  /* audio resize */
  g_signal_connect_after(G_OBJECT(pitch_sampler), "resize-audio-channels",
			 G_CALLBACK(ags_pitch_sampler_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(pitch_sampler), "resize-pads",
			 G_CALLBACK(ags_pitch_sampler_resize_pads), NULL);

  /* flags */
  pitch_sampler->flags = 0;

  /* mapped IO */
  pitch_sampler->mapped_input_pad = 0;
  pitch_sampler->mapped_output_pad = 0;

  pitch_sampler->playback_play_container = ags_recall_container_new();
  pitch_sampler->playback_recall_container = ags_recall_container_new();

  pitch_sampler->notation_play_container = ags_recall_container_new();
  pitch_sampler->notation_recall_container = ags_recall_container_new();

  pitch_sampler->lfo_play_container = ags_recall_container_new();
  pitch_sampler->lfo_recall_container = ags_recall_container_new();

  pitch_sampler->two_pass_aliase_play_container = ags_recall_container_new();
  pitch_sampler->two_pass_aliase_recall_container = ags_recall_container_new();

  pitch_sampler->volume_play_container = ags_recall_container_new();
  pitch_sampler->volume_recall_container = ags_recall_container_new();

  pitch_sampler->envelope_play_container = ags_recall_container_new();
  pitch_sampler->envelope_recall_container = ags_recall_container_new();

  pitch_sampler->buffer_play_container = ags_recall_container_new();
  pitch_sampler->buffer_recall_container = ags_recall_container_new();

  /* context menu */
  ags_machine_popup_add_edit_options((AgsMachine *) pitch_sampler,
				     (AGS_MACHINE_POPUP_ENVELOPE));
  
  /* name and xml type */
  pitch_sampler->name = NULL;
  pitch_sampler->xml_type = "ags-pitch-sampler";

  /* audio container */
  pitch_sampler->audio_container = NULL;

  /* create widgets */
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);
  gtk_container_add((GtkContainer *) (gtk_bin_get_child((GtkBin *) pitch_sampler)),
		    (GtkWidget *) vbox);
  
  /* hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_box_pack_start(vbox,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  /* file */
  expander = gtk_expander_new(i18n("file"));
  gtk_box_pack_start(hbox,
		     (GtkWidget *) expander,
		     FALSE, FALSE,
		     0);
  
  pitch_sampler->file = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
					       0);
  gtk_container_add((GtkContainer *) expander,
		    (GtkWidget *) pitch_sampler->file);
  
  /* add 1 sample */
  file = ags_pitch_sampler_file_new();
  ags_pitch_sampler_add_file(pitch_sampler,
			     file);
  
  /* control */
  control_vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
					0);
  gtk_box_pack_start(hbox,
		     (GtkWidget *) control_vbox,
		     FALSE, FALSE,
		     0);

  /* filename */
  filename_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					 0);
  gtk_box_pack_start(control_vbox,
		     (GtkWidget *) filename_hbox,
		     FALSE, FALSE,
		     0);

  pitch_sampler->filename = (GtkEntry *) gtk_entry_new();
  gtk_box_pack_start(filename_hbox,
		     (GtkWidget *) pitch_sampler->filename,
		     FALSE, FALSE,
		     0);

  pitch_sampler->open = (GtkButton *) gtk_button_new_from_icon_name("document-open",
								    GTK_ICON_SIZE_BUTTON);
  gtk_box_pack_start(filename_hbox,
		     (GtkWidget *) pitch_sampler->open,
		     FALSE, FALSE,
		     0);

  pitch_sampler->sfz_loader = NULL;

  pitch_sampler->position = -1;

  pitch_sampler->sfz_loader_spinner = (GtkSpinner *) gtk_spinner_new();
  gtk_box_pack_start(filename_hbox,
		     (GtkWidget *) pitch_sampler->sfz_loader_spinner,
		     FALSE, FALSE,
		     0);
  gtk_widget_set_no_show_all((GtkWidget *) pitch_sampler->sfz_loader_spinner,
			     TRUE);
  gtk_widget_hide((GtkWidget *) pitch_sampler->sfz_loader_spinner);

  /* synth generator */
  frame = (GtkFrame *) gtk_frame_new(i18n("synth generator"));
  gtk_box_pack_start(hbox,
		     (GtkWidget *) frame,
		     FALSE, FALSE,
		     0);

  synth_generator_vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
						0);
  gtk_container_add((GtkContainer *) frame,
		    (GtkWidget *) synth_generator_vbox);
  
  pitch_sampler->enable_synth_generator = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));
  gtk_box_pack_start(synth_generator_vbox,
		     (GtkWidget *) pitch_sampler->enable_synth_generator,
		     FALSE, FALSE,
		     0);

  /* pitch function */
  pitch_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				      0);
  gtk_box_pack_start(synth_generator_vbox,
		     (GtkWidget *) pitch_hbox,
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("pitch"));
  gtk_box_pack_start(pitch_hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);
  
  pitch_sampler->pitch_function = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text((GtkComboBoxText *) pitch_sampler->pitch_function,
				 "ags-fast-pitch");
  
  gtk_combo_box_text_append_text((GtkComboBoxText *) pitch_sampler->pitch_function,
				 "ags-hq-pitch");

  gtk_combo_box_text_append_text((GtkComboBoxText *) pitch_sampler->pitch_function,
				 "fluid-no-interpolate");

  gtk_combo_box_text_append_text((GtkComboBoxText *) pitch_sampler->pitch_function,
				 "fluid-linear-interpolate");

  gtk_combo_box_text_append_text((GtkComboBoxText *) pitch_sampler->pitch_function,
				 "fluid-4th-order-interpolate");

  gtk_combo_box_text_append_text((GtkComboBoxText *) pitch_sampler->pitch_function,
				 "fluid-7th-order-interpolate");
  
  gtk_combo_box_set_active(pitch_sampler->pitch_function,
			   4);

  gtk_box_pack_start(pitch_hbox,
		     (GtkWidget *) pitch_sampler->pitch_function,
		     FALSE, FALSE,
		     0);

  /* base note */
  base_note_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					  0);
  gtk_box_pack_start(synth_generator_vbox,
		     (GtkWidget *) base_note_hbox,
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("lower"));
  gtk_box_pack_start(base_note_hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  pitch_sampler->lower = (GtkSpinButton *) gtk_spin_button_new_with_range(-70.0,
									  70.0,
									  1.0);
  gtk_spin_button_set_digits(pitch_sampler->lower,
			     2);
  gtk_spin_button_set_value(pitch_sampler->lower,
			    -48.0);
  gtk_box_pack_start(base_note_hbox,
		     (GtkWidget *) pitch_sampler->lower,
		     FALSE, FALSE,
		     0);
  
  /* key count */
  key_count_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					  0);
  gtk_box_pack_start(synth_generator_vbox,
		     (GtkWidget *) key_count_hbox,
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("key count"));
  gtk_box_pack_start(key_count_hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  pitch_sampler->key_count = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									 128.0,
									 1.0);
  gtk_spin_button_set_value(pitch_sampler->key_count,
			    78.0);
  gtk_box_pack_start((GtkBox *) key_count_hbox,
		     (GtkWidget *) pitch_sampler->key_count,
		     FALSE, FALSE,
		     0);

  pitch_sampler->update = (GtkButton *) gtk_button_new_with_label(i18n("update"));
  gtk_widget_set_valign((GtkWidget *) pitch_sampler->update,
			GTK_ALIGN_END);
  gtk_box_pack_start(hbox,
		     (GtkWidget *) pitch_sampler->update,
		     FALSE, FALSE,
		     0);
  
  /* other controls */
  /* aliase */
  frame = (GtkFrame *) gtk_frame_new(i18n("aliase"));

  gtk_widget_set_valign((GtkWidget *) frame,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) frame,
			GTK_ALIGN_FILL);
  
  gtk_box_pack_start(hbox,
		     (GtkWidget *) frame,
		     FALSE, FALSE,
		     0);

  aliase_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				       0);
  gtk_container_add((GtkContainer *) frame,
		    (GtkWidget *) aliase_hbox);
  
  pitch_sampler->enable_aliase = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));
  gtk_box_pack_start(aliase_hbox,
		     (GtkWidget *) pitch_sampler->enable_aliase,
		     FALSE, FALSE,
		     0);

  step = 2.0 * M_PI / 100.0;
  page = 2.0 * M_PI / AGS_DIAL_DEFAULT_PRECISION;

  /* aliase a */
  pitch_sampler->aliase_a_amount = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(pitch_sampler->aliase_a_amount);

  gtk_adjustment_set_step_increment(adjustment,
				    step);
  gtk_adjustment_set_page_increment(adjustment,
				    page);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   2.0 * M_PI);
  
  gtk_box_pack_start(aliase_hbox,
		     (GtkWidget *) pitch_sampler->aliase_a_amount,
		     FALSE, FALSE,
		     0);

  pitch_sampler->aliase_a_phase = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(pitch_sampler->aliase_a_phase);

  gtk_adjustment_set_step_increment(adjustment,
				    step);
  gtk_adjustment_set_page_increment(adjustment,
				    page);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   2.0 * M_PI);
  
  gtk_box_pack_start(aliase_hbox,
		     (GtkWidget *) pitch_sampler->aliase_a_phase,
		     FALSE, FALSE,
		     0);

  /* aliase b */
  pitch_sampler->aliase_b_amount = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(pitch_sampler->aliase_b_amount);

  gtk_adjustment_set_step_increment(adjustment,
				    step);
  gtk_adjustment_set_page_increment(adjustment,
				    page);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   2.0 * M_PI);
  
  gtk_box_pack_start(aliase_hbox,
		     (GtkWidget *) pitch_sampler->aliase_b_amount,
		     FALSE, FALSE,
		     0);

  pitch_sampler->aliase_b_phase = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(pitch_sampler->aliase_b_phase);

  gtk_adjustment_set_step_increment(adjustment,
				    step);
  gtk_adjustment_set_page_increment(adjustment,
				    page);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   2.0 * M_PI);
  
  gtk_box_pack_start(aliase_hbox,
		     (GtkWidget *) pitch_sampler->aliase_b_phase,
		     FALSE, FALSE,
		     0);

  /* volume */
  frame = (GtkFrame *) gtk_frame_new(i18n("volume"));

  gtk_widget_set_valign((GtkWidget *) frame,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) frame,
			GTK_ALIGN_FILL);
  
  gtk_box_pack_start(hbox,
		     (GtkWidget *) frame,
		     FALSE, FALSE,
		     0);

  volume_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				       0);
  gtk_container_add((GtkContainer *) frame,
		    (GtkWidget *) volume_hbox);
  
  pitch_sampler->volume = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
							   0.0,
							   2.0,
							   0.025);
  
  gtk_widget_set_size_request(pitch_sampler->volume,
			      gui_scale_factor * 16, gui_scale_factor * 100);
  
  gtk_box_pack_start(volume_hbox,
		     (GtkWidget *) pitch_sampler->volume,
		     FALSE, FALSE,
		     0);

  gtk_scale_set_digits(pitch_sampler->volume,
		       3);

  gtk_range_set_increments(GTK_RANGE(pitch_sampler->volume),
			   0.025, 0.1);
  gtk_range_set_value(GTK_RANGE(pitch_sampler->volume),
		      1.0);
  gtk_range_set_inverted(GTK_RANGE(pitch_sampler->volume),
			 TRUE);  
  
  /* LFO grid */
  lfo_grid = (GtkGrid *) gtk_grid_new();
  gtk_box_pack_start(hbox,
		     (GtkWidget *) lfo_grid,
		     FALSE, FALSE,
		     0);

  pitch_sampler->enable_lfo = gtk_check_button_new_with_label(i18n("enable LFO"));

  gtk_widget_set_valign((GtkWidget *) pitch_sampler->enable_lfo,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) pitch_sampler->enable_lfo,
			GTK_ALIGN_FILL);

  gtk_grid_attach(lfo_grid,
		  (GtkWidget *) pitch_sampler->enable_lfo,
		  0, 0,
		  1, 1); 

  label = gtk_label_new(i18n("LFO freq"));

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);

  gtk_grid_attach(lfo_grid,
		   (GtkWidget *) label,
		   1, 0,
		   1, 1);
  
  pitch_sampler->lfo_freq = (GtkSpinButton *) gtk_spin_button_new_with_range(AGS_PITCH_SAMPLER_LFO_FREQ_MIN,
									     AGS_PITCH_SAMPLER_LFO_FREQ_MAX,
									     0.001);
  gtk_spin_button_set_digits(pitch_sampler->lfo_freq,
			     3);
  gtk_spin_button_set_value(pitch_sampler->lfo_freq, AGS_PITCH_SAMPLER_DEFAULT_LFO_FREQ);

  gtk_widget_set_valign((GtkWidget *) pitch_sampler->lfo_freq,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) pitch_sampler->lfo_freq,
			GTK_ALIGN_FILL);

  gtk_grid_attach(lfo_grid,
		   (GtkWidget *) pitch_sampler->lfo_freq,
		   2, 0,
		   1, 1);
  
  label = gtk_label_new(i18n("LFO phase"));

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);

  gtk_grid_attach(lfo_grid,
		   (GtkWidget *) label,
		   1, 1,
		   1, 1);
  
  pitch_sampler->lfo_phase = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									      G_MAXDOUBLE,
									      1.0);
  gtk_spin_button_set_value(pitch_sampler->lfo_phase, 0.0);

  gtk_widget_set_valign((GtkWidget *) pitch_sampler->lfo_phase,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) pitch_sampler->lfo_phase,
			GTK_ALIGN_FILL);

  gtk_grid_attach(lfo_grid,
		   (GtkWidget *) pitch_sampler->lfo_phase,
		   2, 1,
		   1, 1);

  label = gtk_label_new(i18n("LFO depth"));

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);

  gtk_grid_attach(lfo_grid,
		   (GtkWidget *) label,
		   1, 2,
		   1, 1);
  
  pitch_sampler->lfo_depth = (GtkSpinButton *) gtk_spin_button_new_with_range(-1200.0,
									      1200.0,
									      1.0);
  gtk_spin_button_set_digits(pitch_sampler->lfo_depth,
			     3);
  gtk_spin_button_set_value(pitch_sampler->lfo_depth, 0.0);

  gtk_widget_set_valign((GtkWidget *) pitch_sampler->lfo_depth,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) pitch_sampler->lfo_depth,
			GTK_ALIGN_FILL);

  gtk_grid_attach(lfo_grid,
		   (GtkWidget *) pitch_sampler->lfo_depth,
		   2, 2,
		   1, 1);

  label = gtk_label_new(i18n("LFO tuning"));

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);

  gtk_grid_attach(lfo_grid,
		   (GtkWidget *) label,
		   1, 3,
		   1, 1);
  
  pitch_sampler->lfo_tuning = (GtkSpinButton *) gtk_spin_button_new_with_range(-1200.0,
									       1200.0,
									       1.0);
  gtk_spin_button_set_digits(pitch_sampler->lfo_tuning,
			     2);
  gtk_spin_button_set_value(pitch_sampler->lfo_tuning, 1.0);

  gtk_widget_set_valign((GtkWidget *) pitch_sampler->lfo_tuning,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) pitch_sampler->lfo_tuning,
			GTK_ALIGN_FILL);

  gtk_grid_attach(lfo_grid,
		   (GtkWidget *) pitch_sampler->lfo_tuning,
		   2, 3,
		   1, 1);

  /* dialog */
  pitch_sampler->open_dialog = NULL;

  /* SFZ loader */
  if(ags_pitch_sampler_sfz_loader_completed == NULL){
    ags_pitch_sampler_sfz_loader_completed = g_hash_table_new_full(g_direct_hash, g_direct_equal,
								   NULL,
								   NULL);
  }

  g_hash_table_insert(ags_pitch_sampler_sfz_loader_completed,
		      pitch_sampler, ags_pitch_sampler_sfz_loader_completed_timeout);
  g_timeout_add(1000 / 4, (GSourceFunc) ags_pitch_sampler_sfz_loader_completed_timeout, (gpointer) pitch_sampler);
}

void
ags_pitch_sampler_finalize(GObject *gobject)
{
  g_hash_table_remove(ags_pitch_sampler_sfz_loader_completed,
		      gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_pitch_sampler_parent_class)->finalize(gobject);
}

void
ags_pitch_sampler_connect(AgsConnectable *connectable)
{
  AgsPitchSampler *pitch_sampler;

  GList *start_list, *list;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_pitch_sampler_parent_connectable_interface->connect(connectable);

  pitch_sampler = AGS_PITCH_SAMPLER(connectable);

  list = 
    start_list = gtk_container_get_children((GtkContainer *) pitch_sampler->file);

  while(list != NULL){
    GList *child_start;

    child_start = gtk_container_get_children(GTK_CONTAINER(list->data));
    
    ags_connectable_connect(AGS_CONNECTABLE(child_start->next->data));

    list = list->next;
  }

  g_list_free(start_list);
  
  /* filename */
  g_signal_connect(pitch_sampler->open, "clicked",
		   G_CALLBACK(ags_pitch_sampler_open_callback), pitch_sampler);

  /* update */
  g_signal_connect(pitch_sampler->update, "clicked",
		   G_CALLBACK(ags_pitch_sampler_update_callback), pitch_sampler);

  /* LFO */
  g_signal_connect_after((GObject *) pitch_sampler->enable_lfo, "toggled",
			 G_CALLBACK(ags_pitch_sampler_enable_lfo_callback), pitch_sampler);

  g_signal_connect_after((GObject *) pitch_sampler->lfo_freq, "value-changed",
			 G_CALLBACK(ags_pitch_sampler_lfo_freq_callback), (gpointer) pitch_sampler);
  
  g_signal_connect_after((GObject *) pitch_sampler->lfo_phase, "value-changed",
			 G_CALLBACK(ags_pitch_sampler_lfo_phase_callback), (gpointer) pitch_sampler);

  g_signal_connect_after((GObject *) pitch_sampler->lfo_depth, "value-changed",
			 G_CALLBACK(ags_pitch_sampler_lfo_depth_callback), (gpointer) pitch_sampler);

  g_signal_connect_after((GObject *) pitch_sampler->lfo_tuning, "value-changed",
			 G_CALLBACK(ags_pitch_sampler_lfo_tuning_callback), (gpointer) pitch_sampler);

  g_signal_connect((GObject *) pitch_sampler->enable_aliase, "clicked",
		   G_CALLBACK(ags_pitch_sampler_enable_aliase_callback), (gpointer) pitch_sampler);

  g_signal_connect((GObject *) pitch_sampler->aliase_a_amount, "value-changed",
		   G_CALLBACK(ags_pitch_sampler_aliase_a_amount_callback), (gpointer) pitch_sampler);

  g_signal_connect((GObject *) pitch_sampler->aliase_a_phase, "value-changed",
		   G_CALLBACK(ags_pitch_sampler_aliase_a_phase_callback), (gpointer) pitch_sampler);

  g_signal_connect((GObject *) pitch_sampler->aliase_b_amount, "value-changed",
		   G_CALLBACK(ags_pitch_sampler_aliase_b_amount_callback), (gpointer) pitch_sampler);

  g_signal_connect((GObject *) pitch_sampler->aliase_b_phase, "value-changed",
		   G_CALLBACK(ags_pitch_sampler_aliase_b_phase_callback), (gpointer) pitch_sampler);

  g_signal_connect((GObject *) pitch_sampler->volume, "value-changed",
		   G_CALLBACK(ags_pitch_sampler_volume_callback), (gpointer) pitch_sampler);
}

void
ags_pitch_sampler_disconnect(AgsConnectable *connectable)
{
  AgsPitchSampler *pitch_sampler;

  GList *start_list, *list;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) == 0){
    return;
  }

  ags_pitch_sampler_parent_connectable_interface->disconnect(connectable);

  pitch_sampler = AGS_PITCH_SAMPLER(connectable);

  list = 
    start_list = gtk_container_get_children((GtkContainer *) pitch_sampler->file);

  while(list != NULL){
    GList *child_start;
   
    child_start = gtk_container_get_children(GTK_CONTAINER(list->data));
    
    ags_connectable_disconnect(AGS_CONNECTABLE(child_start->next->data));

    g_list_free(child_start);
    
    list = list->next;
  }

  g_list_free(start_list);
  
  /* filename */
  g_object_disconnect(pitch_sampler->open,
		      "any_signal::clicked",
		      G_CALLBACK(ags_pitch_sampler_open_callback),
		      pitch_sampler,
		      NULL);
  
  /* update */
  g_object_disconnect(pitch_sampler->update,
		      "any_signal::clicked",
		      G_CALLBACK(ags_pitch_sampler_update_callback),
		      pitch_sampler,
		      NULL);

  /* LFO */
  g_object_disconnect((GObject *) pitch_sampler->enable_lfo,
		      "any_signal::toggled",
		      G_CALLBACK(ags_pitch_sampler_enable_lfo_callback),
		      pitch_sampler,
		      NULL);

  g_object_disconnect((GObject *) pitch_sampler->lfo_freq,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_pitch_sampler_lfo_freq_callback),
		      (gpointer) pitch_sampler,
		      NULL);
  
  g_object_disconnect((GObject *) pitch_sampler->lfo_phase,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_pitch_sampler_lfo_phase_callback),
		      (gpointer) pitch_sampler,
		      NULL);

  g_object_disconnect((GObject *) pitch_sampler->lfo_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_pitch_sampler_lfo_depth_callback),
		      (gpointer) pitch_sampler,
		      NULL);

  g_object_disconnect((GObject *) pitch_sampler->lfo_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_pitch_sampler_lfo_tuning_callback),
		      (gpointer) pitch_sampler,
		      NULL);

  /* aliase */
  g_object_disconnect((GObject *) pitch_sampler->enable_aliase,
		      "any_signal::clicked",
		      G_CALLBACK(ags_pitch_sampler_enable_aliase_callback),
		      (gpointer) pitch_sampler,
		      NULL);

  g_object_disconnect((GObject *) pitch_sampler->aliase_a_amount,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_pitch_sampler_aliase_a_amount_callback),
		      (gpointer) pitch_sampler,
		      NULL);

  g_object_disconnect((GObject *) pitch_sampler->aliase_a_phase,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_pitch_sampler_aliase_a_phase_callback),
		      (gpointer) pitch_sampler,
		      NULL);

  g_object_disconnect((GObject *) pitch_sampler->aliase_b_amount,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_pitch_sampler_aliase_b_amount_callback),
		      (gpointer) pitch_sampler,
		      NULL);

  g_object_disconnect((GObject *) pitch_sampler->aliase_b_phase,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_pitch_sampler_aliase_b_phase_callback),
		      (gpointer) pitch_sampler,
		      NULL);

  /* volume */
  g_object_disconnect((GObject *) pitch_sampler->volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_pitch_sampler_volume_callback),
		      (gpointer) pitch_sampler,
		      NULL);
}

void
ags_pitch_sampler_resize_audio_channels(AgsMachine *machine,
					guint audio_channels, guint audio_channels_old,
					gpointer data)
{
  AgsPitchSampler *pitch_sampler;

  pitch_sampler = (AgsPitchSampler *) machine;  

  /*  */
  if(audio_channels > audio_channels_old){      
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_pitch_sampler_input_map_recall(pitch_sampler,
					 audio_channels_old,
					 0);

      ags_pitch_sampler_output_map_recall(pitch_sampler,
					  audio_channels_old,
					  0);
    }
  }
}

void
ags_pitch_sampler_resize_pads(AgsMachine *machine, GType channel_type,
			      guint pads, guint pads_old,
			      gpointer data)
{
  AgsPitchSampler *pitch_sampler;

  gboolean grow;

  pitch_sampler = (AgsPitchSampler *) machine;

  /* check grow */
  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(g_type_is_a(channel_type, AGS_TYPE_INPUT)){
    if(grow){
      /* depending on destination */
      ags_pitch_sampler_input_map_recall(pitch_sampler,
					 0,
					 pads_old);
    }else{
      pitch_sampler->mapped_input_pad = pads;
    }
  }else if(g_type_is_a(channel_type, AGS_TYPE_OUTPUT)){
    if(grow){
      /* depending on destination */
      ags_pitch_sampler_output_map_recall(pitch_sampler,
					  0,
					  pads_old);
    }else{
      pitch_sampler->mapped_output_pad = pads;
    }
  }else{
    g_critical("unknown channel type");
  }
}

void
ags_pitch_sampler_map_recall(AgsMachine *machine)
{
  AgsNavigation *navigation;
  AgsPitchSampler *pitch_sampler;
  
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

  pitch_sampler = AGS_PITCH_SAMPLER(machine);

  audio = machine->audio;
  
  position = 0;
  
  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       pitch_sampler->playback_play_container, pitch_sampler->playback_recall_container,
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
				       pitch_sampler->notation_play_container, pitch_sampler->notation_recall_container,
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

  /* ags-fx-lfo */
  start_recall = ags_fx_factory_create(audio,
				       pitch_sampler->lfo_play_container, pitch_sampler->lfo_recall_container,
				       "ags-fx-lfo",
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
				       pitch_sampler->two_pass_aliase_play_container, pitch_sampler->two_pass_aliase_recall_container,
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
				       pitch_sampler->volume_play_container, pitch_sampler->volume_recall_container,
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
				       pitch_sampler->envelope_play_container, pitch_sampler->envelope_recall_container,
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
				       pitch_sampler->buffer_play_container, pitch_sampler->buffer_recall_container,
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
  ags_pitch_sampler_input_map_recall(pitch_sampler,
				     0,
				     0);

  /* depending on destination */
  ags_pitch_sampler_output_map_recall(pitch_sampler,
				      0,
				      0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_pitch_sampler_parent_class)->map_recall(machine);  
}

void
ags_pitch_sampler_input_map_recall(AgsPitchSampler *pitch_sampler,
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

  audio = AGS_MACHINE(pitch_sampler)->audio;

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

      input_line = g_list_nth_data(AGS_MACHINE(pitch_sampler)->machine_input_line,
				   (i * audio_channels) + j);

      if(input_line != NULL &&
	 input_line->mapped_recall == FALSE){
	/* ags-fx-playback */
	start_recall = ags_fx_factory_create(audio,
					     pitch_sampler->playback_play_container, pitch_sampler->playback_recall_container,
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
					     pitch_sampler->notation_play_container, pitch_sampler->notation_recall_container,
					     "ags-fx-notation",
					     NULL,
					     NULL,
					     j, j + 1,
					     i, i + 1,
					     position,
					     (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

	g_list_free_full(start_recall,
			 (GDestroyNotify) g_object_unref);

	/* ags-fx-lfo */
	start_recall = ags_fx_factory_create(audio,
					     pitch_sampler->lfo_play_container, pitch_sampler->lfo_recall_container,
					     "ags-fx-lfo",
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
					     pitch_sampler->two_pass_aliase_play_container, pitch_sampler->two_pass_aliase_recall_container,
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
					     pitch_sampler->volume_play_container, pitch_sampler->volume_recall_container,
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
					     pitch_sampler->envelope_play_container, pitch_sampler->envelope_recall_container,
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
					     pitch_sampler->buffer_play_container, pitch_sampler->buffer_recall_container,
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
  
  pitch_sampler->mapped_input_pad = input_pads;
}

void
ags_pitch_sampler_output_map_recall(AgsPitchSampler *pitch_sampler,
				    guint audio_channel_start,
				    guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;

  if(pitch_sampler->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(pitch_sampler)->audio;

  /* get some fields */
  g_object_get(audio,
	       "output-pads", &output_pads,
	       NULL);
  
  pitch_sampler->mapped_output_pad = output_pads;
}

/**
 * ags_pitch_sampler_add_file:
 * @pitch_sampler: the #AgsPitchSampler
 * @file: the #AgsPitchSamplerFile
 * 
 * Add @file to @pitch_sampler.
 * 
 * Since: 3.0.0
 */
void
ags_pitch_sampler_add_file(AgsPitchSampler *pitch_sampler,
			   AgsPitchSamplerFile *file)
{
  GtkBox *hbox;
  GtkCheckButton *check_button;

  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);

  check_button = (GtkCheckButton *) gtk_check_button_new();
  gtk_box_pack_start(hbox,
		     (GtkWidget *) check_button,
		     FALSE,
		     FALSE,
		     0);
  
  gtk_box_pack_start(hbox,
		     (GtkWidget *) file,
		     FALSE,
		     FALSE,
		     0);

  gtk_box_pack_start(pitch_sampler->file,
		     (GtkWidget *) hbox,
		     FALSE,
		     FALSE,
		     0);
  gtk_widget_show_all((GtkWidget *) hbox);
}

/**
 * ags_pitch_sampler_remove_file:
 * @pitch_sampler: the #AgsPitchSampler
 * @nth: the nth #AgsPitchSamplerFile
 * 
 * Remove nth file.
 * 
 * Since: 3.0.0
 */
void
ags_pitch_sampler_remove_file(AgsPitchSampler *pitch_sampler,
			      guint nth)
{
  GList *list, *list_start;

  list_start = gtk_container_get_children(GTK_CONTAINER(pitch_sampler->file));

  list = g_list_nth(list_start,
		    nth);

  if(list != NULL){
    gtk_widget_destroy(list->data);
  }

  g_list_free(list_start);
}

/**
 * ags_pitch_sampler_open_filename:
 * @pitch_sampler: the #AgsPitchSampler
 * @filename: the filename
 * 
 * Open @filename.
 * 
 * Since: 3.0.0
 */
void
ags_pitch_sampler_open_filename(AgsPitchSampler *pitch_sampler,
				gchar *filename)
{
  AgsSFZLoader *sfz_loader;

  if(!AGS_IS_PITCH_SAMPLER(pitch_sampler) ||
     filename == NULL){
    return;
  }
  
  pitch_sampler->sfz_loader = 
    sfz_loader = ags_sfz_loader_new(AGS_MACHINE(pitch_sampler)->audio,
				    filename,
				    TRUE);
  ags_sfz_loader_set_flags(sfz_loader,
			   AGS_SFZ_LOADER_RUN_APPLY_SYNTH);

  ags_sfz_loader_start(sfz_loader);
}

/**
 * ags_pitch_sampler_update:
 * @pitch_sampler: the #AgsPitchSampler
 * 
 * Update @pitch_sampler.
 * 
 * Since: 3.0.0
 */
void
ags_pitch_sampler_update(AgsPitchSampler *pitch_sampler)
{
  AgsAudio *audio;
  AgsChannel *start_input;
  
  AgsAudioContainer *audio_container;

  AgsResizeAudio *resize_audio;
  AgsApplySFZSynth *apply_sfz_synth;
  AgsOpenSFZFile *open_sfz_file;

  AgsApplicationContext *application_context;

  gchar *str;
  
  gdouble lower;
  gdouble key_count;
  guint audio_channels;
  guint output_pads;
  guint pitch_type;

  if(!AGS_IS_PITCH_SAMPLER(pitch_sampler)){
    return;
  }

  application_context = ags_application_context_get_instance();

  audio_container = pitch_sampler->audio_container;

  if(audio_container == NULL){
    return;
  }

  audio = AGS_MACHINE(pitch_sampler)->audio;

  start_input = NULL;

  g_object_get(audio,
	       "input", &start_input,
	       NULL);

  /*  */  
  lower = gtk_spin_button_get_value(pitch_sampler->lower);
  key_count = gtk_spin_button_get_value(pitch_sampler->key_count);

  audio_channels = AGS_MACHINE(pitch_sampler)->audio_channels;
  
  output_pads = AGS_MACHINE(pitch_sampler)->output_pads;

  /* pitch type */
  pitch_type = AGS_FLUID_4TH_ORDER_INTERPOLATE;

  str = gtk_combo_box_text_get_active_text(pitch_sampler->pitch_function);

  if(!g_ascii_strncasecmp(str,
			  "ags-fast-pitch",
			  16)){
    pitch_type = AGS_FAST_PITCH;
  }else if(!g_ascii_strncasecmp(str,
				"ags-hq-pitch",
				14)){
    pitch_type = AGS_HQ_PITCH;
  }else if(!g_ascii_strncasecmp(str,
				"fluid-no-interpolate",
				21)){
    pitch_type = AGS_FLUID_NO_INTERPOLATE;
  }else if(!g_ascii_strncasecmp(str,
				"fluid-linear-interpolate",
				26)){
    pitch_type = AGS_FLUID_LINEAR_INTERPOLATE;
  }else if(!g_ascii_strncasecmp(str,
				"fluid-4th-order-interpolate",
				29)){
    pitch_type = AGS_FLUID_4TH_ORDER_INTERPOLATE;
  }else if(!g_ascii_strncasecmp(str,
				"fluid-7th-order-interpolate",
				29)){
    pitch_type = AGS_FLUID_7TH_ORDER_INTERPOLATE;
  }
  
  /* open sfz file */
  if(gtk_toggle_button_get_active((GtkToggleButton *) pitch_sampler->enable_synth_generator)){
    GList *start_sfz_synth_generator;
    GList *start_sound_resource;

    guint requested_frame_count;
    
    resize_audio = ags_resize_audio_new(audio,
					output_pads,
					key_count,
					audio_channels);
      
    /* append task */
    ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				  (AgsTask *) resize_audio);
    
    start_sfz_synth_generator = NULL;

    g_object_get(audio,
		 "sfz-synth-generator", &start_sfz_synth_generator,
		 NULL);

    requested_frame_count = 0;
    
    start_sound_resource = ags_audio_container_find_sound_resource(audio_container,
								   NULL,
								   NULL,
								   NULL);

    if(start_sound_resource != NULL){
      ags_sound_resource_info(AGS_SOUND_RESOURCE(start_sound_resource->data),
			      &requested_frame_count,
			      NULL, NULL);
    }
    
    if(start_sfz_synth_generator != NULL){
      g_object_set(start_sfz_synth_generator->data,
		   "filename", audio_container->filename,
		   "frame-count", requested_frame_count,
		   "pitch-type", pitch_type,
		   NULL);
      
      apply_sfz_synth = ags_apply_sfz_synth_new(start_sfz_synth_generator->data,
						start_input,
						lower, (guint) key_count);
      
      g_object_set(apply_sfz_synth,
		   "requested-frame-count", requested_frame_count,
		   NULL);
      
      /* append task */
      ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				    (AgsTask *) apply_sfz_synth);
    }

    g_list_free_full(start_sound_resource,
		     (GDestroyNotify) g_object_unref);

    g_list_free_full(start_sfz_synth_generator,
		     (GDestroyNotify) g_object_unref);
  }else{
    open_sfz_file = ags_open_sfz_file_new(audio,
					  AGS_SFZ_FILE(audio_container->sound_container),
					  NULL,
					  0);
    
    /* append task */
    ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				  (AgsTask *) open_sfz_file);
  }  
}

/**
 * ags_pitch_sampler_sfz_loader_completed_timeout:
 * @pitch_sampler: the #AgsPitchSampler
 *
 * Queue draw widget
 *
 * Returns: %TRUE if proceed poll completed, otherwise %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_pitch_sampler_sfz_loader_completed_timeout(AgsPitchSampler *pitch_sampler)
{
  if(g_hash_table_lookup(ags_pitch_sampler_sfz_loader_completed,
			 pitch_sampler) != NULL){
    if(pitch_sampler->sfz_loader != NULL){
      if(ags_sfz_loader_test_flags(pitch_sampler->sfz_loader, AGS_SFZ_LOADER_HAS_COMPLETED)){
	AgsPitchSamplerFile *file;

	GList *start_list, *list;
	
	/* file */
	list =
	  start_list = gtk_container_get_children(GTK_CONTAINER(pitch_sampler->file));

	while(list != NULL){
	  gtk_widget_destroy(list->data);

	  list = list->next;
	}

	g_list_free(start_list);

	/* reassign audio container */
	pitch_sampler->audio_container = pitch_sampler->sfz_loader->audio_container;
	pitch_sampler->sfz_loader->audio_container = NULL;

	if(pitch_sampler->audio_container == NULL ||
	   pitch_sampler->audio_container->sound_container == NULL){
	  g_object_run_dispose((GObject *) pitch_sampler->sfz_loader);
	  g_object_unref(pitch_sampler->sfz_loader);

	  pitch_sampler->sfz_loader = NULL;

	  pitch_sampler->position = -1;

	  gtk_spinner_stop(pitch_sampler->sfz_loader_spinner);
	  gtk_widget_hide((GtkWidget *) pitch_sampler->sfz_loader_spinner);
	  
	  return(TRUE);
	}
	
	g_object_get(pitch_sampler->audio_container->sound_container,
		     "sample", &start_list,
		     NULL);

	list = start_list;

	while(list != NULL){
	  AgsSFZGroup *group;
	  AgsSFZRegion *region;      

	  gchar *filename;
	  gchar *str_key, *str_pitch_keycenter;
	  gchar *str;
	  
	  glong current_key;
	  glong pitch_keycenter, current_pitch_keycenter;
	  guint loop_start, loop_end;
	  int retval;

	  file = ags_pitch_sampler_file_new();
	  ags_pitch_sampler_add_file(pitch_sampler,
				     file);
	  ags_connectable_connect(AGS_CONNECTABLE(file));

	  /* pitch keycenter */
	  g_object_get(list->data,
		       "group", &group,
		       "region", &region,
		       "filename", &filename,
		       NULL);

	  pitch_keycenter = 49;

	  /* group */
	  str_pitch_keycenter = ags_sfz_group_lookup_control(group,
							     "pitch_keycenter");
      
	  str_key = ags_sfz_group_lookup_control(group,
						 "key");
	  
	  if(str_pitch_keycenter != NULL){
	    retval = sscanf(str_pitch_keycenter, "%lu", &current_pitch_keycenter);

	    if(retval > 0){
	      pitch_keycenter = current_pitch_keycenter;
	    }else{
	      retval = ags_diatonic_scale_note_to_midi_key(str_pitch_keycenter,
							   &current_key);

	      if(retval > 0){
		pitch_keycenter = current_key;
	      }
	    }		
	  }else if(str_key != NULL){
	    retval = sscanf(str_key, "%lu", &current_pitch_keycenter);

	    if(retval > 0){
	      pitch_keycenter = current_pitch_keycenter;
	    }else{
	      retval = ags_diatonic_scale_note_to_midi_key(str_key,
							   &current_key);

	      if(retval > 0){
		pitch_keycenter = current_key;
	      }
	    }	
	  }

	  /* region */
	  str_pitch_keycenter = ags_sfz_region_lookup_control(region,
							      "pitch_keycenter");
      
	  str_key = ags_sfz_region_lookup_control(region,
						  "key");

	  if(str_pitch_keycenter != NULL){
	    retval = sscanf(str_pitch_keycenter, "%lu", &current_pitch_keycenter);

	    if(retval > 0){
	      pitch_keycenter = current_pitch_keycenter;
	    }else{
	      retval = ags_diatonic_scale_note_to_midi_key(str_pitch_keycenter,
							   &current_key);

	      if(retval > 0){
		pitch_keycenter = current_key;
	      }
	    }		
	  }else if(str_key != NULL){
	    retval = sscanf(str_key, "%lu", &current_pitch_keycenter);

	    if(retval > 0){
	      pitch_keycenter = current_pitch_keycenter;
	    }else{
	      retval = ags_diatonic_scale_note_to_midi_key(str_key,
							   &current_key);

	      if(retval > 0){
		pitch_keycenter = current_key;
	      }
	    }	
	  }

	  /* set filename */
	  gtk_entry_set_text(file->filename,
			     filename);
	  g_free(filename);
	  
	  /* set pitch keycenter */
	  str = g_strdup_printf("%f",
				27.5 * exp2((gdouble) pitch_keycenter / 12.0));
	  gtk_label_set_text(file->freq,
			     str);
	  
	  g_free(str);

	  str = g_strdup_printf("%ld",
				pitch_keycenter);
	  gtk_label_set_text(file->base_key,
			     str);

	  g_free(str);

	  /* set loop start/end */
	  g_object_get(list->data,
		       "loop-start", &loop_start,
		       "loop-end", &loop_end,
		       NULL);
	  
	  str = g_strdup_printf("%d",
				loop_start);
	  gtk_label_set_text(file->loop_start,
			     str);

	  g_free(str);
	  
	  str = g_strdup_printf("%d",
				loop_end);
	  gtk_label_set_text(file->loop_end,
			     str);	  
	  
	  g_free(str);

	  /* iterate */
	  list = list->next;
	}

	g_list_free_full(start_list,
			 g_object_unref);
	
	/* cleanup */	
	g_object_run_dispose((GObject *) pitch_sampler->sfz_loader);
	g_object_unref(pitch_sampler->sfz_loader);

	pitch_sampler->sfz_loader = NULL;

	pitch_sampler->position = -1;

	gtk_spinner_stop(pitch_sampler->sfz_loader_spinner);
	gtk_widget_hide((GtkWidget *) pitch_sampler->sfz_loader_spinner);
      }else{
	if(pitch_sampler->position == -1){
	  pitch_sampler->position = 0;

	  gtk_widget_show((GtkWidget *) pitch_sampler->sfz_loader_spinner);
	  gtk_spinner_start(pitch_sampler->sfz_loader_spinner);
	}
      }
    }
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_pitch_sampler_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsPitchSampler
 *
 * Returns: the new #AgsPitchSampler
 *
 * Since: 3.0.0
 */
AgsPitchSampler*
ags_pitch_sampler_new(GObject *output_soundcard)
{
  AgsPitchSampler *pitch_sampler;

  pitch_sampler = (AgsPitchSampler *) g_object_new(AGS_TYPE_PITCH_SAMPLER,
						   NULL);

  if(output_soundcard != NULL){
    g_object_set(AGS_MACHINE(pitch_sampler)->audio,
		 "output-soundcard", output_soundcard,
		 NULL);
  }

  return(pitch_sampler);
}
