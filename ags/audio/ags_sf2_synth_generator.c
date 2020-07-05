/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/audio/ags_sf2_synth_generator.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_sf2_synth_util.h>

#include <ags/audio/file/ags_audio_container.h>
#include <ags/audio/file/ags_audio_container_manager.h>
#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_sound_resource.h>
#include <ags/audio/file/ags_ipatch.h>
#include <ags/audio/file/ags_ipatch_sample.h>

#include <math.h>

#include <ags/i18n.h>

void ags_sf2_synth_generator_class_init(AgsSF2SynthGeneratorClass *sf2_synth_generator);
void ags_sf2_synth_generator_init(AgsSF2SynthGenerator *sf2_synth_generator);
void ags_sf2_synth_generator_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec);
void ags_sf2_synth_generator_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec);
void ags_sf2_synth_generator_finalize(GObject *gobject);

/**
 * SECTION:ags_sf2_synth_generator
 * @short_description: generate wave-form
 * @title: AgsSF2SynthGenerator
 * @section_id:
 * @include: ags/file/ags_sf2_synth_generator.h
 *
 * The #AgsSF2SynthGenerator generates wave-forms with oscillators.
 */

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_PRESET,
  PROP_INSTRUMENT,
  PROP_BANK,
  PROP_PROGRAM,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_DELAY,
  PROP_ATTACK,
  PROP_FRAME_COUNT,
  PROP_LOOP_START,
  PROP_LOOP_END,
  PROP_VOLUME,
  PROP_BASE_KEY,
  PROP_TUNING,
  PROP_TIMESTAMP,
};

static gpointer ags_sf2_synth_generator_parent_class = NULL;


GType
ags_sf2_synth_generator_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sf2_synth_generator = 0;

    static const GTypeInfo ags_sf2_synth_generator_info = {
      sizeof (AgsSF2SynthGeneratorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sf2_synth_generator_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSF2SynthGenerator),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sf2_synth_generator_init,
    };

    ags_type_sf2_synth_generator = g_type_register_static(AGS_TYPE_FUNCTION,
							  "AgsSF2SynthGenerator",
							  &ags_sf2_synth_generator_info,
							  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sf2_synth_generator);
  }

  return g_define_type_id__volatile;
}

