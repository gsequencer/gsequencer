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

#include <ags/audio/ags_audio_buffer_util.h>

G_BEGIN_DECLS

#define AGS_TYPE_VOLUME_UTIL         (ags_volume_util_get_type())
#define AGS_VOLUME_UTIL(ptr) ((AgsVolumeUtil *)(ptr))

#define AGS_VOLUME_UTIL_DEFAULT_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)
#define AGS_VOLUME_UTIL_DEFAULT_AUDIO_BUFFER_UTIL_FORMAT (AGS_AUDIO_BUFFER_UTIL_S16)

typedef struct _AgsVolumeUtil AgsVolumeUtil;

struct _AgsVolumeUtil
{
  gpointer destination;
  guint destination_stride;

  gpointer source;
  guint source_stride;
  
  guint buffer_length;
  guint format;

  guint audio_buffer_util_format;
  
  gdouble volume;
};

GType ags_volume_util_get_type(void);

AgsVolumeUtil* ags_volume_util_alloc();

gpointer ags_volume_util_copy(AgsVolumeUtil *ptr);
void ags_volume_util_free(AgsVolumeUtil *ptr);

/* getter/setter */
gpointer ags_volume_util_get_destination(AgsVolumeUtil *volume_util);
void ags_volume_util_set_destination(AgsVolumeUtil *volume_util,
				     gpointer destination);

guint ags_volume_util_get_destination_stride(AgsVolumeUtil *volume_util);
void ags_volume_util_set_destination_stride(AgsVolumeUtil *volume_util,
					    guint destination_stride);

gpointer ags_volume_util_get_source(AgsVolumeUtil *volume_util);
void ags_volume_util_set_source(AgsVolumeUtil *volume_util,
				gpointer source);

guint ags_volume_util_get_source_stride(AgsVolumeUtil *volume_util);
void ags_volume_util_set_source_stride(AgsVolumeUtil *volume_util,
				       guint source_stride);

guint ags_volume_util_get_buffer_length(AgsVolumeUtil *volume_util);
void ags_volume_util_set_buffer_length(AgsVolumeUtil *volume_util,
				       guint buffer_length);

guint ags_volume_util_get_format(AgsVolumeUtil *volume_util);
void ags_volume_util_set_format(AgsVolumeUtil *volume_util,
				guint format);

guint ags_volume_util_get_audio_buffer_util_format(AgsVolumeUtil *volume_util);
void ags_volume_util_set_audio_buffer_util_format(AgsVolumeUtil *volume_util,
						  guint audio_buffer_util_format);

gdouble ags_volume_util_get_volume(AgsVolumeUtil *volume_util);
void ags_volume_util_set_volume(AgsVolumeUtil *volume_util,
				gdouble volume);

/* compute */
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
