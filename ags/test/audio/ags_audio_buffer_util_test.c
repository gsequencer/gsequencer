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

#include <glib.h>
#include <glib-object.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

int ags_audio_buffer_util_test_init_suite();
int ags_audio_buffer_util_test_clean_suite();

void ags_audio_buffer_util_test_format_from_soundcard();
void ags_audio_buffer_util_test_get_copy_mode();
void ags_audio_buffer_util_test_clear_float();
void ags_audio_buffer_util_test_clear_double();
void ags_audio_buffer_util_test_clear_buffer();
void ags_audio_buffer_util_test_copy_s8_to_s8();
void ags_audio_buffer_util_test_copy_s8_to_s16();
void ags_audio_buffer_util_test_copy_s8_to_s24();
void ags_audio_buffer_util_test_copy_s8_to_s32();
void ags_audio_buffer_util_test_copy_s8_to_s64();
void ags_audio_buffer_util_test_copy_s8_to_float();
void ags_audio_buffer_util_test_copy_s8_to_double();
void ags_audio_buffer_util_test_copy_s16_to_s8();
void ags_audio_buffer_util_test_copy_s16_to_s16();
void ags_audio_buffer_util_test_copy_s16_to_s24();
void ags_audio_buffer_util_test_copy_s16_to_s32();
void ags_audio_buffer_util_test_copy_s16_to_s64();
void ags_audio_buffer_util_test_copy_s16_to_float();
void ags_audio_buffer_util_test_copy_s16_to_double();
void ags_audio_buffer_util_test_copy_s24_to_s8();
void ags_audio_buffer_util_test_copy_s24_to_s16();
void ags_audio_buffer_util_test_copy_s24_to_s24();
void ags_audio_buffer_util_test_copy_s24_to_s32();
void ags_audio_buffer_util_test_copy_s24_to_s64();
void ags_audio_buffer_util_test_copy_s24_to_float();
void ags_audio_buffer_util_test_copy_s24_to_double();
void ags_audio_buffer_util_test_copy_s32_to_s8();
void ags_audio_buffer_util_test_copy_s32_to_s16();
void ags_audio_buffer_util_test_copy_s32_to_s24();
void ags_audio_buffer_util_test_copy_s32_to_s32();
void ags_audio_buffer_util_test_copy_s32_to_s64();
void ags_audio_buffer_util_test_copy_s32_to_float();
void ags_audio_buffer_util_test_copy_s32_to_double();
void ags_audio_buffer_util_test_copy_s64_to_s8();
void ags_audio_buffer_util_test_copy_s64_to_s16();
void ags_audio_buffer_util_test_copy_s64_to_s24();
void ags_audio_buffer_util_test_copy_s64_to_s32();
void ags_audio_buffer_util_test_copy_s64_to_s64();
void ags_audio_buffer_util_test_copy_s64_to_float();
void ags_audio_buffer_util_test_copy_s64_to_double();
void ags_audio_buffer_util_test_copy_float_to_s8();
void ags_audio_buffer_util_test_copy_float_to_s16();
void ags_audio_buffer_util_test_copy_float_to_s24();
void ags_audio_buffer_util_test_copy_float_to_s32();
void ags_audio_buffer_util_test_copy_float_to_s64();
void ags_audio_buffer_util_test_copy_float_to_float();
void ags_audio_buffer_util_test_copy_float_to_double();
void ags_audio_buffer_util_test_copy_double_to_s8();
void ags_audio_buffer_util_test_copy_double_to_s16();
void ags_audio_buffer_util_test_copy_double_to_s24();
void ags_audio_buffer_util_test_copy_double_to_s32();
void ags_audio_buffer_util_test_copy_double_to_s64();
void ags_audio_buffer_util_test_copy_double_to_float();
void ags_audio_buffer_util_test_copy_double_to_double();
void ags_audio_buffer_util_test_copy_buffer_to_buffer();

#define AGS_AUDIO_BUFFER_UTIL_TEST_MAX_S24 (0x7fffff)
#define AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY (440.0)
#define AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE (44100.0)

#define AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_FLOAT_BUFFER_SIZE (1024)

#define AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_DOUBLE_BUFFER_SIZE (1024)

#define AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_BUFFER_BUFFER_SIZE (1024)

#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S8_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S16_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S24_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S32_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S64_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_FLOAT_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_DOUBLE_BUFFER_SIZE (8192)

#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S8_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S16_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S24_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S32_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S64_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_FLOAT_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_DOUBLE_BUFFER_SIZE (8192)

#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S8_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S16_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S24_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S32_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S64_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_FLOAT_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_DOUBLE_BUFFER_SIZE (8192)

#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S8_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S16_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S24_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S32_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S64_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_FLOAT_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_DOUBLE_BUFFER_SIZE (8192)

#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S8_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S16_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S24_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S32_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S64_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_FLOAT_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_DOUBLE_BUFFER_SIZE (8192)

#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S8_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S16_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S24_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S32_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S64_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_FLOAT_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_DOUBLE_BUFFER_SIZE (8192)

#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S8_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S16_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S24_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S32_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S64_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_FLOAT_BUFFER_SIZE (8192)
#define AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_DOUBLE_BUFFER_SIZE (8192)