void
ags_sf2_synth_generator_class_init(AgsSF2SynthGeneratorClass *sf2_synth_generator)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_sf2_synth_generator_parent_class = g_type_class_peek_parent(sf2_synth_generator);

  /* GObjectClass */
  gobject = (GObjectClass *) sf2_synth_generator;

  gobject->set_property = ags_sf2_synth_generator_set_property;
  gobject->get_property = ags_sf2_synth_generator_get_property;

  gobject->finalize = ags_sf2_synth_generator_finalize;

  /* properties */
  /**
   * AgsSF2SynthGenerator:filename:
   *
   * The assigned filename.
   * 
   * Since: 3.4.0
   */
  param_spec = g_param_spec_string("filename",
				   i18n_pspec("filename of audio container"),
				   i18n_pspec("The filename of audio container"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsSF2SynthGenerator:preset:
   *
   * The assigned preset.
   * 
   * Since: 3.4.0
   */
  param_spec = g_param_spec_string("preset",
				   i18n_pspec("preset of audio container"),
				   i18n_pspec("The preset of audio container"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PRESET,
				  param_spec);

  /**
   * AgsSF2SynthGenerator:instrument:
   *
   * The assigned instrument.
   * 
   * Since: 3.4.0
   */
  param_spec = g_param_spec_string("instrument",
				   i18n_pspec("instrument of audio container"),
				   i18n_pspec("The instrument of audio container"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INSTRUMENT,
				  param_spec);

  /**
   * AgsSF2SynthGenerator:bank:
   *
   * The bank to be used.
   * 
   * Since: 3.4.0
   */
  param_spec = g_param_spec_int("bank",
				i18n_pspec("using bank"),
				i18n_pspec("The bank to be used"),
				0,
				G_MAXINT32,
				AGS_SF2_SYNTH_GENERATOR_DEFAULT_BANK,
				G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BANK,
				  param_spec);

  /**
   * AgsSF2SynthGenerator:program:
   *
   * The program to be used.
   * 
   * Since: 3.4.0
   */
  param_spec = g_param_spec_int("program",
				i18n_pspec("using program"),
				i18n_pspec("The program to be used"),
				0,
				G_MAXINT32,
				AGS_SF2_SYNTH_GENERATOR_DEFAULT_PROGRAM,
				G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PROGRAM,
				  param_spec);

  /**
   * AgsSF2SynthGenerator:samplerate:
   *
   * The samplerate to be used.
   * 
   * Since: 3.4.0
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("using samplerate"),
				 i18n_pspec("The samplerate to be used"),
				 0,
				 G_MAXUINT32,
				 AGS_SF2_SYNTH_GENERATOR_DEFAULT_SAMPLERATE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsSF2SynthGenerator:buffer-size:
   *
   * The buffer size to be used.
   * 
   * Since: 3.4.0
   */
  param_spec = g_param_spec_uint("buffer-size",
				 i18n_pspec("using buffer size"),
				 i18n_pspec("The buffer size to be used"),
				 0,
				 G_MAXUINT32,
				 AGS_SF2_SYNTH_GENERATOR_DEFAULT_BUFFER_SIZE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsSF2SynthGenerator:format:
   *
   * The format to be used.
   * 
   * Since: 3.4.0
   */
  param_spec = g_param_spec_uint("format",
				 i18n_pspec("using format"),
				 i18n_pspec("The format to be used"),
				 0,
				 G_MAXUINT32,
				 AGS_SF2_SYNTH_GENERATOR_DEFAULT_FORMAT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsSF2SynthGenerator:frame-count:
   *
   * The frame count to be used.
   * 
   * Since: 3.4.0
   */
  param_spec = g_param_spec_uint("frame-count",
				 i18n_pspec("apply frame count"),
				 i18n_pspec("To apply frame count"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FRAME_COUNT,
				  param_spec);

  /**
   * AgsSF2SynthGenerator:loop-start:
   *
   * The loop start to be used.
   * 
   * Since: 3.4.0
   */
  param_spec = g_param_spec_uint("loop-start",
				 i18n_pspec("loop start"),
				 i18n_pspec("The loop start"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOOP_START,
				  param_spec);

  /**
   * AgsSF2SynthGenerator:loop-end:
   *
   * The loop end to be used.
   * 
   * Since: 3.4.0
   */
  param_spec = g_param_spec_uint("loop-end",
				 i18n_pspec("loop end"),
				 i18n_pspec("To loop end"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOOP_END,
				  param_spec);

  /**
   * AgsSF2SynthGenerator:delay:
   *
   * The delay to be used.
   * 
   * Since: 3.4.11
   */
  param_spec = g_param_spec_double("delay",
				   i18n_pspec("using delay"),
				   i18n_pspec("The delay to be used"),
				   0.0,
				   65535.0,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY,
				  param_spec);

  /**
   * AgsSF2SynthGenerator:attack:
   *
   * The attack to be used.
   * 
   * Since: 3.4.11
   */
  param_spec = g_param_spec_uint("attack",
				 i18n_pspec("apply attack"),
				 i18n_pspec("To apply attack"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);

  /**
   * AgsSF2SynthGenerator:volume:
   *
   * The volume to be used.
   * 
   * Since: 3.4.11
   */
  param_spec = g_param_spec_double("volume",
				   i18n_pspec("using volume"),
				   i18n_pspec("The volume to be used"),
				   0.0,
				   2.0,
				   AGS_SF2_SYNTH_GENERATOR_DEFAULT_VOLUME,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_VOLUME,
				  param_spec);
  
  /**
   * AgsSF2SynthGenerator:tuning:
   *
   * The tuning to be used.
   * 
   * Since: 3.4.0
   */
  param_spec = g_param_spec_double("tuning",
				   i18n_pspec("using tuning"),
				   i18n_pspec("The tuning to be used"),
				   0.0,
				   1200.0,
				   AGS_SF2_SYNTH_GENERATOR_DEFAULT_TUNING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TUNING,
				  param_spec);

  /**
   * AgsSF2SynthGenerator:base_key:
   *
   * The base_key to be used.
   * 
   * Since: 3.4.0
   */
  param_spec = g_param_spec_double("base_key",
				   i18n_pspec("using base_key"),
				   i18n_pspec("The base_key to be used"),
				   -65535.0,
				   65535.0,
				   AGS_SF2_SYNTH_GENERATOR_DEFAULT_BASE_KEY,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BASE_KEY,
				  param_spec);
}

void
ags_sf2_synth_generator_init(AgsSF2SynthGenerator *sf2_synth_generator)
{
  AgsConfig *config;

  sf2_synth_generator->flags = 0;

  /* sf2 synth generator mutex */
  g_rec_mutex_init(&(sf2_synth_generator->obj_mutex));

  /* config */
  config = ags_config_get_instance();

  sf2_synth_generator->filename = NULL;

  sf2_synth_generator->preset = NULL;
  sf2_synth_generator->instrument = NULL;

  sf2_synth_generator->bank = 0;
  sf2_synth_generator->program = 0;

  /* presets */
  sf2_synth_generator->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  sf2_synth_generator->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  sf2_synth_generator->format = ags_soundcard_helper_config_get_format(config);

  /* more base init */
  sf2_synth_generator->frame_count = 0;
  sf2_synth_generator->loop_start = 0;
  sf2_synth_generator->loop_end = 0;

  sf2_synth_generator->delay = 0.0;
  sf2_synth_generator->attack = 0;

  sf2_synth_generator->volume = AGS_SF2_SYNTH_GENERATOR_DEFAULT_VOLUME;
  
  sf2_synth_generator->base_key = AGS_SF2_SYNTH_GENERATOR_DEFAULT_BASE_KEY;
  sf2_synth_generator->tuning = AGS_SF2_SYNTH_GENERATOR_DEFAULT_TUNING;

  /* timestamp */
  sf2_synth_generator->timestamp = NULL;
}


void
ags_sf2_synth_generator_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec)
{
  AgsSF2SynthGenerator *sf2_synth_generator;

  GRecMutex *sf2_synth_generator_mutex;

  sf2_synth_generator = AGS_SF2_SYNTH_GENERATOR(gobject);

  /* get sf2 synth generator mutex */
  sf2_synth_generator_mutex = AGS_SF2_SYNTH_GENERATOR_GET_OBJ_MUTEX(sf2_synth_generator);
  
  switch(prop_id){
  case PROP_FILENAME:
  {
    gchar *filename;
    
    filename = (gchar *) g_value_get_string(value);

    g_rec_mutex_lock(sf2_synth_generator_mutex);    

    if(sf2_synth_generator->filename == filename){
      g_rec_mutex_unlock(sf2_synth_generator_mutex);
      
      return;
    }
      
    if(sf2_synth_generator->filename != NULL){
      g_free(sf2_synth_generator->filename);
    }

    sf2_synth_generator->filename = g_strdup(filename);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_PRESET:
  {
    gchar *preset;

    preset = (gchar *) g_value_get_string(value);

    g_rec_mutex_lock(sf2_synth_generator_mutex);
    
    if(sf2_synth_generator->preset == preset){
      g_rec_mutex_unlock(sf2_synth_generator_mutex);

      return;
    }
      
    if(sf2_synth_generator->preset != NULL){
      g_free(sf2_synth_generator->preset);
    }

    sf2_synth_generator->preset = g_strdup(preset);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_INSTRUMENT:
  {
    gchar *instrument;

    instrument = (gchar *) g_value_get_string(value);

    g_rec_mutex_lock(sf2_synth_generator_mutex);
    
    if(sf2_synth_generator->instrument == instrument){
      g_rec_mutex_unlock(sf2_synth_generator_mutex);

      return;
    }
      
    if(sf2_synth_generator->instrument != NULL){
      g_free(sf2_synth_generator->instrument);
    }

    sf2_synth_generator->instrument = g_strdup(instrument);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_BANK:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    sf2_synth_generator->bank = g_value_get_int(value);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_PROGRAM:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    sf2_synth_generator->program = g_value_get_int(value);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_SAMPLERATE:
  {
    guint samplerate;
      
    samplerate = g_value_get_uint(value);

    ags_sf2_synth_generator_set_samplerate(sf2_synth_generator,
					   samplerate);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    guint buffer_size;
      
    buffer_size = g_value_get_uint(value);

    ags_sf2_synth_generator_set_buffer_size(sf2_synth_generator,
					    buffer_size);
  }
  break;
  case PROP_FORMAT:
  {
    guint format;
      
    format = g_value_get_uint(value);

    ags_sf2_synth_generator_set_format(sf2_synth_generator,
				       format);
  }
  break;
  case PROP_DELAY:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    sf2_synth_generator->delay = g_value_get_double(value);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_ATTACK:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    sf2_synth_generator->attack = g_value_get_uint(value);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_FRAME_COUNT:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    sf2_synth_generator->frame_count = g_value_get_uint(value);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_LOOP_START:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    sf2_synth_generator->loop_start = g_value_get_uint(value);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_LOOP_END:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    sf2_synth_generator->loop_end = g_value_get_uint(value);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_VOLUME:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    sf2_synth_generator->volume = g_value_get_double(value);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_BASE_KEY:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    sf2_synth_generator->base_key = g_value_get_double(value);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_TUNING:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    sf2_synth_generator->tuning = g_value_get_double(value);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_TIMESTAMP:
  {
    AgsTimestamp *timestamp;

    timestamp = (AgsTimestamp *) g_value_get_object(value);

    g_rec_mutex_lock(sf2_synth_generator_mutex);

    if(sf2_synth_generator->timestamp == (GObject *) timestamp){
      g_rec_mutex_unlock(sf2_synth_generator_mutex);

      return;
    }

    if(sf2_synth_generator->timestamp != NULL){
      g_object_unref(sf2_synth_generator->timestamp);
    }

    if(timestamp != NULL){
      g_object_ref(timestamp);
    }

    sf2_synth_generator->timestamp = (GObject *) timestamp;

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_sf2_synth_generator_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec)
{
  AgsSF2SynthGenerator *sf2_synth_generator;

  GRecMutex *sf2_synth_generator_mutex;

  sf2_synth_generator = AGS_SF2_SYNTH_GENERATOR(gobject);

  /* get sf2 synth generator mutex */
  sf2_synth_generator_mutex = AGS_SF2_SYNTH_GENERATOR_GET_OBJ_MUTEX(sf2_synth_generator);
  
  switch(prop_id){
  case PROP_FILENAME:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    g_value_set_string(value, sf2_synth_generator->filename);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_PRESET:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    g_value_set_string(value, sf2_synth_generator->preset);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_INSTRUMENT:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    g_value_set_string(value, sf2_synth_generator->instrument);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_SAMPLERATE:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    g_value_set_uint(value, sf2_synth_generator->samplerate);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    g_value_set_uint(value, sf2_synth_generator->buffer_size);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_FORMAT:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    g_value_set_uint(value, sf2_synth_generator->format);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_DELAY:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    g_value_set_double(value, sf2_synth_generator->delay);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_ATTACK:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    g_value_set_uint(value, sf2_synth_generator->attack);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_FRAME_COUNT:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    g_value_set_uint(value, sf2_synth_generator->frame_count);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_LOOP_START:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    g_value_set_uint(value, sf2_synth_generator->loop_start);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_LOOP_END:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    g_value_set_uint(value, sf2_synth_generator->loop_end);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_VOLUME:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    g_value_set_double(value, sf2_synth_generator->volume);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_BASE_KEY:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    g_value_set_double(value, sf2_synth_generator->base_key);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_TUNING:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    g_value_set_double(value, sf2_synth_generator->tuning);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  case PROP_TIMESTAMP:
  {
    g_rec_mutex_lock(sf2_synth_generator_mutex);

    g_value_set_object(value, sf2_synth_generator->timestamp);

    g_rec_mutex_unlock(sf2_synth_generator_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_sf2_synth_generator_finalize(GObject *gobject)
{
  AgsSF2SynthGenerator *sf2_synth_generator;

  sf2_synth_generator = AGS_SF2_SYNTH_GENERATOR(gobject);

  if(sf2_synth_generator->timestamp != NULL){
    g_object_unref(sf2_synth_generator->timestamp);
  }

  /* finalize */
  G_OBJECT_CLASS(ags_sf2_synth_generator_parent_class)->finalize(gobject);
}

/**
 * ags_sf2_synth_generator_get_obj_mutex:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * 
 * Get object mutex.
 * 
 * Returns: the #GRecMutex to lock @sf2_synth_generator
 * 
 * Since: 3.4.0
 */
GRecMutex*
ags_sf2_synth_generator_get_obj_mutex(AgsSF2SynthGenerator *sf2_synth_generator)
{
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return(NULL);
  }

  return(AGS_SF2_SYNTH_GENERATOR_GET_OBJ_MUTEX(sf2_synth_generator));
}

/**
 * ags_sf2_synth_generator_test_flags:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * @flags: the flags
 * 
 * Test @flags to be set on @sf2_synth_generator.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 3.4.0
 */
gboolean
ags_sf2_synth_generator_test_flags(AgsSF2SynthGenerator *sf2_synth_generator, guint flags)
{
  gboolean retval;
  
  GRecMutex *sf2_synth_generator_mutex;

  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return(FALSE);
  }
      
  /* get sf2_synth_generator mutex */  
  sf2_synth_generator_mutex = AGS_TASK_GET_OBJ_MUTEX(sf2_synth_generator);

  /* test */
  g_rec_mutex_lock(sf2_synth_generator_mutex);

  retval = (flags & (sf2_synth_generator->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(sf2_synth_generator_mutex);

  return(retval);
}

/**
 * ags_sf2_synth_generator_set_flags:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * @flags: the flags
 * 
 * Set @flags on @sf2_synth_generator.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_generator_set_flags(AgsSF2SynthGenerator *sf2_synth_generator, guint flags)
{
  GRecMutex *sf2_synth_generator_mutex;

  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return;
  }
      
  /* get sf2_synth_generator mutex */
  sf2_synth_generator_mutex = AGS_TASK_GET_OBJ_MUTEX(sf2_synth_generator);

  /* set */
  g_rec_mutex_lock(sf2_synth_generator_mutex);

  sf2_synth_generator->flags |= flags;
  
  g_rec_mutex_unlock(sf2_synth_generator_mutex);
}

/**
 * ags_sf2_synth_generator_unset_flags:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * @flags: the flags
 * 
 * Unset @flags on @sf2_synth_generator.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_generator_unset_flags(AgsSF2SynthGenerator *sf2_synth_generator, guint flags)
{
  GRecMutex *sf2_synth_generator_mutex;

  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return;
  }
      
  /* get sf2_synth_generator mutex */
  sf2_synth_generator_mutex = AGS_TASK_GET_OBJ_MUTEX(sf2_synth_generator);

  /* unset */
  g_rec_mutex_lock(sf2_synth_generator_mutex);

  sf2_synth_generator->flags &= (~flags);
  
  g_rec_mutex_unlock(sf2_synth_generator_mutex);
}

/**
 * ags_sf2_synth_generator_get_filename:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 *
 * Gets filename.
 * 
 * Returns: the filename
 * 
 * Since: 3.4.0
 */
gchar*
ags_sf2_synth_generator_get_filename(AgsSF2SynthGenerator *sf2_synth_generator)
{
  gchar *filename;
  
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return(NULL);
  }

  g_object_get(sf2_synth_generator,
	       "filename", &filename,
	       NULL);

  return(filename);
}

/**
 * ags_sf2_synth_generator_set_filename:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * @filename: the filename
 * 
 * Set filename.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_generator_set_filename(AgsSF2SynthGenerator *sf2_synth_generator, gchar *filename)
{
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return;
  }

  g_object_set(sf2_synth_generator,
	       "filename", filename,
	       NULL);
}

/**
 * ags_sf2_synth_generator_get_preset:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 *
 * Gets preset.
 * 
 * Returns: the preset
 * 
 * Since: 3.4.0
 */
gchar*
ags_sf2_synth_generator_get_preset(AgsSF2SynthGenerator *sf2_synth_generator)
{
  gchar *preset;
  
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return(NULL);
  }

  g_object_get(sf2_synth_generator,
	       "preset", &preset,
	       NULL);

  return(preset);
}

/**
 * ags_sf2_synth_generator_set_preset:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * @preset: the preset
 * 
 * Set preset.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_generator_set_preset(AgsSF2SynthGenerator *sf2_synth_generator, gchar *preset)
{
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return;
  }

  g_object_set(sf2_synth_generator,
	       "preset", preset,
	       NULL);
}

/**
 * ags_sf2_synth_generator_get_instrument:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 *
 * Gets instrument.
 * 
 * Returns: the instrument
 * 
 * Since: 3.4.0
 */
gchar*
ags_sf2_synth_generator_get_instrument(AgsSF2SynthGenerator *sf2_synth_generator)
{
  gchar *instrument;
  
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return(NULL);
  }

  g_object_get(sf2_synth_generator,
	       "instrument", &instrument,
	       NULL);

  return(instrument);
}

/**
 * ags_sf2_synth_generator_set_instrument:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * @instrument: the instrument
 * 
 * Set instrument.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_generator_set_instrument(AgsSF2SynthGenerator *sf2_synth_generator, gchar *instrument)
{
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return;
  }

  g_object_set(sf2_synth_generator,
	       "instrument", instrument,
	       NULL);
}

/**
 * ags_sf2_synth_generator_get_bank:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 *
 * Gets bank.
 * 
 * Returns: the bank
 * 
 * Since: 3.4.0
 */
gint
ags_sf2_synth_generator_get_bank(AgsSF2SynthGenerator *sf2_synth_generator)
{
  gint bank;
  
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return(0);
  }

  g_object_get(sf2_synth_generator,
	       "bank", &bank,
	       NULL);

  return(bank);
}

/**
 * ags_sf2_synth_generator_set_bank:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * @bank: the buffer size
 * 
 * Set buffer size.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_generator_set_bank(AgsSF2SynthGenerator *sf2_synth_generator, gint bank)
{
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return;
  }

  g_object_set(sf2_synth_generator,
	       "bank", bank,
	       NULL);
}

/**
 * ags_sf2_synth_generator_get_program:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 *
 * Gets program.
 * 
 * Returns: the program
 * 
 * Since: 3.4.0
 */
gint
ags_sf2_synth_generator_get_program(AgsSF2SynthGenerator *sf2_synth_generator)
{
  gint program;
  
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return(0);
  }

  g_object_get(sf2_synth_generator,
	       "program", &program,
	       NULL);

  return(program);
}

/**
 * ags_sf2_synth_generator_set_program:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * @program: the buffer size
 * 
 * Set buffer size.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_generator_set_program(AgsSF2SynthGenerator *sf2_synth_generator, gint program)
{
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return;
  }

  g_object_set(sf2_synth_generator,
	       "program", program,
	       NULL);
}

/**
 * ags_sf2_synth_generator_get_samplerate:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 *
 * Gets samplerate.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.4.0
 */
guint
ags_sf2_synth_generator_get_samplerate(AgsSF2SynthGenerator *sf2_synth_generator)
{
  guint samplerate;
  
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return(0);
  }

  g_object_get(sf2_synth_generator,
	       "samplerate", &samplerate,
	       NULL);

  return(samplerate);
}

/**
 * ags_sf2_synth_generator_set_samplerate:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * @samplerate: the samplerate
 * 
 * Set samplerate.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_generator_set_samplerate(AgsSF2SynthGenerator *sf2_synth_generator, guint samplerate)
{
  guint old_samplerate;
  guint i;  

  GRecMutex *sf2_synth_generator_mutex;

  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return;
  }

  /* get sf2 synth generator mutex */
  sf2_synth_generator_mutex = AGS_SF2_SYNTH_GENERATOR_GET_OBJ_MUTEX(sf2_synth_generator);

  g_rec_mutex_lock(sf2_synth_generator_mutex);

  old_samplerate = sf2_synth_generator->samplerate;

  if(old_samplerate == samplerate){  
    g_rec_mutex_unlock(sf2_synth_generator_mutex);

    return;
  }
  
  sf2_synth_generator->samplerate = samplerate;

  sf2_synth_generator->frame_count = samplerate * (sf2_synth_generator->frame_count / old_samplerate);
  
  sf2_synth_generator->loop_start = samplerate * (sf2_synth_generator->loop_start / old_samplerate);
  sf2_synth_generator->loop_end = samplerate * (sf2_synth_generator->loop_end / old_samplerate);
  
  g_rec_mutex_unlock(sf2_synth_generator_mutex);
}

/**
 * ags_sf2_synth_generator_get_buffer_size:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 *
 * Gets buffer size.
 * 
 * Returns: the buffer size
 * 
 * Since: 3.4.0
 */
guint
ags_sf2_synth_generator_get_buffer_size(AgsSF2SynthGenerator *sf2_synth_generator)
{
  guint buffer_size;
  
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return(0);
  }

  g_object_get(sf2_synth_generator,
	       "buffer-size", &buffer_size,
	       NULL);

  return(buffer_size);
}

/**
 * ags_sf2_synth_generator_set_buffer_size:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * @buffer_size: the buffer size
 * 
 * Set buffer size.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_generator_set_buffer_size(AgsSF2SynthGenerator *sf2_synth_generator, guint buffer_size)
{
  GRecMutex *sf2_synth_generator_mutex;

  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return;
  }

  /* get sf2 synth generator mutex */
  sf2_synth_generator_mutex = AGS_SF2_SYNTH_GENERATOR_GET_OBJ_MUTEX(sf2_synth_generator);

  g_rec_mutex_lock(sf2_synth_generator_mutex);

  sf2_synth_generator->buffer_size = buffer_size;

  g_rec_mutex_unlock(sf2_synth_generator_mutex);
}

/**
 * ags_sf2_synth_generator_get_format:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 *
 * Gets format.
 * 
 * Returns: the format
 * 
 * Since: 3.4.0
 */
guint
ags_sf2_synth_generator_get_format(AgsSF2SynthGenerator *sf2_synth_generator)
{
  guint format;
  
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return(0);
  }

  g_object_get(sf2_synth_generator,
	       "format", &format,
	       NULL);

  return(format);
}

/**
 * ags_sf2_synth_generator_set_format:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * @format: the format
 * 
 * Set format.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_generator_set_format(AgsSF2SynthGenerator *sf2_synth_generator, guint format)
{
  GRecMutex *sf2_synth_generator_mutex;

  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return;
  }

  /* get sf2 synth generator mutex */
  sf2_synth_generator_mutex = AGS_SF2_SYNTH_GENERATOR_GET_OBJ_MUTEX(sf2_synth_generator);

  g_rec_mutex_lock(sf2_synth_generator_mutex);

  sf2_synth_generator->format = format;

  g_rec_mutex_unlock(sf2_synth_generator_mutex);
}

/**
 * ags_sf2_synth_generator_get_delay:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 *
 * Gets delay.
 * 
 * Returns: the delay
 * 
 * Since: 3.4.0
 */
gdouble
ags_sf2_synth_generator_get_delay(AgsSF2SynthGenerator *sf2_synth_generator)
{
  gdouble delay;
  
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return(0.0);
  }

  g_object_get(sf2_synth_generator,
	       "delay", &delay,
	       NULL);

  return(delay);
}

/**
 * ags_sf2_synth_generator_set_delay:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * @delay: the delay
 *
 * Sets delay.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_generator_set_delay(AgsSF2SynthGenerator *sf2_synth_generator, gdouble delay)
{
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return;
  }

  g_object_set(sf2_synth_generator,
	       "delay", delay,
	       NULL);
}

/**
 * ags_sf2_synth_generator_get_frame_count:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 *
 * Gets frame count.
 * 
 * Returns: the frame count
 * 
 * Since: 3.4.0
 */
guint
ags_sf2_synth_generator_get_frame_count(AgsSF2SynthGenerator *sf2_synth_generator)
{
  guint frame_count;
  
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return(0);
  }

  g_object_get(sf2_synth_generator,
	       "frame-count", &frame_count,
	       NULL);

  return(frame_count);
}

/**
 * ags_sf2_synth_generator_set_frame_count:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * @frame_count: the frame count
 *
 * Sets frame count.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_generator_set_frame_count(AgsSF2SynthGenerator *sf2_synth_generator, guint frame_count)
{
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return;
  }

  g_object_set(sf2_synth_generator,
	       "frame-count", frame_count,
	       NULL);
}

/**
 * ags_sf2_synth_generator_get_loop_start:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 *
 * Gets loop start.
 * 
 * Returns: the loop start
 * 
 * Since: 3.4.0
 */
guint
ags_sf2_synth_generator_get_loop_start(AgsSF2SynthGenerator *sf2_synth_generator)
{
  guint loop_start;
  
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return(0);
  }

  g_object_get(sf2_synth_generator,
	       "loop-start", &loop_start,
	       NULL);

  return(loop_start);
}

/**
 * ags_sf2_synth_generator_set_loop_start:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * @loop_start: the loop start
 *
 * Sets loop start.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_generator_set_loop_start(AgsSF2SynthGenerator *sf2_synth_generator, guint loop_start)
{
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return;
  }

  g_object_set(sf2_synth_generator,
	       "loop-start", loop_start,
	       NULL);
}

/**
 * ags_sf2_synth_generator_get_loop_end:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 *
 * Gets loop end.
 * 
 * Returns: the loop end
 * 
 * Since: 3.4.0
 */
guint
ags_sf2_synth_generator_get_loop_end(AgsSF2SynthGenerator *sf2_synth_generator)
{
  guint loop_end;
  
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return(0);
  }

  g_object_get(sf2_synth_generator,
	       "loop-end", &loop_end,
	       NULL);

  return(loop_end);
}

