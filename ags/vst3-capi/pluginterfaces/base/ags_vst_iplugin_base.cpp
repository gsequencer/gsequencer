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

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_iplugin_base.h>

#include <pluginterfaces/base/ipluginbase.h>

using namespace Steinberg;

extern "C" {

  const AgsVstTUID*
  ags_vst_iplugin_base_get_iid()
  {
    return(reinterpret_cast<const AgsVstTUID*>(&INLINE_UID_OF(Steinberg::IPluginBase)));
  }

  const AgsVstTUID*
  ags_vst_iplugin_factory_get_iid()
  {
    return(reinterpret_cast<const AgsVstTUID*>(&INLINE_UID_OF(Steinberg::IPluginFactory)));
  }

  const AgsVstTUID* ags_vst_iplugin_factory2_get_iid()
  {
    return(reinterpret_cast<const AgsVstTUID*>(&INLINE_UID_OF(Steinberg::IPluginFactory2)));
  }

  const AgsVstTUID* ags_vst_iplugin_factory3_get_iid()
  {
    return(reinterpret_cast<const AgsVstTUID*>(&INLINE_UID_OF(Steinberg::IPluginFactory3)));
  }

  AgsVstIPluginFactory*  ags_vst_get_plugin_factory()
  {
    return((AgsVstIPluginFactory *) GetPluginFactory());
  }

}
