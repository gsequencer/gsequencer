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

#include <ags/audio/ags_synth_generator.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_synth_enums.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_synth_util.h>
#include <ags/audio/ags_lfo_synth_util.h>
#include <ags/audio/ags_fm_synth_util.h>

#include <math.h>

#include <ags/i18n.h>

void ags_synth_generator_class_init(AgsSynthGeneratorClass *synth_generator);
void ags_synth_generator_init(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_synth_generator_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_synth_generator_finalize(GObject *gobject);

/**
 * SECTION:ags_synth_generator
 * @short_description: generate wave-form
 * @title: AgsSynthGenerator
 * @section_id:
 * @include: ags/file/ags_synth_generator.h
 *
 * The #AgsSynthGenerator generates wave-forms with oscillators.
 */

enum{
  PROP_0,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_DELAY,
  PROP_ATTACK,
  PROP_FRAME_COUNT,
  PROP_LOOP_START,
  PROP_LOOP_END,
  PROP_DO_LFO,
  PROP_OSCILLATOR,
  PROP_FREQUENCY,
  PROP_PHASE,
  PROP_VOLUME,
  PROP_LFO_DEPTH,
  PROP_TUNING,
  PROP_DO_FM_SYNTH,
  PROP_FM_LFO_OSCILLATOR,
  PROP_FM_LFO_FREQUENCY,
  PROP_FM_LFO_DEPTH,
  PROP_FM_TUNING,
  PROP_TIMESTAMP,
};

static gpointer ags_synth_generator_parent_class = NULL;

static const gchar *ags_synth_generator_plugin_name = "ags-synth-generator";

GType
ags_synth_generator_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_synth_generator = 0;

    static const GTypeInfo ags_synth_generator_info = {
      sizeof (AgsSynthGeneratorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_synth_generator_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSynthGenerator),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_synth_generator_init,
    };

    ags_type_synth_generator = g_type_register_static(AGS_TYPE_FUNCTION,
						      "AgsSynthGenerator",
						      &ags_synth_generator_info,
						      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_synth_generator);
  }

  return g_define_type_id__volatile;
}

