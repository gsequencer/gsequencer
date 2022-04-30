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

#include <glib.h>
#include <glib-object.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

int ags_fm_synth_util_test_init_suite();
int ags_fm_synth_util_test_clean_suite();

void ags_fm_synth_util_test_alloc();
void ags_fm_synth_util_test_copy();
void ags_fm_synth_util_test_free();
void ags_fm_synth_util_test_get_source();
void ags_fm_synth_util_test_set_source();
void ags_fm_synth_util_test_get_source_stride();
void ags_fm_synth_util_test_set_source_stride();
void ags_fm_synth_util_test_get_buffer_length();
void ags_fm_synth_util_test_set_buffer_length();
void ags_fm_synth_util_test_get_samplerate();
void ags_fm_synth_util_test_set_samplerate();
void ags_fm_synth_util_test_get_synth_oscillator_mode();
void ags_fm_synth_util_test_set_synth_oscillator_mode();
void ags_fm_synth_util_test_get_frequency();
void ags_fm_synth_util_test_set_frequency();
void ags_fm_synth_util_test_get_phase();
void ags_fm_synth_util_test_set_phase();
void ags_fm_synth_util_test_get_lfo_oscillator_mode();
void ags_fm_synth_util_test_set_lfo_oscillator_mode();
void ags_fm_synth_util_test_get_lfo_frequency();
void ags_fm_synth_util_test_set_lfo_frequency();
void ags_fm_synth_util_test_get_lfo_depth();
void ags_fm_synth_util_test_set_lfo_depth();
void ags_fm_synth_util_test_get_tuning();
void ags_fm_synth_util_test_set_tuning();
void ags_fm_synth_util_test_get_volume();
void ags_fm_synth_util_test_set_volume();
void ags_fm_synth_util_test_get_frame_count();
void ags_fm_synth_util_test_set_frame_count();
void ags_fm_synth_util_test_get_offset();
void ags_fm_synth_util_test_set_offset();

void ags_fm_synth_util_test_compute_sin_s8();
void ags_fm_synth_util_test_compute_sin_s16();
void ags_fm_synth_util_test_compute_sin_s24();
void ags_fm_synth_util_test_compute_sin_s32();
void ags_fm_synth_util_test_compute_sin_s64();
void ags_fm_synth_util_test_compute_sin_float();
void ags_fm_synth_util_test_compute_sin_double();
void ags_fm_synth_util_test_compute_sin_complex();

void ags_fm_synth_util_test_compute_sawtooth_s8();
void ags_fm_synth_util_test_compute_sawtooth_s16();
void ags_fm_synth_util_test_compute_sawtooth_s24();
void ags_fm_synth_util_test_compute_sawtooth_s32();
void ags_fm_synth_util_test_compute_sawtooth_s64();
void ags_fm_synth_util_test_compute_sawtooth_float();
void ags_fm_synth_util_test_compute_sawtooth_double();
void ags_fm_synth_util_test_compute_sawtooth_complex();

void ags_fm_synth_util_test_compute_triangle_s8();
void ags_fm_synth_util_test_compute_triangle_s16();
void ags_fm_synth_util_test_compute_triangle_s24();
void ags_fm_synth_util_test_compute_triangle_s32();
void ags_fm_synth_util_test_compute_triangle_s64();
void ags_fm_synth_util_test_compute_triangle_float();
void ags_fm_synth_util_test_compute_triangle_double();
void ags_fm_synth_util_test_compute_triangle_complex();

void ags_fm_synth_util_test_compute_square_s8();
void ags_fm_synth_util_test_compute_square_s16();
void ags_fm_synth_util_test_compute_square_s24();
void ags_fm_synth_util_test_compute_square_s32();
void ags_fm_synth_util_test_compute_square_s64();
void ags_fm_synth_util_test_compute_square_float();
void ags_fm_synth_util_test_compute_square_double();
void ags_fm_synth_util_test_compute_square_complex();

