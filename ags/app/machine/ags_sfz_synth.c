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

#include <ags/app/machine/ags_sfz_synth.h>
#include <ags/app/machine/ags_sfz_synth_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>

#include <ags/i18n.h>

void ags_sfz_synth_class_init(AgsSFZSynthClass *sfz_synth);
void ags_sfz_synth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sfz_synth_init(AgsSFZSynth *sfz_synth);
void ags_sfz_synth_finalize(GObject *gobject);

void ags_sfz_synth_connect(AgsConnectable *connectable);
void ags_sfz_synth_disconnect(AgsConnectable *connectable);

void ags_sfz_synth_show(GtkWidget *widget);

void ags_sfz_synth_wah_wah_draw_function(GtkDrawingArea *area,
					 cairo_t *cr,
					 int width,
					 int height,
					 AgsSFZSynth *sfz_synth);

void ags_sfz_synth_resize_audio_channels(AgsMachine *machine,
					 guint audio_channels, guint audio_channels_old,
					 gpointer data);
void ags_sfz_synth_resize_pads(AgsMachine *machine, GType channel_type,
			       guint pads, guint pads_old,
			       gpointer data);

void ags_sfz_synth_map_recall(AgsMachine *machine);

void ags_sfz_synth_input_map_recall(AgsSFZSynth *sfz_synth,
				    guint audio_channel_start,
				    guint input_pad_start);
void ags_sfz_synth_output_map_recall(AgsSFZSynth *sfz_synth,
				     guint audio_channel_start,
				     guint output_pad_start);

void ags_sfz_synth_refresh_port(AgsMachine *machine);

/**
 * SECTION:ags_sfz_synth
 * @short_description: sfz_synth
 * @title: AgsSFZSynth
 * @section_id:
 * @include: ags/app/machine/ags_sfz_synth.h
 *
 * The #AgsSFZSynth is a composite widget to act as sfz_synth.
 */

static gpointer ags_sfz_synth_parent_class = NULL;
static AgsConnectableInterface *ags_sfz_synth_parent_connectable_interface;

GHashTable *ags_sfz_synth_sfz_loader_completed = NULL;

GType
ags_sfz_synth_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_sfz_synth = 0;

    static const GTypeInfo ags_sfz_synth_info = {
      sizeof(AgsSFZSynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sfz_synth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSFZSynth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sfz_synth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_sfz_synth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_sfz_synth = g_type_register_static(AGS_TYPE_MACHINE,
						"AgsSFZSynth", &ags_sfz_synth_info,
						0);
    
    g_type_add_interface_static(ags_type_sfz_synth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_sfz_synth);
  }

  return(g_define_type_id__static);
}

void
ags_sfz_synth_class_init(AgsSFZSynthClass *sfz_synth)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_sfz_synth_parent_class = g_type_class_peek_parent(sfz_synth);

  /* GObjectClass */
  gobject = (GObjectClass *) sfz_synth;

  gobject->finalize = ags_sfz_synth_finalize;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) sfz_synth;

  machine->map_recall = ags_sfz_synth_map_recall;

  machine->refresh_port = ags_sfz_synth_refresh_port;
}

void
ags_sfz_synth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_sfz_synth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_sfz_synth_connect;
  connectable->disconnect = ags_sfz_synth_disconnect;
}

