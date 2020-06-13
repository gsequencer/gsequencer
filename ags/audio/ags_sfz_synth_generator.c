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

#include <ags/audio/ags_sfz_synth_generator.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_filter_util.h>

#include <math.h>

#include <ags/i18n.h>

void ags_sfz_synth_generator_class_init(AgsSFZSynthGeneratorClass *sfz_synth_generator);
void ags_sfz_synth_generator_init(AgsSFZSynthGenerator *sfz_synth_generator);
void ags_sfz_synth_generator_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec);
void ags_sfz_synth_generator_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec);
void ags_sfz_synth_generator_finalize(GObject *gobject);

/**
 * SECTION:ags_sfz_synth_generator
 * @short_description: generate wave-form
 * @title: AgsSFZSynthGenerator
 * @section_id:
 * @include: ags/file/ags_sfz_synth_generator.h
 *
 * The #AgsSFZSynthGenerator generates wave-forms with oscillators.
 */

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_DELAY,
  PROP_ATTACK,
  PROP_FRAME_COUNT,
  PROP_LOOP_START,
  PROP_LOOP_END,
  PROP_BASE_KEY,
  PROP_TUNING,
  PROP_TIMESTAMP,
};

static gpointer ags_sfz_synth_generator_parent_class = NULL;


GType
ags_sfz_synth_generator_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sfz_synth_generator = 0;

    static const GTypeInfo ags_sfz_synth_generator_info = {
      sizeof (AgsSFZSynthGeneratorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sfz_synth_generator_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSFZSynthGenerator),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sfz_synth_generator_init,
    };

    ags_type_sfz_synth_generator = g_type_register_static(AGS_TYPE_FUNCTION,
							  "AgsSFZSynthGenerator",
							  &ags_sfz_synth_generator_info,
							  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sfz_synth_generator);
  }

  return g_define_type_id__volatile;
}

void
ags_sfz_synth_generator_class_init(AgsSFZSynthGeneratorClass *sfz_synth_generator)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_sfz_synth_generator_parent_class = g_type_class_peek_parent(sfz_synth_generator);

  /* GObjectClass */
  gobject = (GObjectClass *) sfz_synth_generator;

  gobject->set_property = ags_sfz_synth_generator_set_property;
  gobject->get_property = ags_sfz_synth_generator_get_property;

  gobject->finalize = ags_sfz_synth_generator_finalize;

  /* properties */
  /**
   * AgsSFZSynthGenerator:filename:
   *
   * The assigned filename.
   * 
   * Since: 3.0.0
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
   * AgsSFZSynthGenerator:samplerate:
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
				 AGS_SFZ_SYNTH_GENERATOR_DEFAULT_SAMPLERATE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsSFZSynthGenerator:buffer-size:
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
				 AGS_SFZ_SYNTH_GENERATOR_DEFAULT_BUFFER_SIZE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsSFZSynthGenerator:format:
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
				 AGS_SFZ_SYNTH_GENERATOR_DEFAULT_FORMAT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsSFZSynthGenerator:frame-count:
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
   * AgsSFZSynthGenerator:loop-start:
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
   * AgsSFZSynthGenerator:loop-end:
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
   * AgsSFZSynthGenerator:tuning:
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
				   AGS_SFZ_SYNTH_GENERATOR_DEFAULT_TUNING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TUNING,
				  param_spec);

  /**
   * AgsSFZSynthGenerator:base_key:
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
				   AGS_SFZ_SYNTH_GENERATOR_DEFAULT_BASE_KEY,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BASE_KEY,
				  param_spec);
}

void
ags_sfz_synth_generator_init(AgsSFZSynthGenerator *sfz_synth_generator)
{
  AgsConfig *config;

  sfz_synth_generator->flags = 0;

  /* config */
  config = ags_config_get_instance();

  sfz_synth_generator->filename = NULL;

  /* presets */
  sfz_synth_generator->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  sfz_synth_generator->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  sfz_synth_generator->format = ags_soundcard_helper_config_get_format(config);

  /* more base init */
  sfz_synth_generator->frame_count = 0;
  sfz_synth_generator->loop_start = 0;
  sfz_synth_generator->loop_end = 0;

  sfz_synth_generator->delay = 0.0;
  sfz_synth_generator->attack = 0;
  
  sfz_synth_generator->base_key = AGS_SFZ_SYNTH_GENERATOR_DEFAULT_BASE_KEY;
  sfz_synth_generator->tuning = AGS_SFZ_SYNTH_GENERATOR_DEFAULT_TUNING;

  /* timestamp */
  sfz_synth_generator->timestamp = NULL;
}


