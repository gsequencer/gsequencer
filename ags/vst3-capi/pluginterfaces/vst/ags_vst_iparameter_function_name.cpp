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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iparameter_function_name.h>

#include <pluginterfaces/vst/ivstparameterfunctionname.h>

extern "C" {
      
  const AgsVstTUID* ags_vst_iparameter_function_name_get_iid()
  {
    extern const Steinberg::TUID IParameterFunctionName__iid;

    return((AgsVstTUID *) (&IParameterFunctionName__iid));
  }
  
  AgsVstTResult ags_vst_iparameter_function_name_get_parameter_id_from_function_name(AgsVstIParameterFunctionName *iparameter_function_name,
										     AgsVstUnitID unit_id, AgsVstFIDString function_name, AgsVstParamID *param_id)
  {
    Steinberg::Vst::ParamID tmp_param_id_0;
    const Steinberg::Vst::ParamID& tmp_param_id_1 = const_cast<Steinberg::Vst::ParamID&>(tmp_param_id_0);

    Steinberg::tresult retval;
    
    retval = ((Steinberg::Vst::IParameterFunctionName *) iparameter_function_name)->getParameterIDFromFunctionName((Steinberg::Vst::UnitID) unit_id, (Steinberg::FIDString) function_name, const_cast<Steinberg::Vst::ParamID&>(tmp_param_id_1));

    if(param_id != NULL){
      param_id[0] = tmp_param_id_0;
    }

    return(retval);
  }
  
}