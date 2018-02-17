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

#ifndef __AGS_BUFFER_UTIL_H__
#define __AGS_BUFFER_UTIL_H__

#include <glib.h>
#include <glib-object.h>

/* signed integers to char buffer */
unsigned char* ags_buffer_util_s8_to_char_buffer(signed char *buffer,
						 guint buffer_length);
unsigned char* ags_buffer_util_s16_to_char_buffer(signed short *buffer,
						  guint buffer_length);
unsigned char* ags_buffer_util_s24_to_char_buffer(signed long *buffer,
						  guint buffer_length);
unsigned char* ags_buffer_util_s32_to_char_buffer(signed long *buffer,
						  guint buffer_length);
unsigned char* ags_buffer_util_s64_to_char_buffer(signed long long *buffer,
						  guint buffer_length);
unsigned char* ags_buffer_util_float_to_char_buffer(float *buffer,
						    guint buffer_length);
unsigned char* ags_buffer_util_double_to_char_buffer(double *buffer,
						     guint buffer_length);

/* char buffer to signed integers */
signed char* ags_buffer_util_char_buffer_to_s8(unsigned char *cbuffer,
					       guint buffer_size);
signed short* ags_buffer_util_char_buffer_to_s16(unsigned char *cbuffer,
						 guint buffer_size);
signed long* ags_buffer_util_char_buffer_to_s24(unsigned char *cbuffer,
						guint buffer_size);
signed long* ags_buffer_util_char_buffer_to_s32(unsigned char *cbuffer,
						guint buffer_size);
signed long long* ags_buffer_util_char_buffer_to_s64(unsigned char *cbuffer,
						     guint buffer_size);
float* ags_buffer_util_char_buffer_to_float(unsigned char *cbuffer,
					    guint buffer_size);
double* ags_buffer_util_char_buffer_to_double(unsigned char *cbuffer,
					      guint buffer_size);

/* read integers */
signed char ags_buffer_util_char_buffer_read_s8(unsigned char *cbuffer,
						gboolean swap_bytes);
signed short ags_buffer_util_char_buffer_read_s16(unsigned char *cbuffer,
						  gboolean swap_bytes);
signed long ags_buffer_util_char_buffer_read_s24(unsigned char *cbuffer,
						 gboolean swap_bytes);
signed long ags_buffer_util_char_buffer_read_s32(unsigned char *cbuffer,
						 gboolean swap_bytes);
signed long long ags_buffer_util_char_buffer_read_s64(unsigned char *cbuffer,
						      gboolean swap_bytes);
float ags_buffer_util_char_buffer_read_float(unsigned char *cbuffer,
					     gboolean swap_bytes);
double ags_buffer_util_char_buffer_read_double(unsigned char *cbuffer,
					       gboolean swap_bytes);

/* write integers */
void ags_buffer_util_char_buffer_write_s8(unsigned char *cbuffer,
					  signed char value,
					  gboolean swap_bytes);
void ags_buffer_util_char_buffer_write_s16(unsigned char *cbuffer,
					   signed short value,
					   gboolean swap_bytes);
void ags_buffer_util_char_buffer_write_s24(unsigned char *cbuffer,
					   signed long value,
					   gboolean swap_bytes);
void ags_buffer_util_char_buffer_write_s32(unsigned char *cbuffer,
					   signed long value,
					   gboolean swap_bytes);
void ags_buffer_util_char_buffer_write_s64(unsigned char *cbuffer,
					   signed long long value,
					   gboolean swap_bytes);
void ags_buffer_util_char_buffer_write_float(unsigned char *cbuffer,
					     float value,
					     gboolean swap_bytes);
void ags_buffer_util_char_buffer_write_double(unsigned char *cbuffer,
					      double value,
					      gboolean swap_bytes);

/* swap bytes */
void ags_buffer_util_char_buffer_swap_bytes(unsigned char *cbuffer, guint word_size,
					    guint buffer_size);

#endif /*__AGS_BUFFER_UTIL_H__*/