void
ags_sfz_synth_generator_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec)
{
  AgsSFZSynthGenerator *sfz_synth_generator;

  sfz_synth_generator = AGS_SFZ_SYNTH_GENERATOR(gobject);
  
  switch(prop_id){
  case PROP_FILENAME:
  {
    gchar *filename;

    filename = (gchar *) g_value_get_string(value);

    if(sfz_synth_generator->filename == filename){
      return;
    }
      
    if(sfz_synth_generator->filename != NULL){
      g_free(sfz_synth_generator->filename);
    }

    sfz_synth_generator->filename = g_strdup(filename);
  }
  break;
  case PROP_SAMPLERATE:
  {
    guint samplerate;
      
    samplerate = g_value_get_uint(value);

    ags_sfz_synth_generator_set_samplerate(sfz_synth_generator,
					   samplerate);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    guint buffer_size;
      
    buffer_size = g_value_get_uint(value);

    ags_sfz_synth_generator_set_buffer_size(sfz_synth_generator,
					    buffer_size);
  }
  break;
  case PROP_FORMAT:
  {
    guint format;
      
    format = g_value_get_uint(value);

    ags_sfz_synth_generator_set_format(sfz_synth_generator,
				       format);
  }
  break;
  case PROP_DELAY:
  {
    sfz_synth_generator->delay = g_value_get_double(value);
  }
  break;
  case PROP_ATTACK:
  {
    sfz_synth_generator->attack = g_value_get_uint(value);
  }
  break;
  case PROP_FRAME_COUNT:
  {
    sfz_synth_generator->frame_count = g_value_get_uint(value);
  }
  break;
  case PROP_LOOP_START:
  {
    sfz_synth_generator->loop_start = g_value_get_uint(value);
  }
  break;
  case PROP_LOOP_END:
  {
    sfz_synth_generator->loop_end = g_value_get_uint(value);
  }
  break;
  case PROP_BASE_KEY:
  {
    sfz_synth_generator->base_key = g_value_get_double(value);
  }
  break;
  case PROP_TUNING:
  {
    sfz_synth_generator->tuning = g_value_get_double(value);
  }
  break;
  case PROP_TIMESTAMP:
  {
    AgsTimestamp *timestamp;

    timestamp = (AgsTimestamp *) g_value_get_object(value);

    if(sfz_synth_generator->timestamp == (GObject *) timestamp){
      return;
    }

    if(sfz_synth_generator->timestamp != NULL){
      g_object_unref(sfz_synth_generator->timestamp);
    }

    if(timestamp != NULL){
      g_object_ref(timestamp);
    }

    sfz_synth_generator->timestamp = (GObject *) timestamp;
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_sfz_synth_generator_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec)
{
  AgsSFZSynthGenerator *sfz_synth_generator;

  sfz_synth_generator = AGS_SFZ_SYNTH_GENERATOR(gobject);
  
  switch(prop_id){
  case PROP_FILENAME:
  {
    g_value_set_string(value, sfz_synth_generator->filename);
  }
  break;
  case PROP_SAMPLERATE:
  {
    g_value_set_uint(value, sfz_synth_generator->samplerate);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    g_value_set_uint(value, sfz_synth_generator->buffer_size);
  }
  break;
  case PROP_FORMAT:
  {
    g_value_set_uint(value, sfz_synth_generator->format);
  }
  break;
  case PROP_DELAY:
  {
    g_value_set_double(value, sfz_synth_generator->delay);
  }
  break;
  case PROP_ATTACK:
  {
    g_value_set_uint(value, sfz_synth_generator->attack);
  }
  break;
  case PROP_FRAME_COUNT:
  {
    g_value_set_uint(value, sfz_synth_generator->frame_count);
  }
  break;
  case PROP_LOOP_START:
  {
    g_value_set_uint(value, sfz_synth_generator->loop_start);
  }
  break;
  case PROP_LOOP_END:
  {
    g_value_set_uint(value, sfz_synth_generator->loop_end);
  }
  break;
  case PROP_BASE_KEY:
  {
    g_value_set_double(value, sfz_synth_generator->base_key);
  }
  break;
  case PROP_TUNING:
  {
    g_value_set_double(value, sfz_synth_generator->tuning);
  }
  break;
  case PROP_TIMESTAMP:
  {
    g_value_set_object(value, sfz_synth_generator->timestamp);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_sfz_synth_generator_finalize(GObject *gobject)
{
  AgsSFZSynthGenerator *sfz_synth_generator;

  sfz_synth_generator = AGS_SFZ_SYNTH_GENERATOR(gobject);

  if(sfz_synth_generator->timestamp != NULL){
    g_object_unref(sfz_synth_generator->timestamp);
  }

  /* finalize */
  G_OBJECT_CLASS(ags_sfz_synth_generator_parent_class)->finalize(gobject);
}

/**
 * ags_sfz_synth_generator_get_obj_mutex:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 * 
 * Get object mutex.
 * 
 * Returns: the #GRecMutex to lock @sfz_synth_generator
 * 
 * Since: 3.4.0
 */
GRecMutex*
ags_sfz_synth_generator_get_obj_mutex(AgsSFZSynthGenerator *sfz_synth_generator)
{
  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return(NULL);
  }

  return(AGS_SFZ_SYNTH_GENERATOR_GET_OBJ_MUTEX(sfz_synth_generator));
}

/**
 * ags_sfz_synth_generator_get_filename:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 *
 * Gets filename.
 * 
 * Returns: the filename
 * 
 * Since: 3.4.0
 */
gchar*
ags_sfz_synth_generator_get_filename(AgsSFZSynthGenerator *sfz_synth_generator)
{
  gchar *filename;
  
  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return(NULL);
  }

  g_object_get(sfz_synth_generator,
	       "filename", &filename,
	       NULL);

  return(filename);
}

/**
 * ags_sfz_synth_generator_set_filename:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 * @filename: the filename
 * 
 * Set filename.
 * 
 * Since: 3.4.0
 */
void
ags_sfz_synth_generator_set_filename(AgsSFZSynthGenerator *sfz_synth_generator, gchar *filename)
{
  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return;
  }

  g_object_set(sfz_synth_generator,
	       "filename", filename,
	       NULL);
}

/**
 * ags_sfz_synth_generator_get_samplerate:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 *
 * Gets samplerate.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.4.0
 */
guint
ags_sfz_synth_generator_get_samplerate(AgsSFZSynthGenerator *sfz_synth_generator)
{
  guint samplerate;
  
  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return(0);
  }

  g_object_get(sfz_synth_generator,
	       "samplerate", &samplerate,
	       NULL);

  return(samplerate);
}

/**
 * ags_sfz_synth_generator_set_samplerate:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 * @samplerate: the samplerate
 * 
 * Set samplerate.
 * 
 * Since: 3.4.0
 */
void
ags_sfz_synth_generator_set_samplerate(AgsSFZSynthGenerator *sfz_synth_generator, guint samplerate)
{
  guint old_samplerate;
  guint i;  

  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return;
  }

  old_samplerate = sfz_synth_generator->samplerate;

  if(old_samplerate == samplerate){
    return;
  }
  
  sfz_synth_generator->samplerate = samplerate;

  sfz_synth_generator->frame_count = samplerate * (sfz_synth_generator->frame_count / old_samplerate);
  
  sfz_synth_generator->loop_start = samplerate * (sfz_synth_generator->loop_start / old_samplerate);
  sfz_synth_generator->loop_end = samplerate * (sfz_synth_generator->loop_end / old_samplerate);
}

