/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2018 Joël Krähemann
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

#include <ags/libags.h>

#include <glib.h>
#include <glib-object.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include <math.h>

int ags_buffer_util_test_init_suite();
int ags_buffer_util_test_clean_suite();

void ags_buffer_util_test_s8_to_char_buffer();
void ags_buffer_util_test_s16_to_char_buffer();
void ags_buffer_util_test_s24_to_char_buffer();
void ags_buffer_util_test_s32_to_char_buffer();
void ags_buffer_util_test_s64_to_char_buffer();
void ags_buffer_util_test_float_to_char_buffer();
void ags_buffer_util_test_double_to_char_buffer();
void ags_buffer_util_test_char_buffer_to_s8();
void ags_buffer_util_test_char_buffer_to_s16();
void ags_buffer_util_test_char_buffer_to_s24();
void ags_buffer_util_test_char_buffer_to_s32();
void ags_buffer_util_test_char_buffer_to_s64();
void ags_buffer_util_test_char_buffer_to_float();
void ags_buffer_util_test_char_buffer_to_double();
void ags_buffer_util_test_read_s8();
void ags_buffer_util_test_read_s16();
void ags_buffer_util_test_read_s24();
void ags_buffer_util_test_read_s32();
void ags_buffer_util_test_read_s64();
void ags_buffer_util_test_read_float();
void ags_buffer_util_test_read_double();
void ags_buffer_util_test_write_s8();
void ags_buffer_util_test_write_s16();
void ags_buffer_util_test_write_s24();
void ags_buffer_util_test_write_s32();
void ags_buffer_util_test_write_s64();
void ags_buffer_util_test_write_float();
void ags_buffer_util_test_write_double();
void ags_buffer_util_test_swap_bytes();

