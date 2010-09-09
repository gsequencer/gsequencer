#include "ags_output.h"

#include "ags_devout.h"
#include "ags_audio.h"

GType ags_output_get_type();
void ags_output_class_init(AgsOutputClass *output_class);
void ags_output_init(AgsOutput *output);
void ags_output_finalize(GObject *gobject);

extern void ags_file_write_output(AgsFile *file, AgsChannel *channel);

static gpointer ags_output_parent_class = NULL;

GType
ags_output_get_type (void)
{
  static GType ags_type_output = 0;

  if(!ags_type_output){
    static const GTypeInfo ags_output_info = {
      sizeof (AgsOutputClass),
      (GBaseInitFunc) NULL, /* base_init */
      (GBaseFinalizeFunc) NULL, /* base_finalize */
      (GClassInitFunc) ags_output_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOutput),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_output_init,
    };
    ags_type_output = g_type_register_static(AGS_TYPE_CHANNEL, "AgsOutput\0", &ags_output_info, 0);
  }
  return (ags_type_output);
}

void
ags_output_class_init(AgsOutputClass *output)
{
  GObjectClass *gobject;
  AgsChannelClass *channel;

  ags_output_parent_class = g_type_class_peek_parent(output);

  gobject = (GObjectClass *) output;
  gobject->finalize = ags_output_finalize;

  channel = (AgsChannelClass *) output;
  channel->write_file = ags_file_write_output;
}

void
ags_output_init(AgsOutput *output)
{
}

void
ags_output_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_output_parent_class)->finalize(gobject);
}

void
ags_output_connect(AgsOutput *output)
{
}

GList*
ags_output_map_audio_signal(AgsOutput *output, AgsRecallID *recall_id)
{
  AgsDevout *devout;
  AgsAudioSignal *audio_signal;
  GList *list_destination;

  if(output != NULL){
    list_destination = g_list_alloc();
    goto ags_copy_pattern_map_destination0;
  }else
    return(NULL);

  devout = AGS_DEVOUT(AGS_AUDIO(AGS_CHANNEL(output)->audio)->devout);
  
  while(output != NULL){
    list_destination->next = g_list_alloc();
    list_destination->next->prev = list_destination;
    list_destination = list_destination->next;
  ags_copy_pattern_map_destination0:
    fprintf(stdout, "ags_output_map_audio_signal\n\0");

    audio_signal = ags_audio_signal_new((GObject *) output->channel.first_recycling,
					(GObject *) recall_id);
    audio_signal->devout = (GObject *) devout;
    ags_audio_signal_connect(audio_signal);

    ags_recycling_add_audio_signal(output->channel.first_recycling,
				   audio_signal);
    audio_signal->stream_current = audio_signal->stream_beginning;
    list_destination->data = (gpointer) audio_signal;

    output = (AgsOutput *) output->channel.next_pad;
  }
  
  return(list_destination);
}

AgsOutput*
ags_output_new()
{
  AgsOutput *output;

  output = (AgsOutput *) g_object_new(AGS_TYPE_OUTPUT, NULL);

  return(output);
}
