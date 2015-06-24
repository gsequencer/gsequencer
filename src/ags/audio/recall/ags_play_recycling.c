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

#include <ags/audio/recall/ags_play_recycling.h>

#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_dynamic_connectable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>

#include <ags/audio/recall/ags_play_audio_signal.h>

#include <stdlib.h>
#include <stdio.h>

void ags_play_recycling_class_init(AgsPlayRecyclingClass *play_recycling);
void ags_play_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_play_recycling_init(AgsPlayRecycling *play_recycling);
void ags_play_recycling_connect(AgsConnectable *connectable);
void ags_play_recycling_disconnect(AgsConnectable *connectable);
void ags_play_recycling_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_play_recycling_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_play_recycling_finalize(GObject *gobject);

void ags_play_recycling_done(AgsRecall *recall);
void ags_play_recycling_cancel(AgsRecall *recall);
AgsRecall* ags_play_recycling_duplicate(AgsRecall *recall,
					AgsRecallID *recall_id,
					guint *n_params, GParameter *parameter);

/**
 * SECTION:ags_play_recycling
 * @short_description: plays recycling
 * @title: AgsPlayRecycling
 * @section_id:
 * @include: ags/audio/recall/ags_play_recycling.h
 *
 * The #AgsPlayRecycling class plays the recycling.
 */

static gpointer ags_play_recycling_parent_class = NULL;
static AgsConnectableInterface *ags_play_recycling_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_play_recycling_parent_dynamic_connectable_interface;

GType
ags_play_recycling_get_type()
{
  static GType ags_type_play_recycling = 0;

  if(!ags_type_play_recycling){
    static const GTypeInfo ags_play_recycling_info = {
      sizeof (AgsPlayRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_recycling_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_recycling_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
						     "AgsPlayRecycling\0",
						     &ags_play_recycling_info,
						     0);

    g_type_add_interface_static(ags_type_play_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_recycling,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return(ags_type_play_recycling);
}

void
ags_play_recycling_class_init(AgsPlayRecyclingClass *play_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_play_recycling_parent_class = g_type_class_peek_parent(play_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) play_recycling;

  gobject->finalize = ags_play_recycling_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) play_recycling;

  recall->done = ags_play_recycling_done;
  recall->cancel = ags_play_recycling_cancel;
  recall->duplicate = ags_play_recycling_duplicate;
}

void
ags_play_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_play_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_play_recycling_connect;
  connectable->disconnect = ags_play_recycling_disconnect;
}

void
ags_play_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_play_recycling_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_play_recycling_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_play_recycling_disconnect_dynamic;
}

void
ags_play_recycling_init(AgsPlayRecycling *play_recycling)
{
  AGS_RECALL(play_recycling)->child_type = AGS_TYPE_PLAY_AUDIO_SIGNAL;
}

void
ags_play_recycling_connect(AgsConnectable *connectable)
{
  /* call parent */
  ags_play_recycling_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_play_recycling_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_play_recycling_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_play_recycling_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_play_recycling_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_play_recycling_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_play_recycling_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_play_recycling_finalize(GObject *gobject)
{
  AgsPlayRecycling *play_recycling;

  play_recycling = AGS_PLAY_RECYCLING(gobject);

  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_play_recycling_parent_class)->finalize(gobject);
}

void 
ags_play_recycling_done(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_play_recycling_parent_class)->done(recall);

  /* empty */
}

void
ags_play_recycling_cancel(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_play_recycling_parent_class)->cancel(recall);

  /* empty */
}

AgsRecall*
ags_play_recycling_duplicate(AgsRecall *recall,
			     AgsRecallID *recall_id,
			     guint *n_params, GParameter *parameter)
{
  AgsPlayRecycling *copy;

  copy = (AgsPlayRecycling *) AGS_RECALL_CLASS(ags_play_recycling_parent_class)->duplicate(recall,
											   recall_id,
											   n_params, parameter);
 return((AgsRecall *) copy);
}

/**
 * ags_play_recycling_new:
 * @source: the source #AgsRecycling
 * @devout: the #AgsDevout outputting to
 * @audio_channel: the audio channel to use
 *
 * Creates an #AgsPlayRecycling
 *
 * Returns: a new #AgsPlayRecycling
 *
 * Since: 0.4
 */
AgsPlayRecycling*
ags_play_recycling_new(AgsRecycling *source,
		       AgsDevout *devout,
		       guint audio_channel)
{
  AgsPlayRecycling *play_recycling;

  play_recycling = (AgsPlayRecycling *) g_object_new(AGS_TYPE_PLAY_RECYCLING,
						     "source\0", source,
						     "devout\0", devout,
						     "audio_channel\0", audio_channel,
						     NULL);

  return(play_recycling);
}
