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
#define AGS_SFZ_SYNTH_UTIL(ptr) ((AgsSFZSynthUtil *)(ptr))

typedef enum{
  AGS_SFZ_SYNTH_UTIL_LOOP_NONE,
  AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD,
  AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE,
  AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG,
}AgsSFZSynthUtilLoopMode;

typedef struct _AgsSFZSynthUtil AgsSFZSynthUtil;

struct _AgsSFZSynthUtil
{
  AgsSFZSample *sfz_sample;
  
  gpointer source;
  guint source_stride;

  guint buffer_length;
  guint audio_buffer_util_format;
  guint samplerate;

  gdouble note;

  gdouble volume;

  guint frame_count;
  guint offset;

  guint loop_mode;

  gint loop_start;
  gint loop_end;
  
  gpointer generic_pitch;
};

GType ags_sfz_synth_util_get_type(void);

AgsSFZSynthUtil* ags_sfz_synth_util_alloc();

gpointer ags_sfz_synth_util_copy(AgsSFZSynthUtil *ptr);
void ags_sfz_synth_util_free(AgsSFZSynthUtil *ptr);

gpointer ags_sfz_synth_util_get_source(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_source(AgsSFZSynthUtil *sfz_synth_util,
				   gpointer source);

guint ags_sfz_synth_util_get_source_stride(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_source_stride(AgsSFZSynthUtil *sfz_synth_util,
					  guint source_stride);

guint ags_sfz_synth_util_get_buffer_length(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_buffer_length(AgsSFZSynthUtil *sfz_synth_util,
					  guint buffer_length);

guint ags_sfz_synth_util_get_audio_buffer_util_format(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_audio_buffer_util_format(AgsSFZSynthUtil *sfz_synth_util,
						     guint audio_buffer_util_format);

guint ags_sfz_synth_util_get_samplerate(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_samplerate(AgsSFZSynthUtil *sfz_synth_util,
				       guint samplerate);

gdouble ags_sfz_synth_util_get_note(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_note(AgsSFZSynthUtil *sfz_synth_util,
				    gdouble note);

gdouble ags_sfz_synth_util_get_volume(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_volume(AgsSFZSynthUtil *sfz_synth_util,
				   gdouble volume);

guint ags_sfz_synth_util_get_frame_count(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_frame_count(AgsSFZSynthUtil *sfz_synth_util,
				       guint frame_count);

guint ags_sfz_synth_util_get_offset(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_offset(AgsSFZSynthUtil *sfz_synth_util,
				       guint offset);

guint ags_sfz_synth_util_get_loop_mode(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_loop_mode(AgsSFZSynthUtil *sfz_synth_util,
				       guint loop_mode);

gint ags_sfz_synth_util_get_loop_start(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_loop_start(AgsSFZSynthUtil *sfz_synth_util,
				       gint loop_start);

gint ags_sfz_synth_util_get_loop_end(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_loop_end(AgsSFZSynthUtil *sfz_synth_util,
				     gint loop_end);

gpointer ags_sfz_synth_util_get_generic_pitch(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_generic_pitch(AgsSFZSynthUtil *sfz_synth_util,
					  gpointer generic_pitch);

void ags_sfz_synth_util_compute_s8(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_compute_s16(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_compute_s24(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_compute_s32(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_compute_s64(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_compute_float(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_compute_double(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_compute_complex(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_compute(AgsSFZSynthUtil *sfz_synth_util);

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
