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

#include <ags/audio/file/ags_audio_file.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_wave.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/file/ags_sound_resource.h>
#include <ags/audio/file/ags_sndfile.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sndfile.h>
#include <string.h>

#include <ags/i18n.h>

void ags_audio_file_class_init(AgsAudioFileClass *audio_file);
void ags_audio_file_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_file_init(AgsAudioFile *audio_file);
void ags_audio_file_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_audio_file_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_audio_file_finalize(GObject *object);

AgsUUID* ags_audio_file_get_uuid(AgsConnectable *connectable);
gboolean ags_audio_file_has_resource(AgsConnectable *connectable);
gboolean ags_audio_file_is_ready(AgsConnectable *connectable);
void ags_audio_file_add_to_registry(AgsConnectable *connectable);
void ags_audio_file_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_audio_file_list_resource(AgsConnectable *connectable);
xmlNode* ags_audio_file_xml_compose(AgsConnectable *connectable);
void ags_audio_file_xml_parse(AgsConnectable *connectable,
				   xmlNode *node);
gboolean ags_audio_file_is_connected(AgsConnectable *connectable);
void ags_audio_file_connect(AgsConnectable *connectable);
void ags_audio_file_disconnect(AgsConnectable *connectable);

enum{
  PROP_0,
  PROP_SOUNDCARD,
  PROP_FILENAME,
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

enum{
  READ_BUFFER,
  LAST_SIGNAL,
};

/**
 * SECTION:ags_audio_file
 * @short_description: Audio file input/output
 * @title: AgsAudioFile
 * @section_id:
 * @include: ags/audio/file/ags_audio_file.h
 *
 * #AgsAudioFile is the base object to read/write audio data.
 */

static gpointer ags_audio_file_parent_class = NULL;
static guint signals[LAST_SIGNAL];

static pthread_mutex_t ags_audio_file_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_audio_file_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_audio_file = 0;

    static const GTypeInfo ags_audio_file_info = {
      sizeof (AgsAudioFileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_file_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_file_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_file_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_file = g_type_register_static(G_TYPE_OBJECT,
						 "AgsAudioFile",
						 &ags_audio_file_info,
						 0);

    g_type_add_interface_static(ags_type_audio_file,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_audio_file);
  }

  return g_define_type_id__volatile;
}

void
ags_audio_file_class_init(AgsAudioFileClass *audio_file)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_audio_file_parent_class = g_type_class_peek_parent(audio_file);

  gobject = (GObjectClass *) audio_file;

  gobject->set_property = ags_audio_file_set_property;
  gobject->get_property = ags_audio_file_get_property;

  gobject->finalize = ags_audio_file_finalize;
  
  /* properties */
  /**
   * AgsAudioFile:soundcard:
   *
   * The assigned soundcard.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("soundcard of audio file"),
				   i18n_pspec("The soundcard what audio file has it's presets"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /**
   * AgsAudioFile:filename:
   *
   * The assigned filename.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("filename",
				   i18n_pspec("filename of audio file"),
				   i18n_pspec("The filename of audio file"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsAudioFile:file-audio-channels:
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
   * AgsAudioFile:file-samplerate:
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
   * AgsAudioFile:file-frame_count:
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
   * AgsAudioFile:audio-channel:
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
   * AgsAudioFile:samplerate:
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
   * AgsAudioFile:buffer-size:
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
   * AgsAudioFile:format:
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
   * AgsAudioFile:audio-signal:
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
   * AgsAudioFile:wave:
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
ags_audio_file_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_audio_file_get_uuid;
  connectable->has_resource = ags_audio_file_has_resource;
  connectable->is_ready = ags_audio_file_is_ready;

  connectable->add_to_registry = ags_audio_file_add_to_registry;
  connectable->remove_from_registry = ags_audio_file_remove_from_registry;

  connectable->list_resource = ags_audio_file_list_resource;
  connectable->xml_compose = ags_audio_file_xml_compose;
  connectable->xml_parse = ags_audio_file_xml_parse;

  connectable->is_connected = ags_audio_file_is_connected;
  
  connectable->connect = ags_audio_file_connect;
  connectable->disconnect = ags_audio_file_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_audio_file_init(AgsAudioFile *audio_file)
{
  AgsConfig *config;

  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  audio_file->flags = 0;

  /* add audio file mutex */
  audio_file->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  audio_file->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);  

  /* uuid */
  audio_file->uuid = ags_uuid_alloc();
  ags_uuid_generate(audio_file->uuid);
  
  audio_file->soundcard = NULL;

  audio_file->filename = NULL;

  audio_file->file_audio_channels = 0;
  audio_file->file_samplerate = 0;
  audio_file->file_frame_count = 0;

  config = ags_config_get_instance();
  
  audio_file->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  audio_file->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  audio_file->format = ags_soundcard_helper_config_get_format(config);

  audio_file->audio_channel = -1;

  audio_file->sound_resource = NULL;
  
  audio_file->audio_signal = NULL;
  audio_file->wave = NULL;
}

