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

#include <ags/vst3-capi/public.sdk/source/vst/ags_vst_component.h>

#include <public.sdk/source/vst/vstcomponent.h>

#include <string.h>

extern "C" {
  
  AgsVstComponent* ags_vst_component_new()
  {
    return((AgsVstComponent *) new Steinberg::Vst::Component());
  }
  
  void ags_vst_component_delete(AgsVstComponent *component)
  {
    delete component;
  }

  void ags_vst_component_set_controller_class(AgsVstComponent *component,
					      AgsVstFUID *cid)
  {
    Steinberg::FUID *tmp_cid_0 = static_cast<Steinberg::FUID*>((void *) cid);
    const Steinberg::FUID& tmp_cid_1 = const_cast<Steinberg::FUID&>(tmp_cid_0[0]);
    
    static_cast<Steinberg::Vst::Component*>((void*) component)->setControllerClass(tmp_cid_1);
  }

  gint32 ags_vst_component_remove_audio_buses(AgsVstComponent *component) 
  {
    return(static_cast<Steinberg::Vst::Component*>((void*) component)->removeAudioBusses());
  }

  gint32 ags_vst_component_remove_event_buses(AgsVstComponent *component) 
  {
    return(static_cast<Steinberg::Vst::Component*>((void*) component)->removeEventBusses());
  }

  gint32 ags_vst_component_rename_bus(AgsVstComponent *component,
				      AgsVstMediaType type, AgsVstBusDirection dir, gint32 index,
				      AgsVstString128 new_name)
  {
    return(static_cast<Steinberg::Vst::Component*>((void*) component)->renameBus(static_cast<Steinberg::Vst::MediaType>(type), static_cast<Steinberg::Vst::BusDirection>(dir), index,
										 static_cast<Steinberg::Vst::TChar*>((void *) new_name)));
  }

  gint32 ags_vst_component_get_controller_class_id(AgsVstComponent *component,
						   AgsVstTUID *class_id)
  {
    Steinberg::TUID tmp_class_id;

    gint32 retval;
    
    retval = static_cast<Steinberg::Vst::Component*>((void*) component)->getControllerClassId(tmp_class_id);

    memcpy(class_id[0], tmp_class_id, 16 * sizeof(char));
    
    return(retval);
  }

  gint32 ags_vst_component_set_io_mode(AgsVstComponent *component,
				       AgsVstIoMode mode)
  {
    return(static_cast<Steinberg::Vst::Component*>((void*) component)->setIoMode(static_cast<Steinberg::Vst::IoMode>(mode)));
  }

  gint32 ags_vst_component_get_bus_count(AgsVstComponent *component,
					 AgsVstMediaType type, AgsVstBusDirection dir)
  {
    return(static_cast<Steinberg::Vst::Component*>((void*) component)->getBusCount(static_cast<Steinberg::Vst::MediaType>(type), static_cast<Steinberg::Vst::BusDirection>(dir)));
  }

  gint32 ags_vst_component_get_bus_info(AgsVstComponent *component,
					AgsVstMediaType type, AgsVstBusDirection dir, gint32 index, AgsVstBusInfo *info)
  {
    Steinberg::Vst::BusInfo *tmp_info_0 = static_cast<Steinberg::Vst::BusInfo*>((void *) info);
    const Steinberg::Vst::BusInfo& tmp_info_1 = const_cast<Steinberg::Vst::BusInfo&>(tmp_info_0[0]);
    
    return(static_cast<Steinberg::Vst::Component*>((void*) component)->getBusInfo(static_cast<Steinberg::Vst::MediaType>(type), static_cast<Steinberg::Vst::BusDirection>(dir), index,
										  const_cast<Steinberg::Vst::BusInfo&>(tmp_info_1)));
  }

  gint32 ags_vst_component_get_routing_info(AgsVstComponent *component,
					    AgsVstRoutingInfo *in_info, AgsVstRoutingInfo *out_info)
  {
    Steinberg::Vst::RoutingInfo *tmp_in_info_0 = static_cast<Steinberg::Vst::RoutingInfo*>((void *) in_info);
    const Steinberg::Vst::RoutingInfo& tmp_in_info_1 = const_cast<Steinberg::Vst::RoutingInfo&>(tmp_in_info_0[0]);
    
    Steinberg::Vst::RoutingInfo *tmp_out_info_0 = static_cast<Steinberg::Vst::RoutingInfo*>((void *) out_info);
    const Steinberg::Vst::RoutingInfo& tmp_out_info_1 = const_cast<Steinberg::Vst::RoutingInfo&>(tmp_out_info_0[0]);
    
    return(static_cast<Steinberg::Vst::Component*>((void*) component)->getRoutingInfo(const_cast<Steinberg::Vst::RoutingInfo&>(tmp_in_info_1), const_cast<Steinberg::Vst::RoutingInfo&>(tmp_out_info_1)));
  }

  gint32 ags_vst_component_activate_bus(AgsVstComponent *component,
					AgsVstMediaType type, AgsVstBusDirection dir, gint32 index, gboolean state)
  {
    return(static_cast<Steinberg::Vst::Component*>((void*) component)->activateBus(static_cast<Steinberg::Vst::MediaType>(type), static_cast<Steinberg::Vst::BusDirection>(dir), index,
										   static_cast<Steinberg::TBool>(state)));
  }

  gint32 ags_vst_component_set_active(AgsVstComponent *component,
				      gboolean state)
  {
    return(static_cast<Steinberg::Vst::Component*>((void*) component)->setActive(static_cast<Steinberg::TBool>(state)));
  }

  gint32 ags_vst_component_set_state(AgsVstComponent *component,
				     AgsVstIBStream *state)
  {
    return(static_cast<Steinberg::Vst::Component*>((void*) component)->setState(static_cast<Steinberg::IBStream*>((void *) state)));
  }

  gint32 ags_vst_component_get_state(AgsVstComponent *component,
				     AgsVstIBStream *state)

  {
    return(static_cast<Steinberg::Vst::Component*>((void*) component)->getState(static_cast<Steinberg::IBStream*>((void *) state)));
  }

  gint32 ags_vst_component_initialize(AgsVstComponent *component,
				      AgsVstFUnknown *context)
  {
    return(static_cast<Steinberg::Vst::Component*>((void*) component)->initialize(static_cast<Steinberg::FUnknown*>((void *) context)));
  }
  
  gint32 ags_vst_component_terminate(AgsVstComponent *component)
  {
    return(static_cast<Steinberg::Vst::Component*>((void*) component)->terminate());
  }

}
