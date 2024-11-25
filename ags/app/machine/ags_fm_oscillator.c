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

#include <ags/app/machine/ags_fm_oscillator.h>
#include <ags/app/machine/ags_fm_oscillator_callbacks.h>

#include <ags/app/ags_ui_provider.h>

#include <ags/i18n.h>

void ags_fm_oscillator_class_init(AgsFMOscillatorClass *fm_oscillator);
void ags_fm_oscillator_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_fm_oscillator_init(AgsFMOscillator *fm_oscillator);

gboolean ags_fm_oscillator_is_connected(AgsConnectable *connectable);
void ags_fm_oscillator_connect(AgsConnectable *connectable);
void ags_fm_oscillator_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_fm_oscillator
 * @short_description: fm oscillator
 * @title: AgsFMOscillator
 * @section_id:
 * @include: ags/app/machine/ags_fm_oscillator.h
 *
 * The #AgsFMOscillator is a composite widget to act as fm oscillator.
 */

enum{
  CONTROL_CHANGED,
  LAST_SIGNAL,
};

static gpointer ags_fm_oscillator_parent_class = NULL;
static guint fm_oscillator_signals[LAST_SIGNAL];

static AgsConnectableInterface *ags_fm_oscillator_parent_connectable_interface;

GType
ags_fm_oscillator_get_type(void)
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fm_oscillator = 0;

    static const GTypeInfo ags_fm_oscillator_info = {
      sizeof(AgsFMOscillatorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fm_oscillator_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFMOscillator),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fm_oscillator_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_fm_oscillator_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_fm_oscillator = g_type_register_static(GTK_TYPE_FRAME,
						 "AgsFMOscillator",
						 &ags_fm_oscillator_info,
						 0);
    
    g_type_add_interface_static(ags_type_fm_oscillator,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fm_oscillator);
  }

  return g_define_type_id__volatile;
}

