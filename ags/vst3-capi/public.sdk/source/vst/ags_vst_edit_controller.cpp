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

#include <ags/vst3-capi/public.sdk/source/vst/ags_vst_edit_controller.h>

#include <public.sdk/source/vst/vsteditcontroller.h>

#include <string.h>

extern "C" {

  /**
   * Instantiate new Steinberg::Vst::EditController the constructor as a C99
   * compatible handle a void pointer.
   * 
   * @return the AgsVstEditController
   * 
   * @since 5.0.0
   */
  AgsVstEditController* ags_vst_edit_controller_new()
  {
    return((AgsVstEditController *) new Steinberg::Vst::EditController());
  }
  
  /**
   * Delete.
   *
   * @param edit_controller the edit controller
   *
   * @since 5.0.0
   */
  void ags_vst_edit_controller_delete(AgsVstEditController *edit_controller)
  {
    delete edit_controller;
  }

  /**
   * Set component state.
   *
   * @param edit_controller the edit controller
   * @param state the state
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_set_component_state(AgsVstEditController *edit_controller, AgsVstIBStream *state)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->setComponentState(static_cast<Steinberg::IBStream*>((void *) state)));
  }

  /**
   * Set state.
   *
   * @param edit_controller the edit controller
   * @param state the state
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_set_state(AgsVstEditController *edit_controller, AgsVstIBStream *state)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->setState(static_cast<Steinberg::IBStream*>((void *) state)));
  }

  /**
   * Get state.
   *
   * @param edit_controller the edit controller
   * @param state the state
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_get_state(AgsVstEditController *edit_controller, AgsVstIBStream *state)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->getState(static_cast<Steinberg::IBStream*>((void *) state)));
  }

  /**
   * Get parameter count.
   *
   * @param edit_controller the edit controller
   * @return the parameter count
   *
   * @since 5.0.0
   */
  gint32 ags_vst_edit_controller_get_parameter_count(AgsVstEditController *edit_controller)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->getParameterCount());
  }

  /**
   * Get parameter info.
   *
   * @param edit_controller the edit controller
   * @param param_index parameter index
   * @param info the parameter info
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_get_parameter_info(AgsVstEditController *edit_controller,
							   gint32 param_index, AgsVstParameterInfo *info)
  {
    Steinberg::Vst::ParameterInfo *tmp_info_0 = static_cast<Steinberg::Vst::ParameterInfo*>((void *) info);
    const Steinberg::Vst::ParameterInfo& tmp_info_1 = const_cast<Steinberg::Vst::ParameterInfo&>(tmp_info_0[0]);
    
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->getParameterInfo(param_index, const_cast<Steinberg::Vst::ParameterInfo&>(tmp_info_1)));
  }

  /**
   * Get parameter string by value.
   *
   * @param edit_controller the edit controller
   * @param tag the tag
   * @param value_normalized the normalized value
   * @str the string
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_get_param_string_by_value(AgsVstEditController *edit_controller,
								  guint32 tag, gdouble value_normalized, AgsVstString128 str)
  {
    Steinberg::Vst::String128 tmp_str_0;
    Steinberg::tresult retval;

    retval = static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->getParamStringByValue(tag, value_normalized, tmp_str_0);

    memcpy(str, tmp_str_0, 128 * sizeof(char16_t));
    
    return(retval);
  }

  /**
   * Get parameter value by string.
   *
   * @param edit_controller the edit controller
   * @param tag the tag
   * @str the string
   * @param value_normalized the normalized value
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_get_param_value_by_string(AgsVstEditController *edit_controller,
								  guint32 tag, gunichar2 *string, gdouble *value_normalized)
  {
    Steinberg::Vst::ParamValue tmp_value_normalized_0;
    const Steinberg::Vst::ParamValue& tmp_value_normalized_1 = const_cast<Steinberg::Vst::ParamValue&>(tmp_value_normalized_0);
    
    Steinberg::tresult retval;

    retval = static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->getParamValueByString(tag, (char16_t *) string, const_cast<Steinberg::Vst::ParamValue&>(tmp_value_normalized_1));

    if(value_normalized != NULL){
      value_normalized[0] = tmp_value_normalized_0;
    }
    
    return(retval);
  }

  /**
   * Normalized parameter to plain.
   *
   * @param edit_controller the edit controller
   * @param value_normalized the normalized value
   * @return the plain value
   *
   * @since 5.0.0
   */
  gdouble ags_vst_edit_controller_normalized_param_to_plain(AgsVstEditController *edit_controller,
							    guint32 tag, gdouble value_normalized)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->normalizedParamToPlain(tag, value_normalized));
  }

  /**
   * Plain parameter to normalized.
   *
   * @param edit_controller the edit controller
   * @param plain_value the plain value
   * @return the normalized value
   *
   * @since 5.0.0
   */
  gdouble ags_vst_edit_controller_plain_param_to_normalized(AgsVstEditController *edit_controller,
							    guint32 tag, gdouble plain_value)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->plainParamToNormalized(tag, plain_value));
  }

  /**
   * Get parameter normalized.
   *
   * @param edit_controller the edit controller
   * @param tag the tag
   * @return the normalized value
   *
   * @since 5.0.0
   */
  gdouble ags_vst_edit_controller_get_param_normalized(AgsVstEditController *edit_controller,
						       guint32 tag)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->getParamNormalized(tag));
  }

  /**
   * Set parameter normalized.
   *
   * @param edit_controller the edit controller
   * @param tag the tag
   * @param value the value
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_set_param_normalized(AgsVstEditController *edit_controller,
							     guint32 tag, gdouble value)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->setParamNormalized(tag, value));
  }

  /**
   * Set component handler.
   *
   * @param edit_controller the edit controller
   * @param handler the component handler
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_set_component_handler(AgsVstEditController *edit_controller,
							      AgsVstIComponentHandler *handler)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->setComponentHandler(static_cast<Steinberg::Vst::IComponentHandler*>((void *) handler)));
  }

  /**
   * Create view.
   *
   * @param edit_controller the edit controller
   * @param view_name the view name
   * @return the iplug view
   *
   * @since 5.0.0
   */
  AgsVstIPlugView* ags_vst_edit_controller_create_view(AgsVstEditController *edit_controller,
						       AgsVstFIDString view_name)
  {
    return((AgsVstIPlugView *) static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->createView(view_name));
  }

  /**
   * Set knob mode.
   *
   * @param edit_controller the edit controller
   * @param mode the knob mode
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_set_knob_mode(AgsVstEditController *edit_controller,
						      guint mode)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->setKnobMode(mode));
  }

  /**
   * Open help.
   *
   * @param edit_controller the edit controller
   * @param only_check the only check
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_open_help(AgsVstEditController *edit_controller,
						  gboolean only_check)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->openHelp(only_check));
  }

  /**
   * Open about box.
   *
   * @param edit_controller the edit controller
   * @param only_check the only check
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_open_about_box(AgsVstEditController *edit_controller,
						       gboolean only_check)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->openAboutBox(only_check));
  }

  /**
   * Initialize.
   *
   * @param edit_controller the edit controller
   * @param context the context
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_initialize(AgsVstEditController *edit_controller,
						   AgsVstFUnknown *context)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->initialize(static_cast<Steinberg::FUnknown*>((void *) context)));
  }

  /**
   * Terminate.
   *
   * @param edit_controller the edit controller
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_terminate(AgsVstEditController *edit_controller)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->terminate());
  }

  /**
   * Begin edit.
   *
   * @param edit_controller the edit controller
   * @param tag the tag
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_begin_edit(AgsVstEditController *edit_controller,
						   guint32 tag)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->beginEdit(tag));
  }

  /**
   * Perform edit.
   *
   * @param edit_controller the edit controller
   * @param tag the tag
   * @param value_normalized the normalized value
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_perform_edit(AgsVstEditController *edit_controller,
						     guint32 tag, gdouble value_normalized)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->performEdit(tag, value_normalized));
  }

  /**
   * End edit.
   *
   * @param edit_controller the edit controller
   * @param tag the tag
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_end_edit(AgsVstEditController *edit_controller,
						 guint32 tag)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->endEdit(tag));
  }

  /**
   * Start group edit.
   *
   * @param edit_controller the edit controller
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_start_group_edit(AgsVstEditController *edit_controller)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->startGroupEdit());
  }

  /**
   * Finish group edit.
   *
   * @param edit_controller the edit controller
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_finish_group_edit(AgsVstEditController *edit_controller)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->finishGroupEdit());
  }

  /**
   * Editor destroyed.
   *
   * @param edit_controller the edit controller
   * @param editor the editor
   *
   * @since 5.0.0
   */
  void ags_vst_edit_controller_editor_destroyed(AgsVstEditController *edit_controller,
						AgsVstEditorView *editor)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->editorDestroyed(static_cast<Steinberg::Vst::EditorView*>((void *) editor)));
  }

  /**
   * Editor attached.
   *
   * @param edit_controller the edit controller
   * @param editor the editor
   *
   * @since 5.0.0
   */
  void ags_vst_edit_controller_editor_attached(AgsVstEditController *edit_controller,
					       AgsVstEditorView *editor)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->editorAttached(static_cast<Steinberg::Vst::EditorView*>((void *) editor)));
  }

  /**
   * Editor removed.
   *
   * @param edit_controller the edit controller
   * @param editor the editor
   *
   * @since 5.0.0
   */
  void ags_vst_edit_controller_editor_removed(AgsVstEditController *edit_controller,
					      AgsVstEditorView *editor)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->editorRemoved(static_cast<Steinberg::Vst::EditorView*>((void *) editor)));
  }

  /**
   * Get host knob mode.
   *
   * @param edit_controller the edit controller
   * @return the host know mode
   *
   * @since 5.0.0
   */
  guint ags_vst_edit_controller_get_host_knob_mode(AgsVstEditController *edit_controller)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->getHostKnobMode());
  }

  /**
   * Get parameter object.
   *
   * @param edit_controller the edit controller
   * @param tag the tag
   * @return the parameter object
   *
   * @since 5.0.0
   */
  AgsVstParameter* ags_vst_edit_controller_get_parameter_object(AgsVstEditController *edit_controller,
								guint32 tag)
  {
    return((AgsVstParameter *) static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->getParameterObject(tag));
  }

  /**
   * Get parameter object.
   *
   * @param edit_controller the edit controller
   * @param tag the tag
   * @param info the parameter info
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_get_parameter_info_by_tag(AgsVstEditController *edit_controller,
								  guint32 tag, AgsVstParameterInfo *info)
  {
    Steinberg::Vst::ParameterInfo *tmp_info_0 = (Steinberg::Vst::ParameterInfo *) info;
    const Steinberg::Vst::ParameterInfo& tmp_info_1 = const_cast<Steinberg::Vst::ParameterInfo&>(tmp_info_0[0]);

    Steinberg::tresult retval;

    retval = static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->getParameterInfoByTag(tag, const_cast<Steinberg::Vst::ParameterInfo&>(tmp_info_1));
    
    return(retval);
  }

  /**
   * Set dirty.
   *
   * @param edit_controller the edit controller
   * @param state true if dirty, otherwise false
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_set_dirty(AgsVstEditController *edit_controller,
						  gboolean state)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->setDirty(state));
  }

  /**
   * Request open editor.
   *
   * @param edit_controller the edit controller
   * @param editor_name the editor name
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_request_open_editor(AgsVstEditController *edit_controller,
							    AgsVstFIDString editor_name)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->requestOpenEditor(editor_name));
  }

  /**
   * Get component handler.
   *
   * @param edit_controller the edit controller
   * @return the component handler
   *
   * @since 5.0.0
   */
  AgsVstIComponentHandler* ags_vst_edit_controller_get_component_handler(AgsVstEditController *edit_controller)
  {
    return((AgsVstIComponentHandler *) static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->getComponentHandler());
  }
    
  /**
   * Instantiate new Steinberg::Vst::EditorView the constructor as a C99
   * compatible handle a void pointer.
   * 
   * @param edit_controller the edit controller
   * @param size the view rect
   * @return the AgsVstEditorView
   * 
   * @since 5.0.0
   */
  AgsVstEditorView* ags_vst_editor_view_new(AgsVstEditController *edit_controller, AgsVstViewRect *size)
  {
    return((AgsVstEditorView *) new Steinberg::Vst::EditorView(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller), (struct Steinberg::ViewRect *) size));
  }

  /**
   * Delete.
   *
   * @param editor_view the editor view
   *
   * @since 5.0.0
   */
  void ags_vst_editor_view_delete(AgsVstEditorView *editor_view)
  {
    delete editor_view;
  }

  /**
   * Get controller.
   *
   * @param editor_view the editor view
   * @return the edit controller
   *
   * @since 5.0.0
   */
  AgsVstEditController* ags_vst_editor_view_get_controller(AgsVstEditorView *editor_view)
  {
    return((AgsVstEditController *) static_cast<Steinberg::Vst::EditorView*>((void *) editor_view)->getController());
  }
  
  /**
   * Attached to parent.
   *
   * @param editor_view the editor view
   *
   * @since 5.0.0
   */
  void ags_vst_editor_view_attached_to_parent(AgsVstEditorView *editor_view)
  {
    return(static_cast<Steinberg::Vst::EditorView*>((void *) editor_view)->attachedToParent());
  }

  /**
   * Removed from parent.
   *
   * @param editor_view the editor view
   *
   * @since 5.0.0
   */
  void ags_vst_editor_view_removed_from_parent(AgsVstEditorView *editor_view)
  {
    return(static_cast<Steinberg::Vst::EditorView*>((void *) editor_view)->removedFromParent());
  }
  
  /**
   * Instantiate new Steinberg::Vst::Unit the constructor as a C99
   * compatible handle a void pointer.
   *
   * @param unit_name the unit name
   * @param unit_id the unit identifier
   * @param parent_unit_id the parent unit identifier
   * @param program_list_id the program list identifier
   * @return the AgsVstUnit
   * 
   * @since 5.0.0
   */
  AgsVstUnit* ags_vst_unit_new(AgsVstString128 unit_name, AgsVstUnitID unit_id, gint32 parent_unit_id,
			       gint32 program_list_id)
  {
    return((AgsVstUnit *) new Steinberg::Vst::Unit((char16_t *) unit_name, unit_id, parent_unit_id,
						   program_list_id));
  }

  /**
   * Delete.
   *
   * @param unit the unit
   *
   * @since 5.0.0
   */
  void ags_vst_unit_delete(AgsVstUnit *unit)
  {
    delete unit;
  }

  /**
   * Get info.
   *
   * @param unit the unit
   * @return the unit info
   *
   * @since 5.0.0
   */
  AgsVstUnitInfo* ags_vst_unit_get_info(AgsVstUnit *unit)
  {
    const Steinberg::Vst::UnitInfo& unit_info = static_cast<Steinberg::Vst::Unit*>((void *) unit)->getInfo();
    
    return((AgsVstUnitInfo *) const_cast<Steinberg::Vst::UnitInfo*>(&unit_info));
  }

  /**
   * Get identifier.
   *
   * @param unit the unit
   * @return the identifier
   *
   * @since 5.0.0
   */
  gint32 ags_vst_unit_get_id(AgsVstUnit *unit)
  {
    return(static_cast<Steinberg::Vst::Unit*>((void *) unit)->getID());
  }

  /**
   * Set identifier.
   *
   * @param unit the unit
   * @param the new identifier
   *
   * @since 5.0.0
   */
  void ags_vst_unit_set_id(AgsVstUnit *unit,
			   gint32 new_id)
  {
    return(static_cast<Steinberg::Vst::Unit*>((void *) unit)->setID(new_id));
  }

  /**
   * Get name.
   *
   * @param unit the unit
   * @return the unicode string
   *
   * @since 5.0.0
   */
  gunichar2* ags_vst_unit_get_name(AgsVstUnit *unit)
  {
    return((gunichar2 *) static_cast<Steinberg::Vst::Unit*>((void *) unit)->getName());
  }

  /**
   * Set name.
   *
   * @param unit the unit
   * @return the new unicode string
   *
   * @since 5.0.0
   */
  void ags_vst_unit_set_name(AgsVstUnit *unit,
			     AgsVstString128 new_name)
  {
    return(static_cast<Steinberg::Vst::Unit*>((void *) unit)->setName((char16_t *) new_name));
  }

  /**
   * Get program list identifier.
   *
   * @param unit the unit
   * @return the program list identifier
   *
   * @since 5.0.0
   */
  gint32 ags_vst_unit_get_program_list_id(AgsVstUnit *unit)
  {
    return(static_cast<Steinberg::Vst::Unit*>((void *) unit)->getProgramListID());
  }

  /**
   * Set program list identifier.
   *
   * @param unit the unit
   * @param the new identifier
   *
   * @since 5.0.0
   */
  void ags_vst_unit_set_program_list_id(AgsVstUnit *unit,
					gint32 new_id)
  {
    return(static_cast<Steinberg::Vst::Unit*>((void *) unit)->setProgramListID(new_id));
  }

  /**
   * Instantiate new Steinberg::Vst::Unit the constructor as a C99
   * compatible handle a void pointer.
   *
   * @param name the name
   * @param list_id the list identifier
   * @param unit_id the unit identifier
   * @return the AgsVstProgramList
   * 
   * @since 5.0.0
   */
  AgsVstProgramList* ags_vst_program_list_new(AgsVstString128 name, gint32 list_id, AgsVstUnitID unit_id)
  {
    return((AgsVstProgramList *) new Steinberg::Vst::ProgramList((char16_t *) name, list_id, unit_id));
  }

  /**
   * Instantiate new Steinberg::Vst::Unit the constructor as a C99
   * compatible handle a void pointer.
   *
   * @param program_list the program list
   * @return the AgsVstProgramList
   * 
   * @since 5.0.0
   */
  AgsVstProgramList* ags_vst_program_list_new_from_program_list(AgsVstProgramList *program_list)
  {
    Steinberg::Vst::ProgramList* tmp_program_list_0 = (Steinberg::Vst::ProgramList *) program_list;
    const Steinberg::Vst::ProgramList& tmp_program_list_1 = const_cast<Steinberg::Vst::ProgramList&>(tmp_program_list_0[0]);
    
    return((AgsVstProgramList *) new Steinberg::Vst::ProgramList(tmp_program_list_1));
  }

  /**
   * Delete.
   *
   * @param program_list the program list
   *
   * @since 5.0.0
   */
  void ags_vst_program_list_delete(AgsVstProgramList *program_list)
  {
    delete program_list;
  }
  
  /**
   * Get info.
   *
   * @param program_list the program list
   * @return the AgsVstProgramListInfo
   * 
   * @since 5.0.0
   */
  AgsVstProgramListInfo* ags_vst_program_list_get_info(AgsVstProgramList *program_list)
  {
    const Steinberg::Vst::ProgramListInfo& tmp_program_list_0 = static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->getInfo();
    Steinberg::Vst::ProgramListInfo *tmp_program_list_1 = const_cast<Steinberg::Vst::ProgramListInfo*>(&tmp_program_list_0);
    
    return((AgsVstProgramListInfo *) (tmp_program_list_1));
  }

  /**
   * Get identifier.
   *
   * @param program_list the program list
   * @return the identifier
   * 
   * @since 5.0.0
   */
  gint32 ags_vst_program_list_get_id(AgsVstProgramList *program_list)
  {
    return(static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->getID());
  }

  /**
   * Get name.
   *
   * @param program_list the program list
   * @return the name
   * 
   * @since 5.0.0
   */
  gunichar2* ags_vst_program_list_get_name(AgsVstProgramList *program_list)
  {
    return((gunichar2 *) static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->getName());
  }

  /**
   * Get count.
   *
   * @param program_list the program list
   * @return the count
   * 
   * @since 5.0.0
   */
  gint32 ags_vst_program_list_get_count(AgsVstProgramList *program_list)
  {
    return(static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->getCount());
  }

  /**
   * Get program name.
   *
   * @param program_list the program list
   * @param program_index the program index
   * @param out_name the output name
   * @return the result code
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_program_list_get_program_name(AgsVstProgramList *program_list,
						      gint32 program_index, AgsVstString128 out_name)
  {
    return(static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->getProgramName(program_index, (char16_t *) out_name));
  }

  /**
   * Set program name.
   *
   * @param program_list the program list
   * @param program_index the program index
   * @param in_name the input name
   * @return the result code
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_program_list_set_program_name(AgsVstProgramList *program_list,
						      gint32 program_index, AgsVstString128 in_name)
  {
    return(static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->setProgramName(program_index, (char16_t *) in_name));
  }

  /**
   * Get program info.
   *
   * @param program_list the program list
   * @param program_index the program index
   * @param out_value the output value
   * @return the result code
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_program_list_get_program_info(AgsVstProgramList *program_list,
						      gint32 program_index, gchar *attribute_id,
						      AgsVstString128 out_value)
  {
    return(static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->getProgramInfo(program_index, attribute_id,
											    (char16_t *) out_value));
  }

  /**
   * Has pitch names.
   *
   * @param program_list the program list
   * @param program_index the program index
   * @return the result code
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_program_list_has_pitch_names(AgsVstProgramList *program_list,
						     gint32 program_index)
  {
    return(static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->hasPitchNames(program_index));
  }

  /**
   * Get pitch name.
   *
   * @param program_list the program list
   * @param program_index the program index
   * @param midi_pitch the MIDI pitch
   * @param out_name the output name
   * @return the result code
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_program_list_get_pitch_name(AgsVstProgramList *program_list,
						    gint32 program_index, gint16 midi_pitch, AgsVstString128 out_name)
  {
    return(static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->getPitchName(program_index, midi_pitch, (char16_t *) out_name));
  }

  /**
   * Add program.
   *
   * @param program_list the program list
   * @param name the name
   * @return the result code
   * 
   * @since 5.0.0
   */
  gint32 ags_vst_program_list_add_program(AgsVstProgramList *program_list,
					  AgsVstString128 name)
  {
    return(static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->addProgram((char16_t *) name));
  }

  /**
   * Set program info.
   *
   * @param program_list the program list
   * @param program_index the program index
   * @param attribute_id the attribute identifier
   * @param value the value
   * @return true on success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_program_list_set_program_info(AgsVstProgramList *program_list,
						 gint32 program_index, gchar *attribute_id, AgsVstString128 value)
  {
    return(static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->setProgramInfo(program_index, attribute_id, (char16_t *) value));
  }

  /**
   * Get parameter.
   *
   * @param program_list the program list
   * @return the parameter
   * 
   * @since 5.0.0
   */
  AgsVstParameter* ags_vst_program_list_get_parameter(AgsVstProgramList *program_list)
  {
    return((AgsVstParameter *) static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->getParameter());
  }
  
  /**
   * Instantiate new Steinberg::Vst::ProgramListWithPitchNames the constructor as a C99
   * compatible handle a void pointer.
   *
   * @param name the name
   * @param list_id the list identifier
   * @param unit_id the unit identifier
   * @return the AgsVstProgramListWithPitchNames
   * 
   * @since 5.0.0
   */
  AgsVstProgramListWithPitchNames* ags_vst_program_list_with_pitch_names_new(AgsVstString128 name, AgsVstProgramListID list_id, AgsVstUnitID unit_id)
  {
    return((AgsVstProgramListWithPitchNames *) new Steinberg::Vst::ProgramListWithPitchNames((char16_t *) name, list_id, unit_id));
  }

  /**
   * Delete.
   *
   * @param program_list_with_pitch_names the program list with pitch names
   *
   * @since 5.0.0
   */
  void ags_vst_program_list_with_pitch_names_delete(AgsVstProgramListWithPitchNames *program_list_with_pitch_names)
  {
    delete program_list_with_pitch_names;
  }

  /**
   * Set pitch name.
   *
   * @param program_list_with_pitch_names the program list with pitch names
   * @param program_index the program index
   * @param pitch the pitch
   * @param pitch_name the pitch name
   * @return true if success, otherwise false
   *
   * @since 5.0.0
   */
  gboolean ags_vst_program_list_with_pitch_names_set_pitch_name(AgsVstProgramListWithPitchNames *program_list_with_pitch_names,
								gint32 program_index, gint16 pitch, AgsVstString128 pitch_name)
  {
    return(static_cast<Steinberg::Vst::ProgramListWithPitchNames*>((void *) program_list_with_pitch_names)->setPitchName(program_index, pitch, (char16_t *) pitch_name));
  }

  /**
   * Remove pitch name.
   *
   * @param program_list_with_pitch_names the program list with pitch names
   * @param program_index the program index
   * @param pitch the pitch
   * @return true if success, otherwise false
   *
   * @since 5.0.0
   */
  gboolean ags_vst_program_list_with_pitch_names_remove_pitch_name(AgsVstProgramListWithPitchNames *program_list_with_pitch_names,
								   gint32 program_index, gint16 pitch)
  {
    return(static_cast<Steinberg::Vst::ProgramListWithPitchNames*>((void *) program_list_with_pitch_names)->removePitchName(program_index, pitch));
  }

  /**
   * Add program.
   *
   * @param program_list_with_pitch_names the program list with pitch names
   * @param name the name
   * @return the result code
   *
   * @since 5.0.0
   */
  gint32 ags_vst_program_list_with_pitch_names_add_program(AgsVstProgramListWithPitchNames *program_list_with_pitch_names,
							   AgsVstString128 name)
  {
    return(static_cast<Steinberg::Vst::ProgramListWithPitchNames*>((void *) program_list_with_pitch_names)->addProgram((char16_t *) name));
  }

  /**
   * Has pitch names.
   *
   * @param program_list_with_pitch_names the program list with pitch names
   * @param program_index the program index
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_program_list_with_pitch_names_has_pitch_names(AgsVstProgramListWithPitchNames *program_list_with_pitch_names,
								      gint32 program_index)
  {
    return(static_cast<Steinberg::Vst::ProgramListWithPitchNames*>((void *) program_list_with_pitch_names)->hasPitchNames(program_index));
  }

  /**
   * Has pitch names.
   *
   * @param program_list_with_pitch_names the program list with pitch names
   * @param program_index the program index
   * @param midi_pitch the MIDI pitch
   * @param name the name
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_program_list_with_pitch_names_get_pitch_name(AgsVstProgramListWithPitchNames *program_list_with_pitch_names,
								     gint32 program_index, gint16 midi_pitch,
								     AgsVstString128 name)
  {
    return(static_cast<Steinberg::Vst::ProgramListWithPitchNames*>((void *) program_list_with_pitch_names)->getPitchName(program_index, midi_pitch,
						       (char16_t *) name));
  }

  /**
   * Instantiate new Steinberg::Vst::EditControllerEx1 the constructor as a C99
   * compatible handle a void pointer.
   *
   * @return the AgsVstEditControllerEx1
   * 
   * @since 5.0.0
   */
  AgsVstEditControllerEx1* ags_vst_edit_controller_ex1_new()
  {
    return((AgsVstEditControllerEx1 *) new Steinberg::Vst::EditControllerEx1());
  }

  /**
   * Delete.
   *
   * @param edit_controller_ex1 the edit controller ex1
   *
   * @since 5.0.0
   */
  void ags_vst_edit_controller_ex1_delete(AgsVstEditControllerEx1 *edit_controller_ex1)
  {
    delete edit_controller_ex1;
  }
  
  /**
   * Add unit.
   *
   * @param edit_controller_ex1 the edit controller ex1
   * @param unit the unit
   * @return true if success, otherwise false
   *
   * @since 5.0.0
   */
  gboolean ags_vst_edit_controller_ex1_add_unit(AgsVstEditControllerEx1 *edit_controller_ex1,
						AgsVstUnit *unit)
  {
    return((gboolean) static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->addUnit(static_cast<Steinberg::Vst::Unit*>((void *) unit)));
  }

  /**
   * Add program list.
   *
   * @param edit_controller_ex1 the edit controller ex1
   * @param list the program list
   * @return true if success, otherwise false
   *
   * @since 5.0.0
   */
  gboolean ags_vst_edit_controller_ex1_add_program_list(AgsVstEditControllerEx1 *edit_controller_ex1,
							AgsVstProgramList *list)
  {
    return((gboolean) static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->addProgramList(static_cast<Steinberg::Vst::ProgramList*>((void *) list)));
  }

  /**
   * Get program list.
   *
   * @param edit_controller_ex1 the edit controller ex1
   * @param list_id the list identifier
   * @return the program list
   *
   * @since 5.0.0
   */
  AgsVstProgramList* ags_vst_edit_controller_ex1_get_program_list(AgsVstEditControllerEx1 *edit_controller_ex1,
								  gint32 list_id)
  {
    return((AgsVstProgramList *) static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->getProgramList(list_id));
  }

  /**
   * Notify program list change.
   *
   * @param edit_controller_ex1 the edit controller ex1
   * @param list_id the list identifier
   * @param program_index the program index
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_ex1_notify_program_list_change(AgsVstEditControllerEx1 *edit_controller_ex1,
								       gint32 list_id, gint32 program_index)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->notifyProgramListChange(list_id, program_index));
  }

  /**
   * Get unit count.
   *
   * @param edit_controller_ex1 the edit controller ex1
   * @return the unit count
   *
   * @since 5.0.0
   */
  gint32 ags_vst_edit_controller_ex1_get_unit_count(AgsVstEditControllerEx1 *edit_controller_ex1)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->getUnitCount());
  }

  /**
   * Get unit info.
   *
   * @param edit_controller_ex1 the edit controller ex1
   * @param unit_index the unit index
   * @param unit_info the unit info
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_ex1_get_unit_info(AgsVstEditControllerEx1 *edit_controller_ex1,
							  gint32 unit_index, AgsVstUnitInfo *info)
  {
    Steinberg::Vst::UnitInfo *tmp_unit_info_0 = static_cast<Steinberg::Vst::UnitInfo*>((void *) info);
    const Steinberg::Vst::UnitInfo& tmp_unit_info_1 = const_cast<Steinberg::Vst::UnitInfo&>(tmp_unit_info_0[0]);
    
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->getUnitInfo(unit_index, const_cast<Steinberg::Vst::UnitInfo&>(tmp_unit_info_1)));
  }

  /**
   * Get program list count.
   *
   * @param edit_controller_ex1 the edit controller ex1
   * @return the program list count
   *
   * @since 5.0.0
   */
  gint32 ags_vst_edit_controller_ex1_get_program_list_count(AgsVstEditControllerEx1 *edit_controller_ex1)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->getProgramListCount());
  }

  /**
   * Get program list info.
   *
   * @param edit_controller_ex1 the edit controller ex1
   * @param list_index the list index
   * @param unit_info the unit info
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_ex1_get_program_list_info(AgsVstEditControllerEx1 *edit_controller_ex1,
								  gint32 list_index,
								  AgsVstProgramListInfo *info)
  {
    Steinberg::Vst::ProgramListInfo *tmp_info_0 = static_cast<Steinberg::Vst::ProgramListInfo*>((void *) info);
    const Steinberg::Vst::ProgramListInfo& tmp_info_1 = const_cast<Steinberg::Vst::ProgramListInfo&>(tmp_info_0[0]);

    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->getProgramListInfo(list_index, const_cast<Steinberg::Vst::ProgramListInfo&>(tmp_info_1)));
  }

  /**
   * Get program name.
   *
   * @param edit_controller_ex1 the edit controller ex1
   * @param list_id the list identifier
   * @param program_index the program index
   * @param name the name
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_ex1_get_program_name(AgsVstEditControllerEx1 *edit_controller_ex1,
							     gint32 list_id, gint32 program_index,
							     AgsVstString128 name)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->getProgramName(list_id, program_index,
					       (char16_t *) name));
  }

  /**
   * Get program info.
   *
   * @param edit_controller_ex1 the edit controller ex1
   * @param list_id the list identifier
   * @param program_index the program index
   * @param attribute_id the attribute identifier
   * @param attribute_value the attribute value
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_ex1_get_program_info(AgsVstEditControllerEx1 *edit_controller_ex1,
							     gint32 list_id, gint32 program_index,
							     gchar *attribute_id,
							     AgsVstString128 attribute_value)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->getProgramInfo(list_id, program_index,
													 attribute_id,
													 (char16_t *) attribute_value));
  }

  /**
   * Has program pitch names.
   *
   * @param edit_controller_ex1 the edit controller ex1
   * @param list_id the list identifier
   * @param program_index the program index
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_ex1_has_program_pitch_names(AgsVstEditControllerEx1 *edit_controller_ex1,
								    gint32 list_id,
								    gint32 program_index)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->hasProgramPitchNames(list_id,
						     program_index));
  }

  /**
   * Get program pitch name.
   *
   * @param edit_controller_ex1 the edit controller ex1
   * @param list_id the list identifier
   * @param program_index the program index
   * @param midi_pitch the MIDI pitch
   * @param name the nmae
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_ex1_get_program_pitch_name(AgsVstEditControllerEx1 *edit_controller_ex1,
								   gint32 list_id, gint32 program_index,
								   gint16 midi_pitch, AgsVstString128 name)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->getProgramPitchName(list_id, program_index,
													      midi_pitch, (char16_t *) name));
  }

  /**
   * Set program name.
   *
   * @param edit_controller_ex1 the edit controller ex1
   * @param list_id the list identifier
   * @param program_index the program index
   * @param name the nmae
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_ex1_set_program_name(AgsVstEditControllerEx1 *edit_controller_ex1,
							     gint32 list_id, gint32 program_index,
							     AgsVstString128 name)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->setProgramName(list_id, program_index,
					       (char16_t *) name));
  }

  /**
   * Get selected unit.
   *
   * @param edit_controller_ex1 the edit controller ex1
   * @return the selected unit
   *
   * @since 5.0.0
   */
  gint32 ags_vst_edit_controller_ex1_get_selected_unit(AgsVstEditControllerEx1 *edit_controller_ex1)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->getSelectedUnit());
  }

  /**
   * Selected unit.
   *
   * @param edit_controller_ex1 the edit controller ex1
   * @param unit_id the unit identifier
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_ex1_select_unit(AgsVstEditControllerEx1 *edit_controller_ex1,
							AgsVstUnitID unit_id)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->selectUnit(unit_id));
  }

  /**
   * Get unit by bus.
   *
   * @param edit_controller_ex1 the edit controller ex1
   * @param type the type
   * @param dir the direction
   * @param bus_index the bus index
   * @param channel the channel
   * @param unit_id the unit identifier
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_ex1_get_unit_by_bus(AgsVstEditControllerEx1 *edit_controller_ex1,
							    guint type, guint dir, gint32 bus_index,
							    gint32 channel, AgsVstUnitID unit_id)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->getUnitByBus(type, dir, bus_index,
												       channel, unit_id));
  }

  /**
   * Set unit program data.
   *
   * @param edit_controller_ex1 the edit controller ex1
   * @param list_or_unit_id the list or unit identifier
   * @param program_index the program index
   * @param data the data
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_ex1_set_unit_program_data(AgsVstEditControllerEx1 *edit_controller_ex1,
								  gint32 list_or_unit_id, gint32 program_index,
								  AgsVstIBStream *data)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->setUnitProgramData(list_or_unit_id, program_index,
													     static_cast<Steinberg::IBStream*>((void *) data)));
  }
  
  /**
   * Notify unit selection.
   *
   * @param edit_controller_ex1 the edit controller ex1
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_edit_controller_ex1_notify_unit_selection(AgsVstEditControllerEx1 *edit_controller_ex1)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->notifyUnitSelection());
  }

  /**
   * Update.
   *
   * @param edit_controller_ex1 the edit controller ex1
   * @param changed_unknown the changed unknown
   * @param message the message
   *
   * @since 5.0.0
   */
  void ags_vst_edit_controller_ex1_update(AgsVstEditControllerEx1 *edit_controller_ex1, AgsVstFUnknown *changed_unknown, gint32 message)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->update(static_cast<Steinberg::FUnknown*>((void *) changed_unknown), message));
  }
  
}
