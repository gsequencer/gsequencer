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

#include <ags/app/machine/ags_sf2_synth.h>
#include <ags/app/machine/ags_sf2_synth_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>

#include <ags/ags_api_config.h>

#ifdef AGS_WITH_LIBINSTPATCH
#include <libinstpatch/libinstpatch.h>
#endif

#include <ags/i18n.h>

void ags_sf2_synth_class_init(AgsSF2SynthClass *sf2_synth);
void ags_sf2_synth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sf2_synth_init(AgsSF2Synth *sf2_synth);
void ags_sf2_synth_finalize(GObject *gobject);

void ags_sf2_synth_connect(AgsConnectable *connectable);
void ags_sf2_synth_disconnect(AgsConnectable *connectable);

void ags_sf2_synth_show(GtkWidget *widget);

void ags_sf2_synth_wah_wah_draw_function(GtkDrawingArea *area,
					 cairo_t *cr,
					 int width,
					 int height,
					 AgsSF2Synth *sf2_synth);

void ags_sf2_synth_resize_audio_channels(AgsMachine *machine,
					 guint audio_channels, guint audio_channels_old,
					 gpointer data);
void ags_sf2_synth_resize_pads(AgsMachine *machine, GType channel_type,
			       guint pads, guint pads_old,
			       gpointer data);

void ags_sf2_synth_map_recall(AgsMachine *machine);

void ags_sf2_synth_input_map_recall(AgsSF2Synth *sf2_synth,
				    guint audio_channel_start,
				    guint input_pad_start);
void ags_sf2_synth_output_map_recall(AgsSF2Synth *sf2_synth,
				     guint audio_channel_start,
				     guint output_pad_start);

gint ags_sf2_synth_int_compare_func(gconstpointer a,
				    gconstpointer b);

void ags_sf2_synth_refresh_port(AgsMachine *machine);

/**
 * SECTION:ags_sf2_synth
 * @short_description: SF2 synth
 * @title: AgsSF2Synth
 * @section_id:
 * @include: ags/app/machine/ags_sf2_synth.h
 *
 * The #AgsSF2Synth is a composite widget to act as SF2 synth.
 */

static gpointer ags_sf2_synth_parent_class = NULL;
static AgsConnectableInterface *ags_sf2_synth_parent_connectable_interface;

GHashTable *ags_sf2_synth_sf2_loader_completed = NULL;

GType
ags_sf2_synth_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_sf2_synth = 0;

    static const GTypeInfo ags_sf2_synth_info = {
      sizeof(AgsSF2SynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sf2_synth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSF2Synth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sf2_synth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_sf2_synth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_sf2_synth = g_type_register_static(AGS_TYPE_MACHINE,
						"AgsSF2Synth", &ags_sf2_synth_info,
						0);
    
    g_type_add_interface_static(ags_type_sf2_synth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_sf2_synth);
  }

  return(g_define_type_id__static);
}

void
ags_sf2_synth_class_init(AgsSF2SynthClass *sf2_synth)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_sf2_synth_parent_class = g_type_class_peek_parent(sf2_synth);

  /* GObjectClass */
  gobject = (GObjectClass *) sf2_synth;

  gobject->finalize = ags_sf2_synth_finalize;

  /* AgsMachineClass */
  machine = (AgsMachineClass *) sf2_synth;

  machine->map_recall = ags_sf2_synth_map_recall;

  machine->refresh_port = ags_sf2_synth_refresh_port;
}

void
ags_sf2_synth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_sf2_synth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_sf2_synth_connect;
  connectable->disconnect = ags_sf2_synth_disconnect;
}

void
ags_sf2_synth_init(AgsSF2Synth *sf2_synth)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  GtkBox *vbox;
  GtkBox *sf2_hbox;
  GtkBox *sf2_file_hbox;
  GtkBox *sf2_synth_pitch_type_hbox;
  GtkBox *sf2_preset_hbox;
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
  GtkTreeView *sf2_bank_tree_view;
  GtkTreeView *sf2_program_tree_view;
  GtkTreeViewColumn *sf2_bank_column;
  GtkTreeViewColumn *sf2_program_column;
  GtkTreeViewColumn *sf2_preset_column;
  GtkScrolledWindow *scrolled_window;
  GtkLabel *label;
    
  GtkCellRenderer *sf2_bank_renderer;
  GtkCellRenderer *sf2_program_renderer;
  GtkCellRenderer *sf2_preset_renderer;
  
  GtkListStore *sf2_bank;
  GtkListStore *sf2_program;

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
								     AGS_TYPE_SF2_SYNTH);

  machine_name = NULL;

  if(machine_counter != NULL){
    machine_name = g_strdup_printf("Default %d",
				   machine_counter->counter);
  
    ags_machine_counter_increment(machine_counter);
  }
  
  g_object_set(sf2_synth,
	       "machine-name", machine_name,
	       NULL);

  g_free(machine_name);
  
  /* machine selector */
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  position = g_list_length(window->machine);
  
  ags_machine_selector_popup_insert_machine(composite_editor->machine_selector,
					    position,
					    (AgsMachine *) sf2_synth);
    
  audio = AGS_MACHINE(sf2_synth)->audio;

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
  
  AGS_MACHINE(sf2_synth)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
				    AGS_MACHINE_REVERSE_NOTATION);
  AGS_MACHINE(sf2_synth)->file_input_flags |= AGS_MACHINE_ACCEPT_SOUNDFONT2;

  /* audio resize */
  g_signal_connect_after(G_OBJECT(sf2_synth), "resize-audio-channels",
			 G_CALLBACK(ags_sf2_synth_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(sf2_synth), "resize-pads",
			 G_CALLBACK(ags_sf2_synth_resize_pads), NULL);

  /* flags */
  sf2_synth->flags = 0;

  /* mapped IO */
  sf2_synth->mapped_output_audio_channel = 0;
  sf2_synth->mapped_input_audio_channel = 0;

  sf2_synth->mapped_input_pad = 0;
  sf2_synth->mapped_output_pad = 0;

  sf2_synth->playback_play_container = ags_recall_container_new();
  sf2_synth->playback_recall_container = ags_recall_container_new();

  sf2_synth->sf2_synth_play_container = ags_recall_container_new();
  sf2_synth->sf2_synth_recall_container = ags_recall_container_new();

  sf2_synth->tremolo_play_container = ags_recall_container_new();
  sf2_synth->tremolo_recall_container = ags_recall_container_new();

  sf2_synth->envelope_play_container = ags_recall_container_new();
  sf2_synth->envelope_recall_container = ags_recall_container_new();

  sf2_synth->wah_wah_play_container = ags_recall_container_new();
  sf2_synth->wah_wah_recall_container = ags_recall_container_new();

  sf2_synth->buffer_play_container = ags_recall_container_new();
  sf2_synth->buffer_recall_container = ags_recall_container_new();
  
  /* name and xml type */
  sf2_synth->name = NULL;
  sf2_synth->xml_type = "ags-sf2-synth";

  /* audio container */
  sf2_synth->audio_container = NULL;

  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_frame_set_child(AGS_MACHINE(sf2_synth)->frame,
		      (GtkWidget *) vbox);

  /* SF2 */
  sf2_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				    AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_valign((GtkWidget *) sf2_hbox,
			GTK_ALIGN_START);  
  gtk_widget_set_halign((GtkWidget *) sf2_hbox,
			GTK_ALIGN_START);

  gtk_widget_set_hexpand((GtkWidget *) sf2_hbox,
			 FALSE);

  gtk_box_append(vbox,
		 (GtkWidget *) sf2_hbox);

  /* file */
  sf2_file_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					 AGS_UI_PROVIDER_DEFAULT_SPACING);  

  gtk_box_set_spacing(sf2_file_hbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(sf2_hbox,
		 (GtkWidget *) sf2_file_hbox);

  sf2_synth->filename = (GtkEntry *) gtk_entry_new();

  gtk_widget_set_valign((GtkWidget *) sf2_synth->filename,
			GTK_ALIGN_START);
  
  gtk_box_append(sf2_file_hbox,
		 (GtkWidget *) sf2_synth->filename);
  
  sf2_synth->open = (GtkButton *) gtk_button_new_with_mnemonic(i18n("_Open"));

  gtk_widget_set_valign((GtkWidget *) sf2_synth->open,
			GTK_ALIGN_START);

  gtk_box_append(sf2_file_hbox,
		 (GtkWidget *) sf2_synth->open);

  sf2_synth->sf2_loader = NULL;

  sf2_synth->load_bank = -1;
  sf2_synth->load_program = -1;
  
  sf2_synth->position = -1;

  sf2_synth->sf2_loader_spinner = (GtkSpinner *) gtk_spinner_new();
  gtk_box_append(sf2_file_hbox,
		     (GtkWidget *) sf2_synth->sf2_loader_spinner);
  gtk_widget_set_visible((GtkWidget *) sf2_synth->sf2_loader_spinner,
			 FALSE);
  
  /* preset - bank and program */
  sf2_synth->bank = -1;
  sf2_synth->program = -1;
  
  sf2_preset_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
					   AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(sf2_hbox,
		 (GtkWidget *) sf2_preset_hbox);

  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();
  gtk_widget_set_size_request((GtkWidget *) scrolled_window,
			      AGS_SF2_SYNTH_BANK_WIDTH_REQUEST,
			      AGS_SF2_SYNTH_BANK_HEIGHT_REQUEST);
  gtk_scrolled_window_set_policy(scrolled_window,
				 GTK_POLICY_AUTOMATIC,
				 GTK_POLICY_ALWAYS);
  gtk_box_append(sf2_preset_hbox,
		 (GtkWidget *) scrolled_window);
  
  sf2_bank_tree_view =
    sf2_synth->bank_tree_view = (GtkTreeView *) gtk_tree_view_new();
  gtk_tree_view_set_activate_on_single_click(sf2_bank_tree_view,
					     TRUE);
  gtk_scrolled_window_set_child(scrolled_window,
				(GtkWidget *) sf2_bank_tree_view);
    
  gtk_widget_set_size_request((GtkWidget *) sf2_bank_tree_view,
			      AGS_SF2_SYNTH_BANK_WIDTH_REQUEST,
			      AGS_SF2_SYNTH_BANK_HEIGHT_REQUEST);

  sf2_bank_renderer = gtk_cell_renderer_text_new();

  sf2_bank_column = gtk_tree_view_column_new_with_attributes(i18n("bank"),
							     sf2_bank_renderer,
							     "text", 0,
							     NULL);
  gtk_tree_view_append_column(sf2_bank_tree_view,
			      sf2_bank_column);
  
  sf2_bank = gtk_list_store_new(1,
				G_TYPE_INT);

  gtk_tree_view_set_model(sf2_bank_tree_view,
			  GTK_TREE_MODEL(sf2_bank));  
  
  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();
  gtk_widget_set_size_request((GtkWidget *) scrolled_window,
			      AGS_SF2_SYNTH_PROGRAM_WIDTH_REQUEST,
			      AGS_SF2_SYNTH_PROGRAM_HEIGHT_REQUEST);
  gtk_scrolled_window_set_policy(scrolled_window,
				 GTK_POLICY_AUTOMATIC,
				 GTK_POLICY_ALWAYS);
  gtk_box_append((GtkBox *) sf2_preset_hbox,
		 (GtkWidget *) scrolled_window);

  sf2_synth->program_tree_view = 
    sf2_program_tree_view = (GtkTreeView *) gtk_tree_view_new();
  gtk_tree_view_set_activate_on_single_click(sf2_program_tree_view,
					     TRUE);
  gtk_scrolled_window_set_child(scrolled_window,
				(GtkWidget *) sf2_program_tree_view);

  gtk_widget_set_size_request((GtkWidget *) sf2_program_tree_view,
			      AGS_SF2_SYNTH_PROGRAM_WIDTH_REQUEST,
			      AGS_SF2_SYNTH_PROGRAM_HEIGHT_REQUEST);

  sf2_program_renderer = gtk_cell_renderer_text_new();
  sf2_preset_renderer = gtk_cell_renderer_text_new();
  
  sf2_program_column = gtk_tree_view_column_new_with_attributes(i18n("program"),
								sf2_program_renderer,
								"text", 1,
								NULL);
  gtk_tree_view_append_column(sf2_program_tree_view,
			      sf2_program_column);

  sf2_preset_column = gtk_tree_view_column_new_with_attributes(i18n("preset"),
							       sf2_preset_renderer,
								"text", 2,
							       NULL);
  gtk_tree_view_append_column(sf2_program_tree_view,
			      sf2_preset_column);
  
  sf2_program = gtk_list_store_new(3,
				   G_TYPE_INT,
				   G_TYPE_INT,
				   G_TYPE_STRING);

  gtk_tree_view_set_model(sf2_program_tree_view,
			  GTK_TREE_MODEL(sf2_program));

  /* effect control */
  effect_vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				       AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(sf2_hbox,
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
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) label,
		  0, 0,
		  1, 1);

  sf2_synth->synth_octave = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->synth_octave);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   0.0);

