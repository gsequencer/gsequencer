#ifndef __AGS_PLAY_AUDIO_FILE_H__
#define __AGS_PLAY_AUDIO_FILE_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_recall.h"

#include "../file/ags_audio_file.h"
#include "../ags_devout.h"

#define AGS_TYPE_PLAY_AUDIO_FILE           (ags_play_audio_file_get_type())
#define AGS_PLAY_AUDIO_FILE(obj)           (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_AUDIO_FILE, AgsPlayAudioFile))
#define AGS_PLAY_AUDIO_FILE_CLASS(class)   (G_TYPE_CHECK_CLASS_CAST((class), AgsPlayAudioFileClass))

typedef struct _AgsPlayAudioFile AgsPlayAudioFile;
typedef struct _AgsPlayAudioFileClass AgsPlayAudioFileClass;

struct _AgsPlayAudioFile
{
  AgsRecall recall;

  AgsAudioFile *audio_file;
  guint current;

  AgsDevout *devout;
};

struct _AgsPlayAudioFileClass
{
  AgsRecallClass recall;
};

GType ags_play_audio_file_get_type();

AgsPlayAudioFile* ags_play_audio_file_new();

#endif /*__AGS_PLAY_AUDIO_FILE_H__*/
