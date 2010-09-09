#include "ags_audio_file_mp3.h"

GType ags_audio_file_mp3_get_type();
void ags_audio_file_mp3_class_init(AgsAudioFileMp3Class *file_mp3);
void ags_audio_file_mp3_init(AgsAudioFileMp3 *file_mp3);
void ags_audio_file_mp3_destroy(GObject *object);

GType
ags_audio_file_mp3_get_type()
{
  static GType ags_type_audio_file_mp3 = 0;

  if(!ags_type_audio_file_mp3){
    static const GTypeInfo ags_audio_file_mp3_info = {
      sizeof (AgsAudioFileMp3Class),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_file_mp3_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_file_mp3_init,
    };
    ags_type_audio_file_mp3 = g_type_register_static(G_TYPE_OBJECT, "AgsAudioFileMp3\0", &ags_audio_file_mp3_info, 0);
  }
  return (ags_type_audio_file_mp3);
}

void
ags_audio_file_mp3_class_init(AgsAudioFileMp3Class *file_mp3)
{
}

void
ags_audio_file_mp3_init(AgsAudioFileMp3 *file_mp3)
{
}

void
ags_audio_file_mp3_destroy(GObject *object)
{
}

AgsAudioFileMp3*
ags_audio_file_mp3_new(AgsAudioFile *audio_file)
{
  AgsAudioFileMp3 *file_mp3;

  file_mp3 = (AgsAudioFileMp3 *) g_object_new(AGS_TYPE_AUDIO_FILE_MP3, NULL);

  return(file_mp3);
}
