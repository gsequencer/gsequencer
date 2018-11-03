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

#include <ags/audio/recall/ags_eq10_channel_run.h>
#include <ags/audio/recall/ags_eq10_recycling.h>

#include <ags/libags.h>

void ags_eq10_channel_run_class_init(AgsEq10ChannelRunClass *eq10_channel_run);
void ags_eq10_channel_run_init(AgsEq10ChannelRun *eq10_channel_run);
void ags_eq10_channel_run_finalize(GObject *gobject);

/**
 * SECTION:ags_eq10_channel_run
 * @short_description: 10 band equalizer on channel
 * @title: AgsEq10ChannelRun
 * @section_id:
 * @include: ags/audio/recall/ags_eq10_channel_run.h
 *
 * The #AgsEq10ChannelRun class does a 10 band equalizer on the channel.
 */

static gpointer ags_eq10_channel_run_parent_class = NULL;

GType
ags_eq10_channel_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_eq10_channel_run = 0;

    static const GTypeInfo ags_eq10_channel_run_info = {
      sizeof(AgsEq10ChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_eq10_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsEq10ChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_eq10_channel_run_init,
    };

    ags_type_eq10_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
						       "AgsEq10ChannelRun",
						       &ags_eq10_channel_run_info,
						       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_eq10_channel_run);
  }

  return g_define_type_id__volatile;
}

void
ags_eq10_channel_run_class_init(AgsEq10ChannelRunClass *eq10_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_eq10_channel_run_parent_class = g_type_class_peek_parent(eq10_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) eq10_channel_run;

  gobject->finalize = ags_eq10_channel_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) eq10_channel_run;
}

void
ags_eq10_channel_run_init(AgsEq10ChannelRun *eq10_channel_run)
{
  ags_recall_set_ability_flags(eq10_channel_run, (AGS_SOUND_ABILITY_PLAYBACK |
						  AGS_SOUND_ABILITY_SEQUENCER |
						  AGS_SOUND_ABILITY_NOTATION |
						  AGS_SOUND_ABILITY_WAVE |
						  AGS_SOUND_ABILITY_MIDI));

  AGS_RECALL(eq10_channel_run)->name = "ags-eq10";
  AGS_RECALL(eq10_channel_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(eq10_channel_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(eq10_channel_run)->xml_type = "ags-eq10-channel-run";
  AGS_RECALL(eq10_channel_run)->port = NULL;

  AGS_RECALL(eq10_channel_run)->child_type = AGS_TYPE_EQ10_RECYCLING;
}

void
ags_eq10_channel_run_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_eq10_channel_run_parent_class)->finalize(gobject);
}

/**
 * ags_eq10_channel_run_new:
 * @source: the #AgsChannel
 *
 * Create a new instance of #AgsEq10ChannelRun
 *
 * Returns: the new #AgsEq10ChannelRun
 *
 * Since: 2.0.0
 */
AgsEq10ChannelRun*
ags_eq10_channel_run_new(AgsChannel *source)
{
  AgsEq10ChannelRun *eq10_channel_run;

  eq10_channel_run = (AgsEq10ChannelRun *) g_object_new(AGS_TYPE_EQ10_CHANNEL_RUN,
							"source", source,
							NULL);

  return(eq10_channel_run);
}
