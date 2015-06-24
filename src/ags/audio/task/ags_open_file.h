/* This file is part of GSequencer.
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

#ifndef __AGS_OPEN_FILE_H__
#define __AGS_OPEN_FILE_H__

#include <glib-object.h>

#include <ags/audio/ags_task.h>
#include <ags/audio/ags_audio.h>

#define AGS_TYPE_OPEN_FILE                (ags_open_file_get_type())
#define AGS_OPEN_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OPEN_FILE, AgsOpenFile))
#define AGS_OPEN_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OPEN_FILE, AgsOpenFileClass))
#define AGS_IS_OPEN_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_OPEN_FILE))
#define AGS_IS_OPEN_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_OPEN_FILE))
#define AGS_OPEN_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_OPEN_FILE, AgsOpenFileClass))

typedef struct _AgsOpenFile AgsOpenFile;
typedef struct _AgsOpenFileClass AgsOpenFileClass;

struct _AgsOpenFile
{
  AgsTask task;

  AgsAudio *audio;

  GSList *filenames;

  gboolean overwrite_channels;
  gboolean create_channels;
};

struct _AgsOpenFileClass
{
  AgsTaskClass task;
};

GType ags_open_file_get_type();

AgsOpenFile* ags_open_file_new(AgsAudio *audio,
			       GSList *filenames,
			       gboolean overwrite_channels,
			       gboolean create_channels);

#endif /*__AGS_OPEN_FILE_H__*/
