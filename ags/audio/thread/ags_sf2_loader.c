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

#include <ags/audio/thread/ags_sf2_loader.h>

#include <ags/libags.h>

#include <ags/audio/ags_input.h>

#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_ipatch.h>

#include <ags/i18n.h>

void ags_sf2_loader_class_init(AgsSF2LoaderClass *sf2_loader);
void ags_sf2_loader_init(AgsSF2Loader *sf2_loader);
void ags_sf2_loader_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_sf2_loader_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_sf2_loader_dispose(GObject *gobject);
void ags_sf2_loader_finalize(GObject *gobject);

void* ags_sf2_loader_run(void *ptr);

/**
 * SECTION:ags_sf2_loader
 * @short_description: load Soundfont2 asynchronously
 * @title: AgsSF2Loader
 * @section_id:
 * @include: ags/plugin/ags_sf2_loader.h
 *
 * The #AgsSF2Loader loads Soundfont2 audio data asynchronously.
 */

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_FILENAME,
  PROP_PRESET,
  PROP_INSTRUMENT,
  PROP_AUDIO_CONTAINER,
};

static gpointer ags_sf2_loader_parent_class = NULL;

static pthread_mutex_t ags_sf2_loader_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_sf2_loader_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sf2_loader = 0;

    static const GTypeInfo ags_sf2_loader_info = {
      sizeof(AgsSF2LoaderClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sf2_loader_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSF2Loader),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sf2_loader_init,
    };

    ags_type_sf2_loader = g_type_register_static(G_TYPE_OBJECT,
						 "AgsSF2Loader",
						 &ags_sf2_loader_info,
						 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sf2_loader);
  }

  return g_define_type_id__volatile;
}

