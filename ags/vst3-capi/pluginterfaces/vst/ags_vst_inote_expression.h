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

#ifndef __AGS_VST_INOTE_EXPRESSION_H__
#define __AGS_VST_INOTE_EXPRESSION_H__

#include <glib.h>

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_types.h>

#ifdef __cplusplus
extern "C" {
#endif
  
  typedef guint32 AgsVstNoteExpressionTypeID;
  typedef gdouble AgsVstNoteExpressionValue;

  typedef enum
  {
    AGS_VST_KVOLUME_TYPE_ID = 0,
    AGS_VST_KPAN_TYPE_ID,
    AGS_VST_KTUNING_TYPE_ID,
    AGS_VST_KVIBRATO_TYPE_ID,
    AGS_VST_KEXPRESSION_TYPE_ID,
    AGS_VST_KBRIGHTNESS_TYPE_ID,
    AGS_VST_KTEXT_TYPE_ID,
    AGS_VST_KPHONEME_TYPE_ID,
    
    AGS_VST_KCUSTOM_START = 100000,
    AGS_VST_KCUSTOM_END   = 200000,
    
    AGS_VST_KINVALID_TYPE_ID = 0xFFFFFFFF
  }AgsVstNoteExpressionTypeIDs;

  typedef struct NoteExpressionValueDescription AgsVstNoteExpressionValueDescription;
  typedef struct NoteExpressionValueEvent AgsVstNoteExpressionValueEvent;
  typedef struct NoteExpressionTextEvent AgsVstNoteExpressionTextEvent;
  typedef struct NoteExpressionTypeInfo AgsVstNoteExpressionTypeInfo;
  typedef struct INoteExpressionController AgsVstINoteExpressionController;
  typedef struct KeyswitchInfo AgsVstKeyswitchInfo;
  typedef guint32 AgsVstKeyswitchTypeID;
  typedef struct IKeyswitchController AgsVstIKeyswitchController;
  
  const AgsVstTUID* ags_vst_inote_expression_controller_get_iid();

  gint32 ags_vst_inote_expression_controller_get_note_expression_count(AgsVstINoteExpressionController *inote_expression_controller,
								       gint32 bus_index, gint16 channel);

  AgsVstTResult ags_vst_inote_expression_controller_get_note_expression_info(AgsVstINoteExpressionController *inote_expression_controller,
									     gint32 bus_index, gint16 channel,
									     gint32 note_expression_index,
									     AgsVstNoteExpressionTypeInfo *note_expression_type_info);
  
  AgsVstTResult ags_vst_inote_expression_controller_get_note_expression_string_by_value(AgsVstINoteExpressionController *inote_expression_controller,
											gint32 bus_index, gint16 channel,
											AgsVstNoteExpressionTypeID id, AgsVstNoteExpressionValue value_normalized, AgsVstString128 *string);

  AgsVstTResult ags_vst_inote_expression_controller_get_note_expression_value_by_string(AgsVstINoteExpressionController *inote_expression_controller,
											gint32 bus_index, gint16 channel,
											AgsVstNoteExpressionTypeID id, AgsVstTChar *in, AgsVstNoteExpressionValue *value_normalized);
  
  const AgsVstTUID* ags_vst_ikeyswitch_controller_get_iid();

  gint32 ags_vst_ikey_switchcontroller_get_keyswitch_count(AgsVstIKeyswitchController *ikeyswitch_controller,
							   gint32 bus_index, gint16 channel);

  AgsVstTResult ags_vst_ikeyswitch_controller_get_keyswitch_info(AgsVstIKeyswitchController *ikeyswitch_controller,
								 gint32 bus_index, gint16 channel,
								 gint32 keyswitch_index, AgsVstKeyswitchInfo *info);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_INOTE_EXPRESSION_H__*/
