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

#include <ags/vst3-capi/pluginterfaces/gui/ags_vst_iplug_view_content_scale_support.h>

#include <pluginterfaces/gui/iplugviewcontentscalesupport.h>

extern "C" {

  /**
   * Get IID.
   * 
   * @return the Steinberg::TUID as AgsVstTUID
   *
   * @since 5.0.0
   */
  AgsVstTUID*
  ags_vst_iplug_view_content_scale_support_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::IPlugViewContentScaleSupport::iid.toTUID()));
  }

  /**
   * Set content scale factor.
   *
   * @param iplug_view_content_scale_support the iplug-view content scale support
   * @param factor the factor
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iplug_view_content_scale_support_set_content_scale_factor(AgsVstIPlugViewContentScaleSupport *iplug_view_content_scale_support,
										  AgsVstScaleFactor factor)
  {
    return(((Steinberg::IPlugViewContentScaleSupport *) iplug_view_content_scale_support)->setContentScaleFactor((gfloat) factor));
  }

}