void
ags_fm_oscillator_class_init(AgsFMOscillatorClass *fm_oscillator)
{
  ags_fm_oscillator_parent_class = g_type_class_peek_parent(fm_oscillator);

  fm_oscillator->control_changed = NULL;

  /* signals */
  /**
   * AgsFMOscillator::control-change:
   * @fm_oscillator: the #AgsFMOscillator
   *
   * The ::control-change signal notifies about controls modified.
   * 
   * Since: 3.0.0
   */
  fm_oscillator_signals[CONTROL_CHANGED] =
    g_signal_new("control-changed",
		 G_TYPE_FROM_CLASS(fm_oscillator),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsFMOscillatorClass, control_changed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_fm_oscillator_connectable_interface_init(AgsConnectableInterface *connectable)
{
  //  ags_fm_oscillator_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_fm_oscillator_is_connected;  
  connectable->connect = ags_fm_oscillator_connect;
  connectable->disconnect = ags_fm_oscillator_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_fm_oscillator_init(AgsFMOscillator *fm_oscillator)
{
  GtkGrid *grid;
  GtkBox *hbox;
  GtkBox *sync_box;
  GtkLabel *label;

  GtkCellRenderer *cell_renderer;

  GtkListStore *model;
  GtkTreeIter iter;

  AgsConfig *config;

  guint samplerate;
  guint x, y;
  guint i;

  config = ags_config_get_instance();
  
  fm_oscillator->flags = 0;
  fm_oscillator->connectable_flags = 0;
  
  grid = (GtkGrid *) gtk_grid_new();

  gtk_grid_set_column_spacing(grid,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(grid,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);

  gtk_frame_set_child((GtkFrame *) fm_oscillator,
		      (GtkWidget *) grid);

  samplerate = ags_soundcard_helper_config_get_samplerate(config);

  fm_oscillator->selector = (GtkCheckButton *) gtk_check_button_new();

  gtk_grid_attach(grid,
		  (GtkWidget *) fm_oscillator->selector,
		  0, 0,
		  1, 1);
  
  /* wave */
  label = (GtkLabel *) gtk_label_new(i18n("wave"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  1, 0,
		  1, 1);

  fm_oscillator->wave = (GtkComboBox *) gtk_combo_box_text_new();
  gtk_grid_attach(grid,
		  (GtkWidget *) fm_oscillator->wave,
		  2, 0,
		  1, 1);

  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(fm_oscillator->wave),
			     cell_renderer,
			     FALSE); 
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(fm_oscillator->wave),
				 cell_renderer,
				 "text", 0,
				 NULL);

  model = gtk_list_store_new(1, G_TYPE_STRING);

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, "sin",
		     -1);

  /*  gtk_list_store_append(model, &iter);
      gtk_list_store_set(model, &iter,
      0, "cos",
      -1);  */

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, i18n("sawtooth"),
		     -1);

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, i18n("square"),
		     -1);

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, i18n("triangle"),
		     -1);

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, i18n("impulse"),
		     -1);

  gtk_combo_box_set_model(fm_oscillator->wave, GTK_TREE_MODEL(model));
  gtk_combo_box_set_active(fm_oscillator->wave, 0);

  /* other controls */
  label = (GtkLabel *) gtk_label_new(i18n("attack"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  3, 0,
		  1, 1);
  fm_oscillator->attack = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100000.0, 1.0);
  gtk_spin_button_set_value(fm_oscillator->attack, 0.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) fm_oscillator->attack,
		  4, 0,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("length"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  5, 0,
		  1, 1);
  fm_oscillator->frame_count = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100000.0, 1.0);
  gtk_spin_button_set_value(fm_oscillator->frame_count,
			    AGS_FM_OSCILLATOR_DEFAULT_FRAME_COUNT * ((gdouble) samplerate / AGS_FM_OSCILLATOR_DEFAULT_SAMPLERATE));
  gtk_grid_attach(grid, (GtkWidget *) fm_oscillator->frame_count,
		  6, 0,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("phase"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  1, 1,
		  1, 1);
  fm_oscillator->phase = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100000.0, 1.0);
  gtk_spin_button_set_value(fm_oscillator->phase, 0.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) fm_oscillator->phase,
		  2, 1,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("frequency"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  3, 1,
		  1, 1);
  fm_oscillator->frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0, 100000.0, 1.0);
  gtk_spin_button_set_digits(fm_oscillator->frequency,
			     3);
  gtk_spin_button_set_value(fm_oscillator->frequency, 27.5);
  gtk_grid_attach(grid,
		  (GtkWidget *) fm_oscillator->frequency,
		  4, 1,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("volume"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  5, 1,
		  1, 1);
  fm_oscillator->volume = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1.0, 0.1);
  gtk_spin_button_set_digits(fm_oscillator->volume,
			     3);
  gtk_spin_button_set_value(fm_oscillator->volume, 0.2);
  gtk_grid_attach(grid,
		  (GtkWidget *) fm_oscillator->volume,
		  6, 1,
		  1, 1);

  /* do sync */
  fm_oscillator->do_sync = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("sync"));

  gtk_grid_attach(grid,
		  (GtkWidget *) fm_oscillator->do_sync,
		  7, 0,
		  1, 1);

  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);

  gtk_box_set_spacing(hbox,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) hbox,
		  7, 1,
		  1, 1);

  fm_oscillator->sync_point_count = AGS_FM_OSCILLATOR_DEFAULT_SYNC_POINT_COUNT; 
  fm_oscillator->sync_point = (GtkSpinButton **) malloc(2 * fm_oscillator->sync_point_count * sizeof(GtkSpinButton *));

  for(i = 0; i < fm_oscillator->sync_point_count; i++){
    sync_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				      0);

    gtk_box_set_spacing(sync_box,
			AGS_UI_PROVIDER_DEFAULT_SPACING);

    gtk_box_append(hbox,
		   (GtkWidget *) sync_box);
    
    fm_oscillator->sync_point[2 * i] = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100000.0, 1.0);
    gtk_box_append(sync_box,
		   (GtkWidget *) fm_oscillator->sync_point[2 * i]);

    fm_oscillator->sync_point[2 * i + 1] = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100000.0, 1.0);
    gtk_box_append(sync_box,
		   (GtkWidget *) fm_oscillator->sync_point[2 * i + 1]);
  }

  /* LFO wave */
  label = (GtkLabel *) gtk_label_new(i18n("LFO wave"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  1, 2,
		  1, 1);

  fm_oscillator->fm_lfo_wave = (GtkComboBox *) gtk_combo_box_text_new();
  gtk_grid_attach(grid,
		  (GtkWidget *) fm_oscillator->fm_lfo_wave,
		  2, 2,
		  1, 1);

  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(fm_oscillator->fm_lfo_wave),
			     cell_renderer,
			     FALSE); 
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(fm_oscillator->fm_lfo_wave),
				 cell_renderer,
				 "text", 0,
				 NULL);

  model = gtk_list_store_new(1, G_TYPE_STRING);

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, "sin",
		     -1);

  /*  gtk_list_store_append(model, &iter);
      gtk_list_store_set(model, &iter,
      0, "cos",
      -1);  */

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, i18n("sawtooth"),
		     -1);

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, i18n("square"),
		     -1);

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, i18n("triangle"),
		     -1);

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, i18n("impulse"),
		     -1);

  gtk_combo_box_set_model(fm_oscillator->fm_lfo_wave, GTK_TREE_MODEL(model));
  gtk_combo_box_set_active(fm_oscillator->fm_lfo_wave, 0);

  /* LFO controls */
  label = (GtkLabel *) gtk_label_new(i18n("LFO frequency"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  3, 2,
		  1, 1);
  fm_oscillator->fm_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 24.0, 1.0);
  gtk_spin_button_set_digits(fm_oscillator->fm_lfo_frequency,
			     3);
  gtk_spin_button_set_value(fm_oscillator->fm_lfo_frequency, 12.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) fm_oscillator->fm_lfo_frequency,
		  4, 2,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("LFO depth"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  5, 2,
		  1, 1);
  fm_oscillator->fm_lfo_depth = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1.0, 0.01);
  gtk_spin_button_set_digits(fm_oscillator->fm_lfo_depth,
			     3);
  gtk_spin_button_set_value(fm_oscillator->fm_lfo_depth, 1.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) fm_oscillator->fm_lfo_depth,
		  6, 2,
		  1, 1);

  label = (GtkLabel *) gtk_label_new(i18n("LFO tuning"));

  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_START);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  3, 3,
		  1, 1);
  fm_oscillator->fm_tuning = (GtkSpinButton *) gtk_spin_button_new_with_range(-96.0, 96.0, 0.01);
  gtk_spin_button_set_digits(fm_oscillator->fm_tuning,
			     2);
  gtk_spin_button_set_value(fm_oscillator->fm_tuning, 0.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) fm_oscillator->fm_tuning,
		  4, 3,
		  1, 1);
}

