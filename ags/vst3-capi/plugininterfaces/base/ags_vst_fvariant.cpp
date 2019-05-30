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

#include <ags/vst3-capi/plugininterfaces/base/ags_vst_fvariant.h>

#include <plugininterfaces/base/fvariant.h>

extern "C" {

  AgsVstFVariant* ags_vst_fvariant_new()
  {
    return(new FVariant());
  }
  
  AgsVstFVariant* ags_vst_fvariant_new_boolean(gboolean val)
  {
    return(new FVariant(static_cast<bool>(val)));
  }
  
  AgsVstFVariant* ags_vst_fvariant_new_uint32(guint32 val)
  {
    return(new FVariant(val));
  }
  
  AgsVstFVariant* ags_vst_fvariant_new_int64(gint64 val)
  {
    return(new FVariant(val));
  }
  
  AgsVstFVariant* ags_vst_fvariant_new_double(gdouble val)
  {
    return(new FVariant(val));
  }
  
  AgsVstFVariant* ags_vst_fvariant_new_string(gchar *str)
  {
    return(new FVariant(str));
  }
  
  AgsVstFVariant* ags_vst_fvariant_new_wstring(gunichar2 *str)
  {
    return(new FVariant(str));
  }
  
  AgsVstFVariant* ags_vst_fvariant_new_funknown(AgsVstFUnknown *funknown, gboolean owner)
  {
    return(new FVariant(funknown, owner));
  }
  
  void ags_vst_fvariant_delete(AgsVstFVariant *fvariant)
  {
    delete fvariant;
  }
  
  void ags_vst_fvariant_set_boolean(AgsVstFVariant *fvariant,
				    gboolean val)
  {
    fvariant->set(static_cast<bool>(val));
  }
  
  void ags_vst_fvariant_set_uin32(AgsVstFVariant *fvariant,
				  guint32 val)
  {
    fvariant->set(val);
  }
  
  void ags_vst_fvariant_set_int64(AgsVstFVariant *fvariant,
				  gint64 val)
  {
    fvariant->set(val);
  }
  
  void ags_vst_fvariant_set_double(AgsVstFVariant *fvariant,
				   gdouble val)
  {
    fvariant->set(val);
  }
  
  void ags_vst_fvariant_set_string(AgsVstFVariant *fvariant,
				   gchar *str)
  {
    fvariant->set(str);
  }
  
  void ags_vst_fvariant_set_wstring(AgsVstFVariant *fvariant,
				    gunichar2 *str)
  {
    fvariant->set(str);
  }
  
  void ags_vst_fvariant_set_object(AgsVstFVariant *fvariant,
				   AgsVstFunknown *obj)
  {
    fvariant->set(obj);
  }
  
  gboolean ags_vst_fvariant_get_boolean(AgsVstFVariant *fvariant)
  {
    return(fvariant->get<bool>());
  }
  
  guint32 ags_vst_fvariant_get_uint32(AgsVstFVariant *fvariant)
  {
    return(fvariant->get<uint32>());
  }
  
  gint64 ags_vst_fvariant_get_int64(AgsVstFVariant *fvariant)
  {
    return(fvariant->get<int64>());
  }
  
  gdouble ags_vst_fvariant_get_double(AgsVstFVariant *fvariant)
  {
    return(fvariant->get<double>());
  }
  
  gchar* ags_vst_fvariant_get_string(AgsVstFVariant *fvariant)
  {
    return(fvariant->get<char8*>());
  }
  
  gunichar2* ags_vst_fvariant_get_wstring(AgsVstFVariant *fvariant)
  {
    return(fvariant->get<char16*>());
  }
  
  AgsVstFUnknown* ags_vst_fvariant_get_object(AgsVstFVariant *fvariant)
  {
    return(fvariant->get<FUnknown*>());
  }
  
  guint16 ags_vst_fvariant_get_type(AgsVstFVariant *fvariant)
  {
    return(fvariant->getType());
  }
  
  gboolean ags_vst_fvariant_is_empty(AgsVstFVariant *fvariant)
  {
    return(static_cast<gboolean>(fvariant->isEmpty()));
  }
  
  gboolean ags_vst_fvariant_is_owner(AgsVstFVariant *fvariant)
  {
    return(static_cast<gboolean>(fvariant->isOwner()));
  }
  
  gboolean ags_vst_fvariant_is_string(AgsVstFVariant *fvariant)
  {
    return(static_cast<gboolean>(fvariant->isString()));
  }
  
  void ags_vst_fvariant_set_owner(AgsVstFVariant *fvariant,
				  gboolean state)
  {
    fvariant->setOwner(static_cast<bool>(state));
  }
  
  void ags_vst_fvariant_empty(AgsVstFVariant *fvariant)
  {
    fvariant->empty()
  }  

}
