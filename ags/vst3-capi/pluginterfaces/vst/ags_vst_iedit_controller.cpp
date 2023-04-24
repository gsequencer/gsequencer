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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iedit_controller.h>

#include <ags/vst3-capi/host/ags_vst_host_context.h>

#include <pluginterfaces/vst/ivsteditcontroller.h>

#include <public.sdk/source/vst/vstcomponent.h>

extern "C" {

  const AgsVstCString ags_vst_editor = "editor";

  /**
   * Allocate parameter info.
   *
   * @return the AgsVstParameterInfo
   *
   * @since 5.0.0
   */
  AgsVstParameterInfo* ags_vst_parameter_info_alloc()
  {
    return((AgsVstParameterInfo *) new Steinberg::Vst::ParameterInfo());
  }

  /**
   * Free parameter info.
   *
   * @param info the parameter info
   *
   * @since 5.0.0
   */
  void ags_vst_parameter_info_free(AgsVstParameterInfo *info)
  {
    delete ((Steinberg::Vst::ParameterInfo *) info);
  }

  /**
   * Get param id.
   *
   * @param info the parameter info
   * @return the parameter identifier
   * 
   * @since 5.0.0
   */
  AgsVstParamID ags_vst_parameter_info_get_param_id(AgsVstParameterInfo *info)
  {
    return((AgsVstParamID) ((Steinberg::Vst::ParameterInfo *) info)->id);
  }

  /**
   * Get title.
   *
   * @param info the parameter info
   * @return the title
   *
   * @since 5.0.0
   */
  gchar* ags_vst_parameter_info_get_title(AgsVstParameterInfo *info)
  {
    return((gchar *) &(((Steinberg::Vst::ParameterInfo *) info)->title));
  }
  
  /**
   * Get short title.
   *
   * @param info the parameter info
   * @return the short title
   *
   * @since 5.0.0
   */
  gchar* ags_vst_parameter_info_get_short_title(AgsVstParameterInfo *info)
  {
    return((gchar *) &(((Steinberg::Vst::ParameterInfo *) info)->shortTitle));
  }
  
  /**
   * Get units.
   *
   * @param info the parameter info
   * @return the units
   *
   * @since 5.0.0
   */
  gchar* ags_vst_parameter_info_get_units(AgsVstParameterInfo *info)
  {
    return((gchar *) &(((Steinberg::Vst::ParameterInfo *) info)->units));
  }
  
  /**
   * Get step count.
   *
   * @param info the parameter info
   * @return the step count
   *
   * @since 5.0.0
   */
  gint32 ags_vst_parameter_info_get_step_count(AgsVstParameterInfo *info)
  {
    return((gint32) (((Steinberg::Vst::ParameterInfo *) info)->stepCount));
  }

  /**
   * Get default normalized value.
   *
   * @param info the parameter info
   * @return the normalized value
   *
   * @since 5.0.0
   */
  AgsVstParamValue ags_vst_parameter_info_get_default_normalized_value(AgsVstParameterInfo *info)
  {
    return((AgsVstParamValue) (((Steinberg::Vst::ParameterInfo *) info)->defaultNormalizedValue));
  }
  
  /**
   * Get unit identifier.
   *
   * @param info the parameter info
   * @return the unit ID
   *
   * @since 5.0.0
   */
  AgsVstUnitID ags_vst_parameter_info_get_unit_id(AgsVstParameterInfo *info)
  {
    return((AgsVstUnitID) (((Steinberg::Vst::ParameterInfo *) info)->unitId));
  }
  
  /**
   * Get flags.
   *
   * @param info the parameter info
   * @return the flags
   *
   * @since 5.0.0
   */
  guint ags_vst_parameter_info_get_flags(AgsVstParameterInfo *info)
  {
    return((guint) (((Steinberg::Vst::ParameterInfo *) info)->flags));
  }
  
  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_icomponent_handler_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IComponentHandler::iid.toTUID()));
  }

  /**
   * Begin edit.
   *
   * @param icomponent_handler the component handler
   * @param id the parameter identifier
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_icomponent_handler_begin_edit(AgsVstIComponentHandler *icomponent_handler,
						      AgsVstParamID id)
  {
    return(((Steinberg::Vst::IComponentHandler *) icomponent_handler)->beginEdit(id));
  }

  /**
   * Perform edit.
   *
   * @param icomponent_handler the component handler
   * @param id the parameter identifier
   * @param value_normalized the normalized value
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_icomponent_handler_perform_edit(AgsVstIComponentHandler *icomponent_handler,
							AgsVstParamID id, AgsVstParamValue value_normalized)
  {
    return(((Steinberg::Vst::IComponentHandler *) icomponent_handler)->performEdit(id, value_normalized));
  }

  /**
   * End edit.
   *
   * @param icomponent_handler the component handler
   * @param id the parameter identifier
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_icomponent_handler_end_edit(AgsVstIComponentHandler *icomponent_handler,
						    AgsVstParamID id)
  {
    return(((Steinberg::Vst::IComponentHandler *) icomponent_handler)->endEdit(id));
  }

  /**
   * Restart component.
   *
   * @param icomponent_handler the component handler
   * @param flats the flags
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_icomponent_handler_restart_component(AgsVstIComponentHandler *icomponent_handler,
							     gint32 flags)
  {
    return(((Steinberg::Vst::IComponentHandler *) icomponent_handler)->restartComponent(flags));
  }
  
  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_icomponent_handler2_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IComponentHandler2::iid.toTUID()));
  }

  /**
   * Set dirty.
   *
   * @param icomponent_handler2 the component handler2
   * @param state true if dirty, otherwise false
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_icomponent_handler2_set_dirty(AgsVstIComponentHandler2 *icomponent_handler2,
						      gboolean state)
  {
    return(((Steinberg::Vst::IComponentHandler2 *) icomponent_handler2)->setDirty(state));
  }

  /**
   * Request open editor.
   *
   * @param icomponent_handler2 the component handler2
   * @param name the name
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_icomponent_handler2_request_open_editor(AgsVstIComponentHandler2 *icomponent_handler2,
								AgsVstFIDString name)
  {
    return(((Steinberg::Vst::IComponentHandler2 *) icomponent_handler2)->requestOpenEditor((Steinberg::FIDString) name));
  }
  
  /**
   * Finish group edit.
   *
   * @param icomponent_handler2 the component handler2
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_icomponent_handler2_finish_group_edit(AgsVstIComponentHandler2 *icomponent_handler2)
  {
    return(((Steinberg::Vst::IComponentHandler2 *) icomponent_handler2)->finishGroupEdit());
  }

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
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
  
  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
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

  /**
   * Update.
   *
   * @param iprogress the iprogress
   * @param id the identifier
   * @param norm_value the normalized value
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iprogress_update(AgsVstIProgress *iprogress,
					 AgsVstID id, AgsVstParamValue norm_value)
  {
    return(((Steinberg::Vst::IProgress *) iprogress)->update((Steinberg::Vst::IProgress::ID) id,
							     norm_value));
  }

  /**
   * Finish.
   *
   * @param iprogress the iprogress
   * @param id the identifier
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iprogress_finish(AgsVstIProgress *iprogress,
					 AgsVstID id)
  {
    return(((Steinberg::Vst::IProgress *) iprogress)->finish((Steinberg::Vst::IProgress::ID) id));
  }
  
  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_iedit_controller_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IEditController::iid.toTUID()));
  }
  
  /**
   * Set component state.
   *
   * @param iedit_controller iedit controller
   * @param state the state
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iedit_controller_set_component_state(AgsVstIEditController *iedit_controller,
							     AgsVstIBStream *state)
  {
    return(((Steinberg::Vst::IEditController *) iedit_controller)->setComponentState((Steinberg::IBStream *) state));
  }
  
  /**
   * Set state.
   *
   * @param iedit_controller iedit-controller
   * @param state the stream
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iedit_controller_set_state(AgsVstIEditController *iedit_controller,
						   AgsVstIBStream *state)
  {
    return(((Steinberg::Vst::IEditController *) iedit_controller)->setState((Steinberg::IBStream *) state));
  }
  
  /**
   * Get state.
   *
   * @param iedit_controller iedit-controller
   * @param state the stream
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iedit_controller_get_state(AgsVstIEditController *iedit_controller,
						   AgsVstIBStream *state)
  {
    return(((Steinberg::Vst::IEditController *) iedit_controller)->getState((Steinberg::IBStream *) state));
  }
  
  /**
   * Get parameter count.
   *
   * @param iedit_controller iedit-controller
   * @return the parameter count
   *
   * @since 5.0.0
   */
  gint32 ags_vst_iedit_controller_get_parameter_count(AgsVstIEditController *iedit_controller)
  {
    return(((Steinberg::Vst::IEditController *) iedit_controller)->getParameterCount());
  }

  /**
   * Get parameter info.
   *
   * @param iedit_controller iedit-controller
   * @param param_index the parameter index
   * @param info the parameter info
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iedit_controller_get_parameter_info(AgsVstIEditController *iedit_controller,
							    gint32 param_index, AgsVstParameterInfo *info)
  {
    return(((Steinberg::Vst::IEditController *) iedit_controller)->getParameterInfo(param_index,
										    const_cast<Steinberg::Vst::ParameterInfo&>(((Steinberg::Vst::ParameterInfo *) info)[0])));
  }

  /**
   * Get parameter string by value.
   *
   * @param iedit_controller iedit-controller
   * @param id the identifier
   * @param value_normalized the normalized value
   * @param string the string
   * @return the return value
   *
   * @since 5.0.0
   */
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

  /**
   * Get parameter value by string.
   *
   * @param iedit_controller iedit-controller
   * @param id the identifier
   * @param string the string
   * @param value_normalized the normalized value
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iedit_controller_get_param_value_by_string(AgsVstIEditController *iedit_controller,
								   AgsVstParamID id,
								   AgsVstTChar *string,
								   AgsVstParamValue *value_normalized)
  {
    return(((Steinberg::Vst::IEditController *) iedit_controller)->getParamValueByString(id,
											 (Steinberg::Vst::TChar *) string,
											 const_cast<Steinberg::Vst::ParamValue&>(((Steinberg::Vst::ParamValue *) value_normalized)[0])));
  }

  /**
   * Normalized parameter to plain.
   *
   * @param iedit_controller iedit-controller
   * @param id the identifier
   * @param value_normalized the normalized value
   *
   * @since 5.0.0
   */
  AgsVstParamValue ags_vst_iedit_controller_normalized_param_to_plain(AgsVstIEditController *iedit_controller,
								      AgsVstParamID id,
								      AgsVstParamValue value_normalized)
  {
    return(((Steinberg::Vst::IEditController *) iedit_controller)->normalizedParamToPlain(id,
											  value_normalized));
  }

  /**
   * Plain parameter to normalized.
   *
   * @param iedit_controller iedit-controller
   * @param id the identifier
   * @param plain_value the plain value
   * @return the parameter value
   * 
   * @since 5.0.0
   */
  AgsVstParamValue ags_vst_iedit_controller_plain_param_to_normalized(AgsVstIEditController *iedit_controller,
								      AgsVstParamID id,
								      AgsVstParamValue plain_value)
  {
    return(((Steinberg::Vst::IEditController *) iedit_controller)->plainParamToNormalized(id,
											  plain_value));
  }
  
  /**
   * Get parameter normalized.
   *
   * @param iedit_controller iedit-controller
   * @param id the identifier
   * @return the parameter value
   * 
   * @since 5.0.0
   */
  AgsVstParamValue ags_vst_iedit_controller_get_param_normalized(AgsVstIEditController *iedit_controller,
								 AgsVstParamID id)
  {
    return(((Steinberg::Vst::IEditController *) iedit_controller)->getParamNormalized(id));
  }
  
  /**
   * Set parameter normalized.
   *
   * @param iedit_controller iedit-controller
   * @param id the identifier
   * @param value the parameter value
   * @return the parameter value
   * 
   * @since 5.0.0
   */
  AgsVstParamValue ags_vst_iedit_controller_set_param_normalized(AgsVstIEditController *iedit_controller,
								 AgsVstParamID id,
								 AgsVstParamValue value)
  {
    return(((Steinberg::Vst::IEditController *) iedit_controller)->setParamNormalized(id, value));
  }

  /**
   * Set component handler.
   *
   * @param iedit_controller iedit-controller
   * @param handler the component handler
   * @return the parameter value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iedit_controller_set_component_handler(AgsVstIEditController *iedit_controller,
							       AgsVstIComponentHandler *handler)
  {
    return(((Steinberg::Vst::IEditController *) iedit_controller)->setComponentHandler((Steinberg::Vst::IComponentHandler *) handler));
  }

  /**
   * Create view.
   *
   * @param iedit_controller iedit-controller
   * @param name the name
   * @return the plug-view
   * 
   * @since 5.0.0
   */
  AgsVstIPlugView* ags_vst_iedit_controller_create_view(AgsVstIEditController *iedit_controller,
							AgsVstFIDString name)
  {
    return((AgsVstIPlugView *) ((Steinberg::Vst::IEditController *) iedit_controller)->createView(name));
  }
  
  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_iedit_controller2_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IEditController2::iid.toTUID()));
  }

  /**
   * Set knob mode.
   * 
   * @param iedit_controller iedit-controller2
   * @param mode the know mode
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iedit_controller2_set_knob_mode(AgsVstIEditController2 *iedit_controller2,
							guint mode)
  {
    return(((Steinberg::Vst::IEditController2 *) iedit_controller2)->setKnobMode(mode));
  }
  
  /**
   * Open help.
   * 
   * @param iedit_controller iedit-controller2
   * @param only_check true if check, otherwise false
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iedit_controller2_open_help(AgsVstIEditController2 *iedit_controller2,
						    gboolean only_check)
  {
    return(((Steinberg::Vst::IEditController2 *) iedit_controller2)->openHelp(only_check));
  }
  
  /**
   * Open about box.
   * 
   * @param iedit_controller iedit-controller2
   * @param only_check true if check, otherwise false
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iedit_controller2_open_about_box(AgsVstIEditController2 *iedit_controller2,
							 gboolean only_check)
  {
    return(((Steinberg::Vst::IEditController2 *) iedit_controller2)->openAboutBox(only_check));
  }

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_imidi_mapping_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IMidiMapping::iid.toTUID()));
  }

  /**
   * Get midi controller assignment.
   * 
   * @param imidi_mapping imidi-mapping
   * @param bus_index the bus index
   * @param channel the channel
   * @param midi_controller_number midi controller number
   * @param id the identifier 
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_imidi_mapping_get_midi_controller_assignment(AgsVstIMidiMapping *imidi_mapping,
								     gint32 bus_index, gint16 channel,
								     AgsVstCtrlNumber midi_controller_number, AgsVstParamID *id)
  {
    return(((Steinberg::Vst::IMidiMapping *) imidi_mapping)->getMidiControllerAssignment(bus_index, channel,
											 midi_controller_number, const_cast<Steinberg::Vst::ParamID&>(((Steinberg::Vst::ParamID *) id)[0])));
  }

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_iedit_controller_host_editing_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IEditControllerHostEditing::iid.toTUID()));
  }

  /**
   * Begin edit from host.
   * 
   * @param iedit_controller_host_editing the iedit-controller host editing
   * @param param_id the parameter identifier
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iedit_controller_host_editing_begin_edit_from_host(AgsVstIEditControllerHostEditing *iedit_controller_host_editing,
									   AgsVstParamID param_id)
  {
    return(((Steinberg::Vst::IEditControllerHostEditing *) iedit_controller_host_editing)->beginEditFromHost(param_id));
  }
  
  /**
   * End edit from host.
   * 
   * @param iedit_controller_host_editing the iedit-controller host editing
   * @param param_id the parameter identifier
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iedit_controller_host_editing_end_edit_from_host(AgsVstIEditControllerHostEditing *iedit_controller_host_editing,
									 AgsVstParamID param_id)
  {
    return(((Steinberg::Vst::IEditControllerHostEditing *) iedit_controller_host_editing)->endEditFromHost(param_id));
  }

}
