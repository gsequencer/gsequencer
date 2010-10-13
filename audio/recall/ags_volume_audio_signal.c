#include "ags_volume_audio_signal.h"

#include "../../object/ags_connectable.h"
#include "../../object/ags_run_connectable.h"

#include "ags_volume_channel.h"

void ags_volume_audio_signal_class_init(AgsVolumeAudioSignalClass *volume_audio_signal);
void ags_volume_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_volume_audio_signal_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_volume_audio_signal_init(AgsVolumeAudioSignal *volume_audio_signal);
void ags_volume_audio_signal_connect(AgsConnectable *connectable);
void ags_volume_audio_signal_disconnect(AgsConnectable *connectable);
void ags_volume_audio_signal_run_connect(AgsRunConnectable *run_connectable);
void ags_volume_audio_signal_run_disconnect(AgsRunConnectable *run_connectable);
void ags_volume_audio_signal_finalize(GObject *gobject);

void ags_volume_audio_signal_run_inter(AgsRecall *recall, guint audio_channel, gpointer data);

AgsRecall* ags_volume_audio_signal_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

static gpointer ags_volume_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_volume_audio_signal_parent_connectable_interface;
static AgsRunConnectableInterface *ags_volume_audio_signal_parent_run_connectable_interface;

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

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_volume_audio_signal_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_volume_audio_signal = g_type_register_static(AGS_TYPE_RECALL,
							  "AgsVolumeAudioSignal\0",
							  &ags_volume_audio_signal_info,
							  0);

    g_type_add_interface_static(ags_type_volume_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_volume_audio_signal,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return (ags_type_volume_audio_signal);
}

void
ags_volume_audio_signal_class_init(AgsVolumeAudioSignalClass *volume_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_volume_audio_signal_parent_class = g_type_class_peek_parent(volume_audio_signal);

  gobject = (GObjectClass *) volume_audio_signal;

  gobject->finalize = ags_volume_audio_signal_finalize;

  recall = (AgsRecallClass *) volume_audio_signal;

  recall->duplicate = ags_volume_audio_signal_duplicate;
}

void
ags_volume_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_volume_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_volume_audio_signal_connect;
  connectable->disconnect = ags_volume_audio_signal_disconnect;
}

void
ags_volume_audio_signal_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_volume_audio_signal_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_volume_audio_signal_run_connect;
  run_connectable->disconnect = ags_volume_audio_signal_run_disconnect;
}

void
ags_volume_audio_signal_init(AgsVolumeAudioSignal *volume_audio_signal)
{
  volume_audio_signal->audio_signal = NULL;
}

void
ags_volume_audio_signal_connect(AgsConnectable *connectable)
{
  AgsVolumeAudioSignal *volume_audio_signal;

  ags_volume_audio_signal_parent_connectable_interface->connect(connectable);

  volume_audio_signal = AGS_VOLUME_AUDIO_SIGNAL(connectable);

  g_signal_connect((GObject *) volume_audio_signal, "run_inter\0",
		   G_CALLBACK(ags_volume_audio_signal_run_inter), NULL);
}

void
ags_volume_audio_signal_disconnect(AgsConnectable *connectable)
{
  ags_volume_audio_signal_parent_connectable_interface->disconnect(connectable);
}

void
ags_volume_audio_signal_run_connect(AgsRunConnectable *run_connectable)
{
  ags_volume_audio_signal_parent_run_connectable_interface->connect(run_connectable);
}

void
ags_volume_audio_signal_run_disconnect(AgsRunConnectable *run_connectable)
{
  ags_volume_audio_signal_parent_run_connectable_interface->disconnect(run_connectable);
}

void
ags_volume_audio_signal_finalize(GObject *gobject)
{
  AgsVolumeAudioSignal *volume_audio_signal;
}

void
ags_volume_audio_signal_run_inter(AgsRecall *recall, guint audio_channel, gpointer data)
{
  AgsVolumeAudioSignal *volume_audio_signal;

  volume_audio_signal = AGS_VOLUME_AUDIO_SIGNAL(recall);

  if(volume_audio_signal->audio_signal->stream_current != NULL){
    AgsDevout *devout;
    short *buffer;
    guint i;

    devout = AGS_DEVOUT(volume_audio_signal->audio_signal->devout);
    buffer = (short *) volume_audio_signal->audio_signal->stream_current->data;

    for(i = 0; i < devout->buffer_size; i++){
      buffer[i] = (short) ((0xffff) & (int)((gdouble)volume_audio_signal->volume[0] * (gdouble)buffer[i]));
    }
  }else{
    ags_recall_done(recall);
  }
}

AgsRecall*
ags_volume_audio_signal_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsVolumeAudioSignal *volume_audio_signal, *copy;

  volume_audio_signal = (AgsVolumeAudioSignal *) recall;
  copy = (AgsVolumeAudioSignal *) AGS_RECALL_CLASS(ags_volume_audio_signal_parent_class)->duplicate(recall, recall_id);

  copy->audio_signal = volume_audio_signal->audio_signal;

  return((AgsRecall *) copy);
}

AgsVolumeAudioSignal*
ags_volume_audio_signal_new(AgsAudioSignal *audio_signal, gdouble *volume)
{
  AgsVolumeAudioSignal *volume_audio_signal;

  volume_audio_signal = (AgsVolumeAudioSignal *) g_object_new(AGS_TYPE_VOLUME_AUDIO_SIGNAL, NULL);

  volume_audio_signal->audio_signal = audio_signal;

  volume_audio_signal->volume = volume;

  return(volume_audio_signal);
}
