/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/X/machine/ags_fm_oscillator.h>
#include <ags/X/machine/ags_fm_oscillator_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/i18n.h>

void ags_fm_oscillator_class_init(AgsFMOscillatorClass *fm_oscillator);
void ags_fm_oscillator_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_fm_oscillator_init(AgsFMOscillator *fm_oscillator);

void ags_fm_oscillator_connect(AgsConnectable *connectable);
void ags_fm_oscillator_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_fm_oscillator
 * @short_description: fm oscillator
 * @title: AgsFMOscillator
 * @section_id:
 * @include: ags/X/machine/ags_fm_oscillator.h
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
  static volatile gsize g_define_type_id__volatile = 0;

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
  GParamSpec *param_spec;

  ags_fm_oscillator_parent_class = g_type_class_peek_parent(fm_oscillator);

  /* signals */
  /**
   * AgsFMOscillator::control-change:
   * @fm_oscillator: the #AgsFMOscillator
   *
   * The ::control-change signal notifies about controls modified.
   * 
   * Since: 2.3.0
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
  AgsConnectableInterface *ags_fm_oscillator_connectable_parent_interface;

  ags_fm_oscillator_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_fm_oscillator_connect;
  connectable->disconnect = ags_fm_oscillator_disconnect;
}

void
ags_fm_oscillator_init(AgsFMOscillator *fm_oscillator)
{
  GtkTable *table;
  GtkHBox *hbox;
  GtkHBox *sync_box;

  GtkCellRenderer *cell_renderer;

  GtkListStore *model;
  GtkTreeIter iter;

  guint i;

  fm_oscillator->flags = 0;
  
  table = (GtkTable *) gtk_table_new(8, 4,
				     FALSE);
  gtk_container_add((GtkContainer *) fm_oscillator,
		    (GtkWidget *) table);

  gtk_table_attach_defaults(table,
			    (GtkWidget *) gtk_label_new(i18n("wave")),
			    0, 1,
			    0, 1);

  /* wave */
  fm_oscillator->wave = (GtkComboBox *) gtk_combo_box_text_new();
  gtk_table_attach_defaults(table,
			    (GtkWidget *) fm_oscillator->wave,
			    1, 2,
			    0, 1);

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
  gtk_table_attach_defaults(table,
			    (GtkWidget *) gtk_label_new(i18n("attack")),
			    2, 3,
			    0, 1);
  fm_oscillator->attack = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100000.0, 1.0);
  fm_oscillator->attack->adjustment->value = 0.0;
  gtk_table_attach_defaults(table,
			    (GtkWidget *) fm_oscillator->attack,
			    3, 4,
			    0, 1);

  gtk_table_attach_defaults(table,
			    (GtkWidget *) gtk_label_new(i18n("length")),
			    4, 5,
			    0, 1);
  fm_oscillator->frame_count = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100000.0, 1.0);
  fm_oscillator->frame_count->adjustment->value = AGS_FM_OSCILLATOR_DEFAULT_FRAME_COUNT;
  gtk_table_attach_defaults(table, (GtkWidget *) fm_oscillator->frame_count, 5, 6, 0, 1);

  gtk_table_attach_defaults(table,
			    (GtkWidget *) gtk_label_new(i18n("phase")),
			    0, 1,
			    1, 2);
  fm_oscillator->phase = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100000.0, 1.0);
  fm_oscillator->phase->adjustment->value = 0.0;
  gtk_table_attach_defaults(table, (GtkWidget *) fm_oscillator->phase, 1, 2, 1, 2);

  gtk_table_attach_defaults(table,
			    (GtkWidget *) gtk_label_new(i18n("frequency")),
			    2, 3,
			    1, 2);
  fm_oscillator->frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0, 100000.0, 1.0);
  gtk_spin_button_set_digits(fm_oscillator->frequency,
			     3);
  fm_oscillator->frequency->adjustment->value = 27.5;
  gtk_table_attach_defaults(table,
			    (GtkWidget *) fm_oscillator->frequency,
			    3, 4,
			    1, 2);

  gtk_table_attach_defaults(table,
			    (GtkWidget *) gtk_label_new(i18n("volume")),
			    4, 5, 1, 2);
  fm_oscillator->volume = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1.0, 0.1);
  gtk_spin_button_set_digits(fm_oscillator->volume,
			     3);
  fm_oscillator->volume->adjustment->value = 0.2;
  gtk_table_attach_defaults(table, (GtkWidget *) fm_oscillator->volume, 5, 6, 1, 2);

  /* do sync */
  fm_oscillator->do_sync = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("sync"));
  gtk_table_attach_defaults(table,
			    (GtkWidget *) fm_oscillator->do_sync,
			    6, 7,
			    0, 1);

  hbox = (GtkHBox *) gtk_hbox_new(TRUE,
				 0);
  gtk_table_attach_defaults(table,
			    (GtkWidget *) hbox,
			    6, 7,
			    1, 2);

  fm_oscillator->sync_point_count = AGS_FM_OSCILLATOR_DEFAULT_SYNC_POINT_COUNT; 
  fm_oscillator->sync_point = (GtkSpinButton **) malloc(2 * fm_oscillator->sync_point_count * sizeof(GtkSpinButton *));

  for(i = 0; i < fm_oscillator->sync_point_count; i++){
    sync_box = (GtkHBox *) gtk_hbox_new(TRUE,
					0);
    gtk_box_pack_start((GtkBox *) hbox,
		       (GtkWidget *) sync_box,
		       FALSE, FALSE,
		       0);
    
    fm_oscillator->sync_point[2 * i] = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100000.0, 1.0);
    gtk_box_pack_start((GtkBox *) sync_box,
		       (GtkWidget *) fm_oscillator->sync_point[2 * i],
		       FALSE, FALSE,
		       0);

    fm_oscillator->sync_point[2 * i + 1] = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100000.0, 1.0);
    gtk_box_pack_start((GtkBox *) sync_box,
		       (GtkWidget *) fm_oscillator->sync_point[2 * i + 1],
		       FALSE, FALSE,
		       0);
  }

  /* LFO wave */
  gtk_table_attach_defaults(table,
			    (GtkWidget *) gtk_label_new(i18n("LFO wave")),
			    0, 1,
			    2, 3);

  fm_oscillator->fm_lfo_wave = (GtkComboBox *) gtk_combo_box_text_new();
  gtk_table_attach_defaults(table,
			    (GtkWidget *) fm_oscillator->fm_lfo_wave,
			    1, 2,
			    2, 3);

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
  gtk_table_attach_defaults(table,
			    (GtkWidget *) gtk_label_new(i18n("LFO frequency")),
			    2, 3,
			    2, 3);
  fm_oscillator->fm_lfo_frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 24.0, 1.0);
  gtk_spin_button_set_digits(fm_oscillator->fm_lfo_frequency,
			     3);
  fm_oscillator->fm_lfo_frequency->adjustment->value = 12.0;
  gtk_table_attach_defaults(table,
			    (GtkWidget *) fm_oscillator->fm_lfo_frequency,
			    3, 4,
			    2, 3);

  gtk_table_attach_defaults(table,
			    (GtkWidget *) gtk_label_new(i18n("LFO depth")),
			    4, 5,
			    2, 3);
  fm_oscillator->fm_lfo_depth = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1.0, 0.01);
  gtk_spin_button_set_digits(fm_oscillator->fm_lfo_depth,
			     3);
  fm_oscillator->fm_lfo_depth->adjustment->value = 1.0;
  gtk_table_attach_defaults(table,
			    (GtkWidget *) fm_oscillator->fm_lfo_depth,
			    5, 6,
			    2, 3);

  gtk_table_attach_defaults(table,
			    (GtkWidget *) gtk_label_new(i18n("LFO tuning")),
			    2, 3,
			    3, 4);
  fm_oscillator->fm_tuning = (GtkSpinButton *) gtk_spin_button_new_with_range(-96.0, 96.0, 0.01);
  gtk_spin_button_set_digits(fm_oscillator->fm_tuning,
			     2);
  fm_oscillator->fm_tuning->adjustment->value = 0.0;
  gtk_table_attach_defaults(table,
			    (GtkWidget *) fm_oscillator->fm_tuning,
			    3, 4,
			    3, 4);
}

