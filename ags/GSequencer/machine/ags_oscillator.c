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

#include <ags/GSequencer/machine/ags_oscillator.h>
#include <ags/GSequencer/machine/ags_oscillator_callbacks.h>

#include <ags/i18n.h>

void ags_oscillator_class_init(AgsOscillatorClass *oscillator);
void ags_oscillator_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_oscillator_init(AgsOscillator *oscillator);

void ags_oscillator_connect(AgsConnectable *connectable);
void ags_oscillator_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_oscillator
 * @short_description: oscillator
 * @title: AgsOscillator
 * @section_id:
 * @include: ags/X/machine/ags_oscillator.h
 *
 * The #AgsOscillator is a composite widget to act as oscillator.
 */

enum{
  CONTROL_CHANGED,
  LAST_SIGNAL,
};

static gpointer ags_oscillator_parent_class = NULL;
static guint oscillator_signals[LAST_SIGNAL];

static AgsConnectableInterface *ags_oscillator_parent_connectable_interface;

GType
ags_oscillator_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_oscillator = 0;

    static const GTypeInfo ags_oscillator_info = {
      sizeof(AgsOscillatorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_oscillator_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsOscillator),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_oscillator_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_oscillator_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_oscillator = g_type_register_static(GTK_TYPE_FRAME,
						 "AgsOscillator",
						 &ags_oscillator_info,
						 0);
    
    g_type_add_interface_static(ags_type_oscillator,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_oscillator);
  }

  return g_define_type_id__volatile;
}

