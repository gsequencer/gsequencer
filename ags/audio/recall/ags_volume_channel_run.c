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

#include <ags/audio/recall/ags_volume_channel_run.h>

#include <ags/libags.h>

#include <ags/audio/recall/ags_volume_recycling.h>

void ags_volume_channel_run_class_init(AgsVolumeChannelRunClass *volume_channel_run);
void ags_volume_channel_run_init(AgsVolumeChannelRun *volume_channel_run);
void ags_volume_channel_run_finalize(GObject *gobject);

/**
 * SECTION:ags_volume_channel_run
 * @short_description: volumes channel
 * @title: AgsVolumeChannelRun
 * @section_id:
 * @include: ags/audio/recall/ags_volume_channel_run.h
 *
 * The #AgsVolumeChannelRun class volumes the channel.
 */

static gpointer ags_volume_channel_run_parent_class = NULL;

GType
ags_volume_channel_run_get_type()
{
  static GType ags_type_volume_channel_run = 0;

  if(!ags_type_volume_channel_run){
    static const GTypeInfo ags_volume_channel_run_info = {
      sizeof (AgsVolumeChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_volume_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsVolumeChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_volume_channel_run_init,
    };

    ags_type_volume_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
							 "AgsVolumeChannelRun",
							 &ags_volume_channel_run_info,
							 0);
  }

  return (ags_type_volume_channel_run);
}

void
ags_volume_channel_run_class_init(AgsVolumeChannelRunClass *volume_channel_run)
{
  GObjectClass *gobject;

  ags_volume_channel_run_parent_class = g_type_class_peek_parent(volume_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) volume_channel_run;

  gobject->finalize = ags_volume_channel_run_finalize;
}

void
ags_volume_channel_run_init(AgsVolumeChannelRun *volume_channel_run)
{
  ags_recall_set_ability_flags(volume_channel_run, (AGS_SOUND_ABILITY_PLAYBACK |
						    AGS_SOUND_ABILITY_SEQUENCER |
						    AGS_SOUND_ABILITY_NOTATION |
						    AGS_SOUND_ABILITY_WAVE |
						    AGS_SOUND_ABILITY_MIDI));

  AGS_RECALL(volume_channel_run)->name = "ags-volume";
  AGS_RECALL(volume_channel_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(volume_channel_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(volume_channel_run)->xml_type = "ags-volume-channel-run";
  AGS_RECALL(volume_channel_run)->port = NULL;

  AGS_RECALL(volume_channel_run)->child_type = AGS_TYPE_VOLUME_RECYCLING;
}

void
ags_volume_channel_run_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_volume_channel_run_parent_class)->finalize(gobject);
}

/**
 * ags_volume_channel_run_new:
 * @source: the #AgsChannel
 *
 * Create a new instance of #AgsVolumeChannelRun
 *
 * Returns: the new #AgsVolumeChannelRun
 *
 * Since: 2.0.0
 */
AgsVolumeChannelRun*
ags_volume_channel_run_new(AgsChannel *source)
{
  AgsVolumeChannelRun *volume_channel_run;

  volume_channel_run = (AgsVolumeChannelRun *) g_object_new(AGS_TYPE_VOLUME_CHANNEL_RUN,
							    "source", source,
							    NULL);

  return(volume_channel_run);
}