/**
 * ags_sf2_synth_generator_set_loop_end:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * @loop_end: the loop end
 *
 * Sets loop end.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_generator_set_loop_end(AgsSF2SynthGenerator *sf2_synth_generator, guint loop_end)
{
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return;
  }

  g_object_set(sf2_synth_generator,
	       "loop-end", loop_end,
	       NULL);
}

/**
 * ags_sf2_synth_generator_get_base_key:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 *
 * Gets base key.
 * 
 * Returns: the base key
 * 
 * Since: 3.4.0
 */
gdouble
ags_sf2_synth_generator_get_base_key(AgsSF2SynthGenerator *sf2_synth_generator)
{
  gdouble base_key;
  
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return(0.0);
  }

  g_object_get(sf2_synth_generator,
	       "base_key", &base_key,
	       NULL);

  return(base_key);
}

/**
 * ags_sf2_synth_generator_set_base_key:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * @base_key: the base key
 *
 * Sets base key.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_generator_set_base_key(AgsSF2SynthGenerator *sf2_synth_generator, gdouble base_key)
{
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return;
  }

  g_object_set(sf2_synth_generator,
	       "base-key", base_key,
	       NULL);
}

/**
 * ags_sf2_synth_generator_get_base_key:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 *
 * Gets tuning.
 * 
 * Returns: the tuning
 * 
 * Since: 3.4.0
 */
