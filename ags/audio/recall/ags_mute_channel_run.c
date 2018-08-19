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

#include <ags/audio/recall/ags_mute_channel_run.h>

#include <ags/libags.h>

#include <ags/audio/recall/ags_mute_recycling.h>

void ags_mute_channel_run_class_init(AgsMuteChannelRunClass *mute_channel_run);
void ags_mute_channel_run_init(AgsMuteChannelRun *mute_channel_run);
void ags_mute_channel_run_finalize(GObject *gobject);

static gpointer ags_mute_channel_run_parent_class = NULL;

GType
ags_mute_channel_run_get_type()
{
  static GType ags_type_mute_channel_run = 0;

  if(!ags_type_mute_channel_run){
    static const GTypeInfo ags_mute_channel_run_info = {
      sizeof (AgsMuteChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_mute_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMuteChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_mute_channel_run_init,
    };

    ags_type_mute_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
						       "AgsMuteChannelRun",
						       &ags_mute_channel_run_info,
						       0);
  }

  return(ags_type_mute_channel_run);
}

void
ags_mute_channel_run_class_init(AgsMuteChannelRunClass *mute_channel_run)
{
  GObjectClass *gobject;

  ags_mute_channel_run_parent_class = g_type_class_peek_parent(mute_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) mute_channel_run;

  gobject->finalize = ags_mute_channel_run_finalize;
}

void
ags_mute_channel_run_init(AgsMuteChannelRun *mute_channel_run)
{
  ags_recall_set_ability_flags(mute_channel_run, (AGS_SOUND_ABILITY_SEQUENCER |
						  AGS_SOUND_ABILITY_NOTATION |
						  AGS_SOUND_ABILITY_WAVE |
						  AGS_SOUND_ABILITY_MIDI));

  AGS_RECALL(mute_channel_run)->name = "ags-mute";
  AGS_RECALL(mute_channel_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(mute_channel_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(mute_channel_run)->xml_type = "ags-mute-channel-run";

  AGS_RECALL(mute_channel_run)->child_type = AGS_TYPE_MUTE_RECYCLING;
}

void
ags_mute_channel_run_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_mute_channel_run_parent_class)->finalize(gobject);
}

/**
 * ags_mute_channel_new:
 * @source: the #AgsChannel
 *
 * Create a new instance of #AgsMuteChannelRun
 *
 * Returns: the new #AgsMuteChannelRun
 *
 * Since: 2.0.0
 */
AgsMuteChannelRun*
ags_mute_channel_run_new(AgsChannel *source)
{
  AgsMuteChannelRun *mute_channel_run;

  mute_channel_run = (AgsMuteChannelRun *) g_object_new(AGS_TYPE_MUTE_CHANNEL_RUN,
							"source", source,
							NULL);

  return(mute_channel_run);
}
