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

#include <ags/app/import/ags_track_collection.h>
#include <ags/app/import/ags_track_collection_callbacks.h>

#include <ags/app/ags_ui_provider.h>

#include <ags/app/import/ags_track_mapper.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_track_collection_class_init(AgsTrackCollectionClass *track_collection);
void ags_track_collection_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_track_collection_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_track_collection_init(AgsTrackCollection *track_collection);
void ags_track_collection_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_track_collection_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);

void ags_track_collection_connect(AgsConnectable *connectable);
void ags_track_collection_disconnect(AgsConnectable *connectable);

void ags_track_collection_set_update(AgsApplicable *applicable, gboolean update);
void ags_track_collection_apply(AgsApplicable *applicable);
void ags_track_collection_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_track_collection
 * @short_description: Pack track mapper
 * @title: AgsTrackCollection
 * @section_id:
 * @include: ags/app/ags_track_collection.h
 *
 * #AgsTrackCollection is a composite widget to property. A property editor 
 * should be packed by a #AgsPropertyCollectionEditor.
 */

enum{
  PROP_0,
  PROP_MIDI_DOCUMENT,
};

static gpointer ags_track_collection_parent_class = NULL;

GType
ags_track_collection_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_track_collection = 0;

    static const GTypeInfo ags_track_collection_info = {
      sizeof (AgsTrackCollectionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_track_collection_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTrackCollection),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_track_collection_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_track_collection_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_track_collection_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_track_collection = g_type_register_static(GTK_TYPE_BOX,
						       "AgsTrackCollection", &ags_track_collection_info,
						       0);
    
    g_type_add_interface_static(ags_type_track_collection,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_track_collection,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_track_collection);
  }

  return g_define_type_id__volatile;
}

void
ags_track_collection_class_init(AgsTrackCollectionClass *track_collection)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_track_collection_parent_class = g_type_class_peek_parent(track_collection);

  /* GObjectClass */
  gobject = (GObjectClass *) track_collection;

  gobject->set_property = ags_track_collection_set_property;
  gobject->get_property = ags_track_collection_get_property;

  /* properties */
  /**
   * AgsTrackCollection:midi-document:
   *
   * The assigned midi document.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("midi-document",
				    i18n_pspec("midi document of track collection"),
				    i18n_pspec("The midi document this track collection is assigned to"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MIDI_DOCUMENT,
				  param_spec);
}

void
ags_track_collection_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_track_collection_connect;
  connectable->disconnect = ags_track_collection_disconnect;
}

void
ags_track_collection_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_track_collection_set_update;
  applicable->apply = ags_track_collection_apply;
  applicable->reset = ags_track_collection_reset;
}

void
ags_track_collection_init(AgsTrackCollection *track_collection)
{
  GtkScrolledWindow *scrolled_window;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(track_collection),
				 GTK_ORIENTATION_VERTICAL);
  
  gtk_box_set_spacing(track_collection,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  track_collection->midi_doc = NULL;

  track_collection->first_offset = 0;
  track_collection->bpm = 120.0;

  track_collection->division = 96;
  track_collection->tempo = 500000;
  
  track_collection->default_length = 4;

  track_collection->track_mapper_type = G_TYPE_NONE;

  track_collection->track_mapper_n_properties = 0;

  track_collection->track_mapper_strv = NULL;
  track_collection->track_mapper_value = NULL;
  
  scrolled_window = (GtkScrolledWindow *) gtk_scrolled_window_new();
  gtk_box_append((GtkBox *) track_collection,
		 (GtkWidget *) scrolled_window);
  
  track_collection->track_mapper_box = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
							      0);
  gtk_scrolled_window_set_child(scrolled_window,
				(GtkWidget *) track_collection->track_mapper_box);
}

void
ags_track_collection_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsTrackCollection *track_collection;

  track_collection = AGS_TRACK_COLLECTION(gobject);

  switch(prop_id){
  case PROP_MIDI_DOCUMENT:
  {
    xmlDoc *midi_document;

    midi_document = (xmlDoc *) g_value_get_pointer(value);

    if(track_collection->midi_doc == midi_document){
      return;
    }

    track_collection->midi_doc = midi_document;      
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_track_collection_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsTrackCollection *track_collection;

  track_collection = AGS_TRACK_COLLECTION(gobject);

  switch(prop_id){
  case PROP_MIDI_DOCUMENT:
  {
    g_value_set_pointer(value, track_collection->midi_doc);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_track_collection_connect(AgsConnectable *connectable)
{
  AgsTrackCollection *track_collection;

  GList *start_list, *list;
  
  track_collection = AGS_TRACK_COLLECTION(connectable);

  /* children */
  list =
    start_list = ags_track_collection_get_track_mapper(track_collection);

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free(start_list);
}