void
ags_audio_file_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsAudioFile *audio_file;

  pthread_mutex_t *audio_file_mutex;

  audio_file = AGS_AUDIO_FILE(gobject);

  /* get audio file mutex */
  pthread_mutex_lock(ags_audio_file_get_class_mutex());
  
  audio_file_mutex = audio_file->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_file_get_class_mutex());

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;
      
      soundcard = (GObject *) g_value_get_object(value);

      pthread_mutex_lock(audio_file_mutex);

      if(soundcard == ((GObject *) audio_file->soundcard)){
	pthread_mutex_unlock(audio_file_mutex);

	return;
      }

      if(audio_file->soundcard != NULL){
	g_object_unref(audio_file->soundcard);
      }
      
      if(soundcard != NULL){
	g_object_ref(G_OBJECT(soundcard));
      }
      
      audio_file->soundcard = (GObject *) soundcard;

      pthread_mutex_unlock(audio_file_mutex);
    }
    break;
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = (gchar *) g_value_get_string(value);

      pthread_mutex_lock(audio_file_mutex);

      if(audio_file->filename == filename){
	pthread_mutex_unlock(audio_file_mutex);

	return;
      }
      
      if(audio_file->filename != NULL){
	g_free(audio_file->filename);
      }

      audio_file->filename = g_strdup(filename);

      pthread_mutex_unlock(audio_file_mutex);
    }
    break;
  case PROP_FILE_AUDIO_CHANNELS:
    {
      pthread_mutex_lock(audio_file_mutex);

      audio_file->file_audio_channels = g_value_get_uint(value);

      pthread_mutex_unlock(audio_file_mutex);
    }
    break;
  case PROP_FILE_SAMPLERATE:
    {
      pthread_mutex_lock(audio_file_mutex);

      audio_file->file_samplerate = g_value_get_uint(value);

      pthread_mutex_unlock(audio_file_mutex);
    }
    break;
  case PROP_FILE_FRAME_COUNT:
    {
      pthread_mutex_lock(audio_file_mutex);

      audio_file->file_frame_count = g_value_get_uint(value);

      pthread_mutex_unlock(audio_file_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      pthread_mutex_lock(audio_file_mutex);

      audio_file->samplerate = g_value_get_uint(value);

      pthread_mutex_unlock(audio_file_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      pthread_mutex_lock(audio_file_mutex);

      audio_file->buffer_size = g_value_get_uint(value);

      pthread_mutex_unlock(audio_file_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      pthread_mutex_lock(audio_file_mutex);

      audio_file->format = g_value_get_uint(value);

      pthread_mutex_unlock(audio_file_mutex);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      pthread_mutex_lock(audio_file_mutex);

      audio_file->audio_channel = g_value_get_int(value);

      pthread_mutex_unlock(audio_file_mutex);
    }
    break;
  case PROP_AUDIO_SIGNAL:
    {
      AgsAudioSignal *audio_signal;

      audio_signal = (AgsAudioSignal *) g_value_get_pointer(value);

      pthread_mutex_lock(audio_file_mutex);

      if(audio_signal == NULL ||
	 g_list_find(audio_file->audio_signal, audio_signal) != NULL){
	pthread_mutex_unlock(audio_file_mutex);

	return;
      }

      pthread_mutex_unlock(audio_file_mutex);
      
      ags_audio_file_add_audio_signal(audio_file,
				      audio_signal);
    }
    break;
  case PROP_WAVE:
    {
      AgsWave *wave;

      wave = (AgsWave *) g_value_get_pointer(value);

      pthread_mutex_lock(audio_file_mutex);

      if(wave == NULL ||
	 g_list_find(audio_file->wave, wave) != NULL){
	pthread_mutex_unlock(audio_file_mutex);

	return;
      }

      pthread_mutex_unlock(audio_file_mutex);
      
      ags_audio_file_add_wave(audio_file,
			      wave);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_file_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsAudioFile *audio_file;

  pthread_mutex_t *audio_file_mutex;

  audio_file = AGS_AUDIO_FILE(gobject);

  /* get audio file mutex */
  pthread_mutex_lock(ags_audio_file_get_class_mutex());
  
  audio_file_mutex = audio_file->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_file_get_class_mutex());

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      pthread_mutex_lock(audio_file_mutex);

      g_value_set_object(value, audio_file->soundcard);

      pthread_mutex_unlock(audio_file_mutex);
    }
    break;
  case PROP_FILENAME:
    {
      pthread_mutex_lock(audio_file_mutex);

      g_value_set_string(value, audio_file->filename);

      pthread_mutex_unlock(audio_file_mutex);
    }
    break;
  case PROP_FILE_AUDIO_CHANNELS:
    {
      pthread_mutex_lock(audio_file_mutex);

      g_value_set_uint(value, audio_file->file_audio_channels);

      pthread_mutex_unlock(audio_file_mutex);
    }
    break;
  case PROP_FILE_SAMPLERATE:
    {
      pthread_mutex_lock(audio_file_mutex);

      g_value_set_uint(value, audio_file->file_samplerate);

      pthread_mutex_unlock(audio_file_mutex);
    }
    break;
  case PROP_FILE_FRAME_COUNT:
    {
      pthread_mutex_lock(audio_file_mutex);

      g_value_set_uint(value, audio_file->file_frame_count);

      pthread_mutex_unlock(audio_file_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      pthread_mutex_lock(audio_file_mutex);

      g_value_set_uint(value, audio_file->samplerate);

      pthread_mutex_unlock(audio_file_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      pthread_mutex_lock(audio_file_mutex);

      g_value_set_uint(value, audio_file->buffer_size);

      pthread_mutex_unlock(audio_file_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      pthread_mutex_lock(audio_file_mutex);

      g_value_set_uint(value, audio_file->format);

      pthread_mutex_unlock(audio_file_mutex);
    }
    break;
  case PROP_AUDIO_CHANNEL:
    {
      pthread_mutex_lock(audio_file_mutex);

      g_value_set_int(value, audio_file->audio_channel);

      pthread_mutex_unlock(audio_file_mutex);
    }
    break;
  case PROP_AUDIO_SIGNAL:
    {
      pthread_mutex_lock(audio_file_mutex);

      g_value_set_pointer(value, g_list_copy(audio_file->audio_signal));

      pthread_mutex_unlock(audio_file_mutex);
    }
    break;
  case PROP_WAVE:
    {
      pthread_mutex_lock(audio_file_mutex);

      g_value_set_pointer(value, g_list_copy(audio_file->wave));

      pthread_mutex_unlock(audio_file_mutex);
    }
    break;    
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_file_finalize(GObject *gobject)
{
  AgsAudioFile *audio_file;

  audio_file = AGS_AUDIO_FILE(gobject);		   

  pthread_mutex_destroy(audio_file->obj_mutex);
  free(audio_file->obj_mutex);

  pthread_mutexattr_destroy(audio_file->obj_mutexattr);
  free(audio_file->obj_mutexattr);
  		   
  /* sound resource */
  if(audio_file->sound_resource != NULL){
    g_object_unref(audio_file->sound_resource);
  }  

  /* AgsAudioSignal */
  g_list_free_full(audio_file->audio_signal,
		   g_object_unref);
		   
  /* AgsWave */
  g_list_free_full(audio_file->wave,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_audio_file_parent_class)->finalize(gobject);
}

AgsUUID*
ags_audio_file_get_uuid(AgsConnectable *connectable)
{
  AgsAudioFile *audio_file;
  
  AgsUUID *ptr;

  pthread_mutex_t *audio_file_mutex;

  audio_file = AGS_AUDIO_FILE(connectable);

  /* get audio file mutex */
  pthread_mutex_lock(ags_audio_file_get_class_mutex());
  
  audio_file_mutex = audio_file->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_file_get_class_mutex());

  /* get UUID */
  pthread_mutex_lock(audio_file_mutex);

  ptr = audio_file->uuid;

  pthread_mutex_unlock(audio_file_mutex);
  
  return(ptr);
}

gboolean
ags_audio_file_has_resource(AgsConnectable *connectable)
{
  return(TRUE);
}

gboolean
ags_audio_file_is_ready(AgsConnectable *connectable)
{
  AgsAudioFile *audio_file;
  
  gboolean is_ready;

  pthread_mutex_t *audio_file_mutex;

  audio_file = AGS_AUDIO_FILE(connectable);

  /* get audio file mutex */
  pthread_mutex_lock(ags_audio_file_get_class_mutex());
  
  audio_file_mutex = audio_file->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_file_get_class_mutex());

  /* check is ready */
  pthread_mutex_lock(audio_file_mutex);
  
  is_ready = (((AGS_AUDIO_FILE_ADDED_TO_REGISTRY & (audio_file->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(audio_file_mutex);

  return(is_ready);
}

void
ags_audio_file_add_to_registry(AgsConnectable *connectable)
{
  AgsAudioFile *audio_file;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  if(ags_connectable_is_ready(connectable)){
    return;
  }

  audio_file = AGS_AUDIO_FILE(connectable);

  ags_audio_file_set_flags(audio_file, AGS_AUDIO_FILE_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(&(entry->entry),
		       (gpointer) audio_file);
    ags_registry_add_entry(registry,
			   entry);
  }  
}

void
ags_audio_file_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_audio_file_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_audio_file_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_audio_file_xml_parse(AgsConnectable *connectable,
			      xmlNode *node)
{
  //TODO:JK: implement me  
}

gboolean
ags_audio_file_is_connected(AgsConnectable *connectable)
{
  AgsAudioFile *audio_file;
  
  gboolean is_connected;

  pthread_mutex_t *audio_file_mutex;

  audio_file = AGS_AUDIO_FILE(connectable);

  /* get audio file mutex */
  pthread_mutex_lock(ags_audio_file_get_class_mutex());
  
  audio_file_mutex = audio_file->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_file_get_class_mutex());

  /* check is connected */
  pthread_mutex_lock(audio_file_mutex);

  is_connected = (((AGS_AUDIO_FILE_CONNECTED & (audio_file->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(audio_file_mutex);

  return(is_connected);
}

void
ags_audio_file_connect(AgsConnectable *connectable)
{
  AgsAudioFile *audio_file;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  audio_file = AGS_AUDIO_FILE(connectable);
  
  ags_audio_file_set_flags(audio_file, AGS_AUDIO_FILE_CONNECTED);
}

void
ags_audio_file_disconnect(AgsConnectable *connectable)
{
  AgsAudioFile *audio_file;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  audio_file = AGS_AUDIO_FILE(connectable);

  ags_audio_file_unset_flags(audio_file, AGS_AUDIO_FILE_CONNECTED);
}

/**
 * ags_audio_file_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.36
 */
pthread_mutex_t*
ags_audio_file_get_class_mutex()
{
  return(&ags_audio_file_class_mutex);
}

/**
 * ags_audio_file_test_flags:
 * @audio_file: the #AgsAudioFile
 * @flags: the flags
 *
 * Test @flags to be set on @audio_file.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.0.36
 */
gboolean
ags_audio_file_test_flags(AgsAudioFile *audio_file, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *audio_file_mutex;

  if(!AGS_IS_AUDIO_FILE(audio_file)){
    return(FALSE);
  }

  /* get audio_file mutex */
  pthread_mutex_lock(ags_audio_file_get_class_mutex());
  
  audio_file_mutex = audio_file->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_file_get_class_mutex());

  /* test */
  pthread_mutex_lock(audio_file_mutex);

  retval = (flags & (audio_file->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(audio_file_mutex);

  return(retval);
}

/**
 * ags_audio_file_set_flags:
 * @audio_file: the #AgsAudioFile
 * @flags: see #AgsAudioFileFlags-enum
 *
 * Enable a feature of @audio_file.
 *
 * Since: 2.0.36
 */
void
ags_audio_file_set_flags(AgsAudioFile *audio_file, guint flags)
{
  pthread_mutex_t *audio_file_mutex;

  if(!AGS_IS_AUDIO_FILE(audio_file)){
    return;
  }

  /* get audio_file mutex */
  pthread_mutex_lock(ags_audio_file_get_class_mutex());
  
  audio_file_mutex = audio_file->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_file_get_class_mutex());

  //TODO:JK: add more?

  /* set flags */
  pthread_mutex_lock(audio_file_mutex);

  audio_file->flags |= flags;
  
  pthread_mutex_unlock(audio_file_mutex);
}
    
/**
 * ags_audio_file_unset_flags:
 * @audio_file: the #AgsAudioFile
 * @flags: see #AgsAudioFileFlags-enum
 *
 * Disable a feature of @audio_file.
 *
 * Since: 2.0.36
 */
void
ags_audio_file_unset_flags(AgsAudioFile *audio_file, guint flags)
{  
  pthread_mutex_t *audio_file_mutex;

  if(!AGS_IS_AUDIO_FILE(audio_file)){
    return;
  }

  /* get audio_file mutex */
  pthread_mutex_lock(ags_audio_file_get_class_mutex());
  
  audio_file_mutex = audio_file->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_file_get_class_mutex());

  //TODO:JK: add more?

  /* unset flags */
  pthread_mutex_lock(audio_file_mutex);

  audio_file->flags &= (~flags);
  
  pthread_mutex_unlock(audio_file_mutex);
}

/**
 * ags_audio_file_check_suffix:
 * @filename: the filename
 * 
 * Check suffix.
 * 
 * Returns: %TRUE if suffix supported, else %FALSE
 * 
 * Since: 2.0.0
 */
gboolean
ags_audio_file_check_suffix(gchar *filename)
{
  if(g_str_has_suffix(filename, ".wav") ||
     g_str_has_suffix(filename, ".ogg") ||
     g_str_has_suffix(filename, ".flac") ||
     g_str_has_suffix(filename, ".aiff")){
    return(TRUE);
  }

  return(FALSE);
}

/**
 * ags_audio_file_add_audio_signal:
 * @audio_file: the #AgsAudioFile
 * @audio_signal: the #AgsAudioSignal
 * 
 * Add @audio_signal to @audio_file.
 * 
 * Since: 2.0.0
 */
void
ags_audio_file_add_audio_signal(AgsAudioFile *audio_file, GObject *audio_signal)
{
  pthread_mutex_t *audio_file_mutex;

  if(!AGS_IS_AUDIO_FILE(audio_file) ||
     !AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return;
  }

  /* get audio_file mutex */
  pthread_mutex_lock(ags_audio_file_get_class_mutex());
  
  audio_file_mutex = audio_file->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_file_get_class_mutex());

  if(g_list_find(audio_file->audio_signal, audio_signal) == NULL){
    g_object_ref(audio_signal);
    audio_file->audio_signal = g_list_prepend(audio_file->audio_signal,
					      audio_signal);
  }
}

/**
 * ags_audio_file_remove_audio_signal:
 * @audio_file: the #AgsAudioFile
 * @audio_signal: the #AgsAudioSignal
 * 
 * Remove @audio_signal from @audio_file.
 * 
 * Since: 2.0.0
 */
void
ags_audio_file_remove_audio_signal(AgsAudioFile *audio_file, GObject *audio_signal)
{
  pthread_mutex_t *audio_file_mutex;

  if(!AGS_IS_AUDIO_FILE(audio_file) ||
     !AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return;
  }

  /* get audio_file mutex */
  pthread_mutex_lock(ags_audio_file_get_class_mutex());
  
  audio_file_mutex = audio_file->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_file_get_class_mutex());

  if(g_list_find(audio_file->audio_signal, audio_signal) != NULL){
    audio_file->audio_signal = g_list_prepend(audio_file->audio_signal,
					      audio_signal);
    g_object_unref(audio_signal);
  }
}

/**
 * ags_audio_file_add_wave:
 * @audio_file: the #AgsAudioFile
 * @wave: the #AgsWave
 * 
 * Add @wave to @audio_file.
 * 
 * Since: 2.0.0
 */
void
ags_audio_file_add_wave(AgsAudioFile *audio_file, GObject *wave)
{
  pthread_mutex_t *audio_file_mutex;

  if(!AGS_IS_AUDIO_FILE(audio_file) ||
     !AGS_IS_WAVE(wave)){
    return;
  }

  /* get audio_file mutex */
  pthread_mutex_lock(ags_audio_file_get_class_mutex());
  
  audio_file_mutex = audio_file->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_file_get_class_mutex());

  if(g_list_find(audio_file->wave, wave) == NULL){
    g_object_ref(wave);
    audio_file->wave = g_list_prepend(audio_file->wave,
				      wave);
  }
}

/**
 * ags_audio_file_remove_wave:
 * @audio_file: the #AgsAudioFile
 * @wave: the #AgsWave
 * 
 * Remove @wave from @audio_file.
 * 
 * Since: 2.0.0
 */
void
ags_audio_file_remove_wave(AgsAudioFile *audio_file, GObject *wave)
{
  pthread_mutex_t *audio_file_mutex;

  if(!AGS_IS_AUDIO_FILE(audio_file) ||
     !AGS_IS_WAVE(wave)){
    return;
  }

  /* get audio_file mutex */
  pthread_mutex_lock(ags_audio_file_get_class_mutex());
  
  audio_file_mutex = audio_file->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_file_get_class_mutex());

  if(g_list_find(audio_file->wave, wave) != NULL){
    audio_file->wave = g_list_prepend(audio_file->wave,
				      wave);
    g_object_unref(wave);
  }
}

/**
 * ags_audio_file_open:
 * @audio_file: the #AgsAudioFile
 *
 * Open the #AgsAudioFile in read mode.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_audio_file_open(AgsAudioFile *audio_file)
{
  GObject *sound_resource;

  gchar *filename;
  guint file_audio_channels;
  guint file_samplerate;
  guint file_frame_count;
  
  pthread_mutex_t *audio_file_mutex;

  if(!AGS_IS_AUDIO_FILE(audio_file)){
    return(FALSE);
  }

#ifdef AGS_DEBUG
  g_message("ags_audio_file_open: %s", audio_file->filename);
#endif

  /* get audio_file mutex */
  pthread_mutex_lock(ags_audio_file_get_class_mutex());
  
  audio_file_mutex = audio_file->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_file_get_class_mutex());

  sound_resource = NULL;

  g_object_get(audio_file,
	       "filename", &filename,
	       NULL);

  if(g_file_test(filename, G_FILE_TEST_EXISTS)){
    if(ags_audio_file_check_suffix(filename)){
      pthread_mutex_lock(audio_file_mutex);
      
      sound_resource = 
	audio_file->sound_resource = (GObject *) ags_sndfile_new();
      g_object_ref(audio_file->sound_resource);

      pthread_mutex_unlock(audio_file_mutex);
    
      if(ags_sound_resource_open(AGS_SOUND_RESOURCE(sound_resource),
				 filename)){
	//FIXME:JK: this call should occure just before reading frames because of the new iterate functions of an AgsPlayable
	ags_sound_resource_info(AGS_SOUND_RESOURCE(sound_resource),
				&file_frame_count,
				NULL, NULL);

	g_object_set(audio_file,
		     "file-frame-count", file_frame_count,
		     NULL);

	ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sound_resource),
				       &file_audio_channels,
				       &file_samplerate,
				       NULL,
				       NULL);

	g_object_set(audio_file,
		     "file-audio-channels", file_audio_channels,
		     "file-samplerate", file_samplerate,
		     NULL);

	return(TRUE);
      }
    }else{
      g_message("ags_audio_file_open: unknown file type\n");
    }
  }

  return(FALSE);
}

/**
 * ags_audio_file_rw_open:
 * @audio_file: the #AgsAudioFile
 * @create: create the file
 *
 * Open the #AgsAudioFile in read/write mode.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_audio_file_rw_open(AgsAudioFile *audio_file,
		       gboolean create)
{
  GObject *sound_resource;

  gchar *filename;
  guint file_audio_channels;
  guint file_samplerate;
  
  pthread_mutex_t *audio_file_mutex;
  
  if(!AGS_IS_AUDIO_FILE(audio_file)){
    return(FALSE);
  }

#ifdef AGS_DEBUG
  g_message("ags_audio_file_rw_open: %s", audio_file->filename);
#endif

  /* get audio_file mutex */
  pthread_mutex_lock(ags_audio_file_get_class_mutex());
  
  audio_file_mutex = audio_file->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_file_get_class_mutex());

  sound_resource = NULL;
  
  g_object_get(audio_file,
	       "filename", &filename,
	       "file-audio-channels", &file_audio_channels,
	       "file-samplerate", &file_samplerate,
	       NULL);
  
  if(!create &&
     !g_file_test(filename, G_FILE_TEST_EXISTS)){
    return(FALSE);
  }

  if(ags_audio_file_check_suffix(audio_file->filename)){
    GError *error;
    guint loop_start, loop_end;

    /* sound resource */
    pthread_mutex_lock(audio_file_mutex);

    sound_resource = 
      audio_file->sound_resource = (GObject *) ags_sndfile_new();
    g_object_ref(audio_file->sound_resource);

    pthread_mutex_unlock(audio_file_mutex);

    if(ags_sound_resource_rw_open(AGS_SOUND_RESOURCE(sound_resource),
				  filename,
				  file_audio_channels, file_samplerate,
				  create)){
      return(TRUE);
    }else{
      return(FALSE);
    }
  }else{
    g_message("ags_audio_file_open: unknown file type\n");

    return(FALSE);
  }
}