void
ags_sf2_loader_class_init(AgsSF2LoaderClass *sf2_loader)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_sf2_loader_parent_class = g_type_class_peek_parent(sf2_loader);

  /* GObject */
  gobject = (GObjectClass *) sf2_loader;

  gobject->set_property = ags_sf2_loader_set_property;
  gobject->get_property = ags_sf2_loader_get_property;

  gobject->dispose = ags_sf2_loader_dispose;
  gobject->finalize = ags_sf2_loader_finalize;

  /* properties */
  /**
   * AgsSF2Loader:audio:
   *
   * The assigned audio.
   * 
   * Since: 2.4.0
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
   * AgsSF2Loader:filename:
   *
   * The filename to open.
   * 
   * Since: 2.4.0
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
   * AgsSF2Loader:preset:
   *
   * The preset to open.
   * 
   * Since: 2.4.0
   */
  param_spec = g_param_spec_string("preset",
				   i18n_pspec("preset"),
				   i18n_pspec("The preset"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PRESET,
				  param_spec);

  /**
   * AgsSF2Loader:instrument:
   *
   * The instrument to open.
   * 
   * Since: 2.4.0
   */
  param_spec = g_param_spec_string("instrument",
				   i18n_pspec("instrument"),
				   i18n_pspec("The instrument"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INSTRUMENT,
				  param_spec);
  
  /**
   * AgsSF2Loader:audio-container:
   *
   * The audio container opened.
   * 
   * Since: 2.4.0
   */
  param_spec = g_param_spec_object("audio-container",
				   i18n_pspec("audio container"),
				   i18n_pspec("The audio container"),
				   AGS_TYPE_AUDIO_CONTAINER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CONTAINER,
				  param_spec);
}

void
ags_sf2_loader_init(AgsSF2Loader *sf2_loader)
{
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  sf2_loader->flags = 0;

  /* add base plugin mutex */
  sf2_loader->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  sf2_loader->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  /* fields */
  sf2_loader->thread = (pthread_t *) malloc(sizeof(pthread_t));
  
  sf2_loader->audio = NULL;

  sf2_loader->filename = NULL;
  sf2_loader->preset = NULL;
  sf2_loader->instrument = NULL;
  
  sf2_loader->audio_container = NULL;
}

void
ags_sf2_loader_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsSF2Loader *sf2_loader;

  pthread_mutex_t *sf2_loader_mutex;

  sf2_loader = AGS_SF2_LOADER(gobject);

  /* get base plugin mutex */
  sf2_loader_mutex = AGS_SF2_LOADER_GET_OBJ_MUTEX(sf2_loader);

  switch(prop_id){
  case PROP_AUDIO:
  {
    AgsAudio *audio;

    audio = g_value_get_object(value);
      
    pthread_mutex_lock(sf2_loader_mutex);

    if(sf2_loader->audio == audio){
      pthread_mutex_unlock(sf2_loader_mutex);

      return;
    }

    if(sf2_loader->audio != NULL){
      g_object_unref(sf2_loader->audio);
    }

    if(audio != NULL){
      g_object_ref(audio);
    }

    sf2_loader->audio = audio;
      
    pthread_mutex_unlock(sf2_loader_mutex);
  }
  break;
  case PROP_FILENAME:
  {
    gchar *filename;

    filename = g_value_get_string(value);
      
    pthread_mutex_lock(sf2_loader_mutex);

    if(sf2_loader->filename == filename){
      pthread_mutex_unlock(sf2_loader_mutex);

      return;
    }

    if(sf2_loader->filename != NULL){
      g_free(sf2_loader->filename);
    }

    sf2_loader->filename = g_strdup(filename);
      
    pthread_mutex_unlock(sf2_loader_mutex);
  }
  break;
  case PROP_PRESET:
  {
    gchar *preset;

    preset = g_value_get_string(value);
      
    pthread_mutex_lock(sf2_loader_mutex);

    if(sf2_loader->preset == preset){
      pthread_mutex_unlock(sf2_loader_mutex);

      return;
    }

    if(sf2_loader->preset != NULL){
      g_free(sf2_loader->preset);
    }

    sf2_loader->preset = g_strdup(preset);
      
    pthread_mutex_unlock(sf2_loader_mutex);
  }
  break;
  case PROP_INSTRUMENT:
  {
    gchar *instrument;

    instrument = g_value_get_string(value);
      
    pthread_mutex_lock(sf2_loader_mutex);

    if(sf2_loader->instrument == instrument){
      pthread_mutex_unlock(sf2_loader_mutex);

      return;
    }

    if(sf2_loader->instrument != NULL){
      g_free(sf2_loader->instrument);
    }

    sf2_loader->instrument = g_strdup(instrument);
      
    pthread_mutex_unlock(sf2_loader_mutex);
  }
  break;
  case PROP_AUDIO_CONTAINER:
  {
    AgsAudioContainer *audio_container;

    audio_container = g_value_get_object(value);
      
    pthread_mutex_lock(sf2_loader_mutex);

    if(sf2_loader->audio_container == audio_container){
      pthread_mutex_unlock(sf2_loader_mutex);

      return;
    }

    if(sf2_loader->audio_container != NULL){
      g_object_unref(sf2_loader->audio_container);
    }

    if(audio_container != NULL){
      g_object_ref(audio_container);
    }

    sf2_loader->audio_container = audio_container;
      
    pthread_mutex_unlock(sf2_loader_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}
    
void
ags_sf2_loader_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsSF2Loader *sf2_loader;

  pthread_mutex_t *sf2_loader_mutex;

  sf2_loader = AGS_SF2_LOADER(gobject);

  /* get base plugin mutex */
  sf2_loader_mutex = AGS_SF2_LOADER_GET_OBJ_MUTEX(sf2_loader);

  switch(prop_id){
  case PROP_AUDIO:
  {
    pthread_mutex_lock(sf2_loader_mutex);
      
    g_value_set_object(value, sf2_loader->audio);

    pthread_mutex_unlock(sf2_loader_mutex);
  }
  break;
  case PROP_FILENAME:
  {
    pthread_mutex_lock(sf2_loader_mutex);
      
    g_value_set_string(value, sf2_loader->filename);

    pthread_mutex_unlock(sf2_loader_mutex);
  }
  break;
  case PROP_PRESET:
  {
    pthread_mutex_lock(sf2_loader_mutex);
      
    g_value_set_string(value, sf2_loader->preset);

    pthread_mutex_unlock(sf2_loader_mutex);
  }
  break;
  case PROP_INSTRUMENT:
  {
    pthread_mutex_lock(sf2_loader_mutex);
      
    g_value_set_string(value, sf2_loader->instrument);

    pthread_mutex_unlock(sf2_loader_mutex);
  }
  break;
  case PROP_AUDIO_CONTAINER:
  {
    pthread_mutex_lock(sf2_loader_mutex);
      
    g_value_set_object(value, sf2_loader->audio_container);

    pthread_mutex_unlock(sf2_loader_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_sf2_loader_dispose(GObject *gobject)
{
  AgsSF2Loader *sf2_loader;

  sf2_loader = AGS_SF2_LOADER(gobject);

  if(sf2_loader->audio != NULL){
    g_object_unref(sf2_loader->audio);

    sf2_loader->audio = NULL;
  }

  if(sf2_loader->audio_container != NULL){
    g_object_unref(sf2_loader->audio_container);

    sf2_loader->audio_container = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_sf2_loader_parent_class)->dispose(gobject);
}

void
ags_sf2_loader_finalize(GObject *gobject)
{
  AgsSF2Loader *sf2_loader;

  sf2_loader = AGS_SF2_LOADER(gobject);
  
  /* destroy object mutex */
  pthread_mutex_destroy(sf2_loader->obj_mutex);
  free(sf2_loader->obj_mutex);

  pthread_mutexattr_destroy(sf2_loader->obj_mutexattr);
  free(sf2_loader->obj_mutexattr);

  if(sf2_loader->audio != NULL){
    g_object_unref(sf2_loader->audio);
  }

  g_free(sf2_loader->filename);
  g_free(sf2_loader->preset);
  g_free(sf2_loader->instrument);

  if(sf2_loader->audio_container != NULL){
    g_object_unref(sf2_loader->audio_container);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_sf2_loader_parent_class)->finalize(gobject);
}

/**
 * ags_sf2_loader_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.4.0
 */
pthread_mutex_t*
ags_sf2_loader_get_class_mutex()
{
  return(&ags_sf2_loader_class_mutex);
}

/**
 * ags_sf2_loader_test_flags:
 * @sf2_loader: the #AgsSF2Loader
 * @flags: the flags
 * 
 * Test @flags to be set on @recall.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 2.4.0
 */
gboolean
ags_sf2_loader_test_flags(AgsSF2Loader *sf2_loader, guint flags)
{
  gboolean retval;
  
  pthread_mutex_t *sf2_loader_mutex;

  if(!AGS_IS_SF2_LOADER(sf2_loader)){
    return(FALSE);
  }
  
  /* get sf2 loader mutex */
  sf2_loader_mutex = AGS_SF2_LOADER_GET_OBJ_MUTEX(sf2_loader);

  /* test flags */
  pthread_mutex_lock(sf2_loader_mutex);

  retval = ((flags & (sf2_loader->flags)) != 0) ? TRUE: FALSE;
  
  pthread_mutex_unlock(sf2_loader_mutex);

  return(retval);
}

/**
 * ags_sf2_loader_set_flags:
 * @sf2_loader: the #AgsSF2Loader
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 2.4.0
 */
void
ags_sf2_loader_set_flags(AgsSF2Loader *sf2_loader, guint flags)
{
  pthread_mutex_t *sf2_loader_mutex;

  if(!AGS_IS_SF2_LOADER(sf2_loader)){
    return;
  }
  
  /* get sf2 loader mutex */
  sf2_loader_mutex = AGS_SF2_LOADER_GET_OBJ_MUTEX(sf2_loader);

  /* set flags */
  pthread_mutex_lock(sf2_loader_mutex);

  sf2_loader->flags |= flags;
  
  pthread_mutex_unlock(sf2_loader_mutex);
}

/**
 * ags_sf2_loader_unset_flags:
 * @sf2_loader: the #AgsSF2Loader
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 2.4.0
 */
void
ags_sf2_loader_unset_flags(AgsSF2Loader *sf2_loader, guint flags)
{
  pthread_mutex_t *sf2_loader_mutex;

  if(!AGS_IS_SF2_LOADER(sf2_loader)){
    return;
  }
  
  /* get sf2 loader mutex */
  sf2_loader_mutex = AGS_SF2_LOADER_GET_OBJ_MUTEX(sf2_loader);

  /* unset flags */
  pthread_mutex_lock(sf2_loader_mutex);

  sf2_loader->flags &= (~flags);
  
  pthread_mutex_unlock(sf2_loader_mutex);
}

void*
ags_sf2_loader_run(void *ptr)
{
  AgsSF2Loader *sf2_loader;

  GObject *output_soundcard;

  GList *start_sf2, *sf2;

  guint n_pads, current_pads;
  guint n_audio_channels, current_audio_channels;

  sf2_loader = AGS_SF2_LOADER(ptr);

  g_object_get(sf2_loader->audio,
	       "output-soundcard", &output_soundcard,
	       NULL);

  sf2_loader->audio_container = ags_audio_container_new(sf2_loader->filename,
							NULL,
							NULL,
							NULL,
							output_soundcard,
							-1);
  ags_audio_container_open(sf2_loader->audio_container);
  
  if(sf2_loader->audio_container->sound_container != NULL){
    ags_sound_container_select_level_by_index(AGS_SOUND_CONTAINER(sf2_loader->audio_container->sound_container),
					      0);

    AGS_IPATCH(sf2_loader->audio_container->sound_container)->nesting_level += 1;
  }

  g_object_unref(output_soundcard);
  
  ags_sf2_loader_set_flags(sf2_loader,
			   AGS_SF2_LOADER_HAS_COMPLETED);

  pthread_exit(NULL);

#ifdef AGS_W32API
  return(NULL);
#endif  
}

void
ags_sf2_loader_start(AgsSF2Loader *sf2_loader)
{
  if(!AGS_IS_SF2_LOADER(sf2_loader)){
    return;
  }
  
  pthread_create(sf2_loader->thread, NULL,
		 ags_sf2_loader_run, sf2_loader);
}


/**
 * ags_sf2_loader_new:
 * @audio: the #AgsAudio
 * @filename: the filename
 * @preset: the preset
 * @instrument: the instrument
 *
 * Create a new instance of #AgsSF2Loader.
 *
 * Returns: the new #AgsSF2Loader
 *
 * Since: 2.4.0
 */ 
AgsSF2Loader*
ags_sf2_loader_new(AgsAudio *audio,
		   gchar *filename,
		   gchar *preset,
		   gchar *instrument)
{
  AgsSF2Loader *sf2_loader;

  sf2_loader = (AgsSF2Loader *) g_object_new(AGS_TYPE_SF2_LOADER,
					     "audio", audio,
					     "filename", filename,
					     "preset", preset,
					     "instrument", instrument,
					     NULL);
  
  return(sf2_loader);
}
