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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_ievents.h>

#include <pluginterfaces/vst/ivstevents.h>

extern "C" {

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

  const AgsVstTUID* ags_vst_ievent_list_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IEventList::iid.toTUID()));
  }
  
  gint32 ags_vst_ievent_list_get_event_count(AgsVstIEventList *ievent_list)
  {
    return(((Steinberg::Vst::IEventList *) ievent_list)->getEventCount());
  }

  AgsVstTResult ags_vst_ievent_list_get_event(AgsVstIEventList *ievent_list,
					      gint32 index,
					      AgsVstEvent *e)
  {
    Steinberg::Vst::Event *tmp_e_0 = (Steinberg::Vst::Event *) e;
    const Steinberg::Vst::Event& tmp_e_1 = const_cast<Steinberg::Vst::Event&>(tmp_e_0[0]);

    return(((Steinberg::Vst::IEventList *) ievent_list)->getEvent(index, const_cast<Steinberg::Vst::Event&>(tmp_e_1)));
  }

  AgsVstTResult ags_vst_ievent_list_add_event(AgsVstIEventList *ievent_list,
					      AgsVstEvent *e)
  {
    Steinberg::Vst::Event *tmp_e_0 = (Steinberg::Vst::Event *) e;
    const Steinberg::Vst::Event& tmp_e_1 = const_cast<Steinberg::Vst::Event&>(tmp_e_0[0]);

    return(((Steinberg::Vst::IEventList *) ievent_list)->addEvent(const_cast<Steinberg::Vst::Event&>(tmp_e_1)));
  }

}
