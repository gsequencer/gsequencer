/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/audio/task/ags_set_samplerate.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>

#include <ags/i18n.h>

void ags_set_samplerate_class_init(AgsSetSamplerateClass *set_samplerate);
void ags_set_samplerate_init(AgsSetSamplerate *set_samplerate);
void ags_set_samplerate_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_set_samplerate_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_set_samplerate_dispose(GObject *gobject);
void ags_set_samplerate_finalize(GObject *gobject);

void ags_set_samplerate_launch(AgsTask *task);

void ags_set_samplerate_audio_signal(AgsSetSamplerate *set_samplerate, AgsAudioSignal *audio_signal);
void ags_set_samplerate_recycling(AgsSetSamplerate *set_samplerate, AgsRecycling *recycling);
void ags_set_samplerate_channel(AgsSetSamplerate *set_samplerate, AgsChannel *channel);
void ags_set_samplerate_audio(AgsSetSamplerate *set_samplerate, AgsAudio *audio);
void ags_set_samplerate_soundcard(AgsSetSamplerate *set_samplerate, GObject *soundcard);

/**
 * SECTION:ags_set_samplerate
 * @short_description: modify samplerate
 * @title: AgsSetSamplerate
 * @section_id:
 * @include: ags/audio/task/ags_set_samplerate.h
 *
 * The #AgsSetSamplerate task modifies samplerate of scope.
 */

static gpointer ags_set_samplerate_parent_class = NULL;

enum{
  PROP_0,
  PROP_SCOPE,
  PROP_SAMPLERATE,
};

GType
ags_set_samplerate_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_set_samplerate = 0;

    static const GTypeInfo ags_set_samplerate_info = {
      sizeof(AgsSetSamplerateClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_set_samplerate_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSetSamplerate),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_set_samplerate_init,
    };

    ags_type_set_samplerate = g_type_register_static(AGS_TYPE_TASK,
						     "AgsSetSamplerate",
						     &ags_set_samplerate_info,
						     0);

    g_once_init_leave(&g_define_type_id__static, ags_type_set_samplerate);
  }

  return(g_define_type_id__static);
}

void
ags_set_samplerate_class_init(AgsSetSamplerateClass *set_samplerate)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_set_samplerate_parent_class = g_type_class_peek_parent(set_samplerate);

  /* gobject */
  gobject = (GObjectClass *) set_samplerate;

  gobject->set_property = ags_set_samplerate_set_property;
  gobject->get_property = ags_set_samplerate_get_property;

  gobject->dispose = ags_set_samplerate_dispose;
  gobject->finalize = ags_set_samplerate_finalize;

  /* properties */
  /**
   * AgsSetSamplerate:scope:
   *
   * The assigned #GObject as scope.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("scope",
				   i18n_pspec("scope of set samplerate"),
				   i18n_pspec("The scope of set samplerate"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SCOPE,
				  param_spec);

  /**
   * AgsSetSamplerate:samplerate:
   *
   * The samplerate to apply to scope.
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

  /* task */
  task = (AgsTaskClass *) set_samplerate;

  task->launch = ags_set_samplerate_launch;
}

void
ags_set_samplerate_init(AgsSetSamplerate *set_samplerate)
{
  set_samplerate->scope = NULL;
  set_samplerate->samplerate = 44100;
}

