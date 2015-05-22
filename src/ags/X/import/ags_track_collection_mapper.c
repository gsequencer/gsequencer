/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/import/ags_track_collection_mapper.h>
#include <ags/X/import/ags_track_collection_mapper_callbacks.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>

#include <ags/X/ags_window.h>

#include <ags/X/import/ags_track_collection.h>

#include <ags/X/machine/ags_drum.h>
#include <ags/X/machine/ags_matrix.h>
#include <ags/X/machine/ags_synth.h>
#include <ags/X/machine/ags_ffplayer.h>

#include <math.h>

void ags_track_collection_mapper_class_init(AgsTrackCollectionMapperClass *track_collection_mapper);
void ags_track_collection_mapper_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_track_collection_mapper_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_track_collection_mapper_init(AgsTrackCollectionMapper *track_collection_mapper);
void ags_track_collection_mapper_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_track_collection_mapper_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_track_collection_mapper_connect(AgsConnectable *connectable);
void ags_track_collection_mapper_disconnect(AgsConnectable *connectable);
void ags_track_collection_mapper_set_update(AgsApplicable *applicable, gboolean update);
void ags_track_collection_mapper_apply(AgsApplicable *applicable);
void ags_track_collection_mapper_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_track_collection_mapper
 * @short_description: pack pad editors.
 * @title: AgsTrackCollectionMapper
 * @section_id:
 * @include: ags/X/ags_track_collection_mapper.h
 *
 * #AgsTrackCollectionMapper is a wizard to import midi files and do track mapping..
 */

enum{
  PROP_0,
  PROP_TRACK,
  PROP_INSTRUMENT,
  PROP_SEQUENCE,
};

