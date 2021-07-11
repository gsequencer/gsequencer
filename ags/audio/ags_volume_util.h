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

#ifndef __AGS_VOLUME_UTIL_H__
#define __AGS_VOLUME_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_VOLUME_UTIL         (ags_volume_util_get_type())
#define AGS_VOLUME_UTIL(ptr) ((AgsVolumeUtil *)(ptr))

typedef struct _AgsVolumeUtil AgsVolumeUtil;

struct _AgsVolumeUtil
{
  gpointer destination;
  gpointer source;
  
  guint audio_channels;
  
  guint buffer_length;
  guint audio_buffer_util_format;
  
  gdouble volume;
};

GType ags_volume_util_get_type(void);

AgsVolumeUtil* ags_volume_util_alloc();

gpointer ags_volume_util_copy(AgsVolumeUtil *ptr);
void ags_volume_util_free(AgsVolumeUtil *ptr);

void ags_volume_util_compute_s8(AgsVolumeUtil *volume_util);
void ags_volume_util_compute_s16(AgsVolumeUtil *volume_util);
void ags_volume_util_compute_s24(AgsVolumeUtil *volume_util);
void ags_volume_util_compute_s32(AgsVolumeUtil *volume_util);
void ags_volume_util_compute_s64(AgsVolumeUtil *volume_util);
void ags_volume_util_compute_float(AgsVolumeUtil *volume_util);
void ags_volume_util_compute_double(AgsVolumeUtil *volume_util);
void ags_volume_util_compute_complex(AgsVolumeUtil *volume_util);

void ags_volume_util_compute(AgsVolumeUtil *volume_util);

G_END_DECLS

#endif /*__AGS_VOLUME_UTIL_H__*/
