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

  /**
   * Check private iid equal.
   *
   * @param iid1 the iid
   * @param iid2 the other iid
   * @return true if equal, otherwise false
   *
   * @since 5.0.0
   */
  gboolean ags_vst_funknown_private_iid_equal(const void* iid1, const void* iid2)
  {
    //TODO:JK: implement me

    return(FALSE);
  }

  /**
   * Private atomic add.
   *
   * @param value the value
   * @param amount the amount
   * @return 0
   *
   * @since 5.0.0
   */
  gint32 ags_vst_funknown_private_atomic_add(gint32 *value, gint32 amount)
  {
    //TODO:JK: implement me

    return(0);
  }

  /**
   * Instantiate Steinberg::FUID and return a C99 compatible handle
   * AgsVstFUID a void pointer.
   *
   * @return the new instance of Steinberg::FUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  AgsVstFUID* ags_vst_fuid_new()
  {
    return((AgsVstFUID *) new Steinberg::FUID());
  }
  
  /**
   * Instantiate Steinberg::FUID and return a C99 compatible handle
   * AgsVstFUID a void pointer.
   *
   * @param l1 iid l1
   * @param l2 iid l2
   * @param l3 iid l3
   * @param l4 iid l4
   * @return the new instance of Steinberg::FUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  AgsVstFUID* ags_vst_fuid_new_with_iid(guint32 l1, guint32 l2, guint32 l3, guint32 l4)
  {
    return((AgsVstFUID *) new Steinberg::FUID(l1, l2, l3, l4));
  }
  
  /**
   * Instantiate Steinberg::FUID and return a C99 compatible handle
   * AgsVstFUID a void pointer.
   *
   * @param fuid the Steinberg::FUID
   * @return the new instance of Steinberg::FUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  AgsVstFUID* ags_vst_fuid_new_from_fuid(AgsVstFUID *fuid)
  {
    const Steinberg::FUID &this_fuid = (Steinberg::FUID &) fuid;
    
    return((AgsVstFUID *) new Steinberg::FUID(this_fuid));
  }
  
  /**
   * Delete Steinberg::FUID.
   *
   * @param fuid the Steinberg::FUID
   * 
   * @since 5.0.0
   */
  void ags_vst_fuid_delete(AgsVstFUID *fuid)
  {
    delete (Steinberg::FUID *) fuid;
  }
    
  /**
   * Generate.
   *
   * @param fuid the Steinberg::FUID
   * @return true on success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_fuid_generate(AgsVstFUID *fuid)
  {
    return(static_cast<gboolean>(((Steinberg::FUID *) fuid)->generate()));
  }
  
  /**
   * Check if valid.
   *
   * @param fuid the Steinberg::FUID
   * @return true if valid, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_fuid_is_valid(AgsVstFUID *fuid)
  {
    return(static_cast<gboolean>(((Steinberg::FUID *) fuid)->isValid()));
  }
  
  /**
   * Equal FUID.
   *
   * @param destination_fuid the destination FUID
   * @param source_fuid the source FUID
   * @return the Steinberg::FUID of the equal operation
   * 
   * @since 5.0.0
   */
  AgsVstFUID* ags_vst_fuid_equal(AgsVstFUID *destination_fuid,
				 AgsVstFUID *source_fuid)
  {
    return((destination_fuid = source_fuid));
  }
  
  /**
   * Test if FUID equals.
   *
   * @param fuid_a the left side FUID
   * @param fuid_b the right side FUID
   * @return true if equal, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_fuid_equals(AgsVstFUID *fuid_a,
			       AgsVstFUID *fuid_b)
  {
    return(static_cast<gboolean>(fuid_a == fuid_b));
  }
  
  /**
   * Test if FUID is smaller.
   *
   * @param fuid_a the left side FUID
   * @param fuid_b the right side FUID
   * @return true if smaller, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_fuid_smaller(AgsVstFUID *fuid_a,
				AgsVstFUID *fuid_b)
  {
    return(static_cast<gboolean>(fuid_a < fuid_b));
  }
  
  /**
   * Test if FUID is not equals.
   *
   * @param fuid_a the left side FUID
   * @param fuid_b the right side FUID
   * @return true if not equals, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_fuid_not_equals(AgsVstFUID *fuid_a,
				   AgsVstFUID *fuid_b)
  {
    return(static_cast<gboolean>(fuid_a != fuid_b));
  }
  
  /**
   * Get long1.
   *
   * @param fuid the Steinberg::FUID
   * @return the long1
   * 
   * @since 5.0.0
   */
  guint32 ags_vst_fuid_get_long1(AgsVstFUID *fuid)
  {
    return(((Steinberg::FUID *) fuid)->getLong1());
  }
  
  /**
   * Get long2.
   *
   * @param fuid the Steinberg::FUID
   * @return the long2
   * 
   * @since 5.0.0
   */
  guint32 ags_vst_fuid_get_long2(AgsVstFUID *fuid)
  {
    return(((Steinberg::FUID *) fuid)->getLong2());
  }
  
  /**
   * Get long3.
   *
   * @param fuid the Steinberg::FUID
   * @return the long3
   * 
   * @since 5.0.0
   */
  guint32 ags_vst_fuid_get_long3(AgsVstFUID *fuid)
  {
    return(((Steinberg::FUID *) fuid)->getLong3());
  }
  
  /**
   * Get long4.
   *
   * @param fuid the Steinberg::FUID
   * @return the long4
   * 
   * @since 5.0.0
   */
  guint32 ags_vst_fuid_get_long4(AgsVstFUID *fuid)
  {
    return(((Steinberg::FUID *) fuid)->getLong4());
  }
  
  /**
   * From uint32.
   *
   * @param fuid the Steinberg::FUID
   * @param d1 the int 1
   * @param d2 the int 2
   * @param d3 the int 3
   * @param d4 the int 4
   * 
   * @since 5.0.0
   */
  void ags_vst_fuid_from_uint32(AgsVstFUID *fuid,
				guint32 d1, guint32 d2, guint32 d3, guint32 d4)
  {
    ((Steinberg::FUID *) fuid)->from4Int(d1, d2, d3, d4);
  }
  
  /**
   * To uint32.
   *
   * @param fuid the Steinberg::FUID
   * @param d1 the return location of int 1
   * @param d2 the return location of int 2
   * @param d3 the return location of int 3
   * @param d4 the return location of int 4
   * 
   * @since 5.0.0
   */
  void ags_vst_fuid_to_uint32(AgsVstFUID *fuid,
			      guint32 *d1, guint32 *d2, guint32 *d3, guint32 *d4)
  {
    ((Steinberg::FUID *) fuid)->to4Int(static_cast<uint32&>(d1[0]), static_cast<uint32&>(d2[0]), static_cast<uint32&>(d3[0]), static_cast<uint32&>(d4[0]));
  }
  
  /**
   * To string.
   *
   * @param fuid the Steinberg::FUID
   * @param string the string
   * 
   * @since 5.0.0
   */
  void ags_vst_fuid_to_string(AgsVstFUID *fuid,
			      gchar *string)
  {
    ((Steinberg::FUID *) fuid)->toString(string);
  }
  
  /**
   * From string.
   *
   * @param fuid the Steinberg::FUID
   * @param string the string
   * @return true on success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_fuid_from_string(AgsVstFUID *fuid,
				    gchar *string)
  {
    return(static_cast<gboolean>(((Steinberg::FUID *) fuid)->fromString(string)));
  }
  
  /**
   * To registry string.
   *
   * @param fuid the Steinberg::FUID
   * @param string the string
   * 
   * @since 5.0.0
   */
  void ags_vst_fuid_to_registry_string(AgsVstFUID *fuid,
				       gchar *string)
  {
    ((Steinberg::FUID *) fuid)->toRegistryString(string);
  }
  
  /**
   * From registry string.
   *
   * @param fuid the Steinberg::FUID
   * @param string the string
   * @return true on success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_fuid_from_registry_string(AgsVstFUID *fuid,
					     gchar *string)
  {
    return(static_cast<gboolean>(((Steinberg::FUID *) fuid)->fromRegistryString(string)));
  }
  
  /**
   * Print.
   *
   * @param fuid the Steinberg::FUID
   * @param string the string
   * @param style the style
   * 
   * @since 5.0.0
   */
  void ags_vst_fuid_print(AgsVstFUID *fuid,
			  gchar *string, gint32 style)
  {
    ((Steinberg::FUID *) fuid)->print(string, style);
  }
  
  /**
   * To TUID.
   *
   * @param fuid the Steinberg::FUID
   * @param result the AgsVstTUID
   * 
   * @since 5.0.0
   */
  void ags_vst_fuid_to_tuid_with_result(AgsVstFUID *fuid,
					AgsVstTUID *result)
  {
    ((Steinberg::FUID *) fuid)->toTUID(*(reinterpret_cast<Steinberg::TUID *>(result)));
  }
    
  /**
   * To TUID.
   *
   * @param fuid the Steinberg::FUID
   * @return the AgsVstTUID
   * 
   * @since 5.0.0
   */
  AgsVstTUID* ags_vst_fuid_to_tuid(AgsVstFUID *fuid)
  {
    return((AgsVstTUID *)(&(((Steinberg::FUID *) fuid)->toTUID())));
  }
  
  /**
   * From registry string.
   *
   * @param fuid the Steinberg::FUID
   * @param tuid the Steinberg::TUID
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  AgsVstFUID* ags_vst_fuid_from_tuid(AgsVstFUID *fuid,
				     AgsVstTUID *tuid)
  {
    Steinberg::FUID *retval;

    retval = (Steinberg::FUID *) new Steinberg::FUID();
    retval[0] = ((Steinberg::FUID *) fuid)->fromTUID(((Steinberg::TUID *) tuid)[0]); 
    
    return((AgsVstFUID *) retval);
  }  

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstFUID
   * 
   * @since 5.0.0
   */
  const AgsVstTUID*
  ags_vst_funknown_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::FUnknown::iid.toTUID()));
  }

  /**
   * Query interface.
   *
   * @param funknown the Steinberg::FUnknown to query
   * @param _iid the IID
   * @param obj the return location of queried interface
   * @return the return code
   * 
   * @since 5.0.0
   */
  gint32 ags_vst_funknown_query_interface(AgsVstFUnknown *funknown,
					  AgsVstTUID *_iid, void **obj)
  {
    return(((Steinberg::FUnknown *) funknown)->queryInterface(((Steinberg::TUID *) _iid)[0], obj));
  }
  
  /**
   * Add ref.
   *
   * @return the return code
   * 
   * @since 5.0.0
   */
  guint32 ags_vst_funknown_add_ref(AgsVstFUnknown *funknown)
  {
    return(((Steinberg::FUnknown *) funknown)->addRef());
  }
  
  /**
   * Release ref.
   *
   * @return the return code
   * 
   * @since 5.0.0
   */
  guint32 ags_vst_funknown_release(AgsVstFUnknown *funknown)
  {
    return(((Steinberg::FUnknown *) funknown)->release());
  }
  
  /**
   * Alloc.
   *
   * @return the Steinberg::FReleaser as AgsVstFReleaser
   * 
   * @since 5.0.0
   */
  AgsVstFReleaser* ags_freleaser_alloc(AgsVstFUnknown *funknown)
  {
    return((AgsVstFReleaser *) new Steinberg::FReleaser((Steinberg::FUnknown *) funknown));
  }
  
  /**
   * Free.
   *
   * @param freleaser the Steinberg::FReleaser
   * 
   * @since 5.0.0
   */
  void ags_freleaser_free(AgsVstFReleaser *freleaser)
  {
    delete (Steinberg::FReleaser *) freleaser;
  }
  
}
