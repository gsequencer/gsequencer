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

#include <ags/audio/file/ags_audio_container.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_wave.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_sound_resource.h>
#include <ags/audio/file/ags_ipatch.h>

#include <ags/i18n.h>

void ags_audio_container_class_init(AgsAudioContainerClass *audio_container);
void ags_audio_container_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_container_init(AgsAudioContainer *audio_container);
void ags_audio_container_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_audio_container_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_audio_container_finalize(GObject *object);

enum{
  PROP_0,
  PROP_SOUNDCARD,
  PROP_FILENAME,
  PROP_PRESET,
  PROP_INSTRUMENT,
  PROP_SAMPLE,
  PROP_FILE_AUDIO_CHANNELS,
  PROP_FILE_SAMPLERATE,
  PROP_FILE_FRAME_COUNT,
  PROP_AUDIO_CHANNEL,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_AUDIO_SIGNAL,
  PROP_WAVE,
};

/**
 * SECTION:ags_audio_container
 * @short_description: Audio container input/output
 * @title: AgsAudioContainer
 * @section_id:
 * @include: ags/audio/file/ags_audio_container.h
 *
 * #AgsAudioContainer is the base object to read/write audio data.
 */

static gpointer ags_audio_container_parent_class = NULL;
static AgsConnectableInterface *ags_audio_container_parent_connectable_interface;

GType
ags_audio_container_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_audio_container = 0;

    static const GTypeInfo ags_audio_container_info = {
      sizeof (AgsAudioContainerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_container_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioContainer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_container_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_container_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_container = g_type_register_static(G_TYPE_OBJECT,
						      "AgsAudioContainer",
						      &ags_audio_container_info,
						      0);

    g_type_add_interface_static(ags_type_audio_container,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_audio_container);
  }

  return g_define_type_id__volatile;
}

