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

#ifndef __AGS_SIMPLE_FILE_WRITE_H__
#define __AGS_SIMPLE_FILE_WRITE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/file/ags_simple_file.h>

#define AGS_TYPE_SIMPLE_FILE_WRITE                (ags_simple_file_write_get_type())
#define AGS_SIMPLE_FILE_WRITE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SIMPLE_FILE_WRITE, AgsSimpleFileWrite))
#define AGS_SIMPLE_FILE_WRITE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SIMPLE_FILE_WRITE, AgsSimpleFileWriteClass))
#define AGS_IS_SIMPLE_FILE_WRITE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SIMPLE_FILE_WRITE))
#define AGS_IS_SIMPLE_FILE_WRITE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SIMPLE_FILE_WRITE))
#define AGS_SIMPLE_FILE_WRITE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SIMPLE_FILE_WRITE, AgsSimpleFileWriteClass))

typedef struct _AgsSimpleFileWrite AgsSimpleFileWrite;
typedef struct _AgsSimpleFileWriteClass AgsSimpleFileWriteClass;

struct _AgsSimpleFileWrite
{
  AgsTask task;

  AgsSimpleFile *simple_file;
};

struct _AgsSimpleFileWriteClass
{
  AgsTaskClass task;
};

GType ags_simple_file_write_get_type();

AgsSimpleFileWrite* ags_simple_file_write_new(AgsSimpleFile *simple_file);

#endif /*__AGS_SIMPLE_FILE_WRITE_H__*/
