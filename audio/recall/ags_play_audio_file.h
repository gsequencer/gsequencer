#ifndef __AGS_PLAY_AUDIO_FILE_H__
#define __AGS_PLAY_AUDIO_FILE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall.h>

#include <ags/audio/file/ags_audio_file.h>
#include <ags/audio/ags_devout.h>

#define AGS_TYPE_PLAY_AUDIO_FILE                (ags_play_audio_file_get_type())
#define AGS_PLAY_AUDIO_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_AUDIO_FILE, AgsPlayAudioFile))
#define AGS_PLAY_AUDIO_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AgsPlayAudioFileClass))
#define AGS_IS_PLAY_AUDIO_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PLAY_AUDIO_FILE))
#define AGS_IS_PLAY_AUDIO_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PLAY_AUDIO_FILE))
#define AGS_PLAY_AUDIO_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PLAY_AUDIO_FILE, AgsPlayAudioFileClass))

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
