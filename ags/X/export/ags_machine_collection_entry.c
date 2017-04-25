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

#include <ags/X/export/ags_machine_collection_entry.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/audio/midi/ags_midi_builder.h>

#include <ags/X/ags_machine.h>

#include <ags/X/export/ags_midi_export_wizard.h>

#include <math.h>

void ags_machine_collection_entry_class_init(AgsMachineCollectionEntryClass *machine_collection_entry);
void ags_machine_collection_entry_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_machine_collection_entry_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_machine_collection_entry_init(AgsMachineCollectionEntry *machine_collection_entry);
void ags_machine_collection_entry_set_property(GObject *gobject,
					       guint prop_id,
					       const GValue *value,
					       GParamSpec *param_spec);
void ags_machine_collection_entry_get_property(GObject *gobject,
					       guint prop_id,
					       GValue *value,
					       GParamSpec *param_spec);
void ags_machine_collection_entry_connect(AgsConnectable *connectable);
void ags_machine_collection_entry_disconnect(AgsConnectable *connectable);
void ags_machine_collection_entry_set_update(AgsApplicable *applicable, gboolean update);
void ags_machine_collection_entry_apply(AgsApplicable *applicable);
void ags_machine_collection_entry_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_machine_collection_entry
 * @short_description: pack pad editors.
 * @title: AgsMachineCollectionEntry
 * @section_id:
 * @include: ags/X/ags_machine_collection_entry.h
 *
 * #AgsMachineCollectionEntry is a wizard to export midi files and do machine mapping.
 */

enum{
  PROP_0,
  PROP_MACHINE,
};

GType
ags_machine_collection_entry_get_type(void)
{
  static GType ags_type_machine_collection_entry = 0;

  if(!ags_type_machine_collection_entry){
    static const GTypeInfo ags_machine_collection_entry_info = {
      sizeof (AgsMachineCollectionEntryClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_machine_collection_entry_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMachineCollectionEntry),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_machine_collection_entry_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_machine_collection_entry_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_machine_collection_entry_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_machine_collection_entry = g_type_register_static(GTK_TYPE_TABLE,
							       "AgsMachineCollectionEntry\0", &ags_machine_collection_entry_info,
							       0);

    g_type_add_interface_static(ags_type_machine_collection_entry,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_machine_collection_entry,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }
  
  return(ags_type_machine_collection_entry);
}

void
ags_machine_collection_entry_class_init(AgsMachineCollectionEntryClass *machine_collection_entry)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  /* GObjectClass */
  gobject = (GObjectClass *) machine_collection_entry;

  gobject->set_property = ags_machine_collection_entry_set_property;
  gobject->get_property = ags_machine_collection_entry_get_property;

  /* properties */
  /**
   * AgsMachineCollectionEntry:machine:
   *
   * The assigned #AgsMachine.
   * 
   * Since: 0.8.0
   */
  param_spec = g_param_spec_object("machine\0",
				   "assigned machine\0",
				   "The machine which this machine entry is assigned with\0",
				   AGS_TYPE_MACHINE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MACHINE,
				  param_spec);
}

void
ags_machine_collection_entry_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_machine_collection_entry_connect;
  connectable->disconnect = ags_machine_collection_entry_disconnect;
}

void
ags_machine_collection_entry_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_machine_collection_entry_set_update;
  applicable->apply = ags_machine_collection_entry_apply;
  applicable->reset = ags_machine_collection_entry_reset;
}

