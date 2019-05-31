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

#include <ags/vst3-capi/plugininterfaces/base/ags_vst_fstrdefs.h>

#include <plugininterfaces/base/fstrdefs.h>

extern "C" {

  gint32 ags_vst_tstrlen(ags_vst_tchar *str)
  {
    return(tstrlen(str));
  }
  
  gint32 ags_vst_strlen8(gchar *str)
  {
    return(strlen8(str););
  }
  
  gint32 ags_vst_strlen16(gunichar2 *str)
  {
    return(strlen16(str));
  }
  
  gint32 ags_vst_tstrcmp(ags_vst_tchar *src, ags_vst_tchar *dst)
  {
    return(tstrcmp(src, dst));
  }
  
  gint32 ags_vst_strcmp8(gchar *src, gchar *dst)
  {
    return(strcmp8(src, dst));
  }
  
  gint32 ags_vst_strcmp16(gunichar2 *src, gunichar2 *dst)
  {
    return(strcmp16(src, dst));
  }
  
  gint32 ags_vst_tstrncmp(ags_vst_tchar *first, ags_vst_tchar *last, guint32 count)
  {
    return(tstrncmp(first, last, count));
  }
  
  gint32 ags_vst_strncmp8(gchar *first, gchar *last, guint32 count)
  {
    return(strncmp8(first, last, count));
  }
  
  gint32 ags_vst_strncmp16(gunichar2 *first, gunichar2 *last, guint32 count)
  {
    return(strncmp16(first, last, count));
  }
  
  ags_vst_tchar* ags_vst_tstrcpy(ags_vst_tchar *dst, ags_vst_tchar *src)
  {
    return(tstrcpy(dst, src));
  }
  
  gchar* ags_vst_strcpy8(gchar *dst, gchar *src)
  {
    return(strcpy8(dst, src));
  }
  
  gunichar2* ags_vst_strcpy16(gunichar2 *dst, gunichar2 *src)
  {
    return(strcpy16(dst, src));
  }
  
  ags_vst_tchar* ags_vst_tstrncpy(ags_vst_tchar* dest, ags_vst_tchar* source, guint32 count)
  {
    return(tstrncpy(dest, source, count));
  }
  
  gchar* ags_vst_strncpy8(gchar* dest, gchar* source, guint32 count)
  {
    return(strncpy8(dest, source, count));
  }
  
  gunichar2* ags_vst_strncpy16(gunichar2* dest, gunichar2* source, guint32 count)
  {
    return(strncpy16(dest, source, count));
  }
  
  ags_vst_tchar* ags_vst_tstrcat(ags_vst_tchar *dst, ags_vst_tchar *src)
  {
    return(tstrcat(dst, src));
  }
  
  gchar* ags_vst_strcat8(gchar *dst, gchar *src)
  {
    return(strcat8(dst, src));
  }
  
  gunichar2* ags_vst_strcat16(gunichar2 *dst, gunichar2 *src)
  {
    return(strcat16(dst, src));
  }
  
  gunichar2* ags_vst_str8_to_str16(gchar *str)
  {
    return(str8ToStr16(dst, src));
  }
  
  gboolean ags_vst_fid_strings_equal(AgsVstFIDString id1, AgsVstFIDString id2)
  {
    return(FIDStringsEqual(id1, id2));
  }  
  
}
