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

#include <ags/vst3-capi/public.sdk/source/vst/ags_vst_edit_controller.h>

#include <public.sdk/source/vst/vsteditcontroller.h>

#include <string.h>

extern "C" {

  AgsVstEditController* ags_vst_edit_controller_new()
  {
    return((AgsVstEditController *) new Steinberg::Vst::EditController());
  }
  
  void ags_vst_edit_controller_delete(AgsVstEditController *edit_controller)
  {
    delete edit_controller;
  }

  AgsVstTResult ags_vst_edit_controller_set_component_state(AgsVstEditController *edit_controller, AgsVstIBStream *state)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->setComponentState(static_cast<Steinberg::IBStream*>((void *) state)));
  }

  AgsVstTResult ags_vst_edit_controller_set_state(AgsVstEditController *edit_controller, AgsVstIBStream *state)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->setState(static_cast<Steinberg::IBStream*>((void *) state)));
  }

  AgsVstTResult ags_vst_edit_controller_get_state(AgsVstEditController *edit_controller, AgsVstIBStream *state)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->getState(static_cast<Steinberg::IBStream*>((void *) state)));
  }

  gint32 ags_vst_edit_controller_get_parameter_count(AgsVstEditController *edit_controller)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->getParameterCount());
  }

  AgsVstTResult ags_vst_edit_controller_get_parameter_info(AgsVstEditController *edit_controller,
							   gint32 param_index, AgsVstParameterInfo *info)
  {
    Steinberg::Vst::ParameterInfo *tmp_info_0 = static_cast<Steinberg::Vst::ParameterInfo*>((void *) info);
    const Steinberg::Vst::ParameterInfo& tmp_info_1 = const_cast<Steinberg::Vst::ParameterInfo&>(tmp_info_0[0]);
    
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->getParameterInfo(param_index, const_cast<Steinberg::Vst::ParameterInfo&>(tmp_info_1)));
  }

  AgsVstTResult ags_vst_edit_controller_get_param_string_by_value(AgsVstEditController *edit_controller,
								  guint32 tag, gdouble value_normalized, AgsVstString128 str)
  {
    Steinberg::Vst::String128 tmp_str_0;
    Steinberg::tresult retval;

    retval = static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->getParamStringByValue(tag, value_normalized, tmp_str_0);

    memcpy(str, tmp_str_0, 128 * sizeof(char16_t));
    
    return(retval);
  }

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

  gdouble ags_vst_edit_controller_normalized_param_to_plain(AgsVstEditController *edit_controller,
							    guint32 tag, gdouble value_normalized)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->normalizedParamToPlain(tag, value_normalized));
  }

  gdouble ags_vst_edit_controller_plain_param_to_normalized(AgsVstEditController *edit_controller,
							    guint32 tag, gdouble plain_value)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->plainParamToNormalized(tag, plain_value));
  }

  gdouble ags_vst_edit_controller_get_param_normalized(AgsVstEditController *edit_controller,
						       guint32 tag)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->getParamNormalized(tag));
  }

  AgsVstTResult ags_vst_edit_controller_set_param_normalized(AgsVstEditController *edit_controller,
							     guint32 tag, gdouble value)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->setParamNormalized(tag, value));
  }

  AgsVstTResult ags_vst_edit_controller_set_component_handler(AgsVstEditController *edit_controller,
							      AgsVstIComponentHandler *handler)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->setComponentHandler(static_cast<Steinberg::Vst::IComponentHandler*>((void *) handler)));
  }

  AgsVstIPlugView* ags_vst_edit_controller_create_view(AgsVstEditController *edit_controller,
						       AgsVstFIDString view_name)
  {
    return((AgsVstIPlugView *) static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->createView(view_name));
  }

  AgsVstTResult ags_vst_edit_controller_set_knob_mode(AgsVstEditController *edit_controller,
						      guint mode)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->setKnobMode(mode));
  }

  AgsVstTResult ags_vst_edit_controller_open_help(AgsVstEditController *edit_controller,
						  gboolean only_check)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->openHelp(only_check));
  }

  AgsVstTResult ags_vst_edit_controller_open_about_box(AgsVstEditController *edit_controller,
						       gboolean only_check)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->openAboutBox(only_check));
  }

  AgsVstTResult ags_vst_edit_controller_initialize(AgsVstEditController *edit_controller,
						   AgsVstFUnknown *context)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->initialize(static_cast<Steinberg::FUnknown*>((void *) context)));
  }

  AgsVstTResult ags_vst_edit_controller_terminate(AgsVstEditController *edit_controller)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->terminate());
  }

  AgsVstTResult ags_vst_edit_controller_begin_edit(AgsVstEditController *edit_controller,
						   guint32 tag)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->beginEdit(tag));
  }

  AgsVstTResult ags_vst_edit_controller_perform_edit(AgsVstEditController *edit_controller,
						     guint32 tag, gdouble value_normalized)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->performEdit(tag, value_normalized));
  }

  AgsVstTResult ags_vst_edit_controller_end_edit(AgsVstEditController *edit_controller,
						 guint32 tag)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->endEdit(tag));
  }

  AgsVstTResult ags_vst_edit_controller_start_group_edit(AgsVstEditController *edit_controller)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->startGroupEdit());
  }

  AgsVstTResult ags_vst_edit_controller_finish_group_edit(AgsVstEditController *edit_controller)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->finishGroupEdit());
  }

  void ags_vst_edit_controller_editor_destroyed(AgsVstEditController *edit_controller,
						AgsVstEditorView *editor)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->editorDestroyed(static_cast<Steinberg::Vst::EditorView*>((void *) editor)));
  }

  void ags_vst_edit_controller_editor_attached(AgsVstEditController *edit_controller,
					       AgsVstEditorView *editor)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->editorAttached(static_cast<Steinberg::Vst::EditorView*>((void *) editor)));
  }

  void ags_vst_edit_controller_editor_removed(AgsVstEditController *edit_controller,
					      AgsVstEditorView *editor)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->editorRemoved(static_cast<Steinberg::Vst::EditorView*>((void *) editor)));
  }

  guint ags_vst_edit_controller_get_host_knob_mode(AgsVstEditController *edit_controller)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->getHostKnobMode());
  }

  AgsVstParameter* ags_vst_edit_controller_get_parameter_object(AgsVstEditController *edit_controller,
								guint32 tag)
  {
    return((AgsVstParameter *) static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->getParameterObject(tag));
  }

  AgsVstTResult ags_vst_edit_controller_get_parameter_info_by_tag(AgsVstEditController *edit_controller,
								  guint32 tag, AgsVstParameterInfo *info)
  {
    Steinberg::Vst::ParameterInfo *tmp_info_0 = (Steinberg::Vst::ParameterInfo *) info;
    const Steinberg::Vst::ParameterInfo& tmp_info_1 = const_cast<Steinberg::Vst::ParameterInfo&>(tmp_info_0[0]);

    Steinberg::tresult retval;

    retval = static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->getParameterInfoByTag(tag, const_cast<Steinberg::Vst::ParameterInfo&>(tmp_info_1));
    
    return(retval);
  }

  AgsVstTResult ags_vst_edit_controller_set_dirty(AgsVstEditController *edit_controller,
						  gboolean state)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->setDirty(state));
  }

  AgsVstTResult ags_vst_edit_controller_request_open_editor(AgsVstEditController *edit_controller,
							    AgsVstFIDString editor_name)
  {
    return(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->requestOpenEditor(editor_name));
  }

  AgsVstIComponentHandler* ags_vst_edit_controller_get_component_handler(AgsVstEditController *edit_controller)
  {
    return((AgsVstIComponentHandler *) static_cast<Steinberg::Vst::EditController*>((void *) edit_controller)->getComponentHandler());
  }
    
  AgsVstEditorView* ags_vst_editor_view_new(AgsVstEditController *edit_controller, AgsVstViewRect *size)
  {
    return((AgsVstEditorView *) new Steinberg::Vst::EditorView(static_cast<Steinberg::Vst::EditController*>((void *) edit_controller), (struct Steinberg::ViewRect *) size));
  }

  void ags_vst_editor_view_delete(AgsVstEditorView *editor_view)
  {
    delete editor_view;
  }

  AgsVstEditController* ags_vst_editor_view_get_controller(AgsVstEditorView *editor_view)
  {
    return((AgsVstEditController *) static_cast<Steinberg::Vst::EditorView*>((void *) editor_view)->getController());
  }
  
  void ags_vst_editor_view_attached_to_parent(AgsVstEditorView *editor_view)
  {
    return(static_cast<Steinberg::Vst::EditorView*>((void *) editor_view)->attachedToParent());
  }

  void ags_vst_editor_view_removed_from_parent(AgsVstEditorView *editor_view)
  {
    return(static_cast<Steinberg::Vst::EditorView*>((void *) editor_view)->removedFromParent());
  }
  
  AgsVstUnit* ags_vst_unit_new(AgsVstString128 unit_name, AgsVstUnitID unit_id, gint32 parent_unit_id,
			       gint32 program_list_id)
  {
    return((AgsVstUnit *) new Steinberg::Vst::Unit((char16_t *) unit_name, unit_id, parent_unit_id,
						   program_list_id));
  }

  void ags_vst_unit_delete(AgsVstUnit *unit)
  {
    delete unit;
  }

  AgsVstUnitInfo* ags_vst_unit_get_info(AgsVstUnit *unit)
  {
    const Steinberg::Vst::UnitInfo& unit_info = static_cast<Steinberg::Vst::Unit*>((void *) unit)->getInfo();
    
    return((AgsVstUnitInfo *) const_cast<Steinberg::Vst::UnitInfo*>(&unit_info));
  }

  gint32 ags_vst_unit_get_id(AgsVstUnit *unit)
  {
    return(static_cast<Steinberg::Vst::Unit*>((void *) unit)->getID());
  }

  void ags_vst_unit_set_id(AgsVstUnit *unit,
			   gint32 new_id)
  {
    return(static_cast<Steinberg::Vst::Unit*>((void *) unit)->setID(new_id));
  }

  gunichar2* ags_vst_unit_get_name(AgsVstUnit *unit)
  {
    return((gunichar2 *) static_cast<Steinberg::Vst::Unit*>((void *) unit)->getName());
  }

  void ags_vst_unit_set_name(AgsVstUnit *unit,
			     AgsVstString128 new_name)
  {
    return(static_cast<Steinberg::Vst::Unit*>((void *) unit)->setName((char16_t *) new_name));
  }

  gint32 ags_vst_unit_get_program_list_id(AgsVstUnit *unit)
  {
    return(static_cast<Steinberg::Vst::Unit*>((void *) unit)->getProgramListID());
  }

  void ags_vst_unit_set_program_list_id(AgsVstUnit *unit,
					gint32 new_id)
  {
    return(static_cast<Steinberg::Vst::Unit*>((void *) unit)->setProgramListID(new_id));
  }

  AgsVstProgramList* ags_vst_program_list_new(AgsVstString128 name, gint32 list_id, AgsVstUnitID unit_id)
  {
    return((AgsVstProgramList *) new Steinberg::Vst::ProgramList((char16_t *) name, list_id, unit_id));
  }

  AgsVstProgramList* ags_vst_program_list_new_from_program_list(AgsVstProgramList *program_list)
  {
    Steinberg::Vst::ProgramList* tmp_program_list_0 = (Steinberg::Vst::ProgramList *) program_list;
    const Steinberg::Vst::ProgramList& tmp_program_list_1 = const_cast<Steinberg::Vst::ProgramList&>(tmp_program_list_0[0]);
    
    return((AgsVstProgramList *) new Steinberg::Vst::ProgramList(tmp_program_list_1));
  }

  void ags_vst_program_list_delete(AgsVstProgramList *program_list)
  {
    delete program_list;
  }
  
  AgsVstProgramListInfo* ags_vst_program_list_get_info(AgsVstProgramList *program_list)
  {
    const Steinberg::Vst::ProgramListInfo& tmp_program_list_0 = static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->getInfo();
    Steinberg::Vst::ProgramListInfo *tmp_program_list_1 = const_cast<Steinberg::Vst::ProgramListInfo*>(&tmp_program_list_0);
    
    return((AgsVstProgramListInfo *) (tmp_program_list_1));
  }

  gint32 ags_vst_program_list_get_id(AgsVstProgramList *program_list)
  {
    return(static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->getID());
  }

  gunichar2* ags_vst_program_list_get_name(AgsVstProgramList *program_list)
  {
    return((gunichar2 *) static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->getName());
  }

  gint32 ags_vst_program_list_get_count(AgsVstProgramList *program_list)
  {
    return(static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->getCount());
  }

  AgsVstTResult ags_vst_program_list_get_program_name(AgsVstProgramList *program_list,
						      gint32 program_index, AgsVstString128 out_name)
  {
    return(static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->getProgramName(program_index, (char16_t *) out_name));
  }

  AgsVstTResult ags_vst_program_list_set_program_name(AgsVstProgramList *program_list,
						      gint32 program_index, AgsVstString128 in_name)
  {
    return(static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->setProgramName(program_index, (char16_t *) in_name));
  }

  AgsVstTResult ags_vst_program_list_get_program_info(AgsVstProgramList *program_list,
						      gint32 program_index, gchar *attribute_id,
						      AgsVstString128 out_value)
  {
    return(static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->getProgramInfo(program_index, attribute_id,
											    (char16_t *) out_value));
  }

  AgsVstTResult ags_vst_program_list_has_pitch_names(AgsVstProgramList *program_list,
						     gint32 program_index)
  {
    return(static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->hasPitchNames(program_index));
  }

  AgsVstTResult ags_vst_program_list_get_pitch_name(AgsVstProgramList *program_list,
						    gint32 program_index, gint16 midi_pitch, AgsVstString128 out_name)
  {
    return(static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->getPitchName(program_index, midi_pitch, (char16_t *) out_name));
  }

  gint32 ags_vst_program_list_add_program(AgsVstProgramList *program_list,
					  AgsVstString128 name)
  {
    return(static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->addProgram((char16_t *) name));
  }

  gboolean ags_vst_program_list_set_program_info(AgsVstProgramList *program_list,
						 gint32 program_index, gchar *attribute_id, AgsVstString128 value)
  {
    return(static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->setProgramInfo(program_index, attribute_id, (char16_t *) value));
  }

  AgsVstParameter* ags_vst_program_list_get_parameter(AgsVstProgramList *program_list)
  {
    return((AgsVstParameter *) static_cast<Steinberg::Vst::ProgramList*>((void *) program_list)->getParameter());
  }
  
  AgsVstProgramListWithPitchNames* ags_vst_program_list_with_pitch_names_new(AgsVstString128 name, AgsVstProgramListID list_id, AgsVstUnitID unit_id)
  {
    return((AgsVstProgramListWithPitchNames *) new Steinberg::Vst::ProgramListWithPitchNames((char16_t *) name, list_id, unit_id));
  }

  void ags_vst_program_list_with_pitch_names_delete(AgsVstProgramListWithPitchNames *program_list_with_pitch_names)
  {
    delete program_list_with_pitch_names;
  }

  gboolean ags_vst_program_list_with_pitch_names_set_pitch_name(AgsVstProgramListWithPitchNames *program_list_with_pitch_names,
								gint32 program_index, gint16 pitch, AgsVstString128 pitch_name)
  {
    return(static_cast<Steinberg::Vst::ProgramListWithPitchNames*>((void *) program_list_with_pitch_names)->setPitchName(program_index, pitch, (char16_t *) pitch_name));
  }

  gboolean ags_vst_program_list_with_pitch_names_remove_pitch_name(AgsVstProgramListWithPitchNames *program_list_with_pitch_names,
								   gint32 program_index, gint16 pitch)
  {
    return(static_cast<Steinberg::Vst::ProgramListWithPitchNames*>((void *) program_list_with_pitch_names)->removePitchName(program_index, pitch));
  }

  gint32 ags_vst_program_list_with_pitch_names_add_program(AgsVstProgramListWithPitchNames *program_list_with_pitch_names,
							   AgsVstString128 name)
  {
    return(static_cast<Steinberg::Vst::ProgramListWithPitchNames*>((void *) program_list_with_pitch_names)->addProgram((char16_t *) name));
  }

  AgsVstTResult ags_vst_program_list_with_pitch_names_has_pitch_names(AgsVstProgramListWithPitchNames *program_list_with_pitch_names,
								      gint32 program_index)
  {
    return(static_cast<Steinberg::Vst::ProgramListWithPitchNames*>((void *) program_list_with_pitch_names)->hasPitchNames(program_index));
  }

  AgsVstTResult ags_vst_program_list_with_pitch_names_get_pitch_name(AgsVstProgramListWithPitchNames *program_list_with_pitch_names,
								     gint32 program_index, gint16 midi_pitch,
								     AgsVstString128 name)
  {
    return(static_cast<Steinberg::Vst::ProgramListWithPitchNames*>((void *) program_list_with_pitch_names)->getPitchName(program_index, midi_pitch,
						       (char16_t *) name));
  }

  AgsVstEditControllerEx1* ags_vst_edit_controller_ex1_new()
  {
    return((AgsVstEditControllerEx1 *) new Steinberg::Vst::EditControllerEx1());
  }

  void ags_vst_edit_controller_ex1_delete(AgsVstEditControllerEx1 *edit_controller_ex1)
  {
    delete edit_controller_ex1;
  }
  
  gboolean ags_vst_edit_controller_ex1_add_unit(AgsVstEditControllerEx1 *edit_controller_ex1,
						AgsVstUnit *unit)
  {
    return((gboolean) static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->addUnit(static_cast<Steinberg::Vst::Unit*>((void *) unit)));
  }

  gboolean ags_vst_edit_controller_ex1_add_program_list(AgsVstEditControllerEx1 *edit_controller_ex1,
							AgsVstProgramList *list)
  {
    return((gboolean) static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->addProgramList(static_cast<Steinberg::Vst::ProgramList*>((void *) list)));
  }

  AgsVstProgramList* ags_vst_edit_controller_ex1_get_program_list(AgsVstEditControllerEx1 *edit_controller_ex1,
								  gint32 list_id)
  {
    return((AgsVstProgramList *) static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->getProgramList(list_id));
  }

  AgsVstTResult ags_vst_edit_controller_ex1_notify_program_list_change(AgsVstEditControllerEx1 *edit_controller_ex1,
								       gint32 list_id, gint32 program_index)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->notifyProgramListChange(list_id, program_index));
  }

  gint32 ags_vst_edit_controller_ex1_get_unit_count(AgsVstEditControllerEx1 *edit_controller_ex1)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->getUnitCount());
  }

  AgsVstTResult ags_vst_edit_controller_ex1_get_unit_info(AgsVstEditControllerEx1 *edit_controller_ex1,
							  gint32 unit_index, AgsVstUnitInfo *info)
  {
    Steinberg::Vst::UnitInfo *tmp_unit_info_0 = static_cast<Steinberg::Vst::UnitInfo*>((void *) info);
    const Steinberg::Vst::UnitInfo& tmp_unit_info_1 = const_cast<Steinberg::Vst::UnitInfo&>(tmp_unit_info_0[0]);
    
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->getUnitInfo(unit_index, const_cast<Steinberg::Vst::UnitInfo&>(tmp_unit_info_1)));
  }

  gint32 ags_vst_edit_controller_ex1_get_program_list_count(AgsVstEditControllerEx1 *edit_controller_ex1)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->getProgramListCount());
  }

  AgsVstTResult ags_vst_edit_controller_ex1_get_program_list_info(AgsVstEditControllerEx1 *edit_controller_ex1,
								  gint32 list_index,
								  AgsVstProgramListInfo *info)
  {
    Steinberg::Vst::ProgramListInfo *tmp_info_0 = static_cast<Steinberg::Vst::ProgramListInfo*>((void *) info);
    const Steinberg::Vst::ProgramListInfo& tmp_info_1 = const_cast<Steinberg::Vst::ProgramListInfo&>(tmp_info_0[0]);

    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->getProgramListInfo(list_index, const_cast<Steinberg::Vst::ProgramListInfo&>(tmp_info_1)));
  }

  AgsVstTResult ags_vst_edit_controller_ex1_get_program_name(AgsVstEditControllerEx1 *edit_controller_ex1,
							     gint32 list_id, gint32 program_index,
							     AgsVstString128 name)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->getProgramName(list_id, program_index,
					       (char16_t *) name));
  }

  AgsVstTResult ags_vst_edit_controller_ex1_get_program_info(AgsVstEditControllerEx1 *edit_controller_ex1,
							     gint32 list_id, gint32 program_index,
							     gchar *attribute_id,
							     AgsVstString128 attribute_value)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->getProgramInfo(list_id, program_index,
													 attribute_id,
													 (char16_t *) attribute_value));
  }

  AgsVstTResult ags_vst_edit_controller_ex1_has_program_pitch_names(AgsVstEditControllerEx1 *edit_controller_ex1,
								    gint32 list_id,
								    gint32 program_index)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->hasProgramPitchNames(list_id,
						     program_index));
  }

  AgsVstTResult ags_vst_edit_controller_ex1_get_program_pitch_name(AgsVstEditControllerEx1 *edit_controller_ex1,
								   gint32 list_id, gint32 program_index,
								   gint16 midi_pitch, AgsVstString128 name)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->getProgramPitchName(list_id, program_index,
													      midi_pitch, (char16_t *) name));
  }

  AgsVstTResult ags_vst_edit_controller_ex1_set_program_name(AgsVstEditControllerEx1 *edit_controller_ex1,
							     gint32 list_id, gint32 program_index,
							     AgsVstString128 name)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->setProgramName(list_id, program_index,
					       (char16_t *) name));
  }

  gint32 ags_vst_edit_controller_ex1_get_selected_unit(AgsVstEditControllerEx1 *edit_controller_ex1)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->getSelectedUnit());
  }

  AgsVstTResult ags_vst_edit_controller_ex1_select_unit(AgsVstEditControllerEx1 *edit_controller_ex1,
							AgsVstUnitID unit_id)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->selectUnit(unit_id));
  }

  AgsVstTResult ags_vst_edit_controller_ex1_get_unit_by_bus(AgsVstEditControllerEx1 *edit_controller_ex1,
							    guint type, guint dir, gint32 bus_index,
							    gint32 channel, AgsVstUnitID unit_id)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->getUnitByBus(type, dir, bus_index,
												       channel, unit_id));
  }

  AgsVstTResult ags_vst_edit_controller_ex1_set_unit_program_data(AgsVstEditControllerEx1 *edit_controller_ex1,
								  gint32 list_or_unit_id, gint32 program_index,
								  AgsVstIBStream *data)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->setUnitProgramData(list_or_unit_id, program_index,
													     static_cast<Steinberg::IBStream*>((void *) data)));
  }
  
  AgsVstTResult ags_vst_edit_controller_ex1_notify_unit_selection(AgsVstEditControllerEx1 *edit_controller_ex1)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->notifyUnitSelection());
  }

  void ags_vst_edit_controller_ex1_update(AgsVstEditControllerEx1 *edit_controller_ex1, AgsVstFUnknown *changed_unknown, gint32 message)
  {
    return(static_cast<Steinberg::Vst::EditControllerEx1*>((void *) edit_controller_ex1)->update(static_cast<Steinberg::FUnknown*>((void *) changed_unknown), message));
  }
  
}
