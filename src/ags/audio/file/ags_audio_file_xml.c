/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/file/ags_audio_file_xml.h>

void ags_audio_file_xml_class_init(AgsAudioFileXmlClass *file_xml);
void ags_audio_file_xml_init(AgsAudioFileXml *file_xml);
void ags_audio_file_xml_destroy(GObject *object);

GType
ags_audio_file_xml_get_type()
{
  static GType ags_type_audio_file_xml = 0;

  if(!ags_type_audio_file_xml){
    static const GTypeInfo ags_audio_file_xml_info = {
      sizeof (AgsAudioFileXmlClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_file_xml_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_file_xml_init,
    };
    ags_type_audio_file_xml = g_type_register_static(G_TYPE_OBJECT, "AgsAudioFileXml", &ags_audio_file_xml_info, 0);
  }
  return (ags_type_audio_file_xml);
}

void
ags_audio_file_xml_class_init(AgsAudioFileXmlClass *file_xml)
{
}

void
ags_audio_file_xml_init(AgsAudioFileXml *file_xml)
{
}

void
ags_audio_file_xml_destroy(GObject *object)
{
}

AgsAudioFileXml*
ags_audio_file_xml_new(AgsAudioFile *audio_file)
{
  AgsAudioFileXml *file_xml;

  file_xml = (AgsAudioFileXml *) g_object_new(AGS_TYPE_AUDIO_FILE_XML, NULL);

  return(file_xml);
}