void
ags_machine_collection_entry_init(AgsMachineCollectionEntry *machine_collection_entry)
{
  GtkLabel *label;

  gtk_table_resize((GtkTable *) machine_collection_entry,
		   4, 4);

  machine_collection_entry->machine = NULL;

  /* enabled */
  machine_collection_entry->enabled = (GtkCheckButton *) gtk_check_button_new_with_label("enabled\0");
  gtk_table_attach((GtkTable *) machine_collection_entry,
		   (GtkWidget *) machine_collection_entry->enabled,
		   0, 4,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* machine label */
  machine_collection_entry->label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
							      "xalign\0", 0.0,
							      NULL);
  gtk_table_attach((GtkTable *) machine_collection_entry,
		   (GtkWidget *) machine_collection_entry->label,
		   0, 4,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* instrument * /
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "instrument: \0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach((GtkTable *) machine_collection_entry,
		   (GtkWidget *) label,
		   0, 2,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  machine_collection_entry->instrument = (GtkEntry *) gtk_entry_new();
  gtk_table_attach((GtkTable *) machine_collection_entry,
		   (GtkWidget *) machine_collection_entry->instrument,
		   2, 4,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  */
  machine_collection_entry->instrument = NULL;
  
  /* sequence */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "sequence: \0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach((GtkTable *) machine_collection_entry,
		   (GtkWidget *) label,
		   0, 2,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  machine_collection_entry->sequence = (GtkEntry *) gtk_entry_new();
  gtk_table_attach((GtkTable *) machine_collection_entry,
		   (GtkWidget *) machine_collection_entry->sequence,
		   2, 4,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_machine_collection_entry_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec)
{
  AgsMachineCollectionEntry *machine_collection_entry;

  machine_collection_entry = AGS_MACHINE_COLLECTION_ENTRY(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    {
      GtkWidget *machine;

      machine = (GtkWidget *) g_value_get_object(value);

      if(machine_collection_entry->machine == machine){
	return;
      }

      if(machine_collection_entry->machine != NULL){
	g_object_unref(machine_collection_entry->machine);
      }
      
      if(machine != NULL){
	g_object_ref(machine);

	/* fill in some fields */
	gtk_label_set_text(machine_collection_entry->label,
			   g_strdup_printf("%s: %s\0",
					   G_OBJECT_TYPE_NAME(machine),
					   AGS_MACHINE(machine)->name));
	
	//	gtk_entry_set_text(machine_collection_entry->instrument,
	//		   G_OBJECT_TYPE_NAME(machine));

      	gtk_entry_set_text(machine_collection_entry->sequence,
			   AGS_MACHINE(machine)->name);
      }

      machine_collection_entry->machine = machine;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_collection_entry_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec)
{
  AgsMachineCollectionEntry *machine_collection_entry;

  machine_collection_entry = AGS_MACHINE_COLLECTION_ENTRY(gobject);

  switch(prop_id){
  case PROP_MACHINE:
    {
      g_value_set_object(value, machine_collection_entry->machine);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_machine_collection_entry_connect(AgsConnectable *connectable)
{
  AgsMachineCollectionEntry *machine_collection_entry;

  machine_collection_entry = AGS_MACHINE_COLLECTION_ENTRY(connectable);
}

void
ags_machine_collection_entry_disconnect(AgsConnectable *connectable)
{
  AgsMachineCollectionEntry *machine_collection_entry;

  machine_collection_entry = AGS_MACHINE_COLLECTION_ENTRY(connectable);

  /* empty */
}

void
ags_machine_collection_entry_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsMachineCollectionEntry *machine_collection_entry;

  machine_collection_entry = AGS_MACHINE_COLLECTION_ENTRY(applicable);
}

void
ags_machine_collection_entry_apply(AgsApplicable *applicable)
{
  AgsMachine *machine;

  AgsMidiExportWizard *midi_export_wizard;
  AgsMachineCollectionEntry *machine_collection_entry;

  AgsMidiBuilder *midi_builder;

  AgsNote **check_match;
  
  GList *notation, *notation_start;
  GList *note;
  
  gint key_on[128];
  gint key_off[128];
  gboolean key_active[128];

  guint delta_time;
  guint pulse_unit;
  guint notation_count;
  guint x, prev_x;
  guint i;
  gboolean success;
  
  machine_collection_entry = AGS_MACHINE_COLLECTION_ENTRY(applicable);

  if(!gtk_toggle_button_get_active(machine_collection_entry->enabled)){
    return;
  }

  midi_export_wizard = gtk_widget_get_ancestor(machine_collection_entry,
					       AGS_TYPE_MIDI_EXPORT_WIZARD);
  
  machine = machine_collection_entry->machine;

  notation_start = machine->audio->notation;
  
  midi_builder = midi_export_wizard->midi_builder;
  pulse_unit = midi_export_wizard->pulse_unit;

  notation_count = g_list_length(notation_start);

  /* append track */
  ags_midi_builder_append_track(midi_builder,
				gtk_entry_get_text(machine_collection_entry->sequence));

  /* put keys */
  check_match = (AgsNote **) malloc(notation_count * sizeof(AgsNote *));
  
  memset(key_on, -1, 128 * sizeof(gint));
  memset(key_off, -1, 128 * sizeof(gint));
  
  memset(key_active, FALSE, 128 * sizeof(gboolean));

  delta_time = 0;
  prev_x = 0;
  
  success = TRUE;

  while(success){
    notation = notation_start;

    for(i = 0; i < notation_count; i++){
      check_match[i] = NULL;
    }
    
    for(i = 0; notation != NULL; i++){
      note = AGS_NOTATION(notation->data)->notes;

      while(note != NULL){
	if(AGS_NOTE(note->data)->y < 128){
	  if(key_on[AGS_NOTE(note->data)->y] < (gint) AGS_NOTE(note->data)->x[0]){
	    check_match[i] = AGS_NOTE(note->data);

	    break;
	  }else if(key_off[AGS_NOTE(note->data)->y] < (gint) AGS_NOTE(note->data)->x[1]){
	    check_match[i] = AGS_NOTE(note->data);

	    break;
	  }
	}
	
	note = note->next;
      }
      
      notation = notation->next;
    }

    /* find next pulse */
    x = G_MAXUINT;
    success = FALSE;
    
    for(i = 0; i < notation_count; i++){
      if(check_match[i] != NULL){
	if(!key_active[check_match[i]->y]){
	  /* check key-on */
	  if(check_match[i]->x[0] < x){
	    x = check_match[i]->x[0];

	    success = TRUE;
	  }
	}else if(key_active[check_match[i]->y]){
	  /* check key-off */
	  if(check_match[i]->x[1] < x){
	    x = check_match[i]->x[1];
	    
	    success = TRUE;
	  }
	}
      }
    }
    
    /* apply events matching pulse */
    if(success){
      for(i = 0; i < notation_count; i++){
	if(check_match[i] != NULL){
	  if(x > prev_x){
	    delta_time += (x - prev_x) * pulse_unit;
	  }
	  
	  if(check_match[i]->x[0] == x){
	    /* append key-on */
	    ags_midi_builder_append_key_on(midi_builder,
					   delta_time,
					   0,
					   check_match[i]->y,
					   127);
	    g_message("key-on %d,%d - %d", check_match[i]->x[0], check_match[i]->x[1], check_match[i]->y);
	    
	    key_on[check_match[i]->y] = check_match[i]->x[0];
	    key_active[check_match[i]->y] = TRUE;
	  }else if(check_match[i]->x[1] == x){
	    /* append key-off */
	    ags_midi_builder_append_key_off(midi_builder,
					    delta_time,
					    0,
					    check_match[i]->y,
					    127);	    

	    key_off[check_match[i]->y] = check_match[i]->x[1];
	    key_active[check_match[i]->y] = FALSE;
	  }

	  prev_x = x;
	}
      }
    }
  }
}

void
ags_machine_collection_entry_reset(AgsApplicable *applicable)
{
  AgsMachineCollectionEntry *machine_collection_entry;

  machine_collection_entry = AGS_MACHINE_COLLECTION_ENTRY(applicable);
}

/**
 * ags_machine_collection_entry_new:
 *
 * Creates an #AgsMachineCollectionEntry
 *
 * Returns: a new #AgsMachineCollectionEntry
 *
 * Since: 0.8.0
 */
AgsMachineCollectionEntry*
ags_machine_collection_entry_new()
{
  AgsMachineCollectionEntry *machine_collection_entry;

  machine_collection_entry = (AgsMachineCollectionEntry *) g_object_new(AGS_TYPE_MACHINE_COLLECTION_ENTRY,
									NULL);
  
  return(machine_collection_entry);
}
