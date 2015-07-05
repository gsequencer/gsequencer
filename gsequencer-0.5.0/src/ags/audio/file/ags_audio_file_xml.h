/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __AGS_AUDIO_FILE_XML_H__
#define __AGS_AUDIO_FILE_XML_H__

#include <glib.h>
#include <glib-object.h>
#include <sndfile.h>

#include <ags/audio/file/ags_audio_file.h>

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

GType ags_audio_file_xml_get_type();

AgsAudioFileXml* ags_audio_file_xml_new(AgsAudioFile *audio_file);

#endif /*__AGS_AUDIO_FILE_XML_H__*/