//  sf2_synth->synth_octave->scale_max_precision = 12;
  
//  ags_dial_set_scale_precision(sf2_synth->synth_octave,
//			       12);
  ags_dial_set_radius(sf2_synth->synth_octave,
		      12);
  
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) sf2_synth->synth_octave,
		  1, 0,
		  1, 1);

  /* WAV 1 - key */
  label = (GtkLabel *) gtk_label_new(i18n("WAV 1 - key"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  sf2_synth->synth_key = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->synth_key);

  gtk_adjustment_set_lower(adjustment,
			   -12.0);
  gtk_adjustment_set_upper(adjustment,
			   12.0);

  gtk_adjustment_set_step_increment(adjustment,
				    1.0);

  gtk_adjustment_set_value(adjustment,
			   2.0);

//  sf2_synth->synth_key->scale_max_precision = 12;
//  ags_dial_set_scale_precision(sf2_synth->synth_key,
//			       12);
  ags_dial_set_radius(sf2_synth->synth_key,
		      12);
  
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) sf2_synth->synth_key,
		  1, 1,
		  1, 1);

  /* WAV 1 - volume */
  label = (GtkLabel *) gtk_label_new(i18n("WAV 1 - volume"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) label,
		  2, 0,
		  1, 1);

  sf2_synth->synth_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->synth_volume);

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

  ags_dial_set_radius(sf2_synth->synth_volume,
		      12);
  
  gtk_grid_attach(synth_grid,
		  (GtkWidget *) sf2_synth->synth_volume,
		  3, 0,
		  1, 1);

  /* pitch type */
  sf2_synth_pitch_type_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				    AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_valign((GtkWidget *) sf2_synth_pitch_type_hbox,
			GTK_ALIGN_START);  
  gtk_widget_set_halign((GtkWidget *) sf2_synth_pitch_type_hbox,
			GTK_ALIGN_START);

  gtk_widget_set_hexpand((GtkWidget *) sf2_synth_pitch_type_hbox,
			 FALSE);

  gtk_grid_attach(synth_grid,
		  (GtkWidget *) sf2_synth_pitch_type_hbox,
		  0, 2,
		  2, 1);
  
  label = (GtkLabel *) gtk_label_new(i18n("pitch type"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_box_append(sf2_synth_pitch_type_hbox,
		 (GtkWidget *) label);

  sf2_synth->synth_pitch_type = (GtkDropDown *) gtk_drop_down_new_from_strings((const gchar * const *) pitch_type_strv);

  gtk_drop_down_set_selected(sf2_synth->synth_pitch_type,
			     2);

  gtk_box_append(sf2_synth_pitch_type_hbox,
		 (GtkWidget *) sf2_synth->synth_pitch_type);

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

  sf2_synth->chorus_input_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->chorus_input_volume);

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

  ags_dial_set_radius(sf2_synth->chorus_input_volume,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sf2_synth->chorus_input_volume,
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

  sf2_synth->chorus_output_volume = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->chorus_output_volume);

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

  ags_dial_set_radius(sf2_synth->chorus_output_volume,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sf2_synth->chorus_output_volume,
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
  
  sf2_synth->chorus_lfo_oscillator = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text((GtkComboBoxText *) sf2_synth->chorus_lfo_oscillator,
				 "sine");
  gtk_combo_box_text_append_text((GtkComboBoxText *) sf2_synth->chorus_lfo_oscillator,
				 "sawtooth");
  gtk_combo_box_text_append_text((GtkComboBoxText *) sf2_synth->chorus_lfo_oscillator,
				 "triangle");
  gtk_combo_box_text_append_text((GtkComboBoxText *) sf2_synth->chorus_lfo_oscillator,
				 "square");
  gtk_combo_box_text_append_text((GtkComboBoxText *) sf2_synth->chorus_lfo_oscillator,
				 "impulse");

  gtk_combo_box_set_active(sf2_synth->chorus_lfo_oscillator,
			   0);

  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sf2_synth->chorus_lfo_oscillator,
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

  sf2_synth->chorus_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.01, 10.0, 0.01);
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sf2_synth->chorus_lfo_frequency,
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

  sf2_synth->chorus_depth = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->chorus_depth);

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

  ags_dial_set_radius(sf2_synth->chorus_depth,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sf2_synth->chorus_depth,
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

  sf2_synth->chorus_mix = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->chorus_mix);

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

  ags_dial_set_radius(sf2_synth->chorus_mix,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sf2_synth->chorus_mix,
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

  sf2_synth->chorus_delay = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->chorus_delay);

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

  ags_dial_set_radius(sf2_synth->chorus_delay,
		      12);
  
  gtk_grid_attach(chorus_grid,
		  (GtkWidget *) sf2_synth->chorus_delay,
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

  sf2_synth->tremolo_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) sf2_synth->tremolo_enabled,
		  0, 0,
		  2, 1);

  label = (GtkLabel *) gtk_label_new(i18n("gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  sf2_synth->tremolo_gain = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(sf2_synth->tremolo_gain,
		      12);

  adjustment = ags_dial_get_adjustment(sf2_synth->tremolo_gain);

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
		  (GtkWidget *) sf2_synth->tremolo_gain,
		  1, 1,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("lfo-depth"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);

  sf2_synth->tremolo_lfo_depth = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(sf2_synth->tremolo_lfo_depth,
		      12);

  adjustment = ags_dial_get_adjustment(sf2_synth->tremolo_lfo_depth);

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
		  (GtkWidget *) sf2_synth->tremolo_lfo_depth,
		  1, 2,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("lfo-freq"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);

  sf2_synth->tremolo_lfo_freq = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(sf2_synth->tremolo_lfo_freq,
		      12);

  adjustment = ags_dial_get_adjustment(sf2_synth->tremolo_lfo_freq);

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
		  (GtkWidget *) sf2_synth->tremolo_lfo_freq,
		  1, 3,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(tremolo_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);

  sf2_synth->tremolo_tuning = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(sf2_synth->tremolo_tuning,
		      12);

  adjustment = ags_dial_get_adjustment(sf2_synth->tremolo_tuning);

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
		  (GtkWidget *) sf2_synth->tremolo_tuning,
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

  sf2_synth->vibrato_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) sf2_synth->vibrato_enabled,
		  0, 0,
		  2, 1);

  label = (GtkLabel *) gtk_label_new(i18n("gain"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  sf2_synth->vibrato_gain = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(sf2_synth->vibrato_gain,
		      12);

  adjustment = ags_dial_get_adjustment(sf2_synth->vibrato_gain);

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
		  (GtkWidget *) sf2_synth->vibrato_gain,
		  1, 1,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("lfo-depth"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);

  sf2_synth->vibrato_lfo_depth = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(sf2_synth->vibrato_lfo_depth,
		      12);

  adjustment = ags_dial_get_adjustment(sf2_synth->vibrato_lfo_depth);

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
		  (GtkWidget *) sf2_synth->vibrato_lfo_depth,
		  1, 2,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("lfo-freq"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);

  sf2_synth->vibrato_lfo_freq = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(sf2_synth->vibrato_lfo_freq,
		      12);
  
  adjustment = ags_dial_get_adjustment(sf2_synth->vibrato_lfo_freq);

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
		  (GtkWidget *) sf2_synth->vibrato_lfo_freq,
		  1, 3,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(vibrato_grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);

  sf2_synth->vibrato_tuning = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(sf2_synth->vibrato_tuning,
		      12);

  adjustment = ags_dial_get_adjustment(sf2_synth->vibrato_tuning);

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
		  (GtkWidget *) sf2_synth->vibrato_tuning,
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
  
  sf2_synth->wah_wah_enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));
  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sf2_synth->wah_wah_enabled,
		  0, 0,
		  2, 1);

  label = (GtkLabel *) gtk_label_new(i18n("length"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) label,
		  0, 1,
		  1, 1);

  sf2_synth->wah_wah_length = (GtkComboBox *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text((GtkComboBoxText *) sf2_synth->wah_wah_length,
				 "1/1");
  gtk_combo_box_text_append_text((GtkComboBoxText *) sf2_synth->wah_wah_length,
				 "2/2");
  gtk_combo_box_text_append_text((GtkComboBoxText *) sf2_synth->wah_wah_length,
				 "4/4");
  gtk_combo_box_text_append_text((GtkComboBoxText *) sf2_synth->wah_wah_length,
				 "8/8");
  gtk_combo_box_text_append_text((GtkComboBoxText *) sf2_synth->wah_wah_length,
				 "16/16");

  gtk_combo_box_set_active(sf2_synth->wah_wah_length,
			   2);

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sf2_synth->wah_wah_length,
		  1, 1,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("attack [x|y]"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) label,
		  2, 1,
		  1, 1);

  sf2_synth->wah_wah_attack_x = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->wah_wah_attack_x);

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

  ags_dial_set_radius(sf2_synth->wah_wah_attack_x,
		      12);

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sf2_synth->wah_wah_attack_x,
		  3, 1,
		  1, 1);

  sf2_synth->wah_wah_attack_y = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->wah_wah_attack_y);

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

  ags_dial_set_radius(sf2_synth->wah_wah_attack_y,
		      12);

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sf2_synth->wah_wah_attack_y,
		  4, 1,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("decay [x|y]"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) label,
		  2, 2,
		  1, 1);

  sf2_synth->wah_wah_decay_x = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->wah_wah_decay_x);

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

  ags_dial_set_radius(sf2_synth->wah_wah_decay_x,
		      12);

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sf2_synth->wah_wah_decay_x,
		  3, 2,
		  1, 1);

  sf2_synth->wah_wah_decay_y = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->wah_wah_decay_y);

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

  ags_dial_set_radius(sf2_synth->wah_wah_decay_y,
		      12);

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sf2_synth->wah_wah_decay_y,
		  4, 2,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("sustain [x|y]"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) label,
		  2, 3,
		  1, 1);

  sf2_synth->wah_wah_sustain_x = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->wah_wah_sustain_x);

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

  ags_dial_set_radius(sf2_synth->wah_wah_sustain_x,
		      12);

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sf2_synth->wah_wah_sustain_x,
		  3, 3,
		  1, 1);

  sf2_synth->wah_wah_sustain_y = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->wah_wah_sustain_y);

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

  ags_dial_set_radius(sf2_synth->wah_wah_sustain_y,
		      12);

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sf2_synth->wah_wah_sustain_y,
		  4, 3,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("release [x|y]"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) label,
		  2, 4,
		  1, 1);

  sf2_synth->wah_wah_release_x = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->wah_wah_release_x);

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

  ags_dial_set_radius(sf2_synth->wah_wah_release_x,
		      12);

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sf2_synth->wah_wah_release_x,
		  3, 4,
		  1, 1);

  sf2_synth->wah_wah_release_y = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->wah_wah_release_y);

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
  
  ags_dial_set_radius(sf2_synth->wah_wah_release_y,
		      12);

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sf2_synth->wah_wah_release_y,
		  4, 4,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("ratio"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) label,
		  2, 5,
		  1, 1);

  sf2_synth->wah_wah_ratio = (AgsDial *) ags_dial_new();

  adjustment = ags_dial_get_adjustment(sf2_synth->wah_wah_ratio);

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

  ags_dial_set_radius(sf2_synth->wah_wah_ratio,
		      12);

  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sf2_synth->wah_wah_ratio,
		  3, 5,
		  1, 1);
  
  label = (GtkLabel *) gtk_label_new(i18n("lfo-depth"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) label,
		  5, 1,
		  1, 1);

  sf2_synth->wah_wah_lfo_depth = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(sf2_synth->wah_wah_lfo_depth,
		      12);

  adjustment = ags_dial_get_adjustment(sf2_synth->wah_wah_lfo_depth);

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
		  (GtkWidget *) sf2_synth->wah_wah_lfo_depth,
		  6, 1,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("lfo-freq"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) label,
		  5, 2,
		  1, 1);

  sf2_synth->wah_wah_lfo_freq = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(sf2_synth->wah_wah_lfo_freq,
		      12);

  adjustment = ags_dial_get_adjustment(sf2_synth->wah_wah_lfo_freq);

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
		  (GtkWidget *) sf2_synth->wah_wah_lfo_freq,
		  6, 2,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("tuning"));
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) label,
		  5, 3,
		  1, 1);

  sf2_synth->wah_wah_tuning = (AgsDial *) ags_dial_new();

  ags_dial_set_radius(sf2_synth->wah_wah_tuning,
		      12);

  adjustment = ags_dial_get_adjustment(sf2_synth->wah_wah_tuning);

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
		  (GtkWidget *) sf2_synth->wah_wah_tuning,
		  6, 3,
		  1, 1);

  sf2_synth->wah_wah_drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();

  gtk_drawing_area_set_content_width(sf2_synth->wah_wah_drawing_area,
				     480);
  gtk_drawing_area_set_content_height(sf2_synth->wah_wah_drawing_area,
				      120);
  
  gtk_grid_attach(wah_wah_grid,
		  (GtkWidget *) sf2_synth->wah_wah_drawing_area,
		  7, 1,
		  1, 4);

  gtk_drawing_area_set_draw_func(sf2_synth->wah_wah_drawing_area,
				 (GtkDrawingAreaDrawFunc) ags_sf2_synth_wah_wah_draw_function,
				 sf2_synth,
				 NULL);

  /* dialog */
  sf2_synth->open_dialog = NULL;

  /* SF2 loader */
  if(ags_sf2_synth_sf2_loader_completed == NULL){
    ags_sf2_synth_sf2_loader_completed = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							       NULL,
							       NULL);
  }

  g_hash_table_insert(ags_sf2_synth_sf2_loader_completed,
		      sf2_synth, ags_sf2_synth_sf2_loader_completed_timeout);
  g_timeout_add(1000 / 4, (GSourceFunc) ags_sf2_synth_sf2_loader_completed_timeout, (gpointer) sf2_synth);
}

