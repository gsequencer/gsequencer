/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/app/machine/ags_matrix.h>
#include <ags/app/machine/ags_matrix_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_navigation.h>

#include <math.h>

#include <ags/i18n.h>

void ags_matrix_class_init(AgsMatrixClass *matrix);
void ags_matrix_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_matrix_init(AgsMatrix *matrix);
void ags_matrix_finalize(GObject *gobject);

void ags_matrix_connect(AgsConnectable *connectable);
void ags_matrix_disconnect(AgsConnectable *connectable);

void ags_matrix_map_recall(AgsMachine *machine);

void ags_matrix_resize_audio_channels(AgsMachine *machine,
				      guint audio_channels, guint audio_channels_old,
				      gpointer data);
void ags_matrix_resize_pads(AgsMachine *machine, GType type,
			    guint pads, guint pads_old,
			    gpointer data);

#define AGS_MATRIX_INPUT_LINE_MAPPED_KEY "AGS_MATRIX_INPUT_LINE_MAPPED_KEY"
#define AGS_MATRIX_INPUT_LINE_MAPPED_DATA "AGS_MATRIX_INPUT_LINE_MAPPED_DATA"

/**
 * SECTION:ags_matrix
 * @short_description: matrix sequencer
 * @title: AgsMatrix
 * @section_id:
 * @include: ags/app/machine/ags_matrix.h
 *
 * The #AgsMatrix is a composite widget to act as matrix sequencer.
 */

static gpointer ags_matrix_parent_class = NULL;
static AgsConnectableInterface *ags_matrix_parent_connectable_interface;

const char *AGS_MATRIX_INDEX = "AgsMatrixIndex";

GType
ags_matrix_get_type(void)
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_matrix = 0;

    static const GTypeInfo ags_matrix_info = {
      sizeof(AgsMatrixClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_matrix_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsMatrix),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_matrix_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_matrix_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_matrix = g_type_register_static(AGS_TYPE_MACHINE,
					    "AgsMatrix", &ags_matrix_info,
					    0);
    
    g_type_add_interface_static(ags_type_matrix,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_matrix);
  }

  return g_define_type_id__volatile;
}

void
ags_matrix_class_init(AgsMatrixClass *matrix)
{
  GObjectClass *gobject;
  AgsMachineClass *machine;

  ags_matrix_parent_class = g_type_class_peek_parent(matrix);

  /* GObjectClass */
  gobject = (GObjectClass *) matrix;

  gobject->finalize = ags_matrix_finalize;

  /* AgsMachine */
  machine = (AgsMachineClass *) matrix;

  machine->map_recall = ags_matrix_map_recall;
}

void
ags_matrix_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_matrix_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_matrix_connect;
  connectable->disconnect = ags_matrix_disconnect;
}

