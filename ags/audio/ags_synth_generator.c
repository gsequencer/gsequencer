/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_plugin.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_timestamp.h>

#include <ags/thread/file/ags_thread_file_xml.h>

#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_lookup.h>
#include <ags/file/ags_file_launch.h>
#include <ags/file/ags_file_link.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_synth_util.h>

#include <math.h>

#include <ags/i18n.h>

void ags_synth_generator_class_init(AgsSynthGeneratorClass *synth_generator);
void ags_synth_generator_plugin_interface_init(AgsPluginInterface *plugin);
void ags_synth_generator_init(AgsSynthGenerator *synth_generator);
void ags_synth_generator_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_synth_generator_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_synth_generator_read(AgsFile *file,
			      xmlNode *node,
			      AgsPlugin *plugin);
xmlNode* ags_synth_generator_write(AgsFile *file,
				   xmlNode *parent,
				   AgsPlugin *plugin);
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
  PROP_N_FRAMES,
  PROP_ATTACK,
  PROP_OSCILLATOR,
  PROP_FREQUENCY,
  PROP_PHASE,
  PROP_VOLUME,
  PROP_TIMESTAMP,
};

static gpointer ags_synth_generator_parent_class = NULL;
static AgsPluginInterface *ags_synth_generator_parent_plugin_interface;

static const gchar *ags_synth_generator_plugin_name = "ags-synth-generator";