void
ags_track_collection_disconnect(AgsConnectable *connectable)
{
  AgsTrackCollection *track_collection;

  GList *start_list, *list;
  
  track_collection = AGS_TRACK_COLLECTION(connectable);

  /* children */
  list =
    start_list = ags_track_collection_get_track_mapper(track_collection);

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free(start_list);
}

void
ags_track_collection_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsTrackCollection *track_collection;

  GList *start_list, *list;
  
  track_collection = AGS_TRACK_COLLECTION(applicable);

  list =
    start_list = ags_track_collection_get_track_mapper(track_collection);

  while(list != NULL){
    ags_applicable_set_update(AGS_APPLICABLE(list->data), update);

    list = list->next;
  }

  g_list_free(start_list);
}

void
ags_track_collection_apply(AgsApplicable *applicable)
{
  AgsTrackCollection *track_collection;

  GList *start_list, *list;
  
  track_collection = AGS_TRACK_COLLECTION(applicable);

  list =
    start_list = ags_track_collection_get_track_mapper(track_collection);

  while(list != NULL){
    ags_applicable_apply(AGS_APPLICABLE(list->data));

    list = list->next;
  }

  g_list_free(start_list);
}

void
ags_track_collection_reset(AgsApplicable *applicable)
{
  AgsTrackCollection *track_collection;

  GList *start_list, *list;
  
  track_collection = AGS_TRACK_COLLECTION(applicable);

  list =
    start_list = ags_track_collection_get_track_mapper(track_collection);

  while(list != NULL){
    ags_applicable_reset(AGS_APPLICABLE(list->data));

    list = list->next;
  }

  g_list_free(start_list);
}

/**
 * ags_track_collection_get_track_mapper:
 * @track_collection: the #AgsTrackCollection
 * 
 * Get effect pad output.
 * 
 * Returns: the #GList-struct containing #AgsTrackMapper
 * 
 * Since: 4.0.0
 */
GList*
ags_track_collection_get_track_mapper(AgsTrackCollection *track_collection)
{
  g_return_val_if_fail(AGS_IS_TRACK_COLLECTION(track_collection), NULL);

  return(g_list_reverse(g_list_copy(track_collection->track_mapper)));
}

/**
 * ags_track_collection_add_track_mapper:
 * @track_collection: the #AgsTrackCollection
 * @track_mapper: the #AgsTrackMapper
 * 
 * Add @track_mapper to output.
 * 
 * Since: 4.0.0
 */
void
ags_track_collection_add_track_mapper(AgsTrackCollection *track_collection,
				      AgsTrackMapper *track_mapper)
{
  g_return_if_fail(AGS_IS_TRACK_COLLECTION(track_collection));
  g_return_if_fail(AGS_IS_TRACK_MAPPER(track_mapper));

  if(g_list_find(track_collection->track_mapper, track_mapper) == NULL){
    track_collection->track_mapper = g_list_prepend(track_collection->track_mapper,
						    track_mapper);
    
    gtk_box_append(track_collection->track_mapper_box,
		   track_mapper);
  }
}

/**
 * ags_track_collection_remove_track_mapper:
 * @track_collection: the #AgsTrackCollection
 * @track_mapper: the #AgsTrackMapper
 * 
 * Remove @track_mapper from output.
 * 
 * Since: 4.0.0
 */
