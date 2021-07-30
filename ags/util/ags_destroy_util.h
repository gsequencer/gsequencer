/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_DESTROY_UTIL_H__
#define __AGS_DESTROY_UTIL_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define AGS_TYPE_DESTROY_UTIL         (ags_destroy_util_get_type())

typedef struct _AgsDestroyUtil AgsDestroyUtil;

struct _AgsDestroyUtil
{
  //empty
};

GType ags_destroy_util_get_type(void);

void ags_destroy_util_dispose_and_unref(GObject *gobject);

G_END_DECLS

#endif /*__AGS_DESTROY_UTIL_H__*/
