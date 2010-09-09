#ifndef __AGS_AUDIO_FILE_MP3_H__
#define __AGS_AUDIO_FILE_MP3_H__

#include <glib.h>
#include <glib-object.h>
#include <sndfile.h>

#include "ags_audio_file.h"

#define AGS_TYPE_AUDIO_FILE_MP3              (ags_audio_file_mp3_get_type())
#define AGS_AUDIO_FILE_MP3(obj)              (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_AUDIO_FILE_MP3, AgsAudioFileMp3))
#define AGS_AUDIO_FILE_MP3_CLASS(class)      (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_AUDIO_FILE_MP3, AgsAudioFileMp3Class))
#define AGS_IS_AUDIO_FILE_MP3(obj)           (G_TYPE_CHECK_INSTANCE_TYPE(obj, AGS_TYPE_AUDIO_FILE_MP3))

typedef struct _AgsAudioFileMp3 AgsAudioFileMp3;
typedef struct _AgsAudioFileMp3Class AgsAudioFileMp3Class;

struct _AgsAudioFileMp3
{
  AgsAudioFile audio_file;
};

struct _AgsAudioFileMp3Class
{
  AgsAudioFileClass audio_file;
};

AgsAudioFileMp3* ags_audio_file_mp3_new(AgsAudioFile *audio_file);

#endif /*__AGS_AUDIO_FILE_MP3_H__*/
