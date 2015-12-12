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

#ifndef __AGS_START_READ_FILE_H__
#define __AGS_START_READ_FILE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>

#include <ags/file/ags_file.h>

#define AGS_TYPE_START_READ_FILE                (ags_start_read_file_get_type())
#define AGS_START_READ_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_START_READ_FILE, AgsStartReadFile))
#define AGS_START_READ_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_START_READ_FILE, AgsStartReadFileClass))
#define AGS_IS_START_READ_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_START_READ_FILE))
#define AGS_IS_START_READ_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_START_READ_FILE))
#define AGS_START_READ_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_START_READ_FILE, AgsStartReadFileClass))

typedef struct _AgsStartReadFile AgsStartReadFile;
typedef struct _AgsStartReadFileClass AgsStartReadFileClass;

struct _AgsStartReadFile
{
  AgsTask task;

  AgsFile *file;
};

struct _AgsStartReadFileClass
{
  AgsTaskClass task;
};

GType ags_start_read_file_get_type();

AgsStartReadFile* ags_start_read_file_new(AgsFile *file);

#endif /*__AGS_START_READ_FILE_H__*/

