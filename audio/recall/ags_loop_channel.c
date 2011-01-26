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

#include <ags/audio/recall/ags_loop_channel.h>

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recall_id.h>

void ags_loop_channel_class_init(AgsLoopChannelClass *loop_channel);
void ags_loop_channel_init(AgsLoopChannel *loop_channel);
void ags_loop_channel_finalize(GObject *gobject);

void ags_loop_channel_run_pre(AgsRecall *recall, AgsRecallID *recall_id,
			      gpointer data);

static gpointer ags_loop_channel_parent_class = NULL;

GType
ags_loop_channel_get_type()
{
  static GType ags_type_loop_channel = 0;

  if(!ags_type_loop_channel){
    static const GTypeInfo ags_loop_channel_info = {
      sizeof (AgsLoopChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_loop_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLoopChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_loop_channel_init,
    };

    ags_type_loop_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						   "AgsLoopChannel\0",
						   &ags_loop_channel_info,
						   0);
  }
  return (ags_type_loop_channel);
}

void
ags_loop_channel_class_init(AgsLoopChannelClass *loop_channel)
{
  GObjectClass *gobject;

  ags_loop_channel_parent_class = g_type_class_peek_parent(loop_channel);

  gobject = (GObjectClass *) loop_channel;
  gobject->finalize = ags_loop_channel_finalize;
}

void
ags_loop_channel_init(AgsLoopChannel *loop_channel)
{
  loop_channel->channel = NULL;
  loop_channel->delay_audio_run = NULL;
}

void
ags_loop_channel_finalize(GObject *gobject)
{
  AgsLoopChannel *loop_channel;
}

void
ags_loop_channel_connect(AgsLoopChannel *loop_channel)
{
  //  ags_recall_connect(AGS_RECALL(loop_channel));

  g_signal_connect((GObject *) loop_channel, "run_pre\0",
		   G_CALLBACK(ags_loop_channel_run_pre), NULL);
}

void
ags_loop_channel_run_pre(AgsRecall *recall, AgsRecallID *recall_id,
			 gpointer data)
{
  AgsLoopChannel *loop_channel;
  AgsDevout *devout;
  AgsRecycling *recycling;
  AgsAudioSignal *audio_signal;

  loop_channel = AGS_LOOP_CHANNEL(recall);

  devout = AGS_DEVOUT(AGS_AUDIO(loop_channel->channel->audio)->devout);

  recycling = loop_channel->channel->first_recycling;
 
  while(recycling != loop_channel->channel->last_recycling->next){
    audio_signal = ags_audio_signal_new((GObject *) devout,
					(GObject *) recycling,
					(GObject *) recall_id);
    ags_connectable_connect(AGS_CONNECTABLE(audio_signal));
    
    ags_recycling_add_audio_signal(recycling,
				   audio_signal);
    audio_signal->stream_current = audio_signal->stream_beginning;
    
    
    recycling = recycling->next;
  }
}

AgsLoopChannel*
ags_loop_channel_new(AgsChannel *channel,
		     AgsDelayAudioRun *delay_audio_run)
{
  AgsLoopChannel *loop_channel;

  loop_channel = (AgsLoopChannel *) g_object_new(AGS_TYPE_LOOP_CHANNEL, NULL);

  loop_channel->channel = channel;
  loop_channel->delay_audio_run = delay_audio_run;

  return(loop_channel);
}
