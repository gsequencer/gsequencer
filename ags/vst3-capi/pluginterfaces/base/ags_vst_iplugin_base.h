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

#ifndef __AGS_VST_IPLUGIN_BASE_H__
#define __AGS_VST_IPLUGIN_BASE_H__

#include <glib.h>

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_fstrdefs.h>

#define AGS_VST_IPLUGIN_BASE_IID (ags_vst_iplugin_base_get_iid());
#define AGS_VST_IPLUGIN_FACTORY_IID (ags_vst_iplugin_factory_get_iid());
#define AGS_VST_IPLUGIN_FACTORY2_IID (ags_vst_iplugin_factory2_get_iid());
#define AGS_VST_IPLUGIN_FACTORY3_IID (ags_vst_iplugin_factory3_get_iid());

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct AgsVstIPluginBase IPluginBase;

  AgsVstTUID* ags_vst_iplugin_base_get_iid();

  typedef struct AgsVstPFactoryInfo PFactoryInfo;
  typedef struct AgsVstPClassInfo PClassInfo;

  typedef struct AgsVstIPluginFactory IPluginFactory;

  AgsVstTUID* ags_vst_iplugin_factory_get_iid();

  typedef struct AgsVstPClassInfo2 PClassInfo2;
  typedef struct AgsVstIPluginFactory2 IPluginFactory2;

  AgsVstTUID* ags_vst_iplugin_factory2_get_iid();

  typedef struct AgsVstPClassInfoW PClassInfoW;

  typedef struct AgsVstIPluginFactory3 IPluginFactory3;

  AgsVstTUID* ags_vst_iplugin_factory3_get_iid();

  AgsVstIPluginFactory*  ags_vst_get_plugin_factory();

  typedef AgsVstIPluginFactory* (*AgsVstGetFactoryProc)();

#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_IPLUGIN_BASE_H__*/