/**
 * ags_audio_file_open_from_data:
 * @audio_file: the #AgsAudioFile
 * @data: the audio data
 *
 * Open #AgsAudioFile using virtual functions.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_audio_file_open_from_data(AgsAudioFile *audio_file, gchar *data)
{
  GObject *sound_resource;

  gchar *filename;
  guint file_audio_channels;
  guint file_samplerate;
  guint file_frame_count;

  pthread_mutex_t *audio_file_mutex;

  if(!AGS_IS_AUDIO_FILE(audio_file)){
    return(FALSE);
  }

#ifdef AGS_DEBUG
  g_message("ags_audio_file_open_from_data:");
#endif

  /* get audio_file mutex */
  pthread_mutex_lock(ags_audio_file_get_class_mutex());
  
  audio_file_mutex = audio_file->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_file_get_class_mutex());

  sound_resource = NULL;

  g_object_get(audio_file,
	       "filename", &filename,
	       NULL);

  if(data != NULL){
    if(ags_audio_file_check_suffix(audio_file->filename)){
      guint loop_start, loop_end;

      GError *error;

      pthread_mutex_lock(audio_file_mutex);
      
      sound_resource = 
	audio_file->sound_resource = (GObject *) ags_sndfile_new();
      g_object_ref(audio_file->sound_resource);

      pthread_mutex_unlock(audio_file_mutex);

      //TODO:JK: thread-safe way
      AGS_SNDFILE(audio_file->sound_resource)->flags = AGS_SNDFILE_VIRTUAL;

      if(ags_sound_resource_open(AGS_SOUND_RESOURCE(sound_resource),
				 filename)){
	AGS_SNDFILE(sound_resource)->pointer = g_base64_decode(data,
							       &(AGS_SNDFILE(sound_resource)->length));
	AGS_SNDFILE(sound_resource)->current = AGS_SNDFILE(audio_file->sound_resource)->pointer;

	ags_sound_resource_info(AGS_SOUND_RESOURCE(sound_resource),
				&file_frame_count,
				NULL, NULL);

	g_object_set(audio_file,
		     "file-frame-count", file_frame_count,
		     NULL);

	ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(sound_resource),
				       &file_audio_channels,
				       &file_samplerate,
				       NULL,
				       NULL);

	g_object_set(audio_file,
		     "file-audio-channels", file_audio_channels,
		     "file-samplerate", file_samplerate,
		     NULL);

	return(TRUE);
      }
    }else{
      g_message("ags_audio_file_open: unknown file type\n");
    }
  }

  return(FALSE);
}

