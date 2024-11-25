/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/audio/task/ags_apply_presets.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_synth_generator.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_synth_util.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>
#include <ags/audio/thread/ags_export_thread.h>
#include <ags/audio/thread/ags_audio_thread.h>
#include <ags/audio/thread/ags_channel_thread.h>

#include <ags/audio/file/ags_audio_file_link.h>
#include <ags/audio/file/ags_audio_file.h>

#include <ags/i18n.h>

void ags_apply_presets_class_init(AgsApplyPresetsClass *apply_presets);
void ags_apply_presets_init(AgsApplyPresets *apply_presets);
void ags_apply_presets_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_apply_presets_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_apply_presets_dispose(GObject *gobject);
void ags_apply_presets_finalize(GObject *gobject);

void ags_apply_presets_launch(AgsTask *task);

void ags_apply_presets_audio(AgsApplyPresets *apply_presets,
			     AgsAudio *audio);
void ags_apply_presets_channel(AgsApplyPresets *apply_presets,
			       AgsChannel *channel);
void ags_apply_presets_audio_signal(AgsApplyPresets *apply_presets,
				    AgsAudioSignal *audio_signal);

/**
 * SECTION:ags_apply_presets
 * @short_description: apply presets
 * @title: AgsApplyPresets
 * @section_id:
 * @include: ags/audio/task/ags_apply_presets.h
 *
 * The #AgsApplyPresets task apply the specified presets.
 */

enum{
  PROP_0,
  PROP_SCOPE,
  PROP_PCM_CHANNELS,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
};

static gpointer ags_apply_presets_parent_class = NULL;

GType
ags_apply_presets_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_apply_presets = 0;

    static const GTypeInfo ags_apply_presets_info = {
      sizeof(AgsApplyPresetsClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_apply_presets_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsApplyPresets),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_apply_presets_init,
    };

    ags_type_apply_presets = g_type_register_static(AGS_TYPE_TASK,
						    "AgsApplyPresets",
						    &ags_apply_presets_info,
						    0);

    g_once_init_leave(&g_define_type_id__static, ags_type_apply_presets);
  }

  return(g_define_type_id__static);
}

void
ags_apply_presets_class_init(AgsApplyPresetsClass *apply_presets)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;
  
  ags_apply_presets_parent_class = g_type_class_peek_parent(apply_presets);

  /* GObjectClass */
  gobject = (GObjectClass *) apply_presets;

  gobject->set_property = ags_apply_presets_set_property;
  gobject->get_property = ags_apply_presets_get_property;

  gobject->dispose = ags_apply_presets_dispose;
  gobject->finalize = ags_apply_presets_finalize;

  /* properties */
  /**
   * AgsApplyPresets:scope:
   *
   * The assigned #GObject
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("scope",
				   i18n_pspec("scope of apply presets"),
				   i18n_pspec("The scope of apply presets task"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SCOPE,
				  param_spec);

  /**
   * AgsApplyPresets:pcm-channels:
   *
   * The count of pcm-channels to apply.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("pcm-channels",
				 i18n_pspec("pcm channel count"),
				 i18n_pspec("The count of pcm channels"),
				 0,
				 65535,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PCM_CHANNELS,
				  param_spec);

  /**
   * AgsApplyPresets:samplerate:
   *
   * The count of samplerate to apply.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("samplerate"),
				 i18n_pspec("The samplerate to apply"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  
  /**
   * AgsApplyPresets:buffer-size:
   *
   * The count of buffer-size to apply.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("buffer-size",
				 i18n_pspec("buffer size"),
				 i18n_pspec("The buffer size to apply"),
				 0,
				 65535,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsApplyPresets:format:
   *
   * The count of format to apply.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("format",
				 i18n_pspec("format"),
				 i18n_pspec("The format to apply"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /* AgsTaskClass */
  task = (AgsTaskClass *) apply_presets;

  task->launch = ags_apply_presets_launch;
}

