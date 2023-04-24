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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_ievents.h>

#include <pluginterfaces/vst/ivstevents.h>

extern "C" {

  /**
   * Allocate note-on event.
   *
   * @param channel the MIDI channel
   * @param pitch the pitch
   * @param tuning the tuning
   * @param velocity the MIDI velocity
   * @param length the length
   * @param note_id the note identifier
   * @return the AgsVstEvent
   *
   * @since 5.0.0
   */
  AgsVstEvent* ags_vst_note_on_event_alloc(gint channel, gint pitch, gfloat tuning, gfloat velocity, gint32 length, gint32 note_id)
  {
    Steinberg::Vst::Event *event = new Steinberg::Vst::Event();

    event->busIndex = 0;
    event->sampleOffset = 0;    
    event->ppqPosition = 0;    
    
    event->flags = 1;
    
    event->type = Steinberg::Vst::Event::EventTypes::kNoteOnEvent;
    
    event->noteOn.channel = channel;
    event->noteOn.pitch = pitch;
    event->noteOn.tuning = tuning;
    event->noteOn.velocity = velocity;
    event->noteOn.length = length;
    event->noteOn.noteId = note_id;

    return((AgsVstEvent *) event);
  }
  
  /**
   * Allocate note-off event.
   *
   * @param channel the MIDI channel
   * @param pitch the pitch
   * @param tuning the tuning
   * @param velocity the MIDI velocity
   * @param length the length
   * @param note_id the note identifier
   * @return the AgsVstEvent
   *
   * @since 5.0.0
   */
  AgsVstEvent* ags_vst_note_off_event_alloc(gint channel, gint pitch, gfloat tuning, gfloat velocity, gint32 length, gint32 note_id)
  {
    Steinberg::Vst::Event *event = new Steinberg::Vst::Event();

    event->busIndex = 0;
    event->sampleOffset = 0;    
    event->ppqPosition = 0;    

    event->flags = 1;
    
    event->type = Steinberg::Vst::Event::EventTypes::kNoteOffEvent;
    
    event->noteOff.channel = channel;
    event->noteOff.pitch = pitch;
    event->noteOff.velocity = velocity;
    event->noteOff.noteId = note_id;
    event->noteOff.tuning = tuning;

    return((AgsVstEvent *) event);
  }

  /**
   * Set sample offset.
   * 
   * @param event the event
   * @param sample_offset sample offset
   *
   * @since 5.0.0
   */
  void ags_vst_event_set_sample_offset(AgsVstEvent *event,
				       gint32 sample_offset)
  {
    ((Steinberg::Vst::Event *) event)->sampleOffset = sample_offset;
  }

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_ievent_list_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IEventList::iid.toTUID()));
  }

  /**
   * Get event count.
   *
   * @param ievent_list the event list
   * @return the event count
   *
   * @since 5.0.0
   */
  gint32 ags_vst_ievent_list_get_event_count(AgsVstIEventList *ievent_list)
  {
    return(((Steinberg::Vst::IEventList *) ievent_list)->getEventCount());
  }

  /**
   * Get event.
   *
   * @param ievent_list the event list
   * @param index the index
   * @param e the event
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_ievent_list_get_event(AgsVstIEventList *ievent_list,
					      gint32 index,
					      AgsVstEvent *e)
  {
    Steinberg::Vst::Event *tmp_e_0 = (Steinberg::Vst::Event *) e;
    const Steinberg::Vst::Event& tmp_e_1 = const_cast<Steinberg::Vst::Event&>(tmp_e_0[0]);

    return(((Steinberg::Vst::IEventList *) ievent_list)->getEvent(index, const_cast<Steinberg::Vst::Event&>(tmp_e_1)));
  }

  /**
   * Add event.
   *
   * @param ievent_list the event list
   * @param e the event
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_ievent_list_add_event(AgsVstIEventList *ievent_list,
					      AgsVstEvent *e)
  {
    Steinberg::Vst::Event *tmp_e_0 = (Steinberg::Vst::Event *) e;
    const Steinberg::Vst::Event& tmp_e_1 = const_cast<Steinberg::Vst::Event&>(tmp_e_0[0]);

    return(((Steinberg::Vst::IEventList *) ievent_list)->addEvent(const_cast<Steinberg::Vst::Event&>(tmp_e_1)));
  }

}