void
ags_sf2_synth_finalize(GObject *gobject)
{
  AgsSF2Synth *sf2_synth;

  sf2_synth = AGS_SF2_SYNTH(gobject);

  g_hash_table_remove(ags_sf2_synth_sf2_loader_completed,
		      gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_sf2_synth_parent_class)->finalize(gobject);
}

void
ags_sf2_synth_connect(AgsConnectable *connectable)
{
  AgsSF2Synth *sf2_synth;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  ags_sf2_synth_parent_connectable_interface->connect(connectable);

  /* AgsSF2Synth */
  sf2_synth = AGS_SF2_SYNTH(connectable);

  g_signal_connect((GObject *) sf2_synth, "destroy",
		   G_CALLBACK(ags_sf2_synth_destroy_callback), (gpointer) sf2_synth);  

  g_signal_connect((GObject *) sf2_synth->open, "clicked",
		   G_CALLBACK(ags_sf2_synth_open_clicked_callback), (gpointer) sf2_synth);

  g_signal_connect((GObject *) sf2_synth->synth_pitch_type, "notify::selected",
		   G_CALLBACK(ags_sf2_synth_synth_pitch_type_callback), (gpointer) sf2_synth);

  g_signal_connect((GObject *) sf2_synth->bank_tree_view, "row-activated",
		   G_CALLBACK(ags_sf2_synth_bank_tree_view_callback), (gpointer) sf2_synth);

  g_signal_connect((GObject *) sf2_synth->program_tree_view, "row-activated",
		   G_CALLBACK(ags_sf2_synth_program_tree_view_callback), (gpointer) sf2_synth);

  g_signal_connect_after(sf2_synth->synth_octave, "value-changed",
			 G_CALLBACK(ags_sf2_synth_synth_octave_callback), sf2_synth);
  
  g_signal_connect_after(sf2_synth->synth_key, "value-changed",
			 G_CALLBACK(ags_sf2_synth_synth_key_callback), sf2_synth);
  
  g_signal_connect_after(sf2_synth->synth_volume, "value-changed",
			 G_CALLBACK(ags_sf2_synth_synth_volume_callback), sf2_synth);

  //  g_signal_connect_after(sf2_synth->chorus_enabled, "toggled",
//			 G_CALLBACK(ags_sf2_synth_chorus_enabled_callback), sf2_synth);
  
  g_signal_connect_after(sf2_synth->chorus_input_volume, "value-changed",
			 G_CALLBACK(ags_sf2_synth_chorus_input_volume_callback), sf2_synth);
  
  g_signal_connect_after(sf2_synth->chorus_output_volume, "value-changed",
			 G_CALLBACK(ags_sf2_synth_chorus_output_volume_callback), sf2_synth);
  
  g_signal_connect_after(sf2_synth->chorus_lfo_oscillator, "changed",
			 G_CALLBACK(ags_sf2_synth_chorus_lfo_oscillator_callback), sf2_synth);
  
  g_signal_connect_after(sf2_synth->chorus_lfo_frequency, "value-changed",
			 G_CALLBACK(ags_sf2_synth_chorus_lfo_frequency_callback), sf2_synth);
  
  g_signal_connect_after(sf2_synth->chorus_depth, "value-changed",
			 G_CALLBACK(ags_sf2_synth_chorus_depth_callback), sf2_synth);
  
  g_signal_connect_after(sf2_synth->chorus_mix, "value-changed",
			 G_CALLBACK(ags_sf2_synth_chorus_mix_callback), sf2_synth);
  
  g_signal_connect_after(sf2_synth->chorus_delay, "value-changed",
			 G_CALLBACK(ags_sf2_synth_chorus_delay_callback), sf2_synth);

  g_signal_connect_after(sf2_synth->tremolo_enabled, "toggled",
			 G_CALLBACK(ags_sf2_synth_tremolo_enabled_callback), sf2_synth);

  g_signal_connect_after(sf2_synth->tremolo_gain, "value-changed",
			 G_CALLBACK(ags_sf2_synth_tremolo_gain_callback), sf2_synth);
  
  g_signal_connect_after(sf2_synth->tremolo_lfo_depth, "value-changed",
			 G_CALLBACK(ags_sf2_synth_tremolo_lfo_depth_callback), sf2_synth);

  g_signal_connect_after(sf2_synth->tremolo_lfo_freq, "value-changed",
			 G_CALLBACK(ags_sf2_synth_tremolo_lfo_freq_callback), sf2_synth);

  g_signal_connect_after(sf2_synth->tremolo_tuning, "value-changed",
			 G_CALLBACK(ags_sf2_synth_tremolo_tuning_callback), sf2_synth);

  g_signal_connect_after(sf2_synth->vibrato_enabled, "toggled",
			 G_CALLBACK(ags_sf2_synth_vibrato_enabled_callback), sf2_synth);

  g_signal_connect_after(sf2_synth->vibrato_gain, "value-changed",
			 G_CALLBACK(ags_sf2_synth_vibrato_gain_callback), sf2_synth);

  g_signal_connect_after(sf2_synth->vibrato_lfo_depth, "value-changed",
			 G_CALLBACK(ags_sf2_synth_vibrato_lfo_depth_callback), sf2_synth);

  g_signal_connect_after(sf2_synth->vibrato_lfo_freq, "value-changed",
			 G_CALLBACK(ags_sf2_synth_vibrato_lfo_freq_callback), sf2_synth);

  g_signal_connect_after(sf2_synth->vibrato_tuning, "value-changed",
			 G_CALLBACK(ags_sf2_synth_vibrato_tuning_callback), sf2_synth);

  g_signal_connect_after(sf2_synth->wah_wah_length, "changed",
			 G_CALLBACK(ags_sf2_synth_wah_wah_length_callback), sf2_synth);

  g_signal_connect_after(sf2_synth->wah_wah_enabled, "toggled",
			 G_CALLBACK(ags_sf2_synth_wah_wah_enabled_callback), sf2_synth);

  g_signal_connect_after(sf2_synth->wah_wah_attack_x, "value-changed",
			 G_CALLBACK(ags_sf2_synth_wah_wah_attack_callback), sf2_synth);

  g_signal_connect_after(sf2_synth->wah_wah_attack_y, "value-changed",
			 G_CALLBACK(ags_sf2_synth_wah_wah_attack_callback), sf2_synth);
  
  g_signal_connect_after(sf2_synth->wah_wah_decay_x, "value-changed",
			 G_CALLBACK(ags_sf2_synth_wah_wah_decay_callback), sf2_synth);

  g_signal_connect_after(sf2_synth->wah_wah_decay_y, "value-changed",
			 G_CALLBACK(ags_sf2_synth_wah_wah_decay_callback), sf2_synth);
  
  g_signal_connect_after(sf2_synth->wah_wah_sustain_x, "value-changed",
			 G_CALLBACK(ags_sf2_synth_wah_wah_sustain_callback), sf2_synth);

  g_signal_connect_after(sf2_synth->wah_wah_sustain_y, "value-changed",
			 G_CALLBACK(ags_sf2_synth_wah_wah_sustain_callback), sf2_synth);

  g_signal_connect_after(sf2_synth->wah_wah_release_x, "value-changed",
			 G_CALLBACK(ags_sf2_synth_wah_wah_release_callback), sf2_synth);

  g_signal_connect_after(sf2_synth->wah_wah_release_y, "value-changed",
			 G_CALLBACK(ags_sf2_synth_wah_wah_release_callback), sf2_synth);

  g_signal_connect_after(sf2_synth->wah_wah_ratio, "value-changed",
			 G_CALLBACK(ags_sf2_synth_wah_wah_ratio_callback), sf2_synth);

  g_signal_connect_after(sf2_synth->wah_wah_lfo_depth, "value-changed",
			 G_CALLBACK(ags_sf2_synth_wah_wah_lfo_depth_callback), sf2_synth);

  g_signal_connect_after(sf2_synth->wah_wah_lfo_freq, "value-changed",
			 G_CALLBACK(ags_sf2_synth_wah_wah_lfo_freq_callback), sf2_synth);

  g_signal_connect_after(sf2_synth->wah_wah_tuning, "value-changed",
			 G_CALLBACK(ags_sf2_synth_wah_wah_tuning_callback), sf2_synth);
}