void
ags_oscillator_class_init(AgsOscillatorClass *oscillator)
{
  ags_oscillator_parent_class = g_type_class_peek_parent(oscillator);

  oscillator->control_changed = NULL;
  
  /* signals */
  /**
   * AgsOscillator::control-change:
   * @oscillator: the #AgsOscillator
   *
   * The ::control-change signal notifies about controls modified.
   * 
   * Since: 3.0.0
   */
  oscillator_signals[CONTROL_CHANGED] =
    g_signal_new("control-changed",
		 G_TYPE_FROM_CLASS(oscillator),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscillatorClass, control_changed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_oscillator_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_oscillator_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_oscillator_connect;
  connectable->disconnect = ags_oscillator_disconnect;
}

void
ags_oscillator_init(AgsOscillator *oscillator)
{
  GtkGrid *grid;
  GtkBox *hbox;
  GtkBox *sync_box;

  GtkCellRenderer *cell_renderer;

  GtkListStore *model;
  GtkTreeIter iter;

  AgsConfig *config;

  guint samplerate;
  guint i;

  config = ags_config_get_instance();
  
  oscillator->flags = 0;

  grid = (GtkGrid *) gtk_grid_new();
  gtk_container_add((GtkContainer *) oscillator,
		    (GtkWidget *) grid);

  samplerate = ags_soundcard_helper_config_get_samplerate(config);
  
  /* wave */
  gtk_grid_attach(grid,
		  (GtkWidget *) gtk_label_new(i18n("wave")),
		  0, 0,
		  1, 1);

  oscillator->wave = (GtkComboBox *) gtk_combo_box_text_new();
  gtk_grid_attach(grid,
		  (GtkWidget *) oscillator->wave,
		  1, 0,
		  1, 1);

  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(oscillator->wave),
			     cell_renderer,
			     FALSE); 
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(oscillator->wave),
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

  gtk_combo_box_set_model(oscillator->wave, GTK_TREE_MODEL(model));
  gtk_combo_box_set_active(oscillator->wave, 0);

  /* attack */
  gtk_grid_attach(grid,
		  (GtkWidget *) gtk_label_new(i18n("attack")),
		  2, 0,
		  1, 1);

  oscillator->attack = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100000.0, 1.0);
  gtk_spin_button_set_value(oscillator->attack, 0.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) oscillator->attack,
		  3, 0,
		  1, 1);

  /* length */
  gtk_grid_attach(grid,
		  (GtkWidget *) gtk_label_new(i18n("length")),
		  4, 0,
		  1, 1);
  
  oscillator->frame_count = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100000.0, 1.0);
  gtk_spin_button_set_value(oscillator->frame_count,
			    AGS_OSCILLATOR_DEFAULT_FRAME_COUNT * ((gdouble) samplerate / AGS_OSCILLATOR_DEFAULT_SAMPLERATE));
  gtk_grid_attach(grid,
		  (GtkWidget *) oscillator->frame_count, 5, 0,
		  1, 1);

  /* phase */
  gtk_grid_attach(grid,
		  (GtkWidget *) gtk_label_new(i18n("phase")),
		  0, 1,
		  1, 1);
  
  oscillator->phase = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100000.0, 1.0);
  gtk_spin_button_set_value(oscillator->phase, 0.0);
  gtk_grid_attach(grid,
		  (GtkWidget *) oscillator->phase,
		  1, 1,
		  1, 1);

  /* frequency */
  gtk_grid_attach(grid,
		  (GtkWidget *) gtk_label_new(i18n("frequency")),
		  2, 1,
		  1, 1);
  
  oscillator->frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0, 100000.0, 1.0);
  gtk_spin_button_set_digits(oscillator->frequency,
			     3);
  gtk_spin_button_set_value(oscillator->frequency, 27.5);
  gtk_grid_attach(grid,
		  (GtkWidget *) oscillator->frequency,
		  3, 1,
		  1, 1);

  /* volume */
  gtk_grid_attach(grid,
		  (GtkWidget *) gtk_label_new(i18n("volume")),
		  4, 1,
		  1, 1);
  
  oscillator->volume = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1.0, 0.1);
  gtk_spin_button_set_digits(oscillator->volume,
			     3);
  gtk_spin_button_set_value(oscillator->volume, 0.2);
  gtk_grid_attach(grid,
		  (GtkWidget *) oscillator->volume,
		  5, 1,
		  1, 1);

  /* do sync */
  oscillator->do_sync = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("sync"));
  gtk_grid_attach(grid,
		  (GtkWidget *) oscillator->do_sync,
		  6, 0,
		  1, 1);
  
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);
  gtk_grid_attach(grid,
		  (GtkWidget *) hbox,
		  6, 1,
		  1, 1);

  oscillator->sync_point_count = AGS_OSCILLATOR_DEFAULT_SYNC_POINT_COUNT; 
  oscillator->sync_point = (GtkSpinButton **) malloc(2 * oscillator->sync_point_count * sizeof(GtkSpinButton *));

  for(i = 0; i < oscillator->sync_point_count; i++){
    sync_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				      0);
    gtk_box_pack_start(hbox,
		       (GtkWidget *) sync_box,
		       FALSE, FALSE,
		       0);
    
    oscillator->sync_point[2 * i] = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100000.0, 1.0);
    gtk_box_pack_start(sync_box,
		       (GtkWidget *) oscillator->sync_point[2 * i],
		       FALSE, FALSE,
		       0);

    oscillator->sync_point[2 * i + 1] = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100000.0, 1.0);
    gtk_box_pack_start(sync_box,
		       (GtkWidget *) oscillator->sync_point[2 * i + 1],
		       FALSE, FALSE,
		       0);
  }
}

