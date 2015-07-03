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

#include <ags/audio/task/ags_init_audio.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_devout.h>

void ags_init_audio_class_init(AgsInitAudioClass *init_audio);
void ags_init_audio_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_init_audio_init(AgsInitAudio *init_audio);
void ags_init_audio_connect(AgsConnectable *connectable);
void ags_init_audio_disconnect(AgsConnectable *connectable);
void ags_init_audio_finalize(GObject *gobject);

void ags_init_audio_launch(AgsTask *task);

/**
 * SECTION:ags_init_audio
 * @short_description: init audio object
 * @title: AgsInitAudio
 * @section_id:
 * @include: ags/audio/task/ags_init_audio.h
 *
 * The #AgsInitAudio task inits #AgsAudio.
 */

static gpointer ags_init_audio_parent_class = NULL;
static AgsConnectableInterface *ags_init_audio_parent_connectable_interface;

GType
ags_init_audio_get_type()
{
  static GType ags_type_init_audio = 0;

  if(!ags_type_init_audio){
    static const GTypeInfo ags_init_audio_info = {
      sizeof (AgsInitAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_init_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsInitAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_init_audio_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_init_audio_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_init_audio = g_type_register_static(AGS_TYPE_TASK,
						 "AgsInitAudio\0",
						 &ags_init_audio_info,
						 0);

    g_type_add_interface_static(ags_type_init_audio,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_init_audio);
}

void
ags_init_audio_class_init(AgsInitAudioClass *init_audio)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_init_audio_parent_class = g_type_class_peek_parent(init_audio);

  /* gobject */
  gobject = (GObjectClass *) init_audio;

  gobject->finalize = ags_init_audio_finalize;

  /* task */
  task = (AgsTaskClass *) init_audio;

  task->launch = ags_init_audio_launch;
}

void
ags_init_audio_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_init_audio_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_init_audio_connect;
  connectable->disconnect = ags_init_audio_disconnect;
}

void
ags_init_audio_init(AgsInitAudio *init_audio)
{
  init_audio->audio = NULL;

  init_audio->playback = FALSE;
  init_audio->sequencer = FALSE;
  init_audio->notation = FALSE;
}

void
ags_init_audio_connect(AgsConnectable *connectable)
{
  ags_init_audio_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_init_audio_disconnect(AgsConnectable *connectable)
{
  ags_init_audio_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_init_audio_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_init_audio_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_init_audio_launch(AgsTask *task)
{
  AgsInitAudio *init_audio;
  AgsAudio *audio;
  AgsRecallID *recall_id;
  GList *devout_play;
  GList *list;

  init_audio = AGS_INIT_AUDIO(task);

  audio = init_audio->audio;

  /* init audio */
  if(init_audio->playback){
    g_atomic_int_or(&(AGS_DEVOUT_PLAY_DOMAIN(audio->devout_play_domain)->flags),
		    AGS_DEVOUT_PLAY_DOMAIN_PLAYBACK);
    devout_play = AGS_DEVOUT_PLAY_DOMAIN(audio->devout_play_domain)->devout_play;
    
    list = ags_audio_recursive_play_init(audio,
					 TRUE, FALSE, FALSE);

    while(list != NULL){
      //      AGS_DEVOUT_PLAY(devout_play->data)->recall_id[0] = list->data;
      //      AGS_DEVOUT_PLAY(devout_play->data)->flags |= AGS_DEVOUT_PLAY_PLAYBACK;

      devout_play = devout_play->next;
      list = list->next;
    }
  }

  if(init_audio->sequencer){
    g_atomic_int_or(&(AGS_DEVOUT_PLAY_DOMAIN(audio->devout_play_domain)->flags),
		    AGS_DEVOUT_PLAY_DOMAIN_SEQUENCER);
    devout_play = AGS_DEVOUT_PLAY_DOMAIN(audio->devout_play_domain)->devout_play;

    list = ags_audio_recursive_play_init(audio,
					 FALSE, TRUE, FALSE);

    while(list != NULL){
      //      AGS_DEVOUT_PLAY(devout_play->data)->recall_id[1] = list->data;
      //      AGS_DEVOUT_PLAY(devout_play->data)->flags |= AGS_DEVOUT_PLAY_SEQUENCER;
      
      devout_play = devout_play->next;
      list = list->next;
    }
  }

  if(init_audio->notation){
    g_atomic_int_or(&(AGS_DEVOUT_PLAY_DOMAIN(audio->devout_play_domain)->flags),
		    AGS_DEVOUT_PLAY_DOMAIN_NOTATION);
    devout_play = AGS_DEVOUT_PLAY_DOMAIN(audio->devout_play_domain)->devout_play;

    list = ags_audio_recursive_play_init(audio,
					 FALSE, FALSE, TRUE);

    while(list != NULL){
      //      AGS_DEVOUT_PLAY(devout_play->data)->recall_id[2] = list->data;
      //      AGS_DEVOUT_PLAY(devout_play->data)->flags |= AGS_DEVOUT_PLAY_NOTATION;

      devout_play = devout_play->next;
      list = list->next;
    }
  }
}

/**
 * ags_init_audio_new:
 * @audio: the #AgsAudio
 * @playback: init playback
 * @sequencer: init sequencer
 * @notation: init notation
 *
 * Creates an #AgsInitAudio.
 *
 * Returns: an new #AgsInitAudio.
 *
 * Since: 0.4
 */
AgsInitAudio*
ags_init_audio_new(AgsAudio *audio,
		   gboolean playback, gboolean sequencer, gboolean notation)
{
  AgsInitAudio *init_audio;

  init_audio = (AgsInitAudio *) g_object_new(AGS_TYPE_INIT_AUDIO,
					     NULL);

  init_audio->audio = audio;

  init_audio->playback = playback;
  init_audio->sequencer = sequencer;
  init_audio->notation = notation;

  return(init_audio);
}
