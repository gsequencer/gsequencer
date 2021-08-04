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

#ifndef __AGS_VST_EDIT_CONTROLLER_H__
#define __AGS_VST_EDIT_CONTROLLER_H__

#include <ags/vst3-capi/public.sdk/source/vst/ags_vst_component_base.h>
#include <ags/vst3-capi/public.sdk/source/vst/ags_vst_parameters.h>
#include <ags/vst3-capi/public.sdk/source/common/ags_vst_plugin_view.h>
#include <ags/vst3-capi/base/source/ags_vst_fstring.h>

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ibstream.h>

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iedit_controller.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iunits.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct AgsVstEditController EditController;
  typedef struct AgsVstEditorView EditorView;
  typedef struct AgsVstUnit Unit;
  typedef struct AgsVstProgramList ProgramList;
  typedef struct AgsVstProgramListWithPitchNames ProgramListWithPitchNames;
  typedef struct AgsVstEditControllerEx1 EditControllerEx1;
    
  AgsVstEditController* ags_vst_edit_controller_new();
  void ags_vst_edit_controller_delete(AgsVstEditController *edit_controller);

  AgsVstTResult ags_vst_edit_controller_set_component_state(AgsVstEditController *edit_controller, AgsVstIBStream *state);
  AgsVstTResult ags_vst_edit_controller_set_state(AgsVstEditController *edit_controller, AgsVstIBStream *state);
  AgsVstTResult ags_vst_edit_controller_get_state(AgsVstEditController *edit_controller, AgsVstIBStream *state);
  gint32 ags_vst_edit_controller_get_parameter_count(AgsVstEditController *edit_controller);
  AgsVstTResult ags_vst_edit_controller_get_parameter_info(AgsVstEditController *edit_controller,
							   gint32 param_index, AgsVstParameterInfo *info);
  AgsVstTResult ags_vst_edit_controller_get_param_string_by_value(AgsVstEditController *edit_controller,
								  guint32 tag, gdouble value_normalized, AgsVstString128 string);
  AgsVstTResult ags_vst_edit_controller_get_param_value_by_string(AgsVstEditController *edit_controller,
								  guint32 tag, gunichar2 *string, gdouble *value_normalized);
  gdouble ags_vst_edit_controller_normalized_param_to_plain(AgsVstEditController *edit_controller,
							    guint32 tag, gdouble value_normalized);
  gdouble ags_vst_edit_controller_plain_param_to_normalized(AgsVstEditController *edit_controller,
							    guint32 tag, gdouble plain_value);
  gdouble ags_vst_edit_controller_get_param_normalized(AgsVstEditController *edit_controller,
						       guint32 tag);
  AgsVstTResult ags_vst_edit_controller_set_param_normalized(AgsVstEditController *edit_controller,
							     guint32 tag, gdouble value);
  AgsVstTResult ags_vst_edit_controller_set_component_handler(AgsVstEditController *edit_controller,
							      AgsVstIComponentHandler* handler);
  AgsVstIPlugView* ags_vst_edit_controller_create_view(AgsVstEditController *edit_controller,
						       AgsVstFIDString view_name);

  AgsVstTResult ags_vst_edit_controller_set_knob_mode(AgsVstEditController *edit_controller,
						      guint mode);
  AgsVstTResult ags_vst_edit_controller_open_help(AgsVstEditController *edit_controller,
						  gboolean only_check);
  AgsVstTResult ags_vst_edit_controller_open_about_box(AgsVstEditController *edit_controller,
						       gboolean only_check);

  AgsVstTResult ags_vst_edit_controller_initialize(AgsVstEditController *edit_controller,
						   AgsVstFUnknown* context);
  AgsVstTResult ags_vst_edit_controller_terminate(AgsVstEditController *edit_controller);

  AgsVstTResult ags_vst_edit_controller_begin_edit(AgsVstEditController *edit_controller,
						   guint32 tag);
  AgsVstTResult ags_vst_edit_controller_perform_edit(AgsVstEditController *edit_controller,
						     guint32 tag, gdouble value_normalized);
  AgsVstTResult ags_vst_edit_controller_end_edit(AgsVstEditController *edit_controller,
						 guint32 tag);
  AgsVstTResult ags_vst_edit_controller_start_group_edit(AgsVstEditController *edit_controller);
  AgsVstTResult ags_vst_edit_controller_finish_group_edit(AgsVstEditController *edit_controller);

  void ags_vst_edit_controller_editor_destroyed(AgsVstEditController *edit_controller,
						AgsVstEditorView *editor);
  void ags_vst_edit_controller_editor_attached(AgsVstEditController *edit_controller,
					       AgsVstEditorView *editor);
  void ags_vst_edit_controller_editor_removed(AgsVstEditController *edit_controller,
					      AgsVstEditorView *editor);

  guint ags_vst_edit_controller_get_host_knob_mode(AgsVstEditController *edit_controller);

  AgsVstParameter* ags_vst_edit_controller_get_parameter_object(AgsVstEditController *edit_controller,
								guint32 tag);

  AgsVstTResult ags_vst_edit_controller_get_parameter_info_by_tag(AgsVstEditController *edit_controller,
								  guint32 tag, AgsVstParameterInfo *info);

  AgsVstTResult ags_vst_edit_controller_set_dirty(AgsVstEditController *edit_controller,
						  gboolean state);

  AgsVstTResult ags_vst_edit_controller_request_open_editor(AgsVstEditController *edit_controller,
							    AgsVstFIDString editor_name);

  AgsVstIComponentHandler* ags_vst_edit_controller_get_component_handler(AgsVstEditController *edit_controller);
    
  AgsVstEditorView* ags_vst_editor_view_new(AgsVstEditController *controller, AgsVstViewRect *size);
  void ags_vst_editor_view_delete(AgsVstEditorView *editor_view);

  AgsVstEditController* ags_vst_editor_view_get_controller(AgsVstEditorView *editor_view);
  
  void ags_vst_editor_view_attached_to_parent(AgsVstEditorView *editor_view);
  void ags_vst_editor_view_removed_from_parent(AgsVstEditorView *editor_view);

  AgsVstUnit* ags_vst_unit_new(AgsVstString128 unit_name, AgsVstUnitID unit_id, gint32 parent_unit_id,
			       gint32 program_list_id);
  void ags_vst_unit_delete(AgsVstUnit *unit);

  AgsVstUnitInfo* ags_vst_unit_get_info(AgsVstUnit *unit);

  gint32 ags_vst_unit_get_id(AgsVstUnit *unit);

  void ags_vst_unit_set_id(AgsVstUnit *unit,
			   gint32 new_id);

  gunichar2* ags_vst_unit_get_name(AgsVstUnit *unit);

  void ags_vst_unit_set_name(AgsVstUnit *unit,
			     AgsVstString128 new_name);

  gint32 ags_vst_unit_get_program_list_id(AgsVstUnit *unit);

  void ags_vst_unit_set_program_list_id(AgsVstUnit *unit,
					gint32 new_id);

  AgsVstProgramList* ags_vst_program_list_new(AgsVstString128 name, gint32 list_id, AgsVstUnitID unit_id);
  AgsVstProgramList* ags_vst_program_list_new_from_program_list(AgsVstProgramList *program_list);
  void ags_vst_program_list_delete(AgsVstProgramList *program_list);
  
  AgsVstProgramListInfo* ags_vst_program_list_get_info(AgsVstProgramList *program_list);
  gint32 ags_vst_program_list_get_id(AgsVstProgramList *program_list);
  gunichar2* ags_vst_program_list_get_name(AgsVstProgramList *program_list);
  gint32 ags_vst_program_list_get_count(AgsVstProgramList *program_list);

  AgsVstTResult ags_vst_program_list_get_program_name(AgsVstProgramList *program_list,
						      gint32 program_index, AgsVstString128 name);
  AgsVstTResult ags_vst_program_list_set_program_name(AgsVstProgramList *program_list,
						      gint32 program_index, AgsVstString128 name);
  AgsVstTResult ags_vst_program_list_get_program_info(AgsVstProgramList *program_list,
						      gint32 program_index, gchar *attribute_id,
						      AgsVstString128 value);
  AgsVstTResult ags_vst_program_list_has_pitch_names(AgsVstProgramList *program_list,
						     gint32 program_index);
  AgsVstTResult ags_vst_program_list_get_pitch_name(AgsVstProgramList *program_list,
						    gint32 program_index, gint16 midi_pitch, AgsVstString128 name);

  gint32 ags_vst_program_list_add_program(AgsVstProgramList *program_list,
					  AgsVstString128 name);

  gboolean ags_vst_program_list_set_program_info(AgsVstProgramList *program_list,
						 gint32 program_index, gchar *attribute_id, AgsVstString128 value);

  AgsVstParameter* ags_vst_program_list_get_parameter(AgsVstProgramList *program_list);

  AgsVstProgramListWithPitchNames* ags_vst_program_list_with_pitch_names_new(AgsVstString128 name, AgsVstProgramListID list_id, AgsVstUnitID unit_id);
  void ags_vst_program_list_with_pitch_names_delete(AgsVstProgramListWithPitchNames *program_list_with_pitch_names);
  
  gboolean ags_vst_program_list_with_pitch_names_set_pitch_name(AgsVstProgramListWithPitchNames *program_list_with_pitch_names,
								gint32 program_index, gint16 pitch, AgsVstString128 pitch_name);

  gboolean ags_vst_program_list_with_pitch_names_remove_pitch_name(AgsVstProgramListWithPitchNames *program_list_with_pitch_names,
								   gint32 program_index, gint16 pitch);

  gint32 ags_vst_program_list_with_pitch_names_add_program(AgsVstProgramListWithPitchNames *program_list_with_pitch_names,
							   AgsVstString128 name);
  AgsVstTResult ags_vst_program_list_with_pitch_names_has_pitch_names(AgsVstProgramListWithPitchNames *program_list_with_pitch_names,
								      gint32 program_index);
  AgsVstTResult ags_vst_program_list_with_pitch_names_get_pitch_name(AgsVstProgramListWithPitchNames *program_list_with_pitch_names,
								     gint32 program_index, gint16 midi_pitch,
								     AgsVstString128 name);

  AgsVstEditControllerEx1* ags_vst_edit_controller_ex1_new();
  void ags_vst_edit_controller_ex1_delete(AgsVstEditControllerEx1 *edit_controller_ex1);
  
  gboolean ags_vst_edit_controller_ex1_add_unit(AgsVstEditControllerEx1 *edit_controller_ex1,
						AgsVstUnit *unit);

  gboolean ags_vst_edit_controller_ex1_add_program_list(AgsVstEditControllerEx1 *edit_controller_ex1,
							AgsVstProgramList *list);

  AgsVstProgramList* ags_vst_edit_controller_ex1_get_program_list(AgsVstEditControllerEx1 *edit_controller_ex1,
								  gint32 list_id);

  AgsVstTResult ags_vst_edit_controller_ex1_notify_program_list_change(AgsVstEditControllerEx1 *edit_controller_ex1,
								       gint32 list_id, gint32 program_index);

  gint32 ags_vst_edit_controller_ex1_get_unit_count(AgsVstEditControllerEx1 *edit_controller_ex1);
  AgsVstTResult ags_vst_edit_controller_ex1_get_unit_info(AgsVstEditControllerEx1 *edit_controller_ex1,
							  gint32 unit_index, AgsVstUnitInfo *info);

  gint32 ags_vst_edit_controller_ex1_get_program_list_count(AgsVstEditControllerEx1 *edit_controller_ex1);
  AgsVstTResult ags_vst_edit_controller_ex1_get_program_list_info(AgsVstEditControllerEx1 *edit_controller_ex1,
								  gint32 list_index,
								  AgsVstProgramListInfo *info);
  AgsVstTResult ags_vst_edit_controller_ex1_get_program_name(AgsVstEditControllerEx1 *edit_controller_ex1,
							     gint32 list_id, gint32 program_index,
							     AgsVstString128 name);
  AgsVstTResult ags_vst_edit_controller_ex1_get_program_info(AgsVstEditControllerEx1 *edit_controller_ex1,
							     gint32 list_id, gint32 program_index,
							     gchar *attribute_id,
							     AgsVstString128 attribute_value);

  AgsVstTResult ags_vst_edit_controller_ex1_has_program_pitch_names(AgsVstEditControllerEx1 *edit_controller_ex1,
								    gint32 list_id,
								    gint32 program_index);
  AgsVstTResult ags_vst_edit_controller_ex1_get_program_pitch_name(AgsVstEditControllerEx1 *edit_controller_ex1,
								   gint32 list_id, gint32 program_index,
								   gint16 midi_pitch, AgsVstString128 name);

  AgsVstTResult ags_vst_edit_controller_ex1_set_program_name(AgsVstEditControllerEx1 *edit_controller_ex1,
							     gint32 list_id, gint32 program_index,
							     AgsVstString128 name);

  gint32 ags_vst_edit_controller_ex1_get_selected_unit(AgsVstEditControllerEx1 *edit_controller_ex1);
  AgsVstTResult ags_vst_edit_controller_ex1_select_unit(AgsVstEditControllerEx1 *edit_controller_ex1,
							AgsVstUnitID unit_id);

  AgsVstTResult ags_vst_edit_controller_ex1_get_unit_by_bus(AgsVstEditControllerEx1 *edit_controller_ex1,
							    guint type, guint dir, gint32 bus_index,
							    gint32 channel, AgsVstUnitID unit_id);
  AgsVstTResult ags_vst_edit_controller_ex1_set_unit_program_data(AgsVstEditControllerEx1 *edit_controller_ex1,
								  gint32 list_or_unit_id, gint32 program_index,
								  AgsVstIBStream *data);
  
  AgsVstTResult ags_vst_edit_controller_ex1_notify_unit_selection(AgsVstEditControllerEx1 *edit_controller_ex1);

  void ags_vst_edit_controller_ex1_update(AgsVstEditControllerEx1 *edit_controller_ex1, AgsVstFUnknown *changed_unknown, gint32 message);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_EDIT_CONTROLLER_H__*/
