#include <ags/audio/recall/ags_play_audio_signal.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_id.h>

#include <stdlib.h>
#include <pthread.h>

void ags_play_audio_signal_class_init(AgsPlayAudioSignalClass *play_audio_signal);
void ags_play_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_audio_signal_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_play_audio_signal_init(AgsPlayAudioSignal *play_audio_signal);
void ags_play_audio_signal_connect(AgsConnectable *connectable);
void ags_play_audio_signal_disconnect(AgsConnectable *connectable);
void ags_play_audio_signal_run_connect(AgsRunConnectable *run_connectable);
void ags_play_audio_signal_run_disconnect(AgsRunConnectable *run_connectable);
void ags_play_audio_signal_finalize(GObject *gobject);

void ags_play_audio_signal_run_inter(AgsRecall *recall, guint audio_channel, gpointer data);
void ags_play_audio_signal_cancel(AgsRecall *recall, guint audio_channel);
AgsRecall* ags_play_audio_signal_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

static gpointer ags_play_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_play_audio_signal_parent_connectable_interface;
static AgsRunConnectableInterface *ags_play_audio_signal_parent_run_connectable_interface;

GType
ags_play_audio_signal_get_type()
{
  static GType ags_type_play_audio_signal = 0;

  if(!ags_type_play_audio_signal){
    static const GTypeInfo ags_play_audio_signal_info = {
      sizeof (AgsPlayAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_audio_signal_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_audio_signal = g_type_register_static(AGS_TYPE_RECALL,
							"AgsPlayAudioSignal\0",
							&ags_play_audio_signal_info,
							0);

    g_type_add_interface_static(ags_type_play_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_audio_signal,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return(ags_type_play_audio_signal);
}

void
ags_play_audio_signal_class_init(AgsPlayAudioSignalClass *play_audio_signal)
{
  GObjectClass *gobject;

  ags_play_audio_signal_parent_class = g_type_class_peek_parent(play_audio_signal);

  gobject = (GObjectClass *) play_audio_signal;
  gobject->finalize = ags_play_audio_signal_finalize;
}

void
ags_play_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_play_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_play_audio_signal_connect;
  connectable->disconnect = ags_play_audio_signal_disconnect;
}

void
ags_play_audio_signal_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_play_audio_signal_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_play_audio_signal_run_connect;
  run_connectable->disconnect = ags_play_audio_signal_run_disconnect;
}

void
ags_play_audio_signal_init(AgsPlayAudioSignal *play_audio_signal)
{
  play_audio_signal->source = NULL;
  play_audio_signal->audio_channel = 0;

  play_audio_signal->devout = NULL;
  play_audio_signal->attack = NULL;
}

void
ags_play_audio_signal_connect(AgsConnectable *connectable)
{
  AgsPlayAudioSignal *play_audio_signal;
  
  ags_play_audio_signal_parent_connectable_interface->connect(connectable);

  /* AgsPlayAudioSignal */
  play_audio_signal = AGS_PLAY_AUDIO_SIGNAL(connectable);

  g_signal_connect((GObject *) play_audio_signal, "run_inter\0",
		   G_CALLBACK(ags_play_audio_signal_run_inter), NULL);
}

void
ags_play_audio_signal_disconnect(AgsConnectable *connectable)
{
  ags_play_audio_signal_parent_connectable_interface->disconnect(connectable);
}

void
ags_play_audio_signal_run_connect(AgsRunConnectable *run_connectable)
{
  ags_play_audio_signal_parent_run_connectable_interface->connect(run_connectable);
}

void
ags_play_audio_signal_run_disconnect(AgsRunConnectable *run_connectable)
{
  ags_play_audio_signal_parent_run_connectable_interface->disconnect(run_connectable);
}

void
ags_play_audio_signal_finalize(GObject *gobject)
{
  AgsPlayAudioSignal *play_audio_signal;

  G_OBJECT_CLASS(ags_play_audio_signal_parent_class)->finalize(gobject);

  play_audio_signal = AGS_PLAY_AUDIO_SIGNAL(gobject);

  g_object_unref(play_audio_signal->source);
}

void
ags_play_audio_signal_run_inter(AgsRecall *recall, guint source_audio_channel, gpointer data)
{
  AgsPlayAudioSignal *play_audio_signal;
  AgsDevout *devout;
  AgsRecycling *recycling;
  AgsAudioSignal *source;
  GList *stream;
  short *buffer0, *buffer1;
  AgsAttack *attack;
  guint audio_channel;

  play_audio_signal = AGS_PLAY_AUDIO_SIGNAL(recall);

  devout = play_audio_signal->devout;
  source = AGS_AUDIO_SIGNAL(play_audio_signal->source);
  stream = source->stream_current;

  if(stream == NULL){
    //    ags_recall_done(recall, recall_id);
    return;
  }

  if((AGS_DEVOUT_BUFFER0 & devout->flags) != 0){
    buffer0 = devout->buffer[1];
    buffer1 = devout->buffer[2];
  }else if((AGS_DEVOUT_BUFFER1 & devout->flags) != 0){
    buffer0 = devout->buffer[2];
    buffer1 = devout->buffer[3];
  }else if((AGS_DEVOUT_BUFFER2 & devout->flags) != 0){
    buffer0 = devout->buffer[3];
    buffer1 = devout->buffer[0];
  }else if((AGS_DEVOUT_BUFFER3 & devout->flags) != 0){
    buffer0 = devout->buffer[0];
    buffer1 = devout->buffer[1];
  }

  attack = play_audio_signal->attack;
  audio_channel = play_audio_signal->audio_channel;

  ags_audio_signal_copy_buffer_to_buffer(&(buffer0[attack->first_start * devout->pcm_channels + audio_channel]), devout->pcm_channels,
					 (short *) stream->data, 1,
					 attack->first_length);

  if(attack->first_start != 0){
    ags_audio_signal_copy_buffer_to_buffer(&(buffer1[audio_channel]), devout->pcm_channels,
					   &(((short *) stream->data)[attack->first_length]), 1,
					   attack->first_start);
  }
  /*
  audio_signal->stream_current = audio_signal->stream_current->next;

  if(audio_signal->stream_current == NULL)
    ags_recall_done(recall, recall_id);
  */
}

AgsRecall*
ags_play_audio_signal_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsPlayAudioSignal *play_audio_signal, *copy;

  play_audio_signal = (AgsPlayAudioSignal *) recall;
  copy = (AgsPlayAudioSignal *) AGS_RECALL_CLASS(ags_play_audio_signal_parent_class)->duplicate(recall, recall_id);

  copy->source = play_audio_signal->source;
  copy->audio_channel = play_audio_signal->audio_channel;
  copy->devout = play_audio_signal->devout;

  return((AgsRecall *) copy);
}

AgsPlayAudioSignal*
ags_play_audio_signal_new(AgsAudioSignal *source, guint audio_channel,
			  AgsDevout *devout)
{
  AgsPlayAudioSignal *play_audio_signal;

  play_audio_signal = (AgsPlayAudioSignal *) g_object_new(AGS_TYPE_PLAY_AUDIO_SIGNAL,
							  NULL);

  play_audio_signal->devout = devout;

  if(devout != NULL)
    play_audio_signal->attack = ags_attack_get_from_devout((GObject *) devout);
  
  play_audio_signal->source = source;
  play_audio_signal->audio_channel = audio_channel;

  return(play_audio_signal);
}
