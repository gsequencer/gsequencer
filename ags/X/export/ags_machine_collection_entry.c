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

#include <ags/X/export/ags_machine_collection_entry.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>

#include <ags/X/export/ags_midi_export_wizard.h>

#include <math.h>

#include <ags/i18n.h>

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
 * @short_description: Machine entry
 * @title: AgsMachineCollectionEntry
 * @section_id:
 * @include: ags/X/ags_machine_collection_entry.h
 *
 * #AgsMachineCollectionEntry is a composite widget specifying machines
 * to export.
 */

enum{
  PROP_0,
  PROP_MACHINE,
};

GType
ags_machine_collection_entry_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_machine_collection_entry = 0;

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

    ags_type_machine_collection_entry = g_type_register_static(GTK_TYPE_GRID,
							       "AgsMachineCollectionEntry", &ags_machine_collection_entry_info,
							       0);

    g_type_add_interface_static(ags_type_machine_collection_entry,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_machine_collection_entry,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_machine_collection_entry);
  }

  return g_define_type_id__volatile;
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
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("machine",
				   i18n_pspec("assigned machine"),
				   i18n_pspec("The machine which this machine entry is assigned with"),
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

  machine_collection_entry->flags = 0;
  
  machine_collection_entry->machine = NULL;

  /* enabled */
  machine_collection_entry->enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));
  
  gtk_widget_set_valign((GtkWidget *) machine_collection_entry->enabled,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) machine_collection_entry->enabled,
			GTK_ALIGN_FILL);
  
  gtk_grid_attach((GtkGrid *) machine_collection_entry,
		  (GtkWidget *) machine_collection_entry->enabled,
		  0, 0,
		  4, 1);

  /* machine label */
  machine_collection_entry->label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
							      "xalign", 0.0,
							      NULL);

  gtk_widget_set_valign((GtkWidget *) machine_collection_entry->label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) machine_collection_entry->label,
			GTK_ALIGN_FILL);

  gtk_widget_set_margin_end((GtkWidget *) machine_collection_entry->label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach((GtkGrid *) machine_collection_entry,
		  (GtkWidget *) machine_collection_entry->label,
		  0, 1,
		  4, 1);

  machine_collection_entry->instrument = NULL;
  
  /* sequence */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("sequence: "),
				    "xalign", 0.0,
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach((GtkGrid *) machine_collection_entry,
		  (GtkWidget *) label,
		  0, 3,
		  2, 1);

  machine_collection_entry->sequence = (GtkEntry *) gtk_entry_new();

  gtk_widget_set_valign((GtkWidget *) machine_collection_entry->sequence,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) machine_collection_entry->sequence,
			GTK_ALIGN_FILL);

  gtk_grid_attach((GtkGrid *) machine_collection_entry,
		  (GtkWidget *) machine_collection_entry->sequence,
		  2, 3,
		  2, 1);
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
			   g_strdup_printf("%s: %s",
					   G_OBJECT_TYPE_NAME(machine),
					   AGS_MACHINE(machine)->machine_name));
	
	//	gtk_entry_set_text(machine_collection_entry->instrument,
	//		   G_OBJECT_TYPE_NAME(machine));

      	gtk_entry_set_text(machine_collection_entry->sequence,
			   AGS_MACHINE(machine)->machine_name);
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

  if((AGS_MACHINE_COLLECTION_ENTRY_CONNECTED & (machine_collection_entry->flags)) != 0){
    return;
  }

  machine_collection_entry->flags |= AGS_MACHINE_COLLECTION_ENTRY_CONNECTED;
}

void
ags_machine_collection_entry_disconnect(AgsConnectable *connectable)
{
  AgsMachineCollectionEntry *machine_collection_entry;

  machine_collection_entry = AGS_MACHINE_COLLECTION_ENTRY(connectable);

  if((AGS_MACHINE_COLLECTION_ENTRY_CONNECTED & (machine_collection_entry->flags)) == 0){
    return;
  }

  machine_collection_entry->flags &= (~AGS_MACHINE_COLLECTION_ENTRY_CONNECTED);
}

void
ags_machine_collection_entry_set_update(AgsApplicable *applicable, gboolean update)
{
  //TODO:JK: implement me
}

