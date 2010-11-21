#include <ags/audio/recall/ags_stream_audio_signal.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/recall/ags_stream_channel.h>

void ags_stream_audio_signal_class_init(AgsStreamAudioSignalClass *stream_audio_signal);
void ags_stream_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_stream_audio_signal_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_stream_audio_signal_init(AgsStreamAudioSignal *stream_audio_signal);
void ags_stream_audio_signal_connect(AgsConnectable *connectable);
void ags_stream_audio_signal_disconnect(AgsConnectable *connectable);
void ags_stream_audio_signal_run_connect(AgsRunConnectable *run_connectable);
void ags_stream_audio_signal_run_disconnect(AgsRunConnectable *run_connectable);
void ags_stream_audio_signal_finalize(GObject *gobject);

void ags_stream_audio_signal_run_post(AgsRecall *recall, guint audio_channel, gpointer data);

AgsRecall* ags_stream_audio_signal_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

static gpointer ags_stream_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_stream_audio_signal_parent_connectable_interface;
static AgsRunConnectableInterface *ags_stream_audio_signal_parent_run_connectable_interface;

GType
ags_stream_audio_signal_get_type()
{
  static GType ags_type_stream_audio_signal = 0;

  if(!ags_type_stream_audio_signal){
    static const GTypeInfo ags_stream_audio_signal_info = {
      sizeof (AgsStreamAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_stream_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsStreamAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_stream_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_stream_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_stream_audio_signal_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_stream_audio_signal = g_type_register_static(AGS_TYPE_RECALL,
							  "AgsStreamAudioSignal\0",
							  &ags_stream_audio_signal_info,
							  0);

    g_type_add_interface_static(ags_type_stream_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_stream_audio_signal,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return (ags_type_stream_audio_signal);
}

void
ags_stream_audio_signal_class_init(AgsStreamAudioSignalClass *stream_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_stream_audio_signal_parent_class = g_type_class_peek_parent(stream_audio_signal);

  gobject = (GObjectClass *) stream_audio_signal;

  gobject->finalize = ags_stream_audio_signal_finalize;

  recall = (AgsRecallClass *) stream_audio_signal;

  recall->duplicate = ags_stream_audio_signal_duplicate;
}

void
ags_stream_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_stream_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_stream_audio_signal_connect;
  connectable->disconnect = ags_stream_audio_signal_disconnect;
}

void
ags_stream_audio_signal_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_stream_audio_signal_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_stream_audio_signal_run_connect;
  run_connectable->disconnect = ags_stream_audio_signal_run_disconnect;
}

void
ags_stream_audio_signal_init(AgsStreamAudioSignal *stream_audio_signal)
{
  stream_audio_signal->audio_signal = NULL;
}

void
ags_stream_audio_signal_connect(AgsConnectable *connectable)
{
  AgsStreamAudioSignal *stream_audio_signal;

  ags_stream_audio_signal_parent_connectable_interface->connect(connectable);

  stream_audio_signal = AGS_STREAM_AUDIO_SIGNAL(connectable);

  g_signal_connect((GObject *) stream_audio_signal, "run_post\0",
		   G_CALLBACK(ags_stream_audio_signal_run_post), NULL);
}

void
ags_stream_audio_signal_disconnect(AgsConnectable *connectable)
{
  ags_stream_audio_signal_parent_connectable_interface->disconnect(connectable);
}

void
ags_stream_audio_signal_run_connect(AgsRunConnectable *run_connectable)
{
  ags_stream_audio_signal_parent_run_connectable_interface->connect(run_connectable);
}

void
ags_stream_audio_signal_run_disconnect(AgsRunConnectable *run_connectable)
{
  ags_stream_audio_signal_parent_run_connectable_interface->disconnect(run_connectable);
}

void
ags_stream_audio_signal_finalize(GObject *gobject)
{
  AgsStreamAudioSignal *stream_audio_signal;
}

void
ags_stream_audio_signal_run_post(AgsRecall *recall, guint audio_channel, gpointer data)
{
  AgsStreamAudioSignal *stream_audio_signal;

  stream_audio_signal = AGS_STREAM_AUDIO_SIGNAL(recall);

  if(stream_audio_signal->audio_signal->stream_current != NULL){
    stream_audio_signal->audio_signal->stream_current = stream_audio_signal->audio_signal->stream_current->next;
  }else{
    ags_recall_done(recall);
  }
}

AgsRecall*
ags_stream_audio_signal_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsStreamAudioSignal *stream_audio_signal, *copy;

  stream_audio_signal = (AgsStreamAudioSignal *) recall;
  copy = (AgsStreamAudioSignal *) AGS_RECALL_CLASS(ags_stream_audio_signal_parent_class)->duplicate(recall, recall_id);

  copy->audio_signal = stream_audio_signal->audio_signal;

  return((AgsRecall *) copy);
}

AgsStreamAudioSignal*
ags_stream_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsStreamAudioSignal *stream_audio_signal;

  stream_audio_signal = (AgsStreamAudioSignal *) g_object_new(AGS_TYPE_STREAM_AUDIO_SIGNAL, NULL);

  stream_audio_signal->audio_signal = audio_signal;

  return(stream_audio_signal);
}
