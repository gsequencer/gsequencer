#ifndef __AGS_AUDIO_FILE_WAV_H__
#define __AGS_AUDIO_FILE_WAV_H__

#include <glib.h>
#include <glib-object.h>
#include <sndfile.h>

#include "ags_audio_file.h"

#define AGS_TYPE_AUDIO_FILE_WAV              (ags_audio_file_wav_get_type())
#define AGS_AUDIO_FILE_WAV(obj)              (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_AUDIO_FILE_WAV, AgsAudioFileWav))
#define AGS_AUDIO_FILE_WAV_CLASS(class)      (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_AUDIO_FILE_WAV, AgsAudioFileWavClass))
#define AGS_IS_AUDIO_FILE_WAV(obj)           (G_TYPE_CHECK_INSTANCE_TYPE(obj, AGS_TYPE_AUDIO_FILE_WAV))

typedef struct _AgsAudioFileWav AgsAudioFileWav;
typedef struct _AgsAudioFileWavClass AgsAudioFileWavClass;

struct _AgsAudioFileWav
{
  GObject object;

  SNDFILE *file;
  SF_INFO info;
};

struct _AgsAudioFileWavClass
{
  GObjectClass object;
};

GType ags_audio_file_wav_get_type();

AgsAudioFileWav* ags_audio_file_wav_new(AgsAudioFile *audio_file);

#endif /*__AGS_AUDIO_FILE_WAV_H__*/
