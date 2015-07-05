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

#include <ags/audio/recall/ags_peak_audio_signal.h>
#include <ags/audio/recall/ags_peak_channel.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/lib/ags_list.h>
#include <ags/lib/ags_parameter.h>

#include <ags/object/ags_dynamic_connectable.h>

#include <ags/audio/ags_recall_channel_run.h>

void ags_peak_audio_signal_class_init(AgsPeakAudioSignalClass *peak_audio_signal);
void ags_peak_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_peak_audio_signal_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_peak_audio_signal_init(AgsPeakAudioSignal *peak_audio_signal);
void ags_peak_audio_signal_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_peak_audio_signal_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_peak_audio_signal_connect(AgsConnectable *connectable);
void ags_peak_audio_signal_disconnect(AgsConnectable *connectable);
void ags_peak_audio_signal_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_peak_audio_signal_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_peak_audio_signal_finalize(GObject *gobject);

void ags_peak_audio_signal_run_inter(AgsRecall *recall);
AgsRecall* ags_peak_audio_signal_duplicate(AgsRecall *recall,
					   AgsRecallID *recall_id,
					   guint *n_params, GParameter *parameter);

/**
 * SECTION:ags_peak_audio_signal
 * @short_description: peaks audio signal
 * @title: AgsPeakAudioSignal
 * @section_id:
 * @include: ags/audio/recall/ags_peak_audio_signal.h
 *
 * The #AgsPeakAudioSignal class peaks the audio signal.
 */

enum{
  PROP_0,
  PROP_PEAK,
};

static gpointer ags_peak_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_peak_audio_signal_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_peak_audio_signal_parent_dynamic_connectable_interface;

GType
ags_peak_audio_signal_get_type()
{
  static GType ags_type_peak_audio_signal = 0;

  if(!ags_type_peak_audio_signal){
    static const GTypeInfo ags_peak_audio_signal_info = {
      sizeof (AgsPeakAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_peak_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPeakAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_peak_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_peak_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_peak_audio_signal_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_peak_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							"AgsPeakAudioSignal\0",
							&ags_peak_audio_signal_info,
							0);

    g_type_add_interface_static(ags_type_peak_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_peak_audio_signal,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return (ags_type_peak_audio_signal);
}

void
ags_peak_audio_signal_class_init(AgsPeakAudioSignalClass *peak_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_peak_audio_signal_parent_class = g_type_class_peek_parent(peak_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) peak_audio_signal;

  gobject->finalize = ags_peak_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) peak_audio_signal;

  recall->run_inter = ags_peak_audio_signal_run_inter;
}

void
ags_peak_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_peak_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_peak_audio_signal_connect;
  connectable->disconnect = ags_peak_audio_signal_disconnect;
}

void
ags_peak_audio_signal_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_peak_audio_signal_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_peak_audio_signal_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_peak_audio_signal_disconnect_dynamic;
}

void
ags_peak_audio_signal_init(AgsPeakAudioSignal *peak_audio_signal)
{
  AGS_RECALL(peak_audio_signal)->name = "ags-peak\0";
  AGS_RECALL(peak_audio_signal)->version = AGS_EFFECTS_DEFAULT_VERSION;
  AGS_RECALL(peak_audio_signal)->build_id = AGS_BUILD_ID;
  AGS_RECALL(peak_audio_signal)->xml_type = "ags-peak-audio-signal\0";
  AGS_RECALL(peak_audio_signal)->port = NULL;
}

void
ags_peak_audio_signal_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_peak_audio_signal_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_peak_audio_signal_connect(AgsConnectable *connectable)
{
  if((AGS_RECALL_CONNECTED & (AGS_RECALL(connectable)->flags)) != 0){
    return;
  }

  /* call parent */
  ags_peak_audio_signal_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_peak_audio_signal_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_peak_audio_signal_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_peak_audio_signal_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  if((AGS_RECALL_DYNAMIC_CONNECTED & (AGS_RECALL(dynamic_connectable)->flags)) != 0){
    return;
  }

  /* call parent */
  ags_peak_audio_signal_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_peak_audio_signal_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_peak_audio_signal_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_peak_audio_signal_run_inter(AgsRecall *recall)
{
  AgsPeakAudioSignal *peak_audio_signal;

  AGS_RECALL_CLASS(ags_peak_audio_signal_parent_class)->run_inter(recall);

  peak_audio_signal = AGS_PEAK_AUDIO_SIGNAL(recall);

  if(AGS_RECALL_AUDIO_SIGNAL(recall)->source->stream_current != NULL){
    AgsPeakChannel *peak_channel;
    signed short *buffer;
    gdouble peak;
    guint buffer_size;
    guint i;
    GValue value = {0,};

    peak_channel = AGS_PEAK_CHANNEL(AGS_RECALL_CHANNEL_RUN(recall->parent->parent)->recall_channel);

    ags_peak_channel_retrieve_peak(peak_channel,
				   TRUE);
  }else{
    ags_recall_done(recall);
  }
}

/**
 * ags_peak_audio_signal_new:
 * @audio_signal: an #AgsAudioSignal
 *
 * Creates an #AgsPeakAudioSignal
 *
 * Returns: a new #AgsPeakAudioSignal
 *
 * Since: 0.4
 */
AgsPeakAudioSignal*
ags_peak_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsPeakAudioSignal *peak_audio_signal;

  peak_audio_signal = (AgsPeakAudioSignal *) g_object_new(AGS_TYPE_PEAK_AUDIO_SIGNAL,
							  NULL);

  return(peak_audio_signal);
}
