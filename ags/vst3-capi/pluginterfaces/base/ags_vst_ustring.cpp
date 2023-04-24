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

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ustring.h>

#include <pluginterfaces/base/ustring.h>

extern "C" {
  
  /**
   * Instantiate Steinberg::UString and return a C99 compatible handle
   * AgsVstUstring a void pointer.
   *
   * @return the new instance of Steinberg::UString as AgsVstUstring
   * 
   * @since 5.0.0
   */
  AgsVstUstring* ags_vst_ustring_new_from_buffer(gunichar2 *buffer, gint32 size)
  {
    return((AgsVstUstring *) new Steinberg::UString((char16_t *) buffer, size));
  }

  /**
   * Get size.
   *
   * @param ustring the u-string
   * @return the size
   * 
   * @since 5.0.0
   */
  gint32 ags_vst_ustring_get_size(AgsVstUstring *ustring)
  {
    return(((Steinberg::UString *) ustring)->getSize());
  }

  /**
   * Get length.
   *
   * @param ustring the u-string
   * @return the length
   * 
   * @since 5.0.0
   */
  gint32 ags_vst_ustring_get_length(AgsVstUstring *ustring)
  {
    return(((Steinberg::UString *) ustring)->getLength());
  }

  /**
   * Assign.
   *
   * @param ustring the u-string
   * @param src the source
   * @param src_size the source size
   * @return the AgsVstUstring
   * 
   * @since 5.0.0
   */
  AgsVstUstring* ags_vst_ustring_assign(AgsVstUstring *ustring, gunichar2 *src, gint32 src_size)
  {
    return((AgsVstUstring *) &(((Steinberg::UString *) ustring)->assign((char16_t *) src, src_size)));
  }

  /**
   * Append.
   *
   * @param ustring the u-string
   * @param src the source
   * @param src_size the source size
   * @return the AgsVstUstring
   * 
   * @since 5.0.0
   */
  AgsVstUstring* ags_vst_ustring_append(AgsVstUstring *ustring, gunichar2 *src, gint32 src_size)
  {
    return((AgsVstUstring *) &(((Steinberg::UString *) ustring)->append((char16_t *) src, src_size)));
  }

  /**
   * Copy to.
   *
   * @param ustring the u-string
   * @param src the source
   * @param src_size the source size
   * @return the AgsVstUstring
   * 
   * @since 5.0.0
   */
  AgsVstUstring* ags_vst_ustring_copy_to(AgsVstUstring *ustring, gunichar2 *dst, gint32 dst_size)
  {
    return((AgsVstUstring *) &(((Steinberg::UString *) ustring)->copyTo((char16_t *) dst, dst_size)));
  }

  /**
   * From ASCII.
   *
   * @param ustring the u-string
   * @param src the source
   * @param src_size the source size
   * @return the AgsVstUstring
   * 
   * @since 5.0.0
   */
  AgsVstUstring* ags_vst_ustring_from_ascii(AgsVstUstring *ustring, const gchar *src, gint32 src_size)
  {
    return((AgsVstUstring *) &(((Steinberg::UString *) ustring)->fromAscii(src, src_size)));
  }

  /**
   * Assign from ASCII.
   *
   * @param ustring the u-string
   * @param src the source
   * @param src_size the source size
   * @return the AgsVstUstring
   * 
   * @since 5.0.0
   */
  AgsVstUstring* ags_vst_ustring_assign_from_ascii(AgsVstUstring *ustring, const gchar *src, gint32 src_size)
  {
    return((AgsVstUstring *) &(((Steinberg::UString *) ustring)->assign(src, src_size)));
  }

  /**
   * To ASCII.
   *
   * @param ustring the u-string
   * @param dst the destination
   * @param dst_size the destination size
   * @return the AgsVstUstring
   * 
   * @since 5.0.0
   */
  AgsVstUstring* ags_vst_ustring_to_ascii(AgsVstUstring *ustring, gchar *dst, gint32 dst_size)
  {
    return((AgsVstUstring *) &(((Steinberg::UString *) ustring)->toAscii(dst, dst_size)));
  }

  /**
   * Scan int.
   *
   * @param ustring the u-string
   * @param value the value
   * @return true on success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_ustring_scan_int(AgsVstUstring *ustring, gint64 *value)
  {
    Steinberg::int64 tmp_value;
    gboolean retval;

    tmp_value = 0;    
    
    retval = ((Steinberg::UString *) ustring)->scanInt(tmp_value);

    if(value != NULL){
      value[0] = tmp_value;
    }
    
    return(retval);
  }

  /**
   * Print int.
   *
   * @param ustring the u-string
   * @param value the value
   * @return true on success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_ustring_print_int(AgsVstUstring *ustring, gint64 value)
  {
    return(((Steinberg::UString *) ustring)->printInt(value));
  }

  /**
   * Scan float.
   *
   * @param ustring the u-string
   * @param value the value
   * @return true on success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_ustring_scan_float(AgsVstUstring *ustring, gdouble *value)
  {
    gdouble tmp_value;
    gboolean retval;

    retval = ((Steinberg::UString *) ustring)->scanFloat(tmp_value);

    if(value != NULL){
      value[0] = tmp_value;
    }
    
    return(retval);
  }

  /**
   * Print float.
   *
   * @param ustring the u-string
   * @param value the value
   * @return true on success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_ustring_print_float(AgsVstUstring *ustring, gdouble value)
  {
    return(((Steinberg::UString *) ustring)->printFloat(value));
  }
  
}
