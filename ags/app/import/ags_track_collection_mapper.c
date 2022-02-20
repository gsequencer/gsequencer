/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/app/import/ags_track_collection_mapper.h>
#include <ags/app/import/ags_track_collection_mapper_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>

#include <ags/app/import/ags_midi_import_wizard.h>
#include <ags/app/import/ags_track_collection.h>

#include <ags/app/editor/ags_notation_edit.h>

#include <ags/app/machine/ags_drum.h>
#include <ags/app/machine/ags_matrix.h>
#include <ags/app/machine/ags_synth.h>
#include <ags/app/machine/ags_syncsynth.h>

#ifdef AGS_WITH_LIBINSTPATCH
#include <ags/app/machine/ags_ffplayer.h>
#endif

#include <math.h>

#include <ags/i18n.h>

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
 * @include: ags/app/ags_track_collection_mapper.h
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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_track_collection_mapper = 0;

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

    ags_type_track_collection_mapper = g_type_register_static(GTK_TYPE_GRID,
							      "AgsTrackCollectionMapper", &ags_track_collection_mapper_info,
							      0);

    g_type_add_interface_static(ags_type_track_collection_mapper,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_track_collection_mapper,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_track_collection_mapper);
  }

  return g_define_type_id__volatile;
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
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("track",
				    i18n_pspec("assigned track"),
				    i18n_pspec("The track which this track mapper is assigned with"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TRACK,
				  param_spec);

  /**
   * AgsTrackCollectionMapper:instrument:
   *
   * The instruments as string to parse.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("instrument",
				   i18n_pspec("assigned instrument"),
				   i18n_pspec("The instrument which this track mapper is assigned with"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INSTRUMENT,
				  param_spec);

  /**
   * AgsTrackCollectionMapper:sequence:
   *
   * The sequences as string to parse.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("sequence",
				   i18n_pspec("assigned sequence"),
				   i18n_pspec("The sequence which this track mapper is assigned with"),
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
  
  track_collection_mapper->instrument = NULL;
  track_collection_mapper->sequence = NULL;

  track_collection_mapper->track = NULL;

  track_collection_mapper->notation = NULL;

  /* enabled */
  track_collection_mapper->enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));

  gtk_widget_set_valign((GtkWidget *) track_collection_mapper->enabled,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) track_collection_mapper->enabled,
			GTK_ALIGN_FILL);
  
  gtk_grid_attach((GtkGrid *) track_collection_mapper,
		  (GtkWidget *) track_collection_mapper->enabled,
		  0, 0,
		  4, 1);

  /* info box */
  track_collection_mapper->info = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
							 0);

  gtk_widget_set_valign((GtkWidget *) track_collection_mapper->info,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) track_collection_mapper->info,
			GTK_ALIGN_FILL);

  gtk_grid_attach((GtkGrid *) track_collection_mapper,
		  (GtkWidget *) track_collection_mapper->info,
		  0, 1,
		  4, 1);

  /* instrument */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("instrument: "),
				    "xalign", 0.0,
				    NULL);
  gtk_box_pack_start(GTK_BOX(track_collection_mapper->info),
		     GTK_WIDGET(label),
		     TRUE, TRUE,
		     AGS_UI_PROVIDER_DEFAULT_PADDING);

  /* sequence */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("sequence: "),
				    "xalign", 0.0,
				    NULL);
  gtk_box_pack_start(GTK_BOX(track_collection_mapper->info),
		     GTK_WIDGET(label),
		     TRUE, TRUE,
		     AGS_UI_PROVIDER_DEFAULT_PADDING);

  /* machine type */
  track_collection_mapper->machine_type = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_widget_set_valign((GtkWidget *) track_collection_mapper->machine_type,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) track_collection_mapper->machine_type,
			GTK_ALIGN_FILL);

  gtk_grid_attach((GtkGrid *) track_collection_mapper,
		  (GtkWidget *) track_collection_mapper->machine_type,
		  1, 2,
		  1, 1);

  /* audio channels */
  //NOTE:JK: midi only knows 16 channels
  track_collection_mapper->audio_channels = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 16.0, 1.0);
  gtk_spin_button_set_value(track_collection_mapper->audio_channels,
			    2.0);

  gtk_widget_set_valign((GtkWidget *) track_collection_mapper->audio_channels,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) track_collection_mapper->audio_channels,
			GTK_ALIGN_FILL);

  gtk_grid_attach((GtkGrid *) track_collection_mapper,
		  (GtkWidget *) track_collection_mapper->audio_channels,
		  2, 2,
		  1, 1);

  /* offset */
  track_collection_mapper->offset = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
										     (gdouble) AGS_NOTATION_EDITOR_MAX_CONTROLS,
										     1.0);

  gtk_widget_set_valign((GtkWidget *) track_collection_mapper->offset,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) track_collection_mapper->offset,
			GTK_ALIGN_FILL);

  gtk_grid_attach((GtkGrid *) track_collection_mapper,
		  (GtkWidget *) track_collection_mapper->offset,
		  3, 2,
		  1, 1);
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

      gchar *str;
      gchar *instrument;

      instrument = g_value_get_string(value);

      if(instrument == track_collection_mapper->instrument){
	return;
      }

      if(track_collection_mapper->instrument != NULL){
	g_free(track_collection_mapper->instrument);
      }
      
      track_collection_mapper->instrument = g_strdup(instrument);

      list_start =
	list = gtk_container_get_children((GtkContainer *) track_collection_mapper->info);

      str = g_strdup_printf("%s: %s",
			    i18n("instrument"),
			    instrument);
      gtk_label_set_text(list->data,
			 str);

      g_list_free(list_start);
      g_free(str);
    }
    break;
  case PROP_SEQUENCE:
    {
      GList *list, *list_start;

      gchar *str;
      gchar *sequence;
      
      sequence = g_value_get_string(value);

      if(sequence == track_collection_mapper->sequence){
	return;
      }

      if(track_collection_mapper->sequence != NULL){
	g_free(track_collection_mapper->sequence);
      }
      
      track_collection_mapper->sequence = g_strdup(sequence);

      list_start =
	list = gtk_container_get_children((GtkContainer *) track_collection_mapper->info);
      list = list->next;

      str = g_strdup_printf("%s: %s",
			    i18n("sequence"),
			    sequence);
      gtk_label_set_text(list->data,
			 str);

      g_list_free(list_start);
      g_free(str);
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
    {
      g_value_set_pointer(value, g_list_copy(track_collection_mapper->track));
    }
    break;
  case PROP_INSTRUMENT:
    {
      g_value_set_string(value, track_collection_mapper->instrument);
    }
    break;
  case PROP_SEQUENCE:
    {
      g_value_set_string(value, track_collection_mapper->sequence);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_track_collection_mapper_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_track_collection_mapper_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_track_collection_mapper_set_update(AgsApplicable *applicable, gboolean update)
{
  //TODO:JK: implement me
}

void
ags_track_collection_mapper_apply(AgsApplicable *applicable)
{
  AgsWindow *window;
  AgsMachine *machine;
  AgsMidiImportWizard *midi_import_wizard;
  AgsTrackCollectionMapper *track_collection_mapper;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;

  GObject *default_soundcard;
  
  GList *imported_notation;
  
  gchar *machine_type;
  
  track_collection_mapper = AGS_TRACK_COLLECTION_MAPPER(applicable);

  if(!gtk_toggle_button_get_active((GtkToggleButton *) track_collection_mapper->enabled)){
    return;
  }  

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  midi_import_wizard = (AgsMidiImportWizard *) gtk_widget_get_ancestor((GtkWidget *) track_collection_mapper,
								       AGS_TYPE_MIDI_IMPORT_WIZARD);

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create machine */
  machine = NULL;
  
  machine_type = gtk_combo_box_text_get_active_text(track_collection_mapper->machine_type);
  
  if(!g_ascii_strcasecmp(machine_type,
			 g_type_name(AGS_TYPE_DRUM))){
    machine = (AgsMachine *) ags_drum_new(default_soundcard);
  }else if(!g_ascii_strcasecmp(machine_type,
			       g_type_name(AGS_TYPE_MATRIX))){
    machine = (AgsMachine *) ags_matrix_new(default_soundcard);
  }else if(!g_ascii_strcasecmp(machine_type,
			       g_type_name(AGS_TYPE_SYNCSYNTH))){
    machine = (AgsMachine *) ags_syncsynth_new(default_soundcard);
#ifdef AGS_WITH_LIBINSTPATCH
  }else if(!g_ascii_strcasecmp(machine_type,
			       g_type_name(AGS_TYPE_FFPLAYER))){
    machine = (AgsMachine *) ags_ffplayer_new(default_soundcard);
#endif
  }else if(!g_ascii_strcasecmp(machine_type,
			       g_type_name(AGS_TYPE_SYNTH))){
    machine = (AgsMachine *) ags_synth_new(default_soundcard);
  }else{
    g_warning("unknown machine type");

    return;
  }

  /* add audio */  
  add_audio = ags_add_audio_new(machine->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);
  
  gtk_box_pack_start((GtkBox *) window->machines,
		     GTK_WIDGET(machine),
		     FALSE, FALSE,
		     0);

  /* connect everything */
  ags_connectable_connect(AGS_CONNECTABLE(machine));

  /* set size */
  ags_audio_set_audio_channels(machine->audio,
			       gtk_spin_button_get_value_as_int(track_collection_mapper->audio_channels), 0);
  ags_audio_set_pads(machine->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(machine->audio,
		     AGS_TYPE_INPUT,
		     128, 0);

  /* apply notation */
  imported_notation = track_collection_mapper->notation;
  g_list_free_full(machine->audio->notation,
		   g_object_unref);
  
  machine->audio->notation = imported_notation;
  
  while(imported_notation != NULL){
    g_object_ref(imported_notation->data);
    
    imported_notation = imported_notation->next;
  }

  /* */
  gtk_widget_show_all(GTK_WIDGET(machine));
}

void
ags_track_collection_mapper_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

/**
 * ags_track_collection_mapper_find_instrument_with_sequence:
 * @track_collection_mapper: a #GList containing #AgsTrackCollectionMapper
 * @instrument: the instrument as string
 * @sequence: the sequence as string
 *
 * Finds next matching track in a #GList.
 *
 * Returns: the next matching #GList
 *
 * Since: 3.0.0
 */
GList*
ags_track_collection_mapper_find_instrument_with_sequence(GList *track_collection_mapper,
							  gchar *instrument, gchar *sequence)
{
  if(instrument == NULL ||
     sequence == NULL){
    return(NULL);
  }
  
  while(track_collection_mapper != NULL){
    if((!g_ascii_strcasecmp(AGS_TRACK_COLLECTION_MAPPER(track_collection_mapper->data)->instrument,
			    instrument) &&
	!g_ascii_strcasecmp(AGS_TRACK_COLLECTION_MAPPER(track_collection_mapper->data)->sequence,
			    sequence))){
      return(track_collection_mapper);
    }

    track_collection_mapper = track_collection_mapper->next;
  }
  
  return(NULL);
}

/**
 * ags_track_collection_mapper_map:
 * @track_collection_mapper: an #AgsTrackCollectionMapper
 *
 * Maps XML tracks to #AgsNotation
 *
 * Since: 3.0.0
 */
void
ags_track_collection_mapper_map(AgsTrackCollectionMapper *track_collection_mapper)
{
  AgsTrackCollection *track_collection;

  AgsNotation *current_notation;
  AgsNote *note;
  
  AgsTimestamp *timestamp;

  xmlNode *current, *child;
  GList *track, *notation_start, *notation;
  GList *list;

  gchar *segmentation;

  gdouble delay_factor;
  guint audio_channels;
  guint n_key_on, n_key_off;
  guint x, y, velocity;
  guint prev_x;
  guint default_length;
  guint i;
  gboolean pattern;
  
  track_collection = (AgsTrackCollection *) gtk_widget_get_ancestor((GtkWidget *) track_collection_mapper,
								    AGS_TYPE_TRACK_COLLECTION);

  track = track_collection_mapper->track;

  /* map notation */
  notation_start =
    notation = NULL;

  audio_channels = 1;
  
  for(i = 0; i < audio_channels; i++){
    current_notation = ags_notation_new(NULL,
					i);
    notation_start = ags_notation_add(notation_start,
				      current_notation);
  }

  track_collection_mapper->notation = notation_start;
  
  n_key_on = 0;
  n_key_off = 0;

  default_length = track_collection->default_length;

  if(default_length == 0){
    default_length = 1;
  }

  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  timestamp->timer.ags_offset.offset = 0;
  
  prev_x = 0;

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
  
  while(track != NULL){
    current = track->data;
    
    child = current->children;

    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	xmlChar *str;

	glong delta_time;

	if(!xmlStrncmp(xmlGetProp(child,
				  BAD_CAST "event"),
		       "note-on",
		       8)){
	  str = xmlGetProp(child,
			   BAD_CAST "delta-time");
	  delta_time = g_ascii_strtod(str,
				      NULL);

	  xmlFree(str);
	  
	  x = ags_midi_util_delta_time_to_offset(delay_factor,
						 track_collection->division,
						 track_collection->tempo,
						 (glong) track_collection->bpm,
						 delta_time);
	  x -= track_collection->first_offset;

	  str = xmlGetProp(child,
			   BAD_CAST "note");
	  y = (guint) g_ascii_strtoull(str,
				       NULL,
				       10);
	  xmlFree(str);

	  str = xmlGetProp(child,
			   BAD_CAST "velocity");
	  velocity = (guint) g_ascii_strtoull(str,
					      NULL,
					      10);
	  xmlFree(str);

	  notation = notation_start;
	  
	  for(i = 0; i < audio_channels; i++){
	    note = ags_note_new();
	    note->x[0] = x;
	    note->x[1] = x + default_length;
	    note->y = y;

	    /* velocity */
#if 0	    
	    note->attack.imag = (gdouble) velocity / 127.0;
#endif
	    
	    if(x >= prev_x + AGS_NOTATION_DEFAULT_OFFSET){
	      current_notation = ags_notation_new(NULL,
						  i);
	      ags_timestamp_set_ags_offset(current_notation->timestamp,
					   AGS_NOTATION_DEFAULT_OFFSET * floor(x / AGS_NOTATION_DEFAULT_OFFSET));
	      
	      notation_start = ags_notation_add(notation_start,
						current_notation);

	    }else{
	      ags_timestamp_set_ags_offset(timestamp,
					   AGS_NOTATION_DEFAULT_OFFSET * floor(x / AGS_NOTATION_DEFAULT_OFFSET));

	      notation = ags_notation_find_near_timestamp(notation_start, i,
							  timestamp);
	      current_notation = notation->data;
	    }
	    
	    ags_notation_add_note(current_notation,
				  note,
				  FALSE);
	    
	    notation = notation->next;
	  }

	  //	  g_object_unref(note);
	  n_key_on++;
	}else if(!xmlStrncmp(xmlGetProp(child,
					BAD_CAST "event"),
			     "note-off",
			     9)){	  
	  str = xmlGetProp(child,
			   BAD_CAST "delta-time");
	  delta_time = g_ascii_strtod(str,
				      NULL);

	  xmlFree(str);
	  
	  x = ags_midi_util_delta_time_to_offset(delay_factor,
						 track_collection->division,
						 track_collection->tempo,
						 (glong) track_collection->bpm,
						 delta_time);
	  x -= track_collection->first_offset;

	  str = xmlGetProp(child,
			   BAD_CAST "note");
	  y = (guint) g_ascii_strtoull(str,
				       NULL,
				       10);
	  xmlFree(str);

	  str = xmlGetProp(child,
			   BAD_CAST "velocity");
	  velocity = (guint) g_ascii_strtoull(str,
					      NULL,
					      10);
	  xmlFree(str);
	  
	  for(i = 0; i < audio_channels; i++){
	    notation = g_list_last(notation_start);

	    while(notation != NULL){
	      list = ags_note_find_prev(AGS_NOTATION(notation->data)->note,
					x, y);

	      if(list != NULL){
		note = list->data;

		if(note->x[0] == x){
		  note->x[1] = x + 1;
		}else{
		  note->x[1] = x;
		}
	      
		note->y = y;

		/* velocity */
#if 0	    
		note->release.imag = (gdouble) velocity / 127.0;
#endif
		
		break;
	      }
	    
	      notation = notation->prev;
	    }
	  }
	  
	  n_key_off++;
	}
      }

      child = child->next;
    }
    
    track = track->next;
  }

  g_object_unref(timestamp);
  
  /* populate machine_type */
  gtk_combo_box_text_append_text(track_collection_mapper->machine_type,
				 g_type_name(AGS_TYPE_SYNCSYNTH));

#ifdef AGS_WITH_LIBINSTPATCH
  gtk_combo_box_text_append_text(track_collection_mapper->machine_type,
				 g_type_name(AGS_TYPE_FFPLAYER));
#endif
  
  gtk_combo_box_text_append_text(track_collection_mapper->machine_type,
				 g_type_name(AGS_TYPE_DRUM));
    
  gtk_combo_box_text_append_text(track_collection_mapper->machine_type,
				 g_type_name(AGS_TYPE_MATRIX));

  gtk_combo_box_set_active(GTK_COMBO_BOX(track_collection_mapper->machine_type),
			   0);
}

/**
 * ags_track_collection_mapper_new:
 *
 * Creates an #AgsTrackCollectionMapper
 *
 * Returns: a new #AgsTrackCollectionMapper
 *
 * Since: 3.0.0
 */
AgsTrackCollectionMapper*
ags_track_collection_mapper_new()
{
  AgsTrackCollectionMapper *track_collection_mapper;

  track_collection_mapper = (AgsTrackCollectionMapper *) g_object_new(AGS_TYPE_TRACK_COLLECTION_MAPPER,
								      NULL);
  
  return(track_collection_mapper);
}
