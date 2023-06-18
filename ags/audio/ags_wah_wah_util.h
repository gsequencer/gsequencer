/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_WAH_WAH_UTIL_H__
#define __AGS_WAH_WAH_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_buffer_util.h>

G_BEGIN_DECLS

#define AGS_TYPE_WAH_WAH_UTIL         (ags_wah_wah_util_get_type())
#define AGS_WAH_WAH_UTIL(ptr) ((AgsWahWahUtil *)(ptr))

#define AGS_WAH_WAH_UTIL_DEFAULT_FORMAT (AGS_SOUNDCARD_SIGNED_16_BIT)
#define AGS_WAH_WAH_UTIL_DEFAULT_AUDIO_BUFFER_UTIL_FORMAT (AGS_AUDIO_BUFFER_UTIL_S16)

typedef struct _AgsWahWahUtil AgsWahWahUtil;

struct _AgsWahWahUtil
{
  gpointer destination;
  guint destination_stride;

  gpointer source;
  guint source_stride;
  
  guint buffer_length;
  guint format;
  guint samplerate;
  
  gdouble volume;
  gdouble amount;

  gboolean wah_wah_enabled;

  gdouble wah_wah_delay;
  
  gdouble wah_wah_lfo_depth;
  gdouble wah_wah_lfo_freq;
  gdouble wah_wah_tuning;

  guint wah_wah_lfo_frame_count;
  guint wah_wah_lfo_offset;
};

GType ags_wah_wah_util_get_type(void);

AgsWahWahUtil* ags_wah_wah_util_alloc();

gpointer ags_wah_wah_util_copy(AgsWahWahUtil *ptr);
void ags_wah_wah_util_free(AgsWahWahUtil *ptr);

gpointer ags_wah_wah_util_get_destination(AgsWahWahUtil *wah_wah_util);
void ags_wah_wah_util_set_destination(AgsWahWahUtil *wah_wah_util,
				       gpointer destination);

guint ags_wah_wah_util_get_destination_stride(AgsWahWahUtil *wah_wah_util);
void ags_wah_wah_util_set_destination_stride(AgsWahWahUtil *wah_wah_util,
					      guint destination_stride);

gpointer ags_wah_wah_util_get_source(AgsWahWahUtil *wah_wah_util);
void ags_wah_wah_util_set_source(AgsWahWahUtil *wah_wah_util,
				  gpointer source);

guint ags_wah_wah_util_get_source_stride(AgsWahWahUtil *wah_wah_util);
void ags_wah_wah_util_set_source_stride(AgsWahWahUtil *wah_wah_util,
					 guint source_stride);

guint ags_wah_wah_util_get_buffer_length(AgsWahWahUtil *wah_wah_util);
void ags_wah_wah_util_set_buffer_length(AgsWahWahUtil *wah_wah_util,
					 guint buffer_length);

guint ags_wah_wah_util_get_format(AgsWahWahUtil *wah_wah_util);
void ags_wah_wah_util_set_format(AgsWahWahUtil *wah_wah_util,
				  guint format);

gdouble ags_wah_wah_util_get_volume(AgsWahWahUtil *wah_wah_util);
void ags_wah_wah_util_set_volume(AgsWahWahUtil *wah_wah_util,
				  gdouble volume);

gdouble ags_wah_wah_util_get_amount(AgsWahWahUtil *wah_wah_util);
void ags_wah_wah_util_set_amount(AgsWahWahUtil *wah_wah_util,
				  gdouble amount);

gdouble ags_wah_wah_util_get_wah_wah_lfo_depth(AgsWahWahUtil *wah_wah_util);
void ags_wah_wah_util_set_wah_wah_lfo_depth(AgsWahWahUtil *wah_wah_util,
					    gdouble wah_wah_lfo_depth);

gdouble ags_wah_wah_util_get_wah_wah_lfo_freq(AgsWahWahUtil *wah_wah_util);
void ags_wah_wah_util_set_wah_wah_lfo_freq(AgsWahWahUtil *wah_wah_util,
					   gdouble wah_wah_lfo_freq);

gdouble ags_wah_wah_util_get_wah_wah_tuning(AgsWahWahUtil *wah_wah_util);
void ags_wah_wah_util_set_wah_wah_tuning(AgsWahWahUtil *wah_wah_util,
					 gdouble wah_wah_tuning);

guint ags_wah_wah_util_get_wah_wah_lfo_offset(AgsWahWahUtil *wah_wah_util);
void ags_wah_wah_util_set_wah_wah_lfo_offset(AgsWahWahUtil *wah_wah_util,
					      guint wah_wah_lfo_offset);

void ags_wah_wah_util_compute_s8(AgsWahWahUtil *wah_wah_util);
void ags_wah_wah_util_compute_s16(AgsWahWahUtil *wah_wah_util);
void ags_wah_wah_util_compute_s24(AgsWahWahUtil *wah_wah_util);
void ags_wah_wah_util_compute_s32(AgsWahWahUtil *wah_wah_util);
void ags_wah_wah_util_compute_s64(AgsWahWahUtil *wah_wah_util);
void ags_wah_wah_util_compute_float(AgsWahWahUtil *wah_wah_util);
void ags_wah_wah_util_compute_double(AgsWahWahUtil *wah_wah_util);
void ags_wah_wah_util_compute_complex(AgsWahWahUtil *wah_wah_util);

void ags_wah_wah_util_compute(AgsWahWahUtil *wah_wah_util);

G_END_DECLS

#endif /*__AGS_WAH_WAH_UTIL_H__*/
