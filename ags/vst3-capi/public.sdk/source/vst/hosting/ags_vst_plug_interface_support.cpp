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

#include <ags/vst3-capi/public.sdk/source/vst/hosting/ags_vst_plug_interface_support.h>

#include <public.sdk/source/vst/hosting/pluginterfacesupport.h>

extern "C" {

  /**
   * Instantiate new Steinberg::Vst::PlugInterfaceSupport the constructor as a C99
   * compatible handle a void pointer.
   * 
   * @return the new instance of plug interface support
   * 
   * @since 5.0.0
   */
  AgsVstPlugInterfaceSupport* ags_vst_plug_interface_support_new()
  {
    return((AgsVstPlugInterfaceSupport *) new Steinberg::Vst::PlugInterfaceSupport());
  }

  /**
   * Add plug interface supported.
   * 
   * @param plug_interface_support the plug interface support
   * @param tuid the plug interface TUID
   * 
   * @since 5.0.0
   */
  void ags_vst_plug_interface_support_add_plug_interface_supported(AgsVstPlugInterfaceSupport *plug_interface_support,
								   AgsVstTUID *tuid)
  {
    ((Steinberg::Vst::PlugInterfaceSupport *) plug_interface_support)->addPlugInterfaceSupported(((Steinberg::TUID *) tuid)[0]);
  }

  /**
   * Remove plug interface supported.
   * 
   * @param plug_interface_support the plug interface support
   * @param tuid the plug interface TUID
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_plug_interface_support_remove_plug_interface_supported(AgsVstPlugInterfaceSupport *plug_interface_support,
									  AgsVstTUID *tuid)
  {
    return(((Steinberg::Vst::PlugInterfaceSupport *) plug_interface_support)->removePlugInterfaceSupported(((Steinberg::TUID *) tuid)[0]));
  }
  
}