gboolean
ags_fm_oscillator_is_connected(AgsConnectable *connectable)
{
  AgsFMOscillator *fm_oscillator;
  
  gboolean is_connected;
  
  fm_oscillator = AGS_FM_OSCILLATOR(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (fm_oscillator->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_fm_oscillator_connect(AgsConnectable *connectable)
{
  AgsFMOscillator *fm_oscillator;

  guint i;
  
  fm_oscillator = AGS_FM_OSCILLATOR(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  fm_oscillator->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  g_signal_connect(G_OBJECT(fm_oscillator->wave), "changed",
		   G_CALLBACK(ags_fm_oscillator_wave_callback), fm_oscillator);
  
  g_signal_connect(G_OBJECT(fm_oscillator->attack), "value-changed",
		   G_CALLBACK(ags_fm_oscillator_attack_callback), fm_oscillator);
  
  g_signal_connect(G_OBJECT(fm_oscillator->frame_count), "value-changed",
		   G_CALLBACK(ags_fm_oscillator_frame_count_callback), fm_oscillator);
  
  g_signal_connect(G_OBJECT(fm_oscillator->frequency), "value-changed",
		   G_CALLBACK(ags_fm_oscillator_frequency_callback), fm_oscillator);

  g_signal_connect(G_OBJECT(fm_oscillator->phase), "value-changed",
		   G_CALLBACK(ags_fm_oscillator_phase_callback), fm_oscillator);

  g_signal_connect(G_OBJECT(fm_oscillator->volume), "value-changed",
		   G_CALLBACK(ags_fm_oscillator_volume_callback), fm_oscillator);

  g_signal_connect(G_OBJECT(fm_oscillator->do_sync), "toggled",
		   G_CALLBACK(ags_fm_oscillator_do_sync_callback), fm_oscillator);

  for(i = 0; i < 2 * fm_oscillator->sync_point_count; i++){
    g_signal_connect(G_OBJECT(fm_oscillator->sync_point[i]), "value-changed",
		     G_CALLBACK(ags_fm_oscillator_sync_point_callback), fm_oscillator);
  }

  g_signal_connect(G_OBJECT(fm_oscillator->fm_lfo_wave), "changed",
		   G_CALLBACK(ags_fm_oscillator_fm_lfo_wave_callback), fm_oscillator);

  g_signal_connect(G_OBJECT(fm_oscillator->fm_lfo_frequency), "value-changed",
		   G_CALLBACK(ags_fm_oscillator_fm_lfo_frequency_callback), fm_oscillator);

  g_signal_connect(G_OBJECT(fm_oscillator->fm_lfo_depth), "value-changed",
		   G_CALLBACK(ags_fm_oscillator_fm_lfo_depth_callback), fm_oscillator);

  g_signal_connect(G_OBJECT(fm_oscillator->fm_tuning), "value-changed",
		   G_CALLBACK(ags_fm_oscillator_fm_tuning_callback), fm_oscillator);
}

void
ags_fm_oscillator_disconnect(AgsConnectable *connectable)
{
  AgsFMOscillator *fm_oscillator;

  guint i;
  
  fm_oscillator = AGS_FM_OSCILLATOR(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  fm_oscillator->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_object_disconnect((GObject *) fm_oscillator->wave,
		      "any_signal::changed",
		      G_CALLBACK(ags_fm_oscillator_wave_callback),
		      (gpointer) fm_oscillator,
		      NULL);
  
  g_object_disconnect((GObject *) fm_oscillator->frame_count,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_fm_oscillator_frame_count_callback),
		      (gpointer) fm_oscillator,
		      NULL);
  g_object_disconnect((GObject *) fm_oscillator->attack,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_fm_oscillator_attack_callback),
		      (gpointer) fm_oscillator,
		      NULL);

  g_object_disconnect((GObject *) fm_oscillator->frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_fm_oscillator_frequency_callback),
		      (gpointer) fm_oscillator,
		      NULL);
  g_object_disconnect((GObject *) fm_oscillator->phase,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_fm_oscillator_phase_callback),
		      (gpointer) fm_oscillator,
		      NULL);

  g_object_disconnect((GObject *) fm_oscillator->volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_fm_oscillator_volume_callback),
		      (gpointer) fm_oscillator,
		      NULL);

  g_object_disconnect((GObject *) fm_oscillator->do_sync,
		      "any_signal::toggled",
		      G_CALLBACK(ags_fm_oscillator_do_sync_callback),
		      (gpointer) fm_oscillator,
		      NULL);

  for(i = 0; i < 2 * fm_oscillator->sync_point_count; i++){
    g_object_disconnect((GObject *) fm_oscillator->sync_point[i],
			"any_signal::value-changed",
			G_CALLBACK(ags_fm_oscillator_sync_point_callback),
			(gpointer) fm_oscillator,
			NULL);
  }

  g_object_disconnect((GObject *) fm_oscillator->fm_lfo_wave,
		      "any_signal::changed",
		      G_CALLBACK(ags_fm_oscillator_fm_lfo_wave_callback),
		      (gpointer) fm_oscillator,
		      NULL);

  g_object_disconnect((GObject *) fm_oscillator->fm_lfo_frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_fm_oscillator_fm_lfo_frequency_callback),
		      (gpointer) fm_oscillator,
		      NULL);

  g_object_disconnect((GObject *) fm_oscillator->fm_lfo_depth,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_fm_oscillator_fm_lfo_depth_callback),
		      (gpointer) fm_oscillator,
		      NULL);

  g_object_disconnect((GObject *) fm_oscillator->fm_tuning,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_fm_oscillator_fm_tuning_callback),
		      (gpointer) fm_oscillator,
		      NULL);
}

/**
 * ags_fm_oscillator_control_changed:
 * @fm_oscillator: the #AgsFMOscillator
 * 
 * The control changed event notifies about changed controls.
 * 
 * Since: 3.0.0
 */
void
ags_fm_oscillator_control_changed(AgsFMOscillator *fm_oscillator)
{
  g_return_if_fail(AGS_IS_FM_OSCILLATOR(fm_oscillator));

  g_object_ref((GObject *) fm_oscillator);
  g_signal_emit(G_OBJECT(fm_oscillator),
		fm_oscillator_signals[CONTROL_CHANGED], 0);
  g_object_unref((GObject *) fm_oscillator);
}

/**
 * ags_fm_oscillator_new:
 *
 * Create a new instance of #AgsFMOscillator
 *
 * Returns: the new #AgsFMOscillator
 *
 * Since: 3.0.0
 */
AgsFMOscillator*
ags_fm_oscillator_new()
{
  AgsFMOscillator *fm_oscillator;

  fm_oscillator = (AgsFMOscillator *) g_object_new(AGS_TYPE_FM_OSCILLATOR,
						   NULL);

  return(fm_oscillator);
}
