#include "ags_audio_file_ogg.h"

#include <stdlib.h>
//#include <ogg/ogg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void ags_audio_file_ogg_class_init(AgsAudioFileOggClass *file_ogg);
void ags_audio_file_ogg_init(AgsAudioFileOgg *file_ogg);
void ags_audio_file_ogg_destroy(GObject *object);

GType
ags_audio_file_ogg_get_type()
{
  static GType ags_type_audio_file_ogg = 0;

  if(!ags_type_audio_file_ogg){
    static const GTypeInfo ags_audio_file_ogg_info = {
      sizeof (AgsAudioFileOggClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_file_ogg_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_file_ogg_init,
    };
    ags_type_audio_file_ogg = g_type_register_static(G_TYPE_OBJECT, "AgsAudioFileOgg\0", &ags_audio_file_ogg_info, 0);
  }
  return (ags_type_audio_file_ogg);
}

void
ags_audio_file_ogg_class_init(AgsAudioFileOggClass *file_ogg)
{
}

void
ags_audio_file_ogg_init(AgsAudioFileOgg *file_ogg)
{
  //  file_ogg->oy = (ogg_sync_state *) malloc(sizeof(ogg_sync_state));
}

void
ags_audio_file_ogg_destroy(GObject *object)
{
  AgsAudioFileOgg *file_ogg;

  //  free(file_ogg->oy);
}

AgsAudioFileOgg*
ags_audio_file_ogg_new(AgsAudioFile *audio_file)
{
  AgsAudioFileOgg *file_ogg;

  file_ogg = (AgsAudioFileOgg *) g_object_new(AGS_TYPE_AUDIO_FILE_OGG, NULL);

  file_ogg->fd = open(audio_file->name, O_RDONLY, 0);
  //  file_ogg->buffer = ogg_sync_buffer(file_ogg->oy, );

  return(file_ogg);
}
