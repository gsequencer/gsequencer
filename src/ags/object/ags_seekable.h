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

#ifndef __AGS_SEEKABLE_H__
#define __AGS_SEEKABLE_H__

#include <glib-object.h>

#define AGS_TYPE_SEEKABLE                    (ags_seekable_get_type())
#define AGS_SEEKABLE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SEEKABLE, AgsSeekable))
#define AGS_SEEKABLE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_SEEKABLE, AgsSeekableInterface))
#define AGS_IS_SEEKABLE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SEEKABLE))
#define AGS_IS_SEEKABLE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_SEEKABLE))
#define AGS_SEEKABLE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_SEEKABLE, AgsSeekableInterface))

typedef void AgsSeekable;
typedef struct _AgsSeekableInterface AgsSeekableInterface;

struct _AgsSeekableInterface
{
  GTypeInterface interface;

  void (*seek)(AgsSeekable *seekable, guint steps, gboolean forward);
};

GType ags_seekable_get_type();

void ags_seekable_seek(AgsSeekable *seekable, guint steps, gboolean forward);

#endif /*__AGS_SEEKABLE_H__*/

