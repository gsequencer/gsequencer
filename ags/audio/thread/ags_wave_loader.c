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

#include <ags/audio/thread/ags_wave_loader.h>

#include <ags/libags.h>

#include <ags/audio/ags_input.h>
#include <ags/audio/ags_wave.h>

#include <ags/audio/file/ags_sound_resource.h>

#include <ags/i18n.h>

void ags_wave_loader_class_init(AgsWaveLoaderClass *wave_loader);
void ags_wave_loader_init(AgsWaveLoader *wave_loader);
void ags_wave_loader_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_wave_loader_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_wave_loader_dispose(GObject *gobject);
void ags_wave_loader_finalize(GObject *gobject);

void* ags_wave_loader_run(void *ptr);

/**
 * SECTION:ags_wave_loader
 * @short_description: load wave asynchronously
 * @title: AgsWaveLoader
 * @section_id:
 * @include: ags/plugin/ags_wave_loader.h
 *
 * The #AgsWaveLoader loads wave audio data asynchronously.
 */

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_FILENAME,
  PROP_AUDIO_FILE,
};

static gpointer ags_wave_loader_parent_class = NULL;

static pthread_mutex_t ags_wave_loader_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_wave_loader_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_wave_loader = 0;

    static const GTypeInfo ags_wave_loader_info = {
      sizeof(AgsWaveLoaderClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_wave_loader_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsWaveLoader),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_wave_loader_init,
    };

    ags_type_wave_loader = g_type_register_static(G_TYPE_OBJECT,
						  "AgsWaveLoader",
						  &ags_wave_loader_info,
						  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_wave_loader);
  }

  return g_define_type_id__volatile;
}

