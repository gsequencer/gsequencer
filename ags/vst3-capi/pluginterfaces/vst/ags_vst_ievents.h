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

#ifndef __AGS_VST_IEVENTS_H__
#define __AGS_VST_IEVENTS_H__

#include <glib.h>

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iprocess_context.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_inote_expression.h>

#ifdef __cplusplus
extern "C" {
#endif
  
  typedef struct IEventList AgsVstIEventList;
  typedef struct NoteOnEvent AgsVstNoteOnEvent;
  typedef struct NoteOffEvent AgsVstNoteOffEvent;
  typedef struct DataEvent AgsVstDataEvent;
  typedef struct PolyPressureEvent AgsVstPolyPressureEvent;
  typedef struct ChordEvent AgsVstChordEvent;
  typedef struct ScaleEvent AgsVstScaleEvent;
  typedef struct LegacyMIDICCOutEventEvent AgsVstLegacyMIDICCOutEventEvent;
  typedef struct Event AgsVstEvent;  
  
  typedef enum{
    AGS_VST_KNOTE_ID_USER_RANGE_LOWER_BOUND = -10000, 
    AGS_VST_KNOTE_ID_USER_RANGE_UPPER_BOUND = -1000,
  }AgsVstNoteMidiUserRange;

  AgsVstEvent* ags_vst_note_on_event_alloc(gint channel, gint pitch, gfloat tuning, gfloat velocity, gint32 length, gint32 note_id);
  
  AgsVstEvent* ags_vst_note_off_event_alloc(gint channel, gint pitch, gfloat tuning, gfloat velocity, gint32 length, gint32 note_id);
  
  void ags_vst_event_set_sample_offset(AgsVstEvent *event,
				       gint32 sample_offset);

  const AgsVstTUID* ags_vst_ievent_list_get_iid();
  
  gint32 ags_vst_ievent_list_get_event_count(AgsVstIEventList *ievent_list);

  AgsVstTResult ags_vst_ievent_list_get_event(AgsVstIEventList *ievent_list,
					      gint32 index,
					      AgsVstEvent *e);
  AgsVstTResult ags_vst_ievent_list_add_event(AgsVstIEventList *ievent_list,
					      AgsVstEvent *e);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_IEVENTS_H__*/
