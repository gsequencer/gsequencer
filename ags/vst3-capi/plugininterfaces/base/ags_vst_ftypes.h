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

#ifndef __AGS_VST_FTYPES_H__
#define __AGS_VST_FTYPES_H__

//#define UNICODE_OFF   // disable / enable unicode

#ifdef AGS_VST_UNICODE_OFF
#ifdef AGS_VST_UNICODE
#undef AGS_VST_UNICODE
#endif
#else
#define AGS_VST_UNICODE 1
#endif

#ifdef AGS_VST_UNICODE
#define _AGS_VST_UNICODE 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

  static const gint32 ags_vst_kmax_long = 0x7fffffff;
  static const gint32 ags_vst_kmin_long = (-0x7fffffff - 1);
  static const gint32 ags_vst_kmax_int32 = kMaxLong;
  static const gint32 ags_vst_kmin_int32 = kMinLong;
  static const guint32 ags_vst_kmax_int32u = 0xffffffff;

  static const gint64 ags_vst_kmax_int64 = 0x7fffffffffffffffLL;
  static const gint64 ags_vst_kmin_int64 = (-0x7fffffffffffffffLL-1);
  static const guint64 ags_vst_kmax_int64u = uint64 (0xffffffff) | (uint64 (0xffffffff) << 32);
  
  typedef gint64 AgsVstTSize;

#if SMTG_PLATFORM_64
  typedef guint64 AgsVstTPtrInt;
#else
  typedef guint32 AgsVstTPtrInt;
#endif  

  static const float ags_vst_kmax_float = 3.40282346638528860E38;
  static const double ags_vst_kmax_double = 1.7976931348623158E308;

#ifdef AGS_VST_UNICODE
  typedef gunichar2 tchar;
#else
  typedef gchar tchar;
#endif

  typedef const gchar* AgsVstCStringA;
  typedef const gunichar2* AgsVstCStringW;
  typedef const tchar* AgsVstCString;
  inline bool strEmpty (const tchar* str) { return (!str || *str == 0); }
  inline bool str8Empty (const char8* str) { return (!str || *str == 0); }
  inline bool str16Empty (const char16* str) { return (!str || *str == 0); }

  typedef const gchar* AgsVstFIDString;

  const AgsVstFIDString ags_vst_kplatform_string_win = "WIN";
  const AgsVstFIDString ags_vst_kplatform_string_mac = "MAC";
  const AgsVstFIDString ags_vst_kplatform_string_ios = "IOS";
  const AgsVstFIDString ags_vst_kplatform_string_linux = "Linux";
#if SMTG_OS_WINDOWS
  const AgsVstFIDString ags_vst_kplatform_string = ags_vst_kplatform_string_win;
#elif SMTG_OS_IOS
  const AgsVstFIDString ags_vst_kplatform_string = ags_vst_kplatform_string_ios;
#elif SMTG_OS_MACOS
  const AgsVstFIDString ags_vst_kplatform_string = ags_vst_kplatform_string_mac;
#elif SMTG_OS_LINUX
  const AgsVstFIDString ags_vst_kplatform_string = ags_vst_kplatform_string_linux;
#endif

  typedef gint32 AgsVstUCoord;
  static const AgsVstUCoord ags_vst_kmax_coord = ((AgsVstUCoord) 0x7FFFFFFF);
  static const AgsVstUCoord ags_vst_kmin_coord = ((AgsVstUCoord) -0x7FFFFFFF);
  
#ifdef __cplusplus
}
#endif

#define AGS_VST_SWAP_32(l) { \
        unsigned char* p = (unsigned char*)& (l); \
        unsigned char t; \
        t = p[0]; p[0] = p[3]; p[3] = t; t = p[1]; p[1] = p[2]; p[2] = t; }

#define AGS_VST_SWAP_16(w) { \
        unsigned char* p = (unsigned char*)& (w); \
        unsigned char t; \
        t = p[0]; p[0] = p[1]; p[1] = t; }

#define AGS_VST_SWAP_64(i) { \
        unsigned char* p = (unsigned char*)& (i); \
        unsigned char t; \
        t = p[0]; p[0] = p[7]; p[7] = t; t = p[1]; p[1] = p[6]; p[6] = t; \
        t = p[2]; p[2] = p[5]; p[5] = t; t = p[3]; p[3] = p[4]; p[4] = t;}


#endif /*__AGS_VST_FTYPES_H__*/
