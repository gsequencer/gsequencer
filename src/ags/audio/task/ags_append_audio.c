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

#include <ags/audio/task/ags_append_audio.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/thread/ags_audio_thread.h>
#include <ags/thread/ags_channel_thread.h>

#include <ags/audio/ags_config.h>
#include <ags/audio/ags_devout.h>

void ags_append_audio_class_init(AgsAppendAudioClass *append_audio);
void ags_append_audio_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_append_audio_init(AgsAppendAudio *append_audio);
void ags_append_audio_connect(AgsConnectable *connectable);
void ags_append_audio_disconnect(AgsConnectable *connectable);
void ags_append_audio_finalize(GObject *gobject);

void ags_append_audio_launch(AgsTask *task);

/**
 * SECTION:ags_append_audio
 * @short_description: append audio object to audio loop
 * @title: AgsAppendAudio
 * @section_id:
 * @include: ags/audio/task/ags_append_audio.h
 *
 * The #AgsAppendAudio task appends #AgsAudio to #AgsAudioLoop.
 */

static gpointer ags_append_audio_parent_class = NULL;
static AgsConnectableInterface *ags_append_audio_parent_connectable_interface;

extern AgsConfig *config;

GType
ags_append_audio_get_type()
{
  static GType ags_type_append_audio = 0;

  if(!ags_type_append_audio){
    static const GTypeInfo ags_append_audio_info = {
      sizeof (AgsAppendAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_append_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAppendAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_append_audio_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_append_audio_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_append_audio = g_type_register_static(AGS_TYPE_TASK,
						   "AgsAppendAudio\0",
						   &ags_append_audio_info,
						   0);

    g_type_add_interface_static(ags_type_append_audio,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_append_audio);
}

void
ags_append_audio_class_init(AgsAppendAudioClass *append_audio)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_append_audio_parent_class = g_type_class_peek_parent(append_audio);

  /* gobject */
  gobject = (GObjectClass *) append_audio;

  gobject->finalize = ags_append_audio_finalize;

  /* task */
  task = (AgsTaskClass *) append_audio;

  task->launch = ags_append_audio_launch;
}

void
ags_append_audio_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_append_audio_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_append_audio_connect;
  connectable->disconnect = ags_append_audio_disconnect;
}

void
ags_append_audio_init(AgsAppendAudio *append_audio)
{
  append_audio->audio_loop = NULL;
  append_audio->audio = NULL;
}

void
ags_append_audio_connect(AgsConnectable *connectable)
{
  ags_append_audio_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_append_audio_disconnect(AgsConnectable *connectable)
{
  ags_append_audio_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_append_audio_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_append_audio_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_append_audio_launch(AgsTask *task)
{
  AgsAudio *audio;

  AgsAppendAudio *append_audio;

  AgsAudioLoop *audio_loop;
  
  AgsServer *server;

  append_audio = AGS_APPEND_AUDIO(task);

  audio = append_audio->audio;
  audio_loop = AGS_AUDIO_LOOP(append_audio->audio_loop);

  /* append to AgsDevout */
  ags_audio_loop_add_audio(audio_loop,
			   audio);

  /**/  
  if(!g_ascii_strncasecmp(ags_config_get(config,
					 AGS_CONFIG_THREAD,
					 "model\0"),
			  "super-threaded\0",
			  15)){
    /* super threaed setup */
    if(!g_ascii_strncasecmp(ags_config_get(config,
					   AGS_CONFIG_THREAD,
					   "super-threaded-scope\0"),
			    "audio\0",
			    6) ||
       !g_ascii_strncasecmp(ags_config_get(config,
						 AGS_CONFIG_THREAD,
						 "super-threaded-scope\0"),
				  "channel\0",
				  8)){
      ags_thread_add_child_extended(audio_loop, AGS_DEVOUT_PLAY_DOMAIN(audio->devout_play_domain)->audio_thread[1],
				    TRUE, TRUE);
      ags_connectable_connect(AGS_CONNECTABLE(AGS_DEVOUT_PLAY_DOMAIN(audio->devout_play_domain)->audio_thread[1]));

      ags_thread_add_child_extended(audio_loop, AGS_DEVOUT_PLAY_DOMAIN(audio->devout_play_domain)->audio_thread[2],
				    TRUE, TRUE);
      ags_connectable_connect(AGS_CONNECTABLE(AGS_DEVOUT_PLAY_DOMAIN(audio->devout_play_domain)->audio_thread[2]));
    }
  }
  
  /* add to server registry */
  server = AGS_MAIN(audio_loop->ags_main)->server;

  if(server != NULL && (AGS_SERVER_RUNNING & (server->flags)) != 0){
    ags_connectable_add_to_registry(AGS_CONNECTABLE(append_audio->audio));
  }
}

/**
 * ags_append_audio_new:
 * @audio_loop: the #AgsAudioLoop
 * @audio: the #AgsAudio to append
 *
 * Creates an #AgsAppendAudio.
 *
 * Returns: an new #AgsAppendAudio.
 *
 * Since: 0.4
 */
AgsAppendAudio*
ags_append_audio_new(GObject *audio_loop,
		     GObject *audio)
{
  AgsAppendAudio *append_audio;

  append_audio = (AgsAppendAudio *) g_object_new(AGS_TYPE_APPEND_AUDIO,
						 NULL);

  append_audio->audio_loop = audio_loop;
  append_audio->audio = audio;

  return(append_audio);
}
