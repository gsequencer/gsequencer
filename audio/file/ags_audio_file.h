#ifndef __AGS_AUDIO_FILE_H__
#define __AGS_AUDIO_FILE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_devout.h>

#define AGS_TYPE_AUDIO_FILE                (ags_audio_file_get_type())
#define AGS_AUDIO_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_FILE, AgsAudioFile))
#define AGS_AUDIO_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUDIO_FILE, AgsAudioFileClass))
#define AGS_IS_AUDIO_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_AUDIO_FILE))
#define AGS_IS_AUDIO_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_AUDIO_FILE))
#define AGS_AUDIO_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_AUDIO_FILE, AgsAudioFileClass))

typedef struct _AgsAudioFile AgsAudioFile;
typedef struct _AgsAudioFileClass AgsAudioFileClass;

struct _AgsAudioFile
{
  GObject object;

  AgsDevout *devout;

  gchar *name;
  guint frames;
  guint channels;

  guint start_channel;
  guint audio_channels;

  GList *audio_signal;

  GObject *file;
};

struct _AgsAudioFileClass
{
  GObjectClass object;
};

GType ags_audio_file_get_type();

gboolean ags_audio_file_open(AgsAudioFile *audio_file);
void ags_audio_file_read_audio_signal(AgsAudioFile *audio_file);
void ags_audio_file_close(AgsAudioFile *audio_file);

AgsAudioFile* ags_audio_file_new(gchar *name,
				 AgsDevout *devout,
				 guint start_channel, guint audio_channels);

#endif /*__AGS_AUDIO_FILE_H__*/