void
ags_sf2_synth_disconnect(AgsConnectable *connectable)
{
  AgsSF2Synth *sf2_synth;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  ags_sf2_synth_parent_connectable_interface->disconnect(connectable);

  /* AgsSF2Synth */
  sf2_synth = AGS_SF2_SYNTH(connectable);

  g_object_disconnect((GObject *) sf2_synth,
		      "any_signal::destroy",
		      G_CALLBACK(ags_sf2_synth_destroy_callback),
		      (gpointer) sf2_synth,
		      NULL);  

  g_object_disconnect((GObject *) sf2_synth->open,
		      "any_signal::clicked",
		      G_CALLBACK(ags_sf2_synth_open_clicked_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->synth_pitch_type,
		      "any_signal::notify::selected",
		      G_CALLBACK(ags_sf2_synth_synth_pitch_type_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->program_tree_view,
		      "any_signal::row-activated",
		      G_CALLBACK(ags_sf2_synth_program_tree_view_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->synth_octave,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_synth_octave_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->synth_key,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_synth_key_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->synth_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_synth_volume_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->chorus_input_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_chorus_input_volume_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->chorus_output_volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_chorus_output_volume_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->chorus_lfo_oscillator,
		      "any_signal::changed",
		      G_CALLBACK(ags_sf2_synth_chorus_lfo_oscillator_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->chorus_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_chorus_lfo_frequency_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->chorus_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_chorus_depth_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->chorus_mix,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_chorus_mix_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->chorus_delay,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_chorus_delay_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->tremolo_enabled,
		      "any_signal::toggled",
		      G_CALLBACK(ags_sf2_synth_tremolo_enabled_callback),
		      sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->tremolo_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_tremolo_gain_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->tremolo_lfo_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_tremolo_lfo_depth_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->tremolo_lfo_freq,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_tremolo_lfo_freq_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->tremolo_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_tremolo_tuning_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->vibrato_enabled,
		      "any_signal::toggled",
		      G_CALLBACK(ags_sf2_synth_vibrato_enabled_callback),
		      sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->vibrato_gain,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_vibrato_gain_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->vibrato_lfo_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_vibrato_lfo_depth_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->vibrato_lfo_freq,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_vibrato_lfo_freq_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->wah_wah_enabled,
		      "any_signal::toggled",
		      G_CALLBACK(ags_sf2_synth_wah_wah_enabled_callback),
		      sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->wah_wah_length,
		      "any_signal::changed",
		      G_CALLBACK(ags_sf2_synth_wah_wah_length_callback),
		      sf2_synth,
		      NULL);
  
  g_object_disconnect((GObject *) sf2_synth->wah_wah_attack_x,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_wah_wah_attack_callback),
		      sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->wah_wah_attack_y,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_wah_wah_attack_callback),
		      sf2_synth,
		      NULL);
  
  g_object_disconnect((GObject *) sf2_synth->wah_wah_decay_x,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_wah_wah_decay_callback),
		      sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->wah_wah_decay_y,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_wah_wah_decay_callback),
		      sf2_synth,
		      NULL);
  
  g_object_disconnect((GObject *) sf2_synth->wah_wah_sustain_x,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_wah_wah_sustain_callback),
		      sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->wah_wah_sustain_y,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_wah_wah_sustain_callback),
		      sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->wah_wah_release_x,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_wah_wah_release_callback),
		      sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->wah_wah_release_y,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_wah_wah_release_callback),
		      sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->wah_wah_ratio,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_wah_wah_ratio_callback),
		      sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->wah_wah_lfo_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_wah_wah_lfo_depth_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->wah_wah_lfo_freq,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_wah_wah_lfo_freq_callback),
		      (gpointer) sf2_synth,
		      NULL);

  g_object_disconnect((GObject *) sf2_synth->wah_wah_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_sf2_synth_wah_wah_tuning_callback),
		      (gpointer) sf2_synth,
		      NULL);
}