#define AGS_BUFFER_UTIL_TEST_MAX_S24 (0x7fffff)

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_buffer_util_test_init_suite()
{
  return(0);
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int
ags_buffer_util_test_clean_suite()
{
  return(0);
}

void
ags_buffer_util_test_s8_to_char_buffer()
{
  gint8 source[] = {0x80, 0x7f, 0xff, 0x01, 0x00, 0xa0, 0xb0, 0xde, 0xad, 0xbe, 0xef, 0x10, 0xaf, 0xfe, 0x00, 0x00};
  guchar *retval;

  retval = ags_buffer_util_s8_to_char_buffer(source,
					     16);

  CU_ASSERT(retval != NULL);
}

void
ags_buffer_util_test_s16_to_char_buffer()
{
  gint16 source[] = {
    G_MAXINT16,
    G_MAXINT16 * (7.0 / 8.0),
    G_MAXINT16 * (6.0 / 8.0),
    G_MAXINT16 * (5.0 / 8.0),
    G_MAXINT16 * (4.0 / 8.0),
    G_MAXINT16 * (3.0 / 8.0),
    G_MAXINT16 * (2.0 / 8.0),
    G_MAXINT16 * (1.0 / 8.0),
    0,
    -1 * G_MAXINT16 * (1.0 / 8.0),
    -1 * G_MAXINT16 * (2.0 / 8.0),
    -1 * G_MAXINT16 * (3.0 / 8.0),
    -1 * G_MAXINT16 * (4.0 / 8.0),
    -1 * G_MAXINT16 * (5.0 / 8.0),
    -1 * G_MAXINT16 * (6.0 / 8.0),
    -1 * G_MAXINT16 * (7.0 / 8.0),
    -1 * G_MAXINT16,
  };
  guchar *retval;

  retval = ags_buffer_util_s16_to_char_buffer(source,
					      17);

  CU_ASSERT(retval != NULL);
}

void
ags_buffer_util_test_s24_to_char_buffer()
{
  gint32 source[] = {
    AGS_BUFFER_UTIL_TEST_MAX_S24,
    AGS_BUFFER_UTIL_TEST_MAX_S24 * (7.0 / 8.0),
    AGS_BUFFER_UTIL_TEST_MAX_S24 * (6.0 / 8.0),
    AGS_BUFFER_UTIL_TEST_MAX_S24 * (5.0 / 8.0),
    AGS_BUFFER_UTIL_TEST_MAX_S24 * (4.0 / 8.0),
    AGS_BUFFER_UTIL_TEST_MAX_S24 * (3.0 / 8.0),
    AGS_BUFFER_UTIL_TEST_MAX_S24 * (2.0 / 8.0),
    AGS_BUFFER_UTIL_TEST_MAX_S24 * (1.0 / 8.0),
    0,
    -1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (1.0 / 8.0),
    -1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (2.0 / 8.0),
    -1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (3.0 / 8.0),
    -1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (4.0 / 8.0),
    -1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (5.0 / 8.0),
    -1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (6.0 / 8.0),
    -1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (7.0 / 8.0),
    -1 * AGS_BUFFER_UTIL_TEST_MAX_S24,
  };
  guchar *retval;

  retval = ags_buffer_util_s24_to_char_buffer(source,
					      17);

  CU_ASSERT(retval != NULL);
}

void
ags_buffer_util_test_s32_to_char_buffer()
{
  gint32 source[] = {
    G_MAXINT32,
    G_MAXINT32 * (7.0 / 8.0),
    G_MAXINT32 * (6.0 / 8.0),
    G_MAXINT32 * (5.0 / 8.0),
    G_MAXINT32 * (4.0 / 8.0),
    G_MAXINT32 * (3.0 / 8.0),
    G_MAXINT32 * (2.0 / 8.0),
    G_MAXINT32 * (1.0 / 8.0),
    0,
    -1 * G_MAXINT32 * (1.0 / 8.0),
    -1 * G_MAXINT32 * (2.0 / 8.0),
    -1 * G_MAXINT32 * (3.0 / 8.0),
    -1 * G_MAXINT32 * (4.0 / 8.0),
    -1 * G_MAXINT32 * (5.0 / 8.0),
    -1 * G_MAXINT32 * (6.0 / 8.0),
    -1 * G_MAXINT32 * (7.0 / 8.0),
    -1 * G_MAXINT32,
  };
  guchar *retval;

  retval = ags_buffer_util_s32_to_char_buffer(source,
					      17);

  CU_ASSERT(retval != NULL);
}

void
ags_buffer_util_test_s64_to_char_buffer()
{
  gint64 source[] = {
    G_MAXINT64,
    G_MAXINT64 * (7.0 / 8.0),
    G_MAXINT64 * (6.0 / 8.0),
    G_MAXINT64 * (5.0 / 8.0),
    G_MAXINT64 * (4.0 / 8.0),
    G_MAXINT64 * (3.0 / 8.0),
    G_MAXINT64 * (2.0 / 8.0),
    G_MAXINT64 * (1.0 / 8.0),
    0,
    -1 * G_MAXINT64 * (1.0 / 8.0),
    -1 * G_MAXINT64 * (2.0 / 8.0),
    -1 * G_MAXINT64 * (3.0 / 8.0),
    -1 * G_MAXINT64 * (4.0 / 8.0),
    -1 * G_MAXINT64 * (5.0 / 8.0),
    -1 * G_MAXINT64 * (6.0 / 8.0),
    -1 * G_MAXINT64 * (7.0 / 8.0),
    -1 * G_MAXINT64,
  };
  guchar *retval;

  retval = ags_buffer_util_s64_to_char_buffer(source,
					      17);

  CU_ASSERT(retval != NULL);
}

void
ags_buffer_util_test_float_to_char_buffer()
{
  gfloat source[] = {
    1.0,
    1.0 * (7.0 / 8.0),
    1.0 * (6.0 / 8.0),
    1.0 * (5.0 / 8.0),
    1.0 * (4.0 / 8.0),
    1.0 * (3.0 / 8.0),
    1.0 * (2.0 / 8.0),
    1.0 * (1.0 / 8.0),
    0.0,
    -1.0 * (1.0 / 8.0),
    -1.0 * (2.0 / 8.0),
    -1.0 * (3.0 / 8.0),
    -1.0 * (4.0 / 8.0),
    -1.0 * (5.0 / 8.0),
    -1.0 * (6.0 / 8.0),
    -1.0 * (7.0 / 8.0),
    -1.0,
  };
  guchar *retval;

  retval = ags_buffer_util_float_to_char_buffer(source,
						17);

  CU_ASSERT(retval != NULL);
}

void
ags_buffer_util_test_double_to_char_buffer()
{
  gdouble source[] = {
    1.0,
    1.0 * (7.0 / 8.0),
    1.0 * (6.0 / 8.0),
    1.0 * (5.0 / 8.0),
    1.0 * (4.0 / 8.0),
    1.0 * (3.0 / 8.0),
    1.0 * (2.0 / 8.0),
    1.0 * (1.0 / 8.0),
    0.0,
    -1.0 * (1.0 / 8.0),
    -1.0 * (2.0 / 8.0),
    -1.0 * (3.0 / 8.0),
    -1.0 * (4.0 / 8.0),
    -1.0 * (5.0 / 8.0),
    -1.0 * (6.0 / 8.0),
    -1.0 * (7.0 / 8.0),
    -1.0,
  };
  guchar *retval;

  retval = ags_buffer_util_double_to_char_buffer(source,
						 17);

  CU_ASSERT(retval != NULL);
}

void
ags_buffer_util_test_char_buffer_to_s8()
{
  guchar source[] = {0x80, 0x7f, 0xff, 0x01, 0x00, 0xa0, 0xb0, 0xde, 0xad, 0xbe, 0xef, 0x10, 0xaf, 0xfe, 0x00, 0x00};
  gint8 *retval;
  
  retval = ags_buffer_util_char_buffer_to_s8(source,
					     16 * sizeof(guchar));

  CU_ASSERT(retval != NULL);
}

void
ags_buffer_util_test_char_buffer_to_s16()
{
  guchar source[] = {
    0xff & (gint16) (G_MAXINT16), (0xff00 & (gint16) (G_MAXINT16)) >> 8,
    0xff & (gint16) (G_MAXINT16 * (7.0 / 8.0)), (0xff00 & (gint16) (G_MAXINT16 * (7.0 / 8.0))) >> 8,
    0xff & (gint16) (G_MAXINT16 * (6.0 / 8.0)), (0xff00 & (gint16) (G_MAXINT16 * (6.0 / 8.0))) >> 8,
    0xff & (gint16) (G_MAXINT16 * (5.0 / 8.0)), (0xff00 & (gint16) (G_MAXINT16 * (5.0 / 8.0))) >> 8,
    0xff & (gint16) (G_MAXINT16 * (4.0 / 8.0)), (0xff00 & (gint16) (G_MAXINT16 * (4.0 / 8.0))) >> 8,
    0xff & (gint16) (G_MAXINT16 * (3.0 / 8.0)), (0xff00 & (gint16) (G_MAXINT16 * (3.0 / 8.0))) >> 8,
    0xff & (gint16) (G_MAXINT16 * (2.0 / 8.0)), (0xff00 & (gint16) (G_MAXINT16 * (2.0 / 8.0))) >> 8,
    0xff & (gint16) (G_MAXINT16 * (1.0 / 8.0)), (0xff00 & (gint16) (G_MAXINT16 * (1.0 / 8.0))) >> 8,
    0, 0,
    0xff & (gint16) (-1 * G_MAXINT16 * (1.0 / 8.0)), (0xff00 & (gint16) (-1 * G_MAXINT16 * (1.0 / 8.0))) >> 8,
    0xff & (gint16) (-1 * G_MAXINT16 * (2.0 / 8.0)), (0xff00 & (gint16) (-1 * G_MAXINT16 * (2.0 / 8.0))) >> 8,
    0xff & (gint16) (-1 * G_MAXINT16 * (3.0 / 8.0)), (0xff00 & (gint16) (-1 * G_MAXINT16 * (3.0 / 8.0))) >> 8,
    0xff & (gint16) (-1 * G_MAXINT16 * (4.0 / 8.0)), (0xff00 & (gint16) (-1 * G_MAXINT16 * (4.0 / 8.0))) >> 8,
    0xff & (gint16) (-1 * G_MAXINT16 * (5.0 / 8.0)), (0xff00 & (gint16) (-1 * G_MAXINT16 * (5.0 / 8.0))) >> 8,
    0xff & (gint16) (-1 * G_MAXINT16 * (6.0 / 8.0)), (0xff00 & (gint16) (-1 * G_MAXINT16 * (6.0 / 8.0))) >> 8,
    0xff & (gint16) (-1 * G_MAXINT16 * (7.0 / 8.0)), (0xff00 & (gint16) (-1 * G_MAXINT16 * (7.0 / 8.0))) >> 8,
    0xff & (gint16) (-1 * G_MAXINT16), (0xff00 & (gint16) (-1 * G_MAXINT16)) >> 8,
  };
  gint16 *retval;

  retval = ags_buffer_util_char_buffer_to_s16(source,
					      (2 * 17) * sizeof(guchar));

  CU_ASSERT(retval != NULL);
}

void
ags_buffer_util_test_char_buffer_to_s24()
{
  guchar source[] = {
    0xff & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24), (0xff00 & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24)) >> 8, (0xff0000 & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24)) >> 16, 0,
    0xff & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24 * (7.0 / 8.0)), (0xff00 & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24 * (7.0 / 8.0))) >> 8, (0xff0000 & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24 * (7.0 / 8.0))) >> 16, 0,
    0xff & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24 * (6.0 / 8.0)), (0xff00 & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24 * (6.0 / 8.0))) >> 8, (0xff0000 & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24 * (6.0 / 8.0))) >> 16, 0,
    0xff & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24 * (5.0 / 8.0)), (0xff00 & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24 * (5.0 / 8.0))) >> 8, (0xff0000 & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24 * (5.0 / 8.0))) >> 16, 0,
    0xff & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24 * (4.0 / 8.0)), (0xff00 & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24 * (4.0 / 8.0))) >> 8, (0xff0000 & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24 * (4.0 / 8.0))) >> 16, 0,
    0xff & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24 * (3.0 / 8.0)), (0xff00 & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24 * (3.0 / 8.0))) >> 8, (0xff0000 & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24 * (3.0 / 8.0))) >> 16, 0,
    0xff & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24 * (2.0 / 8.0)), (0xff00 & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24 * (2.0 / 8.0))) >> 8, (0xff0000 & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24 * (2.0 / 8.0))) >> 16, 0,
    0xff & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24 * (1.0 / 8.0)), (0xff00 & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24 * (1.0 / 8.0))) >> 8, (0xff0000 & (gint32) (AGS_BUFFER_UTIL_TEST_MAX_S24 * (1.0 / 8.0))) >> 16, 0,
    0, 0, 0, 0,
    0xff & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (1.0 / 8.0)), (0xff00 & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (1.0 / 8.0))) >> 8, (0xff0000 & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (1.0 / 8.0))) >> 16, 0,
    0xff & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (2.0 / 8.0)), (0xff00 & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (2.0 / 8.0))) >> 8, (0xff0000 & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (2.0 / 8.0))) >> 16, 0,
    0xff & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (3.0 / 8.0)), (0xff00 & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (3.0 / 8.0))) >> 8, (0xff0000 & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (3.0 / 8.0))) >> 16, 0,
    0xff & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (4.0 / 8.0)), (0xff00 & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (4.0 / 8.0))) >> 8, (0xff0000 & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (4.0 / 8.0))) >> 16, 0,
    0xff & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (5.0 / 8.0)), (0xff00 & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (5.0 / 8.0))) >> 8, (0xff0000 & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (5.0 / 8.0))) >> 16, 0,
    0xff & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (6.0 / 8.0)), (0xff00 & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (6.0 / 8.0))) >> 8, (0xff0000 & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (6.0 / 8.0))) >> 16, 0,
    0xff & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (7.0 / 8.0)), (0xff00 & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (7.0 / 8.0))) >> 8, (0xff0000 & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24 * (7.0 / 8.0))) >> 16, 0,
    0xff & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24), (0xff00 & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24)) >> 8, (0xff0000 & (gint32) (-1 * AGS_BUFFER_UTIL_TEST_MAX_S24)) >> 16, 0,
  };
  gint32 *retval;

  retval = ags_buffer_util_char_buffer_to_s24(source,
					      (4 * 17) * sizeof(guchar));

  CU_ASSERT(retval != NULL);
}

