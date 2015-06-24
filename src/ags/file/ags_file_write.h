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

#ifndef __AGS_FILE_WRITE_H__
#define __AGS_FILE_WRITE_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <libxml/tree.h>

#define AGS_TYPE_FILE_WRITE                (ags_file_write_get_type())
#define AGS_FILE_WRITE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FILE_WRITE, AgsFileWrite))
#define AGS_FILE_WRITE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FILE_WRITE, AgsFileWriteClass))
#define AGS_IS_FILE_WRITE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FILE_WRITE))
#define AGS_IS_FILE_WRITE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FILE_WRITE))
#define AGS_FILE_WRITE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FILE_WRITE, AgsFileWriteClass))

typedef struct _AgsFileWrite AgsFileWrite;
typedef struct _AgsFileWriteClass AgsFileWriteClass;

struct _AgsFileWrite
{
  GObject object;
};

struct _AgsFileWriteClass
{
  GObjectClass object;
};

GType ags_file_write_get_type(void);

AgsFileWrite* ags_file_write_new();

#endif /*__AGS_FILE_WRITE_H__*/