void
ags_fm_oscillator_connect(AgsConnectable *connectable)
{
  AgsFMOscillator *fm_oscillator;

  guint i;
  
  fm_oscillator = AGS_FM_OSCILLATOR(connectable);

  if((AGS_FM_OSCILLATOR_CONNECTED & (fm_oscillator->flags)) != 0){
    return;
  }

  fm_oscillator->flags |= AGS_FM_OSCILLATOR_CONNECTED;

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

  if((AGS_FM_OSCILLATOR_CONNECTED & (fm_oscillator->flags)) == 0){
    return;
  }

  fm_oscillator->flags &= (~AGS_FM_OSCILLATOR_CONNECTED);
  
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

void
ags_file_read_fm_oscillator(AgsFile *file, xmlNode *node, AgsFMOscillator **fm_oscillator)
{
  AgsFMOscillator *gobject;
  xmlChar *wave;

  if(*fm_oscillator == NULL){
    gobject = (AgsFMOscillator *) g_object_new(AGS_TYPE_FM_OSCILLATOR,
					     NULL);
    *fm_oscillator = gobject;
  }else{
    gobject = *fm_oscillator;
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference", gobject,
				   NULL));

  wave = (xmlChar *) xmlGetProp(node,
				"wave");

  if(!xmlStrncmp(wave,
		 "sin",
		 4)){
    gtk_combo_box_set_active(gobject->wave,
			     0);
  }else if(!xmlStrncmp(wave,
		       "sawtooth",
		       9)){
    gtk_combo_box_set_active(gobject->wave,
			     1);
  }else if(!xmlStrncmp(wave,
		       "square",
		       7)){
    gtk_combo_box_set_active(gobject->wave,
			     2);
  }else if(!xmlStrncmp(wave,
		       "triangle",
		       9)){
    gtk_combo_box_set_active(gobject->wave,
			     3);
  }else if(!xmlStrncmp(wave,
		       "impulse",
		       8)){
    gtk_combo_box_set_active(gobject->wave,
			     4);
  }

  gtk_spin_button_set_value(gobject->attack,
			    g_ascii_strtod(xmlGetProp(node,
						      "attack"),
					   NULL));

  gtk_spin_button_set_value(gobject->frame_count,
			    g_ascii_strtod(xmlGetProp(node,
						      "frame-count"),
					   NULL));
  
  gtk_spin_button_set_value(gobject->frequency,
			    g_ascii_strtod(xmlGetProp(node,
						      "frequency"),
					   NULL));

  gtk_spin_button_set_value(gobject->phase,
			    g_ascii_strtod(xmlGetProp(node,
						      "phase"),
					   NULL));
  
  gtk_spin_button_set_value(gobject->volume,
			    g_ascii_strtod(xmlGetProp(node,
						      "volume"),
					   NULL));
  
  /* LFO */
  wave = (xmlChar *) xmlGetProp(node,
				"fm-lfo-wave");

  if(!xmlStrncmp(wave,
		 "sin",
		 4)){
    gtk_combo_box_set_active(gobject->fm_lfo_wave,
			     0);
  }else if(!xmlStrncmp(wave,
		       "sawtooth",
		       9)){
    gtk_combo_box_set_active(gobject->fm_lfo_wave,
			     1);
  }else if(!xmlStrncmp(wave,
		       "square",
		       7)){
    gtk_combo_box_set_active(gobject->fm_lfo_wave,
			     2);
  }else if(!xmlStrncmp(wave,
		       "triangle",
		       9)){
    gtk_combo_box_set_active(gobject->fm_lfo_wave,
			     3);
  }else if(!xmlStrncmp(wave,
		       "impulse",
		       8)){
    gtk_combo_box_set_active(gobject->fm_lfo_wave,
			     4);
  }

  gtk_spin_button_set_value(gobject->fm_lfo_frequency,
			    g_ascii_strtod(xmlGetProp(node,
						      "fm-lfo-frequency"),
					   NULL));

  gtk_spin_button_set_value(gobject->fm_lfo_depth,
			    g_ascii_strtod(xmlGetProp(node,
						      "fm-lfo-depth"),
					   NULL));

  gtk_spin_button_set_value(gobject->fm_tuning,
			    g_ascii_strtod(xmlGetProp(node,
						      "fm-tuning"),
					   NULL));
}

