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

#ifndef __AGS_VST_IPARAMETER_FUNCTION_NAME_H__
#define __AGS_VST_IPARAMETER_FUNCTION_NAME_H__

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_types.h>

#ifdef __cplusplus
extern "C" {
#endif
  
  extern const gchar *ags_vst_kcomp_gain_reduction;
  extern const gchar *ags_vst_kcomp_gain_reduction_max;
  extern const gchar *ags_vst_kcomp_gain_reduction_peak_hold;
  extern const gchar *ags_vst_kcomp_reset_gain_reduction_max;

  extern const gchar *ags_vst_klow_latency_mode;

  extern const gchar *ags_vst_kdry_wet_mix;

  extern const gchar *ags_vst_krandomize;

  typedef struct IParameterFunctionName AgsVstIParameterFunctionName;
  
  const AgsVstTUID* ags_vst_iparameter_function_name_get_iid();

  AgsVstTResult ags_vst_iparameter_function_name_get_parameter_id_from_function_name(AgsVstIParameterFunctionName *iparameter_function_name,
										     AgsVstUnitID unit_id, AgsVstFIDString function_name, AgsVstParamID *param_id);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_IPARAMETER_FUNCTION_NAME_H__*/
