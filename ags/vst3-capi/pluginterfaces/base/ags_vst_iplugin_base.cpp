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
    return((AgsVstTUID *) &(Steinberg::IPluginBase::iid.toTUID()));
  }

  AgsVstTResult ags_vst_iplugin_base_initialize(AgsVstIPluginBase *iplugin_base, AgsVstFUnknown *funknown)
  {
    return(((Steinberg::IPluginBase *) iplugin_base)->initialize((Steinberg::FUnknown *) funknown));
  }

  AgsVstTResult ags_vst_iplugin_base_terminate(AgsVstIPluginBase *iplugin_base)
  {
    return(((Steinberg::IPluginBase *) iplugin_base)->terminate());
  }

  AgsVstPClassInfo* ags_vst_pclass_info_alloc()
  {
    Steinberg::PClassInfo *info;

    info = new Steinberg::PClassInfo();

    return((AgsVstPClassInfo *) info);
  }

  void ags_vst_pclass_info_free(AgsVstPClassInfo *info)
  {
    delete info;
  }

  AgsVstTUID* ags_vst_pclass_info_get_cid(AgsVstPClassInfo *info)
  {
    return((AgsVstTUID *) &(((Steinberg::PClassInfo *) info)->cid));
  }
  
  gint32 ags_vst_pclass_info_get_cardinality(AgsVstPClassInfo *info)
  {
    return(((Steinberg::PClassInfo *) info)->cardinality);
  }
  
  gchar* ags_vst_pclass_info_get_category(AgsVstPClassInfo *info)
  {
    return(((Steinberg::PClassInfo *) info)->category);
  }
  
  gchar* ags_vst_pclass_info_get_name(AgsVstPClassInfo *info)
  {
    return(((Steinberg::PClassInfo *) info)->name);
  }

  const AgsVstTUID*
  ags_vst_iplugin_factory_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::IPluginFactory::iid.toTUID()));
  }

  AgsVstTResult ags_vst_iplugin_factory_get_factory_info(AgsVstIPluginFactory *iplugin_factory,
							 AgsVstPFactoryInfo *pfactory_info)
  {
    return(((Steinberg::IPluginFactory *) iplugin_factory)->getFactoryInfo((Steinberg::PFactoryInfo *) pfactory_info));
  }

  gint32 ags_vst_iplugin_factory_count_classes(AgsVstIPluginFactory *iplugin_factory)
  {
    return(((Steinberg::IPluginFactory *) iplugin_factory)->countClasses());
  }
  
  AgsVstTResult ags_vst_iplugin_factory_get_class_info(AgsVstIPluginFactory *iplugin_factory,
						       gint32 index, AgsVstPClassInfo *info)
  {
    return(((Steinberg::IPluginFactory *) iplugin_factory)->getClassInfo(index, (Steinberg::PClassInfo *) info));
  }

  AgsVstTResult ags_vst_iplugin_factory_create_instance(AgsVstIPluginFactory *iplugin_factory,
							AgsVstFIDString *cid, AgsVstFIDString *_iid, void **obj)
  {
    return(((Steinberg::IPluginFactory *) iplugin_factory)->createInstance((Steinberg::FIDString) cid, (Steinberg::FIDString) _iid, obj));
  }

  const AgsVstTUID* ags_vst_iplugin_factory2_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::IPluginFactory2::iid.toTUID()));
  }

  AgsVstTResult ags_vst_iplugin_factory2_get_class_info2(AgsVstIPluginFactory2 *iplugin_factory2,
							 gint32 index, AgsVstPClassInfo *info)
  {
    return(((Steinberg::IPluginFactory2 *) iplugin_factory2)->getClassInfo(index, (Steinberg::PClassInfo *) info));
  }

  const AgsVstTUID* ags_vst_iplugin_factory3_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::IPluginFactory3::iid.toTUID()));
  }

  AgsVstTResult ags_vst_iplugin_factory3_get_class_info_unicode(AgsVstIPluginFactory3 *iplugin_factory3,
								gint32 index, AgsVstPClassInfoW *info)
  {
    return(((Steinberg::IPluginFactory3 *) iplugin_factory3)->getClassInfoUnicode(index, (Steinberg::PClassInfoW *) info));
  }

}
