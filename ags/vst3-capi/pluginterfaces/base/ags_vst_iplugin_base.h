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

#ifndef __AGS_VST_IPLUGIN_BASE_H__
#define __AGS_VST_IPLUGIN_BASE_H__

#include <glib.h>

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.h>
#include <ags/vst3-capi/pluginterfaces/base/ags_vst_fstrdefs.h>

#define AGS_VST_IPLUGIN_BASE_IID (ags_vst_iplugin_base_get_iid())
#define AGS_VST_IPLUGIN_FACTORY_IID (ags_vst_iplugin_factory_get_iid())
#define AGS_VST_IPLUGIN_FACTORY2_IID (ags_vst_iplugin_factory2_get_iid())
#define AGS_VST_IPLUGIN_FACTORY3_IID (ags_vst_iplugin_factory3_get_iid())

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct IPluginBase AgsVstIPluginBase;

  typedef struct PFactoryInfo AgsVstPFactoryInfo;
  typedef struct PClassInfo AgsVstPClassInfo;

  typedef struct IPluginFactory AgsVstIPluginFactory;

  typedef struct PClassInfo2 AgsVstPClassInfo2;
  typedef struct IPluginFactory2 AgsVstIPluginFactory2;

  typedef struct PClassInfoW AgsVstPClassInfoW;

  typedef struct IPluginFactory3 AgsVstIPluginFactory3;

  typedef AgsVstIPluginFactory* (*AgsVstGetFactoryProc)();

  typedef enum {
    AGS_VST_KNOFLAGS                        = 0,
    AGS_VST_KCLASSES_DISCARDABLE            = 1 << 0,
    AGS_VST_KLICENSE_CHECK                  = 1 << 1,
    AGS_VST_KCOMPONENT_NON_DISCARDABLE      = 1 << 3,
    AGS_VST_KUNICODE                        = 1 << 4
  }AgsVstFactoryFlags;
  
  enum {
    AGS_VST_KURL_SIZE   = 256,
    AGS_VST_KEMAIL_SIZE = 128,
    AGS_VST_KNAME_SIZE  = 64
  };
  
  const AgsVstTUID* ags_vst_iplugin_base_get_iid();

  AgsVstTResult ags_vst_iplugin_base_initialize(AgsVstIPluginBase *iplugin_base, AgsVstFUnknown *funknown);

  AgsVstTResult ags_vst_iplugin_base_terminate(AgsVstIPluginBase *iplugin_base);

  const AgsVstTUID* ags_vst_iplugin_factory_get_iid();

  AgsVstTResult ags_vst_iplugin_factory_get_factory_info(AgsVstIPluginFactory *iplugin_factory,
							 AgsVstPFactoryInfo *pfactory_info);

  gint32 ags_vst_iplugin_factory_count_classes(AgsVstIPluginFactory *iplugin_factory);

  AgsVstTResult ags_vst_iplugin_factory_get_class_info(AgsVstIPluginFactory *iplugin_factory,
						       gint32 index, AgsVstPClassInfo *info);

  AgsVstTResult ags_vst_iplugin_factory_create_instance(AgsVstIPluginFactory *iplugin_factory,
							AgsVstFIDString *cid, AgsVstFIDString *_iid, void **obj);
  const AgsVstTUID* ags_vst_iplugin_factory2_get_iid();

  AgsVstTResult ags_vst_iplugin_factory2_get_class_info2(AgsVstIPluginFactory2 *iplugin_factory2,
							 gint32 index, AgsVstPClassInfo *info);

  const AgsVstTUID* ags_vst_iplugin_factory3_get_iid();

  AgsVstTResult ags_vst_iplugin_factory3_get_class_info_unicode(AgsVstIPluginFactory3 *iplugin_factory3,
								gint32 index, AgsVstPClassInfoW *info);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_IPLUGIN_BASE_H__*/