void
ags_sfz_synth_init(AgsSFZSynth *sfz_synth)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  GtkBox *vbox;
  GtkBox *sfz_hbox;
  GtkBox *sfz_file_hbox;
  GtkBox *sfz_synth_pitch_type_hbox;
  GtkBox *sfz_opcode_hbox;
  GtkBox *effect_vbox;
  GtkGrid *synth_grid;
  GtkGrid *chorus_grid;
  GtkBox *ext_hbox;
  GtkFrame *tremolo_frame;
  GtkGrid *tremolo_grid;
  GtkFrame *vibrato_frame;
  GtkGrid *vibrato_grid;
  GtkFrame *wah_wah_frame;
  GtkGrid *wah_wah_grid;
  GtkTreeView *sfz_opcode_tree_view;
  GtkTreeViewColumn *sfz_opcode_column;
  GtkTreeViewColumn *sfz_opcode_value_column;
  GtkScrolledWindow *scrolled_window;
  GtkLabel *label;

  GtkCellRenderer *sfz_opcode_renderer;
  GtkCellRenderer *sfz_opcode_value_renderer;

  GtkListStore *sfz_opcode;

  GtkAdjustment *adjustment;

  AgsAudio *audio;

  AgsMachineCounterManager *machine_counter_manager;
  AgsMachineCounter *machine_counter;
  
  AgsApplicationContext *application_context;
  
  gchar *machine_name;

  gint position;

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

  application_context = ags_application_context_get_instance();
  
  /* machine counter */
  machine_counter_manager = ags_machine_counter_manager_get_instance();

  machine_counter = ags_machine_counter_manager_find_machine_counter(machine_counter_manager,
								     AGS_TYPE_SFZ_SYNTH);

  machine_name = NULL;

  if(machine_counter != NULL){
    machine_name = g_strdup_printf("Default %d",
				   machine_counter->counter);
  
    ags_machine_counter_increment(machine_counter);
  }
  
  g_object_set(sfz_synth,
	       "machine-name", machine_name,
	       NULL);

  g_free(machine_name);
  
  /* machine selector */
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  position = g_list_length(window->machine);
  
  ags_machine_selector_popup_insert_machine(composite_editor->machine_selector,
					    position,
					    (AgsMachine *) sfz_synth);
  
  audio = AGS_MACHINE(sfz_synth)->audio;

  ags_audio_set_flags(audio, (AGS_AUDIO_SYNC |
			      AGS_AUDIO_ASYNC |
			      AGS_AUDIO_OUTPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_RECYCLING));
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
  
  AGS_MACHINE(sfz_synth)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
				    AGS_MACHINE_REVERSE_NOTATION);
  AGS_MACHINE(sfz_synth)->file_input_flags |= AGS_MACHINE_ACCEPT_SFZ;

  /* audio resize */
  g_signal_connect_after(G_OBJECT(sfz_synth), "resize-audio-channels",
			 G_CALLBACK(ags_sfz_synth_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(sfz_synth), "resize-pads",
			 G_CALLBACK(ags_sfz_synth_resize_pads), NULL);

  /* flags */
  sfz_synth->flags = 0;

  /* mapped IO */
  sfz_synth->mapped_output_audio_channel = 0;
  sfz_synth->mapped_input_audio_channel = 0;

  sfz_synth->mapped_input_pad = 0;
  sfz_synth->mapped_output_pad = 0;

  sfz_synth->playback_play_container = ags_recall_container_new();
  sfz_synth->playback_recall_container = ags_recall_container_new();

  sfz_synth->sfz_synth_play_container = ags_recall_container_new();
  sfz_synth->sfz_synth_recall_container = ags_recall_container_new();

  sfz_synth->tremolo_play_container = ags_recall_container_new();
  sfz_synth->tremolo_recall_container = ags_recall_container_new();

  sfz_synth->envelope_play_container = ags_recall_container_new();
  sfz_synth->envelope_recall_container = ags_recall_container_new();

  sfz_synth->wah_wah_play_container = ags_recall_container_new();
  sfz_synth->wah_wah_recall_container = ags_recall_container_new();

  sfz_synth->buffer_play_container = ags_recall_container_new();
  sfz_synth->buffer_recall_container = ags_recall_container_new();
  
  /* name and xml type */
  sfz_synth->name = NULL;
  sfz_synth->xml_type = "ags-sfz-synth";

  /* audio container */
  sfz_synth->audio_container = NULL;

  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_frame_set_child(AGS_MACHINE(sfz_synth)->frame,
		      (GtkWidget *) vbox);

  /* SFZ */
  sfz_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				    0);

  gtk_widget_set_valign((GtkWidget *) sfz_hbox,
			GTK_ALIGN_START);  
  gtk_widget_set_halign((GtkWidget *) sfz_hbox,
			GTK_ALIGN_START);

  gtk_widget_set_hexpand((GtkWidget *) sfz_hbox,
			 FALSE);

  gtk_box_set_spacing(sfz_hbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(vbox,
		 (GtkWidget *) sfz_hbox);

  /* file */
  sfz_file_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					 0);  

  gtk_box_set_spacing(sfz_file_hbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(sfz_hbox,
		 (GtkWidget *) sfz_file_hbox);

  sfz_synth->filename = (GtkEntry *) gtk_entry_new();

  gtk_widget_set_valign((GtkWidget *) sfz_synth->filename,
			GTK_ALIGN_START);
  
  gtk_box_append(sfz_file_hbox,
		 (GtkWidget *) sfz_synth->filename);
  
  sfz_synth->open = (GtkButton *) gtk_button_new_with_mnemonic(i18n("_Open"));

  gtk_widget_set_valign((GtkWidget *) sfz_synth->open,
			GTK_ALIGN_START);

  gtk_box_append(sfz_file_hbox,
		 (GtkWidget *) sfz_synth->open);

  sfz_synth->sfz_loader = NULL;
  
  sfz_synth->position = -1;

  sfz_synth->sfz_loader_spinner = (GtkSpinner *) gtk_spinner_new();
  gtk_box_append(sfz_file_hbox,
		 (GtkWidget *) sfz_synth->sfz_loader_spinner);
  gtk_widget_hide((GtkWidget *) sfz_synth->sfz_loader_spinner);

  /* opcode */
  sfz_opcode_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					   0);

  gtk_box_set_spacing(sfz_opcode_hbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(sfz_hbox,
		 (GtkWidget *) sfz_opcode_hbox);

  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();
  gtk_widget_set_size_request((GtkWidget *) scrolled_window,
			      AGS_SFZ_SYNTH_OPCODE_WIDTH_REQUEST,
			      AGS_SFZ_SYNTH_OPCODE_HEIGHT_REQUEST);
  gtk_scrolled_window_set_policy(scrolled_window,
				 GTK_POLICY_AUTOMATIC,
				 GTK_POLICY_ALWAYS);
  gtk_box_append(sfz_opcode_hbox,
		 (GtkWidget *) scrolled_window);
  
  sfz_opcode_tree_view =
    sfz_synth->opcode_tree_view = (GtkTreeView *) gtk_tree_view_new();
  gtk_tree_view_set_activate_on_single_click(sfz_opcode_tree_view,
					     TRUE);
  gtk_scrolled_window_set_child(scrolled_window,
				(GtkWidget *) sfz_opcode_tree_view);
    
  gtk_widget_set_size_request((GtkWidget *) sfz_opcode_tree_view,
			      AGS_SFZ_SYNTH_OPCODE_WIDTH_REQUEST,
			      AGS_SFZ_SYNTH_OPCODE_HEIGHT_REQUEST);

  sfz_opcode_renderer = gtk_cell_renderer_text_new();

  sfz_opcode_column = gtk_tree_view_column_new_with_attributes(i18n("opcode"),
							       sfz_opcode_renderer,
							       "text", 0,
							       NULL);
  gtk_tree_view_append_column(sfz_opcode_tree_view,
			      sfz_opcode_column);

  sfz_opcode_value_renderer = gtk_cell_renderer_text_new();

  sfz_opcode_value_column = gtk_tree_view_column_new_with_attributes(i18n("value"),
								     sfz_opcode_value_renderer,
								     "text", 1,
								     NULL);
  gtk_tree_view_append_column(sfz_opcode_tree_view,
			      sfz_opcode_value_column);
  
  sfz_opcode = gtk_list_store_new(2,
				  G_TYPE_STRING,
				  G_TYPE_STRING);

  gtk_tree_view_set_model(sfz_opcode_tree_view,
			  GTK_TREE_MODEL(sfz_opcode));  

  /* effect control */
  effect_vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				       0);

  gtk_box_set_spacing(effect_vbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(sfz_hbox,
		 (GtkWidget *) effect_vbox);

  /*  */
  synth_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(synth_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(synth_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_widget_set_valign((GtkWidget *) synth_grid,
			GTK_ALIGN_START);  
  gtk_widget_set_halign((GtkWidget *) synth_grid,
			GTK_ALIGN_START);

  gtk_widget_set_hexpand((GtkWidget *) synth_grid,
			 FALSE);

  gtk_box_append(effect_vbox,
		 (GtkWidget *) synth_grid);

  /* WAV 1 - octave */
  label = (GtkLabel *) gtk_label_new(i18n("WAV 1 - octave"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);

  sfz_synth->synth_octave = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->synth_octave);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);

//  sfz_synth->synth_octave->scale_max_precision = 12;
  
//  ags_dial_set_scale_precision(sfz_synth->synth_octave,
//			       12);
  ags_dial_set_radius(sfz_synth->synth_octave,
		      12);
  
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) sfz_synth->synth_octave,
		  1, 0,
		  1, 1);

  /* WAV 1 - key */
  label = (GtkLabel *) gtk_label_new(i18n("WAV 1 - key"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  sfz_synth->synth_key = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->synth_key);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   2.0);

