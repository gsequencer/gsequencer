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

#include <ags/audio/task/ags_append_audio_threaded.h>

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/i18n.h>

void ags_append_audio_threaded_class_init(AgsAppendAudioThreadedClass *append_audio_threaded);
void ags_append_audio_threaded_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_append_audio_threaded_init(AgsAppendAudioThreaded *append_audio_threaded);
void ags_append_audio_threaded_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec);
void ags_append_audio_threaded_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec);
void ags_append_audio_threaded_connect(AgsConnectable *connectable);
void ags_append_audio_threaded_disconnect(AgsConnectable *connectable);
void ags_append_audio_threaded_dispose(GObject *gobject);
void ags_append_audio_threaded_finalize(GObject *gobject);

void ags_append_audio_threaded_launch(AgsTask *task);

enum{
  PROP_0,
  PROP_AUDIO_LOOP,
  PROP_PLAYBACK,
};

/**
 * SECTION:ags_append_audio_threaded
 * @short_description: append audio object to audio loop
 * @title: AgsAppendAudio
 * @section_id:
 * @include: ags/audio/task/ags_append_audio_threaded.h
 *
 * The #AgsAppendAudio task appends #AgsAudio to #AgsAudioLoop doing super-threaded
 * audio processing.
 */

static gpointer ags_append_audio_threaded_parent_class = NULL;
static AgsConnectableInterface *ags_append_audio_threaded_parent_connectable_interface;

GType
ags_append_audio_threaded_get_type()
{
  static GType ags_type_append_audio_threaded = 0;

  if(!ags_type_append_audio_threaded){
    static const GTypeInfo ags_append_audio_threaded_info = {
      sizeof (AgsAppendAudioThreadedClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_append_audio_threaded_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAppendAudioThreaded),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_append_audio_threaded_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_append_audio_threaded_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_append_audio_threaded = g_type_register_static(AGS_TYPE_TASK,
							    "AgsAppendAudioThreaded",
							    &ags_append_audio_threaded_info,
							    0);
    
    g_type_add_interface_static(ags_type_append_audio_threaded,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_append_audio_threaded);
}

void
ags_append_audio_threaded_class_init(AgsAppendAudioThreadedClass *append_audio_threaded)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;
  
  ags_append_audio_threaded_parent_class = g_type_class_peek_parent(append_audio_threaded);

  /* gobject */
  gobject = (GObjectClass *) append_audio_threaded;

  gobject->set_property = ags_append_audio_threaded_set_property;
  gobject->get_property = ags_append_audio_threaded_get_property;

  gobject->dispose = ags_append_audio_threaded_dispose;
  gobject->finalize = ags_append_audio_threaded_finalize;

  /* properties */
  /**
   * AgsAppendAudioThreaded:audio-loop:
   *
   * The assigned #AgsAudioLoop
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("audio-loop",
				   i18n_pspec("audio loop of append audio"),
				   i18n_pspec("The audio loop of append audio threaded task"),
				   AGS_TYPE_AUDIO_LOOP,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_LOOP,
				  param_spec);

  /**
   * AgsAppendAudioThreaded:playback:
   *
   * The assigned #AgsPlayback
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("playback",
				   i18n_pspec("playback of append recall"),
				   i18n_pspec("The playback of append audio threaded task"),
				   AGS_TYPE_PLAYBACK,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAYBACK,
				  param_spec);
  

  /* task */
  task = (AgsTaskClass *) append_audio_threaded;

  task->launch = ags_append_audio_threaded_launch;
}

void
ags_append_audio_threaded_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_append_audio_threaded_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_append_audio_threaded_connect;
  connectable->disconnect = ags_append_audio_threaded_disconnect;
}

void
ags_append_audio_threaded_init(AgsAppendAudioThreaded *append_audio_threaded)
{
  append_audio_threaded->audio_loop = NULL;
  append_audio_threaded->playback = NULL;
}