void
ags_audio_container_class_init(AgsAudioContainerClass *audio_container)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_audio_container_parent_class = g_type_class_peek_parent(audio_container);

  gobject = (GObjectClass *) audio_container;

  gobject->set_property = ags_audio_container_set_property;
  gobject->get_property = ags_audio_container_get_property;

  gobject->finalize = ags_audio_container_finalize;
  
  /* properties */
  /**
   * AgsAudioContainer:soundcard:
   *
   * The assigned soundcard.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("soundcard of audio container"),
				   i18n_pspec("The soundcard what audio container has it's presets"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /**
   * AgsAudioContainer:filename:
   *
   * The assigned filename.
   * 
   * Since: 2.0.0
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
   * AgsAudioContainer:preset:
   *
   * The assigned preset.
   * 
   * Since: 2.0.0
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
   * AgsAudioContainer:instrument:
   *
   * The assigned instrument.
   * 
   * Since: 2.0.0
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
   * AgsAudioContainer:sample:
   *
   * The assigned sample.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("sample",
				   i18n_pspec("sample of audio container"),
				   i18n_pspec("The sample of audio container"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLE,
				  param_spec);

  /**
   * AgsAudioContainer:file-audio-channels:
   *
   * The audio channel count of this file.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("file-audio-channels",
				 i18n_pspec("file audio channels"),
				 i18n_pspec("The audio channel count of the file"),
				 0, G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE_AUDIO_CHANNELS,
				  param_spec);

  /**
   * AgsAudioContainer:file-samplerate:
   *
   * The samplerate of this file.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("file-samplerate",
				 i18n_pspec("file samplerate"),
				 i18n_pspec("The samplerate of the file"),
				 0, G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE_SAMPLERATE,
				  param_spec);

  /**
   * AgsAudioContainer:file-frame_count:
   *
   * The frame count of this file.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("file-frame-count",
				 i18n_pspec("file frame count"),
				 i18n_pspec("The frame count of the file"),
				 0, G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE_FRAME_COUNT,
				  param_spec);
  
  /**
   * AgsAudioContainer:audio-channel:
   *
   * The audio channel to be read.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_int("audio-channel",
				i18n_pspec("read audio channel"),
				i18n_pspec("The audio channel to be read"),
				-1, G_MAXINT,
				0,
				G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);

  /**
   * AgsAudioContainer:samplerate:
   *
   * The samplerate to be used.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("using samplerate"),
				 i18n_pspec("The samplerate to be used"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsAudioContainer:buffer-size:
   *
   * The buffer size to be used.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("buffer-size",
				 i18n_pspec("using buffer size"),
				 i18n_pspec("The buffer size to be used"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsAudioContainer:format:
   *
   * The format to be used.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("format",
				 i18n_pspec("using format"),
				 i18n_pspec("The format to be used"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsAudioContainer:audio-signal:
   *
   * The containing #AgsAudioSignal.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("audio-signal",
				    i18n_pspec("containing audio signal"),
				    i18n_pspec("The audio signal it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_SIGNAL,
				  param_spec);

  /**
   * AgsAudioContainer:wave:
   *
   * The containing #AgsWave.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("wave",
				    i18n_pspec("containing wave"),
				    i18n_pspec("The wave it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAVE,
				  param_spec);
}

void
ags_audio_container_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_audio_container_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_audio_container_init(AgsAudioContainer *audio_container)
{
  AgsConfig *config;
  
  audio_container->soundcard = NULL;

  audio_container->filename = NULL;
  audio_container->preset = NULL;
  audio_container->instrument = NULL;
  audio_container->sample = NULL;

  audio_container->file_audio_channels = 0;
  audio_container->file_samplerate = 0;
  audio_container->file_frame_count = 0;

  audio_container->samplerate = 0;
  audio_container->buffer_size = 0;
  audio_container->format = 0;

  audio_container->audio_channel = -1;

  audio_container->sound_container = NULL;

  audio_container->audio_signal = NULL;
  audio_container->wave = NULL;
}

void
ags_audio_container_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsAudioContainer *audio_container;

  audio_container = AGS_AUDIO_CONTAINER(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;
      
      soundcard = (GObject *) g_value_get_object(value);

      if(soundcard == ((GObject *) audio_container->soundcard)){
	return;
      }

      if(audio_container->soundcard != NULL){
	g_object_unref(audio_container->soundcard);
      }
      
      if(soundcard != NULL){
	g_object_ref(G_OBJECT(soundcard));
      }
      
      audio_container->soundcard = (GObject *) soundcard;
    }
    break;
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = (gchar *) g_value_get_string(value);

      if(audio_container->filename == filename){
	return;
      }
      
      if(audio_container->filename != NULL){
	g_free(audio_container->filename);
      }

      audio_container->filename = g_strdup(filename);
    }
    break;
  case PROP_PRESET:
    {
      gchar *preset;

      preset = (gchar *) g_value_get_string(value);

      if(audio_container->preset == preset){
	return;
      }
      
      if(audio_container->preset != NULL){
	g_free(audio_container->preset);
      }

      audio_container->preset = g_strdup(preset);
    }
    break;
  case PROP_INSTRUMENT:
    {
      gchar *instrument;

      instrument = (gchar *) g_value_get_string(value);

      if(audio_container->instrument == instrument){
	return;
      }
      
      if(audio_container->instrument != NULL){
	g_free(audio_container->instrument);
      }

      audio_container->instrument = g_strdup(instrument);
    }
    break;
  case PROP_SAMPLE:
    {
      gchar *sample;

      sample = (gchar *) g_value_get_string(value);

      if(audio_container->sample == sample){
	return;
      }
      
      if(audio_container->sample != NULL){
	g_free(audio_container->sample);
      }

      audio_container->sample = g_strdup(sample);
    }
    break;
  case PROP_FILE_AUDIO_CHANNELS:
    {
      audio_container->file_audio_channels = g_value_get_uint(value);
    }
    break;
  case PROP_FILE_SAMPLERATE:
    {
      audio_container->file_samplerate = g_value_get_uint(value);
    }
    break;
  case PROP_FILE_FRAME_COUNT:
    {
      audio_container->file_frame_count = g_value_get_uint(value);
    }
    break;
  case PROP_SAMPLERATE:
    {
      audio_container->samplerate = g_value_get_uint(value);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      audio_container->buffer_size = g_value_get_uint(value);
    }
    break;
  case PROP_FORMAT:
    {
      audio_container->format = g_value_get_uint(value);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      audio_container->audio_channel = g_value_get_int(value);
    }
    break;
  case PROP_AUDIO_SIGNAL:
    {
      AgsAudioSignal *audio_signal;

      audio_signal = (AgsAudioSignal *) g_value_get_pointer(value);

      if(audio_signal == NULL ||
	 g_list_find(audio_container->audio_signal, audio_signal) != NULL){
	return;
      }
      
      ags_audio_container_add_audio_signal(audio_container,
					   audio_signal);
    }
    break;
  case PROP_WAVE:
    {
      AgsWave *wave;

      wave = (AgsWave *) g_value_get_pointer(value);

      if(wave == NULL ||
	 g_list_find(audio_container->wave, wave) != NULL){
	return;
      }
      
      ags_audio_container_add_wave(audio_container,
				   wave);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_container_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsAudioContainer *audio_container;

  audio_container = AGS_AUDIO_CONTAINER(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, audio_container->soundcard);
    }
    break;
  case PROP_FILENAME:
    {
      g_value_set_string(value, audio_container->filename);
    }
    break;
  case PROP_PRESET:
    {
      g_value_set_string(value, audio_container->preset);
    }
    break;
  case PROP_INSTRUMENT:
    {
      g_value_set_string(value, audio_container->instrument);
    }
    break;
  case PROP_SAMPLE:
    {
      g_value_set_string(value, audio_container->sample);
    }
    break;
  case PROP_FILE_AUDIO_CHANNELS:
    {
      g_value_set_uint(value, audio_container->file_audio_channels);
    }
    break;
  case PROP_FILE_SAMPLERATE:
    {
      g_value_set_uint(value, audio_container->file_samplerate);
    }
    break;
  case PROP_FILE_FRAME_COUNT:
    {
      g_value_set_uint(value, audio_container->file_frame_count);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_value_set_uint(value, audio_container->samplerate);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value, audio_container->buffer_size);
    }
    break;
  case PROP_FORMAT:
    {
      g_value_set_uint(value, audio_container->format);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      g_value_set_int(value, audio_container->audio_channel);
    }
    break;
  case PROP_AUDIO_SIGNAL:
    {
      g_value_set_pointer(value, g_list_copy(audio_container->audio_signal));
    }
    break;
  case PROP_WAVE:
    {
      g_value_set_pointer(value, g_list_copy(audio_container->wave));
    }
    break;    
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_container_finalize(GObject *gobject)
{
  AgsAudioContainer *audio_container;

  audio_container = AGS_AUDIO_CONTAINER(gobject);
  		   
  /* sound resource */
  if(audio_container->sound_container != NULL){
    g_object_unref(audio_container->sound_container);
  }  

  /* AgsAudioSignal */
  g_list_free_full(audio_container->audio_signal,
		   g_object_unref);
		   
  /* AgsWave */
  g_list_free_full(audio_container->wave,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_audio_container_parent_class)->finalize(gobject);
}

