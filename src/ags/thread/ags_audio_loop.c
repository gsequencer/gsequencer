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

#include <ags/thread/ags_audio_loop.h>

#include <ags/object/ags_connectable.h>

void ags_audio_loop_class_init(AgsAudioLoopClass *audio_loop);
void ags_audio_loop_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_loop_init(AgsAudioLoop *audio_loop);
void ags_audio_loop_connect(AgsConnectable *connectable);
void ags_audio_loop_disconnect(AgsConnectable *connectable);
void ags_audio_loop_finalize(GObject *gobject);

void ags_audio_loop_run(AgsThread *thread);

static gpointer ags_audio_loop_parent_class = NULL;
static AgsConnectableInterface *ags_audio_loop_parent_connectable_interface;

GType
ags_audio_loop_get_type()
{
  static GType ags_type_audio_loop = 0;

  if(!ags_type_audio_loop){
    static const GTypeInfo ags_audio_loop_info = {
      sizeof (AgsAudioLoopClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_loop_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioLoop),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_loop_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_loop_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_loop = g_type_register_static(AGS_TYPE_THREAD,
						 "AgsAudioLoop\0",
						 &ags_audio_loop_info,
						 0);
    
    g_type_add_interface_static(ags_type_audio_loop,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_audio_loop);
}

void
ags_audio_loop_class_init(AgsAudioLoopClass *audio_loop)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;

  ags_audio_loop_parent_class = g_type_class_peek_parent(audio_loop);

  /* GObject */
  gobject = (GObjectClass *) audio_loop;

  gobject->finalize = ags_audio_loop_finalize;

  /* AgsThread */
  thread = (AgsThreadClass *) audio_loop;

  thread->run = ags_audio_loop_run;
}

void
ags_audio_loop_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_audio_loop_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_audio_loop_connect;
  connectable->disconnect = ags_audio_loop_disconnect;
}

void
ags_audio_loop_init(AgsAudioLoop *audio_loop)
{
  AgsThread *thread;

  thread = AGS_THREAD(audio_loop);

  thread->flags |= AGS_THREAD_WAIT_FOR_CHILDREN;

  audio_loop->flags = 0;

  audio_loop->play_recall_ref = 0;
  audio_loop->play_recall = NULL;

  audio_loop->play_channel_ref = 0;
  audio_loop->play_channel = NULL;

  audio_loop->play_audio_ref = 0;
  audio_loop->play_audio = NULL;
}

void
ags_audio_loop_connect(AgsConnectable *connectable)
{
  ags_audio_loop_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_audio_loop_disconnect(AgsConnectable *connectable)
{
  ags_audio_loop_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_audio_loop_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_audio_loop_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_audio_loop_run(AgsThread *thread)
{
  //TODO:JK: implement me
}

void
ags_audio_loop_add_audio(AgsAudioLoop *audio_loop, GObject *audio)
{
  //TODO:JK: implement me
}

void
ags_audio_loop_remove_audio(AgsAudioLoop *audio_loop, GObject *audio)
{
  //TODO:JK: implement me
}

void
ags_audio_loop_add_channel(AgsAudioLoop *audio_loop, GObject *channel)
{
  //TODO:JK: implement me
}

void
ags_audio_loop_remove_channel(AgsAudioLoop *audio_loop, GObject *channel)
{
  //TODO:JK: implement me
}

void
ags_audio_loop_add_recall(AgsAudioLoop *audio_loop, GObject *recall)
{
  //TODO:JK: implement me
}

void
ags_audio_loop_remove_recall(AgsAudioLoop *audio_loop, GObject *recall)
{
  //TODO:JK: implement me
}

AgsAudioLoop*
ags_audio_loop_new()
{
  AgsAudioLoop *audio_loop;

  audio_loop = (AgsAudioLoop *) g_object_new(AGS_TYPE_AUDIO_LOOP,
					     NULL);

  return(audio_loop);
}
