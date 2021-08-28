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

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_iupdate_handler.h>

#include <pluginterfaces/base/iupdatehandler.h>

extern "C" {

  const AgsVstTUID* ags_vst_iupdate_handler_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::IUpdateHandler::iid.toTUID()));
  }
  
  AgsVstTResult ags_vst_iupdate_handler_add_dependent(AgsVstIUpdateHandler *iupdate_handler, AgsVstFUnknown *funknown, AgsVstIDependent *idependent)
  {
    return(((Steinberg::IUpdateHandler *) iupdate_handler)->addDependent((Steinberg::FUnknown*) funknown, (Steinberg::IDependent*) idependent));
  }
  
  AgsVstTResult ags_vst_iupdate_handler_remove_dependent(AgsVstIUpdateHandler *iupdate_handler, AgsVstFUnknown *funknown, AgsVstIDependent *idependent)
  {
    return(((Steinberg::IUpdateHandler *) iupdate_handler)->removeDependent((Steinberg::FUnknown*) funknown, (Steinberg::IDependent*) idependent));
  }
  
  AgsVstTResult ags_vst_iupdate_handler_trigger_updates(AgsVstIUpdateHandler *iupdate_handler, AgsVstFUnknown *funknown, gint32 message)
  {
    return(((Steinberg::IUpdateHandler *) iupdate_handler)->triggerUpdates((Steinberg::FUnknown*) funknown, message));
  }
  
  AgsVstTResult ags_vst_iupdate_handler_defer_updates(AgsVstIUpdateHandler *iupdate_handler, AgsVstFUnknown *funknown, gint32 message)
  {
    return(((Steinberg::IUpdateHandler *) iupdate_handler)->deferUpdates((Steinberg::FUnknown*) funknown, message));
  }
  
  const AgsVstTUID* ags_vst_idependent_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::IDependent::iid.toTUID()));
  }
  
  void ags_vst_idependent_update(AgsVstIDependent *idependent, AgsVstFUnknown *changed_funknown, gint32 message)
  {
    ((Steinberg::IDependent *) idependent)->update((Steinberg::FUnknown*) changed_funknown, message);
  }
}