void
ags_buffer_util_test_char_buffer_to_s32()
{
  guchar source[] = {
    0xff & (gint32) (G_MAXINT32), (0xff00 & (gint32) (G_MAXINT32)) >> 8, (0xff0000 & (gint32) (G_MAXINT32)) >> 16, (0xff000000 & (gint32) (G_MAXINT32)) >> 24,
    0xff & (gint32) (G_MAXINT32 * (7.0 / 8.0)), (0xff00 & (gint32) (G_MAXINT32 * (7.0 / 8.0))) >> 8, (0xff0000 & (gint32) (G_MAXINT32 * (7.0 / 8.0))) >> 16, (0xff000000 & (gint32) (G_MAXINT32 * (7.0 / 8.0))) >> 24,
    0xff & (gint32) (G_MAXINT32 * (6.0 / 8.0)), (0xff00 & (gint32) (G_MAXINT32 * (6.0 / 8.0))) >> 8, (0xff0000 & (gint32) (G_MAXINT32 * (6.0 / 8.0))) >> 16, (0xff000000 & (gint32) (G_MAXINT32 * (6.0 / 8.0))) >> 24,
    0xff & (gint32) (G_MAXINT32 * (5.0 / 8.0)), (0xff00 & (gint32) (G_MAXINT32 * (5.0 / 8.0))) >> 8, (0xff0000 & (gint32) (G_MAXINT32 * (5.0 / 8.0))) >> 16, (0xff000000 & (gint32) (G_MAXINT32 * (5.0 / 8.0))) >> 24,
    0xff & (gint32) (G_MAXINT32 * (4.0 / 8.0)), (0xff00 & (gint32) (G_MAXINT32 * (4.0 / 8.0))) >> 8, (0xff0000 & (gint32) (G_MAXINT32 * (4.0 / 8.0))) >> 16, (0xff000000 & (gint32) (G_MAXINT32 * (4.0 / 8.0))) >> 24,
    0xff & (gint32) (G_MAXINT32 * (3.0 / 8.0)), (0xff00 & (gint32) (G_MAXINT32 * (3.0 / 8.0))) >> 8, (0xff0000 & (gint32) (G_MAXINT32 * (3.0 / 8.0))) >> 16, (0xff000000 & (gint32) (G_MAXINT32 * (3.0 / 8.0))) >> 24,
    0xff & (gint32) (G_MAXINT32 * (2.0 / 8.0)), (0xff00 & (gint32) (G_MAXINT32 * (2.0 / 8.0))) >> 8, (0xff0000 & (gint32) (G_MAXINT32 * (2.0 / 8.0))) >> 16, (0xff000000 & (gint32) (G_MAXINT32 * (2.0 / 8.0))) >> 24,
    0xff & (gint32) (G_MAXINT32 * (1.0 / 8.0)), (0xff00 & (gint32) (G_MAXINT32 * (1.0 / 8.0))) >> 8, (0xff0000 & (gint32) (G_MAXINT32 * (1.0 / 8.0))) >> 16, (0xff000000 & (gint32) (G_MAXINT32 * (1.0 / 8.0))) >> 24,
    0, 0, 0, 0,
    0xff & (gint32) (-1 * G_MAXINT32 * (1.0 / 8.0)), (0xff00 & (gint32) (-1 * G_MAXINT32 * (1.0 / 8.0))) >> 8, (0xff0000 & (gint32) (-1 * G_MAXINT32 * (1.0 / 8.0))) >> 16, (0xff000000 & (gint32) (-1 * G_MAXINT32 * (1.0 / 8.0))) >> 24,
    0xff & (gint32) (-1 * G_MAXINT32 * (2.0 / 8.0)), (0xff00 & (gint32) (-1 * G_MAXINT32 * (2.0 / 8.0))) >> 8, (0xff0000 & (gint32) (-1 * G_MAXINT32 * (2.0 / 8.0))) >> 16, (0xff000000 & (gint32) (-1 * G_MAXINT32 * (2.0 / 8.0))) >> 24,
    0xff & (gint32) (-1 * G_MAXINT32 * (3.0 / 8.0)), (0xff00 & (gint32) (-1 * G_MAXINT32 * (3.0 / 8.0))) >> 8, (0xff0000 & (gint32) (-1 * G_MAXINT32 * (3.0 / 8.0))) >> 16, (0xff000000 & (gint32) (-1 * G_MAXINT32 * (3.0 / 8.0))) >> 24,
    0xff & (gint32) (-1 * G_MAXINT32 * (4.0 / 8.0)), (0xff00 & (gint32) (-1 * G_MAXINT32 * (4.0 / 8.0))) >> 8, (0xff0000 & (gint32) (-1 * G_MAXINT32 * (4.0 / 8.0))) >> 16, (0xff000000 & (gint32) (-1 * G_MAXINT32 * (4.0 / 8.0))) >> 24,
    0xff & (gint32) (-1 * G_MAXINT32 * (5.0 / 8.0)), (0xff00 & (gint32) (-1 * G_MAXINT32 * (5.0 / 8.0))) >> 8, (0xff0000 & (gint32) (-1 * G_MAXINT32 * (5.0 / 8.0))) >> 16, (0xff000000 & (gint32) (-1 * G_MAXINT32 * (5.0 / 8.0))) >> 24,
    0xff & (gint32) (-1 * G_MAXINT32 * (6.0 / 8.0)), (0xff00 & (gint32) (-1 * G_MAXINT32 * (6.0 / 8.0))) >> 8, (0xff0000 & (gint32) (-1 * G_MAXINT32 * (6.0 / 8.0))) >> 16, (0xff000000 & (gint32) (-1 * G_MAXINT32 * (6.0 / 8.0))) >> 24,
    0xff & (gint32) (-1 * G_MAXINT32 * (7.0 / 8.0)), (0xff00 & (gint32) (-1 * G_MAXINT32 * (7.0 / 8.0))) >> 8, (0xff0000 & (gint32) (-1 * G_MAXINT32 * (7.0 / 8.0))) >> 16, (0xff000000 & (gint32) (-1 * G_MAXINT32 * (7.0 / 8.0))) >> 24,
    0xff & (gint32) (-1 * G_MAXINT32), (0xff00 & (gint32) (-1 * G_MAXINT32)) >> 8, (0xff0000 & (gint32) (-1 * G_MAXINT32)) >> 16, (0xff000000 & (gint32) (-1 * G_MAXINT32)) >> 24,
  };
  gint32 *retval;

  retval = ags_buffer_util_char_buffer_to_s32(source,
					      (4 * 17) * sizeof(guchar));

  CU_ASSERT(retval != NULL);
}

