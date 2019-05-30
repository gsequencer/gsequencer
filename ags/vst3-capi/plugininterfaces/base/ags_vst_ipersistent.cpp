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

#include <ags/vst3-capi/plugininterfaces/base/ags_vst_funknown.h>

#include <plugininterfaces/base/ipersistent.h>

extern "C" {

  AgsVstTUID
  ags_vst_ipersistent_get_iid()
  {
    return(IPersistent__iid);
  }

  gint32 ags_vst_ipersistent_load_attributes(AgsVstIPersistent *persistent,
					     AgsVstIAttributes *attributes)
  {
    return(persistent->loadAttributes(attributes));
  }

  AgsVstTUID
  ags_vst_iattributes_get_iid()
  {
    return(IAttributes__iid);
  }

  gint32 ags_vst_iattributes_set(AgsVstIAttributes *attr,
				 AgsVstIAttrID attr_id, const AgsVstFVariant **data)
  {
    return(attr->set(attr_id, static_cast<FVariant&>(data[0])));
  }

  gint32 ags_vst_iattributes_queue(AgsVstIAttributes *attr,
				   AgsVstIAttrID list_id, const AgsVstFVariant **data)
  {
    return(attr->queue(list_id, static_cast<FVariant&>(data[0])));
  }

  gint32 ags_vst_iattributes_set_binary_data(AgsVstIAttributes *attr,
					     AgsVstIAttrID attr_id, void *data, guint32 bytes, gboolean copy_bytes)
  {
    return(attr->setBinaryData(attr_id, data, bytes, copy_bytes));
  }

  gint32 ags_vst_iattributes_get(AgsVstIAttributes *attr,
				 AgsVstIAttrID attr_id, AgsVstFVariant **data)
  {
    return(attr->get(attr_id, static_cast<FVariant&>(data[0])));
  }

  gint32 ags_vst_iattributes_unqueue(AgsVstIAttributes *attr,
				     AgsVstIAttrID list_id, AgsVstFVariant **data)
  {
    return(attr->unqueue(list_id, static_cast<FVariant&>(data[0])));
  }

  gint32 ags_vst_iattributes_get_queue_item_count(AgsVstIAttributes *attr,
						  AgsVstIAttrID attr_id)
  {
    return(attr->getQueueItemCount(attr_id));
  }
        
  gint32 ags_vst_iattributes_reset_queue(AgsVstIAttributes *attr,
					 AgsVstIAttrID attr_id)
  {
    return(attr->resetQueue(attr_id));
  }

  gint32 ags_vst_iattributes_reset_all_queues(AgsVstIAttributes *attr)
  {
    return(attr->resetAllQueues());
  }

  gint32 ags_vst_iattributes_get_binary_data(AgsVstIAttributes *attr,
					     AgsVstIAttrID attr_id, void *data, guint32 bytes)
  {
    return(attr->getBinaryData(attr_id, data, bytes));
  }

  guint32 ags_vst_iattributes_get_binary_data_size(AgsVstIAttributes *attr,
						   AgsVstIAttrID attr_id)
  {
    return(attr->getBinaryDataSize(attr_id));
  }

  AgsVstTUID
  ags_vst_iattributes2_get_iid()
  {
    return(IAttributes2__iid);
  }

  gint32 ags_vst_iattributes2_count_attributes(AgsVstIAttributes2 *attr)
  {
    return(attr->countAttributes());
  }

  AgsVstIAttrID ags_vst_iattributes2_get_attribute_id(AgsVstIAttributes2 *attr,
						      gint32 index)
  {
    return(attr->getAttributeID(index));
  }

}
