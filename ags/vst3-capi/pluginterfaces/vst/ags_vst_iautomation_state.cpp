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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iautomation_state.h>

#include <ags/vst3-capi/ags_vst_headers.h>

extern "C" {
  
  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_iautomation_state_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::Vst::IAutomationState::iid.toTUID()));
  }

  /**
   * Set automation state.
   *
   * @param iautomation_state the automation state
   * @param state the state
   * @return the return value
   * 
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iautomation_state_set_automation_state(AgsVstIAutomationState *iautomation_state,
							       gint32 state)
  {
    return(((Steinberg::Vst::IAutomationState *) iautomation_state)->setAutomationState(state));
  }
  
}
