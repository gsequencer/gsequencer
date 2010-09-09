#ifndef __AGS_AUDIO_FILE_RAW_H__
#define __AGS_AUDIO_FILE_RAW_H__

#include <glib.h>
#include <glib-object.h>
#include <sndfile.h>

#include "ags_audio_file.h"

#define AGS_TYPE_AUDIO_FILE_RAW              (ags_audio_file_raw_get_type())
#define AGS_AUDIO_FILE_RAW(obj)              (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_AUDIO_FILE_RAW, AgsAudioFileRaw))
#define AGS_AUDIO_FILE_RAW_CLASS(class)      (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_AUDIO_FILE_RAW, AgsAudioFileRawClass))
#define AGS_IS_AUDIO_FILE_RAW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE(obj, AGS_TYPE_AUDIO_FILE_RAW))

typedef struct _AgsAudioFileRaw AgsAudioFileRaw;
typedef struct _AgsAudioFileRawClass AgsAudioFileRawClass;

struct _AgsAudioFileRaw
{
  GObject object;

  int fd;
};

struct _AgsAudioFileRawClass
{
  GObjectClass object;
};

AgsAudioFileRaw* ags_audio_file_raw_new(AgsAudioFile *audio_file);

#endif /*__AGS_AUDIO_FILE_RAW_H__*/
