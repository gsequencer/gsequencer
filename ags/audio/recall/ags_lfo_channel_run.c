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

#include <ags/audio/recall/ags_lfo_channel_run.h>

#include <ags/audio/recall/ags_lfo_recycling.h>

void ags_lfo_channel_run_class_init(AgsLfoChannelRunClass *lfo_channel_run);
void ags_lfo_channel_run_init(AgsLfoChannelRun *lfo_channel_run);
void ags_lfo_channel_run_finalize(GObject *gobject);

/**
 * SECTION:ags_lfo_channel_run
 * @short_description: LFO channel
 * @title: AgsLfoChannelRun
 * @section_id:
 * @include: ags/audio/recall/ags_lfo_channel_run.h
 *
 * The #AgsLfoChannelRun LFO the channel.
 */

static gpointer ags_lfo_channel_run_parent_class = NULL;

GType
ags_lfo_channel_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_lfo_channel_run = 0;

    static const GTypeInfo ags_lfo_channel_run_info = {
      sizeof (AgsLfoChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lfo_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLfoChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lfo_channel_run_init,
    };

    ags_type_lfo_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
						      "AgsLfoChannelRun",
						      &ags_lfo_channel_run_info,
						      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_lfo_channel_run);
  }

  return g_define_type_id__volatile;
}

void
ags_lfo_channel_run_class_init(AgsLfoChannelRunClass *lfo_channel_run)
{
  GObjectClass *gobject;

  ags_lfo_channel_run_parent_class = g_type_class_peek_parent(lfo_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) lfo_channel_run;

  gobject->finalize = ags_lfo_channel_run_finalize;
}

void
ags_lfo_channel_run_init(AgsLfoChannelRun *lfo_channel_run)
{
  ags_recall_set_ability_flags((AgsRecall *) lfo_channel_run, (AGS_SOUND_ABILITY_PLAYBACK |
							       AGS_SOUND_ABILITY_SEQUENCER |
							       AGS_SOUND_ABILITY_NOTATION |
							       AGS_SOUND_ABILITY_WAVE |
							       AGS_SOUND_ABILITY_MIDI));

  AGS_RECALL(lfo_channel_run)->name = "ags-lfo";
  AGS_RECALL(lfo_channel_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(lfo_channel_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(lfo_channel_run)->xml_type = "ags-lfo-channel-run";
  AGS_RECALL(lfo_channel_run)->port = NULL;

  AGS_RECALL(lfo_channel_run)->child_type = AGS_TYPE_LFO_RECYCLING;
}

void
ags_lfo_channel_run_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_lfo_channel_run_parent_class)->finalize(gobject);
}

/**
 * ags_lfo_channel_run_new:
 * @source: the #AgsChannel
 *
 * Create a new instance of #AgsLfoChannelRun
 *
 * Returns: the new #AgsLfoChannelRun
 *
 * Since: 2.3.0
 */
AgsLfoChannelRun*
ags_lfo_channel_run_new(AgsChannel *source)
{
  AgsLfoChannelRun *lfo_channel_run;

  lfo_channel_run = (AgsLfoChannelRun *) g_object_new(AGS_TYPE_LFO_CHANNEL_RUN,
						      "source", source,
						      NULL);

  return(lfo_channel_run);
}
