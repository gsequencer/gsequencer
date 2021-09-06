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

#ifndef __AGS_VST_FTYPES_H__
#define __AGS_VST_FTYPES_H__

#include <glib.h>

// #define AGS_VST_UNICODE_OFF   // disable / enable unicode

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

#ifdef AGS_VST_UNICODE
#include <wchar.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

  extern const gint32 ags_vst_kmax_long;
  extern const gint32 ags_vst_kmin_long;
  extern const gint32 ags_vst_kmax_int32;
  extern const gint32 ags_vst_kmin_int32;
  extern const guint32 ags_vst_kmax_int32u;

  extern const gint64 ags_vst_kmax_int64;
  extern const gint64 ags_vst_kmin_int64;
  extern const guint64 ags_vst_kmax_int64u;
  
  typedef gint64 AgsVstTSize;
  typedef gint32 AgsVstTResult;
  
#if SMTG_PLATFORM_64
  typedef guint64 AgsVstTPtrInt;
#else
  typedef guint32 AgsVstTPtrInt;
#endif  
  
  extern const float ags_vst_kmax_float;
  extern const double ags_vst_kmax_double;

#ifdef AGS_VST_UNICODE
  typedef wchar_t ags_vst_tchar;
#else
  typedef gchar ags_vst_tchar;
#endif

  typedef const gchar* AgsVstCStringA;
  typedef const gunichar2* AgsVstCStringW;
  typedef const char* AgsVstCString;
  gboolean ags_vst_str_empty(ags_vst_tchar *str);
  gboolean ags_vst_str8_empty(gchar *str);
  gboolean ags_vst_str16_empty(gunichar2 *str);

  typedef const gchar* AgsVstFIDString;

  extern const AgsVstFIDString ags_vst_kplatform_string_win;
  extern const AgsVstFIDString ags_vst_kplatform_string_mac;
  extern const AgsVstFIDString ags_vst_kplatform_string_ios;
  extern const AgsVstFIDString ags_vst_kplatform_string_linux;
#if SMTG_OS_WINDOWS
  extern const AgsVstFIDString ags_vst_kplatform_string;
#elif SMTG_OS_IOS
  extern const AgsVstFIDString ags_vst_kplatform_string;
#elif SMTG_OS_MACOS
  extern const AgsVstFIDString ags_vst_kplatform_string;
#elif SMTG_OS_LINUX
  extern const AgsVstFIDString ags_vst_kplatform_string;
#endif

  typedef gint32 AgsVstUCoord;
  extern const AgsVstUCoord ags_vst_kmax_coord;
  extern const AgsVstUCoord ags_vst_kmin_coord;
  
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
