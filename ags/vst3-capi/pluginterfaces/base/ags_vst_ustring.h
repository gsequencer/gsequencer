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

#ifndef __AGS_VST_USTRING_H__
#define __AGS_VST_USTRING_H__

#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct UString AgsVstUstring;

  typedef struct UString128 AgsVstUstring128;
  typedef struct UString256 AgsVstUstring256;
  
  AgsVstUstring* ags_vst_ustring_new_from_buffer(gunichar2 *buffer, gint32 size);

  gint32 ags_vst_ustring_get_size(AgsVstUstring *ustring);

  gint32 ags_vst_ustring_get_length(AgsVstUstring *ustring);

  AgsVstUstring* ags_vst_ustring_assign(AgsVstUstring *ustring, gunichar2 *src, gint32 src_size);
  AgsVstUstring* ags_vst_ustring_append(AgsVstUstring *ustring, gunichar2 *src, gint32 src_size);

  AgsVstUstring* ags_vst_ustring_copy_to(AgsVstUstring *ustring, gunichar2 *dst, gint32 dst_size);

  AgsVstUstring* ags_vst_ustring_from_ascii(AgsVstUstring *ustring, const gchar *src, gint32 src_size);
  AgsVstUstring* ags_vst_ustring_assign_from_ascii(AgsVstUstring *ustring, const gchar *src, gint32 src_size);

  AgsVstUstring* ags_vst_ustring_to_ascii(AgsVstUstring *ustring, gchar *dst, gint32 dst_size);

  gboolean ags_vst_ustring_scan_int(AgsVstUstring *ustring, gint64 *value);
  gboolean ags_vst_ustring_print_int(AgsVstUstring *ustring, gint64 value);

  gboolean ags_vst_ustring_scan_float(AgsVstUstring *ustring, gdouble *value);
  gboolean ags_vst_ustring_print_float(AgsVstUstring *ustring, gdouble value);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_USTRING_H__*/
