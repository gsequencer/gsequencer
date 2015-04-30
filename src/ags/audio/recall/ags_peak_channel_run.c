/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/audio/recall/ags_peak_channel_run.h>

#include <ags/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/lib/ags_list.h>
#include <ags/lib/ags_parameter.h>

#include <ags/object/ags_dynamic_connectable.h>
#include <ags/object/ags_plugin.h>

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
void ags_peak_channel_run_run_post(AgsRecall *recall);

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
						       "AgsPeakChannelRun\0",
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
  recall->run_post = ags_peak_channel_run_run_post;
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
  AGS_RECALL(peak_channel_run)->name = "ags-peak\0";
  AGS_RECALL(peak_channel_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(peak_channel_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(peak_channel_run)->xml_type = "ags-peak-channel-run\0";
  AGS_RECALL(peak_channel_run)->port = NULL;

  AGS_RECALL(peak_channel_run)->flags |= AGS_RECALL_INPUT_ORIENTATED;
  AGS_RECALL(peak_channel_run)->child_type = G_TYPE_NONE;
}

void
ags_peak_channel_run_finalize(GObject *gobject)
{
  AgsPeakChannelRun *peak_channel_run;

  peak_channel_run = AGS_PEAK_CHANNEL_RUN(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_peak_channel_run_parent_class)->finalize(gobject);
}

void
ags_peak_channel_run_connect(AgsConnectable *connectable)
{
  AgsPeakChannelRun *peak_channel_run;

  ags_peak_channel_run_parent_connectable_interface->connect(connectable);

  peak_channel_run = AGS_PEAK_CHANNEL_RUN(connectable);
}

void
ags_peak_channel_run_disconnect(AgsConnectable *connectable)
{
  ags_peak_channel_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_peak_channel_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  ags_peak_channel_run_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);
}

void
ags_peak_channel_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  ags_peak_channel_run_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);
}

AgsRecall*
ags_peak_channel_run_duplicate(AgsRecall *recall,
			     AgsRecallID *recall_id,
			     guint *n_params, GParameter *parameter)
{
  AgsPeakChannelRun *peak_channel_run, *copy;

  peak_channel_run = (AgsPeakChannelRun *) recall;
  
  parameter = ags_parameter_grow(G_OBJECT_TYPE(recall),
				 parameter, n_params,
				 NULL);
  copy = (AgsPeakChannelRun *) AGS_RECALL_CLASS(ags_peak_channel_run_parent_class)->duplicate(recall,
											      recall_id,
											      n_params, parameter);
  
  return((AgsRecall *) copy);
}

void
ags_peak_channel_run_run_post(AgsRecall *recall)
{
  AgsChannel *source;
  AgsRecallChannel *recall_channel;
  GList *list;

  /* call parent */
  AGS_RECALL_CLASS(ags_peak_channel_run_parent_class)->run_post(recall);

  /*  */
  source = AGS_RECALL_CHANNEL_RUN(recall)->source;
  recall_channel = AGS_RECALL_CHANNEL_RUN(recall)->recall_channel;

  if(AGS_RECYCLING_CONTAINER(AGS_RECALL_ID(recall->recall_id)->recycling_container)->parent == NULL){
    list = ags_recall_find_type(source->play,
				AGS_TYPE_PEAK_CHANNEL_RUN);

    //    if(g_list_last(list) == recall){
      ags_peak_channel_retrieve_peak(recall_channel,
				     TRUE);
      //    }
  }else{
    list = ags_recall_find_type(source->recall,
				AGS_TYPE_PEAK_CHANNEL_RUN);

    //    if(g_list_last(list) == recall){
      ags_peak_channel_retrieve_peak(recall_channel,
				     FALSE);
      //    }
  }
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
							    "source\0", channel,
							    NULL);

  return(peak_channel_run);
}
