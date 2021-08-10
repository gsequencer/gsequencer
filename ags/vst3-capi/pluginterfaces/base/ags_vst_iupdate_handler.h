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

#ifndef __AGS_VST_IUPDATE_HANDLER_H__
#define __AGS_VST_IUPDATE_HANDLER_H__

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.h>

#define AGS_VST_IUPDATE_HANDLER_IID (ags_vst_iupdate_handler_get_iid())
#define AGS_VST_IDEPENDENT_IID (ags_vst_idependent_get_iid())

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct AgsVstIUpdateHandler IUpdateHandler;
  typedef struct AgsVstIDependent IDependent;

  typedef enum{
    AGS_VST_KWILL_CHANGE,
    AGS_VST_KCHANGED,
    AGS_VST_KDESTROYED,
    AGS_VST_KWILL_DESTROY,
    AGS_VST_KSTD_CHANGE_MESSAGE_LAST = AGS_VST_KWILL_DESTROY
  }AgsVstChangeMessage;
  
  const AgsVstTUID* ags_vst_iupdate_handler_get_iid();

  AgsVstTResult ags_vst_iupdate_handler_add_dependent(AgsVstIUpdateHandler *iupdate_handler, AgsVstFUnknown *funknown, AgsVstIDependent *idependent);
  AgsVstTResult ags_vst_iupdate_handler_remove_dependent(AgsVstIUpdateHandler *iupdate_handler, AgsVstFUnknown *funknown, AgsVstIDependent *idependent);

  AgsVstTResult ags_vst_iupdate_handler_trigger_updates(AgsVstIUpdateHandler *iupdate_handler, AgsVstFUnknown *funknown, gint32 message);
  AgsVstTResult ags_vst_iupdate_handler_defer_updates(AgsVstIUpdateHandler *iupdate_handler, AgsVstFUnknown *funknown, gint32 message);
  
  const AgsVstTUID* ags_vst_idependent_get_iid();

  void ags_vst_idependent_update(AgsVstIDependent *idependent, AgsVstFUnknown *changed_funknown, gint32 message);

#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_IUPDATE_HANDLER_H__*/