void ags_fm_synth_util_test_compute_impulse_s8();
void ags_fm_synth_util_test_compute_impulse_s16();
void ags_fm_synth_util_test_compute_impulse_s24();
void ags_fm_synth_util_test_compute_impulse_s32();
void ags_fm_synth_util_test_compute_impulse_s64();
void ags_fm_synth_util_test_compute_impulse_float();
void ags_fm_synth_util_test_compute_impulse_double();
void ags_fm_synth_util_test_compute_impulse_complex();

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
ags_fm_synth_util_test_alloc()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_copy()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_free()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_get_source()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_set_source()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_get_source_stride()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_set_source_stride()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_get_buffer_length()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_set_buffer_length()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_get_samplerate()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_set_samplerate()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_get_synth_oscillator_mode()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_set_synth_oscillator_mode()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_get_frequency()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_set_frequency()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_get_phase()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_set_phase()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_get_lfo_oscillator_mode()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_set_lfo_oscillator_mode()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_get_lfo_frequency()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_set_lfo_frequency()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_get_lfo_depth()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_set_lfo_depth()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_get_tuning()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_set_tuning()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_get_volume()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_set_volume()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_get_frame_count()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_set_frame_count()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_get_offset()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_set_offset()
{
  //TODO:JK: implement me
}