void
ags_append_audio_threaded_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec)
{
  AgsAppendAudioThreaded *append_audio_threaded;

  append_audio_threaded = AGS_APPEND_AUDIO_THREADED(gobject);

  switch(prop_id){
  case PROP_AUDIO_LOOP:
    {
      AgsAudioLoop *audio_loop;

      audio_loop = (AgsAudioLoop *) g_value_get_object(value);

      if(append_audio_threaded->audio_loop == (GObject *) audio_loop){
	return;
      }

      if(append_audio_threaded->audio_loop != NULL){
	g_object_unref(append_audio_threaded->audio_loop);
      }

      if(audio_loop != NULL){
	g_object_ref(audio_loop);
      }

      append_audio_threaded->audio_loop = (GObject *) audio_loop;
    }
    break;
  case PROP_PLAYBACK:
    {
      AgsPlayback *playback;

      playback = (AgsPlayback *) g_value_get_object(value);

      if(append_audio_threaded->playback == (GObject *) playback){
	return;
      }

      if(append_audio_threaded->playback != NULL){
	g_object_unref(append_audio_threaded->playback);
      }

      if(playback != NULL){
	g_object_ref(playback);
      }

      append_audio_threaded->playback = (GObject *) playback;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_append_audio_threaded_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec)
{
  AgsAppendAudioThreaded *append_audio_threaded;

  append_audio_threaded = AGS_APPEND_AUDIO_THREADED(gobject);

  switch(prop_id){
  case PROP_AUDIO_LOOP:
    {
      g_value_set_object(value, append_audio_threaded->audio_loop);
    }
    break;
  case PROP_PLAYBACK:
    {
      g_value_set_object(value, append_audio_threaded->playback);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_append_audio_threaded_connect(AgsConnectable *connectable)
{
  ags_append_audio_threaded_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_append_audio_threaded_disconnect(AgsConnectable *connectable)
{
  ags_append_audio_threaded_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_append_audio_threaded_dispose(GObject *gobject)
{
  AgsAppendAudioThreaded *append_audio_threaded;

  append_audio_threaded = AGS_APPEND_AUDIO_THREADED(gobject);

  if(append_audio_threaded->audio_loop != NULL){
    g_object_unref(append_audio_threaded->audio_loop);

    append_audio_threaded->audio_loop = NULL;
  }

  if(append_audio_threaded->playback != NULL){
    g_object_unref(append_audio_threaded->playback);

    append_audio_threaded->playback = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_append_audio_threaded_parent_class)->dispose(gobject);
}

void
ags_append_audio_threaded_finalize(GObject *gobject)
{
  AgsAppendAudioThreaded *append_audio_threaded;

  append_audio_threaded = AGS_APPEND_AUDIO_THREADED(gobject);

  if(append_audio_threaded->audio_loop != NULL){
    g_object_unref(append_audio_threaded->audio_loop);
  }

  if(append_audio_threaded->playback != NULL){
    g_object_unref(append_audio_threaded->playback);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_append_audio_threaded_parent_class)->finalize(gobject);
}

void
ags_append_audio_threaded_launch(AgsTask *task)
{
  AgsAppendAudioThreaded *append_audio_threaded;
  AgsAudioLoop *audio_loop;

  append_audio_threaded = AGS_APPEND_AUDIO_THREADED(task);

  audio_loop = AGS_AUDIO_LOOP(append_audio_threaded->audio_loop);

  /* append to AgsDevout */
  //TODO:JK: implement me
}

AgsAppendAudioThreaded*
ags_append_audio_threaded_new(GObject *audio_loop,
			      GObject *playback)
{
  AgsAppendAudioThreaded *append_audio_threaded;

  append_audio_threaded = (AgsAppendAudioThreaded *) g_object_new(AGS_TYPE_APPEND_AUDIO_THREADED,
								  "audio-loop", audio_loop,
								  "playback", playback,
								  NULL);

  return(append_audio_threaded);
}
