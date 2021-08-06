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
