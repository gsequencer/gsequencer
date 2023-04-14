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

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_fvariant.h>

#include <pluginterfaces/base/fvariant.h>

using namespace Steinberg;

extern "C" {

  /**
   * Instantiate Steinberg::FVariant and return a C99 compatible handle
   * AgsVstFVariant a void pointer.
   *
   * @return the new instance of Steinberg::FVariant as AgsVstFVariant
   * 
   * @since 5.0.0
   */
  AgsVstFVariant* ags_vst_fvariant_new()
  {
    return((AgsVstFVariant *) new Steinberg::FVariant());
  }
  
  /**
   * Instantiate Steinberg::FVariant and return a C99 compatible handle
   * AgsVstFVariant a void pointer.
   *
   * @param val the boolean
   * @return the new instance of Steinberg::FVariant as AgsVstFVariant
   * 
   * @since 5.0.0
   */
  AgsVstFVariant* ags_vst_fvariant_new_boolean(gboolean val)
  {
    return((AgsVstFVariant *) new Steinberg::FVariant(static_cast<bool>(val)));
  }
  
  /**
   * Instantiate Steinberg::FVariant and return a C99 compatible handle
   * AgsVstFVariant a void pointer.
   *
   * @param val the uint32
   * @return the new instance of Steinberg::FVariant as AgsVstFVariant
   * 
   * @since 5.0.0
   */
  AgsVstFVariant* ags_vst_fvariant_new_uint32(guint32 val)
  {
    return((AgsVstFVariant *) new Steinberg::FVariant(static_cast<uint32>(val)));
  }
  
  /**
   * Instantiate Steinberg::FVariant and return a C99 compatible handle
   * AgsVstFVariant a void pointer.
   *
   * @param val the int64
   * @return the new instance of Steinberg::FVariant as AgsVstFVariant
   * 
   * @since 5.0.0
   */
  AgsVstFVariant* ags_vst_fvariant_new_int64(gint64 val)
  {
    return((AgsVstFVariant *) new Steinberg::FVariant(static_cast<int64>(val)));
  }
  
  /**
   * Instantiate Steinberg::FVariant and return a C99 compatible handle
   * AgsVstFVariant a void pointer.
   *
   * @param val the double
   * @return the new instance of Steinberg::FVariant as AgsVstFVariant
   * 
   * @since 5.0.0
   */
  AgsVstFVariant* ags_vst_fvariant_new_double(gdouble val)
  {
    return((AgsVstFVariant *) new Steinberg::FVariant(static_cast<double>(val)));
  }
  
  /**
   * Instantiate Steinberg::FVariant and return a C99 compatible handle
   * AgsVstFVariant a void pointer.
   *
   * @param str the string
   * @return the new instance of Steinberg::FVariant as AgsVstFVariant
   * 
   * @since 5.0.0
   */
  AgsVstFVariant* ags_vst_fvariant_new_string(gchar *str)
  {
    return((AgsVstFVariant *) new Steinberg::FVariant(static_cast<const char8 *>(str)));
  }
  
  /**
   * Instantiate Steinberg::FVariant and return a C99 compatible handle
   * AgsVstFVariant a void pointer.
   *
   * @str the unicode string
   * @return the new instance of Steinberg::FVariant as AgsVstFVariant
   * 
   * @since 5.0.0
   */
  AgsVstFVariant* ags_vst_fvariant_new_wstring(gunichar2 *str)
  {
    return((AgsVstFVariant *) new Steinberg::FVariant(str));
  }
  
  /**
   * Instantiate Steinberg::FVariant and return a C99 compatible handle
   * AgsVstFVariant a void pointer.
   *
   * @param funknown the Steinberg::FUnknown
   * @param owner owner
   * @return the new instance of Steinberg::FVariant as AgsVstFVariant
   * 
   * @since 5.0.0
   */
  AgsVstFVariant* ags_vst_fvariant_new_funknown(AgsVstFUnknown *funknown, gboolean owner)
  {
    return((AgsVstFVariant *) new Steinberg::FVariant((Steinberg::FUnknown *) funknown, static_cast<bool>(owner)));
  }
  
  /**
   * Delete Steinberg::FVariant.
   *
   * @param fuid the Steinberg::FVariant
   * 
   * @since 5.0.0
   */
  void ags_vst_fvariant_delete(AgsVstFVariant *fvariant)
  {
    delete fvariant;
  }

  /**
   * Set boolean.
   *
   * @param fvariant the Steinberg::FVariant
   * @param val the boolean
   * 
   * @since 5.0.0
   */
  void ags_vst_fvariant_set_boolean(AgsVstFVariant *fvariant,
				    gboolean val)
  {
    ((Steinberg::FVariant *) fvariant)->set(static_cast<bool>(val));
  }
  
  /**
   * Set uint32.
   *
   * @param fvariant the Steinberg::FVariant
   * @param val the uint32
   * 
   * @since 5.0.0
   */
  void ags_vst_fvariant_set_uint32(AgsVstFVariant *fvariant,
				   guint32 val)
  {
    ((Steinberg::FVariant *) fvariant)->set(static_cast<uint32>(val));
  }
  
  /**
   * Set int64.
   *
   * @param fvariant the Steinberg::FVariant
   * @param val the int64
   * 
   * @since 5.0.0
   */
  void ags_vst_fvariant_set_int64(AgsVstFVariant *fvariant,
				  gint64 val)
  {
    ((Steinberg::FVariant *) fvariant)->set(static_cast<int64>(val));
  }
  
  /**
   * Set double.
   *
   * @param fvariant the Steinberg::FVariant
   * @param val the double
   * 
   * @since 5.0.0
   */
  void ags_vst_fvariant_set_double(AgsVstFVariant *fvariant,
				   gdouble val)
  {
    ((Steinberg::FVariant *) fvariant)->set(static_cast<double>(val));
  }
  
  /**
   * Set string.
   *
   * @param fvariant the Steinberg::FVariant
   * @param str the string
   * 
   * @since 5.0.0
   */
  void ags_vst_fvariant_set_string(AgsVstFVariant *fvariant,
				   gchar *str)
  {
    ((Steinberg::FVariant *) fvariant)->set(static_cast<char8 *>(str));
  }
  
  /**
   * Set unicode string.
   *
   * @param fvariant the Steinberg::FVariant
   * @param str the unicode string
   * 
   * @since 5.0.0
   */
  void ags_vst_fvariant_set_wstring(AgsVstFVariant *fvariant,
				    gunichar2 *str)
  {
    ((Steinberg::FVariant *) fvariant)->set(reinterpret_cast<char16 *>(str));
  }
  
  /**
   * Set object.
   *
   * @param fvariant the Steinberg::FVariant
   * @param obj the object
   * 
   * @since 5.0.0
   */
  void ags_vst_fvariant_set_object(AgsVstFVariant *fvariant,
				   AgsVstFUnknown *obj)
  {
    ((Steinberg::FVariant *) fvariant)->set((Steinberg::FUnknown *) obj);
  }

  /**
   * Get boolean.
   *
   * @param fvariant the variant
   * @return the value
   *
   * @since 5.0.0
   */
  gboolean ags_vst_fvariant_get_boolean(AgsVstFVariant *fvariant)
  {
    return(((Steinberg::FVariant *) fvariant)->get<bool>());
  }
  
  /**
   * Get uint32.
   *
   * @param fvariant the variant
   * @return the value
   *
   * @since 5.0.0
   */
  guint32 ags_vst_fvariant_get_uint32(AgsVstFVariant *fvariant)
  {
    return(((Steinberg::FVariant *) fvariant)->get<uint32>());
  }
  
  /**
   * Get int64.
   *
   * @param fvariant the variant
   * @return the value
   *
   * @since 5.0.0
   */
  gint64 ags_vst_fvariant_get_int64(AgsVstFVariant *fvariant)
  {
    return(((Steinberg::FVariant *) fvariant)->get<int64>());
  }
  
  /**
   * Get double.
   *
   * @param fvariant the variant
   * @return the value
   *
   * @since 5.0.0
   */
  gdouble ags_vst_fvariant_get_double(AgsVstFVariant *fvariant)
  {
    return(((Steinberg::FVariant *) fvariant)->get<double>());
  }
  
  /**
   * Get string.
   *
   * @param fvariant the variant
   * @return the string
   *
   * @since 5.0.0
   */
  gchar* ags_vst_fvariant_get_string(AgsVstFVariant *fvariant)
  {
    //FIXME:JK: check string
//    return(((Steinberg::FVariant *) fvariant)->get<char8*>());

    return(NULL);
  }
  
  /**
   * Get unicode string.
   *
   * @param fvariant the variant
   * @return the unicode string
   *
   * @since 5.0.0
   */
  gunichar2* ags_vst_fvariant_get_wstring(AgsVstFVariant *fvariant)
  {
    //FIXME:JK: check string
//    return(reinterpret_cast<gunichar2 *>(((Steinberg::FVariant *) fvariant)->get<char16*>()));

    return(NULL);
  }
  
  /**
   * Get object.
   *
   * @param fvariant the variant
   * @return the object
   *
   * @since 5.0.0
   */
  AgsVstFUnknown* ags_vst_fvariant_get_object(AgsVstFVariant *fvariant)
  {
    return((AgsVstFUnknown *) ((Steinberg::FVariant *) fvariant)->getObject());
  }
  
  /**
   * Get type.
   *
   * @param fvariant the variant
   * @return the type
   *
   * @since 5.0.0
   */
  guint16 ags_vst_fvariant_get_type(AgsVstFVariant *fvariant)
  {
    return(((Steinberg::FVariant *) fvariant)->getType());
  }
  
  /**
   * Is empty.
   *
   * @param fvariant the variant
   * @return true if is empty, otherwise false
   *
   * @since 5.0.0
   */
  gboolean ags_vst_fvariant_is_empty(AgsVstFVariant *fvariant)
  {
    return(static_cast<gboolean>(((Steinberg::FVariant *) fvariant)->isEmpty()));
  }
  
  /**
   * Is owner.
   *
   * @param fvariant the variant
   * @return true if is owner, otherwise false
   *
   * @since 5.0.0
   */
  gboolean ags_vst_fvariant_is_owner(AgsVstFVariant *fvariant)
  {
    return(static_cast<gboolean>(((Steinberg::FVariant *) fvariant)->isOwner()));
  }
  
  /**
   * Is string.
   *
   * @param fvariant the variant
   * @return true if is string, otherwise false
   *
   * @since 5.0.0
   */
  gboolean ags_vst_fvariant_is_string(AgsVstFVariant *fvariant)
  {
    return(static_cast<gboolean>(((Steinberg::FVariant *) fvariant)->isString()));
  }
  
  /**
   * Set owner.
   *
   * @param fvariant the variant
   * @param state the state
   *
   * @since 5.0.0
   */
  void ags_vst_fvariant_set_owner(AgsVstFVariant *fvariant,
				  gboolean state)
  {
    ((Steinberg::FVariant *) fvariant)->setOwner(static_cast<bool>(state));
  }
  
  /**
   * Empty.
   *
   * @param fvariant the variant
   *
   * @since 5.0.0
   */
  void ags_vst_fvariant_empty(AgsVstFVariant *fvariant)
  {
    ((Steinberg::FVariant *) fvariant)->empty();
  }  

}
