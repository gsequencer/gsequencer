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

#ifndef __AGS_VST_AUDIO_EFFECT_H__
#define __AGS_VST_AUDIO_EFFECT_H__

#include <glib.h>

#include <ags/vst3-capi/public.sdk/source/vst/ags_vst_component_base.h>
#include <ags/vst3-capi/public.sdk/source/vst/ags_vst_bus.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_icomponent.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct AgsVstComponent Component;
  
  AgsVstComponent* ags_vst_component_new();
  void ags_vst_component_delete(AgsVstComponent *component);

  void ags_vst_component_set_controller_class(AgsVstComponent *component,
					      FUID **cid);
  
  tresult ags_vst_component_remove_audio_buses(AgsVstComponent *component); 

  tresult ags_vst_component_remove_event_buses(AgsVstComponent *component); 

  tresult ags_vst_component_rename_bus(AgsVstComponent *component,
				       guint type, guint dir, gint32 index,
				       gchar *new_name);

  tresult PLUGIN_API ags_vst_component_get_controller_class_id(AgsVstComponent *component,
							       void *class_id);
  tresult PLUGIN_API ags_vst_component_set_io_mode(AgsVstComponent *component,
						   guint mode);
  gint32 PLUGIN_API ags_vst_component_get_bus_count(AgsVstComponent *component,
						    guint type, guint dir);
  tresult PLUGIN_API ags_vst_component_get_bus_info(AgsVstComponent *component,
						    guint type, guint dir, gint32 index, AgsVstBusInfo **info);
  tresult PLUGIN_API ags_vst_component_get_routing_info(AgsVstComponent *component,
							AgsVstRoutingInfo **in_info, AgsVstRoutingInfo **out_info);
  tresult PLUGIN_API ags_vst_component_activate_bus(AgsVstComponent *component,
						    guint type, guint dir, gint32 index, gboolean state);
  tresult PLUGIN_API ags_vst_component_set_active(AgsVstComponent *component,
						  gboolean state);
  tresult PLUGIN_API ags_vst_component_set_state(AgsVstComponent *component,
						 AgsVstIBStream *state);
  tresult PLUGIN_API ags_vst_component_get_state(AgsVstComponent *component,
						 AgsVstIBStream *state);

  tresult PLUGIN_API ags_vst_component_initialize(AgsVstComponent *component,
						  AgsVstFUnknown *context);
  tresult PLUGIN_API ags_vst_component_terminate(AgsVstComponent *component);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_AUDIO_EFFECT_H__*/