void
ags_track_collection_remove_track_mapper(AgsTrackCollection *track_collection,
					 AgsTrackMapper *track_mapper)
{
  g_return_if_fail(AGS_IS_TRACK_COLLECTION(track_collection));
  g_return_if_fail(AGS_IS_TRACK_MAPPER(track_mapper));

  if(g_list_find(track_collection->track_mapper, track_mapper) != NULL){
    track_collection->track_mapper = g_list_remove(track_collection->track_mapper,
						   track_mapper);
    
    gtk_box_remove(track_collection->track_mapper_box,
		   track_mapper);
  }
}

void
ags_track_collection_parse(AgsTrackCollection *track_collection)
{
  xmlXPathContext *xpath_context;
  xmlXPathObject *xpath_object;
  xmlNode *header_node, *tempo_node;
  xmlNode *time_signature_node;
  xmlNode **node, **instrument_node, **sequence_node;

  GList *start_list, *list;

  xmlChar *str;
  gchar *instrument, *sequence;

  gdouble sec_val;
  guint denominator, numerator;
  guint i, j;
  
  /* bpm and first_offset */
  header_node = NULL;
  
  xpath_context = xmlXPathNewContext(track_collection->midi_doc);
  xpath_object = xmlXPathEval((xmlChar *) "//midi-header",
			      xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;
    
    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	header_node = node[i];
      }
    }
  }

  tempo_node = NULL;
  
  xpath_context = xmlXPathNewContext(track_collection->midi_doc);
  xpath_object = xmlXPathEval((xmlChar *) "//midi-tracks/midi-track/midi-message[@event=\"tempo-number\"]",
			      xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;
    
    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	tempo_node = node[i];
      }
    }
  }

  if(header_node == NULL){
    return;
  }

  str = xmlGetProp(header_node,
		   BAD_CAST "division");
  track_collection->division = g_ascii_strtoull(str,
						NULL,
						10);
  
  xmlFree(str);
  
  if(tempo_node != NULL){
    track_collection->first_offset = 0;
    
    str = xmlGetProp(tempo_node,
		     BAD_CAST "tempo");
    track_collection->tempo = g_ascii_strtoull(str,
					       NULL,
					       10);
    xmlFree(str);
    
    sec_val = ags_midi_parser_ticks_to_sec(NULL,
					   (guint) track_collection->division,
					   (gint) track_collection->division,
					   (guint) track_collection->tempo);
    //  g_message("", sec_val);
    track_collection->bpm = 60.0 / sec_val;
  }
  //  g_message("bpm %f", track_collection->bpm);

  /* default length */
  time_signature_node = NULL;
  
  denominator = 4;
  numerator = 4;

  xpath_context = xmlXPathNewContext(track_collection->midi_doc);
  xpath_object = xmlXPathEval((xmlChar *) "//midi-tracks/midi-track/midi-message[@event=\"time-signature\"]",
			      xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;
    
    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	time_signature_node = node[i];	
      }
    }

    if(time_signature_node != NULL){
      xmlChar *str;

      str = xmlGetProp(time_signature_node,
		       BAD_CAST "timesig");

      if(str != NULL){
	sscanf(str, "%d/%d", &numerator, &denominator);
      }
    }
  }
  
  track_collection->default_length = numerator * (numerator / denominator);
  
  /* collect */
  xpath_context = xmlXPathNewContext(track_collection->midi_doc);
  xpath_object = xmlXPathEval((xmlChar *) "//midi-tracks/midi-track",
			      xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;
    
    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	xmlXPathContext *xpath_context;
	xmlXPathObject *xpath_object;

	GList *track_mapper;

	instrument = NULL;
	sequence = NULL;
	
	xpath_context = xmlXPathNewContext(track_collection->midi_doc);
	//	      xmlXPathSetContextNode(node[i],
	//		     xpath_context);
	xpath_context->node = node[i];
	      
	xpath_object = xmlXPathEval((xmlChar *) "./midi-message[boolean(@instrument-name)]",
				    xpath_context);

	if(xpath_object->nodesetval != NULL){
	  instrument_node = xpath_object->nodesetval->nodeTab;
    
	  for(j = 0; j < xpath_object->nodesetval->nodeNr; j++){
	    if(instrument_node[j]->type == XML_ELEMENT_NODE){
	      instrument = xmlGetProp(instrument_node[j],
				      BAD_CAST "instrument-name");
	      break;
	    }
	  }
	}

	xpath_context = xmlXPathNewContext(track_collection->midi_doc);
	//	      xmlXPathSetContextNode(node[i],
	//		     xpath_context);
	xpath_context->node = node[i];
	      
	xpath_object = xmlXPathEval((xmlChar *) "./midi-message[boolean(@sequence-name)]",
				    xpath_context);
	
	if(xpath_object->nodesetval != NULL){
	  sequence_node = xpath_object->nodesetval->nodeTab;
    
	  for(j = 0; j < xpath_object->nodesetval->nodeNr; j++){
	    if(sequence_node[j]->type == XML_ELEMENT_NODE){
	      sequence = xmlGetProp(sequence_node[j],
				    BAD_CAST "sequence-name");
	      break;
	    }
	  }
	}

	if(instrument == NULL){
	  instrument = g_strdup("GSequencer - instrument default");
	}
	
	if(sequence == NULL){
	  sequence = g_strdup("GSequencer - sequence default");
	}

	
	start_list = ags_track_collection_get_track_mapper(track_collection);

	track_mapper = ags_track_mapper_find_instrument_with_sequence(start_list,
								      instrument, sequence);
	      
	if(track_mapper == NULL){
	  ags_track_collection_add_mapper(track_collection,
					  node[i],
					  instrument, sequence);
	}else{
	  g_object_set(AGS_TRACK_MAPPER(track_mapper->data),
		       "track", node[i],
		       NULL);
	}
	
	g_list_free(start_list);
      }
    }
  }

  //  xmlSaveFormatFileEnc("-", track_collection->midi_doc, "UTF-8", 1);
  
  /* map */
  list =
    start_list = ags_track_collection_get_track_mapper(track_collection);

  while(list != NULL){
    ags_track_mapper_map(list->data);

    gtk_widget_show(GTK_WIDGET(list->data));

    list = list->next;
  }

  g_list_free(start_list);
}

