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

#ifndef __AGS_VST_COMPONENT_BASE_H__
#define __AGS_VST_COMPONENT_BASE_H__

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_iplugin_base.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_imessage.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_ihost_application.h>
#include <ags/vst3-capi/base/source/ags_vst_fobject.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct AgsVstComponentBase ComponentBase;
  
  AgsVstComponentBase* ags_vst_component_base_new();
  void ags_vst_component_base_delete(AgsVstComponentBase *component_base);

  AgsVstFUnknown* ags_vst_component_base_get_host_context(AgsVstComponentBase *component_base);

  AgsVstIConnectionPoint* ags_vst_component_base_get_peer(AgsVstComponentBase *component_base);

  AgsVstIMessage* ags_vst_component_base_allocate_message(AgsVstComponentBase *component_base);

  AgsVstTResult ags_vst_component_base_send_message(AgsVstComponentBase *component_base,
						    AgsVstIMessage* message);
  
  AgsVstTResult ags_vst_component_base_send_text_message(AgsVstComponentBase *component_base,
							 gchar *text);

  AgsVstTResult ags_vst_component_base_receive_text(AgsVstComponentBase *component_base,
						    gchar *text);

  AgsVstTResult ags_vst_component_base_initialize(AgsVstComponentBase *component_base,
						  AgsVstFUnknown* context);
  AgsVstTResult ags_vst_component_base_terminate(AgsVstComponentBase *component_base);

  AgsVstTResult ags_vst_component_base_connect(AgsVstComponentBase *component_base,
					       AgsVstIConnectionPoint* other);
  AgsVstTResult ags_vst_component_base_disconnect(AgsVstComponentBase *component_base,
						  AgsVstIConnectionPoint* other);
  AgsVstTResult ags_vst_component_base_notify(AgsVstComponentBase *component_base,
					      AgsVstIMessage* message);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_COMPONENT_BASE_H__*/
