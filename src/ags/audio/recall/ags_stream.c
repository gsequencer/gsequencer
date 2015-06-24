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

#include <ags/audio/recall/ags_stream.h>

#include <ags/audio/ags_devout.h>

void ags_stream_class_init(AgsStream *stream);
void ags_stream_init(AgsStream *stream);

GType
ags_stream_get_type()
{
  static GType ags_type_stream = 0;

  if(!ags_type_stream){
    static const GTypeInfo ags_stream_info = {
      sizeof (AgsStreamClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_stream_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsStream),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_stream_init,
    };
    ags_type_stream = g_type_register_static(G_TYPE_OBJECT, "AgsStream\0", &ags_stream_info, 0);
  }
  return (ags_type_stream);
}

void
ags_stream_class_init(AgsStream *stream)
{
}

void
ags_stream_init(AgsStream *stream)
{
  stream->audio_signal = NULL;
}

void
ags_stream(AgsRecall *recall, AgsRecallID *recall_id, gpointer data)
{
  /* -- deprecated --
  AgsStream *stream;
  AgsDevout *devout;
  AgsAudioSignal *audio_signal;
  GList *list;
  guint *buffer;
  guint i;

  stream = (AgsStream *) recall;
  audio_signal = (AgsAudioSignal *) stream->audio_signal;
  devout = (AgsDevout *) audio_signal->devout;

  if(audio_signal->stream_current == NULL)
    if(devout->offset < audio_signal->stream_end){
      //      AGS_RECALL_GET_CLASS(recall)->done(recall, recall_id);
      g_signal_emit_by_name((GObject *) recall, "done\0", recall_id);
    }else{
      ags_audio_signal_add_stream(audio_signal);
    }

  buffer = audio_signal->stream_current->data;

  for(i = 0; i < devout->buffer_size; i++){
    list = recall->recall;

    while(list != NULL){
      g_signal_emit_by_name((GObject *) recall, "run_stream\0", recall_id);

      list = list->next;
    }
  }
  */
}

AgsStream*
ags_stream_new()
{
  AgsStream *stream;

  stream = (AgsStream *) g_object_new(AGS_TYPE_STREAM, NULL);

  return(stream);
}
