#ifndef __AGS_AUDIO_FILE_AGS_H__
#define __AGS_AUDIO_FILE_AGS_H__

#include <glib.h>
#include <glib-object.h>

#include "../../file/ags_file.h"

#define AGS_TYPE_AUDIO_FILE_AGS              (ags_audio_file_ags_get_type())
#define AGS_AUDIO_FILE_AGS(obj)              (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_AUDIO_FILE_AGS, AgsAudioFileAgs))
#define AGS_AUDIO_FILE_AGS_CLASS(class)      (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_AUDIO_FILE_AGS, AgsAudioFileAgsClass))

typedef struct _AgsAudioFile AgsAudioFile;
typedef struct _AgsAudioFileClass AgsAudioFileClass;

struct _AgsAudioFileAgs
{
  GObject object;

  AgsFile *file;
};

struct _AgsAudioFileAgsClass
{
  GObjectClass object;
};

AgsAudioFileAgs* ags_audio_file_ags_new(AgsAudioFile *audio_file);

#endif /*__AGS_AUDIO_FILE_AGS_H__*/
