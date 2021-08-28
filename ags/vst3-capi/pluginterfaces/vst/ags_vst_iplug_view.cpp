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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iplug_view.h>

#include <pluginterfaces/vst/ivstplugview.h>

extern "C" {
    
  const AgsVstTUID* ags_vst_iparameter_finder_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IParameterFinder::iid.toTUID()));
  }

  AgsVstTResult ags_vst_iparameter_finder_find_parameter(AgsVstIParameterFinder *iparameter_finder,
							 gint32 x_pos, gint32 y_pos,
							 AgsVstParamID *result_tag)
  {
    Steinberg::Vst::ParamID tmp_result_tag_0;
    const Steinberg::Vst::ParamID& tmp_result_tag_1 = const_cast<Steinberg::Vst::ParamID&>(tmp_result_tag_0);
    
    Steinberg::tresult retval;

    retval = ((Steinberg::Vst::IParameterFinder *) iparameter_finder)->findParameter(x_pos, y_pos, const_cast<Steinberg::Vst::ParamID&>(tmp_result_tag_1));

    if(result_tag != NULL){
      result_tag[0] = tmp_result_tag_0;
    }
   
    return(retval);
  }

}
