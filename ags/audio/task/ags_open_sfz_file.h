/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_OPEN_SFZ_FILE_H__
#define __AGS_OPEN_SFZ_FILE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>

#include <ags/audio/file/ags_sfz_file.h>

G_BEGIN_DECLS

#define AGS_TYPE_OPEN_SFZ_FILE                (ags_open_sfz_file_get_type())
#define AGS_OPEN_SFZ_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OPEN_SFZ_FILE, AgsOpenSFZFile))
#define AGS_OPEN_SFZ_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OPEN_SFZ_FILE, AgsOpenSFZFileClass))
#define AGS_IS_OPEN_SFZ_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_OPEN_SFZ_FILE))
#define AGS_IS_OPEN_SFZ_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_OPEN_SFZ_FILE))
#define AGS_OPEN_SFZ_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_OPEN_SFZ_FILE, AgsOpenSFZFileClass))

typedef struct _AgsOpenSFZFile AgsOpenSFZFile;
typedef struct _AgsOpenSFZFileClass AgsOpenSFZFileClass;

struct _AgsOpenSFZFile
{
  AgsTask task;

  AgsAudio *audio;

  AgsSFZFile *sfz_file;
  
  gchar *filename;
  
  guint start_pad;
};

struct _AgsOpenSFZFileClass
{
  AgsTaskClass task;
};

GType ags_open_sfz_file_get_type();

AgsOpenSFZFile* ags_open_sfz_file_new(AgsAudio *audio,
				      AgsSFZFile *sfz_file,
				      gchar *filename,
				      guint start_pad);

G_END_DECLS

#endif /*__AGS_OPEN_SFZ_FILE_H__*/
