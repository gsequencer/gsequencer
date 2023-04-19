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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iprefetchable_support.h>

#include <pluginterfaces/vst/ivstprefetchablesupport.h>

extern "C" {

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_iprefetchable_support_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IPrefetchableSupport::iid.toTUID()));
  }

  /**
   * Get prefetchable support.
   *
   * @param iprefetchable the prefetchable
   * @param out the output prefetchable
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iprefetchable_support_get_prefetchable_support(AgsVstIPrefetchableSupport *iprefetchable,
								       AgsVstPrefetchableSupport *out)
  {
    return(((Steinberg::Vst::IPrefetchableSupport *) iprefetchable)->getPrefetchableSupport(const_cast<Steinberg::Vst::PrefetchableSupport&>(((Steinberg::Vst::PrefetchableSupport *) out)[0])));
  }

}
