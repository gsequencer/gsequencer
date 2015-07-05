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

#include <ags/audio/recall/ags_volume_audio_signal.h>
#include <ags/audio/recall/ags_volume_channel.h>

#include <ags/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/lib/ags_list.h>
#include <ags/lib/ags_parameter.h>

#include <ags/object/ags_dynamic_connectable.h>

#include <ags/audio/ags_recall_channel_run.h>

void ags_volume_audio_signal_class_init(AgsVolumeAudioSignalClass *volume_audio_signal);
void ags_volume_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_volume_audio_signal_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_volume_audio_signal_init(AgsVolumeAudioSignal *volume_audio_signal);
void ags_volume_audio_signal_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec);
void ags_volume_audio_signal_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec);
void ags_volume_audio_signal_connect(AgsConnectable *connectable);
void ags_volume_audio_signal_disconnect(AgsConnectable *connectable);
void ags_volume_audio_signal_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_volume_audio_signal_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_volume_audio_signal_finalize(GObject *gobject);

void ags_volume_audio_signal_run_inter(AgsRecall *recall);
AgsRecall* ags_volume_audio_signal_duplicate(AgsRecall *recall,
					     AgsRecallID *recall_id,
					     guint *n_params, GParameter *parameter);

/**
 * SECTION:ags_volume_audio_signal
 * @short_description: volumes audio signal
 * @title: AgsVolumeAudioSignal
 * @section_id:
 * @include: ags/audio/recall/ags_volume_audio_signal.h
 *
 * The #AgsVolumeAudioSignal class volumes the audio signal.
 */

enum{
  PROP_0,
  PROP_VOLUME,
};

static gpointer ags_volume_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_volume_audio_signal_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_volume_audio_signal_parent_dynamic_connectable_interface;

GType
ags_volume_audio_signal_get_type()
{
  static GType ags_type_volume_audio_signal = 0;

  if(!ags_type_volume_audio_signal){
    static const GTypeInfo ags_volume_audio_signal_info = {
      sizeof (AgsVolumeAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_volume_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsVolumeAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_volume_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_volume_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_volume_audio_signal_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_volume_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							  "AgsVolumeAudioSignal\0",
							  &ags_volume_audio_signal_info,
							  0);

    g_type_add_interface_static(ags_type_volume_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_volume_audio_signal,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return (ags_type_volume_audio_signal);
}

void
ags_volume_audio_signal_class_init(AgsVolumeAudioSignalClass *volume_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_volume_audio_signal_parent_class = g_type_class_peek_parent(volume_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) volume_audio_signal;

  gobject->finalize = ags_volume_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) volume_audio_signal;

  recall->run_inter = ags_volume_audio_signal_run_inter;
}

void
ags_volume_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_volume_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_volume_audio_signal_connect;
  connectable->disconnect = ags_volume_audio_signal_disconnect;
}

void
ags_volume_audio_signal_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_volume_audio_signal_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_volume_audio_signal_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_volume_audio_signal_disconnect_dynamic;
}

void
ags_volume_audio_signal_init(AgsVolumeAudioSignal *volume_audio_signal)
{
  AGS_RECALL(volume_audio_signal)->name = "ags-volume\0";
  AGS_RECALL(volume_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(volume_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(volume_audio_signal)->xml_type = "ags-volume-audio-signal\0";
  AGS_RECALL(volume_audio_signal)->port = NULL;
}

void
ags_volume_audio_signal_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_volume_audio_signal_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_volume_audio_signal_connect(AgsConnectable *connectable)
{
  /* call parent */
  ags_volume_audio_signal_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_volume_audio_signal_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_volume_audio_signal_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_volume_audio_signal_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_volume_audio_signal_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_volume_audio_signal_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_volume_audio_signal_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_volume_audio_signal_run_inter(AgsRecall *recall)
{
  AgsVolumeAudioSignal *volume_audio_signal;

  AGS_RECALL_CLASS(ags_volume_audio_signal_parent_class)->run_inter(recall);

  volume_audio_signal = AGS_VOLUME_AUDIO_SIGNAL(recall);

  if(AGS_RECALL_AUDIO_SIGNAL(recall)->source->stream_current != NULL){
    AgsVolumeChannel *volume_channel;
    signed short *buffer;
    gdouble volume;
    guint buffer_size;
    guint i;
    GValue value = {0,};

    volume_channel = AGS_VOLUME_CHANNEL(AGS_RECALL_CHANNEL_RUN(recall->parent->parent)->recall_channel);

    buffer = (signed short *) AGS_RECALL_AUDIO_SIGNAL(recall)->source->stream_current->data;
    buffer_size = AGS_RECALL_AUDIO_SIGNAL(recall)->source->buffer_size;

    g_value_init(&value, G_TYPE_DOUBLE);
    ags_port_safe_read(volume_channel->volume, &value);

    volume = g_value_get_double(&value);

    for(i = 0; i < buffer_size; i++){
      buffer[i] = (signed short) ((0xffff) & (int)((gdouble)volume * (gdouble)buffer[i]));
    }
  }else{
    ags_recall_done(recall);
  }
}

/**
 * ags_volume_audio_signal_new:
 * @audio_signal: an #AgsAudioSignal
 *
 * Creates an #AgsVolumeAudioSignal
 *
 * Returns: a new #AgsVolumeAudioSignal
 *
 * Since: 0.4
 */
AgsVolumeAudioSignal*
ags_volume_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsVolumeAudioSignal *volume_audio_signal;

  volume_audio_signal = (AgsVolumeAudioSignal *) g_object_new(AGS_TYPE_VOLUME_AUDIO_SIGNAL,
							      NULL);

  return(volume_audio_signal);
}