/**
 * ags_sfz_synth_generator_get_buffer_size:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 *
 * Gets buffer size.
 * 
 * Returns: the buffer size
 * 
 * Since: 3.4.0
 */
guint
ags_sfz_synth_generator_get_buffer_size(AgsSFZSynthGenerator *sfz_synth_generator)
{
  guint buffer_size;
  
  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return(0);
  }

  g_object_get(sfz_synth_generator,
	       "buffer-size", &buffer_size,
	       NULL);

  return(buffer_size);
}

/**
 * ags_sfz_synth_generator_set_buffer_size:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 * @buffer_size: the buffer size
 * 
 * Set buffer size.
 * 
 * Since: 3.4.0
 */
void
ags_sfz_synth_generator_set_buffer_size(AgsSFZSynthGenerator *sfz_synth_generator, guint buffer_size)
{
  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return;
  }

  sfz_synth_generator->buffer_size = buffer_size;
}

/**
 * ags_sfz_synth_generator_get_format:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 *
 * Gets format.
 * 
 * Returns: the format
 * 
 * Since: 3.4.0
 */
guint
ags_sfz_synth_generator_get_format(AgsSFZSynthGenerator *sfz_synth_generator)
{
  guint format;
  
  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return(0);
  }

  g_object_get(sfz_synth_generator,
	       "format", &format,
	       NULL);

  return(format);
}

