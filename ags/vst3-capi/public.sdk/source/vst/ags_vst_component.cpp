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

#include <ags/vst3-capi/public.sdk/source/vst/ags_vst_component.h>

#include <public.sdk/source/vst/vstcomponent.h>

#include <string.h>

extern "C" {
  
  AgsVstComponent* ags_vst_component_new()
  {
    return(new Component());
  }
  
  void ags_vst_component_delete(AgsVstComponent *component)
  {
    delete component;
  }

  void ags_vst_component_set_controller_class(AgsVstComponent *component,
					      AgsVstFUID **cid)
  {
    component->setControllerClass(static_cast<FUID&>(cid[0]));
  }

  gint32 ags_vst_component_remove_audio_buses(AgsVstComponent *component) 
  {
    return(component->removeAudioBuses());
  }

  gint32 ags_vst_component_remove_event_buses(AgsVstComponent *component) 
  {
    return(component->removeEventBuses());
  }

  gint32 ags_vst_component_rename_bus(AgsVstComponent *component,
				      AgsVstMediaType type, AgsVstBusDirection dir, gint32 index,
				      AgsVstString128 new_name)
  {
    return(component->renameBus(static_cast<MediaType>(type), static_cast<BusDirection>(dir), index,
				new_name));
  }

  gint32 ags_vst_component_get_controller_class_id(AgsVstComponent *component,
						   AgsVstTUID class_id)
  {
    return(component->getControllerClassId(static_cast<TUID>(class_id)));
  }

  gint32 ags_vst_component_set_io_mode(AgsVstComponent *component,
				       AgsVstIoMode mode)
  {
    return(component->setIoMode(static_cast<IoMode>(mode)));
  }

  gint32 ags_vst_component_get_bus_count(AgsVstComponent *component,
					 AgsVstMediaType type, AgsVstBusDirection dir)
  {
    return(component->getBusCount(static_cast<MediaType>(type), static_cast<BusDirection>(dir)));
  }

  gint32 ags_vst_component_get_bus_info(AgsVstComponent *component,
					AgsVstMediaType type, AgsVstBusDirection dir, gint32 index, AgsVstBusInfo **info)
  {
    return(component->getBusInfo(static_cast<MediaType>(type), static_cast<BusDirection>(dir), index,
				 static_cast<BusInfo&>(info[0])));
  }

  gint32 ags_vst_component_get_routing_info(AgsVstComponent *component,
					    AgsVstRoutingInfo **in_info, AgsVstRoutingInfo **out_info)
  {
    return(component->getRoutingInfo(static_cast<RoutingInfo&>(in_info[0]), static_cast<RoutingInfo&>(out_info[0])));
  }

  gint32 ags_vst_component_activate_bus(AgsVstComponent *component,
					AgsVstMediaType type, AgsVstBusDirection dir, gint32 index, gboolean state)
  {
    return(component->activateBus(static_cast<MediaType>(type), static_cast<BusDirection>(dir), index,
				  static_cast<TBool>(state)));
  }

  gint32 ags_vst_component_set_active(AgsVstComponent *component,
				      gboolean state)
  {
    return(component->setActive(static_cast<TBool>(state)));
  }

  gint32 ags_vst_component_set_state(AgsVstComponent *component,
				     AgsVstIBStream *state)
  {
    return(component->setState(state));
  }

  gint32 ags_vst_component_get_state(AgsVstComponent *component,
				     AgsVstIBStream *state)

  {
    return(component->getState(state));
  }

  gint32 ags_vst_component_initialize(AgsVstComponent *component,
				      AgsVstFUnknown *context)
  {
    return(component->initialize(context));
  }
  
  gint32 ags_vst_component_terminate(AgsVstComponent *component)
  {
    return(component->terminate());
  }

}