void
ags_apply_presets_init(AgsApplyPresets *apply_presets)
{
  apply_presets->scope = NULL;

  apply_presets->pcm_channels = 0;
  apply_presets->samplerate = 0;
  apply_presets->buffer_size = 0;
  apply_presets->format = 0;
}

void
ags_apply_presets_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsApplyPresets *apply_presets;

  apply_presets = AGS_APPLY_PRESETS(gobject);

  switch(prop_id){
  case PROP_SCOPE:
    {
      GObject *scope;

      scope = (GObject *) g_value_get_object(value);

      if(apply_presets->scope == (GObject *) scope){
	return;
      }

      if(apply_presets->scope != NULL){
	g_object_unref(apply_presets->scope);
      }

      if(scope != NULL){
	g_object_ref(scope);
      }

      apply_presets->scope = (GObject *) scope;
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      apply_presets->pcm_channels = g_value_get_uint(value);
    }
    break;
  case PROP_SAMPLERATE:
    {
      apply_presets->samplerate = g_value_get_uint(value);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      apply_presets->buffer_size = g_value_get_uint(value);
    }
    break;
  case PROP_FORMAT:
    {
      apply_presets->format = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_apply_presets_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsApplyPresets *apply_presets;

  apply_presets = AGS_APPLY_PRESETS(gobject);

  switch(prop_id){
  case PROP_SCOPE:
    {
      g_value_set_object(value, apply_presets->scope);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_value_set_uint(value, apply_presets->pcm_channels);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_value_set_uint(value, apply_presets->samplerate);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value, apply_presets->buffer_size);
    }
    break;
  case PROP_FORMAT:
    {
      g_value_set_uint(value, apply_presets->format);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_apply_presets_dispose(GObject *gobject)
{
  AgsApplyPresets *apply_presets;

  apply_presets = AGS_APPLY_PRESETS(gobject);

  if(apply_presets->scope != NULL){
    g_object_unref(apply_presets->scope);

    apply_presets->scope = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_apply_presets_parent_class)->dispose(gobject);
}

void
ags_apply_presets_finalize(GObject *gobject)
{
  AgsApplyPresets *apply_presets;

  apply_presets = AGS_APPLY_PRESETS(gobject);

  if(apply_presets->scope != NULL){
    g_object_unref(apply_presets->scope);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_apply_presets_parent_class)->finalize(gobject);
}

void
ags_apply_presets_launch(AgsTask *task)
{
  AgsApplyPresets *apply_presets;

  apply_presets = AGS_APPLY_PRESETS(task);
  
  if(AGS_IS_SOUNDCARD(apply_presets->scope)){
    ags_apply_presets_soundcard(apply_presets,
				apply_presets->scope);
  }else if(AGS_IS_AUDIO(apply_presets->scope)){
    ags_apply_presets_audio(apply_presets,
			    (AgsAudio *) apply_presets->scope);
  }else if(AGS_IS_CHANNEL(apply_presets->scope)){
    ags_apply_presets_channel(apply_presets,
			      (AgsChannel *) apply_presets->scope);
  }else if(AGS_IS_AUDIO_SIGNAL(apply_presets->scope)){
    ags_apply_presets_audio_signal(apply_presets,
				   (AgsAudioSignal *) apply_presets->scope);
  }else{
    if(apply_presets->scope != NULL){
      g_warning("ags_apply_presets_launch() - unsupported scope %s", G_OBJECT_TYPE_NAME(apply_presets->scope));
    }else{
      g_warning("ags_apply_presets_launch() - unsupported scope (null)0");
    }
  }
}

void
ags_apply_presets_soundcard(AgsApplyPresets *apply_presets,
			    GObject *soundcard)
{
  AgsAudio *audio;
  
  AgsThread *main_loop;
  AgsThread *export_thread;
  AgsThread *soundcard_thread;
  AgsThread *audio_thread;
  AgsThread *channel_thread;
  
  AgsApplicationContext *application_context;
  
  GList *list_start, *list;

  gdouble freq;
  guint channels;

  application_context = ags_application_context_get_instance();

  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));
  
  /* calculate thread frequency */
  freq = ceil((gdouble) apply_presets->samplerate / (gdouble) apply_presets->buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;

  /* reset soundcard */
  ags_soundcard_set_presets(AGS_SOUNDCARD(soundcard),
			    apply_presets->pcm_channels,
			    apply_presets->samplerate,
			    apply_presets->buffer_size,
			    apply_presets->format);

  /* reset audio loop frequency */
  if(main_loop != NULL){
    g_object_set(G_OBJECT(main_loop),
		 "frequency", freq,
		 NULL);
  }
  
  /* reset export thread frequency */
  export_thread = ags_thread_find_type(main_loop,
				       AGS_TYPE_EXPORT_THREAD);
  
  while(export_thread != NULL){
    if(AGS_IS_EXPORT_THREAD(export_thread)){
      g_object_set(export_thread,
		   "frequency", freq,
		   NULL);
    }
    
    /* iterate */
    export_thread = ags_atomic_pointer_get(&(export_thread->next));
  }

  /* reset soundcard thread frequency */
  soundcard_thread = ags_thread_find_type(main_loop,
					  AGS_TYPE_SOUNDCARD_THREAD);
  
  while(soundcard_thread != NULL){
    if(AGS_IS_SOUNDCARD_THREAD(export_thread)){
      g_object_set(soundcard_thread,
		   "frequency", freq,
		   NULL);
    }
    
    /* iterate */
    soundcard_thread = ags_atomic_pointer_get(&(soundcard_thread->next));
  }

  /* reset playback on soundcard */
  if(ags_soundcard_is_playing(AGS_SOUNDCARD(soundcard))){
    ags_soundcard_stop(AGS_SOUNDCARD(soundcard));
    ags_soundcard_play_init(AGS_SOUNDCARD(soundcard),
			    &(AGS_SOUNDCARD_THREAD(soundcard_thread)->error));
  }
  
  /* descend children */
  list =
    list_start = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));

  while(list != NULL){
    audio = AGS_AUDIO(list->data);

    /* apply presets to audio */
    ags_apply_presets_audio(apply_presets,
			    audio);

    /* iterate */
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);

  /* unref */
  if(main_loop != NULL){
    g_object_unref(main_loop);
  }
}

