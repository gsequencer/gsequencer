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

#include <ags/vst3-capi/plugininterfaces/base/ags_vst_funknown.h>

#include <plugininterfaces/base/funknown.h>

extern "C" {

  gboolean ags_vst_funknown_private_iid_equal(const void* iid1, const void* iid2)
  {
    //TODO:JK: implement me
  }
  
  gint32 ags_vst_funknown_private_atomic_add(gint32 *value, gint32 amount)
  {
    //TODO:JK: implement me
  }
  
  AgsVstFUID* ags_vst_fuid_new()
  {
    return(new FUID());
  }
  
  AgsVstFUID* ags_vst_fuid_new_with_iid(uint32 l1, uint32 l2, uint32 l3, uint32 l4)
  {
    return(new FUID(l1, l2, l3, l4));
  }
  
  AgsVstFUID* ags_vst_fuid_new_from_fuid(AgsVstFUID *fuid)
  {
    return(new FUID(fuid));
  }
  
  void ags_vst_fuid_delete(AgsVstFUID *fuid)
  {
    delete fuid;
  }
    
  gboolean ags_vst_fuid_generate(AgsVstFUID *fuid)
  {
    return(static_cast<gboolean>(fuid->generate()));
  }
  
  gboolean ags_vst_fuid_is_valid(AgsVstFUID *fuid)
  {
    return(static_cast<gboolean>(fuid->isValid()));
  }
  
  AgsVstFUID* ags_vst_fuid_equal(AgsVstFUID *destination_fuid,
				 AgsVstFUID *source_fuid)
  {
    return((destination_fuid = source_fuid));
  }
  
  gboolean ags_vst_fuid_equals(AgsVstFUID *fuid_a,
			       AgsVstFUID *fuid_b)
  {
    return(fuid_a == fuid_b);
  }
  
  gboolean ags_vst_fuid_smaller(AgsVstFUID *fuid_a,
				AgsVstFUID *fuid_b)
  {
    return(fuid_a < fuid_b);
  }
  
  gboolean ags_vst_fuid_not_equals(AgsVstFUID *fuid_a,
				   AgsVstFUID *fuid_b)
  {
    return(fuid_a != fuid_b);
  }
  
  guint32 ags_vst_fuid_get_long1(AgsVstFUID *fuid)
  {
    return(fuid->getLong1());
  }
  
  guint32 ags_vst_fuid_get_long2(AgsVstFUID *fuid)
  {
    return(fuid->getLong2());
  }
  
  guint32 ags_vst_fuid_get_long3(AgsVstFUID *fuid)
  {
    return(fuid->getLong3());
  }
  
  guint32 ags_vst_fuid_get_long4(AgsVstFUID *fuid)
  {
    return(fuid->getLong4());
  }
  
  void ags_vst_fuid_from_uint32(AgsVstFUID *fuid,
				guint32 d1, guint32 d2, guint32 d3, guint32 d4)
  {
    fuid->from4Int(d1, d2, d3, d4);
  }
  
  void ags_vst_fuid_to_uint32(AgsVstFUID *fuid,
			      guint32 *d1, guint32 *d2, guint32 *d3, guint32 *d4)
  {
    fuid->to4Int(static_cast<uint32&>(d1[0]), static_cast<uint32&>(d2[0]), static_cast<uint32&>(d3[0]), static_cast<uint32&>(d4[0]));
  }
  
  void ags_vst_fuid_to_string(AgsVstFUID *fuid,
			      gchar *string)
  {
    fuid->toString(string);
  }
  
  gboolean ags_vst_fuid_from_string(AgsVstFUID *fuid,
				    gchar *string)
  {
    return(static_cast<gboolean>(fuid->fromString(string)));
  }
  
  void ags_vst_fuid_to_registry_string(AgsVstFUID *fuid,
				       gchar *string)
  {
    fuid->toRegistryString(string);
  }
  
  gboolean ags_vst_fuid_from_registry_string(AgsVstFUID *fuid,
					     gchar *string)
  {
    return(static_cast<gboolean>(fuid->fromRegistryString(string)));
  }
  
  void ags_vst_fuid_print(AgsVstFUID *fuid,
			  gchar *string, gint32 style)
  {
    fuid->print(string, style);
  }
  
  void ags_vst_fuid_to_tuid_with_result(AgsVstFUID *fuid,
					AgsVstTUID tuid,
					AgsVstTUID result)
  {
    tuid = fuid->toTUID(result);
  }
  
  AgsVstTUID ags_vst_tuid_get_data(AgsVstFUID *fuid,
				   AgsVstTUID tuid)
  {
    return(fuid->getData(tuid));
  }
  
  AgsVstTUID ags_vst_fuid_to_tuid(AgsVstFUID *fuid,
				  AgsVstTUID tuid)
  {
    return(fuid->toTUID(tuid));
  }
  
  AgsVstFUID* ags_vst_fuid_from_tuid(AgsVstFUID *fuid,
				     AgsVstTUID uid)
  {
    return(fuid->fromTUID(uid));
  }  

  AgsVstTUID
  ags_vst_funknown_get_iid()
  {
    return(FUnkown__iid);
  }

  gint32 ags_vst_funknown_query_interface(AgsVstFUnknown *funknown,
					  TUID _iid, void** obj)
  {
    return(funknown->queryInterface(_iid, obj));
  }
  
  guint32 ags_vst_funknown_add_ref(AgsVstFUnknown *funknown)
  {
    return(funknown->addRef());
  }
  
  guint32 ags_vst_funknown_release(AgsVstFUnknown *funknown)
  {
    return(funknown->release());
  }
  
  AgsVstFunknownPtr* ags_vst_funknown_ptr_new()
  {
    return(new FUnknownPtr());
  }
  
  AgsVstFunknownPtr* ags_vst_funknown_ptr_new_from_template(AgsVstFUnknownPtr *funknown_ptr)
  {
    return(new FUnknownPtr(funknown_ptr));
  }
  
  AgsVstFunknownPtr* ags_vst_funknown_ptr_new_from_funknown(AgsVstFUnknown *funknown)
  {
    return(new FUnknownPtr(funknown));
  }
    
  void ags_funknown_get_interface(AgsVstFUnknownPtr *funknown_ptr,
				  AgsVstI *vst_interface)
  {
    return(funknown_ptr->getInterface(vst_interface));
  }
  
  void ags_vst_funknown_ptr_set_funknown(AgsVstFunknownPtr *funknown_ptr,
					 AgsVstFUnknown *funknown)
  {
    //TODO:JK: implement me
  }
  
  void ags_vst_funknown_ptr_set_funknown_interface(AgsVstFunknownPtr *funknown_ptr,
						   AgsVstFUnknown *funknown, AgsVstI *vst_interface)
  {
    //TODO:JK: implement me
  }
  
  AgsVstFReleaser* ags_freleaser_alloc(AgsVstFunknown *funknown)
  {
    return(new FReleaser());
  }
  
  void ags_freleaser_free(AgsVstFReleaser *freleaser)
  {
    delete freleaser;
  }
  
}
