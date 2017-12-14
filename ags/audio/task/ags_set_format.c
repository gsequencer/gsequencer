/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/audio/task/ags_set_format.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>

#include <ags/i18n.h>

void ags_set_format_class_init(AgsSetFormatClass *set_format);
void ags_set_format_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_set_format_init(AgsSetFormat *set_format);
void ags_set_format_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_set_format_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_set_format_connect(AgsConnectable *connectable);
void ags_set_format_disconnect(AgsConnectable *connectable);
void ags_set_format_dispose(GObject *gobject);
void ags_set_format_finalize(GObject *gobject);

void ags_set_format_launch(AgsTask *task);

void ags_set_format_audio_signal(AgsSetFormat *set_format, AgsAudioSignal *audio_signal);
void ags_set_format_recycling(AgsSetFormat *set_format, AgsRecycling *recycling);
void ags_set_format_channel(AgsSetFormat *set_format, AgsChannel *channel);
void ags_set_format_audio(AgsSetFormat *set_format, AgsAudio *audio);
void ags_set_format_soundcard(AgsSetFormat *set_format, GObject *soundcard);

/**
 * SECTION:ags_set_format
 * @short_description: resets format
 * @title: AgsSetFormat
 * @section_id:
 * @include: ags/audio/task/ags_set_format.h
 *
 * The #AgsSetFormat task resets format of scope.
 */

static gpointer ags_set_format_parent_class = NULL;
static AgsConnectableInterface *ags_set_format_parent_connectable_interface;

enum{
  PROP_0,
  PROP_SCOPE,
  PROP_FORMAT,
};

GType
ags_set_format_get_type()
{
  static GType ags_type_set_format = 0;

  if(!ags_type_set_format){
    static const GTypeInfo ags_set_format_info = {
      sizeof (AgsSetFormatClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_set_format_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSetFormat),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_set_format_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_set_format_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_set_format = g_type_register_static(AGS_TYPE_TASK,
						 "AgsSetFormat",
						 &ags_set_format_info,
						 0);
    
    g_type_add_interface_static(ags_type_set_format,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_set_format);
}

void
ags_set_format_class_init(AgsSetFormatClass *set_format)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_set_format_parent_class = g_type_class_peek_parent(set_format);

  /* gobject */
  gobject = (GObjectClass *) set_format;

  gobject->set_property = ags_set_format_set_property;
  gobject->get_property = ags_set_format_get_property;

  gobject->dispose = ags_set_format_dispose;
  gobject->finalize = ags_set_format_finalize;

  /* properties */
  /**
   * AgsSetFormat:scope:
   *
   * The assigned #GObject as scope.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("scope",
				   i18n_pspec("scope of set format"),
				   i18n_pspec("The scope of set format"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SCOPE,
				  param_spec);

  /**
   * AgsSetFormat:format:
   *
   * The format to apply to scope.
   * 
   * Since: 1.0.0
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

  /* task */
  task = (AgsTaskClass *) set_format;

  task->launch = ags_set_format_launch;
}

void
ags_set_format_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_set_format_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_set_format_connect;
  connectable->disconnect = ags_set_format_disconnect;
}

void
ags_set_format_init(AgsSetFormat *set_format)
{
  set_format->scope = NULL;
  set_format->format = 16;
}