void
ags_sf2_synth_wah_wah_draw_function(GtkDrawingArea *area,
				    cairo_t *cr,
				    int width,
				    int height,
				    AgsSF2Synth *sf2_synth)
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
		position_x, 120.0 - 120.0 * ags_dial_get_value(sf2_synth->wah_wah_ratio));

  cairo_line_to(cr,
		position_x + 480.0 * ags_dial_get_value(sf2_synth->wah_wah_attack_x), 120.0 - 120.0 * (ags_dial_get_value(sf2_synth->wah_wah_ratio) + ags_dial_get_value(sf2_synth->wah_wah_attack_y)));

  /* decay */
  position_x += 480.0 * ags_dial_get_value(sf2_synth->wah_wah_attack_x);
  
  cairo_move_to(cr,
		position_x, 120.0 - 120.0 * (ags_dial_get_value(sf2_synth->wah_wah_ratio) + ags_dial_get_value(sf2_synth->wah_wah_attack_y)));

  cairo_line_to(cr,
		position_x + 480.0 * ags_dial_get_value(sf2_synth->wah_wah_decay_x), 120.0 - 120.0 * (ags_dial_get_value(sf2_synth->wah_wah_ratio) + ags_dial_get_value(sf2_synth->wah_wah_decay_y)));

  /* sustain */
  position_x += 480.0 * ags_dial_get_value(sf2_synth->wah_wah_decay_x);
  
  cairo_move_to(cr,
		position_x, 120.0 - 120.0 * (ags_dial_get_value(sf2_synth->wah_wah_ratio) + ags_dial_get_value(sf2_synth->wah_wah_decay_y)));

  cairo_line_to(cr,
		position_x + 480.0 * ags_dial_get_value(sf2_synth->wah_wah_sustain_x), 120.0 - 120.0 * (ags_dial_get_value(sf2_synth->wah_wah_ratio) + ags_dial_get_value(sf2_synth->wah_wah_sustain_y)));

  /* release */
  position_x += 480.0 * ags_dial_get_value(sf2_synth->wah_wah_sustain_x);
  
  cairo_move_to(cr,
		position_x, 120.0 - 120.0 * (ags_dial_get_value(sf2_synth->wah_wah_ratio) + ags_dial_get_value(sf2_synth->wah_wah_sustain_y)));

  cairo_line_to(cr,
		position_x + 480.0 * ags_dial_get_value(sf2_synth->wah_wah_release_x), 120.0 - 120.0 * (ags_dial_get_value(sf2_synth->wah_wah_ratio) + ags_dial_get_value(sf2_synth->wah_wah_release_y)));

  cairo_stroke(cr);
}