AgsAudioBufferUtil audio_buffer_util;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_audio_buffer_util_test_init_suite()
{ 
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_audio_buffer_util_test_clean_suite()
{
  return(0);
}

void
ags_audio_buffer_util_test_format_from_soundcard()
{
  CU_ASSERT(ags_audio_buffer_util_format_from_soundcard(&audio_buffer_util,
							AGS_SOUNDCARD_SIGNED_8_BIT) == AGS_AUDIO_BUFFER_UTIL_S8);

  CU_ASSERT(ags_audio_buffer_util_format_from_soundcard(&audio_buffer_util,
							AGS_SOUNDCARD_SIGNED_16_BIT) == AGS_AUDIO_BUFFER_UTIL_S16);

  CU_ASSERT(ags_audio_buffer_util_format_from_soundcard(&audio_buffer_util,
							AGS_SOUNDCARD_SIGNED_24_BIT) == AGS_AUDIO_BUFFER_UTIL_S24);

  CU_ASSERT(ags_audio_buffer_util_format_from_soundcard(&audio_buffer_util,
							AGS_SOUNDCARD_SIGNED_32_BIT) == AGS_AUDIO_BUFFER_UTIL_S32);

  CU_ASSERT(ags_audio_buffer_util_format_from_soundcard(&audio_buffer_util,
							AGS_SOUNDCARD_SIGNED_64_BIT) == AGS_AUDIO_BUFFER_UTIL_S64);

  CU_ASSERT(ags_audio_buffer_util_format_from_soundcard(&audio_buffer_util,
							AGS_SOUNDCARD_FLOAT) == AGS_AUDIO_BUFFER_UTIL_FLOAT);

  CU_ASSERT(ags_audio_buffer_util_format_from_soundcard(&audio_buffer_util,
							AGS_SOUNDCARD_DOUBLE) == AGS_AUDIO_BUFFER_UTIL_DOUBLE);
}

void
ags_audio_buffer_util_test_get_copy_mode()
{
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S8, AGS_AUDIO_BUFFER_UTIL_S8) == AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S8);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S16, AGS_AUDIO_BUFFER_UTIL_S8) == AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S16);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S24, AGS_AUDIO_BUFFER_UTIL_S8) == AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S24);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S32, AGS_AUDIO_BUFFER_UTIL_S8) == AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S32);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S64, AGS_AUDIO_BUFFER_UTIL_S8) == AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S64);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_FLOAT, AGS_AUDIO_BUFFER_UTIL_S8) == AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_FLOAT);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_DOUBLE, AGS_AUDIO_BUFFER_UTIL_S8) == AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_DOUBLE);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S8, AGS_AUDIO_BUFFER_UTIL_S16) == AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S8);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S16, AGS_AUDIO_BUFFER_UTIL_S16) == AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S16);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S24, AGS_AUDIO_BUFFER_UTIL_S16) == AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S24);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S32, AGS_AUDIO_BUFFER_UTIL_S16) == AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S32);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S64, AGS_AUDIO_BUFFER_UTIL_S16) == AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S64);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_FLOAT, AGS_AUDIO_BUFFER_UTIL_S16) == AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_FLOAT);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_DOUBLE, AGS_AUDIO_BUFFER_UTIL_S16) == AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_DOUBLE);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S8, AGS_AUDIO_BUFFER_UTIL_S24) == AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S8);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S16, AGS_AUDIO_BUFFER_UTIL_S24) == AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S16);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S24, AGS_AUDIO_BUFFER_UTIL_S24) == AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S24);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S32, AGS_AUDIO_BUFFER_UTIL_S24) == AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S32);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S64, AGS_AUDIO_BUFFER_UTIL_S24) == AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_S64);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_FLOAT, AGS_AUDIO_BUFFER_UTIL_S24) == AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_FLOAT);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_DOUBLE, AGS_AUDIO_BUFFER_UTIL_S24) == AGS_AUDIO_BUFFER_UTIL_COPY_S24_TO_DOUBLE);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S8, AGS_AUDIO_BUFFER_UTIL_S32) == AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S8);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S16, AGS_AUDIO_BUFFER_UTIL_S32) == AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S16);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S24, AGS_AUDIO_BUFFER_UTIL_S32) == AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S24);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S32, AGS_AUDIO_BUFFER_UTIL_S32) == AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S64, AGS_AUDIO_BUFFER_UTIL_S32) == AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S64);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_FLOAT, AGS_AUDIO_BUFFER_UTIL_S32) == AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_FLOAT);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_DOUBLE, AGS_AUDIO_BUFFER_UTIL_S32) == AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_DOUBLE);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S8, AGS_AUDIO_BUFFER_UTIL_S64) == AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S8);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S16, AGS_AUDIO_BUFFER_UTIL_S64) == AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S16);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S24, AGS_AUDIO_BUFFER_UTIL_S64) == AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S24);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S32, AGS_AUDIO_BUFFER_UTIL_S64) == AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S32);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S64, AGS_AUDIO_BUFFER_UTIL_S64) == AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S64);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_FLOAT, AGS_AUDIO_BUFFER_UTIL_S64) == AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_FLOAT);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_DOUBLE, AGS_AUDIO_BUFFER_UTIL_S64) == AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_DOUBLE);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S8, AGS_AUDIO_BUFFER_UTIL_FLOAT) == AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S8);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S16, AGS_AUDIO_BUFFER_UTIL_FLOAT) == AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S16);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S24, AGS_AUDIO_BUFFER_UTIL_FLOAT) == AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S24);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S32, AGS_AUDIO_BUFFER_UTIL_FLOAT) == AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S32);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S64, AGS_AUDIO_BUFFER_UTIL_FLOAT) == AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_S64);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_FLOAT, AGS_AUDIO_BUFFER_UTIL_FLOAT) == AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_DOUBLE, AGS_AUDIO_BUFFER_UTIL_FLOAT) == AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_DOUBLE);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S8, AGS_AUDIO_BUFFER_UTIL_DOUBLE) == AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S8);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S16, AGS_AUDIO_BUFFER_UTIL_DOUBLE) == AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S16);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S24, AGS_AUDIO_BUFFER_UTIL_DOUBLE) == AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S24);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S32, AGS_AUDIO_BUFFER_UTIL_DOUBLE) == AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S32);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_S64, AGS_AUDIO_BUFFER_UTIL_DOUBLE) == AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_S64);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_FLOAT, AGS_AUDIO_BUFFER_UTIL_DOUBLE) == AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_FLOAT);
  CU_ASSERT(ags_audio_buffer_util_get_copy_mode_from_format(&audio_buffer_util, AGS_AUDIO_BUFFER_UTIL_DOUBLE, AGS_AUDIO_BUFFER_UTIL_DOUBLE) == AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_DOUBLE);
}

void
ags_audio_buffer_util_test_clear_float()
{
  gfloat *buffer;

  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_FLOAT_BUFFER_SIZE,
			    AGS_SOUNDCARD_FLOAT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_FLOAT_BUFFER_SIZE; i++){
    buffer[i] = 1.0 / rand();
  }

  /* clear and assert */
  ags_audio_buffer_util_clear_float(&audio_buffer_util,
				    buffer, 1,
				    AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_FLOAT_BUFFER_SIZE);

  success = TRUE;

  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_FLOAT_BUFFER_SIZE; i++){
    if(buffer[i] != 0.0){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success);
}