void
ags_apply_presets_audio(AgsApplyPresets *apply_presets,
			AgsAudio *audio)
{
  g_object_set(audio,
	       "samplerate", apply_presets->samplerate,
	       "buffer-size", apply_presets->buffer_size,
	       "format", apply_presets->format,
	       NULL);
}

void
ags_apply_presets_channel(AgsApplyPresets *apply_presets,
			  AgsChannel *channel)
{
  g_object_set(channel,
	       "samplerate", apply_presets->samplerate,
	       "buffer-size", apply_presets->buffer_size,
	       "format", apply_presets->format,
	       NULL);
}

void
ags_apply_presets_audio_signal(AgsApplyPresets *apply_presets,
			       AgsAudioSignal *audio_signal)
{
  g_object_set(audio_signal,
	       "samplerate", apply_presets->samplerate,
	       "buffer-size", apply_presets->buffer_size,
	       "format", apply_presets->format,
	       NULL);
}

/**
 * ags_apply_presets_new:
 * @scope: a #GObject
 * @pcm_channels: pcm channels
 * @samplerate: samplerate
 * @buffer_size: buffer size
 * @format: format
 *
 * Creates a new instance of #AgsApplyPresets.
 *
 * Returns: the new #AgsApplyPresets.
 *
 * Since: 3.0.0
 */
AgsApplyPresets*
ags_apply_presets_new(GObject *scope,
		      guint pcm_channels,
		      guint samplerate,
		      guint buffer_size,
		      guint format)
{
  AgsApplyPresets *apply_presets;

  apply_presets = (AgsApplyPresets *) g_object_new(AGS_TYPE_APPLY_PRESETS,
						   "scope", scope,
						   "pcm-channels", pcm_channels,
						   "samplerate", samplerate,
						   "buffer-size", buffer_size,
						   "format", format,
						   NULL);

  return(apply_presets);
}