void
ags_matrix_init(AgsMatrix *matrix)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  GtkFrame *frame;
  GtkGrid *grid;
  GtkToggleButton *button;
  GtkBox *vbox;
  GtkBox *hbox;
  GtkBox *volume_hbox;
  GtkLabel *label;
  
  AgsAudio *audio;

  AgsApplicationContext *application_context;   

  AgsMachineCounterManager *machine_counter_manager;
  AgsMachineCounter *machine_counter;
  
  gchar *machine_name;

  gint position;
  gdouble gui_scale_factor;
  int i, j;

  application_context = ags_application_context_get_instance();
  
  /* machine counter */
  machine_counter_manager = ags_machine_counter_manager_get_instance();

  machine_counter = ags_machine_counter_manager_find_machine_counter(machine_counter_manager,
								     AGS_TYPE_MATRIX);

  machine_name = NULL;

  if(machine_counter != NULL){
    machine_name = g_strdup_printf("Default %d",
				   machine_counter->counter);
  
    ags_machine_counter_increment(machine_counter);
  }
  
  g_object_set(matrix,
	       "machine-name", machine_name,
	       NULL);

  g_free(machine_name);

  /* machine selector */
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  position = g_list_length(window->machine);
  
  ags_machine_selector_popup_insert_machine(composite_editor->machine_selector,
					    position,
					    (AgsMachine *) matrix);

  /* scale factor */
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));

  audio = AGS_MACHINE(matrix)->audio;
  ags_audio_set_flags(audio, (AGS_AUDIO_SYNC |
			      AGS_AUDIO_ASYNC |
			      AGS_AUDIO_OUTPUT_HAS_RECYCLING |
			      AGS_AUDIO_INPUT_HAS_RECYCLING));
  ags_audio_set_ability_flags(audio, (AGS_SOUND_ABILITY_PLAYBACK |
				      AGS_SOUND_ABILITY_SEQUENCER |
				      AGS_SOUND_ABILITY_NOTATION));
  ags_audio_set_behaviour_flags(audio, (AGS_SOUND_BEHAVIOUR_PATTERN_MODE |
					AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING |
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

  audio->bank_dim[0] = 1;
  audio->bank_dim[1] = 9;
  audio->bank_dim[2] = 32;

  AGS_MACHINE(matrix)->flags |= (AGS_MACHINE_REVERSE_NOTATION);
  
  AGS_MACHINE(matrix)->input_pad_type = G_TYPE_NONE;
  AGS_MACHINE(matrix)->input_line_type = G_TYPE_NONE;
  AGS_MACHINE(matrix)->output_pad_type = G_TYPE_NONE;
  AGS_MACHINE(matrix)->output_line_type = G_TYPE_NONE;

  g_signal_connect_after(G_OBJECT(matrix), "resize-audio-channels",
			 G_CALLBACK(ags_matrix_resize_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(matrix), "resize-pads",
			 G_CALLBACK(ags_matrix_resize_pads), NULL);

  /*  */
  AGS_MACHINE(matrix)->flags |= (AGS_MACHINE_IS_SEQUENCER |
				 AGS_MACHINE_REVERSE_NOTATION);
  AGS_MACHINE(matrix)->mapping_flags |= AGS_MACHINE_MONO;
  
  matrix->flags = 0;

  matrix->name = NULL;
  matrix->xml_type = "ags-matrix";

  matrix->mapped_input_pad = 0;
  matrix->mapped_output_pad = 0;

  matrix->playback_play_container = ags_recall_container_new();
  matrix->playback_recall_container = ags_recall_container_new();

  matrix->pattern_play_container = ags_recall_container_new();
  matrix->pattern_recall_container = ags_recall_container_new();

  matrix->notation_play_container = ags_recall_container_new();
  matrix->notation_recall_container = ags_recall_container_new();

  matrix->volume_play_container = ags_recall_container_new();
  matrix->volume_recall_container = ags_recall_container_new();

  matrix->envelope_play_container = ags_recall_container_new();
  matrix->envelope_recall_container = ags_recall_container_new();

  matrix->buffer_play_container = ags_recall_container_new();
  matrix->buffer_recall_container = ags_recall_container_new();
  
  /* create widgets */
  matrix->grid = (GtkGrid *) gtk_grid_new();

  gtk_widget_set_vexpand((GtkWidget *) matrix->grid,
			 FALSE);
  gtk_widget_set_hexpand((GtkWidget *) matrix->grid,
			 FALSE);

  gtk_widget_set_halign((GtkWidget *) matrix->grid,
			GTK_ALIGN_START);
  gtk_widget_set_valign((GtkWidget *) matrix->grid,
			GTK_ALIGN_START);

  gtk_grid_set_column_spacing(matrix->grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(matrix->grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_frame_set_child(AGS_MACHINE(matrix)->frame,
		      (GtkWidget *) matrix->grid);

  AGS_MACHINE(matrix)->play = 
    matrix->run = (GtkToggleButton *) gtk_toggle_button_new_with_label("run");

  gtk_widget_set_valign((GtkWidget *) matrix->run,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) matrix->run,
			GTK_ALIGN_FILL);
  
  gtk_grid_attach(matrix->grid,
		  (GtkWidget *) matrix->run,
		  0, 0,
		  1, 1);

  grid = (GtkGrid *) gtk_grid_new();

  gtk_widget_set_valign((GtkWidget *) grid,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) grid,
			GTK_ALIGN_FILL);

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_grid_attach(matrix->grid,
		  (GtkWidget *) grid,
		  1, 0,
		  1, 1);
  matrix->selected = NULL;

  for(i = 0; i < 3; i++){
    for(j = 0; j < 3; j++){
      //TODO:JK: verify leak of string
      matrix->index[i * 3 + j] =
	button = (GtkToggleButton *) gtk_toggle_button_new_with_label(g_strdup_printf("%d", i * 3 + j +1));
      g_object_set_data((GObject *) button, AGS_MATRIX_INDEX, GUINT_TO_POINTER(i * 3 + j));

      gtk_widget_set_valign((GtkWidget *) button,
			    GTK_ALIGN_FILL);
      gtk_widget_set_halign((GtkWidget *) button,
			    GTK_ALIGN_FILL);
      
      gtk_grid_attach(grid,
		      (GtkWidget *) button,
		      j, i,
		      1, 1);
    }
  }
		  
  matrix->selected = matrix->index[0];
  gtk_toggle_button_set_active(matrix->selected, TRUE);

  /* sequencer */
  matrix->cell_pattern = ags_cell_pattern_new();

  gtk_widget_set_valign((GtkWidget *) matrix->cell_pattern,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) matrix->cell_pattern,
			GTK_ALIGN_FILL);
  
  gtk_grid_attach(matrix->grid,
		  (GtkWidget *) matrix->cell_pattern,
		  2, 0,
		  1, 1);

  /* length and loop */
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);

  gtk_widget_set_valign((GtkWidget *) vbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) vbox,
			GTK_ALIGN_FILL);

  gtk_box_set_spacing(vbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_grid_attach(matrix->grid,
		  (GtkWidget *) vbox,
		  3, 0,
		  1, 1);

  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);

  gtk_box_set_spacing(hbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  label = (GtkLabel *) gtk_label_new(i18n("length"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_box_append(hbox,
		 (GtkWidget *) label);

  matrix->length_spin = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0, 32.0, 1.0);
  gtk_spin_button_set_value(matrix->length_spin, 16.0);
  gtk_box_append(hbox,
		 (GtkWidget *) matrix->length_spin);

  matrix->loop_button = (GtkCheckButton *) gtk_check_button_new_with_label("loop");
  gtk_box_append(vbox,
		 (GtkWidget *) matrix->loop_button);

  /* volume */
  frame = (GtkFrame *) gtk_frame_new(i18n("volume"));

  gtk_widget_set_valign((GtkWidget *) frame,
			GTK_ALIGN_START);
  gtk_widget_set_halign((GtkWidget *) frame,
			GTK_ALIGN_START);
  
  gtk_grid_attach(matrix->grid,
		  (GtkWidget *) frame,
		  4, 0,
		  1, 3);

  volume_hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				       0);
  gtk_frame_set_child(frame,
		      (GtkWidget *) volume_hbox);
  
  matrix->volume = (GtkScale *) gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
							 0.0,
							 2.0,
							 0.025);

  gtk_widget_set_size_request((GtkWidget *) matrix->volume,
			      (gint) (gui_scale_factor * 16.0), (gint) (gui_scale_factor * 100.0));
  
  gtk_box_append(volume_hbox,
		 (GtkWidget *) matrix->volume);

  gtk_scale_set_digits(matrix->volume,
		       3);

  gtk_range_set_increments(GTK_RANGE(matrix->volume),
			   0.025, 0.1);
  gtk_range_set_value(GTK_RANGE(matrix->volume),
		      1.0);
  gtk_range_set_inverted(GTK_RANGE(matrix->volume),
			 TRUE);  
}

