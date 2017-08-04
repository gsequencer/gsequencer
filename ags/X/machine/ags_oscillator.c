/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/machine/ags_oscillator.h>
#include <ags/X/machine/ags_oscillator_callbacks.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>

#include <ags/object/ags_plugin.h>

#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>

#include <ags/i18n.h>

void ags_oscillator_class_init(AgsOscillatorClass *oscillator);
void ags_oscillator_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_oscillator_init(AgsOscillator *oscillator);
void ags_oscillator_connect(AgsConnectable *connectable);
void ags_oscillator_disconnect(AgsConnectable *connectable);
void ags_oscillator_destroy(GtkObject *object);
void ags_oscillator_show(GtkWidget *widget);

/**
 * SECTION:ags_oscillator
 * @short_description: oscillator
 * @title: AgsOscillator
 * @section_id:
 * @include: ags/X/machine/ags_oscillator.h
 *
 * The #AgsOscillator is a composite widget to act as oscillator.
 */

static AgsConnectableInterface *ags_oscillator_parent_connectable_interface;

GType
ags_oscillator_get_type(void)
{
  static GType ags_type_oscillator = 0;

  if(!ags_type_oscillator){
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
  }

  return(ags_type_oscillator);
}

void
ags_oscillator_class_init(AgsOscillatorClass *oscillator)
{
}

void
ags_oscillator_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_oscillator_connectable_parent_interface;

  ags_oscillator_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_oscillator_connect;
  connectable->disconnect = ags_oscillator_disconnect;
}

void
ags_oscillator_init(AgsOscillator *oscillator)
{
  GtkTable *table;
  GtkCellRenderer *cell_renderer;
  GtkListStore *model;
  GtkTreeIter iter;

  table = (GtkTable *) gtk_table_new(8, 2, FALSE);
  gtk_container_add((GtkContainer *) oscillator, (GtkWidget *) table);

  gtk_table_attach_defaults(table,
			    (GtkWidget *) gtk_label_new(i18n("wave")),
			    0, 1, 0, 1);

  /* wave */
  oscillator->wave = (GtkComboBox *) gtk_combo_box_text_new();
  gtk_table_attach_defaults(table,
			    (GtkWidget *) oscillator->wave,
			    1, 2, 0, 1);

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

  gtk_combo_box_set_model(oscillator->wave, GTK_TREE_MODEL(model));
  gtk_combo_box_set_active(oscillator->wave, 0);

  /* other controls */
  gtk_table_attach_defaults(table,
			    (GtkWidget *) gtk_label_new(i18n("attack")),
			    2, 3, 0, 1);
  oscillator->attack = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100000.0, 1.0);
  oscillator->attack->adjustment->value = 0.0;
  gtk_table_attach_defaults(table, (GtkWidget *) oscillator->attack, 3, 4, 0, 1);

  gtk_table_attach_defaults(table,
			    (GtkWidget *) gtk_label_new(i18n("length")),
			    4, 5, 0, 1);
  oscillator->frame_count = (GtkSpinButton *) gtk_spin_button_new_with_range(44100.0 / 27.5, 100000.0, 1.0);
  oscillator->frame_count->adjustment->value = 3200;
  gtk_table_attach_defaults(table, (GtkWidget *) oscillator->frame_count, 5, 6, 0, 1);

  gtk_table_attach_defaults(table,
			    (GtkWidget *) gtk_label_new(i18n("phase")),
			    0, 1, 1, 2);
  oscillator->phase = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 100000.0, 1.0);
  oscillator->phase->adjustment->value = 0.0;
  gtk_table_attach_defaults(table, (GtkWidget *) oscillator->phase, 1, 2, 1, 2);

  gtk_table_attach_defaults(table,
			    (GtkWidget *) gtk_label_new(i18n("frequency")),
			    2, 3, 1, 2);
  oscillator->frequency = (GtkSpinButton *) gtk_spin_button_new_with_range(8.0, 100000.0, 1.0);
  gtk_spin_button_set_digits(oscillator->frequency,
			     3);
  oscillator->frequency->adjustment->value = 27.5;
  gtk_table_attach_defaults(table,
			    (GtkWidget *) oscillator->frequency,
			    3, 4,
			    1, 2);

  gtk_table_attach_defaults(table,
			    (GtkWidget *) gtk_label_new(i18n("volume")),
			    4, 5, 1, 2);
  oscillator->volume = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1.0, 0.1);
  gtk_spin_button_set_digits(oscillator->volume,
			     3);
  oscillator->volume->adjustment->value = 0.2;
  gtk_table_attach_defaults(table, (GtkWidget *) oscillator->volume, 5, 6, 1, 2);
}

