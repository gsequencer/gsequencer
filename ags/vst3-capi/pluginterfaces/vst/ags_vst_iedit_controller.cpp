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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iedit_controller.h>

#include <pluginterfaces/vst/ivsteditcontroller.h>

extern "C" {
  
  const AgsVstTUID* ags_vst_icomponent_handler_get_iid()
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_icomponent_handler_begin_edit(AgsVstIComponentHandler *icomponent_handler,
						      AgsVstParamID id)
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_icomponent_handler_perform_edit(AgsVstIComponentHandler *icomponent_handler,
							AgsVstParamID id, AgsVstParamValue value_normalized)
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_icomponent_handler_end_edit(AgsVstIComponentHandler *icomponent_handler,
						    AgsVstParamID id)
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_icomponent_handler_restart_component(AgsVstIComponentHandler *icomponent_handler,
							     gint32 flags)
  {
    //TODO:JK: implement me
  }
  
  const AgsVstTUID* ags_vst_icomponent_handler2_get_iid()
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_icomponent_handler2_set_dirty(AgsVstIComponentHandler2 *icomponent_handler2,
						      gboolean state)
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_icomponent_handler2_request_open_editor(AgsVstIComponentHandler2 *icomponent_handler2,
								AgsVstFIDString name)
  {
    //TODO:JK: implement me
  }
  
  AgsVstTResult ags_vst_icomponent_handler2_finish_group_edit(AgsVstIComponentHandler2 *icomponent_handler2)
  {
    //TODO:JK: implement me
  }
  
  const AgsVstTUID* ags_vst_icomponent_handler_bus_activation_get_iid()
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_icomponent_handler_bus_activation_request_bus_activation(AgsVstIComponentHandlerBusActivation *icomponent_handler_bus_activation,
										 AgsVstMediaType media_type, AgsVstBusDirection dir,
										 gint32 index,
										 gboolean state)
  {
    //TODO:JK: implement me
  }
  
  const AgsVstTUID* ags_vst_iprogress_get_iid()
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iprogress_start(AgsVstIProgress *iprogress,
					AgsVstProgressType progress_type,
					ags_vst_tchar *optional_description,
					AgsVstID *out_id)
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iprogress_update(AgsVstIProgress *iprogress,
					 AgsVstID id, AgsVstParamValue norm_value)
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iprogress_finish(AgsVstIProgress *iprogress,
					 AgsVstID id)
  {
    //TODO:JK: implement me
  }
  
  const AgsVstTUID* ags_vst_iedit_controller_get_iid()
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iedit_controller_set_component_state(AgsVstIEditController *iedit_controller,
							     AgsVstIBStream *state)
  {
    //TODO:JK: implement me
  }
  
  AgsVstTResult ags_vst_iedit_controller_set_state(AgsVstIEditController *iedit_controller,
						   AgsVstIBStream *state)
  {
    //TODO:JK: implement me
  }
  
  AgsVstTResult ags_vst_iedit_controller_get_state(AgsVstIEditController *iedit_controller,
						   AgsVstIBStream *state)
  {
    //TODO:JK: implement me
  }
  
  gint32 ags_vst_iedit_controller_get_parameter_count(AgsVstIEditController *iedit_controller)
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iedit_controller_get_parameter_info(AgsVstIEditController *iedit_controller,
							    gint32 paramIndex, AgsVstParameterInfo *info)
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iedit_controller_get_param_string_by_value(AgsVstIEditController *iedit_controller,
								   AgsVstParamID id, AgsVstParamValue value_normalized,
								   AgsVstString128 *string)
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iedit_controller_get_param_value_by_string(AgsVstIEditController *iedit_controller,
								   AgsVstParamID id,
								   AgsVstTChar *string,
								   AgsVstParamValue *value_normalized)
  {
    //TODO:JK: implement me
  }
  
  AgsVstParamValue ags_vst_iedit_controller_normalized_param_to_plain(AgsVstIEditController *iedit_controller,
								      AgsVstParamID id,
								      AgsVstParamValue value_normalized)
  {
    //TODO:JK: implement me
  }

  AgsVstParamValue ags_vst_iedit_controller_plain_param_to_normalized(AgsVstIEditController *iedit_controller,
								      AgsVstParamID id,
								      AgsVstParamValue plain_value)
  {
    //TODO:JK: implement me
  }
  
  AgsVstParamValue ags_vst_iedit_controller_get_param_normalized(AgsVstIEditController *iedit_controller,
								 AgsVstParamID id)
  {
    //TODO:JK: implement me
  }
  
  AgsVstParamValue ags_vst_iedit_controller_set_param_normalized(AgsVstIEditController *iedit_controller,
								 AgsVstParamID id,
								 AgsVstParamValue value)
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iedit_controller_set_component_handler(AgsVstIEditController *iedit_controller,
							       AgsVstIComponent *handler)
  {
    //TODO:JK: implement me
  }

  AgsVstIPlugView* ags_vst_iedit_controller_create_view(AgsVstIEditController *iedit_controller,
							AgsVstFIDString name)
  {
    //TODO:JK: implement me
  }
  
  const AgsVstTUID* ags_vst_iedit_controller2_get_iid()
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iedit_controller_set_knob_mode(AgsVstIEditController2 *iedit_controller2,
						       guint mode)
  {
    //TODO:JK: implement me
  }
  
  AgsVstTResult ags_vst_iedit_controller_open_help(AgsVstIEditController2 *iedit_controller2,
						   gboolean only_check)
  {
    //TODO:JK: implement me
  }
  
  AgsVstTResult ags_vst_iedit_controller_open_about_box(AgsVstIEditController2 *iedit_controller2,
							gboolean only_check)
  {
    //TODO:JK: implement me
  }

  const AgsVstTUID* ags_vst_imidi_mapping_get_iid()
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_imidi_mapping_get_midi_controller_assignment(AgsVstIMidiMapping *imidi_mapping,
								     gint32 bus_index, gint16 channel,
								     AgsVstCtrlNumber midiControllerNumber, AgsVstParamID *id)
  {
    //TODO:JK: implement me
  }

  const AgsVstTUID* ags_vst_iedit_controller_host_editing_get_iid()
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iedit_controller_host_editing_begin_edit_from_host(AgsVstIEditControllerHostEditing *iedit_controller_host_editing,
								      AgsVstParamID param_id)
  {
    //TODO:JK: implement me
  }
  
  AgsVstTResult ags_vst_iedit_controller_host_editing_end_edit_from_host(AgsVstIEditControllerHostEditing *iedit_controller_host_editing,
									 AgsVstParamID param_id)
  {
    //TODO:JK: implement me
  }

}
