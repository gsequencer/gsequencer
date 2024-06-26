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

#ifndef __AGS_VST_IAUTOMATION_STATE_H__
#define __AGS_VST_IAUTOMATION_STATE_H__

#include <glib.h>

#include <ags/libags-vst.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_types.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct IAutomationState AgsVstIAutomationState;

  typedef enum
  {
    AGS_VST_KNO_AUTOMATION    = 0,
    AGS_VST_KREAD_STATE       = 1,
    AGS_VST_KWRITE_STATE      = 1 << 1,
    AGS_VST_KREAD_WRITE_STATE = AGS_VST_KREAD_STATE | AGS_VST_KWRITE_STATE,
  }AgsVstAutomationStates;

  const AgsVstTUID* ags_vst_iautomation_state_get_iid();

  AgsVstTResult ags_vst_iautomation_state_set_automation_state(AgsVstIAutomationState *iautomation_state,
							       gint32 state);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_IAUTOMATION_STATE_H__*/
