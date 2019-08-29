/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2019 Joël Krähemann
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

#include <glib.h>
#include <glib-object.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

int ags_fm_synth_util_test_init_suite();
int ags_fm_synth_util_test_clean_suite();

void ags_fm_synth_util_test_sin_s8();
void ags_fm_synth_util_test_sin_s16();
void ags_fm_synth_util_test_sin_s24();
void ags_fm_synth_util_test_sin_s32();
void ags_fm_synth_util_test_sin_s64();
void ags_fm_synth_util_test_sin_float();
void ags_fm_synth_util_test_sin_double();
void ags_fm_synth_util_test_sin_complex();

void ags_fm_synth_util_test_sawtooth_s8();
void ags_fm_synth_util_test_sawtooth_s16();
void ags_fm_synth_util_test_sawtooth_s24();
void ags_fm_synth_util_test_sawtooth_s32();
void ags_fm_synth_util_test_sawtooth_s64();
void ags_fm_synth_util_test_sawtooth_float();
void ags_fm_synth_util_test_sawtooth_double();
void ags_fm_synth_util_test_sawtooth_complex();

void ags_fm_synth_util_test_triangle_s8();
void ags_fm_synth_util_test_triangle_s16();
void ags_fm_synth_util_test_triangle_s24();
void ags_fm_synth_util_test_triangle_s32();
void ags_fm_synth_util_test_triangle_s64();
void ags_fm_synth_util_test_triangle_float();
void ags_fm_synth_util_test_triangle_double();
void ags_fm_synth_util_test_triangle_complex();

void ags_fm_synth_util_test_square_s8();
void ags_fm_synth_util_test_square_s16();
void ags_fm_synth_util_test_square_s24();
void ags_fm_synth_util_test_square_s32();
void ags_fm_synth_util_test_square_s64();
void ags_fm_synth_util_test_square_float();
void ags_fm_synth_util_test_square_double();
void ags_fm_synth_util_test_square_complex();

void ags_fm_synth_util_test_impulse_s8();
void ags_fm_synth_util_test_impulse_s16();
void ags_fm_synth_util_test_impulse_s24();
void ags_fm_synth_util_test_impulse_s32();
void ags_fm_synth_util_test_impulse_s64();
void ags_fm_synth_util_test_impulse_float();
void ags_fm_synth_util_test_impulse_double();
void ags_fm_synth_util_test_impulse_complex();

#define AGS_FM_SYNTH_UTIL_TEST_FREQ (440.0)
#define AGS_FM_SYNTH_UTIL_TEST_PHASE (0.0)
#define AGS_FM_SYNTH_UTIL_TEST_VOLUME (1.0)
#define AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE (44100)
#define AGS_FM_SYNTH_UTIL_TEST_OFFSET (0)
#define AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT (1024)
#define AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE (AGS_SYNTH_OSCILLATOR_SIN)
#define AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ (8.0)
#define AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH (0.5)
#define AGS_FM_SYNTH_UTIL_TEST_TUNING (0.0)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_fm_synth_util_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_fm_synth_util_test_clean_suite()
{
  return(0);
}

