/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <cstring>

extern "C" {

  /**
   * Instantiate Steinberg::Buffer and return a C99 compatible handle 
   * AgsVstBuffer a void pointer.
   *
   * @return the new instance of Steinberg::Buffer as AgsVstBuffer
   * 
   * @since 5.0.0
   */
  AgsVstBuffer* ags_vst_buffer_new()
  {
    return((AgsVstBuffer *) new Steinberg::Buffer());
  }
  
  /**
   * Instantiate Steinberg::Buffer and return a C99 compatible handle 
   * AgsVstBuffer a void pointer.
   *
   * @param b the string 
   * @param size the size
   * @return the new instance of Steinberg::Buffer as AgsVstBuffer
   * 
   * @since 5.0.0
   */
  AgsVstBuffer* ags_vst_buffer_new_from_string(void *b, guint32 size)
  {
    return((AgsVstBuffer *) new Steinberg::Buffer(b, size));
  }
  
  /**
   * Instantiate Steinberg::Buffer and return a C99 compatible handle 
   * AgsVstBuffer a void pointer.
   *
   * @param size the size
   * @param init_val the initial value 
   * @return the new instance of Steinberg::Buffer as AgsVstBuffer
   * 
   * @since 5.0.0
   */
  AgsVstBuffer* ags_vst_buffer_new_and_fill_up(guint32 size, guint8 init_val)
  {
    return((AgsVstBuffer *) new Steinberg::Buffer(size, init_val));
  }
  
  /**
   * Instantiate Steinberg::Buffer and return a C99 compatible handle 
   * AgsVstBuffer a void pointer.
   *
   * @param size the size
   * @return the new instance of Steinberg::Buffer as AgsVstBuffer
   * 
   * @since 5.0.0
   */
  AgsVstBuffer* ags_vst_buffer_new_with_size(guint32 size)
  {
    return((AgsVstBuffer *) new Steinberg::Buffer(size));
  }
  
  /**
   * Instantiate Steinberg::Buffer and return a C99 compatible handle 
   * AgsVstBuffer a void pointer.
   *
   * @param buffer the buffer
   * @return the new instance of Steinberg::Buffer as AgsVstBuffer
   * 
   * @since 5.0.0
   */
  AgsVstBuffer* ags_vst_buffer_new_from_buffer(AgsVstBuffer *buffer)
  {
    Steinberg::Buffer *tmp_buffer_0 = static_cast<Steinberg::Buffer*>((void *) buffer);
    const Steinberg::Buffer& tmp_buffer_1 = (const Steinberg::Buffer&) tmp_buffer_0;
    
    return((AgsVstBuffer *) new Steinberg::Buffer(tmp_buffer_1));
  }
  
  /**
   * Delete Steinberg::Buffer.
   *
   * @param buffer the buffer
   * 
   * @since 5.0.0
   */
  void ags_vst_buffer_delete(AgsVstBuffer *buffer)
  {
    delete buffer;
  }
        
  /**
   * The destination is made equals to source.
   *
   * @param destination_buffer the destination buffer
   * @param source_buffer the source buffer
   * 
   * @since 5.0.0
   */
  void ags_vst_buffer_equal(AgsVstBuffer *destination_buffer,
			    AgsVstBuffer *source_buffer)
  {
    destination_buffer = source_buffer;
  }

  /**
   * The buffers a and b are tested to be equals.
   *
   * @param buffer_a the buffer
   * @param buffer_b the other buffer
   * @return true if equals, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_buffer_equals(AgsVstBuffer *buffer_a,
				 AgsVstBuffer *buffer_b)
  {
    return(buffer_a == buffer_b);
  }

  /**
   * Get buffer size.
   *
   * @param buffer the buffer
   * @return the size of buffer
   * 
   * @since 5.0.0
   */
  guint32 ags_vst_buffer_get_size(AgsVstBuffer *buffer)
  {
    return(static_cast<Steinberg::Buffer*>((void *) buffer)->getSize());
  }

  /**
   * Set buffer size.
   *
   * @param buffer the buffer
   * @param new_size the new size
   * @return true if success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_buffer_set_size(AgsVstBuffer *buffer,
				   guint32 new_size)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->setSize(new_size)));
  }

  /**
   * Grow buffer size.
   *
   * @param buffer the buffer
   * @param mem_size the mem size
   * @return true if success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_buffer_grow(AgsVstBuffer *buffer,
			       guint32 mem_size)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->grow(mem_size)));
  }

  /**
   * Set buffer max size.
   *
   * @param buffer the buffer
   * @param size the max size
   * @return true if success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_buffer_set_max_size(AgsVstBuffer *buffer,
				       guint32 size)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->setMaxSize(size)));
  }

  /**
   * Fill up buffer.
   * 
   * @param buffer the buffer
   * @param init_val the initial value
   * 
   * @since 5.0.0
   */
  void ags_vst_buffer_fillup(AgsVstBuffer *buffer,
			     guint8 init_val)
  {
    static_cast<Steinberg::Buffer*>((void *) buffer)->fillup(init_val);
  }
  
  /**
   * Get fill size.
   * 
   * @param buffer the buffer
   * @return the fill size
   * 
   * @since 5.0.0
   */
  guint32 ags_vst_buffer_get_fill_size(AgsVstBuffer *buffer)
  {
    return(static_cast<Steinberg::Buffer*>((void *) buffer)->getFillSize());
  }
  
  /**
   * Set fill size.
   * 
   * @param buffer the buffer
   * @param c the fill size
   * @return true on success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_buffer_set_fill_size(AgsVstBuffer *buffer,
					guint32 c)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->setFillSize(c)));
  }

  /**
   * Flush.
   * 
   * @param buffer the buffer
   * 
   * @since 5.0.0
   */
  void ags_vst_buffer_flush(AgsVstBuffer *buffer)
  {
    static_cast<Steinberg::Buffer*>((void *) buffer)->flush();
  }
  
  /**
   * Truncate to fill size.
   * 
   * @param buffer the buffer
   * @return true on success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_buffer_truncate_to_fill_size(AgsVstBuffer *buffer)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->truncateToFillSize()));
  }

  /**
   * Test if is full.
   * 
   * @param buffer the buffer
   * @return true if full, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_buffer_is_full(AgsVstBuffer *buffer)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->isFull()));
  }

  /**
   * Get free.
   * 
   * @param buffer the buffer
   * @return the free size
   * 
   * @since 5.0.0
   */
  guint32 ags_vst_buffer_get_free(AgsVstBuffer *buffer)
  {
    return(static_cast<Steinberg::Buffer*>((void *) buffer)->getFree());
  }

  /**
   * Shift start.
   * 
   * @param buffer the buffer
   * @param amount the amount
   * 
   * @since 5.0.0
   */
  void ags_vst_buffer_shift_start(AgsVstBuffer *buffer,
				  gint32 amount)
  {
    static_cast<Steinberg::Buffer*>((void *) buffer)->shiftStart(amount);
  }
  
  /**
   * Shift at.
   * 
   * @param buffer the buffer
   * @param position the position
   * @param amount the amount
   * 
   * @since 5.0.0
   */
  void ags_vst_buffer_shift_at(AgsVstBuffer *buffer,
			       guint32 position, gint32 amount)
  {
    static_cast<Steinberg::Buffer*>((void *) buffer)->shiftAt(position, amount);
  }
  
  /**
   * Move.
   * 
   * @param buffer the buffer
   * @param amount the amount
   * @param init_val the initial value
   * 
   * @since 5.0.0
   */
  void ags_vst_buffer_move(AgsVstBuffer *buffer,
			   gint32 amount, guint8 init_val)
  {
    static_cast<Steinberg::Buffer*>((void *) buffer)->move(amount, init_val);
  }

  /**
   * Copy.
   * 
   * @param buffer the buffer
   * @param from the from position
   * @param to the to position
   * @param bytes the bytes count
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_buffer_copy(AgsVstBuffer *buffer,
			       guint32 from, guint32 to, guint32 bytes)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->copy(from, to, bytes)));
  }

  /**
   * Get.
   * 
   * @param buffer the buffer
   * @param b the pointer
   * @param size the size
   * @return the value
   * 
   * @since 5.0.0
   */
  guint32 ags_vst_buffer_get(AgsVstBuffer *buffer,
			     void *b, guint32 size)
  {
    return(static_cast<Steinberg::Buffer*>((void *) buffer)->get(b, size));
  }

  /**
   * Set delta.
   * 
   * @param buffer the buffer
   * @param d the delta
   * 
   * @since 5.0.0
   */
  void ags_vst_buffer_set_delta(AgsVstBuffer *buffer,
				guint32 d)
  {
    static_cast<Steinberg::Buffer*>((void *) buffer)->setDelta(d);
  }

  /**
   * Put from unsigned byte.
   * 
   * @param buffer the buffer
   * @param val the value
   * @return true on success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_buffer_put_from_uint8(AgsVstBuffer *buffer,
					 guint8 val)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->put((Steinberg::uint8) val)));
  }

  /**
   * Put from unicode character.
   * 
   * @param buffer the buffer
   * @param c the value
   * @return true on success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_buffer_put_from_unichar2(AgsVstBuffer *buffer,
					    gunichar2 c)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->put((Steinberg::char16) c)));
  }

  /**
   * Put from character.
   * 
   * @param buffer the buffer
   * @param c the value
   * @return true on success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_buffer_put_from_char(AgsVstBuffer *buffer,
					gchar c)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->put((char) c)));
  }

  /**
   * Put from buffer.
   * 
   * @param buffer the buffer
   * @param ptr_buffer the pointer on buffer
   * @param size the size
   * @return true on success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_buffer_put_from_buffer(AgsVstBuffer *buffer,
					  void *ptr_buffer, guint32 size)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->put(ptr_buffer, size)));
  }

  /**
   * Put from unsigned byte buffer.
   * 
   * @param buffer the buffer
   * @param ptr_buffer the pointer on buffer
   * @param size the size
   * @return true on success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_buffer_put_from_uint8_buffer(AgsVstBuffer *buffer,
						guint8 *ptr_buffer, guint32 size)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->put(ptr_buffer, size)));
  }

  /**
   * Put from character buffer.
   * 
   * @param buffer the buffer
   * @param ptr_buffer the pointer on buffer
   * @param size the size
   * @return true on success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_buffer_put_from_char_buffer(AgsVstBuffer *buffer,
					       gchar *ptr_buffer, guint32 size)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->put(ptr_buffer, size)));
  }

  /**
   * Put from string.
   * 
   * @param buffer the buffer
   * @param string the string
   * @return true on success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_buffer_put_from_string(AgsVstBuffer *buffer,
					  gchar *string)
  {
    Steinberg::String *tmp_string_0 = static_cast<Steinberg::String*>((void *) string);
    const Steinberg::String& tmp_string_1 = (const Steinberg::String&) tmp_string_0;
    
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->put(tmp_string_1)));
  }

  /**
   * Set unsigned byte value.
   * 
   * @param buffer the buffer
   * @param value the value
   * 
   * @since 5.0.0
   */
  void ags_vst_buffer_set(AgsVstBuffer *buffer,
			  guint8 value)
  {
    static_cast<Steinberg::Buffer*>((void *) buffer)->set(value);
  }
        
  gboolean ags_vst_buffer_append_string(AgsVstBuffer *buffer,
					ags_vst_tchar *s)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->appendString((Steinberg::tchar*) s)));
  }

  gboolean ags_vst_buffer_append_char(AgsVstBuffer *buffer,
				      ags_vst_tchar c)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->appendString(c)));
  }
  
  gboolean ags_vst_buffer_append_string_from_char_buffer(AgsVstBuffer *buffer,
							 gchar *s)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->appendString8(s)));
  }

  gboolean ags_vst_buffer_append_string_from_uchar_buffer(AgsVstBuffer *buffer,
							  guchar *s)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->appendString8(s)));
  }

  gboolean ags_vst_buffer_append_string_from_unichar2_buffer(AgsVstBuffer *buffer,
							     gunichar2 *s)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->appendString16((Steinberg::char16*) s)));
  }

  gboolean ags_vst_buffer_append_string_from_char_byte(AgsVstBuffer *buffer,
						       gchar c)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->appendString8(c)));
  }

  gboolean ags_vst_buffer_append_string_from_uchar_byte(AgsVstBuffer *buffer,
							guchar c)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->appendString8(c)));
  }

  gboolean ags_vst_buffer_append_string_from_unichar2_byte(AgsVstBuffer *buffer,
							   gunichar2 c)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->appendString16((Steinberg::char16) c)));
  }

  gboolean ags_vst_buffer_prepend_string(AgsVstBuffer *buffer,
					 ags_vst_tchar *s)
  {
#if !defined(AGS_VST_UNICODE)
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->prependString((char *) s)));
#else
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->prependString((char16_t *) s)));
#endif
  }

  gboolean ags_vst_buffer_prepend_char(AgsVstBuffer *buffer,
				       ags_vst_tchar c)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->prependString((Steinberg::tchar) c)));
  }

  
  gboolean ags_vst_buffer_prepend_string_from_char_buffer(AgsVstBuffer *buffer,
							  gchar *s)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->prependString8(s)));
  }

  gboolean ags_vst_buffer_prepend_string_from_uchar_buffer(AgsVstBuffer *buffer,
							   guchar *s)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->prependString8(s)));
  }

  gboolean ags_vst_buffer_prepend_string_from_unichar2_buffer(AgsVstBuffer *buffer,
							      gunichar2 *s)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->prependString16((Steinberg::char16 *) s)));
  }


  gboolean ags_vst_buffer_prepend_string_from_char_byte(AgsVstBuffer *buffer,
							gchar c)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->prependString8(c)));
  }

  gboolean ags_vst_buffer_prepend_string_from_uchar_byte(AgsVstBuffer *buffer,
							 guchar c)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->prependString8(c)));
  }

  gboolean ags_vst_buffer_prepend_string_from_unichar2_byte(AgsVstBuffer *buffer,
							    gunichar2 c)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->prependString16(c)));
  }

  void ags_vst_buffer_end_string(AgsVstBuffer *buffer)
  {
    static_cast<Steinberg::Buffer*>((void *) buffer)->endString();
  }
  
  void ags_vst_buffer_end_string_char(AgsVstBuffer *buffer)
  {
    static_cast<Steinberg::Buffer*>((void *) buffer)->endString8();
  }
  
  void ags_vst_buffer_end_string_unichar2(AgsVstBuffer *buffer)
  {
    static_cast<Steinberg::Buffer*>((void *) buffer)->endString16();
  }

  gboolean ags_vst_buffer_make_hex_string(AgsVstBuffer *buffer,
					  gchar **result)
  {
    Steinberg::String *tmp_result_0;
    const Steinberg::String& tmp_result_1 = (const Steinberg::String&) tmp_result_0;

    gboolean retval;
    
    retval = static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->makeHexString(const_cast<Steinberg::String&>(tmp_result_1)));

    if(result != NULL){
      result[0] = (gchar *) tmp_result_0;
    }
    
    return(retval);
  }

  gboolean ags_vst_buffer_from_hex_string(AgsVstBuffer *buffer,
					  gchar *string)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->fromHexString(string)));
  }

  void* ags_vst_buffer_to_buffer(AgsVstBuffer *buffer)
  {
    return(buffer);
  }
  
  ags_vst_tchar* ags_vst_buffer_to_str(AgsVstBuffer *buffer)
  {
    return((ags_vst_tchar *) static_cast<Steinberg::Buffer*>((void *) buffer)->str());
  }
  
  gchar* ags_vst_buffer_to_str8(AgsVstBuffer *buffer)
  {
    return(static_cast<Steinberg::Buffer*>((void *) buffer)->str8());
  }
  
  gunichar2* ags_vst_buffer_to_str16(AgsVstBuffer *buffer)
  {
    return((gunichar2 *) static_cast<Steinberg::Buffer*>((void *) buffer)->str16());
  }
  
  gint8* ags_vst_buffer_to_int8_ptr(AgsVstBuffer *buffer)
  {
    return((gint8 *) static_cast<Steinberg::Buffer*>((void *) buffer)->int8Ptr());
  }
  
  guint8* ags_vst_buffer_to_uint8_ptr(AgsVstBuffer *buffer)
  {
    return((guint8 *) static_cast<Steinberg::Buffer*>((void *) buffer)->uint8Ptr());
  }
  
  gint16* ags_vst_buffer_to_int16_ptr(AgsVstBuffer *buffer)
  {
    return((gint16 *) static_cast<Steinberg::Buffer*>((void *) buffer)->int16Ptr());
  }
  
  guint16* ags_vst_buffer_to_uint16_ptr(AgsVstBuffer *buffer)
  {
    return((guint16 *) static_cast<Steinberg::Buffer*>((void *) buffer)->uint16Ptr());
  }
  
  gint32* ags_vst_buffer_to_int32_ptr(AgsVstBuffer *buffer)
  {
    return((gint32 *) static_cast<Steinberg::Buffer*>((void *) buffer)->int32Ptr());
  }
  
  guint32* ags_vst_buffer_to_uint32_ptr(AgsVstBuffer *buffer)
  {
    return((guint32 *) static_cast<Steinberg::Buffer*>((void *) buffer)->uint32Ptr());
  }
  
  gfloat* ags_vst_buffer_to_float_ptr(AgsVstBuffer *buffer)
  {
    return(static_cast<Steinberg::Buffer*>((void *) buffer)->floatPtr());
  }
  
  gdouble* ags_vst_buffer_to_double_ptr(AgsVstBuffer *buffer)
  {
    return(static_cast<Steinberg::Buffer*>((void *) buffer)->doublePtr());
  }
  
  gunichar2* ags_vst_buffer_to_wchar_ptr(AgsVstBuffer *buffer)
  {
    return((gunichar2 *) static_cast<Steinberg::Buffer*>((void *) buffer)->wcharPtr());
  }
    
  gboolean ags_vst_buffer_swap(AgsVstBuffer *buffer,
			       gint16 swap_size)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->swap(swap_size)));
  }

  gboolean ags_vst_buffer_swap_buffer(AgsVstBuffer *buffer,
				      void *ptr_buffer, guint32 buffer_size, gint16 swap_size)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->swap(ptr_buffer, buffer_size, swap_size)));
  }
  
  void ags_vst_buffer_take(AgsVstBuffer *buffer,
			   AgsVstBuffer *from)
  {
    Steinberg::Buffer *tmp_buffer_0 = static_cast<Steinberg::Buffer*>((void *) from);
    const Steinberg::Buffer& tmp_buffer_1 = (const Steinberg::Buffer&) tmp_buffer_0;

    static_cast<Steinberg::Buffer*>((void *) buffer)->take(const_cast<Steinberg::Buffer&>(tmp_buffer_1));
  }
  
  gint8* ags_vst_buffer_pass(AgsVstBuffer *buffer)
  {
    return((gint8 *) static_cast<Steinberg::Buffer*>((void *) buffer)->pass());
  }

  gboolean ags_vst_buffer_to_wide_string(AgsVstBuffer *buffer,
					 gint32 source_code_page)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->toWideString(source_code_page)));
  }

  gboolean ags_vst_buffer_to_multibyte_string(AgsVstBuffer *buffer,
					      gint32 dest_code_page)
  {
    return(static_cast<gboolean>(static_cast<Steinberg::Buffer*>((void *) buffer)->toMultibyteString(dest_code_page)));
  }
  
}
