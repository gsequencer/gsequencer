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

#include <ags/app/import/ags_track_mapper.h>
#include <ags/app/import/ags_track_mapper_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_navigation.h>

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

void ags_track_mapper_class_init(AgsTrackMapperClass *track_mapper);
void ags_track_mapper_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_track_mapper_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_track_mapper_init(AgsTrackMapper *track_mapper);
void ags_track_mapper_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_track_mapper_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_track_mapper_connect(AgsConnectable *connectable);
void ags_track_mapper_disconnect(AgsConnectable *connectable);
void ags_track_mapper_set_update(AgsApplicable *applicable, gboolean update);
void ags_track_mapper_apply(AgsApplicable *applicable);
void ags_track_mapper_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_track_mapper
 * @short_description: map track
 * @title: AgsTrackMapper
 * @section_id:
 * @include: ags/app/ags_track_mapper.h
 *
 * #AgsTrackMapper is a wizard to import midi files and do track mapping.
 */

enum{
  PROP_0,
  PROP_TRACK,
  PROP_INSTRUMENT,
  PROP_SEQUENCE,
};

GType
ags_track_mapper_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_track_mapper = 0;

    static const GTypeInfo ags_track_mapper_info = {
      sizeof (AgsTrackMapperClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_track_mapper_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTrackMapper),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_track_mapper_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_track_mapper_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_track_mapper_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_track_mapper = g_type_register_static(GTK_TYPE_GRID,
						   "AgsTrackMapper", &ags_track_mapper_info,
						   0);

    g_type_add_interface_static(ags_type_track_mapper,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_track_mapper,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_track_mapper);
  }

  return g_define_type_id__volatile;
}

void
ags_track_mapper_class_init(AgsTrackMapperClass *track_mapper)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  /* GObjectClass */
  gobject = (GObjectClass *) track_mapper;

  gobject->set_property = ags_track_mapper_set_property;
  gobject->get_property = ags_track_mapper_get_property;

  /* properties */
  /**
   * AgsTrackMapper:track:
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
   * AgsTrackMapper:instrument:
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
   * AgsTrackMapper:sequence:
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
ags_track_mapper_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_track_mapper_connect;
  connectable->disconnect = ags_track_mapper_disconnect;
}

void
ags_track_mapper_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_track_mapper_set_update;
  applicable->apply = ags_track_mapper_apply;
  applicable->reset = ags_track_mapper_reset;
}

void
ags_track_mapper_init(AgsTrackMapper *track_mapper)
{
  gchar *str;

  gtk_grid_set_column_spacing(track_mapper,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);
  gtk_grid_set_row_spacing(track_mapper,
			   AGS_UI_PROVIDER_DEFAULT_ROW_SPACING);
  
  track_mapper->instrument = NULL;
  track_mapper->sequence = NULL;

  track_mapper->track = NULL;

  track_mapper->notation = NULL;

  /* enabled */
  track_mapper->enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));

  gtk_widget_set_valign((GtkWidget *) track_mapper->enabled,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) track_mapper->enabled,
			GTK_ALIGN_FILL);
  
  gtk_grid_attach((GtkGrid *) track_mapper,
		  (GtkWidget *) track_mapper->enabled,
		  0, 0,
		  4, 1);

  /* info box */
  track_mapper->info = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
					      AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_valign((GtkWidget *) track_mapper->info,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) track_mapper->info,
			GTK_ALIGN_FILL);

  gtk_grid_attach((GtkGrid *) track_mapper,
		  (GtkWidget *) track_mapper->info,
		  0, 1,
		  4, 1);

  /* instrument */
  str = g_strdup_printf("%s: ",
			i18n("instrument"));
  
  track_mapper->info_instrument = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
							    "label", str,
							    "xalign", 0.0,
							    NULL);
  gtk_box_append(track_mapper->info,
		 (GtkWidget *) track_mapper->info_instrument);

  g_free(str);
  
  /* sequence */
  str = g_strdup_printf("%s: ",
			i18n("sequence"));
  
  track_mapper->info_sequence = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
							  "label", str,
							  "xalign", 0.0,
							  NULL);
  gtk_box_append(track_mapper->info,
		 (GtkWidget *) track_mapper->info_sequence);

  g_free(str);

  /* machine type */
  track_mapper->machine_type = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_widget_set_valign((GtkWidget *) track_mapper->machine_type,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) track_mapper->machine_type,
			GTK_ALIGN_FILL);

  gtk_grid_attach((GtkGrid *) track_mapper,
		  (GtkWidget *) track_mapper->machine_type,
		  1, 2,
		  1, 1);

  /* audio channels */
  //NOTE:JK: midi only knows 16 channels
  track_mapper->audio_channels = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 16.0, 1.0);
  gtk_spin_button_set_value(track_mapper->audio_channels,
			    2.0);

  gtk_widget_set_valign((GtkWidget *) track_mapper->audio_channels,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) track_mapper->audio_channels,
			GTK_ALIGN_FILL);

  gtk_grid_attach((GtkGrid *) track_mapper,
		  (GtkWidget *) track_mapper->audio_channels,
		  2, 2,
		  1, 1);

  /* offset */
  track_mapper->offset = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									  (gdouble) AGS_NAVIGATION_MAX_POSITION_TACT,
									  1.0);

  gtk_widget_set_valign((GtkWidget *) track_mapper->offset,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) track_mapper->offset,
			GTK_ALIGN_FILL);

  gtk_grid_attach((GtkGrid *) track_mapper,
		  (GtkWidget *) track_mapper->offset,
		  3, 2,
		  1, 1);
}

