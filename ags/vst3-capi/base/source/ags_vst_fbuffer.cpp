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

#include <ags/vst3-capi/base/source/ags_vst_fbuffer.h>

#include <base/source/fbuffer.h>

extern "C" {

  AgsVstBuffer* ags_vst_buffer_new()
  {
    return(new Buffer());
  }
  
  AgsVstBuffer* ags_vst_buffer_new_from_string(void *b, guint32 size)
  {
    return(new Buffer(b, size));
  }
  
  AgsVstBuffer* ags_vst_buffer_new_and_fill_up(guint32 size, guint8 init_val)
  {
    return(new Buffer(size, init_val));
  }
  
  AgsVstBuffer* ags_vst_buffer_new_with_size(guint32 size)
  {
    return(new Buffer(size));
  }
  
  AgsVstBuffer* ags_vst_buffer_new_from_buffer(AgsVstBuffer **buffer)
  {
    return(new Buffer(static_cast<Buffer&>(buffer)));
  }
  
  void ags_vst_buffer_delete(AgsVstBuffer *buffer)
  {
    delete buffer;
  }
        
  void ags_vst_buffer_equal(AgsVstBuffer *destination_buffer,
			    AgsVstBuffer *source_buffer)
  {
    destination_buffer = source_buffer;
  }

  gboolean ags_vst_buffer_equals(AgsVstBuffer *buffer_a,
				 AgsVstBuffer *buffer_b)
  {
    return(buffer_a == buffer_b);
  }

  guint32 ags_vst_buffer_get_size(AgsVstBuffer *buffer)
  {
    return(buffer->getSize());
  }

  gboolean ags_vst_buffer_set_size(AgsVstBuffer *buffer,
				   guint32 new_size)
  {
    return(static_cast<gboolean>(buffer->setSize(new_size)));
  }

  gboolean ags_vst_buffer_grow(AgsVstBuffer *buffer,
			       guint32 mem_size)
  {
    return(static_cast<gboolean>(buffer->grow(mem_size)));
  }

  gboolean ags_vst_buffer_set_max_size(AgsVstBuffer *buffer,
				       guint32 size)
  {
    return(static_cast<gboolean>(buffer->setMaxSize(size)));
  }

  void ags_vst_buffer_fillup(AgsVstBuffer *buffer,
			     guint8 init_val)
  {
    buffer->fillup(init_value);
  }
  
  guint32 ags_vst_buffer_get_fill_size(AgsVstBuffer *buffer)
  {
    return(buffer->getFillSize());
  }
  
  gboolean ags_vst_buffer_set_fill_size(AgsVstBuffer *buffer,
					guint32 c)
  {
    return(static_cast<gboolean>(buffer->setFillSize(c)));
  }

  void ags_vst_buffer_flush(AgsVstBuffer *buffer)
  {
    buffer->flush()
  }
  
  gboolean ags_vst_buffer_truncate_to_fill_size(AgsVstBuffer *buffer)
  {
    return(static_cast<gboolean>(buffer->truncateToFillSize()));
  }

  gboolean ags_vst_buffer_is_full(AgsVstBuffer *buffer)
  {
    return(static_cast<gboolean>(buffer->isFull()));
  }

  guint32 ags_vst_buffer_get_free(AgsVstBuffer *buffer)
  {
    return(buffer->getFree());
  }

  void ags_vst_buffer_shift_start(AgsVstBuffer *buffer,
				  gint32 amount)
  {
    buffer->shiftStart(amount);
  }
  
  void ags_vst_buffer_shift_at(AgsVstBuffer *buffer,
			       guint32 position, gint32 amount)
  {
    buffer->shiftAt(position, amount);
  }
  
  void ags_vst_buffer_move(AgsVstBuffer *buffer,
			   gint32 amount, guint8 init_val)
  {
    buffer->move(amount, init_val);
  }

  gboolean ags_vst_buffer_copy(AgsVstBuffer *buffer,
			       guint32 from, guint32 to, guint32 bytes)
  {
    return(static_cast<gboolean>(buffer->copy(from, to, bytes)));
  }

  guint32 ags_vst_buffer_get(AgsVstBuffer *buffer,
			     void *b, guint32 size)
  {
    return(buffer->get(b, size));
  }

  void ags_vst_buffer_set_delta(AgsVstBuffer *buffer,
				guint32 d)
  {
    buffer->setDelta(d);
  }

  gboolean ags_vst_buffer_put_from_uint8(AgsVstBuffer *buffer,
					 guint8 val)
  {
    return(static_cast<gboolean>(buffer->put(val)));
  }

  gboolean ags_vst_buffer_put_from_unichar2(AgsVstBuffer *buffer,
					    gunichar2 c)
  {
    return(static_cast<gboolean>(buffer->put(c)));
  }

  gboolean ags_vst_buffer_put_from_char(AgsVstBuffer *buffer,
					gchar c)
  {
    return(static_cast<gboolean>(buffer->put(c)));
  }

  gboolean ags_vst_buffer_put_from_buffer(AgsVstBuffer *buffer,
					  void *buffer, guint32 size)
  {
    return(static_cast<gboolean>(buffer->put(buffer, size)));
  }

  gboolean ags_vst_buffer_put_from_uint8_buffer(AgsVstBuffer *buffer,
						guint8 *buffer, guint32 size)
  {
    return(static_cast<gboolean>(buffer->put(buffer, size)));
  }

  gboolean ags_vst_buffer_put_from_char_buffer(AgsVstBuffer *buffer,
					       gchar *buffer, guint32 size)
  {
    return(static_cast<gboolean>(buffer->put(buffer, size)));
  }

  gboolean ags_vst_buffer_put_from_string(AgsVstBuffer *buffer,
					  gchar **string)
  {
    return(static_cast<gboolean>(buffer->put(static_cast<String&>(string[0]))));
  }


  void ags_vst_buffer_set(AgsVstBuffer *buffer,
			  guint8 value)
  {
    buffer->set(value);
  }
        
  gboolean ags_vst_buffer_append_string(AgsVstBuffer *buffer,
					ags_vst_tchar *s)
  {
    return(static_cast<gboolean>(buffer->append(s)));
  }

  gboolean ags_vst_buffer_append_char(AgsVstBuffer *buffer,
				      ags_vst_tchar c)
  {
    return(static_cast<gboolean>(buffer->append(c)));
  }
  
  gboolean ags_vst_buffer_append_string_from_char_buffer(AgsVstBuffer *buffer,
							 gchar *s)
  {
    return(static_cast<gboolean>(buffer->appendString8(s)));
  }

  gboolean ags_vst_buffer_append_string_from_uchar_buffer(AgsVstBuffer *buffer,
							  guchar *s)
  {
    return(static_cast<gboolean>(buffer->appendString8(s)));
  }

  gboolean ags_vst_buffer_append_string_from_unichar2_buffer(AgsVstBuffer *buffer,
							     gunichar2 *s)
  {
    return(static_cast<gboolean>(buffer->appendString16(s)));
  }

  gboolean ags_vst_buffer_append_string_from_char_byte(AgsVstBuffer *buffer,
						       gchar c)
  {
    return(static_cast<gboolean>(buffer->appendString8(c)));
  }

  gboolean ags_vst_buffer_append_string_from_uchar_byte(AgsVstBuffer *buffer,
							guchar c)
  {
    return(static_cast<gboolean>(buffer->appendString8(c)));
  }

  gboolean ags_vst_buffer_append_string_from_unichar2_byte(AgsVstBuffer *buffer,
							   gunichar2 c)
  {
    return(static_cast<gboolean>(buffer->appendString16()));
  }

  gboolean ags_vst_buffer_prepend_string(AgsVstBuffer *buffer,
					 ags_vst_tchar *s)
  {
    return(static_cast<gboolean>(buffer->prependString(s)));
  }

  gboolean ags_vst_buffer_prepend_char(AgsVstBuffer *buffer,
				       ags_vst_tchar c)
  {
    return(static_cast<gboolean>(buffer->prependString8()));
  }

  
  gboolean ags_vst_buffer_prepend_string_from_char_buffer(AgsVstBuffer *buffer,
							  gchar *s)
  {
    return(static_cast<gboolean>(buffer->prependString8(s)));
  }

  gboolean ags_vst_buffer_prepend_string_from_uchar_buffer(AgsVstBuffer *buffer,
							   guchar *s)
  {
    return(static_cast<gboolean>(buffer->prependString8(s)));
  }

  gboolean ags_vst_buffer_prepend_string_from_unichar2_buffer(AgsVstBuffer *buffer,
							      gunichar2 *s)
  {
    return(static_cast<gboolean>(buffer->prependString16(s)));
  }


  gboolean ags_vst_buffer_prepend_string_from_char_byte(AgsVstBuffer *buffer,
							gchar c)
  {
    return(static_cast<gboolean>(buffer->prependString8(c)));
  }

  gboolean ags_vst_buffer_prepend_string_from_uchar_byte(AgsVstBuffer *buffer,
							 guchar c)
  {
    return(static_cast<gboolean>(buffer->prependString8(c)));
  }

  gboolean ags_vst_buffer_prepend_string_from_unichar2_byte(AgsVstBuffer *buffer,
							    gunichar2 c)
  {
    return(static_cast<gboolean>(buffer->prependString16(c)));
  }

  void ags_vst_buffer_end_string(AgsVstBuffer *buffer)
  {
    buffer->endString();
  }
  
  void ags_vst_buffer_end_string_char(AgsVstBuffer *buffer)
  {
    buffer->endString8();
  }
  
  void ags_vst_buffer_end_string_unichar2(AgsVstBuffer *buffer)
  {
    buffer->endString16();
  }

  gboolean ags_vst_buffer_make_hex_string(AgsVstBuffer *buffer,
					  gchar **result)
  {
    return(static_cast<gboolean>(buffer->makeHexString(static_cast<String&>(result[0]))));
  }

  gboolean ags_vst_buffer_from_hex_string(AgsVstBuffer *buffer,
					  gchar *string)
  {
    return(static_cast<gboolean>(buffer->fromHexString(string)));
  }

  void* ags_vst_buffer_to_buffer(AgsVstBuffer *buffer)
  {
    return(buffer);
  }
  
  ags_vst_tchar* ags_vst_buffer_to_str(AgsVstBuffer *buffer)
  {
    return(buffer->str());
  }
  
  gchar* ags_vst_buffer_to_str8(AgsVstBuffer *buffer)
  {
    return(buffer->str8());
  }
  
  gunichar2* ags_vst_buffer_to_str16(AgsVstBuffer *buffer)
  {
    return(buffer->str16());
  }
  
  gint8* ags_vst_buffer_to_int8_ptr(AgsVstBuffer *buffer)
  {
    return(buffer->int8Ptr());
  }
  
  guint8* ags_vst_buffer_to_uint8_ptr(AgsVstBuffer *buffer)
  {
    return(buffer->uint8Ptr());
  }
  
  gint16* ags_vst_buffer_to_int16_ptr(AgsVstBuffer *buffer)
  {
    return(buffer->int16Ptr());
  }
  
  guint16* ags_vst_buffer_to_uint16_ptr(AgsVstBuffer *buffer)
  {
    return(buffer->uint16Ptr());
  }
  
  gint32* ags_vst_buffer_to_int32_ptr(AgsVstBuffer *buffer)
  {
    return(buffer->int32Ptr());
  }
  
  guint32* ags_vst_buffer_to_uint32_ptr(AgsVstBuffer *buffer)
  {
    return(buffer->uint32Ptr());
  }
  
  gfloat* ags_vst_buffer_to_float_ptr(AgsVstBuffer *buffer)
  {
    return(buffer->floatPtr());
  }
  
  gdouble* ags_vst_buffer_to_double_ptr(AgsVstBuffer *buffer)
  {
    return(buffer->doublePtr());
  }
  
  gunichar2* ags_vst_buffer_to_wchar_ptr(AgsVstBuffer *buffer)
  {
    return(buffer->wcharPtr());
  }
  
  gint8 ags_vst_buffer_offset(AgsVstBuffer *buffer,
			      guint32 i)
  {
    return(buffer->offset(i));
  }
  
  gint8 ags_vst_buffer_is_empty(AgsVstBuffer *buffer,
				guint32 i)
  {
    return(buffer->isEmpty(i));
  }
  
  gboolean ags_vst_buffer_swap(AgsVstBuffer *buffer,
			       gint16 swap_size)
  {
    return(static_cast<gboolean>(buffer->swap(swap_size)));
  }

  gboolean ags_vst_buffer_swap_buffer(AgsVstBuffer *buffer,
				      void* buffer, guint32 buffer_size, gint16 swap_size)
  {
    return(static_cast<gboolean>(buffer->swapBuffer(buffer, buffer_size, swap_size)));
  }
  
  void ags_vst_buffer_take(AgsVstBuffer **bufferBuffer)
  {
    buffer->take();
  }
  
  gint8* ags_vst_buffer_pass(AgsVstBuffer *buffer)
  {
    return(buffer->pass());
  }

  gboolean ags_vst_buffer_to_wide_string(AgsVstBuffer *buffer,
					 gint32 source_code_page)
  {
    return(static_cast<gboolean>(buffer->toWideString(source_code_page)));
  }

  gboolean ags_vst_buffer_to_multibyte_string(AgsVstBuffer *buffer,
					      gint32 dest_code_page)
  {
    return(static_cast<gboolean>(buffer->toMultibyteString(dest_code_page)));
  }
  
}
