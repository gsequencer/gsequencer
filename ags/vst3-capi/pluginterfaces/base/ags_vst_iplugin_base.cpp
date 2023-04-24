/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

  /**
   * Get IID.
   * 
   * @return the Steinberg::TUID as AgsVstTUID
   *
   * @since 5.0.0
   */
  const AgsVstTUID*
  ags_vst_iplugin_base_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::IPluginBase::iid.toTUID()));
  }

  /**
   * Base initialize.
   * 
   * @param iplugin_base the plugin base
   * @param funknown the FUnknown
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iplugin_base_initialize(AgsVstIPluginBase *iplugin_base, AgsVstFUnknown *funknown)
  {
    return(((Steinberg::IPluginBase *) iplugin_base)->initialize((Steinberg::FUnknown *) funknown));
  }

  /**
   * Base terminate.
   * 
   * @param iplugin_base the plugin base
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iplugin_base_terminate(AgsVstIPluginBase *iplugin_base)
  {
    return(((Steinberg::IPluginBase *) iplugin_base)->terminate());
  }

  /**
   * Allocate pclass info.
   * 
   * @return the newly instantiated Steinberg::PClassInfo as AgsVstPClassInfo
   *
   * @since 5.0.0
   */
  AgsVstPClassInfo* ags_vst_pclass_info_alloc()
  {
    Steinberg::PClassInfo *info;

    info = new Steinberg::PClassInfo();

    return((AgsVstPClassInfo *) info);
  }

  /**
   * Free pclass info.
   *
   * @param info pclass info to free
   *
   * @since 5.0.0
   */
  void ags_vst_pclass_info_free(AgsVstPClassInfo *info)
  {
    delete info;
  }

  /**
   * Get CID.
   *
   * @param info pclass info
   * @return the Steinberg::TUID as AgsVstTUID
   *
   * @since 5.0.0
   */
  AgsVstTUID* ags_vst_pclass_info_get_cid(AgsVstPClassInfo *info)
  {
    return((AgsVstTUID *) &(((Steinberg::PClassInfo *) info)->cid));
  }
  
  /**
   * Get cardinality.
   *
   * @param info pclass info
   * @return the cardinality
   *
   * @since 5.0.0
   */
  gint32 ags_vst_pclass_info_get_cardinality(AgsVstPClassInfo *info)
  {
    return(((Steinberg::PClassInfo *) info)->cardinality);
  }
  
  /**
   * Get category.
   *
   * @param info pclass info
   * @return the category
   *
   * @since 5.0.0
   */
  gchar* ags_vst_pclass_info_get_category(AgsVstPClassInfo *info)
  {
    return(((Steinberg::PClassInfo *) info)->category);
  }
  
  /**
   * Get name.
   *
   * @param info pclass info
   * @return the name
   *
   * @since 5.0.0
   */
  gchar* ags_vst_pclass_info_get_name(AgsVstPClassInfo *info)
  {
    return(((Steinberg::PClassInfo *) info)->name);
  }

  /**
   * Get IID.
   * 
   * @return the Steinberg::TUID as AgsVstTUID
   *
   * @since 5.0.0
   */
  const AgsVstTUID*
  ags_vst_iplugin_factory_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::IPluginFactory::iid.toTUID()));
  }

  /**
   * Get plugin factory info.
   *
   * @param iplugin_factory the plugin factory
   * @param pfactory_info the plugin factory info
   * @return the return code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iplugin_factory_get_factory_info(AgsVstIPluginFactory *iplugin_factory,
							 AgsVstPFactoryInfo *pfactory_info)
  {
    return(((Steinberg::IPluginFactory *) iplugin_factory)->getFactoryInfo((Steinberg::PFactoryInfo *) pfactory_info));
  }

  /**
   * Count classes.
   *
   * @param iplugin_factory the plugin factory
   * @return the count of classes
   *
   * @since 5.0.0
   */
  gint32 ags_vst_iplugin_factory_count_classes(AgsVstIPluginFactory *iplugin_factory)
  {
    return(((Steinberg::IPluginFactory *) iplugin_factory)->countClasses());
  }
  
  /**
   * Get plugin factory info.
   *
   * @param iplugin_factory the plugin factory
   * @param index the index
   * @param info the plugin factory info
   * @return the return code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iplugin_factory_get_class_info(AgsVstIPluginFactory *iplugin_factory,
						       gint32 index, AgsVstPClassInfo *info)
  {
    return(((Steinberg::IPluginFactory *) iplugin_factory)->getClassInfo(index, (Steinberg::PClassInfo *) info));
  }

  /**
   * Get plugin factory info.
   *
   * @param iplugin_factory the plugin factory
   * @param cid the CID
   * @param _iid the IID
   * @param obj the return location of object
   * @return the return code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iplugin_factory_create_instance(AgsVstIPluginFactory *iplugin_factory,
							AgsVstFIDString *cid, AgsVstFIDString *_iid, void **obj)
  {
    return(((Steinberg::IPluginFactory *) iplugin_factory)->createInstance((Steinberg::FIDString) cid, (Steinberg::FIDString) _iid, obj));
  }

  /**
   * Get IID.
   * 
   * @return the Steinberg::TUID as AgsVstTUID
   *
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_iplugin_factory2_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::IPluginFactory2::iid.toTUID()));
  }

  /**
   * Allocate pclass info2.
   * 
   * @return the newly instantiated Steinberg::PClassInfo2 as AgsVstPClassInfo2
   *
   * @since 5.0.0
   */
  AgsVstPClassInfo2* ags_vst_pclass_info2_alloc()
  {
    Steinberg::PClassInfo2 *info2;

    info2 = new Steinberg::PClassInfo2();

    return((AgsVstPClassInfo2 *) info2);
  }

  /**
   * Free pclass info2.
   *
   * @param info pclass info2 to free
   *
   * @since 5.0.0
   */
  void ags_vst_pclass_info2_free(AgsVstPClassInfo2 *info2)
  {
    delete info2;
  }

  /**
   * Get category.
   *
   * @param info2 pclass info2
   * @return the category
   *
   * @since 5.0.0
   */
  gchar* ags_vst_pclass_info2_get_category(AgsVstPClassInfo2 *info2)
  {
    return((gchar *) ((Steinberg::PClassInfo2 *) info2)->category);
  }

  /**
   * Get subcategories.
   *
   * @param info2 pclass info2
   * @return the category
   *
   * @since 5.0.0
   */
  gchar* ags_vst_pclass_info2_get_sub_categories(AgsVstPClassInfo2 *info2)
  {
    return((gchar *) ((Steinberg::PClassInfo2 *) info2)->subCategories);
  }
  
  /**
   * Get plugin factory class info2.
   *
   * @param iplugin_factory2 the plugin factory
   * @param index the index
   * @param info the plugin factory info2
   * @return the return code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iplugin_factory2_get_class_info2(AgsVstIPluginFactory2 *iplugin_factory2,
							 gint32 index, AgsVstPClassInfo2 *info)
  {
    return(((Steinberg::IPluginFactory2 *) iplugin_factory2)->getClassInfo2(index, (Steinberg::PClassInfo2 *) info));
  }

  /**
   * Get IID.
   * 
   * @return the Steinberg::TUID as AgsVstTUID
   *
   * @since 5.0.0
   */
  const AgsVstTUID* ags_vst_iplugin_factory3_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::IPluginFactory3::iid.toTUID()));
  }

  /**
   * Get plugin factory class infoW unicode.
   *
   * @param iplugin_factory3 the plugin factory
   * @param index the index
   * @param info the plugin factory infoW
   * @return the return code
   *
   * @since 5.0.0
   */
  AgsVstTResult ags_vst_iplugin_factory3_get_class_info_unicode(AgsVstIPluginFactory3 *iplugin_factory3,
								gint32 index, AgsVstPClassInfoW *info)
  {
    return(((Steinberg::IPluginFactory3 *) iplugin_factory3)->getClassInfoUnicode(index, (Steinberg::PClassInfoW *) info));
  }

}
