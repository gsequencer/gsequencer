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

#include <ags/audio/recall/ags_copy_channel_run.h>

#include <ags/audio/recall/ags_copy_recycling.h>

#include <stdlib.h>
#include <stdio.h>

void ags_copy_channel_run_class_init(AgsCopyChannelRunClass *copy_channel_run);
void ags_copy_channel_run_init(AgsCopyChannelRun *copy_channel_run);
void ags_copy_channel_run_finalize(GObject *gobject);

/**
 * SECTION:ags_copy_channel_run
 * @short_description: copy channel
 * @title: AgsCopyChannelRun
 * @section_id:
 * @include: ags/audio/recall/ags_copy_channel_run.h
 *
 * The #AgsCopyChannelRun class copies the channel.
 */

static gpointer ags_copy_channel_run_parent_class = NULL;

GType
ags_copy_channel_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_copy_channel_run = 0;

    static const GTypeInfo ags_copy_channel_run_info = {
      sizeof(AgsCopyChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCopyChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_channel_run_init,
    };

    ags_type_copy_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
						       "AgsCopyChannelRun",
						       &ags_copy_channel_run_info,
						       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_copy_channel_run);
  }

  return g_define_type_id__volatile;
}

void
ags_copy_channel_run_class_init(AgsCopyChannelRunClass *copy_channel_run)
{
  GObjectClass *gobject;

  ags_copy_channel_run_parent_class = g_type_class_peek_parent(copy_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) copy_channel_run;

  gobject->finalize = ags_copy_channel_run_finalize;
}

void
ags_copy_channel_run_init(AgsCopyChannelRun *copy_channel_run)
{
  ags_recall_set_ability_flags((AgsRecall *) copy_channel_run, (AGS_SOUND_ABILITY_SEQUENCER |
								AGS_SOUND_ABILITY_NOTATION |
								AGS_SOUND_ABILITY_WAVE |
								AGS_SOUND_ABILITY_MIDI));
  
  AGS_RECALL(copy_channel_run)->name = "ags-copy";
  AGS_RECALL(copy_channel_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(copy_channel_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(copy_channel_run)->xml_type = "ags-copy-channel-run";
  AGS_RECALL(copy_channel_run)->port = NULL;

  AGS_RECALL(copy_channel_run)->child_type = AGS_TYPE_COPY_RECYCLING;
}

void
ags_copy_channel_run_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_copy_channel_run_parent_class)->finalize(gobject);
}

/**
 * ags_copy_channel_run_new:
 * @destination: the destination #AgsChannel
 * @source: the source #AgsChannel
 *
 * Create a new instance of #AgsCopyChannelRun
 *
 * Returns: the new #AgsCopyChannelRun
 *
 * Since: 3.0.0
 */
AgsCopyChannelRun*
ags_copy_channel_run_new(AgsChannel *destination,
			 AgsChannel *source)
{
  AgsCopyChannelRun *copy_channel_run;

  copy_channel_run = (AgsCopyChannelRun *) g_object_new(AGS_TYPE_COPY_CHANNEL_RUN,
							"destination", destination,
							"source", source,
							NULL);

  return(copy_channel_run);
}
