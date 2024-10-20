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

#ifndef __AGS_SFZ_SYNTH_UTIL_H__
#define __AGS_SFZ_SYNTH_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_resample_util.h>
#include <ags/audio/ags_common_pitch_util.h>
#include <ags/audio/ags_volume_util.h>

#include <ags/audio/file/ags_audio_container.h>
#include <ags/audio/file/ags_sfz_file.h>
#include <ags/audio/file/ags_sfz_sample.h>

G_BEGIN_DECLS

#define AGS_TYPE_SFZ_SYNTH_UTIL         (ags_sfz_synth_util_get_type())
#define AGS_SFZ_SYNTH_UTIL(ptr) ((AgsSFZSynthUtil *)(ptr))

#define AGS_SFZ_SYNTH_UTIL_INITIALIZER ((AgsSFZSynthUtil) {		\
      .flags = 0,							\
      .sfz_file = NULL,							\
      .sfz_sample_count = 0,						\
      .sfz_sample_arr = (AgsSFZSample **) g_malloc(128 * sizeof(AgsSFZSample*)), \
      .sfz_note_range = (gint **) g_malloc(128 * sizeof(gint*)),	\
      .sfz_orig_buffer = (gpointer *) g_malloc(128 * sizeof(gpointer)), \
      .sfz_resampled_buffer = (gpointer *) g_malloc(128 * sizeof(gpointer)), \
      .source = NULL,							\
      .source_stride = 1,						\
      .sample_buffer = NULL,						\
      .im_buffer = NULL,						\
      .buffer_length = 0,						\
      .format = AGS_SOUNDCARD_DEFAULT_FORMAT,				\
      .samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE,			\
      .midi_key = -1,							\
      .note = 0.0,							\
      .volume = 1.0,							\
      .frame_count = 0,							\
      .offset = 0,							\
      .loop_mode = AGS_SFZ_SYNTH_UTIL_LOOP_NONE,			\
      .loop_start = 0,							\
      .loop_end = 0,							\
      .audio_buffer_util = ags_audio_buffer_util_alloc(),		\
      .resample_util = ags_resample_util_alloc(),			\
      .pitch_type = AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL,		\
      .pitch_util = ags_fluid_interpolate_4th_order_util_alloc(),	\
      .volume_util = ags_volume_util_alloc(),				\
      .note_256th_mode = FALSE,						\
      .offset_256th = 0 })

/**
 * AgsSFZSynthUtilLoopMode:
 * @AGS_SFZ_SYNTH_UTIL_LOOP_NONE: loop none
 * @AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD: loop standard
 * @AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE: loop release
 * @AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG: loop pingpong
 * 
 * Enum values to specify loop mode.
 */
typedef enum{
  AGS_SFZ_SYNTH_UTIL_LOOP_NONE,
  AGS_SFZ_SYNTH_UTIL_LOOP_STANDARD,
  AGS_SFZ_SYNTH_UTIL_LOOP_RELEASE,
  AGS_SFZ_SYNTH_UTIL_LOOP_PINGPONG,
}AgsSFZSynthUtilLoopMode;

typedef struct _AgsSFZSynthUtil AgsSFZSynthUtil;

struct _AgsSFZSynthUtil
{
  guint flags;
  
  AgsAudioContainer *sfz_file;

  guint sfz_sample_count;
  AgsSFZSample **sfz_sample_arr;
  gint **sfz_note_range;

  gint sfz_loop_mode[128];
  gint sfz_loop_start[128];
  gint sfz_loop_end[128];

  guint sfz_orig_buffer_length[128];
  gpointer *sfz_orig_buffer;
  
  guint sfz_resampled_buffer_length[128];
  gpointer *sfz_resampled_buffer;  

  gpointer source;
  guint source_stride;

  gpointer sample_buffer;
  gpointer im_buffer;

  guint buffer_length;
  AgsSoundcardFormat format;
  guint samplerate;
  guint orig_samplerate;
  
  gint midi_key;
  
  gdouble note;
  gdouble volume;

  guint frame_count;
  guint offset;

  guint loop_mode;

  guint loop_start;
  guint loop_end;
  
  AgsAudioBufferUtil *audio_buffer_util;

  AgsResampleUtil *resample_util;

  GType pitch_type;  
  gpointer pitch_util;

  AgsVolumeUtil *volume_util;

  gboolean note_256th_mode;

  guint offset_256th;
};

GType ags_sfz_synth_util_get_type(void);

GType ags_sfz_synth_util_loop_mode_get_type();

AgsSFZSynthUtil* ags_sfz_synth_util_alloc();

gpointer ags_sfz_synth_util_copy(AgsSFZSynthUtil *ptr);
void ags_sfz_synth_util_free(AgsSFZSynthUtil *ptr);

AgsAudioContainer* ags_sfz_synth_util_get_sfz_file(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_sfz_file(AgsSFZSynthUtil *sfz_synth_util,
				     AgsAudioContainer *sfz_file);

gpointer ags_sfz_synth_util_get_source(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_source(AgsSFZSynthUtil *sfz_synth_util,
				   gpointer source);

guint ags_sfz_synth_util_get_source_stride(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_source_stride(AgsSFZSynthUtil *sfz_synth_util,
					  guint source_stride);

guint ags_sfz_synth_util_get_buffer_length(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_buffer_length(AgsSFZSynthUtil *sfz_synth_util,
					  guint buffer_length);

AgsSoundcardFormat ags_sfz_synth_util_get_format(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_format(AgsSFZSynthUtil *sfz_synth_util,
				   AgsSoundcardFormat format);

guint ags_sfz_synth_util_get_samplerate(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_samplerate(AgsSFZSynthUtil *sfz_synth_util,
				       guint samplerate);

gint ags_sfz_synth_util_get_midi_key(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_midi_key(AgsSFZSynthUtil *sfz_synth_util,
				     gint midi_key);

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

guint ags_sfz_synth_util_get_loop_start(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_loop_start(AgsSFZSynthUtil *sfz_synth_util,
				       guint loop_start);

guint ags_sfz_synth_util_get_loop_end(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_loop_end(AgsSFZSynthUtil *sfz_synth_util,
				     guint loop_end);

GType ags_sfz_synth_util_get_pitch_type(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_pitch_type(AgsSFZSynthUtil *sfz_synth_util,
				       GType pitch_type);

gpointer ags_sfz_synth_util_get_pitch_util(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_pitch_util(AgsSFZSynthUtil *sfz_synth_util,
				       gpointer pitch_util);

gboolean ags_sfz_synth_util_get_note_256th_mode(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_note_256th_mode(AgsSFZSynthUtil *sfz_synth_util,
					    gboolean note_256th_mode);

guint ags_sfz_synth_util_get_offset_256th(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_set_offset_256th(AgsSFZSynthUtil *sfz_synth_util,
					 guint offset_256th);

void ags_sfz_synth_util_load_instrument(AgsSFZSynthUtil *sfz_synth_util);

void ags_sfz_synth_util_compute_s8(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_compute_s16(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_compute_s24(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_compute_s32(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_compute_s64(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_compute_float(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_compute_double(AgsSFZSynthUtil *sfz_synth_util);
void ags_sfz_synth_util_compute_complex(AgsSFZSynthUtil *sfz_synth_util);

void ags_sfz_synth_util_compute(AgsSFZSynthUtil *sfz_synth_util);

G_END_DECLS

#endif /*__AGS_SFZ_SYNTH_UTIL_H__*/
