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

#include <ags/audio/recall/ags_prepare_channel_run.h>

#include <ags/libags.h>

#include <ags/audio/recall/ags_prepare_recycling.h>

void ags_prepare_channel_run_class_init(AgsPrepareChannelRunClass *prepare_channel_run);
void ags_prepare_channel_run_init(AgsPrepareChannelRun *prepare_channel_run);
void ags_prepare_channel_run_finalize(GObject *gobject);

/**
 * SECTION:ags_prepare_channel_run
 * @short_description: prepares channel
 * @title: AgsPrepareChannelRun
 * @section_id:
 * @include: ags/audio/recall/ags_prepare_channel_run.h
 *
 * The #AgsPrepareChannelRun class prepares the channel.
 */

static gpointer ags_prepare_channel_run_parent_class = NULL;

GType
ags_prepare_channel_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_prepare_channel_run = 0;

    static const GTypeInfo ags_prepare_channel_run_info = {
      sizeof (AgsPrepareChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_prepare_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPrepareChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_prepare_channel_run_init,
    };

    ags_type_prepare_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
							  "AgsPrepareChannelRun",
							  &ags_prepare_channel_run_info,
							  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_prepare_channel_run);
  }

  return g_define_type_id__volatile;
}

void
ags_prepare_channel_run_class_init(AgsPrepareChannelRunClass *prepare_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_prepare_channel_run_parent_class = g_type_class_peek_parent(prepare_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) prepare_channel_run;

  gobject->finalize = ags_prepare_channel_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) prepare_channel_run;
}

void
ags_prepare_channel_run_init(AgsPrepareChannelRun *prepare_channel_run)
{
  ags_recall_set_ability_flags(prepare_channel_run, (AGS_SOUND_ABILITY_SEQUENCER |
						     AGS_SOUND_ABILITY_NOTATION |
						     AGS_SOUND_ABILITY_WAVE |
						     AGS_SOUND_ABILITY_MIDI));

  AGS_RECALL(prepare_channel_run)->name = "ags-prepare";
  AGS_RECALL(prepare_channel_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(prepare_channel_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(prepare_channel_run)->xml_type = "ags-prepare-channel-run";
  AGS_RECALL(prepare_channel_run)->port = NULL;

  AGS_RECALL(prepare_channel_run)->child_type = AGS_TYPE_PREPARE_RECYCLING;
}

void
ags_prepare_channel_run_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_prepare_channel_run_parent_class)->finalize(gobject);
}

/**
 * ags_prepare_channel_run_new:
 * @source: the #AgsChannel
 *
 * Create a new instance of #AgsPrepareChannelRun
 *
 * Returns: the new #AgsPrepareChannelRun
 *
 * Since: 2.0.0
 */
AgsPrepareChannelRun*
ags_prepare_channel_run_new(AgsChannel *source)
{
  AgsPrepareChannelRun *prepare_channel_run;

  prepare_channel_run = (AgsPrepareChannelRun *) g_object_new(AGS_TYPE_PREPARE_CHANNEL_RUN,
							      "source", source,
							      NULL);
  
  return(prepare_channel_run);
}
