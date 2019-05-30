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

#include <ags/vst3-capi/plugininterfaces/base/ags_vst_iplugin_base.h>

#include <plugininterfaces/base/ipluginbase.h>

extern "C" {

  AgsVstTUID
  ags_vst_iplugin_base_get_iid()
  {
    return(IPluginBase__iid);
  }

  gint32 ags_vst_iplugin_base_initialize(AgsVstIPluginBase *plugin_base,
					 AgsVstFUnknown *context)
  {
    return(plugin_base->initialize(context));
  }

  gint32 ags_vst_iplugin_base_terminate(AgsVstIPluginBase *plugin_base)
  {
    return(plugin_base->terminate());
  }

  AgsVstTUID
  ags_vst_iplugin_factory_get_iid()
  {
    return(IPluginFactory__iid);
  }

  gint32 ags_vst_iplugin_factory_get_factory_info(AgsVstIPluginFactory *factory,
						  AgsVstPFactoryInfo *info)
  {
    return(factory->getFactoryInfo(info));
  }
  
  gint32 ags_vst_iplugin_factory_count_classes(AgsVstIPluginFactory *factory)
  {
    return(factory->countClasses());
  }

  gint32 ags_vst_iplugin_factory_get_clas_info(AgsVstIPluginFactory *factory,
					       gint32 index, AgsVstPClassInfo *info)
  {
    return(factory->getClassInfo(index, info));
  }

  gint32 ags_vst_iplugin_factory_create_instance(AgsVstIPluginFactory *factory,
						 AgsVstFIDString cid, AgsVstFIDString _iid, void **obj)
  {
    return(factory->createInstance(cid, _iid, obj));
  }

  AgsVstTUID ags_vst_iplugin_factory2_get_iid()
  {
    return(IPluginFactory2__iid);
  }

  gint32 ags_vst_iplugin_factory2_get_class_info2(AgsVstIPluginFactory2 *factory,
						  gint32 index, AgsVstPClassInfo2 *info)
  {
    return(factory->getClassInfo2(index, info));
  }

  AgsVstTUID ags_vst_iplugin_factory3_get_iid()
  {
    return(IPluginFactory3__iid);
  }
  
  gint32 ags_vst_iplugin_factory3_get_class_info_unicode(AgsVstIPluginFactory3 *factory,
							 gint32 index, AgsVstPClassInfoW *info)
  {
    return(factory->getClassInfoUnicode(index, info));
  }

  gint32 ags_vst_iplugin_factory3_set_host_context(AgsVstIPluginFactory3 *factory,
						   AgsVstFUnknown *context)
  {
    return(factory->setHostContext(context));
  }

  AgsVstIPluginFactory*  ags_vst_get_plugin_factory()
  {
    return(GetPluginFactory());
  }

}