/**
 * ags_sfz_synth_generator_set_format:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 * @format: the format
 * 
 * Set format.
 * 
 * Since: 3.4.0
 */
void
ags_sfz_synth_generator_set_format(AgsSFZSynthGenerator *sfz_synth_generator, guint format)
{
  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return;
  }

  sfz_synth_generator->format = format;
}

/**
 * ags_sfz_synth_generator_get_delay:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 *
 * Gets delay.
 * 
 * Returns: the delay
 * 
 * Since: 3.4.0
 */
gdouble
ags_sfz_synth_generator_get_delay(AgsSFZSynthGenerator *sfz_synth_generator)
{
  gdouble delay;
  
  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return(0.0);
  }

  g_object_get(sfz_synth_generator,
	       "delay", &delay,
	       NULL);

  return(delay);
}

/**
 * ags_sfz_synth_generator_set_delay:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 * @delay: the delay
 *
 * Sets delay.
 * 
 * Since: 3.4.0
 */
void
ags_sfz_synth_generator_set_delay(AgsSFZSynthGenerator *sfz_synth_generator, gdouble delay)
{
  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return;
  }

  g_object_set(sfz_synth_generator,
	       "delay", delay,
	       NULL);
}

/**
 * ags_sfz_synth_generator_get_frame_count:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 *
 * Gets frame count.
 * 
 * Returns: the frame count
 * 
 * Since: 3.4.0
 */
guint
ags_sfz_synth_generator_get_frame_count(AgsSFZSynthGenerator *sfz_synth_generator)
{
  guint frame_count;
  
  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return(0);
  }

  g_object_get(sfz_synth_generator,
	       "frame-count", &frame_count,
	       NULL);

  return(frame_count);
}

/**
 * ags_sfz_synth_generator_set_frame_count:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 * @frame_count: the frame count
 *
 * Sets frame count.
 * 
 * Since: 3.4.0
 */
