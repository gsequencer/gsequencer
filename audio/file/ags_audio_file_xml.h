#ifndef __AGS_AUDIO_FILE_XML_H__
#define __AGS_AUDIO_FILE_XML_H__

#include <glib.h>
#include <glib-object.h>
#include <sndfile.h>

#include "ags_audio_file.h"

#define AGS_TYPE_AUDIO_FILE_XML              (ags_audio_file_xml_get_type())
#define AGS_AUDIO_FILE_XML(obj)              (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_AUDIO_FILE_XML, AgsAudioFileXml))
#define AGS_AUDIO_FILE_XML_CLASS(class)      (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_AUDIO_FILE_XML, AgsAudioFileXmlClass))
#define AGS_IS_AUDIO_FILE_XML(obj)           (G_TYPE_CHECK_INSTANCE_TYPE(obj, AGS_TYPE_AUDIO_FILE_XML))

typedef struct _AgsAudioFileXml AgsAudioFileXml;
typedef struct _AgsAudioFileXmlClass AgsAudioFileXmlClass;

struct _AgsAudioFileXml
{
  GObject object;
};

struct _AgsAudioFileXmlClass
{
  GObjectClass object;
};

AgsAudioFileXml* ags_audio_file_xml_new(AgsAudioFile *audio_file);

#endif /*__AGS_AUDIO_FILE_XML_H__*/