void
ags_sf2_synth_resize_audio_channels(AgsMachine *machine,
				    guint audio_channels, guint audio_channels_old,
				    gpointer data)
{
  AgsSF2Synth *sf2_synth;

  guint i;
  guint j;

  sf2_synth = (AgsSF2Synth *) machine;

  if(audio_channels > audio_channels_old){    
    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_sf2_synth_input_map_recall(sf2_synth,
				     audio_channels_old,
				     0);
      
      ags_sf2_synth_output_map_recall(sf2_synth,
				      audio_channels_old,
				      0);
    }
  }
}

void
ags_sf2_synth_resize_pads(AgsMachine *machine, GType channel_type,
			  guint pads, guint pads_old,
			  gpointer data)
{
  AgsSF2Synth *sf2_synth;

  guint i;
  guint j;
  gboolean grow;

  sf2_synth = (AgsSF2Synth *) machine;
  
  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(g_type_is_a(channel_type, AGS_TYPE_INPUT)){
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	/* depending on destination */
	ags_sf2_synth_input_map_recall(sf2_synth,
				       0,
				       pads_old);
      }
    }else{
      sf2_synth->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_sf2_synth_output_map_recall(sf2_synth,
					0,
					pads_old);
      }
    }else{
      sf2_synth->mapped_output_pad = pads;
    }
  }
}