void
ags_sfz_synth_generator_set_frame_count(AgsSFZSynthGenerator *sfz_synth_generator, guint frame_count)
{
  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return;
  }

  g_object_set(sfz_synth_generator,
	       "frame-count", frame_count,
	       NULL);
}

/**
 * ags_sfz_synth_generator_get_loop_start:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 *
 * Gets loop start.
 * 
 * Returns: the loop start
 * 
 * Since: 3.4.0
 */
guint
ags_sfz_synth_generator_get_loop_start(AgsSFZSynthGenerator *sfz_synth_generator)
{
  guint loop_start;
  
  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return(0);
  }

  g_object_get(sfz_synth_generator,
	       "loop-start", &loop_start,
	       NULL);

  return(loop_start);
}

/**
 * ags_sfz_synth_generator_set_loop_start:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 * @loop_start: the loop start
 *
 * Sets loop start.
 * 
 * Since: 3.4.0
 */
void
ags_sfz_synth_generator_set_loop_start(AgsSFZSynthGenerator *sfz_synth_generator, guint loop_start)
{
  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return;
  }

  g_object_set(sfz_synth_generator,
	       "loop-start", loop_start,
	       NULL);
}

/**
 * ags_sfz_synth_generator_get_loop_end:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 *
 * Gets loop end.
 * 
 * Returns: the loop end
 * 
 * Since: 3.4.0
 */
guint
ags_sfz_synth_generator_get_loop_end(AgsSFZSynthGenerator *sfz_synth_generator)
{
  guint loop_end;
  
  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return(0);
  }

  g_object_get(sfz_synth_generator,
	       "loop-end", &loop_end,
	       NULL);

  return(loop_end);
}

/**
 * ags_sfz_synth_generator_set_loop_end:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 * @loop_end: the loop end
 *
 * Sets loop end.
 * 
 * Since: 3.4.0
 */
void
ags_sfz_synth_generator_set_loop_end(AgsSFZSynthGenerator *sfz_synth_generator, guint loop_end)
{
  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return;
  }

  g_object_set(sfz_synth_generator,
	       "loop-end", loop_end,
	       NULL);
}

/**
 * ags_sfz_synth_generator_get_base_key:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 *
 * Gets base key.
 * 
 * Returns: the base key
 * 
 * Since: 3.4.0
 */
gdouble
ags_sfz_synth_generator_get_base_key(AgsSFZSynthGenerator *sfz_synth_generator)
{
  gdouble base_key;
  
  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return(0.0);
  }

  g_object_get(sfz_synth_generator,
	       "base_key", &base_key,
	       NULL);

  return(base_key);
}

/**
 * ags_sfz_synth_generator_set_base_key:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 * @base_key: the base key
 *
 * Sets base key.
 * 
 * Since: 3.4.0
 */
void
ags_sfz_synth_generator_set_base_key(AgsSFZSynthGenerator *sfz_synth_generator, gdouble base_key)
{
  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return;
  }

  g_object_set(sfz_synth_generator,
	       "base-key", base_key,
	       NULL);
}

/**
 * ags_sfz_synth_generator_get_base_key:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 *
 * Gets tuning.
 * 
 * Returns: the tuning
 * 
 * Since: 3.4.0
 */
gdouble
ags_sfz_synth_generator_get_tuning(AgsSFZSynthGenerator *sfz_synth_generator)
{
  gdouble tuning;
  
  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return(0.0);
  }

  g_object_get(sfz_synth_generator,
	       "tuning", &tuning,
	       NULL);

  return(tuning);
}

/**
 * ags_sfz_synth_generator_set_tuning:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 * @tuning: the tuning
 *
 * Sets tuning.
 * 
 * Since: 3.4.0
 */
void
ags_sfz_synth_generator_set_tuning(AgsSFZSynthGenerator *sfz_synth_generator, gdouble tuning)
{
  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return;
  }

  g_object_set(sfz_synth_generator,
	       "tuning", tuning,
	       NULL);
}

