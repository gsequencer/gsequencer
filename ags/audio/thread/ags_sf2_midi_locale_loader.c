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

#include <ags/audio/thread/ags_sf2_midi_locale_loader.h>

#include <ags/audio/ags_input.h>

#include <ags/audio/file/ags_audio_container.h>
#include <ags/audio/file/ags_audio_container_manager.h>
#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_ipatch.h>

#include <ags/audio/task/ags_apply_sf2_midi_locale.h>

#include <ags/i18n.h>

void ags_sf2_midi_locale_loader_class_init(AgsSF2MidiLocaleLoaderClass *sf2_midi_locale_loader);
void ags_sf2_midi_locale_loader_init(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader);
void ags_sf2_midi_locale_loader_set_property(GObject *gobject,
					     guint prop_id,
					     const GValue *value,
					     GParamSpec *param_spec);
void ags_sf2_midi_locale_loader_get_property(GObject *gobject,
					     guint prop_id,
					     GValue *value,
					     GParamSpec *param_spec);
void ags_sf2_midi_locale_loader_dispose(GObject *gobject);
void ags_sf2_midi_locale_loader_finalize(GObject *gobject);

void* ags_sf2_midi_locale_loader_run(void *ptr);

/**
 * SECTION:ags_sf2_midi_locale_loader
 * @short_description: load Soundfont2 MIDI locale asynchronously
 * @title: AgsSF2MidiLocaleLoader
 * @section_id:
 * @include: ags/plugin/ags_sf2_midi_locale_loader.h
 *
 * The #AgsSF2MidiLocaleLoader loads Soundfont2 audio data of MIDI locale asynchronously.
 */

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_FILENAME,
  PROP_BANK,
  PROP_PROGRAM,
  PROP_AUDIO_CONTAINER,
  PROP_TEMPLATE,
  PROP_SYNTH,
};

static gpointer ags_sf2_midi_locale_loader_parent_class = NULL;

GType
ags_sf2_midi_locale_loader_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sf2_midi_locale_loader = 0;

    static const GTypeInfo ags_sf2_midi_locale_loader_info = {
      sizeof(AgsSF2MidiLocaleLoaderClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sf2_midi_locale_loader_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSF2MidiLocaleLoader),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sf2_midi_locale_loader_init,
    };

    ags_type_sf2_midi_locale_loader = g_type_register_static(G_TYPE_OBJECT,
							     "AgsSF2MidiLocaleLoader",
							     &ags_sf2_midi_locale_loader_info,
							     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sf2_midi_locale_loader);
  }

  return g_define_type_id__volatile;
}

