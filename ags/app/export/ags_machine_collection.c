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

#include <ags/app/export/ags_machine_collection.h>
#include <ags/app/export/ags_machine_collection_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <ags/app/export/ags_midi_export_wizard.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_machine_collection_class_init(AgsMachineCollectionClass *machine_collection);
void ags_machine_collection_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_machine_collection_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_machine_collection_init(AgsMachineCollection *machine_collection);

void ags_machine_collection_connect(AgsConnectable *connectable);
void ags_machine_collection_disconnect(AgsConnectable *connectable);

void ags_machine_collection_set_update(AgsApplicable *applicable, gboolean update);
void ags_machine_collection_apply(AgsApplicable *applicable);
void ags_machine_collection_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_machine_collection
 * @short_description: Pack machine entry
 * @title: AgsMachineCollection
 * @section_id:
 * @include: ags/app/ags_machine_collection.h
 *
 * #AgsMachineCollection is a composite widget which packs
 * several #AgsMachineCollectionEntry widgets.
 */

enum{
  PROP_0,
};

static gpointer ags_machine_collection_parent_class = NULL;

GType
ags_machine_collection_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_machine_collection = 0;

    static const GTypeInfo ags_machine_collection_info = {
      sizeof (AgsMachineCollectionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_machine_collection_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMachineCollection),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_machine_collection_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_machine_collection_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_machine_collection_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_machine_collection = g_type_register_static(GTK_TYPE_BOX,
							 "AgsMachineCollection", &ags_machine_collection_info,
							 0);
    
    g_type_add_interface_static(ags_type_machine_collection,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_machine_collection,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_machine_collection);
  }

  return g_define_type_id__volatile;
}

void
ags_machine_collection_class_init(AgsMachineCollectionClass *machine_collection)
{  
  ags_machine_collection_parent_class = g_type_class_peek_parent(machine_collection);

  /* properties */
}

void
ags_machine_collection_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_machine_collection_connect;
  connectable->disconnect = ags_machine_collection_disconnect;
}

void
ags_machine_collection_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_machine_collection_set_update;
  applicable->apply = ags_machine_collection_apply;
  applicable->reset = ags_machine_collection_reset;
}

void
ags_machine_collection_init(AgsMachineCollection *machine_collection)
{
  GtkScrolledWindow *scrolled_window;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(machine_collection),
				 GTK_ORIENTATION_VERTICAL);
  
  gtk_box_set_spacing((GtkBox *) machine_collection,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  machine_collection->flags = 0;

  machine_collection->machine_mapper_type = G_TYPE_NONE;

  machine_collection->machine_mapper_n_properties = 0;

  machine_collection->machine_mapper_strv = NULL;
  machine_collection->machine_mapper_value = NULL;
  
  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();
  
  gtk_widget_set_halign((GtkWidget *) scrolled_window,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign((GtkWidget *) scrolled_window,
			GTK_ALIGN_FILL);

  gtk_widget_set_hexpand((GtkWidget *) scrolled_window,
			 TRUE);
  gtk_widget_set_vexpand((GtkWidget *) scrolled_window,
			 TRUE);
  
  gtk_box_append((GtkBox *) machine_collection,
		 (GtkWidget *) scrolled_window);
  
  machine_collection->machine_mapper_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
								  AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_scrolled_window_set_child(scrolled_window,
				(GtkWidget *) machine_collection->machine_mapper_box);
}

void
ags_machine_collection_connect(AgsConnectable *connectable)
{
  AgsMachineCollection *machine_collection;

  GList *start_list, *list;
  
  machine_collection = AGS_MACHINE_COLLECTION(connectable);

  if((AGS_MACHINE_COLLECTION_CONNECTED & (machine_collection->flags)) != 0){
    return;
  }

  machine_collection->flags |= AGS_MACHINE_COLLECTION_CONNECTED;
  
  /* children */
  list =
    start_list = ags_machine_collection_get_machine_mapper(machine_collection);

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free(start_list);
}

void
ags_machine_collection_disconnect(AgsConnectable *connectable)
{
  AgsMachineCollection *machine_collection;

  GList *start_list, *list;
    
  machine_collection = AGS_MACHINE_COLLECTION(connectable);

  if((AGS_MACHINE_COLLECTION_CONNECTED & (machine_collection->flags)) == 0){
    return;
  }

  machine_collection->flags &= (~AGS_MACHINE_COLLECTION_CONNECTED);

  /* children */
  list =
    start_list = ags_machine_collection_get_machine_mapper(machine_collection);

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free(start_list);
}

void
ags_machine_collection_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsMachineCollection *machine_collection;

  GList *start_list, *list;

  machine_collection = AGS_MACHINE_COLLECTION(applicable);

  list =
    start_list = ags_machine_collection_get_machine_mapper(machine_collection);

  while(list != NULL){
    ags_applicable_set_update(AGS_APPLICABLE(list->data), update);

    list = list->next;
  }

  g_list_free(start_list);
}

void
ags_machine_collection_apply(AgsApplicable *applicable)
{
  AgsMachineCollection *machine_collection;
  
  GList *start_list, *list;

  machine_collection = AGS_MACHINE_COLLECTION(applicable);

  list =
    start_list = ags_machine_collection_get_machine_mapper(machine_collection);

  while(list != NULL){
    ags_applicable_apply(AGS_APPLICABLE(list->data));

    list = list->next;
  }

  g_list_free(start_list);
}

void
ags_machine_collection_reset(AgsApplicable *applicable)
{
  AgsMachineCollection *machine_collection;

  GList *start_list, *list;
  
  machine_collection = AGS_MACHINE_COLLECTION(applicable);

  list =
    start_list = ags_machine_collection_get_machine_mapper(machine_collection);

  while(list != NULL){
    ags_applicable_reset(AGS_APPLICABLE(list->data));

    list = list->next;
  }

  g_list_free(start_list);
}

