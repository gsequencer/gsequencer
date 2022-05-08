/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/audio/thread/ags_sfz_loader.h>

#include <ags/audio/file/ags_audio_container.h>
#include <ags/audio/file/ags_audio_container_manager.h>
#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_sfz_file.h>

#include <ags/audio/task/ags_apply_sfz_synth.h>

#include <ags/i18n.h>

void ags_sfz_loader_class_init(AgsSFZLoaderClass *sfz_loader);
void ags_sfz_loader_init(AgsSFZLoader *sfz_loader);
void ags_sfz_loader_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_sfz_loader_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_sfz_loader_dispose(GObject *gobject);
void ags_sfz_loader_finalize(GObject *gobject);

void* ags_sfz_loader_run(void *ptr);

/**
 * SECTION:ags_sfz_loader
 * @short_description: load sfz asynchronously
 * @title: AgsSFZLoader
 * @section_id:
 * @include: ags/plugin/ags_sfz_loader.h
 *
 * The #AgsSFZLoader loads sfz audio data asynchronously.
 */

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_FILENAME,
  PROP_AUDIO_CONTAINER,
};

static gpointer ags_sfz_loader_parent_class = NULL;

GType
ags_sfz_loader_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sfz_loader = 0;

    static const GTypeInfo ags_sfz_loader_info = {
      sizeof(AgsSFZLoaderClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sfz_loader_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSFZLoader),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sfz_loader_init,
    };

    ags_type_sfz_loader = g_type_register_static(G_TYPE_OBJECT,
						 "AgsSFZLoader",
						 &ags_sfz_loader_info,
						 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sfz_loader);
  }

  return g_define_type_id__volatile;
}

void
ags_sfz_loader_class_init(AgsSFZLoaderClass *sfz_loader)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_sfz_loader_parent_class = g_type_class_peek_parent(sfz_loader);

  /* GObject */
  gobject = (GObjectClass *) sfz_loader;

  gobject->set_property = ags_sfz_loader_set_property;
  gobject->get_property = ags_sfz_loader_get_property;

  gobject->dispose = ags_sfz_loader_dispose;
  gobject->finalize = ags_sfz_loader_finalize;

  /* properties */
  /**
   * AgsSFZLoader:audio:
   *
   * The assigned audio.
   * 
   * Since: 3.0.0
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
   * AgsSFZLoader:filename:
   *
   * The filename to open.
   * 
   * Since: 3.0.0
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
   * AgsSFZLoader:audio-container:
   *
   * The audio container opened.
   * 
   * Since: 3.0.0
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
ags_sfz_loader_init(AgsSFZLoader *sfz_loader)
{
  sfz_loader->flags = 0;
  sfz_loader->connectable_flags = 0;

  /* add base plugin mutex */
  g_rec_mutex_init(&(sfz_loader->obj_mutex)); 

  /* fields */
  sfz_loader->thread = NULL;
  
  sfz_loader->audio = NULL;

  sfz_loader->filename = NULL;

  sfz_loader->base_note = 0.0;
  sfz_loader->count = 1;

  sfz_loader->audio_container = NULL;
}

