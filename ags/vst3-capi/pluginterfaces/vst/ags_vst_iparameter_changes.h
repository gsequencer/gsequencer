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

#ifndef __AGS_VST_IINTER_APP_AUDIO_H__
#define __AGS_VST_IINTER_APP_AUDIO_H__

#include <glib.h>

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.h>

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_types.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct IParamValueQueue AgsVstIParamValueQueue;
  typedef struct IParameterChanges AgsVstIParameterChanges;

  const AgsVstTUID* ags_vst_iparam_value_queue_get_iid();

  AgsVstParamID ags_vst_iparam_value_queue_get_parameter_id(AgsVstIParamValueQueue *iparam_value_queue);

  gint32 ags_vst_iparam_value_queue_get_point_count(AgsVstIParamValueQueue *iparam_value_queue);

  AgsVstTResult ags_vst_iparam_value_queue_get_point(AgsVstIParamValueQueue *iparam_value_queue,
						     gint32 index,
						     gint32 *sample_offset, AgsVstParamValue *value);

  AgsVstTResult ags_vst_iparam_value_queue_add_point(AgsVstIParamValueQueue *iparam_value_queue,
						     gint32 sample_offset, AgsVstParamValue value,
						     gint32 *index);
						      
   
  const AgsVstTUID* ags_vst_iparameter_changes_get_iid();

  gint32 ags_vst_iparameter_changes_get_parameter_count(AgsVstIParameterChanges *iparameter_changes);

  AgsVstIParamValueQueue* ags_vst_iparameter_changes_get_parameter_data(AgsVstIParameterChanges *iparameter_changes,
									gint32 index);
  
  AgsVstIParamValueQueue* ags_vst_iparameter_changes_add_parameter_data(AgsVstIParameterChanges *iparameter_changes,
									AgsVstParamID *param_id,
									gint32 *index);

#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_IINTER_APP_AUDIO_H__*/