void
ags_synth_generator_class_init(AgsSynthGeneratorClass *synth_generator)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_synth_generator_parent_class = g_type_class_peek_parent(synth_generator);

  /* GObjectClass */
  gobject = (GObjectClass *) synth_generator;

  gobject->set_property = ags_synth_generator_set_property;
  gobject->get_property = ags_synth_generator_get_property;

  gobject->finalize = ags_synth_generator_finalize;

  /* properties */
  /**
   * AgsSynthGenerator:samplerate:
   *
   * The samplerate to be used.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("using samplerate"),
				 i18n_pspec("The samplerate to be used"),
				 0,
				 G_MAXUINT32,
				 AGS_SYNTH_GENERATOR_DEFAULT_SAMPLERATE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsSynthGenerator:buffer-size:
   *
   * The buffer size to be used.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("buffer-size",
				 i18n_pspec("using buffer size"),
				 i18n_pspec("The buffer size to be used"),
				 0,
				 G_MAXUINT32,
				 AGS_SYNTH_GENERATOR_DEFAULT_BUFFER_SIZE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsSynthGenerator:format:
   *
   * The format to be used.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("format",
				 i18n_pspec("using format"),
				 i18n_pspec("The format to be used"),
				 0,
				 G_MAXUINT32,
				 AGS_SYNTH_GENERATOR_DEFAULT_FORMAT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsSynthGenerator:frame-count:
   *
   * The frame count to be used.
   * 
   * Since: 3.0.0
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
   * AgsSynthGenerator:loop-start:
   *
   * The loop start to be used.
   * 
   * Since: 3.0.0
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
   * AgsSynthGenerator:loop-end:
   *
   * The loop end to be used.
   * 
   * Since: 3.0.0
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
   * AgsSynthGenerator:delay:
   *
   * The delay to be used.
   * 
   * Since: 3.0.0
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
   * AgsSynthGenerator:attack:
   *
   * The attack to be used.
   * 
   * Since: 3.0.0
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
   * AgsSynthGenerator:do-lfo:
   *
   * If %TRUE compute LFO amplification, otherwise not.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_boolean("do-lfo",
				    i18n_pspec("do LFO amplification"),
				    i18n_pspec("do LFO amplification"),
				    FALSE,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DO_LFO,
				  param_spec);

  /**
   * AgsSynthGenerator:oscillator:
   *
   * The oscillator to be used.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("oscillator",
				 i18n_pspec("using oscillator"),
				 i18n_pspec("The oscillator to be used"),
				 0,
				 G_MAXUINT32,
				 AGS_SYNTH_GENERATOR_DEFAULT_OSCILLATOR,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OSCILLATOR,
				  param_spec);

  /**
   * AgsAudioSignal:frequency:
   *
   * The frequency to be used.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("frequency",
				   i18n_pspec("using frequency"),
				   i18n_pspec("The frequency to be used"),
				   0.0,
				   65535.0,
				   AGS_SYNTH_GENERATOR_DEFAULT_FREQUENCY,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FREQUENCY,
				  param_spec);

  /**
   * AgsAudioSignal:phase:
   *
   * The phase to be used.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("phase",
				   i18n_pspec("using phase"),
				   i18n_pspec("The phase to be used"),
				   0.0,
				   65535.0,
				   AGS_SYNTH_GENERATOR_DEFAULT_PHASE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PHASE,
				  param_spec);

  /**
   * AgsAudioSignal:volume:
   *
   * The volume to be used.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("volume",
				   i18n_pspec("using volume"),
				   i18n_pspec("The volume to be used"),
				   0.0,
				   2.0,
				   AGS_SYNTH_GENERATOR_DEFAULT_VOLUME,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_VOLUME,
				  param_spec);

  /**
   * AgsAudioSignal:lfo-depth:
   *
   * The LFO depth to be used.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("lfo-depth",
				   i18n_pspec("using LFO depth"),
				   i18n_pspec("The LFO depth to be used"),
				   0.0,
				   1.0,
				   AGS_SYNTH_GENERATOR_DEFAULT_LFO_DEPTH,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LFO_DEPTH,
				  param_spec);

  /**
   * AgsAudioSignal:tuning:
   *
   * The tuning to be used.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("tuning",
				   i18n_pspec("using tuning"),
				   i18n_pspec("The tuning to be used"),
				   0.0,
				   1200.0,
				   AGS_SYNTH_GENERATOR_DEFAULT_TUNING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TUNING,
				  param_spec);

  /**
   * AgsSynthGenerator:do-fm-synth:
   *
   * If %TRUE compute FM synth, otherwise not.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_boolean("do-fm-synth",
				    i18n_pspec("do fm synth"),
				    i18n_pspec("do fm synth"),
				    FALSE,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DO_FM_SYNTH,
				  param_spec);

  /**
   * AgsSynthGenerator:fm-lfo-oscillator:
   *
   * The frame count to be used.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("fm-lfo-oscillator",
				 i18n_pspec("fm LFO oscillator mode"),
				 i18n_pspec("the frequency modulation LFO oscillator mode"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FM_LFO_OSCILLATOR,
				  param_spec);

  /**
   * AgsAudioSignal:fm-lfo-frequency:
   *
   * The fm LFO frequency to be used.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("fm-lfo-frequency",
				   i18n_pspec("using fm LFO frequency"),
				   i18n_pspec("The fm LFO frequency to be used"),
				   0.001,
				   27.5,
				   AGS_SYNTH_GENERATOR_DEFAULT_FM_LFO_FREQUENCY,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FM_LFO_FREQUENCY,
				  param_spec);

  /**
   * AgsAudioSignal:fm-lfo-depth:
   *
   * The fm LFO depth to be used.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("fm-lfo-depth",
				   i18n_pspec("using fm LFO depth"),
				   i18n_pspec("The fm LFO depth to be used"),
				   0.0,
				   1.0,
				   AGS_SYNTH_GENERATOR_DEFAULT_FM_LFO_DEPTH,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FM_LFO_DEPTH,
				  param_spec);

  /**
   * AgsAudioSignal:fm-tuning:
   *
   * The fm tuning to be used.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("fm-tuning",
				   i18n_pspec("using FM tuning"),
				   i18n_pspec("The FM tuning to be used"),
				   0.0,
				   1200.0,
				   AGS_SYNTH_GENERATOR_DEFAULT_FM_TUNING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FM_TUNING,
				  param_spec);

  /**
   * AgsSynthGenerator:timestamp:
   *
   * The assigned timestamp.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("timestamp",
				   i18n_pspec("timestamp"),
				   i18n_pspec("The timestamp"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TIMESTAMP,
				  param_spec);
}

void
ags_synth_generator_init(AgsSynthGenerator *synth_generator)
{
  AgsConfig *config;

  synth_generator->flags = 0;

  /* config */
  config = ags_config_get_instance();

  /* presets */
  synth_generator->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  synth_generator->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  synth_generator->format = ags_soundcard_helper_config_get_format(config);

  /* more base init */
  synth_generator->frame_count = 0;
  synth_generator->loop_start = 0;
  synth_generator->loop_end = 0;

  synth_generator->delay = 0.0;
  synth_generator->attack = 0;

  synth_generator->do_lfo = FALSE;

  synth_generator->oscillator = AGS_SYNTH_GENERATOR_DEFAULT_OSCILLATOR;
  
  synth_generator->frequency = AGS_SYNTH_GENERATOR_DEFAULT_FREQUENCY;
  synth_generator->phase = AGS_SYNTH_GENERATOR_DEFAULT_PHASE;
  synth_generator->volume = AGS_SYNTH_GENERATOR_DEFAULT_VOLUME;

  synth_generator->lfo_depth = AGS_SYNTH_GENERATOR_DEFAULT_LFO_DEPTH;
  
  synth_generator->tuning = AGS_SYNTH_GENERATOR_DEFAULT_TUNING;

  synth_generator->sync_relative = FALSE;
  synth_generator->sync_point = NULL;
  synth_generator->sync_point_count = 0;

  synth_generator->damping = ags_complex_alloc();
  synth_generator->vibration = ags_complex_alloc();

  synth_generator->do_fm_synth = FALSE;

  synth_generator->fm_lfo_oscillator = 0;

  synth_generator->fm_lfo_frequency = AGS_SYNTH_GENERATOR_DEFAULT_FM_LFO_FREQUENCY;
  synth_generator->fm_lfo_depth = AGS_SYNTH_GENERATOR_DEFAULT_FM_LFO_DEPTH;
  
  synth_generator->fm_tuning = AGS_SYNTH_GENERATOR_DEFAULT_FM_TUNING;
  
  /* timestamp */
  synth_generator->timestamp = NULL;
}