/**
 * ags_audio_file_close:
 * @audio_file: the #AgsAudioFile
 *
 * Close the #AgsAudioFile.
 *
 * Since: 2.0.0
 */
void
ags_audio_file_close(AgsAudioFile *audio_file)
{
  GObject *sound_resource;

  pthread_mutex_t *audio_file_mutex;

  if(!AGS_IS_AUDIO_FILE(audio_file)){
    return;
  }

  /* get audio_file mutex */
  pthread_mutex_lock(ags_audio_file_get_class_mutex());
  
  audio_file_mutex = audio_file->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_file_get_class_mutex());

  /* get sound resource */
  pthread_mutex_lock(audio_file_mutex);
      
  sound_resource = audio_file->sound_resource;

  pthread_mutex_unlock(audio_file_mutex);
  
  /* close */
  ags_sound_resource_close(AGS_SOUND_RESOURCE(sound_resource));
}

/**
 * ags_audio_file_read:
 * @audio_file: the #AgsAudioFile
 * @audio_channel: nth channel
 * @format: the format
 * @error: returned error
 *
 * Read audio buffer.
 *
 * Since: 2.0.0
 */
void*
ags_audio_file_read(AgsAudioFile *audio_file,
		    guint audio_channel,
		    guint format,
		    GError **error)
{
  GObject *sound_resource;

  void *buffer;

  guint file_frame_count;
  
  pthread_mutex_t *audio_file_mutex;

  if(!AGS_IS_AUDIO_FILE(audio_file)){
    return(NULL);
  }

  /* get audio_file mutex */
  pthread_mutex_lock(ags_audio_file_get_class_mutex());
  
  audio_file_mutex = audio_file->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_file_get_class_mutex());

  /* get sound resource */
  pthread_mutex_lock(audio_file_mutex);
      
  sound_resource = audio_file->sound_resource;

  file_frame_count = audio_file->file_frame_count;
  
  pthread_mutex_unlock(audio_file_mutex);
  
  /* read */
  buffer = ags_stream_alloc(file_frame_count,
			    format);
  
  ags_sound_resource_read(AGS_SOUND_RESOURCE(sound_resource),
			  buffer, 1,
			  audio_channel,
			  file_frame_count, format);

  return(buffer);
}

