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

#include <ags/vst3-capi/public.sdk/source/vst/hosting/ags_vst_module.h>

#include <public.sdk/source/vst/hosting/hostclasses.h>
#include <public.sdk/source/vst/hosting/module.h>

extern "C" {

  void ags_vst_plugin_factory_set_host_context(AgsVstPluginFactory *plugin_factory,
					       AgsVstHostContext *host_context)
  {
    ((VST3::Hosting::PluginFactory *) plugin_factory)->setHostContext((Steinberg::FUnknown *) host_context);
  }

  //FIXME:JK: ???
#if 0
  gpointer ags_vst_plugin_factory_create_instance(AgsVstPluginFactory *plugin_factory,
						  AgsVstUID *class_id)
  {
    return((gpointer) ((VST3::Hosting::PluginFactory *) plugin_factory)->createInstance(const_cast<VST3::UID&>(((VST3::UID *) class_id)[0])));
  }
#endif
  
  //FIXME:JK: ???
#if 0
  AgsVstModule* ags_vst_module_new(gchar *path, gchar *error_description)
  {
    std::string *tmp_path = new std::string(path);
    std::string *tmp_error_description = new std::string(error_description);
    
    VST3::Hosting::Module::Ptr module = VST3::Hosting::Module::create(const_cast<std::string&>(tmp_path[0]), const_cast<std::string&>(tmp_error_description[0]));
    
    return((AgsVstModule *) &module);
  }
#endif
  
  AgsVstPluginFactory* ags_vst_module_get_factory(AgsVstModule *module)
  {
    return((AgsVstPluginFactory *) const_cast<VST3::Hosting::PluginFactory *>(&(((VST3::Hosting::Module *) module)->getFactory())));
  }

}
