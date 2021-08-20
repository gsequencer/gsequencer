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

#ifndef __AGS_VST_IMESSAGE_H__
#define __AGS_VST_IMESSAGE_H__

#include <glib.h>

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iattributes.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct IMessage AgsVstIMessage;

  typedef struct IConnectionPoint AgsVstIConnectionPoint;

  const AgsVstTUID* ags_vst_imessage_get_iid();

  AgsVstFIDString ags_vst_imessage_get_message_id(AgsVstIMessage *imessage);

  void ags_vst_imessage_set_message_id(AgsVstIMessage *imessage,
				       AgsVstFIDString id);
  
  AgsVstIAttributeList* ags_vst_imessage_get_attributes(AgsVstIMessage *imessage);

  const AgsVstTUID* ags_vst_iconnection_point_get_iid();

  AgsVstTResult ags_vst_iconnection_point_connect(AgsVstIConnectionPoint *iconnection_point,
						  AgsVstIConnectionPoint *other);

  AgsVstTResult ags_vst_iconnection_point_disconnect(AgsVstIConnectionPoint *iconnection_point,
						     AgsVstIConnectionPoint *other);

  AgsVstTResult ags_vst_iconnection_point_notify(AgsVstIConnectionPoint *iconnection_point,
						 AgsVstIMessage *imessage);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_IMESSAGE_H__*/
