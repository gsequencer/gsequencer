/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/lib/ags_endian.h>
#include <ags/lib/ags_complex.h>

/* signed integers to char buffer */
guchar* ags_buffer_util_s8_to_char_buffer(gint8 *buffer,
					  guint buffer_length);
guchar* ags_buffer_util_s16_to_char_buffer(gint16 *buffer,
					   guint buffer_length);
guchar* ags_buffer_util_s24_to_char_buffer(gint32 *buffer,
					   guint buffer_length);
guchar* ags_buffer_util_s32_to_char_buffer(gint32 *buffer,
					   guint buffer_length);
guchar* ags_buffer_util_s64_to_char_buffer(gint64 *buffer,
					   guint buffer_length);
guchar* ags_buffer_util_float_to_char_buffer(gfloat *buffer,
					     guint buffer_length);
guchar* ags_buffer_util_double_to_char_buffer(gdouble *buffer,
					      guint buffer_length);
guchar* ags_buffer_util_complex_to_char_buffer(AgsComplex *buffer,
					       guint buffer_length);

/* char buffer to signed integers */
gint8* ags_buffer_util_char_buffer_to_s8(guchar *cbuffer,
					 guint buffer_size);
gint16* ags_buffer_util_char_buffer_to_s16(guchar *cbuffer,
					   guint buffer_size);
gint32* ags_buffer_util_char_buffer_to_s24(guchar *cbuffer,
					   guint buffer_size);
gint32* ags_buffer_util_char_buffer_to_s32(guchar *cbuffer,
					   guint buffer_size);
gint64* ags_buffer_util_char_buffer_to_s64(guchar *cbuffer,
					   guint buffer_size);
gfloat* ags_buffer_util_char_buffer_to_float(guchar *cbuffer,
					     guint buffer_size);
gdouble* ags_buffer_util_char_buffer_to_double(guchar *cbuffer,
					       guint buffer_size);
AgsComplex* ags_buffer_util_char_buffer_to_complex(guchar *cbuffer,
						   guint buffer_size);

/* read integers */
gint8 ags_buffer_util_char_buffer_read_s8(guchar *cbuffer,
					  guint byte_order);
gint16 ags_buffer_util_char_buffer_read_s16(guchar *cbuffer,
					    guint byte_order);
gint32 ags_buffer_util_char_buffer_read_s24(guchar *cbuffer,
					    guint byte_order);
gint32 ags_buffer_util_char_buffer_read_s32(guchar *cbuffer,
					    guint byte_order);
gint64 ags_buffer_util_char_buffer_read_s64(guchar *cbuffer,
					    guint byte_order);
gfloat ags_buffer_util_char_buffer_read_float(guchar *cbuffer,
					      guint byte_order);
gdouble ags_buffer_util_char_buffer_read_double(guchar *cbuffer,
						guint byte_order);
AgsComplex* ags_buffer_util_char_buffer_read_complex(guchar *cbuffer,
						     guint byte_order);

/* write integers */
void ags_buffer_util_char_buffer_write_s8(guchar *cbuffer,
					  gint8 value,
					  guint byte_order);
void ags_buffer_util_char_buffer_write_s16(guchar *cbuffer,
					   gint16 value,
					   guint byte_order);
void ags_buffer_util_char_buffer_write_s24(guchar *cbuffer,
					   gint32 value,
					   guint byte_order);
void ags_buffer_util_char_buffer_write_s32(guchar *cbuffer,
					   gint32 value,
					   guint byte_order);
void ags_buffer_util_char_buffer_write_s64(guchar *cbuffer,
					   gint64 value,
					   guint byte_order);
void ags_buffer_util_char_buffer_write_float(guchar *cbuffer,
					     gfloat value,
					     guint byte_order);
void ags_buffer_util_char_buffer_write_double(guchar *cbuffer,
					      gdouble value,
					      guint byte_order);
void ags_buffer_util_char_buffer_write_complex(guchar *cbuffer,
					       AgsComplex *value,
					       guint byte_order);

/* swap bytes */
void ags_buffer_util_char_buffer_swap_bytes(guchar *cbuffer, guint word_size,
					    guint buffer_size);

#endif /*__AGS_BUFFER_UTIL_H__*/