void
ags_synth_generator_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsSynthGenerator *synth_generator;

  synth_generator = AGS_SYNTH_GENERATOR(gobject);
  
  switch(prop_id){
  case PROP_SAMPLERATE:
    {
      guint samplerate;
      
      samplerate = g_value_get_uint(value);

      ags_synth_generator_set_samplerate(synth_generator,
					 samplerate);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;
      
      buffer_size = g_value_get_uint(value);

      ags_synth_generator_set_buffer_size(synth_generator,
					  buffer_size);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;
      
      format = g_value_get_uint(value);

      ags_synth_generator_set_format(synth_generator,
				     format);
    }
    break;
  case PROP_DELAY:
    {
      synth_generator->delay = g_value_get_double(value);
    }
    break;
  case PROP_ATTACK:
    {
      synth_generator->attack = g_value_get_uint(value);
    }
    break;
  case PROP_FRAME_COUNT:
    {
      synth_generator->frame_count = g_value_get_uint(value);
    }
    break;
  case PROP_LOOP_START:
    {
      synth_generator->loop_start = g_value_get_uint(value);
    }
    break;
  case PROP_LOOP_END:
    {
      synth_generator->loop_end = g_value_get_uint(value);
    }
    break;
  case PROP_OSCILLATOR:
    {
      synth_generator->oscillator = g_value_get_uint(value);
    }
    break;
  case PROP_FREQUENCY:
    {
      synth_generator->frequency = g_value_get_double(value);
    }
    break;
  case PROP_PHASE:
    {
      synth_generator->phase = g_value_get_double(value);
    }
    break;
  case PROP_VOLUME:
    {
      synth_generator->volume = g_value_get_double(value);
    }
    break;
  case PROP_DO_FM_SYNTH:
    {
      synth_generator->do_fm_synth = g_value_get_boolean(value);
    }
    break;
  case PROP_FM_LFO_OSCILLATOR:
    {
      synth_generator->fm_lfo_oscillator = g_value_get_uint(value);
    }
    break;
  case PROP_FM_LFO_FREQUENCY:
    {
      synth_generator->fm_lfo_frequency = g_value_get_double(value);
    }
    break;
  case PROP_FM_LFO_DEPTH:
    {
      synth_generator->fm_lfo_depth = g_value_get_double(value);
    }
    break;
  case PROP_FM_TUNING:
    {
      synth_generator->fm_tuning = g_value_get_double(value);
    }
    break;
  case PROP_TIMESTAMP:
    {
      AgsTimestamp *timestamp;

      timestamp = (AgsTimestamp *) g_value_get_object(value);

      if(synth_generator->timestamp == (GObject *) timestamp){
	return;
      }

      if(synth_generator->timestamp != NULL){
	g_object_unref(synth_generator->timestamp);
      }

      if(timestamp != NULL){
	g_object_ref(timestamp);
      }

      synth_generator->timestamp = (GObject *) timestamp;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_synth_generator_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsSynthGenerator *synth_generator;

  synth_generator = AGS_SYNTH_GENERATOR(gobject);
  
  switch(prop_id){
  case PROP_SAMPLERATE:
    {
      g_value_set_uint(value, synth_generator->samplerate);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value, synth_generator->buffer_size);
    }
    break;
  case PROP_FORMAT:
    {
      g_value_set_uint(value, synth_generator->format);
    }
    break;
  case PROP_DELAY:
    {
      g_value_set_double(value, synth_generator->delay);
    }
    break;
  case PROP_ATTACK:
    {
      g_value_set_uint(value, synth_generator->attack);
    }
    break;
  case PROP_FRAME_COUNT:
    {
      g_value_set_uint(value, synth_generator->frame_count);
    }
    break;
  case PROP_LOOP_START:
    {
      g_value_set_uint(value, synth_generator->loop_start);
    }
    break;
  case PROP_LOOP_END:
    {
      g_value_set_uint(value, synth_generator->loop_end);
    }
    break;
  case PROP_OSCILLATOR:
    {
      g_value_set_uint(value, synth_generator->oscillator);
    }
    break;
  case PROP_FREQUENCY:
    {
      g_value_set_double(value, synth_generator->frequency);
    }
    break;
  case PROP_PHASE:
    {
      g_value_set_double(value, synth_generator->phase);
    }
    break;
  case PROP_DO_FM_SYNTH:
    {
      g_value_set_boolean(value, synth_generator->do_fm_synth);
    }
    break;
  case PROP_FM_LFO_OSCILLATOR:
    {
      g_value_set_uint(value, synth_generator->fm_lfo_oscillator);
    }
    break;
  case PROP_FM_LFO_FREQUENCY:
    {
      g_value_set_double(value, synth_generator->fm_lfo_frequency);
    }
    break;
  case PROP_FM_LFO_DEPTH:
    {
      g_value_set_double(value, synth_generator->fm_lfo_depth);
    }
    break;
  case PROP_FM_TUNING:
    {
      g_value_set_double(value, synth_generator->fm_tuning);
    }
    break;
  case PROP_TIMESTAMP:
    {
      g_value_set_object(value, synth_generator->timestamp);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_synth_generator_finalize(GObject *gobject)
{
  AgsSynthGenerator *synth_generator;

  synth_generator = AGS_SYNTH_GENERATOR(gobject);

  if(synth_generator->timestamp != NULL){
    g_object_unref(synth_generator->timestamp);
  }

  /* finalize */
  G_OBJECT_CLASS(ags_synth_generator_parent_class)->finalize(gobject);
}

/**
 * ags_synth_generator_get_obj_mutex:
 * @synth_generator: the #AgsSynthGenerator
 * 
 * Get object mutex.
 * 
 * Returns: the #GRecMutex to lock @synth_generator
 * 
 * Since: 3.1.0
 */
GRecMutex*
ags_synth_generator_get_obj_mutex(AgsSynthGenerator *synth_generator)
{
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return(NULL);
  }

  return(AGS_SYNTH_GENERATOR_GET_OBJ_MUTEX(synth_generator));
}

/**
 * ags_synth_generator_get_samplerate:
 * @synth_generator: the #AgsSynthGenerator
 *
 * Gets samplerate.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.1.0
 */
guint
ags_synth_generator_get_samplerate(AgsSynthGenerator *synth_generator)
{
  guint samplerate;
  
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return(0);
  }

  g_object_get(synth_generator,
	       "samplerate", &samplerate,
	       NULL);

  return(samplerate);
}

/**
 * ags_synth_generator_set_samplerate:
 * @synth_generator: the #AgsSynthGenerator
 * @samplerate: the samplerate
 * 
 * Set samplerate.
 * 
 * Since: 3.0.0
 */
void
ags_synth_generator_set_samplerate(AgsSynthGenerator *synth_generator, guint samplerate)
{
  guint old_samplerate;
  guint i;  

  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  old_samplerate = synth_generator->samplerate;

  if(old_samplerate == samplerate){
    return;
  }
  
  synth_generator->samplerate = samplerate;

  synth_generator->frame_count = samplerate * (synth_generator->frame_count / old_samplerate);
  
  synth_generator->loop_start = samplerate * (synth_generator->loop_start / old_samplerate);
  synth_generator->loop_end = samplerate * (synth_generator->loop_end / old_samplerate);

  synth_generator->attack = samplerate * (synth_generator->attack / old_samplerate);

  synth_generator->phase = samplerate * (synth_generator->phase / old_samplerate);

  for(i = 0; i < synth_generator->sync_point_count; i++){
    synth_generator->sync_point[i][0].real = samplerate * (synth_generator->sync_point[i][0].real / old_samplerate);
    synth_generator->sync_point[i][0].imag = samplerate * (synth_generator->sync_point[i][0].imag / old_samplerate);
  }
}

/**
 * ags_synth_generator_get_buffer_size:
 * @synth_generator: the #AgsSynthGenerator
 *
 * Gets buffer size.
 * 
 * Returns: the buffer size
 * 
 * Since: 3.1.0
 */
guint
ags_synth_generator_get_buffer_size(AgsSynthGenerator *synth_generator)
{
  guint buffer_size;
  
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return(0);
  }

  g_object_get(synth_generator,
	       "buffer-size", &buffer_size,
	       NULL);

  return(buffer_size);
}

