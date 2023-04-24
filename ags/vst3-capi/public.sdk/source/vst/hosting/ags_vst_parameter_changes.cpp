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

#include <ags/vst3-capi/public.sdk/source/vst/hosting/ags_vst_parameter_changes.h>

#include <public.sdk/source/vst/hosting/parameterchanges.h>

extern "C" {

  /**
   * Instantiate new Steinberg::Vst::ParameterValueQueue the constructor as a C99
   * compatible handle a void pointer.
   * 
   * @param param_id the parameter identifier
   * @return the new instance of parameter value queue
   * 
   * @since 5.0.0
   */
  AgsVstParameterValueQueue* ags_vst_parameter_value_queue_new(AgsVstParamID param_id)
  {
    Steinberg::Vst::ParameterValueQueue *parameter_value_queue = new Steinberg::Vst::ParameterValueQueue((Steinberg::Vst::ParamID) param_id);
    
    return((AgsVstParameterValueQueue *) parameter_value_queue);
  }

  /**
   * Get point count.
   *
   * @param parameter_value_queue the parameter value queue
   * @return the point count
   * 
   * @since 5.0.0
   */
  gint32 ags_vst_parameter_value_queue_get_point_count(AgsVstParameterValueQueue *parameter_value_queue)
  {
    return(((Steinberg::Vst::ParameterValueQueue *) parameter_value_queue)->getPointCount());
  }
  
  /**
   * Get point.
   *
   * @param parameter_value_queue the parameter value queue
   * @param index the index
   * @param sample_offset the sample offset
   * @param value the value
   * @return the result code
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_parameter_value_queue_get_point(AgsVstParameterValueQueue *parameter_value_queue,
							gint32 index,
							gint32 *sample_offset, AgsVstParamValue *value)
  {
    return(((Steinberg::Vst::ParameterValueQueue *) parameter_value_queue)->getPoint(index,
									   const_cast<Steinberg::int32&>(sample_offset[0]), const_cast<Steinberg::Vst::ParamValue&>(value[0])));
  }
  
  /**
   * Add point.
   *
   * @param parameter_value_queue the parameter value queue
   * @param index the index
   * @param sample_offset the sample offset
   * @param value the value
   * @param index the index
   * @return the result code
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_parameter_value_queue_add_point(AgsVstParameterValueQueue *parameter_value_queue,
							gint32 sample_offset, AgsVstParamValue value,
							gint32 *index)
  {
    return(((Steinberg::Vst::ParameterValueQueue *) parameter_value_queue)->addPoint(sample_offset, (Steinberg::Vst::ParamValue) value,
										     const_cast<Steinberg::int32&>(index[0])));
  }

  /**
   * Set parameter identifier.
   *
   * @param parameter_value_queue the parameter value queue
   * @param param_id the parameter identifier
   * 
   * @since 5.0.0
   */
  void ags_vst_parameter_value_queue_set_param_id(AgsVstParameterValueQueue *parameter_value_queue,
						  AgsVstParamID param_id)
  {
    ((Steinberg::Vst::ParameterValueQueue *) parameter_value_queue)->setParamID((Steinberg::Vst::ParamID) param_id);
  }  

  /**
   * Clear.
   *
   * @param parameter_value_queue the parameter value queue
   * 
   * @since 5.0.0
   */
  void ags_vst_parameter_value_queue_clear(AgsVstParameterValueQueue *parameter_value_queue)
  {
    ((Steinberg::Vst::ParameterValueQueue *) parameter_value_queue)->clear();
  }
    
  /**
   * Instantiate new Steinberg::Vst::ParameterChanges the constructor as a C99
   * compatible handle a void pointer.
   * 
   * @return the new instance of parameter changes
   * 
   * @since 5.0.0
   */
  AgsVstParameterChanges* ags_vst_parameter_changes_new()
  {
    Steinberg::Vst::ParameterChanges *parameter_changes = new Steinberg::Vst::ParameterChanges();
    
    return((AgsVstParameterChanges *) parameter_changes);
  }

  /**
   * Clear queue.
   *
   * @param parameter_changes the parameter changes
   * 
   * @since 5.0.0
   */
  void ags_vst_parameter_changes_clear_queue(AgsVstParameterChanges *parameter_changes)
  {
    ((Steinberg::Vst::ParameterChanges *) parameter_changes)->clearQueue();
  }

  /**
   * Set max parameters.
   *
   * @param parameter_changes the parameter changes
   * @param max_parameters the maximum parameters
   * 
   * @since 5.0.0
   */
  void ags_vst_parameter_changes_set_max_parameters(AgsVstParameterChanges *parameter_changes,
						    gint32 max_parameters)
  {
    ((Steinberg::Vst::ParameterChanges *) parameter_changes)->setMaxParameters(max_parameters);
  }
  
  /**
   * Get parameter count.
   *
   * @param parameter_changes the parameter changes
   * @return the parameter count
   * 
   * @since 5.0.0
   */
  gint32 ags_vst_parameter_changes_get_parameter_count(AgsVstParameterChanges *parameter_changes)
  {
    return(((Steinberg::Vst::ParameterChanges *) parameter_changes)->getParameterCount());
  }

  /**
   * Get parameter count.
   *
   * @param parameter_changes the parameter changes
   * @param index the index
   * @return the parameter value queue
   * 
   * @since 5.0.0
   */
  AgsVstParameterValueQueue* ags_vst_parameter_changes_get_parameter_data(AgsVstParameterChanges *parameter_changes,
									  gint32 index)
  {
    return((AgsVstParameterValueQueue *) (((Steinberg::Vst::ParameterChanges *) parameter_changes)->getParameterData(index)));
  }

  /**
   * Get parameter count.
   *
   * @param parameter_changes the parameter changes
   * @param param_id the param identifier
   * @param index the index
   * @return the parameter value queue
   * 
   * @since 5.0.0
   */
  AgsVstParameterValueQueue* ags_vst_parameter_changes_add_parameter_data(AgsVstParameterChanges *parameter_changes,
									  AgsVstParamID *param_id, gint32 *index)
  {
    return(((AgsVstParameterValueQueue *) ((Steinberg::Vst::ParameterChanges *) parameter_changes)->addParameterData(const_cast<Steinberg::Vst::ParamID&>(param_id[0]), const_cast<Steinberg::int32&>(index[0]))));
  }
}
