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

#include <public.sdk/source/vst/hosting/module.h>

extern "C" {

  void ags_vst_plugin_factory_set_host_context(AgsVstPluginFactory *plugin_factory,
					       AgsVstHostContext *host_context)
  {
    ((Vst3::Hosting::PluginFactory *) plugin_factory)->setHostContext((Vst3::Hosting::HostContext *) host_context);
  }

  gpointer ags_vst_plugin_factory_create_instance(AgsVstPluginFactory *plugin_factory,
						  AgsVstUID *class_id)
  {
    return(((Vst3::Hosting::PluginFactory *) plugin_factory)->createInstance(const_cast<Vst3::UID&>((Vst3::UID *) class_id)[0]));
  }

  AgsVstEventList* ags_vst_module_new()
  {
    Vst3::Hosting::Module *module = new Vst3::Hosting::Module();
    
    return((AgsVstModule *) module);
  }

  AgsVstPluginFactory* ags_vst_module_get_factory(AgsVstModule *module)
  {
    return(((Vst3::Hosting::Module *) module)->getFactory());
  }

  gboolean ags_vst_module_load(AgsVstModule *module, gchar *path, gchar *error_description)
  {
    return(((Vst3::Hosting::Module *) module)->load(path, error_description));
  }

}
