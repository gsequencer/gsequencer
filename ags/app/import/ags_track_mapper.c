/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/ags_api_config.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_machine_util.h>
#include <ags/app/ags_navigation.h>

#include <ags/app/import/ags_track_collection.h>

#include <ags/app/editor/ags_notation_edit.h>

#include <ags/app/machine/ags_drum.h>
#include <ags/app/machine/ags_matrix.h>
#include <ags/app/machine/ags_syncsynth.h>
#include <ags/app/machine/ags_fm_syncsynth.h>
#include <ags/app/machine/ags_pitch_sampler.h>
#include <ags/app/machine/ags_sfz_synth.h>

#ifdef AGS_WITH_LIBINSTPATCH
#include <ags/app/machine/ags_ffplayer.h>
#include <ags/app/machine/ags_sf2_synth.h>
#endif

#include <ags/app/machine/ags_hybrid_synth.h>
#include <ags/app/machine/ags_hybrid_fm_synth.h>
#include <ags/app/machine/ags_stargazer_synth.h>
#include <ags/app/machine/ags_quantum_synth.h>
#include <ags/app/machine/ags_raven_synth.h>

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
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
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

    g_once_init_leave(&g_define_type_id__static, ags_type_track_mapper);
  }

  return(g_define_type_id__static);
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
  GtkLabel *label;

  gchar *str;

  gtk_grid_set_column_spacing((GtkGrid *) track_mapper,
			      AGS_UI_PROVIDER_DEFAULT_COLUMN_SPACING);

  gtk_grid_set_row_spacing((GtkGrid *) track_mapper,
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
  label = (GtkLabel *) gtk_label_new(i18n("soft synth"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach((GtkGrid *) track_mapper,
		  (GtkWidget *) label,
		  0, 2,
		  1, 1);

  track_mapper->machine_type = (GtkComboBoxText *) gtk_combo_box_text_new();

  gtk_combo_box_text_append_text(track_mapper->machine_type,
				 "AgsDrum");
  gtk_combo_box_text_append_text(track_mapper->machine_type,
				 "AgsMatrix");
  gtk_combo_box_text_append_text(track_mapper->machine_type,
				 "AgsSyncsynth");
  gtk_combo_box_text_append_text(track_mapper->machine_type,
				 "AgsFMSyncsynth");
  gtk_combo_box_text_append_text(track_mapper->machine_type,
				 "AgsPitchSampler");
  gtk_combo_box_text_append_text(track_mapper->machine_type,
				 "AgsSFZSynth");
  gtk_combo_box_text_append_text(track_mapper->machine_type,
				 "AgsFFPlayer");
  gtk_combo_box_text_append_text(track_mapper->machine_type,
				 "AgsSF2Synth");
  gtk_combo_box_text_append_text(track_mapper->machine_type,
				 "AgsHybridSynth");
  gtk_combo_box_text_append_text(track_mapper->machine_type,
				 "AgsHybridFMSynth");
  gtk_combo_box_text_append_text(track_mapper->machine_type,
				 "AgsStargazerSynth");
  gtk_combo_box_text_append_text(track_mapper->machine_type,
				 "AgsQuantumSynth");
  gtk_combo_box_text_append_text(track_mapper->machine_type,
				 "AgsRavenSynth");
  
  gtk_widget_set_valign((GtkWidget *) track_mapper->machine_type,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) track_mapper->machine_type,
			GTK_ALIGN_FILL);

  gtk_grid_attach((GtkGrid *) track_mapper,
		  (GtkWidget *) track_mapper->machine_type,
		  1, 2,
		  1, 1);

  gtk_combo_box_set_active((GtkComboBox *) track_mapper->machine_type,
			   7);

  /* midi channel */
  label = (GtkLabel *) gtk_label_new(i18n("MIDI channel"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach((GtkGrid *) track_mapper,
		  (GtkWidget *) label,
		  0, 3,
		  1, 1);

  //NOTE:JK: midi only knows 16 channels, -1 means ignore channel information
  track_mapper->midi_channel = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0, 16.0, 1.0);
  gtk_spin_button_set_value(track_mapper->midi_channel,
			    -1.0);

  gtk_widget_set_valign((GtkWidget *) track_mapper->midi_channel,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) track_mapper->midi_channel,
			GTK_ALIGN_FILL);

  gtk_grid_attach((GtkGrid *) track_mapper,
		  (GtkWidget *) track_mapper->midi_channel,
		  1, 3,
		  1, 1);

  /* audio channels */
  label = (GtkLabel *) gtk_label_new(i18n("audio channels"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach((GtkGrid *) track_mapper,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);

  track_mapper->audio_channels = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 16.0, 1.0);
  gtk_spin_button_set_value(track_mapper->audio_channels,
			    2.0);

  gtk_widget_set_valign((GtkWidget *) track_mapper->audio_channels,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) track_mapper->audio_channels,
			GTK_ALIGN_FILL);

  gtk_grid_attach((GtkGrid *) track_mapper,
		  (GtkWidget *) track_mapper->audio_channels,
		  1, 4,
		  1, 1);

  /* offset */
  label = (GtkLabel *) gtk_label_new(i18n("offset"));
  gtk_label_set_xalign(label,
		       0.0);
  gtk_grid_attach((GtkGrid *) track_mapper,
		  (GtkWidget *) label,
		  0, 5,
		  1, 1);

  track_mapper->offset = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
									  (gdouble) AGS_NAVIGATION_MAX_POSITION_TACT,
									  1.0);

  gtk_widget_set_valign((GtkWidget *) track_mapper->offset,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) track_mapper->offset,
			GTK_ALIGN_FILL);

  gtk_grid_attach((GtkGrid *) track_mapper,
		  (GtkWidget *) track_mapper->offset,
		  1, 5,
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
  AgsTrackMapper *track_mapper;

  AgsAddAudio *add_audio;

  AgsApplicationContext *application_context;

  GObject *default_soundcard;
  
  GList *imported_notation;
  
  gchar *machine_type;

  gint channels;
  
  track_mapper = AGS_TRACK_MAPPER(applicable);

  if(!gtk_check_button_get_active(track_mapper->enabled)){
    return;
  }  

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  /* create machine */
  machine = NULL;
  
  machine_type = gtk_combo_box_text_get_active_text(track_mapper->machine_type);

  channels = gtk_spin_button_get_value_as_int(track_mapper->audio_channels);
  
  if(!g_ascii_strcasecmp(machine_type,
			 g_type_name(AGS_TYPE_DRUM))){
    machine = (AgsMachine *) ags_machine_util_new_drum();

    /* set size */
    ags_audio_set_audio_channels(machine->audio,
				 channels, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_OUTPUT,
		       1, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_INPUT,
		       128, 0);
  }else if(!g_ascii_strcasecmp(machine_type,
			       g_type_name(AGS_TYPE_MATRIX))){
    machine = (AgsMachine *) ags_machine_util_new_matrix();

    /* set size */
    ags_audio_set_audio_channels(machine->audio,
				 1, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_OUTPUT,
		       channels, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_INPUT,
		       128, 0);
  }else if(!g_ascii_strcasecmp(machine_type,
			       g_type_name(AGS_TYPE_SYNCSYNTH))){
    machine = (AgsMachine *) ags_machine_util_new_syncsynth();

    /* set size */
    ags_audio_set_audio_channels(machine->audio,
				 1, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_OUTPUT,
		       channels, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_INPUT,
		       128, 0);
  }else if(!g_ascii_strcasecmp(machine_type,
			       g_type_name(AGS_TYPE_FM_SYNCSYNTH))){
    machine = (AgsMachine *) ags_machine_util_new_fm_syncsynth();

    /* set size */
    ags_audio_set_audio_channels(machine->audio,
				 1, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_OUTPUT,
		       channels, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_INPUT,
		       128, 0);
  }else if(!g_ascii_strcasecmp(machine_type,
			       g_type_name(AGS_TYPE_PITCH_SAMPLER))){
    machine = (AgsMachine *) ags_machine_util_new_pitch_sampler();

    /* set size */
    ags_audio_set_audio_channels(machine->audio,
				 channels, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_OUTPUT,
		       1, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_INPUT,
		       128, 0);
  }else if(!g_ascii_strcasecmp(machine_type,
			       g_type_name(AGS_TYPE_SFZ_SYNTH))){
    machine = (AgsMachine *) ags_machine_util_new_sfz_synth();

    /* set size */
    ags_audio_set_audio_channels(machine->audio,
				 channels, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_OUTPUT,
		       1, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_INPUT,
		       128, 0);
#ifdef AGS_WITH_LIBINSTPATCH
  }else if(!g_ascii_strcasecmp(machine_type,
			       g_type_name(AGS_TYPE_FFPLAYER))){
    machine = (AgsMachine *) ags_machine_util_new_ffplayer();

    /* set size */
    ags_audio_set_audio_channels(machine->audio,
				 channels, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_OUTPUT,
		       1, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_INPUT,
		       128, 0);
  }else if(!g_ascii_strcasecmp(machine_type,
			       g_type_name(AGS_TYPE_SF2_SYNTH))){
    machine = (AgsMachine *) ags_machine_util_new_sf2_synth();

    /* set size */
    ags_audio_set_audio_channels(machine->audio,
				 channels, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_OUTPUT,
		       1, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_INPUT,
		       128, 0);
#endif
  }else if(!g_ascii_strcasecmp(machine_type,
			       g_type_name(AGS_TYPE_HYBRID_SYNTH))){
    machine = (AgsMachine *) ags_machine_util_new_hybrid_synth();

    /* set size */
    ags_audio_set_audio_channels(machine->audio,
				 1, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_OUTPUT,
		       channels, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_INPUT,
		       128, 0);
  }else if(!g_ascii_strcasecmp(machine_type,
			       g_type_name(AGS_TYPE_HYBRID_FM_SYNTH))){
    machine = (AgsMachine *) ags_machine_util_new_hybrid_fm_synth();

    /* set size */
    ags_audio_set_audio_channels(machine->audio,
				 1, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_OUTPUT,
		       channels, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_INPUT,
		       128, 0);
  }else if(!g_ascii_strcasecmp(machine_type,
			       g_type_name(AGS_TYPE_STARGAZER_SYNTH))){
    machine = (AgsMachine *) ags_machine_util_new_stargazer_synth();

    /* set size */
    ags_audio_set_audio_channels(machine->audio,
				 1, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_OUTPUT,
		       channels, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_INPUT,
		       128, 0);
  }else if(!g_ascii_strcasecmp(machine_type,
			       g_type_name(AGS_TYPE_QUANTUM_SYNTH))){
    machine = (AgsMachine *) ags_machine_util_new_quantum_synth();

    /* set size */
    ags_audio_set_audio_channels(machine->audio,
				 1, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_OUTPUT,
		       channels, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_INPUT,
		       128, 0);
  }else if(!g_ascii_strcasecmp(machine_type,
			       g_type_name(AGS_TYPE_RAVEN_SYNTH))){
    machine = (AgsMachine *) ags_machine_util_new_raven_synth();

    /* set size */
    ags_audio_set_audio_channels(machine->audio,
				 1, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_OUTPUT,
		       channels, 0);
    ags_audio_set_pads(machine->audio,
		       AGS_TYPE_INPUT,
		       128, 0);
  }else{
    g_warning("unknown machine type");

    return;
  }

  /* add audio */  
  add_audio = ags_add_audio_new(machine->audio);
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) add_audio);

  /* apply notation */
  imported_notation = track_mapper->notation;
  g_list_free_full(machine->audio->notation,
		   g_object_unref);
  
  machine->audio->notation = NULL;
  
  while(imported_notation != NULL){
    ags_audio_add_notation(machine->audio,
			   imported_notation->data);
    
    imported_notation = imported_notation->next;
  }

  g_list_free(track_mapper->notation);

  track_mapper->notation = NULL;
  
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
    if((AGS_TRACK_MAPPER(track_mapper->data)->instrument != NULL &&
	!g_ascii_strcasecmp(AGS_TRACK_MAPPER(track_mapper->data)->instrument,
			    instrument) &&
	AGS_TRACK_MAPPER(track_mapper->data)->sequence != NULL &&
	!g_ascii_strcasecmp(AGS_TRACK_MAPPER(track_mapper->data)->sequence,
			    sequence))){
      return(track_mapper);
    }

    track_mapper = track_mapper->next;
  }
  
  return(NULL);
}