GType
ags_synth_generator_get_type()
{
  static GType ags_type_synth_generator = 0;

  if(!ags_type_synth_generator){
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

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_synth_generator_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_synth_generator = g_type_register_static(AGS_TYPE_FUNCTION,
						      "AgsSynthGenerator",
						      &ags_synth_generator_info,
						      0);

    g_type_add_interface_static(ags_type_synth_generator,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return (ags_type_synth_generator);
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
   * Since: 0.7.45
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("using samplerate"),
				 i18n_pspec("The samplerate to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsSynthGenerator:buffer-size:
   *
   * The buffer size to be used.
   * 
   * Since: 0.7.45
   */
  param_spec = g_param_spec_uint("buffer-size",
				 i18n_pspec("using buffer size"),
				 i18n_pspec("The buffer size to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsSynthGenerator:format:
   *
   * The format to be used.
   * 
   * Since: 0.7.45
   */
  param_spec = g_param_spec_uint("format",
				 i18n_pspec("using format"),
				 i18n_pspec("The format to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsSynthGenerator:n-frames:
   *
   * The n-frames to be used.
   * 
   * Since: 0.7.45
   */
  param_spec = g_param_spec_uint("n-frames",
				 i18n_pspec("apply n-frames"),
				 i18n_pspec("To apply n-frames"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_N_FRAMES,
				  param_spec);

  /**
   * AgsSynthGenerator:attack:
   *
   * The attack to be used.
   * 
   * Since: 0.7.45
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
   * AgsSynthGenerator:oscillator:
   *
   * The oscillator to be used.
   * 
   * Since: 0.7.45
   */
  param_spec = g_param_spec_uint("oscillator",
				 i18n_pspec("using oscillator"),
				 i18n_pspec("The oscillator to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OSCILLATOR,
				  param_spec);

  /**
   * AgsAudioSignal:frequency:
   *
   * The frequency to be used.
   * 
   * Since: 0.7.45
   */
  param_spec = g_param_spec_double("frequency",
				   i18n_pspec("using frequency"),
				   i18n_pspec("The frequency to be used"),
				   0.0,
				   65535.0,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FREQUENCY,
				  param_spec);

  /**
   * AgsAudioSignal:phase:
   *
   * The phase to be used.
   * 
   * Since: 0.7.45
   */
  param_spec = g_param_spec_double("phase",
				   i18n_pspec("using phase"),
				   i18n_pspec("The phase to be used"),
				   0.0,
				   65535.0,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PHASE,
				  param_spec);

  /**
   * AgsAudioSignal:volume:
   *
   * The volume to be used.
   * 
   * Since: 0.7.45
   */
  param_spec = g_param_spec_double("volume",
				   i18n_pspec("using volume"),
				   i18n_pspec("The volume to be used"),
				   0.0,
				   65535.0,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_VOLUME,
				  param_spec);

  /**
   * AgsSynthGenerator:timestamp:
   *
   * The assigned timestamp.
   * 
   * Since: 0.7.45
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
ags_synth_generator_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_synth_generator_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->read = ags_synth_generator_read;
  plugin->write = ags_synth_generator_write;
}

void
ags_synth_generator_init(AgsSynthGenerator *synth_generator)
{
  AgsConfig *config;
  
  gchar *str;
  
  /* config */
  config = ags_config_get_instance();

  /* base init */
  synth_generator->flags = 0;

  synth_generator->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  synth_generator->buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  synth_generator->format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  /* samplerate */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "samplerate");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "samplerate");
  }
  
  if(str != NULL){
    synth_generator->samplerate = g_ascii_strtoull(str,
						   NULL,
						   10);

    free(str);
  }

  /* buffer size */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "buffer-size");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "buffer-size");
  }
  
  if(str != NULL){
    synth_generator->buffer_size = g_ascii_strtoull(str,
						    NULL,
						    10);

    free(str);
  }

  /* format */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "format");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "format");
  }
  
  if(str != NULL){
    synth_generator->format = g_ascii_strtoull(str,
					       NULL,
					       10);

    free(str);
  }

  /* more base init */
  synth_generator->n_frames = 0;
  synth_generator->attack = 0;

  synth_generator->oscillator = 0;
  
  synth_generator->frequency = 0.0;
  synth_generator->phase = 0.0;
  synth_generator->volume = 1.0;
  
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
      synth_generator->samplerate = g_value_get_uint(value);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      synth_generator->buffer_size = g_value_get_uint(value);
    }
    break;
  case PROP_FORMAT:
    {
      synth_generator->format = g_value_get_uint(value);
    }
    break;
  case PROP_N_FRAMES:
    {
      synth_generator->n_frames = g_value_get_uint(value);
    }
    break;
  case PROP_ATTACK:
    {
      synth_generator->attack = g_value_get_uint(value);
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
  case PROP_N_FRAMES:
    {
      g_value_set_uint(value, synth_generator->n_frames);
    }
    break;
  case PROP_ATTACK:
    {
      g_value_set_uint(value, synth_generator->attack);
    }
    break;
  case PROP_OSCILLATOR:
    {
      g_value_set_uint(value, synth_generator->oscillator);
    }
    break;
  case PROP_FREQUENCY:
    {
      g_value_set_uint(value, synth_generator->frequency);
    }
    break;
  case PROP_PHASE:
    {
      g_value_set_uint(value, synth_generator->phase);
    }
    break;
  case PROP_VOLUME:
    {
      g_value_set_uint(value, synth_generator->volume);
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
ags_synth_generator_read(AgsFile *file,
			 xmlNode *node,
			 AgsPlugin *plugin)
{
  //  AgsSynthGenerator *gobject;

  //  xmlChar *str;

  //  gobject = AGS_SYNTH_GENERATOR(plugin);
  
  //TODO:JK: implement me
}

xmlNode*
ags_synth_generator_write(AgsFile *file,
			  xmlNode *parent,
			  AgsPlugin *plugin)
{
  AgsSynthGenerator *synth_generator;
  xmlNode *node;
  gchar *id;

  synth_generator = AGS_SYNTH_GENERATOR(plugin);
  
  /* allocate new node with uuid */
  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-synth-generator");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  /* add reference and node to file object */
  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", synth_generator,
				   NULL));

  //TODO:JK: implement me

  /* write timestamp */
  if(synth_generator->timestamp != NULL){
    ags_file_write_timestamp(file,
			     node,
			     (AgsTimestamp *) synth_generator->timestamp);
  }

  return(node);
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
 * ags_synth_generator_compute:
 * @synth_generator: the #AgsSynthGenerator
 * @note: the note to compute
 * 
 * Compute synth for @note.
 *
 * Returns: an #AgsAudioSignal applied specified synth to stream
 * 
 * Since: 0.9.7
 */
GObject*
ags_synth_generator_compute(AgsSynthGenerator *synth_generator,
			    gdouble note)
{
  AgsAudioSignal *audio_signal;

  audio_signal = ags_audio_signal_new(NULL,
				      NULL,
				      NULL);
  ags_audio_signal_stream_resize(audio_signal,
				 ceil(synth_generator->n_frames / synth_generator->buffer_size));

  ags_synth_generator_compute_with_audio_signal(synth_generator,
						audio_signal,
						note,
						NULL, NULL,
						AGS_SYNTH_GENERATOR_COMPUTE_FIXED_LENGTH);
  
  return(audio_signal);
}

/**
 * ags_synth_generator_compute:
 * @synth_generator: the #AgsSynthGenerator
 * @audio_signal: the #AgsAudioSignal
 * @note: the note to compute
 * @sync_start: not used for now
 * @sync_end: not used for now
 * @compute_flags: computation flags
 * 
 * Compute synth for @note for @audio_signal.
 * 
 * Since: 0.9.7
 */
void
ags_synth_generator_compute_with_audio_signal(AgsSynthGenerator *synth_generator,
					      GObject *audio_signal,
					      gdouble note,
					      AgsComplex *sync_start, AgsComplex *sync_end,
					      guint compute_flags)
{
  GList *stream;
  
  void *buffer;
  
  gdouble samplerate;
  gdouble start_frequency, frequency;
  gdouble current_frequency;
  gdouble phase, volume;
  gdouble current_phase;
  guint format;
  guint audio_buffer_util_format;
  guint frame_count, stop_frame;
  guint buffer_size;
  guint offset;
  guint attack, frame_copy_count;

  gdouble current_rate;
  gdouble xcross_factor;
  guint xcross_count;
  guint i;
  gboolean initial_run;

  samplerate = AGS_AUDIO_SIGNAL(audio_signal)->samplerate;
  
  start_frequency = 48.0;
  frequency = synth_generator->frequency;
  
  current_frequency = (guint) ((double) frequency * exp2((double)((start_frequency * -1.0) + (double) note) / 12.0));

  phase = synth_generator->phase;
  volume = synth_generator->volume;

  format = AGS_AUDIO_SIGNAL(audio_signal)->format;
  audio_buffer_util_format = ags_audio_buffer_util_format_from_soundcard(format);

  frame_count = AGS_AUDIO_SIGNAL(audio_signal)->frame_count;
  buffer_size = AGS_AUDIO_SIGNAL(audio_signal)->buffer_size;
  
  /* generate synth */
  if((AGS_SYNTH_GENERATOR_COMPUTE_SYNC & compute_flags) != 0){
    stream = AGS_AUDIO_SIGNAL(audio_signal)->stream_beginning;
    xcross_count = 0;
    
    while(stream != NULL){
      xcross_count += ags_synth_util_get_xcross_count(stream->data,
						      audio_buffer_util_format,
						      buffer_size);

      stream = stream->next;
    }
  }

  if(xcross_count == 0){
    xcross_count = 1;
  }

  attack = synth_generator->attack;

  if((AGS_SYNTH_GENERATOR_COMPUTE_FIXED_LENGTH & compute_flags) != 0){
    stop_frame = synth_generator->n_frames;
  }else{
    stop_frame = frame_count;
  }
  
  stream = AGS_AUDIO_SIGNAL(audio_signal)->stream_beginning;
  stream = g_list_nth(stream,
		      floor(attack / buffer_size));

  attack %= buffer_size;

  offset = 0;
  current_phase = phase;

  if((AGS_SYNTH_GENERATOR_COMPUTE_SYNC & compute_flags) != 0){
    if(xcross_count == 0){
      current_rate = current_frequency;
    }else{
      current_rate = (frame_count / xcross_count) * (frame_count / samplerate);
    }
    
    if((AGS_SYNTH_GENERATOR_COMPUTE_16HZ & compute_flags) != 0){
      xcross_factor = current_rate * (16.0 / samplerate);
    }else if((AGS_SYNTH_GENERATOR_COMPUTE_440HZ & compute_flags) != 0){
      xcross_factor = current_rate * (440.0 / samplerate);
    }else if((AGS_SYNTH_GENERATOR_COMPUTE_22000HZ & compute_flags) != 0){
      xcross_factor = current_rate * (22000.0 / samplerate);
    }else if((AGS_SYNTH_GENERATOR_COMPUTE_LIMIT & compute_flags) != 0){
      xcross_factor = current_rate * (G_MAXDOUBLE / samplerate);
    }else if((AGS_SYNTH_GENERATOR_COMPUTE_NOHZ & compute_flags) != 0){
      if(xcross_count != 0){
	xcross_factor = (1.0 / xcross_count);
      }else{
	xcross_factor = 1.0;
      }
    }else if((AGS_SYNTH_GENERATOR_COMPUTE_FREQUENCY & compute_flags) != 0){
      xcross_factor = current_rate * (frequency / samplerate);
    }else if((AGS_SYNTH_GENERATOR_COMPUTE_NOTE & compute_flags) != 0){
      xcross_factor = current_rate * (current_frequency / samplerate);
    }
  }

  i = 0;
  
  initial_run = TRUE;
  
  while(stream != NULL &&
	offset < stop_frame){
    if(initial_run){
      if(buffer_size - attack < stop_frame){
	frame_copy_count = buffer_size - attack;
      }else{
	frame_copy_count = stop_frame;
      }
    }else{
      if(offset + buffer_size < stop_frame){
	frame_copy_count = buffer_size;
      }else{
	frame_copy_count = stop_frame - offset;
      }
    }

    switch(synth_generator->oscillator){
    case AGS_SYNTH_GENERATOR_OSCILLATOR_SIN:
      {
	ags_synth_util_sin(stream->data,
			   current_frequency, current_phase, volume,
			   samplerate, audio_buffer_util_format,
			   attack, frame_copy_count);
      }
      break;
    case AGS_SYNTH_GENERATOR_OSCILLATOR_SAWTOOTH:
      {
	ags_synth_util_sawtooth(stream->data,
				current_frequency, current_phase, volume,
				samplerate, audio_buffer_util_format,
				attack, frame_copy_count);
      }
      break;
    case AGS_SYNTH_GENERATOR_OSCILLATOR_TRIANGLE:
      {
	ags_synth_util_triangle(stream->data,
				current_frequency, current_phase, volume,
				samplerate, audio_buffer_util_format,
				attack, frame_copy_count);
      }
      break;
    case AGS_SYNTH_GENERATOR_OSCILLATOR_SQUARE:
      {
	ags_synth_util_square(stream->data,
			      current_frequency, current_phase, volume,
			      samplerate, audio_buffer_util_format,
			      attack, frame_copy_count);
      }
      break;
    default:
      g_message("unknown oscillator");
    }

    stream = stream->next;
    offset += frame_copy_count;

    if((AGS_SYNTH_GENERATOR_COMPUTE_SYNC & compute_flags) == 0 ||
       (xcross_factor * i) > (frame_count / xcross_count)){
      current_phase = (guint) (offset - attack + phase) % (guint) floor(samplerate / current_frequency);

      i = 0;
    }else{
      current_phase = phase;

      i++;
    }
    
    attack = 0;

    initial_run = FALSE;
  }

  free(buffer);
}

/**
 * ags_synth_generator_new:
 *
 * Creates an #AgsSynthGenerator
 *
 * Returns: a new #AgsSynthGenerator
 *
 * Since: 0.7.45
 */
AgsSynthGenerator*
ags_synth_generator_new()
{
  AgsSynthGenerator *synth_generator;

  synth_generator = (AgsSynthGenerator *) g_object_new(AGS_TYPE_SYNTH_GENERATOR,
						      NULL);

  return(synth_generator);
}
