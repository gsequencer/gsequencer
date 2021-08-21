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

  const gchar *ags_vst_kcomp_gain_reduction = "Comp:GainReduction";
  const gchar *ags_vst_kcomp_gain_reduction_max = "Comp:GainReductionMax";
  const gchar *ags_vst_kcomp_gain_reduction_peak_hold = "Comp:GainReductionPeakHold";
  const gchar *ags_vst_kcomp_reset_gain_reduction_max = "Comp:ResetGainReductionMax";

  const gchar *ags_vst_klow_latency_mode = "LowLatencyMode";

  const gchar *ags_vst_kdry_wet_mix = "DryWetMix";

  const gchar *ags_vst_krandomize = "Randomize";
      
  const AgsVstTUID* ags_vst_iparameter_function_name_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IParameterFunctionName::iid));
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
