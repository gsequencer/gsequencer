#include "ags_audio_file_ags.h"

GType ags_audio_file_ags_get_type();
void ags_audio_file_ags_class_init(AgsAudioFileAgsClass *file_ags);
void ags_audio_file_ags_init(AgsAudioFileAgs *file_ags);
void ags_audio_file_ags_destroy(GObject *object);

GType
ags_audio_file_ags_get_type()
{
  static GType ags_type_audio_file_ags = 0;

  if(!ags_type_audio_file_ags){
    static const GTypeInfo ags_audio_file_ags_info = {
      sizeof (AgsAudioFileAgsClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_file_ags_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_file_ags_init,
    };
    ags_type_audio_file_ags = g_type_register_static(G_TYPE_OBJECT, "AgsAudioFileAgs\0", &ags_audio_file_ags_info, 0);
  }
  return (ags_type_audio_file_ags);
}

void
ags_audio_file_ags_class_init(AgsAudioFileAgsClass *file_ags)
{
}

void
ags_audio_file_ags_init(AgsAudioFileAgs *file_ags)
{
  file_ags->audio_file = NULL;
}

void
ags_audio_file_ags_destroy(GObject *object)
{
}

AgsAudioFileAgs*
ags_audio_file_ags_new(AgsAudioFile *audio_file)
{
  AgsAudioFileAgs *file_ags;

  file_ags = (AgsAudioFileAgs *) g_object_new(AGS_TYPE_AUDIO_FILE_AGS, NULL);

  file_ags->file = ags_file_new();
  file_ags->file->name = audio_file->name;
  ags_file_read(file_ags->file);

  return(file_ags);
}
