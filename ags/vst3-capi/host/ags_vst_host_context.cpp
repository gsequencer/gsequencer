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

#include <ags/vst3-capi/host/ags_vst_host_context.h>
#include <ags/vst3-capi/host/hostcontext.h>

extern "C" {

  Steinberg::Vst::HostApplication *host_context = NULL;

  AgsVstHostContext* ags_vst_host_context_get_instance()
  {
    if(host_context == NULL){
      host_context = new Steinberg::Vst::HostApplication();
    }
    
    return((AgsVstHostContext *) host_context);
  }

  AgsVstHostContext* ags_vst_host_context_new()
  {
    return((AgsVstHostContext *) new Steinberg::Vst::HostApplication());
  }

}