/**
 * ags_synth_generator_set_buffer_size:
 * @synth_generator: the #AgsSynthGenerator
 * @buffer_size: the buffer size
 * 
 * Set buffer size.
 * 
 * Since: 3.0.0
 */
void
ags_synth_generator_set_buffer_size(AgsSynthGenerator *synth_generator, guint buffer_size)
{
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  synth_generator->buffer_size = buffer_size;
}

/**
 * ags_synth_generator_get_format:
 * @synth_generator: the #AgsSynthGenerator
 *
 * Gets format.
 * 
 * Returns: the format
 * 
 * Since: 3.1.0
 */
guint
ags_synth_generator_get_format(AgsSynthGenerator *synth_generator)
{
  guint format;
  
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return(0);
  }

  g_object_get(synth_generator,
	       "format", &format,
	       NULL);

  return(format);
}

/**
 * ags_synth_generator_set_format:
 * @synth_generator: the #AgsSynthGenerator
 * @format: the format
 * 
 * Set format.
 * 
 * Since: 3.0.0
 */
void
ags_synth_generator_set_format(AgsSynthGenerator *synth_generator, guint format)
{
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  synth_generator->format = format;
}

/**
 * ags_synth_generator_get_delay:
 * @synth_generator: the #AgsSynthGenerator
 *
 * Gets delay.
 * 
 * Returns: the delay
 * 
 * Since: 3.1.0
 */
gdouble
ags_synth_generator_get_delay(AgsSynthGenerator *synth_generator)
{
  gdouble delay;
  
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return(0.0);
  }

  g_object_get(synth_generator,
	       "delay", &delay,
	       NULL);

  return(delay);
}

/**
 * ags_synth_generator_set_delay:
 * @synth_generator: the #AgsSynthGenerator
 * @delay: the delay
 *
 * Sets delay.
 * 
 * Since: 3.1.0
 */
void
ags_synth_generator_set_delay(AgsSynthGenerator *synth_generator, gdouble delay)
{
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  g_object_set(synth_generator,
	       "delay", delay,
	       NULL);
}

/**
 * ags_synth_generator_get_frame_count:
 * @synth_generator: the #AgsSynthGenerator
 *
 * Gets frame count.
 * 
 * Returns: the frame count
 * 
 * Since: 3.1.0
 */
guint
ags_synth_generator_get_frame_count(AgsSynthGenerator *synth_generator)
{
  guint frame_count;
  
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return(0);
  }

  g_object_get(synth_generator,
	       "frame-count", &frame_count,
	       NULL);

  return(frame_count);
}

/**
 * ags_synth_generator_set_frame_count:
 * @synth_generator: the #AgsSynthGenerator
 * @frame_count: the frame count
 *
 * Sets frame count.
 * 
 * Since: 3.1.0
 */
void
ags_synth_generator_set_frame_count(AgsSynthGenerator *synth_generator, guint frame_count)
{
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  g_object_set(synth_generator,
	       "frame-count", frame_count,
	       NULL);
}

/**
 * ags_synth_generator_get_loop_start:
 * @synth_generator: the #AgsSynthGenerator
 *
 * Gets loop start.
 * 
 * Returns: the loop start
 * 
 * Since: 3.1.0
 */
guint
ags_synth_generator_get_loop_start(AgsSynthGenerator *synth_generator)
{
  guint loop_start;
  
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return(0);
  }

  g_object_get(synth_generator,
	       "loop-start", &loop_start,
	       NULL);

  return(loop_start);
}

/**
 * ags_synth_generator_set_loop_start:
 * @synth_generator: the #AgsSynthGenerator
 * @loop_start: the loop start
 *
 * Sets loop start.
 * 
 * Since: 3.1.0
 */
void
ags_synth_generator_set_loop_start(AgsSynthGenerator *synth_generator, guint loop_start)
{
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  g_object_set(synth_generator,
	       "loop-start", loop_start,
	       NULL);
}

/**
 * ags_synth_generator_get_loop_end:
 * @synth_generator: the #AgsSynthGenerator
 *
 * Gets loop end.
 * 
 * Returns: the loop end
 * 
 * Since: 3.1.0
 */
guint
ags_synth_generator_get_loop_end(AgsSynthGenerator *synth_generator)
{
  guint loop_end;
  
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return(0);
  }

  g_object_get(synth_generator,
	       "loop-end", &loop_end,
	       NULL);

  return(loop_end);
}

/**
 * ags_synth_generator_set_loop_end:
 * @synth_generator: the #AgsSynthGenerator
 * @loop_end: the loop end
 *
 * Sets loop end.
 * 
 * Since: 3.1.0
 */
void
ags_synth_generator_set_loop_end(AgsSynthGenerator *synth_generator, guint loop_end)
{
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  g_object_set(synth_generator,
	       "loop-end", loop_end,
	       NULL);
}

/**
 * ags_synth_generator_get_do_lfo:
 * @synth_generator: the #AgsSynthGenerator
 *
 * Gets do lfo.
 * 
 * Returns: the do lfo
 * 
 * Since: 3.1.0
 */
gboolean
ags_synth_generator_get_do_lfo(AgsSynthGenerator *synth_generator)
{
  gboolean do_lfo;
  
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return(FALSE);
  }

  g_object_get(synth_generator,
	       "do-lfo", &do_lfo,
	       NULL);

  return(do_lfo);
}

/**
 * ags_synth_generator_set_do_lfo:
 * @synth_generator: the #AgsSynthGenerator
 * @do_lfo: the do lfo
 *
 * Sets do lfo.
 * 
 * Since: 3.1.0
 */
void
ags_synth_generator_set_do_lfo(AgsSynthGenerator *synth_generator, gboolean do_lfo)
{
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  g_object_set(synth_generator,
	       "do-lfo", do_lfo,
	       NULL);
}

/**
 * ags_synth_generator_get_oscillator:
 * @synth_generator: the #AgsSynthGenerator
 *
 * Gets oscillator.
 * 
 * Returns: the oscillator
 * 
 * Since: 3.1.0
 */
