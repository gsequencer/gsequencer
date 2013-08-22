/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/task/ags_append_audio.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

void ags_append_audio_class_init(AgsAppendAudioClass *append_audio);
void ags_append_audio_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_append_audio_init(AgsAppendAudio *append_audio);
void ags_append_audio_connect(AgsConnectable *connectable);
void ags_append_audio_disconnect(AgsConnectable *connectable);
void ags_append_audio_finalize(GObject *gobject);

void ags_append_audio_launch(AgsTask *task);

static gpointer ags_append_audio_parent_class = NULL;
static AgsConnectableInterface *ags_append_audio_parent_connectable_interface;

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
  append_audio->devout_play = NULL;
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
  AgsServer *server;
  AgsAppendAudio *append_audio;
  AgsAudioLoop *audio_loop;

  append_audio = AGS_APPEND_AUDIO(task);

  audio_loop = AGS_AUDIO_LOOP(append_audio->audio_loop);

  /* append to AgsDevout */
  append_audio->devout_play->flags &= (~AGS_DEVOUT_PLAY_REMOVE);
  audio_loop->play_audio = g_list_append(audio_loop->play_audio,
					 append_audio->devout_play);
  audio_loop->play_audio_ref += 1;

  /* add to server registry */
  server = AGS_MAIN(audio_loop->main);

  if((AGS_SERVER_RUNNING & (server->flags)) != 0){
    ags_connectable_add_to_registry(AGS_CONNECTABLE(append_audio->devout_play->source));
  }
}

AgsAppendAudio*
ags_append_audio_new(GObject *audio_loop,
		     AgsDevoutPlay *devout_play)
{
  AgsAppendAudio *append_audio;

  append_audio = (AgsAppendAudio *) g_object_new(AGS_TYPE_APPEND_AUDIO,
						 NULL);

  append_audio->audio_loop = audio_loop;
  append_audio->devout_play = devout_play;

  return(append_audio);
}
