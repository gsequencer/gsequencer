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

#include <ags/audio/thread/ags_sfz_instrument_loader.h>

#include <ags/audio/ags_input.h>

#include <ags/audio/file/ags_audio_container.h>
#include <ags/audio/file/ags_audio_container_manager.h>
#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_sfz_file.h>

#include <ags/audio/task/ags_apply_sfz_instrument.h>

#include <ags/audio/fx/ags_fx_sfz_synth_audio.h>

#include <ags/i18n.h>

void ags_sfz_instrument_loader_class_init(AgsSFZInstrumentLoaderClass *sfz_instrument_loader);
void ags_sfz_instrument_loader_init(AgsSFZInstrumentLoader *sfz_instrument_loader);
void ags_sfz_instrument_loader_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec);
void ags_sfz_instrument_loader_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec);
void ags_sfz_instrument_loader_dispose(GObject *gobject);
void ags_sfz_instrument_loader_finalize(GObject *gobject);

void* ags_sfz_instrument_loader_run(void *ptr);

/**
 * SECTION:ags_sfz_instrument_loader
 * @short_description: load Soundfont2 MIDI locale asynchronously
 * @title: AgsSFZInstrumentLoader
 * @section_id:
 * @include: ags/plugin/ags_sfz_instrument_loader.h
 *
 * The #AgsSFZInstrumentLoader loads Soundfont2 audio data of MIDI locale asynchronously.
 */

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_FILENAME,
  PROP_AUDIO_CONTAINER,
  PROP_TEMPLATE,
  PROP_SYNTH,
};

static gpointer ags_sfz_instrument_loader_parent_class = NULL;

GType
ags_sfz_instrument_loader_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sfz_instrument_loader = 0;

    static const GTypeInfo ags_sfz_instrument_loader_info = {
      sizeof(AgsSFZInstrumentLoaderClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sfz_instrument_loader_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSFZInstrumentLoader),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sfz_instrument_loader_init,
    };

    ags_type_sfz_instrument_loader = g_type_register_static(G_TYPE_OBJECT,
							    "AgsSFZInstrumentLoader",
							    &ags_sfz_instrument_loader_info,
							    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sfz_instrument_loader);
  }

  return g_define_type_id__volatile;
}

