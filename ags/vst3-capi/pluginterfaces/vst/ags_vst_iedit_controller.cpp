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

  const AgsVstCString ags_vst_editor = "editor";
  
  AgsVstParameterInfo* ags_vst_parameter_info_alloc()
  {
    return((AgsVstParameterInfo *) new Steinberg::Vst::ParameterInfo);
  }

  void ags_vst_parameter_info_free(AgsVstParameterInfo *info)
  {
    delete ((Steinberg::Vst::ParameterInfo *) info);
  }

  AgsVstParamID ags_vst_parameter_info_get_param_id(AgsVstParameterInfo *info)
  {
    return((AgsVstParamID) ((Steinberg::Vst::ParameterInfo *) info)->id);
  }
  
  gchar* ags_vst_parameter_info_get_title(AgsVstParameterInfo *info)
  {
    return((gchar *) &(((Steinberg::Vst::ParameterInfo *) info)->title));
  }
  
  gchar* ags_vst_parameter_info_get_short_title(AgsVstParameterInfo *info)
  {
    return((gchar *) &(((Steinberg::Vst::ParameterInfo *) info)->shortTitle));
  }
  
  gchar* ags_vst_parameter_info_get_units(AgsVstParameterInfo *info)
  {
    return((gchar *) &(((Steinberg::Vst::ParameterInfo *) info)->units));
  }
  
  gint32 ags_vst_parameter_info_get_step_count(AgsVstParameterInfo *info)
  {
    return((gint32) (((Steinberg::Vst::ParameterInfo *) info)->stepCount));
  }

  AgsVstParamValue ags_vst_parameter_info_get_default_normalized_value(AgsVstParameterInfo *info)
  {
    return((AgsVstParamValue) (((Steinberg::Vst::ParameterInfo *) info)->defaultNormalizedValue));
  }
  
  AgsVstUnitID ags_vst_parameter_info_get_unit_id(AgsVstParameterInfo *info)
  {
    return((AgsVstUnitID) (((Steinberg::Vst::ParameterInfo *) info)->unitId));
  }
  
  guint ags_vst_parameter_info_get_flags(AgsVstParameterInfo *info)
  {
    return((guint) (((Steinberg::Vst::ParameterInfo *) info)->flags));
  }
  
  const AgsVstTUID* ags_vst_icomponent_handler_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IComponentHandler::iid.toTUID()));
  }

  AgsVstTResult ags_vst_icomponent_handler_begin_edit(AgsVstIComponentHandler *icomponent_handler,
						      AgsVstParamID id)
  {
    return(((Steinberg::Vst::IComponentHandler *) icomponent_handler)->beginEdit(id));
  }

  AgsVstTResult ags_vst_icomponent_handler_perform_edit(AgsVstIComponentHandler *icomponent_handler,
							AgsVstParamID id, AgsVstParamValue value_normalized)
  {
    return(((Steinberg::Vst::IComponentHandler *) icomponent_handler)->performEdit(id, value_normalized));
  }

  AgsVstTResult ags_vst_icomponent_handler_end_edit(AgsVstIComponentHandler *icomponent_handler,
						    AgsVstParamID id)
  {
    return(((Steinberg::Vst::IComponentHandler *) icomponent_handler)->endEdit(id));
  }

  AgsVstTResult ags_vst_icomponent_handler_restart_component(AgsVstIComponentHandler *icomponent_handler,
							     gint32 flags)
  {
    return(((Steinberg::Vst::IComponentHandler *) icomponent_handler)->restartComponent(flags));
  }
  
  const AgsVstTUID* ags_vst_icomponent_handler2_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IComponentHandler2::iid.toTUID()));
  }

  AgsVstTResult ags_vst_icomponent_handler2_set_dirty(AgsVstIComponentHandler2 *icomponent_handler2,
						      gboolean state)
  {
    return(((Steinberg::Vst::IComponentHandler2 *) icomponent_handler2)->setDirty(state));
  }

  AgsVstTResult ags_vst_icomponent_handler2_request_open_editor(AgsVstIComponentHandler2 *icomponent_handler2,
								AgsVstFIDString name)
  {
    return(((Steinberg::Vst::IComponentHandler2 *) icomponent_handler2)->requestOpenEditor((Steinberg::FIDString) name));
  }
  
  AgsVstTResult ags_vst_icomponent_handler2_finish_group_edit(AgsVstIComponentHandler2 *icomponent_handler2)
  {
    return(((Steinberg::Vst::IComponentHandler2 *) icomponent_handler2)->finishGroupEdit());
  }

  const AgsVstTUID* ags_vst_icomponent_handler_bus_activation_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IComponentHandlerBusActivation::iid.toTUID()));
  }

  AgsVstTResult ags_vst_icomponent_handler_bus_activation_request_bus_activation(AgsVstIComponentHandlerBusActivation *icomponent_handler_bus_activation,
										 AgsVstMediaType media_type, AgsVstBusDirection dir,
										 gint32 index,
										 gboolean state)
  {
    return(((Steinberg::Vst::IComponentHandlerBusActivation *) icomponent_handler_bus_activation)->requestBusActivation(media_type, dir,
															index,
															state));
  }
  
  const AgsVstTUID* ags_vst_iprogress_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IProgress::iid.toTUID()));
  }

  AgsVstTResult ags_vst_iprogress_start(AgsVstIProgress *iprogress,
					AgsVstProgressType progress_type,
					ags_vst_tchar *optional_description,
					AgsVstID *out_id)
  {
    return(((Steinberg::Vst::IProgress *) iprogress)->start((Steinberg::Vst::IProgress::ProgressType) progress_type,
							    (Steinberg::tchar *) optional_description,
							    const_cast<Steinberg::Vst::IProgress::ID&>(((Steinberg::Vst::IProgress::ID *) out_id)[0])));
  }

  AgsVstTResult ags_vst_iprogress_update(AgsVstIProgress *iprogress,
					 AgsVstID id, AgsVstParamValue norm_value)
  {
    return(((Steinberg::Vst::IProgress *) iprogress)->update((Steinberg::Vst::IProgress::ID) id,
							     norm_value));
  }

  AgsVstTResult ags_vst_iprogress_finish(AgsVstIProgress *iprogress,
					 AgsVstID id)
  {
    return(((Steinberg::Vst::IProgress *) iprogress)->finish((Steinberg::Vst::IProgress::ID) id));
  }
  
  const AgsVstTUID* ags_vst_iedit_controller_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IEditController::iid.toTUID()));
  }

  AgsVstTResult ags_vst_iedit_controller_set_component_state(AgsVstIEditController *iedit_controller,
							     AgsVstIBStream *state)
  {
    return(((Steinberg::Vst::IEditController *) iedit_controller)->setComponentState((Steinberg::IBStream *) state));
  }
  
  AgsVstTResult ags_vst_iedit_controller_set_state(AgsVstIEditController *iedit_controller,
						   AgsVstIBStream *state)
  {
    return(((Steinberg::Vst::IEditController *) iedit_controller)->setState((Steinberg::IBStream *) state));
  }
  
  AgsVstTResult ags_vst_iedit_controller_get_state(AgsVstIEditController *iedit_controller,
						   AgsVstIBStream *state)
  {
    return(((Steinberg::Vst::IEditController *) iedit_controller)->getState((Steinberg::IBStream *) state));
  }
  
  gint32 ags_vst_iedit_controller_get_parameter_count(AgsVstIEditController *iedit_controller)
  {
    return(((Steinberg::Vst::IEditController *) iedit_controller)->getParameterCount());
  }

  AgsVstTResult ags_vst_iedit_controller_get_parameter_info(AgsVstIEditController *iedit_controller,
							    gint32 param_index, AgsVstParameterInfo *info)
  {
    return(((Steinberg::Vst::IEditController *) iedit_controller)->getParameterInfo(param_index,
										    const_cast<Steinberg::Vst::ParameterInfo&>(((Steinberg::Vst::ParameterInfo *) info)[0])));
  }

  AgsVstTResult ags_vst_iedit_controller_get_param_string_by_value(AgsVstIEditController *iedit_controller,
								   AgsVstParamID id, AgsVstParamValue value_normalized,
								   gchar *string)
  {
    Steinberg::Vst::String128 tmp_string;
    Steinberg::tresult retval;
    
    retval = ((Steinberg::Vst::IEditController *) iedit_controller)->getParamStringByValue(id, value_normalized,
											   tmp_string);

    memcpy(string, &tmp_string, sizeof(Steinberg::Vst::String128));
      
    return(retval);
  }

  AgsVstTResult ags_vst_iedit_controller_get_param_value_by_string(AgsVstIEditController *iedit_controller,
								   AgsVstParamID id,
								   AgsVstTChar *string,
								   AgsVstParamValue *value_normalized)
  {
    return(((Steinberg::Vst::IEditController *) iedit_controller)->getParamValueByString(id,
											 (Steinberg::Vst::TChar *) string,
											 const_cast<Steinberg::Vst::ParamValue&>(((Steinberg::Vst::ParamValue *) value_normalized)[0])));
  }
  
  AgsVstParamValue ags_vst_iedit_controller_normalized_param_to_plain(AgsVstIEditController *iedit_controller,
								      AgsVstParamID id,
								      AgsVstParamValue value_normalized)
  {
    return(((Steinberg::Vst::IEditController *) iedit_controller)->normalizedParamToPlain(id,
											  value_normalized));
  }

  AgsVstParamValue ags_vst_iedit_controller_plain_param_to_normalized(AgsVstIEditController *iedit_controller,
								      AgsVstParamID id,
								      AgsVstParamValue plain_value)
  {
    return(((Steinberg::Vst::IEditController *) iedit_controller)->plainParamToNormalized(id,
											  plain_value));
  }
  
  AgsVstParamValue ags_vst_iedit_controller_get_param_normalized(AgsVstIEditController *iedit_controller,
								 AgsVstParamID id)
  {
    return(((Steinberg::Vst::IEditController *) iedit_controller)->getParamNormalized(id));
  }
  
  AgsVstParamValue ags_vst_iedit_controller_set_param_normalized(AgsVstIEditController *iedit_controller,
								 AgsVstParamID id,
								 AgsVstParamValue value)
  {
    return(((Steinberg::Vst::IEditController *) iedit_controller)->setParamNormalized(id, value));
  }

  AgsVstTResult ags_vst_iedit_controller_set_component_handler(AgsVstIEditController *iedit_controller,
							       AgsVstIComponentHandler *handler)
  {
    return(((Steinberg::Vst::IEditController *) iedit_controller)->setComponentHandler((Steinberg::Vst::IComponentHandler *) handler));
  }

  AgsVstIPlugView* ags_vst_iedit_controller_create_view(AgsVstIEditController *iedit_controller,
							AgsVstFIDString name)
  {
    return((AgsVstIPlugView *) ((Steinberg::Vst::IEditController *) iedit_controller)->createView(name));
  }
  
  const AgsVstTUID* ags_vst_iedit_controller2_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IEditController2::iid.toTUID()));
  }

  AgsVstTResult ags_vst_iedit_controller2_set_knob_mode(AgsVstIEditController2 *iedit_controller2,
						       guint mode)
  {
    return(((Steinberg::Vst::IEditController2 *) iedit_controller2)->setKnobMode(mode));
  }
  
  AgsVstTResult ags_vst_iedit_controller2_open_help(AgsVstIEditController2 *iedit_controller2,
						    gboolean only_check)
  {
    return(((Steinberg::Vst::IEditController2 *) iedit_controller2)->openHelp(only_check));
  }
  
  AgsVstTResult ags_vst_iedit_controller2_open_about_box(AgsVstIEditController2 *iedit_controller2,
							 gboolean only_check)
  {
    return(((Steinberg::Vst::IEditController2 *) iedit_controller2)->openAboutBox(only_check));
  }

  const AgsVstTUID* ags_vst_imidi_mapping_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IMidiMapping::iid.toTUID()));
  }

  AgsVstTResult ags_vst_imidi_mapping_get_midi_controller_assignment(AgsVstIMidiMapping *imidi_mapping,
								     gint32 bus_index, gint16 channel,
								     AgsVstCtrlNumber midi_controller_number, AgsVstParamID *id)
  {
    return(((Steinberg::Vst::IMidiMapping *) imidi_mapping)->getMidiControllerAssignment(bus_index, channel,
											 midi_controller_number, const_cast<Steinberg::Vst::ParamID&>(((Steinberg::Vst::ParamID *) id)[0])));
  }

  const AgsVstTUID* ags_vst_iedit_controller_host_editing_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IEditControllerHostEditing::iid.toTUID()));
  }

  AgsVstTResult ags_vst_iedit_controller_host_editing_begin_edit_from_host(AgsVstIEditControllerHostEditing *iedit_controller_host_editing,
									   AgsVstParamID param_id)
  {
    return(((Steinberg::Vst::IEditControllerHostEditing *) iedit_controller_host_editing)->beginEditFromHost(param_id));
  }
  
  AgsVstTResult ags_vst_iedit_controller_host_editing_end_edit_from_host(AgsVstIEditControllerHostEditing *iedit_controller_host_editing,
									 AgsVstParamID param_id)
  {
    return(((Steinberg::Vst::IEditControllerHostEditing *) iedit_controller_host_editing)->endEditFromHost(param_id));
  }

}
