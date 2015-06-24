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

#ifndef __AGS_FILE_READ_H__
#define __AGS_FILE_READ_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <libxml/tree.h>

#define AGS_TYPE_FILE_READ                (ags_file_read_get_type())
#define AGS_FILE_READ(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FILE_READ, AgsFileRead))
#define AGS_FILE_READ_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FILE_READ, AgsFileReadClass))
#define AGS_IS_FILE_READ(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FILE_READ))
#define AGS_IS_FILE_READ_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FILE_READ))
#define AGS_FILE_READ_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FILE_READ, AgsFileReadClass))

typedef struct _AgsFileRead AgsFileRead;
typedef struct _AgsFileReadClass AgsFileReadClass;

struct _AgsFileRead
{
  GObject object;
};

struct _AgsFileReadClass
{
  GObjectClass object;
};

GType ags_file_read_get_type(void);

AgsFileRead* ags_file_read_new();

#endif /*__AGS_FILE_READ_H__*/
