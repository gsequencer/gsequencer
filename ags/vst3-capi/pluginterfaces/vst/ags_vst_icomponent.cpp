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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_icomponent.h>

#include <pluginterfaces/vst/ivstcomponent.h>

extern "C" {

  const gint32 ags_vst_kdefault_factory_flags = AGS_VST_KUNICODE;

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_icomponent_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IComponent::iid.toTUID()));
  }

  /**
   * Destroy.
   *
   * @param icomponent the icomponent
   *
   * @since 5.0.0
   */
  void ags_vst_icomponent_destroy(AgsVstIComponent *icomponent)
  {
    delete ((Steinberg::Vst::IComponent *) icomponent);
  }

  /**
   * Get controller class identifier.
   *
   * @param icomponent the icomponent
   * @param class_id the class identifier
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_icomponent_get_controller_class_id(AgsVstIComponent *icomponent, AgsVstTUID *class_id)
  {
    return(((Steinberg::Vst::IComponent *) icomponent)->getControllerClassId((char *) class_id));
  }

  /**
   * Set input/output mode.
   *
   * @param icomponent the icomponent
   * @param io_mode the IO mode
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_icomponent_set_io_mode(AgsVstIComponent *icomponent, guint io_mode)
  {
    return(((Steinberg::Vst::IComponent *) icomponent)->setIoMode(io_mode));
  }

  /**
   * Get bus count.
   *
   * @param icomponent the icomponent
   * @param type the bus type
   * @param dir the direction
   * @return the bus count
   *
   * @since 5.0.0
   */
  gint32 ags_vst_icomponent_get_bus_count(AgsVstIComponent *icomponent,
					  guint type, guint dir)
  {
    return(((Steinberg::Vst::IComponent *) icomponent)->getBusCount(type, dir));
  }
  
  /**
   * Get bus info.
   *
   * @param icomponent the icomponent
   * @param type the bus type
   * @param dir the direction
   * @param index the index
   * @param bus the bus info
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_icomponent_get_bus_info(AgsVstIComponent *icomponent,
						guint type, guint dir,
						gint32 index,
						AgsVstBusInfo *bus)
  {
    Steinberg::Vst::BusInfo *tmp_bus_info_0 = (Steinberg::Vst::BusInfo *) bus;
    const Steinberg::Vst::BusInfo& tmp_bus_info_1 = const_cast<Steinberg::Vst::BusInfo&>(tmp_bus_info_0[0]);

    Steinberg::tresult retval;
    
    retval = ((Steinberg::Vst::IComponent *) icomponent)->getBusInfo(type, dir,
								     index,
								     const_cast<Steinberg::Vst::BusInfo&>(tmp_bus_info_1));
    
    return(retval);
  }

  /**
   * Get routing info.
   *
   * @param icomponent the icomponent
   * @param in_info the input info
   * @param out_info the output info
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_icomponent_get_routing_info(AgsVstIComponent *icomponent,
						    AgsVstRoutingInfo *in_info, AgsVstRoutingInfo *out_info)
  {
    Steinberg::Vst::RoutingInfo *tmp_in_info_0 = (Steinberg::Vst::RoutingInfo *) in_info;
    Steinberg::Vst::RoutingInfo *tmp_out_info_0 = (Steinberg::Vst::RoutingInfo *) out_info;
    const Steinberg::Vst::RoutingInfo& tmp_in_info_1 = const_cast<Steinberg::Vst::RoutingInfo&>(tmp_in_info_0[0]);
    const Steinberg::Vst::RoutingInfo& tmp_out_info_1 = const_cast<Steinberg::Vst::RoutingInfo&>(tmp_out_info_0[0]);

    Steinberg::tresult retval;
    
    retval = ((Steinberg::Vst::IComponent *) icomponent)->getRoutingInfo(const_cast<Steinberg::Vst::RoutingInfo&>(tmp_in_info_1), const_cast<Steinberg::Vst::RoutingInfo&>(tmp_out_info_1));

    return(retval);
  }

  /**
   * Activate bus.
   *
   * @param icomponent the icomponent
   * @param type the bus type
   * @param dir the direction
   * @param index the index
   * @param state true if active, otherwise false
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_icomponent_activate_bus(AgsVstIComponent *icomponent,
						guint type, guint dir,
						gint32 index,
						gboolean state)
  {
    return(((Steinberg::Vst::IComponent *) icomponent)->activateBus(type, dir, index, state));
  }

  /**
   * Set active.
   *
   * @param icomponent the icomponent
   * @param state true if active, otherwise false
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_icomponent_set_active(AgsVstIComponent *icomponent,
					      gboolean state)
  {
    return(((Steinberg::Vst::IComponent *) icomponent)->setActive(state));
  }

  /**
   * Set state.
   *
   * @param icomponent the icomponent
   * @param state the stream
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_icomponent_set_state(AgsVstIComponent *icomponent,
					     AgsVstIBStream *state)
  {
    return(((Steinberg::Vst::IComponent *) icomponent)->setState((Steinberg::IBStream *) state));
  }

  /**
   * Get state.
   *
   * @param icomponent the icomponent
   * @param state the stream
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_icomponent_get_state(AgsVstIComponent *icomponent,
					     AgsVstIBStream *state)
  {
    return(((Steinberg::Vst::IComponent *) icomponent)->getState((Steinberg::IBStream *) state));
  }

  
}

