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

#ifndef __AGS_SF2_SYNTH_UTIL_H__
#define __AGS_SF2_SYNTH_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/file/ags_ipatch.h>
#include <ags/audio/file/ags_ipatch_sample.h>

G_BEGIN_DECLS

#define AGS_TYPE_SF2_SYNTH_UTIL         (ags_sf2_synth_util_get_type())
#define AGS_SF2_SYNTH_UTIL(ptr) ((AgsSF2SynthUtil *)(ptr))

typedef enum{
  AGS_SF2_SYNTH_UTIL_LOOP_NONE,
  AGS_SF2_SYNTH_UTIL_LOOP_STANDARD,
  AGS_SF2_SYNTH_UTIL_LOOP_RELEASE,
  AGS_SF2_SYNTH_UTIL_LOOP_PINGPONG,
}AgsSF2SynthUtilLoopMode;

typedef struct _AgsSF2SynthUtil AgsSF2SynthUtil;

struct _AgsSF2SynthUtil
{
  AgsIpatchSample *ipatch_sample;
  
  gpointer source;
  guint source_stride;

  guint buffer_length;
  guint audio_buffer_util_format;
  guint samplerate;
  
  gchar *preset;
  gchar *instrument;
  gchar *sample;
  
  gint bank;
  gint program;
  
  gint midi_key;
  
  gdouble note;

  gdouble volume;

  guint frame_count;
  guint offset;

  guint loop_mode;

  gint loop_start;
  gint loop_end;
  
  gpointer generic_pitch_util;
};

GType ags_sf2_synth_util_get_type(void);

AgsSF2SynthUtil* ags_sf2_synth_util_alloc();

gpointer ags_sf2_synth_util_copy(AgsSF2SynthUtil *ptr);
void ags_sf2_synth_util_free(AgsSF2SynthUtil *ptr);

