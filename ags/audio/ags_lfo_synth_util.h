/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_LFO_SYNTH_UTIL_H__
#define __AGS_LFO_SYNTH_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_LFO_SYNTH_UTIL         (ags_lfo_synth_util_get_type())

typedef struct _AgsLFOSynthUtil AgsLFOSynthUtil;

struct _AgsLFOSynthUtil
{
  gpointer source;
  guint source_stride;
  
  guint buffer_length;
  guint format;
  guint samplerate;

  guint lfo_synth_oscillator_mode;

  gdouble frequency;
  gdouble phase;

  gdouble lfo_depth;
  gdouble tuning;

  guint offset;
  guint frame_count;
};

GType ags_lfo_synth_util_get_type(void);

gpointer ags_lfo_synth_util_get_source(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_source(AgsLFOSynthUtil *lfo_synth_util,
				   gpointer source);

guint ags_lfo_synth_util_get_source_stride(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_source_stride(AgsLFOSynthUtil *lfo_synth_util,
					  guint source_stride);

guint ags_lfo_synth_util_get_buffer_length(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_buffer_length(AgsLFOSynthUtil *lfo_synth_util,
					  guint buffer_length);

guint ags_lfo_synth_util_get_format(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_format(AgsLFOSynthUtil *lfo_synth_util,
				   guint format);

guint ags_lfo_synth_util_get_samplerate(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_samplerate(AgsLFOSynthUtil *lfo_synth_util,
				       guint samplerate);

guint ags_lfo_synth_util_get_lfo_synth_oscillator_mode(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_lfo_synth_oscillator_mode(AgsLFOSynthUtil *lfo_synth_util,
						      guint lfo_synth_oscillator_mode);

gdouble ags_lfo_synth_util_get_frequency(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_frequency(AgsLFOSynthUtil *lfo_synth_util,
				      gdouble frequency);

gdouble ags_lfo_synth_util_get_phase(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_phase(AgsLFOSynthUtil *lfo_synth_util,
				  gdouble phase);

gdouble ags_lfo_synth_util_get_lfo_depth(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_lfo_depth(AgsLFOSynthUtil *lfo_synth_util,
				      gdouble lfo_depth);

gdouble ags_lfo_synth_util_get_tuning(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_tuning(AgsLFOSynthUtil *lfo_synth_util,
				   gdouble tuning);

guint ags_lfo_synth_util_get_offset(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_offset(AgsLFOSynthUtil *lfo_synth_util,
				   guint offset);

guint ags_lfo_synth_util_get_frame_count(AgsLFOSynthUtil *lfo_synth_util);
void ags_lfo_synth_util_set_frame_count(AgsLFOSynthUtil *lfo_synth_util,
					guint frame_count);

/* sin LFO */
void ags_lfo_synth_util_sin_s8(gint8 *buffer,
			       gdouble freq, gdouble phase,
			       gdouble depth,
			       gdouble tuning,
			       guint samplerate,
			       guint offset, guint n_frames);
void ags_lfo_synth_util_sin_s16(gint16 *buffer,
				gdouble freq, gdouble phase,
				gdouble depth,
				gdouble tuning,
				guint samplerate,
				guint offset, guint n_frames);
void ags_lfo_synth_util_sin_s24(gint32 *buffer,
				gdouble freq, gdouble phase,
				gdouble depth,
				gdouble tuning,
				guint samplerate,
				guint offset, guint n_frames);
void ags_lfo_synth_util_sin_s32(gint32 *buffer,
				gdouble freq, gdouble phase,
				gdouble depth,
				gdouble tuning,
				guint samplerate,
				guint offset, guint n_frames);
void ags_lfo_synth_util_sin_s64(gint64 *buffer,
				gdouble freq, gdouble phase,
				gdouble depth,
				gdouble tuning,
				guint samplerate,
				guint offset, guint n_frames);
void ags_lfo_synth_util_sin_float(gfloat *buffer,
				  gdouble freq, gdouble phase,
				  gdouble depth,
				  gdouble tuning,
				  guint samplerate,
				  guint offset, guint n_frames);
void ags_lfo_synth_util_sin_double(gdouble *buffer,
				   gdouble freq, gdouble phase,
				   gdouble depth,
				   gdouble tuning,
				   guint samplerate,
				   guint offset, guint n_frames);
void ags_lfo_synth_util_sin_complex(AgsComplex *buffer,
				    gdouble freq, gdouble phase,
				    gdouble depth,
				    gdouble tuning,
				    guint samplerate,
				    guint offset, guint n_frames);

/* sawtooth LFO */
void ags_lfo_synth_util_sawtooth_s8(gint8 *buffer,
				    gdouble freq, gdouble phase,
				    gdouble depth,
				    gdouble tuning,
				    guint samplerate,
				    guint offset, guint n_frames);
void ags_lfo_synth_util_sawtooth_s16(gint16 *buffer,
				     gdouble freq, gdouble phase,
				     gdouble depth,
				     gdouble tuning,
				     guint samplerate,
				     guint offset, guint n_frames);
void ags_lfo_synth_util_sawtooth_s24(gint32 *buffer,
				     gdouble freq, gdouble phase,
				     gdouble depth,
				     gdouble tuning,
				     guint samplerate,
				     guint offset, guint n_frames);
void ags_lfo_synth_util_sawtooth_s32(gint32 *buffer,
				     gdouble freq, gdouble phase,
				     gdouble depth,
				     gdouble tuning,
				     guint samplerate,
				     guint offset, guint n_frames);
void ags_lfo_synth_util_sawtooth_s64(gint64 *buffer,
				     gdouble freq, gdouble phase,
				     gdouble depth,
				     gdouble tuning,
				     guint samplerate,
				     guint offset, guint n_frames);
void ags_lfo_synth_util_sawtooth_float(gfloat *buffer,
				       gdouble freq, gdouble phase,
				       gdouble depth,
				       gdouble tuning,
				       guint samplerate,
				       guint offset, guint n_frames);
void ags_lfo_synth_util_sawtooth_double(gdouble *buffer,
					gdouble freq, gdouble phase,
					gdouble depth,
					gdouble tuning,
					guint samplerate,
					guint offset, guint n_frames);
void ags_lfo_synth_util_sawtooth_complex(AgsComplex *buffer,
					 gdouble freq, gdouble phase,
					 gdouble depth,
					 gdouble tuning,
					 guint samplerate,
					 guint offset, guint n_frames);

/* triangle LFO */
void ags_lfo_synth_util_triangle_s8(gint8 *buffer,
				    gdouble freq, gdouble phase,
				    gdouble depth,
				    gdouble tuning,
				    guint samplerate,
				    guint offset, guint n_frames);
void ags_lfo_synth_util_triangle_s16(gint16 *buffer,
				     gdouble freq, gdouble phase,
				     gdouble depth,
				     gdouble tuning,
				     guint samplerate,
				     guint offset, guint n_frames);
void ags_lfo_synth_util_triangle_s24(gint32 *buffer,
				     gdouble freq, gdouble phase,
				     gdouble depth,
				     gdouble tuning,
				     guint samplerate,
				     guint offset, guint n_frames);
void ags_lfo_synth_util_triangle_s32(gint32 *buffer,
				     gdouble freq, gdouble phase,
				     gdouble depth,
				     gdouble tuning,
				     guint samplerate,
				     guint offset, guint n_frames);
void ags_lfo_synth_util_triangle_s64(gint64 *buffer,
				     gdouble freq, gdouble phase,
				     gdouble depth,
				     gdouble tuning,
				     guint samplerate,
				     guint offset, guint n_frames);
void ags_lfo_synth_util_triangle_float(gfloat *buffer,
				       gdouble freq, gdouble phase,
				       gdouble depth,
				       gdouble tuning,
				       guint samplerate,
				       guint offset, guint n_frames);
void ags_lfo_synth_util_triangle_double(gdouble *buffer,
					gdouble freq, gdouble phase,
					gdouble depth,
					gdouble tuning,
					guint samplerate,
					guint offset, guint n_frames);
void ags_lfo_synth_util_triangle_complex(AgsComplex *buffer,
					 gdouble freq, gdouble phase,
					 gdouble depth,
					 gdouble tuning,
					 guint samplerate,
					 guint offset, guint n_frames);

/* square LFO */
void ags_lfo_synth_util_square_s8(gint8 *buffer,
				  gdouble freq, gdouble phase,
				  gdouble depth,
				  gdouble tuning,
				  guint samplerate,
				  guint offset, guint n_frames);
void ags_lfo_synth_util_square_s16(gint16 *buffer,
				   gdouble freq, gdouble phase,
				   gdouble depth,
				   gdouble tuning,
				   guint samplerate,
				   guint offset, guint n_frames);
void ags_lfo_synth_util_square_s24(gint32 *buffer,
				   gdouble freq, gdouble phase,
				   gdouble depth,
				   gdouble tuning,
				   guint samplerate,
				   guint offset, guint n_frames);
void ags_lfo_synth_util_square_s32(gint32 *buffer,
				   gdouble freq, gdouble phase,
				   gdouble depth,
				   gdouble tuning,
				   guint samplerate,
				   guint offset, guint n_frames);
void ags_lfo_synth_util_square_s64(gint64 *buffer,
				   gdouble freq, gdouble phase,
				   gdouble depth,
				   gdouble tuning,
				   guint samplerate,
				   guint offset, guint n_frames);
void ags_lfo_synth_util_square_float(gfloat *buffer,
				     gdouble freq, gdouble phase,
				     gdouble depth,
				     gdouble tuning,
				     guint samplerate,
				     guint offset, guint n_frames);
void ags_lfo_synth_util_square_double(gdouble *buffer,
				      gdouble freq, gdouble phase,
				      gdouble depth,
				      gdouble tuning,
				      guint samplerate,
				      guint offset, guint n_frames);
void ags_lfo_synth_util_square_complex(AgsComplex *buffer,
				       gdouble freq, gdouble phase,
				       gdouble depth,
				       gdouble tuning,
				       guint samplerate,
				       guint offset, guint n_frames);

/* impulse LFO */
void ags_lfo_synth_util_impulse_s8(gint8 *buffer,
				   gdouble freq, gdouble phase,
				   gdouble depth,
				   gdouble tuning,
				   guint samplerate,
				   guint offset, guint n_frames);
void ags_lfo_synth_util_impulse_s16(gint16 *buffer,
				    gdouble freq, gdouble phase,
				    gdouble depth,
				    gdouble tuning,
				    guint samplerate,
				    guint offset, guint n_frames);
void ags_lfo_synth_util_impulse_s24(gint32 *buffer,
				    gdouble freq, gdouble phase,
				    gdouble depth,
				    gdouble tuning,
				    guint samplerate,
				    guint offset, guint n_frames);
void ags_lfo_synth_util_impulse_s32(gint32 *buffer,
				    gdouble freq, gdouble phase,
				    gdouble depth,
				    gdouble tuning,
				    guint samplerate,
				    guint offset, guint n_frames);
void ags_lfo_synth_util_impulse_s64(gint64 *buffer,
				    gdouble freq, gdouble phase,
				    gdouble depth,
				    gdouble tuning,
				    guint samplerate,
				    guint offset, guint n_frames);
void ags_lfo_synth_util_impulse_float(gfloat *buffer,
				      gdouble freq, gdouble phase,
				      gdouble depth,
				      gdouble tuning,
				      guint samplerate,
				      guint offset, guint n_frames);
void ags_lfo_synth_util_impulse_double(gdouble *buffer,
				       gdouble freq, gdouble phase,
				       gdouble depth,
				       gdouble tuning,
				       guint samplerate,
				       guint offset, guint n_frames);
void ags_lfo_synth_util_impulse_complex(AgsComplex *buffer,
					gdouble freq, gdouble phase,
					gdouble depth,
					gdouble tuning,
					guint samplerate,
					guint offset, guint n_frames);

/* LFOs */
void ags_lfo_synth_util_sin(void *buffer,
			    gdouble freq, gdouble phase,
			    gdouble depth,
			    gdouble tuning,
			    guint samplerate, guint audio_buffer_util_format,
			    guint offset, guint n_frames);

void ags_lfo_synth_util_sawtooth(void *buffer,
				 gdouble freq, gdouble phase,
				 gdouble depth,
				 gdouble tuning,
				 guint samplerate, guint audio_buffer_util_format,
				 guint offset, guint n_frames);

void ags_lfo_synth_util_triangle(void *buffer,
				 gdouble freq, gdouble phase,
				 gdouble depth,
				 gdouble tuning,
				 guint samplerate, guint audio_buffer_util_format,
				 guint offset, guint n_frames);

void ags_lfo_synth_util_square(void *buffer,
			       gdouble freq, gdouble phase,
			       gdouble depth,
			       gdouble tuning,
			       guint samplerate, guint audio_buffer_util_format,
			       guint offset, guint n_frames);

void ags_lfo_synth_util_impulse(void *buffer,
				gdouble freq, gdouble phase,
				gdouble depth,
				gdouble tuning,
				guint samplerate, guint audio_buffer_util_format,
				guint offset, guint n_frames);

G_END_DECLS

#endif /*__AGS_LFO_SYNTH_UTIL_H__*/