void
ags_set_format_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsSetFormat *set_format;

  set_format = AGS_SET_FORMAT(gobject);

  switch(prop_id){
  case PROP_SCOPE:
    {
      GObject *scope;

      scope = (GObject *) g_value_get_object(value);

      if(set_format->scope == (GObject *) scope){
	return;
      }

      if(set_format->scope != NULL){
	g_object_unref(set_format->scope);
      }

      if(scope != NULL){
	g_object_ref(scope);
      }

      set_format->scope = (GObject *) scope;
    }
    break;
  case PROP_FORMAT:
    {
      set_format->format = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_set_format_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsSetFormat *set_format;

  set_format = AGS_SET_FORMAT(gobject);

  switch(prop_id){
  case PROP_SCOPE:
    {
      g_value_set_object(value, set_format->scope);
    }
    break;
  case PROP_FORMAT:
    {
      g_value_set_uint(value, set_format->format);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_set_format_connect(AgsConnectable *connectable)
{
  ags_set_format_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_set_format_disconnect(AgsConnectable *connectable)
{
  ags_set_format_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_set_format_dispose(GObject *gobject)
{
  AgsSetFormat *set_format;

  set_format = AGS_SET_FORMAT(gobject);

  if(set_format->scope != NULL){
    g_object_unref(set_format->scope);

    set_format->scope = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_set_format_parent_class)->dispose(gobject);
}

void
ags_set_format_finalize(GObject *gobject)
{
  AgsSetFormat *set_format;

  set_format = AGS_SET_FORMAT(gobject);

  if(set_format->scope != NULL){
    g_object_unref(set_format->scope);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_set_format_parent_class)->finalize(gobject);
}

void
ags_set_format_launch(AgsTask *task)
{
  AgsSetFormat *set_format;
  GObject *scope;

  set_format = AGS_SET_FORMAT(task);

  scope = set_format->scope;

  if(AGS_IS_SOUNDCARD(scope)){
    ags_set_format_soundcard(set_format, scope);
  }else if(AGS_IS_AUDIO(scope)){
    ags_set_format_audio(set_format, AGS_AUDIO(scope));
  }else if(AGS_IS_CHANNEL(scope)){
    ags_set_format_channel(set_format, AGS_CHANNEL(scope));
  }else if(AGS_IS_RECYCLING(scope)){
    ags_set_format_recycling(set_format, AGS_RECYCLING(scope));
  }else if(AGS_IS_AUDIO_SIGNAL(scope)){
    ags_set_format_audio_signal(set_format, AGS_AUDIO_SIGNAL(scope));
  }
}

void
ags_set_format_audio_signal(AgsSetFormat *set_format, AgsAudioSignal *audio_signal)
{
  g_object_set(audio_signal,
	       "format", set_format->format,
	       NULL);
}

void
ags_set_format_recycling(AgsSetFormat *set_format, AgsRecycling *recycling)
{
  GList *list;
  
  list = recycling->audio_signal;

  while(list != NULL){
    ags_set_format_audio_signal(set_format, AGS_AUDIO_SIGNAL(list->data));

    list = list->next;
  }
}

void
ags_set_format_channel(AgsSetFormat *set_format, AgsChannel *channel)
{
  g_object_set(channel,
	       "format", set_format->format,
	       NULL);
}

void
ags_set_format_audio(AgsSetFormat *set_format, AgsAudio *audio)
{
  AgsChannel *channel;

  /* AgsOutput */
  channel = audio->output;

  while(channel != NULL){
    ags_set_format_channel(set_format, channel);

    channel = channel->next;
  }

  /* AgsInput */
  channel = audio->input;

  while(channel != NULL){
    ags_set_format_channel(set_format, channel);

    channel = channel->next;
  }
}

void
ags_set_format_soundcard(AgsSetFormat *set_format, GObject *soundcard)
{
  GList *list;

  guint channels;
  guint samplerate;
  guint buffer_size;
  guint format;
  
  /*  */
  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    &channels,
			    &samplerate,
			    &buffer_size,
			    &format);

  ags_soundcard_set_presets(AGS_SOUNDCARD(soundcard),
			    channels,
			    samplerate,
			    buffer_size,
			    set_format->format);

  /* AgsAudio */
  list = ags_soundcard_get_audio(AGS_SOUNDCARD(soundcard));

  while(list != NULL){
    ags_set_format_audio(set_format, AGS_AUDIO(list->data));

    list = list->next;
  }
}

/**
 * ags_set_format_new:
 * @scope: the #GObject to reset
 * @format: the new format
 *
 * Creates an #AgsSetFormat.
 *
 * Returns: an new #AgsSetFormat.
 *
 * Since: 1.0.0
 */
AgsSetFormat*
ags_set_format_new(GObject *scope,
		   guint format)
{
  AgsSetFormat *set_format;

  set_format = (AgsSetFormat *) g_object_new(AGS_TYPE_SET_FORMAT,
					     "scope", scope,
					     "format", format,
					     NULL);

  return(set_format);
}