/**
 * ags_audio_file_read_audio_signal:
 * @audio_file: the #AgsAudioFile
 *
 * Convert the #AgsAudioFile to a #GList-struct of #AgsAudioSignal.
 *
 * Since: 2.0.0
 */
void
ags_audio_file_read_audio_signal(AgsAudioFile *audio_file)
{
  GObject *sound_resource;
  GObject *soundcard;
  
  GList *list;

  guint audio_channel;
  
  pthread_mutex_t *audio_file_mutex;

  if(!AGS_IS_AUDIO_FILE(audio_file)){
    return;
  }

  /* get audio_file mutex */
  pthread_mutex_lock(ags_audio_file_get_class_mutex());
  
  audio_file_mutex = audio_file->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_file_get_class_mutex());

  /* get sound resource */
  pthread_mutex_lock(audio_file_mutex);
      
  sound_resource = audio_file->sound_resource;
  soundcard = audio_file->soundcard;

  audio_channel = audio_file->audio_channel;
  
  pthread_mutex_unlock(audio_file_mutex);

  /* read audio signal */
  list = ags_sound_resource_read_audio_signal(AGS_SOUND_RESOURCE(sound_resource),
					      soundcard,
					      audio_channel);
 
  /* set audio signal */
  pthread_mutex_lock(audio_file_mutex);
      
  audio_file->audio_signal = list;

  pthread_mutex_unlock(audio_file_mutex);    
}

