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

#pragma once

#include <pluginterfaces/base/conststringtable.h>
#include <pluginterfaces/base/funknown.h>
#include <pluginterfaces/vst/ivsteditcontroller.h>
#include <public.sdk/source/vst/hosting/pluginterfacesupport.h>

#include <libags-vst.h>

#include <mutex>

namespace Ags
{
  namespace VstCAPI {

    typedef AgsVstTResult (*ComponentHandlerBeginEdit)(AgsVstIComponentHandler *icomponent_handler, AgsVstParamID id, gpointer data);
    typedef AgsVstTResult (*ComponentHandlerPerformEdit)(AgsVstIComponentHandler *icomponent_handler, AgsVstParamID id, AgsVstParamValue value_normalized, gpointer data);
    typedef AgsVstTResult (*ComponentHandlerEndEdit)(AgsVstIComponentHandler *icomponent_handler, AgsVstParamID id, gpointer data);
    typedef AgsVstTResult (*ComponentHandlerRestartComponent)(AgsVstIComponentHandler *icomponent_handler, gint32 flags, gpointer data);

    struct EventHandler
    {
      char *event_name;
      
      void *callback;
      void *data;
      int handler_id;
    };
      
    class ComponentHandler : public Steinberg::Vst::IComponentHandler
    {
    public:
      ComponentHandler();
      
      virtual ~ComponentHandler () { FUNKNOWN_DTOR }
      
      Steinberg::tresult PLUGIN_API beginEdit(Steinberg::Vst::ParamID id) override;

      Steinberg::tresult PLUGIN_API performEdit(Steinberg::Vst::ParamID id, Steinberg::Vst::ParamValue valueNormalized) override;

      Steinberg::tresult PLUGIN_API endEdit(Steinberg::Vst::ParamID id) override;

      Steinberg::tresult PLUGIN_API restartComponent(Steinberg::int32 flags) override;

      DECLARE_FUNKNOWN_METHODS

      Steinberg::Vst::PlugInterfaceSupport* getPlugInterfaceSupport () const { return mPlugInterfaceSupport; }

      int connectHandler(char *event_name, void *callback, void *data);
      void disconnectHandler(int handler_id);

      EventHandler *handler;
      
      int handlerCount;
      int handlerIDCount;

      std::mutex componentHandlerMutex;

protected:
	Steinberg::IPtr<Steinberg::Vst::PlugInterfaceSupport> mPlugInterfaceSupport;
    };
    
  }
}
