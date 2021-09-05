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

#include <ags/vst3-capi/public.sdk/source/vst/hosting/ags_vst_parameter_changes.h>

#include <public.sdk/source/vst/hosting/parameterchanges.h>

extern "C" {

  AgsVstParameterValueQueue* ags_vst_parameter_value_queue_new(AgsVstParamID param_id)
  {
    Steinberg::Vst::ParameterValueQueue *parameter_value_queue = new Steinberg::Vst::ParameterValueQueue((Steinberg::Vst::ParamID) param_id);
    
    return((AgsVstParameterValueQueue *) parameter_value_queue);
  }
  
  gint32 ags_vst_parameter_value_queue_get_point_count(AgsVstParameterValueQueue *parameter_value_queue)
  {
    return(((Steinberg::Vst::ParameterValueQueue *) parameter_value_queue)->getPointCount());
  }
  
  AgsVstTResult ags_vst_parameter_value_queue_get_point(AgsVstParameterValueQueue *parameter_value_queue,
							gint32 index,
							gint32 *sample_offset, AgsVstParamValue *value)
  {
    return(((Steinberg::Vst::ParameterValueQueue *) parameter_value_queue)->getPoint(index,
									   const_cast<Steinberg::int32&>(sample_offset[0]), const_cast<Steinberg::Vst::ParamValue&>(value[0])));
  }
  
  AgsVstTResult ags_vst_parameter_value_queue_add_point(AgsVstParameterValueQueue *parameter_value_queue,
							gint32 sample_offset, AgsVstParamValue value,
							gint32 *index)
  {
    return(((Steinberg::Vst::ParameterValueQueue *) parameter_value_queue)->addPoint(sample_offset, (Steinberg::Vst::ParamValue) value,
										     const_cast<Steinberg::int32&>(index[0])));
  }

  void ags_vst_parameter_value_queue_set_param_id(AgsVstParameterValueQueue *parameter_value_queue,
						  AgsVstParamID param_id)
  {
    ((Steinberg::Vst::ParameterValueQueue *) parameter_value_queue)->setParamID((Steinberg::Vst::ParamID) param_id);
  }  

  void ags_vst_parameter_value_queue_clear(AgsVstParameterValueQueue *parameter_value_queue)
  {
    ((Steinberg::Vst::ParameterValueQueue *) parameter_value_queue)->clear();
  }
    
  AgsVstParameterChanges* ags_vst_parameter_changes_new()
  {
    Steinberg::Vst::ParameterChanges *parameter_changes = new Steinberg::Vst::ParameterChanges();
    
    return((AgsVstParameterChanges *) parameter_changes);
  }

  void ags_vst_parameter_changes_clear_queue(AgsVstParameterChanges *parameter_changes)
  {
    ((Steinberg::Vst::ParameterChanges *) parameter_changes)->clearQueue();
  }

  void ags_vst_parameter_changes_set_max_parameters(AgsVstParameterChanges *parameter_changes,
						    gint32 max_parameters)
  {
    ((Steinberg::Vst::ParameterChanges *) parameter_changes)->setMaxParameters(max_parameters);
  }
  
  gint32 ags_vst_parameter_changes_get_parameter_count(AgsVstParameterChanges *parameter_changes)
  {
    return(((Steinberg::Vst::ParameterChanges *) parameter_changes)->getParameterCount());
  }

  AgsVstParameterValueQueue* ags_vst_parameter_changes_get_parameter_data(AgsVstParameterChanges *parameter_changes,
									  gint32 index)
  {
    return((AgsVstParameterValueQueue *) (((Steinberg::Vst::ParameterChanges *) parameter_changes)->getParameterData(index)));
  }

  AgsVstParameterValueQueue* ags_vst_parameter_changes_add_parameter_data(AgsVstParameterChanges *parameter_changes,
									  AgsVstParamID *param_id, gint32 *index)
  {
    return(((AgsVstParameterValueQueue *) ((Steinberg::Vst::ParameterChanges *) parameter_changes)->addParameterData(const_cast<Steinberg::Vst::ParamID&>(param_id[0]), const_cast<Steinberg::int32&>(index[0]))));
  }
}
