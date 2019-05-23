/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/vst3-capi/public.sdk/source/vst/ags_vst_component_base.h>

#include <public.sdk/source/vst/vstcomponentbase.h>

#include <string.h>

extern "C" {

  AgsVstComponentBase* ags_vst_component_base_new()
  {
    return(new ComponentBase());
  }
  
  void ags_vst_component_base_delete(AgsVstComponentBase *component_base)
  {
    delete component_base;
  }

  AgsVstFUnknown* ags_vst_component_base_get_host_context(AgsVstComponentBase *component_base)
  {
    return(component_base->getHostContext());
  }

  AgsVstIConnectionPoint* ags_vst_component_base_get_peer(AgsVstComponentBase *component_base)
  {
    return(component_base->getPeer());
  }

  AgsVstIMessage* ags_vst_component_base_allocate_message(AgsVstComponentBase *component_base)
  {
    return(component_base->allocateMessage());
  }

  tresult ags_vst_component_base_send_message(AgsVstComponentBase *component_base,
					      AgsVstIMessage* message)
  {
    return(component_base->sendMessage(message));
  }

  tresult ags_vst_component_base_send_text_message(AgsVstComponentBase *component_base,
						   gchar *text)
  {
    return(component_base->sendTextMessage(static_cast<char8*>(text)));
  }

  tresult ags_vst_component_base_receive_text(AgsVstComponentBase *component_base,
					      gchar *text)
  {
    return(component_base->receiveText(static_cast<char8*>(text)));
  }

  tresult PLUGIN_API ags_vst_component_base_initialize(AgsVstComponentBase *component_base,
						       AgsVstFUnknown* context)
  {
    return(component_base->initialize(context));
  }

  tresult PLUGIN_API ags_vst_component_base_terminate(AgsVstComponentBase *component_base)
  {
    return(component_base->terminate());
  }

  tresult PLUGIN_API ags_vst_component_base_connect(AgsVstComponentBase *component_base,
						    AgsVstIConnectionPoint* other)
  {
    return(component_base->connect(other));
  }

  tresult PLUGIN_API ags_vst_component_base_disconnect(AgsVstComponentBase *component_base,
						       AgsVstIConnectionPoint* other)
  {
    return(component_base->disconnect(other));
  }

  tresult PLUGIN_API ags_vst_component_base_notify(AgsVstComponentBase *component_base,
						   AgsVstIMessage* message)
  {
    return(component_base->notify(message));
  }

}
