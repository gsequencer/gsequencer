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

#ifndef __AGS_VST_PARAMETER_CHANGES_H__
#define __AGS_VST_PARAMETER_CHANGES_H__

#include <glib.h>

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_ievents.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct ParameterValueQueue AgsVstParameterValueQueue;
  typedef struct ParameterChanges AgsVstParameterChanges;

  AgsVstParameterValueQueue* ags_vst_parameter_value_queue_new(AgsVstParamID param_id);

  void ags_vst_parameter_value_queue_add_point(AgsVstParameterValueQueue *parameter_value_queue,
					       gint32 sample_offset, AgsVstParamValue value,
					       gint32 *index);

  void ags_vst_parameter_value_queue_clear(AgsVstParameterValueQueue *parameter_value_queue);
    
  AgsVstParameterChanges* ags_vst_parameter_changes_new();

  void ags_vst_parameter_changes_clear_queue(AgsVstParameterChanges *parameter_changes);

  void ags_vst_parameter_changes_set_max_parameters(AgsVstParameterChanges *parameter_changes,
						    gint32 max_parameters);
  
  gint32 ags_vst_parameter_changes_get_parameter_count(AgsVstParameterChanges *parameter_changes);

  AgsVstParameterValueQueue* ags_vst_parameter_changes_get_parameter_data(AgsVstParameterChanges *parameter_changes,
									  gint32 index);

  AgsVstParameterValueQueue* ags_vst_parameter_changes_add_parameter_data(AgsVstParameterChanges *parameter_changes,
									  AgsVstParamID *param_id, gint32 *index);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_PARAMETER_CHANGES_H__*/
