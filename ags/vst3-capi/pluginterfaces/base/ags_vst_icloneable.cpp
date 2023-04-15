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

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_icloneable.h>

#include <pluginterfaces/base/icloneable.h>

extern "C" {

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstTUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID*
  ags_vst_icloneable_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::ICloneable::iid.toTUID()));
  }

  /**
   * Clone.
   * 
   * @param cloneable the cloneable
   * @return Steinberg::FUnknown as AgsVstFUnknown
   *
   * @since 5.0.0
   */
  AgsVstFUnknown* ags_vst_icloneable_clone(AgsVstICloneable *cloneable)
  {
    return((AgsVstFUnknown *) ((Steinberg::ICloneable *) cloneable)->clone());
  }
  
}
