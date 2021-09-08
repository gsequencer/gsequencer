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

#include <ags/vst3-capi/host/componenthandler.h>

#include <ags/libags-vst.h>

using namespace Ags;
using namespace Ags::VstCAPI;
using namespace Steinberg::Vst;

namespace Ags {

  namespace VstCAPI {

    ComponentHandler::ComponentHandler()
    {
      this->beginEditArr = NULL;
      this->beginEditData = NULL;
      this->beginEditHandler = NULL;
      this->beginEditDataCount = 0;
      
      this->performEditArr = NULL;
      this->performEditData = NULL;
      this->performEditHandler = NULL;
      this->performEditDataCount = 0;

      this->endEditArr = NULL;
      this->endEditData = NULL;
      this->endEditHandler = NULL;
      this->endEditDataCount = 0;

      this->restartComponentArr = NULL;
      this->restartComponentData = NULL;
      this->restartComponentHandler = NULL;
      this->restartComponentDataCount = 0;

      this->handlerCount = 0;
    }

    Steinberg::tresult PLUGIN_API ComponentHandler::beginEdit(Steinberg::Vst::ParamID id)
    {
      ComponentHandlerBeginEdit **beginEditIter;

      void *component_handler;
      
      int i;

      component_handler = (void *) this;
      
      this->componentHandlerMutex.lock();
      
      beginEditIter = (ComponentHandlerBeginEdit **) this->beginEditArr;

      for(i = 0; i < this->beginEditDataCount; i++){
	if(beginEditIter[i] != NULL){
	  void *data;

	  data = (void *) this->beginEditData[i];
	  
	  beginEditIter[i][0]((AgsVstIComponentHandler *) component_handler, id, data);
	}
      }
      
      this->componentHandlerMutex.unlock();

      return(0);
    }

    Steinberg::tresult PLUGIN_API ComponentHandler::performEdit(Steinberg::Vst::ParamID id, Steinberg::Vst::ParamValue valueNormalized)
    {
      ComponentHandlerPerformEdit **performEditIter;

      void *component_handler;

      int i;

      component_handler = (void *) this;
      
      this->componentHandlerMutex.lock();
      
      performEditIter = (ComponentHandlerPerformEdit **) this->performEditArr;

      for(i = 0; i < this->performEditDataCount; i++){
	if(performEditIter[i] != NULL){
	  void *data;

	  data = (void *) this->performEditData[i];
	  
	  performEditIter[i][0]((AgsVstIComponentHandler *) component_handler, id, valueNormalized, data);
	}
      }

      this->componentHandlerMutex.unlock();

      return(0);
    }

    Steinberg::tresult PLUGIN_API ComponentHandler::endEdit(Steinberg::Vst::ParamID id)
    {
      ComponentHandlerEndEdit **endEditIter;

      void *component_handler;

      int i;

      component_handler = (void *) this;
      
      this->componentHandlerMutex.lock();
      
      endEditIter = (ComponentHandlerEndEdit **) this->endEditArr;

      for(i = 0; i < this->endEditDataCount; i++){
	if(endEditIter[i] != NULL){
	  void *data;

	  data = (void *) this->endEditData[i];
	  
	  endEditIter[i][0]((AgsVstIComponentHandler *) component_handler, id, data);
	}
      }

      this->componentHandlerMutex.unlock();

      return(0);
    }

    Steinberg::tresult PLUGIN_API ComponentHandler::restartComponent(Steinberg::int32 flags)
    {
      ComponentHandlerRestartComponent **restartComponentIter;

      void *component_handler;

      int i;
      
      component_handler = (void *) this;
      
      this->componentHandlerMutex.lock();
      
      restartComponentIter = (ComponentHandlerRestartComponent **) this->restartComponentArr;

      for(i = 0; i < this->restartComponentDataCount; i++){
	if(restartComponentIter[i] != NULL){
	  void *data;

	  data = (void *) this->restartComponentData[i];
	  
	  restartComponentIter[i][0]((AgsVstIComponentHandler *) component_handler, flags, data);
	}
      }

      this->componentHandlerMutex.unlock();

      return(0);
    }

    int ComponentHandler::connectBeginEdit(void *beginEdit, void *data)
    {
      int handlerID;

      handlerID = -1;
      
      return(handlerID);
    }
      
    int ComponentHandler::connectPerformEdit(void *beginEdit, void *data)
    {
      int handlerID;

      handlerID = -1;
      
      return(handlerID);
    }

    int ComponentHandler::connectEndEdit(void *beginEdit, void *data)
    {
      int handlerID;

      handlerID = -1;
      
      return(handlerID);
    }

    int ComponentHandler::connectRestartComponent(void *beginEdit, void *data)
    {
      int handlerID;

      handlerID = -1;
      
      return(handlerID);
    }

    void ComponentHandler::disconnectHandler(int handler_id)
    {
    }
    
  }
  
}