/**
 * ags_audio_container_check_suffix:
 * @filename: the filename
 * 
 * Check suffix.
 * 
 * Returns: %TRUE if suffix supported, else %FALSE
 * 
 * Since: 2.0.0
 */
gboolean
ags_audio_container_check_suffix(gchar *filename)
{
  if(g_str_has_suffix(filename, ".sf2") ||
     g_str_has_suffix(filename, ".gig") ||
     g_str_has_suffix(filename, ".dls")){
    return(TRUE);
  }

  return(FALSE);
}

/**
 * ags_audio_container_add_audio_signal:
 * @audio_container: the #AgsAudioContainer
 * @audio_signal: the #AgsAudioSignal
 * 
 * Add @audio_signal to @audio_container.
 * 
 * Since: 2.0.0
 */
void
ags_audio_container_add_audio_signal(AgsAudioContainer *audio_container, GObject *audio_signal)
{
  if(!AGS_IS_AUDIO_CONTAINER(audio_container) ||
     !AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return;
  }

  if(g_list_find(audio_container->audio_signal, audio_signal) == NULL){
    g_object_ref(audio_signal);
    audio_container->audio_signal = g_list_prepend(audio_container->audio_signal,
						   audio_signal);
  }
}

/**
 * ags_audio_container_remove_audio_signal:
 * @audio_container: the #AgsAudioContainer
 * @audio_signal: the #AgsAudioSignal
 * 
 * Remove @audio_signal from @audio_container.
 * 
 * Since: 2.0.0
 */