/**
 * ags_sfz_synth_generator_get_timestamp:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 * 
 * Get timestamp.
 * 
 * Returns: (transfer full): the #AgsTimestamp
 * 
 * Since: 3.4.0
 */
AgsTimestamp*
ags_sfz_synth_generator_get_timestamp(AgsSFZSynthGenerator *sfz_synth_generator)
{
  AgsTimestamp *timestamp;

  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return(NULL);
  }

  g_object_get(sfz_synth_generator,
	       "timestamp", &timestamp,
	       NULL);

  return(timestamp);
}

/**
 * ags_sfz_synth_generator_set_timestamp:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 * @timestamp: the #AgsTimestamp
 * 
 * Set timestamp.
 * 
 * Since: 3.4.0
 */
void
ags_sfz_synth_generator_set_timestamp(AgsSFZSynthGenerator *sfz_synth_generator, AgsTimestamp *timestamp)
{
  if(!AGS_IS_SFZ_SYNTH_GENERATOR(sfz_synth_generator)){
    return;
  }

  g_object_set(sfz_synth_generator,
	       "timestamp", timestamp,
	       NULL);
}

/**
 * ags_sfz_synth_generator_compute_instrument:
 * @sfz_synth_generator: the #AgsSFZSynthGenerator
 * @audio_signal: the #AgsAudioSignal
 * @note: the note to compute
 * 
 * Compute instrument for @note.
 * 
 * Since: 3.4.0
 */
void
ags_sfz_synth_generator_compute(AgsSFZSynthGenerator *sfz_synth_generator,
				GObject *audio_signal,
				gdouble note)
{
  GList *stream_start, *stream;

  gdouble delay;
  guint attack;
  guint frame_count;
  guint buffer_size;
  guint current_frame_count, requested_frame_count;
  gdouble samplerate;
  guint format;
  gdouble volume;

  delay = sfz_synth_generator->delay;
  attack = sfz_synth_generator->attack;

  frame_count = sfz_synth_generator->frame_count;
  
  buffer_size = AGS_AUDIO_SIGNAL(audio_signal)->buffer_size;

  current_frame_count = AGS_AUDIO_SIGNAL(audio_signal)->length * buffer_size;
  requested_frame_count = (guint) ceil(((floor(delay) * buffer_size + attack) + frame_count) / buffer_size) * buffer_size;
  
  if(current_frame_count < requested_frame_count){
    ags_audio_signal_stream_resize((AgsAudioSignal *) audio_signal,
				   ceil(requested_frame_count / buffer_size));
  }

  g_object_set(audio_signal,
	       "loop-start", sfz_synth_generator->loop_start,
	       "loop-end", sfz_synth_generator->loop_end,
	       "last-frame", attack + frame_count,
	       NULL);

  /*  */
  stream = 
    stream_start = g_list_nth(AGS_AUDIO_SIGNAL(audio_signal)->stream,
			      (guint) floor(delay));
  
  samplerate = AGS_AUDIO_SIGNAL(audio_signal)->samplerate;

  format = AGS_AUDIO_SIGNAL(audio_signal)->format;

  volume = synth_generator->volume;

  while(stream != NULL){


    stream = stream->next;
  }
  
  //TODO:JK: implement me
}

/**
 * ags_sfz_synth_generator_new:
 *
 * Creates an #AgsSFZSynthGenerator
 *
 * Returns: a new #AgsSFZSynthGenerator
 *
 * Since: 3.4.0
 */
AgsSFZSynthGenerator*
ags_sfz_synth_generator_new()
{
  AgsSFZSynthGenerator *sfz_synth_generator;

  sfz_synth_generator = (AgsSFZSynthGenerator *) g_object_new(AGS_TYPE_SFZ_SYNTH_GENERATOR,
							      NULL);

  return(sfz_synth_generator);
}
