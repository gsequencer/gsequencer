/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/thread/ags_export_thread.h>

#include <ags/audio/alsa/ags_alsa_devout.h>

#include <ags/audio/oss/ags_oss_devout.h>

#include <ags/audio/wasapi/ags_wasapi_devout.h>

#include <ags/audio/jack/ags_jack_devout.h>

#include <ags/audio/pulse/ags_pulse_devout.h>

#include <ags/audio/core-audio/ags_core_audio_devout.h>

#include <math.h>

#include <ags/i18n.h>

void ags_export_thread_class_init(AgsExportThreadClass *export_thread);
void ags_export_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_export_thread_init(AgsExportThread *export_thread);
void ags_export_thread_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_export_thread_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_export_thread_connect(AgsConnectable *connectable);
void ags_export_thread_disconnect(AgsConnectable *connectable);
void ags_export_thread_dispose(GObject *gobject);
void ags_export_thread_finalize(GObject *gobject);

void ags_export_thread_start(AgsThread *thread);
void ags_export_thread_run(AgsThread *thread);
void ags_export_thread_stop(AgsThread *thread);

/**
 * SECTION:ags_export_thread
 * @short_description: export thread
 * @title: AgsExportThread
 * @section_id:
 * @include: ags/audio/thread/ags_export_thread.h
 *
 * The #AgsExportThread acts as audio output thread to file.
 */

enum{
  PROP_0,
  PROP_SOUNDCARD,
  PROP_AUDIO_FILE,
  PROP_TIC,
};

static gpointer ags_export_thread_parent_class = NULL;
static AgsConnectableInterface *ags_export_thread_parent_connectable_interface;

GType
ags_export_thread_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_export_thread = 0;

    static const GTypeInfo ags_export_thread_info = {
      sizeof (AgsExportThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_export_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsExportThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_export_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_export_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_export_thread = g_type_register_static(AGS_TYPE_THREAD,
						    "AgsExportThread",
						    &ags_export_thread_info,
						    0);
    
    g_type_add_interface_static(ags_type_export_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_export_thread);
  }

  return g_define_type_id__volatile;
}

void
ags_export_thread_class_init(AgsExportThreadClass *export_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;
  GParamSpec *param_spec;

  ags_export_thread_parent_class = g_type_class_peek_parent(export_thread);

  /* GObject */
  gobject = (GObjectClass *) export_thread;

  gobject->get_property = ags_export_thread_get_property;
  gobject->set_property = ags_export_thread_set_property;

  gobject->dispose = ags_export_thread_dispose;
  gobject->finalize = ags_export_thread_finalize;

  /* properties */
  /**
   * AgsExportThread:soundcard:
   *
   * The assigned #AgsSoundcard.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("soundcard assigned to"),
				   i18n_pspec("The AgsSoundcard it is assigned to"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /**
   * AgsExportThread:audio-file:
   *
   * The assigned #AgsAudioFile.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("audio-file",
				   i18n_pspec("audio file to write"),
				   i18n_pspec("The audio file to write output"),
				   AGS_TYPE_AUDIO_FILE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_FILE,
				  param_spec);

  /**
   * AgsExportThread:tic:
   *
   * The tic.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("tic",
				  i18n_pspec("tic"),
				  i18n_pspec("The tic"),
				  0,
				  G_MAXUINT,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TIC,
				  param_spec);

  /* AgsThread */
  thread = (AgsThreadClass *) export_thread;

  thread->start = ags_export_thread_start;
  thread->run = ags_export_thread_run;
  thread->stop = ags_export_thread_stop;
}

void
ags_export_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_export_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_export_thread_connect;
  connectable->disconnect = ags_export_thread_disconnect;
}