void
ags_audio_container_remove_audio_signal(AgsAudioContainer *audio_container, GObject *audio_signal)
{
  if(!AGS_IS_AUDIO_CONTAINER(audio_container) ||
     !AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return;
  }

  if(g_list_find(audio_container->audio_signal, audio_signal) != NULL){
    audio_container->audio_signal = g_list_prepend(audio_container->audio_signal,
						   audio_signal);
    g_object_unref(audio_signal);
  }
}

/**
 * ags_audio_container_add_wave:
 * @audio_container: the #AgsAudioContainer
 * @wave: the #AgsWave
 * 
 * Add @wave to @audio_container.
 * 
 * Since: 2.0.0
 */
void
ags_audio_container_add_wave(AgsAudioContainer *audio_container, GObject *wave)
{
  if(!AGS_IS_AUDIO_CONTAINER(audio_container) ||
     !AGS_IS_WAVE(wave)){
    return;
  }

  if(g_list_find(audio_container->wave, wave) == NULL){
    g_object_ref(wave);
    audio_container->wave = g_list_prepend(audio_container->wave,
					   wave);
  }
}

/**
 * ags_audio_container_remove_wave:
 * @audio_container: the #AgsAudioContainer
 * @wave: the #AgsWave
 * 
 * Remove @wave from @audio_container.
 * 
 * Since: 2.0.0
 */
void
ags_audio_container_remove_wave(AgsAudioContainer *audio_container, GObject *wave)
{
  if(!AGS_IS_AUDIO_CONTAINER(audio_container) ||
     !AGS_IS_WAVE(wave)){
    return;
  }

  if(g_list_find(audio_container->wave, wave) != NULL){
    audio_container->wave = g_list_prepend(audio_container->wave,
					   wave);
    g_object_unref(wave);
  }
}

/**
 * ags_audio_container_open:
 * @audio_container: the #AgsAudioContainer
 *
 * Open the #AgsAudioContainer in read mode.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_audio_container_open(AgsAudioContainer *audio_container)
{
  if(!AGS_IS_AUDIO_CONTAINER(audio_container)){
    return(FALSE);
  }

#ifdef AGS_DEBUG
  g_message("ags_audio_container_open: %s", audio_container->filename);
#endif

  if(g_file_test(audio_container->filename, G_FILE_TEST_EXISTS)){
    if(ags_audio_container_check_suffix(audio_container->filename)){
      if(ags_ipatch_check_suffix(audio_container->filename)){
	audio_container->sound_container = (GObject *) ags_ipatch_new();
	g_object_ref(audio_container->sound_container);
      }

      if(ags_sound_container_open(AGS_SOUND_CONTAINER(audio_container->sound_container),
				  audio_container->filename)){
	return(TRUE);
      }
    }else{
      g_message("ags_audio_container_open: unknown file type\n");
    }
  }
  
  return(FALSE);
}

gboolean
ags_audio_container_open_from_data(AgsAudioContainer *audio_container, gchar *data)
{  
  g_message("missing implementation");

  //TODO:JK: implement me
}

gboolean
ags_audio_container_rw_open(AgsAudioContainer *audio_container,
			    gboolean create)
{
  g_message("missing implementation");

  //TODO:JK: implement me
}

/**
 * ags_audio_container_close:
 * @audio_container: the #AgsAudioContainer
 *
 * Close the #AgsAudioContainer.
 *
 * Since: 2.0.0
 */
void
ags_audio_container_close(AgsAudioContainer *audio_container)
{
  if(!AGS_IS_AUDIO_CONTAINER(audio_container)){
    return;
  }

  ags_sound_container_close(AGS_SOUND_CONTAINER(audio_container->sound_container));
}