void
ags_sfz_instrument_loader_class_init(AgsSFZInstrumentLoaderClass *sfz_instrument_loader)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_sfz_instrument_loader_parent_class = g_type_class_peek_parent(sfz_instrument_loader);

  /* GObject */
  gobject = (GObjectClass *) sfz_instrument_loader;

  gobject->set_property = ags_sfz_instrument_loader_set_property;
  gobject->get_property = ags_sfz_instrument_loader_get_property;

  gobject->dispose = ags_sfz_instrument_loader_dispose;
  gobject->finalize = ags_sfz_instrument_loader_finalize;

  /* properties */
  /**
   * AgsSFZInstrumentLoader:audio:
   *
   * The assigned audio.
   * 
   * Since: 3.17.0
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
   * AgsSFZInstrumentLoader:filename:
   *
   * The filename to open.
   * 
   * Since: 3.17.0
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
   * AgsSFZInstrumentLoader:audio-container:
   *
   * The audio container opened.
   * 
   * Since: 3.17.0
   */
  param_spec = g_param_spec_object("audio-container",
				   i18n_pspec("audio container"),
				   i18n_pspec("The audio container"),
				   AGS_TYPE_AUDIO_CONTAINER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CONTAINER,
				  param_spec);

  /**
   * AgsSFZInstrumentLoader:template:
   *
   * The assigned template.
   * 
   * Since: 3.17.0
   */
  param_spec = g_param_spec_pointer("template",
				    i18n_pspec("template"),
				    i18n_pspec("The template"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TEMPLATE,
				  param_spec);

  /**
   * AgsSFZInstrumentLoader:synth:
   *
   * The assigned synth.
   * 
   * Since: 3.17.0
   */
  param_spec = g_param_spec_pointer("synth",
				    i18n_pspec("synth"),
				    i18n_pspec("The synth"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH,
				  param_spec);
}

void
ags_sfz_instrument_loader_init(AgsSFZInstrumentLoader *sfz_instrument_loader)
{
  sfz_instrument_loader->flags = 0;

  /* add base plugin mutex */
  g_rec_mutex_init(&(sfz_instrument_loader->obj_mutex));

  /* fields */
  sfz_instrument_loader->thread = NULL;
  
  sfz_instrument_loader->audio = NULL;

  sfz_instrument_loader->filename = NULL;
  
  sfz_instrument_loader->template = NULL;
  sfz_instrument_loader->synth = NULL;

  sfz_instrument_loader->audio_container = NULL;
}

void
ags_sfz_instrument_loader_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec)
{
  AgsSFZInstrumentLoader *sfz_instrument_loader;

  GRecMutex *sfz_instrument_loader_mutex;

  sfz_instrument_loader = AGS_SFZ_INSTRUMENT_LOADER(gobject);

  /* get base plugin mutex */
  sfz_instrument_loader_mutex = AGS_SFZ_INSTRUMENT_LOADER_GET_OBJ_MUTEX(sfz_instrument_loader);

  switch(prop_id){
  case PROP_AUDIO:
  {
    AgsAudio *audio;

    audio = g_value_get_object(value);
      
    g_rec_mutex_lock(sfz_instrument_loader_mutex);

    if(sfz_instrument_loader->audio == audio){
      g_rec_mutex_unlock(sfz_instrument_loader_mutex);

      return;
    }

    if(sfz_instrument_loader->audio != NULL){
      g_object_unref(sfz_instrument_loader->audio);
    }

    if(audio != NULL){
      g_object_ref(audio);
    }

    sfz_instrument_loader->audio = audio;
      
    g_rec_mutex_unlock(sfz_instrument_loader_mutex);
  }
  break;
  case PROP_FILENAME:
  {
    gchar *filename;

    filename = g_value_get_string(value);
      
    g_rec_mutex_lock(sfz_instrument_loader_mutex);

    if(sfz_instrument_loader->filename == filename){
      g_rec_mutex_unlock(sfz_instrument_loader_mutex);

      return;
    }

    if(sfz_instrument_loader->filename != NULL){
      g_free(sfz_instrument_loader->filename);
    }

    sfz_instrument_loader->filename = g_strdup(filename);
      
    g_rec_mutex_unlock(sfz_instrument_loader_mutex);
  }
  break;
  case PROP_AUDIO_CONTAINER:
  {
    AgsAudioContainer *audio_container;

    audio_container = g_value_get_object(value);
      
    g_rec_mutex_lock(sfz_instrument_loader_mutex);

    if(sfz_instrument_loader->audio_container == audio_container){
      g_rec_mutex_unlock(sfz_instrument_loader_mutex);

      return;
    }

    if(sfz_instrument_loader->audio_container != NULL){
      g_object_unref(sfz_instrument_loader->audio_container);
    }

    if(audio_container != NULL){
      g_object_ref(audio_container);
    }

    sfz_instrument_loader->audio_container = audio_container;
      
    g_rec_mutex_unlock(sfz_instrument_loader_mutex);
  }
  break;
  case PROP_TEMPLATE:
  {
    AgsSFZSynthUtil *template;

    template = g_value_get_pointer(value);
      
    g_rec_mutex_lock(sfz_instrument_loader_mutex);

    sfz_instrument_loader->template = template;
      
    g_rec_mutex_unlock(sfz_instrument_loader_mutex);
  }
  break;
  case PROP_SYNTH:
  {
    AgsSFZSynthUtil *synth;

    synth = g_value_get_pointer(value);
      
    g_rec_mutex_lock(sfz_instrument_loader_mutex);

    sfz_instrument_loader->synth = synth;
      
    g_rec_mutex_unlock(sfz_instrument_loader_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}
    
void
ags_sfz_instrument_loader_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec)
{
  AgsSFZInstrumentLoader *sfz_instrument_loader;

  GRecMutex *sfz_instrument_loader_mutex;

  sfz_instrument_loader = AGS_SFZ_INSTRUMENT_LOADER(gobject);

  /* get base plugin mutex */
  sfz_instrument_loader_mutex = AGS_SFZ_INSTRUMENT_LOADER_GET_OBJ_MUTEX(sfz_instrument_loader);

  switch(prop_id){
  case PROP_AUDIO:
  {
    g_rec_mutex_lock(sfz_instrument_loader_mutex);
      
    g_value_set_object(value, sfz_instrument_loader->audio);

    g_rec_mutex_unlock(sfz_instrument_loader_mutex);
  }
  break;
  case PROP_FILENAME:
  {
    g_rec_mutex_lock(sfz_instrument_loader_mutex);
      
    g_value_set_string(value, sfz_instrument_loader->filename);

    g_rec_mutex_unlock(sfz_instrument_loader_mutex);
  }
  break;
  case PROP_AUDIO_CONTAINER:
  {
    g_rec_mutex_lock(sfz_instrument_loader_mutex);
      
    g_value_set_object(value, sfz_instrument_loader->audio_container);

    g_rec_mutex_unlock(sfz_instrument_loader_mutex);
  }
  break;
  case PROP_TEMPLATE:
  {
    g_rec_mutex_lock(sfz_instrument_loader_mutex);
      
    g_value_set_pointer(value, sfz_instrument_loader->template);

    g_rec_mutex_unlock(sfz_instrument_loader_mutex);
  }
  break;
  case PROP_SYNTH:
  {
    g_rec_mutex_lock(sfz_instrument_loader_mutex);
      
    g_value_set_pointer(value, sfz_instrument_loader->synth);

    g_rec_mutex_unlock(sfz_instrument_loader_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_sfz_instrument_loader_dispose(GObject *gobject)
{
  AgsSFZInstrumentLoader *sfz_instrument_loader;

  sfz_instrument_loader = AGS_SFZ_INSTRUMENT_LOADER(gobject);

  if(sfz_instrument_loader->audio != NULL){
    g_object_unref(sfz_instrument_loader->audio);

    sfz_instrument_loader->audio = NULL;
  }

  if(sfz_instrument_loader->audio_container != NULL){
    g_object_unref(sfz_instrument_loader->audio_container);

    sfz_instrument_loader->audio_container = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_sfz_instrument_loader_parent_class)->dispose(gobject);
}

void
ags_sfz_instrument_loader_finalize(GObject *gobject)
{
  AgsSFZInstrumentLoader *sfz_instrument_loader;

  sfz_instrument_loader = AGS_SFZ_INSTRUMENT_LOADER(gobject);
  
  /* destroy object mutex */
  if(sfz_instrument_loader->audio != NULL){
    g_object_unref(sfz_instrument_loader->audio);
  }

  g_free(sfz_instrument_loader->filename);

  if(sfz_instrument_loader->audio_container != NULL){
    g_object_unref(sfz_instrument_loader->audio_container);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_sfz_instrument_loader_parent_class)->finalize(gobject);
}

/**
 * ags_sfz_instrument_loader_test_flags:
 * @sfz_instrument_loader: the #AgsSFZInstrumentLoader
 * @flags: the flags
 * 
 * Test @flags to be set on @recall.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 3.17.0
 */
gboolean
ags_sfz_instrument_loader_test_flags(AgsSFZInstrumentLoader *sfz_instrument_loader, guint flags)
{
  gboolean retval;
  
  GRecMutex *sfz_instrument_loader_mutex;

  if(!AGS_IS_SFZ_INSTRUMENT_LOADER(sfz_instrument_loader)){
    return(FALSE);
  }
  
  /* get sfz loader mutex */
  sfz_instrument_loader_mutex = AGS_SFZ_INSTRUMENT_LOADER_GET_OBJ_MUTEX(sfz_instrument_loader);

  /* test flags */
  g_rec_mutex_lock(sfz_instrument_loader_mutex);

  retval = ((flags & (sfz_instrument_loader->flags)) != 0) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(sfz_instrument_loader_mutex);

  return(retval);
}

/**
 * ags_sfz_instrument_loader_set_flags:
 * @sfz_instrument_loader: the #AgsSFZInstrumentLoader
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 3.17.0
 */
void
ags_sfz_instrument_loader_set_flags(AgsSFZInstrumentLoader *sfz_instrument_loader, guint flags)
{
  GRecMutex *sfz_instrument_loader_mutex;

  if(!AGS_IS_SFZ_INSTRUMENT_LOADER(sfz_instrument_loader)){
    return;
  }
  
  /* get sfz loader mutex */
  sfz_instrument_loader_mutex = AGS_SFZ_INSTRUMENT_LOADER_GET_OBJ_MUTEX(sfz_instrument_loader);

  /* set flags */
  g_rec_mutex_lock(sfz_instrument_loader_mutex);

  sfz_instrument_loader->flags |= flags;
  
  g_rec_mutex_unlock(sfz_instrument_loader_mutex);
}

/**
 * ags_sfz_instrument_loader_unset_flags:
 * @sfz_instrument_loader: the #AgsSFZInstrumentLoader
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 3.17.0
 */
void
ags_sfz_instrument_loader_unset_flags(AgsSFZInstrumentLoader *sfz_instrument_loader, guint flags)
{
  GRecMutex *sfz_instrument_loader_mutex;

  if(!AGS_IS_SFZ_INSTRUMENT_LOADER(sfz_instrument_loader)){
    return;
  }
  
  /* get sfz loader mutex */
  sfz_instrument_loader_mutex = AGS_SFZ_INSTRUMENT_LOADER_GET_OBJ_MUTEX(sfz_instrument_loader);

  /* unset flags */
  g_rec_mutex_lock(sfz_instrument_loader_mutex);

  sfz_instrument_loader->flags &= (~flags);
  
  g_rec_mutex_unlock(sfz_instrument_loader_mutex);
}

void*
ags_sfz_instrument_loader_run(void *ptr)
{
  AgsAudioContainerManager *audio_container_manager;

  AgsSFZInstrumentLoader *sfz_instrument_loader;

  GObject *output_soundcard;

  guint samplerate;
  guint buffer_length;
  guint format;
  
  GRecMutex *audio_container_manager_mutex;

  sfz_instrument_loader = AGS_SFZ_INSTRUMENT_LOADER(ptr);

  output_soundcard = NULL;
  
  g_object_get(sfz_instrument_loader->audio,
	       "output-soundcard", &output_soundcard,
	       NULL);

  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  buffer_length = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  
  if(output_soundcard != NULL){
    ags_soundcard_get_presets(AGS_SOUNDCARD(output_soundcard),
			      NULL,
			      &samplerate,
			      &buffer_length,
			      &format);
  }
  
  audio_container_manager = ags_audio_container_manager_get_instance();

  /* get audio container manager mutex */
  audio_container_manager_mutex = AGS_AUDIO_CONTAINER_MANAGER_GET_OBJ_MUTEX(audio_container_manager);
  
  g_rec_mutex_lock(audio_container_manager_mutex);

  sfz_instrument_loader->audio_container = ags_audio_container_manager_find_audio_container(audio_container_manager,
											    sfz_instrument_loader->filename);  
  g_object_ref(sfz_instrument_loader->audio_container);

  if(sfz_instrument_loader->audio_container == NULL){
    sfz_instrument_loader->audio_container = ags_audio_container_new(sfz_instrument_loader->filename,
								     NULL,
								     NULL,
								     NULL,
								     output_soundcard,
								     -1);
    g_object_ref(sfz_instrument_loader->audio_container);
    
    ags_audio_container_open(sfz_instrument_loader->audio_container);
  
    ags_audio_container_manager_add_audio_container(audio_container_manager,
						    sfz_instrument_loader->audio_container);
  }
  
  g_rec_mutex_unlock(audio_container_manager_mutex);

  if(sfz_instrument_loader->audio_container->sound_container != NULL){
    ags_sound_container_level_up(AGS_SOUND_CONTAINER(sfz_instrument_loader->audio_container->sound_container),
				 5);
    
    ags_sound_container_select_level_by_index(AGS_SOUND_CONTAINER(sfz_instrument_loader->audio_container->sound_container),
					      0);

    AGS_SFZ_FILE(sfz_instrument_loader->audio_container->sound_container)->nesting_level += 1;
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }
  
  if(ags_sfz_instrument_loader_test_flags(sfz_instrument_loader, AGS_SFZ_INSTRUMENT_LOADER_RUN_APPLY_INSTRUMENT)){
    AgsApplySFZInstrument *apply_sfz_instrument;

    AgsTaskLauncher *task_launcher;
    
    AgsApplicationContext *application_context;

    AgsSFZSynthUtil *template;
    
    application_context = ags_application_context_get_instance();

    task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

    template =
      sfz_instrument_loader->template = ags_sfz_synth_util_alloc();
    
    template->sfz_file = sfz_instrument_loader->audio_container;

    if(template->sfz_file != NULL){
      g_object_ref(template->sfz_file);
    }
    
    template->source = ags_stream_alloc(buffer_length,
					format);

    template->sample_buffer = ags_stream_alloc(buffer_length,
					       AGS_SOUNDCARD_DOUBLE);

    template->im_buffer = ags_stream_alloc(buffer_length,
					   AGS_SOUNDCARD_DOUBLE);

    /*  */
    ags_common_pitch_util_set_source(template->pitch_util,
				     template->pitch_type,
				     template->sample_buffer);

    ags_common_pitch_util_set_destination(template->pitch_util,
					  template->pitch_type,
					  template->im_buffer);
    
    ags_common_pitch_util_set_buffer_length(template->pitch_util,
					    template->pitch_type,
					    buffer_length);
    ags_common_pitch_util_set_format(template->pitch_util,
				     template->pitch_type,
				     AGS_SOUNDCARD_DOUBLE);
    ags_common_pitch_util_set_samplerate(template->pitch_util,
					 template->pitch_type,
					 samplerate);

    template->volume_util->source = template->im_buffer;
    
    template->volume_util->destination = template->im_buffer;

    template->volume_util->buffer_length = buffer_length;
    template->volume_util->format = AGS_SOUNDCARD_DOUBLE;

    template->volume_util->audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_DOUBLE;
    
    template->samplerate = samplerate;
    template->buffer_length = buffer_length;
    template->format = format;

    ags_sfz_synth_util_load_instrument(template);
    
    apply_sfz_instrument = ags_apply_sfz_instrument_new(template,
							sfz_instrument_loader->synth);
    
    ags_task_launcher_add_task(task_launcher,
			       apply_sfz_instrument);
  }
  
  ags_sfz_instrument_loader_set_flags(sfz_instrument_loader,
				      AGS_SFZ_INSTRUMENT_LOADER_HAS_COMPLETED);

  g_thread_exit(NULL);

  return(NULL);
}

/**
 * ags_sfz_instrument_loader_get_audio:
 * @sfz_instrument_loader: the #AgsSFZInstrumentLoader
 * 
 * Get audio of @sfz_instrument_loader.
 * 
 * Returns: (transfer full): the assigned #AgsAudio
 * 
 * Since: 3.17.0
 */
AgsAudio*
ags_sfz_instrument_loader_get_audio(AgsSFZInstrumentLoader *sfz_instrument_loader)
{
  AgsAudio *audio;

  if(!AGS_IS_SFZ_INSTRUMENT_LOADER(sfz_instrument_loader)){
    return(NULL);
  }

  g_object_get(sfz_instrument_loader,
	       "audio", &audio,
	       NULL);

  return(audio);
}

/**
 * ags_sfz_instrument_loader_set_audio:
 * @sfz_instrument_loader: the #AgsSFZInstrumentLoader
 * @audio: the #AgsAudio
 * 
 * Set audio of @sfz_instrument_loader.
 * 
 * Since: 3.17.0
 */
void
ags_sfz_instrument_loader_set_audio(AgsSFZInstrumentLoader *sfz_instrument_loader,
				    AgsAudio *audio)
{
  if(!AGS_IS_SFZ_INSTRUMENT_LOADER(sfz_instrument_loader)){
    return;
  }

  g_object_set(sfz_instrument_loader,
	       "audio", audio,
	       NULL);
}

/**
 * ags_sfz_instrument_loader_get_filename:
 * @sfz_instrument_loader: the #AgsSFZInstrumentLoader
 * 
 * Get filename of @sfz_instrument_loader.
 * 
 * Returns: (transfer full): the assigned filename
 * 
 * Since: 3.17.0
 */
gchar*
ags_sfz_instrument_loader_get_filename(AgsSFZInstrumentLoader *sfz_instrument_loader)
{
  gchar *filename;
  
  if(!AGS_IS_SFZ_INSTRUMENT_LOADER(sfz_instrument_loader)){
    return(NULL);
  }

  g_object_get(sfz_instrument_loader,
	       "filename", &filename,
	       NULL);

  return(filename);
}

/**
 * ags_sfz_instrument_loader_set_filename:
 * @sfz_instrument_loader: the #AgsSFZInstrumentLoader
 * @filename: the filename
 * 
 * Set filename of @sfz_instrument_loader.
 * 
 * Since: 3.17.0
 */
void
ags_sfz_instrument_loader_set_filename(AgsSFZInstrumentLoader *sfz_instrument_loader,
				       gchar *filename)
{
  if(!AGS_IS_SFZ_INSTRUMENT_LOADER(sfz_instrument_loader)){
    return;
  }

  g_object_set(sfz_instrument_loader,
	       "filename", filename,
	       NULL);
}

/**
 * ags_sfz_instrument_loader_get_audio_container:
 * @sfz_instrument_loader: the #AgsSFZInstrumentLoader
 * 
 * Get audio container of @sfz_instrument_loader.
 * 
 * Returns: the assigned #AgsAudioContainer
 * 
 * Since: 3.17.0
 */
AgsAudioContainer*
ags_sfz_instrument_loader_get_audio_container(AgsSFZInstrumentLoader *sfz_instrument_loader)
{
  AgsAudioContainer *audio_container;
  
  if(!AGS_IS_SFZ_INSTRUMENT_LOADER(sfz_instrument_loader)){
    return(NULL);
  }

  g_object_get(sfz_instrument_loader,
	       "audio-container", &audio_container,
	       NULL);

  return(audio_container);
}

/**
 * ags_sfz_instrument_loader_set_audio_container:
 * @sfz_instrument_loader: the #AgsSFZInstrumentLoader
 * @audio_container: the #AgsAudioContainer
 * 
 * Set audio container of @sfz_instrument_loader.
 * 
 * Since: 3.17.0
 */
void
ags_sfz_instrument_loader_set_audio_container(AgsSFZInstrumentLoader *sfz_instrument_loader,
					      AgsAudioContainer *audio_container)
{
  if(!AGS_IS_SFZ_INSTRUMENT_LOADER(sfz_instrument_loader)){
    return;
  }

  g_object_set(sfz_instrument_loader,
	       "audio-container", audio_container,
	       NULL);
}

/**
 * ags_sfz_instrument_loader_get_template:
 * @sfz_instrument_loader: the #AgsSFZInstrumentLoader
 * 
 * Get #AgsSFZSynthUtil-struct template of @sfz_instrument_loader.
 * 
 * Returns: the assigned #AgsSFZSynthUtil-struct
 * 
 * Since: 3.17.0
 */
AgsSFZSynthUtil*
ags_sfz_instrument_loader_get_template(AgsSFZInstrumentLoader *sfz_instrument_loader)
{
  AgsSFZSynthUtil *template;

  if(!AGS_IS_SFZ_INSTRUMENT_LOADER(sfz_instrument_loader)){
    return(NULL);
  }

  g_object_get(sfz_instrument_loader,
	       "template", &template,
	       NULL);

  return(template);
}

/**
 * ags_sfz_instrument_loader_set_template:
 * @sfz_instrument_loader: the #AgsSFZInstrumentLoader
 * @template: the #AgsSFZSynthUtil-struct
 * 
 * Set #AgsSFZSynthUtil-struct template of @sfz_instrument_loader.
 * 
 * Since: 3.17.0
 */
void
ags_sfz_instrument_loader_set_template(AgsSFZInstrumentLoader *sfz_instrument_loader,
				       AgsSFZSynthUtil *template)
{
  if(!AGS_IS_SFZ_INSTRUMENT_LOADER(sfz_instrument_loader)){
    return;
  }

  g_object_set(sfz_instrument_loader,
	       "template", template,
	       NULL);
}

/**
 * ags_sfz_instrument_loader_get_synth:
 * @sfz_instrument_loader: the #AgsSFZInstrumentLoader
 * 
 * Get #AgsSFZSynthUtil-struct synth of @sfz_instrument_loader.
 * 
 * Returns: the assigned #AgsSFZSynthUtil-struct
 * 
 * Since: 3.17.0
 */
AgsSFZSynthUtil*
ags_sfz_instrument_loader_get_synth(AgsSFZInstrumentLoader *sfz_instrument_loader)
{
  AgsSFZSynthUtil *synth;

  if(!AGS_IS_SFZ_INSTRUMENT_LOADER(sfz_instrument_loader)){
    return(NULL);
  }

  g_object_get(sfz_instrument_loader,
	       "synth", &synth,
	       NULL);

  return(synth);
}

/**
 * ags_sfz_instrument_loader_set_synth:
 * @sfz_instrument_loader: the #AgsSFZInstrumentLoader
 * @synth: the #AgsSFZSynthUtil-struct
 * 
 * Set #AgsSFZSynthUtil-struct synth of @sfz_instrument_loader.
 * 
 * Since: 3.17.0
 */
void
ags_sfz_instrument_loader_set_synth(AgsSFZInstrumentLoader *sfz_instrument_loader,
				    AgsSFZSynthUtil *synth)
{
  if(!AGS_IS_SFZ_INSTRUMENT_LOADER(sfz_instrument_loader)){
    return;
  }

  g_object_set(sfz_instrument_loader,
	       "synth", synth,
	       NULL);
}

/**
 * ags_sfz_instrument_loader_start:
 * @sfz_instrument_loader: the #AgsSFZInstrumentLoader
 * 
 * Start @sfz_instrument_loader.
 * 
 * Since: 3.17.0
 */
void
ags_sfz_instrument_loader_start(AgsSFZInstrumentLoader *sfz_instrument_loader)
{
  if(!AGS_IS_SFZ_INSTRUMENT_LOADER(sfz_instrument_loader)){
    return;
  }
  
  sfz_instrument_loader->thread = g_thread_new("Advanced Gtk+ Sequencer - SFZ MIDI locale loader",
					       ags_sfz_instrument_loader_run,
					       sfz_instrument_loader);
}

/**
 * ags_sfz_instrument_loader_new:
 * @audio: the #AgsAudio
 * @filename: the filename
 *
 * Create a new instance of #AgsSFZInstrumentLoader.
 *
 * Returns: the new #AgsSFZInstrumentLoader
 *
 * Since: 3.17.0
 */ 
AgsSFZInstrumentLoader*
ags_sfz_instrument_loader_new(AgsAudio *audio,
			      gchar *filename)
{
  AgsSFZInstrumentLoader *sfz_instrument_loader;

  sfz_instrument_loader = (AgsSFZInstrumentLoader *) g_object_new(AGS_TYPE_SFZ_INSTRUMENT_LOADER,
								  "audio", audio,
								  "filename", filename,
								  NULL);
  
  return(sfz_instrument_loader);
}
