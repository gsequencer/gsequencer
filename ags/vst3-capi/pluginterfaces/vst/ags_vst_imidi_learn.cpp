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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_imidi_learn.h>

#include <pluginterfaces/vst/ivstmidilearn.h>

extern "C" {

  const AgsVstTUID* ags_vst_imidi_learn_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IMidiLearn::iid));
  }

  AgsVstTResult ags_vst_midi_learn_on_live_midi_controller_input(AgsVstIMidiLearn *imidi_learn,
								 gint32 bus_index, gint16 channel,
								 AgsVstCtrlNumber midi_cc)
  {
    return(((Steinberg::Vst::IMidiLearn *) imidi_learn)->onLiveMIDIControllerInput(bus_index, channel,
										   midi_cc));
  }

}