void
ags_machine_collection_entry_apply(AgsApplicable *applicable)
{
  AgsMachine *machine;

  AgsMidiExportWizard *midi_export_wizard;
  AgsMachineCollectionEntry *machine_collection_entry;

  AgsMidiBuilder *midi_builder;

  GList *start_notation, *notation;
  
  gchar *segmentation;

  guint audio_channels;
  gdouble delay_factor;
  gdouble bpm;
  guint prev_delta_time;
  guint delta_time;
  guint i;
  
  machine_collection_entry = AGS_MACHINE_COLLECTION_ENTRY(applicable);

  if(!gtk_toggle_button_get_active((GtkToggleButton *) machine_collection_entry->enabled)){
    return;
  }

  midi_export_wizard = (AgsMidiExportWizard *) gtk_widget_get_ancestor((GtkWidget *) machine_collection_entry,
								       AGS_TYPE_MIDI_EXPORT_WIZARD);

  machine = (AgsMachine *) machine_collection_entry->machine;

  g_object_get(machine->audio,
	       "notation", &start_notation,
	       "audio-channels", &audio_channels,
	       NULL);
  
  midi_builder = midi_export_wizard->midi_builder;

  bpm = midi_builder->midi_header->beat;
  
  delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;  

  /* segmentation */
  segmentation = ags_config_get_value(ags_config_get_instance(),
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    guint denominator, numerator;
    
    sscanf(segmentation, "%d/%d",
	   &denominator,
	   &numerator);
    
    delay_factor = 1.0 / numerator * (numerator / denominator);

    g_free(segmentation);
  }
  
  delta_time = 0;
  prev_delta_time = 0;

  for(i = 0; i < audio_channels; i++){
    GList *start_note, *note;
    GList *start_active_note, *active_note;

    gchar *str;
    gint key_on[128];
    
    /* append track */
    str = g_strdup_printf("%s #%d",
			  gtk_entry_get_text(machine_collection_entry->sequence),
			  i);
    
    ags_midi_builder_append_track(midi_builder,
				  str);

    g_free(str);

    /* append tempo */
    ags_midi_builder_append_time_signature(midi_builder,
					   0,
					   4, 4,
					   36, 8);  
    
    notation = start_notation;

    start_active_note = NULL;
    
    /* put keys */
    memset(key_on, 0, 128 * sizeof(gint));

    while(notation != NULL){
      guint audio_channel;

      g_object_get(notation->data,
		   "audio-channel", &audio_channel,
		   NULL);
      
      if(i != audio_channel){
	notation = notation->next;

	continue;
      }

      g_object_get(notation->data,
		   "note", &start_note,
		   NULL);

      note = start_note;

      while(note != NULL){
	guint note_x0;
	guint note_y;

	g_object_get(note->data,
		     "x0", &note_x0,
		     "y", &note_y,
		     NULL);

	/* check note-off */
	active_note = start_active_note;
	
	while(active_note != NULL){
	  guint active_x1;
	  guint active_y;

	  g_object_get(active_note->data,
		       "x1", &active_x1,
		       "y", &active_y,
		       NULL);

	  
	  if(active_x1 <= note_x0){
	    key_on[active_y] -= 1;
	    
	    if(key_on[note_y] == 0){
	      delta_time = ags_midi_util_offset_to_delta_time(delay_factor,
							      AGS_MIDI_EXPORT_WIZARD_DEFAULT_DIVISION,
							      AGS_MIDI_EXPORT_WIZARD_DEFAULT_TEMPO,
							      AGS_MIDI_EXPORT_WIZARD_DEFAULT_BPM,
							      note_x0);
	    
	      /* append key-off */
	      ags_midi_builder_append_key_off(midi_builder,
					      delta_time - prev_delta_time,
					      0,
					      active_y,
					      (guint) 127);  /* * AGS_NOTE(active_note->data)->release.imag */

	      prev_delta_time = delta_time;
	      
	      start_active_note = g_list_remove(start_active_note,
						active_note->data);
	    }
	  }
	  
	  active_note = active_note->next;
	}
	
	/* note-on */
	if(note_y < 128){
	  start_active_note = g_list_prepend(start_active_note,
					     note->data);
	  
	  key_on[note_y] += 1;

	  if(key_on[note_y] == 1){
	    delta_time = ags_midi_util_offset_to_delta_time(delay_factor,
							    AGS_MIDI_EXPORT_WIZARD_DEFAULT_DIVISION,
							    AGS_MIDI_EXPORT_WIZARD_DEFAULT_TEMPO,
							    AGS_MIDI_EXPORT_WIZARD_DEFAULT_BPM,
							    note_x0);
	    
	    /* append key-on */
	    ags_midi_builder_append_key_on(midi_builder,
					   delta_time - prev_delta_time,
					   0,
					   note_y,
					   (guint) 127); /*  * AGS_NOTE(note->data)->attack.imag */

	    prev_delta_time = delta_time;
	  }
	}

	note = note->next;
      }

      g_list_free_full(start_note,
		       g_object_unref);
            
      notation = notation->next;
    }
  }
    
  g_list_free_full(start_notation,
		   g_object_unref);
}

void
ags_machine_collection_entry_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

/**
 * ags_machine_collection_entry_new:
 * @machine: the #AgsMachine
 *
 * Create a new instance of #AgsMachineCollectionEntry
 *
 * Returns: the new #AgsMachineCollectionEntry
 *
 * Since: 3.0.0
 */
AgsMachineCollectionEntry*
ags_machine_collection_entry_new(GtkWidget *machine)
{
  AgsMachineCollectionEntry *machine_collection_entry;

  machine_collection_entry = (AgsMachineCollectionEntry *) g_object_new(AGS_TYPE_MACHINE_COLLECTION_ENTRY,
									"machine", machine,
									NULL);
  
  return(machine_collection_entry);
}
