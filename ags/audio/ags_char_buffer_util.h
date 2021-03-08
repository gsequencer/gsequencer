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

#ifndef __AGS_CHAR_BUFFER_UTIL_H__
#define __AGS_CHAR_BUFFER_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

/**
 * AgsCharBufferUtilCopyMode:
 * @AGS_CHAR_BUFFER_UTIL_COPY_S8_TO_CBUFFER: copy signed 8 bit to char buffer
 * @AGS_CHAR_BUFFER_UTIL_COPY_S16_TO_CBUFFER: copy signed 16 bit to char buffer
 * @AGS_CHAR_BUFFER_UTIL_COPY_S24_TO_CBUFFER: copy signed 24 bit to char buffer
 * @AGS_CHAR_BUFFER_UTIL_COPY_S32_TO_CBUFFER: copy signed 32 bit to char buffer
 * @AGS_CHAR_BUFFER_UTIL_COPY_S64_TO_CBUFFER: copy signed 64 bit to char buffer
 * @AGS_CHAR_BUFFER_UTIL_COPY_FLOAT_TO_CBUFFER: copy float to char buffer
 * @AGS_CHAR_BUFFER_UTIL_COPY_DOUBLE_TO_CBUFFER: copy double to char buffer
 * @AGS_CHAR_BUFFER_UTIL_COPY_CBUFFER_TO_S8: copy char buffer to signed 8 bit
 * @AGS_CHAR_BUFFER_UTIL_COPY_CBUFFER_TO_S16: copy char buffer to signed 16 bit
 * @AGS_CHAR_BUFFER_UTIL_COPY_CBUFFER_TO_S24: copy char buffer to signed 24 bit
 * @AGS_CHAR_BUFFER_UTIL_COPY_CBUFFER_TO_S32: copy char buffer to signed 32 bit
 * @AGS_CHAR_BUFFER_UTIL_COPY_CBUFFER_TO_S64: copy char buffer to signed 64 bit
 * @AGS_CHAR_BUFFER_UTIL_COPY_CBUFFER_TO_FLOAT: copy char buffer to float
 * @AGS_CHAR_BUFFER_UTIL_COPY_CBUFFER_TO_DOUBLE: copy char buffer to double
 * 
 * Copy modes.
 */
typedef enum{
  AGS_CHAR_BUFFER_UTIL_COPY_S8_TO_CBUFFER,
  AGS_CHAR_BUFFER_UTIL_COPY_S16_TO_CBUFFER,
  AGS_CHAR_BUFFER_UTIL_COPY_S24_TO_CBUFFER,
  AGS_CHAR_BUFFER_UTIL_COPY_S32_TO_CBUFFER,
  AGS_CHAR_BUFFER_UTIL_COPY_S64_TO_CBUFFER,
  AGS_CHAR_BUFFER_UTIL_COPY_FLOAT_TO_CBUFFER,
  AGS_CHAR_BUFFER_UTIL_COPY_DOUBLE_TO_CBUFFER,
  AGS_CHAR_BUFFER_UTIL_COPY_CBUFFER_TO_S8,
  AGS_CHAR_BUFFER_UTIL_COPY_CBUFFER_TO_S16,
  AGS_CHAR_BUFFER_UTIL_COPY_CBUFFER_TO_S24,
  AGS_CHAR_BUFFER_UTIL_COPY_CBUFFER_TO_S32,
  AGS_CHAR_BUFFER_UTIL_COPY_CBUFFER_TO_S64,
  AGS_CHAR_BUFFER_UTIL_COPY_CBUFFER_TO_FLOAT,
  AGS_CHAR_BUFFER_UTIL_COPY_CBUFFER_TO_DOUBLE,
}AgsCharBufferUtilCopyMode;

/* copy to cbuffer */
void ags_char_buffer_util_copy_s8_to_cbuffer(guchar *destination, guint word_size, guint dchannels,
					     gint8 *source, guint schannels,
					     guint frame_count, guint byte_order);
void ags_char_buffer_util_copy_s16_to_cbuffer(guchar *destination, guint word_size, guint dchannels,
					      gint16 *source, guint schannels,
					      guint frame_count, guint byte_order);
void ags_char_buffer_util_copy_s24_to_cbuffer(guchar *destination, guint word_size, guint dchannels,
					      gint32 *source, guint schannels,
					      guint frame_count, guint byte_order);
void ags_char_buffer_util_copy_s32_to_cbuffer(guchar *destination, guint word_size, guint dchannels,
					      gint32 *source, guint schannels,
					      guint frame_count, guint byte_order);
void ags_char_buffer_util_copy_s64_to_cbuffer(guchar *destination, guint word_size, guint dchannels,
					      gint64 *source, guint schannels,
					      guint frame_count, guint byte_order);
void ags_char_buffer_util_copy_float_to_cbuffer(guchar *destination, guint word_size, guint dchannels,
						gfloat *source, guint schannels,
						guint frame_count, guint byte_order);
void ags_char_buffer_util_copy_double_to_cbuffer(guchar *destination, guint word_size, guint dchannels,
						 gdouble *source, guint schannels,
						 guint frame_count, guint byte_order);

/* copy from cbuffer */
void ags_char_buffer_util_copy_cbuffer_to_s8(gint8 *destination, guint dchannels,
					     guchar *source, guint word_size, guint schannels,
					     guint frame_count, guint byte_order);
void ags_char_buffer_util_copy_cbuffer_to_s16(gint16 *destination, guint dchannels,
					      guchar *source, guint word_size, guint schannels,
					      guint frame_count, guint byte_order);
void ags_char_buffer_util_copy_cbuffer_to_s24(gint32 *destination, guint dchannels,
					      guchar *source, guint word_size, guint schannels,
					      guint frame_count, guint byte_order);
void ags_char_buffer_util_copy_cbuffer_to_s32(gint32 *destination, guint dchannels,
					      guchar *source, guint word_size, guint schannels,
					      guint frame_count, guint byte_order);
void ags_char_buffer_util_copy_cbuffer_to_s64(gint64 *destination, guint dchannels,
					      guchar *source, guint word_size, guint schannels,
					      guint frame_count, guint byte_order);
void ags_char_buffer_util_copy_cbuffer_to_float(gfloat *destination, guint dchannels,
						gint8 *source, guint word_size, guint schannels,
						guint frame_count, guint byte_order);
void ags_char_buffer_util_copy_cbuffer_to_double(gdouble *destination, guint dchannels,
						 guchar *source, guint word_size, guint schannels,
						 guint frame_count, guint byte_order);

/* copy */
void ags_char_buffer_util_copy_buffer_to_buffer(void *destination, guint dchannels, guint doffset,
						void *source, guint schannels, guint soffset,
						guint frame_count, guint byte_order,
						guint word_size, guint mode);

G_END_DECLS

#endif /*__AGS_CHAR_BUFFER_UTIL_H__*/
