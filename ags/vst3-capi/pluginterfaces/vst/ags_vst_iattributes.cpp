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
  
  AgsVstTResult ags_vst_iattribute_list_set_int(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, gint64 value)
  {
    //TODO:JK: implement me
  }
  
  AgsVstTResult ags_vst_iattribute_list_get_int(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, gint64 *value)
  {
    //TODO:JK: implement me
  }
  
  AgsVstTResult ags_vst_iattribute_list_set_float(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, gdouble value)
  {
    //TODO:JK: implement me
  }
  
  AgsVstTResult ags_vst_iattribute_list_get_float(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, gdouble *value)
  {
    //TODO:JK: implement me
  }
  
  AgsVstTResult ags_vst_iattribute_list_set_string(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, ags_vst_tchar *value)
  {
    //TODO:JK: implement me
  }
  
  AgsVstTResult ags_vst_iattribute_list_get_string(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, ags_vst_tchar *value, guint size_in_bytes)
  {
    //TODO:JK: implement me
  }
  
  AgsVstTResult ags_vst_iattribute_list_set_binary(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, void *data, guint size_in_bytes)
  {
    //TODO:JK: implement me
  }
  
  AgsVstTResult ags_vst_iattribute_list_get_binary(AgsVstIAttributeList *iattribute_list, AgsVstAttrID id, void *data, guint size_in_bytes)
  {
    //TODO:JK: implement me
  }

}