void
ags_track_collection_add_mapper(AgsTrackCollection *track_collection,
				xmlNode *track,
				gchar *instrument, gchar *sequence)
{
  AgsTrackMapper *track_mapper;

  if(track == NULL){
    return;
  }

  g_message("%s", instrument);
  g_message("%s", sequence);

  track_mapper = (AgsTrackMapper *) g_object_new_with_properties(track_collection->track_mapper_type,
								 track_collection->track_mapper_n_properties,
								 track_collection->track_mapper_strv,
								 track_collection->track_mapper_value);
  g_object_set(track_mapper,
	       "track", track,
	       "instrument", instrument,
	       "sequence", sequence,
	       NULL);

  ags_track_collection_add_track_mapper(track_collection,
					track_mapper);
}

/**
 * ags_track_collection_new:
 * @track_mapper_type: the track mapper type
 * @track_mapper_n_properties: the track mapper properties count
 * @track_mapper_strv: the track mapper string vector
 * @track_mapper_value: the track mapper value array
 *
 * Creates an #AgsTrackCollection
 *
 * Returns: a new #AgsTrackCollection
 *
 * Since: 3.0.0
 */
AgsTrackCollection*
ags_track_collection_new(GType track_mapper_type,
			 guint track_mapper_n_properties,
			 gchar **track_mapper_strv,
			 GValue *track_mapper_value)
{
  AgsTrackCollection *track_collection;

  track_collection = (AgsTrackCollection *) g_object_new(AGS_TYPE_TRACK_COLLECTION,
							 NULL);

  track_collection->track_mapper_type = track_mapper_type;
  track_collection->track_mapper_n_properties = track_mapper_n_properties;
  track_collection->track_mapper_strv = track_mapper_strv;
  track_collection->track_mapper_value = track_mapper_value;
  
  return(track_collection);
}
