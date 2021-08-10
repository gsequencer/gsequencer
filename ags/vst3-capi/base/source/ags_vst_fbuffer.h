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

#ifndef __AGS_VST_FBUFFER_H__
#define __AGS_VST_FBUFFER_H__

#include <glib.h>

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ftypes.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct AgsVstBuffer Buffer;

  AgsVstBuffer* ags_vst_buffer_new();
  AgsVstBuffer* ags_vst_buffer_new_from_string(void *b, guint32 size);
  AgsVstBuffer* ags_vst_buffer_new_and_fill_up(guint32 size, guint8 init_val);
  AgsVstBuffer* ags_vst_buffer_new_with_size(guint32 size);
  AgsVstBuffer* ags_vst_buffer_new_from_buffer(AgsVstBuffer *buffer);

  void ags_vst_buffer_delete(AgsVstBuffer *buffer);
        
  void ags_vst_buffer_equal(AgsVstBuffer *destination_buffer,
			    AgsVstBuffer *source_buffer);

  gboolean ags_vst_buffer_equals(AgsVstBuffer *buffer_a,
				 AgsVstBuffer *buffer_b);

  guint32 ags_vst_buffer_get_size(AgsVstBuffer *buffer);

  gboolean ags_vst_buffer_set_size(AgsVstBuffer *buffer,
				   guint32 new_size);

  gboolean ags_vst_buffer_grow(AgsVstBuffer *buffer,
			       guint32 mem_size);
  gboolean ags_vst_buffer_set_max_size(AgsVstBuffer *buffer,
				       guint32 size);

  void ags_vst_buffer_fillup(AgsVstBuffer *buffer,
			     guint8 init_val);
  guint32 ags_vst_buffer_get_fill_size(AgsVstBuffer *buffer);
  gboolean ags_vst_buffer_set_fill_size(AgsVstBuffer *buffer,
					guint32 c);
  void ags_vst_buffer_flush(AgsVstBuffer *buffer);
  gboolean ags_vst_buffer_truncate_to_fill_size(AgsVstBuffer *buffer);

  gboolean ags_vst_buffer_is_full(AgsVstBuffer *buffer);
  guint32 ags_vst_buffer_get_free(AgsVstBuffer *buffer);

  void ags_vst_buffer_shift_start(AgsVstBuffer *buffer,
				  gint32 amount);
  void ags_vst_buffer_shift_at(AgsVstBuffer *buffer,
			       guint32 position, gint32 amount);
  void ags_vst_buffer_move(AgsVstBuffer *buffer,
			   gint32 amount, guint8 init_val);

  gboolean ags_vst_buffer_copy(AgsVstBuffer *buffer,
			       guint32 from, guint32 to, guint32 bytes);
  guint32 ags_vst_buffer_get(AgsVstBuffer *buffer,
			     void *b, guint32 size);

  void ags_vst_buffer_set_delta(AgsVstBuffer *buffer,
				guint32 d);

  gboolean ags_vst_buffer_put_from_uint8(AgsVstBuffer *buffer,
					 guint8 val);
  gboolean ags_vst_buffer_put_from_unichar2(AgsVstBuffer *buffer,
					    gunichar2 c);
  gboolean ags_vst_buffer_put_from_char(AgsVstBuffer *buffer,
					gchar c);
  gboolean ags_vst_buffer_put_from_buffer(AgsVstBuffer *buffer,
					  void *ptr_buffer, guint32 size);
  gboolean ags_vst_buffer_put_from_uint8_buffer(AgsVstBuffer *buffer,
						guint8 *ptr_buffer, guint32 size);
  gboolean ags_vst_buffer_put_from_char_buffer(AgsVstBuffer *buffer,
					       gchar *ptr_buffer, guint32 size);
  gboolean ags_vst_buffer_put_from_string(AgsVstBuffer *buffer,
					  gchar *string);

  void ags_vst_buffer_set(AgsVstBuffer *buffer,
			  guint8 value);
        
  gboolean ags_vst_buffer_append_string(AgsVstBuffer *buffer,
					ags_vst_tchar *s);
  gboolean ags_vst_buffer_append_char(AgsVstBuffer *buffer,
				      ags_vst_tchar c);
  
  gboolean ags_vst_buffer_append_string_from_char_buffer(AgsVstBuffer *buffer,
							 gchar *s);
  gboolean ags_vst_buffer_append_string_from_uchar_buffer(AgsVstBuffer *buffer,
							  guchar *s);
  gboolean ags_vst_buffer_append_string_from_unichar2_buffer(AgsVstBuffer *buffer,
							     gunichar2 *s);

  gboolean ags_vst_buffer_append_string_from_char_byte(AgsVstBuffer *buffer,
						       gchar c);
  gboolean ags_vst_buffer_append_string_from_uchar_byte(AgsVstBuffer *buffer,
							guchar c);
  gboolean ags_vst_buffer_append_string_from_unichar2_byte(AgsVstBuffer *buffer,
							   gunichar2 c);

  gboolean ags_vst_buffer_prepend_string(AgsVstBuffer *buffer,
					 ags_vst_tchar *s);
  gboolean ags_vst_buffer_prepend_char(AgsVstBuffer *buffer,
				       ags_vst_tchar c);
  
  gboolean ags_vst_buffer_prepend_string_from_char_buffer(AgsVstBuffer *buffer,
							  gchar *s);
  gboolean ags_vst_buffer_prepend_string_from_uchar_buffer(AgsVstBuffer *buffer,
							   guchar *s);
  gboolean ags_vst_buffer_prepend_string_from_unichar2_buffer(AgsVstBuffer *buffer,
							      gunichar2 *s);

  gboolean ags_vst_buffer_prepend_string_from_char_byte(AgsVstBuffer *buffer,
							gchar c);
  gboolean ags_vst_buffer_prepend_string_from_uchar_byte(AgsVstBuffer *buffer,
							 guchar c);
  gboolean ags_vst_buffer_prepend_string_from_unichar2_byte(AgsVstBuffer *buffer,
							    gunichar2 c);

  void ags_vst_buffer_end_string(AgsVstBuffer *buffer);
  void ags_vst_buffer_end_string_char(AgsVstBuffer *buffer);
  void ags_vst_buffer_end_string_unichar2(AgsVstBuffer *buffer);

  gboolean ags_vst_buffer_make_hex_string(AgsVstBuffer *buffer,
					  gchar **result);
  gboolean ags_vst_buffer_from_hex_string(AgsVstBuffer *buffer,
					  gchar *string);

  void* ags_vst_buffer_to_buffer(AgsVstBuffer *buffer);
  ags_vst_tchar* ags_vst_buffer_to_str(AgsVstBuffer *buffer);
  gchar* ags_vst_buffer_to_str8(AgsVstBuffer *buffer);
  gunichar2* ags_vst_buffer_to_str16(AgsVstBuffer *buffer);
  gint8* ags_vst_buffer_to_int8_ptr(AgsVstBuffer *buffer);
  guint8* ags_vst_buffer_to_uint8_ptr(AgsVstBuffer *buffer);
  gint16* ags_vst_buffer_to_int16_ptr(AgsVstBuffer *buffer);
  guint16* ags_vst_buffer_to_uint16_ptr(AgsVstBuffer *buffer);
  gint32* ags_vst_buffer_to_int32_ptr(AgsVstBuffer *buffer);
  guint32* ags_vst_buffer_to_uint32_ptr(AgsVstBuffer *buffer);
  gfloat* ags_vst_buffer_to_float_ptr(AgsVstBuffer *buffer);
  gdouble* ags_vst_buffer_to_double_ptr(AgsVstBuffer *buffer);
  gunichar2* ags_vst_buffer_to_wchar_ptr(AgsVstBuffer *buffer);
        
  enum AgsVstSwapSize 
  {
    AGS_VST_KSWAP16 = 2,
    AGS_VST_KSWAP32 = 4,
    AGS_VST_KSWAP64 = 8,
  };

  gboolean ags_vst_buffer_swap(AgsVstBuffer *buffer,
			       gint16 swap_size);
  gboolean ags_vst_buffer_swap_buffer(AgsVstBuffer *buffer,
				      void *ptr_buffer, guint32 buffer_size, gint16 swap_size);
  
  void ags_vst_buffer_take(AgsVstBuffer *buffer,
			   AgsVstBuffer *from);
  gint8* ags_vst_buffer_pass(AgsVstBuffer *buffer);

  gboolean ags_vst_buffer_to_wide_string(AgsVstBuffer *buffer,
					 gint32 source_code_page);
  gboolean ags_vst_buffer_to_multibyte_string(AgsVstBuffer *buffer,
					      gint32 dest_code_page);

  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_FBUFFER_H__*/