void
ags_oscillator_connect(AgsConnectable *connectable)
{
  AgsOscillator *oscillator;

  oscillator = AGS_OSCILLATOR(connectable);

  oscillator->wave_handler = g_signal_connect(G_OBJECT(oscillator->wave), "changed",
					      G_CALLBACK(ags_oscillator_wave_callback), oscillator);

  oscillator->attack_handler = g_signal_connect(G_OBJECT(oscillator->attack), "value-changed",
						G_CALLBACK(ags_oscillator_attack_callback), oscillator);
  
  oscillator->frame_count_handler = g_signal_connect(G_OBJECT(oscillator->frame_count), "value-changed",
						     G_CALLBACK(ags_oscillator_frame_count_callback), oscillator);

  oscillator->frequency_handler = g_signal_connect(G_OBJECT(oscillator->frequency), "value-changed",
						   G_CALLBACK(ags_oscillator_frequency_callback), oscillator);

  oscillator->phase_handler = g_signal_connect(G_OBJECT(oscillator->phase), "value-changed",
					       G_CALLBACK(ags_oscillator_phase_callback), oscillator);

  oscillator->volume_handler = g_signal_connect(G_OBJECT(oscillator->volume), "value-changed",
						G_CALLBACK(ags_oscillator_volume_callback), oscillator);
}

void
ags_oscillator_disconnect(AgsConnectable *connectable)
{
  AgsOscillator *oscillator;

  oscillator = AGS_OSCILLATOR(connectable);

  g_signal_handler_disconnect(G_OBJECT(oscillator->wave), oscillator->wave_handler);
  g_signal_handler_disconnect(G_OBJECT(oscillator->attack), oscillator->attack_handler);
  g_signal_handler_disconnect(G_OBJECT(oscillator->frame_count), oscillator->frame_count_handler);
  g_signal_handler_disconnect(G_OBJECT(oscillator->frequency), oscillator->frequency_handler);
  g_signal_handler_disconnect(G_OBJECT(oscillator->phase), oscillator->phase_handler);
  g_signal_handler_disconnect(G_OBJECT(oscillator->volume), oscillator->volume_handler);
}

void
ags_oscillator_destroy(GtkObject *object)
{
}

void
ags_oscillator_show(GtkWidget *widget)
{
}

void
ags_file_read_oscillator(AgsFile *file, xmlNode *node, AgsOscillator **oscillator)
{
  AgsOscillator *gobject;
  xmlChar *wave;

  if(*oscillator == NULL){
    gobject = (AgsOscillator *) g_object_new(AGS_TYPE_OSCILLATOR,
					     NULL);
    *oscillator = gobject;
  }else{
    gobject = *oscillator;
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
}

xmlNode*
ags_file_write_oscillator(AgsFile *file, xmlNode *parent, AgsOscillator *oscillator)
{
  xmlNode *node;
  gchar *id;

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-oscillator");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", oscillator,
				   NULL));

  xmlNewProp(node,
	     "wave",
	     gtk_combo_box_text_get_active_text((GtkComboBoxText *) oscillator->wave));

  xmlNewProp(node,
	     "attack",
	     g_strdup_printf("%f", oscillator->attack->adjustment->value));

  xmlNewProp(node,
	     "frame-count",
	     g_strdup_printf("%f", oscillator->frame_count->adjustment->value));

  xmlNewProp(node,
	     "frequency",
	     g_strdup_printf("%f", oscillator->frequency->adjustment->value));

  xmlNewProp(node,
	     "phase",
	     g_strdup_printf("%f", oscillator->phase->adjustment->value));

  xmlNewProp(node,
	     "volume",
	     g_strdup_printf("%f", oscillator->volume->adjustment->value));

  xmlAddChild(parent,
	      node);  

  return(node);
}

/**
 * ags_oscillator_new:
 *
 * Creates an #AgsOscillator
 *
 * Returns: a new #AgsOscillator
 *
 * Since: 0.3
 */
AgsOscillator*
ags_oscillator_new()
{
  AgsOscillator *oscillator;

  oscillator = (AgsOscillator *) g_object_new(AGS_TYPE_OSCILLATOR, NULL);

  return(oscillator);
}
