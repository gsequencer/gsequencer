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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_imessage.h>

#include <pluginterfaces/vst/ivstmessage.h>

extern "C" {
  
  const AgsVstTUID* ags_vst_imessage_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IMessage::iid.toTUID()));
  }

  AgsVstFIDString ags_vst_imessage_get_message_id(AgsVstIMessage *imessage)
  {
    return(((Steinberg::Vst::IMessage *) imessage)->getMessageID());
  }

  void ags_vst_imessage_set_message_id(AgsVstIMessage *imessage,
				       AgsVstFIDString id)
  {
    return(((Steinberg::Vst::IMessage *) imessage)->setMessageID(id));
  }
  
  AgsVstIAttributeList* ags_vst_imessage_get_attributes(AgsVstIMessage *imessage)
  {
    return((AgsVstIAttributeList *) ((Steinberg::Vst::IMessage *) imessage)->getAttributes());
  }

  const AgsVstTUID* ags_vst_iconnection_point_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IConnectionPoint::iid.toTUID()));
  }

  AgsVstTResult ags_vst_iconnection_point_connect(AgsVstIConnectionPoint *iconnection_point,
						  AgsVstIConnectionPoint *other)
  {
    return(((Steinberg::Vst::IConnectionPoint *) iconnection_point)->connect((Steinberg::Vst::IConnectionPoint *) other));
  }

  AgsVstTResult ags_vst_iconnection_point_disconnect(AgsVstIConnectionPoint *iconnection_point,
						     AgsVstIConnectionPoint *other)
  {
    return(((Steinberg::Vst::IConnectionPoint *) iconnection_point)->disconnect((Steinberg::Vst::IConnectionPoint *) other));
  }

  AgsVstTResult ags_vst_iconnection_point_notify(AgsVstIConnectionPoint *iconnection_point,
						 AgsVstIMessage *imessage)
  {
    return(((Steinberg::Vst::IConnectionPoint *) iconnection_point)->notify((Steinberg::Vst::IMessage *) imessage));
  }

}
