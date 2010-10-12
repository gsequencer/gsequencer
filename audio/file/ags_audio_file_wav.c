#include "ags_audio_file_wav.h"

#include "../ags_audio_signal.h"

#include <stdlib.h>
#include <math.h>
#include <sndfile.h>

void ags_audio_file_wav_class_init(AgsAudioFileWavClass *file_wav);
void ags_audio_file_wav_init(AgsAudioFileWav *file_wav);
void ags_audio_file_wav_destroy(GObject *object);

GType
ags_audio_file_wav_get_type()
{
  static GType ags_type_audio_file_wav = 0;

  if(!ags_type_audio_file_wav){
    static const GTypeInfo ags_audio_file_wav_info = {
      sizeof (AgsAudioFileWavClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_file_wav_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_file_wav_init,
    };
    ags_type_audio_file_wav = g_type_register_static(G_TYPE_OBJECT, "AgsAudioFileWav\0", &ags_audio_file_wav_info, 0);
  }
  return (ags_type_audio_file_wav);
}

void
ags_audio_file_wav_class_init(AgsAudioFileWavClass *file_wav)
{
}

void
ags_audio_file_wav_init(AgsAudioFileWav *file_wav)
{
  file_wav->file = NULL;
  //  file_wav->info = NULL;
}

void
ags_audio_file_wav_destroy(GObject *object)
{
  AgsAudioFileWav *file_wav;

  file_wav = (AgsAudioFileWav *) object;

  sf_close(file_wav->file);
  //  free(file_wav->info);
}

AgsAudioFileWav*
ags_audio_file_wav_new(AgsAudioFile *audio_file)
{
  AgsAudioFileWav *file_wav;
  AgsAudioSignal *audio_signal;
  GList *list;
  guint i, stop;

  file_wav = (AgsAudioFileWav *) g_object_new(AGS_TYPE_AUDIO_FILE_WAV, NULL);

  //  file_wav->info = (SF_INFO *) malloc(sizeof(SF_INFO));
  file_wav->file = (SNDFILE *) sf_open(audio_file->name, SFM_READ, &(file_wav->info));
  fprintf(stdout, "ags_audio_file_wav_new: file opened\n\0");

  if(file_wav->file == NULL)
    fprintf(stdout, "  not able to open file\n\0");

  audio_file->frames = file_wav->info.frames;
  audio_file->channels = file_wav->info.channels;

  return(file_wav);
}
