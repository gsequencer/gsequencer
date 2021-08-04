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

#ifndef __AGS_VST_FVARIANT_H__
#define __AGS_VST_FVARIANT_H__

#include <glib.h>

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_fstrdefs.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct AgsVstFVariant FVariant;

  enum
  {
    AGS_VST_KEMPTY = 0,
    AGS_VST_KINTEGER = 1 << 0,
    AGS_VST_KFLOAT = 1 << 1,
    AGS_VST_KSTRING8 = 1 << 2,
    AGS_VST_KOBJECT = 1 << 3,
    AGS_VST_KOWNER = 1 << 4,
    AGS_VST_KSTRING16 = 1 << 5,
  };

  AgsVstFVariant* ags_vst_fvariant_new();
  AgsVstFVariant* ags_vst_fvariant_new_boolean(gboolean val);
  AgsVstFVariant* ags_vst_fvariant_new_uint32(guint32 val);
  AgsVstFVariant* ags_vst_fvariant_new_int64(gint64 val);
  AgsVstFVariant* ags_vst_fvariant_new_double(gdouble val);
  AgsVstFVariant* ags_vst_fvariant_new_string(gchar *str);
  AgsVstFVariant* ags_vst_fvariant_new_wstring(gunichar2 *str);
  AgsVstFVariant* ags_vst_fvariant_new_funknown(AgsVstFUnknown *funknown, gboolean owner);

  void ags_vst_fvariant_delete(AgsVstFVariant *fvariant);

  void ags_vst_fvariant_set_boolean(AgsVstFVariant *fvariant,
				    gboolean val);
  void ags_vst_fvariant_set_uin32(AgsVstFVariant *fvariant,
				  guint32 val);
  void ags_vst_fvariant_set_int64(AgsVstFVariant *fvariant,
				  gint64 val);
  void ags_vst_fvariant_set_double(AgsVstFVariant *fvariant,
				   gdouble val);
  void ags_vst_fvariant_set_string(AgsVstFVariant *fvariant,
				   gchar *str);
  void ags_vst_fvariant_set_wstring(AgsVstFVariant *fvariant,
				    gunichar2 *str);
  void ags_vst_fvariant_set_object(AgsVstFVariant *fvariant,
				   AgsVstFUnknown *obj);

  gboolean ags_vst_fvariant_get_boolean(AgsVstFVariant *fvariant);
  guint32 ags_vst_fvariant_get_uint32(AgsVstFVariant *fvariant);
  gint64 ags_vst_fvariant_get_int64(AgsVstFVariant *fvariant);
  gdouble ags_vst_fvariant_get_double(AgsVstFVariant *fvariant);
  gchar* ags_vst_fvariant_get_string(AgsVstFVariant *fvariant);
  gunichar2* ags_vst_fvariant_get_wstring(AgsVstFVariant *fvariant);
  AgsVstFUnknown* ags_vst_fvariant_get_object(AgsVstFVariant *fvariant);

  guint16 ags_vst_fvariant_get_type(AgsVstFVariant *fvariant);
  gboolean ags_vst_fvariant_is_empty(AgsVstFVariant *fvariant);
  gboolean ags_vst_fvariant_is_owner(AgsVstFVariant *fvariant);
  gboolean ags_vst_fvariant_is_string(AgsVstFVariant *fvariant);
  void ags_vst_fvariant_set_owner(AgsVstFVariant *fvariant,
				  gboolean state);

  void ags_vst_fvariant_empty(AgsVstFVariant *fvariant);

#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_FVARIANT_H__*/

