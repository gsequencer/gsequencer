/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/libags.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>
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
  PROP_DELAY,
  PROP_ATTACK,
  PROP_FRAME_COUNT,
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * AgsSynthGenerator:frame-count:
   *
   * The frame count to be used.
   * 
   * Since: 2.0.0
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
   * AgsAudioSignal:delay:
   *
   * The delay to be used.
   * 
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
   */
  param_spec = g_param_spec_double("volume",
				   i18n_pspec("using volume"),
				   i18n_pspec("The volume to be used"),
				   0.0,
				   2.0,
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
   * Since: 2.0.0
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

  synth_generator->oscillator = 0;
  
  synth_generator->frequency = 0.0;
  synth_generator->phase = 0.0;
  synth_generator->volume = 1.0;

  synth_generator->sync_relative = FALSE;
  synth_generator->sync_point = NULL;
  synth_generator->sync_point_count = 0;

  synth_generator->damping = ags_complex_alloc();
  synth_generator->vibration = ags_complex_alloc();
  
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
  case PROP_DELAY:
    {
      g_value_set_uint(value, synth_generator->delay);
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
 * @audio_signal: the #AgsAudioSignal
 * @note: the note to compute
 * 
 * Compute synth for @note.
 *
 * Returns: an #AgsAudioSignal applied specified synth to stream
 * 
 * Since: 2.0.0
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
  gdouble samplerate;
  gdouble current_frequency;
  gdouble phase, volume;
  gdouble current_phase;
  guint format;
  guint audio_buffer_util_format;
  guint current_attack, current_count;
  guint offset;
  guint last_sync;
  guint i, j;
  AgsComplex **sync_point;
  guint sync_point_count;
  gboolean synced;

  delay = synth_generator->delay;
  attack = synth_generator->attack;

  frame_count = synth_generator->frame_count;
  
  buffer_size = AGS_AUDIO_SIGNAL(audio_signal)->buffer_size;

  sync_point = synth_generator->sync_point;
  sync_point_count = synth_generator->sync_point_count;

  current_frame_count = AGS_AUDIO_SIGNAL(audio_signal)->length * buffer_size;
  requested_frame_count = (guint) ceil(((floor(delay) * buffer_size + attack) + frame_count) / buffer_size) * buffer_size;
  
  if(current_frame_count < requested_frame_count){
    ags_audio_signal_stream_resize((AgsAudioSignal *) audio_signal,
				   ceil(requested_frame_count / buffer_size));
  }
  
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

  current_attack = attack;
  current_count = buffer_size - attack;
  
  offset = 0;
  last_sync = 0;

  if(sync_point != NULL){
    if(sync_point_count > 1 &&
       floor(sync_point[1][0][0]) > 0.0){
      if(sync_point[1][0][0] < current_count){
	current_count = sync_point[1][0][0];
      }
    }else{
      if(sync_point[0][0][0] < current_count &&
	 floor(sync_point[0][0][0]) > 0.0){
	current_count = sync_point[0][0][0];
      }
    }
  }
  
  synced = FALSE;
  
  for(i = attack, j = 0; i < frame_count + attack && stream != NULL;){
    switch(synth_generator->oscillator){
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
    default:
      g_message("unknown oscillator");
    }

    if(current_frequency == 0.0){
      current_phase = (guint) ((offset + current_count) + phase);
    }else if(floor(samplerate / current_frequency) == 0){
      current_phase = (guint) ((offset + current_count) + phase);
    }else{
      current_phase = (guint) ((offset + current_count) + phase) % (guint) floor(samplerate / current_frequency);
    }
    
    if(sync_point != NULL){
      if(floor(sync_point[j][0][0]) > 0.0 &&
	 last_sync + sync_point[j][0][0] < offset + current_count){
	current_phase = sync_point[j][0][1];

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
	 floor(sync_point[j + 1][0][0]) > 0.0){
	if(sync_point[j + 1][0][0] < current_count){
	  current_count = sync_point[j + 1][0][0];
	}
      }else{
	if(floor(sync_point[0][0][0]) > 0.0 &&
	   sync_point[0][0][0] < current_count){
	  current_count = sync_point[0][0][0];
	}
      }
    }
    
    if(sync_point != NULL){
      if(synced){
	last_sync = last_sync + sync_point[j][0][0];
	j++;

	if(j >= sync_point_count ||
	   floor(sync_point[j][0][0]) == 0.0){
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
 * Since: 2.0.0
 */
AgsSynthGenerator*
ags_synth_generator_new()
{
  AgsSynthGenerator *synth_generator;

  synth_generator = (AgsSynthGenerator *) g_object_new(AGS_TYPE_SYNTH_GENERATOR,
						      NULL);

  return(synth_generator);
}
