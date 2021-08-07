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

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iattributes.h>

#include <pluginterfaces/vst/ivstattributes.h>

extern "C" {

  const AgsVstTUID* ags_vst_attribute_list_get_iid()
  {
    extern const Steinberg::TUID IAttributeList__iid;

    return((AgsVstTUID *) (&IAttributeList__iid));
  }
  
  AgsVstTResult ags_vst_iattribute_list_set_int(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, gint64 value)
  {
    return(((Steinberg::Vst::IAttributeList *) iattribute_list)->setInt(id, value));
  }
  
  AgsVstTResult ags_vst_iattribute_list_get_int(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, gint64 *value)
  {
    Steinberg::int64 *tmp_value_0 = (Steinberg::int64 *) value; 
    const Steinberg::int64& tmp_value_1 = const_cast<Steinberg::int64&>(tmp_value_0[0]);
    
    return(((Steinberg::Vst::IAttributeList *) iattribute_list)->getInt(id, const_cast<Steinberg::int64&>(tmp_value_1)));
  }
  
  AgsVstTResult ags_vst_iattribute_list_set_float(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, gdouble value)
  {
    return(((Steinberg::Vst::IAttributeList *) iattribute_list)->setFloat(id, value));
  }
  
  AgsVstTResult ags_vst_iattribute_list_get_float(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, gdouble *value)
  {
    double *tmp_value_0 = (double *) value; 
    const double& tmp_value_1 = const_cast<double&>(tmp_value_0[0]);
    
    return(((Steinberg::Vst::IAttributeList *) iattribute_list)->getFloat(id, const_cast<double&>(tmp_value_1)));
  }
  
  AgsVstTResult ags_vst_iattribute_list_set_string(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, AgsVstTChar *value)
  {
    return(((Steinberg::Vst::IAttributeList *) iattribute_list)->setString(id, (Steinberg::Vst::TChar *) value));
  }
  
  AgsVstTResult ags_vst_iattribute_list_get_string(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, AgsVstTChar *value, guint32 size_in_bytes)
  {
    return(((Steinberg::Vst::IAttributeList *) iattribute_list)->getString(id, (Steinberg::Vst::TChar *) value, size_in_bytes));
  }
  
  AgsVstTResult ags_vst_iattribute_list_set_binary(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, const void *data, guint32 size_in_bytes)
  {
    return(((Steinberg::Vst::IAttributeList *) iattribute_list)->setBinary(id, data, size_in_bytes));
  }
  
  AgsVstTResult ags_vst_iattribute_list_get_binary(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, const void *data, guint32 size_in_bytes)
  {
    const void*& tmp_value_0 = const_cast<const void*&>(data);
    
    return(((Steinberg::Vst::IAttributeList *) iattribute_list)->getBinary(id, tmp_value_0, size_in_bytes));
  }

  const AgsVstTUID* ags_vst_istream_attributes_get_iid()
  {
    extern const Steinberg::TUID IStreamAttributes__iid;

    return((AgsVstTUID *) (&IStreamAttributes__iid));
  }

  AgsVstIAttributeList* ags_vst_istream_attributes_get_attributes(AgsVstIStreamAttributes *istream_attributes)
  {
    return((AgsVstIAttributeList *) ((Steinberg::Vst::IStreamAttributes *) istream_attributes)->getAttributes());
  }

}
