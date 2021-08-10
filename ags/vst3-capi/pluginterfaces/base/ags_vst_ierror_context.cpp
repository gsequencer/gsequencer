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

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ierror_context.h>

#include <pluginterfaces/base/ierrorcontext.h>

using namespace Steinberg;

extern "C" {

  const AgsVstTUID*
  ags_vst_ierror_context_get_iid()
  {
    return(reinterpret_cast<const AgsVstTUID*>(&INLINE_UID_OF(Steinberg::IErrorContext)));
  }

  void ags_vst_ierror_context_disable_error_ui(AgsVstIErrorContext *error_context,
					       gboolean state)
  {
    ((Steinberg::IErrorContext *) error_context)->disableErrorUI(state);
  }

  AgsVstTResult ags_vst_ierror_context_error_message_shown(AgsVstIErrorContext *error_context)
  {
    return(((Steinberg::IErrorContext *) error_context)->errorMessageShown());
  }
  
  AgsVstTResult ags_vst_ierror_context_get_error_message(AgsVstIErrorContext *error_context,
							 AgsVstIString *message)
  {
    return(((Steinberg::IErrorContext *) error_context)->getErrorMessage((Steinberg::IString *) message));
  }

}
