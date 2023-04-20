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

#include <ags/vst3-capi/public.sdk/source/vst/ags_vst_component_base.h>

#include <public.sdk/source/vst/vstcomponentbase.h>

#include <string.h>

extern "C" {

  /**
   * Instantiate new Steinberg::Vst::ComponentBase the constructor as a C99
   * compatible handle a void pointer.
   * 
   * @return the AgsVstComponentBase
   * 
   * @since 5.0.0
   */
  AgsVstComponentBase* ags_vst_component_base_new()
  {
    return((AgsVstComponentBase*) new Steinberg::Vst::ComponentBase());
  }
  
  /**
   * Delete.
   *
   * @param component_base the component base
   *
   * @since 5.0.0
   */
  void ags_vst_component_base_delete(AgsVstComponentBase *component_base)
  {
    delete component_base;
  }

  /**
   * Get host context.
   *
   * @param component_base the component base
   * @return the host context
   *
   * @since 5.0.0
   */
  AgsVstFUnknown* ags_vst_component_base_get_host_context(AgsVstComponentBase *component_base)
  {
    return((AgsVstFUnknown*) static_cast<Steinberg::Vst::ComponentBase*>((void*) component_base)->getHostContext());
  }

  /**
   * Get peer.
   *
   * @param component_base the component base
   * @return the connection point
   *
   * @since 5.0.0
   */
  AgsVstIConnectionPoint* ags_vst_component_base_get_peer(AgsVstComponentBase *component_base)
  {
    return((AgsVstIConnectionPoint*) static_cast<Steinberg::Vst::ComponentBase*>((void*) component_base)->getPeer());
  }

  /**
   * Allocate message.
   *
   * @param component_base the component base
   * @return the imessage
   *
   * @since 5.0.0
   */
  AgsVstIMessage* ags_vst_component_base_allocate_message(AgsVstComponentBase *component_base)
  {
    return((AgsVstIMessage*) static_cast<Steinberg::Vst::ComponentBase*>((void*) component_base)->allocateMessage());
  }

  /**
   * Send message.
   *
   * @param component_base the component base
   * @param message the imessage
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_component_base_send_message(AgsVstComponentBase *component_base,
						    AgsVstIMessage* message)
  {
    return(static_cast<Steinberg::Vst::ComponentBase*>((void*) component_base)->sendMessage(static_cast<Steinberg::Vst::IMessage*>((void*) message)));
  }

  /**
   * Send text message.
   *
   * @param component_base the component base
   * @param text the text
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_component_base_send_text_message(AgsVstComponentBase *component_base,
							 gchar *text)
  {
    return(static_cast<Steinberg::Vst::ComponentBase*>((void*) component_base)->sendTextMessage(static_cast<Steinberg::char8*>(text)));
  }

  /**
   * Receive text.
   *
   * @param component_base the component base
   * @param text the text
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_component_base_receive_text(AgsVstComponentBase *component_base,
						    gchar *text)
  {
    return(static_cast<Steinberg::Vst::ComponentBase*>((void*) component_base)->receiveText(static_cast<Steinberg::char8*>(text)));
  }

  /**
   * Base initialize.
   *
   * @param component_base the component base
   * @param context the context
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_component_base_initialize(AgsVstComponentBase *component_base,
						  AgsVstFUnknown* context)
  {
    return(static_cast<Steinberg::Vst::ComponentBase*>((void*) component_base)->initialize(static_cast<Steinberg::FUnknown*>((void*) context)));
  }

  /**
   * Base terminate.
   *
   * @param component_base the component base
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_component_base_terminate(AgsVstComponentBase *component_base)
  {
    return(static_cast<Steinberg::Vst::ComponentBase*>((void*) component_base)->terminate());
  }

  /**
   * Base connect.
   *
   * @param component_base the component base
   * @param other the connection point
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_component_base_connect(AgsVstComponentBase *component_base,
					       AgsVstIConnectionPoint* other)
  {
    return(static_cast<Steinberg::Vst::ComponentBase*>((void*) component_base)->connect(static_cast<Steinberg::Vst::IConnectionPoint*>((void*) other)));
  }

  /**
   * Base disconnect.
   *
   * @param component_base the component base
   * @param other the connection point
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_component_base_disconnect(AgsVstComponentBase *component_base,
						  AgsVstIConnectionPoint* other)
  {
    return(static_cast<Steinberg::Vst::ComponentBase*>((void*) component_base)->disconnect(static_cast<Steinberg::Vst::IConnectionPoint*>((void*) other)));
  }

  /**
   * Base notify.
   *
   * @param component_base the component base
   * @param message the message
   * @return the result code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_component_base_notify(AgsVstComponentBase *component_base,
					      AgsVstIMessage* message)
  {
    return(static_cast<Steinberg::Vst::ComponentBase*>((void*) component_base)->notify(static_cast<Steinberg::Vst::IMessage*>((void*) message)));
  }

}