void
ags_matrix_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_matrix_parent_class)->finalize(gobject);
}

void
ags_matrix_connect(AgsConnectable *connectable)
{
  AgsMatrix *matrix;

  int i;

  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) != 0){
    return;
  }

  /* call parent */
  ags_matrix_parent_connectable_interface->connect(connectable);

  /* AgsMatrix */
  matrix = AGS_MATRIX(connectable);

  for(i  = 0; i < 9; i++){
    g_signal_connect(G_OBJECT (matrix->index[i]), "notify::active",
		     G_CALLBACK (ags_matrix_index_callback), (gpointer) matrix);
  }

  ags_connectable_connect(AGS_CONNECTABLE(matrix->cell_pattern));
  
  g_signal_connect_after((GObject *) matrix->length_spin, "value-changed",
			 G_CALLBACK(ags_matrix_length_spin_callback), (gpointer) matrix);

  g_signal_connect((GObject *) matrix->loop_button, "notify::active",
		   G_CALLBACK(ags_matrix_loop_button_callback), (gpointer) matrix);

  g_signal_connect_after(G_OBJECT(matrix), "stop",
			 G_CALLBACK(ags_matrix_stop_callback), NULL);

  g_signal_connect((GObject *) matrix->volume, "value-changed",
		   G_CALLBACK(ags_matrix_volume_callback), (gpointer) matrix);
}

