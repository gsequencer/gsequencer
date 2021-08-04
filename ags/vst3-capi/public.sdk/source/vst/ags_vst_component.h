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

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct AgsVstComponent Component;

  /**
   * ags_vst_component_new:
   * 
   * Wrapper of Component constructor.
   * 
   * Returns: the #AgsVstComponent-struct pointer
   * 
   * Since: 2.2.0
   */
  AgsVstComponent* ags_vst_component_new();

  /**
   * ags_vst_component_delete:
   * @component: the #AgsVstComponent-struct pointer
   * 
   * Delete @component.
   * 
   * Since: 2.2.0
   */
  void ags_vst_component_delete(AgsVstComponent *component);

  /**
   * ags_vst_component_set_controller_class:
   * @component: the #AgsVstComponent-struct pointer
   * @cid: the #AgsVstFUID-struct pointer
   * 
   * Set @cid as controller class of @component.
   * 
   * Since: 2.2.0
   */
  void ags_vst_component_set_controller_class(AgsVstComponent *component,
					      AgsVstFUID **cid);

  /**
   * ags_vst_component_remove_audio_buses:
   * @component: the #AgsVstComponent-struct pointer
   * 
   * Remove audio buses from @component.
   * 
   * Returns: the result code
   * 
   * Since: 2.2.0
   */
  gint32 ags_vst_component_remove_audio_buses(AgsVstComponent *component); 

  /**
   * ags_vst_component_remove_event_buses:
   * @component: the #AgsVstComponent-struct pointer
   * 
   * Remove event buses from @component.
   * 
   * Returns: the result code
   * 
   * Since: 2.2.0
   */
  gint32 ags_vst_component_remove_event_buses(AgsVstComponent *component); 

  /**
   * ags_vst_component_rename_bus:
   * @component: the #AgsVstComponent-struct pointer
   * @type: the #AgsVstMediaTypes-enum
   * @dir: the #AgsVstBusDirections-enum
   * @index: the index as integer
   * @new_name: the new name as string
   * 
   * Rename bus specified by @type, @dir and @index to @new_name.
   * 
   * Returns: the result code
   * 
   * Since: 2.2.0
   */
  gint32 ags_vst_component_rename_bus(AgsVstComponent *component,
				      AgsVstMediaType type, AgsVstBusDirection dir, gint32 index,
				      AgsVstString128 new_name);

  /**
   * ags_vst_component_get_controller_class_id:
   * @component: the #AgsVstComponent-struct pointer
   * @class_id: the AgsVstTUID
   * 
   * Get controller class id.
   * 
   * Returns: the result code
   * 
   * Since: 2.2.0
   */
  gint32 ags_vst_component_get_controller_class_id(AgsVstComponent *component,
						   AgsVstTUID class_id);

  /**
   * ags_vst_component_set_io_mode:
   * @component: the #AgsVstComponent-struct pointer
   * @mode: the AgsVstIoMode
   * 
   * Set IO @mode of @component.
   * 
   * Returns: the result code
   * 
   * Since: 2.2.0
   */
  gint32 ags_vst_component_set_io_mode(AgsVstComponent *component,
				       AgsVstIoMode mode);

  /**
   * ags_vst_component_get_bus_count:
   * @component: the #AgsVstComponent-struct pointer
   * @type: the #AgsVstMediaTypes-enum
   * @dir: the #AgsVstBusDirections-enum
   * 
   * Get bus count of @component specified by @type and @dir.
   * 
   * Returns: the bus count
   * 
   * Since: 2.2.0
   */
  gint32 ags_vst_component_get_bus_count(AgsVstComponent *component,
					 AgsVstMediaType type, AgsVstBusDirection dir);

  /**
   * ags_vst_component_get_bus_info:
   * @component: the #AgsVstComponent-struct pointer
   * @type: the #AgsVstMediaTypes-enum
   * @dir: the #AgsVstBusDirections-enum
   * @index: the index
   * @info: the #AgsVstBusInfo-struct pointer to a pointer
   * 
   * Get bus info of @component specified by @type, @dir and @index.
   * 
   * Returns: the result code
   * 
   * Since: 2.2.0
   */
  gint32 ags_vst_component_get_bus_info(AgsVstComponent *component,
					AgsVstMediaType type, AgsVstBusDirection dir, gint32 index, AgsVstBusInfo **info);

  /**
   * ags_vst_component_get_routing_info:
   * @component: the #AgsVstComponent-struct pointer
   * @in_info: the #AgsVstRoutingInfo-struct pointer to a pointer
   * @out_info: the #AgsVstRoutingInfo-struct pointer to a pointer 
   * 
   * Get routing info of @component.
   * 
   * Returns: the result code
   * 
   * Since: 2.2.0
   */
  gint32 ags_vst_component_get_routing_info(AgsVstComponent *component,
					    AgsVstRoutingInfo **in_info, AgsVstRoutingInfo **out_info);

  /**
   * ags_vst_component_activate_bus:
   * @component: the #AgsVstComponent-struct pointer
   * @type: the #AgsVstMediaTypes-enum 
   * @dir: the #AgsVstBusDirections-enum
   * @index: the index
   * @state: %TRUE if state active, else %FALSE
   * 
   * Activate @component's bus specified by @type, @dir and @index.
   * 
   * Returns: the result code
   * 
   * Since: 2.2.0
   */
  gint32 ags_vst_component_activate_bus(AgsVstComponent *component,
					AgsVstMediaType type, AgsVstBusDirection dir, gint32 index, gboolean state);

  /**
   * ags_vst_component_set_active:
   * @component: the #AgsVstComponent-struct pointer
   * @state: %TRUE if active, else %FALSe
   * 
   * Set @component active.
   * 
   * Returns: the result code
   * 
   * Since: 2.2.0
   */
  gint32 ags_vst_component_set_active(AgsVstComponent *component,
				      gboolean state);

  /**
   * ags_vst_component_set_state:
   * @component: the #AgsVstComponent-struct pointer
   * @state: the #AgsVstIBStream-struct pointer
   * 
   * Set state of @component.
   * 
   * Returns: the result code
   * 
   * Since: 2.2.0
   */
  gint32 ags_vst_component_set_state(AgsVstComponent *component,
				     AgsVstIBStream *state);

  /**
   * ags_vst_component_get_state:
   * @component: the #AgsVstComponent-struct pointer
   * @state: the #AgsVstIBStream-struct pointer
   * 
   * Get state of @component.
   * 
   * Returns: the result code
   * 
   * Since: 2.2.0
   */
  gint32 ags_vst_component_get_state(AgsVstComponent *component,
				     AgsVstIBStream *state);

  /**
   * ags_vst_component_initialize:
   * @component: the #AgsVstComponent-struct pointer
   * @context: the #AgsVstFUnknown-struct pointer
   * 
   * Initialize @component with @context.
   * 
   * Returns: the result code
   * 
   * Since: 2.2.0
   */
  gint32 ags_vst_component_initialize(AgsVstComponent *component,
				      AgsVstFUnknown *context);

  /**
   * ags_vst_component_terminate:
   * @component: the #AgsVstComponent-struct pointer
   * 
   * Terminate @component.
   * 
   * Returns: the result code
   * 
   * Since: 2.2.0
   */
  gint32 ags_vst_component_terminate(AgsVstComponent *component);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_COMPONENT_H__*/
