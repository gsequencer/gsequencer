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

#ifndef __AGS_SFZ_SYNTH_UTIL_H__
#define __AGS_SFZ_SYNTH_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/file/ags_sfz_sample.h>

G_BEGIN_DECLS

#define AGS_TYPE_SFZ_SYNTH_UTIL         (ags_sfz_synth_util_get_type())

typedef enum{
  AGS_SFZ_SYNTH_UTIL_LOOP_NONE,
  AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD,
  AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE,
  AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG,
}AgsSFZSynthUtilLoopMode;

typedef struct _AgsSFZSynthUtil AgsSFZSynthUtil;

struct _AgsSFZSynthUtil
{
  //empty
};

GType ags_sfz_synth_util_get_type(void);

void ags_sfz_synth_util_copy_s8(gint8 *buffer,
				guint buffer_size,
				AgsSFZSample *sfz_sample,
				gdouble note,
				gdouble volume,
				guint samplerate,
				guint offset, guint n_frames,
				guint loop_mode,
				gint loop_start, gint loop_end);
void ags_sfz_synth_util_copy_s16(gint16 *buffer,
				 guint buffer_size,
				 AgsSFZSample *sfz_sample,
				 gdouble note,
				 gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames,
				 guint loop_mode,
				 gint loop_start, gint loop_end);
void ags_sfz_synth_util_copy_s24(gint32 *buffer,
				 guint buffer_size,
				 AgsSFZSample *sfz_sample,
				 gdouble note,
				 gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames,
				 guint loop_mode,
				 gint loop_start, gint loop_end);
void ags_sfz_synth_util_copy_s32(gint32 *buffer,
				 guint buffer_size,
				 AgsSFZSample *sfz_sample,
				 gdouble note,
				 gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames,
				 guint loop_mode,
				 gint loop_start, gint loop_end);
void ags_sfz_synth_util_copy_s64(gint64 *buffer,
				 guint buffer_size,
				 AgsSFZSample *sfz_sample,
				 gdouble note,
				 gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames,
				 guint loop_mode,
				 gint loop_start, gint loop_end);
void ags_sfz_synth_util_copy_float(gfloat *buffer,
				   guint buffer_size,
				   AgsSFZSample *sfz_sample,
				   gdouble note,
				   gdouble volume,
				   guint samplerate,
				   guint offset, guint n_frames,
				   guint loop_mode,
				   gint loop_start, gint loop_end);
void ags_sfz_synth_util_copy_double(gdouble *buffer,
				    guint buffer_size,
				    AgsSFZSample *sfz_sample,
				    gdouble note,
				    gdouble volume,
				    guint samplerate,
				    guint offset, guint n_frames,
				    guint loop_mode,
				    gint loop_start, gint loop_end);
void ags_sfz_synth_util_copy_complex(AgsComplex *buffer,
				     guint buffer_size,
				     AgsSFZSample *sfz_sample,
				     gdouble note,
				     gdouble volume,
				     guint samplerate,
				     guint offset, guint n_frames,
				     guint loop_mode,
				     gint loop_start, gint loop_end);

void ags_sfz_synth_util_copy(void *buffer,
			     guint buffer_size,
			     AgsSFZSample *sfz_sample,
			     gdouble note,
			     gdouble volume,
			     guint samplerate, guint audio_buffer_util_format,
			     guint offset, guint n_frames,
			     guint loop_mode,
			     gint loop_start, gint loop_end);

G_END_DECLS

#endif /*__AGS_SFZ_SYNTH_UTIL_H__*/
