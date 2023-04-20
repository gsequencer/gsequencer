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

#ifndef __AGS_VST_PARAMETERS_H__
#define __AGS_VST_PARAMETERS_H__

#include <ags/vst3-capi/base/source/ags_vst_fobject.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iedit_controller.h>
#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_iunits.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct AgsVstParameter Parameter;

  typedef struct AgsVstRangeParameter RangeParameter;

  typedef struct AgsVstStringListParameter StringListParameter;

  typedef struct AgsVstParameterContainer ParameterContainer;
  
  AgsVstParameter* ags_vst_parameter_new();
  AgsVstParameter* ags_vst_parameter_new_with_info(AgsVstParameterInfo *info);
  AgsVstParameter* ags_vst_parameter_new_full(gunichar2 *title, guint32 tag, gunichar2 *units,
					      gdouble default_value_normalized, gint32 step_count,
					      gint32 flags, gint32 unit_id,
					      gunichar2 *short_title);
  
  void ags_vst_parameter_delete(AgsVstParameter *parameter);

  AgsVstParameterInfo* ags_vst_parameter_get_info(AgsVstParameter *parameter);

  void ags_vst_parameter_set_unit_id(AgsVstParameter *parameter,
				     gint32 id);
  gint32 ags_vst_parameter_get_unit_id(AgsVstParameter *parameter);

  gdouble ags_vst_parameter_get_normalized(AgsVstParameter *parameter);
  gboolean ags_vst_parameter_set_normalized(AgsVstParameter *parameter,
					    gdouble v);

  void ags_vst_parameter_to_string(AgsVstParameter *parameter,
				   gdouble value_normalized, AgsVstString128 string);
  gboolean ags_vst_parameter_from_string(AgsVstParameter *parameter,
					 gunichar2 *string, gdouble *value_normalized);
  gdouble ags_vst_parameter_to_plain(AgsVstParameter *parameter,
				     gdouble value_normalized);
  gdouble ags_vst_parameter_to_normalized(AgsVstParameter *parameter,
					  gdouble plain_value);
  gint32 ags_vst_parameter_get_precision(AgsVstParameter *parameter);
  void ags_vst_parameter_set_precision(AgsVstParameter *parameter,
				       gint32 val);
  
  AgsVstRangeParameter* ags_vst_range_parameter_new_with_info(AgsVstParameterInfo *param_info, gdouble min, gdouble max);
  AgsVstRangeParameter* ags_vst_range_parameter_new_full(gunichar2 *title, guint32 tag, gunichar2 *units,
							 gdouble min_plain, gdouble max_plain,
							 gdouble default_value_plain, gint32 step_count,
							 gint32 flags, gint32 unit_id,
							 gunichar2 *short_title);
  
  gdouble ags_vst_range_parameter_get_min(AgsVstRangeParameter *range_parameter);
  void ags_vst_range_parameter_set_min(AgsVstRangeParameter *range_parameter,
				       gdouble value);
  gdouble ags_vst_range_parameter_get_max(AgsVstRangeParameter *range_parameter);
  void ags_vst_range_parameter_set_max(AgsVstRangeParameter *range_parameter,
				       gdouble value);

  void ags_vst_range_parameter_to_string(AgsVstRangeParameter *range_parameter,
					 gdouble value_normalized, AgsVstString128 string);
  gboolean ags_vst_range_parameter_from_string(AgsVstRangeParameter *range_parameter,
					       gunichar2 *string, gdouble *value_normalized);

  gdouble ags_vst_range_parameter_to_plain(AgsVstRangeParameter *range_parameter,
					   gdouble value_normalized);
  gdouble ags_vst_range_parameter_to_normalized(AgsVstRangeParameter *range_parameter,
						gdouble plain_value);

  AgsVstStringListParameter* ags_vst_string_list_parameter_new_with_info(AgsVstParameterInfo *param_info);
  AgsVstStringListParameter* ags_vst_string_list_parameter_new_full(gunichar2 *title, guint32 tag, gunichar2 *units,
								    gint32 flags,
								    gint32 unit_id, gunichar2 *short_title);

  void ags_vst_string_list_parameter_append_string(AgsVstStringListParameter *string_list_parameter,
						   AgsVstString128 string);
  gboolean ags_vst_string_list_parameter_replace_string(AgsVstStringListParameter *string_list_parameter,
							gint32 index, AgsVstString128 string);

  void ags_vst_string_list_parameter_to_string(AgsVstStringListParameter *string_list_parameter,
					       gdouble value_normalized, AgsVstString128 string);
  gboolean ags_vst_string_list_parameter_from_string(AgsVstStringListParameter *string_list_parameter,
						     gunichar2 *string, gdouble *_value_normalized);

  gdouble ags_vst_string_list_parameter_to_plain(AgsVstStringListParameter *string_list_parameter,
						 gdouble value_normalized);
  gdouble ags_vst_string_list_parameter_to_normalized(AgsVstStringListParameter *string_list_parameter,
						      gdouble plain_value);

  AgsVstParameterContainer* ags_vst_parameter_container_new();

  void ags_vst_parameter_container_init(AgsVstParameterContainer *parameter_container,
					gint32 initial_size, gint32 resize_delta);

  AgsVstParameter* ags_vst_parameter_container_add_parameter_with_info(AgsVstParameterContainer *parameter_container,
								       AgsVstParameterInfo *info);

  AgsVstParameter* ags_vst_parameter_container_add_parameter_extended(AgsVstParameterContainer *parameter_container,
								      gunichar2 *title, gunichar2 *units, gint32 step_count,
								      gdouble default_value_normalized,
								      gint32 flags, gint32 tag,
								      gint32 unit_id, gunichar2 *short_title);
  
  AgsVstParameter* ags_vst_parameter_container_add_parameter(AgsVstParameterContainer *parameter_container,
							     AgsVstParameter *p);

  gint32 ags_vst_parameter_container_get_parameter_count(AgsVstParameterContainer *parameter_container);

  AgsVstParameter* ags_vst_parameter_container_get_parameter_by_index(AgsVstParameterContainer *parameter_container,
								      gint32 index);

  void ags_vst_parameter_container_remove_all(AgsVstParameterContainer *parameter_container);
  AgsVstParameter* ags_vst_parameter_container_get_parameter(AgsVstParameterContainer *parameter_container,
							     guint32 tag);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_PARAMETERS_H__*/
