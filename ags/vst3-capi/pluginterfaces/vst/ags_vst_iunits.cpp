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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iunits.h>

#include <pluginterfaces/vst/ivstunits.h>

extern "C" {

  /**
   * Alloc.
   *
   * @return the Steinberg::Vst::UnitInfo as AgsVstUnitInfo
   * 
   * @since 5.0.0
   */
  AgsVstUnitInfo* ags_vst_unit_info_alloc()
  {
    return((AgsVstUnitInfo *) new Steinberg::Vst::UnitInfo());
  }

  /**
   * Free.
   *
   * @param unit_info the unit info
   * 
   * @since 5.0.0
   */
  void ags_vst_unit_info_free(AgsVstUnitInfo *unit_info)
  {
    delete unit_info;
  }

  /**
   * Get identifier.
   *
   * @param unit_info the unit info
   * @return the unit identifier
   *
   * @since 5.0.0
   */
  AgsVstUnitID ags_vst_unit_info_get_id(AgsVstUnitInfo *unit_info)
  {
    return((AgsVstUnitID) ((Steinberg::Vst::UnitInfo *) unit_info)->id);
  }

  /**
   * Get parent unit identifier.
   *
   * @param unit_info the unit info
   * @return the parent unit identifier
   *
   * @since 5.0.0
   */
  AgsVstUnitID ags_vst_unit_info_get_parent_unit_id(AgsVstUnitInfo *unit_info)
  {
    return((AgsVstUnitID) ((Steinberg::Vst::UnitInfo *) unit_info)->parentUnitId);
  }

  /**
   * Get name.
   *
   * @param unit_info the unit info
   * @return the name
   *
   * @since 5.0.0
   */
  gchar* ags_vst_unit_info_get_name(AgsVstUnitInfo *unit_info)
  {
    gchar *name;
    
    name = g_utf16_to_utf8((gunichar2 *) ((Steinberg::Vst::UnitInfo *) unit_info)->name,
			   -1,
			   NULL,
			   NULL,
			   NULL);

    return(name);
  }

  /**
   * Get program list identifier.
   *
   * @param unit_info the unit info
   * @return the program list identifier
   *
   * @since 5.0.0
   */
  AgsVstProgramListID ags_vst_unit_info_get_program_list_id(AgsVstUnitInfo *unit_info)
  {
    return((AgsVstProgramListID) ((Steinberg::Vst::UnitInfo *) unit_info)->programListId);
  }
  
  /**
   * Alloc.
   *
   * @return the Steinberg::Vst::ProgramListInfo as AgsVstProgramListInfo
   * 
   * @since 5.0.0
   */
  AgsVstProgramListInfo* ags_vst_program_list_info_alloc()
  {
    return((AgsVstProgramListInfo *) new Steinberg::Vst::ProgramListInfo());
  }

  /**
   * Free.
   *
   * @param program_list_info the program list info
   * 
   * @since 5.0.0
   */
  void ags_vst_program_list_info_free(AgsVstProgramListInfo *program_list_info)
  {
    delete program_list_info;
  }

  /**
   * Get identifier.
   *
   * @param program_list_info the program list info
   * @return the program list identifier
   *
   * @since 5.0.0
   */
  AgsVstProgramListID ags_vst_program_list_info_get_id(AgsVstProgramListInfo *program_list_info)
  {
    return((AgsVstProgramListID) ((Steinberg::Vst::ProgramListInfo *) program_list_info)->id);
  }

  /**
   * Get name.
   *
   * @param program_list_info the program list info
   * @return the name
   *
   * @since 5.0.0
   */
  gchar* ags_vst_program_list_info_get_name(AgsVstProgramListInfo *program_list_info)
  {
    gchar *name;
    
    name = g_utf16_to_utf8((gunichar2 *) ((Steinberg::Vst::ProgramListInfo *) program_list_info)->name,
			   -1,
			   NULL,
			   NULL,
			   NULL);

    return(name);
  }
  
  /**
   * Get program count.
   *
   * @param program_list_info the program list info
   * @return the program count
   *
   * @since 5.0.0
   */
  gint32 ags_vst_program_list_info_get_program_count(AgsVstProgramListInfo *program_list_info)
  {
    return((gint32) ((Steinberg::Vst::ProgramListInfo *) program_list_info)->programCount);
  }

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_iunit_handler_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IUnitHandler::iid.toTUID()));
  }

  /**
   * Notify unit selection.
   *
   * @param iunit_handler the unit handler
   * @param unit_id the unit identifier
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iunit_handler_notify_unit_selection(AgsVstIUnitHandler *iunit_handler,
							    AgsVstUnitID unit_id)
  {
    return(((Steinberg::Vst::IUnitHandler *) iunit_handler)->notifyUnitSelection((Steinberg::Vst::UnitID) unit_id));
  }

  /**
   * Notify program list change.
   *
   * @param iunit_handler the unit handler
   * @param program_list_id the program list identifier
   * @param program_index the program index
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iunit_handler_notify_program_list_change(AgsVstIUnitHandler *iunit_handler,
								 AgsVstProgramListID program_list_id, gint32 program_index)
  {
    return(((Steinberg::Vst::IUnitHandler *) iunit_handler)->notifyProgramListChange((Steinberg::Vst::ProgramListID) program_list_id, program_index));
  }

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_iunit_handler2_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IUnitHandler2::iid.toTUID()));
  }

  /**
   * Notify unit by bus change.
   *
   * @param iunit_handler2 the unit handler2
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iunit_handler2_notify_unit_by_bus_change(AgsVstIUnitHandler2 *iunit_handler2)
  {
    return(((Steinberg::Vst::IUnitHandler2 *) iunit_handler2)->notifyUnitByBusChange());
  }

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_iunit_info_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IUnitInfo::iid.toTUID()));
  }

  /**
   * Get unit count.
   *
   * @param iunit_info the iunit info
   * @return the unit count
   * 
   * @since 5.0.0
   */
  gint32 ags_vst_iunit_info_get_unit_count(AgsVstIUnitInfo *iunit_info)
  {
    return(((Steinberg::Vst::IUnitInfo *) iunit_info)->getUnitCount());
  }

  /**
   * Get unit info.
   *
   * @param iunit_info the iunit info
   * @param unit_index the unit index
   * @param unit_info the unit info
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iunit_info_get_unit_info(AgsVstIUnitInfo *iunit_info,
						 gint32 unit_index,
						 AgsVstUnitInfo *unit_info)
  {
    Steinberg::Vst::UnitInfo *tmp_unit_info_0 = (Steinberg::Vst::UnitInfo *) unit_info;
    const Steinberg::Vst::UnitInfo& tmp_unit_info_1 = const_cast<Steinberg::Vst::UnitInfo&>(tmp_unit_info_0[0]);
    
    return(((Steinberg::Vst::IUnitInfo *) iunit_info)->getUnitInfo(unit_index, const_cast<Steinberg::Vst::UnitInfo&>(tmp_unit_info_1)));
  }

  /**
   * Get program list count.
   *
   * @param iunit_info the iunit info
   * @return the program list count
   * 
   * @since 5.0.0
   */
  gint32 ags_vst_iunit_info_get_program_list_count(AgsVstIUnitInfo *iunit_info)
  {
    return(((Steinberg::Vst::IUnitInfo *) iunit_info)->getProgramListCount());
  }

  /**
   * Get program list info.
   *
   * @param iunit_info the iunit info
   * @param list_index the list index
   * @param info the program list info
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iunit_info_get_program_list_info(AgsVstIUnitInfo *iunit_info,
							 gint32 list_index,
							 AgsVstProgramListInfo *info)
  {
    Steinberg::Vst::ProgramListInfo *tmp_info_0 = (Steinberg::Vst::ProgramListInfo *) info;
    const Steinberg::Vst::ProgramListInfo& tmp_info_1 = const_cast<Steinberg::Vst::ProgramListInfo&>(tmp_info_0[0]);
    
    return(((Steinberg::Vst::IUnitInfo *) iunit_info)->getProgramListInfo(list_index, const_cast<Steinberg::Vst::ProgramListInfo&>(tmp_info_1)));
  }

  /**
   * Get program name.
   *
   * @param iunit_info the iunit info
   * @param list_id the list identifier
   * @param program_index the program index
   * @param name the return location of name
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iunit_info_get_program_name(AgsVstIUnitInfo *iunit_info,
						    AgsVstProgramListID list_id, gint32 program_index,
						    gchar **name)
  {
    Steinberg::Vst::String128 tmp_name_0;

    AgsVstTResult retval;
    
    retval = ((Steinberg::Vst::IUnitInfo *) iunit_info)->getProgramName((Steinberg::Vst::ProgramListID) list_id, program_index, tmp_name_0);

    name[0] = g_utf16_to_utf8((gunichar2 *) tmp_name_0,
			      -1,
			      NULL,
			      NULL,
			      NULL);
    
    return(retval);
  }

  /**
   * Get program info.
   *
   * @param iunit_info the iunit info
   * @param list_id the list identifier
   * @param program_index the program index
   * @param attribute_id the attribute identifier
   * @param attribute_value the attribute value
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iunit_info_get_program_info(AgsVstIUnitInfo *iunit_info,
						    AgsVstProgramListID list_id, gint32 program_index,
						    AgsVstCString attribute_id,
						    gchar *attribute_value)
  {
    Steinberg::Vst::String128 *tmp_attribute_value_0 = (Steinberg::Vst::String128 *) attribute_value;

#ifdef AGS_VST_UNICODE
    char *tmp_attribute_id;

    GError *error;
#endif

    AgsVstTResult retval;

#ifdef AGS_VST_UNICODE
    error = NULL;
    tmp_attribute_id = g_utf16_to_utf8((gunichar2 *) attribute_id,
				       -1,
				       NULL,
				       NULL,
				       &error);

    if(error != NULL){
      g_warning("error occured while converting from wchar_t");
    }
    
    retval = ((Steinberg::Vst::IUnitInfo *) iunit_info)->getProgramInfo((Steinberg::Vst::ProgramListID) list_id, program_index, tmp_attribute_id, tmp_attribute_value_0[0]);
#else
    retval = ((Steinberg::Vst::IUnitInfo *) iunit_info)->getProgramInfo((Steinberg::Vst::ProgramListID) list_id, program_index, attribute_id, tmp_attribute_value_0[0]);
#endif
        
    return(retval);
  }
  
  /**
   * Has program pitch names.
   *
   * @param iunit_info the iunit info
   * @param list_id the list identifier
   * @param program_index the program index
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iunit_info_has_program_pitch_names(AgsVstIUnitInfo *iunit_info,
							   AgsVstProgramListID list_id, gint32 program_index)
  {
    return(((Steinberg::Vst::IUnitInfo *) iunit_info)->hasProgramPitchNames((Steinberg::Vst::ProgramListID) list_id, program_index));
  }

  /**
   * Get program pitch name.
   *
   * @param iunit_info the iunit info
   * @param list_id the list identifier
   * @param program_index the program index
   * @param midi_pitch the MIDI pitch
   * @param name the return location of name
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iunit_info_get_program_pitch_name(AgsVstIUnitInfo *iunit_info,
							  AgsVstProgramListID list_id, gint32 program_index,
							  gint16 midi_pitch,
							  gchar **name)
  {
    Steinberg::Vst::String128 tmp_name_0;

    AgsVstTResult retval;
    
    retval = ((Steinberg::Vst::IUnitInfo *) iunit_info)->getProgramPitchName((Steinberg::Vst::ProgramListID) list_id, program_index,
									     midi_pitch,
									     tmp_name_0);

    name[0] = g_utf16_to_utf8((gunichar2 *) tmp_name_0,
			      -1,
			      NULL,
			      NULL,
			      NULL);
    
    return(retval);
  }
  
  /**
   * Get selected unit.
   *
   * @param iunit_info the iunit info
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstUnitID ags_vst_iunit_info_get_selected_unit(AgsVstIUnitInfo *iunit_info)
  {
    return(((Steinberg::Vst::IUnitInfo *) iunit_info)->getSelectedUnit());
  }
  
  /**
   * Select unit.
   *
   * @param iunit_info the iunit info
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iunit_info_select_unit(AgsVstIUnitInfo *iunit_info,
					       AgsVstUnitID unit_id)
  {
    return(((Steinberg::Vst::IUnitInfo *) iunit_info)->selectUnit(unit_id));
  }

  /**
   * Get unit by bus.
   *
   * @param iunit_info the iunit info
   * @param type the type
   * @param dir the bus direction
   * @param bus_index the bus index
   * @param channel the MIDI channel
   * @param unit_id the unit identifier
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iunit_info_get_unit_by_bus(AgsVstIUnitInfo *iunit_info,
						   AgsVstMediaType type, AgsVstBusDirection dir, gint32 bus_index,
						   gint32 channel, AgsVstUnitID *unit_id)
  {
    Steinberg::Vst::UnitID *tmp_unit_id_0 = (Steinberg::Vst::UnitID *) unit_id;
    const Steinberg::Vst::UnitID& tmp_unit_id_1 = const_cast<Steinberg::Vst::UnitID&>(tmp_unit_id_0[0]);
    
    return(((Steinberg::Vst::IUnitInfo *) iunit_info)->getUnitByBus(type, dir, bus_index,
								    channel, const_cast<Steinberg::Vst::UnitID&>(tmp_unit_id_1)));
  }

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_iprogram_list_data_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IProgramListData::iid.toTUID()));
  }

  /**
   * Program data supported.
   *
   * @param iprogram_list_data iprogram list data
   * @param list_id the list identifier
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iprogram_list_data_program_data_supported(AgsVstIProgramListData *iprogram_list_data,
								  AgsVstProgramListID list_id)
  {
    return(((Steinberg::Vst::IProgramListData *) iprogram_list_data)->programDataSupported(list_id));
  }

  /**
   * Get Program data.
   *
   * @param iprogram_list_data pirogram list data
   * @param list_id the list identifier
   * @param program_index the program index
   * @param data the data
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iprogram_list_data_get_program_data(AgsVstIProgramListData *iprogram_list_data,
							    AgsVstProgramListID list_id, gint32 program_index,
							    AgsVstIBStream *data)
  {
    return(((Steinberg::Vst::IProgramListData *) iprogram_list_data)->getProgramData(list_id, program_index, (Steinberg::IBStream *) data));
  }

  /**
   * Set Program data.
   *
   * @param iprogram_list_data iprogram list data
   * @param list_id the list identifier
   * @param program_index the program index
   * @param data the data
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iprogram_list_data_set_program_data(AgsVstIProgramListData *iprogram_list_data,
							    AgsVstProgramListID list_id, gint32 program_index,
							    AgsVstIBStream *data)
  {
    return(((Steinberg::Vst::IProgramListData *) iprogram_list_data)->setProgramData(list_id, program_index, (Steinberg::IBStream *) data));
  }
  
  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_iunit_data_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IUnitData::iid.toTUID()));
  }

  /**
   * Program data supported.
   *
   * @param iunit_data iunit data
   * @param unit_id the identifier
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iunit_data_unit_data_supported(AgsVstIUnitData *iunit_data,
						       AgsVstUnitID unit_id)
  {
    return(((Steinberg::Vst::IUnitData *) iunit_data)->unitDataSupported(unit_id));
  }

  /**
   * Get unit data.
   *
   * @param iunit_data iunit data
   * @param unit_id the identifier
   * @param data the data
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iunit_data_get_unit_data(AgsVstIUnitData *iunit_data,
						 AgsVstUnitID unit_id,
						 AgsVstIBStream *data)
  {
    return(((Steinberg::Vst::IUnitData *) iunit_data)->getUnitData(unit_id, (Steinberg::IBStream *) data));
  }
  
  /**
   * Set unit data.
   *
   * @param iunit_data iunit data
   * @param unit_id the identifier
   * @param data the data
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iunit_data_set_unit_data(AgsVstIUnitData *iunit_data,
						 AgsVstUnitID unit_id,
						 AgsVstIBStream *data)
  {
    return(((Steinberg::Vst::IUnitData *) iunit_data)->setUnitData(unit_id, (Steinberg::IBStream *) data));
  }

}
