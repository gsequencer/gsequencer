#include "ags_audio_file.h"

#include "ags_audio_file_xml.h"
#include "ags_audio_file_raw.h"
#include "ags_audio_file_wav.h"
#include "ags_audio_file_ogg.h"
#include "ags_audio_file_mp3.h"

#include "../ags_audio_signal.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sndfile.h>
//#include <ogg/ogg.h>
#include <string.h>

GType ags_audio_file_get_type();
void ags_audio_file_class_init(AgsAudioFileClass *audio_file);
void ags_audio_file_init(AgsAudioFile *audio_file);
void ags_audio_file_finalize(GObject *object);

void ags_audio_file_close(AgsAudioFile *audio_file);

void ags_audio_file_read(AgsAudioFile *audio_file);
void ags_audio_file_read_xml(AgsAudioFile *audio_file);
void ags_audio_file_read_wav(AgsAudioFile *audio_file);
void ags_audio_file_read_raw(AgsAudioFile *audio_file);
void ags_audio_file_read_ogg(AgsAudioFile *audio_file);
void ags_audio_file_read_mp3(AgsAudioFile *audio_file);

enum{
  READ_BUFFER,
  LAST_SIGNAL,
};

static gpointer ags_audio_file_parent_class = NULL;
static guint signals[LAST_SIGNAL];

