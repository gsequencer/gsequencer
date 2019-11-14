/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/recall/ags_peak_channel_run.h>

#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/recall/ags_peak_channel.h>
#include <ags/audio/recall/ags_peak_recycling.h>

void ags_peak_channel_run_class_init(AgsPeakChannelRunClass *peak_channel_run);
void ags_peak_channel_run_init(AgsPeakChannelRun *peak_channel_run);
void ags_peak_channel_run_finalize(GObject *gobject);

void ags_peak_channel_run_run_pre(AgsRecall *recall);

/**
 * SECTION:ags_peak_channel_run
 * @short_description: peak
 * @title: AgsPeakChannelRun
 * @section_id:
 * @include: ags/audio/recall/ags_peak_channel_run.h
 *
 * The #AgsPeakChannelRun class peaks the channel.
 */

static gpointer ags_peak_channel_run_parent_class = NULL;

GType
ags_peak_channel_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_peak_channel_run = 0;

    static const GTypeInfo ags_peak_channel_run_info = {
      sizeof (AgsPeakChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_peak_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPeakChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_peak_channel_run_init,
    };
    
    ags_type_peak_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
						       "AgsPeakChannelRun",
						       &ags_peak_channel_run_info,
						       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_peak_channel_run);
  }

  return g_define_type_id__volatile;
}

void
ags_peak_channel_run_class_init(AgsPeakChannelRunClass *peak_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_peak_channel_run_parent_class = g_type_class_peek_parent(peak_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) peak_channel_run;

  gobject->finalize = ags_peak_channel_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) peak_channel_run;

  recall->run_pre = ags_peak_channel_run_run_pre;
}

void
ags_peak_channel_run_init(AgsPeakChannelRun *peak_channel_run)
{
  ags_recall_set_ability_flags((AgsRecall *) peak_channel_run, (AGS_SOUND_ABILITY_SEQUENCER |
								AGS_SOUND_ABILITY_NOTATION |
								AGS_SOUND_ABILITY_WAVE |
								AGS_SOUND_ABILITY_MIDI));

  AGS_RECALL(peak_channel_run)->name = "ags-peak";
  AGS_RECALL(peak_channel_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(peak_channel_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(peak_channel_run)->xml_type = "ags-peak-channel-run";
  AGS_RECALL(peak_channel_run)->port = NULL;

  AGS_RECALL(peak_channel_run)->child_type = AGS_TYPE_PEAK_RECYCLING;
}

void
ags_peak_channel_run_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_peak_channel_run_parent_class)->finalize(gobject);
}

void
ags_peak_channel_run_run_pre(AgsRecall *recall)
{
  AgsPort *buffer_computed;
  AgsPeakChannel *peak_channel;

  guint buffer_size;
  guint format;
  gboolean current_buffer_computed;
  
  GValue value = {0,};
  
  void (*parent_class_run_pre)(AgsRecall *recall);  

  GRecMutex *buffer_mutex;

  g_object_get(recall,
	       "recall-channel", &peak_channel,
	       NULL);

  /* get mutex */
  buffer_mutex = &(peak_channel->buffer_mutex);
  
  /* get parent class */
  AGS_RECALL_LOCK_CLASS();

  parent_class_run_pre = AGS_RECALL_CLASS(ags_peak_channel_run_parent_class)->run_pre;

  AGS_RECALL_UNLOCK_CLASS()

  /* call parent */
  parent_class_run_pre(recall);

  /* calculate of previous run */
  g_object_get(peak_channel,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       "buffer-computed", &buffer_computed,
	       NULL);
  
  g_rec_mutex_lock(buffer_mutex);
  
  g_value_init(&value,
	       G_TYPE_BOOLEAN);

  ags_port_safe_read(buffer_computed,
		     &value);

  current_buffer_computed = g_value_get_boolean(&value);

  g_value_unset(&value);

  g_object_unref(buffer_computed);
  
  if(!current_buffer_computed){    
    /* set buffer-computed port to TRUE */
    g_value_init(&value,
		 G_TYPE_BOOLEAN);
    g_value_set_boolean(&value,
			TRUE);

    ags_port_safe_write(buffer_computed,
			&value);
    g_value_unset(&value);
  }
  
  g_rec_mutex_unlock(buffer_mutex);

  /* lock free - buffer-computed reset by cyclic-task AgsResetPeak */
  if(!current_buffer_computed){
    /* retrieve peak */    
    ags_peak_channel_retrieve_peak_internal(peak_channel);

    /* clear buffer */
    ags_audio_buffer_util_clear_buffer(peak_channel->buffer, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(format));
  }

  g_object_unref(peak_channel);
}

/**
 * ags_peak_channel_run_new:
 * @source: the #AgsChannel
 *
 * Creates a new instance of #AgsPeakChannelRun
 *
 * Returns: the new #AgsPeakChannelRun
 *
 * Since: 2.0.0
 */
AgsPeakChannelRun*
ags_peak_channel_run_new(AgsChannel *source)
{
  AgsPeakChannelRun *peak_channel_run;
  
  peak_channel_run = (AgsPeakChannelRun *) g_object_new(AGS_TYPE_PEAK_CHANNEL_RUN,
							"source", source,
							NULL);

  return(peak_channel_run);
}
