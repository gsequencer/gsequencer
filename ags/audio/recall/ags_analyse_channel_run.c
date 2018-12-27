/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/recall/ags_analyse_channel_run.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/recall/ags_analyse_channel.h>
#include <ags/audio/recall/ags_analyse_recycling.h>

void ags_analyse_channel_run_class_init(AgsAnalyseChannelRunClass *analyse_channel_run);
void ags_analyse_channel_run_init(AgsAnalyseChannelRun *analyse_channel_run);
void ags_analyse_channel_run_finalize(GObject *gobject);

void ags_analyse_channel_run_run_pre(AgsRecall *recall);

/**
 * SECTION:ags_analyse_channel_run
 * @short_description: analyse channel
 * @title: AgsAnalyseChannelRun
 * @section_id:
 * @include: ags/audio/recall/ags_analyse_channel_run.h
 *
 * The #AgsAnalyseChannelRun class analyses the channel.
 */

static gpointer ags_analyse_channel_run_parent_class = NULL;

GType
ags_analyse_channel_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_analyse_channel_run = 0;

    static const GTypeInfo ags_analyse_channel_run_info = {
      sizeof (AgsAnalyseChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_analyse_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAnalyseChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_analyse_channel_run_init,
    };
    
    ags_type_analyse_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
							  "AgsAnalyseChannelRun",
							  &ags_analyse_channel_run_info,
							  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_analyse_channel_run);
  }

  return g_define_type_id__volatile;
}

void
ags_analyse_channel_run_class_init(AgsAnalyseChannelRunClass *analyse_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_analyse_channel_run_parent_class = g_type_class_peek_parent(analyse_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) analyse_channel_run;

  gobject->finalize = ags_analyse_channel_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) analyse_channel_run;

  recall->run_pre = ags_analyse_channel_run_run_pre;
}

void
ags_analyse_channel_run_init(AgsAnalyseChannelRun *analyse_channel_run)
{
  ags_recall_set_ability_flags((AgsRecall *) analyse_channel_run, (AGS_SOUND_ABILITY_PLAYBACK |
								   AGS_SOUND_ABILITY_SEQUENCER |
								   AGS_SOUND_ABILITY_NOTATION |
								   AGS_SOUND_ABILITY_WAVE |
								   AGS_SOUND_ABILITY_MIDI));
  
  AGS_RECALL(analyse_channel_run)->name = "ags-analyse";
  AGS_RECALL(analyse_channel_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(analyse_channel_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(analyse_channel_run)->xml_type = "ags-analyse-channel-run";
  AGS_RECALL(analyse_channel_run)->port = NULL;

  AGS_RECALL(analyse_channel_run)->child_type = AGS_TYPE_ANALYSE_RECYCLING;
}

void
ags_analyse_channel_run_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_analyse_channel_run_parent_class)->finalize(gobject);
}

void
ags_analyse_channel_run_run_pre(AgsRecall *recall)
{
  AgsPort *buffer_computed;
  AgsAnalyseChannel *analyse_channel;
  AgsAnalyseChannelRun *analyse_channel_run;

  guint cache_buffer_size;
  guint cache_format;
  gboolean current_buffer_computed;
  
  GValue value = {0,};

  void (*parent_class_run_pre)(AgsRecall *recall);
  
  pthread_mutex_t *recall_mutex;
  pthread_mutex_t *buffer_mutex;

  analyse_channel_run = AGS_ANALYSE_CHANNEL_RUN(recall);

  g_object_get(recall,
	       "recall-channel", &analyse_channel,
	       NULL);
  
  /* get mutex and buffer mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = recall->obj_mutex;
  buffer_mutex = analyse_channel->buffer_mutex;

  parent_class_run_pre = AGS_RECALL_CLASS(ags_analyse_channel_run_parent_class)->run_pre;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* call parent */
  parent_class_run_pre(recall);

  /* get some fields */
  g_object_get(analyse_channel,
	       "buffer-computed", &buffer_computed,
	       "cache-buffer-size", &cache_buffer_size,
	       "cache-format", &cache_format,
	       NULL);
  
  /* calculate of previous run */
  g_value_init(&value,
	       G_TYPE_BOOLEAN);

  ags_port_safe_read(analyse_channel->buffer_computed,
		     &value);
  
  current_buffer_computed = g_value_get_boolean(&value);

  if(!current_buffer_computed){    
    /* set buffer-computed port to TRUE */
    g_value_reset(&value);
    g_value_set_boolean(&value,
			TRUE);

    ags_port_safe_write(buffer_computed,
			&value);
  }

  g_value_unset(&value);

  /* lock free - buffer-computed reset by cyclic-task AgsResetAnalyse */
  if(!current_buffer_computed){
    /* retrieve analyse */    
    ags_analyse_channel_retrieve_frequency_and_magnitude(analyse_channel);

    /* clear buffer */
    pthread_mutex_lock(buffer_mutex);
  
    ags_audio_buffer_util_clear_buffer(analyse_channel->in, 1,
				       cache_buffer_size, cache_format);
  
    pthread_mutex_unlock(buffer_mutex);
  }
}

/**
 * ags_analyse_channel_run_new:
 * @source: the source #AgsChannel
 *
 * Create a new instance of #AgsAnalyseChannelRun
 *
 * Returns: the new #AgsAnalyseChannelRun
 *
 * Since: 2.0.0
 */
AgsAnalyseChannelRun*
ags_analyse_channel_run_new(AgsChannel *source)
{
  AgsAnalyseChannelRun *analyse_channel_run;

  analyse_channel_run = (AgsAnalyseChannelRun *) g_object_new(AGS_TYPE_ANALYSE_CHANNEL_RUN,
							      "source", source,
							      NULL);

  return(analyse_channel_run);
}
