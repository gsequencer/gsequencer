/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <stdio.h>

#include <float.h>

int
main(int argc, char **argv)
{
  long long int lossy;
  long double result;

  int digs = 36;

  /* s8 conversion */
  result = ((2 << 14) - 1) / ((2 << 6) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S8_TO_S16 (%.*LfL)\n", digs, result);

  result = ((long double) (2 << 22) - 1) / ((long double) (2 << 6) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S8_TO_S24 (%.*LfL)\n", digs, result);

  result = ((long double) ((long long int) 2 << 30) - 1) / ((long double) (2 << 6) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S8_TO_S32 (%.*LfL)\n", digs, result);

  result = (long double) ((((long long int) 2 << 62) - 1) / ((2 << 6) - 1));
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S8_TO_S64 (%.*LfL)\n", digs, result);

  result = 1.0L / ((2 << 6) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S8_TO_FLOAT (%.*LfL)\n", digs, result);

  printf("\n");
  
  /* s16 conversion */
  result = 1.0 / (((2 << 14) - 1) * ((2 << 6) - 1));
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S16_TO_S8 (%.*LfL)\n", digs, result);

  result = ((2 << 22) - 1) / ((2 << 14) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S16_TO_S24 (%.*LfL)\n", digs, result);

  result = (((long long int) 2 << 30) - 1) / ((2 << 14) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S16_TO_S32 (%.*LfL)\n", digs, result);

  result = (((long long int) 2 << 62) - 1) / ((2 << 14) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S16_TO_S64 (%.*LfL)\n", digs, result);

  result = 1.0L / ((2 << 14) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S16_TO_FLOAT (%.*LfL)\n", digs, result);

  printf("\n");
  
  /* s24 conversion */
  result = 1.0 / ((2 << 22) - 1) * ((2 << 6) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S24_TO_S8 (%.*LfL)\n", digs, result);

  result = 1.0 / ((2 << 22) - 1) * ((2 << 14) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S24_TO_S16 (%.*LfL)\n", digs, result);

  result = (((long long int) 2 << 30) - 1) / ((2 << 22) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S24_TO_S32 (%.*LfL)\n", digs, result);

  result = (((long long int) 2 << 62) - 1) / ((2 << 22) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S24_TO_S64 (%.*LfL)\n", digs, result);

  result = 1.0L / ((2 << 22) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S24_TO_FLOAT (%.*LfL)\n", digs, result);

  printf("\n");
  
  /* s32 conversion */
  result = 1.0 / (((long long int) 2 << 30) - 1) * ((2 << 6) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S32_TO_S8 (%.*LfL)\n", digs, result);

  result = 1.0 / (((long long int) 2 << 30) - 1) * ((2 << 14) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S32_TO_S16 (%.*LfL)\n", digs, result);

  result = 1.0 / (((long long int) 2 << 30) - 1) * ((2 << 22) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S32_TO_S24 (%.*LfL)\n", digs, result);

  result = (((long long int) 2 << 62) - 1) / (((long long int) 2 << 30) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S32_TO_S64 (%.*LfL)\n", digs, result);

  result = 1.0L / (((long long int) 2 << 30) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S32_TO_FLOAT (%.*LfL)\n", digs, result);

  printf("\n");
  
  /* s64 conversion */
  result = 1.0 / (((long long int) 1 << 61) - 1) / 2.0 * ((2 << 6) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S64_TO_S8 (%.*LfL)\n", digs, result);

  result = 1.0 / (((long long int) 1 << 61) - 1) / 2.0 * ((2 << 14) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S64_TO_S16 (%.*LfL)\n", digs, result);

  result = 1.0 / (((long long int) 1 << 61) - 1) / 2.0 * ((2 << 22) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S64_TO_S24 (%.*LfL)\n", digs, result);

  result = 1.0 / (((long long int) 1 << 61) - 1) / 2.0 * ((2 << 30) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S64_TO_S32 (%.*LfL)\n", digs, result);

  result = 1.0L / (((long long int) 2 << 61) - 1) / 2.0;
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_S64_TO_FLOAT (%.*LfL)\n", digs, result);

  printf("\n");
  
  /* float conversion */
  result = ((2 << 6) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_FLOAT_TO_S8 (%.*LfL)\n", digs, result);

  result = ((2 << 14) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_FLOAT_TO_S16 (%.*LfL)\n", digs, result);

  result = ((2 << 22) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_FLOAT_TO_S24 (%.*LfL)\n", digs, result);

  result = (((long long int) 2 << 30) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_FLOAT_TO_S32 (%.*LfL)\n", digs, result);

  result = (((long long int) 2 << 62) - 1);
  printf("#define AGS_AUDIO_BUFFER_UTIL_FACTOR_FLOAT_TO_S64 (%.*LfL)\n", digs, result);

  printf("\n");
  
  return(0);
}