guint
ags_synth_generator_get_oscillator(AgsSynthGenerator *synth_generator)
{
  guint oscillator;
  
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return(0);
  }

  g_object_get(synth_generator,
	       "oscillator", &oscillator,
	       NULL);

  return(oscillator);
}

/**
 * ags_synth_generator_set_oscillator:
 * @synth_generator: the #AgsSynthGenerator
 * @oscillator: the oscillator
 *
 * Sets oscillator.
 * 
 * Since: 3.1.0
 */
void
ags_synth_generator_set_oscillator(AgsSynthGenerator *synth_generator, guint oscillator)
{
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  g_object_set(synth_generator,
	       "oscillator", oscillator,
	       NULL);
}

/**
 * ags_synth_generator_get_frequency:
 * @synth_generator: the #AgsSynthGenerator
 *
 * Gets frequency.
 * 
 * Returns: the frequency
 * 
 * Since: 3.1.0
 */
gdouble
ags_synth_generator_get_frequency(AgsSynthGenerator *synth_generator)
{
  gdouble frequency;
  
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return(0.0);
  }

  g_object_get(synth_generator,
	       "frequency", &frequency,
	       NULL);

  return(frequency);
}

/**
 * ags_synth_generator_set_frequency:
 * @synth_generator: the #AgsSynthGenerator
 * @frequency: the frequency
 *
 * Sets frequency.
 * 
 * Since: 3.1.0
 */
void
ags_synth_generator_set_frequency(AgsSynthGenerator *synth_generator, gdouble frequency)
{
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  g_object_set(synth_generator,
	       "frequency", frequency,
	       NULL);
}

/**
 * ags_synth_generator_get_phase:
 * @synth_generator: the #AgsSynthGenerator
 *
 * Gets phase.
 * 
 * Returns: the phase
 * 
 * Since: 3.1.0
 */
gdouble
ags_synth_generator_get_phase(AgsSynthGenerator *synth_generator)
{
  gdouble phase;
  
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return(0.0);
  }

  g_object_get(synth_generator,
	       "phase", &phase,
	       NULL);

  return(phase);
}

/**
 * ags_synth_generator_set_phase:
 * @synth_generator: the #AgsSynthGenerator
 * @phase: the phase
 *
 * Sets phase.
 * 
 * Since: 3.1.0
 */
void
ags_synth_generator_set_phase(AgsSynthGenerator *synth_generator, gdouble phase)
{
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  g_object_set(synth_generator,
	       "phase", phase,
	       NULL);
}

/**
 * ags_synth_generator_get_volume:
 * @synth_generator: the #AgsSynthGenerator
 *
 * Gets volume.
 * 
 * Returns: the volume
 * 
 * Since: 3.1.0
 */
gdouble
ags_synth_generator_get_volume(AgsSynthGenerator *synth_generator)
{
  gdouble volume;
  
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return(0.0);
  }

  g_object_get(synth_generator,
	       "volume", &volume,
	       NULL);

  return(volume);
}

/**
 * ags_synth_generator_set_volume:
 * @synth_generator: the #AgsSynthGenerator
 * @volume: the volume
 *
 * Sets volume.
 * 
 * Since: 3.1.0
 */
void
ags_synth_generator_set_volume(AgsSynthGenerator *synth_generator, gdouble volume)
{
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  g_object_set(synth_generator,
	       "volume", volume,
	       NULL);
}

/**
 * ags_synth_generator_get_lfo_depth:
 * @synth_generator: the #AgsSynthGenerator
 *
 * Gets LFO depth.
 * 
 * Returns: the LFO depth
 * 
 * Since: 3.1.0
 */
gdouble
ags_synth_generator_get_lfo_depth(AgsSynthGenerator *synth_generator)
{
  gdouble lfo_depth;
  
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return(0.0);
  }

  g_object_get(synth_generator,
	       "lfo-depth", &lfo_depth,
	       NULL);

  return(lfo_depth);
}

/**
 * ags_synth_generator_set_lfo_depth:
 * @synth_generator: the #AgsSynthGenerator
 * @lfo_depth: the LFO depth
 *
 * Sets LFO depth.
 * 
 * Since: 3.1.0
 */
void
ags_synth_generator_set_lfo_depth(AgsSynthGenerator *synth_generator, gdouble lfo_depth)
{
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  g_object_set(synth_generator,
	       "lfo-depth", lfo_depth,
	       NULL);
}

/**
 * ags_synth_generator_get_tuning:
 * @synth_generator: the #AgsSynthGenerator
 *
 * Gets tuning.
 * 
 * Returns: the tuning
 * 
 * Since: 3.1.0
 */
gdouble
ags_synth_generator_get_tuning(AgsSynthGenerator *synth_generator)
{
  gdouble tuning;
  
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return(0.0);
  }

  g_object_get(synth_generator,
	       "tuning", &tuning,
	       NULL);

  return(tuning);
}

/**
 * ags_synth_generator_set_tuning:
 * @synth_generator: the #AgsSynthGenerator
 * @tuning: the tuning
 *
 * Sets tuning.
 * 
 * Since: 3.1.0
 */
void
ags_synth_generator_set_tuning(AgsSynthGenerator *synth_generator, gdouble tuning)
{
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  g_object_set(synth_generator,
	       "tuning", tuning,
	       NULL);
}

/**
 * ags_synth_generator_get_do_fm_synth:
 * @synth_generator: the #AgsSynthGenerator
 *
 * Gets do FM synth.
 * 
 * Returns: the do FM synth
 * 
 * Since: 3.1.0
 */
gboolean
ags_synth_generator_get_do_fm_synth(AgsSynthGenerator *synth_generator)
{
  gboolean do_fm_synth;
  
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return(FALSE);
  }

  g_object_get(synth_generator,
	       "do-fm-synth", &do_fm_synth,
	       NULL);

  return(do_fm_synth);
}

/**
 * ags_synth_generator_set_do_fm_synth:
 * @synth_generator: the #AgsSynthGenerator
 * @do_fm_synth: the do FM synth
 *
 * Sets do FM synth.
 * 
 * Since: 3.1.0
 */
