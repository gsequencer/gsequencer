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

#include <ags/audio/task/ags_set_buffer_size.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>

#include <ags/i18n.h>

void ags_set_buffer_size_class_init(AgsSetBufferSizeClass *set_buffer_size);
void ags_set_buffer_size_init(AgsSetBufferSize *set_buffer_size);
void ags_set_buffer_size_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_set_buffer_size_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_set_buffer_size_dispose(GObject *gobject);
void ags_set_buffer_size_finalize(GObject *gobject);

void ags_set_buffer_size_launch(AgsTask *task);

void ags_set_buffer_size_audio_signal(AgsSetBufferSize *set_buffer_size, AgsAudioSignal *audio_signal);
void ags_set_buffer_size_recycling(AgsSetBufferSize *set_buffer_size, AgsRecycling *recycling);
void ags_set_buffer_size_channel(AgsSetBufferSize *set_buffer_size, AgsChannel *channel);
void ags_set_buffer_size_audio(AgsSetBufferSize *set_buffer_size, AgsAudio *audio);
void ags_set_buffer_size_soundcard(AgsSetBufferSize *set_buffer_size, GObject *soundcard);

/**
 * SECTION:ags_set_buffer_size
 * @short_description: adjust buffer size
 * @title: AgsSetBufferSize
 * @section_id:
 * @include: ags/audio/task/ags_set_buffer_size.h
 *
 * The #AgsSetBufferSize task adjusts buffer size of scope.
 */

enum{
  PROP_0,
  PROP_SCOPE,
  PROP_BUFFER_SIZE,
};

static gpointer ags_set_buffer_size_parent_class = NULL;

GType
ags_set_buffer_size_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_set_buffer_size;

    static const GTypeInfo ags_set_buffer_size_info = {
      sizeof(AgsSetBufferSizeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_set_buffer_size_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSetBufferSize),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_set_buffer_size_init,
    };

    ags_type_set_buffer_size = g_type_register_static(AGS_TYPE_TASK,
						      "AgsSetBufferSize",
						      &ags_set_buffer_size_info,
						      0);
  }
  
  return(ags_type_set_buffer_size);
}