GType
ags_audio_file_get_type()
{
  static GType ags_type_audio_file = 0;

  if(!ags_type_audio_file){
    static const GTypeInfo ags_audio_file_info = {
      sizeof (AgsAudioFileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_file_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_file_init,
    };
    ags_type_audio_file = g_type_register_static(G_TYPE_OBJECT, "AgsAudioFile\0", &ags_audio_file_info, 0);
  }
  return (ags_type_audio_file);
}

void
ags_audio_file_class_init(AgsAudioFileClass *audio_file)
{
  GObjectClass *gobject;

  ags_audio_file_parent_class = g_type_class_peek_parent(audio_file);

  gobject = (GObjectClass *) audio_file;
  gobject->finalize = ags_audio_file_finalize;

  audio_file->read_buffer = ags_audio_file_read_wav;

  signals[READ_BUFFER] =
    g_signal_new("read_buffer\0",
		 G_TYPE_FROM_CLASS (audio_file),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsAudioFileClass, read_buffer),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_audio_file_init(AgsAudioFile *audio_file)
{
  audio_file->flags = 0;

  audio_file->devout = NULL;

  audio_file->name = NULL;
  audio_file->frames = 0;
  audio_file->channels = 0;

  audio_file->buffer_channels = 0;
  audio_file->channel = 0;
  audio_file->buffer = NULL;

  audio_file->audio_signal = NULL;

  audio_file->file = NULL;
}

void
ags_audio_file_finalize(GObject *gobject)
{
  AgsAudioFile *audio_file;
  GList *list, *list_next;

  audio_file = AGS_AUDIO_FILE(gobject);

  /* AgsAudioSignal */
  list = audio_file->audio_signal;

  while(list != NULL){
    list_next = list->next;

    g_object_unref(G_OBJECT(list->data));
    g_list_free1(list);
    
    list = list_next;
  }

  /* file */
  g_object_unref(audio_file->file);

  G_OBJECT_CLASS(ags_audio_file_parent_class)->finalize(gobject);
}

void
ags_audio_file_connect(AgsAudioFile *audio_file)
{
}

void
ags_audio_file_open(AgsAudioFile *audio_file)
{
  fprintf(stdout, "ags_audio_file_open: %s\n\0", audio_file->name);

  if(g_file_test(audio_file->name, G_FILE_TEST_EXISTS)){
    AgsAudioFileClass *class;

    class = AGS_AUDIO_FILE_GET_CLASS(audio_file);

    if(g_str_has_suffix(audio_file->name, ".xml\0")){
      audio_file->file = (GObject *) ags_audio_file_xml_new(audio_file);

      class->read_buffer = ags_audio_file_read_xml;
    }else if(g_str_has_suffix(audio_file->name, ".raw\0")){
      audio_file->file = (GObject *) ags_audio_file_raw_new(audio_file);

      class->read_buffer = ags_audio_file_read_raw;
    }else if(g_str_has_suffix(audio_file->name, ".wav\0")){
      fprintf(stdout, "ags_audio_file_open: found .wav file\n\0");

      audio_file->file = (GObject *) ags_audio_file_wav_new(audio_file);

      class->read_buffer = ags_audio_file_read_wav;
    }else if(g_str_has_suffix(audio_file->name, ".ogg\0")){
      audio_file->file = (GObject *) ags_audio_file_ogg_new(audio_file);

      class->read_buffer = ags_audio_file_read_ogg;
    }else if(g_str_has_suffix(audio_file->name, ".mp3\0")){
      audio_file->file = (GObject *) ags_audio_file_mp3_new(audio_file);

      class->read_buffer = ags_audio_file_read_mp3;
    }else
      fprintf(stdout, "  unknown file type\n\0");
  }
}

void
ags_audio_file_set_devout(AgsAudioFile *audio_file, AgsDevout *devout)
{
  GList *list;
  guint i;

  audio_file->devout = devout;

  list = audio_file->audio_signal;

  while(list != NULL){
    AGS_AUDIO_SIGNAL(list->data)->devout = (GObject *) devout;

    list = list->next;
  }
}

void
ags_audio_file_read_audio_signal(AgsAudioFile *audio_file)
{
  AgsAudioSignal *audio_signal;
  GList *stream, *list;
  guint64 stop;
  guint i, j, k, i_stop;

  stop = (guint) ceil((double)(audio_file->frames) / (double)(audio_file->devout->buffer_size));

  fprintf(stdout, "ags_audio_file_read()\n  audio_file->frames = %u\n  audio_file->devout->buffer_size = %u\n  stop = %u\n\0", audio_file->frames, audio_file->devout->buffer_size, stop);

  if(audio_file->channels > 0){
    audio_file->audio_signal =
      list = g_list_alloc();
    i = 0;

    if((AGS_AUDIO_FILE_ALL_CHANNELS & (audio_file->flags)) != 0)
      i_stop = audio_file->channels;
    else
      i_stop = 1;

    goto ags_audio_file_read_audio_signal0;
  }else
    i_stop = 0;

  for(; i < i_stop; i++){
    list->next = g_list_alloc();
    list->next->prev = list;
    list = list->next;

  ags_audio_file_read_audio_signal0:
    audio_signal = ags_audio_signal_new(NULL, NULL);
    ags_audio_signal_connect(audio_signal);

    list->data = (gpointer) audio_signal;
    audio_signal->devout = (GObject *) audio_file->devout;
  }

  if((AGS_AUDIO_FILE_ALL_CHANNELS & (audio_file->flags)) != 0){
    fprintf(stdout, "  audio_file->all_channels == TRUE\n\0");

    list = audio_file->audio_signal;

    for(i = 0; list != NULL; i++){
      audio_signal = AGS_AUDIO_SIGNAL(list->data);
      ags_audio_signal_stream_resize(audio_signal, stop);

      stream = audio_signal->stream_beginning;

      for(j = 0; j < (guint) floor((double)(audio_file->frames) / (double)(audio_file->devout->buffer_size)); j++){
	for(k = 0; k < audio_file->devout->buffer_size; k++)
	  ((short *) stream->data)[k] = audio_file->buffer[i + j * audio_file->devout->buffer_size + k * audio_file->channels];

	stream = stream->next;
      }

      for(k = 0; k < (audio_file->frames % audio_file->devout->buffer_size); k++)
	((short *) stream->data)[k] = audio_file->buffer[i + j * audio_file->devout->buffer_size + k * audio_file->channels];

      list = list->next;
    }
  }else{
    audio_signal = AGS_AUDIO_SIGNAL(audio_file->audio_signal->data);
    ags_audio_signal_stream_resize(audio_signal, stop);
    stream = audio_signal->stream_beginning;

    for(i = 0; i < (guint) floor((double)(audio_file->frames) / (double)(audio_file->devout->buffer_size)); i++){
      for(j = 0; j < audio_file->devout->buffer_size; j++){
	((short *) stream->data)[j] = audio_file->buffer[audio_file->channel + i * audio_file->devout->buffer_size + j * audio_file->channels];
      }

      stream = stream->next;
    }

    for(j = 0; j < (audio_file->frames % audio_file->devout->buffer_size); j++){
      ((short *) stream->data)[j] = audio_file->buffer[audio_file->channel + i * audio_file->devout->buffer_size + j * audio_file->channels];
    }
  }
}

void
ags_audio_file_close(AgsAudioFile *audio_file)
{
  if(audio_file->file != NULL){
    if(AGS_IS_AUDIO_FILE_XML(audio_file->file))
      ags_audio_file_xml_destroy(audio_file->file);
    else if(AGS_IS_AUDIO_FILE_RAW(audio_file->file))
      ags_audio_file_raw_destroy(audio_file->file);
    else if(AGS_IS_AUDIO_FILE_WAV(audio_file->file))
      ags_audio_file_wav_destroy(audio_file->file);
    else if(AGS_IS_AUDIO_FILE_OGG(audio_file->file)){
      ags_audio_file_ogg_destroy(audio_file->file);
    }else if(AGS_IS_AUDIO_FILE_MP3(audio_file->file)){
      ags_audio_file_mp3_destroy(audio_file->file);
    }else{
      fprintf(stdout, "ags_audio_file_close unknown file type\n\0");
    }
  }
}

void
ags_audio_file_read_xml(AgsAudioFile *audio_file)
{
}

void
ags_audio_file_read_raw(AgsAudioFile *audio_file)
{
}

void
ags_audio_file_read_wav(AgsAudioFile *audio_file)
{
  AgsAudioFileWav *file_wav;

  if(audio_file->channels == 0)
    return;

  file_wav = (AgsAudioFileWav *) audio_file->file;

  audio_file->buffer = (short *) malloc(audio_file->frames * audio_file->channels * sizeof(short));
  fprintf(stdout, "ags_audio_file_read_wav:  malloc(%d * %d * %d)\n\0", audio_file->frames, audio_file->channels, sizeof(short));

  sf_read_short(file_wav->file, audio_file->buffer, file_wav->info.frames * audio_file->channels);
}

void
ags_audio_file_read_ogg(AgsAudioFile *audio_file)
{
}

void
ags_audio_file_read_mp3(AgsAudioFile *audio_file)
{
}

AgsAudioFile*
ags_audio_file_new()
{
  AgsAudioFile *audio_file;

  audio_file = (AgsAudioFile *) g_object_new(AGS_TYPE_AUDIO_FILE, NULL);

  return(audio_file);
}