void
ags_audio_buffer_util_test_clear_double()
{
  gdouble *buffer;

  guint i;
  gboolean success;
  
  buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_DOUBLE_BUFFER_SIZE,
			    AGS_SOUNDCARD_DOUBLE);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_DOUBLE_BUFFER_SIZE; i++){
    buffer[i] = 1.0 / rand();
  }

  /* clear and assert */
  ags_audio_buffer_util_clear_double(&audio_buffer_util,
				     buffer, 1,
				     AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_DOUBLE_BUFFER_SIZE);

  success = TRUE;

  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_DOUBLE_BUFFER_SIZE; i++){
    if(buffer[i] != 0.0){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success);
}

void
ags_audio_buffer_util_test_clear_buffer()
{
  gint8 *s8_buffer;
  gint16 *s16_buffer;
  gint32 *s24_buffer;
  gint32 *s32_buffer;
  gint64 *s64_buffer;

  guint i;
  gboolean success;

  /* signed 8 bit */
  s8_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_BUFFER_BUFFER_SIZE,
			       AGS_SOUNDCARD_SIGNED_8_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_BUFFER_BUFFER_SIZE; i++){
    s8_buffer[i] = G_MAXINT8 / rand();
  }

  /* clear and assert */
  ags_audio_buffer_util_clear_buffer(&audio_buffer_util,
				     s8_buffer, 1,
				     AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_BUFFER_BUFFER_SIZE, AGS_AUDIO_BUFFER_UTIL_S8);

  success = TRUE;

  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_BUFFER_BUFFER_SIZE; i++){
    if(s8_buffer[i] != 0){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success);

  /* signed 16 bit */
  s16_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_BUFFER_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_16_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_BUFFER_BUFFER_SIZE; i++){
    s16_buffer[i] = G_MAXINT16 / rand();
  }

  /* clear and assert */
  ags_audio_buffer_util_clear_buffer(&audio_buffer_util,
				     s16_buffer, 1,
				     AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_BUFFER_BUFFER_SIZE, AGS_AUDIO_BUFFER_UTIL_S16);

  success = TRUE;

  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_BUFFER_BUFFER_SIZE; i++){
    if(s16_buffer[i] != 0){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success);

  /* signed 24 bit */
  s24_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_BUFFER_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_24_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_BUFFER_BUFFER_SIZE; i++){
    s24_buffer[i] = AGS_AUDIO_BUFFER_UTIL_TEST_MAX_S24 / rand();
  }

  /* clear and assert */
  ags_audio_buffer_util_clear_buffer(&audio_buffer_util,
				     s24_buffer, 1,
				     AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_BUFFER_BUFFER_SIZE, AGS_AUDIO_BUFFER_UTIL_S24);

  success = TRUE;

  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_BUFFER_BUFFER_SIZE; i++){
    if(s24_buffer[i] != 0){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success);

  /* signed 32 bit */
  s32_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_BUFFER_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_32_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_BUFFER_BUFFER_SIZE; i++){
    s32_buffer[i] = G_MAXINT32 / rand();
  }

  /* clear and assert */
  ags_audio_buffer_util_clear_buffer(&audio_buffer_util,
				     s32_buffer, 1,
				     AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_BUFFER_BUFFER_SIZE, AGS_AUDIO_BUFFER_UTIL_S32);

  success = TRUE;

  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_BUFFER_BUFFER_SIZE; i++){
    if(s32_buffer[i] != 0){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success);

  /* signed 64 bit */
  s64_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_BUFFER_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_64_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_BUFFER_BUFFER_SIZE; i++){
    s64_buffer[i] = G_MAXINT64 / rand();
  }

  /* clear and assert */
  ags_audio_buffer_util_clear_buffer(&audio_buffer_util,
				     s64_buffer, 1,
				     AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_BUFFER_BUFFER_SIZE, AGS_AUDIO_BUFFER_UTIL_S64);

  success = TRUE;

  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_CLEAR_BUFFER_BUFFER_SIZE; i++){
    if(s64_buffer[i] != 0){
      success = FALSE;

      break;
    }
  }

  CU_ASSERT(success);
}