void
ags_set_samplerate_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsSetSamplerate *set_samplerate;

  set_samplerate = AGS_SET_SAMPLERATE(gobject);

  switch(prop_id){
  case PROP_SCOPE:
    {
      GObject *scope;

      scope = (GObject *) g_value_get_object(value);

      if(set_samplerate->scope == (GObject *) scope){
	return;
      }

      if(set_samplerate->scope != NULL){
	g_object_unref(set_samplerate->scope);
      }

      if(scope != NULL){
	g_object_ref(scope);
      }

      set_samplerate->scope = (GObject *) scope;
    }
    break;
  case PROP_SAMPLERATE:
    {
      set_samplerate->samplerate = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_set_samplerate_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsSetSamplerate *set_samplerate;

  set_samplerate = AGS_SET_SAMPLERATE(gobject);

  switch(prop_id){
  case PROP_SCOPE:
    {
      g_value_set_object(value, set_samplerate->scope);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_value_set_uint(value, set_samplerate->samplerate);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_set_samplerate_dispose(GObject *gobject)
{
  AgsSetSamplerate *set_samplerate;

  set_samplerate = AGS_SET_SAMPLERATE(gobject);

  if(set_samplerate->scope != NULL){
    g_object_unref(set_samplerate->scope);

    set_samplerate->scope = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_set_samplerate_parent_class)->dispose(gobject);
}

void
ags_set_samplerate_finalize(GObject *gobject)
{
  AgsSetSamplerate *set_samplerate;

  set_samplerate = AGS_SET_SAMPLERATE(gobject);

  if(set_samplerate->scope != NULL){
    g_object_unref(set_samplerate->scope);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_set_samplerate_parent_class)->finalize(gobject);
}

void
ags_set_samplerate_launch(AgsTask *task)
{
  AgsSetSamplerate *set_samplerate;
  GObject *scope;

  set_samplerate = AGS_SET_SAMPLERATE(task);

  scope = set_samplerate->scope;

  if(AGS_IS_SOUNDCARD(scope)){
    ags_set_samplerate_soundcard(set_samplerate, scope);
  }else if(AGS_IS_AUDIO(scope)){
    ags_set_samplerate_audio(set_samplerate, AGS_AUDIO(scope));
  }else if(AGS_IS_CHANNEL(scope)){
    ags_set_samplerate_channel(set_samplerate, AGS_CHANNEL(scope));
  }else if(AGS_IS_RECYCLING(scope)){
    ags_set_samplerate_recycling(set_samplerate, AGS_RECYCLING(scope));
  }else if(AGS_IS_AUDIO_SIGNAL(scope)){
    ags_set_samplerate_audio_signal(set_samplerate, AGS_AUDIO_SIGNAL(scope));
  }
}

void
ags_set_samplerate_audio_signal(AgsSetSamplerate *set_samplerate, AgsAudioSignal *audio_signal)
{
  g_object_set(audio_signal,
	       "samplerate", set_samplerate->samplerate,
	       NULL);
}

void
ags_set_samplerate_recycling(AgsSetSamplerate *set_samplerate, AgsRecycling *recycling)
{
  g_object_set(recycling,
	       "samplerate", set_samplerate->samplerate,
	       NULL);
}

void
ags_set_samplerate_channel(AgsSetSamplerate *set_samplerate, AgsChannel *channel)
{
  g_object_set(channel,
	       "samplerate", set_samplerate->samplerate,
	       NULL);
}

void
ags_set_samplerate_audio(AgsSetSamplerate *set_samplerate, AgsAudio *audio)
{
  g_object_set(audio,
	       "samplerate", set_samplerate->samplerate,
	       NULL);
}

void
ags_set_samplerate_soundcard(AgsSetSamplerate *set_samplerate, GObject *soundcard)
{
  AgsThread *audio_loop;
  
  AgsApplicationContext *application_context;
  
  GObject *default_soundcard;

  GList *list_start, *list;

  gdouble thread_frequency;
  guint channels;
  guint samplerate;
  guint buffer_size;
  guint format;

  application_context = ags_application_context_get_instance();

  /* get main loop */
  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));
  
  /* set buffer size */
  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    &channels,
			    &samplerate,
			    &buffer_size,
			    &format);
    
  /* reset soundcards */
  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  if(soundcard == default_soundcard){
    /* reset soundcards if applied to first soundcard */
    ags_soundcard_set_presets(AGS_SOUNDCARD(soundcard),
			      channels,
			      set_samplerate->samplerate,
			      buffer_size,
			      format);

    list =
      list_start = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));


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
				  buffer_size * (target_samplerate / samplerate),
				  target_format);
      }

      list = list->next;
    }
  
    g_list_free_full(list_start,
		     g_object_unref);
    
    /* reset thread frequency */
    thread_frequency = set_samplerate->samplerate / buffer_size + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;

    ags_main_loop_change_frequency(AGS_MAIN_LOOP(audio_loop),
				   thread_frequency);
  }else{
    g_warning("buffer size can only adjusted of your very first soundcard");
  }

  if(default_soundcard != NULL){
    g_object_unref(default_soundcard);
  }
  
  g_object_unref(audio_loop);
}

/**
 * ags_set_samplerate_new:
 * @scope: the #AgsSoundcard to reset
 * @samplerate: the new samplerate
 *
 * Create a new instance of #AgsSetSamplerate.
 *
 * Returns: the new #AgsSetSamplerate
 *
 * Since: 3.0.0
 */
AgsSetSamplerate*
ags_set_samplerate_new(GObject *scope,
		       guint samplerate)
{
  AgsSetSamplerate *set_samplerate;

  set_samplerate = (AgsSetSamplerate *) g_object_new(AGS_TYPE_SET_SAMPLERATE,
						     "scope", scope,
						     "samplerate", samplerate,
						     NULL);

  return(set_samplerate);
}