//  sfz_synth->synth_key->scale_max_precision = 12;
//  ags_dial_set_scale_precision(sfz_synth->synth_key,
//			       12);
  ags_dial_set_radius(sfz_synth->synth_key,
		      12);
  
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) sfz_synth->synth_key,
		  1, 1,
		  1, 1);

  /* WAV 1 - volume */
  label = (GtkLabel *) gtk_label_new(i18n("WAV 1 - volume"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) label,
		  2, 0,
		  1, 1);

  sfz_synth->synth_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->synth_volume);

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

  ags_dial_set_radius(sfz_synth->synth_volume,
		      12);
  
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) sfz_synth->synth_volume,
		  3, 0,
		  1, 1);

  /* pitch type */
  sfz_synth_pitch_type_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				    AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_valign((GtkWidget *) sfz_synth_pitch_type_hbox,
			GTK_ALIGN_START);  
  gtk_widget_set_halign((GtkWidget *) sfz_synth_pitch_type_hbox,
			GTK_ALIGN_START);

  gtk_widget_set_hexpand((GtkWidget *) sfz_synth_pitch_type_hbox,
			 FALSE);

  gtk_grid_attach(synth_grid,
		  (GtkWidget *) sfz_synth_pitch_type_hbox,
		  0, 2,
		  2, 1);
  
  label = (GtkLabel *) gtk_label_new(i18n("pitch type"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_box_append(sfz_synth_pitch_type_hbox,
		 (GtkWidget *) label);

  sfz_synth->synth_pitch_type = (GtkDropDown *) gtk_drop_down_new_from_strings((const gchar * const *) pitch_type_strv);

  gtk_drop_down_set_selected(sfz_synth->synth_pitch_type,
			     2);

  gtk_box_append(sfz_synth_pitch_type_hbox,
		 (GtkWidget *) sfz_synth->synth_pitch_type);

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
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);

  sfz_synth->chorus_input_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->chorus_input_volume);

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

  ags_dial_set_radius(sfz_synth->chorus_input_volume,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sfz_synth->chorus_input_volume,
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

  sfz_synth->chorus_output_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->chorus_output_volume);

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

  ags_dial_set_radius(sfz_synth->chorus_output_volume,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sfz_synth->chorus_output_volume,
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
  
  sfz_synth->chorus_lfo_oscillator = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text((GtkComboBoxText *) sfz_synth->chorus_lfo_oscillator,
				 "sine");
  gtk_combo_box_text_append_text((GtkComboBoxText *) sfz_synth->chorus_lfo_oscillator,
				 "sawtooth");
  gtk_combo_box_text_append_text((GtkComboBoxText *) sfz_synth->chorus_lfo_oscillator,
				 "triangle");
  gtk_combo_box_text_append_text((GtkComboBoxText *) sfz_synth->chorus_lfo_oscillator,
				 "square");
  gtk_combo_box_text_append_text((GtkComboBoxText *) sfz_synth->chorus_lfo_oscillator,
				 "impulse");

  gtk_combo_box_set_active(sfz_synth->chorus_lfo_oscillator,
			   0);

  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sfz_synth->chorus_lfo_oscillator,
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

  sfz_synth->chorus_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 10.0, 0.01);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sfz_synth->chorus_lfo_frequency,
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

  sfz_synth->chorus_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->chorus_depth);

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

  ags_dial_set_radius(sfz_synth->chorus_depth,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sfz_synth->chorus_depth,
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

  sfz_synth->chorus_mix = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->chorus_mix);

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

  ags_dial_set_radius(sfz_synth->chorus_mix,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sfz_synth->chorus_mix,
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

  sfz_synth->chorus_delay = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->chorus_delay);

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

  ags_dial_set_radius(sfz_synth->chorus_delay,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sfz_synth->chorus_delay,
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

  sfz_synth->tremolo_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) sfz_synth->tremolo_enabled,
		  0, 0,
		  2, 1);

  label = (GtkLabel *) gtk_label_new(i18n("gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  sfz_synth->tremolo_gain = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(sfz_synth->tremolo_gain,
		      12);

  adjustment = ags_dial_get_adjustment(sfz_synth->tremolo_gain);

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
		  (GtkWidget *) sfz_synth->tremolo_gain,
		  1, 1,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("lfo-depth"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);

  sfz_synth->tremolo_lfo_depth = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(sfz_synth->tremolo_lfo_depth,
		      12);

  adjustment = ags_dial_get_adjustment(sfz_synth->tremolo_lfo_depth);

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
		  (GtkWidget *) sfz_synth->tremolo_lfo_depth,
		  1, 2,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("lfo-freq"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);

  sfz_synth->tremolo_lfo_freq = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(sfz_synth->tremolo_lfo_freq,
		      12);

  adjustment = ags_dial_get_adjustment(sfz_synth->tremolo_lfo_freq);

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
		  (GtkWidget *) sfz_synth->tremolo_lfo_freq,
		  1, 3,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);

  sfz_synth->tremolo_tuning = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(sfz_synth->tremolo_tuning,
		      12);

  adjustment = ags_dial_get_adjustment(sfz_synth->tremolo_tuning);

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
		  (GtkWidget *) sfz_synth->tremolo_tuning,
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

  sfz_synth->vibrato_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) sfz_synth->vibrato_enabled,
		  0, 0,
		  2, 1);

  label = (GtkLabel *) gtk_label_new(i18n("gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  sfz_synth->vibrato_gain = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(sfz_synth->vibrato_gain,
		      12);

  adjustment = ags_dial_get_adjustment(sfz_synth->vibrato_gain);

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
		  (GtkWidget *) sfz_synth->vibrato_gain,
		  1, 1,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("lfo-depth"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);

  sfz_synth->vibrato_lfo_depth = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(sfz_synth->vibrato_lfo_depth,
		      12);

  adjustment = ags_dial_get_adjustment(sfz_synth->vibrato_lfo_depth);

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
		  (GtkWidget *) sfz_synth->vibrato_lfo_depth,
		  1, 2,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("lfo-freq"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);

  sfz_synth->vibrato_lfo_freq = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(sfz_synth->vibrato_lfo_freq,
		      12);
  
  adjustment = ags_dial_get_adjustment(sfz_synth->vibrato_lfo_freq);

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
		  (GtkWidget *) sfz_synth->vibrato_lfo_freq,
		  1, 3,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);

  sfz_synth->vibrato_tuning = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(sfz_synth->vibrato_tuning,
		      12);

  adjustment = ags_dial_get_adjustment(sfz_synth->vibrato_tuning);

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
		  (GtkWidget *) sfz_synth->vibrato_tuning,
		  1, 4,
		  1, 1);
  
  /* wah-wah */
  wah_wah_frame = (GtkFrame *) gtk_frame_new(i18n("wah-wah"));

  gtk_box_append(ext_hbox,
		 (GtkWidget *) wah_wah_frame);

  wah_wah_grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(wah_wah_grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(wah_wah_grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_frame_set_child(wah_wah_frame,
		      (GtkWidget *) wah_wah_grid);
  
  sfz_synth->wah_wah_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));
  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sfz_synth->wah_wah_enabled,
		  0, 0,
		  2, 1);

  label = (GtkLabel *) gtk_label_new(i18n("length"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  sfz_synth->wah_wah_length = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text((GtkComboBoxText *) sfz_synth->wah_wah_length,
				 "1/1");
  gtk_combo_box_text_append_text((GtkComboBoxText *) sfz_synth->wah_wah_length,
				 "2/2");
  gtk_combo_box_text_append_text((GtkComboBoxText *) sfz_synth->wah_wah_length,
				 "4/4");
  gtk_combo_box_text_append_text((GtkComboBoxText *) sfz_synth->wah_wah_length,
				 "8/8");
  gtk_combo_box_text_append_text((GtkComboBoxText *) sfz_synth->wah_wah_length,
				 "16/16");

  gtk_combo_box_set_active(sfz_synth->wah_wah_length,
			   2);

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sfz_synth->wah_wah_length,
		  1, 1,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("attack [x|y]"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) label,
		  2, 1,
		  1, 1);

  sfz_synth->wah_wah_attack_x = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->wah_wah_attack_x);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.25);

  ags_dial_set_radius(sfz_synth->wah_wah_attack_x,
		      12);

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sfz_synth->wah_wah_attack_x,
		  3, 1,
		  1, 1);

  sfz_synth->wah_wah_attack_y = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->wah_wah_attack_y);

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

  ags_dial_set_radius(sfz_synth->wah_wah_attack_y,
		      12);

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sfz_synth->wah_wah_attack_y,
		  4, 1,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("decay [x|y]"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) label,
		  2, 2,
		  1, 1);

  sfz_synth->wah_wah_decay_x = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->wah_wah_decay_x);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.25);

  ags_dial_set_radius(sfz_synth->wah_wah_decay_x,
		      12);

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sfz_synth->wah_wah_decay_x,
		  3, 2,
		  1, 1);

  sfz_synth->wah_wah_decay_y = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->wah_wah_decay_y);

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

  ags_dial_set_radius(sfz_synth->wah_wah_decay_y,
		      12);

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sfz_synth->wah_wah_decay_y,
		  4, 2,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("sustain [x|y]"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) label,
		  2, 3,
		  1, 1);

  sfz_synth->wah_wah_sustain_x = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->wah_wah_sustain_x);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.25);

  ags_dial_set_radius(sfz_synth->wah_wah_sustain_x,
		      12);

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sfz_synth->wah_wah_sustain_x,
		  3, 3,
		  1, 1);

  sfz_synth->wah_wah_sustain_y = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->wah_wah_sustain_y);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.5);

  ags_dial_set_radius(sfz_synth->wah_wah_sustain_y,
		      12);

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sfz_synth->wah_wah_sustain_y,
		  4, 3,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("release [x|y]"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) label,
		  2, 4,
		  1, 1);

  sfz_synth->wah_wah_release_x = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->wah_wah_release_x);

  gtk_adjustment_set_lower(adjustment,
			   0.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.25);

  ags_dial_set_radius(sfz_synth->wah_wah_release_x,
		      12);

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sfz_synth->wah_wah_release_x,
		  3, 4,
		  1, 1);

  sfz_synth->wah_wah_release_y = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->wah_wah_release_y);

  gtk_adjustment_set_lower(adjustment,
			   -1.0);
  gtk_adjustment_set_upper(adjustment,
			   1.0);

  gtk_adjustment_set_step_increment(adjustment,
				    0.01);
  gtk_adjustment_set_page_increment(adjustment,
				    0.1);

  gtk_adjustment_set_value(adjustment,
			   0.5);
  
  ags_dial_set_radius(sfz_synth->wah_wah_release_y,
		      12);

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sfz_synth->wah_wah_release_y,
		  4, 4,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("ratio"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) label,
		  2, 5,
		  1, 1);

  sfz_synth->wah_wah_ratio = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sfz_synth->wah_wah_ratio);

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

  ags_dial_set_radius(sfz_synth->wah_wah_ratio,
		      12);

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sfz_synth->wah_wah_ratio,
		  3, 5,
		  1, 1);
  
  label = (GtkLabel *) gtk_label_new(i18n("lfo-depth"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) label,
		  5, 1,
		  1, 1);

  sfz_synth->wah_wah_lfo_depth = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(sfz_synth->wah_wah_lfo_depth,
		      12);

  adjustment = ags_dial_get_adjustment(sfz_synth->wah_wah_lfo_depth);

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

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sfz_synth->wah_wah_lfo_depth,
		  6, 1,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("lfo-freq"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) label,
		  5, 2,
		  1, 1);

  sfz_synth->wah_wah_lfo_freq = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(sfz_synth->wah_wah_lfo_freq,
		      12);

  adjustment = ags_dial_get_adjustment(sfz_synth->wah_wah_lfo_freq);

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

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sfz_synth->wah_wah_lfo_freq,
		  6, 2,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) label,
		  5, 3,
		  1, 1);

  sfz_synth->wah_wah_tuning = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(sfz_synth->wah_wah_tuning,
		      12);

  adjustment = ags_dial_get_adjustment(sfz_synth->wah_wah_tuning);

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

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sfz_synth->wah_wah_tuning,
		  6, 3,
		  1, 1);

  sfz_synth->wah_wah_drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();

  gtk_drawing_area_set_content_width(sfz_synth->wah_wah_drawing_area,
				     480);
  gtk_drawing_area_set_content_height(sfz_synth->wah_wah_drawing_area,
				      120);
  
  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sfz_synth->wah_wah_drawing_area,
		  7, 1,
		  1, 4);

  gtk_drawing_area_set_draw_func(sfz_synth->wah_wah_drawing_area,
				 (GtkDrawingAreaDrawFunc) ags_sfz_synth_wah_wah_draw_function,
				 sfz_synth,
				 NULL);

  /* dialog */
  sfz_synth->open_dialog = NULL;

  /* SFZ loader */
  if(ags_sfz_synth_sfz_loader_completed == NULL){
    ags_sfz_synth_sfz_loader_completed = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							       NULL,
							       NULL);
  }

  g_hash_table_insert(ags_sfz_synth_sfz_loader_completed,
		      sfz_synth, ags_sfz_synth_sfz_loader_completed_timeout);
  g_timeout_add(1000 / 4, (GSourceFunc) ags_sfz_synth_sfz_loader_completed_timeout, (gpointer) sfz_synth);
}

