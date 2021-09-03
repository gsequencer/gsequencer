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

#ifndef __AGS_VST_COMPONENT_H__
#define __AGS_VST_COMPONENT_H__

#include <glib.h>

#include <ags/vst3-capi/public.sdk/source/vst/ags_vst_component_base.h>
#include <ags/vst3-capi/public.sdk/source/vst/ags_vst_bus.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_icomponent.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ibstream.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct AgsVstComponent Component;

  AgsVstComponent* ags_vst_component_new();

  void ags_vst_component_delete(AgsVstComponent *component);

  void ags_vst_component_set_controller_class(AgsVstComponent *component,
					      AgsVstFUID *cid);

  gint32 ags_vst_component_remove_audio_buses(AgsVstComponent *component); 

  gint32 ags_vst_component_remove_event_buses(AgsVstComponent *component); 

  gint32 ags_vst_component_rename_bus(AgsVstComponent *component,
				      AgsVstMediaType type, AgsVstBusDirection dir, gint32 index,
				      AgsVstString128 new_name);

  gint32 ags_vst_component_get_controller_class_id(AgsVstComponent *component,
						   AgsVstTUID *class_id);

  gint32 ags_vst_component_set_io_mode(AgsVstComponent *component,
				       AgsVstIoMode mode);

  gint32 ags_vst_component_get_bus_count(AgsVstComponent *component,
					 AgsVstMediaType type, AgsVstBusDirection dir);

  gint32 ags_vst_component_get_bus_info(AgsVstComponent *component,
					AgsVstMediaType type, AgsVstBusDirection dir, gint32 index, AgsVstBusInfo *info);

  gint32 ags_vst_component_get_routing_info(AgsVstComponent *component,
					    AgsVstRoutingInfo *in_info, AgsVstRoutingInfo *out_info);

  gint32 ags_vst_component_activate_bus(AgsVstComponent *component,
					AgsVstMediaType type, AgsVstBusDirection dir, gint32 index, gboolean state);

  gint32 ags_vst_component_set_active(AgsVstComponent *component,
				      gboolean state);

  gint32 ags_vst_component_set_state(AgsVstComponent *component,
				     AgsVstIBStream *state);

  gint32 ags_vst_component_get_state(AgsVstComponent *component,
				     AgsVstIBStream *state);

  gint32 ags_vst_component_initialize(AgsVstComponent *component,
				      AgsVstFUnknown *context);

  gint32 ags_vst_component_terminate(AgsVstComponent *component);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_COMPONENT_H__*/
