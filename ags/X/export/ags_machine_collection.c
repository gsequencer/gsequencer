/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/export/ags_machine_collection.h>
#include <ags/X/export/ags_machine_collection_callbacks.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/audio/ags_audio.h>

#include <ags/X/ags_window.h>

#include <ags/X/export/ags_midi_export_wizard.h>
#include <ags/X/export/ags_machine_collection_entry.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_machine_collection_class_init(AgsMachineCollectionClass *machine_collection);
void ags_machine_collection_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_machine_collection_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_machine_collection_init(AgsMachineCollection *machine_collection);
void ags_machine_collection_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_machine_collection_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_machine_collection_connect(AgsConnectable *connectable);
void ags_machine_collection_disconnect(AgsConnectable *connectable);
void ags_machine_collection_set_update(AgsApplicable *applicable, gboolean update);
void ags_machine_collection_apply(AgsApplicable *applicable);
void ags_machine_collection_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_machine_collection
 * @short_description: Property of audio, channels or pads in bulk mode.
 * @title: AgsMachineCollection
 * @section_id:
 * @include: ags/X/ags_machine_collection.h
 *
 * #AgsMachineCollection is a composite widget to property. A property editor 
 * should be packed by a #AgsPropertyCollectionEditor.
 */

enum{
  PROP_0,
};

static gpointer ags_machine_collection_parent_class = NULL;

GType
ags_machine_collection_get_type(void)
{
  static GType ags_type_machine_collection = 0;

  if(!ags_type_machine_collection){
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

    ags_type_machine_collection = g_type_register_static(GTK_TYPE_VBOX,
							 "AgsMachineCollection", &ags_machine_collection_info,
							 0);
    
    g_type_add_interface_static(ags_type_machine_collection,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_machine_collection,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }

  return(ags_type_machine_collection);
}

void
ags_machine_collection_class_init(AgsMachineCollectionClass *machine_collection)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_machine_collection_parent_class = g_type_class_peek_parent(machine_collection);

  /* GObjectClass */
  gobject = (GObjectClass *) machine_collection;

  gobject->set_property = ags_machine_collection_set_property;
  gobject->get_property = ags_machine_collection_get_property;

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
  
  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new(NULL,
								  NULL);
  gtk_box_pack_start(GTK_BOX(machine_collection),
		     (GtkWidget *) scrolled_window,
		     TRUE, TRUE,
		     0);
  
  machine_collection->child = (GtkVBox *) gtk_vbox_new(FALSE,
						       0);
  gtk_scrolled_window_add_with_viewport(scrolled_window,
					(GtkWidget *) machine_collection->child);
}

void
ags_machine_collection_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsMachineCollection *machine_collection;

  machine_collection = AGS_MACHINE_COLLECTION(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_collection_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsMachineCollection *machine_collection;

  machine_collection = AGS_MACHINE_COLLECTION(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_collection_connect(AgsConnectable *connectable)
{
  AgsMachineCollection *machine_collection;
  GList *list, *list_start;
  
  machine_collection = AGS_MACHINE_COLLECTION(connectable);

  /* children */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) machine_collection->child);

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_machine_collection_disconnect(AgsConnectable *connectable)
{
  AgsMachineCollection *machine_collection;
  GList *list, *list_start;
  
  machine_collection = AGS_MACHINE_COLLECTION(connectable);

  /* children */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) machine_collection->child);

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_machine_collection_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsMachineCollection *machine_collection;
  GList *list, *list_start;
  
  machine_collection = AGS_MACHINE_COLLECTION(applicable);

  list_start = 
    list = gtk_container_get_children((GtkContainer *) machine_collection->child);

  while(list != NULL){
    ags_applicable_set_update(AGS_APPLICABLE(list->data), update);

    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_machine_collection_apply(AgsApplicable *applicable)
{
  AgsMachineCollection *machine_collection;
  GList *list, *list_start;
  
  machine_collection = AGS_MACHINE_COLLECTION(applicable);

  list_start = 
    list = gtk_container_get_children((GtkContainer *) machine_collection->child);

  while(list != NULL){
    ags_applicable_apply(AGS_APPLICABLE(list->data));

    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_machine_collection_reset(AgsApplicable *applicable)
{
  AgsMachineCollection *machine_collection;

  GList *list, *list_start;
  
  machine_collection = AGS_MACHINE_COLLECTION(applicable);

  list_start = 
    list = gtk_container_get_children((GtkContainer *) machine_collection->child);

  while(list != NULL){
    ags_applicable_reset(AGS_APPLICABLE(list->data));

    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_machine_collection_reload(AgsMachineCollection *machine_collection)
{
  AgsWindow *window;
  AgsMidiExportWizard *midi_export_wizard;
  GtkWidget *parent;
  
  GList *machine, *machine_start;

  midi_export_wizard = gtk_widget_get_ancestor(machine_collection,
					       AGS_TYPE_MIDI_EXPORT_WIZARD);
  window = midi_export_wizard->main_window;

  /* destroy old */
  parent = GTK_WIDGET(machine_collection->child)->parent;
  gtk_widget_destroy(machine_collection->child);
  
  machine_collection->child = (GtkVBox *) gtk_vbox_new(FALSE,
						       0);
  gtk_container_add(parent,
		    machine_collection->child);
  
  /* add entry */
  machine =
    machine_start = gtk_container_get_children(window->machines);
  
  while(machine != NULL){
    if(AGS_MACHINE(machine->data)->audio != NULL &&
       (AGS_AUDIO_HAS_NOTATION & (AGS_MACHINE(machine->data)->audio->flags)) != 0){
      ags_machine_collection_add_entry(machine_collection,
				       machine->data);
    }
    
    machine = machine->next;
  }

  g_list_free(machine_start);
}

void
ags_machine_collection_add_entry(AgsMachineCollection *machine_collection,
				 GtkWidget *machine)
{
  AgsMachineCollectionEntry *machine_collection_entry;

  if(machine == NULL){
    return;
  }

  machine_collection_entry = (AgsMachineCollectionEntry *) g_object_newv(machine_collection->child_type,
									 machine_collection->child_parameter_count,
									 machine_collection->child_parameter);
  g_object_set(machine_collection_entry,
	       "machine", machine,
	       NULL);
  gtk_box_pack_start(GTK_BOX(machine_collection->child),
		     GTK_WIDGET(machine_collection_entry),
		     FALSE, FALSE,
		     0);
}

/**
 * ags_machine_collection_new:
 * @child_type: the child type
 * @child_parameter_count: the child parameter count
 * @child_parameter: the child parameters
 *
 * Creates an #AgsMachineCollection
 *
 * Returns: a new #AgsMachineCollection
 *
 * Since: 1.0.0
 */
AgsMachineCollection*
ags_machine_collection_new(GType child_type,
			   guint child_parameter_count,
			   GParameter *child_parameter)
{
  AgsMachineCollection *machine_collection;

  machine_collection = (AgsMachineCollection *) g_object_new(AGS_TYPE_MACHINE_COLLECTION,
							     NULL);

  machine_collection->child_type = child_type;
  machine_collection->child_parameter_count = child_parameter_count;
  machine_collection->child_parameter = child_parameter;
  
  return(machine_collection);
}