void
ags_sfz_synth_finalize(GObject *gobject)
{
  AgsSFZSynth *sfz_synth;

  sfz_synth = AGS_SFZ_SYNTH(gobject);

  g_hash_table_remove(ags_sfz_synth_sfz_loader_completed,
		      sfz_synth);
  
  /* call parent */
  G_OBJECT_CLASS(ags_sfz_synth_parent_class)->finalize(gobject);
}

void
ags_sfz_synth_connect(AgsConnectable *connectable)
{
  AgsSFZSynth *sfz_synth;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  ags_sfz_synth_parent_connectable_interface->connect(connectable);

  /* AgsSFZSynth */
  sfz_synth = AGS_SFZ_SYNTH(connectable);

  g_signal_connect((GObject *) sfz_synth, "destroy",
		   G_CALLBACK(ags_sfz_synth_destroy_callback), (gpointer) sfz_synth);  

  g_signal_connect((GObject *) sfz_synth->open, "clicked",
		   G_CALLBACK(ags_sfz_synth_open_clicked_callback), (gpointer) sfz_synth);

  g_signal_connect((GObject *) sfz_synth->synth_pitch_type, "notify::activate",
		   G_CALLBACK(ags_sfz_synth_synth_pitch_type_callback), (gpointer) sfz_synth);

  g_signal_connect_after(sfz_synth->synth_octave, "value-changed",
			 G_CALLBACK(ags_sfz_synth_synth_octave_callback), sfz_synth);
  
  g_signal_connect_after(sfz_synth->synth_key, "value-changed",
			 G_CALLBACK(ags_sfz_synth_synth_key_callback), sfz_synth);
  
  g_signal_connect_after(sfz_synth->synth_volume, "value-changed",
			 G_CALLBACK(ags_sfz_synth_synth_volume_callback), sfz_synth);

  //  g_signal_connect_after(sfz_synth->chorus_enabled, "toggled",
//			 G_CALLBACK(ags_sfz_synth_chorus_enabled_callback), sfz_synth);
  
  g_signal_connect_after(sfz_synth->chorus_input_volume, "value-changed",
			 G_CALLBACK(ags_sfz_synth_chorus_input_volume_callback), sfz_synth);
  
  g_signal_connect_after(sfz_synth->chorus_output_volume, "value-changed",
			 G_CALLBACK(ags_sfz_synth_chorus_output_volume_callback), sfz_synth);
  
  g_signal_connect_after(sfz_synth->chorus_lfo_oscillator, "changed",
			 G_CALLBACK(ags_sfz_synth_chorus_lfo_oscillator_callback), sfz_synth);
  
  g_signal_connect_after(sfz_synth->chorus_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_sfz_synth_chorus_lfo_frequency_callback), sfz_synth);
  
  g_signal_connect_after(sfz_synth->chorus_depth, "value-changed",
			 G_CALLBACK(ags_sfz_synth_chorus_depth_callback), sfz_synth);
  
  g_signal_connect_after(sfz_synth->chorus_mix, "value-changed",
			 G_CALLBACK(ags_sfz_synth_chorus_mix_callback), sfz_synth);
  
  g_signal_connect_after(sfz_synth->chorus_delay, "value-changed",
			 G_CALLBACK(ags_sfz_synth_chorus_delay_callback), sfz_synth);

  g_signal_connect_after(sfz_synth->tremolo_enabled, "toggled",
			 G_CALLBACK(ags_sfz_synth_tremolo_enabled_callback), sfz_synth);

  g_signal_connect_after(sfz_synth->tremolo_gain, "value-changed",
			 G_CALLBACK(ags_sfz_synth_tremolo_gain_callback), sfz_synth);

  g_signal_connect_after(sfz_synth->tremolo_lfo_depth, "value-changed",
			 G_CALLBACK(ags_sfz_synth_tremolo_lfo_depth_callback), sfz_synth);

  g_signal_connect_after(sfz_synth->tremolo_lfo_freq, "value-changed",
			 G_CALLBACK(ags_sfz_synth_tremolo_lfo_freq_callback), sfz_synth);

  g_signal_connect_after(sfz_synth->tremolo_tuning, "value-changed",
			 G_CALLBACK(ags_sfz_synth_tremolo_tuning_callback), sfz_synth);

  g_signal_connect_after(sfz_synth->vibrato_enabled, "toggled",
			 G_CALLBACK(ags_sfz_synth_vibrato_enabled_callback), sfz_synth);

  g_signal_connect_after(sfz_synth->vibrato_gain, "value-changed",
			 G_CALLBACK(ags_sfz_synth_vibrato_gain_callback), sfz_synth);

  g_signal_connect_after(sfz_synth->vibrato_lfo_depth, "value-changed",
			 G_CALLBACK(ags_sfz_synth_vibrato_lfo_depth_callback), sfz_synth);

  g_signal_connect_after(sfz_synth->vibrato_lfo_freq, "value-changed",
			 G_CALLBACK(ags_sfz_synth_vibrato_lfo_freq_callback), sfz_synth);

  g_signal_connect_after(sfz_synth->vibrato_tuning, "value-changed",
			 G_CALLBACK(ags_sfz_synth_vibrato_tuning_callback), sfz_synth);

  g_signal_connect_after(sfz_synth->wah_wah_enabled, "toggled",
			 G_CALLBACK(ags_sfz_synth_wah_wah_enabled_callback), sfz_synth);

  g_signal_connect_after(sfz_synth->wah_wah_length, "changed",
			 G_CALLBACK(ags_sfz_synth_wah_wah_length_callback), sfz_synth);
  
  g_signal_connect_after(sfz_synth->wah_wah_attack_x, "value-changed",
			 G_CALLBACK(ags_sfz_synth_wah_wah_attack_callback), sfz_synth);

  g_signal_connect_after(sfz_synth->wah_wah_attack_y, "value-changed",
			 G_CALLBACK(ags_sfz_synth_wah_wah_attack_callback), sfz_synth);
  
  g_signal_connect_after(sfz_synth->wah_wah_decay_x, "value-changed",
			 G_CALLBACK(ags_sfz_synth_wah_wah_decay_callback), sfz_synth);

  g_signal_connect_after(sfz_synth->wah_wah_decay_y, "value-changed",
			 G_CALLBACK(ags_sfz_synth_wah_wah_decay_callback), sfz_synth);
  
  g_signal_connect_after(sfz_synth->wah_wah_sustain_x, "value-changed",
			 G_CALLBACK(ags_sfz_synth_wah_wah_sustain_callback), sfz_synth);

  g_signal_connect_after(sfz_synth->wah_wah_sustain_y, "value-changed",
			 G_CALLBACK(ags_sfz_synth_wah_wah_sustain_callback), sfz_synth);

  g_signal_connect_after(sfz_synth->wah_wah_release_x, "value-changed",
			 G_CALLBACK(ags_sfz_synth_wah_wah_release_callback), sfz_synth);

  g_signal_connect_after(sfz_synth->wah_wah_release_y, "value-changed",
			 G_CALLBACK(ags_sfz_synth_wah_wah_release_callback), sfz_synth);

  g_signal_connect_after(sfz_synth->wah_wah_ratio, "value-changed",
			 G_CALLBACK(ags_sfz_synth_wah_wah_ratio_callback), sfz_synth);

  g_signal_connect_after(sfz_synth->wah_wah_lfo_depth, "value-changed",
			 G_CALLBACK(ags_sfz_synth_wah_wah_lfo_depth_callback), sfz_synth);

  g_signal_connect_after(sfz_synth->wah_wah_lfo_freq, "value-changed",
			 G_CALLBACK(ags_sfz_synth_wah_wah_lfo_freq_callback), sfz_synth);

  g_signal_connect_after(sfz_synth->wah_wah_tuning, "value-changed",
			 G_CALLBACK(ags_sfz_synth_wah_wah_tuning_callback), sfz_synth);
}

