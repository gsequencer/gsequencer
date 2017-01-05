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

#include <ags/audio/thread/ags_export_thread.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/audio/ags_devout.h>

#include <math.h>

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
};

static gpointer ags_export_thread_parent_class = NULL;
static AgsConnectableInterface *ags_export_thread_parent_connectable_interface;

GType
ags_export_thread_get_type()
{
  static GType ags_type_export_thread = 0;

  if(!ags_type_export_thread){
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
						    "AgsExportThread\0",
						    &ags_export_thread_info,
						    0);
    
    g_type_add_interface_static(ags_type_export_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_export_thread);
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

  gobject->finalize = ags_export_thread_finalize;

  /* properties */
  /**
   * AgsExportThread:soundcard:
   *
   * The assigned #AgsSoundcard.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("soundcard\0",
				   "soundcard assigned to\0",
				   "The AgsSoundcard it is assigned to.\0",
				   G_TYPE_OBJECT,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /**
   * AgsExportThread:audio-file:
   *
   * The assigned #AgsAudioFile.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("audio-file\0",
				   "audio file to write\0",
				   "The audio file to write output.\0",
				   AGS_TYPE_AUDIO_FILE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_FILE,
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
  
  gchar *str0, *str1;
  
  thread = (AgsThread *) export_thread;

  g_atomic_int_or(&(thread->flags),
		  (AGS_THREAD_START_SYNCED_FREQ));
  
  config = ags_config_get_instance();
  
  str0 = ags_config_get_value(config,
			      AGS_CONFIG_SOUNDCARD,
			      "samplerate\0");

  if(str0 == NULL){
    str0 = ags_config_get_value(config,
				AGS_CONFIG_SOUNDCARD_0,
				"samplerate\0");
  }
  
  str1 = ags_config_get_value(config,
			      AGS_CONFIG_SOUNDCARD,
			      "buffer-size\0");

  if(str1 == NULL){
    str1 = ags_config_get_value(config,
				AGS_CONFIG_SOUNDCARD_0,
				"buffer-size\0");
  }

  if(str0 == NULL || str1 == NULL){
    thread->freq = AGS_EXPORT_THREAD_DEFAULT_JIFFIE;
  }else{
    guint samplerate;
    guint buffer_size;

    samplerate = g_ascii_strtoull(str0,
				  NULL,
				  10);
    buffer_size = g_ascii_strtoull(str1,
				   NULL,
				   10);

    thread->freq = ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;
  }

  g_free(str0);
  g_free(str1);

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

  export_thread = AGS_EXPORT_THREAD(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = (GObject *) g_value_get_object(value);

      if(export_thread->soundcard != NULL){
	g_object_unref(G_OBJECT(export_thread->soundcard));
      }

      if(soundcard != NULL){
	g_object_ref(G_OBJECT(soundcard));

	if(AGS_IS_DEVOUT(soundcard)){
	  g_atomic_int_or(&(AGS_THREAD(export_thread)->flags),
			  (AGS_THREAD_INTERMEDIATE_POST_SYNC));
	}
      }

      export_thread->soundcard = G_OBJECT(soundcard);
    }
    break;
  case PROP_AUDIO_FILE:
    {
      AgsAudioFile *audio_file;

      audio_file = g_value_get_object(value);

      if(export_thread->audio_file == audio_file){
	return;
      }

      if(export_thread->audio_file != NULL){
	g_object_unref(export_thread->audio_file);
      }

      if(audio_file != NULL){
	g_object_ref(audio_file);
      }

      export_thread->audio_file = audio_file;
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

  export_thread = AGS_EXPORT_THREAD(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, G_OBJECT(export_thread->soundcard));
    }
    break;
  case PROP_AUDIO_FILE:
    {
      g_value_set_object(value, export_thread->audio_file);
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
ags_export_thread_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_export_thread_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_export_thread_start(AgsThread *thread)
{
  AgsExportThread *export_thread;
  
  //TODO:JK: implement me
  g_message("export start");

  export_thread = (AgsExportThread *) thread;
  
  export_thread->counter = 0;

  AGS_THREAD_CLASS(ags_export_thread_parent_class)->start(thread);
}

void
ags_export_thread_run(AgsThread *thread)
{
  AgsExportThread *export_thread;

  AgsMutexManager *mutex_manager;
  AgsSoundcard *soundcard;

  void *soundcard_buffer;

  guint pcm_channels;
  guint buffer_size;
  guint format;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  
  export_thread = AGS_EXPORT_THREAD(thread);

  if(export_thread->counter == export_thread->tic){
    ags_thread_stop(thread);
  }else{
    export_thread->counter += 1;
  }

  /*  */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) export_thread->soundcard);

  pthread_mutex_unlock(application_mutex);

  /*  */
  pthread_mutex_lock(mutex);

  soundcard = AGS_SOUNDCARD(export_thread->soundcard);

  soundcard_buffer = ags_soundcard_get_buffer(soundcard);
  ags_soundcard_get_presets(soundcard,
			    &pcm_channels,
			    NULL,
			    &buffer_size,
			    &format);

  ags_audio_file_write(export_thread->audio_file,
		       soundcard_buffer,
		       (guint) buffer_size,
		       format);
  
  pthread_mutex_unlock(mutex);
}

void
ags_export_thread_stop(AgsThread *thread)
{
  AgsExportThread *export_thread;

  export_thread = AGS_EXPORT_THREAD(thread);

  AGS_THREAD_CLASS(ags_export_thread_parent_class)->stop(thread);

  ags_audio_file_flush(export_thread->audio_file);
  ags_audio_file_close(export_thread->audio_file);
}

/**
 * ags_export_thread_find_soundcard:
 * @export_thread: the #AgsExportThread
 * @soundcard: the #AgsSoundcard to find
 * 
 * Returns: the matching #AgsExportThread, if not
 * found %NULL.
 * 
 * Since: 0.7.119
 */
AgsExportThread*
ags_export_thread_find_soundcard(AgsExportThread *export_thread,
				 GObject *soundcard)
{
  if(export_thread == NULL ||
     !AGS_IS_EXPORT_THREAD(export_thread)){
    return(NULL);
  }
  
  while(export_thread != NULL){
    if(AGS_IS_EXPORT_THREAD(export_thread) &&
       export_thread->soundcard == soundcard){
      return(export_thread);
    }
    
    export_thread = g_atomic_pointer_get(&(((AgsThread *) export_thread)->next));
  }
  
  return(NULL);
}

/**
 * ags_export_thread_new:
 * @soundcard: the #AgsSoundcard
 * @audio_file: the output file
 *
 * Create a new #AgsExportThread.
 *
 * Returns: the new #AgsExportThread
 *
 * Since: 0.4
 */
AgsExportThread*
ags_export_thread_new(GObject *soundcard, AgsAudioFile *audio_file)
{
  AgsExportThread *export_thread;

  export_thread = (AgsExportThread *) g_object_new(AGS_TYPE_EXPORT_THREAD,
						   "soundcard\0", soundcard,
						   "audio-file\0", audio_file,
						   NULL);
  
  return(export_thread);
}
