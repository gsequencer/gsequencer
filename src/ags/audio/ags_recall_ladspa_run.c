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

#include <ags/audio/ags_recall_ladspa.h>
#include <ags/audio/ags_recall_ladspa_run.h>

#include <ags/main.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_plugin.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_port.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void ags_recall_ladspa_run_class_init(AgsRecallLadspaRunClass *recall_ladspa_run_class);
void ags_recall_ladspa_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_ladspa_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_recall_ladspa_run_init(AgsRecallLadspaRun *recall_ladspa_run);
void ags_recall_ladspa_run_connect(AgsConnectable *connectable);
void ags_recall_ladspa_run_disconnect(AgsConnectable *connectable);
void ags_recall_ladspa_run_finalize(GObject *gobject);

void ags_recall_ladspa_run_run_pre(AgsRecall *recall);
void ags_recall_ladspa_run_run_inter(AgsRecall *recall);
void ags_recall_ladspa_run_run_post(AgsRecall *recall);

static gpointer ags_recall_ladspa_run_parent_class = NULL;
static AgsConnectableInterface* ags_recall_ladspa_run_parent_connectable_interface;

GType
ags_recall_ladspa_run_get_type (void)
{
  static GType ags_type_recall_ladspa_run = 0;

  if(!ags_type_recall_ladspa_run){
    static const GTypeInfo ags_recall_ladspa_run_info = {
      sizeof (AgsRecallLadspaRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_ladspa_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallLadspaRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_ladspa_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_ladspa_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_recall_ladspa_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_ladspa_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							"AgsRecallLadspaRun\0",
							&ags_recall_ladspa_run_info,
							0);

    g_type_add_interface_static(ags_type_recall_ladspa_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_ladspa_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_recall_ladspa_run);
}

void
ags_recall_ladspa_run_class_init(AgsRecallLadspaRunClass *recall_ladspa_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_recall_ladspa_run_parent_class = g_type_class_peek_parent(recall_ladspa_run);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_ladspa_run;

  gobject->finalize = ags_recall_ladspa_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_ladspa_run;

  recall->run_pre = ags_recall_ladspa_run_run_pre;
  recall->run_inter = ags_recall_ladspa_run_run_inter;
  recall->run_post = ags_recall_ladspa_run_run_post;
}


void
ags_recall_ladspa_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_recall_ladspa_run_connect;
  connectable->disconnect = ags_recall_ladspa_run_disconnect;
}

void
ags_recall_ladspa_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  //TODO:JK: implement me
}

void
ags_recall_ladspa_run_init(AgsRecallLadspaRun *recall_ladspa_run)
{
  recall_ladspa_run->input = (LADSPA_Data *) malloc(AGS_DEVOUT_DEFAULT_BUFFER_SIZE * sizeof(LADSPA_Data));
  recall_ladspa_run->output = (LADSPA_Data *) malloc(AGS_DEVOUT_DEFAULT_BUFFER_SIZE * sizeof(LADSPA_Data));
}

void
ags_recall_ladspa_run_connect(AgsConnectable *connectable)
{
  ags_recall_ladspa_run_parent_connectable_interface->connect(connectable);
}

void
ags_recall_ladspa_run_disconnect(AgsConnectable *connectable)
{
  ags_recall_ladspa_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_recall_ladspa_run_finalize(GObject *gobject)
{
  //TODO:JK: implement me

  /* call parent */
  G_OBJECT_CLASS(ags_recall_ladspa_run_parent_class)->finalize(gobject);
}

void
ags_recall_ladspa_run_run_pre(AgsRecall *recall)
{
  AgsRecallLadspa *recall_ladspa;
  AgsRecallLadspaRun *recall_ladspa_run;
  AgsAudioSignal *audio_signal;
  guint i;

  LADSPA_PortDescriptor *port_descriptor;

  /* call parent */
  AGS_RECALL_CLASS(ags_recall_ladspa_run_parent_class)->run_pre(recall);

  /* set up buffer */
  recall_ladspa_run = AGS_RECALL_LADSPA_RUN(recall);
  audio_signal = AGS_RECALL_AUDIO_SIGNAL(recall_ladspa_run);

  memset(recall_ladspa_run->output, 0, AGS_DEVOUT_DEFAULT_BUFFER_SIZE * sizeof(LADSPA_Data));
  memset(recall_ladspa_run->input, 0, AGS_DEVOUT_DEFAULT_BUFFER_SIZE * sizeof(LADSPA_Data));

  ags_recall_ladspa_short_to_float(audio_signal->stream_current->data,
				   recall_ladspa_run->input);

  /* can't be done in ags_recall_ladspa_run_run_init_inter since possebility of overlapping buffers */
  /* connect audio port */
  recall_ladspa = AGS_RECALL_LADSPA(recall->parent->parent);

  port_descriptor = recall_ladspa->plugin_descriptor->PortDescriptors;

  recall_ladspa->plugin_descriptor->connect_port(recall_ladspa->plugin_descriptor,
						 recall_ladspa->input_port,
						 recall_ladspa_run->input);

  recall_ladspa->plugin_descriptor->connect_port(recall_ladspa->plugin_descriptor,
						 recall_ladspa->output_port,
						 recall_ladspa_run->output);
}

void
ags_recall_ladspa_run_run_inter(AgsRecall *recall)
{
  AgsRecallLadspa *recall_ladspa;
  AgsRecallLadspaRun *recall_ladspa_run;

  /* call parent */
  AGS_RECALL_CLASS(ags_recall_ladspa_run_parent_class)->run_inter(recall);

  /* process data */
  recall_ladspa = AGS_RECALL_LADSPA(recall->parent);
  recall_ladspa_run = AGS_RECALL_LADSPA_RUN(recall);

  recall_ladspa->plugin_descriptor->run(recall_ladspa->plugin_descriptor,
					AGS_DEVOUT_DEFAULT_BUFFER_SIZE);
}

void
ags_recall_ladspa_run_run_post(AgsRecall *recall)
{
  AgsRecallLadspaRun *recall_ladspa_run;
  AgsAudioSignal *audio_signal;

  /* call parent */
  AGS_RECALL_CLASS(ags_recall_ladspa_run_parent_class)->run_post(recall);

  /* copy processed data */
  recall_ladspa_run = AGS_RECALL_LADSPA_RUN(recall);
  audio_signal = AGS_RECALL_AUDIO_SIGNAL(recall_ladspa_run);

  ags_recall_ladspa_float_to_short(recall_ladspa_run->output,
				   audio_signal->stream_current->data);
}

AgsRecallLadspaRun*
ags_recall_ladspa_run_new(AgsAudioSignal *audio_signal)
{
  AgsRecallLadspaRun *recall_ladspa_run;

  recall_ladspa_run = (AgsRecallLadspaRun *) g_object_new(AGS_TYPE_RECALL_LADSPA_RUN,
							  "source\0", audio_signal,
							  NULL);

  return(recall_ladspa_run);
}