xmlNode*
ags_file_write_fm_oscillator(AgsFile *file, xmlNode *parent, AgsFMOscillator *fm_oscillator)
{
  xmlNode *node;
  gchar *id;

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-fm_oscillator");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", fm_oscillator,
				   NULL));

  xmlNewProp(node,
	     "wave",
	     gtk_combo_box_text_get_active_text((GtkComboBoxText *) fm_oscillator->wave));

  xmlNewProp(node,
	     "attack",
	     g_strdup_printf("%f", fm_oscillator->attack->adjustment->value));

  xmlNewProp(node,
	     "frame-count",
	     g_strdup_printf("%f", fm_oscillator->frame_count->adjustment->value));

  xmlNewProp(node,
	     "frequency",
	     g_strdup_printf("%f", fm_oscillator->frequency->adjustment->value));

  xmlNewProp(node,
	     "phase",
	     g_strdup_printf("%f", fm_oscillator->phase->adjustment->value));

  xmlNewProp(node,
	     "volume",
	     g_strdup_printf("%f", fm_oscillator->volume->adjustment->value));

  /* LFO */
  xmlNewProp(node,
	     "fm-lfo-wave",
	     gtk_combo_box_text_get_active_text((GtkComboBoxText *) fm_oscillator->fm_lfo_wave));
  
  xmlNewProp(node,
	     "fm-lfo-frequency",
	     g_strdup_printf("%f", fm_oscillator->fm_lfo_frequency->adjustment->value));
  
  xmlNewProp(node,
	     "fm-lfo-depth",
	     g_strdup_printf("%f", fm_oscillator->fm_lfo_depth->adjustment->value));
  
  xmlNewProp(node,
	     "fm-tuning",
	     g_strdup_printf("%f", fm_oscillator->fm_tuning->adjustment->value));

  xmlAddChild(parent,
	      node);  

  return(node);
}

/**
 * ags_fm_oscillator_control_changed:
 * @fm_oscillator: the #AgsFMOscillator
 * 
 * The control changed event notifies about changed controls.
 * 
 * Since: 2.3.0
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
 * Since: 2.3.0
 */
AgsFMOscillator*
ags_fm_oscillator_new()
{
  AgsFMOscillator *fm_oscillator;

  fm_oscillator = (AgsFMOscillator *) g_object_new(AGS_TYPE_FM_OSCILLATOR,
						   NULL);

  return(fm_oscillator);
}