void
ags_fm_synth_util_test_sin_s8()
{
  gint8 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_8_BIT);

  ags_fm_synth_util_sin_s8(buffer,
			   AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
			   AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
			   AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			   AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
			   AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
			   AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S8,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_sin_s16()
{
  gint16 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_16_BIT);

  ags_fm_synth_util_sin_s16(buffer,
			    AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
			    AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
			    AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
			    AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
			    AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S16,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_sin_s24()
{
  gint32 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_24_BIT);

  ags_fm_synth_util_sin_s24(buffer,
			    AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
			    AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
			    AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
			    AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
			    AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S24,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_sin_s32()
{
  gint32 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_32_BIT);

  ags_fm_synth_util_sin_s32(buffer,
			    AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
			    AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
			    AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
			    AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
			    AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S32,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_sin_s64()
{
  gint64 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_64_BIT);

  ags_fm_synth_util_sin_s64(buffer,
			    AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
			    AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
			    AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
			    AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
			    AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S64,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_sin_float()
{
  gfloat *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_FLOAT);

  ags_fm_synth_util_sin_float(buffer,
			      AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
			      AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
			      AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
			      AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
			      AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_FLOAT,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_sin_double()
{
  gdouble *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_DOUBLE);

  ags_fm_synth_util_sin_double(buffer,
			       AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
			       AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
			       AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			       AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
			       AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
			       AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_sin_complex()
{
  AgsComplex *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_COMPLEX);

  ags_fm_synth_util_sin_complex(buffer,
				AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_COMPLEX,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_sawtooth_s8()
{
  gint8 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_8_BIT);

  ags_fm_synth_util_sawtooth_s8(buffer,
				AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S8,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_sawtooth_s16()
{
  gint16 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_16_BIT);

  ags_fm_synth_util_sawtooth_s16(buffer,
				 AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				 AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				 AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				 AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				 AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				 AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S16,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_sawtooth_s24()
{
  gint32 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_24_BIT);

  ags_fm_synth_util_sawtooth_s24(buffer,
				 AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				 AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				 AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				 AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				 AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				 AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S24,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_sawtooth_s32()
{
  gint32 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_32_BIT);

  ags_fm_synth_util_sawtooth_s32(buffer,
				 AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				 AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				 AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				 AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				 AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				 AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S32,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_sawtooth_s64()
{
  gint64 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_64_BIT);

  ags_fm_synth_util_sawtooth_s64(buffer,
				 AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				 AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				 AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				 AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				 AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				 AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S64,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_sawtooth_float()
{
  gfloat *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_FLOAT);

  ags_fm_synth_util_sawtooth_float(buffer,
				   AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				   AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				   AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				   AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				   AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				   AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_FLOAT,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_sawtooth_double()
{
  gdouble *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_DOUBLE);

  ags_fm_synth_util_sawtooth_double(buffer,
				    AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				    AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				    AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				    AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				    AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				    AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_sawtooth_complex()
{
  AgsComplex *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_COMPLEX);

  ags_fm_synth_util_sawtooth_complex(buffer,
				     AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				     AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				     AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				     AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				     AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				     AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_COMPLEX,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_triangle_s8()
{
  gint8 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_8_BIT);

  ags_fm_synth_util_triangle_s8(buffer,
				AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S8,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_triangle_s16()
{
  gint16 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_16_BIT);

  ags_fm_synth_util_triangle_s16(buffer,
				 AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				 AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				 AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				 AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				 AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				 AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S16,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_triangle_s24()
{
  gint32 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_24_BIT);

  ags_fm_synth_util_triangle_s24(buffer,
				 AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				 AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				 AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				 AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				 AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				 AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S24,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_triangle_s32()
{
  gint32 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_32_BIT);

  ags_fm_synth_util_triangle_s32(buffer,
				 AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				 AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				 AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				 AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				 AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				 AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S32,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_triangle_s64()
{
  gint64 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_64_BIT);

  ags_fm_synth_util_triangle_s64(buffer,
				 AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				 AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				 AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				 AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				 AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				 AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S64,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_triangle_float()
{
  gfloat *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_FLOAT);

  ags_fm_synth_util_triangle_float(buffer,
				   AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				   AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				   AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				   AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				   AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				   AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_FLOAT,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_triangle_double()
{
  gdouble *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_DOUBLE);

  ags_fm_synth_util_triangle_double(buffer,
				    AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				    AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				    AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				    AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				    AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				    AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_triangle_complex()
{
  AgsComplex *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_COMPLEX);

  ags_fm_synth_util_triangle_complex(buffer,
				     AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				     AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				     AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				     AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				     AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				     AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_COMPLEX,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_square_s8()
{
  gint8 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_8_BIT);

  ags_fm_synth_util_square_s8(buffer,
			      AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
			      AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
			      AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
			      AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
			      AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S8,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_square_s16()
{
  gint16 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_16_BIT);

  ags_fm_synth_util_square_s16(buffer,
			       AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
			       AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
			       AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			       AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
			       AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
			       AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S16,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_square_s24()
{
  gint32 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_24_BIT);

  ags_fm_synth_util_square_s24(buffer,
			       AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
			       AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
			       AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			       AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
			       AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
			       AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S24,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_square_s32()
{
  gint32 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_32_BIT);

  ags_fm_synth_util_square_s32(buffer,
			       AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
			       AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
			       AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			       AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
			       AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
			       AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S32,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_square_s64()
{
  gint64 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_64_BIT);

  ags_fm_synth_util_square_s64(buffer,
			       AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
			       AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
			       AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			       AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
			       AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
			       AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S64,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_square_float()
{
  gfloat *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_FLOAT);

  ags_fm_synth_util_square_float(buffer,
				 AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				 AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				 AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				 AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				 AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				 AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_FLOAT,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_square_double()
{
  gdouble *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_DOUBLE);

  ags_fm_synth_util_square_double(buffer,
				  AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				  AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				  AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				  AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				  AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				  AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_square_complex()
{
  AgsComplex *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_COMPLEX);

  ags_fm_synth_util_square_complex(buffer,
				   AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				   AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				   AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				   AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				   AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				   AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_COMPLEX,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_impulse_s8()
{
  gint8 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_8_BIT);

  ags_fm_synth_util_impulse_s8(buffer,
			       AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
			       AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
			       AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			       AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
			       AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
			       AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S8,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_impulse_s16()
{
  gint16 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_16_BIT);

  ags_fm_synth_util_impulse_s16(buffer,
				AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S16,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_impulse_s24()
{
  gint32 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_24_BIT);

  ags_fm_synth_util_impulse_s24(buffer,
				AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S24,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_impulse_s32()
{
  gint32 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_32_BIT);

  ags_fm_synth_util_impulse_s32(buffer,
				AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S32,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_impulse_s64()
{
  gint64 *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_SIGNED_64_BIT);

  ags_fm_synth_util_impulse_s64(buffer,
				AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S64,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_impulse_float()
{
  gfloat *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_FLOAT);

  ags_fm_synth_util_impulse_float(buffer,
				  AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				  AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				  AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				  AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				  AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				  AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_FLOAT,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_impulse_double()
{
  gdouble *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_DOUBLE);

  ags_fm_synth_util_impulse_double(buffer,
				   AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				   AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				   AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				   AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				   AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				   AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_impulse_complex()
{
  AgsComplex *buffer;

  guint xcross_count;

  buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			    AGS_SOUNDCARD_COMPLEX);

  ags_fm_synth_util_impulse_complex(buffer,
				    AGS_FM_SYNTH_UTIL_TEST_FREQ, AGS_FM_SYNTH_UTIL_TEST_PHASE, AGS_FM_SYNTH_UTIL_TEST_VOLUME,
				    AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE,
				    AGS_FM_SYNTH_UTIL_TEST_OFFSET, AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
				    AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE,
				    AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ, AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH,
				    AGS_FM_SYNTH_UTIL_TEST_TUNING);

  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_COMPLEX,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

  putenv("LC_ALL=C");
  putenv("LANG=C");
  
  /* initialize the CUnit test registry */
  if(CUE_SUCCESS != CU_initialize_registry()){
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("AgsFMSynthUtilTest", ags_fm_synth_util_test_init_suite, ags_fm_synth_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of ags_fm_synth_util.c sin signed 8 bit", ags_fm_synth_util_test_sin_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c sin signed 16 bit", ags_fm_synth_util_test_sin_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c sin signed 24 bit", ags_fm_synth_util_test_sin_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c sin signed 32 bit", ags_fm_synth_util_test_sin_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c sin signed 64 bit", ags_fm_synth_util_test_sin_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c sin float", ags_fm_synth_util_test_sin_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c sin double", ags_fm_synth_util_test_sin_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c sin complex", ags_fm_synth_util_test_sin_complex) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c sawtooth signed 8 bit", ags_fm_synth_util_test_sawtooth_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c sawtooth signed 16 bit", ags_fm_synth_util_test_sawtooth_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c sawtooth signed 24 bit", ags_fm_synth_util_test_sawtooth_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c sawtooth signed 32 bit", ags_fm_synth_util_test_sawtooth_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c sawtooth signed 64 bit", ags_fm_synth_util_test_sawtooth_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c sawtooth float", ags_fm_synth_util_test_sawtooth_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c sawtooth double", ags_fm_synth_util_test_sawtooth_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c sawtooth complex", ags_fm_synth_util_test_sawtooth_complex) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c triangle signed 8 bit", ags_fm_synth_util_test_triangle_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c triangle signed 16 bit", ags_fm_synth_util_test_triangle_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c triangle signed 24 bit", ags_fm_synth_util_test_triangle_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c triangle signed 32 bit", ags_fm_synth_util_test_triangle_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c triangle signed 64 bit", ags_fm_synth_util_test_triangle_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c triangle float", ags_fm_synth_util_test_triangle_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c triangle double", ags_fm_synth_util_test_triangle_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c triangle complex", ags_fm_synth_util_test_triangle_complex) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c square signed 8 bit", ags_fm_synth_util_test_square_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c square signed 16 bit", ags_fm_synth_util_test_square_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c square signed 24 bit", ags_fm_synth_util_test_square_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c square signed 32 bit", ags_fm_synth_util_test_square_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c square signed 64 bit", ags_fm_synth_util_test_square_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c square float", ags_fm_synth_util_test_square_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c square double", ags_fm_synth_util_test_square_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c square complex", ags_fm_synth_util_test_square_complex) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c impulse signed 8 bit", ags_fm_synth_util_test_impulse_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c impulse signed 16 bit", ags_fm_synth_util_test_impulse_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c impulse signed 24 bit", ags_fm_synth_util_test_impulse_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c impulse signed 32 bit", ags_fm_synth_util_test_impulse_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c impulse signed 64 bit", ags_fm_synth_util_test_impulse_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c impulse float", ags_fm_synth_util_test_impulse_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c impulse double", ags_fm_synth_util_test_impulse_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c impulse complex", ags_fm_synth_util_test_impulse_complex) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
