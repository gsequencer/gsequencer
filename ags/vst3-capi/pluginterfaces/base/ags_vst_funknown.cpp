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

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.h>

#include <pluginterfaces/base/funknown.h>

using namespace Steinberg;

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
    return((AgsVstFUID *) new Steinberg::FUID());
  }
  
  AgsVstFUID* ags_vst_fuid_new_with_iid(guint32 l1, guint32 l2, guint32 l3, guint32 l4)
  {
    return((AgsVstFUID *) new Steinberg::FUID(l1, l2, l3, l4));
  }
  
  AgsVstFUID* ags_vst_fuid_new_from_fuid(AgsVstFUID *fuid)
  {
    const Steinberg::FUID &this_fuid = (Steinberg::FUID &) fuid;
    
    return((AgsVstFUID *) new Steinberg::FUID(this_fuid));
  }
  
  void ags_vst_fuid_delete(AgsVstFUID *fuid)
  {
    delete fuid;
  }
    
  gboolean ags_vst_fuid_generate(AgsVstFUID *fuid)
  {
    return(static_cast<gboolean>(((Steinberg::FUID *) fuid)->generate()));
  }
  
  gboolean ags_vst_fuid_is_valid(AgsVstFUID *fuid)
  {
    return(static_cast<gboolean>(((Steinberg::FUID *) fuid)->isValid()));
  }
  
  AgsVstFUID* ags_vst_fuid_equal(AgsVstFUID *destination_fuid,
				 AgsVstFUID *source_fuid)
  {
    return((destination_fuid = source_fuid));
  }
  
  gboolean ags_vst_fuid_equals(AgsVstFUID *fuid_a,
			       AgsVstFUID *fuid_b)
  {
    return(static_cast<gboolean>(fuid_a == fuid_b));
  }
  
  gboolean ags_vst_fuid_smaller(AgsVstFUID *fuid_a,
				AgsVstFUID *fuid_b)
  {
    return(static_cast<gboolean>(fuid_a < fuid_b));
  }
  
  gboolean ags_vst_fuid_not_equals(AgsVstFUID *fuid_a,
				   AgsVstFUID *fuid_b)
  {
    return(static_cast<gboolean>(fuid_a != fuid_b));
  }
  
  guint32 ags_vst_fuid_get_long1(AgsVstFUID *fuid)
  {
    return(((Steinberg::FUID *) fuid)->getLong1());
  }
  
  guint32 ags_vst_fuid_get_long2(AgsVstFUID *fuid)
  {
    return(((Steinberg::FUID *) fuid)->getLong2());
  }
  
  guint32 ags_vst_fuid_get_long3(AgsVstFUID *fuid)
  {
    return(((Steinberg::FUID *) fuid)->getLong3());
  }
  
  guint32 ags_vst_fuid_get_long4(AgsVstFUID *fuid)
  {
    return(((Steinberg::FUID *) fuid)->getLong4());
  }
  
  void ags_vst_fuid_from_uint32(AgsVstFUID *fuid,
				guint32 d1, guint32 d2, guint32 d3, guint32 d4)
  {
    ((Steinberg::FUID *) fuid)->from4Int(d1, d2, d3, d4);
  }
  
  void ags_vst_fuid_to_uint32(AgsVstFUID *fuid,
			      guint32 *d1, guint32 *d2, guint32 *d3, guint32 *d4)
  {
    ((Steinberg::FUID *) fuid)->to4Int(static_cast<uint32&>(d1[0]), static_cast<uint32&>(d2[0]), static_cast<uint32&>(d3[0]), static_cast<uint32&>(d4[0]));
  }
  
  void ags_vst_fuid_to_string(AgsVstFUID *fuid,
			      gchar *string)
  {
    ((Steinberg::FUID *) fuid)->toString(string);
  }
  
  gboolean ags_vst_fuid_from_string(AgsVstFUID *fuid,
				    gchar *string)
  {
    return(static_cast<gboolean>(((Steinberg::FUID *) fuid)->fromString(string)));
  }
  
  void ags_vst_fuid_to_registry_string(AgsVstFUID *fuid,
				       gchar *string)
  {
    ((Steinberg::FUID *) fuid)->toRegistryString(string);
  }
  
  gboolean ags_vst_fuid_from_registry_string(AgsVstFUID *fuid,
					     gchar *string)
  {
    return(static_cast<gboolean>(((Steinberg::FUID *) fuid)->fromRegistryString(string)));
  }
  
  void ags_vst_fuid_print(AgsVstFUID *fuid,
			  gchar *string, gint32 style)
  {
    ((Steinberg::FUID *) fuid)->print(string, style);
  }
  
  void ags_vst_fuid_to_tuid_with_result(AgsVstFUID *fuid,
					AgsVstTUID *result)
  {
    ((Steinberg::FUID *) fuid)->toTUID(*(reinterpret_cast<Steinberg::TUID *>(result)));
  }
    
  AgsVstTUID* ags_vst_fuid_to_tuid(AgsVstFUID *fuid)
  {
    return((AgsVstTUID *)(&(((Steinberg::FUID *) fuid)->toTUID())));
  }
  
  AgsVstFUID* ags_vst_fuid_from_tuid(AgsVstFUID *fuid,
				     AgsVstTUID *tuid)
  {
    Steinberg::FUID *retval;

    retval = (Steinberg::FUID *) new Steinberg::FUID();
    retval[0] = ((Steinberg::FUID *) fuid)->fromTUID(((Steinberg::TUID *) tuid)[0]); 
    
    return((AgsVstFUID *) retval);
  }  

  const AgsVstTUID*
  ags_vst_funknown_get_iid()
  {
    return(reinterpret_cast<const AgsVstTUID*>(&INLINE_UID_OF(Steinberg::FUnknown)));
  }

  gint32 ags_vst_funknown_query_interface(AgsVstFUnknown *funknown,
					  AgsVstTUID *_iid, void **obj)
  {
    return(((Steinberg::FUnknown *) funknown)->queryInterface(((Steinberg::TUID *) _iid)[0], obj));
  }
  
  guint32 ags_vst_funknown_add_ref(AgsVstFUnknown *funknown)
  {
    return(((Steinberg::FUnknown *) funknown)->addRef());
  }
  
  guint32 ags_vst_funknown_release(AgsVstFUnknown *funknown)
  {
    return(((Steinberg::FUnknown *) funknown)->release());
  }
  
  AgsVstFReleaser* ags_freleaser_alloc(AgsVstFUnknown *funknown)
  {
    return((AgsVstFReleaser *) new Steinberg::FReleaser((Steinberg::FUnknown *) funknown));
  }
  
  void ags_freleaser_free(AgsVstFReleaser *freleaser)
  {
    delete freleaser;
  }
  
}
