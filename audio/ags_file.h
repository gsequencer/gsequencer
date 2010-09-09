#ifndef __AGS_AUDIO_FILE_H__
#define __AGS_AUDIO_FILE_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_AUDIO_FILE_TYPE         (ags_audio_file_get_type())
#define AGS_AUDIO_FILE(obj)         (G_TYPE_INSTANCE_CAST(obj, AGS_TYPE_AUDIO_FILE, AgsAudioFile))
#define AGS_AUDIO_FILE_CLASS(class) (G_TYPE_CLASS_CAST(class, AGS_TYPE_AUDIO_FILE, AgsAudioFile))

typedef struct _AgsAudioFile AgsAudioFile;
typedef struct _AgsAudioFileClass AgsAudioFileClass;

typedef enum{
  AGS_AUDIO_FILE_CODEC,
}AgsAudioFileFlags;

struct _AgsAudioFile{
  GObject object;

  guint flags;

  guint audio_channels;
  GList *buffer;
};

struct _AgsAudioFileClass{
  GObjectClass object;
};

void ags_audio_file_read(AgsAudioFile *audio_file, char *name);

AgsAudioFile* ags_audio_file_new(char *name);

#endif /*__AGS_AUDIO_FILE_H__*/
