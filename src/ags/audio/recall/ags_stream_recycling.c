/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/recall/ags_stream_recycling.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/ags_devout.h>

#include <ags/audio/recall/ags_stream_audio_signal.h>

#include <ags/audio/task/ags_remove_audio_signal.h>

void ags_stream_recycling_class_init(AgsStreamRecyclingClass *stream_recycling);
void ags_stream_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_stream_recycling_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_stream_recycling_init(AgsStreamRecycling *stream_recycling);
void ags_stream_recycling_connect(AgsConnectable *connectable);
void ags_stream_recycling_disconnect(AgsConnectable *connectable);
void ags_stream_recycling_run_connect(AgsRunConnectable *run_connectable);
void ags_stream_recycling_run_disconnect(AgsRunConnectable *run_connectable);
void ags_stream_recycling_finalize(GObject *gobject);

void ags_stream_recycling_child_added(AgsRecall *recall, AgsRecall *child);
AgsRecall* ags_stream_recycling_duplicate(AgsRecall *recall,
					  AgsRecallID *recall_id,
					  guint *n_params, GParameter *parameter);
void ags_stream_recycling_remove(AgsRecall *recall);

void ags_stream_recycling_stream_audio_signal_done(AgsRecall *recall, AgsStreamRecycling *stream_recycling);

static gpointer ags_stream_recycling_parent_class = NULL;
static AgsConnectableInterface *ags_stream_recycling_parent_connectable_interface;
static AgsRunConnectableInterface *ags_stream_recycling_parent_run_connectable_interface;

GType
ags_stream_recycling_get_type()
{
  static GType ags_type_stream_recycling = 0;

  if(!ags_type_stream_recycling){
    static const GTypeInfo ags_stream_recycling_info = {
      sizeof (AgsStreamRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_stream_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsStreamRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_stream_recycling_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_stream_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_stream_recycling_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_stream_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
						       "AgsStreamRecycling\0",
						       &ags_stream_recycling_info,
						       0);

    g_type_add_interface_static(ags_type_stream_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_stream_recycling,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return (ags_type_stream_recycling);
}

void
ags_stream_recycling_class_init(AgsStreamRecyclingClass *stream_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_stream_recycling_parent_class = g_type_class_peek_parent(stream_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) stream_recycling;

  gobject->finalize = ags_stream_recycling_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) stream_recycling;

  recall->duplicate = ags_stream_recycling_duplicate;
}

void
ags_stream_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_stream_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_stream_recycling_connect;
  connectable->disconnect = ags_stream_recycling_disconnect;
}

void
ags_stream_recycling_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_stream_recycling_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_stream_recycling_run_connect;
  run_connectable->disconnect = ags_stream_recycling_run_disconnect;
}

void
ags_stream_recycling_init(AgsStreamRecycling *stream_recycling)
{
  AGS_RECALL(stream_recycling)->child_type = AGS_TYPE_STREAM_AUDIO_SIGNAL;
}

void
ags_stream_recycling_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_stream_recycling_parent_class)->finalize(gobject);
}

void
ags_stream_recycling_connect(AgsConnectable *connectable)
{
  ags_stream_recycling_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_stream_recycling_disconnect(AgsConnectable *connectable)
{
  ags_stream_recycling_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_stream_recycling_run_connect(AgsRunConnectable *run_connectable)
{
  ags_stream_recycling_parent_run_connectable_interface->connect(run_connectable);
}

void
ags_stream_recycling_run_disconnect(AgsRunConnectable *run_connectable)
{
  ags_stream_recycling_parent_run_connectable_interface->disconnect(run_connectable);
}

void
ags_stream_recycling_child_added(AgsRecall *recall, AgsRecall *child)
{
  g_signal_connect(G_OBJECT(child), "done\0",
		   G_CALLBACK(ags_stream_recycling_stream_audio_signal_done), recall);
}

AgsRecall*
ags_stream_recycling_duplicate(AgsRecall *recall,
			       AgsRecallID *recall_id,
			       guint *n_params, GParameter *parameter)
{
  AgsStreamRecycling *copy;

  copy = (AgsStreamRecycling *) AGS_RECALL_CLASS(ags_stream_recycling_parent_class)->duplicate(recall,
											       recall_id,
											       n_params, parameter);


  return((AgsRecall *) copy);
}

void
ags_stream_recycling_stream_audio_signal_done(AgsRecall *recall, AgsStreamRecycling *stream_recycling)
{
  AgsRemoveAudioSignal *remove_audio_signal;

  remove_audio_signal = ags_remove_audio_signal_new(AGS_RECALL_RECYCLING(stream_recycling)->source,
						    AGS_RECALL_AUDIO_SIGNAL(recall)->source);

  ags_devout_append_task(AGS_DEVOUT(recall->devout),
			 AGS_TASK(remove_audio_signal));
}

AgsStreamRecycling*
ags_stream_recycling_new(AgsRecycling *recycling)
{
  AgsStreamRecycling *stream_recycling;

  stream_recycling = (AgsStreamRecycling *) g_object_new(AGS_TYPE_STREAM_RECYCLING,
							 "source\0", recycling,
							 NULL);

  return(stream_recycling);
}