void
ags_sf2_midi_locale_loader_class_init(AgsSF2MidiLocaleLoaderClass *sf2_midi_locale_loader)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_sf2_midi_locale_loader_parent_class = g_type_class_peek_parent(sf2_midi_locale_loader);

  /* GObject */
  gobject = (GObjectClass *) sf2_midi_locale_loader;

  gobject->set_property = ags_sf2_midi_locale_loader_set_property;
  gobject->get_property = ags_sf2_midi_locale_loader_get_property;

  gobject->dispose = ags_sf2_midi_locale_loader_dispose;
  gobject->finalize = ags_sf2_midi_locale_loader_finalize;

  /* properties */
  /**
   * AgsSF2MidiLocaleLoader:audio:
   *
   * The assigned audio.
   * 
   * Since: 3.16.0
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
   * AgsSF2MidiLocaleLoader:filename:
   *
   * The filename to open.
   * 
   * Since: 3.16.0
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
   * AgsSF2MidiLocaleLoader:bank:
   *
   * The bank to open.
   * 
   * Since: 3.16.0
   */
  param_spec = g_param_spec_int("bank",
				i18n_pspec("bank"),
				i18n_pspec("The bank"),
				-1,
				128,
				-1,
				G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BANK,
				  param_spec);

  /**
   * AgsSF2MidiLocaleLoader:program:
   *
   * The program to open.
   * 
   * Since: 3.16.0
   */
  param_spec = g_param_spec_int("program",
				i18n_pspec("program"),
				i18n_pspec("The program"),
				-1,
				128,
				-1,
				G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PROGRAM,
				  param_spec);
  
  /**
   * AgsSF2MidiLocaleLoader:audio-container:
   *
   * The audio container opened.
   * 
   * Since: 3.16.0
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
   * AgsSF2MidiLocaleLoader:template:
   *
   * The assigned template.
   * 
   * Since: 3.16.0
   */
  param_spec = g_param_spec_pointer("template",
				    i18n_pspec("template"),
				    i18n_pspec("The template"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TEMPLATE,
				  param_spec);

  /**
   * AgsSF2MidiLocaleLoader:synth:
   *
   * The assigned synth.
   * 
   * Since: 3.16.0
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
ags_sf2_midi_locale_loader_init(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader)
{
  sf2_midi_locale_loader->flags = 0;

  /* add base plugin mutex */
  g_rec_mutex_init(&(sf2_midi_locale_loader->obj_mutex));

  /* fields */
  sf2_midi_locale_loader->thread = NULL;
  
  sf2_midi_locale_loader->audio = NULL;

  sf2_midi_locale_loader->filename = NULL;

  sf2_midi_locale_loader->bank = -1;
  sf2_midi_locale_loader->program = -1;
  
  sf2_midi_locale_loader->template = NULL;
  sf2_midi_locale_loader->synth = NULL;

  sf2_midi_locale_loader->audio_container = NULL;
}

