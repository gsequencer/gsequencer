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

#include <ags/audio/task/ags_set_samplerate.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>

void ags_set_samplerate_class_init(AgsSetSamplerateClass *set_samplerate);
void ags_set_samplerate_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_set_samplerate_init(AgsSetSamplerate *set_samplerate);
void ags_set_samplerate_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_set_samplerate_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_set_samplerate_connect(AgsConnectable *connectable);
void ags_set_samplerate_disconnect(AgsConnectable *connectable);
void ags_set_samplerate_finalize(GObject *gobject);

void ags_set_samplerate_launch(AgsTask *task);

void ags_set_samplerate_audio_signal(AgsSetSamplerate *set_samplerate, AgsAudioSignal *audio_signal);
void ags_set_samplerate_recycling(AgsSetSamplerate *set_samplerate, AgsRecycling *recycling);
void ags_set_samplerate_channel(AgsSetSamplerate *set_samplerate, AgsChannel *channel);
void ags_set_samplerate_audio(AgsSetSamplerate *set_samplerate, AgsAudio *audio);
void ags_set_samplerate_soundcard(AgsSetSamplerate *set_samplerate, GObject *soundcard);

/**
 * SECTION:ags_set_samplerate
 * @short_description: modifies samplerate
 * @title: AgsSetSamplerate
 * @section_id:
 * @include: ags/audio/task/ags_set_samplerate.h
 *
 * The #AgsSetSamplerate task modifies samplerate of #AgsSoundcard.
 */

static gpointer ags_set_samplerate_parent_class = NULL;
static AgsConnectableInterface *ags_set_samplerate_parent_connectable_interface;

enum{
  PROP_0,
  PROP_SCOPE,
  PROP_SAMPLERATE,
};

GType
ags_set_samplerate_get_type()
{
  static GType ags_type_set_samplerate = 0;

  if(!ags_type_set_samplerate){
    static const GTypeInfo ags_set_samplerate_info = {
      sizeof (AgsSetSamplerateClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_set_samplerate_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSetSamplerate),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_set_samplerate_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_set_samplerate_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_set_samplerate = g_type_register_static(AGS_TYPE_TASK,
						     "AgsSetSamplerate\0",
						     &ags_set_samplerate_info,
						     0);
    
    g_type_add_interface_static(ags_type_set_samplerate,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_set_samplerate);
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

  gobject->finalize = ags_set_samplerate_finalize;

  /* properties */
  /**
   * AgsSetSamplerate:scope:
   *
   * The assigned #GObject as scope.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("scope\0",
				   "scope of set samplerate\0",
				   "The scope of set samplerate\0",
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
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("samplerate\0",
				 "samplerate\0",
				 "The samplerate to apply\0",
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
ags_set_samplerate_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_set_samplerate_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_set_samplerate_connect;
  connectable->disconnect = ags_set_samplerate_disconnect;
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
ags_set_samplerate_connect(AgsConnectable *connectable)
{
  ags_set_samplerate_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_set_samplerate_disconnect(AgsConnectable *connectable)
{
  ags_set_samplerate_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_set_samplerate_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_set_samplerate_parent_class)->finalize(gobject);

  /* empty */
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
  ags_audio_signal_set_samplerate(audio_signal,
				  set_samplerate->samplerate);
}

void
ags_set_samplerate_recycling(AgsSetSamplerate *set_samplerate, AgsRecycling *recycling)
{
  GList *list;
  
  list = recycling->audio_signal;

  while(list != NULL){
    ags_set_samplerate_audio_signal(set_samplerate, AGS_AUDIO_SIGNAL(list->data));

    list = list->next;
  }
}

void
ags_set_samplerate_channel(AgsSetSamplerate *set_samplerate, AgsChannel *channel)
{
  GObject *soundcard;

  guint samplerate, buffer_size;

  soundcard = channel->soundcard;
  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    NULL,
			    &samplerate,
			    &buffer_size,
			    NULL);
  
  g_object_set(channel,
	       "samplerate\0", samplerate,
	       "buffer-size\0", buffer_size,
	       NULL);
}

void
ags_set_samplerate_audio(AgsSetSamplerate *set_samplerate, AgsAudio *audio)
{
  AgsChannel *channel;

  /* AgsOutput */
  channel = audio->output;

  while(channel != NULL){
    ags_set_samplerate_channel(set_samplerate, channel);

    channel = channel->next;
  }

  /* AgsInput */
  channel = audio->input;

  while(channel != NULL){
    ags_set_samplerate_channel(set_samplerate, channel);

    channel = channel->next;
  }
}

void
ags_set_samplerate_soundcard(AgsSetSamplerate *set_samplerate, GObject *soundcard)
{
  AgsApplicationContext *application_context;
  
  GList *list;

  guint channels;
  guint samplerate;
  guint buffer_size;
  guint format;

  application_context = ags_soundcard_get_application_context(AGS_SOUNDCARD(soundcard));

  /*  */
  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    &channels,
			    &samplerate,
			    &buffer_size,
			    &format);
  
  ags_soundcard_set_presets(AGS_SOUNDCARD(soundcard),
			    channels,
			    set_samplerate->samplerate,
			    buffer_size,
			    format);

  /* reset soundcards */
  list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

  while(list != NULL){
    if(list->data != soundcard){
      guint target_channels;
      guint target_samplerate;
      guint target_buffer_size;
      guint target_format;

      ags_soundcard_get_presets(AGS_SOUNDCARD(list->data),
				&target_channels,
				&target_samplerate,
				&target_buffer_size,
				&target_format);
      

      ags_soundcard_set_presets(AGS_SOUNDCARD(soundcard),
				target_channels,
				target_samplerate,
				buffer_size * (target_samplerate / set_samplerate->samplerate),
				target_format);
    }

    list = list->next;
  }

  /* AgsAudio */
  list = ags_soundcard_get_audio(AGS_SOUNDCARD(soundcard));

  while(list != NULL){
    ags_set_samplerate_audio(set_samplerate, AGS_AUDIO(list->data));

    list = list->next;
  }
}

/**
 * ags_set_samplerate_new:
 * @scope: the #AgsSoundcard to reset
 * @samplerate: the new samplerate
 *
 * Creates an #AgsSetSamplerate.
 *
 * Returns: an new #AgsSetSamplerate.
 *
 * Since: 0.4
 */
AgsSetSamplerate*
ags_set_samplerate_new(GObject *scope,
		       guint samplerate)
{
  AgsSetSamplerate *set_samplerate;

  set_samplerate = (AgsSetSamplerate *) g_object_new(AGS_TYPE_SET_SAMPLERATE,
						     NULL);

  set_samplerate->scope = scope;
  set_samplerate->samplerate = samplerate;

  return(set_samplerate);
}
