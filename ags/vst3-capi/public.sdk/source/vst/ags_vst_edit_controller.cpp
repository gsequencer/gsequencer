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
    return(new EditController());
  }
  
  void ags_vst_edit_controller_delete(AgsVstEditController *edit_controller)
  {
    delete edit_controller;
  }

  AgsVstTResult ags_vst_edit_controller_set_component_state(AgsVstEditController *edit_controller, AgsVstIBStream *state)
  {
    return(edit_controller->setComponentState(state));
  }

  AgsVstTResult ags_vst_edit_controller_set_state(AgsVstEditController *edit_controller, AgsVstIBStream *state)
  {
    return(edit_controller->setState(state));
  }

  AgsVstTResult ags_vst_edit_controller_get_state(AgsVstEditController *edit_controller, AgsVstIBStream *state)
  {
    return(edit_controller->getState(state));
  }

  gint32 ags_vst_edit_controller_get_parameter_count(AgsVstEditController *edit_controller)
  {
    return(edit_controller->getParameterCount());
  }

  AgsVstTResult ags_vst_edit_controller_get_parameter_info(AgsVstEditController *edit_controller,
							   gint32 param_index, AgsVstParameterInfo **info)
  {
    return(edit_controller->getParameterInfo(param_index, static_cast<ParameterInfo&>(info[0])));
  }

  AgsVstTResult ags_vst_edit_controller_get_param_string_by_value(AgsVstEditController *edit_controller,
								  guint32 tag, gdouble value_normalized, gchar *str)
  {
    return(edit_controller->getParamStringByValue(tag, value_normalized, str));
  }

  AgsVstTResult ags_vst_edit_controller_get_param_value_by_string(AgsVstEditController *edit_controller,
								  guint32 tag, gchar *string, gdouble *value_normalized)
  {
    return(edit_controller->getParamValueByString(tag, string, static_cast<ParamValue&>(value_normalized[0])));
  }

  gdouble ags_vst_edit_controller_normalized_param_to_plain(AgsVstEditController *edit_controller,
							    guint32 tag, gdouble value_normalized)
  {
    return(edit_controller->normalizedParamToPlain(tag, value_normalized));
  }

  gdouble ags_vst_edit_controller_plain_param_to_normalized(AgsVstEditController *edit_controller,
							    guint32 tag, gdouble plain_value)
  {
    return(edit_controller->plainParamToNormalized(tag, plain_value));
  }

  gdouble ags_vst_edit_controller_get_param_normalized(AgsVstEditController *edit_controller,
						       guint32 tag)
  {
    return(edit_controller->getParamNormalized(tag));
  }

  AgsVstTResult ags_vst_edit_controller_set_param_normalized(AgsVstEditController *edit_controller,
							     guint32 tag, gdouble value)
  {
    return(edit_controller->setParamNormalized(tag, value));
  }

  AgsVstTResult ags_vst_edit_controller_set_component_handler(AgsVstEditController *edit_controller,
							      AgsVstIComponentHandler *handler)
  {
    return(edit_controller->setComponentHandler(handler));
  }

  AgsVstIPlugView* ags_vst_edit_controller_create_view(AgsVstEditController *edit_controller,
						       gchar *view_name)
  {
    return(edit_controller->createView(view_name));
  }

  AgsVstTResult ags_vst_edit_controller_set_knob_mode(AgsVstEditController *edit_controller,
						      guint mode)
  {
    return(edit_controller->setKnowMode(mode));
  }

  AgsVstTResult ags_vst_edit_controller_open_help(AgsVstEditController *edit_controller,
						  gboolean only_check)
  {
    return(edit_controller->openHelp(only_check));
  }

  AgsVstTResult ags_vst_edit_controller_open_about_box(AgsVstEditController *edit_controller,
						       gboolean only_check)
  {
    return(edit_controller->openAboutBox(only_check));
  }

  AgsVstTResult ags_vst_edit_controller_initialize(AgsVstEditController *edit_controller,
						   AgsVstFUnknown *context)
  {
    return(edit_controller->initialize(context));
  }

  AgsVstTResult ags_vst_edit_controller_terminate(AgsVstEditController *edit_controller)
  {
    return(edit_controller->terminate());
  }

  AgsVstTResult ags_vst_edit_controller_begin_edit(AgsVstEditController *edit_controller,
						   guint32 tag)
  {
    return(edit_controller->beginEdit(tag));
  }

  AgsVstTResult ags_vst_edit_controller_perform_edit(AgsVstEditController *edit_controller,
						     guint32 tag, gdouble value_normalized)
  {
    return(edit_controller->performEdit(tag, value_normalized));
  }

  AgsVstTResult ags_vst_edit_controller_end_edit(AgsVstEditController *edit_controller,
						 guint32 tag)
  {
    return(edit_controller->endEdit(tag));
  }

  AgsVstTResult ags_vst_edit_controller_start_group_edit(AgsVstEditController *edit_controller)
  {
    return(edit_controller->startGroupEdit());
  }

  AgsVstTResult ags_vst_edit_controller_finish_group_edit(AgsVstEditController *edit_controller)
  {
    return(edit_controller->finishGroupEdit());
  }

  void ags_vst_edit_controller_editor_destroyed(AgsVstEditController *edit_controller,
						AgsVstEditorView *editor)
  {
    return(edit_controller->editorDestroyed(editor));
  }

  void ags_vst_edit_controller_editor_attached(AgsVstEditController *edit_controller,
					       AgsVstEditorView *editor)
  {
    return(edit_controller->editorAttached(editor));
  }

  void ags_vst_edit_controller_editor_removed(AgsVstEditController *edit_controller,
					      AgsVstEditorView *editor)
  {
    return(edit_controller->editorRemoved(editor));
  }

  guint ags_vst_edit_controller_get_host_knob_mode(AgsVstEditController *edit_controller)
  {
    return(edit_controller->getHostKnobMode());
  }

  AgsVstParameter* ags_vst_edit_controller_get_parameter_object(AgsVstEditController *edit_controller,
								guint32 tag)
  {
    return(edit_controller->getParameterObject(tag));
  }

  AgsVstTResult ags_vst_edit_controller_get_parameter_info_by_tag(AgsVstEditController *edit_controller,
								  guint32 tag, AgsVstParameterInfo **info)
  {
    return(edit_controller->getParameterInfoByTag(tag, static_cast<ParameterInfo&>(info[0])));
  }

  AgsVstTResult ags_vst_edit_controller_set_dirty(AgsVstEditController *edit_controller,
						  gboolean state)
  {
    return(edit_controller->setDirty(state));
  }

  AgsVstTResult ags_vst_edit_controller_request_open_editor(AgsVstEditController *edit_controller,
							    gchar *editor_name)
  {
    return(edit_controller->openEditor(editor_name));
  }

  AgsVstIComponentHandler* ags_vst_edit_controller_get_component_handler(AgsVstEditController *edit_controller)
  {
    return(edit_controller->getComponentHandler());
  }
    
  AgsVstEditorView* ags_vst_editor_view_new(AgsVstEditController *controller, AgsVstViewRect *size)
  {
    return(new EditorView(controller, size));
  }

  void ags_vst_editor_view_delete(AgsVstEditorView *editor_view)
  {
    delete editor_view;
  }

  AgsVstEditController* ags_vst_editor_view_get_controller(AgsVstEditorView *editor_view)
  {
    return(editor_view->getController());
  }
  
  void ags_vst_editor_view_attached_to_parent(AgsVstEditorView *editor_view)
  {
    return(editor_view->attachedToParent());
  }

  void ags_vst_editor_view_removed_from_parent(AgsVstEditorView *editor_view)
  {
    return(editor_view->removedFromParent());
  }
  
  AgsVstUnit* ags_vst_unit_new(gchar *unit_name, AgsVstUnitID unit_id, gint32 parent_unit_id,
			       gint32 program_list_id)
  {
    return(new Unit(unit_name, unit_id, parent_unit_id,
		    program_list_id));
  }

  void ags_vst_unit_delete(AgsVstUnit *unit)
  {
    delete unit;
  }

  AgsVstUnitInfo* ags_vst_unit_get_info(AgsVstUnit *unit)
  {
    return(unit->getInfo());
  }

  gint32 ags_vst_unit_get_id(AgsVstUnit *unit)
  {
    return(unit->getId());
  }

  void ags_vst_unit_set_id(AgsVstUnit *unit,
			   gint32 new_id)
  {
    return(unit->setId(new_id));
  }

  gchar *ags_vst_unit_get_name(AgsVstUnit *unit)
  {
    return(unit->getName());
  }

  void ags_vst_unit_set_name(AgsVstUnit *unit,
			     gchar *new_name)
  {
    return(unit->setName(new_name));
  }

  gint32 ags_vst_unit_get_program_list_id(AgsVstUnit *unit)
  {
    return(unit->getProgramListId());
  }

  void ags_vst_unit_set_program_list_id(AgsVstUnit *unit,
					gint32 new_id)
  {
    return(unit->setProgramListId(new_id));
  }

  AgsVstProgramList* ags_vst_program_list_new(gchar *name, gint32 list_id, AgsVstUnitID unit_id)
  {
    return(new ProgramList(name, list_id, unit_id));
  }

  AgsVstProgramList* ags_vst_program_list_new_from_program_list(AgsVstProgramList **program_list)
  {
    return(new ProgramList(static_cast<ProgramList&>(program_list[0])));
  }

  void ags_vst_program_list_delete(AgsVstProgramList *program_list)
  {
    delete program_list;
  }
  
  AgsVstProgramListInfo* ags_vst_program_list_get_info(AgsVstProgramList *program_list)
  {
    return(program_list->getInfo());
  }

  gint32 ags_vst_program_list_get_id(AgsVstProgramList *program_list)
  {
    return(program_list->getId());
  }

  gchar* ags_vst_program_list_get_name(AgsVstProgramList *program_list)
  {
    return(program_list->getName());
  }

  gint32 ags_vst_program_list_get_count(AgsVstProgramList *program_list)
  {
    return(program_list->getCount());
  }

  AgsVstTResult ags_vst_program_list_get_program_name(AgsVstProgramList *program_list,
						      gint32 program_index, gchar *out_name)
  {
    return(program_list->getProgramName(program_index, out_name));
  }

  AgsVstTResult ags_vst_program_list_set_program_name(AgsVstProgramList *program_list,
						      gint32 program_index, gchar *in_name)
  {
    return(program_list->setProgramName(program_index, in_name));
  }

  AgsVstTResult ags_vst_program_list_get_program_info(AgsVstProgramList *program_list,
						      gint32 program_index, gchar *attribute_id,
						      gchar *out_value)
  {
    return(program_list->getProgramInfo(program_index, attribute_id,
					out_value));
  }

  AgsVstTResult ags_vst_program_list_has_pitch_names(AgsVstProgramList *program_list,
						     gint32 program_index)
  {
    return(program_list->hasPitchNames(program_index));
  }

  AgsVstTResult ags_vst_program_list_get_pitch_name(AgsVstProgramList *program_list,
						    gint32 program_index, gint16 midi_pitch, gchar *out_name)
  {
    return(program_list->getPitchName(program_index, midi_pitch, out_name));
  }

  gint32 ags_vst_program_list_add_program(AgsVstProgramList *program_list,
					  gchar *name)
  {
    return(program_list->addProgram(name));
  }

  gboolean ags_vst_program_list_set_program_info(AgsVstProgramList *program_list,
						 gint32 program_index, gchar *attribute_id, gchar *value)
  {
    return(program_list->setProgramInfo(program_index, attribute_id, value));
  }

  AgsVstParameter* ags_vst_program_list_get_parameter(AgsVstProgramList *program_list)
  {
    return(program_list->getParameter());
  }
  
  AgsVstProgramListWithPitchNames* ags_vst_program_list_with_pitch_names_new(gchar *name, gint32 list_id, AgsVstUnitID unit_id)
  {
    return(new ProgramListWithPitchNames(name, list_id, unit_id));
  }

  void ags_vst_program_list_with_pitch_names_delete(AgsVstProgramListWithPitchNames *program_list_with_pitch_names)
  {
    delete program_list_with_pitch_names;
  }

  gboolean ags_vst_program_list_with_pitch_names_set_pitch_name(AgsVstProgramListWithPitchNames *program_list_with_pitch_names,
								gint32 program_index, gint16 pitch, gchar *pitch_name)
  {
    return(program_list_with_pitch_names->setPitchName(program_index, pitch, pitch_name));
  }

  gboolean ags_vst_program_list_with_pitch_names_remove_pitch_name(AgsVstProgramListWithPitchNames *program_list_with_pitch_names,
								   gint32 program_index, gint16 pitch)
  {
    return(program_list_with_pitch_names->removePitchName(program_index, pitch));
  }

  gint32 ags_vst_program_list_with_pitch_names_add_program(AgsVstProgramListWithPitchNames *program_list_with_pitch_names,
							   gchar *name)
  {
    return(program_list_with_pitch_names->addProgram(name));
  }

  AgsVstTResult ags_vst_program_list_with_pitch_names_has_pitch_names(AgsVstProgramListWithPitchNames *program_list_with_pitch_names,
								      gint32 program_index)
  {
    return(program_list_with_pitch_names->hasPitchNames(program_index));
  }

  AgsVstTResult ags_vst_program_list_with_pitch_names_get_pitch_name(AgsVstProgramListWithPitchNames *program_list_with_pitch_names,
								     gint32 program_index, gint16 midi_pitch,
								     gchar *name)
  {
    return(program_list_with_pitch_names->getPitchName(program_index, midi_pitch,
						       name));
  }

  AgsVstEditControllerEx1* ags_vst_edit_controller_ex1_new()
  {
    return(new EditControllerEx1());
  }

  void ags_vst_edit_controller_ex1_delete(AgsVstEditControllerEx1 *edit_controller_ex1)
  {
    delete edit_controller_ex1;
  }
  
  gboolean ags_vst_edit_controller_ex1_add_unit(AgsVstEditControllerEx1 *edit_controller_ex1,
						AgsVstUnit *unit)
  {
    return(edit_controller_ex1->addUnit(unit));
  }

  gboolean ags_vst_edit_controller_ex1_add_program_list(AgsVstEditControllerEx1 *edit_controller_ex1,
							AgsVstProgramList *list)
  {
    return(edit_controller_ex1->addProgramList(list));
  }

  AgsVstProgramList* ags_vst_edit_controller_ex1_get_program_list(AgsVstEditControllerEx1 *edit_controller_ex1,
								  gint32 list_id)
  {
    return(edit_controller_ex1->getProgramList(list_id));
  }

  AgsVstTResult ags_vst_edit_controller_ex1_notify_program_list_change(AgsVstEditControllerEx1 *edit_controller_ex1,
								       gint32 list_id, gint32 program_index)
  {
    return(edit_controller_ex1->notify_program_list_change(list_id, program_index));
  }

  gint32 ags_vst_edit_controller_ex1_get_unit_count(AgsVstEditControllerEx1 *edit_controller_ex1)
  {
    return(edit_controller_ex1->getUnitCount());
  }

  AgsVstTResult ags_vst_edit_controller_ex1_get_unit_info(AgsVstEditControllerEx1 *edit_controller_ex1,
							  gint32 unit_index, AgsVstUnitInfo **info)
  {
    return(edit_controller_ex1->getUnitInfo(unit_index, static_cast<UnitInfo&>(info[0])));
  }

  gint32 ags_vst_edit_controller_ex1_get_program_list_count(AgsVstEditControllerEx1 *edit_controller_ex1)
  {
    return(edit_controller_ex1->getProgramListCount());
  }

  AgsVstTResult ags_vst_edit_controller_ex1_get_program_list_info(AgsVstEditControllerEx1 *edit_controller_ex1,
								  gint32 list_index,
								  AgsVstProgramListInfo **info)
  {
    return(edit_controller_ex1->getProgramListInfo(list_index,
						   static_cast<ProgramListInfo&>(info[0])));
  }

  AgsVstTResult ags_vst_edit_controller_ex1_get_program_name(AgsVstEditControllerEx1 *edit_controller_ex1,
							     gint32 list_id, gint32 program_index,
							     gchar *name)
  {
    return(edit_controller_ex1->getProgramName(list_id, program_index,
					       name));
  }

  AgsVstTResult ags_vst_edit_controller_ex1_get_program_info(AgsVstEditControllerEx1 *edit_controller_ex1,
							     gint32 list_id, gint32 program_index,
							     gchar *attribute_id,
							     gchar *attribute_value)
  {
    return(edit_controller_ex1->getProgramInfo(list_id, program_index,
					       attribtue_id,
					       attribute_value));
  }

  AgsVstTResult ags_vst_edit_controller_ex1_has_program_pitch_names(AgsVstEditControllerEx1 *edit_controller_ex1,
								    gint32 list_id,
								    gint32 program_index)
  {
    return(edit_controller_ex1->hasProgramPitchNames(list_id,
						     program_index));
  }

  AgsVstTResult ags_vst_edit_controller_ex1_get_program_pitch_name(AgsVstEditControllerEx1 *edit_controller_ex1,
								   gint32 list_id, gint32 program_index,
								   gint16 midi_pitch, gchar *name)
  {
    return(edit_controller_ex1->getProgramPitchName(list_id, program_index,
						    midi_pitch, name));
  }

  AgsVstTResult ags_vst_edit_controller_ex1_set_program_name(AgsVstEditControllerEx1 *edit_controller_ex1,
							     gint32 list_id, gint32 program_index,
							     gchar *name)
  {
    return(edit_controller_ex1->setProgramName(list_id, program_index,
					       name));
  }

  gint32 ags_vst_edit_controller_ex1_get_selected_unit(AgsVstEditControllerEx1 *edit_controller_ex1)
  {
    return(edit_controller_ex1->getSelectedUnit());
  }

  AgsVstTResult ags_vst_edit_controller_ex1_select_unit(AgsVstEditControllerEx1 *edit_controller_ex1,
							AgsVstUnitID unit_id)
  {
    return(edit_controller_ex1->selectUnit(unit_id));
  }

  AgsVstTResult ags_vst_edit_controller_ex1_get_unit_by_bus(AgsVstEditControllerEx1 *edit_controller_ex1,
							    guint type, guint dir, gint32 bus_index,
							    gint32 channel, gint32 *unit_id)
  {
    return(edit_controller_ex1->getUnitByBus(type, dir, bus_index,
					     channel, unit_id));
  }

  AgsVstTResult ags_vst_edit_controller_ex1_set_unit_program_data(AgsVstEditControllerEx1 *edit_controller_ex1,
								  gint32 list_or_unit_id, gint32 program_index,
								  AgsVstIBStream *data)
  {
    return(edit_controller_ex1->setUnitProgramData(list_or_unit_id, program_index,
						   data));
  }
  
  AgsVstTResult ags_vst_edit_controller_ex1_notify_unit_selection(AgsVstEditControllerEx1 *edit_controller_ex1)
  {
    return(edit_controller_ex1->notifyUnitSelection());
  }

  void ags_vst_edit_controller_ex1_update(AgsVstFUnknown *changedUnknown, gint32 message)
  {
    return(edit_controller_ex1->update(message));
  }
  
}
