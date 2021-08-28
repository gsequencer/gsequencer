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
    return((AgsVstTUID *) &(Steinberg::Vst::IUnitHandler::iid.toTUID()));
  }
  
  AgsVstTResult ags_vst_iunit_handler_notify_unit_selection(AgsVstIUnitHandler *iunit_handler,
							    AgsVstUnitID unit_id)
  {
    return(((Steinberg::Vst::IUnitHandler *) iunit_handler)->notifyUnitSelection((Steinberg::Vst::UnitID) unit_id));
  }

  AgsVstTResult ags_vst_iunit_handler_notify_program_list_change(AgsVstIUnitHandler *iunit_handler,
								 AgsVstProgramListID program_list_id, gint32 program_index)
  {
    return(((Steinberg::Vst::IUnitHandler *) iunit_handler)->notifyProgramListChange((Steinberg::Vst::ProgramListID) program_list_id, program_index));
  }

  const AgsVstTUID* ags_vst_iunit_handler2_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IUnitHandler2::iid.toTUID()));
  }

  AgsVstTResult ags_vst_iunit_handler2_notify_unit_by_bus_change(AgsVstIUnitHandler2 *iunit_handler2)
  {
    return(((Steinberg::Vst::IUnitHandler2 *) iunit_handler2)->notifyUnitByBusChange());
  }

  const AgsVstTUID* ags_vst_iunit_info_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IUnitInfo::iid.toTUID()));
  }

  gint32 ags_vst_iunit_info_get_unit_count(AgsVstIUnitInfo *iunit_info)
  {
    return(((Steinberg::Vst::IUnitInfo *) iunit_info)->getUnitCount());
  }

  AgsVstTResult ags_vst_iunit_info_get_unit_info(AgsVstIUnitInfo *iunit_info,
						 gint32 unit_index,
						 AgsVstUnitInfo *unit_info)
  {
    Steinberg::Vst::UnitInfo *tmp_unit_info_0 = (Steinberg::Vst::UnitInfo *) unit_info;
    const Steinberg::Vst::UnitInfo& tmp_unit_info_1 = const_cast<Steinberg::Vst::UnitInfo&>(tmp_unit_info_0[0]);
    
    return(((Steinberg::Vst::IUnitInfo *) iunit_info)->getUnitInfo(unit_index, const_cast<Steinberg::Vst::UnitInfo&>(tmp_unit_info_1)));
  }

  gint32 ags_vst_iunit_info_get_program_list_count(AgsVstIUnitInfo *iunit_info)
  {
    return(((Steinberg::Vst::IUnitInfo *) iunit_info)->getProgramListCount());
  }

  AgsVstTResult ags_vst_iunit_info_get_program_list_info(AgsVstIUnitInfo *iunit_info,
							 gint32 list_index,
							 AgsVstProgramListInfo *info)
  {
    Steinberg::Vst::ProgramListInfo *tmp_info_0 = (Steinberg::Vst::ProgramListInfo *) info;
    const Steinberg::Vst::ProgramListInfo& tmp_info_1 = const_cast<Steinberg::Vst::ProgramListInfo&>(tmp_info_0[0]);
    
    return(((Steinberg::Vst::IUnitInfo *) iunit_info)->getProgramListInfo(list_index, const_cast<Steinberg::Vst::ProgramListInfo&>(tmp_info_1)));
  }

  AgsVstTResult ags_vst_iunit_info_get_program_name(AgsVstIUnitInfo *iunit_info,
						    AgsVstProgramListID list_id, gint32 program_index,
						    AgsVstString128 name)
  {
    Steinberg::Vst::String128 *tmp_name_0 = (Steinberg::Vst::String128 *) &name;
    
    return(((Steinberg::Vst::IUnitInfo *) iunit_info)->getProgramName((Steinberg::Vst::ProgramListID) list_id, program_index, tmp_name_0[0]));
  }

  AgsVstTResult ags_vst_iunit_info_get_program_info(AgsVstIUnitInfo *iunit_info,
						    AgsVstProgramListID list_id, gint32 program_index,
						    AgsVstCString attribute_id,
						    AgsVstString128 attribute_value)
  {
    Steinberg::Vst::String128 *tmp_attribute_value_0 = (Steinberg::Vst::String128 *) &attribute_value;
    
    return(((Steinberg::Vst::IUnitInfo *) iunit_info)->getProgramInfo((Steinberg::Vst::ProgramListID) list_id, program_index, attribute_id, tmp_attribute_value_0[0]));
  }
  
  AgsVstTResult ags_vst_iunit_info_has_program_pitch_names(AgsVstIUnitInfo *iunit_info,
							   AgsVstProgramListID list_id, gint32 program_index)
  {
    return(((Steinberg::Vst::IUnitInfo *) iunit_info)->hasProgramPitchNames((Steinberg::Vst::ProgramListID) list_id, program_index));
  }

  AgsVstTResult ags_vst_iunit_info_get_program_pitch_name(AgsVstIUnitInfo *iunit_info,
							  AgsVstProgramListID list_id, gint32 program_index,
							  gint16 midi_pitch,
							  AgsVstString128 name)
  {
    Steinberg::Vst::String128 *tmp_name_0 = (Steinberg::Vst::String128 *) &name;
    
    return(((Steinberg::Vst::IUnitInfo *) iunit_info)->getProgramPitchName((Steinberg::Vst::ProgramListID) list_id, program_index,
									   midi_pitch,
									   tmp_name_0[0]));
  }
  
  AgsVstUnitID ags_vst_iunit_info_get_selected_unit(AgsVstIUnitInfo *iunit_info)
  {
    return(((Steinberg::Vst::IUnitInfo *) iunit_info)->getSelectedUnit());
  }
  
  AgsVstTResult ags_vst_iunit_info_select_unit(AgsVstIUnitInfo *iunit_info,
					       AgsVstUnitID unit_id)
  {
    return(((Steinberg::Vst::IUnitInfo *) iunit_info)->selectUnit(unit_id));
  }

  AgsVstTResult ags_vst_iunit_info_get_unit_by_bus(AgsVstIUnitInfo *iunit_info,
						   AgsVstMediaType type, AgsVstBusDirection dir, gint32 bus_index,
						   gint32 channel, AgsVstUnitID *unit_id)
  {
    Steinberg::Vst::UnitID *tmp_unit_id_0 = (Steinberg::Vst::UnitID *) unit_id;
    const Steinberg::Vst::UnitID& tmp_unit_id_1 = const_cast<Steinberg::Vst::UnitID&>(tmp_unit_id_0[0]);
    
    return(((Steinberg::Vst::IUnitInfo *) iunit_info)->getUnitByBus(type, dir, bus_index,
								    channel, const_cast<Steinberg::Vst::UnitID&>(tmp_unit_id_1)));
  }

  const AgsVstTUID* ags_vst_iprogram_list_data_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IProgramListData::iid.toTUID()));
  }

  AgsVstTResult ags_vst_iprogram_list_data_program_data_supported(AgsVstIProgramListData *iprogram_list_data,
								  AgsVstProgramListID list_id)
  {
    return(((Steinberg::Vst::IProgramListData *) iprogram_list_data)->programDataSupported(list_id));
  }

  AgsVstTResult ags_vst_iprogram_list_data_get_program_data(AgsVstIProgramListData *iprogram_list_data,
							    AgsVstProgramListID list_id, gint32 program_index,
							    AgsVstIBStream *data)
  {
    return(((Steinberg::Vst::IProgramListData *) iprogram_list_data)->getProgramData(list_id, program_index, (Steinberg::IBStream *) data));
  }

  AgsVstTResult ags_vst_iprogram_list_data_set_program_data(AgsVstIProgramListData *iprogram_list_data,
							    AgsVstProgramListID list_id, gint32 program_index,
							    AgsVstIBStream *data)
  {
    return(((Steinberg::Vst::IProgramListData *) iprogram_list_data)->setProgramData(list_id, program_index, (Steinberg::IBStream *) data));
  }
  
  const AgsVstTUID* ags_vst_iunit_data_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IUnitData::iid.toTUID()));
  }

  AgsVstTResult ags_vst_iunit_data_unit_data_supported(AgsVstIUnitData *iunit_data,
						       AgsVstUnitID unit_id)
  {
    return(((Steinberg::Vst::IUnitData *) iunit_data)->unitDataSupported(unit_id));
  }

  AgsVstTResult ags_vst_iunit_data_get_unit_data(AgsVstIUnitData *iunit_data,
						 AgsVstUnitID unit_id,
						 AgsVstIBStream *data)
  {
    return(((Steinberg::Vst::IUnitData *) iunit_data)->getUnitData(unit_id, (Steinberg::IBStream *) data));
  }
  
  AgsVstTResult ags_vst_iunit_data_set_unit_data(AgsVstIUnitData *iunit_data,
						 AgsVstUnitID unit_id,
						 AgsVstIBStream *data)
  {
    return(((Steinberg::Vst::IUnitData *) iunit_data)->setUnitData(unit_id, (Steinberg::IBStream *) data));
  }

}