void
ags_oscillator_connect(AgsConnectable *connectable)
{
  AgsOscillator *oscillator;

  guint i;
  
  oscillator = AGS_OSCILLATOR(connectable);

  if((AGS_OSCILLATOR_CONNECTED & (oscillator->flags)) != 0){
    return;
  }

  oscillator->flags |= AGS_OSCILLATOR_CONNECTED;

  g_signal_connect(G_OBJECT(oscillator->wave), "changed",
		   G_CALLBACK(ags_oscillator_wave_callback), oscillator);
  
  g_signal_connect(G_OBJECT(oscillator->attack), "value-changed",
		   G_CALLBACK(ags_oscillator_attack_callback), oscillator);
  
  g_signal_connect(G_OBJECT(oscillator->frame_count), "value-changed",
		   G_CALLBACK(ags_oscillator_frame_count_callback), oscillator);
  
  g_signal_connect(G_OBJECT(oscillator->frequency), "value-changed",
		   G_CALLBACK(ags_oscillator_frequency_callback), oscillator);

  g_signal_connect(G_OBJECT(oscillator->phase), "value-changed",
		   G_CALLBACK(ags_oscillator_phase_callback), oscillator);

  g_signal_connect(G_OBJECT(oscillator->volume), "value-changed",
		   G_CALLBACK(ags_oscillator_volume_callback), oscillator);

  g_signal_connect(G_OBJECT(oscillator->do_sync), "toggled",
		   G_CALLBACK(ags_oscillator_do_sync_callback), oscillator);

  for(i = 0; i < 2 * oscillator->sync_point_count; i++){
    g_signal_connect(G_OBJECT(oscillator->sync_point[i]), "value-changed",
		     G_CALLBACK(ags_oscillator_sync_point_callback), oscillator);
  }
}

void
ags_oscillator_disconnect(AgsConnectable *connectable)
{
  AgsOscillator *oscillator;

  guint i;
  
  oscillator = AGS_OSCILLATOR(connectable);

  if((AGS_OSCILLATOR_CONNECTED & (oscillator->flags)) == 0){
    return;
  }

  oscillator->flags &= (~AGS_OSCILLATOR_CONNECTED);
  
  g_object_disconnect((GObject *) oscillator->wave,
		      "any_signal::changed",
		      G_CALLBACK(ags_oscillator_wave_callback),
		      (gpointer) oscillator,
		      NULL);
  
  g_object_disconnect((GObject *) oscillator->frame_count,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_oscillator_frame_count_callback),
		      (gpointer) oscillator,
		      NULL);
  g_object_disconnect((GObject *) oscillator->attack,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_oscillator_attack_callback),
		      (gpointer) oscillator,
		      NULL);

  g_object_disconnect((GObject *) oscillator->frequency,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_oscillator_frequency_callback),
		      (gpointer) oscillator,
		      NULL);
  g_object_disconnect((GObject *) oscillator->phase,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_oscillator_phase_callback),
		      (gpointer) oscillator,
		      NULL);

  g_object_disconnect((GObject *) oscillator->volume,
		      "any_signal::value-changed",
		      G_CALLBACK(ags_oscillator_volume_callback),
		      (gpointer) oscillator,
		      NULL);

  g_object_disconnect((GObject *) oscillator->do_sync,
		      "any_signal::toggled",
		      G_CALLBACK(ags_oscillator_do_sync_callback),
		      (gpointer) oscillator,
		      NULL);

  for(i = 0; i < 2 * oscillator->sync_point_count; i++){
    g_object_disconnect((GObject *) oscillator->sync_point[i],
			"any_signal::value-changed",
			G_CALLBACK(ags_oscillator_sync_point_callback),
			(gpointer) oscillator,
			NULL);
  }
}

/**
 * ags_oscillator_control_changed:
 * @oscillator: the #AgsOscillator
 * 
 * The control changed event notifies about changed controls.
 * 
 * Since: 3.0.0
 */
void
ags_oscillator_control_changed(AgsOscillator *oscillator)
{
  g_return_if_fail(AGS_IS_OSCILLATOR(oscillator));

  g_object_ref((GObject *) oscillator);
  g_signal_emit(G_OBJECT(oscillator),
		oscillator_signals[CONTROL_CHANGED], 0);
  g_object_unref((GObject *) oscillator);
}

/**
 * ags_oscillator_new:
 *
 * Create a new instance of #AgsOscillator
 *
 * Returns: the new #AgsOscillator
 *
 * Since: 3.0.0
 */
AgsOscillator*
ags_oscillator_new()
{
  AgsOscillator *oscillator;

  oscillator = (AgsOscillator *) g_object_new(AGS_TYPE_OSCILLATOR,
					      NULL);

  return(oscillator);
}
