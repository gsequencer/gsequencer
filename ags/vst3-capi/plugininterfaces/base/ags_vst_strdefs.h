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

#ifndef __AGS_VST_STRDEFS_H__
#define __AGS_VST_STRDEFS_H__

#ifdef __cplusplus
extern "C" {
#endif

  static const ags_vst_tchar ags_vst_kempty_string[] = { 0 };
  static const gchar ags_vst_kempty_string8[] = { 0 };
  static const gunichar2 ags_vst_kempty_string16[] = { 0 };

#ifdef UNICODE
  static const ags_vst_tchar ags_vst_kInfiniteSymbol[] = { 0x221E, 0 };
#else
  static const ags_vst_tchar* const ags_vst_kInfiniteSymbol = STR ("oo");
#endif

  gint32 ags_vst_tstrlen(ags_vst_tchar *str);
  gint32 ags_vst_strlen8(gchar *str);
  gint32 ags_vst_strlen16(gunichar2 *str);

  gint32 ags_vst_tstrcmp(ags_vst_tchar *src, ags_vst_tchar *dst);
  gint32 ags_vst_strcmp8(gchar *src, gchar *dst);
  gint32 ags_vst_strcmp16(gunichar2 *src, gunichar2 *dst);
  
  gint32 ags_vst_tstrncmp(ags_vst_tchar *first, ags_vst_tchar *last, guint32 count);
  gint32 ags_vst_strncmp8(gchar *first, gchar *last, guint32 count);
  gint32 ags_vst_strncmp16(gunichar2 *first, gunichar2 *last, guint32 count);

  ags_vst_tchar* ags_vst_tstrcpy(ags_vst_tchar *dst, ags_vst_tchar *src);
  gchar* ags_vst_strcpy8(gchar *dst, gchar *src);
  gunichar2* ags_vst_strcpy16(gunichar2 *dst, gunichar2 *src);

  ags_vst_tchar* ags_vst_tstrncpy(ags_vst_tchar* dest, ags_vst_tchar* source, guint32 count);
  gchar* ags_vst_strncpy8(gchar* dest, gchar* source, guint32 count);
  gunichar2* ags_vst_strncpy16(gunichar2* dest, gunichar2* source, guint32 count);

  ags_vst_tchar* ags_vst_tstrcat(ags_vst_tchar *dst, ags_vst_tchar *src);
  gchar* ags_vst_strcat8(gchar *dst, gchar *src);
  gunichar2* ags_vst_strcat16(gunichar2 *dst, gunichar2 *src);

  gunichar2* ags_vst_str8_to_str16(gchar *str);

  bool ags_vst_FIDStringsEqual(AgsVstFIDString id1, AgsVstFIDString id2);

  static const guint32 ags_vst_kprintf_buffer_size = 4096;

#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_STRDEFS_H__*/
