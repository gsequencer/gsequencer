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

#ifndef __AGS_VST_MODULE_H__
#define __AGS_VST_MODULE_H__

#include <glib.h>

#include <ags/vst3-capi/host/ags_vst_host_context.h>

#include <ags/vst3-capi/public.sdk/source/vst/utility/ags_vst_uid.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_iplugin_base.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct FactoryInfo AgsVstFactoryInfo;
  typedef struct ClassInfo AgsVstClassInfo;
  typedef struct PluginFactory AgsVstPluginFactory;
  typedef struct Module AgsVstModule;
  
  //TODO:JK: implement me

  void ags_vst_plugin_factory_set_host_context(AgsVstPluginFactory *plugin_factory,
					       AgsVstHostContext *host_context);
  
  AgsVstPluginFactory* ags_vst_module_get_factory(AgsVstModule *module);

#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_MODULE_H__*/