void
ags_sfz_synth_disconnect(AgsConnectable *connectable)
{
  AgsSFZSynth *sfz_synth;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  ags_sfz_synth_parent_connectable_interface->disconnect(connectable);

  /* AgsSFZSynth */
  sfz_synth = AGS_SFZ_SYNTH(connectable);

  g_object_disconnect((GObject *) sfz_synth,
		      "any_signal::destroy",
		      G_CALLBACK(ags_sfz_synth_destroy_callback),
		      (gpointer) sfz_synth,
		      NULL);  

  g_object_disconnect((GObject *) sfz_synth->open,
		      "any_signal::clicked",
		      G_CALLBACK(ags_sfz_synth_open_clicked_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->synth_pitch_type,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_sfz_synth_synth_pitch_type_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->synth_octave,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_synth_octave_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->synth_key,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_synth_key_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->synth_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_synth_volume_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->chorus_input_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_chorus_input_volume_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->chorus_output_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_chorus_output_volume_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->chorus_lfo_oscillator,
		      "any_signal::changed",
		      G_CALLBACK(ags_sfz_synth_chorus_lfo_oscillator_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->chorus_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_chorus_lfo_frequency_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->chorus_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_chorus_depth_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->chorus_mix,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_chorus_mix_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->chorus_delay,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_chorus_delay_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->tremolo_enabled,
		      "any_signal::toggled",
		      G_CALLBACK(ags_sfz_synth_tremolo_enabled_callback),
		      sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->tremolo_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_tremolo_gain_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->tremolo_lfo_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_tremolo_lfo_depth_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->tremolo_lfo_freq,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_tremolo_lfo_freq_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->tremolo_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_tremolo_tuning_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->vibrato_enabled,
		      "any_signal::toggled",
		      G_CALLBACK(ags_sfz_synth_vibrato_enabled_callback),
		      sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->vibrato_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_vibrato_gain_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->vibrato_lfo_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_vibrato_lfo_depth_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->vibrato_lfo_freq,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_vibrato_lfo_freq_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->wah_wah_enabled,
		      "any_signal::toggled",
		      G_CALLBACK(ags_sfz_synth_wah_wah_enabled_callback),
		      sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->wah_wah_length,
		      "any_signal::changed",
		      G_CALLBACK(ags_sfz_synth_wah_wah_length_callback),
		      sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->wah_wah_attack_x,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_wah_wah_attack_callback),
		      sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->wah_wah_attack_y,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_wah_wah_attack_callback),
		      sfz_synth,
		      NULL);
  
  g_object_disconnect((GObject *) sfz_synth->wah_wah_decay_x,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_wah_wah_decay_callback),
		      sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->wah_wah_decay_y,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_wah_wah_decay_callback),
		      sfz_synth,
		      NULL);
  
  g_object_disconnect((GObject *) sfz_synth->wah_wah_sustain_x,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_wah_wah_sustain_callback),
		      sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->wah_wah_sustain_y,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_wah_wah_sustain_callback),
		      sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->wah_wah_release_x,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_wah_wah_release_callback),
		      sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->wah_wah_release_y,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_wah_wah_release_callback),
		      sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->wah_wah_ratio,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_wah_wah_ratio_callback),
		      sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->wah_wah_lfo_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_wah_wah_lfo_depth_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->wah_wah_lfo_freq,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_wah_wah_lfo_freq_callback),
		      (gpointer) sfz_synth,
		      NULL);

  g_object_disconnect((GObject *) sfz_synth->wah_wah_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sfz_synth_wah_wah_tuning_callback),
		      (gpointer) sfz_synth,
		      NULL);
}

