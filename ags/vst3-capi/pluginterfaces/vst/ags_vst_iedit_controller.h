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

#ifndef __AGS_VST_IEDIT_CONTROLLER_H__
#define __AGS_VST_IEDIT_CONTROLLER_H__

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_iplugin_base.h>

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_types.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_icomponent.h>

#include <ags/vst3-capi/pluginterfaces/gui/ags_vst_iplug_view.h>

#ifdef __cplusplus
extern "C" {
#endif
  
  typedef struct ParameterInfo AgsVstParameterInfo;
  typedef struct IComponentHandler AgsVstIComponentHandler;
  typedef struct IComponentHandler2 AgsVstIComponentHandler2;
  typedef struct IComponentHandlerBusActivation AgsVstIComponentHandlerBusActivation;
  typedef struct IProgress AgsVstIProgress;
  typedef struct IEditController AgsVstIEditController;
  typedef struct IEditController2 AgsVstIEditController2;
  typedef struct MyEditorController AgsVstMyEditorController;
  typedef struct IMidiMapping AgsVstIMidiMapping;
  typedef struct IEditControllerHostEditing AgsVstIEditControllerHostEditing;
  typedef guint64 AgsVstID;

  typedef enum
  {
    AsyncStateRestoration = 0,
    UIBackgroundTask
  }AgsVstProgressType;

  typedef enum
  {
    AGS_VST_KNO_FLAGS                 = 0,
    AGS_VST_KCAN_AUTOMATE             = 1,
    AGS_VST_KIS_READ_ONLY             = 1 << 1,
    AGS_VST_KIS_WRAP_AROUND           = 1 << 2,
    AGS_VST_KIS_LIST                  = 1 << 3,
    AGS_VST_KIS_HIDDEN                = 1 << 4,
    AGS_VST_KIS_PROGRAM_CHANGE        = 1 << 15,
    AGS_VST_KIS_BYPASS                = 1 << 16
  }AgsVstParameterFlags;

  extern const AgsVstCString ags_vst_editor;

  typedef enum
  {
    AGS_VST_KRELOAD_COMPONENT                        = 1,
    AGS_VST_KIO_CHANGED                              = 1 << 1,
    AGS_VST_KPARAM_VALUES_CHANGED                    = 1 << 2,
    AGS_VST_KLATENCY_CHANGED                         = 1 << 3,
    AGS_VST_KPARAM_TITLES_CHANGED                    = 1 << 4,
    AGS_VST_KMIDI_CC_ASSIGNMENT_CHANGED              = 1 << 5,
    AGS_VST_KNOTE_EXPRESSION_CHANGED                 = 1 << 6,
    AGS_VST_KIO_TITLES_CHANGED                       = 1 << 7,
    AGS_VST_KPREFETCHABLE_SUPPORT_CHANGED            = 1 << 8,
    AGS_VST_KROUTING_INFO_CHANGED                    = 1 << 9
  }AgsVstRestartFlags;

  typedef enum
  {
    AGS_VST_KCIRCULAR_MODE = 0,
    AGS_VST_KRELATIVE_CIRCULAR_MODE,
    AGS_VST_KLINEAR_MODE
  }AgsVstKnobModes;

  AgsVstParameterInfo* ags_vst_parameter_info_alloc();

  void ags_vst_parameter_info_free(AgsVstParameterInfo *info);

  AgsVstParamID ags_vst_parameter_info_get_param_id(AgsVstParameterInfo *info);
  
  AgsVstString128* ags_vst_parameter_info_get_title(AgsVstParameterInfo *info);
  
  AgsVstString128* ags_vst_parameter_info_get_short_title(AgsVstParameterInfo *info);
  
  AgsVstString128* ags_vst_parameter_info_get_units(AgsVstParameterInfo *info);
  
  gint32 ags_vst_parameter_info_get_step_count(AgsVstParameterInfo *info);

  AgsVstParamValue ags_vst_parameter_info_get_default_normalized_value(AgsVstParameterInfo *info);
  
  AgsVstUnitID ags_vst_parameter_info_get_unit_id(AgsVstParameterInfo *info);
  
  guint ags_vst_parameter_info_get_flags(AgsVstParameterInfo *info);
  
  const AgsVstTUID* ags_vst_icomponent_handler_get_iid();

  AgsVstTResult ags_vst_icomponent_handler_begin_edit(AgsVstIComponentHandler *icomponent_handler,
						      AgsVstParamID id);

  AgsVstTResult ags_vst_icomponent_handler_perform_edit(AgsVstIComponentHandler *icomponent_handler,
							AgsVstParamID id, AgsVstParamValue value_normalized);

  AgsVstTResult ags_vst_icomponent_handler_end_edit(AgsVstIComponentHandler *icomponent_handler,
						    AgsVstParamID id);

  AgsVstTResult ags_vst_icomponent_handler_restart_component(AgsVstIComponentHandler *icomponent_handler,
							     gint32 flags);
  
  const AgsVstTUID* ags_vst_icomponent_handler2_get_iid();

  AgsVstTResult ags_vst_icomponent_handler2_set_dirty(AgsVstIComponentHandler2 *icomponent_handler2,
						      gboolean state);

  AgsVstTResult ags_vst_icomponent_handler2_request_open_editor(AgsVstIComponentHandler2 *icomponent_handler2,
								AgsVstFIDString name);
  
  AgsVstTResult ags_vst_icomponent_handler2_finish_group_edit(AgsVstIComponentHandler2 *icomponent_handler2);
  
  const AgsVstTUID* ags_vst_icomponent_handler_bus_activation_get_iid();

  AgsVstTResult ags_vst_icomponent_handler_bus_activation_request_bus_activation(AgsVstIComponentHandlerBusActivation *icomponent_handler_bus_activation,
										 AgsVstMediaType media_type, AgsVstBusDirection dir,
										 gint32 index,
										 gboolean state);
  
  const AgsVstTUID* ags_vst_iprogress_get_iid();

  AgsVstTResult ags_vst_iprogress_start(AgsVstIProgress *iprogress,
					AgsVstProgressType progress_type,
					ags_vst_tchar *optional_description,
					AgsVstID *out_id);

  AgsVstTResult ags_vst_iprogress_update(AgsVstIProgress *iprogress,
					 AgsVstID id, AgsVstParamValue norm_value);

  AgsVstTResult ags_vst_iprogress_finish(AgsVstIProgress *iprogress,
					 AgsVstID id);
  
  const AgsVstTUID* ags_vst_iedit_controller_get_iid();

  AgsVstTResult ags_vst_iedit_controller_set_component_state(AgsVstIEditController *iedit_controller,
							     AgsVstIBStream *state);
  
  AgsVstTResult ags_vst_iedit_controller_set_state(AgsVstIEditController *iedit_controller,
						   AgsVstIBStream *state);
  
  AgsVstTResult ags_vst_iedit_controller_get_state(AgsVstIEditController *iedit_controller,
						   AgsVstIBStream *state);
  
  gint32 ags_vst_iedit_controller_get_parameter_count(AgsVstIEditController *iedit_controller);

  AgsVstTResult ags_vst_iedit_controller_get_parameter_info(AgsVstIEditController *iedit_controller,
							    gint32 param_index, AgsVstParameterInfo *info);

  AgsVstTResult ags_vst_iedit_controller_get_param_string_by_value(AgsVstIEditController *iedit_controller,
								   AgsVstParamID id, AgsVstParamValue value_normalized,
								   AgsVstString128 *string);

  AgsVstTResult ags_vst_iedit_controller_get_param_value_by_string(AgsVstIEditController *iedit_controller,
								   AgsVstParamID id,
								   AgsVstTChar *string,
								   AgsVstParamValue *value_normalized);
  
  AgsVstParamValue ags_vst_iedit_controller_normalized_param_to_plain(AgsVstIEditController *iedit_controller,
								      AgsVstParamID id,
								      AgsVstParamValue value_normalized);

  AgsVstParamValue ags_vst_iedit_controller_plain_param_to_normalized(AgsVstIEditController *iedit_controller,
								      AgsVstParamID id,
								      AgsVstParamValue plain_value);
  
  AgsVstParamValue ags_vst_iedit_controller_get_param_normalized(AgsVstIEditController *iedit_controller,
								 AgsVstParamID id);
  
  AgsVstParamValue ags_vst_iedit_controller_set_param_normalized(AgsVstIEditController *iedit_controller,
								 AgsVstParamID id,
								 AgsVstParamValue value);

  AgsVstTResult ags_vst_iedit_controller_set_component_handler(AgsVstIEditController *iedit_controller,
							       AgsVstIComponentHandler *handler);

  AgsVstIPlugView* ags_vst_iedit_controller_create_view(AgsVstIEditController *iedit_controller,
							AgsVstFIDString name);
  
  const AgsVstTUID* ags_vst_iedit_controller2_get_iid();

  AgsVstTResult ags_vst_iedit_controller2_set_knob_mode(AgsVstIEditController2 *iedit_controller2,
							guint mode);
  
  AgsVstTResult ags_vst_iedit_controller2_open_help(AgsVstIEditController2 *iedit_controller2,
						    gboolean only_check);
  
  AgsVstTResult ags_vst_iedit_controller2_open_about_box(AgsVstIEditController2 *iedit_controller2,
							 gboolean only_check);

  const AgsVstTUID* ags_vst_imidi_mapping_get_iid();

  AgsVstTResult ags_vst_imidi_mapping_get_midi_controller_assignment(AgsVstIMidiMapping *imidi_mapping,
								     gint32 bus_index, gint16 channel,
								     AgsVstCtrlNumber midi_controller_number, AgsVstParamID *id);

  const AgsVstTUID* ags_vst_iedit_controller_host_editing_get_iid();

  AgsVstTResult ags_vst_iedit_controller_host_editing_begin_edit_from_host(AgsVstIEditControllerHostEditing *iedit_controller_host_editing,
								      AgsVstParamID param_id);
  
  AgsVstTResult ags_vst_iedit_controller_host_editing_end_edit_from_host(AgsVstIEditControllerHostEditing *iedit_controller_host_editing,
									 AgsVstParamID param_id);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_IEDIT_CONTROLLER_H__*/
