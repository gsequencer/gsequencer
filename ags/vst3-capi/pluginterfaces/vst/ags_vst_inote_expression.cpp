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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_inote_expression.h>

#include <pluginterfaces/vst/ivstnoteexpression.h>

extern "C" {

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_inote_expression_controller_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::INoteExpressionController::iid.toTUID()));
  }

  /**
   * Get note expression count.
   *
   * @param inote_expression_controller the note expression controller
   * @param bus_index the bus index
   * @param channel the MIDI channel
   * @return the note expression count
   *
   * @since 5.0.0
   */
  gint32 ags_vst_inote_expression_controller_get_note_expression_count(AgsVstINoteExpressionController *inote_expression_controller,
								       gint32 bus_index, gint16 channel)
  {
    return(((Steinberg::Vst::INoteExpressionController *) inote_expression_controller)->getNoteExpressionCount(bus_index, channel));
  }

  /**
   * Get note expression info.
   *
   * @param inote_expression_controller note note expression controller
   * @param bus_index the bus index
   * @param channel the MIDI channel
   * @param note_expression_index the note expression index
   * @param note_expression_type_info the note expression type info
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_inote_expression_controller_get_note_expression_info(AgsVstINoteExpressionController *inote_expression_controller,
									     gint32 bus_index, gint16 channel,
									     gint32 note_expression_index,
									     AgsVstNoteExpressionTypeInfo *note_expression_type_info)
  {
    const Steinberg::Vst::NoteExpressionTypeInfo& tmp_note_expression_type_info_0 = const_cast<Steinberg::Vst::NoteExpressionTypeInfo&>(((Steinberg::Vst::NoteExpressionTypeInfo *) note_expression_type_info)[0]);
    
    return(((Steinberg::Vst::INoteExpressionController *) inote_expression_controller)->getNoteExpressionInfo(bus_index, channel,
													      note_expression_index,
													      const_cast<Steinberg::Vst::NoteExpressionTypeInfo&>(tmp_note_expression_type_info_0)));
  }
  
  /**
   * Get note expression string by value.
   *
   * @param inote_expression_controller note note expression controller
   * @param bus_index the bus index
   * @param channel the MIDI channel
   * @param id the type identifier
   * @param value_normalized the normalized value
   * @param string the string
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_inote_expression_controller_get_note_expression_string_by_value(AgsVstINoteExpressionController *inote_expression_controller,
											gint32 bus_index, gint16 channel,
											AgsVstNoteExpressionTypeID id, AgsVstNoteExpressionValue value_normalized, AgsVstString128 *string)
  {
    return(((Steinberg::Vst::INoteExpressionController *) inote_expression_controller)->getNoteExpressionStringByValue(bus_index, channel,
														       (Steinberg::Vst::NoteExpressionTypeID) id, (Steinberg::Vst::NoteExpressionValue) value_normalized, ((Steinberg::Vst::String128 *) string)[0]));
  }

  /**
   * Get note expression value by string.
   *
   * @param inote_expression_controller note note expression controller
   * @param bus_index the bus index
   * @param channel the MIDI channel
   * @param id the type identifier
   * @param in the string
   * @param value_normalized the normalized value
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_inote_expression_controller_get_note_expression_value_by_string(AgsVstINoteExpressionController *inote_expression_controller,
											gint32 bus_index, gint16 channel,
											AgsVstNoteExpressionTypeID id, AgsVstTChar *in, AgsVstNoteExpressionValue *value_normalized)
  {
    const Steinberg::Vst::NoteExpressionValue& tmp_value_normalized_0 = const_cast<Steinberg::Vst::NoteExpressionValue&>(value_normalized[0]);
    
    return(((Steinberg::Vst::INoteExpressionController *) inote_expression_controller)->getNoteExpressionValueByString(bus_index, channel,
														       (Steinberg::Vst::NoteExpressionTypeID) id, (Steinberg::Vst::TChar *) in, const_cast<Steinberg::Vst::NoteExpressionValue&>(tmp_value_normalized_0)));
  }
  
  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_ikeyswitch_controller_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IKeyswitchController::iid.toTUID()));
  }

  /**
   * Get key switch count.
   *
   * @param ikeyswitch_controller the key switch controller
   * @param bus_index the bus index
   * @param channel the MIDI channel
   * @return the key switch count
   *
   * @since 5.0.0
   */
  gint32 ags_vst_ikeyswitch_controller_get_keyswitch_count(AgsVstIKeyswitchController *ikeyswitch_controller,
							   gint32 bus_index, gint16 channel)
  {
    return(((Steinberg::Vst::IKeyswitchController *) ikeyswitch_controller)->getKeyswitchCount(bus_index, channel));
  }

  /**
   * Get key switch count.
   *
   * @param ikeyswitch_controller the key switch controller
   * @param bus_index the bus index
   * @param channel the MIDI channel
   * @param keyswitch_index the key switch index
   * @param info the key switch info
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vstikey_switch_controller_get_keyswitch_info(AgsVstIKeyswitchController *ikeyswitch_controller,
								 gint32 bus_index, gint16 channel,
								 gint32 keyswitch_index, AgsVstKeyswitchInfo *info)
  {
    const Steinberg::Vst::KeyswitchInfo& tmp_info_0 = const_cast<Steinberg::Vst::KeyswitchInfo&>(((Steinberg::Vst::KeyswitchInfo *) info)[0]);
    
    return(((Steinberg::Vst::IKeyswitchController *) ikeyswitch_controller)->getKeyswitchInfo(bus_index, channel,
											      keyswitch_index, const_cast<Steinberg::Vst::KeyswitchInfo&>(tmp_info_0)));
  }

}
