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

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_istring_result.h>

#include <pluginterfaces/base/istringresult.h>

extern "C" {

  /**
   * Get IID.
   * 
   * @return the Steinberg::TUID as AgsVstTUID
   *
   * @since 5.0.0
   */
  AgsVstTUID*
  ags_vst_istring_result_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::IStringResult::iid.toTUID()));
  }

  /**
   * Result set text.
   *
   * @param istring_result the string result
   * @param text the text
   *
   * @since 5.0.0
   */
  void ags_vst_istring_result_set_text(AgsVstIStringResult *istring_result, gchar *text)
  {
    ((Steinberg::IStringResult *) istring_result)->setText(text);
  }

  /**
   * Get IID.
   * 
   * @return the Steinberg::TUID as AgsVstTUID
   *
   * @since 5.0.0
   */
  AgsVstTUID*
  ags_vst_istring_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::IString::iid.toTUID()));
  }

  /**
   * Set text.
   *
   * @param istring the string
   * @param text the text
   *
   * @since 5.0.0
   */
  void ags_vst_istring_set_text8(AgsVstIString *istring, gchar *text)
  {
    ((Steinberg::IString *) istring)->setText8(text);
  }

  /**
   * Set unicode text.
   *
   * @param istring the string
   * @param text the unicode text
   *
   * @since 5.0.0
   */
  void ags_vst_istring_set_text16(AgsVstIString *istring, gunichar2 *text)
  {
    ((Steinberg::IString *) istring)->setText16((char16_t *) text);
  }

  /**
   * Get text.
   *
   * @param istring the string
   * @return the text
   *
   * @since 5.0.0
   */
  const gchar* ags_vst_istring_get_text8(AgsVstIString *istring)
  {
    return(((Steinberg::IString *) istring)->getText8());
  }

  /**
   * Get unicode text.
   *
   * @param istring the string
   * @return the unicode text
   *
   * @since 5.0.0
   */
  const gunichar2* ags_vst_istring_get_text16(AgsVstIString *istring)
  {
    return((gunichar2 *) ((Steinberg::IString *) istring)->getText16());
  }

  /**
   * Take.
   *
   * @param istring the string
   * @param s a pointer
   * @param is_wide true if unicode, otherwise false
   *
   * @since 5.0.0
   */
  void ags_vst_istring_take(AgsVstIString *istring, void *s, gboolean is_wide)
  {
    ((Steinberg::IString *) istring)->take(s, is_wide);
  }

  /**
   * Is wide string.
   *
   * @param istring the string
   * @return true if unicode, otherwise false
   *
   * @since 5.0.0
   */
  gboolean ags_vst_istring_is_wide_string(AgsVstIString *istring)
  {
    return(((Steinberg::IString *) istring)->isWideString());
  }
  
}