GType
ags_track_collection_mapper_get_type(void)
{
  static GType ags_type_track_collection_mapper = 0;

  if(!ags_type_track_collection_mapper){
    static const GTypeInfo ags_track_collection_mapper_info = {
      sizeof (AgsTrackCollectionMapperClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_track_collection_mapper_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTrackCollectionMapper),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_track_collection_mapper_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_track_collection_mapper_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_track_collection_mapper_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_track_collection_mapper = g_type_register_static(GTK_TYPE_TABLE,
							      "AgsTrackCollectionMapper\0", &ags_track_collection_mapper_info,
							      0);

    g_type_add_interface_static(ags_type_track_collection_mapper,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_track_collection_mapper,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }
  
  return(ags_type_track_collection_mapper);
}

void
ags_track_collection_mapper_class_init(AgsTrackCollectionMapperClass *track_collection_mapper)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  /* GObjectClass */
  gobject = (GObjectClass *) track_collection_mapper;

  gobject->set_property = ags_track_collection_mapper_set_property;
  gobject->get_property = ags_track_collection_mapper_get_property;

  /* properties */
  /**
   * AgsTrackCollectionMapper:track:
   *
   * The tracks as xmlNode to parse.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_pointer("track\0",
				    "assigned track\0",
				    "The track which this track mapper is assigned with\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TRACK,
				  param_spec);

  /**
   * AgsTrackCollectionMapper:instrument:
   *
   * The instruments as xmlNode to parse.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_string("instrument\0",
				   "assigned instrument\0",
				   "The instrument which this track mapper is assigned with\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INSTRUMENT,
				  param_spec);

  /**
   * AgsTrackCollectionMapper:sequence:
   *
   * The sequences as xmlNode to parse.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_string("sequence\0",
				   "assigned sequence\0",
				   "The sequence which this track mapper is assigned with\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEQUENCE,
				  param_spec);
}

void
ags_track_collection_mapper_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_track_collection_mapper_connect;
  connectable->disconnect = ags_track_collection_mapper_disconnect;
}

void
ags_track_collection_mapper_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_track_collection_mapper_set_update;
  applicable->apply = ags_track_collection_mapper_apply;
  applicable->reset = ags_track_collection_mapper_reset;
}

void
ags_track_collection_mapper_init(AgsTrackCollectionMapper *track_collection_mapper)
{
  GtkLabel *label;
  
  gtk_table_resize(track_collection_mapper,
		   3, 4);

  track_collection_mapper->instrument = NULL;
  track_collection_mapper->sequence = NULL;

  track_collection_mapper->track = NULL;

  track_collection_mapper->notation = NULL;
  
  track_collection_mapper->enabled = (GtkCheckButton *) gtk_check_button_new_with_label("enabled\0");
  gtk_table_attach(track_collection_mapper,
		   track_collection_mapper->enabled,
		   0, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  track_collection_mapper->info = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_table_attach(track_collection_mapper,
		   track_collection_mapper->info,
		   0, 4,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  label = gtk_label_new("instrument: \0");
  gtk_box_pack_start(GTK_BOX(track_collection_mapper->info),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  label = gtk_label_new("sequence: \0");
  gtk_box_pack_start(GTK_BOX(track_collection_mapper->info),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);
    
  track_collection_mapper->machine_type = gtk_combo_box_text_new();
  gtk_table_attach(track_collection_mapper,
		   track_collection_mapper->machine_type,
		   1, 2,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  track_collection_mapper->audio_channels = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 16.0, 1.0);
  gtk_spin_button_set_value(track_collection_mapper->audio_channels,
			    2.0);
  gtk_table_attach(track_collection_mapper,
		   track_collection_mapper->audio_channels,
		   2, 3,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  track_collection_mapper->offset = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 256.0 * (gdouble) AGS_NOTE_EDIT_MAX_CONTROLS, 1.0);
  gtk_table_attach(track_collection_mapper,
		   track_collection_mapper->offset,
		   3, 4,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_track_collection_mapper_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsTrackCollectionMapper *track_collection_mapper;

  track_collection_mapper = AGS_TRACK_COLLECTION_MAPPER(gobject);

  switch(prop_id){
  case PROP_TRACK:
    {
      xmlNode *track;

      track = (xmlNode *) g_value_get_pointer(value);

      if(g_list_find(track_collection_mapper->track,
		     track) != NULL){
	return;
      }

      track_collection_mapper->track = g_list_prepend(track_collection_mapper->track,
						      track);
    }
    break;
  case PROP_INSTRUMENT:
    {
      GList *list, *list_start;
      gchar *instrument;

      instrument = g_value_get_string(value);

      if(instrument == track_collection_mapper->instrument){
	return;
      }

      track_collection_mapper->instrument = g_strdup(instrument);

      list_start =
	list = gtk_container_get_children(track_collection_mapper->info);
      gtk_label_set_text(list->data,
			 g_strdup_printf("instrument: %s\0",
					 instrument));

      g_list_free(list_start);
    }
    break;
  case PROP_SEQUENCE:
    {
      GList *list, *list_start;
      gchar *sequence;

      sequence = g_value_get_string(value);

      if(sequence == track_collection_mapper->sequence){
	return;
      }

      track_collection_mapper->sequence = g_strdup(sequence);

      list_start =
	list = gtk_container_get_children(track_collection_mapper->info);
      list = list->next;
      gtk_label_set_text(list->data,
			 g_strdup_printf("sequence: %s\0",
					 sequence));

      g_list_free(list_start);

    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_track_collection_mapper_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsTrackCollectionMapper *track_collection_mapper;

  track_collection_mapper = AGS_TRACK_COLLECTION_MAPPER(gobject);

  switch(prop_id){
  case PROP_TRACK:
    g_value_set_pointer(value, g_list_copy(track_collection_mapper->track));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_track_collection_mapper_connect(AgsConnectable *connectable)
{
  AgsTrackCollectionMapper *track_collection_mapper;

  track_collection_mapper = AGS_TRACK_COLLECTION_MAPPER(connectable);
}

void
ags_track_collection_mapper_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_track_collection_mapper_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsTrackCollectionMapper *track_collection_mapper;

  track_collection_mapper = AGS_TRACK_COLLECTION_MAPPER(applicable);
}

void
ags_track_collection_mapper_apply(AgsApplicable *applicable)
{
  AgsTrackCollectionMapper *track_collection_mapper;

  track_collection_mapper = AGS_TRACK_COLLECTION_MAPPER(applicable);
}

void
ags_track_collection_mapper_reset(AgsApplicable *applicable)
{
  AgsTrackCollectionMapper *track_collection_mapper;

  track_collection_mapper = AGS_TRACK_COLLECTION_MAPPER(applicable);
}

GList*
ags_track_collection_mapper_get_instrument_with_sequence(GList *track_collection_mapper,
							 gchar *instrument, gchar *sequence)
{
  while(track_collection_mapper != NULL){
    if((AGS_TRACK_COLLECTION_MAPPER(track_collection_mapper->data)->instrument == instrument &&
       AGS_TRACK_COLLECTION_MAPPER(track_collection_mapper->data)->sequence == sequence) ||
	(!g_ascii_strcasecmp(AGS_TRACK_COLLECTION_MAPPER(track_collection_mapper->data)->instrument,
			     instrument) &&
	 !g_ascii_strcasecmp(AGS_TRACK_COLLECTION_MAPPER(track_collection_mapper->data)->sequence,
			     sequence))){
      return(track_collection_mapper);
    }

    track_collection_mapper = track_collection_mapper->next;
  }
  
  return(NULL);
}

void
ags_track_collection_mapper_map(AgsTrackCollectionMapper *track_collection_mapper)
{
  AgsTrackCollection *track_collection;

  AgsNotation *current_notation;
  AgsNote *note;

  xmlNode *current, *child;
  GList *track, *notation_start, *notation;

  guint audio_channels;
  guint n_key_on, n_key_off;
  guint x, y, velocity;
  guint i;
  gboolean pattern;
  
  track_collection = gtk_widget_get_ancestor(track_collection_mapper,
					     AGS_TYPE_TRACK_COLLECTION);
  track = track_collection_mapper->track;

  /* map notation */
  notation_start =
    notation = NULL;

  audio_channels = (guint) gtk_spin_button_get_value(track_collection_mapper->audio_channels);
  
  for(i = 0; i < audio_channels; i++){
    current_notation = ags_notation_new(i);
    notation_start = g_list_prepend(notation_start,
				    current_notation);
  }

  track_collection_mapper->notation = notation_start;
  
  n_key_on = 0;
  n_key_off = 0;
  
  while(track != NULL){
    current = track->data;
    
    notation = notation_start;
    child = current->children;

    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!xmlStrncmp(xmlGetProp(child,
				  "event\0"),
		       "note-on\0",
		       8)){
	  x = (guint) round(g_ascii_strtod(xmlGetProp(child,
						      "delta-time\0"),
					   NULL) / track_collection->bpm);
	  y = (guint) g_ascii_strtoull(xmlGetProp(child,
						  "note\0"),
				       NULL,
				       10);
	  velocity = (guint) g_ascii_strtoull(xmlGetProp(child,
							 "velocity\0"),
					      NULL,
					      10);
	  
	  while(notation != NULL){
	    note = ags_note_new();
	    note->x[0] = x;
	    note->x[1] = x + 1;
	    note->y = y;
	    note->velocity[0] = velocity;
	    note->velocity[1] = 0.0;

	    ags_notation_add_note(notation->data,
				  note,
				  FALSE);
	    
	    notation = notation->next;
	  }
	  
	  n_key_on++;
	}else if(!xmlStrncmp(xmlGetProp(child,
					"event\0"),
			     "note-off\0",
			     9)){	  
	  x = (guint) round(g_ascii_strtod(xmlGetProp(child,
						      "delta-time\0"),
					   NULL) / track_collection->bpm);
	  y = (guint) g_ascii_strtoull(xmlGetProp(child,
						  "note\0"),
				       NULL,
				       10);
	  velocity = (guint) g_ascii_strtoull(xmlGetProp(child,
							 "velocity\0"),
					      NULL,
					      10);
	  
	  while(notation != NULL){
	    note = ags_note_find_prev(AGS_NOTATION(notation->data)->notes,
				      x, y);
	    
	    note->x[1] = x;
	    note->y = y;
	    note->velocity[1] = velocity;
	    
	    notation = notation->next;
	  }
	  
	  n_key_off++;
	}
      }

      child = child->next;
    }
    
    track = track->next;
  }

  /* populate machine_type */
  if(n_key_off > 0){
    gtk_combo_box_text_append_text(track_collection_mapper,
				   g_type_name(AGS_TYPE_SYNTH));
    
    gtk_combo_box_text_append_text(track_collection_mapper,
				   g_type_name(AGS_TYPE_FFPLAYER));
  }else{
    gtk_combo_box_text_append_text(track_collection_mapper,
				   g_type_name(AGS_TYPE_DRUM));
    
    gtk_combo_box_text_append_text(track_collection_mapper,
				   g_type_name(AGS_TYPE_MATRIX));
  }
}

/**
 * ags_track_collection_mapper_new:
 *
 * Creates an #AgsTrackCollectionMapper
 *
 * Returns: a new #AgsTrackCollectionMapper
 *
 * Since: 0.4.3
 */
AgsTrackCollectionMapper*
ags_track_collection_mapper_new()
{
  AgsTrackCollectionMapper *track_collection_mapper;

  track_collection_mapper = (AgsTrackCollectionMapper *) g_object_new(AGS_TYPE_TRACK_COLLECTION_MAPPER,
								      NULL);
  
  return(track_collection_mapper);
}