/**
 * ags_audio_file_read_wave:
 * @audio_file: the #AgsAudioFile
 *
 * Convert the #AgsAudioFile to a #GList of buffers.
 *
 * Since: 2.0.0
 */
void
ags_audio_file_read_wave(AgsAudioFile *audio_file,
			 guint64 x_offset,
			 gdouble delay, guint attack)
{
  GObject *sound_resource;
  GObject *soundcard;

  GList *list;

  guint audio_channel;
  
  pthread_mutex_t *audio_file_mutex;

  if(!AGS_IS_AUDIO_FILE(audio_file)){
    return;
  }

  /* get audio_file mutex */
  pthread_mutex_lock(ags_audio_file_get_class_mutex());
  
  audio_file_mutex = audio_file->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_file_get_class_mutex());

  /* get sound resource */
  pthread_mutex_lock(audio_file_mutex);
      
  sound_resource = audio_file->sound_resource;
  soundcard = audio_file->soundcard;

  audio_channel = audio_file->audio_channel;
  
  pthread_mutex_unlock(audio_file_mutex);

  /* read wave */
  list = ags_sound_resource_read_wave(AGS_SOUND_RESOURCE(sound_resource),
				      soundcard,
				      audio_channel,
				      x_offset,
				      delay, attack);
  
  /* set wave */
  pthread_mutex_lock(audio_file_mutex);
      
  audio_file->wave = list;

  pthread_mutex_unlock(audio_file_mutex);
}