/**
 * ags_audio_container_read:
 * @audio_container: the #AgsAudioContainer
 * @audio_channel: nth channel
 * @format: the format
 * @error: returned error
 *
 * Read audio buffer.
 *
 * Since: 2.0.0
 */
void*
ags_audio_container_read(AgsAudioContainer *audio_container,
			 guint audio_channel,
			 guint format,
			 GError **error)
{
  GList *start_sound_resource;
  void *buffer;

  if(!AGS_IS_AUDIO_CONTAINER(audio_container)){
    return(NULL);
  }

  start_sound_resource = ags_sound_container_get_resource_current(AGS_SOUND_CONTAINER(audio_container->sound_container));

  if(start_sound_resource == NULL){
    return(NULL);
  }

  ags_sound_resource_info(AGS_SOUND_RESOURCE(start_sound_resource->data),
			  &(audio_container->file_frame_count),
			  NULL, NULL);
  
  buffer = ags_stream_alloc(audio_container->file_frame_count,
			    format);

  ags_sound_resource_read(AGS_SOUND_RESOURCE(start_sound_resource->data),
			  buffer, 1,
			  audio_channel,
			  audio_container->file_frame_count, format);

#if 0
#endif
  
  return(buffer);
}

/**
 * ags_audio_container_read_audio_signal:
 * @audio_container: the #AgsAudioContainer
 *
 * Convert the #AgsAudioContainer to a #GList-struct of #AgsAudioSignal.
 *
 * Since: 2.0.0
 */
GList*
ags_audio_container_read_audio_signal(AgsAudioContainer *audio_container)
{
  GList *start_sound_resource, *sound_resource;
  GList *list;

  if(!AGS_IS_AUDIO_CONTAINER(audio_container)){
    return;
  }

  sound_resource =
    start_sound_resource = ags_sound_container_get_resource_current(AGS_SOUND_CONTAINER(audio_container->sound_container));

  list = NULL;
  
  while(sound_resource != NULL){
    GList *current;
    
    current = ags_sound_resource_read_audio_signal(AGS_SOUND_RESOURCE(sound_resource->data),
						   audio_container->soundcard,
						   audio_container->audio_channel);
    
    if(current != NULL){
      if(list == NULL){
	list = current;
      }else{
	list = g_list_concat(list, current);
      }
    }

    sound_resource = sound_resource->next;
  }

  g_list_free(start_sound_resource);
  
  audio_container->audio_signal = list;
}

GList*
ags_audio_container_read_wave(AgsAudioContainer *audio_container,
			      guint64 x_offset,
			      gdouble delay, guint attack)
{
  g_message("missing implementation");

  //TODO:JK: implement me
}

void
ags_audio_container_seek(AgsAudioContainer *audio_container, guint frames, gint whence)
{
  g_message("missing implementation");

  //TODO:JK: implement me
}

void
ags_audio_container_write(AgsAudioContainer *audio_container,
			  void *buffer, guint buffer_size,
			  guint format)
{
  g_message("missing implementation");

  //TODO:JK: implement me
}

void
ags_audio_container_flush(AgsAudioContainer *audio_container)
{
  g_message("missing implementation");

  //TODO:JK: implement me
}

/**
 * ags_audio_container_new:
 * @filename: the filename
 * @preset: the preset
 * @instrument: the instrument
 * @sample: the sample
 * @soundcard: defaults of #AgsSoundcard
 * @audio_channel: the audio channel to read
 *
 * Create a new instance of #AgsAudioContainer.
 *
 * Returns: the new #AgsAudioContainer.
 *
 * Since: 2.0.0
 */
AgsAudioContainer*
ags_audio_container_new(gchar *filename,
			gchar *preset,
			gchar *instrument,
			gchar *sample,
			GObject *soundcard,
			gint audio_channel)
{
  AgsAudioContainer *audio_container;

  audio_container = (AgsAudioContainer *) g_object_new(AGS_TYPE_AUDIO_CONTAINER,
						       "filename", filename,
						       "soundcard", soundcard,
						       "audio-channel", audio_channel,
						       NULL);

  return(audio_container);
}