void
ags_sfz_synth_wah_wah_draw_function(GtkDrawingArea *area,
				    cairo_t *cr,
				    int width,
				    int height,
				    AgsSFZSynth *sfz_synth)
{
  GtkStyleContext *context;

  gdouble position_x;
  
  context = gtk_widget_get_style_context (GTK_WIDGET (area));

  cairo_set_source_rgba(cr,
			0.0,
			0.0,
			0.0,
			1.0);
  
  cairo_rectangle(cr,
		  0.0, 0.0,
		  (double) width, (double) height);

  cairo_fill(cr);

  /* wah-wah */
  cairo_set_source_rgba(cr,
			0.0,
			1.0,
			1.0,
			1.0);
  
  cairo_set_line_width(cr,
		       2.5);

  /* attack */
  position_x = 0.0;
  
  cairo_move_to(cr,
		position_x, 120.0 - 120.0 * ags_dial_get_value(sfz_synth->wah_wah_ratio));

  cairo_line_to(cr,
		position_x + 480.0 * ags_dial_get_value(sfz_synth->wah_wah_attack_x), 120.0 - 120.0 * (ags_dial_get_value(sfz_synth->wah_wah_ratio) + ags_dial_get_value(sfz_synth->wah_wah_attack_y)));

  /* decay */
  position_x += 480.0 * ags_dial_get_value(sfz_synth->wah_wah_attack_x);
  
  cairo_move_to(cr,
		position_x, 120.0 - 120.0 * (ags_dial_get_value(sfz_synth->wah_wah_ratio) + ags_dial_get_value(sfz_synth->wah_wah_attack_y)));

  cairo_line_to(cr,
		position_x + 480.0 * ags_dial_get_value(sfz_synth->wah_wah_decay_x), 120.0 - 120.0 * (ags_dial_get_value(sfz_synth->wah_wah_ratio) + ags_dial_get_value(sfz_synth->wah_wah_decay_y)));

  /* sustain */
  position_x += 480.0 * ags_dial_get_value(sfz_synth->wah_wah_decay_x);
  
  cairo_move_to(cr,
		position_x, 120.0 - 120.0 * (ags_dial_get_value(sfz_synth->wah_wah_ratio) + ags_dial_get_value(sfz_synth->wah_wah_decay_y)));

  cairo_line_to(cr,
		position_x + 480.0 * ags_dial_get_value(sfz_synth->wah_wah_sustain_x), 120.0 - 120.0 * (ags_dial_get_value(sfz_synth->wah_wah_ratio) + ags_dial_get_value(sfz_synth->wah_wah_sustain_y)));

  /* release */
  position_x += 480.0 * ags_dial_get_value(sfz_synth->wah_wah_sustain_x);
  
  cairo_move_to(cr,
		position_x, 120.0 - 120.0 * (ags_dial_get_value(sfz_synth->wah_wah_ratio) + ags_dial_get_value(sfz_synth->wah_wah_sustain_y)));

  cairo_line_to(cr,
		position_x + 480.0 * ags_dial_get_value(sfz_synth->wah_wah_release_x), 120.0 - 120.0 * (ags_dial_get_value(sfz_synth->wah_wah_ratio) + ags_dial_get_value(sfz_synth->wah_wah_release_y)));

  cairo_stroke(cr);
}

void
ags_sfz_synth_resize_audio_channels(AgsMachine *machine,
				    guint audio_channels, guint audio_channels_old,
				    gpointer data)
{
  AgsSFZSynth *sfz_synth;

  sfz_synth = (AgsSFZSynth *) machine;

  if(audio_channels > audio_channels_old){    
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_sfz_synth_input_map_recall(sfz_synth,
				     audio_channels_old,
				     0);
      
      ags_sfz_synth_output_map_recall(sfz_synth,
				      audio_channels_old,
				      0);
    }
  }
}

void
ags_sfz_synth_resize_pads(AgsMachine *machine, GType type,
			  guint pads, guint pads_old,
			  gpointer data)
{
  AgsSFZSynth *sfz_synth;

  gboolean grow;

  sfz_synth = (AgsSFZSynth *) machine;
  
  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(type == AGS_TYPE_INPUT){
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	/* depending on destination */
	ags_sfz_synth_input_map_recall(sfz_synth,
				       0,
				       pads_old);
      }
    }else{
      sfz_synth->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_sfz_synth_output_map_recall(sfz_synth,
					0,
					pads_old);
      }
    }else{
      sfz_synth->mapped_output_pad = pads;
    }
  }
}