/**
 * ags_audio_file_seek:
 * @audio_file: the #AgsAudioFile
 * @frames: number of frames to seek
 * @whence: SEEK_SET, SEEK_CUR, or SEEK_END
 *
 * Position the #AgsAudioFile's internal data address.
 *
 * Since: 2.0.0
 */
void
ags_audio_file_seek(AgsAudioFile *audio_file, guint frames, gint whence)
{
  GObject *sound_resource;

  pthread_mutex_t *audio_file_mutex;

  if(!AGS_IS_AUDIO_FILE(audio_file)){
    return;
  }

  /* get audio_file mutex */
  pthread_mutex_lock(ags_audio_file_get_class_mutex());
  
  audio_file_mutex = audio_file->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_file_get_class_mutex());

  /* get sound resource */
  pthread_mutex_lock(audio_file_mutex);
      
  sound_resource = audio_file->sound_resource;

  pthread_mutex_unlock(audio_file_mutex);
  
  /* seek */
  ags_sound_resource_seek(AGS_SOUND_RESOURCE(sound_resource),
			  frames, whence);
}

/**
 * ags_audio_file_write:
 * @audio_file: the #AgsAudioFile
 * @buffer: the audio data
 * @buffer_size: the count of frames to write
 * @format: the format
 *
 * Write the buffer to #AgsAudioFile.
 *
 * Since: 2.0.0
 */
