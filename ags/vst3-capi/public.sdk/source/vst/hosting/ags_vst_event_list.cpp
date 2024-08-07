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

#include <ags/vst3-capi/public.sdk/source/vst/hosting/ags_vst_event_list.h>

#include <public.sdk/source/vst/hosting/eventlist.h>

extern "C" {

  /**
   * Instantiate new Steinberg::Vst::EventList the constructor as a C99
   * compatible handle a void pointer.
   * 
   * @return the AgsVstEventList
   * 
   * @since 5.0.0
   */
  AgsVstEventList* ags_vst_event_list_new()
  {
    Steinberg::Vst::EventList *event_list = new Steinberg::Vst::EventList();
    
    return((AgsVstEventList *) event_list);
  }

  /**
   * Clear.
   *
   * @param event_list the event list
   *
   * @since 5.0.0
   */
  void ags_vst_event_list_clear(AgsVstEventList *event_list)
  {
    ((Steinberg::Vst::EventList *) event_list)->clear();
  }
}
