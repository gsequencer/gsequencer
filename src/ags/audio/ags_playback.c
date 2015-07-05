/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#include <ags/audio/ags_playback.h>

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_recall_id.h>
#include <ags/audio/thread/ags_iterator_thread.h>

/**
 * SECTION:ags_playback
 * @short_description: Outputting to soundcard context
 * @title: AgsPlayback
 * @section_id:
 * @include: ags/audio/ags_playback.h
 *
 * #AgsPlayback represents a context to output.
 */

void ags_playback_class_init(AgsPlaybackClass *playback);
void ags_playback_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_playback_init(AgsPlayback *playback);
void ags_playback_disconnect(AgsConnectable *connectable);
void ags_playback_connect(AgsConnectable *connectable);
void ags_playback_finalize(GObject *gobject);

static gpointer ags_playback_parent_class = NULL;

GType
ags_playback_get_type (void)
{
  static GType ags_type_playback = 0;

  if(!ags_type_playback){
    static const GTypeInfo ags_playback_info = {
      sizeof (AgsPlaybackClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_playback_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayback),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_playback_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_playback_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_playback = g_type_register_static(G_TYPE_OBJECT,
					       "AgsPlayback\0",
					       &ags_playback_info,
					       0);

    g_type_add_interface_static(ags_type_playback,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_playback);
}

void
ags_playback_class_init(AgsPlaybackClass *playback)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_playback_parent_class = g_type_class_peek_parent(playback);

  /* GObjectClass */
  gobject = (GObjectClass *) playback;

  gobject->finalize = ags_playback_finalize;
}

void
ags_playback_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_playback_connect;
  connectable->disconnect = ags_playback_disconnect;
}

void
ags_playback_init(AgsPlayback *playback)
{
  playback->flags = 0;

  playback->iterator_thread = (AgsIteratorThread **) malloc(3 * sizeof(AgsIteratorThread *));

  playback->iterator_thread[0] = ags_iterator_thread_new();
  playback->iterator_thread[1] = ags_iterator_thread_new();
  playback->iterator_thread[2] = ags_iterator_thread_new();

  playback->source = NULL;
  playback->audio_channel = 0;

  playback->recall_id = (AgsRecallID **) malloc(3 * sizeof(AgsRecallID *));

  playback->recall_id[0] = NULL;
  playback->recall_id[1] = NULL;
  playback->recall_id[2] = NULL;
}

void
ags_playback_finalize(GObject *gobject)
{
  AgsPlayback *playback;

  playback = AGS_PLAYBACK(gobject);

  g_object_unref(G_OBJECT(playback->iterator_thread[0]));
  g_object_unref(G_OBJECT(playback->iterator_thread[1]));
  g_object_unref(G_OBJECT(playback->iterator_thread[2]));

  free(playback->iterator_thread);

  /* call parent */
  G_OBJECT_CLASS(ags_playback_parent_class)->finalize(gobject);
}

void
ags_playback_connect(AgsConnectable *connectable)
{
  AgsPlayback *playback;

  playback = AGS_PLAYBACK(connectable);

  //TODO:JK: implement me
}

void
ags_playback_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_playback_play_find_source:
 * @playback_play: a #GList containing #AgsPlayback
 * 
 * Find source
 *
 * Returns: the matching playback play
 *
 * Since: 0.4
 */
AgsPlayback*
ags_playback_find_source(GList *playback,
			 GObject *source)
{
  while(playback != NULL){
    if(AGS_PLAYBACK(playback->data)->source == source){
      return(playback->data);
    }

    playback = playback->next;
  }

  return(NULL);
}

/**
 * ags_playback_new:
 *
 * Creates an #AgsPlayback, refering to @application_context.
 *
 * Returns: a new #AgsPlayback
 *
 * Since: 0.4
 */
AgsPlayback*
ags_playback_new()
{
  AgsPlayback *playback;

  playback = (AgsPlayback *) g_object_new(AGS_TYPE_PLAYBACK,
					  NULL);
  
  return(playback);
}
