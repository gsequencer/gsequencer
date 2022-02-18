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

#ifndef __AGS_SIMPLE_FILE_READ_H__
#define __AGS_SIMPLE_FILE_READ_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/file/ags_simple_file.h>

G_BEGIN_DECLS

#define AGS_TYPE_SIMPLE_FILE_READ                (ags_simple_file_read_get_type())
#define AGS_SIMPLE_FILE_READ(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SIMPLE_FILE_READ, AgsSimpleFileRead))
#define AGS_SIMPLE_FILE_READ_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SIMPLE_FILE_READ, AgsSimpleFileReadClass))
#define AGS_IS_SIMPLE_FILE_READ(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SIMPLE_FILE_READ))
#define AGS_IS_SIMPLE_FILE_READ_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SIMPLE_FILE_READ))
#define AGS_SIMPLE_FILE_READ_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SIMPLE_FILE_READ, AgsSimpleFileReadClass))

typedef struct _AgsSimpleFileRead AgsSimpleFileRead;
typedef struct _AgsSimpleFileReadClass AgsSimpleFileReadClass;

struct _AgsSimpleFileRead
{
  AgsTask task;

  AgsSimpleFile *simple_file;
};

struct _AgsSimpleFileReadClass
{
  AgsTaskClass task;
};

GType ags_simple_file_read_get_type();

AgsSimpleFileRead* ags_simple_file_read_new(AgsSimpleFile *simple_file);

G_END_DECLS

#endif /*__AGS_SIMPLE_FILE_READ_H__*/
