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

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ipersistent.h>

#include <pluginterfaces/base/ipersistent.h>
#include <pluginterfaces/base/fvariant.h>

extern "C" {

  const AgsVstTUID*
  ags_vst_ipersistent_get_iid()
  {
    return(reinterpret_cast<const AgsVstTUID*>(&INLINE_UID_OF(Steinberg::IPersistent)));
  }

  AgsVstTResult ags_vst_ipersistent_load_attributes(AgsVstIPersistent *persistent,
						    AgsVstIAttributes *attributes)
  {
    return(static_cast<Steinberg::IPersistent*>((void *) persistent)->loadAttributes(static_cast<Steinberg::IAttributes*>((void *) attributes)));
  }

  const AgsVstTUID*
  ags_vst_iattributes_get_iid()
  {
    return(reinterpret_cast<const AgsVstTUID*>(&INLINE_UID_OF(Steinberg::IAttributes)));
  }

  AgsVstTResult ags_vst_iattributes_set(AgsVstIAttributes *attr,
					AgsVstIAttrID attr_id, AgsVstFVariant *data)
  {
    const Steinberg::FVariant& tmp_data = static_cast<Steinberg::FVariant>((void *) data);
    
    return(static_cast<Steinberg::IAttributes*>((void *) attr)->set(attr_id, tmp_data));
  }

  AgsVstTResult ags_vst_iattributes_queue(AgsVstIAttributes *attr,
					  AgsVstIAttrID list_id, AgsVstFVariant *data)
  {
    const Steinberg::FVariant& tmp_data = static_cast<Steinberg::FVariant>((void *) data);
    
    return(static_cast<Steinberg::IAttributes*>((void *) attr)->queue(list_id, tmp_data));
  }

  AgsVstTResult ags_vst_iattributes_set_binary_data(AgsVstIAttributes *attr,
						    AgsVstIAttrID attr_id, void *data, guint32 bytes, gboolean copy_bytes)
  {
    return(static_cast<Steinberg::IAttributes*>((void *) attr)->setBinaryData(attr_id, data, bytes, copy_bytes));
  }

  AgsVstTResult ags_vst_iattributes_get(AgsVstIAttributes *attr,
					AgsVstIAttrID attr_id, AgsVstFVariant *data)
  {
    Steinberg::FVariant *tmp_data_0 = static_cast<Steinberg::FVariant*>((void *) data);
    const Steinberg::FVariant& tmp_data_1 = tmp_data_0;

    return(static_cast<Steinberg::IAttributes*>((void *) attr)->get(attr_id, const_cast<Steinberg::FVariant&>(tmp_data_1)));
  }

  AgsVstTResult ags_vst_iattributes_unqueue(AgsVstIAttributes *attr,
					    AgsVstIAttrID list_id, AgsVstFVariant *data)
  {
    Steinberg::FVariant *tmp_data_0 = static_cast<Steinberg::FVariant*>((void *) data);
    const Steinberg::FVariant& tmp_data_1 = tmp_data_0;

    return(static_cast<Steinberg::IAttributes*>((void *) attr)->unqueue(list_id, const_cast<Steinberg::FVariant&>(tmp_data_1)));
  }

  gint32 ags_vst_iattributes_get_queue_item_count(AgsVstIAttributes *attr,
						  AgsVstIAttrID attr_id)
  {
    return(static_cast<Steinberg::IAttributes*>((void *) attr)->getQueueItemCount(attr_id));
  }
        
  AgsVstTResult ags_vst_iattributes_reset_queue(AgsVstIAttributes *attr,
						AgsVstIAttrID attr_id)
  {
    return(static_cast<Steinberg::IAttributes*>((void *) attr)->resetQueue(attr_id));
  }

  AgsVstTResult ags_vst_iattributes_reset_all_queues(AgsVstIAttributes *attr)
  {
    return(static_cast<Steinberg::IAttributes*>((void *) attr)->resetAllQueues());
  }

  AgsVstTResult ags_vst_iattributes_get_binary_data(AgsVstIAttributes *attr,
						    AgsVstIAttrID attr_id, void *data, guint32 bytes)
  {
    return(static_cast<Steinberg::IAttributes*>((void *) attr)->getBinaryData(attr_id, data, bytes));
  }

  guint32 ags_vst_iattributes_get_binary_data_size(AgsVstIAttributes *attr,
						   AgsVstIAttrID attr_id)
  {
    return(static_cast<Steinberg::IAttributes*>((void *) attr)->getBinaryDataSize(attr_id));
  }

  const AgsVstTUID*
  ags_vst_iattributes2_get_iid()
  {
    return(reinterpret_cast<const AgsVstTUID*>(&INLINE_UID_OF(Steinberg::IAttributes2)));
  }

  gint32 ags_vst_iattributes2_count_attributes(AgsVstIAttributes2 *attr)
  {
    return(static_cast<Steinberg::IAttributes2*>((void *) attr)->countAttributes());
  }

  AgsVstIAttrID ags_vst_iattributes2_get_attribute_id(AgsVstIAttributes2 *attr,
						      gint32 index)
  {
    return(static_cast<Steinberg::IAttributes2*>((void *) attr)->getAttributeID(index));
  }

}