/**
 * ags_machine_collection_get_machine_mapper:
 * @machine_collection: the #AgsMachineCollection
 * 
 * Get effect pad output.
 * 
 * Returns: the #GList-struct containing #AgsMachineMapper
 * 
 * Since: 4.0.0
 */
GList*
ags_machine_collection_get_machine_mapper(AgsMachineCollection *machine_collection)
{
  g_return_val_if_fail(AGS_IS_MACHINE_COLLECTION(machine_collection), NULL);

  return(g_list_reverse(g_list_copy(machine_collection->machine_mapper)));
}

/**
 * ags_machine_collection_add_machine_mapper:
 * @machine_collection: the #AgsMachineCollection
 * @machine_mapper: the #AgsMachineMapper
 * 
 * Add @machine_mapper to output.
 * 
 * Since: 4.0.0
 */
void
ags_machine_collection_add_machine_mapper(AgsMachineCollection *machine_collection,
					  AgsMachineMapper *machine_mapper)
{
  g_return_if_fail(AGS_IS_MACHINE_COLLECTION(machine_collection));
  g_return_if_fail(AGS_IS_MACHINE_MAPPER(machine_mapper));

  if(g_list_find(machine_collection->machine_mapper, machine_mapper) == NULL){
    machine_collection->machine_mapper = g_list_prepend(machine_collection->machine_mapper,
							machine_mapper);
    
    gtk_box_append(machine_collection->machine_mapper_box,
		   (GtkWidget *) machine_mapper);
  }
}

/**
 * ags_machine_collection_remove_machine_mapper:
 * @machine_collection: the #AgsMachineCollection
 * @machine_mapper: the #AgsMachineMapper
 * 
 * Remove @machine_mapper from output.
 * 
 * Since: 4.0.0
 */
void
ags_machine_collection_remove_machine_mapper(AgsMachineCollection *machine_collection,
					     AgsMachineMapper *machine_mapper)
{
  g_return_if_fail(AGS_IS_MACHINE_COLLECTION(machine_collection));
  g_return_if_fail(AGS_IS_MACHINE_MAPPER(machine_mapper));

  if(g_list_find(machine_collection->machine_mapper, machine_mapper) != NULL){
    machine_collection->machine_mapper = g_list_remove(machine_collection->machine_mapper,
						       machine_mapper);
    
    gtk_box_remove(machine_collection->machine_mapper_box,
		   (GtkWidget *) machine_mapper);
  }
}

void
ags_machine_collection_reload(AgsMachineCollection *machine_collection)
{
  AgsWindow *window;
  AgsMidiExportWizard *midi_export_wizard;

  AgsApplicationContext *application_context;
  
  GList *start_machine, *machine;
  GList *start_list, *list;
  
  application_context = ags_application_context_get_instance();
  
  midi_export_wizard = (AgsMidiExportWizard *) gtk_widget_get_ancestor((GtkWidget *) machine_collection,
								       AGS_TYPE_MIDI_EXPORT_WIZARD);


  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  /* destroy old */
  list =
    start_list = ags_machine_collection_get_machine_mapper(machine_collection);

  while(list != NULL){
    ags_machine_collection_remove_machine_mapper(machine_collection,
						 list->data);

    list = list->next;
  }

  g_list_free(start_list);
  
  /* add entry */
  machine =
    start_machine = ags_window_get_machine(window);
  
  while(machine != NULL){
    if(AGS_MACHINE(machine->data)->audio != NULL &&
       ags_audio_test_ability_flags(AGS_MACHINE(machine->data)->audio, AGS_SOUND_ABILITY_NOTATION)){
      ags_machine_collection_add_entry(machine_collection,
				       machine->data);
    }
    
    machine = machine->next;
  }

  g_list_free(start_machine);
}

void
ags_machine_collection_add_entry(AgsMachineCollection *machine_collection,
				 GtkWidget *machine)
{
  AgsMachineMapper *machine_mapper;

  if(machine == NULL){
    return;
  }

  machine_mapper = (AgsMachineMapper *) g_object_new_with_properties(machine_collection->machine_mapper_type,
								     machine_collection->machine_mapper_n_properties,
								     machine_collection->machine_mapper_strv,
								     machine_collection->machine_mapper_value);
  g_object_set(machine_mapper,
	       "machine", machine,
	       NULL);
  ags_machine_collection_add_machine_mapper(machine_collection,
					    (AgsMachineMapper *) machine_mapper);
}

/**
 * ags_machine_collection_new:
 * @machine_mapper_type: the machine mapper type
 * @machine_mapper_n_properties: the machine mapper properties count
 * @machine_mapper_strv: the machine mapper string vector
 * @machine_mapper_value: the machine mapper value array
 *
 * Creates an #AgsMachineCollection
 *
 * Returns: a new #AgsMachineCollection
 *
 * Since: 3.0.0
 */
AgsMachineCollection*
ags_machine_collection_new(GType machine_mapper_type,
			   guint machine_mapper_n_properties,
			   gchar **machine_mapper_strv,
			   GValue *machine_mapper_value)
{
  AgsMachineCollection *machine_collection;

  machine_collection = (AgsMachineCollection *) g_object_new(AGS_TYPE_MACHINE_COLLECTION,
							     NULL);

  machine_collection->machine_mapper_type = machine_mapper_type;
  machine_collection->machine_mapper_n_properties = machine_mapper_n_properties;
  machine_collection->machine_mapper_strv = machine_mapper_strv;
  machine_collection->machine_mapper_value = machine_mapper_value;
  
  return(machine_collection);
}
