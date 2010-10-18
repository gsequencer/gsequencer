#include "ags_audio_file.h"

#include "../../object/ags_connectable.h"
#include "../../object/ags_playable.h"

#include "../ags_audio_signal.h"

#include "ags_sndfile.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sndfile.h>
#include <string.h>

void ags_audio_file_class_init(AgsAudioFileClass *audio_file);
void ags_audio_file_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_file_init(AgsAudioFile *audio_file);
void ags_audio_file_connect(AgsConnectable *connectable);
void ags_audio_file_disconnect(AgsConnectable *connectable);
void ags_audio_file_finalize(GObject *object);

enum{
  READ_BUFFER,
  LAST_SIGNAL,
};

static gpointer ags_audio_file_parent_class = NULL;
static AgsConnectableInterface *ags_audio_file_parent_connectable_interface;
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

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_file_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_file = g_type_register_static(G_TYPE_OBJECT,
						 "AgsAudioFile\0",
						 &ags_audio_file_info,
						 0);

    g_type_add_interface_static(ags_type_audio_file,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
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
}

void
ags_audio_file_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_audio_file_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_audio_file_connect;
  connectable->disconnect = ags_audio_file_disconnect;
}

void
ags_audio_file_init(AgsAudioFile *audio_file)
{
  audio_file->devout = NULL;

  audio_file->name = NULL;
  audio_file->frames = 0;
  audio_file->channels = 0;

  audio_file->start_channel = 0;
  audio_file->audio_channels = 0;

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
ags_audio_file_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_audio_file_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_audio_file_open(AgsAudioFile *audio_file)
{
  fprintf(stdout, "ags_audio_file_open: %s\n\0", audio_file->name);

  if(g_file_test(audio_file->name, G_FILE_TEST_EXISTS)){
    if(g_str_has_suffix(audio_file->name, ".wav\0") ||
       g_str_has_suffix(audio_file->name, ".ogg\0") ||
       g_str_has_suffix(audio_file->name, ".flac\0")){
      fprintf(stdout, "ags_audio_file_open: using libsndfile\n\0");
      audio_file->file = (GObject *) ags_sndfile_new();
      if(ags_playable_open(AGS_PLAYABLE(audio_file->file),
			   audio_file->name)){
	ags_playable_info(AGS_PLAYABLE(audio_file->file),
			&(audio_file->channels), &(audio_file->frames));
	return(TRUE);
      }else{
	return(FALSE);
      }
    }else{
      fprintf(stdout, "ags_audio_file_open: unknown file type\n\0");
      return(FALSE);
    }
  }
}

void
ags_audio_file_read_audio_signal(AgsAudioFile *audio_file)
{
  AgsAudioSignal *audio_signal;
  GList *stream, *list;
  short *buffer;
  guint length;
  guint i, j, k, i_stop, j_stop;

  length = (guint) ceil((double)(audio_file->frames) / (double)(audio_file->devout->buffer_size));

  fprintf(stdout, "ags_audio_file_read:\n  audio_file->frames = %u\n  audio_file->devout->buffer_size = %u\n  length = %u\n\0", audio_file->frames, audio_file->devout->buffer_size, length);

  if(audio_file->channels > 0){
    audio_file->audio_signal =
      list = g_list_alloc();
    i = audio_file->start_channel;
    i_stop = audio_file->start_channel + audio_file->audio_channels;

    goto ags_audio_file_read_audio_signal0;
  }else
    i_stop = 0;


  for(; i < i_stop; i++){
    list->next = g_list_alloc();
    list->next->prev = list;
    list = list->next;

  ags_audio_file_read_audio_signal0:
    audio_signal = ags_audio_signal_new(NULL, NULL);
    audio_signal->devout = (GObject *) audio_file->devout;
    ags_audio_signal_connect(audio_signal);

    list->data = (gpointer) audio_signal;
    audio_signal->devout = (GObject *) audio_file->devout;
  }

  //  if((AGS_AUDIO_FILE_ALL_CHANNELS & (audio_file->flags)) != 0){
  //    fprintf(stdout, "  audio_file->all_channels == TRUE\n\0");

  list = audio_file->audio_signal;
  j_stop = (guint) floor((double)(audio_file->frames) / (double)(audio_file->devout->buffer_size));

  for(i = 0; list != NULL; i++){
    audio_signal = AGS_AUDIO_SIGNAL(list->data);
    ags_audio_signal_stream_resize(audio_signal, length);
    buffer = ags_playable_read(AGS_PLAYABLE(audio_file->file), i);

    stream = audio_signal->stream_beginning;
    
    for(j = 0; j < j_stop; j++){
      for(k = 0; k < audio_file->devout->buffer_size; k++)
	((short *) stream->data)[k] = buffer[j * audio_file->devout->buffer_size + k];
      
      stream = stream->next;
    }
    
    for(k = 0; k < (audio_file->frames % audio_file->devout->buffer_size); k++)
      ((short *) stream->data)[k] = buffer[j * audio_file->devout->buffer_size + k];
    
    free(buffer);
    list = list->next;
  }

  //}
  /*
  else{
    audio_signal = AGS_AUDIO_SIGNAL(audio_file->audio_signal->data);
    ags_audio_signal_stream_resize(audio_signal, length);
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
  */
}

void
ags_audio_file_close(AgsAudioFile *audio_file)
{
  ags_playable_close(AGS_PLAYABLE(audio_file->file));
}

AgsAudioFile*
ags_audio_file_new(gchar *name,
		   AgsDevout *devout,
		   guint start_channel, guint audio_channels)
{
  AgsAudioFile *audio_file;

  audio_file = (AgsAudioFile *) g_object_new(AGS_TYPE_AUDIO_FILE, NULL);

  audio_file->name = name;
  audio_file->devout = devout;
  audio_file->start_channel = start_channel;
  audio_file->audio_channels = audio_channels;

  return(audio_file);
}