void
ags_audio_file_write(AgsAudioFile *audio_file,
		     void *buffer,
		     guint buffer_size, guint format)
{
  GObject *sound_resource;

  guint file_audio_channels;
  gint audio_channel;
  guint i;

  pthread_mutex_t *audio_file_mutex;
  
  if(!AGS_IS_AUDIO_FILE(audio_file) ||
     buffer == NULL){
    return;
  }

  /* get audio_file mutex */
  pthread_mutex_lock(ags_audio_file_get_class_mutex());
  
  audio_file_mutex = audio_file->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_file_get_class_mutex());

  /* get sound resource */
  pthread_mutex_lock(audio_file_mutex);
      
  sound_resource = audio_file->sound_resource;

  file_audio_channels = audio_file->file_audio_channels;
  audio_channel = audio_file->audio_channel;
  
  pthread_mutex_unlock(audio_file_mutex);

  if(audio_channel == -1){
    for(i = 0; i < file_audio_channels; i++){
      ags_sound_resource_write(AGS_SOUND_RESOURCE(sound_resource),
			       buffer, file_audio_channels,
			       i,
			       buffer_size, format);
    }
  }else{
    ags_sound_resource_write(AGS_SOUND_RESOURCE(sound_resource),
			     buffer, 1,
			     audio_channel,
			     buffer_size, format);
  }
}

/**
 * ags_audio_file_flush:
 * @audio_file: the #AgsAudioFile
 *
 * Flushes the #AgsAudioFile's internal buffer.
 *
 * Since: 2.0.0
 */
void
ags_audio_file_flush(AgsAudioFile *audio_file)
{
  GObject *sound_resource;

  pthread_mutex_t *audio_file_mutex;

  if(!AGS_IS_AUDIO_FILE(audio_file)){
    return;
  }

  /* get audio_file mutex */
  pthread_mutex_lock(ags_audio_file_get_class_mutex());
  
  audio_file_mutex = audio_file->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_file_get_class_mutex());
  
  /* get sound resource */
  pthread_mutex_lock(audio_file_mutex);
      
  sound_resource = audio_file->sound_resource;

  pthread_mutex_unlock(audio_file_mutex);

  /* flush */
  ags_sound_resource_flush(AGS_SOUND_RESOURCE(sound_resource));
}

/**
 * ags_audio_file_new:
 * @filename: the filename
 * @soundcard: defaults of #AgsSoundcard
 * @audio_channel: the audio channel to read
 *
 * Create a new instance of #AgsAudioFile.
 *
 * Returns: the new #AgsAudioFile.
 *
 * Since: 2.0.0
 */
AgsAudioFile*
ags_audio_file_new(gchar *filename,
		   GObject *soundcard,
		   gint audio_channel)
{
  AgsAudioFile *audio_file;

  audio_file = (AgsAudioFile *) g_object_new(AGS_TYPE_AUDIO_FILE,
					     "filename", filename,
					     "soundcard", soundcard,
					     "audio-channel", audio_channel,
					     NULL);

  return(audio_file);
}
