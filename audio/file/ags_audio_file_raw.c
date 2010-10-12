#include "ags_audio_file_raw.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void ags_audio_file_raw_class_init(AgsAudioFileRawClass *file_raw);
void ags_audio_file_raw_init(AgsAudioFileRaw *file_raw);
void ags_audio_file_raw_destroy(GObject *object);

GType
ags_audio_file_raw_get_type()
{
  static GType ags_type_audio_file_raw = 0;

  if(!ags_type_audio_file_raw){
    static const GTypeInfo ags_audio_file_raw_info = {
      sizeof (AgsAudioFileRawClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_file_raw_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_file_raw_init,
    };
    ags_type_audio_file_raw = g_type_register_static(G_TYPE_OBJECT, "AgsAudioFileRaw\0", &ags_audio_file_raw_info, 0);
  }
  return (ags_type_audio_file_raw);
}

void
ags_audio_file_raw_class_init(AgsAudioFileRawClass *file_raw)
{
}

void
ags_audio_file_raw_init(AgsAudioFileRaw *file_raw)
{
}

void
ags_audio_file_raw_destroy(GObject *object)
{
  AgsAudioFileRaw *file_raw;

  file_raw = (AgsAudioFileRaw *) object;
  close(file_raw->fd);
}

AgsAudioFileRaw*
ags_audio_file_raw_new(AgsAudioFile *audio_file)
{
  AgsAudioFileRaw *file_raw;

  file_raw = (AgsAudioFileRaw *) g_object_new(AGS_TYPE_AUDIO_FILE_RAW, NULL);

  fprintf(stdout, "ags_audio_file_raw_new:\n\0");

  file_raw->fd = open(audio_file->name, O_RDONLY, 0);

  return(file_raw);
}