void
ags_matrix_disconnect(AgsConnectable *connectable)
{
  AgsMatrix *matrix;

  int i;

  if((AGS_CONNECTABLE_CONNECTED & (AGS_MACHINE(connectable)->connectable_flags)) == 0){
    return;
  }

  /* AgsMatrix */
  matrix = AGS_MATRIX(connectable);

  for(i  = 0; i < 9; i++){
    g_object_disconnect(G_OBJECT (matrix->index[i]),
			"any_signal::toggled",
			G_CALLBACK (ags_matrix_index_callback),
			(gpointer) matrix,
			NULL);
  }

  ags_connectable_disconnect(AGS_CONNECTABLE(matrix->cell_pattern));
  
  g_object_disconnect((GObject *) matrix->length_spin,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_matrix_length_spin_callback),
		      (gpointer) matrix,
		      NULL);

  g_object_disconnect((GObject *) matrix->loop_button,
		      "any_signal::toggled",
		      G_CALLBACK(ags_matrix_loop_button_callback),
		      (gpointer) matrix,
		      NULL);

  g_object_disconnect(G_OBJECT(matrix),
		      "any_signal::stop",
		      G_CALLBACK(ags_matrix_stop_callback),
		      NULL,
		      NULL);

  g_object_disconnect((GObject *) matrix->volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_matrix_volume_callback),
		      (gpointer) matrix,
		      NULL);

  /* call parent */
  ags_matrix_parent_connectable_interface->disconnect(connectable);
}

void
ags_matrix_resize_audio_channels(AgsMachine *machine,
				 guint audio_channels, guint audio_channels_old,
				 gpointer data)
{
  g_message("AgsMatrix only pads can be adjusted");
}

void
ags_matrix_resize_pads(AgsMachine *machine, GType type,
		       guint pads, guint pads_old,
		       gpointer data)
{
  AgsMatrix *matrix;

  gboolean grow;

  matrix = (AgsMatrix *) machine;
  
  /* set size request if needed */
  if(g_type_is_a(type, AGS_TYPE_INPUT)){
    gtk_adjustment_set_upper(gtk_scrollbar_get_adjustment(matrix->cell_pattern->vscrollbar),
			     (double) pads);

    
    if(pads < AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY){
      //      gtk_widget_set_size_request((GtkWidget *) matrix->cell_pattern,
      //			  -1,
      //			  pads * matrix->cell_pattern->cell_height + AGS_LED_DEFAULT_HEIGHT + 2);
    }else if(pads_old < AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY){
      //      gtk_widget_set_size_request((GtkWidget *) matrix->cell_pattern,
      //				  -1,
      //				  AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY * matrix->cell_pattern->cell_height + AGS_LED_DEFAULT_HEIGHT + 2);
    }
  }

  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(g_type_is_a(type, AGS_TYPE_INPUT)){
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_matrix_input_map_recall(matrix,
				    pads_old);
      }
    }else{
      matrix->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_matrix_output_map_recall(matrix,
				     pads_old);
      }
    }else{
      matrix->mapped_output_pad = pads;
    }
  }
}