gpointer ags_sf2_synth_util_get_source(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_set_source(AgsSF2SynthUtil *sf2_synth_util,
				   gpointer source);

guint ags_sf2_synth_util_get_source_stride(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_set_source_stride(AgsSF2SynthUtil *sf2_synth_util,
					  guint source_stride);

guint ags_sf2_synth_util_get_buffer_length(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_set_buffer_length(AgsSF2SynthUtil *sf2_synth_util,
					  guint buffer_length);

guint ags_sf2_synth_util_get_audio_buffer_util_format(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_set_audio_buffer_util_format(AgsSF2SynthUtil *sf2_synth_util,
						     guint audio_buffer_util_format);

guint ags_sf2_synth_util_get_samplerate(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_set_samplerate(AgsSF2SynthUtil *sf2_synth_util,
				       guint samplerate);

gchar* ags_sf2_synth_util_get_preset(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_set_preset(AgsSF2SynthUtil *sf2_synth_util,
				   gchar *preset);

gchar* ags_sf2_synth_util_get_instrument(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_set_instrument(AgsSF2SynthUtil *sf2_synth_util,
				       gchar *instrument);

gchar* ags_sf2_synth_util_get_sample(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_set_sample(AgsSF2SynthUtil *sf2_synth_util,
				   gchar *sample);

gint ags_sf2_synth_util_get_bank(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_set_bank(AgsSF2SynthUtil *sf2_synth_util,
				 gint bank);

gint ags_sf2_synth_util_get_program(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_set_program(AgsSF2SynthUtil *sf2_synth_util,
				    gint program);

gint ags_sf2_synth_util_get_midi_key(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_set_midi_key(AgsSF2SynthUtil *sf2_synth_util,
				     gint midi_key);

gdouble ags_sf2_synth_util_get_note(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_set_note(AgsSF2SynthUtil *sf2_synth_util,
				    gdouble note);

gdouble ags_sf2_synth_util_get_volume(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_set_volume(AgsSF2SynthUtil *sf2_synth_util,
				   gdouble volume);

guint ags_sf2_synth_util_get_frame_count(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_set_frame_count(AgsSF2SynthUtil *sf2_synth_util,
				       guint frame_count);

guint ags_sf2_synth_util_get_offset(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_set_offset(AgsSF2SynthUtil *sf2_synth_util,
				       guint offset);

guint ags_sf2_synth_util_get_loop_mode(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_set_loop_mode(AgsSF2SynthUtil *sf2_synth_util,
				       guint loop_mode);

gint ags_sf2_synth_util_get_loop_start(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_set_loop_start(AgsSF2SynthUtil *sf2_synth_util,
				       gint loop_start);

gint ags_sf2_synth_util_get_loop_end(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_set_loop_end(AgsSF2SynthUtil *sf2_synth_util,
				     gint loop_end);

gpointer ags_sf2_synth_util_get_generic_pitch_util(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_set_generic_pitch_util(AgsSF2SynthUtil *sf2_synth_util,
					       gpointer generic_pitch_util);

void ags_sf2_synth_util_compute_s8(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_compute_s16(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_compute_s24(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_compute_s32(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_compute_s64(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_compute_float(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_compute_double(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_compute_complex(AgsSF2SynthUtil *sf2_synth_util);
void ags_sf2_synth_util_compute(AgsSF2SynthUtil *sf2_synth_util);

AgsIpatchSample* ags_sf2_synth_util_midi_locale_find_sample_near_midi_key(AgsIpatch *ipatch,
									  gint bank,
									  gint program,
									  gint midi_key,
									  gchar **preset,
									  gchar **instrument,
									  gchar **sample);

void ags_sf2_synth_util_copy_s8(gint8 *buffer,
				guint buffer_size,
				AgsIpatchSample *ipatch_sample,
				gdouble note,
				gdouble volume,
				guint samplerate,
				guint offset, guint n_frames,
				guint loop_mode,
				gint loop_start, gint loop_end);
void ags_sf2_synth_util_copy_s16(gint16 *buffer,
				 guint buffer_size,
				 AgsIpatchSample *ipatch_sample,
				 gdouble note,
				 gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames,
				 guint loop_mode,
				 gint loop_start, gint loop_end);
void ags_sf2_synth_util_copy_s24(gint32 *buffer,
				 guint buffer_size,
				 AgsIpatchSample *ipatch_sample,
				 gdouble note,
				 gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames,
				 guint loop_mode,
				 gint loop_start, gint loop_end);
void ags_sf2_synth_util_copy_s32(gint32 *buffer,
				 guint buffer_size,
				 AgsIpatchSample *ipatch_sample,
				 gdouble note,
				 gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames,
				 guint loop_mode,
				 gint loop_start, gint loop_end);
void ags_sf2_synth_util_copy_s64(gint64 *buffer,
				 guint buffer_size,
				 AgsIpatchSample *ipatch_sample,
				 gdouble note,
				 gdouble volume,
				 guint samplerate,
				 guint offset, guint n_frames,
				 guint loop_mode,
				 gint loop_start, gint loop_end);
void ags_sf2_synth_util_copy_float(gfloat *buffer,
				   guint buffer_size,
				   AgsIpatchSample *ipatch_sample,
				   gdouble note,
				   gdouble volume,
				   guint samplerate,
				   guint offset, guint n_frames,
				   guint loop_mode,
				   gint loop_start, gint loop_end);
void ags_sf2_synth_util_copy_double(gdouble *buffer,
				    guint buffer_size,
				    AgsIpatchSample *ipatch_sample,
				    gdouble note,
				    gdouble volume,
				    guint samplerate,
				    guint offset, guint n_frames,
				    guint loop_mode,
				    gint loop_start, gint loop_end);
void ags_sf2_synth_util_copy_complex(AgsComplex *buffer,
				     guint buffer_size,
				     AgsIpatchSample *ipatch_sample,
				     gdouble note,
				     gdouble volume,
				     guint samplerate,
				     guint offset, guint n_frames,
				     guint loop_mode,
				     gint loop_start, gint loop_end);

void ags_sf2_synth_util_copy(void *buffer,
			     guint buffer_size,
			     AgsIpatchSample *ipatch_sample,
			     gdouble note,
			     gdouble volume,
			     guint samplerate, guint audio_buffer_util_format,
			     guint offset, guint n_frames,
			     guint loop_mode,
			     gint loop_start, gint loop_end);

G_END_DECLS

#endif /*__AGS_SF2_SYNTH_UTIL_H__*/