gdouble
ags_sf2_synth_generator_get_tuning(AgsSF2SynthGenerator *sf2_synth_generator)
{
  gdouble tuning;
  
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return(0.0);
  }

  g_object_get(sf2_synth_generator,
	       "tuning", &tuning,
	       NULL);

  return(tuning);
}

/**
 * ags_sf2_synth_generator_set_tuning:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * @tuning: the tuning
 *
 * Sets tuning.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_generator_set_tuning(AgsSF2SynthGenerator *sf2_synth_generator, gdouble tuning)
{
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return;
  }

  g_object_set(sf2_synth_generator,
	       "tuning", tuning,
	       NULL);
}

/**
 * ags_sf2_synth_generator_get_timestamp:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * 
 * Get timestamp.
 * 
 * Returns: (transfer full): the #AgsTimestamp
 * 
 * Since: 3.4.0
 */
AgsTimestamp*
ags_sf2_synth_generator_get_timestamp(AgsSF2SynthGenerator *sf2_synth_generator)
{
  AgsTimestamp *timestamp;

  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return(NULL);
  }

  g_object_get(sf2_synth_generator,
	       "timestamp", &timestamp,
	       NULL);

  return(timestamp);
}

/**
 * ags_sf2_synth_generator_set_timestamp:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * @timestamp: the #AgsTimestamp
 * 
 * Set timestamp.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_generator_set_timestamp(AgsSF2SynthGenerator *sf2_synth_generator, AgsTimestamp *timestamp)
{
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return;
  }

  g_object_set(sf2_synth_generator,
	       "timestamp", timestamp,
	       NULL);
}

/**
 * ags_sf2_synth_generator_compute_instrument:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * @audio_signal: the #AgsAudioSignal
 * @note: the note to compute
 * 
 * Compute SF2 synth for @note.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_generator_compute(AgsSF2SynthGenerator *sf2_synth_generator,
				GObject *audio_signal,
				gdouble note)
{
  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator)){
    return;
  }
  
  if(ags_sf2_synth_generator_test_flags(sf2_synth_generator, AGS_SF2_SYNTH_GENERATOR_COMPUTE_INSTRUMENT)){
    gchar *preset;
    gchar *instrument;

    preset = NULL;
    instrument = NULL;
    
    g_object_get(sf2_synth_generator,
		 "preset", &preset,
		 "instrument", &instrument,
		 NULL);
    
    ags_sf2_synth_generator_compute_instrument(sf2_synth_generator,
					       audio_signal,
					       preset,
					       instrument,
					       note);

    g_free(preset);
    g_free(instrument);
  }else if(ags_sf2_synth_generator_test_flags(sf2_synth_generator, AGS_SF2_SYNTH_GENERATOR_COMPUTE_MIDI_LOCALE)){
    gint program;
    gint bank;

    program = 0;
    bank = 0;
    
    g_object_get(sf2_synth_generator,
		 "program", &program,
		 "bank", &bank,
		 NULL);
    
    ags_sf2_synth_generator_compute_midi_locale(sf2_synth_generator,
						audio_signal,
						bank,
						program,
						note);
  }
}

/**
 * ags_sf2_synth_generator_compute_instrument:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * @audio_signal: the #AgsAudioSignal
 * @preset: the preset
 * @instrument: the instrument
 * @note: the note to compute
 * 
 * Compute instrument for @note.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_generator_compute_instrument(AgsSF2SynthGenerator *sf2_synth_generator,
					   GObject *audio_signal,
					   gchar *preset,
					   gchar *instrument,
					   gdouble note)
{
  AgsAudioContainerManager *audio_container_manager;
  AgsAudioContainer *audio_container;
  AgsIpatchSample *ipatch_sample;  

  GObject *output_soundcard;

  GList *start_list, *list;
  GList *stream_start, *stream;

  void *buffer;
  
  gchar *filename;

  gint root_note;
  gint midi_key;
  gdouble delay;
  guint attack;
  guint frame_count;
  guint buffer_size;
  guint length;
  guint current_frame_count, requested_frame_count;
  gdouble samplerate;
  guint format;
  gdouble volume;
  guint audio_buffer_util_format;
  guint loop_mode;
  gint loop_start, loop_end;
  guint copy_mode;
  guint i;
  
  GRecMutex *sf2_synth_generator_mutex;
  GRecMutex *stream_mutex;
  GRecMutex *audio_container_manager_mutex;

  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator) ||
     !AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return;
  }

  /* get sf2 synth generator mutex */
  sf2_synth_generator_mutex = AGS_SF2_SYNTH_GENERATOR_GET_OBJ_MUTEX(sf2_synth_generator);

  /* get stream mutex */
  stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(audio_signal);

  output_soundcard = NULL;
  
  g_object_get(audio_signal,
	       "output-soundcard", &output_soundcard,
	       NULL);

  filename = NULL;
  
  g_object_get(sf2_synth_generator,
	       "filename", &filename,
	       NULL);

  audio_container_manager = ags_audio_container_manager_get_instance();

  /* get audio container manager mutex */
  audio_container_manager_mutex = AGS_AUDIO_CONTAINER_MANAGER_GET_OBJ_MUTEX(audio_container_manager);
  
  g_rec_mutex_lock(audio_container_manager_mutex);
  
  audio_container = ags_audio_container_manager_find_audio_container(audio_container_manager,
								     filename);

  if(audio_container == NULL){    
    audio_container = ags_audio_container_new(filename,
					      preset,
					      instrument,
					      NULL,
					      output_soundcard,
					      -1);
    ags_audio_container_open(audio_container);

    ags_audio_container_manager_add_audio_container(audio_container_manager,
						    audio_container);
  }
  
  g_rec_mutex_unlock(audio_container_manager_mutex);

  list = 
    start_list = ags_audio_container_find_sound_resource(audio_container,
							 preset,
							 instrument,
							 NULL);

  ipatch_sample = NULL;
  
  if(list != NULL){
    ipatch_sample = list->data;
  }

  root_note = 60;
  
  midi_key = (gint) floor(note) + 60;
  
  delay = 0.0;
  attack = 0;

  frame_count = 0;

  volume = 1.0;
  
  g_object_get(sf2_synth_generator,
	       "delay", &delay,
	       "attack", &attack,
	       "frame-count", &frame_count,
	       "volume", &volume,
	       NULL);
  
  if(ipatch_sample != NULL){
    guint loop_start, loop_end;

    ags_sound_resource_info(AGS_SOUND_RESOURCE(ipatch_sample),
			    &frame_count,
			    &loop_start, &loop_end);
    
    g_object_set(audio_signal,
		 "loop-start", loop_start,
		 "loop-end", loop_end,
		 "last-frame", attack + frame_count,
		 NULL);

    g_object_get(ipatch_sample->sample,
		 "root-note", &root_note,
		 NULL);
  }

  buffer_size = AGS_SF2_SYNTH_GENERATOR_DEFAULT_BUFFER_SIZE;
  samplerate = AGS_SF2_SYNTH_GENERATOR_DEFAULT_SAMPLERATE;
  format = AGS_SF2_SYNTH_GENERATOR_DEFAULT_FORMAT;

  length = 0;
  
  g_object_get(audio_signal,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       "samplerate", &samplerate,
	       "length", &length,
	       NULL);

  current_frame_count = length * buffer_size;
  requested_frame_count = (guint) ceil(((floor(delay) * buffer_size + attack) + frame_count) / buffer_size) * buffer_size;
  
  if(current_frame_count < requested_frame_count){
    ags_audio_signal_stream_resize((AgsAudioSignal *) audio_signal,
				   ceil(requested_frame_count / buffer_size));
  }

  ags_audio_signal_clear(audio_signal);
  
  /*  */
  g_rec_mutex_lock(stream_mutex);

  stream = 
    stream_start = g_list_nth(AGS_AUDIO_SIGNAL(audio_signal)->stream,
			      (guint) floor(delay));
  
  g_rec_mutex_unlock(stream_mutex);
  
  loop_mode = AGS_SF2_SYNTH_UTIL_LOOP_NONE;

  loop_start = 0;
  loop_end = 0;

  audio_buffer_util_format = ags_audio_buffer_util_format_from_soundcard(format);

  buffer = ags_stream_alloc(frame_count,
			    format);

  ags_sf2_synth_util_copy(buffer,
			  frame_count,
			  ipatch_sample,
			  (gdouble) (root_note - 69) + note,
			  volume,
			  samplerate, audio_buffer_util_format,
			  0, frame_count,
			  AGS_SF2_SYNTH_UTIL_LOOP_NONE,
			  0, 0);

  copy_mode = ags_audio_buffer_util_get_copy_mode(audio_buffer_util_format,
						  audio_buffer_util_format);
  
  g_rec_mutex_lock(stream_mutex);

  for(i = 0; i < frame_count && stream != NULL;){
    guint copy_count;

    copy_count = buffer_size;

    if(i + copy_count > frame_count){
      copy_count = frame_count - i;
    }

    ags_audio_buffer_util_copy_buffer_to_buffer(stream->data, 1, 0,
						buffer, 1, i,
						copy_count, copy_mode);
    i += copy_count;

    stream = stream->next;
  }
  
  g_rec_mutex_unlock(stream_mutex);
  
  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }  

  g_free(filename);
  
  ags_stream_free(buffer);
}

