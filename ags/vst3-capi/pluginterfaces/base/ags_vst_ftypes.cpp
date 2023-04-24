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

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ftypes.h>

#include <pluginterfaces/base/ftypes.h>

using namespace Steinberg;

extern "C" {

  const gint32 ags_vst_kmax_long = 0x7fffffff;
  const gint32 ags_vst_kmin_long = (-0x7fffffff - 1);
  const gint32 ags_vst_kmax_int32 = ags_vst_kmax_long;
  const gint32 ags_vst_kmin_int32 = ags_vst_kmin_long;
  const guint32 ags_vst_kmax_int32u = 0xffffffff;

  const gint64 ags_vst_kmax_int64 = 0x7fffffffffffffffLL;
  const gint64 ags_vst_kmin_int64 = (-0x7fffffffffffffffLL-1);
  const guint64 ags_vst_kmax_int64u = (guint64) (0xffffffff) | ((guint64) (0xffffffff) << 32);
    
  const float ags_vst_kmax_float = 3.40282346638528860E38;
  const double ags_vst_kmax_double = 1.7976931348623158E308;

#if SMTG_OS_WINDOWS
  const AgsVstFIDString ags_vst_kplatform_string = ags_vst_kplatform_string_win;
#elif SMTG_OS_IOS
  const AgsVstFIDString ags_vst_kplatform_string = ags_vst_kplatform_string_ios;
#elif SMTG_OS_MACOS
  const AgsVstFIDString ags_vst_kplatform_string = ags_vst_kplatform_string_mac;
#elif SMTG_OS_LINUX
  const AgsVstFIDString ags_vst_kplatform_string = ags_vst_kplatform_string_linux;
#endif
  
  const AgsVstUCoord ags_vst_kmax_coord = ((AgsVstUCoord) 0x7FFFFFFF);
  const AgsVstUCoord ags_vst_kmin_coord = ((AgsVstUCoord) -0x7FFFFFFF);

  /**
   * Empty.
   *
   * @param str the string
   * @return true if empty, otherwise false
   *
   * @since 5.0.0
   */
  gboolean ags_vst_str_empty(ags_vst_tchar *str)
  {
    return(static_cast<gboolean>(strEmpty(reinterpret_cast<tchar *>(str))));
  }
  
  /**
   * Empty.
   *
   * @param str the string
   * @return true if empty, otherwise false
   *
   * @since 5.0.0
   */
  gboolean ags_vst_str8_empty(gchar *str)
  {
    return(static_cast<gboolean>(str8Empty(reinterpret_cast<char8 *>(str))));
  }
  
  /**
   * Unicode empty.
   *
   * @param str the string
   * @return true if empty, otherwise false
   *
   * @since 5.0.0
   */
  gboolean ags_vst_str16_empty(gunichar2 *str)
  {
    return(static_cast<gboolean>(str16Empty(reinterpret_cast<char16 *>(str))));
  }
  
}