void
ags_buffer_util_test_char_buffer_to_s64()
{
  guchar source[] = {
    0xff & (gint64) (G_MAXINT64), (0xff00 & (gint64) (G_MAXINT64)) >> 8, (0xff0000 & (gint64) (G_MAXINT64)) >> 16, (0xff000000 & (gint64) (G_MAXINT64)) >> 24, (0xff00000000 & (gint64) (G_MAXINT64)) >> 32, (0xff0000000000 & (gint64) (G_MAXINT64)) >> 40, (0xff000000000000 & (gint64) (G_MAXINT64)) >> 48, (0xff00000000000000 & (gint64) (G_MAXINT64)) >> 56,
    0xff & (gint64) (G_MAXINT64 * (7.0 / 8.0)), (0xff00 & (gint64) (G_MAXINT64 * (7.0 / 8.0))) >> 8, (0xff0000 & (gint64) (G_MAXINT64 * (7.0 / 8.0))) >> 16, (0xff000000 & (gint64) (G_MAXINT64 * (7.0 / 8.0))) >> 24, (0xff00000000 & (gint64) (G_MAXINT64 * (7.0 / 8.0))) >> 32, (0xff0000000000 & (gint64) (G_MAXINT64 * (7.0 / 8.0))) >> 40, (0xff000000000000 & (gint64) (G_MAXINT64 * (7.0 / 8.0))) >> 48, (0xff00000000000000 & (gint64) (G_MAXINT64 * (7.0 / 8.0))) >> 56,
    0xff & (gint64) (G_MAXINT64 * (6.0 / 8.0)), (0xff00 & (gint64) (G_MAXINT64 * (6.0 / 8.0))) >> 8, (0xff0000 & (gint64) (G_MAXINT64 * (6.0 / 8.0))) >> 16, (0xff000000 & (gint64) (G_MAXINT64 * (6.0 / 8.0))) >> 24, (0xff00000000 & (gint64) (G_MAXINT64 * (6.0 / 8.0))) >> 32, (0xff0000000000 & (gint64) (G_MAXINT64 * (6.0 / 8.0))) >> 40, (0xff000000000000 & (gint64) (G_MAXINT64 * (6.0 / 8.0))) >> 48, (0xff00000000000000 & (gint64) (G_MAXINT64 * (6.0 / 8.0))) >> 56,
    0xff & (gint64) (G_MAXINT64 * (5.0 / 8.0)), (0xff00 & (gint64) (G_MAXINT64 * (5.0 / 8.0))) >> 8, (0xff0000 & (gint64) (G_MAXINT64 * (5.0 / 8.0))) >> 16, (0xff000000 & (gint64) (G_MAXINT64 * (5.0 / 8.0))) >> 24, (0xff00000000 & (gint64) (G_MAXINT64 * (5.0 / 8.0))) >> 32, (0xff0000000000 & (gint64) (G_MAXINT64 * (5.0 / 8.0))) >> 40, (0xff000000000000 & (gint64) (G_MAXINT64 * (5.0 / 8.0))) >> 48, (0xff00000000000000 & (gint64) (G_MAXINT64 * (5.0 / 8.0))) >> 56,
    0xff & (gint64) (G_MAXINT64 * (4.0 / 8.0)), (0xff00 & (gint64) (G_MAXINT64 * (4.0 / 8.0))) >> 8, (0xff0000 & (gint64) (G_MAXINT64 * (4.0 / 8.0))) >> 16, (0xff000000 & (gint64) (G_MAXINT64 * (4.0 / 8.0))) >> 24, (0xff00000000 & (gint64) (G_MAXINT64 * (4.0 / 8.0))) >> 32, (0xff0000000000 & (gint64) (G_MAXINT64 * (4.0 / 8.0))) >> 40, (0xff000000000000 & (gint64) (G_MAXINT64 * (4.0 / 8.0))) >> 48, (0xff00000000000000 & (gint64) (G_MAXINT64 * (4.0 / 8.0))) >> 56,
    0xff & (gint64) (G_MAXINT64 * (3.0 / 8.0)), (0xff00 & (gint64) (G_MAXINT64 * (3.0 / 8.0))) >> 8, (0xff0000 & (gint64) (G_MAXINT64 * (3.0 / 8.0))) >> 16, (0xff000000 & (gint64) (G_MAXINT64 * (3.0 / 8.0))) >> 24, (0xff00000000 & (gint64) (G_MAXINT64 * (3.0 / 8.0))) >> 32, (0xff0000000000 & (gint64) (G_MAXINT64 * (3.0 / 8.0))) >> 40, (0xff000000000000 & (gint64) (G_MAXINT64 * (3.0 / 8.0))) >> 48, (0xff00000000000000 & (gint64) (G_MAXINT64 * (3.0 / 8.0))) >> 56,
    0xff & (gint64) (G_MAXINT64 * (2.0 / 8.0)), (0xff00 & (gint64) (G_MAXINT64 * (2.0 / 8.0))) >> 8, (0xff0000 & (gint64) (G_MAXINT64 * (2.0 / 8.0))) >> 16, (0xff000000 & (gint64) (G_MAXINT64 * (2.0 / 8.0))) >> 24, (0xff00000000 & (gint64) (G_MAXINT64 * (2.0 / 8.0))) >> 32, (0xff0000000000 & (gint64) (G_MAXINT64 * (2.0 / 8.0))) >> 40, (0xff000000000000 & (gint64) (G_MAXINT64 * (2.0 / 8.0))) >> 48, (0xff00000000000000 & (gint64) (G_MAXINT64 * (2.0 / 8.0))) >> 56,
    0xff & (gint64) (G_MAXINT64 * (1.0 / 8.0)), (0xff00 & (gint64) (G_MAXINT64 * (1.0 / 8.0))) >> 8, (0xff0000 & (gint64) (G_MAXINT64 * (1.0 / 8.0))) >> 16, (0xff000000 & (gint64) (G_MAXINT64 * (1.0 / 8.0))) >> 24, (0xff00000000 & (gint64) (G_MAXINT64 * (1.0 / 8.0))) >> 32, (0xff0000000000 & (gint64) (G_MAXINT64 * (1.0 / 8.0))) >> 40, (0xff000000000000 & (gint64) (G_MAXINT64 * (1.0 / 8.0))) >> 48, (0xff00000000000000 & (gint64) (G_MAXINT64 * (1.0 / 8.0))) >> 56,
    0, 0, 0, 0, 0, 0, 0, 0,
    0xff & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0)), (0xff00 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 8, (0xff0000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 16, (0xff000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 24, (0xff00000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 32, (0xff0000000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 40, (0xff000000000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 48, (0xff00000000000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 56,
    0xff & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0)), (0xff00 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 8, (0xff0000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 16, (0xff000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 24, (0xff00000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 32, (0xff0000000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 40, (0xff000000000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 48, (0xff00000000000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 56,
    0xff & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0)), (0xff00 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 8, (0xff0000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 16, (0xff000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 24, (0xff00000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 32, (0xff0000000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 40, (0xff000000000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 48, (0xff00000000000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 56,
    0xff & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0)), (0xff00 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 8, (0xff0000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 16, (0xff000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 24, (0xff00000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 32, (0xff0000000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 40, (0xff000000000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 48, (0xff00000000000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 56,
    0xff & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0)), (0xff00 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 8, (0xff0000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 16, (0xff000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 24, (0xff00000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 32, (0xff0000000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 40, (0xff000000000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 48, (0xff00000000000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 56,
    0xff & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0)), (0xff00 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 8, (0xff0000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 16, (0xff000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 24, (0xff00000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 32, (0xff0000000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 40, (0xff000000000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 48, (0xff00000000000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 56,
    0xff & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0)), (0xff00 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 8, (0xff0000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 16, (0xff000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 24, (0xff00000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 32, (0xff0000000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 40, (0xff000000000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 48, (0xff00000000000000 & (gint64) (-1 * G_MAXINT64 * (1.0 / 8.0))) >> 56,
    0xff & (gint64) (-1 * G_MAXINT64), (0xff00 & (gint64) (-1 * G_MAXINT64)) >> 8, (0xff0000 & (gint64) (-1 * G_MAXINT64)) >> 16, (0xff000000 & (gint64) (-1 * G_MAXINT64)) >> 24, (0xff00000000 & (gint64) (-1 * G_MAXINT64)) >> 32, (0xff0000000000 & (gint64) (-1 * G_MAXINT64)) >> 40, (0xff000000000000 & (gint64) (-1 * G_MAXINT64)) >> 48,
  };
  gint64 *retval;

  retval = ags_buffer_util_char_buffer_to_s64(source,
					      (8 * 17) * sizeof(guchar));

  CU_ASSERT(retval != NULL);
}

void
ags_buffer_util_test_char_buffer_to_float()
{
  guchar source[17 * sizeof(guint32)];
  union{
    guint32 val;
    GFloatIEEE754 ieee_float;
  }data;
  gfloat *retval;
  guint i;

  for(i = 0; i < 8; i++){
    data.ieee_float.v_float = 1.0 * ((8 - i) / 8.0);

    source[i * sizeof(guint32)] = 0xff & (data.val);
    source[i * sizeof(guint32) + 1] = (0xff00 & (data.val)) >> 8;
    source[i * sizeof(guint32) + 2] = (0xff0000 & (data.val)) >> 16;
    source[i * sizeof(guint32) + 3] = (0xff000000 & (data.val)) >> 24;
  }

  data.ieee_float.v_float = 0.0;

  source[i * sizeof(guint32)] = 0xff & (data.val);
  source[i * sizeof(guint32) + 1] = (0xff00 & (data.val)) >> 8;
  source[i * sizeof(guint32) + 2] = (0xff0000 & (data.val)) >> 16;
  source[i * sizeof(guint32) + 3] = (0xff000000 & (data.val)) >> 24;
  
  for(i = 0; i < 8; i++){
    data.ieee_float.v_float = -1.0 * (i / 8.0);

    source[(i + 9) * sizeof(guint32)] = 0xff & (data.val);
    source[(i + 9) * sizeof(guint32) + 1] = (0xff00 & (data.val)) >> 8;
    source[(i + 9) * sizeof(guint32) + 2] = (0xff0000 & (data.val)) >> 16;
    source[(i + 9) * sizeof(guint32) + 3] = (0xff000000 & (data.val)) >> 24;
  }

  retval = ags_buffer_util_char_buffer_to_float(source,
						(17 * sizeof(guint32)) * sizeof(guchar));

  CU_ASSERT(retval != NULL);
}

void
ags_buffer_util_test_char_buffer_to_double()
{
  guchar source[17 * sizeof(guint64)];
  union{
    guint64 val;
    GDoubleIEEE754 ieee_double;
  }data;
  gdouble *retval;
  guint i;

  for(i = 0; i < 8; i++){
    data.ieee_double.v_double = 1.0 * ((8 - i) / 8.0);

    source[i * sizeof(guint64)] = 0xff & (data.val);
    source[i * sizeof(guint64) + 1] = (0xff00 & (data.val)) >> 8;
    source[i * sizeof(guint64) + 2] = (0xff0000 & (data.val)) >> 16;
    source[i * sizeof(guint64) + 3] = (0xff000000 & (data.val)) >> 24;
    source[i * sizeof(guint64) + 4] = (0xff000000 & (data.val)) >> 24;
    source[i * sizeof(guint64) + 5] = (0xff000000 & (data.val)) >> 24;
    source[i * sizeof(guint64) + 6] = (0xff000000 & (data.val)) >> 24;
    source[i * sizeof(guint64) + 7] = (0xff000000 & (data.val)) >> 24;
  }

  data.ieee_double.v_double = 0.0;

  source[i * sizeof(guint64)] = 0xff & (data.val);
  source[i * sizeof(guint64) + 1] = (0xff00 & (data.val)) >> 8;
  source[i * sizeof(guint64) + 2] = (0xff0000 & (data.val)) >> 16;
  source[i * sizeof(guint64) + 3] = (0xff000000 & (data.val)) >> 24;
  source[i * sizeof(guint64) + 4] = (0xff000000 & (data.val)) >> 24;
  source[i * sizeof(guint64) + 5] = (0xff000000 & (data.val)) >> 24;
  source[i * sizeof(guint64) + 6] = (0xff000000 & (data.val)) >> 24;
  source[i * sizeof(guint64) + 7] = (0xff000000 & (data.val)) >> 24;
  
  for(i = 0; i < 8; i++){
    data.ieee_double.v_double = -1.0 * (i / 8.0);

    source[(i + 9) * sizeof(guint64)] = 0xff & (data.val);
    source[(i + 9) * sizeof(guint64) + 1] = (0xff00 & (data.val)) >> 8;
    source[(i + 9) * sizeof(guint64) + 2] = (0xff0000 & (data.val)) >> 16;
    source[(i + 9) * sizeof(guint64) + 3] = (0xff000000 & (data.val)) >> 24;
    source[(i + 9) * sizeof(guint64) + 4] = (0xff000000 & (data.val)) >> 24;
    source[(i + 9) * sizeof(guint64) + 5] = (0xff000000 & (data.val)) >> 24;
    source[(i + 9) * sizeof(guint64) + 6] = (0xff000000 & (data.val)) >> 24;
    source[(i + 9) * sizeof(guint64) + 7] = (0xff000000 & (data.val)) >> 24;
  }

  retval = ags_buffer_util_char_buffer_to_double(source,
						 (17 * sizeof(guint64)) * sizeof(guchar));

  CU_ASSERT(retval != NULL);
}

void
ags_buffer_util_test_read_s8()
{
  //TODO:JK: implement me
}

void
ags_buffer_util_test_read_s16()
{
  //TODO:JK: implement me
}

void
ags_buffer_util_test_read_s24()
{
  //TODO:JK: implement me
}

void
ags_buffer_util_test_read_s32()
{
  //TODO:JK: implement me
}

void
ags_buffer_util_test_read_s64()
{
  //TODO:JK: implement me
}

void
ags_buffer_util_test_read_float()
{
  //TODO:JK: implement me
}

void
ags_buffer_util_test_read_double()
{
  //TODO:JK: implement me
}

void
ags_buffer_util_test_write_s8()
{
  //TODO:JK: implement me
}

void
ags_buffer_util_test_write_s16()
{
  //TODO:JK: implement me
}

void
ags_buffer_util_test_write_s24()
{
  //TODO:JK: implement me
}

void
ags_buffer_util_test_write_s32()
{
  //TODO:JK: implement me
}

void
ags_buffer_util_test_write_s64()
{
  //TODO:JK: implement me
}

void
ags_buffer_util_test_write_float()
{
  //TODO:JK: implement me
}

void
ags_buffer_util_test_write_double()
{
  //TODO:JK: implement me
}

void
ags_buffer_util_test_swap_bytes()
{
  //TODO:JK: implement me
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
  pSuite = CU_add_suite("AgsBufferUtilTest", ags_buffer_util_test_init_suite, ags_buffer_util_test_clean_suite);
  
  if(pSuite == NULL){
    CU_cleanup_registry();
    
    return CU_get_error();
  }

  /* add the tests to the suite */
  if((CU_add_test(pSuite, "test of ags_buffer_util.c s8 to char buffer", ags_buffer_util_test_s8_to_char_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c s16 to char buffer", ags_buffer_util_test_s16_to_char_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c s24 to char buffer", ags_buffer_util_test_s24_to_char_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c s32 to char buffer", ags_buffer_util_test_s32_to_char_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c s64 to char buffer", ags_buffer_util_test_s64_to_char_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c float to char buffer", ags_buffer_util_test_float_to_char_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c double to char buffer", ags_buffer_util_test_double_to_char_buffer) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c char buffer to s8", ags_buffer_util_test_char_buffer_to_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c char buffer to s16", ags_buffer_util_test_char_buffer_to_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c char buffer to s24", ags_buffer_util_test_char_buffer_to_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c char buffer to s32", ags_buffer_util_test_char_buffer_to_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c char buffer to s64", ags_buffer_util_test_char_buffer_to_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c char buffer to float", ags_buffer_util_test_char_buffer_to_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c char buffer to double", ags_buffer_util_test_char_buffer_to_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c read s8", ags_buffer_util_test_read_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c read s16", ags_buffer_util_test_read_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c read s24", ags_buffer_util_test_read_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c read s32", ags_buffer_util_test_read_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c read s64", ags_buffer_util_test_read_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c read float", ags_buffer_util_test_read_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c read double", ags_buffer_util_test_read_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c write s8", ags_buffer_util_test_write_s8) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c write s16", ags_buffer_util_test_write_s16) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c write s24", ags_buffer_util_test_write_s24) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c write s32", ags_buffer_util_test_write_s32) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c write s64", ags_buffer_util_test_write_s64) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c write float", ags_buffer_util_test_write_float) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c write double", ags_buffer_util_test_write_double) == NULL) ||
     (CU_add_test(pSuite, "test of ags_buffer_util.c swap bytes", ags_buffer_util_test_swap_bytes) == NULL)){
    CU_cleanup_registry();
    
    return CU_get_error();
  }
  
  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  
  CU_cleanup_registry();
  
  return(CU_get_error());
}
