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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iunits.h>

#include <pluginterfaces/vst/ivstunits.h>

extern "C" {

  const AgsVstTUID* ags_vst_iunit_handler_get_iid()
  {
    //TODO:JK: implement me
  }
  
  AgsVstTResult ags_vst_iunit_handler_notify_unit_selection(AgsVstIUnitHandler *iunit_handler,
							    AgsVstUnitID unit_id)
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iunit_handler_notify_program_list_change(AgsVstIUnitHandler *iunit_handler,
								 AgsVstProgramListID program_list_id, gint32 program_index)
  {
    //TODO:JK: implement me
  }

  const AgsVstTUID* ags_vst_iunit_handler2_get_iid()
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iunit_handler2_notify_unit_bus_change(AgsVstIUnitHandler2 *iunit_handler2)
  {
    //TODO:JK: implement me
  }

  const AgsVstTUID* ags_vst_iunit_info_get_iid()
  {
    //TODO:JK: implement me
  }

  gint32 ags_vst_iunit_info_get_unit_count(AgsVstIUnitInfo *iunit_info)
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iunit_info_get_unit_info(AgsVstIUnitInfo *iunit_info,
						 gint32 unit_index,
						 AgsVstUnitInfo *unit_info)
  {
    //TODO:JK: implement me
  }

  gint32 ags_vst_iunit_info_get_program_list_count(AgsVstIUnitInfo *iunit_info)
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iunit_info_get_program_list_info(AgsVstIUnitInfo *iunit_info,
							 gint32 list_index,
							 AgsVstProgramListInfo *info)
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iunit_info_get_program_name(AgsVstIUnitInfo *iunit_info,
						    AgsVstProgramListID list_id, gint32 program_index,
						    AgsVstString128 name)
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iunit_info_get_program_info(AgsVstIUnitInfo *iunit_info,
						    AgsVstProgramListID list_id, gint32 program_index,
						    AgsVstCString attribute_id,
						    AgsVstString128 attribute_value)
  {
    //TODO:JK: implement me
  }
  
  AgsVstTResult ags_vst_iunit_info_has_program_pitch_names(AgsVstIUnitInfo *iunit_info,
							   AgsVstProgramListID list_id, gint32 program_index)
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iunit_info_get_program_pitch_name(AgsVstIUnitInfo *iunit_info,
							  AgsVstProgramListID list_id, gint32 program_index,
							  AgsVstString128 name)
  {
    //TODO:JK: implement me
  }
  
  AgsVstUnitID ags_vst_iunit_info_get_selected_unit(AgsVstIUnitInfo *iunit_info)
  {
    //TODO:JK: implement me
  }
  
  AgsVstTResult ags_vst_iunit_info_select_unit(AgsVstIUnitInfo *iunit_info,
					       AgsVstUnitID unit_id)
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iunit_info_get_unit_by_bus(AgsVstIUnitInfo *iunit_info,
						   AgsVstMediaType type, AgsVstBusDirection dir, gint32 busIndex,
						   gint32 channel, AgsVstUnitID *unit_id)
  {
    //TODO:JK: implement me
  }

  const AgsVstTUID* ags_vst_iprogram_list_data_get_iid()
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iprogram_list_data_program_data_supported(AgsVstIProgramListData *iprogram_list_data,
								  AgsVstProgramListID list_id)
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iprogram_list_data_get_program_data(AgsVstIProgramListData *iprogram_list_data,
							    AgsVstProgramListID list_id, gint32 program_index,
							    AgsVstIBStream *data)
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iprogram_list_data_set_program_data(AgsVstIProgramListData *iprogram_list_data,
							    AgsVstProgramListID list_id, gint32 program_index,
							    AgsVstIBStream *data)
  {
    //TODO:JK: implement me
  }
  
  const AgsVstTUID* ags_vst_iunit_data_get_iid()
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iunit_data_unit_data_supported(AgsVstIUnitData *iunit_data,
						       AgsVstUnitID unit_id)
  {
    //TODO:JK: implement me
  }

  AgsVstTResult ags_vst_iunit_data_get_unit_data(AgsVstIUnitData *iunit_data,
						 AgsVstUnitID unit_id,
						 AgsVstIBStream *data)
  {
    //TODO:JK: implement me
  }
  
  AgsVstTResult ags_vst_iunit_data_set_unit_data(AgsVstIUnitData *iunit_data,
						 AgsVstUnitID unit_id,
						 AgsVstIBStream *data)
  {
    //TODO:JK: implement me
  }

}
