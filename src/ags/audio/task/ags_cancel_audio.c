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

#include <ags/audio/task/ags_cancel_audio.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_channel.h>

void ags_cancel_audio_class_init(AgsCancelAudioClass *cancel_audio);
void ags_cancel_audio_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_cancel_audio_init(AgsCancelAudio *cancel_audio);
void ags_cancel_audio_connect(AgsConnectable *connectable);
void ags_cancel_audio_disconnect(AgsConnectable *connectable);
void ags_cancel_audio_finalize(GObject *gobject);

void ags_cancel_audio_launch(AgsTask *task);

/**
 * SECTION:ags_cancel_audio
 * @short_description: cancel audio object in audio loop
 * @title: AgsCancelAudio
 * @section_id:
 * @include: ags/audio/task/ags_cancel_audio.h
 *
 * The #AgsCancelAudio task cancels #AgsAudio playback.
 */

static gpointer ags_cancel_audio_parent_class = NULL;
static AgsConnectableInterface *ags_cancel_audio_parent_connectable_interface;

GType
ags_cancel_audio_get_type()
{
  static GType ags_type_cancel_audio = 0;

  if(!ags_type_cancel_audio){
    static const GTypeInfo ags_cancel_audio_info = {
      sizeof (AgsCancelAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_cancel_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCancelAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_cancel_audio_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_cancel_audio_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_cancel_audio = g_type_register_static(AGS_TYPE_TASK,
						   "AgsCancelAudio\0",
						   &ags_cancel_audio_info,
						   0);
    
    g_type_add_interface_static(ags_type_cancel_audio,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_cancel_audio);
}

void
ags_cancel_audio_class_init(AgsCancelAudioClass *cancel_audio)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_cancel_audio_parent_class = g_type_class_peek_parent(cancel_audio);

  /* gobject */
  gobject = (GObjectClass *) cancel_audio;

  gobject->finalize = ags_cancel_audio_finalize;

  /* task */
  task = (AgsTaskClass *) cancel_audio;

  task->launch = ags_cancel_audio_launch;
}

void
ags_cancel_audio_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_cancel_audio_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_cancel_audio_connect;
  connectable->disconnect = ags_cancel_audio_disconnect;
}

void
ags_cancel_audio_init(AgsCancelAudio *cancel_audio)
{
  cancel_audio->audio = NULL;

  cancel_audio->playback = FALSE;
  cancel_audio->sequencer = FALSE;
  cancel_audio->notation = FALSE;
}

void
ags_cancel_audio_connect(AgsConnectable *connectable)
{
  ags_cancel_audio_parent_connectable_interface->connect(connectable);


  /* empty */
}

void
ags_cancel_audio_disconnect(AgsConnectable *connectable)
{
  ags_cancel_audio_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_cancel_audio_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_cancel_audio_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_cancel_audio_launch(AgsTask *task)
{
  AgsCancelAudio *cancel_audio;
  AgsAudio *audio;
  AgsChannel *channel;

  cancel_audio = AGS_CANCEL_AUDIO(task);

  audio = cancel_audio->audio;

  /* cancel playback */
  if(cancel_audio->playback){
    channel = audio->output;

    while(channel != NULL){
      if(AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[0] == NULL){
	channel = channel->next;
	
	continue;
      }

      g_object_ref(AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[0]);
      ags_channel_tillrecycling_cancel(channel,
				       AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[0]);
      AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[0] = NULL;

      if(AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[1] == NULL &&
	 AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[2] == NULL){
	/* set remove flag only as no notation and sequencer */
	AGS_DEVOUT_PLAY(channel->devout_play)->flags |= (AGS_DEVOUT_PLAY_DONE | AGS_DEVOUT_PLAY_REMOVE);
      }
      
      channel = channel->next;
    }
  }

  /* cancel sequencer */
  if(cancel_audio->sequencer){
    channel = audio->output;

    while(channel != NULL){
      if(AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[1] == NULL){
	channel = channel->next;
	
	continue;
      }

      g_object_ref(AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[1]);
      ags_channel_tillrecycling_cancel(channel,
				       AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[1]);
      AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[1] = NULL;

      g_message("unset\0");
      
      if(AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[2] == NULL){
	/* set remove flag only as no notation */
	AGS_DEVOUT_PLAY(channel->devout_play)->flags |= (AGS_DEVOUT_PLAY_DONE | AGS_DEVOUT_PLAY_REMOVE);
      }
      
      channel = channel->next;
    }
  }

  /* cancel notation */
  if(cancel_audio->notation){
    channel = audio->output;

    while(channel != NULL){
      if(AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[2] == NULL){
	channel = channel->next;
	
	continue;
      }

      g_object_ref(AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[2]);
      ags_channel_tillrecycling_cancel(channel,
				       AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[2]);
      AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[2] = NULL;

      if(AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[1] == NULL){
	/* set remove flag only as no sequencer */
	AGS_DEVOUT_PLAY(channel->devout_play)->flags |= (AGS_DEVOUT_PLAY_DONE | AGS_DEVOUT_PLAY_REMOVE);
      }
      
      channel = channel->next;
    }
  }
}

/**
 * ags_cancel_audio_new:
 * @audio: the #AgsAudio to cancel
 * @playback: if %TRUE playback is canceld
 * @sequencer: if %TRUE sequencer is canceld
 * @notation: if %TRUE notation is canceld
 *
 * Creates an #AgsCancelAudio.
 *
 * Returns: an new #AgsCancelAudio.
 *
 * Since: 0.4
 */
AgsCancelAudio*
ags_cancel_audio_new(AgsAudio *audio,
		     gboolean playback, gboolean sequencer, gboolean notation)
{
  AgsCancelAudio *cancel_audio;

  cancel_audio = (AgsCancelAudio *) g_object_new(AGS_TYPE_CANCEL_AUDIO,
						 NULL);

  cancel_audio->audio = audio;

  cancel_audio->playback = playback;
  cancel_audio->sequencer = sequencer;
  cancel_audio->notation = notation;

  return(cancel_audio);
}
