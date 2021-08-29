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

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_fstrdefs.h>

#include <pluginterfaces/base/fstrdefs.h>

using namespace Steinberg;

extern "C" {

  gint32 ags_vst_tstrlen(ags_vst_tchar *str)
  {
    return(tstrlen((tchar *) str));
  }
  
  gint32 ags_vst_strlen8(gchar *str)
  {
    return(strlen8((char8 *) str));
  }
  
  gint32 ags_vst_strlen16(gunichar2 *str)
  {
    return(strlen16((char16 *) str));
  }
  
  gint32 ags_vst_tstrcmp(ags_vst_tchar *src, ags_vst_tchar *dst)
  {
    return(tstrcmp((tchar *) src, (tchar *) dst));
  }
  
  gint32 ags_vst_strcmp8(gchar *src, gchar *dst)
  {
    return(strcmp8((char8 *) src, (char8 *) dst));
  }
  
  gint32 ags_vst_strcmp16(gunichar2 *src, gunichar2 *dst)
  {
    return(strcmp16((char16 *) src, (char16 *) dst));
  }
  
  gint32 ags_vst_tstrncmp(ags_vst_tchar *first, ags_vst_tchar *last, guint32 count)
  {
    return(tstrncmp((tchar *) first, (tchar *) last, count));
  }
  
  gint32 ags_vst_strncmp8(gchar *first, gchar *last, guint32 count)
  {
    return(strncmp8((char8 *) first, (char8 *) last, count));
  }
  
  gint32 ags_vst_strncmp16(gunichar2 *first, gunichar2 *last, guint32 count)
  {
    return(strncmp16((char16 *) first, (char16 *) last, count));
  }
  
  ags_vst_tchar* ags_vst_tstrcpy(ags_vst_tchar *dst, ags_vst_tchar *src)
  {
    return(tstrcpy((tchar *) dst, (tchar *) src));
  }
  
  gchar* ags_vst_strcpy8(gchar *dst, gchar *src)
  {
    return(strcpy8((char8 *) dst, (char8 *) src));
  }
  
  gunichar2* ags_vst_strcpy16(gunichar2 *dst, gunichar2 *src)
  {
    return((gunichar2 *) strcpy16((char16 *) dst, (char16 *) src));
  }
  
  ags_vst_tchar* ags_vst_tstrncpy(ags_vst_tchar* dest, ags_vst_tchar* source, guint32 count)
  {
    return(tstrncpy((tchar *) dest, (tchar *) source, count));
  }
  
  gchar* ags_vst_strncpy8(gchar* dest, gchar* source, guint32 count)
  {
    return(strncpy8((char8 *) dest, (char8 *) source, count));
  }
  
  gunichar2* ags_vst_strncpy16(gunichar2* dest, gunichar2* source, guint32 count)
  {
    return((gunichar2 *) strncpy16((char16 *) dest, (char16 *) source, count));
  }
  
  ags_vst_tchar* ags_vst_tstrcat(ags_vst_tchar *dst, ags_vst_tchar *src)
  {
    return(tstrcat((tchar *) dst, (tchar *) src));
  }
  
  gchar* ags_vst_strcat8(gchar *dst, gchar *src)
  {
    return(strcat8((char8 *) dst, (char8 *) src));
  }
  
  gunichar2* ags_vst_strcat16(gunichar2 *dst, gunichar2 *src)
  {
    return((gunichar2 *) strcat16((char16 *) dst, (char16 *) src));
  }
  
  void ags_vst_str8_to_str16(gunichar2 *dst, gchar *src, gint32 n)
  {
    return(str8ToStr16((char16 *) dst, (char8 *) src, (int32) n));
  }
  
  gboolean ags_vst_fid_strings_equal(AgsVstFIDString id1, AgsVstFIDString id2)
  {
    return(FIDStringsEqual((FIDString) id1, (FIDString) id2));
  }  
  
}