void
ags_sf2_synth_map_recall(AgsMachine *machine)
{
  AgsSF2Synth *sf2_synth;
  
  AgsAudio *audio;

  GList *start_recall;

  gint position;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  sf2_synth = AGS_SF2_SYNTH(machine);

  audio = machine->audio;
  
  position = 0;
  
  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       sf2_synth->playback_play_container, sf2_synth->playback_recall_container,
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

  /* ags-fx-sf2-synth */
  start_recall = ags_fx_factory_create(audio,
				       sf2_synth->sf2_synth_play_container, sf2_synth->sf2_synth_recall_container,
				       "ags-fx-sf2-synth",
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
				       sf2_synth->tremolo_play_container, sf2_synth->tremolo_recall_container,
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
				       sf2_synth->envelope_play_container, sf2_synth->envelope_recall_container,
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
				       sf2_synth->wah_wah_play_container, sf2_synth->wah_wah_recall_container,
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
				       sf2_synth->buffer_play_container, sf2_synth->buffer_recall_container,
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
  ags_sf2_synth_input_map_recall(sf2_synth,
				 0,
				 0);

  /* depending on destination */
  ags_sf2_synth_output_map_recall(sf2_synth,
				  0,
				  0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_sf2_synth_parent_class)->map_recall(machine);  
}

void
ags_sf2_synth_input_map_recall(AgsSF2Synth *sf2_synth,
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

  audio = AGS_MACHINE(sf2_synth)->audio;

  position = 0;

  /* get some fields */
  input_pads = AGS_MACHINE(sf2_synth)->input_pads;
  audio_channels = AGS_MACHINE(sf2_synth)->audio_channels;    

  for(i = 0; i < input_pads; i++){
    for(j = 0; j < audio_channels; j++){
      AgsMachineInputLine* input_line;

      input_line = g_list_nth_data(AGS_MACHINE(sf2_synth)->machine_input_line,
				   (i * audio_channels) + j);

      if(input_line != NULL &&
	 input_line->mapped_recall == FALSE){
	/* ags-fx-playback */
	start_recall = ags_fx_factory_create(audio,
					     sf2_synth->playback_play_container, sf2_synth->playback_recall_container,
					     "ags-fx-playback",
					     NULL,
					     NULL,
					     j, j + 1,
					     i, i + 1,
					     position,
					     (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

	g_list_free_full(start_recall,
			 (GDestroyNotify) g_object_unref);

	/* ags-fx-sf2-synth */
	start_recall = ags_fx_factory_create(audio,
					     sf2_synth->sf2_synth_play_container, sf2_synth->sf2_synth_recall_container,
					     "ags-fx-sf2-synth",
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
					     sf2_synth->tremolo_play_container, sf2_synth->tremolo_recall_container,
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
					     sf2_synth->envelope_play_container, sf2_synth->envelope_recall_container,
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
					     sf2_synth->wah_wah_play_container, sf2_synth->wah_wah_recall_container,
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
					     sf2_synth->buffer_play_container, sf2_synth->buffer_recall_container,
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
  
  sf2_synth->mapped_input_audio_channel = audio_channels;
  sf2_synth->mapped_input_pad = input_pads;
}

void
ags_sf2_synth_output_map_recall(AgsSF2Synth *sf2_synth,
				guint audio_channel_start,
				guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;
  guint audio_channels;

  audio = AGS_MACHINE(sf2_synth)->audio;
  
  /* get some fields */
  output_pads = AGS_MACHINE(sf2_synth)->output_pads;
  audio_channels = AGS_MACHINE(sf2_synth)->audio_channels;    
  
  sf2_synth->mapped_output_audio_channel = audio_channels;
  sf2_synth->mapped_output_pad = output_pads;
}

gint
ags_sf2_synth_int_compare_func(gconstpointer a,
			       gconstpointer b)
{
  if(GPOINTER_TO_INT(a) < GPOINTER_TO_INT(b)){
    return(-1);
  }else if(GPOINTER_TO_INT(a) == GPOINTER_TO_INT(b)){
    return(0);
  }else{
    return(1);
  }
}

void
ags_sf2_synth_refresh_port(AgsMachine *machine)
{
  AgsSF2Synth *sf2_synth;
  AgsChannel *start_channel, *channel;
  
  GList *start_play, *start_recall, *recall;

  sf2_synth = (AgsSF2Synth *) machine;
  
  start_play = ags_audio_get_play(machine->audio);
  start_recall = ags_audio_get_recall(machine->audio);

  recall =
    start_recall = g_list_concat(start_play, start_recall);

  machine->flags |= AGS_MACHINE_NO_UPDATE;

  if((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SF2_SYNTH_AUDIO)) != NULL){
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

      ags_dial_set_value(sf2_synth->synth_octave,
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

      ags_dial_set_value(sf2_synth->synth_key,
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

      ags_dial_set_value(sf2_synth->synth_volume,
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
	gtk_check_button_set_active(sf2_synth->chorus_enabled,
				    TRUE);
      }else{
	gtk_check_button_set_active(sf2_synth->chorus_enabled,
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

      gtk_combo_box_set_active(sf2_synth->chorus_lfo_oscillator,
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

      gtk_spin_button_set_value(sf2_synth->chorus_lfo_frequency,
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

      ags_dial_set_value(sf2_synth->chorus_depth,
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

      ags_dial_set_value(sf2_synth->chorus_mix,
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

      ags_dial_set_value(sf2_synth->chorus_delay,
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

      ags_dial_set_value(sf2_synth->vibrato_gain,
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

      ags_dial_set_value(sf2_synth->vibrato_lfo_depth,
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

      ags_dial_set_value(sf2_synth->vibrato_lfo_freq,
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

      ags_dial_set_value(sf2_synth->vibrato_tuning,
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

      ags_dial_set_value(sf2_synth->tremolo_gain,
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

      ags_dial_set_value(sf2_synth->tremolo_lfo_depth,
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

      ags_dial_set_value(sf2_synth->tremolo_lfo_freq,
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

      ags_dial_set_value(sf2_synth->tremolo_tuning,
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

	ags_dial_set_value(sf2_synth->wah_wah_attack_x,
			   (gdouble) creal((double _Complex) g_value_get_float(&value)));

	ags_dial_set_value(sf2_synth->wah_wah_attack_y,
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

	ags_dial_set_value(sf2_synth->wah_wah_decay_x,
			   (gdouble) creal((double _Complex) g_value_get_float(&value)));

	ags_dial_set_value(sf2_synth->wah_wah_decay_y,
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

	ags_dial_set_value(sf2_synth->wah_wah_sustain_x,
			   (gdouble) creal((double _Complex) g_value_get_float(&value)));

	ags_dial_set_value(sf2_synth->wah_wah_sustain_y,
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

	ags_dial_set_value(sf2_synth->wah_wah_release_x,
			   (gdouble) creal((double _Complex) g_value_get_float(&value)));

	ags_dial_set_value(sf2_synth->wah_wah_release_y,
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

	ags_dial_set_value(sf2_synth->wah_wah_lfo_depth,
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

	ags_dial_set_value(sf2_synth->wah_wah_lfo_freq,
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

	ags_dial_set_value(sf2_synth->wah_wah_tuning,
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
 * ags_sf2_synth_open_filename:
 * @sf2_synth: the #AgsSF2Synth
 * @filename: the filename
 * 
 * Open @filename.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_open_filename(AgsSF2Synth *sf2_synth,
			    gchar *filename)
{
  AgsSF2Loader *sf2_loader;

  if(!AGS_IS_SF2_SYNTH(sf2_synth) ||
     filename == NULL ||
     strlen(filename) == 0 ||
     !g_file_test(filename, G_FILE_TEST_EXISTS)){
    return;
  }
  
  sf2_synth->sf2_loader = 
    sf2_loader = ags_sf2_loader_new(AGS_MACHINE(sf2_synth)->audio,
				    filename,
				    NULL,
				    NULL);

  ags_sf2_loader_start(sf2_loader);
}

/**
 * ags_sf2_synth_load_bank:
 * @sf2_synth: the #AgsSF2Synth
 * @bank: the bank
 * 
 * Load bank of @sf2_synth.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_load_bank(AgsSF2Synth *sf2_synth,
			gint bank)
{
  GtkListStore *program_list_store;
	
  AgsIpatch *ipatch;

  IpatchSF2 *sf2;
  IpatchItem *ipatch_item;
  IpatchList *ipatch_list;
	
  IpatchIter preset_iter;

  GError *error;
  
  GRecMutex *audio_container_mutex;

  program_list_store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(sf2_synth->program_tree_view)));

  gtk_list_store_clear(program_list_store);

  audio_container_mutex = AGS_AUDIO_CONTAINER_GET_OBJ_MUTEX(sf2_synth->audio_container);

  g_rec_mutex_lock(audio_container_mutex);
  
  ipatch = (AgsIpatch *) sf2_synth->audio_container->sound_container;

  error = NULL;
  sf2 = (IpatchSF2 *) ipatch_convert_object_to_type((GObject *) ipatch->handle->file,
						    IPATCH_TYPE_SF2,
						    &error);

  if(error != NULL){
    g_error_free(error);
  }
	
  ipatch_list = ipatch_container_get_children((IpatchContainer *) sf2,
					      IPATCH_TYPE_SF2_PRESET);

  if(ipatch_list != NULL){
    ipatch_list_init_iter(ipatch_list, &preset_iter);
    
    if(ipatch_iter_first(&preset_iter) != NULL){
      GtkTreeModel *model;
      
      GList *start_list_bank, *list_bank;
      GList *start_list_program, *list_program;
      GList *start_list_name, *list_name;

      GtkTreeIter tree_iter;

      guint i;
      guint j;

      start_list_bank = NULL;
      start_list_program = NULL;
      start_list_name = NULL;

      i = 0;
      j = 0;
      
      do{
	gchar *current_name;
	
	int current_bank, current_program;
	      
	ipatch_item = ipatch_iter_get(&preset_iter);

	ipatch_sf2_preset_get_midi_locale((IpatchSF2Preset *) ipatch_item,
					  &current_bank,
					  &current_program);

	if(current_bank == bank){	    
	  if(g_list_find(start_list_program, GINT_TO_POINTER(current_program)) == NULL){
	    current_name = ipatch_sf2_preset_get_name((IpatchSF2Preset *) ipatch_item);
	  
	    start_list_program = g_list_insert_sorted(start_list_program,
						      GINT_TO_POINTER(current_program),
						      ags_sf2_synth_int_compare_func);

	    start_list_bank = g_list_insert(start_list_bank,
					    GINT_TO_POINTER(current_bank),
					    g_list_index(start_list_program,
							 GINT_TO_POINTER(current_program)));

	    start_list_name = g_list_insert(start_list_name,
					    current_name,
					    g_list_index(start_list_program,
							 GINT_TO_POINTER(current_program)));
	    j++;	    
	  }
	  
	  i++;	  
	}
      }while(ipatch_iter_next(&preset_iter) != NULL);

      list_bank = start_list_bank;
      list_program = start_list_program;
      list_name = start_list_name;

      while(list_program != NULL){
	gchar *current_name;
	
	int current_bank, current_program;

	current_bank = GPOINTER_TO_INT(list_bank->data);
	current_program = GPOINTER_TO_INT(list_program->data);
	current_name = list_name->data;
	
	if(current_bank == bank){
	  gtk_list_store_append(program_list_store,
				&tree_iter);

	  gtk_list_store_set(program_list_store, &tree_iter,
			     0, bank,
			     1, current_program,
			     2, current_name,
			     -1);
	}

	list_bank = list_bank->next;
	list_program = list_program->next;
	list_name = list_name->next;
      }

      model = GTK_TREE_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(sf2_synth->program_tree_view)));

       if(model != NULL &&
	  gtk_tree_model_get_iter_first(model, &tree_iter)){
	 gtk_tree_view_set_cursor(GTK_TREE_VIEW(sf2_synth->program_tree_view),
				  gtk_tree_model_get_path(model,
							  &tree_iter),
				  NULL,
				  FALSE);

	 gtk_tree_view_row_activated(GTK_TREE_VIEW(sf2_synth->program_tree_view),
				     gtk_tree_model_get_path(model,
							     &tree_iter),
				     NULL);
       }
       
       ags_sf2_synth_load_midi_locale(sf2_synth,
				      bank,
				      GPOINTER_TO_INT(start_list_program->data));

      g_list_free(start_list_bank);
      g_list_free(start_list_program);
      g_list_free(start_list_name);
    }
  }
  
  g_rec_mutex_unlock(audio_container_mutex);
}

/**
 * ags_sf2_synth_load_midi_locale:
 * @sf2_synth: the #AgsSF2Synth
 * @bank: the bank
 * @program: the program
 * 
 * Load bank and program of @sf2_synth.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_load_midi_locale(AgsSF2Synth *sf2_synth,
			       gint bank,
			       gint program)
{
  AgsIpatch *ipatch;
  AgsFxSF2SynthAudio *fx_sf2_synth_audio;
  
  IpatchSF2 *sf2;
  IpatchSF2Preset *sf2_preset;  

  guint i;
  guint j;
  guint k;
  
  GError *error;
  
  GRecMutex *audio_container_mutex;

  g_return_if_fail(AGS_IS_SF2_SYNTH(sf2_synth));
  g_return_if_fail(sf2_synth->audio_container != NULL);
  g_return_if_fail(sf2_synth->audio_container->sound_container != NULL);
  
  audio_container_mutex = AGS_AUDIO_CONTAINER_GET_OBJ_MUTEX(sf2_synth->audio_container);
  
  g_rec_mutex_lock(audio_container_mutex);

  ipatch = (AgsIpatch *) sf2_synth->audio_container->sound_container;

  fx_sf2_synth_audio = NULL;

  error = NULL;
  sf2 = (IpatchSF2 *) ipatch_convert_object_to_type((GObject *) ipatch->handle->file,
						    IPATCH_TYPE_SF2,
						    &error);

  if(error != NULL){
    g_error_free(error);
  }

  sf2_preset = ipatch_sf2_find_preset(sf2,
				      NULL,
				      bank,
				      program,
				      NULL);

  if(sf2_synth->audio_container != NULL &&
     sf2_preset != NULL){
    fx_sf2_synth_audio = (AgsFxSF2SynthAudio *) ags_recall_container_get_recall_audio(sf2_synth->sf2_synth_recall_container);
    
    sf2_synth->bank = bank;
    sf2_synth->program = program;

    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      AgsFxSF2SynthAudioScopeData *scope_data;
      
      scope_data = fx_sf2_synth_audio->scope_data[i];

      if(scope_data != NULL){
	for(j = 0; j < scope_data->audio_channels; j++){
	  AgsFxSF2SynthAudioChannelData *channel_data;

	  channel_data = scope_data->channel_data[j];
	  
	  if(channel_data != NULL){
	    AgsSF2MidiLocaleLoader *sf2_midi_locale_loader;
	  
	    sf2_midi_locale_loader =
	      sf2_synth->sf2_midi_locale_loader = ags_sf2_midi_locale_loader_new(AGS_MACHINE(sf2_synth)->audio,
										 sf2_synth->audio_container->filename,
										 bank,
										 program);
	    
	    ags_sf2_midi_locale_loader_set_flags(sf2_midi_locale_loader,
						 AGS_SF2_MIDI_LOCALE_LOADER_RUN_APPLY_MIDI_LOCALE);
      
	    sf2_midi_locale_loader->synth = channel_data->synth;
      
	    ags_sf2_midi_locale_loader_start(sf2_midi_locale_loader);
	  }
	}
      }
    }
    
    if(fx_sf2_synth_audio != NULL){
      g_object_unref(fx_sf2_synth_audio);
    }
  }
  
  g_rec_mutex_unlock(audio_container_mutex);
}

/**
 * ags_sf2_synth_sf2_loader_completed_timeout:
 * @sf2_synth: the #AgsSF2Synth
 *
 * Queue draw widget
 *
 * Returns: %TRUE if proceed poll completed, otherwise %FALSE
 *
 * Since: 3.4.0
 */
gboolean
ags_sf2_synth_sf2_loader_completed_timeout(AgsSF2Synth *sf2_synth)
{
  if(g_hash_table_lookup(ags_sf2_synth_sf2_loader_completed,
			 sf2_synth) != NULL){
    if(sf2_synth->sf2_loader != NULL){
      if(ags_sf2_loader_test_flags(sf2_synth->sf2_loader, AGS_SF2_LOADER_HAS_COMPLETED)){
	GtkListStore *bank_list_store;
	GtkTreeModel *model;
	
	AgsIpatch *ipatch;

	IpatchSF2 *sf2;
	IpatchItem *ipatch_item;
	IpatchList *ipatch_list;

	GtkTreeIter tree_iter;	
	IpatchIter preset_iter;

	gint bank;
	
	GError *error;
	
	/* reassign audio container */
	sf2_synth->audio_container = sf2_synth->sf2_loader->audio_container;
	sf2_synth->sf2_loader->audio_container = NULL;

	bank_list_store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(sf2_synth->bank_tree_view)));

	gtk_list_store_clear(bank_list_store);
	gtk_list_store_clear(GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(sf2_synth->program_tree_view))));

	ipatch = NULL;
	
	if(sf2_synth->audio_container != NULL){
	  ipatch = (AgsIpatch *) sf2_synth->audio_container->sound_container;
	}

	if(ipatch == NULL ||
	   ipatch->handle == NULL ||
	   ipatch->handle->file == NULL){
	  g_object_unref(sf2_synth->sf2_loader);

	  sf2_synth->sf2_loader = NULL;

	  sf2_synth->position = -1;

	  gtk_spinner_stop(sf2_synth->sf2_loader_spinner);
	  gtk_widget_hide((GtkWidget *) sf2_synth->sf2_loader_spinner);
	  
	  return(TRUE);
	}
	
	error = NULL;
	sf2 = (IpatchSF2 *) ipatch_convert_object_to_type((GObject *) ipatch->handle->file,
							  IPATCH_TYPE_SF2,
							  &error);

	if(error != NULL){
	  g_error_free(error);
	}
	
	ipatch_list = ipatch_container_get_children((IpatchContainer *) sf2,
						    IPATCH_TYPE_SF2_PRESET);

	if(ipatch_list != NULL){
	  ipatch_list_init_iter(ipatch_list, &preset_iter);
    
	  if(ipatch_iter_first(&preset_iter) != NULL){
	    GList *start_list, *list;

	    GtkTreeIter tree_iter;

	    start_list = NULL;
	    
	    do{	      
	      int bank, program;
	      
	      ipatch_item = ipatch_iter_get(&preset_iter);

	      bank = 0;
	      program = 0;
	      
	      ipatch_sf2_preset_get_midi_locale((IpatchSF2Preset *) ipatch_item,
						&bank,
						&program);

	      if(g_list_find(start_list, GINT_TO_POINTER(bank)) == NULL){
		start_list = g_list_insert_sorted(start_list,
						  GINT_TO_POINTER(bank),
						  ags_sf2_synth_int_compare_func);
	      }
	    }while(ipatch_iter_next(&preset_iter) != NULL);

	    list = start_list;

	    while(list != NULL){
	      gtk_list_store_append(bank_list_store,
				    &tree_iter);

	      gtk_list_store_set(bank_list_store, &tree_iter,
				 0, GPOINTER_TO_INT(list->data),
				 -1);

	      list = list->next;
	    }

	    bank = GPOINTER_TO_INT(start_list->data);

	    g_list_free(start_list);
	  }
	}

	model = GTK_TREE_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(sf2_synth->bank_tree_view)));

	if(model != NULL &&
	   gtk_tree_model_get_iter_first(model, &tree_iter)){
	  gtk_tree_view_set_cursor(GTK_TREE_VIEW(sf2_synth->bank_tree_view),
				   gtk_tree_model_get_path(model,
							   &tree_iter),
				   NULL,
				   FALSE);

	  gtk_tree_view_row_activated(GTK_TREE_VIEW(sf2_synth->bank_tree_view),
				      gtk_tree_model_get_path(model,
							      &tree_iter),
				      NULL);
	}

	g_object_unref(sf2_synth->sf2_loader);

	sf2_synth->sf2_loader = NULL;

	sf2_synth->position = -1;

	gtk_spinner_stop(sf2_synth->sf2_loader_spinner);
	gtk_widget_set_visible((GtkWidget *) sf2_synth->sf2_loader_spinner,
			       FALSE);
	  
	return(TRUE);
      }else{
	if(sf2_synth->position == -1){
	  sf2_synth->position = 0;

	  gtk_widget_set_visible((GtkWidget *) sf2_synth->sf2_loader_spinner,
				 TRUE);
	  gtk_spinner_start(sf2_synth->sf2_loader_spinner);
	}

	return(TRUE);
      }
    }

    if(sf2_synth->load_bank >= 0){
      GtkTreeModel *model;
	  
      GtkTreeIter tree_iter;
	  
      model = GTK_TREE_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(sf2_synth->bank_tree_view)));

      if(model != NULL &&
	 gtk_tree_model_get_iter_first(model, &tree_iter)){
	do{
	  gint current_bank;

	  current_bank = 0;
	      
	  gtk_tree_model_get(model,
			     &tree_iter,
			     0, &current_bank,
			     -1);

	  if(current_bank == sf2_synth->load_bank){
	    gtk_tree_view_set_cursor(GTK_TREE_VIEW(sf2_synth->bank_tree_view),
				     gtk_tree_model_get_path(model,
							     &tree_iter),
				     NULL,
				     FALSE);
	    
	    ags_sf2_synth_load_bank(sf2_synth,
				    current_bank);
		
	    break;
	  }
	}while(gtk_tree_model_iter_next(model, &tree_iter));
      }

      sf2_synth->load_bank = -1;
	  
      return(TRUE);
    }

    if(sf2_synth->load_program >= 0){
      GtkTreeModel *model;

      GtkTreeIter tree_iter;
	  
      model = GTK_TREE_MODEL(gtk_tree_view_get_model(GTK_TREE_VIEW(sf2_synth->program_tree_view)));

      if(model != NULL &&
	 gtk_tree_model_get_iter_first(model, &tree_iter)){
	do{
	  gint current_bank;
	  gint current_program;

	  current_bank = 0;
	  current_program = 0;
	      
	  gtk_tree_model_get(model,
			     &tree_iter,
			     0, &current_bank,
			     1, &current_program,
			     -1);

	  if(current_program == sf2_synth->load_program){
	    gtk_tree_view_set_cursor(GTK_TREE_VIEW(sf2_synth->program_tree_view),
				     gtk_tree_model_get_path(model,
							     &tree_iter),
				     NULL,
				     FALSE);
		
	    ags_sf2_synth_load_midi_locale(sf2_synth,
					   current_bank,
					   current_program);
	    break;
	  }
	}while(gtk_tree_model_iter_next(model, &tree_iter));
      }

      sf2_synth->load_program = -1;
	  
      return(TRUE);
    }	
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_sf2_synth_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsSF2Synth
 *
 * Returns: the new #AgsSF2Synth
 *
 * Since: 3.4.0
 */
AgsSF2Synth*
ags_sf2_synth_new(GObject *soundcard)
{
  AgsSF2Synth *sf2_synth;

  sf2_synth = (AgsSF2Synth *) g_object_new(AGS_TYPE_SF2_SYNTH,
					   NULL);

  g_object_set(AGS_MACHINE(sf2_synth)->audio,
	       "output-soundcard", soundcard,
	       NULL);

  return(sf2_synth);
}