void
ags_set_buffer_size_class_init(AgsSetBufferSizeClass *set_buffer_size)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_set_buffer_size_parent_class = g_type_class_peek_parent(set_buffer_size);

  /* gobject */
  gobject = (GObjectClass *) set_buffer_size;

  gobject->set_property = ags_set_buffer_size_set_property;
  gobject->get_property = ags_set_buffer_size_get_property;

  gobject->dispose = ags_set_buffer_size_dispose;
  gobject->finalize = ags_set_buffer_size_finalize;

  /* properties */
  /**
   * AgsSetBufferSize:scope:
   *
   * The assigned #GObject as scope.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("scope",
				   i18n_pspec("scope of set buffer size"),
				   i18n_pspec("The scope of set buffer size"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SCOPE,
				  param_spec);

  /**
   * AgsSetBufferSize:buffer-size:
   *
   * The buffer size to apply to scope.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("buffer-size",
				 i18n_pspec("buffer size"),
				 i18n_pspec("The buffer size to apply"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) set_buffer_size;

  task->launch = ags_set_buffer_size_launch;
}

void
ags_set_buffer_size_init(AgsSetBufferSize *set_buffer_size)
{
  set_buffer_size->scope = NULL;
  set_buffer_size->buffer_size = 128;
}

void
ags_set_buffer_size_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsSetBufferSize *set_buffer_size;

  set_buffer_size = AGS_SET_BUFFER_SIZE(gobject);

  switch(prop_id){
  case PROP_SCOPE:
    {
      GObject *scope;

      scope = (GObject *) g_value_get_object(value);

      if(set_buffer_size->scope == (GObject *) scope){
	return;
      }

      if(set_buffer_size->scope != NULL){
	g_object_unref(set_buffer_size->scope);
      }

      if(scope != NULL){
	g_object_ref(scope);
      }

      set_buffer_size->scope = (GObject *) scope;
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      set_buffer_size->buffer_size = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_set_buffer_size_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsSetBufferSize *set_buffer_size;

  set_buffer_size = AGS_SET_BUFFER_SIZE(gobject);

  switch(prop_id){
  case PROP_SCOPE:
    {
      g_value_set_object(value, set_buffer_size->scope);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value, set_buffer_size->buffer_size);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_set_buffer_size_dispose(GObject *gobject)
{
  AgsSetBufferSize *set_buffer_size;

  set_buffer_size = AGS_SET_BUFFER_SIZE(gobject);

  if(set_buffer_size->scope != NULL){
    g_object_unref(set_buffer_size->scope);

    set_buffer_size->scope = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_set_buffer_size_parent_class)->dispose(gobject);
}

void
ags_set_buffer_size_finalize(GObject *gobject)
{
  AgsSetBufferSize *set_buffer_size;

  set_buffer_size = AGS_SET_BUFFER_SIZE(gobject);

  if(set_buffer_size->scope != NULL){
    g_object_unref(set_buffer_size->scope);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_set_buffer_size_parent_class)->finalize(gobject);
}

void
ags_set_buffer_size_launch(AgsTask *task)
{
  AgsSetBufferSize *set_buffer_size;
  GObject *scope;

  set_buffer_size = AGS_SET_BUFFER_SIZE(task);

  scope = set_buffer_size->scope;

  if(AGS_IS_SOUNDCARD(scope)){
    ags_set_buffer_size_soundcard(set_buffer_size, scope);
  }else if(AGS_IS_AUDIO(scope)){
    ags_set_buffer_size_audio(set_buffer_size, AGS_AUDIO(scope));
  }else if(AGS_IS_CHANNEL(scope)){
    ags_set_buffer_size_channel(set_buffer_size, AGS_CHANNEL(scope));
  }else if(AGS_IS_RECYCLING(scope)){
    ags_set_buffer_size_recycling(set_buffer_size, AGS_RECYCLING(scope));
  }else if(AGS_IS_AUDIO_SIGNAL(scope)){
    ags_set_buffer_size_audio_signal(set_buffer_size, AGS_AUDIO_SIGNAL(scope));
  }
}

void
ags_set_buffer_size_audio_signal(AgsSetBufferSize *set_buffer_size, AgsAudioSignal *audio_signal)
{
  g_object_set(audio_signal,
	       "buffer-size", set_buffer_size->buffer_size,
	       NULL);
}

void
ags_set_buffer_size_recycling(AgsSetBufferSize *set_buffer_size, AgsRecycling *recycling)
{
  g_object_set(recycling,
	       "buffer-size", set_buffer_size->buffer_size,
	       NULL);
}

void
ags_set_buffer_size_channel(AgsSetBufferSize *set_buffer_size, AgsChannel *channel)
{
  g_object_set(channel,
	       "buffer-size", set_buffer_size->buffer_size,
	       NULL);
}

void
ags_set_buffer_size_audio(AgsSetBufferSize *set_buffer_size, AgsAudio *audio)
{
  g_object_set(audio,
	       "buffer-size", set_buffer_size->buffer_size,
	       NULL);
}

void
ags_set_buffer_size_soundcard(AgsSetBufferSize *set_buffer_size, GObject *soundcard)
{
  AgsThread *audio_loop;
  
  AgsApplicationContext *application_context;
  
  GList *list_start, *list;

  gdouble thread_frequency;
  guint channels;
  guint samplerate;
  guint buffer_size;
  guint format;

  application_context = ags_soundcard_get_application_context(AGS_SOUNDCARD(soundcard));

  /* get main loop */
  g_object_get(application_context,
	       "main-loop", &audio_loop,
	       NULL);
  
  /* set buffer size */
  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    &channels,
			    &samplerate,
			    &buffer_size,
			    &format);
    
  /* reset soundcards */
  list =
    list_start = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

  if(soundcard == list->data){
    /* reset soundcards if applied to first soundcard */
    ags_soundcard_set_presets(AGS_SOUNDCARD(soundcard),
			      channels,
			      samplerate,
			      set_buffer_size->buffer_size,
			      format);

    while(list != NULL){
      if(list->data != soundcard){
	guint target_channels;
	guint target_samplerate;
	guint target_buffer_size;
	guint target_format;

	/* get soundcard mutex */
	ags_soundcard_get_presets(AGS_SOUNDCARD(list->data),
				  &target_channels,
				  &target_samplerate,
				  &target_buffer_size,
				  &target_format);
	
	ags_soundcard_set_presets(AGS_SOUNDCARD(soundcard),
				  target_channels,
				  target_samplerate,
				  set_buffer_size->buffer_size * (target_samplerate / samplerate),
				  target_format);
      }

      list = list->next;
    }
    
    /* reset thread frequency */
    thread_frequency = samplerate / set_buffer_size->buffer_size + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;

    ags_main_loop_change_frequency(AGS_MAIN_LOOP(audio_loop),
				   thread_frequency);
  }else{
    g_warning("buffer size can only adjusted of your very first soundcard");
  }
  
  g_list_free(list_start);
}

/**
 * ags_set_buffer_size_new:
 * @scope: the #GObject to reset
 * @buffer_size: the new buffer size
 *
 * Create a new instance of #AgsSetBufferSize.
 *
 * Returns: the new #AgsSetBufferSize
 *
 * Since: 2.0.0
 */
AgsSetBufferSize*
ags_set_buffer_size_new(GObject *scope,
			guint buffer_size)
{
  AgsSetBufferSize *set_buffer_size;

  set_buffer_size = (AgsSetBufferSize *) g_object_new(AGS_TYPE_SET_BUFFER_SIZE,
						      "scope", scope,
						      "buffer-size", buffer_size,
						      NULL);

  return(set_buffer_size);
}