void
ags_sfz_synth_map_recall(AgsMachine *machine)
{
  AgsSFZSynth *sfz_synth;
  
  AgsAudio *audio;

  GList *start_recall;

  gint position;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  sfz_synth = AGS_SFZ_SYNTH(machine);

  audio = machine->audio;
  
  position = 0;
  
  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       sfz_synth->playback_play_container, sfz_synth->playback_recall_container,
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

  /* ags-fx-sfz-synth */
  start_recall = ags_fx_factory_create(audio,
				       sfz_synth->sfz_synth_play_container, sfz_synth->sfz_synth_recall_container,
				       "ags-fx-sfz-synth",
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
				       sfz_synth->tremolo_play_container, sfz_synth->tremolo_recall_container,
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

  /* ags-fx-envelope */
  start_recall = ags_fx_factory_create(audio,
				       sfz_synth->envelope_play_container, sfz_synth->envelope_recall_container,
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

  /* ags-fx-wah-wah */
  start_recall = ags_fx_factory_create(audio,
				       sfz_synth->wah_wah_play_container, sfz_synth->wah_wah_recall_container,
				       "ags-fx-wah-wah",
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
				       sfz_synth->buffer_play_container, sfz_synth->buffer_recall_container,
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
  ags_sfz_synth_input_map_recall(sfz_synth,
				 0,
				 0);

  /* depending on destination */
  ags_sfz_synth_output_map_recall(sfz_synth,
				  0,
				  0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_sfz_synth_parent_class)->map_recall(machine);  
}

void
ags_sfz_synth_input_map_recall(AgsSFZSynth *sfz_synth,
			       guint audio_channel_start,
			       guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  guint input_pads;
  guint audio_channels;
  gint position;
  guint i;
  guint j;

  audio = AGS_MACHINE(sfz_synth)->audio;

  position = 0;

  /* get some fields */
  input_pads = AGS_MACHINE(sfz_synth)->input_pads;
  audio_channels = AGS_MACHINE(sfz_synth)->audio_channels;    
  
  for(i = 0; i < input_pads; i++){
    for(j = 0; j < audio_channels; j++){
      AgsMachineInputLine* input_line;

      input_line = g_list_nth_data(AGS_MACHINE(sfz_synth)->machine_input_line,
				   (i * audio_channels) + j);

      if(input_line != NULL &&
	 input_line->mapped_recall == FALSE){
	/* ags-fx-playback */
	start_recall = ags_fx_factory_create(audio,
					     sfz_synth->playback_play_container, sfz_synth->playback_recall_container,
					     "ags-fx-playback",
					     NULL,
					     NULL,
					     j, j + 1,
					     i, i + 1,
					     position,
					     (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

	g_list_free_full(start_recall,
			 (GDestroyNotify) g_object_unref);

	/* ags-fx-sfz-synth */
	start_recall = ags_fx_factory_create(audio,
					     sfz_synth->sfz_synth_play_container, sfz_synth->sfz_synth_recall_container,
					     "ags-fx-sfz-synth",
					     NULL,
					     NULL,
					     j, j + 1,
					     i, i + 1,
					     position,
					     (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

	g_list_free_full(start_recall,
			 (GDestroyNotify) g_object_unref);

	/* ags-fx-tremolo */
	start_recall = ags_fx_factory_create(audio,
					     sfz_synth->tremolo_play_container, sfz_synth->tremolo_recall_container,
					     "ags-fx-tremolo",
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
					     sfz_synth->envelope_play_container, sfz_synth->envelope_recall_container,
					     "ags-fx-envelope",
					     NULL,
					     NULL,
					     j, j + 1,
					     i, i + 1,
					     position,
					     (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

	g_list_free_full(start_recall,
			 (GDestroyNotify) g_object_unref);

	/* ags-fx-wah-wah */
	start_recall = ags_fx_factory_create(audio,
					     sfz_synth->wah_wah_play_container, sfz_synth->wah_wah_recall_container,
					     "ags-fx-wah-wah",
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
					     sfz_synth->buffer_play_container, sfz_synth->buffer_recall_container,
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
  
  sfz_synth->mapped_input_audio_channel = audio_channels;
  sfz_synth->mapped_input_pad = input_pads;
}

void
ags_sfz_synth_output_map_recall(AgsSFZSynth *sfz_synth,
				guint audio_channel_start,
				guint output_pad_start)
{
  guint output_pads;
  guint audio_channels;

  if(sfz_synth->mapped_output_pad > output_pad_start){
    return;
  }
  
  /* get some fields */
  output_pads = AGS_MACHINE(sfz_synth)->output_pads;
  audio_channels = AGS_MACHINE(sfz_synth)->audio_channels;    
  
  sfz_synth->mapped_output_audio_channel = audio_channels;
  sfz_synth->mapped_output_pad = output_pads;
}


void
ags_sfz_synth_refresh_port(AgsMachine *machine)
{
  AgsSFZSynth *sfz_synth;
  AgsChannel *start_channel, *channel;
  
  GList *start_play, *start_recall, *recall;

  sfz_synth = (AgsSFZSynth *) machine;
  
  start_play = ags_audio_get_play(machine->audio);
  start_recall = ags_audio_get_recall(machine->audio);

  recall =
    start_recall = g_list_concat(start_play, start_recall);

  machine->flags |= AGS_MACHINE_NO_UPDATE;

  if((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SFZ_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    /* synth octave */
    port = NULL;

    g_object_get(recall->data,
		 "synth-octave", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(sfz_synth->synth_octave,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth key */
    port = NULL;

    g_object_get(recall->data,
		 "synth-key", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(sfz_synth->synth_key,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }

    /* synth volume */
    port = NULL;

    g_object_get(recall->data,
		 "synth-volume", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      ags_port_safe_read(port,
			 &value);

      ags_dial_set_value(sfz_synth->synth_volume,
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
	gtk_check_button_set_active(sfz_synth->chorus_enabled,
				    TRUE);
      }else{
	gtk_check_button_set_active(sfz_synth->chorus_enabled,
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

      gtk_combo_box_set_active(sfz_synth->chorus_lfo_oscillator,
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

      gtk_spin_button_set_value(sfz_synth->chorus_lfo_frequency,
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

      ags_dial_set_value(sfz_synth->chorus_depth,
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

      ags_dial_set_value(sfz_synth->chorus_mix,
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

      ags_dial_set_value(sfz_synth->chorus_delay,
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

      ags_dial_set_value(sfz_synth->vibrato_gain,
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

      ags_dial_set_value(sfz_synth->vibrato_lfo_depth,
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

      ags_dial_set_value(sfz_synth->vibrato_lfo_freq,
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

      ags_dial_set_value(sfz_synth->vibrato_tuning,
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

      ags_dial_set_value(sfz_synth->tremolo_gain,
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

      ags_dial_set_value(sfz_synth->tremolo_lfo_depth,
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

      ags_dial_set_value(sfz_synth->tremolo_lfo_freq,
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

      ags_dial_set_value(sfz_synth->tremolo_tuning,
			 (gdouble) g_value_get_float(&value));

      g_object_unref(port);
    }
  }

  start_channel =
    channel = ags_audio_get_input(machine->audio);

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
  
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    if((recall = ags_recall_find_type(recall, AGS_TYPE_FX_WAH_WAH_CHANNEL)) != NULL){
      AgsPort *port;

      /* wah-wah attack */
      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-attack", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	ags_port_safe_read(port,
			   &value);

	ags_dial_set_value(sfz_synth->wah_wah_attack_x,
			   (gdouble) creal((double _Complex) g_value_get_float(&value)));

	ags_dial_set_value(sfz_synth->wah_wah_attack_y,
			   (gdouble) cimag((double _Complex) g_value_get_float(&value)));
	
	g_object_unref(port);
      }

      /* wah-wah decay */
      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-decay", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	ags_port_safe_read(port,
			   &value);

	ags_dial_set_value(sfz_synth->wah_wah_decay_x,
			   (gdouble) creal((double _Complex) g_value_get_float(&value)));

	ags_dial_set_value(sfz_synth->wah_wah_decay_y,
			   (gdouble) cimag((double _Complex) g_value_get_float(&value)));

	g_object_unref(port);
      }

      /* wah-wah sustain */
      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-sustain", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	ags_port_safe_read(port,
			   &value);

	ags_dial_set_value(sfz_synth->wah_wah_sustain_x,
			   (gdouble) creal((double _Complex) g_value_get_float(&value)));

	ags_dial_set_value(sfz_synth->wah_wah_sustain_y,
			   (gdouble) cimag((double _Complex) g_value_get_float(&value)));

	g_object_unref(port);
      }

      /* wah-wah release */
      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-release", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	ags_port_safe_read(port,
			   &value);

	ags_dial_set_value(sfz_synth->wah_wah_release_x,
			   (gdouble) creal((double _Complex) g_value_get_float(&value)));

	ags_dial_set_value(sfz_synth->wah_wah_release_y,
			   (gdouble) cimag((double _Complex) g_value_get_float(&value)));

	g_object_unref(port);
      }

      /* wah-wah LFO depth */
      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-lfo-depth", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	ags_port_safe_read(port,
			   &value);

	ags_dial_set_value(sfz_synth->wah_wah_lfo_depth,
			   (gdouble) g_value_get_float(&value));

	g_object_unref(port);
      }

      /* wah-wah LFO freq */
      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-lfo-freq", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	ags_port_safe_read(port,
			   &value);

	ags_dial_set_value(sfz_synth->wah_wah_lfo_freq,
			   (gdouble) g_value_get_float(&value));

	g_object_unref(port);
      }

      /* wah-wah tuning */
      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-tuning", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	ags_port_safe_read(port,
			   &value);

	ags_dial_set_value(sfz_synth->wah_wah_tuning,
			   (gdouble) g_value_get_float(&value));

	g_object_unref(port);
      }
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /**/
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
  
  machine->flags &= (~AGS_MACHINE_NO_UPDATE);
}

/**
 * ags_sfz_synth_open_filename:
 * @sfz_synth: the #AgsSFZSynth
 * @filename: the filename
 * 
 * Open @filename.
 * 
 * Since: 3.17.0
 */
void
ags_sfz_synth_open_filename(AgsSFZSynth *sfz_synth,
			    gchar *filename)
{
  AgsSFZLoader *sfz_loader;

  if(!AGS_IS_SFZ_SYNTH(sfz_synth) ||
     filename == NULL ||
     strlen(filename) == 0 ||
     !g_file_test(filename, G_FILE_TEST_EXISTS)){
    return;
  }
  
  sfz_synth->sfz_loader = 
    sfz_loader = ags_sfz_loader_new(AGS_MACHINE(sfz_synth)->audio,
				    filename,
				    TRUE);

  ags_sfz_loader_start(sfz_loader);
}

/**
 * ags_sfz_synth_load_opcode:
 * @sfz_synth: the #AgsSFZSynth
 * 
 * Load opcodes of @sfz_synth.
 * 
 * Since: 3.17.0
 */
void
ags_sfz_synth_load_opcode(AgsSFZSynth *sfz_synth)
{
  GtkListStore *opcode_list_store;	

  GObject *sound_container;
  
  GList *start_group, *group;
  GList *start_region, *region;
  GList *start_sample, *sample;
  GList *start_control, *control;

  GtkTreeIter tree_iter;
  
  GRecMutex *audio_container_mutex;

  g_return_if_fail(AGS_IS_SFZ_SYNTH(sfz_synth));
  g_return_if_fail(sfz_synth->audio_container != NULL);
  g_return_if_fail(sfz_synth->audio_container->sound_container != NULL);

  audio_container_mutex = AGS_AUDIO_CONTAINER_GET_OBJ_MUTEX(sfz_synth->audio_container);

  g_rec_mutex_lock(audio_container_mutex);

  sound_container = sfz_synth->audio_container->sound_container;
  
  g_rec_mutex_unlock(audio_container_mutex);
  
  opcode_list_store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(sfz_synth->opcode_tree_view)));

  start_group = ags_sfz_file_get_group(AGS_SFZ_FILE(sound_container));
  start_region = ags_sfz_file_get_region(AGS_SFZ_FILE(sound_container));
  start_sample = ags_sfz_file_get_sample(AGS_SFZ_FILE(sound_container));

  group = start_group;

  while(group != NULL){
    GList *start_tmp_region, *tmp_region;

    if(AGS_SFZ_GROUP(group->data)->sample != NULL){
      start_sample = g_list_remove(start_sample,
				   AGS_SFZ_GROUP(group->data)->sample);
      
      g_object_unref(AGS_SFZ_GROUP(group->data)->sample);
    }
    
    control =
      start_control = ags_sfz_group_get_control(group->data);
      
    while(control != NULL){
      gtk_list_store_append(opcode_list_store,
			    &tree_iter);

      gtk_list_store_set(opcode_list_store, &tree_iter,
			 0, g_strdup(control->data),
			 1, ags_sfz_group_lookup_control(group->data, control->data),
			 -1);

      /* iterate */
      control = control->next;
    }

    g_list_free(start_control);

    tmp_region = 
      start_tmp_region = ags_sfz_group_get_region(group->data);

    while(tmp_region != NULL){
      if(AGS_SFZ_REGION(tmp_region->data)->sample != NULL){
	start_sample = g_list_remove(start_sample,
				     AGS_SFZ_REGION(tmp_region->data)->sample);
      
	g_object_unref(AGS_SFZ_REGION(tmp_region->data)->sample);
      }
      
      start_region = g_list_remove(start_region,
				   tmp_region->data);
      g_object_unref(tmp_region->data);
      
      control =
	start_control = ags_sfz_region_get_control(tmp_region->data);
      
      while(control != NULL){
	gtk_list_store_append(opcode_list_store,
			      &tree_iter);

	gtk_list_store_set(opcode_list_store, &tree_iter,
			   0, g_strdup(control->data),
			   1, ags_sfz_region_lookup_control(tmp_region->data, control->data),
			   -1);

	control = control->next;
      }

      g_list_free(start_control);

      /* iterate */
      tmp_region = tmp_region->next;
    }

    g_list_free_full(start_tmp_region,
		     (GDestroyNotify) g_object_unref);
    
    /* iterate */
    group = group->next;
  }

  g_list_free_full(start_group,
		   (GDestroyNotify) g_object_unref);

  region = start_region;
  
  while(region != NULL){
    if(AGS_SFZ_REGION(region->data)->sample != NULL){
      start_sample = g_list_remove(start_sample,
				   AGS_SFZ_REGION(region->data)->sample);
      
      g_object_unref(AGS_SFZ_REGION(region->data)->sample);
    }
      
    control =
      start_control = ags_sfz_region_get_control(region->data);
      
    while(control != NULL){
      gtk_list_store_append(opcode_list_store,
			    &tree_iter);

      gtk_list_store_set(opcode_list_store, &tree_iter,
			 0, g_strdup(control->data),
			 1, ags_sfz_region_lookup_control(region->data, control->data),
			 -1);

      control = control->next;
    }

    /* iterate */
    region = region->next;
  }

  g_list_free_full(start_region,
		   (GDestroyNotify) g_object_unref);

  sample = start_sample;
  
  while(sample != NULL){
    gtk_list_store_append(opcode_list_store,
			  &tree_iter);

    gtk_list_store_set(opcode_list_store, &tree_iter,
		       0, g_strdup("sample"),
		       1, g_strdup(AGS_SFZ_SAMPLE(sample->data)->filename),
		       -1);

    /* iterate */
    sample = sample->next;
  }

  g_list_free_full(start_sample,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_sfz_synth_sfz_loader_completed_timeout:
 * @sfz_synth: the #AgsSFZSynth
 *
 * Queue draw widget
 *
 * Returns: %TRUE if proceed poll completed, otherwise %FALSE
 *
 * Since: 3.4.0
 */
gboolean
ags_sfz_synth_sfz_loader_completed_timeout(AgsSFZSynth *sfz_synth)
{
  if(g_hash_table_lookup(ags_sfz_synth_sfz_loader_completed,
			 sfz_synth) != NULL){
    if(sfz_synth->sfz_loader != NULL){
      if(ags_sfz_loader_test_flags(sfz_synth->sfz_loader, AGS_SFZ_LOADER_HAS_COMPLETED)){
	GtkListStore *opcode_list_store;
	
	AgsFxSFZSynthAudio *fx_sfz_synth_audio;
	
	guint i;
	guint j;
	guint k;
	
	/* reassign audio container */
	sfz_synth->audio_container = sfz_synth->sfz_loader->audio_container;
	sfz_synth->sfz_loader->audio_container = NULL;

	opcode_list_store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(sfz_synth->opcode_tree_view)));

	gtk_list_store_clear(opcode_list_store);

	if(sfz_synth->audio_container == NULL ||
	   sfz_synth->audio_container->sound_container == NULL){
	  g_object_unref(sfz_synth->sfz_loader);

	  sfz_synth->sfz_loader = NULL;

	  sfz_synth->position = -1;

	  gtk_spinner_stop(sfz_synth->sfz_loader_spinner);
	  gtk_widget_hide((GtkWidget *) sfz_synth->sfz_loader_spinner);
	  
	  return(TRUE);
	}
	
	ags_sfz_synth_load_opcode(sfz_synth);
	
	fx_sfz_synth_audio = (AgsFxSFZSynthAudio *) ags_recall_container_get_recall_audio(sfz_synth->sfz_synth_recall_container);

	for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
	  AgsFxSFZSynthAudioScopeData *scope_data;
      
	  scope_data = fx_sfz_synth_audio->scope_data[i];

	  if(scope_data != NULL){
	    for(j = 0; j < scope_data->audio_channels; j++){
	      AgsFxSFZSynthAudioChannelData *channel_data;

	      channel_data = scope_data->channel_data[j];
	  
	      if(channel_data != NULL){
		AgsSFZInstrumentLoader *sfz_instrument_loader;
	  
		sfz_instrument_loader =
		  sfz_synth->sfz_instrument_loader = ags_sfz_instrument_loader_new(AGS_MACHINE(sfz_synth)->audio,
										   sfz_synth->audio_container->filename);
	    
		ags_sfz_instrument_loader_set_flags(sfz_instrument_loader,
						    AGS_SFZ_INSTRUMENT_LOADER_RUN_APPLY_INSTRUMENT);
		
		sfz_instrument_loader->synth = channel_data->synth;
      
		ags_sfz_instrument_loader_start(sfz_instrument_loader);
	      }
	    }
	  }
	}
    
	if(fx_sfz_synth_audio != NULL){
	  g_object_unref(fx_sfz_synth_audio);
	}
	
	/* cleanup */	
	g_object_unref(sfz_synth->sfz_loader);

	sfz_synth->sfz_loader = NULL;

	sfz_synth->position = -1;

	gtk_spinner_stop(sfz_synth->sfz_loader_spinner);
	gtk_widget_hide((GtkWidget *) sfz_synth->sfz_loader_spinner);
      }else{
	if(sfz_synth->position == -1){
	  sfz_synth->position = 0;

	  gtk_widget_show((GtkWidget *) sfz_synth->sfz_loader_spinner);
	  gtk_spinner_start(sfz_synth->sfz_loader_spinner);
	}
      }
    }
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_sfz_synth_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsSFZSynth
 *
 * Returns: the new #AgsSFZSynth
 *
 * Since: 3.4.0
 */
AgsSFZSynth*
ags_sfz_synth_new(GObject *soundcard)
{
  AgsSFZSynth *sfz_synth;

  sfz_synth = (AgsSFZSynth *) g_object_new(AGS_TYPE_SFZ_SYNTH,
					   NULL);

  g_object_set(AGS_MACHINE(sfz_synth)->audio,
	       "output-soundcard", soundcard,
	       NULL);

  return(sfz_synth);
}
