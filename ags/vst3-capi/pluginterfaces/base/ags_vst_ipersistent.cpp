/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstTUID
   *
   * @since 5.0.0
   */
  const AgsVstTUID*
  ags_vst_ipersistent_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::IPersistent::iid.toTUID()));
  }

  /**
   * Load attributes.
   *
   * @param persistent the persistent
   * @param attributes the attributes
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_ipersistent_load_attributes(AgsVstIPersistent *persistent,
						    AgsVstIAttributes *attributes)
  {
    return(static_cast<Steinberg::IPersistent*>((void *) persistent)->loadAttributes(static_cast<Steinberg::IAttributes*>((void *) attributes)));
  }

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstTUID
   *
   * @since 5.0.0
   */
  const AgsVstTUID*
  ags_vst_iattributes_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::IAttributes::iid.toTUID()));
  }

  /**
   * Set.
   *
   * @param attr the attributes
   * @param attr_id the attribute identifier
   * @param data the data
   * @return the return code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iattributes_set(AgsVstIAttributes *attr,
					AgsVstIAttrID attr_id, AgsVstFVariant *data)
  {
    const Steinberg::FVariant& tmp_data = static_cast<Steinberg::FVariant>((void *) data);
    
    return(static_cast<Steinberg::IAttributes*>((void *) attr)->set(attr_id, tmp_data));
  }

  /**
   * Queue.
   *
   * @param attr the attributes
   * @param list_id the attribute identifier
   * @param data the data
   * @return the return code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iattributes_queue(AgsVstIAttributes *attr,
					  AgsVstIAttrID list_id, AgsVstFVariant *data)
  {
    const Steinberg::FVariant& tmp_data = static_cast<Steinberg::FVariant>((void *) data);
    
    return(static_cast<Steinberg::IAttributes*>((void *) attr)->queue(list_id, tmp_data));
  }

  /**
   * Set binary data.
   *
   * @param attr the attributes
   * @param attr_id the attribute identifier
   * @param data the data
   * @param bytes bytes
   * @param copy_bytes copy bytes
   * @return the return code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iattributes_set_binary_data(AgsVstIAttributes *attr,
						    AgsVstIAttrID attr_id, void *data, guint32 bytes, gboolean copy_bytes)
  {
    return(static_cast<Steinberg::IAttributes*>((void *) attr)->setBinaryData(attr_id, data, bytes, copy_bytes));
  }

  /**
   * Get.
   *
   * @param attr the attributes
   * @param attr_id the attribute identifier
   * @param data the data
   * @return the return code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iattributes_get(AgsVstIAttributes *attr,
					AgsVstIAttrID attr_id, AgsVstFVariant *data)
  {
    Steinberg::FVariant *tmp_data_0 = static_cast<Steinberg::FVariant*>((void *) data);
    const Steinberg::FVariant& tmp_data_1 = tmp_data_0;

    return(static_cast<Steinberg::IAttributes*>((void *) attr)->get(attr_id, const_cast<Steinberg::FVariant&>(tmp_data_1)));
  }

  /**
   * Unqueue.
   *
   * @param attr the attributes
   * @param list_id the attribute identifier
   * @param data the data
   * @return the return code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iattributes_unqueue(AgsVstIAttributes *attr,
					    AgsVstIAttrID list_id, AgsVstFVariant *data)
  {
    Steinberg::FVariant *tmp_data_0 = static_cast<Steinberg::FVariant*>((void *) data);
    const Steinberg::FVariant& tmp_data_1 = tmp_data_0;

    return(static_cast<Steinberg::IAttributes*>((void *) attr)->unqueue(list_id, const_cast<Steinberg::FVariant&>(tmp_data_1)));
  }

  /**
   * Get item count.
   *
   * @param attr the attributes
   * @param attr_id the attribute identifier
   * @return the count
   *
   * @since 5.0.0
   */
  gint32 ags_vst_iattributes_get_queue_item_count(AgsVstIAttributes *attr,
						  AgsVstIAttrID attr_id)
  {
    return(static_cast<Steinberg::IAttributes*>((void *) attr)->getQueueItemCount(attr_id));
  }
        
  /**
   * Reset queue.
   *
   * @param attr the attributes
   * @param attr_id the attribute identifier
   * @return the return code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iattributes_reset_queue(AgsVstIAttributes *attr,
						AgsVstIAttrID attr_id)
  {
    return(static_cast<Steinberg::IAttributes*>((void *) attr)->resetQueue(attr_id));
  }

  /**
   * Reset all queues.
   *
   * @param attr the attributes
   * @return the return code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iattributes_reset_all_queues(AgsVstIAttributes *attr)
  {
    return(static_cast<Steinberg::IAttributes*>((void *) attr)->resetAllQueues());
  }

  /**
   * Get binary data.
   *
   * @param attr the attributes
   * @param attr_id the attribute identifier
   * @param data the data
   * @param bytes bytes
   * @return the return code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iattributes_get_binary_data(AgsVstIAttributes *attr,
						    AgsVstIAttrID attr_id, void *data, guint32 bytes)
  {
    return(static_cast<Steinberg::IAttributes*>((void *) attr)->getBinaryData(attr_id, data, bytes));
  }

  /**
   * Get binary data size.
   *
   * @param attr the attributes
   * @param attr_id the attribute identifier
   * @return the size
   *
   * @since 5.0.0
   */
  guint32 ags_vst_iattributes_get_binary_data_size(AgsVstIAttributes *attr,
						   AgsVstIAttrID attr_id)
  {
    return(static_cast<Steinberg::IAttributes*>((void *) attr)->getBinaryDataSize(attr_id));
  }

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstTUID
   *
   * @since 5.0.0
   */
  const AgsVstTUID*
  ags_vst_iattributes2_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::IAttributes2::iid.toTUID()));
  }

  /**
   * Count attributes.
   *
   * @param attr the attributes
   * @return the count
   *
   * @since 5.0.0
   */
  gint32 ags_vst_iattributes2_count_attributes(AgsVstIAttributes2 *attr)
  {
    return(static_cast<Steinberg::IAttributes2*>((void *) attr)->countAttributes());
  }

  /**
   * Get IID.
   *
   * @param attr the attributes
   * @para index the index of attribute
   * @return the attribute identifier
   *
   * @since 5.0.0
   */
  AgsVstIAttrID ags_vst_iattributes2_get_attribute_id(AgsVstIAttributes2 *attr,
						      gint32 index)
  {
    return(static_cast<Steinberg::IAttributes2*>((void *) attr)->getAttributeID(index));
  }

}