void
ags_audio_buffer_util_test_copy_s8_to_s8()
{
  gint8 *s8_buffer, *s8_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s8_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S8_BUFFER_SIZE,
			       AGS_SOUNDCARD_SIGNED_8_BIT);
  s8_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S8_BUFFER_SIZE,
				    AGS_SOUNDCARD_SIGNED_8_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S8_BUFFER_SIZE; i++){
    s8_buffer[i] = G_MAXINT8 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s8(s8_buffer,
							 AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S8_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s8_to_s8(&audio_buffer_util,
				      s8_destination, 1,
				      s8_buffer, 1,
				      AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S8_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s8(s8_destination,
						    AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S8_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s8_to_s16()
{
  gint8 *s8_buffer;
  gint16 *s16_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s8_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S16_BUFFER_SIZE,
			       AGS_SOUNDCARD_SIGNED_8_BIT);
  s16_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S16_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_16_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S16_BUFFER_SIZE; i++){
    s8_buffer[i] = G_MAXINT8 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s8(s8_buffer,
							 AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S16_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s8_to_s16(&audio_buffer_util,
				       s16_destination, 1,
				       s8_buffer, 1,
				       AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S16_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s16(s16_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S16_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s8_to_s24()
{
  gint8 *s8_buffer;
  gint32 *s24_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s8_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S24_BUFFER_SIZE,
			       AGS_SOUNDCARD_SIGNED_8_BIT);
  s24_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S24_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_24_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S24_BUFFER_SIZE; i++){
    s8_buffer[i] = G_MAXINT8 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s8(s8_buffer,
							 AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S24_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s8_to_s24(&audio_buffer_util,
				       s24_destination, 1,
				       s8_buffer, 1,
				       AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S24_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s24(s24_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S24_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s8_to_s32()
{
  gint8 *s8_buffer;
  gint32 *s32_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s8_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S32_BUFFER_SIZE,
			       AGS_SOUNDCARD_SIGNED_8_BIT);
  s32_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S32_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_32_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S32_BUFFER_SIZE; i++){
    s8_buffer[i] = G_MAXINT8 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s8(s8_buffer,
							 AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S32_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s8_to_s32(&audio_buffer_util,
				       s32_destination, 1,
				       s8_buffer, 1,
				       AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S32_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s32(s32_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S32_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s8_to_s64()
{
  gint8 *s8_buffer;
  gint64 *s64_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s8_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S64_BUFFER_SIZE,
			       AGS_SOUNDCARD_SIGNED_8_BIT);
  s64_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S64_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_64_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S64_BUFFER_SIZE; i++){
    s8_buffer[i] = G_MAXINT8 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s8(s8_buffer,
							 AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S64_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s8_to_s64(&audio_buffer_util,
				       s64_destination, 1,
				       s8_buffer, 1,
				       AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S64_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s64(s64_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_S64_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s8_to_float()
{
  gint8 *s8_buffer;
  gfloat *float_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s8_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_FLOAT_BUFFER_SIZE,
			       AGS_SOUNDCARD_SIGNED_8_BIT);
  float_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_FLOAT_BUFFER_SIZE,
				       AGS_SOUNDCARD_FLOAT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_FLOAT_BUFFER_SIZE; i++){
    s8_buffer[i] = G_MAXINT8 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s8(s8_buffer,
							 AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_FLOAT_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s8_to_float(&audio_buffer_util,
					 float_destination, 1,
					 s8_buffer, 1,
					 AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_FLOAT_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_float(float_destination,
						       AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_FLOAT_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s8_to_double()
{
  gint8 *s8_buffer;
  gdouble *double_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s8_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_DOUBLE_BUFFER_SIZE,
			       AGS_SOUNDCARD_SIGNED_8_BIT);
  double_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_DOUBLE_BUFFER_SIZE,
					AGS_SOUNDCARD_DOUBLE);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_DOUBLE_BUFFER_SIZE; i++){
    s8_buffer[i] = G_MAXINT8 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s8(s8_buffer,
							 AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_DOUBLE_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s8_to_double(&audio_buffer_util,
					  double_destination, 1,
					  s8_buffer, 1,
					  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_DOUBLE_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_double(double_destination,
							AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S8_TO_DOUBLE_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s16_to_s8()
{
  gint16 *s16_buffer;
  gint8 *s8_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s16_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S8_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_16_BIT);
  s8_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S8_BUFFER_SIZE,
				    AGS_SOUNDCARD_SIGNED_8_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S8_BUFFER_SIZE; i++){
    s16_buffer[i] = G_MAXINT16 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s16(s16_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S8_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s16_to_s8(&audio_buffer_util,
				       s8_destination, 1,
				       s16_buffer, 1,
				       AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S8_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s8(s8_destination,
						    AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S8_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s16_to_s16()
{
  gint16 *s16_buffer, *s16_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s16_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S16_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_16_BIT);
  s16_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S16_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_16_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S16_BUFFER_SIZE; i++){
    s16_buffer[i] = G_MAXINT16 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s16(s16_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S16_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s16_to_s16(&audio_buffer_util,
					s16_destination, 1,
					s16_buffer, 1,
					AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S16_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s16(s16_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S16_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s16_to_s24()
{
  gint16 *s16_buffer;
  gint32 *s24_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s16_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S24_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_16_BIT);
  s24_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S24_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_24_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S24_BUFFER_SIZE; i++){
    s16_buffer[i] = G_MAXINT16 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s16(s16_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S24_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s16_to_s24(&audio_buffer_util,
					s24_destination, 1,
					s16_buffer, 1,
					AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S24_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s24(s24_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S24_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s16_to_s32()
{
  gint16 *s16_buffer;
  gint32 *s32_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s16_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S32_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_16_BIT);
  s32_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S32_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_32_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S32_BUFFER_SIZE; i++){
    s16_buffer[i] = G_MAXINT16 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s16(s16_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S32_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s16_to_s32(&audio_buffer_util,
					s32_destination, 1,
					s16_buffer, 1,
					AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S32_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s32(s32_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S32_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s16_to_s64()
{
  gint16 *s16_buffer;
  gint64 *s64_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s16_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S64_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_16_BIT);
  s64_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S64_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_64_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S64_BUFFER_SIZE; i++){
    s16_buffer[i] = G_MAXINT16 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s16(s16_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S64_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s16_to_s64(&audio_buffer_util,
					s64_destination, 1,
					s16_buffer, 1,
					AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S64_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s64(s64_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_S64_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s16_to_float()
{
  gint16 *s16_buffer;
  gfloat *float_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s16_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_FLOAT_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_16_BIT);
  float_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_FLOAT_BUFFER_SIZE,
				       AGS_SOUNDCARD_FLOAT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_FLOAT_BUFFER_SIZE; i++){
    s16_buffer[i] = G_MAXINT16 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s16(s16_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_FLOAT_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s16_to_float(&audio_buffer_util,
					  float_destination, 1,
					  s16_buffer, 1,
					  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_FLOAT_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_float(float_destination,
						       AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_FLOAT_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s16_to_double()
{
  gint16 *s16_buffer;
  gdouble *double_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s16_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_DOUBLE_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_16_BIT);
  double_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_DOUBLE_BUFFER_SIZE,
					AGS_SOUNDCARD_DOUBLE);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_DOUBLE_BUFFER_SIZE; i++){
    s16_buffer[i] = G_MAXINT16 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s16(s16_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_DOUBLE_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s16_to_double(&audio_buffer_util,
					   double_destination, 1,
					   s16_buffer, 1,
					   AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_DOUBLE_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_double(double_destination,
							AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S16_TO_DOUBLE_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s24_to_s8()
{
  gint32 *s24_buffer;
  gint8 *s8_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s24_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S8_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_24_BIT);
  s8_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S8_BUFFER_SIZE,
				    AGS_SOUNDCARD_SIGNED_8_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S8_BUFFER_SIZE; i++){
    s24_buffer[i] = AGS_AUDIO_BUFFER_UTIL_TEST_MAX_S24 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s24(s24_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S8_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s24_to_s8(&audio_buffer_util,
				       s8_destination, 1,
				       s24_buffer, 1,
				       AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S8_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s8(s8_destination,
						    AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S8_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s24_to_s16()
{
  gint32 *s24_buffer;
  gint16 *s16_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s24_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S16_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_24_BIT);
  s16_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S16_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_16_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S16_BUFFER_SIZE; i++){
    s24_buffer[i] = AGS_AUDIO_BUFFER_UTIL_TEST_MAX_S24 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s24(s24_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S16_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s24_to_s16(&audio_buffer_util,
					s16_destination, 1,
					s24_buffer, 1,
					AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S16_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s16(s16_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S16_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s24_to_s24()
{
  gint32 *s24_buffer, *s24_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s24_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S24_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_24_BIT);
  s24_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S24_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_24_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S24_BUFFER_SIZE; i++){
    s24_buffer[i] = AGS_AUDIO_BUFFER_UTIL_TEST_MAX_S24 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s24(s24_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S24_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s24_to_s24(&audio_buffer_util,
					s24_destination, 1,
					s24_buffer, 1,
					AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S24_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s24(s24_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S24_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s24_to_s32()
{
  gint32 *s24_buffer;
  gint32 *s32_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s24_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S32_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_24_BIT);
  s32_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S32_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_32_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S32_BUFFER_SIZE; i++){
    s24_buffer[i] = AGS_AUDIO_BUFFER_UTIL_TEST_MAX_S24 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s24(s24_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S32_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s24_to_s32(&audio_buffer_util,
					s32_destination, 1,
					s24_buffer, 1,
					AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S32_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s32(s32_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S32_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s24_to_s64()
{
  gint32 *s24_buffer;
  gint64 *s64_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s24_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S64_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_24_BIT);
  s64_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S64_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_64_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S64_BUFFER_SIZE; i++){
    s24_buffer[i] = AGS_AUDIO_BUFFER_UTIL_TEST_MAX_S24 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s24(s24_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S64_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s24_to_s64(&audio_buffer_util,
					s64_destination, 1,
					s24_buffer, 1,
					AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S64_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s64(s64_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_S64_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s24_to_float()
{
  gint32 *s24_buffer;
  gfloat *float_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s24_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_FLOAT_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_24_BIT);
  float_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_FLOAT_BUFFER_SIZE,
				       AGS_SOUNDCARD_FLOAT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_FLOAT_BUFFER_SIZE; i++){
    s24_buffer[i] = AGS_AUDIO_BUFFER_UTIL_TEST_MAX_S24 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s24(s24_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_FLOAT_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s24_to_float(&audio_buffer_util,
					  float_destination, 1,
					  s24_buffer, 1,
					  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_FLOAT_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_float(float_destination,
						       AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_FLOAT_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s24_to_double()
{
  gint32 *s24_buffer;
  gdouble *double_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s24_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_DOUBLE_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_24_BIT);
  double_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_DOUBLE_BUFFER_SIZE,
					AGS_SOUNDCARD_DOUBLE);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_DOUBLE_BUFFER_SIZE; i++){
    s24_buffer[i] = AGS_AUDIO_BUFFER_UTIL_TEST_MAX_S24 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s24(s24_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_DOUBLE_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s24_to_double(&audio_buffer_util,
					   double_destination, 1,
					   s24_buffer, 1,
					   AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_DOUBLE_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_double(double_destination,
							AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S24_TO_DOUBLE_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s32_to_s8()
{
  gint32 *s32_buffer;
  gint8 *s8_destination;

  guint orig_xcross_count, xcross_count;
  guint i;

  static const gdouble scale = 214748363.0;
  
  s32_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S8_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_32_BIT);
  s8_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S8_BUFFER_SIZE,
				    AGS_SOUNDCARD_SIGNED_8_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S8_BUFFER_SIZE; i++){
    s32_buffer[i] = (gint32) (scale * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE));
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s32(s32_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S8_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s32_to_s8(&audio_buffer_util,
				       s8_destination, 1,
				       s32_buffer, 1,
				       AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S8_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s8(s8_destination,
						    AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S8_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s32_to_s16()
{
  gint32 *s32_buffer;
  gint16 *s16_destination;

  guint orig_xcross_count, xcross_count;
  guint i;

  static const gdouble scale = 214748363.0;
  
  s32_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S16_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_32_BIT);
  s16_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S16_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_16_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S16_BUFFER_SIZE; i++){
    s32_buffer[i] = (gint32) (scale * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE));
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s32(s32_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S16_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s32_to_s16(&audio_buffer_util,
					s16_destination, 1,
					s32_buffer, 1,
					AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S16_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s16(s16_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S16_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s32_to_s24()
{
  gint32 *s32_buffer;
  gint32 *s24_destination;

  guint orig_xcross_count, xcross_count;
  guint i;

  static const gdouble scale = 214748363.0;
  
  s32_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S24_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_32_BIT);
  s24_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S24_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_24_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S24_BUFFER_SIZE; i++){
    s32_buffer[i] = (gint32) (scale * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE));
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s32(s32_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S24_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s32_to_s24(&audio_buffer_util,
					s24_destination, 1,
					s32_buffer, 1,
					AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S24_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s24(s24_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S24_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s32_to_s32()
{
  gint32 *s32_buffer, *s32_destination;

  guint orig_xcross_count, xcross_count;
  guint i;

  static const gdouble scale = 214748363.0;
  
  s32_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S32_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_32_BIT);
  s32_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S32_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_32_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S32_BUFFER_SIZE; i++){
    s32_buffer[i] = (gint32) (scale * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE));
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s32(s32_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S32_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s32_to_s32(&audio_buffer_util,
					s32_destination, 1,
					s32_buffer, 1,
					AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S32_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s32(s32_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S32_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s32_to_s64()
{
  gint32 *s32_buffer;
  gint64 *s64_destination;

  guint orig_xcross_count, xcross_count;
  guint i;

  static const gdouble scale = 214748363.0;
  
  s32_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S64_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_32_BIT);
  s64_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S64_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_64_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S64_BUFFER_SIZE; i++){
    s32_buffer[i] = (gint32) (scale * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE));
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s32(s32_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S64_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s32_to_s64(&audio_buffer_util,
					s64_destination, 1,
					s32_buffer, 1,
					AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S64_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s64(s64_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_S64_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s32_to_float()
{
  gint32 *s32_buffer;
  gfloat *float_destination;

  guint orig_xcross_count, xcross_count;
  guint i;

  static const gdouble scale = 214748363.0;
  
  s32_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_FLOAT_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_32_BIT);
  float_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_FLOAT_BUFFER_SIZE,
				       AGS_SOUNDCARD_FLOAT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_FLOAT_BUFFER_SIZE; i++){
    s32_buffer[i] = (gint32)(scale * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE));
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s32(s32_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_FLOAT_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s32_to_float(&audio_buffer_util,
					  float_destination, 1,
					  s32_buffer, 1,
					  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_FLOAT_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_float(float_destination,
						       AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_FLOAT_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s32_to_double()
{
  gint32 *s32_buffer;
  gdouble *double_destination;

  guint orig_xcross_count, xcross_count;
  guint i;

  static const gdouble scale = 214748363.0;
  
  s32_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_DOUBLE_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_32_BIT);
  double_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_DOUBLE_BUFFER_SIZE,
					AGS_SOUNDCARD_DOUBLE);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_DOUBLE_BUFFER_SIZE; i++){
    s32_buffer[i] = (gint32)(scale * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE));
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s32(s32_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_DOUBLE_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s32_to_double(&audio_buffer_util,
					   double_destination, 1,
					   s32_buffer, 1,
					   AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_DOUBLE_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_double(double_destination,
							AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S32_TO_DOUBLE_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s64_to_s8()
{
  gint64 *s64_buffer;
  gint8 *s8_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s64_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S8_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_64_BIT);
  s8_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S8_BUFFER_SIZE,
				    AGS_SOUNDCARD_SIGNED_8_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S8_BUFFER_SIZE; i++){
    s64_buffer[i] = G_MAXINT64 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s64(s64_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S8_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s64_to_s8(&audio_buffer_util,
				       s8_destination, 1,
				       s64_buffer, 1,
				       AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S8_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s8(s8_destination,
						    AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S8_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s64_to_s16()
{
  gint64 *s64_buffer;
  gint16 *s16_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s64_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S16_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_64_BIT);
  s16_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S16_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_16_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S16_BUFFER_SIZE; i++){
    s64_buffer[i] = G_MAXINT64 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s64(s64_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S16_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s64_to_s16(&audio_buffer_util,
					s16_destination, 1,
					s64_buffer, 1,
					AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S16_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s16(s16_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S16_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s64_to_s24()
{
  gint64 *s64_buffer;
  gint32 *s24_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s64_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S24_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_64_BIT);
  s24_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S24_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_24_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S24_BUFFER_SIZE; i++){
    s64_buffer[i] = G_MAXINT64 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s64(s64_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S24_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s64_to_s24(&audio_buffer_util,
					s24_destination, 1,
					s64_buffer, 1,
					AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S24_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s24(s24_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S24_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s64_to_s32()
{
  gint64 *s64_buffer;
  gint32 *s32_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s64_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S32_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_64_BIT);
  s32_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S32_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_32_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S32_BUFFER_SIZE; i++){
    s64_buffer[i] = G_MAXINT64 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s64(s64_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S32_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s64_to_s32(&audio_buffer_util,
					s32_destination, 1,
					s64_buffer, 1,
					AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S32_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s32(s32_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S32_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s64_to_s64()
{
  gint64 *s64_buffer, *s64_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s64_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S64_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_64_BIT);
  s64_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S64_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_64_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S64_BUFFER_SIZE; i++){
    s64_buffer[i] = G_MAXINT64 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s64(s64_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S64_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s64_to_s64(&audio_buffer_util,
					s64_destination, 1,
					s64_buffer, 1,
					AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S64_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s64(s64_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_S64_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s64_to_float()
{
  gint64 *s64_buffer;
  gfloat *float_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s64_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_FLOAT_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_64_BIT);
  float_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_FLOAT_BUFFER_SIZE,
				       AGS_SOUNDCARD_FLOAT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_FLOAT_BUFFER_SIZE; i++){
    s64_buffer[i] = G_MAXINT64 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s64(s64_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_FLOAT_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s64_to_float(&audio_buffer_util,
					  float_destination, 1,
					  s64_buffer, 1,
					  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_FLOAT_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_float(float_destination,
						       AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_FLOAT_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_s64_to_double()
{
  gint64 *s64_buffer;
  gdouble *double_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  s64_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_DOUBLE_BUFFER_SIZE,
				AGS_SOUNDCARD_SIGNED_64_BIT);
  double_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_DOUBLE_BUFFER_SIZE,
					AGS_SOUNDCARD_DOUBLE);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_DOUBLE_BUFFER_SIZE; i++){
    s64_buffer[i] = G_MAXINT64 * sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_s64(s64_buffer,
							  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_DOUBLE_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_s64_to_double(&audio_buffer_util,
					   double_destination, 1,
					   s64_buffer, 1,
					   AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_DOUBLE_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_double(double_destination,
							AGS_AUDIO_BUFFER_UTIL_TEST_COPY_S64_TO_DOUBLE_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_float_to_s8()
{
  gfloat *float_buffer;
  gint8 *s8_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  float_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S8_BUFFER_SIZE,
				  AGS_SOUNDCARD_FLOAT);
  s8_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S8_BUFFER_SIZE,
				    AGS_SOUNDCARD_SIGNED_8_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S8_BUFFER_SIZE; i++){
    float_buffer[i] = sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_float(float_buffer,
							    AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S8_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_float_to_s8(&audio_buffer_util,
					 s8_destination, 1,
					 float_buffer, 1,
					 AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S8_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s8(s8_destination,
						    AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S8_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_float_to_s16()
{
  gfloat *float_buffer;
  gint16 *s16_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  float_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S16_BUFFER_SIZE,
				  AGS_SOUNDCARD_FLOAT);
  s16_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S16_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_16_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S16_BUFFER_SIZE; i++){
    float_buffer[i] = sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_float(float_buffer,
							    AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S16_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_float_to_s16(&audio_buffer_util,
					  s16_destination, 1,
					  float_buffer, 1,
					  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S16_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s16(s16_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S16_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_float_to_s24()
{
  gfloat *float_buffer;
  gint32 *s24_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  float_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S24_BUFFER_SIZE,
				  AGS_SOUNDCARD_FLOAT);
  s24_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S24_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_24_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S24_BUFFER_SIZE; i++){
    float_buffer[i] = sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_float(float_buffer,
							    AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S24_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_float_to_s24(&audio_buffer_util,
					  s24_destination, 1,
					  float_buffer, 1,
					  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S24_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s24(s24_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S24_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_float_to_s32()
{
  gfloat *float_buffer;
  gint32 *s32_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  float_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S32_BUFFER_SIZE,
				  AGS_SOUNDCARD_FLOAT);
  s32_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S32_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_32_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S32_BUFFER_SIZE; i++){
    float_buffer[i] = sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_float(float_buffer,
							    AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S32_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_float_to_s32(&audio_buffer_util,
					  s32_destination, 1,
					  float_buffer, 1,
					  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S32_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s32(s32_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S32_BUFFER_SIZE);
  
  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_float_to_s64()
{
  gfloat *float_buffer;
  gint64 *s64_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  float_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S64_BUFFER_SIZE,
				  AGS_SOUNDCARD_FLOAT);
  s64_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S64_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_64_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S64_BUFFER_SIZE; i++){
    float_buffer[i] = sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_float(float_buffer,
							    AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S64_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_float_to_s64(&audio_buffer_util,
					  s64_destination, 1,
					  float_buffer, 1,
					  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S64_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s64(s64_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_S64_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_float_to_float()
{
  gfloat *float_buffer, *float_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  float_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_FLOAT_BUFFER_SIZE,
				  AGS_SOUNDCARD_FLOAT);
  float_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_FLOAT_BUFFER_SIZE,
				       AGS_SOUNDCARD_FLOAT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_FLOAT_BUFFER_SIZE; i++){
    float_buffer[i] = sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_float(float_buffer,
							    AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_FLOAT_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_float_to_float(&audio_buffer_util,
					    float_destination, 1,
					    float_buffer, 1,
					    AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_FLOAT_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_float(float_destination,
						       AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_FLOAT_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_float_to_double()
{
  gfloat *float_buffer;
  gdouble *double_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  float_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_DOUBLE_BUFFER_SIZE,
				  AGS_SOUNDCARD_FLOAT);
  double_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_DOUBLE_BUFFER_SIZE,
					AGS_SOUNDCARD_DOUBLE);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_DOUBLE_BUFFER_SIZE; i++){
    float_buffer[i] = sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_float(float_buffer,
							    AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_DOUBLE_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_float_to_double(&audio_buffer_util,
					     double_destination, 1,
					     float_buffer, 1,
					     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_DOUBLE_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_double(double_destination,
							AGS_AUDIO_BUFFER_UTIL_TEST_COPY_FLOAT_TO_DOUBLE_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_double_to_s8()
{
  gdouble *double_buffer;
  gint8 *s8_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  double_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S8_BUFFER_SIZE,
				   AGS_SOUNDCARD_DOUBLE);
  s8_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S8_BUFFER_SIZE,
				    AGS_SOUNDCARD_SIGNED_8_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S8_BUFFER_SIZE; i++){
    double_buffer[i] = sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_double(double_buffer,
							     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S8_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_double_to_s8(&audio_buffer_util,
					  s8_destination, 1,
					  double_buffer, 1,
					  AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S8_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s8(s8_destination,
						    AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S8_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_double_to_s16()
{
  gdouble *double_buffer;
  gint16 *s16_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  double_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S16_BUFFER_SIZE,
				   AGS_SOUNDCARD_DOUBLE);
  s16_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S16_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_16_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S16_BUFFER_SIZE; i++){
    double_buffer[i] = sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_double(double_buffer,
							     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S16_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_double_to_s16(&audio_buffer_util,
					   s16_destination, 1,
					   double_buffer, 1,
					   AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S16_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s16(s16_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S16_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_double_to_s24()
{
  gdouble *double_buffer;
  gint32 *s24_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  double_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S24_BUFFER_SIZE,
				   AGS_SOUNDCARD_DOUBLE);
  s24_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S24_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_24_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S24_BUFFER_SIZE; i++){
    double_buffer[i] = sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_double(double_buffer,
							     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S24_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_double_to_s24(&audio_buffer_util,
					   s24_destination, 1,
					   double_buffer, 1,
					   AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S24_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s24(s24_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S24_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_double_to_s32()
{
  gdouble *double_buffer;
  gint32 *s32_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  double_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S32_BUFFER_SIZE,
				   AGS_SOUNDCARD_DOUBLE);
  s32_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S32_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_32_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S32_BUFFER_SIZE; i++){
    double_buffer[i] = sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_double(double_buffer,
							     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S32_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_double_to_s32(&audio_buffer_util,
					   s32_destination, 1,
					   double_buffer, 1,
					   AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S32_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s32(s32_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S32_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_double_to_s64()
{
  gdouble *double_buffer;
  gint64 *s64_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  double_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S64_BUFFER_SIZE,
				   AGS_SOUNDCARD_DOUBLE);
  s64_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S64_BUFFER_SIZE,
				     AGS_SOUNDCARD_SIGNED_64_BIT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S64_BUFFER_SIZE; i++){
    double_buffer[i] = sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_double(double_buffer,
							     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S64_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_double_to_s64(&audio_buffer_util,
					   s64_destination, 1,
					   double_buffer, 1,
					   AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S64_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_s64(s64_destination,
						     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_S64_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_double_to_float()
{
  gdouble *double_buffer;
  gfloat *float_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  double_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_FLOAT_BUFFER_SIZE,
				   AGS_SOUNDCARD_DOUBLE);
  float_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_FLOAT_BUFFER_SIZE,
				       AGS_SOUNDCARD_FLOAT);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_FLOAT_BUFFER_SIZE; i++){
    double_buffer[i] = sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_double(double_buffer,
							     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_FLOAT_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_double_to_float(&audio_buffer_util,
					     float_destination, 1,
					     double_buffer, 1,
					     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_FLOAT_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_float(float_destination,
						       AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_FLOAT_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_double_to_double()
{
  gdouble *double_buffer, *double_destination;

  guint orig_xcross_count, xcross_count;
  guint i;
  
  double_buffer = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_DOUBLE_BUFFER_SIZE,
				   AGS_SOUNDCARD_DOUBLE);
  double_destination = ags_stream_alloc(AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_DOUBLE_BUFFER_SIZE,
					AGS_SOUNDCARD_DOUBLE);
  
  for(i = 0; i < AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_DOUBLE_BUFFER_SIZE; i++){
    double_buffer[i] = sin(i * 2.0 * M_PI * AGS_AUDIO_BUFFER_UTIL_TEST_FREQUENCY / AGS_AUDIO_BUFFER_UTIL_TEST_SAMPLERATE);
  }

  orig_xcross_count = ags_synth_util_get_xcross_count_double(double_buffer,
							     AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_DOUBLE_BUFFER_SIZE);
  
  /* test */
  ags_audio_buffer_util_copy_double_to_double(&audio_buffer_util,
					      double_destination, 1,
					      double_buffer, 1,
					      AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_DOUBLE_BUFFER_SIZE);

  xcross_count = ags_synth_util_get_xcross_count_double(double_destination,
							AGS_AUDIO_BUFFER_UTIL_TEST_COPY_DOUBLE_TO_DOUBLE_BUFFER_SIZE);

  CU_ASSERT(orig_xcross_count == xcross_count);
}

void
ags_audio_buffer_util_test_copy_buffer_to_buffer()
{
  //TODO:JK: implement me
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
  pSuite = CU_add_suite("AgsAudioBufferUtilTest", ags_audio_buffer_util_test_init_suite, ags_audio_buffer_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of ags_audio_buffer_util.c format from soundcard", ags_audio_buffer_util_test_format_from_soundcard) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c get copy mode", ags_audio_buffer_util_test_get_copy_mode) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c clear float", ags_audio_buffer_util_test_clear_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c clear double", ags_audio_buffer_util_test_clear_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c clear buffer", ags_audio_buffer_util_test_clear_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s8 to s8", ags_audio_buffer_util_test_copy_s8_to_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s8 to s16", ags_audio_buffer_util_test_copy_s8_to_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s8 to s24", ags_audio_buffer_util_test_copy_s8_to_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s8 to s32", ags_audio_buffer_util_test_copy_s8_to_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s8 to s64", ags_audio_buffer_util_test_copy_s8_to_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s8 to float", ags_audio_buffer_util_test_copy_s8_to_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s8 to double", ags_audio_buffer_util_test_copy_s8_to_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s16 to s8", ags_audio_buffer_util_test_copy_s16_to_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s16 to s16", ags_audio_buffer_util_test_copy_s16_to_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s16 to s24", ags_audio_buffer_util_test_copy_s16_to_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s16 to s32", ags_audio_buffer_util_test_copy_s16_to_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s16 to s64", ags_audio_buffer_util_test_copy_s16_to_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s16 to float", ags_audio_buffer_util_test_copy_s16_to_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s16 to double", ags_audio_buffer_util_test_copy_s16_to_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s24 to s8", ags_audio_buffer_util_test_copy_s24_to_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s24 to s16", ags_audio_buffer_util_test_copy_s24_to_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s24 to s24", ags_audio_buffer_util_test_copy_s24_to_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s24 to s32", ags_audio_buffer_util_test_copy_s24_to_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s24 to s64", ags_audio_buffer_util_test_copy_s24_to_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s24 to float", ags_audio_buffer_util_test_copy_s24_to_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s24 to double", ags_audio_buffer_util_test_copy_s24_to_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s32 to s8", ags_audio_buffer_util_test_copy_s32_to_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s32 to s16", ags_audio_buffer_util_test_copy_s32_to_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s32 to s24", ags_audio_buffer_util_test_copy_s32_to_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s32 to s32", ags_audio_buffer_util_test_copy_s32_to_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s32 to s64", ags_audio_buffer_util_test_copy_s32_to_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s32 to float", ags_audio_buffer_util_test_copy_s32_to_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s32 to double", ags_audio_buffer_util_test_copy_s32_to_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s64 to s8", ags_audio_buffer_util_test_copy_s64_to_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s64 to s16", ags_audio_buffer_util_test_copy_s64_to_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s64 to s24", ags_audio_buffer_util_test_copy_s64_to_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s64 to s32", ags_audio_buffer_util_test_copy_s64_to_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s64 to s64", ags_audio_buffer_util_test_copy_s64_to_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s64 to float", ags_audio_buffer_util_test_copy_s64_to_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy s64 to double", ags_audio_buffer_util_test_copy_s64_to_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy float to s8", ags_audio_buffer_util_test_copy_float_to_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy float to s16", ags_audio_buffer_util_test_copy_float_to_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy float to s24", ags_audio_buffer_util_test_copy_float_to_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy float to s32", ags_audio_buffer_util_test_copy_float_to_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy float to s64", ags_audio_buffer_util_test_copy_float_to_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy float to float", ags_audio_buffer_util_test_copy_float_to_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy float to double", ags_audio_buffer_util_test_copy_float_to_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy double to s8", ags_audio_buffer_util_test_copy_double_to_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy double to s16", ags_audio_buffer_util_test_copy_double_to_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy double to s24", ags_audio_buffer_util_test_copy_double_to_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy double to s32", ags_audio_buffer_util_test_copy_double_to_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy double to s64", ags_audio_buffer_util_test_copy_double_to_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy double to float", ags_audio_buffer_util_test_copy_double_to_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy double to double", ags_audio_buffer_util_test_copy_double_to_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_audio_buffer_util.c copy buffer to buffer", ags_audio_buffer_util_test_copy_buffer_to_buffer) == NULL)){
    CU_cleanup_registry();
      
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}