void
ags_export_thread_init(AgsExportThread *export_thread)
{
  AgsThread *thread;

  AgsConfig *config;

  gdouble frequency;
  guint samplerate;
  guint buffer_size;
  
  thread = (AgsThread *) export_thread;

  ags_thread_set_flags(thread, (AGS_THREAD_START_SYNCED_FREQ |
				AGS_THREAD_INTERMEDIATE_POST_SYNC));
  
  config = ags_config_get_instance();
  
  samplerate = ags_soundcard_helper_config_get_samplerate(config);
  buffer_size = ags_soundcard_helper_config_get_buffer_size(config);

  frequency = ((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;

  g_object_set(thread,
	       "frequency", frequency,
	       NULL);

  export_thread->flags = 0;

  export_thread->tic = 0;
  export_thread->counter = 0;

  export_thread->soundcard = NULL;

  export_thread->audio_file = NULL;
}

void
ags_export_thread_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsExportThread *export_thread;

  GRecMutex *thread_mutex;

  export_thread = AGS_EXPORT_THREAD(gobject);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(export_thread);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = (GObject *) g_value_get_object(value);

      g_rec_mutex_lock(thread_mutex);

      if(export_thread->soundcard == soundcard){
	g_rec_mutex_unlock(thread_mutex);

	return;
      }
      
      if(export_thread->soundcard != NULL){
	g_object_unref(G_OBJECT(export_thread->soundcard));
      }

      if(soundcard != NULL){
	g_object_ref(G_OBJECT(soundcard));
      }
      
      export_thread->soundcard = soundcard;

      g_rec_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_AUDIO_FILE:
    {
      AgsAudioFile *audio_file;

      audio_file = g_value_get_object(value);

      g_rec_mutex_lock(thread_mutex);

      if(export_thread->audio_file == audio_file){
	g_rec_mutex_unlock(thread_mutex);

	return;
      }

      if(export_thread->audio_file != NULL){
	g_object_unref(export_thread->audio_file);
      }

      if(audio_file != NULL){
	g_object_ref(audio_file);
      }

      export_thread->audio_file = audio_file;

      g_rec_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_TIC:
    {
      g_rec_mutex_lock(thread_mutex);

      export_thread->tic = g_value_get_uint(value);

      g_rec_mutex_unlock(thread_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_export_thread_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsExportThread *export_thread;

  GRecMutex *thread_mutex;

  export_thread = AGS_EXPORT_THREAD(gobject);

  /* get thread mutex */
  thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(export_thread);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_rec_mutex_lock(thread_mutex);

      g_value_set_object(value, G_OBJECT(export_thread->soundcard));

      g_rec_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_AUDIO_FILE:
    {
      g_rec_mutex_lock(thread_mutex);

      g_value_set_object(value, export_thread->audio_file);

      g_rec_mutex_unlock(thread_mutex);
    }
    break;
  case PROP_TIC:
    {
      g_rec_mutex_lock(thread_mutex);

      g_value_set_uint(value, export_thread->tic);

      g_rec_mutex_unlock(thread_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_export_thread_connect(AgsConnectable *connectable)
{
  ags_export_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_export_thread_disconnect(AgsConnectable *connectable)
{
  ags_export_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_export_thread_dispose(GObject *gobject)
{
  AgsExportThread *export_thread;

  export_thread = AGS_EXPORT_THREAD(gobject);

  /* soundcard */
  if(export_thread->soundcard != NULL){
    g_object_unref(export_thread->soundcard);

    export_thread->soundcard = NULL;
  }
  
  /* audio file */
  if(export_thread->audio_file != NULL){
    g_object_unref(export_thread->audio_file);

    export_thread->audio_file = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_export_thread_parent_class)->dispose(gobject);
}

void
ags_export_thread_finalize(GObject *gobject)
{
  AgsExportThread *export_thread;

  export_thread = AGS_EXPORT_THREAD(gobject);

  /* soundcard */
  if(export_thread->soundcard != NULL){
    g_object_unref(export_thread->soundcard);
  }
  
  /* audio file */  
  if(export_thread->audio_file != NULL){
    g_object_unref(export_thread->audio_file);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_export_thread_parent_class)->finalize(gobject);
}

void
ags_export_thread_start(AgsThread *thread)
{
  AgsExportThread *export_thread;
  
  export_thread = (AgsExportThread *) thread;
  
  export_thread->counter = 0;

  AGS_THREAD_CLASS(ags_export_thread_parent_class)->start(thread);
}

void
ags_export_thread_run(AgsThread *thread)
{
  AgsExportThread *export_thread;

  AgsSoundcard *soundcard;

  void *soundcard_buffer;

  guint pcm_channels;
  guint buffer_size;
  guint format;
  
  export_thread = AGS_EXPORT_THREAD(thread);

  if(export_thread->audio_file == NULL){
    return;
  }
  
  if(export_thread->counter == export_thread->tic){
    ags_thread_stop(thread);
  }else{
    export_thread->counter += 1;
  }

  /*  */
  soundcard = AGS_SOUNDCARD(export_thread->soundcard);

  soundcard_buffer = NULL;
  
  if(AGS_IS_ALSA_DEVOUT(soundcard) ||
     AGS_IS_OSS_DEVOUT(soundcard) ||
     AGS_IS_WASAPI_DEVOUT(soundcard)){
    soundcard_buffer = ags_soundcard_get_buffer(soundcard);
  }else if(AGS_IS_JACK_DEVOUT(soundcard) ||
	   AGS_IS_PULSE_DEVOUT(soundcard)){
    soundcard_buffer = ags_soundcard_get_prev_buffer(soundcard);
  }else if(AGS_IS_CORE_AUDIO_DEVOUT(soundcard)){
    soundcard_buffer = ags_soundcard_get_buffer(soundcard);
  }
  
  ags_soundcard_get_presets(soundcard,
			    &pcm_channels,
			    NULL,
			    &buffer_size,
			    &format);

  ags_soundcard_lock_buffer(soundcard,
			    soundcard_buffer);
  
  ags_audio_file_write(export_thread->audio_file,
		       soundcard_buffer,
		       (guint) buffer_size,
		       format);

  ags_soundcard_unlock_buffer(soundcard,
			    soundcard_buffer);
}

void
ags_export_thread_stop(AgsThread *thread)
{
  AgsExportThread *export_thread;

  export_thread = AGS_EXPORT_THREAD(thread);

  AGS_THREAD_CLASS(ags_export_thread_parent_class)->stop(thread);

  ags_audio_file_flush(export_thread->audio_file);
  ags_audio_file_close(export_thread->audio_file);

  export_thread->audio_file = NULL;
}

/**
 * ags_export_thread_find_soundcard:
 * @export_thread: the #AgsExportThread
 * @soundcard: the #AgsSoundcard to find
 * 
 * Returns: (transfer full): the matching #AgsExportThread, if not
 * found %NULL.
 * 
 * Since: 3.0.0
 */
AgsExportThread*
ags_export_thread_find_soundcard(AgsExportThread *export_thread,
				 GObject *soundcard)
{
  AgsThread *thread, *next_thread;
  
  if(!AGS_IS_EXPORT_THREAD(export_thread)){
    return(NULL);
  }

  thread = export_thread;
  g_object_ref(thread);
  
  while(thread != NULL){
    if(AGS_IS_EXPORT_THREAD(thread) &&
       AGS_EXPORT_THREAD(thread)->soundcard == soundcard){
      return(thread);
    }

    /* iterate */
    next_thread = ags_thread_next(thread);

    g_object_unref(thread);
    
    thread = next_thread;
  }

  return(NULL);
}

/**
 * ags_export_thread_new:
 * @soundcard: the #AgsSoundcard
 * @audio_file: the output file
 *
 * Create a new instance of #AgsExportThread.
 *
 * Returns: the new #AgsExportThread
 *
 * Since: 3.0.0
 */
AgsExportThread*
ags_export_thread_new(GObject *soundcard, AgsAudioFile *audio_file)
{
  AgsExportThread *export_thread;

  export_thread = (AgsExportThread *) g_object_new(AGS_TYPE_EXPORT_THREAD,
						   "soundcard", soundcard,
						   "audio-file", audio_file,
						   NULL);
  
  return(export_thread);
}