/**
 * ags_sf2_synth_generator_compute_midi_locale:
 * @sf2_synth_generator: the #AgsSF2SynthGenerator
 * @audio_signal: the #AgsAudioSignal
 * @bank: the bank
 * @program: the program
 * @note: the note to compute
 * 
 * Compute synth for @note.
 * 
 * Since: 3.4.0
 */
void
ags_sf2_synth_generator_compute_midi_locale(AgsSF2SynthGenerator *sf2_synth_generator,
					    GObject *audio_signal,
					    gint bank,
					    gint program,
					    gdouble note)
{
  AgsAudioContainerManager *audio_container_manager;
  AgsAudioContainer *audio_container;
  AgsIpatchSample *ipatch_sample;  

  GObject *output_soundcard;
  
  GList *stream_start, *stream;

  void *buffer;

  gchar *filename;

  gint root_note;
  gint midi_key;
  gdouble delay;
  guint attack;
  guint frame_count;
  guint buffer_size;
  guint length;
  guint current_frame_count, requested_frame_count;
  gdouble samplerate;
  guint format;
  gdouble volume;
  guint audio_buffer_util_format;
  guint loop_mode;
  gint loop_start, loop_end;
  guint copy_mode;
  guint i;
  
  GRecMutex *sf2_synth_generator_mutex;
  GRecMutex *stream_mutex;
  GRecMutex *audio_container_manager_mutex;

  if(!AGS_IS_SF2_SYNTH_GENERATOR(sf2_synth_generator) ||
     !AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return;
  }

  /* get sf2 synth generator mutex */
  sf2_synth_generator_mutex = AGS_SF2_SYNTH_GENERATOR_GET_OBJ_MUTEX(sf2_synth_generator);

  /* get stream mutex */
  stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(audio_signal);

  output_soundcard = NULL;
  
  g_object_get(audio_signal,
	       "output-soundcard", &output_soundcard,
	       NULL);

  filename = NULL;
  
  g_object_get(sf2_synth_generator,
	       "filename", &filename,
	       NULL);
  
  audio_container_manager = ags_audio_container_manager_get_instance();

  /* get audio container manager mutex */
  audio_container_manager_mutex = AGS_AUDIO_CONTAINER_MANAGER_GET_OBJ_MUTEX(audio_container_manager);
  
  g_rec_mutex_lock(audio_container_manager_mutex);

  audio_container = ags_audio_container_manager_find_audio_container(audio_container_manager,
								     filename);

  if(audio_container == NULL){    
    audio_container = ags_audio_container_new(filename,
					      NULL,
					      NULL,
					      NULL,
					      output_soundcard,
					      -1);
    ags_audio_container_open(audio_container);

    ags_audio_container_manager_add_audio_container(audio_container_manager,
						    audio_container);
  }
  
  g_rec_mutex_unlock(audio_container_manager_mutex);
  
  root_note = 60;

  midi_key = (gint) floor(note) + 69;
    
  delay = 0.0;
  attack = 0;

  frame_count = 0;

  volume = 1.0;
  
  g_object_get(sf2_synth_generator,
	       "delay", &delay,
	       "attack", &attack,
	       "frame-count", &frame_count,
	       "volume", &volume,
	       NULL);

  buffer_size = AGS_SF2_SYNTH_GENERATOR_DEFAULT_BUFFER_SIZE;
  samplerate = AGS_SF2_SYNTH_GENERATOR_DEFAULT_SAMPLERATE;
  format = AGS_SF2_SYNTH_GENERATOR_DEFAULT_FORMAT;

  length = 0;
  
  g_object_get(audio_signal,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       "samplerate", &samplerate,
	       "length", &length,
	       NULL);

  current_frame_count = AGS_AUDIO_SIGNAL(audio_signal)->length * buffer_size;
  requested_frame_count = (guint) ceil(((floor(delay) * buffer_size + attack) + frame_count) / buffer_size) * buffer_size;
  
  if(current_frame_count < requested_frame_count){
    ags_audio_signal_stream_resize((AgsAudioSignal *) audio_signal,
				   ceil(requested_frame_count / buffer_size));
  }

  ipatch_sample = ags_sf2_synth_util_midi_locale_find_sample_near_midi_key(audio_container->sound_container,
									   bank,
									   program,
									   midi_key,
									   NULL,
									   NULL,
									   NULL);  

  if(ipatch_sample != NULL){
    guint loop_start, loop_end;

    ags_sound_resource_info(AGS_SOUND_RESOURCE(ipatch_sample),
			    &frame_count,
			    &loop_start, &loop_end);
    
    g_object_set(audio_signal,
		 "loop-start", loop_start,
		 "loop-end", loop_end,
		 "last-frame", attack + frame_count,
		 NULL);

    g_object_get(ipatch_sample->sample,
		 "root-note", &root_note,
		 NULL);
  }

  /*  */
  g_rec_mutex_lock(stream_mutex);

  stream = 
    stream_start = g_list_nth(AGS_AUDIO_SIGNAL(audio_signal)->stream,
			      (guint) floor(delay));

  g_rec_mutex_unlock(stream_mutex);
  
  loop_mode = AGS_SF2_SYNTH_UTIL_LOOP_NONE;

  loop_start = 0;
  loop_end = 0;
  
  if(attack >= buffer_size){
    stream = g_list_nth(stream_start,
			(guint) floor((double) attack / (double) buffer_size));
  }

  audio_buffer_util_format = ags_audio_buffer_util_format_from_soundcard(format);

  buffer = ags_stream_alloc(frame_count,
			    format);

  ags_sf2_synth_util_copy(buffer,
			  frame_count,
			  ipatch_sample,
			  (gdouble) (root_note - 69) + note,
			  volume,
			  samplerate, audio_buffer_util_format,
			  0, frame_count,
			  AGS_SF2_SYNTH_UTIL_LOOP_NONE,
			  0, 0);

  copy_mode = ags_audio_buffer_util_get_copy_mode(audio_buffer_util_format,
						  audio_buffer_util_format);

  g_rec_mutex_lock(stream_mutex);

  for(i = 0; i < frame_count && stream != NULL;){
    guint copy_count;

    copy_count = buffer_size;

    if(i + copy_count > frame_count){
      copy_count = frame_count - i;
    }

    ags_audio_buffer_util_copy_buffer_to_buffer(stream->data, 1, 0,
						buffer, 1, i,
						copy_count, copy_mode);

    i += copy_count;

    stream = stream->next;
  }
  
  g_rec_mutex_unlock(stream_mutex);

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }  

  g_free(filename);

  ags_stream_free(buffer);
}

/**
 * ags_sf2_synth_generator_new:
 *
 * Creates an #AgsSF2SynthGenerator
 *
 * Returns: a new #AgsSF2SynthGenerator
 *
 * Since: 3.4.0
 */
AgsSF2SynthGenerator*
ags_sf2_synth_generator_new()
{
  AgsSF2SynthGenerator *sf2_synth_generator;

  sf2_synth_generator = (AgsSF2SynthGenerator *) g_object_new(AGS_TYPE_SF2_SYNTH_GENERATOR,
							      NULL);

  return(sf2_synth_generator);
}