void
ags_matrix_map_recall(AgsMachine *machine)
{
  AgsMatrix *matrix;
  
  AgsAudio *audio;

  AgsApplicationContext *application_context;

  GList *start_recall, *recall;

  gint position;

  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }
  
  application_context = ags_application_context_get_instance();

  matrix = AGS_MATRIX(machine);
  
  audio = machine->audio;

  position = 0;
  
  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       matrix->playback_play_container, matrix->playback_recall_container,
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

  /* ags-fx-pattern */
  start_recall = ags_fx_factory_create(audio,
				       matrix->pattern_play_container, matrix->pattern_recall_container,
				       "ags-fx-pattern",
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
				       matrix->notation_play_container, matrix->notation_recall_container,
				       "ags-fx-notation",
				       NULL,
				       NULL,
				       0, 0,
				       0, 0,
				       position,
				       (AGS_FX_FACTORY_ADD | AGS_FX_FACTORY_INPUT),
				       0);
  
  recall = start_recall;

  while((recall = ags_recall_template_find_type(recall, AGS_TYPE_FX_NOTATION_AUDIO)) != NULL){
    ags_fx_notation_audio_set_pattern_mode(recall->data,
					   TRUE);

    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-volume */
  start_recall = ags_fx_factory_create(audio,
				       matrix->volume_play_container, matrix->volume_recall_container,
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
				       matrix->envelope_play_container, matrix->envelope_recall_container,
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
				       matrix->buffer_play_container, matrix->buffer_recall_container,
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
  ags_matrix_input_map_recall(matrix,
			      0);

  /* depending on destination */
  ags_matrix_output_map_recall(matrix,
			       0);

  AGS_MACHINE_CLASS(ags_matrix_parent_class)->map_recall(machine);
}

void
ags_matrix_input_map_recall(AgsMatrix *matrix,
			    guint input_pad_start)
{
  AgsAudio *audio;

  GList *start_recall;

  guint input_pads;
  guint audio_channels;
  gint position;

  if(matrix->mapped_input_pad > input_pad_start){
    return;
  }

  audio = AGS_MACHINE(matrix)->audio;

  /* get some fields */
  input_pads = 0;
  audio_channels = 0;
  
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);

  if(audio_channels == 0){
    return;
  }

  position = 0;

  /* ags-fx-playback */
  start_recall = ags_fx_factory_create(audio,
				       matrix->playback_play_container, matrix->playback_recall_container,
				       "ags-fx-playback",
				       NULL,
				       NULL,
				       0, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT),
				       0);

  /* unref */
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-pattern */
  start_recall = ags_fx_factory_create(audio,
				       matrix->pattern_play_container, matrix->pattern_recall_container,
				       "ags-fx-pattern",
				       NULL,
				       NULL,
				       0, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT),
				       0);

  /* unref */
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-notation */
  start_recall = ags_fx_factory_create(audio,
				       matrix->notation_play_container, matrix->notation_recall_container,
				       "ags-fx-notation",
				       NULL,
				       NULL,
				       0, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT),
				       0);

  /* unref */
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-volume */
  start_recall = ags_fx_factory_create(audio,
				       matrix->volume_play_container, matrix->volume_recall_container,
				       "ags-fx-volume",
				       NULL,
				       NULL,
				       0, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT), 0);

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-envelope */
  start_recall = ags_fx_factory_create(audio,
				       matrix->envelope_play_container, matrix->envelope_recall_container,
				       "ags-fx-envelope",
				       NULL,
				       NULL,
				       0, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT),
				       0);

  /* unref */
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);

  /* ags-fx-buffer */
  start_recall = ags_fx_factory_create(audio,
				       matrix->buffer_play_container, matrix->buffer_recall_container,
				       "ags-fx-buffer",
				       NULL,
				       NULL,
				       0, audio_channels,
				       input_pad_start, input_pads,
				       position,
				       (AGS_FX_FACTORY_REMAP | AGS_FX_FACTORY_INPUT),
				       0);

  /* unref */
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
  
  matrix->mapped_input_pad = input_pads;
}

void
ags_matrix_output_map_recall(AgsMatrix *matrix,
			     guint output_pad_start)
{
  AgsAudio *audio;

  guint output_pads;
  guint audio_channels;

  if(matrix->mapped_output_pad > output_pad_start){
    return;
  }

  audio = AGS_MACHINE(matrix)->audio;

  /* get some fields */
  output_pads = 0;
  audio_channels = 0;

  g_object_get(audio,
	       "output-pads", &output_pads,
	       "audio-channels", &audio_channels,
	       NULL);
  
  if(audio_channels == 0){
    return;
  }  
  
  matrix->mapped_output_pad = output_pads;
}

/**
 * ags_matrix_new:
 * @soundcard: the assigned soundcard.
 *
 * Create a new instance of #AgsMatrix
 *
 * Returns: the new #AgsMatrix
 *
 * Since: 3.0.0
 */
AgsMatrix*
ags_matrix_new(GObject *soundcard)
{
  AgsMatrix *matrix;

  matrix = (AgsMatrix *) g_object_new(AGS_TYPE_MATRIX,
				      NULL);
  
  g_object_set(AGS_MACHINE(matrix)->audio,
	       "output-soundcard", soundcard,
	       NULL);
  
  return(matrix);
}
