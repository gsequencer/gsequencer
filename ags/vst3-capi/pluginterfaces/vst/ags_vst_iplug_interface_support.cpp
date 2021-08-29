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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iplug_interface_support.h>

#include <pluginterfaces/vst/ivstpluginterfacesupport.h>

extern "C" {

  const AgsVstTUID* ags_vst_iplug_interface_support_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IPlugInterfaceSupport::iid.toTUID()));
  }

  AgsVstTResult ags_vst_iplug_interface_support_is_plug_interface_supported(AgsVstIPlugInterfaceSupport *iplug_interface_support,
									    const AgsVstTUID *iid)
  {
    Steinberg::TUID *tmp_iid = (Steinberg::TUID *) iid;
    
    return(((Steinberg::Vst::IPlugInterfaceSupport *) iplug_interface_support)->isPlugInterfaceSupported(tmp_iid[0]));
  }

}
