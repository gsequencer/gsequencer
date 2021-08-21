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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iparameter_changes.h>

#include <pluginterfaces/vst/ivstparameterchanges.h>

extern "C" {

  const AgsVstTUID* ags_vst_iparam_value_queue_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IParamValueQueue::iid));
  }

  AgsVstParamID ags_vst_iparam_value_queue_get_parameter_id(AgsVstIParamValueQueue *iparam_value_queue)
  {
    return(((Steinberg::Vst::IParamValueQueue *) iparam_value_queue)->getParameterId());
  }

  gint32 ags_vst_iparam_value_queue_get_point_count(AgsVstIParamValueQueue *iparam_value_queue)
  {
    return(((Steinberg::Vst::IParamValueQueue *) iparam_value_queue)->getPointCount());
  }

  AgsVstTResult ags_vst_iparam_value_queue_get_point(AgsVstIParamValueQueue *iparam_value_queue,
						     gint32 index,
						     gint32 *sample_offset, AgsVstParamValue *value)
  {
    const Steinberg::int32& tmp_sample_offset_0 = const_cast<Steinberg::int32&>(sample_offset[0]);
    const Steinberg::Vst::ParamValue& tmp_value_0 = const_cast<Steinberg::Vst::ParamValue&>(value[0]);
    
    return(((Steinberg::Vst::IParamValueQueue *) iparam_value_queue)->getPoint(index,
									       const_cast<Steinberg::int32&>(tmp_sample_offset_0), const_cast<Steinberg::Vst::ParamValue&>(tmp_value_0)));
  }

  AgsVstTResult ags_vst_iparam_value_queue_add_point(AgsVstIParamValueQueue *iparam_value_queue,
						     gint32 sample_offset, AgsVstParamValue value,
						     gint32 *index)
  {
    const Steinberg::int32& tmp_index_0 = const_cast<Steinberg::int32&>(index[0]);

    return(((Steinberg::Vst::IParamValueQueue *) iparam_value_queue)->addPoint(sample_offset, (Steinberg::Vst::ParamValue) value,
									       const_cast<Steinberg::int32&>(tmp_index_0)));
  }
						      
   
  const AgsVstTUID* ags_vst_iparameter_changes_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IParameterChanges::iid));
  }

  gint32 ags_vst_iparameter_changes_get_parameter_count(AgsVstIParameterChanges *iparameter_changes)
  {
    return(((Steinberg::Vst::IParameterChanges *) iparameter_changes)->getParameterCount());
  }

  AgsVstIParamValueQueue* ags_vst_iparameter_changes_get_parameter_data(AgsVstIParameterChanges *iparameter_changes,
									gint32 index)
  {
    return((AgsVstIParamValueQueue *) ((Steinberg::Vst::IParameterChanges *) iparameter_changes)->getParameterData(index));
  }
  
  AgsVstIParamValueQueue* ags_vst_iparameter_changes_add_parameter_data(AgsVstIParameterChanges *iparameter_changes,
									AgsVstParamID *param_id,
									gint32 *index)
  {
    const Steinberg::Vst::ParamID& tmp_param_id_0 = const_cast<Steinberg::Vst::ParamID&>(param_id[0]);
    const Steinberg::int32& tmp_index_0 = const_cast<Steinberg::int32&>(index[0]);

    return((AgsVstIParamValueQueue *) ((Steinberg::Vst::IParameterChanges *) iparameter_changes)->addParameterData(const_cast<Steinberg::Vst::ParamID&>(tmp_param_id_0),
														   const_cast<Steinberg::int32&>(tmp_index_0)));
  }

}
