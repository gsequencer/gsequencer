/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#ifndef __AGS_SOUND_TYPES_H__
#define __AGS_SOUND_TYPES_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

typedef gint8 ags_v8s8 __attribute__ ((vector_size(8 * sizeof(gint8))));
typedef gint16 ags_v8s16 __attribute__ ((vector_size(8 * sizeof(gint16))));
typedef gint32 ags_v8s32 __attribute__ ((vector_size(8 * sizeof(gint32))));
typedef gint64 ags_v8s64 __attribute__ ((vector_size(8 * sizeof(gint64))));
typedef gfloat ags_v8float __attribute__ ((vector_size(8 * sizeof(gfloat))));
typedef gdouble ags_v8double __attribute__ ((vector_size(8 * sizeof(gdouble))));

G_END_DECLS

#endif /*__AGS_SOUND_TYPES_H__*/
