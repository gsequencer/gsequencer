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

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_fvariant.h>

#include <pluginterfaces/base/fvariant.h>

using namespace Steinberg;

extern "C" {

  AgsVstFVariant* ags_vst_fvariant_new()
  {
    return((AgsVstFVariant *) new Steinberg::FVariant());
  }
  
  AgsVstFVariant* ags_vst_fvariant_new_boolean(gboolean val)
  {
    return((AgsVstFVariant *) new Steinberg::FVariant(static_cast<bool>(val)));
  }
  
  AgsVstFVariant* ags_vst_fvariant_new_uint32(guint32 val)
  {
    return((AgsVstFVariant *) new Steinberg::FVariant(static_cast<uint32>(val)));
  }
  
  AgsVstFVariant* ags_vst_fvariant_new_int64(gint64 val)
  {
    return((AgsVstFVariant *) new Steinberg::FVariant(static_cast<int64>(val)));
  }
  
  AgsVstFVariant* ags_vst_fvariant_new_double(gdouble val)
  {
    return((AgsVstFVariant *) new Steinberg::FVariant(static_cast<double>(val)));
  }
  
  AgsVstFVariant* ags_vst_fvariant_new_string(gchar *str)
  {
    return((AgsVstFVariant *) new Steinberg::FVariant(static_cast<const char8 *>(str)));
  }
  
  AgsVstFVariant* ags_vst_fvariant_new_wstring(gunichar2 *str)
  {
    return((AgsVstFVariant *) new Steinberg::FVariant(str));
  }
  
  AgsVstFVariant* ags_vst_fvariant_new_funknown(AgsVstFUnknown *funknown, gboolean owner)
  {
    return((AgsVstFVariant *) new Steinberg::FVariant((Steinberg::FUnknown *) funknown, static_cast<bool>(owner)));
  }
  
  void ags_vst_fvariant_delete(AgsVstFVariant *fvariant)
  {
    delete fvariant;
  }
  
  void ags_vst_fvariant_set_boolean(AgsVstFVariant *fvariant,
				    gboolean val)
  {
    ((Steinberg::FVariant *) fvariant)->set(static_cast<bool>(val));
  }
  
  void ags_vst_fvariant_set_uin32(AgsVstFVariant *fvariant,
				  guint32 val)
  {
    ((Steinberg::FVariant *) fvariant)->set(static_cast<uint32>(val));
  }
  
  void ags_vst_fvariant_set_int64(AgsVstFVariant *fvariant,
				  gint64 val)
  {
    ((Steinberg::FVariant *) fvariant)->set(static_cast<int64>(val));
  }
  
  void ags_vst_fvariant_set_double(AgsVstFVariant *fvariant,
				   gdouble val)
  {
    ((Steinberg::FVariant *) fvariant)->set(static_cast<double>(val));
  }
  
  void ags_vst_fvariant_set_string(AgsVstFVariant *fvariant,
				   gchar *str)
  {
    ((Steinberg::FVariant *) fvariant)->set(static_cast<char8 *>(str));
  }
  
  void ags_vst_fvariant_set_wstring(AgsVstFVariant *fvariant,
				    gunichar2 *str)
  {
    ((Steinberg::FVariant *) fvariant)->set(reinterpret_cast<char16 *>(str));
  }
  
  void ags_vst_fvariant_set_object(AgsVstFVariant *fvariant,
				   AgsVstFUnknown *obj)
  {
    ((Steinberg::FVariant *) fvariant)->set((Steinberg::FUnknown *) obj);
  }
  
  gboolean ags_vst_fvariant_get_boolean(AgsVstFVariant *fvariant)
  {
    return(((Steinberg::FVariant *) fvariant)->get<bool>());
  }
  
  guint32 ags_vst_fvariant_get_uint32(AgsVstFVariant *fvariant)
  {
    return(((Steinberg::FVariant *) fvariant)->get<uint32>());
  }
  
  gint64 ags_vst_fvariant_get_int64(AgsVstFVariant *fvariant)
  {
    return(((Steinberg::FVariant *) fvariant)->get<int64>());
  }
  
  gdouble ags_vst_fvariant_get_double(AgsVstFVariant *fvariant)
  {
    return(((Steinberg::FVariant *) fvariant)->get<double>());
  }
  
  gchar* ags_vst_fvariant_get_string(AgsVstFVariant *fvariant)
  {
    return(((Steinberg::FVariant *) fvariant)->get<char8*>());
  }
  
  gunichar2* ags_vst_fvariant_get_wstring(AgsVstFVariant *fvariant)
  {
    return(reinterpret_cast<gunichar2 *>(((Steinberg::FVariant *) fvariant)->get<char16*>()));
  }
  
  AgsVstFUnknown* ags_vst_fvariant_get_object(AgsVstFVariant *fvariant)
  {
    return((AgsVstFUnknown *) ((Steinberg::FVariant *) fvariant)->getObject());
  }
  
  guint16 ags_vst_fvariant_get_type(AgsVstFVariant *fvariant)
  {
    return(((Steinberg::FVariant *) fvariant)->getType());
  }
  
  gboolean ags_vst_fvariant_is_empty(AgsVstFVariant *fvariant)
  {
    return(static_cast<gboolean>(((Steinberg::FVariant *) fvariant)->isEmpty()));
  }
  
  gboolean ags_vst_fvariant_is_owner(AgsVstFVariant *fvariant)
  {
    return(static_cast<gboolean>(((Steinberg::FVariant *) fvariant)->isOwner()));
  }
  
  gboolean ags_vst_fvariant_is_string(AgsVstFVariant *fvariant)
  {
    return(static_cast<gboolean>(((Steinberg::FVariant *) fvariant)->isString()));
  }
  
  void ags_vst_fvariant_set_owner(AgsVstFVariant *fvariant,
				  gboolean state)
  {
    ((Steinberg::FVariant *) fvariant)->setOwner(static_cast<bool>(state));
  }
  
  void ags_vst_fvariant_empty(AgsVstFVariant *fvariant)
  {
    ((Steinberg::FVariant *) fvariant)->empty();
  }  

}