void
ags_sfz_loader_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsSFZLoader *sfz_loader;

  GRecMutex *sfz_loader_mutex;

  sfz_loader = AGS_SFZ_LOADER(gobject);

  /* get base plugin mutex */
  sfz_loader_mutex = AGS_SFZ_LOADER_GET_OBJ_MUTEX(sfz_loader);

  switch(prop_id){
  case PROP_AUDIO:
  {
    AgsAudio *audio;

    audio = g_value_get_object(value);
      
    g_rec_mutex_lock(sfz_loader_mutex);

    if(sfz_loader->audio == audio){
      g_rec_mutex_unlock(sfz_loader_mutex);

      return;
    }

    if(sfz_loader->audio != NULL){
      g_object_unref(sfz_loader->audio);
    }

    if(audio != NULL){
      g_object_ref(audio);
    }

    sfz_loader->audio = audio;
      
    g_rec_mutex_unlock(sfz_loader_mutex);
  }
  break;
  case PROP_FILENAME:
  {
    gchar *filename;

    filename = g_value_get_string(value);
      
    g_rec_mutex_lock(sfz_loader_mutex);

    if(sfz_loader->filename == filename){
      g_rec_mutex_unlock(sfz_loader_mutex);

      return;
    }

    if(sfz_loader->filename != NULL){
      g_free(sfz_loader->filename);
    }

    sfz_loader->filename = g_strdup(filename);
      
    g_rec_mutex_unlock(sfz_loader_mutex);
  }
  break;
  case PROP_AUDIO_CONTAINER:
  {
    AgsAudioContainer *audio_container;

    audio_container = g_value_get_object(value);
      
    g_rec_mutex_lock(sfz_loader_mutex);

    if(sfz_loader->audio_container == audio_container){
      g_rec_mutex_unlock(sfz_loader_mutex);

      return;
    }

    if(sfz_loader->audio_container != NULL){
      g_object_unref(sfz_loader->audio_container);
    }

    if(audio_container != NULL){
      g_object_ref(audio_container);
    }

    sfz_loader->audio_container = audio_container;
      
    g_rec_mutex_unlock(sfz_loader_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}
    
void
ags_sfz_loader_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsSFZLoader *sfz_loader;

  GRecMutex *sfz_loader_mutex;

  sfz_loader = AGS_SFZ_LOADER(gobject);

  /* get base plugin mutex */
  sfz_loader_mutex = AGS_SFZ_LOADER_GET_OBJ_MUTEX(sfz_loader);

  switch(prop_id){
  case PROP_AUDIO:
  {
    g_rec_mutex_lock(sfz_loader_mutex);
      
    g_value_set_object(value, sfz_loader->audio);

    g_rec_mutex_unlock(sfz_loader_mutex);
  }
  break;
  case PROP_FILENAME:
  {
    g_rec_mutex_lock(sfz_loader_mutex);
      
    g_value_set_string(value, sfz_loader->filename);

    g_rec_mutex_unlock(sfz_loader_mutex);
  }
  break;
  case PROP_AUDIO_CONTAINER:
  {
    g_rec_mutex_lock(sfz_loader_mutex);
      
    g_value_set_object(value, sfz_loader->audio_container);

    g_rec_mutex_unlock(sfz_loader_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_sfz_loader_dispose(GObject *gobject)
{
  AgsSFZLoader *sfz_loader;

  sfz_loader = AGS_SFZ_LOADER(gobject);

  if(sfz_loader->audio != NULL){
    g_object_unref(sfz_loader->audio);

    sfz_loader->audio = NULL;
  }

  if(sfz_loader->audio_container != NULL){
    g_object_unref(sfz_loader->audio_container);

    sfz_loader->audio_container = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_sfz_loader_parent_class)->dispose(gobject);
}

void
ags_sfz_loader_finalize(GObject *gobject)
{
  AgsSFZLoader *sfz_loader;

  sfz_loader = AGS_SFZ_LOADER(gobject);
  
  /* destroy object mutex */
  if(sfz_loader->audio != NULL){
    g_object_unref(sfz_loader->audio);
  }

  g_free(sfz_loader->filename);

  if(sfz_loader->audio_container != NULL){
    g_object_unref(sfz_loader->audio_container);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_sfz_loader_parent_class)->finalize(gobject);
}

/**
 * ags_sfz_loader_test_flags:
 * @sfz_loader: the #AgsSFZLoader
 * @flags: the flags
 * 
 * Test @flags to be set on @recall.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_sfz_loader_test_flags(AgsSFZLoader *sfz_loader, guint flags)
{
  gboolean retval;
  
  GRecMutex *sfz_loader_mutex;

  if(!AGS_IS_SFZ_LOADER(sfz_loader)){
    return(FALSE);
  }
  
  /* get sfz loader mutex */
  sfz_loader_mutex = AGS_SFZ_LOADER_GET_OBJ_MUTEX(sfz_loader);

  /* test flags */
  g_rec_mutex_lock(sfz_loader_mutex);

  retval = ((flags & (sfz_loader->flags)) != 0) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(sfz_loader_mutex);

  return(retval);
}

/**
 * ags_sfz_loader_set_flags:
 * @sfz_loader: the #AgsSFZLoader
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 3.0.0
 */
void
ags_sfz_loader_set_flags(AgsSFZLoader *sfz_loader, guint flags)
{
  GRecMutex *sfz_loader_mutex;

  if(!AGS_IS_SFZ_LOADER(sfz_loader)){
    return;
  }
  
  /* get sfz loader mutex */
  sfz_loader_mutex = AGS_SFZ_LOADER_GET_OBJ_MUTEX(sfz_loader);

  /* set flags */
  g_rec_mutex_lock(sfz_loader_mutex);

  sfz_loader->flags |= flags;
  
  g_rec_mutex_unlock(sfz_loader_mutex);
}

/**
 * ags_sfz_loader_unset_flags:
 * @sfz_loader: the #AgsSFZLoader
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 3.0.0
 */
void
ags_sfz_loader_unset_flags(AgsSFZLoader *sfz_loader, guint flags)
{
  GRecMutex *sfz_loader_mutex;

  if(!AGS_IS_SFZ_LOADER(sfz_loader)){
    return;
  }
  
  /* get sfz loader mutex */
  sfz_loader_mutex = AGS_SFZ_LOADER_GET_OBJ_MUTEX(sfz_loader);

  /* unset flags */
  g_rec_mutex_lock(sfz_loader_mutex);

  sfz_loader->flags &= (~flags);
  
  g_rec_mutex_unlock(sfz_loader_mutex);
}

void*
ags_sfz_loader_run(void *ptr)
{
  AgsAudioContainerManager *audio_container_manager;

  AgsSFZLoader *sfz_loader;
  
  GObject *output_soundcard;
  
  GRecMutex *audio_container_manager_mutex;

  sfz_loader = AGS_SFZ_LOADER(ptr);
  
  output_soundcard = NULL;  

  g_object_get(sfz_loader->audio,
	       "output-soundcard", &output_soundcard,
	       NULL);

  audio_container_manager = ags_audio_container_manager_get_instance();

  /* get audio container manager mutex */
  audio_container_manager_mutex = AGS_AUDIO_CONTAINER_MANAGER_GET_OBJ_MUTEX(audio_container_manager);
  
  g_rec_mutex_lock(audio_container_manager_mutex);

  sfz_loader->audio_container = ags_audio_container_manager_find_audio_container(audio_container_manager,
										 sfz_loader->filename);
  
  if(sfz_loader->audio_container == NULL){
    sfz_loader->audio_container = ags_audio_container_new(sfz_loader->filename,
							  NULL,
							  NULL,
							  NULL,
							  output_soundcard,
							  -1);
    g_object_ref(sfz_loader->audio_container);

    ags_audio_container_open(sfz_loader->audio_container);

    ags_audio_container_manager_add_audio_container(audio_container_manager,
						    sfz_loader->audio_container);
  }
  
  g_rec_mutex_unlock(audio_container_manager_mutex);
  
  /* select */
  if(sfz_loader->audio_container->sound_container != NULL){
    ags_sound_container_level_up(AGS_SOUND_CONTAINER(sfz_loader->audio_container->sound_container),
				 5);

    ags_sound_container_select_level_by_index(AGS_SOUND_CONTAINER(sfz_loader->audio_container->sound_container),
					      0);

    AGS_SFZ_FILE(sfz_loader->audio_container->sound_container)->nesting_level += 1;
  }
  
  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }
  
  if(ags_sfz_loader_test_flags(sfz_loader, AGS_SFZ_LOADER_RUN_APPLY_SYNTH)){
    AgsChannel *start_channel;
    
    AgsApplySFZSynth *apply_sfz_synth;

    AgsTaskLauncher *task_launcher;
    
    AgsApplicationContext *application_context;
    
    GList *start_sfz_synth_generator, *sfz_synth_generator;

    application_context = ags_application_context_get_instance();

    task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));
    
    start_sfz_synth_generator = NULL;

    start_channel = NULL;
    
    g_object_get(sfz_loader->audio,
		 "sfz-synth-generator", &start_sfz_synth_generator,
		 "input", &start_channel,
		 NULL);

    g_object_set(start_sfz_synth_generator->data,
		 "filename", sfz_loader->filename,
		 NULL);
    
    apply_sfz_synth = ags_apply_sfz_synth_new(start_sfz_synth_generator->data,
					      start_channel,
					      sfz_loader->base_note,
					      sfz_loader->count);

    ags_task_launcher_add_task(task_launcher,
			       apply_sfz_synth);
    
    g_list_free_full(start_sfz_synth_generator,
		     (GDestroyNotify) g_object_unref);
    
    if(start_channel != NULL){
      g_object_unref(start_channel);
    }
  }

  ags_sfz_loader_set_flags(sfz_loader,
			   AGS_SFZ_LOADER_HAS_COMPLETED);
  
  g_thread_exit(NULL);

  return(NULL);
}

/**
 * ags_sfz_loader_get_audio:
 * @sfz_loader: the #AgsSFZLoader
 * 
 * Get audio of @sfz_loader.
 * 
 * Returns: the assigned #AgsAudio
 * 
 * Since: 3.2.0
 */
AgsAudio*
ags_sfz_loader_get_audio(AgsSFZLoader *sfz_loader)
{
  AgsAudio *audio;

  if(!AGS_IS_SFZ_LOADER(sfz_loader)){
    return(NULL);
  }

  g_object_get(sfz_loader,
	       "audio", &audio,
	       NULL);

  return(audio);
}

/**
 * ags_sfz_loader_set_audio:
 * @sfz_loader: the #AgsSFZLoader
 * @audio: the #AgsAudio
 * 
 * Set audio of @sfz_loader.
 * 
 * Since: 3.2.0
 */
void
ags_sfz_loader_set_audio(AgsSFZLoader *sfz_loader,
			 AgsAudio *audio)
{
  if(!AGS_IS_SFZ_LOADER(sfz_loader)){
    return;
  }

  g_object_set(sfz_loader,
	       "audio", audio,
	       NULL);
}

/**
 * ags_sfz_loader_get_filename:
 * @sfz_loader: the #AgsSFZLoader
 * 
 * Get filename of @sfz_loader.
 * 
 * Returns: (transfer full): the assigned filename
 * 
 * Since: 3.2.0
 */
gchar*
ags_sfz_loader_get_filename(AgsSFZLoader *sfz_loader)
{
  gchar *filename;
  
  if(!AGS_IS_SFZ_LOADER(sfz_loader)){
    return(NULL);
  }

  g_object_get(sfz_loader,
	       "filename", &filename,
	       NULL);

  return(filename);
}

/**
 * ags_sfz_loader_set_filename:
 * @sfz_loader: the #AgsSFZLoader
 * @filename: the filename
 * 
 * Set filename of @sfz_loader.
 * 
 * Since: 3.2.0
 */
void
ags_sfz_loader_set_filename(AgsSFZLoader *sfz_loader,
			    gchar *filename)
{
  if(!AGS_IS_SFZ_LOADER(sfz_loader)){
    return;
  }

  g_object_set(sfz_loader,
	       "filename", filename,
	       NULL);
}

/**
 * ags_sfz_loader_get_audio_container:
 * @sfz_loader: the #AgsSFZLoader
 * 
 * Get audio container of @sfz_loader.
 * 
 * Returns: (transfer full): the assigned #AgsAudioContainer
 * 
 * Since: 3.2.0
 */
AgsAudioContainer*
ags_sfz_loader_get_audio_container(AgsSFZLoader *sfz_loader)
{
  AgsAudioContainer *audio_container;
  
  if(!AGS_IS_SFZ_LOADER(sfz_loader)){
    return(NULL);
  }

  g_object_get(sfz_loader,
	       "audio-container", &audio_container,
	       NULL);

  return(audio_container);
}

/**
 * ags_sfz_loader_set_audio_container:
 * @sfz_loader: the #AgsSFZLoader
 * @audio_container: the #AgsAudioContainer
 * 
 * Set audio container of @sfz_loader.
 * 
 * Since: 3.2.0
 */
void
ags_sfz_loader_set_audio_container(AgsSFZLoader *sfz_loader,
				   AgsAudioContainer *audio_container)
{
  if(!AGS_IS_SFZ_LOADER(sfz_loader)){
    return;
  }

  g_object_set(sfz_loader,
	       "audio-container", audio_container,
	       NULL);
}

/**
 * ags_sfz_loader_start:
 * @sfz_loader: the #AgsSFZLoader
 * 
 * Start @sfz_loader.
 * 
 * Since: 3.0.0
 */
void
ags_sfz_loader_start(AgsSFZLoader *sfz_loader)
{
  if(!AGS_IS_SFZ_LOADER(sfz_loader)){
    return;
  }
  
  sfz_loader->thread = g_thread_new("Advanced Gtk+ Sequencer - SFZ loader",
				    ags_sfz_loader_run,
				    sfz_loader);
}

/**
 * ags_sfz_loader_new:
 * @audio: the #AgsAudio
 * @filename: the filename
 * @do_replace: if %TRUE replace @audio's sfz
 *
 * Create a new instance of #AgsSFZLoader.
 *
 * Returns: the new #AgsSFZLoader
 *
 * Since: 3.0.0
 */ 
AgsSFZLoader*
ags_sfz_loader_new(AgsAudio *audio,
		   gchar *filename,
		   gboolean do_replace)
{
  AgsSFZLoader *sfz_loader;

  sfz_loader = (AgsSFZLoader *) g_object_new(AGS_TYPE_SFZ_LOADER,
					     "audio", audio,
					     "filename", filename,
					     NULL);

  if(do_replace){
    ags_sfz_loader_set_flags(sfz_loader,
			     AGS_SFZ_LOADER_DO_REPLACE);
  }
  
  return(sfz_loader);
}