void
ags_sf2_midi_locale_loader_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec)
{
  AgsSF2MidiLocaleLoader *sf2_midi_locale_loader;

  GRecMutex *sf2_midi_locale_loader_mutex;

  sf2_midi_locale_loader = AGS_SF2_MIDI_LOCALE_LOADER(gobject);

  /* get base plugin mutex */
  sf2_midi_locale_loader_mutex = AGS_SF2_MIDI_LOCALE_LOADER_GET_OBJ_MUTEX(sf2_midi_locale_loader);

  switch(prop_id){
  case PROP_AUDIO:
  {
    AgsAudio *audio;

    audio = g_value_get_object(value);
      
    g_rec_mutex_lock(sf2_midi_locale_loader_mutex);

    if(sf2_midi_locale_loader->audio == audio){
      g_rec_mutex_unlock(sf2_midi_locale_loader_mutex);

      return;
    }

    if(sf2_midi_locale_loader->audio != NULL){
      g_object_unref(sf2_midi_locale_loader->audio);
    }

    if(audio != NULL){
      g_object_ref(audio);
    }

    sf2_midi_locale_loader->audio = audio;
      
    g_rec_mutex_unlock(sf2_midi_locale_loader_mutex);
  }
  break;
  case PROP_FILENAME:
  {
    gchar *filename;

    filename = g_value_get_string(value);
      
    g_rec_mutex_lock(sf2_midi_locale_loader_mutex);

    if(sf2_midi_locale_loader->filename == filename){
      g_rec_mutex_unlock(sf2_midi_locale_loader_mutex);

      return;
    }

    if(sf2_midi_locale_loader->filename != NULL){
      g_free(sf2_midi_locale_loader->filename);
    }

    sf2_midi_locale_loader->filename = g_strdup(filename);
      
    g_rec_mutex_unlock(sf2_midi_locale_loader_mutex);
  }
  break;
  case PROP_BANK:
  {
    gint bank;

    bank = g_value_get_int(value);
      
    g_rec_mutex_lock(sf2_midi_locale_loader_mutex);

    sf2_midi_locale_loader->bank = bank;
      
    g_rec_mutex_unlock(sf2_midi_locale_loader_mutex);
  }
  break;
  case PROP_PROGRAM:
  {
    gint program;

    program = g_value_get_int(value);
      
    g_rec_mutex_lock(sf2_midi_locale_loader_mutex);

    sf2_midi_locale_loader->program = program;
      
    g_rec_mutex_unlock(sf2_midi_locale_loader_mutex);
  }
  break;
  case PROP_AUDIO_CONTAINER:
  {
    AgsAudioContainer *audio_container;

    audio_container = g_value_get_object(value);
      
    g_rec_mutex_lock(sf2_midi_locale_loader_mutex);

    if(sf2_midi_locale_loader->audio_container == audio_container){
      g_rec_mutex_unlock(sf2_midi_locale_loader_mutex);

      return;
    }

    if(sf2_midi_locale_loader->audio_container != NULL){
      g_object_unref(sf2_midi_locale_loader->audio_container);
    }

    if(audio_container != NULL){
      g_object_ref(audio_container);
    }

    sf2_midi_locale_loader->audio_container = audio_container;
      
    g_rec_mutex_unlock(sf2_midi_locale_loader_mutex);
  }
  break;
  case PROP_TEMPLATE:
  {
    AgsSF2SynthUtil *template;

    template = g_value_get_pointer(value);
      
    g_rec_mutex_lock(sf2_midi_locale_loader_mutex);

    sf2_midi_locale_loader->template = template;
      
    g_rec_mutex_unlock(sf2_midi_locale_loader_mutex);
  }
  break;
  case PROP_SYNTH:
  {
    AgsSF2SynthUtil *synth;

    synth = g_value_get_pointer(value);
      
    g_rec_mutex_lock(sf2_midi_locale_loader_mutex);

    sf2_midi_locale_loader->synth = synth;
      
    g_rec_mutex_unlock(sf2_midi_locale_loader_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}
    
void
ags_sf2_midi_locale_loader_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec)
{
  AgsSF2MidiLocaleLoader *sf2_midi_locale_loader;

  GRecMutex *sf2_midi_locale_loader_mutex;

  sf2_midi_locale_loader = AGS_SF2_MIDI_LOCALE_LOADER(gobject);

  /* get base plugin mutex */
  sf2_midi_locale_loader_mutex = AGS_SF2_MIDI_LOCALE_LOADER_GET_OBJ_MUTEX(sf2_midi_locale_loader);

  switch(prop_id){
  case PROP_AUDIO:
  {
    g_rec_mutex_lock(sf2_midi_locale_loader_mutex);
      
    g_value_set_object(value, sf2_midi_locale_loader->audio);

    g_rec_mutex_unlock(sf2_midi_locale_loader_mutex);
  }
  break;
  case PROP_FILENAME:
  {
    g_rec_mutex_lock(sf2_midi_locale_loader_mutex);
      
    g_value_set_string(value, sf2_midi_locale_loader->filename);

    g_rec_mutex_unlock(sf2_midi_locale_loader_mutex);
  }
  break;
  case PROP_BANK:
  {
    g_rec_mutex_lock(sf2_midi_locale_loader_mutex);
      
    g_value_set_int(value, sf2_midi_locale_loader->bank);

    g_rec_mutex_unlock(sf2_midi_locale_loader_mutex);
  }
  break;
  case PROP_PROGRAM:
  {
    g_rec_mutex_lock(sf2_midi_locale_loader_mutex);
      
    g_value_set_int(value, sf2_midi_locale_loader->program);

    g_rec_mutex_unlock(sf2_midi_locale_loader_mutex);
  }
  break;
  case PROP_AUDIO_CONTAINER:
  {
    g_rec_mutex_lock(sf2_midi_locale_loader_mutex);
      
    g_value_set_object(value, sf2_midi_locale_loader->audio_container);

    g_rec_mutex_unlock(sf2_midi_locale_loader_mutex);
  }
  break;
  case PROP_TEMPLATE:
  {
    g_rec_mutex_lock(sf2_midi_locale_loader_mutex);
      
    g_value_set_pointer(value, sf2_midi_locale_loader->template);

    g_rec_mutex_unlock(sf2_midi_locale_loader_mutex);
  }
  break;
  case PROP_SYNTH:
  {
    g_rec_mutex_lock(sf2_midi_locale_loader_mutex);
      
    g_value_set_pointer(value, sf2_midi_locale_loader->synth);

    g_rec_mutex_unlock(sf2_midi_locale_loader_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_sf2_midi_locale_loader_dispose(GObject *gobject)
{
  AgsSF2MidiLocaleLoader *sf2_midi_locale_loader;

  sf2_midi_locale_loader = AGS_SF2_MIDI_LOCALE_LOADER(gobject);

  if(sf2_midi_locale_loader->audio != NULL){
    g_object_unref(sf2_midi_locale_loader->audio);

    sf2_midi_locale_loader->audio = NULL;
  }

  if(sf2_midi_locale_loader->audio_container != NULL){
    g_object_unref(sf2_midi_locale_loader->audio_container);

    sf2_midi_locale_loader->audio_container = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_sf2_midi_locale_loader_parent_class)->dispose(gobject);
}

void
ags_sf2_midi_locale_loader_finalize(GObject *gobject)
{
  AgsSF2MidiLocaleLoader *sf2_midi_locale_loader;

  sf2_midi_locale_loader = AGS_SF2_MIDI_LOCALE_LOADER(gobject);
  
  /* destroy object mutex */
  if(sf2_midi_locale_loader->audio != NULL){
    g_object_unref(sf2_midi_locale_loader->audio);
  }

  g_free(sf2_midi_locale_loader->filename);

  if(sf2_midi_locale_loader->audio_container != NULL){
    g_object_unref(sf2_midi_locale_loader->audio_container);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_sf2_midi_locale_loader_parent_class)->finalize(gobject);
}

/**
 * ags_sf2_midi_locale_loader_test_flags:
 * @sf2_midi_locale_loader: the #AgsSF2MidiLocaleLoader
 * @flags: the flags
 * 
 * Test @flags to be set on @recall.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 3.16.0
 */
gboolean
ags_sf2_midi_locale_loader_test_flags(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader, guint flags)
{
  gboolean retval;
  
  GRecMutex *sf2_midi_locale_loader_mutex;

  if(!AGS_IS_SF2_MIDI_LOCALE_LOADER(sf2_midi_locale_loader)){
    return(FALSE);
  }
  
  /* get sf2 loader mutex */
  sf2_midi_locale_loader_mutex = AGS_SF2_MIDI_LOCALE_LOADER_GET_OBJ_MUTEX(sf2_midi_locale_loader);

  /* test flags */
  g_rec_mutex_lock(sf2_midi_locale_loader_mutex);

  retval = ((flags & (sf2_midi_locale_loader->flags)) != 0) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(sf2_midi_locale_loader_mutex);

  return(retval);
}

/**
 * ags_sf2_midi_locale_loader_set_flags:
 * @sf2_midi_locale_loader: the #AgsSF2MidiLocaleLoader
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 3.16.0
 */
void
ags_sf2_midi_locale_loader_set_flags(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader, guint flags)
{
  GRecMutex *sf2_midi_locale_loader_mutex;

  if(!AGS_IS_SF2_MIDI_LOCALE_LOADER(sf2_midi_locale_loader)){
    return;
  }
  
  /* get sf2 loader mutex */
  sf2_midi_locale_loader_mutex = AGS_SF2_MIDI_LOCALE_LOADER_GET_OBJ_MUTEX(sf2_midi_locale_loader);

  /* set flags */
  g_rec_mutex_lock(sf2_midi_locale_loader_mutex);

  sf2_midi_locale_loader->flags |= flags;
  
  g_rec_mutex_unlock(sf2_midi_locale_loader_mutex);
}

/**
 * ags_sf2_midi_locale_loader_unset_flags:
 * @sf2_midi_locale_loader: the #AgsSF2MidiLocaleLoader
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 3.16.0
 */
void
ags_sf2_midi_locale_loader_unset_flags(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader, guint flags)
{
  GRecMutex *sf2_midi_locale_loader_mutex;

  if(!AGS_IS_SF2_MIDI_LOCALE_LOADER(sf2_midi_locale_loader)){
    return;
  }
  
  /* get sf2 loader mutex */
  sf2_midi_locale_loader_mutex = AGS_SF2_MIDI_LOCALE_LOADER_GET_OBJ_MUTEX(sf2_midi_locale_loader);

  /* unset flags */
  g_rec_mutex_lock(sf2_midi_locale_loader_mutex);

  sf2_midi_locale_loader->flags &= (~flags);
  
  g_rec_mutex_unlock(sf2_midi_locale_loader_mutex);
}

void*
ags_sf2_midi_locale_loader_run(void *ptr)
{
  AgsAudioContainerManager *audio_container_manager;

  AgsSF2MidiLocaleLoader *sf2_midi_locale_loader;

  GObject *output_soundcard;
  
  GRecMutex *audio_container_manager_mutex;

  sf2_midi_locale_loader = AGS_SF2_MIDI_LOCALE_LOADER(ptr);

  output_soundcard = NULL;
  
  g_object_get(sf2_midi_locale_loader->audio,
	       "output-soundcard", &output_soundcard,
	       NULL);

  audio_container_manager = ags_audio_container_manager_get_instance();

  /* get audio container manager mutex */
  audio_container_manager_mutex = AGS_AUDIO_CONTAINER_MANAGER_GET_OBJ_MUTEX(audio_container_manager);
  
  g_rec_mutex_lock(audio_container_manager_mutex);

  sf2_midi_locale_loader->audio_container = ags_audio_container_manager_find_audio_container(audio_container_manager,
											     sf2_midi_locale_loader->filename);
  
  if(sf2_midi_locale_loader->audio_container == NULL){
    sf2_midi_locale_loader->audio_container = ags_audio_container_new(sf2_midi_locale_loader->filename,
								      NULL,
								      NULL,
								      NULL,
								      output_soundcard,
								      -1);
    ags_audio_container_open(sf2_midi_locale_loader->audio_container);
  
    ags_audio_container_manager_add_audio_container(audio_container_manager,
						    sf2_midi_locale_loader->audio_container);
  }
  
  g_rec_mutex_unlock(audio_container_manager_mutex);

  if(sf2_midi_locale_loader->audio_container->sound_container != NULL){
    ags_sound_container_level_up(AGS_SOUND_CONTAINER(sf2_midi_locale_loader->audio_container->sound_container),
				 5);
    
    ags_sound_container_select_level_by_index(AGS_SOUND_CONTAINER(sf2_midi_locale_loader->audio_container->sound_container),
					      0);

    AGS_IPATCH(sf2_midi_locale_loader->audio_container->sound_container)->nesting_level += 1;
  }

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }
  
  if(ags_sf2_midi_locale_loader_test_flags(sf2_midi_locale_loader, AGS_SF2_MIDI_LOCALE_LOADER_RUN_APPLY_MIDI_LOCALE)){
    AgsApplySF2MidiLocale *apply_sf2_midi_locale;

    AgsTaskLauncher *task_launcher;
    
    AgsApplicationContext *application_context;
    
    application_context = ags_application_context_get_instance();

    task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

    sf2_midi_locale_loader->template = ags_sf2_synth_util_alloc();

    sf2_midi_locale_loader->template->sf2_file = sf2_midi_locale_loader->audio_container;

    if(sf2_midi_locale_loader->template->sf2_file != NULL){
      g_object_ref(sf2_midi_locale_loader->template->sf2_file);
    }
    
    ags_sf2_synth_util_load_midi_locale(sf2_midi_locale_loader->template,
					sf2_midi_locale_loader->bank,
					sf2_midi_locale_loader->program);
    
    apply_sf2_midi_locale = ags_apply_sf2_midi_locale_new(sf2_midi_locale_loader->template,
							  sf2_midi_locale_loader->synth);
    
    ags_task_launcher_add_task(task_launcher,
			       apply_sf2_midi_locale);
  }
  
  ags_sf2_midi_locale_loader_set_flags(sf2_midi_locale_loader,
				       AGS_SF2_MIDI_LOCALE_LOADER_HAS_COMPLETED);

  g_thread_exit(NULL);

  return(NULL);
}

/**
 * ags_sf2_midi_locale_loader_get_audio:
 * @sf2_midi_locale_loader: the #AgsSF2MidiLocaleLoader
 * 
 * Get audio of @sf2_midi_locale_loader.
 * 
 * Returns: (transfer full): the assigned #AgsAudio
 * 
 * Since: 3.16.0
 */
AgsAudio*
ags_sf2_midi_locale_loader_get_audio(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader)
{
  AgsAudio *audio;

  if(!AGS_IS_SF2_MIDI_LOCALE_LOADER(sf2_midi_locale_loader)){
    return(NULL);
  }

  g_object_get(sf2_midi_locale_loader,
	       "audio", &audio,
	       NULL);

  return(audio);
}

/**
 * ags_sf2_midi_locale_loader_set_audio:
 * @sf2_midi_locale_loader: the #AgsSF2MidiLocaleLoader
 * @audio: the #AgsAudio
 * 
 * Set audio of @sf2_midi_locale_loader.
 * 
 * Since: 3.16.0
 */
void
ags_sf2_midi_locale_loader_set_audio(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader,
				     AgsAudio *audio)
{
  if(!AGS_IS_SF2_MIDI_LOCALE_LOADER(sf2_midi_locale_loader)){
    return;
  }

  g_object_set(sf2_midi_locale_loader,
	       "audio", audio,
	       NULL);
}

/**
 * ags_sf2_midi_locale_loader_get_filename:
 * @sf2_midi_locale_loader: the #AgsSF2MidiLocaleLoader
 * 
 * Get filename of @sf2_midi_locale_loader.
 * 
 * Returns: (transfer full): the assigned filename
 * 
 * Since: 3.16.0
 */
gchar*
ags_sf2_midi_locale_loader_get_filename(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader)
{
  gchar *filename;
  
  if(!AGS_IS_SF2_MIDI_LOCALE_LOADER(sf2_midi_locale_loader)){
    return(NULL);
  }

  g_object_get(sf2_midi_locale_loader,
	       "filename", &filename,
	       NULL);

  return(filename);
}

/**
 * ags_sf2_midi_locale_loader_set_filename:
 * @sf2_midi_locale_loader: the #AgsSF2MidiLocaleLoader
 * @filename: the filename
 * 
 * Set filename of @sf2_midi_locale_loader.
 * 
 * Since: 3.16.0
 */
void
ags_sf2_midi_locale_loader_set_filename(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader,
					gchar *filename)
{
  if(!AGS_IS_SF2_MIDI_LOCALE_LOADER(sf2_midi_locale_loader)){
    return;
  }

  g_object_set(sf2_midi_locale_loader,
	       "filename", filename,
	       NULL);
}

/**
 * ags_sf2_midi_locale_loader_get_bank:
 * @sf2_midi_locale_loader: the #AgsSF2MidiLocaleLoader
 * 
 * Get bank of @sf2_midi_locale_loader.
 * 
 * Returns: the assigned bank
 * 
 * Since: 3.16.0
 */
gint
ags_sf2_midi_locale_loader_get_bank(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader)
{
  gint bank;
  
  if(!AGS_IS_SF2_MIDI_LOCALE_LOADER(sf2_midi_locale_loader)){
    return(NULL);
  }

  g_object_get(sf2_midi_locale_loader,
	       "bank", &bank,
	       NULL);

  return(bank);
}

/**
 * ags_sf2_midi_locale_loader_set_bank:
 * @sf2_midi_locale_loader: the #AgsSF2MidiLocaleLoader
 * @bank: the bank
 * 
 * Set bank of @sf2_midi_locale_loader.
 * 
 * Since: 3.16.0
 */
void
ags_sf2_midi_locale_loader_set_bank(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader,
				    gint bank)
{
  if(!AGS_IS_SF2_MIDI_LOCALE_LOADER(sf2_midi_locale_loader)){
    return;
  }

  g_object_set(sf2_midi_locale_loader,
	       "bank", bank,
	       NULL);
}

/**
 * ags_sf2_midi_locale_loader_get_program:
 * @sf2_midi_locale_loader: the #AgsSF2MidiLocaleLoader
 * 
 * Get program of @sf2_midi_locale_loader.
 * 
 * Returns: the assigned program
 * 
 * Since: 3.16.0
 */
gint
ags_sf2_midi_locale_loader_get_program(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader)
{
  gint program;
  
  if(!AGS_IS_SF2_MIDI_LOCALE_LOADER(sf2_midi_locale_loader)){
    return(NULL);
  }

  g_object_get(sf2_midi_locale_loader,
	       "program", &program,
	       NULL);

  return(program);
}

/**
 * ags_sf2_midi_locale_loader_set_program:
 * @sf2_midi_locale_loader: the #AgsSF2MidiLocaleLoader
 * @program: the program
 * 
 * Set program of @sf2_midi_locale_loader.
 * 
 * Since: 3.16.0
 */
void
ags_sf2_midi_locale_loader_set_program(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader,
				       gint program)
{
  if(!AGS_IS_SF2_MIDI_LOCALE_LOADER(sf2_midi_locale_loader)){
    return;
  }

  g_object_set(sf2_midi_locale_loader,
	       "program", program,
	       NULL);
}

/**
 * ags_sf2_midi_locale_loader_get_audio_container:
 * @sf2_midi_locale_loader: the #AgsSF2MidiLocaleLoader
 * 
 * Get audio container of @sf2_midi_locale_loader.
 * 
 * Returns: the assigned #AgsAudioContainer
 * 
 * Since: 3.16.0
 */
AgsAudioContainer*
ags_sf2_midi_locale_loader_get_audio_container(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader)
{
  AgsAudioContainer *audio_container;
  
  if(!AGS_IS_SF2_MIDI_LOCALE_LOADER(sf2_midi_locale_loader)){
    return(NULL);
  }

  g_object_get(sf2_midi_locale_loader,
	       "audio-container", &audio_container,
	       NULL);

  return(audio_container);
}

/**
 * ags_sf2_midi_locale_loader_set_audio_container:
 * @sf2_midi_locale_loader: the #AgsSF2MidiLocaleLoader
 * @audio_container: the #AgsAudioContainer
 * 
 * Set audio container of @sf2_midi_locale_loader.
 * 
 * Since: 3.16.0
 */
void
ags_sf2_midi_locale_loader_set_audio_container(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader,
					       AgsAudioContainer *audio_container)
{
  if(!AGS_IS_SF2_MIDI_LOCALE_LOADER(sf2_midi_locale_loader)){
    return;
  }

  g_object_set(sf2_midi_locale_loader,
	       "audio-container", audio_container,
	       NULL);
}

/**
 * ags_sf2_midi_locale_loader_get_template:
 * @sf2_midi_locale_loader: the #AgsSF2MidiLocaleLoader
 * 
 * Get #AgsSF2SynthUtil-struct template of @sf2_midi_locale_loader.
 * 
 * Returns: the assigned #AgsSF2SynthUtil-struct
 * 
 * Since: 3.16.0
 */
AgsSF2SynthUtil*
ags_sf2_midi_locale_loader_get_template(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader)
{
  AgsSF2SynthUtil *template;

  if(!AGS_IS_SF2_MIDI_LOCALE_LOADER(sf2_midi_locale_loader)){
    return(NULL);
  }

  g_object_get(sf2_midi_locale_loader,
	       "template", &template,
	       NULL);

  return(template);
}

/**
 * ags_sf2_midi_locale_loader_set_template:
 * @sf2_midi_locale_loader: the #AgsSF2MidiLocaleLoader
 * @template: the #AgsSF2SynthUtil-struct
 * 
 * Set #AgsSF2SynthUtil-struct template of @sf2_midi_locale_loader.
 * 
 * Since: 3.16.0
 */
void
ags_sf2_midi_locale_loader_set_template(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader,
					AgsSF2SynthUtil *template)
{
  if(!AGS_IS_SF2_MIDI_LOCALE_LOADER(sf2_midi_locale_loader)){
    return;
  }

  g_object_set(sf2_midi_locale_loader,
	       "template", template,
	       NULL);
}

/**
 * ags_sf2_midi_locale_loader_get_synth:
 * @sf2_midi_locale_loader: the #AgsSF2MidiLocaleLoader
 * 
 * Get #AgsSF2SynthUtil-struct synth of @sf2_midi_locale_loader.
 * 
 * Returns: the assigned #AgsSF2SynthUtil-struct
 * 
 * Since: 3.16.0
 */
AgsSF2SynthUtil*
ags_sf2_midi_locale_loader_get_synth(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader)
{
  AgsSF2SynthUtil *synth;

  if(!AGS_IS_SF2_MIDI_LOCALE_LOADER(sf2_midi_locale_loader)){
    return(NULL);
  }

  g_object_get(sf2_midi_locale_loader,
	       "synth", &synth,
	       NULL);

  return(synth);
}

/**
 * ags_sf2_midi_locale_loader_set_synth:
 * @sf2_midi_locale_loader: the #AgsSF2MidiLocaleLoader
 * @synth: the #AgsSF2SynthUtil-struct
 * 
 * Set #AgsSF2SynthUtil-struct synth of @sf2_midi_locale_loader.
 * 
 * Since: 3.16.0
 */
void
ags_sf2_midi_locale_loader_set_synth(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader,
				     AgsSF2SynthUtil *synth)
{
  if(!AGS_IS_SF2_MIDI_LOCALE_LOADER(sf2_midi_locale_loader)){
    return;
  }

  g_object_set(sf2_midi_locale_loader,
	       "synth", synth,
	       NULL);
}

/**
 * ags_sf2_midi_locale_loader_start:
 * @sf2_midi_locale_loader: the #AgsSF2MidiLocaleLoader
 * 
 * Start @sf2_midi_locale_loader.
 * 
 * Since: 3.16.0
 */
void
ags_sf2_midi_locale_loader_start(AgsSF2MidiLocaleLoader *sf2_midi_locale_loader)
{
  if(!AGS_IS_SF2_MIDI_LOCALE_LOADER(sf2_midi_locale_loader)){
    return;
  }
  
  sf2_midi_locale_loader->thread = g_thread_new("Advanced Gtk+ Sequencer - SF2 MIDI locale loader",
						ags_sf2_midi_locale_loader_run,
						sf2_midi_locale_loader);
}

/**
 * ags_sf2_midi_locale_loader_new:
 * @audio: the #AgsAudio
 * @filename: the filename
 * @bank: the bank
 * @program: the program
 *
 * Create a new instance of #AgsSF2MidiLocaleLoader.
 *
 * Returns: the new #AgsSF2MidiLocaleLoader
 *
 * Since: 3.16.0
 */ 
AgsSF2MidiLocaleLoader*
ags_sf2_midi_locale_loader_new(AgsAudio *audio,
			       gchar *filename,
			       gint bank,
			       gint program)
{
  AgsSF2MidiLocaleLoader *sf2_midi_locale_loader;

  sf2_midi_locale_loader = (AgsSF2MidiLocaleLoader *) g_object_new(AGS_TYPE_SF2_MIDI_LOCALE_LOADER,
								   "audio", audio,
								   "filename", filename,
								   "bank", bank,
								   "program", program,
								   NULL);
  
  return(sf2_midi_locale_loader);
}
