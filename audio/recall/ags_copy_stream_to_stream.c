#include "ags_copy_stream_to_stream.h"

GType ags_copy_stream_to_stream_get_type();
void ags_copy_stream_to_stream_class_init(AgsCopyStreamToStreamClass *copy_stream_to_stream);
void ags_copy_stream_to_stream_init(AgsCopyStreamToStream *copy_stream_to_stream);

GType
ags_copy_stream_to_stream_get_type()
{
  static GType ags_type_copy_stream_to_stream = 0;

  if(!ags_type_copy_stream_to_stream){
    static const GTypeInfo ags_copy_stream_to_stream_info = {
      sizeof (AgsCopyStreamToStreamClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_stream_to_stream_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyStreamToStream),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_stream_to_stream_init,
    };
    ags_type_copy_stream_to_stream = g_type_register_static(AGS_TYPE_RECALL, "AgsCopyStreamToStream\0", &ags_copy_stream_to_stream_info, 0);
  }
  return (ags_type_copy_stream_to_stream);
}

void
ags_copy_stream_to_stream_class_init(AgsCopyStreamToStreamClass *copy_stream_to_stream)
{
  //  AGS_RECALL_GET_CLASS(copy_stream_to_stream)->run = ags_copy_stream_to_stream;
}

void
ags_copy_stream_to_stream_init(AgsCopyStreamToStream *copy_stream_to_stream)
{
  copy_stream_to_stream->destination = NULL;
  copy_stream_to_stream->source = NULL;

  copy_stream_to_stream->stream0 = NULL;
  copy_stream_to_stream->stream1 = NULL;
}

void
ags_copy_stream_to_stream(AgsRecall *recall)
{
  AgsCopyStreamToStream *copy_stream_to_stream;
  AgsAudioSignal *destination, *source;
  AgsDevout *devout;
  GList *list0, *list1;
  //  guint dattack[2], sattack[2];

  copy_stream_to_stream = (AgsCopyStreamToStream *) recall;

  destination = copy_stream_to_stream->destination;
  source = copy_stream_to_stream->source;

  list0 = (GList *) copy_stream_to_stream->stream0;
  list1 = (GList *) copy_stream_to_stream->stream1;

  if(list1 == NULL){
    AGS_RECALL_GET_CLASS(recall)->done(recall);
    return;
  }

  if(list0 == NULL)
    ags_audio_signal_add_stream(destination);

  //  AGS_RECALL_GET_CLASS(recall)->run_stream(recall);

  if(source->attack[4] == 0){
    devout = source->devout;

    if(source->attack[0] == 0){
      ags_audio_signal_copy_buffer_to_buffer((short *) list0->data, devout->buffer_size, (short *) list1->data, devout->buffer_size);

      if(list1->next == NULL){
	AGS_RECALL_GET_CLASS(recall)->done(recall);
	return;
      }

      if(list0->next == NULL)
	ags_audio_signal_add_stream(destination);

      list0 = list0->next;
      list1 = list1->next;
    }else{
      ags_audio_signal_copy_buffer_to_buffer((short *) list0->data, devout->buffer_size, &(((short *) list1->data)[source->attack[0]]), source->attack[1]);

      if(list1->next == NULL){
	AGS_RECALL_GET_CLASS(recall)->done(recall);
	return;
      }

      list1 = list1->next;

      ags_audio_signal_copy_buffer_to_buffer(&(((short *) list0->data)[source->attack[5]]), devout->buffer_size, &(((short *) list1->data)[source->attack[1]]), source->attack[0]);

      if(list0->next == NULL)
	ags_audio_signal_add_stream(destination);

      list0 = list0->next;
    }
  }else if(source->attack[0] == 0){
    devout = source->devout;

    ags_audio_signal_copy_buffer_to_buffer(&(((short *) list0->data)[source->attack[4]]), source->attack[5], (short *) list1->data, devout->buffer_size);

    if(list0->next == NULL)
      ags_audio_signal_add_stream(destination);

    list0 = list0->next;

    ags_audio_signal_copy_buffer_to_buffer(&(((short *) list0->data)[source->attack[5]]), source->attack[4], &(((short *) list1->data)[source->attack[1]]), devout->buffer_size);

    if(list1->next == NULL){
      AGS_RECALL_GET_CLASS(recall)->done(recall);
      return;
    }

    list1 = list1->next;
  }else
    if((AGS_AUDIO_SIGNAL_STOPS_FIRST & source->flags) != 0){
      ags_audio_signal_copy_buffer_to_buffer(&(((short *) list0->data)[source->attack[4]]), source->attack[5], &(((short *) list1->data)[source->attack[0]]), source->attack[1]);

      if(list1->next == NULL){
	AGS_RECALL_GET_CLASS(recall)->done(recall);
	return;
      }

      list1 = list1->next;

      ags_audio_signal_copy_buffer_to_buffer(&(((short *) list0->data)[source->attack[6]]), source->attack[7], (short *) list1->data, source->attack[0]);

      if(list0->next == NULL)
	ags_audio_signal_add_stream(destination);

      list0 = list0->next;

      ags_audio_signal_copy_buffer_to_buffer((short *) list0->data, source->attack[6], &(((short *) list1->data)[source->attack[2]]), source->attack[3]);
    }else{
      ags_audio_signal_copy_buffer_to_buffer(&(((short *) list0->data)[source->attack[4]]), source->attack[5], &(((short *) list1->data)[source->attack[0]]), source->attack[1]);

      if(list0->next == NULL)
	ags_audio_signal_add_stream(destination);

      list0 = list0->next;

      ags_audio_signal_copy_buffer_to_buffer((short *) list0->data, source->attack[4], &(((short *) list1->data)[source->attack[2]]), source->attack[3]);

      if(list1->next == NULL){
	AGS_RECALL_GET_CLASS(recall)->done(recall);
	return;
      }

      list1 = list1->next;

      ags_audio_signal_copy_buffer_to_buffer(&(((short *) list0->data)[source->attack[6]]), source->attack[7], (short *) list1->data, source->attack[0]);
    }

  copy_stream_to_stream->stream0 = list0;
  copy_stream_to_stream->stream1 = list1;
}

AgsCopyStreamToStream*
ags_copy_stream_to_stream_new()
{
  AgsCopyStreamToStream *copy_stream_to_stream;

  copy_stream_to_stream = (AgsCopyStreamToStream *) g_object_new(AGS_TYPE_COPY_STREAM_TO_STREAM, NULL);

  return(copy_stream_to_stream);
}
