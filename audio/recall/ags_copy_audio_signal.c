#include "ags_copy_audio_signal.h"

#include "../../object/ags_connectable.h"
#include "../../object/ags_run_connectable.h"

#include "../ags_devout.h"
#include "../ags_audio_signal.h"
#include "../ags_recycling.h"
#include "../ags_channel.h"
#include "../ags_recall_id.h"

#include "ags_copy_channel.h"

#include <stdlib.h>

void ags_copy_audio_signal_class_init(AgsCopyAudioSignalClass *copy_audio_signal);
void ags_copy_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_copy_audio_signal_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_copy_audio_signal_init(AgsCopyAudioSignal *copy_audio_signal);
void ags_copy_audio_signal_connect(AgsConnectable *connectable);
void ags_copy_audio_signal_disconnect(AgsConnectable *connectable);
void ags_copy_audio_signal_run_connect(AgsRunConnectable *run_connectable);
void ags_copy_audio_signal_run_disconnect(AgsRunConnectable *run_connectable);
void ags_copy_audio_signal_finalize(GObject *gobject);

void ags_copy_audio_signal_run_inter(AgsRecall *recall, guint audio_channel, gpointer data);

AgsRecall* ags_copy_audio_signal_duplicate(AgsRecall *recall, AgsRecallID *recall_id);

static gpointer ags_copy_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_copy_audio_signal_parent_connectable_interface;
static AgsRunConnectableInterface *ags_copy_audio_signal_parent_run_connectable_interface;

GType
ags_copy_audio_signal_get_type()
{
  static GType ags_type_copy_audio_signal = 0;

  if(!ags_type_copy_audio_signal){
    static const GTypeInfo ags_copy_audio_signal_info = {
      sizeof (AgsCopyAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_audio_signal_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_copy_audio_signal = g_type_register_static(AGS_TYPE_RECALL,
							"AgsCopyAudioSignal\0",
							&ags_copy_audio_signal_info,
							0);

    g_type_add_interface_static(ags_type_copy_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_copy_audio_signal,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return(ags_type_copy_audio_signal);
}

void
ags_copy_audio_signal_class_init(AgsCopyAudioSignalClass *copy_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_copy_audio_signal_parent_class = g_type_class_peek_parent(copy_audio_signal);

  gobject = (GObjectClass *) copy_audio_signal;

  gobject->finalize = ags_copy_audio_signal_finalize;

  recall = (AgsRecallClass *) copy_audio_signal;
  
  recall->duplicate = ags_copy_audio_signal_duplicate;
}

void
ags_copy_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_copy_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_copy_audio_signal_connect;
  connectable->disconnect = ags_copy_audio_signal_disconnect;
}

void
ags_copy_audio_signal_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_copy_audio_signal_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_copy_audio_signal_run_connect;
  run_connectable->disconnect = ags_copy_audio_signal_run_disconnect;
}

void
ags_copy_audio_signal_init(AgsCopyAudioSignal *copy_audio_signal)
{
  copy_audio_signal->devout = NULL;

  copy_audio_signal->source = NULL;
  copy_audio_signal->destination = NULL;
  copy_audio_signal->attack = NULL;
}

void
ags_copy_audio_signal_connect(AgsConnectable *connectable)
{
  AgsCopyAudioSignal *copy_audio_signal;

  ags_copy_audio_signal_parent_connectable_interface->connect(connectable);

  /* AgsCopyAudioSignal */
  copy_audio_signal = AGS_COPY_AUDIO_SIGNAL(connectable);

  g_signal_connect((GObject *) copy_audio_signal, "run_inter\0",
		   G_CALLBACK(ags_copy_audio_signal_run_inter), NULL);
}

void
ags_copy_audio_signal_disconnect(AgsConnectable *connectable)
{
  ags_copy_audio_signal_parent_connectable_interface->disconnect(connectable);
}

void
ags_copy_audio_signal_run_connect(AgsRunConnectable *run_connectable)
{
  ags_copy_audio_signal_parent_run_connectable_interface->connect(run_connectable);
}

void
ags_copy_audio_signal_run_disconnect(AgsRunConnectable *run_connectable)
{
  ags_copy_audio_signal_parent_run_connectable_interface->disconnect(run_connectable);
}

void
ags_copy_audio_signal_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_copy_audio_signal_parent_class)->finalize(gobject);
}

void
ags_copy_audio_signal_run_inter(AgsRecall *recall, guint audio_channel, gpointer data)
{
  AgsCopyAudioSignal *copy_audio_signal;
  AgsAudioSignal *source, *destination;
  AgsAttack *attack;
  GList *stream_source, *stream_destination;

  copy_audio_signal = AGS_COPY_AUDIO_SIGNAL(recall);

  source = copy_audio_signal->source;
  stream_source = source->stream_current;

  if(stream_source == NULL){
    ags_recall_done(recall);
    return;
  }

  destination = copy_audio_signal->destination;
  stream_destination = destination->stream_current;
  attack = copy_audio_signal->attack;

  if(stream_destination->next == NULL)
    ags_audio_signal_add_stream(destination);

  ags_audio_signal_copy_buffer_to_buffer(&(((short *) stream_destination->data)[attack->first_start]), 1,
					 (short *) stream_source->data, 1,
					 attack->first_length);

  if(attack->first_start != 0){
    ags_audio_signal_copy_buffer_to_buffer((short *) stream_destination->data, 1,
					   &(((short *) stream_source->data)[attack->first_length]), 1,
					   attack->first_start);
  }
  /*  
  source->stream_current = source->stream_current->next;

  if(source->stream_current == NULL)
    ags_recall_done(recall);
  */
}

AgsRecall*
ags_copy_audio_signal_duplicate(AgsRecall *recall, AgsRecallID *recall_id)
{
  AgsCopyAudioSignal *copy_audio_signal, *copy;

  copy_audio_signal = (AgsCopyAudioSignal *) recall;
  copy = (AgsCopyAudioSignal *) AGS_RECALL_CLASS(ags_copy_audio_signal_parent_class)->duplicate(recall, recall_id);

  copy->destination = copy_audio_signal->destination;
  copy->source = copy_audio_signal->source;
  copy->devout = copy_audio_signal->devout;
  copy->attack = copy_audio_signal->attack;

  return((AgsRecall *) copy);
}

AgsCopyAudioSignal*
ags_copy_audio_signal_new(AgsAudioSignal *destination,
			  AgsAudioSignal *source,
			  AgsDevout *devout)
{
  AgsCopyAudioSignal *copy_audio_signal;

  copy_audio_signal = (AgsCopyAudioSignal *) g_object_new(AGS_TYPE_COPY_AUDIO_SIGNAL, NULL);

  copy_audio_signal->destination = destination;

  copy_audio_signal->source = source;

  copy_audio_signal->devout = devout;

  if(devout != NULL)
    copy_audio_signal->attack = ags_attack_get_from_devout((GObject *) devout);

  return(copy_audio_signal);
}