/**
 * ags_track_mapper_find_midi_channel:
 * @track_mapper: a #GList-struct containing #AgsTrackMapper
 * @midi_channel: the MIDI channel valid 0 through 15
 *
 * Finds next matching track in a #GList-struct.
 *
 * Returns: the next matching #GList-struct
 *
 * Since: 6.16.17
 */
GList*
ags_track_mapper_find_midi_channel(GList *track_mapper,
				   gint midi_channel)
{
  while(track_mapper != NULL){
    if((gint) gtk_spin_button_get_value(AGS_TRACK_MAPPER(track_mapper->data)->midi_channel) == midi_channel){
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

  AgsMidiUtil midi_util;  

  xmlNode *current, *child;
  GList *track, *notation_start, *notation;
  GList *list;

  gchar *segmentation;

  gint midi_channel;
  gdouble delay_factor;
  guint audio_channels;
  guint n_key_on, n_key_off;
  guint x, y, velocity;
  guint x_256th;
  guint default_length;
  guint i;
  gboolean pattern;
  
  track_collection = (AgsTrackCollection *) gtk_widget_get_ancestor((GtkWidget *) track_mapper,
								    AGS_TYPE_TRACK_COLLECTION);

  track = track_mapper->track;

  midi_util.major = 1;
  midi_util.minor = 0;  

  midi_channel = gtk_spin_button_get_value_as_int(track_mapper->midi_channel);
  
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
    
    delay_factor = 1.0 / (16.0 / (double) denominator);

    g_free(segmentation);
  }
  
  while(track != NULL){
    current = track->data;
    
    child = current->children;

    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	xmlChar *event;
	xmlChar *key;
	xmlChar *str;

	glong delta_time;

	event = xmlGetProp(child,
			   BAD_CAST "event");
	
	if(!xmlStrncmp(event,
		       "note-on",
		       8)){
	  key = NULL;
	  
	  if(midi_channel >= 0){
	    key = xmlGetProp(child,
			     BAD_CAST "key");

	    if(midi_channel != (gint) g_ascii_strtoll(key,
						      NULL,
						      10)){
	      child = child->next;
	      
	      xmlFree(event);
	      xmlFree(key);
	      
	      continue;
	    }
	  }
	  
	  str = xmlGetProp(child,
			   BAD_CAST "delta-time");

	  delta_time = g_ascii_strtod(str,
				      NULL);

	  xmlFree(str);
	  
	  x = ags_midi_util_delta_time_to_offset(&midi_util,
						 delay_factor,
						 track_collection->division,
						 track_collection->tempo,
						 (glong) track_collection->bpm,
						 delta_time);
	  x -= track_collection->first_offset;

	  x_256th = ags_midi_util_delta_time_to_note_256th_offset(&midi_util,
								  delay_factor,
								  track_collection->division,
								  track_collection->tempo,
								  (glong) track_collection->bpm,
								  delta_time);
	  x_256th -= track_collection->first_note_256th_offset;
	  
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
	    note = ags_note_new();
	    note->x[0] = x;
	    note->x[1] = x + default_length;

	    note->x_256th[0] = x_256th;
	    note->x_256th[1] = x_256th + 16 * default_length;

	    note->y = y;

	    /* velocity */
#if 0	    
	    note->attack.imag = (gdouble) velocity / 127.0;
#endif

	    ags_timestamp_set_ags_offset(timestamp,
					 AGS_NOTATION_DEFAULT_OFFSET * floor(x / AGS_NOTATION_DEFAULT_OFFSET));

	    notation = ags_notation_find_near_timestamp(notation_start, i,
							timestamp);
	    
	    if(notation == NULL){
	      current_notation = ags_notation_new(NULL,
						  i);
	      ags_timestamp_set_ags_offset(current_notation->timestamp,
					   AGS_NOTATION_DEFAULT_OFFSET * floor(x / AGS_NOTATION_DEFAULT_OFFSET));
	      
	      notation_start = ags_notation_add(notation_start,
						current_notation);
	    }else{
	      current_notation = notation->data;
	    }
	    
	    ags_notation_add_note(current_notation,
				  note,
				  FALSE);
	  }

	  //	  g_object_unref(note);
	  n_key_on++;
	}else if(!xmlStrncmp(event,
			     "note-off",
			     9)){	  
	  key = NULL;
	  
	  if(midi_channel >= 0){
	    key = xmlGetProp(child,
			     BAD_CAST "key");

	    if(midi_channel != (gint) g_ascii_strtoll(key,
						      NULL,
						      10)){
	      child = child->next;
	      
	      xmlFree(event);
	      xmlFree(key);
	      
	      continue;
	    }
	  }

	  str = xmlGetProp(child,
			   BAD_CAST "delta-time");

	  delta_time = g_ascii_strtod(str,
				      NULL);

	  xmlFree(str);
	  
	  x = ags_midi_util_delta_time_to_offset(&midi_util,
						 delay_factor,
						 track_collection->division,
						 track_collection->tempo,
						 (glong) track_collection->bpm,
						 delta_time);
	  x -= track_collection->first_offset;

	  x_256th = ags_midi_util_delta_time_to_note_256th_offset(&midi_util,
								  delay_factor,
								  track_collection->division,
								  track_collection->tempo,
								  (glong) track_collection->bpm,
								  delta_time);
	  x_256th -= track_collection->first_note_256th_offset;

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
	    list = NULL;
	    
	    ags_timestamp_set_ags_offset(timestamp,
					 AGS_NOTATION_DEFAULT_OFFSET * floor(x / AGS_NOTATION_DEFAULT_OFFSET));

	    notation = ags_notation_find_near_timestamp(notation_start, i,
							timestamp);

	    if(notation == NULL){
	      if(AGS_NOTATION_DEFAULT_OFFSET * floor(x / AGS_NOTATION_DEFAULT_OFFSET) - AGS_NOTATION_DEFAULT_OFFSET >= 0){
		ags_timestamp_set_ags_offset(timestamp,
					     AGS_NOTATION_DEFAULT_OFFSET * floor(x / AGS_NOTATION_DEFAULT_OFFSET) - AGS_NOTATION_DEFAULT_OFFSET);

		notation = ags_notation_find_near_timestamp(notation_start, i,
							    timestamp);
	      }
	    }
	    
	    if(notation != NULL){
	      list = ags_note_find_prev(AGS_NOTATION(notation->data)->note,
					x, y);
	    }
	    
	    if(list == NULL){
	      if(AGS_NOTATION_DEFAULT_OFFSET * floor(x / AGS_NOTATION_DEFAULT_OFFSET) - AGS_NOTATION_DEFAULT_OFFSET >= 0){
		ags_timestamp_set_ags_offset(timestamp,
					     AGS_NOTATION_DEFAULT_OFFSET * floor(x / AGS_NOTATION_DEFAULT_OFFSET) - AGS_NOTATION_DEFAULT_OFFSET);

		notation = ags_notation_find_near_timestamp(notation_start, i,
							    timestamp);

		if(notation != NULL){
		  list = ags_note_find_prev(AGS_NOTATION(notation->data)->note,
					    x, y);
		}
	      }
	    }
	      
	    if(list != NULL){
	      note = list->data;

	      if(note->x[0] == x){
		note->x[1] = x + 1;
	      }else{
		note->x[1] = x;
	      }

	      if(note->x_256th[0] == x_256th){
		note->x_256th[1] = x_256th + 1;
	      }else{
		note->x_256th[1] = x_256th;
	      }
		
	      note->y = y;

	      /* velocity */
#if 0	    
	      note->release.imag = (gdouble) velocity / 127.0;
#endif
		
	      break;
	    }	    
	  }
	  
	  n_key_off++;
	}

	xmlFree(event);
      }

      child = child->next;
    }
    
    track = track->next;
  }

  g_object_unref(timestamp);
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
