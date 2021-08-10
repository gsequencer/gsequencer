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

#ifndef __AGS_VST_IATTRIBUTES_H__
#define __AGS_VST_IATTRIBUTES_H__

#include <glib.h>

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_types.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef const gchar* AgsVstAttrID;
  typedef struct AgsVstIAttributeList IAttributeList;
  typedef struct AgsVstIStreamAttributes IStreamAttributes;
  
  const AgsVstTUID* ags_vst_iattribute_list_get_iid();

  AgsVstTResult ags_vst_iattribute_list_set_int(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, gint64 value);
  AgsVstTResult ags_vst_iattribute_list_get_int(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, gint64 *value);

  AgsVstTResult ags_vst_iattribute_list_set_float(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, gdouble value);
  AgsVstTResult ags_vst_iattribute_list_get_float(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, gdouble *value);

  AgsVstTResult ags_vst_iattribute_list_set_string(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, AgsVstTChar *value);
  AgsVstTResult ags_vst_iattribute_list_get_string(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, AgsVstTChar *value, guint32 size_in_bytes);

  AgsVstTResult ags_vst_iattribute_list_set_binary(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, const void *data, guint32 size_in_bytes);
  AgsVstTResult ags_vst_iattribute_list_get_binary(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, const void *data, guint32 size_in_bytes);

  const AgsVstTUID* ags_vst_istream_attributes_get_iid();

  AgsVstIAttributeList* ags_vst_istream_attributes_get_attributes(AgsVstIStreamAttributes *istream_attributes);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_IATTRIBUTES_H__*/
