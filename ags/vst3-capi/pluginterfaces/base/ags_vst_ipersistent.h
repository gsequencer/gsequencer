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

#ifndef __AGS_VST_IPERSISTENT_H__
#define __AGS_VST_IPERSISTENT_H__

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_fvariant.h>

#define AGS_VST_IPERSISTENT_IID (ags_vst_ipersistent_get_iid())
#define AGS_VST_IATTRIBUTES_IID (ags_vst_iattributes_get_iid())
#define AGS_VST_IATTRIBUTES2_IID (ags_vst_iattributes2_get_iid())

#ifdef __cplusplus
extern "C" {
#endif

  typedef const gchar* AgsVstIAttrID;
  typedef struct AgsVstIPersistent IPersistent;  
  typedef struct AgsVstIAttributes IAttributes;
  typedef struct AgsVstIAttributes2 IAttributes2;

  const AgsVstTUID* ags_vst_ipersistent_get_iid();
  
  AgsVstTResult ags_vst_ipersistent_load_attributes(AgsVstIPersistent *persistent,
						    AgsVstIAttributes *attributes);

  const AgsVstTUID* ags_vst_iattributes_get_iid();

  AgsVstTResult ags_vst_iattributes_set(AgsVstIAttributes *attr,
					AgsVstIAttrID attr_id, AgsVstFVariant *data);

  AgsVstTResult ags_vst_iattributes_queue(AgsVstIAttributes *attr,
					  AgsVstIAttrID list_id, AgsVstFVariant *data);

  AgsVstTResult ags_vst_iattributes_set_binary_data(AgsVstIAttributes *attr,
						    AgsVstIAttrID attr_id, void *data, guint32 bytes, gboolean copy_bytes);

  AgsVstTResult ags_vst_iattributes_get(AgsVstIAttributes *attr,
					AgsVstIAttrID attr_id, AgsVstFVariant *data);

  AgsVstTResult ags_vst_iattributes_unqueue(AgsVstIAttributes *attr,
					    AgsVstIAttrID list_id, AgsVstFVariant *data);

  gint32 ags_vst_iattributes_get_queue_item_count(AgsVstIAttributes *attr,
						  AgsVstIAttrID attr_id);
        
  AgsVstTResult ags_vst_iattributes_reset_queue(AgsVstIAttributes *attr,
						AgsVstIAttrID attr_id);
  AgsVstTResult ags_vst_iattributes_reset_all_queues(AgsVstIAttributes *attr);

  AgsVstTResult ags_vst_iattributes_get_binary_data(AgsVstIAttributes *attr,
						    AgsVstIAttrID attr_id, void *data, guint32 bytes);
  guint32 ags_vst_iattributes_get_binary_data_size(AgsVstIAttributes *attr,
						   AgsVstIAttrID attr_id);

  const AgsVstTUID* ags_vst_iattributes2_get_iid();

  gint32 ags_vst_iattributes2_count_attributes(AgsVstIAttributes2 *attr);
  AgsVstIAttrID ags_vst_iattributes2_get_attribute_id(AgsVstIAttributes2 *attr,
						      gint32 index);

#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_IPERSISTENT_H__*/