void
ags_fm_synth_util_test_compute_sin_s8()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint8 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_8_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_8_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SIN;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_sin_s8(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S8,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_sin_s16()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint16 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_16_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_16_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SIN;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_sin_s16(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S16,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_sin_s24()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint32 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_24_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_24_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SIN;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_sin_s24(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S24,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_sin_s32()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint32 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_32_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_32_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SIN;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_sin_s32(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S32,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_sin_s64()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint64 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_64_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_64_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SIN;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_sin_s64(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S64,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_sin_float()
{
  AgsFMSynthUtil fm_synth_util;
  
  gfloat *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_FLOAT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_FLOAT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SIN;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_sin_float(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_FLOAT,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_sin_double()
{
  AgsFMSynthUtil fm_synth_util;
  
  gdouble *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_DOUBLE);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_DOUBLE;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SIN;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_sin_double(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_sin_complex()
{
  AgsFMSynthUtil fm_synth_util;
  
  AgsComplex *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_COMPLEX);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_COMPLEX;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SIN;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_sin_complex(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_COMPLEX,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}


void
ags_fm_synth_util_test_compute_sawtooth_s8()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint8 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_8_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_8_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SAWTOOTH;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_sawtooth_s8(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S8,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_sawtooth_s16()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint16 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_16_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_16_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SAWTOOTH;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_sawtooth_s16(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S16,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_sawtooth_s24()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint32 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_24_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_24_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SAWTOOTH;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_sawtooth_s24(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S24,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_sawtooth_s32()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint32 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_32_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_32_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SAWTOOTH;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_sawtooth_s32(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S32,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_sawtooth_s64()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint64 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_64_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_64_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SAWTOOTH;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_sawtooth_s64(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S64,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_sawtooth_float()
{
  AgsFMSynthUtil fm_synth_util;
  
  gfloat *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_FLOAT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_FLOAT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SAWTOOTH;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_sawtooth_float(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_FLOAT,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_sawtooth_double()
{
  AgsFMSynthUtil fm_synth_util;
  
  gdouble *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_DOUBLE);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_DOUBLE;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SAWTOOTH;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_sawtooth_double(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_sawtooth_complex()
{
  AgsFMSynthUtil fm_synth_util;
  
  AgsComplex *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_COMPLEX);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_COMPLEX;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SAWTOOTH;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_sawtooth_complex(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_COMPLEX,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_triangle_s8()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint8 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_8_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_8_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_TRIANGLE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_triangle_s8(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S8,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_triangle_s16()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint16 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_16_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_16_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_TRIANGLE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_triangle_s16(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S16,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_triangle_s24()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint32 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_24_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_24_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_TRIANGLE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_triangle_s24(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S24,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_triangle_s32()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint32 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_32_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_32_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_TRIANGLE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_triangle_s32(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S32,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_triangle_s64()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint64 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_64_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_64_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_TRIANGLE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_triangle_s64(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S64,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_triangle_float()
{
  AgsFMSynthUtil fm_synth_util;
  
  gfloat *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_FLOAT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_FLOAT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_TRIANGLE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_triangle_float(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_FLOAT,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_triangle_double()
{
  AgsFMSynthUtil fm_synth_util;
  
  gdouble *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_DOUBLE);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_DOUBLE;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_TRIANGLE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_triangle_double(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_triangle_complex()
{
  AgsFMSynthUtil fm_synth_util;
  
  AgsComplex *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_COMPLEX);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_COMPLEX;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_TRIANGLE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_triangle_complex(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_COMPLEX,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_square_s8()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint8 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_8_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_8_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SQUARE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_square_s8(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S8,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_square_s16()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint16 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_16_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_16_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SQUARE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_square_s16(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S16,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_square_s24()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint32 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_24_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_24_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SQUARE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_square_s24(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S24,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_square_s32()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint32 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_32_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_32_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SQUARE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_square_s32(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S32,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_square_s64()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint64 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_64_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_64_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SQUARE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_square_s64(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S64,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_square_float()
{
  AgsFMSynthUtil fm_synth_util;
  
  gfloat *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_FLOAT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_FLOAT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SQUARE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_square_float(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_FLOAT,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_square_double()
{
  AgsFMSynthUtil fm_synth_util;
  
  gdouble *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_DOUBLE);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_DOUBLE;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SQUARE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_square_double(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_square_complex()
{
  AgsFMSynthUtil fm_synth_util;
  
  AgsComplex *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_COMPLEX);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_COMPLEX;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_SQUARE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_square_complex(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_COMPLEX,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_impulse_s8()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint8 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_8_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_8_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_IMPULSE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_impulse_s8(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S8,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_impulse_s16()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint16 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_16_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_16_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_IMPULSE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_impulse_s16(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S16,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_impulse_s24()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint32 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_24_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_24_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_IMPULSE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_impulse_s24(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S24,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_impulse_s32()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint32 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_32_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_32_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_IMPULSE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_impulse_s32(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S32,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_impulse_s64()
{
  AgsFMSynthUtil fm_synth_util;
  
  gint64 *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_SIGNED_64_BIT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_SIGNED_64_BIT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_IMPULSE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_impulse_s64(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_S64,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_impulse_float()
{
  AgsFMSynthUtil fm_synth_util;
  
  gfloat *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_FLOAT);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_FLOAT;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_IMPULSE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_impulse_float(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_FLOAT,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_impulse_double()
{
  AgsFMSynthUtil fm_synth_util;
  
  gdouble *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_DOUBLE);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_DOUBLE;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_IMPULSE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_impulse_double(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

void
ags_fm_synth_util_test_compute_impulse_complex()
{
  AgsFMSynthUtil fm_synth_util;
  
  AgsComplex *buffer;

  guint xcross_count;

  fm_synth_util.source =
    buffer = ags_stream_alloc(AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT,
			      AGS_SOUNDCARD_COMPLEX);

  fm_synth_util.source_stride = 1;
  
  fm_synth_util.buffer_length = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  fm_synth_util.format = AGS_SOUNDCARD_COMPLEX;
  fm_synth_util.samplerate = AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE;

  fm_synth_util.synth_oscillator_mode = AGS_SYNTH_OSCILLATOR_IMPULSE;

  fm_synth_util.frequency = AGS_FM_SYNTH_UTIL_TEST_FREQ;
  fm_synth_util.phase = AGS_FM_SYNTH_UTIL_TEST_PHASE;
  fm_synth_util.volume = AGS_FM_SYNTH_UTIL_TEST_VOLUME;

  fm_synth_util.lfo_oscillator_mode = AGS_FM_SYNTH_UTIL_TEST_LFO_OSC_MODE;
  
  fm_synth_util.lfo_frequency = AGS_FM_SYNTH_UTIL_TEST_LFO_FREQ;
  fm_synth_util.lfo_depth = AGS_FM_SYNTH_UTIL_TEST_LFO_DEPTH;
  fm_synth_util.tuning = AGS_FM_SYNTH_UTIL_TEST_TUNING;

  fm_synth_util.offset = 0;
  fm_synth_util.frame_count = AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT;
  
  ags_fm_synth_util_compute_impulse_complex(&fm_synth_util);
  
  xcross_count = ags_synth_util_get_xcross_count(buffer,
						 AGS_AUDIO_BUFFER_UTIL_COMPLEX,
						 AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT);

  CU_ASSERT(xcross_count + 2 > ((gdouble) AGS_FM_SYNTH_UTIL_TEST_FRAME_COUNT / (gdouble) AGS_FM_SYNTH_UTIL_TEST_SAMPLERATE) * AGS_FM_SYNTH_UTIL_TEST_FREQ);
}

int
main(int argc, char **argv)
{
  CU_pSuite pSuite = NULL;

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
  if((CU_add_test(pSuite, "test of ags_fm_synth_util.c alloc", ags_fm_synth_util_test_alloc) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c copy", ags_fm_synth_util_test_copy) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c free", ags_fm_synth_util_test_free) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c get source", ags_fm_synth_util_test_get_source) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c set source", ags_fm_synth_util_test_set_source) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c get source stride", ags_fm_synth_util_test_get_source_stride) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c set source stride", ags_fm_synth_util_test_set_source_stride) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c get buffer length", ags_fm_synth_util_test_get_buffer_length) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c set buffer length", ags_fm_synth_util_test_set_buffer_length) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c get samplerate", ags_fm_synth_util_test_get_samplerate) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c set samplerate", ags_fm_synth_util_test_set_samplerate) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c get synth oscillator mode", ags_fm_synth_util_test_get_synth_oscillator_mode) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c set synth oscillator mode", ags_fm_synth_util_test_set_synth_oscillator_mode) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c get frequency", ags_fm_synth_util_test_get_frequency) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c set frequency", ags_fm_synth_util_test_set_frequency) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c get phase", ags_fm_synth_util_test_get_phase) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c set phase", ags_fm_synth_util_test_set_phase) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c get volume", ags_fm_synth_util_test_get_volume) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c set volume", ags_fm_synth_util_test_set_volume) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c get lfo oscillator mode", ags_fm_synth_util_test_get_lfo_oscillator_mode) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c set lfo oscillator mode", ags_fm_synth_util_test_set_lfo_oscillator_mode) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c get lfo frequency", ags_fm_synth_util_test_get_lfo_frequency) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c set lfo frequency", ags_fm_synth_util_test_set_lfo_frequency) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c get lfo depth", ags_fm_synth_util_test_get_lfo_depth) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c set lfo depth", ags_fm_synth_util_test_set_lfo_depth) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c get tuning", ags_fm_synth_util_test_get_tuning) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c set tuning", ags_fm_synth_util_test_set_tuning) == NULL) ||     
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c get frame count", ags_fm_synth_util_test_get_frame_count) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c set frame count", ags_fm_synth_util_test_set_frame_count) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c get offset", ags_fm_synth_util_test_get_offset) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c set offset", ags_fm_synth_util_test_set_offset) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute sin s8", ags_fm_synth_util_test_compute_sin_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute sin s16", ags_fm_synth_util_test_compute_sin_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute sin s24", ags_fm_synth_util_test_compute_sin_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute sin s32", ags_fm_synth_util_test_compute_sin_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute sin s64", ags_fm_synth_util_test_compute_sin_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute sin float", ags_fm_synth_util_test_compute_sin_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute sin double", ags_fm_synth_util_test_compute_sin_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute sin complex", ags_fm_synth_util_test_compute_sin_complex) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute sawtooth s8", ags_fm_synth_util_test_compute_sawtooth_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute sawtooth s16", ags_fm_synth_util_test_compute_sawtooth_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute sawtooth s24", ags_fm_synth_util_test_compute_sawtooth_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute sawtooth s32", ags_fm_synth_util_test_compute_sawtooth_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute sawtooth s64", ags_fm_synth_util_test_compute_sawtooth_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute sawtooth float", ags_fm_synth_util_test_compute_sawtooth_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute sawtooth double", ags_fm_synth_util_test_compute_sawtooth_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute sawtooth complex", ags_fm_synth_util_test_compute_sawtooth_complex) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute triangle s8", ags_fm_synth_util_test_compute_triangle_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute triangle s16", ags_fm_synth_util_test_compute_triangle_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute triangle s24", ags_fm_synth_util_test_compute_triangle_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute triangle s32", ags_fm_synth_util_test_compute_triangle_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute triangle s64", ags_fm_synth_util_test_compute_triangle_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute triangle float", ags_fm_synth_util_test_compute_triangle_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute triangle double", ags_fm_synth_util_test_compute_triangle_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute triangle complex", ags_fm_synth_util_test_compute_triangle_complex) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute square s8", ags_fm_synth_util_test_compute_square_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute square s16", ags_fm_synth_util_test_compute_square_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute square s24", ags_fm_synth_util_test_compute_square_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute square s32", ags_fm_synth_util_test_compute_square_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute square s64", ags_fm_synth_util_test_compute_square_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute square float", ags_fm_synth_util_test_compute_square_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute square double", ags_fm_synth_util_test_compute_square_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute square complex", ags_fm_synth_util_test_compute_square_complex) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute impulse s16", ags_fm_synth_util_test_compute_impulse_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute impulse s24", ags_fm_synth_util_test_compute_impulse_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute impulse s32", ags_fm_synth_util_test_compute_impulse_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute impulse s64", ags_fm_synth_util_test_compute_impulse_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute impulse float", ags_fm_synth_util_test_compute_impulse_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute impulse double", ags_fm_synth_util_test_compute_impulse_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_fm_synth_util.c compute impulse complex", ags_fm_synth_util_test_compute_impulse_complex) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
