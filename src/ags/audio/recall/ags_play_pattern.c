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

#include <ags/audio/recall/ags_play_pattern.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/task/ags_cancel_recall.h>

#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_audio_signal.h>

#include <ags/X/ags_machine.h>

void ags_play_pattern_class_init(AgsPlayPatternClass *play_pattern);
void ags_play_pattern_init(AgsPlayPattern *play_pattern);
void ags_play_pattern_finalize(GObject *gobject);

void ags_play_pattern(AgsRecall *recall, AgsRecallID *recall_id);
void ags_play_pattern_stop(AgsRecall *recall);
void ags_play_pattern_cancel(AgsRecall *recall);

void ags_play_pattern_play_channel_done(AgsRecall *recall);

static gpointer ags_play_pattern_parent_class = NULL;

GType
ags_play_pattern_get_type()
{
  static GType ags_type_play_pattern = 0;

  if(!ags_type_play_pattern){
    static const GTypeInfo ags_play_pattern_info = {
      sizeof (AgsPlayPatternClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_pattern_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayPattern),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_pattern_init,
    };
    ags_type_play_pattern = g_type_register_static(AGS_TYPE_RECALL, "AgsPlayPattern\0", &ags_play_pattern_info, 0);
  }
  return (ags_type_play_pattern);
}

void
ags_play_pattern_class_init(AgsPlayPatternClass *play_pattern)
{
  GObjectClass *gobject;

  ags_play_pattern_parent_class = g_type_class_peek_parent(play_pattern);

  gobject = (GObjectClass *) play_pattern;
  gobject->finalize = ags_play_pattern_finalize;
}

void
ags_play_pattern_init(AgsPlayPattern *play_pattern)
{
  play_pattern->channel = NULL;
  play_pattern->pattern = NULL;

  play_pattern->i = 0;
  play_pattern->j = 0;
  play_pattern->bit = 0;
  play_pattern->length = 0;

  play_pattern->loop = FALSE;
}

void
ags_play_pattern_finalize(GObject *gobject)
{
}

void
ags_play_pattern_connect(AgsPlayPattern *play_pattern)
{
  //  ags_recall_connect(AGS_RECALL(play_pattern));
}

void
ags_play_pattern(AgsRecall *recall, AgsRecallID *recall_id)
{
  /*
  AgsDelay *delay;
  AgsPlayPattern *play_pattern;
  AgsPlayChannel *play_channel;
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  play_pattern = (AgsPlayPattern *) recall;
  delay = play_pattern->delay;
  //  fprintf(stdout, "ags_play_pattern\n\0");

  pthread_mutex_lock(&mutex);

  if(play_pattern->bit == play_pattern->length){
    if(play_pattern->loop){
      pthread_mutex_unlock(&mutex);

      if(delay->counter == 0){
	play_pattern->bit = 0;
	goto ags_play_pattern0;
      }
    }else{

      if(play_pattern->recall.recall == NULL){
	AGS_RECALL_GET_CLASS((AgsRecall *) play_pattern)->done(AGS_RECALL(play_pattern), recall_id);
	//	play_pattern->recall.flags |= AGS_RECALL_REMOVE;

	AGS_DELAY(play_pattern->recall.parent)->recall_ref--;
	play_pattern->bit = 0;
      }

      pthread_mutex_unlock(&mutex);
    }
  }else{
    pthread_mutex_unlock(&mutex);

    if(delay->counter == 0){
      //      fprintf(stdout, "  (AGS_RECALL_DELAY & recall->flags) == 0\n\0");
    ags_play_pattern0:

      if(ags_pattern_get_bit(play_pattern->pattern, play_pattern->i, play_pattern->j, play_pattern->bit)){
	//	fprintf(stdout, "  ags_pattern_get_bit(play_pattern->pattern, play_pattern->i, play_pattern->j, play_pattern->bit)\n\0");
	play_channel = ags_play_channel_new(play_pattern->channel);

	play_channel->recall.parent = (GObject *) play_pattern;
	AGS_RECALL_GET_CLASS((AgsRecall *) play_channel)->done = ags_play_pattern_play_channel_done;

	ags_play_channel_map_play_audio_signal(play_channel, NULL);

	recall->recall = g_list_append(recall->recall, play_channel);
      }

      play_pattern->bit++;
    }
  }

  //  _ags_recall_run(recall);
  */
}

void
ags_play_pattern_stop(AgsRecall *recall)
{
  /*
  AgsSoundcard *soundcard;
  GList *list, *list_next;

  soundcard = AGS_SOUNDCARD(AGS_AUDIO(AGS_RECALL(recall->parent)->parent)->soundcard);

  list = recall->recall;
  recall->recall = NULL;

  while(list != NULL){
    list_next = list->next;

    g_object_unref(G_OBJECT(list->data));
    g_list_free1(list);

    list = list_next;
  }
  */
}

void
ags_play_pattern_play_channel_done(AgsRecall *recall)
{
  /*
  AgsPlayPattern *play_pattern;
  AgsPlayChannel *play_channel;
  GList *list, *list_next;

  play_pattern = AGS_PLAY_PATTERN(recall->parent);
  play_channel = AGS_PLAY_CHANNEL(recall);

  AGS_RECALL(play_pattern)->recall = g_list_remove(AGS_RECALL(play_pattern)->recall, recall);

  list = play_channel->recall.recall;
  play_channel->recall.recall = NULL;

  while(list != NULL){
    list_next = list->next;

    g_object_unref(G_OBJECT(list->data));
    g_list_free1(list);

    list = list_next;
  }

  g_object_unref((GObject *) play_channel);
  */
}

void
ags_play_pattern_cancel(AgsRecall *recall)
{
  AgsPlayPattern *play_pattern;
  /*
  play_pattern = (AgsPlayPattern *) recall;

  if(play_pattern->bit == play_pattern->length){
    if(play_pattern->loop && play_pattern->recall.run_main)
      AGS_RECALL_GET_CLASS(recall)->loop(recall);

    if(play_pattern->recall.recall == NULL)
      AGS_RECALL_GET_CLASS((AgsRecall *) play_pattern)->done((AgsRecall *) play_pattern);
  }else
    if(recall->delay)
      play_pattern->bit++;

  _ags_recall_cancel(recall);
  */
}
/*
AgsPlayPattern*
ags_play_pattern_new(AgsChannel *channel, AgsDelay *delay)
{
  AgsPlayPattern *play_pattern;

  play_pattern = (AgsPlayPattern *) g_object_new(AGS_TYPE_PLAY_PATTERN, NULL);

  if(channel != NULL){
    play_pattern->delay = delay;

    play_pattern->channel = channel;
    play_pattern->pattern = AGS_PATTERN(channel->pattern->data);
  }

  return(play_pattern);
}
*/
