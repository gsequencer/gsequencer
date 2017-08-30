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

#include <ags/audio/recall/ags_peak_channel_run.h>
#include <ags/audio/recall/ags_peak_channel.h>
#include <ags/audio/recall/ags_peak_recycling.h>

#include <ags/lib/ags_parameter.h>

#include <ags/object/ags_connectable.h>

#include <ags/object/ags_dynamic_connectable.h>
#include <ags/object/ags_plugin.h>
#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall_id.h>

void ags_peak_channel_run_class_init(AgsPeakChannelRunClass *peak_channel_run);
void ags_peak_channel_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_peak_channel_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_peak_channel_run_init(AgsPeakChannelRun *peak_channel_run);
void ags_peak_channel_run_connect(AgsConnectable *connectable);
void ags_peak_channel_run_disconnect(AgsConnectable *connectable);
void ags_peak_channel_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_peak_channel_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_peak_channel_run_finalize(GObject *gobject);

AgsRecall* ags_peak_channel_run_duplicate(AgsRecall *recall,
					  AgsRecallID *recall_id,
					  guint *n_params, GParameter *parameter);
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
static AgsConnectableInterface *ags_peak_channel_run_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_peak_channel_run_parent_dynamic_connectable_interface;

GType
ags_peak_channel_run_get_type()
{
  static GType ags_type_peak_channel_run = 0;

  if(!ags_type_peak_channel_run){
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

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_peak_channel_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_peak_channel_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_peak_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
						       "AgsPeakChannelRun",
						       &ags_peak_channel_run_info,
						       0);
    
    g_type_add_interface_static(ags_type_peak_channel_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_peak_channel_run,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return (ags_type_peak_channel_run);
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

  recall->duplicate = ags_peak_channel_run_duplicate;
  recall->run_pre = ags_peak_channel_run_run_pre;
}

void
ags_peak_channel_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_peak_channel_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_peak_channel_run_connect;
  connectable->disconnect = ags_peak_channel_run_disconnect;
}

void
ags_peak_channel_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_peak_channel_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_peak_channel_run_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_peak_channel_run_disconnect_dynamic;
}

void
ags_peak_channel_run_init(AgsPeakChannelRun *peak_channel_run)
{
  AGS_RECALL(peak_channel_run)->name = "ags-peak";
  AGS_RECALL(peak_channel_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(peak_channel_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(peak_channel_run)->xml_type = "ags-peak-channel-run";
  AGS_RECALL(peak_channel_run)->port = NULL;

  AGS_RECALL(peak_channel_run)->flags |= AGS_RECALL_INPUT_ORIENTATED;
  AGS_RECALL(peak_channel_run)->child_type = AGS_TYPE_PEAK_RECYCLING;
}

void
ags_peak_channel_run_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_peak_channel_run_parent_class)->finalize(gobject);
}

void
ags_peak_channel_run_connect(AgsConnectable *connectable)
{
  /* call parent */
  ags_peak_channel_run_parent_connectable_interface->connect(connectable);
}

void
ags_peak_channel_run_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_peak_channel_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_peak_channel_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_peak_channel_run_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);
}

void
ags_peak_channel_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_peak_channel_run_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);
}

AgsRecall*
ags_peak_channel_run_duplicate(AgsRecall *recall,
			     AgsRecallID *recall_id,
			     guint *n_params, GParameter *parameter)
{
  AgsPeakChannelRun *copy;

  copy = (AgsPeakChannelRun *) AGS_RECALL_CLASS(ags_peak_channel_run_parent_class)->duplicate(recall,
											      recall_id,
											      n_params, parameter);
  
  return((AgsRecall *) copy);
}

void
ags_peak_channel_run_run_pre(AgsRecall *recall)
{
  AgsPeakChannel *peak_channel;

  gboolean buffer_computed;
  
  GValue value = {0,};

  AGS_RECALL_CLASS(ags_peak_channel_run_parent_class)->run_pre(recall);

  /* calculate of previous run */
  peak_channel = AGS_RECALL_CHANNEL_RUN(recall)->recall_channel;

  pthread_mutex_lock(peak_channel->buffer_mutex);
  
  g_value_init(&value,
	       G_TYPE_BOOLEAN);

  ags_port_safe_read(peak_channel->buffer_computed,
		     &value);
  buffer_computed = g_value_get_boolean(&value);

  if(!buffer_computed){    
    /* set buffer-computed port to TRUE */
    g_value_set_boolean(&value,
			TRUE);

    ags_port_safe_write(peak_channel->buffer_computed,
			&value);
  }
  
  pthread_mutex_unlock(peak_channel->buffer_mutex);

  /* lock free - buffer-computed reset by cyclic-task AgsResetPeak */
  if(!buffer_computed){
    /* retrieve peak */    
    ags_peak_channel_retrieve_peak_internal(peak_channel);

    /* clear buffer */
    ags_audio_buffer_util_clear_buffer(peak_channel->buffer, 1,
				       peak_channel->buffer_size, ags_audio_buffer_util_format_from_soundcard(peak_channel->format));
  }

  g_value_unset(&value);
}

/**
 * ags_peak_channel_run_new:
 * @channel: the #AgsChannel as source
 *
 * Creates an #AgsPeakChannelRun
 *
 * Returns: a new #AgsPeakChannelRun
 *
 * Since: 0.4
 */
AgsPeakChannelRun*
ags_peak_channel_run_new(AgsChannel *channel)
{
  AgsPeakChannelRun *peak_channel_run;

  peak_channel_run = (AgsPeakChannelRun *) g_object_new(AGS_TYPE_PEAK_CHANNEL_RUN,
							    "source", channel,
							    NULL);

  return(peak_channel_run);
}
