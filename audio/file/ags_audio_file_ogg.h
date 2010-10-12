#ifndef __AGS_AUDIO_FILE_OGG_H__
#define __AGS_AUDIO_FILE_OGG_H__

#include <glib.h>
#include <glib-object.h>
//#include <ogg/ogg.h>

#include "ags_audio_file.h"

#define AGS_TYPE_AUDIO_FILE_OGG              (ags_audio_file_ogg_get_type())
#define AGS_AUDIO_FILE_OGG(obj)              (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_AUDIO_FILE_OGG, AgsAudioFileOgg))
#define AGS_AUDIO_FILE_OGG_CLASS(class)      (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_AUDIO_FILE_OGG, AgsAudioFileOggClass))
#define AGS_IS_AUDIO_FILE_OGG(obj)           (G_TYPE_CHECK_INSTANCE_TYPE(obj, AGS_TYPE_AUDIO_FILE_OGG))

typedef struct _AgsAudioFileOgg AgsAudioFileOgg;
typedef struct _AgsAudioFileOggClass AgsAudioFileOggClass;

struct _AgsAudioFileOgg
{
  GObject object;

  int fd;

  //  ogg_sync_state *oy;
};

struct _AgsAudioFileOggClass
{
  GObjectClass object;
};

GType ags_audio_file_ogg_get_type();

AgsAudioFileOgg* ags_audio_file_ogg_new(AgsAudioFile *audio_file);

#endif /*__AGS_AUDIO_FILE_OGG_H__*/
