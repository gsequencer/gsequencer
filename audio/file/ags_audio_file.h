#ifndef __AGS_AUDIO_FILE_H__
#define __AGS_AUDIO_FILE_H__

#include <glib.h>
#include <glib-object.h>

#include "../ags_devout.h"

#define AGS_TYPE_AUDIO_FILE                (ags_audio_file_get_type())
#define AGS_AUDIO_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_FILE, AgsAudioFile))
#define AGS_AUDIO_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUDIO_FILE, AgsAudioFileClass))
#define AGS_IS_AUDIO_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_AUDIO_FILE))
#define AGS_IS_AUDIO_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_AUDIO_FILE))
#define AGS_AUDIO_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_AUDIO_FILE, AgsAudioFileClass))

typedef struct _AgsAudioFile AgsAudioFile;
typedef struct _AgsAudioFileClass AgsAudioFileClass;

typedef enum{
  AGS_AUDIO_FILE_ALL_CHANNELS        = 1,
}AgsAudioFileFlags;

struct _AgsAudioFile
{
  GObject object;

  guint flags;

  AgsDevout *devout;

  gchar *name;
  guint frames;
  guint channels;

  guint buffer_channels;
  guint channel;
  short *buffer;

  GList *audio_signal;

  GObject *file;
};

struct _AgsAudioFileClass
{
  GObjectClass object;

  void (*read_buffer)(AgsAudioFile *audio_file);
};

void ags_audio_file_connect(AgsAudioFile *audio_file);

void ags_audio_file_open(AgsAudioFile *audio_file);
void ags_audio_file_read_audio_signal(AgsAudioFile *audio_file);

void ags_audio_file_set_devout(AgsAudioFile *audio_file, AgsDevout *devout);

void ags_audio_file_close(AgsAudioFile *audio_file);

AgsAudioFile* ags_audio_file_new(); // audio_file->audio_signal->devout should have been set before reading

#endif /*__AGS_AUDIO_FILE_H__*/