void
ags_track_mapper_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsTrackMapper *track_mapper;

  track_mapper = AGS_TRACK_MAPPER(gobject);

  switch(prop_id){
  case PROP_TRACK:
  {
    xmlNode *track;

    track = (xmlNode *) g_value_get_pointer(value);

    if(g_list_find(track_mapper->track,
		   track) != NULL){
      return;
    }

    track_mapper->track = g_list_prepend(track_mapper->track,
					 track);
  }
  break;
  case PROP_INSTRUMENT:
  {
    gchar *str;
    gchar *instrument;

    instrument = g_value_get_string(value);

    if(instrument == track_mapper->instrument){
      return;
    }

    if(track_mapper->instrument != NULL){
      g_free(track_mapper->instrument);
    }
      
    track_mapper->instrument = g_strdup(instrument);

    str = g_strdup_printf("%s: %s",
			  i18n("instrument"),
			  instrument);

    gtk_label_set_text(track_mapper->info_instrument,
		       str);

    g_free(str);
  }
  break;
  case PROP_SEQUENCE:
  {
    gchar *str;
    gchar *sequence;
      
    sequence = g_value_get_string(value);

    if(sequence == track_mapper->sequence){
      return;
    }

    if(track_mapper->sequence != NULL){
      g_free(track_mapper->sequence);
    }
      
    track_mapper->sequence = g_strdup(sequence);

    str = g_strdup_printf("%s: %s",
			  i18n("sequence"),
			  sequence);

    gtk_label_set_text(track_mapper->info_sequence,
		       str);

    g_free(str);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_track_mapper_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsTrackMapper *track_mapper;

  track_mapper = AGS_TRACK_MAPPER(gobject);

  switch(prop_id){
  case PROP_TRACK:
  {
    g_value_set_pointer(value, g_list_copy(track_mapper->track));
  }
  break;
  case PROP_INSTRUMENT:
  {
    g_value_set_string(value, track_mapper->instrument);
  }
  break;
  case PROP_SEQUENCE:
  {
    g_value_set_string(value, track_mapper->sequence);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_track_mapper_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_track_mapper_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_track_mapper_set_update(AgsApplicable *applicable, gboolean update)
{
  //TODO:JK: implement me
}

void
ags_track_mapper_apply(AgsApplicable *applicable)
{
  AgsWindow *window;
  AgsMachine *machine;
  AgsMidiImportWizard *midi_import_wizard;
  AgsTrackMapper *track_mapper;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;

  GObject *default_soundcard;
  
  GList *imported_notation;
  
  gchar *machine_type;
  
  track_mapper = AGS_TRACK_MAPPER(applicable);

  if(!gtk_toggle_button_get_active((GtkToggleButton *) track_mapper->enabled)){
    return;
  }  

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  midi_import_wizard = (AgsMidiImportWizard *) gtk_widget_get_ancestor((GtkWidget *) track_mapper,
								       AGS_TYPE_MIDI_IMPORT_WIZARD);

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create machine */
  machine = NULL;
  
  machine_type = gtk_combo_box_text_get_active_text(track_mapper->machine_type);
  
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

  ags_window_add_machine(window,
			 machine);

  /* connect everything */
  ags_connectable_connect(AGS_CONNECTABLE(machine));

  /* set size */
  ags_audio_set_audio_channels(machine->audio,
			       gtk_spin_button_get_value_as_int(track_mapper->audio_channels), 0);
  ags_audio_set_pads(machine->audio,
		     AGS_TYPE_OUTPUT,
		     1, 0);
  ags_audio_set_pads(machine->audio,
		     AGS_TYPE_INPUT,
		     128, 0);

  /* apply notation */
  imported_notation = track_mapper->notation;
  g_list_free_full(machine->audio->notation,
		   g_object_unref);
  
  machine->audio->notation = imported_notation;
  
  while(imported_notation != NULL){
    g_object_ref(imported_notation->data);
    
    imported_notation = imported_notation->next;
  }

  /* */
  gtk_widget_show(GTK_WIDGET(machine));
}

void
ags_track_mapper_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

/**
 * ags_track_mapper_find_instrument_with_sequence:
 * @track_mapper: a #GList containing #AgsTrackMapper
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
ags_track_mapper_find_instrument_with_sequence(GList *track_mapper,
					       gchar *instrument, gchar *sequence)
{
  if(instrument == NULL ||
     sequence == NULL){
    return(NULL);
  }
  
  while(track_mapper != NULL){
    if((!g_ascii_strcasecmp(AGS_TRACK_MAPPER(track_mapper->data)->instrument,
			    instrument) &&
	!g_ascii_strcasecmp(AGS_TRACK_MAPPER(track_mapper->data)->sequence,
			    sequence))){
      return(track_mapper);
    }

    track_mapper = track_mapper->next;
  }
  
  return(NULL);
}

/**
 * ags_track_mapper_map:
 * @track_mapper: an #AgsTrackMapper
 *
 * Maps XML tracks to #AgsNotation
 *
 * Since: 3.0.0
 */
void
ags_track_mapper_map(AgsTrackMapper *track_mapper)
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
  
  track_collection = (AgsTrackCollection *) gtk_widget_get_ancestor((GtkWidget *) track_mapper,
								    AGS_TYPE_TRACK_COLLECTION);

  track = track_mapper->track;

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

  track_mapper->notation = notation_start;
  
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
  gtk_combo_box_text_append_text(track_mapper->machine_type,
				 g_type_name(AGS_TYPE_SYNCSYNTH));

#ifdef AGS_WITH_LIBINSTPATCH
  gtk_combo_box_text_append_text(track_mapper->machine_type,
				 g_type_name(AGS_TYPE_FFPLAYER));
#endif
  
  gtk_combo_box_text_append_text(track_mapper->machine_type,
				 g_type_name(AGS_TYPE_DRUM));
    
  gtk_combo_box_text_append_text(track_mapper->machine_type,
				 g_type_name(AGS_TYPE_MATRIX));

  gtk_combo_box_set_active(GTK_COMBO_BOX(track_mapper->machine_type),
			   0);
}

/**
 * ags_track_mapper_new:
 *
 * Creates an #AgsTrackMapper
 *
 * Returns: a new #AgsTrackMapper
 *
 * Since: 3.0.0
 */
AgsTrackMapper*
ags_track_mapper_new()
{
  AgsTrackMapper *track_mapper;

  track_mapper = (AgsTrackMapper *) g_object_new(AGS_TYPE_TRACK_MAPPER,
						 NULL);
  
  return(track_mapper);
}