void
ags_synth_generator_set_do_fm_synth(AgsSynthGenerator *synth_generator, gboolean do_fm_synth)
{
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  g_object_set(synth_generator,
	       "do-fm-synth", do_fm_synth,
	       NULL);
}

/**
 * ags_synth_generator_get_fm_lfo_oscillator:
 * @synth_generator: the #AgsSynthGenerator
 *
 * Gets FM LFO oscillator.
 * 
 * Returns: the FM LFO oscillator
 * 
 * Since: 3.1.0
 */
guint
ags_synth_generator_get_fm_lfo_oscillator(AgsSynthGenerator *synth_generator)
{
  guint fm_lfo_oscillator;
  
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return(0);
  }

  g_object_get(synth_generator,
	       "fm-lfo-oscillator", &fm_lfo_oscillator,
	       NULL);

  return(fm_lfo_oscillator);
}

/**
 * ags_synth_generator_set_fm_lfo_oscillator:
 * @synth_generator: the #AgsSynthGenerator
 * @fm_lfo_oscillator: the FM LFO oscillator
 *
 * Sets FM LFO oscillator.
 * 
 * Since: 3.1.0
 */
void
ags_synth_generator_set_fm_lfo_oscillator(AgsSynthGenerator *synth_generator, guint fm_lfo_oscillator)
{
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  g_object_set(synth_generator,
	       "fm-lfo-oscillator", fm_lfo_oscillator,
	       NULL);
}

/**
 * ags_synth_generator_get_fm_lfo_frequency:
 * @synth_generator: the #AgsSynthGenerator
 *
 * Gets FM LFO frequency.
 * 
 * Returns: the FM LFO frequency
 * 
 * Since: 3.1.0
 */
gdouble
ags_synth_generator_get_fm_lfo_frequency(AgsSynthGenerator *synth_generator)
{
  gdouble fm_lfo_frequency;
  
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return(0.0);
  }

  g_object_get(synth_generator,
	       "fm-lfo-frequency", &fm_lfo_frequency,
	       NULL);

  return(fm_lfo_frequency);
}

/**
 * ags_synth_generator_set_fm_lfo_frequency:
 * @synth_generator: the #AgsSynthGenerator
 * @fm_lfo_frequency: the FM LFO frequency
 *
 * Sets FM LFO frequency.
 * 
 * Since: 3.1.0
 */
void
ags_synth_generator_set_fm_lfo_frequency(AgsSynthGenerator *synth_generator, gdouble fm_lfo_frequency)
{
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  g_object_set(synth_generator,
	       "fm-lfo-frequency", fm_lfo_frequency,
	       NULL);
}

/**
 * ags_synth_generator_get_fm_lfo_depth:
 * @synth_generator: the #AgsSynthGenerator
 *
 * Gets FM LFO depth.
 * 
 * Returns: the FM LFO depth
 * 
 * Since: 3.1.0
 */
gdouble
ags_synth_generator_get_fm_lfo_depth(AgsSynthGenerator *synth_generator)
{
  gdouble fm_lfo_depth;
  
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return(0.0);
  }

  g_object_get(synth_generator,
	       "fm-lfo-depth", &fm_lfo_depth,
	       NULL);

  return(fm_lfo_depth);
}

/**
 * ags_synth_generator_set_fm_lfo_depth:
 * @synth_generator: the #AgsSynthGenerator
 * @fm_lfo_depth: the FM LFO depth
 *
 * Sets FM LFO depth.
 * 
 * Since: 3.1.0
 */
void
ags_synth_generator_set_fm_lfo_depth(AgsSynthGenerator *synth_generator, gdouble fm_lfo_depth)
{
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  g_object_set(synth_generator,
	       "fm-lfo-depth", fm_lfo_depth,
	       NULL);
}

/**
 * ags_synth_generator_get_fm_tuning:
 * @synth_generator: the #AgsSynthGenerator
 *
 * Gets FM tuning.
 * 
 * Returns: the FM tuning
 * 
 * Since: 3.1.0
 */
gdouble
ags_synth_generator_get_fm_tuning(AgsSynthGenerator *synth_generator)
{
  gdouble fm_tuning;
  
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return(0.0);
  }

  g_object_get(synth_generator,
	       "fm-tuning", &fm_tuning,
	       NULL);

  return(fm_tuning);
}

/**
 * ags_synth_generator_set_fm_tuning:
 * @synth_generator: the #AgsSynthGenerator
 * @fm_tuning: the FM tuning
 *
 * Sets FM tuning.
 * 
 * Since: 3.1.0
 */
void
ags_synth_generator_set_fm_tuning(AgsSynthGenerator *synth_generator, gdouble fm_tuning)
{
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  g_object_set(synth_generator,
	       "fm-tuning", fm_tuning,
	       NULL);
}

/**
 * ags_synth_generator_get_timestamp:
 * @synth_generator: the #AgsSynthGenerator
 * 
 * Get timestamp.
 * 
 * Returns: (transfer full): the #AgsTimestamp
 * 
 * Since: 3.1.0
 */
AgsTimestamp*
ags_synth_generator_get_timestamp(AgsSynthGenerator *synth_generator)
{
  AgsTimestamp *timestamp;

  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return(NULL);
  }

  g_object_get(synth_generator,
	       "timestamp", &timestamp,
	       NULL);

  return(timestamp);
}

/**
 * ags_synth_generator_set_timestamp:
 * @synth_generator: the #AgsSynthGenerator
 * @timestamp: the #AgsTimestamp
 * 
 * Set timestamp.
 * 
 * Since: 3.1.0
 */
void
ags_synth_generator_set_timestamp(AgsSynthGenerator *synth_generator, AgsTimestamp *timestamp)
{
  if(!AGS_IS_SYNTH_GENERATOR(synth_generator)){
    return;
  }

  g_object_set(synth_generator,
	       "timestamp", timestamp,
	       NULL);
}

/**
 * ags_synth_generator_compute:
 * @synth_generator: the #AgsSynthGenerator
 * @audio_signal: the #AgsAudioSignal
 * @note: the note to compute
 * 
 * Compute synth for @note.
 * 
 * Since: 3.0.0
 */
