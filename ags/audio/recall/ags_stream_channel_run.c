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

#include <ags/audio/recall/ags_stream_channel_run.h>

#include <ags/libags.h>

#include <ags/audio/recall/ags_stream_recycling.h>

void ags_stream_channel_run_class_init(AgsStreamChannelRunClass *stream_channel_run);
void ags_stream_channel_run_init(AgsStreamChannelRun *stream_channel_run);
void ags_stream_channel_run_finalize(GObject *gobject);

/**
 * SECTION:ags_stream_channel_run
 * @short_description: streams channel
 * @title: AgsStreamChannelRun
 * @section_id:
 * @include: ags/audio/recall/ags_stream_channel_run.h
 *
 * The #AgsStreamChannelRun class streams the channel.
 */

static gpointer ags_stream_channel_run_parent_class = NULL;

GType
ags_stream_channel_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_stream_channel_run = 0;

    static const GTypeInfo ags_stream_channel_run_info = {
      sizeof (AgsStreamChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_stream_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsStreamChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_stream_channel_run_init,
    };

    ags_type_stream_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
							 "AgsStreamChannelRun",
							 &ags_stream_channel_run_info,
							 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_stream_channel_run);
  }

  return g_define_type_id__volatile;
}

void
ags_stream_channel_run_class_init(AgsStreamChannelRunClass *stream_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_stream_channel_run_parent_class = g_type_class_peek_parent(stream_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) stream_channel_run;

  gobject->finalize = ags_stream_channel_run_finalize;
}

void
ags_stream_channel_run_init(AgsStreamChannelRun *stream_channel_run)
{
  ags_recall_set_ability_flags(stream_channel_run, (AGS_SOUND_ABILITY_PLAYBACK |
						    AGS_SOUND_ABILITY_SEQUENCER |
						    AGS_SOUND_ABILITY_NOTATION |
						    AGS_SOUND_ABILITY_WAVE |
						    AGS_SOUND_ABILITY_MIDI));

  AGS_RECALL(stream_channel_run)->name = "ags-stream";
  AGS_RECALL(stream_channel_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(stream_channel_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(stream_channel_run)->xml_type = "ags-stream-channel-run";
  AGS_RECALL(stream_channel_run)->port = NULL;

  AGS_RECALL(stream_channel_run)->child_type = AGS_TYPE_STREAM_RECYCLING;
}

void
ags_stream_channel_run_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_stream_channel_run_parent_class)->finalize(gobject);
}

/**
 * ags_stream_channel_run_new:
 * @source: the #AgsChannel
 *
 * Create a new instance of #AgsStreamChannelRun
 *
 * Returns: the new #AgsStreamChannelRun
 *
 * Since: 2.0.0
 */
AgsStreamChannelRun*
ags_stream_channel_run_new(AgsChannel *source)
{
  AgsStreamChannelRun *stream_channel_run;

  stream_channel_run = (AgsStreamChannelRun *) g_object_new(AGS_TYPE_STREAM_CHANNEL_RUN,
							    "source", source,
							    NULL);
  
  return(stream_channel_run);
}
