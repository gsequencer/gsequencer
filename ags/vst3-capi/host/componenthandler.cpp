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

#include <ags/vst3-capi/host/componenthandler.h>

#include <ags/libags-vst.h>

using namespace Ags;
using namespace Ags::VstCAPI;
using namespace Steinberg::Vst;

namespace Ags {

  namespace VstCAPI {

    /**
     * Instantiate a new Ags::VstCAPI::ComponentHandler.
     * 
     * @since 5.0.0
     */
    ComponentHandler::ComponentHandler()
    {
      this->handler = nullptr;

      this->handlerCount = 0;
      this->handlerIDCount = 0;
    }

    /**
     * Begin edit component handler.
     *
     * @param id the identifier
     * @return result code of action
     * 
     * @since 5.0.0
     */
    Steinberg::tresult PLUGIN_API ComponentHandler::beginEdit(Steinberg::Vst::ParamID id)
    {
      void *component_handler;
      
      int i;

      component_handler = (void *) this;
      
      this->componentHandlerMutex.lock();
      
      for(i = 0; i < this->handlerCount; i++){
	if(!strncmp(this->handler[i].event_name, "beginEdit", 10)){
	  void *data;

	  data = (void *) this->handler[i].data;
	  
	  ((ComponentHandlerBeginEdit) (this->handler[i].callback))((AgsVstIComponentHandler *) component_handler, id, data);
	}
      }
      
      this->componentHandlerMutex.unlock();

      return(0);
    }

    /**
     * Perform edit component handler.
     *
     * @param id the identifier
     * @param valueNormalized the normalized value
     * @return result code of action
     * 
     * @since 5.0.0
     */
    Steinberg::tresult PLUGIN_API ComponentHandler::performEdit(Steinberg::Vst::ParamID id, Steinberg::Vst::ParamValue valueNormalized)
    {
      void *component_handler;

      int i;

      component_handler = (void *) this;
      
      this->componentHandlerMutex.lock();

      for(i = 0; i < this->handlerCount; i++){
	if(!strncmp(this->handler[i].event_name, "performEdit", 12)){
	  void *data;

	  data = (void *) this->handler[i].data;
	  
	  ((ComponentHandlerPerformEdit) (this->handler[i].callback))((AgsVstIComponentHandler *) component_handler, id, valueNormalized, data);
	}
      }

      this->componentHandlerMutex.unlock();

      return(0);
    }

    /**
     * End edit component handler.
     *
     * @param id the identifier
     * @return result code of action
     * 
     * @since 5.0.0
     */
    Steinberg::tresult PLUGIN_API ComponentHandler::endEdit(Steinberg::Vst::ParamID id)
    {
      void *component_handler;

      int i;

      component_handler = (void *) this;
      
      this->componentHandlerMutex.lock();
      
      for(i = 0; i < this->handlerCount; i++){
	if(!strncmp(this->handler[i].event_name, "endEdit", 8)){
	  void *data;

	  data = (void *) this->handler[i].data;
	  
	  ((ComponentHandlerEndEdit) (this->handler[i].callback))((AgsVstIComponentHandler *) component_handler, id, data);
	}
      }

      this->componentHandlerMutex.unlock();

      return(0);
    }

    /**
     * Restart component.
     *
     * @param flags flags
     * @return result code of action
     * 
     * @since 5.0.0
     */
    Steinberg::tresult PLUGIN_API ComponentHandler::restartComponent(Steinberg::int32 flags)
    {
      void *component_handler;

      int i;
      
      component_handler = (void *) this;
      
      this->componentHandlerMutex.lock();

      for(i = 0; i < this->handlerCount; i++){
	if(!strncmp(this->handler[i].event_name, "restartComponent", 18)){
	  void *data;

	  data = (void *) this->handler[i].data;
	  
	  ((ComponentHandlerRestartComponent) (this->handler[i].callback))((AgsVstIComponentHandler *) component_handler, flags, data);
	}
      }

      this->componentHandlerMutex.unlock();

      return(0);
    }

    /**
     * Connect handler with event name to callback passing data.
     *
     * @param event_name the event name
     * @param callback the callback to invoke
     * @param data the data to pass to callback
     * @return the handler identifier
     * 
     * @since 5.0.0
     */
    int ComponentHandler::connectHandler(char *event_name, void *callback, void *data)
    {
      EventHandler *handler;
      int handlerID;
      
      handlerID = -1;
      
      this->componentHandlerMutex.lock();

      handlerID = this->handlerIDCount;

      if(!strncmp(event_name, "beginEdit", 10)){
      }else if(!strncmp(event_name, "performEdit", 12)){
      }else if(!strncmp(event_name, "endEdit", 8)){
      }else if(!strncmp(event_name, "restartComponent", 18)){
      }else{
	this->componentHandlerMutex.unlock();
	
	g_critical("unknown event name of callback");

	return(-1);
      }

      this->handler = (EventHandler *) realloc(this->handler,
					       (this->handlerCount + 1) * sizeof(EventHandler));

      this->handler[this->handlerCount].event_name = strdup(event_name);
      this->handler[this->handlerCount].callback = callback;      
      this->handler[this->handlerCount].data = data;

      this->handler[this->handlerCount].handler_id = this->handlerIDCount;
      
      this->handlerCount++;
      this->handlerIDCount++;
      
      this->componentHandlerMutex.unlock();      
      
      return(handlerID);
    }

    /**
     * Disconnect handler by handler ID.
     *
     * @param handler_id the handler identifier
     * 
     * @since 5.0.0
     */
    void ComponentHandler::disconnectHandler(int handler_id)
    {
      int position;
      int i;

      position = -1;
      
      this->componentHandlerMutex.lock();

      for(i = 0; i < this->handlerCount; i++){
	if(this->handler[i].handler_id == handler_id){
	  position = i;
	  
	  break;
	}
      }

      if(position >= 0){
	EventHandler *handler;

	handler = NULL;
	handler = (EventHandler *) realloc(handler,
					   (this->handlerCount - 1) * sizeof(EventHandler));

	if(position > 0){
	  memcpy(handler, this->handler, (position - 1) * sizeof(EventHandler));
	}

	if(position < this->handlerCount - 1){
	  memcpy(handler + position, this->handler + position + 1, (this->handlerCount - position - 1) * sizeof(EventHandler));
	}

	free(this->handler);
	
	this->handler = handler;
	
	this->handlerCount--;      
      }
      
      this->componentHandlerMutex.unlock();
    }
    
    /**
     * Query interface of component handler.
     *
     * @param _iid the iid to query
     * @param obj return location of interface
     * @return result code of action
     *
     * @since 5.0.0
     */
    Steinberg::tresult PLUGIN_API ComponentHandler::queryInterface (const char* _iid, void** obj)
    {
      QUERY_INTERFACE (_iid, obj, FUnknown::iid, IComponentHandler)
	QUERY_INTERFACE (_iid, obj, IComponentHandler::iid, IComponentHandler)

	if(mPlugInterfaceSupport && mPlugInterfaceSupport->queryInterface (iid, obj) == Steinberg::kResultTrue){
	  return Steinberg::kResultOk;
	}

      *obj = nullptr;

      return Steinberg::kResultFalse;
    }

    /**
     * Add reference count.
     *
     * @return 1
     *
     * @since 5.0.0
     */
    Steinberg::uint32 PLUGIN_API ComponentHandler::addRef ()
    {
      return 1;
    }

    /**
     * Release reference count.
     *
     * @return 1
     *
     * @since 5.0.0
     */
    Steinberg::uint32 PLUGIN_API ComponentHandler::release ()
    {
      return 1;
    }
  }
  
}
