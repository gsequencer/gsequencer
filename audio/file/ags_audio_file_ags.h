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

#ifndef __AGS_AUDIO_FILE_AGS_H__
#define __AGS_AUDIO_FILE_AGS_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/file/ags_file.h>

#define AGS_TYPE_AUDIO_FILE_AGS              (ags_audio_file_ags_get_type())
#define AGS_AUDIO_FILE_AGS(obj)              (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_AUDIO_FILE_AGS, AgsAudioFileAgs))
#define AGS_AUDIO_FILE_AGS_CLASS(class)      (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_AUDIO_FILE_AGS, AgsAudioFileAgsClass))

typedef struct _AgsAudioFile AgsAudioFile;
typedef struct _AgsAudioFileClass AgsAudioFileClass;

struct _AgsAudioFileAgs
{
  GObject object;

  AgsFile *file;
};

struct _AgsAudioFileAgsClass
{
  GObjectClass object;
};

GType ags_audio_file_ags_get_type();

AgsAudioFileAgs* ags_audio_file_ags_new(AgsAudioFile *audio_file);

#endif /*__AGS_AUDIO_FILE_AGS_H__*/