void
ags_wave_loader_class_init(AgsWaveLoaderClass *wave_loader)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_wave_loader_parent_class = g_type_class_peek_parent(wave_loader);

  /* GObject */
  gobject = (GObjectClass *) wave_loader;

  gobject->set_property = ags_wave_loader_set_property;
  gobject->get_property = ags_wave_loader_get_property;

  gobject->dispose = ags_wave_loader_dispose;
  gobject->finalize = ags_wave_loader_finalize;

  /* properties */
  /**
   * AgsWaveLoader:audio:
   *
   * The assigned audio.
   * 
   * Since: 2.0.13
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("audio"),
				   i18n_pspec("The audio"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsWaveLoader:filename:
   *
   * The filename to open.
   * 
   * Since: 2.0.13
   */
  param_spec = g_param_spec_string("filename",
				   i18n_pspec("filename"),
				   i18n_pspec("The filename"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsWaveLoader:audio-file:
   *
   * The audio file opened.
   * 
   * Since: 2.0.13
   */
  param_spec = g_param_spec_object("audio-file",
				   i18n_pspec("audio file"),
				   i18n_pspec("The audio file"),
				   AGS_TYPE_AUDIO_FILE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_FILE,
				  param_spec);
}

void
ags_wave_loader_init(AgsWaveLoader *wave_loader)
{
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  wave_loader->flags = 0;

  /* add base plugin mutex */
  wave_loader->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  wave_loader->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  /* fields */
  wave_loader->thread = (pthread_t *) malloc(sizeof(pthread_t));
  
  wave_loader->audio = NULL;

  wave_loader->filename = NULL;

  wave_loader->audio_file = NULL;
}

void
ags_wave_loader_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsWaveLoader *wave_loader;

  pthread_mutex_t *wave_loader_mutex;

  wave_loader = AGS_WAVE_LOADER(gobject);

  /* get base plugin mutex */
  pthread_mutex_lock(ags_wave_loader_get_class_mutex());
  
  wave_loader_mutex = wave_loader->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_loader_get_class_mutex());

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = g_value_get_object(value);
      
      pthread_mutex_lock(wave_loader_mutex);

      if(wave_loader->audio == audio){
	pthread_mutex_unlock(wave_loader_mutex);

	return;
      }

      if(wave_loader->audio != NULL){
	g_object_unref(wave_loader->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      wave_loader->audio = audio;
      
      pthread_mutex_unlock(wave_loader_mutex);
    }
    break;
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);
      
      pthread_mutex_lock(wave_loader_mutex);

      if(wave_loader->filename == filename){
	pthread_mutex_unlock(wave_loader_mutex);

	return;
      }

      if(wave_loader->filename != NULL){
	g_free(wave_loader->filename);
      }

      wave_loader->filename = g_strdup(filename);
      
      pthread_mutex_unlock(wave_loader_mutex);
    }
    break;
  case PROP_AUDIO_FILE:
    {
      AgsAudioFile *audio_file;

      audio_file = g_value_get_object(value);
      
      pthread_mutex_lock(wave_loader_mutex);

      if(wave_loader->audio_file == audio_file){
	pthread_mutex_unlock(wave_loader_mutex);

	return;
      }

      if(wave_loader->audio_file != NULL){
	g_object_unref(wave_loader->audio_file);
      }

      if(audio_file != NULL){
	g_object_ref(audio_file);
      }

      wave_loader->audio_file = audio_file;
      
      pthread_mutex_unlock(wave_loader_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}
    
void
ags_wave_loader_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsWaveLoader *wave_loader;

  pthread_mutex_t *wave_loader_mutex;

  wave_loader = AGS_WAVE_LOADER(gobject);

  /* get base plugin mutex */
  pthread_mutex_lock(ags_wave_loader_get_class_mutex());
  
  wave_loader_mutex = wave_loader->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_loader_get_class_mutex());

  switch(prop_id){
  case PROP_AUDIO:
    {
      pthread_mutex_lock(wave_loader_mutex);
      
      g_value_set_object(value, wave_loader->audio);

      pthread_mutex_unlock(wave_loader_mutex);
    }
    break;
  case PROP_FILENAME:
    {
      pthread_mutex_lock(wave_loader_mutex);
      
      g_value_set_string(value, wave_loader->filename);

      pthread_mutex_unlock(wave_loader_mutex);
    }
    break;
  case PROP_AUDIO_FILE:
    {
      pthread_mutex_lock(wave_loader_mutex);
      
      g_value_set_object(value, wave_loader->audio_file);

      pthread_mutex_unlock(wave_loader_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wave_loader_dispose(GObject *gobject)
{
  AgsWaveLoader *wave_loader;

  wave_loader = AGS_WAVE_LOADER(gobject);

  if(wave_loader->audio != NULL){
    g_object_unref(wave_loader->audio);

    wave_loader->audio = NULL;
  }

  if(wave_loader->audio_file != NULL){
    g_object_unref(wave_loader->audio_file);

    wave_loader->audio_file = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_wave_loader_parent_class)->dispose(gobject);
}

void
ags_wave_loader_finalize(GObject *gobject)
{
  AgsWaveLoader *wave_loader;

  wave_loader = AGS_WAVE_LOADER(gobject);
  
  /* destroy object mutex */
  pthread_mutex_destroy(wave_loader->obj_mutex);
  free(wave_loader->obj_mutex);

  pthread_mutexattr_destroy(wave_loader->obj_mutexattr);
  free(wave_loader->obj_mutexattr);

  if(wave_loader->audio != NULL){
    g_object_unref(wave_loader->audio);
  }

  g_free(wave_loader->filename);

  if(wave_loader->audio_file != NULL){
    g_object_unref(wave_loader->audio_file);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_wave_loader_parent_class)->finalize(gobject);
}

/**
 * ags_wave_loader_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.13
 */
pthread_mutex_t*
ags_wave_loader_get_class_mutex()
{
  return(&ags_wave_loader_class_mutex);
}

/**
 * ags_wave_loader_test_flags:
 * @wave_loader: the #AgsWaveLoader
 * @flags: the flags
 * 
 * Test @flags to be set on @recall.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 2.0.13
 */
gboolean
ags_wave_loader_test_flags(AgsWaveLoader *wave_loader, guint flags)
{
  gboolean retval;
  
  pthread_mutex_t *wave_loader_mutex;

  if(!AGS_IS_WAVE_LOADER(wave_loader)){
    return(FALSE);
  }
  
  /* get wave loader mutex */
  pthread_mutex_lock(ags_wave_loader_get_class_mutex());
  
  wave_loader_mutex = wave_loader->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_loader_get_class_mutex());

  /* test flags */
  pthread_mutex_lock(wave_loader_mutex);

  retval = ((flags & (wave_loader->flags)) != 0) ? TRUE: FALSE;
  
  pthread_mutex_unlock(wave_loader_mutex);

  return(retval);
}

/**
 * ags_wave_loader_set_flags:
 * @wave_loader: the #AgsWaveLoader
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 2.0.13
 */
void
ags_wave_loader_set_flags(AgsWaveLoader *wave_loader, guint flags)
{
  pthread_mutex_t *wave_loader_mutex;

  if(!AGS_IS_WAVE_LOADER(wave_loader)){
    return;
  }
  
  /* get wave loader mutex */
  pthread_mutex_lock(ags_wave_loader_get_class_mutex());
  
  wave_loader_mutex = wave_loader->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_loader_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(wave_loader_mutex);

  wave_loader->flags |= flags;
  
  pthread_mutex_unlock(wave_loader_mutex);
}

/**
 * ags_wave_loader_unset_flags:
 * @wave_loader: the #AgsWaveLoader
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 2.0.13
 */
void
ags_wave_loader_unset_flags(AgsWaveLoader *wave_loader, guint flags)
{
  pthread_mutex_t *wave_loader_mutex;

  if(!AGS_IS_WAVE_LOADER(wave_loader)){
    return;
  }
  
  /* get wave loader mutex */
  pthread_mutex_lock(ags_wave_loader_get_class_mutex());
  
  wave_loader_mutex = wave_loader->obj_mutex;
  
  pthread_mutex_unlock(ags_wave_loader_get_class_mutex());

  /* unset flags */
  pthread_mutex_lock(wave_loader_mutex);

  wave_loader->flags &= (~flags);
  
  pthread_mutex_unlock(wave_loader_mutex);
}

void*
ags_wave_loader_run(void *ptr)
{
  AgsWaveLoader *wave_loader;

  GObject *output_soundcard;

  GList *start_wave, *wave;

  guint n_pads, current_pads;
  guint n_audio_channels, current_audio_channels;

  wave_loader = AGS_WAVE_LOADER(ptr);

  g_object_get(wave_loader->audio,
	       "output-soundcard", &output_soundcard,
	       NULL);

  g_object_unref(output_soundcard);
  
  wave_loader->audio_file = ags_audio_file_new(wave_loader->filename,
					       output_soundcard,
					       -1);
  ags_audio_file_open(wave_loader->audio_file);
  
  if(ags_wave_loader_test_flags(wave_loader, AGS_WAVE_LOADER_DO_REPLACE)){
    g_object_get(wave_loader->audio,
		 "wave", &start_wave,
		 NULL);

    wave = start_wave;

    while(wave != NULL){
      ags_audio_remove_wave(wave_loader->audio,
			    wave->data);
    
      wave = wave->next;
    }

    g_list_free_full(start_wave,
		     g_object_unref);

    g_object_set(wave_loader->audio,
		 "input-audio-file", wave_loader->audio_file,
		 NULL);
  }
  
  /* get some fields */
  g_object_get(wave_loader->audio,
	       "input-pads", &n_pads,
	       "audio-channels", &n_audio_channels,
	       "output-soundcard", &output_soundcard,
	       NULL);

  ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(wave_loader->audio_file->sound_resource),
				 &current_audio_channels,
				 NULL,
				 NULL,
				 NULL);
  
  if(current_audio_channels > n_audio_channels){
    n_audio_channels = current_audio_channels;
  }
  
  if(n_audio_channels > 0){
    GList *start_wave, *wave;

    /* resize */
    ags_audio_set_audio_channels(wave_loader->audio,
				 n_audio_channels, 0);

    if(n_pads < 1){
      ags_audio_set_pads(wave_loader->audio,
			 AGS_TYPE_INPUT,
			 1, 0);
    }
    
    wave =
      start_wave = ags_sound_resource_read_wave(AGS_SOUND_RESOURCE(wave_loader->audio_file->sound_resource),
						output_soundcard,
						-1,
						0,
						0.0, 0);

    if(ags_wave_loader_test_flags(wave_loader, AGS_WAVE_LOADER_DO_REPLACE)){
      while(wave != NULL){
	guint current_line;

#if 0	
	g_object_get(wave->data,
		     "line", &current_line,
		     NULL);
      
	g_object_set(wave->data,
		     "line", current_line,
		     NULL);
#endif
	
	ags_audio_add_wave(wave_loader->audio,
			   wave->data);
      
	wave = wave->next;
      }
    }
    
    g_list_free(start_wave);
  }
  
  ags_wave_loader_set_flags(wave_loader,
			    AGS_WAVE_LOADER_HAS_COMPLETED);
  
  pthread_exit(NULL);

#ifdef AGS_W32API
  return(NULL);
#endif  
}

void
ags_wave_loader_start(AgsWaveLoader *wave_loader)
{
  if(!AGS_IS_WAVE_LOADER(wave_loader)){
    return;
  }
  
  pthread_create(wave_loader->thread, NULL,
		 ags_wave_loader_run, wave_loader);
}


/**
 * ags_wave_loader_new:
 * @audio: the #AgsAudio
 * @filename: the filename
 * @do_replace: if %TRUE replace @audio's wave
 *
 * Create a new instance of #AgsWaveLoader.
 *
 * Returns: the new #AgsWaveLoader
 *
 * Since: 2.0.13
 */ 
AgsWaveLoader*
ags_wave_loader_new(AgsAudio *audio,
		    gchar *filename,
		    gboolean do_replace)
{
  AgsWaveLoader *wave_loader;

  wave_loader = (AgsWaveLoader *) g_object_new(AGS_TYPE_WAVE_LOADER,
					       "audio", audio,
					       "filename", filename,
					       NULL);

  if(do_replace){
    ags_wave_loader_set_flags(wave_loader,
			      AGS_WAVE_LOADER_DO_REPLACE);
  }
  
  return(wave_loader);
}