void
ags_synth_generator_compute(AgsSynthGenerator *synth_generator,
			    GObject *audio_signal,
			    gdouble note)
{
  GList *stream_start, *stream;

  gdouble delay;
  guint attack;
  guint frame_count;
  guint buffer_size;
  guint current_frame_count, requested_frame_count;
  gboolean do_lfo;
  guint oscillator;
  gdouble samplerate;
  gdouble current_frequency;
  gdouble phase, volume;
  gdouble lfo_depth;
  gdouble tuning;
  gdouble current_phase;
  guint format;
  guint audio_buffer_util_format;
  guint current_attack, current_count;
  guint offset;
  guint last_sync;
  guint i, j;
  AgsComplex **sync_point;
  guint sync_point_count;
  gboolean do_fm_synth;
  guint fm_lfo_osc_mode;
  gdouble fm_lfo_freq; 
  gdouble fm_lfo_depth;
  gdouble fm_tuning;
  gboolean synced;

  delay = synth_generator->delay;
  attack = synth_generator->attack;

  frame_count = synth_generator->frame_count;
  
  buffer_size = AGS_AUDIO_SIGNAL(audio_signal)->buffer_size;

  do_lfo = synth_generator->do_lfo;

  oscillator = synth_generator->oscillator;
  
  sync_point = synth_generator->sync_point;
  sync_point_count = synth_generator->sync_point_count;

  current_frame_count = AGS_AUDIO_SIGNAL(audio_signal)->length * buffer_size;
  requested_frame_count = (guint) ceil(((floor(delay) * buffer_size + attack) + frame_count) / buffer_size) * buffer_size;
  
  if(current_frame_count < requested_frame_count){
    ags_audio_signal_stream_resize((AgsAudioSignal *) audio_signal,
				   ceil(requested_frame_count / buffer_size));
  }

  g_object_set(audio_signal,
	       "loop-start", synth_generator->loop_start,
	       "loop-end", synth_generator->loop_end,
	       "last-frame", attack + frame_count,
	       NULL);
  
  /*  */
  stream = 
    stream_start = g_list_nth(AGS_AUDIO_SIGNAL(audio_signal)->stream,
			      (guint) floor(delay));
  
  samplerate = AGS_AUDIO_SIGNAL(audio_signal)->samplerate;

  current_frequency = (guint) ((double) synth_generator->frequency * exp2((double)((double) note + 48.0) / 12.0));

  phase =
    current_phase = synth_generator->phase;
  volume = synth_generator->volume;

  format = AGS_AUDIO_SIGNAL(audio_signal)->format;
  audio_buffer_util_format = ags_audio_buffer_util_format_from_soundcard(format);

  lfo_depth = synth_generator->lfo_depth;
  tuning = synth_generator->tuning;

  current_attack = attack;
  current_count = buffer_size;
  
  if(attack < buffer_size){
    current_count = buffer_size - attack;
  }else{
    stream = g_list_nth(stream_start,
			(guint) floor((double) attack / (double) buffer_size));

    current_count = buffer_size - (attack % buffer_size);
  }
  
  offset = 0;
  last_sync = 0;

  if(sync_point != NULL){
    if(sync_point_count > 1 &&
       floor(sync_point[1][0].real) > 0.0){
      if(sync_point[1][0].real < current_count){
	current_count = sync_point[1][0].real;
      }
    }else{
      if(sync_point[0][0].real < current_count &&
	 floor(sync_point[0][0].real) > 0.0){
	current_count = sync_point[0][0].real;
      }
    }
  }

  do_fm_synth = synth_generator->do_fm_synth;

  fm_lfo_osc_mode = 0;
  
  switch(synth_generator->fm_lfo_oscillator){
  case AGS_SYNTH_GENERATOR_OSCILLATOR_SIN:
  {
    fm_lfo_osc_mode = AGS_SYNTH_OSCILLATOR_SIN;
  }
  break;
  case AGS_SYNTH_GENERATOR_OSCILLATOR_SAWTOOTH:
  {
    fm_lfo_osc_mode = AGS_SYNTH_OSCILLATOR_SAWTOOTH;
  }
  break;
  case AGS_SYNTH_GENERATOR_OSCILLATOR_TRIANGLE:
  {
    fm_lfo_osc_mode = AGS_SYNTH_OSCILLATOR_TRIANGLE;
  }
  break;
  case AGS_SYNTH_GENERATOR_OSCILLATOR_SQUARE:
  {
    fm_lfo_osc_mode = AGS_SYNTH_OSCILLATOR_SQUARE;
  }
  break;
  case AGS_SYNTH_GENERATOR_OSCILLATOR_IMPULSE:
  {
    fm_lfo_osc_mode = AGS_SYNTH_OSCILLATOR_IMPULSE;
  }
  break;
  }
  
  fm_lfo_freq = synth_generator->fm_lfo_frequency;
  fm_lfo_depth = synth_generator->fm_lfo_depth;

  fm_tuning = synth_generator->fm_tuning;

  synced = FALSE;
  
  for(i = attack, j = 0; i < frame_count + attack && stream != NULL;){

    if(!do_fm_synth && !do_lfo){

      switch(oscillator){
      case AGS_SYNTH_GENERATOR_OSCILLATOR_SIN:
      {
	ags_synth_util_sin(stream->data,
			   current_frequency, current_phase, volume,
			   samplerate, audio_buffer_util_format,
			   current_attack, current_count);
      }
      break;
      case AGS_SYNTH_GENERATOR_OSCILLATOR_SAWTOOTH:
      {
	ags_synth_util_sawtooth(stream->data,
				current_frequency, current_phase, volume,
				samplerate, audio_buffer_util_format,
				current_attack, current_count);
      }
      break;
      case AGS_SYNTH_GENERATOR_OSCILLATOR_TRIANGLE:
      {
	ags_synth_util_triangle(stream->data,
				current_frequency, current_phase, volume,
				samplerate, audio_buffer_util_format,
				current_attack, current_count);
      }
      break;
      case AGS_SYNTH_GENERATOR_OSCILLATOR_SQUARE:
      {
	ags_synth_util_square(stream->data,
			      current_frequency, current_phase, volume,
			      samplerate, audio_buffer_util_format,
			      current_attack, current_count);
      }
      break;
      case AGS_SYNTH_GENERATOR_OSCILLATOR_IMPULSE:
      {
	ags_synth_util_impulse(stream->data,
			       current_frequency, current_phase, volume,
			       samplerate, audio_buffer_util_format,
			       current_attack, current_count);
      }
      break;
      default:
	g_message("unknown oscillator");
      }

    }else if(do_lfo){

      switch(oscillator){
      case AGS_SYNTH_GENERATOR_OSCILLATOR_SIN:
      {
	ags_lfo_synth_util_sin(stream->data,
			       current_frequency, current_phase,
			       lfo_depth,
			       tuning,
			       samplerate, audio_buffer_util_format,
			       current_attack, current_count);
      }
      break;
      case AGS_SYNTH_GENERATOR_OSCILLATOR_SAWTOOTH:
      {
	ags_lfo_synth_util_sawtooth(stream->data,
				    current_frequency, current_phase,
				    lfo_depth,
				    tuning,
				    samplerate, audio_buffer_util_format,
				    current_attack, current_count);
      }
      break;
      case AGS_SYNTH_GENERATOR_OSCILLATOR_TRIANGLE:
      {
	ags_lfo_synth_util_triangle(stream->data,
				    current_frequency, current_phase,
				    lfo_depth,
				    tuning,
				    samplerate, audio_buffer_util_format,
				    current_attack, current_count);
      }
      break;
      case AGS_SYNTH_GENERATOR_OSCILLATOR_SQUARE:
      {
	ags_lfo_synth_util_square(stream->data,
				  current_frequency, current_phase,
				  lfo_depth,
				  tuning,
				  samplerate, audio_buffer_util_format,
				  current_attack, current_count);
      }
      break;
      case AGS_SYNTH_GENERATOR_OSCILLATOR_IMPULSE:
      {
	ags_lfo_synth_util_impulse(stream->data,
				   current_frequency, current_phase,
				   lfo_depth,
				   tuning,
				   samplerate, audio_buffer_util_format,
				   current_attack, current_count);
      }
      break;
      default:
	g_message("unknown oscillator");
      }

    }else if(do_fm_synth){

      switch(synth_generator->oscillator){
      case AGS_SYNTH_GENERATOR_OSCILLATOR_SIN:
      {
	ags_fm_synth_util_sin(stream->data,
			      current_frequency, current_phase, volume,
			      samplerate, audio_buffer_util_format,
			      current_attack, current_count,
			      fm_lfo_osc_mode,
			      fm_lfo_freq, fm_lfo_depth,
			      fm_tuning);
      }
      break;
      case AGS_SYNTH_GENERATOR_OSCILLATOR_SAWTOOTH:
      {
	ags_fm_synth_util_sawtooth(stream->data,
				   current_frequency, current_phase, volume,
				   samplerate, audio_buffer_util_format,
				   current_attack, current_count,
				   fm_lfo_osc_mode,
				   fm_lfo_freq, fm_lfo_depth,
				   fm_tuning);
      }
      break;
      case AGS_SYNTH_GENERATOR_OSCILLATOR_TRIANGLE:
      {
	ags_fm_synth_util_triangle(stream->data,
				   current_frequency, current_phase, volume,
				   samplerate, audio_buffer_util_format,
				   current_attack, current_count,
				   fm_lfo_osc_mode,
				   fm_lfo_freq, fm_lfo_depth,
				   fm_tuning);
      }
      break;
      case AGS_SYNTH_GENERATOR_OSCILLATOR_SQUARE:
      {
	ags_fm_synth_util_square(stream->data,
				 current_frequency, current_phase, volume,
				 samplerate, audio_buffer_util_format,
				 current_attack, current_count,
				 fm_lfo_osc_mode,
				 fm_lfo_freq, fm_lfo_depth,
				 fm_tuning);
      }
      break;
      case AGS_SYNTH_GENERATOR_OSCILLATOR_IMPULSE:
      {
	ags_fm_synth_util_impulse(stream->data,
				  current_frequency, current_phase, volume,
				  samplerate, audio_buffer_util_format,
				  current_attack, current_count,
				  fm_lfo_osc_mode,
				  fm_lfo_freq, fm_lfo_depth,
				  fm_tuning);
      }
      break;
      default:
	g_message("unknown oscillator");
      }

    }
    
    if(current_frequency == 0.0){
      current_phase = (guint) ((offset + current_count) + phase);
    }else if(floor(samplerate / current_frequency) == 0){
      current_phase = (guint) ((offset + current_count) + phase);
    }else{
      current_phase = (guint) ((offset + current_count) + phase) % (guint) floor(samplerate / current_frequency);
    }
    
    if(sync_point != NULL){
      if(floor(sync_point[j][0].real) > 0.0 &&
	 last_sync + sync_point[j][0].real < offset + current_count){
	current_phase = sync_point[j][0].imag;

	synced = TRUE;
      }
    }
    
    offset += current_count;
    i += current_count;

    if(buffer_size > (current_attack + current_count)){
      current_count = buffer_size - (current_attack + current_count);
      current_attack = buffer_size - current_count;
    }else{
      current_count = buffer_size;
      current_attack = 0;
    }
    
    if(sync_point != NULL){
      if(j + 1 < sync_point_count &&
	 floor(sync_point[j + 1][0].real) > 0.0){
	if(sync_point[j + 1][0].real < current_count){
	  current_count = sync_point[j + 1][0].real;
	}
      }else{
	if(floor(sync_point[0][0].real) > 0.0 &&
	   sync_point[0][0].real < current_count){
	  current_count = sync_point[0][0].real;
	}
      }
    }
    
    if(sync_point != NULL){
      if(synced){
	last_sync = last_sync + sync_point[j][0].real;
	j++;

	if(j >= sync_point_count ||
	   floor(sync_point[j][0].real) == 0.0){
	  j = 0;
	}

	synced = FALSE;
      }
    }

    if(i != 0 &&
       i % buffer_size == 0){
      stream = stream->next;
    }
  }  
}

/**
 * ags_synth_generator_new:
 *
 * Creates an #AgsSynthGenerator
 *
 * Returns: a new #AgsSynthGenerator
 *
 * Since: 3.0.0
 */
AgsSynthGenerator*
ags_synth_generator_new()
{
  AgsSynthGenerator *synth_generator;

  synth_generator = (AgsSynthGenerator *) g_object_new(AGS_TYPE_SYNTH_GENERATOR,
						      NULL);

  return(synth_generator);
}
