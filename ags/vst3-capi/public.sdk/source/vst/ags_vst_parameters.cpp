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

#include <ags/vst3-capi/public.sdk/source/vst/ags_vst_parameters.h>

#include <public.sdk/source/vst/vstparameters.h>

#include <string.h>

extern "C" {

  AgsVstParameter* ags_vst_parameter_new()
  {
    return(new Parameter());
  }
  
  AgsVstParameter* ags_vst_parameter_new_with_info(AgsVstParameterInfo **info)
  {
    return(new Parameter(static_cast<ParameterInfo&>(info[0])));
  }
  
  AgsVstParameter* ags_vst_parameter_new_full(gchar *title, guint32 tag, gchar *units,
					      gdouble default_value_normalized, gint32 step_count,
					      gint32 flags, gint32 unit_id,
					      gchar *short_title)
  {
    return(new Parameter(title, tag, units,
			 default_value_normalized, step_count,
			 flags, unit_id,
			 short_title));
  }
  
  void ags_vst_parameters_delete(AgsVstParameter *parameter)
  {
    delete parameter;
  }  

  AgsVstParameterInfo* ags_vst_parameter_get_info(AgsVstParameter *parameter)
  {
    return(static_cast<AgsVstParameterInfo*>(parameter->getInfo()));
  }
  
  void ags_vst_parameter_set_unit_id(AgsVstParameter *parameter,
				     gint32 id)
  {
    parameter->setUnitID(id);
  }
  
  gint32 ags_vst_parameter_get_unit_id(AgsVstParameter *parameter)
  {
    return(parameter->getUnitID());
  }  

  gdouble ags_vst_parameter_get_normalized(AgsVstParameter *parameter)
  {
    return(parameter->getNormalized());
  }
  
  gboolean ags_vst_parameter_set_normalized(AgsVstParameter *parameter,
					    gdouble v)
  {
    return(parameter->setNormalized(v));
  }  

  void ags_vst_parameter_to_string(AgsVstParameter *parameter,
				   gdouble value_normalized, gchar *string)
  {
    parameter->toString(value_normalized, string);
  }

  gboolean ags_vst_parameter_from_string(AgsVstParameter *parameter,
					 gchar *string, gdouble *value_normalized)
  {
    return(parameter->fromString(string, static_cast<ParamValue&>(value_normalized[0])));
  }
  
  gdouble ags_vst_parameter_to_plain(AgsVstParameter *parameter,
				     gdouble value_normalized)
  {
    return(parameter->toPlain(value_normalized));
  }
  
  gdouble ags_vst_parameter_to_normalized(AgsVstParameter *parameter,
					  gdouble plain_value)
  {
    return(parameter->toNormalized(plain_value));
  }
  
  gint32 ags_vst_parameter_get_precision(AgsVstParameter *parameter)
  {
    return(parameter->getPrecision());
  }
  
  void ags_vst_parameter_set_precision(AgsVstParameter *parameter,
				       gint32 val)
  {
    parameter->setPrecision(val);
  }
    
  AgsVstRangeParameter* ags_vst_range_parameter_new()
  {
    return(new RangeParameter());
  }
  
  AgsVstRangeParameter* ags_vst_range_parameter_new_with_info(AgsVstParameterInfo **param_info, gdouble min, gdouble max)
  {
    return(new RangeParameter(static_cast<ParameterInfo&>(param_info[0]), min, max));
  }
  
  AgsVstRangeParameter* ags_vst_range_parameter_new_full(gchar *title, guint32 tag, gchar *units,
							 gdouble min_plain, gdouble max_plain,
							 gdouble default_value_plain, gint32 step_count,
							 gint32 flags, gint32 unit_id,
							 gchar *short_title)
  {
    return(new RangeParameter(title, tag, units,
			      min_plain, max_plain,
			      default_value_plain, step_count,
			      flags, unit_id,
			      short_title));
  }
    
  gdouble ags_vst_range_parameter_get_min(AgsVstRangeParameter *range_parameter)
  {
    return(range_parameter->getMin());
  }
  
  void ags_vst_range_parameter_set_min(AgsVstRangeParameter *range_parameter,
				       gdouble value)
  {
    range_parameter->setMin(value);
  }
  
  gdouble ags_vst_range_parameter_get_max(AgsVstRangeParameter *range_parameter)
  {
    return(range_parameter->getMax());
  }
  
  void ags_vst_range_parameter_set_max(AgsVstRangeParameter *range_parameter,
				       gdouble value)
  {
    range_parameter->setMax(value);
  }
  
  void ags_vst_range_parameter_to_string(AgsVstRangeParameter *range_parameter,
					 gdouble _value_normalized, gchar *string)
  {
    range_parameter->toString(_value_normalized, string);
  }
  
  gboolean ags_vst_range_parameter_from_string(AgsVstRangeParameter *range_parameter,
					       gchar *string, gdouble *_value_normalized)
  {
    return(range_parameter->fromString(string, static_cast<ParamValue&>(_value_normalized[0])));
  }

  gdouble ags_vst_range_parameter_to_plain(AgsVstRangeParameter *range_parameter,
					   gdouble _value_normalized)
  {
    return(range_parameter->toPlain(_value_normalized));
  }
  
  gdouble ags_vst_range_parameter_to_normalized(AgsVstRangeParameter *range_parameter,
						gdouble plain_value)
  {
    return(range_parameter->toNormalized(plain_value));
  }
  
  AgsVstStringListParameter* ags_vst_string_list_parameter_new()
  {
    return(new StringListParameter());
  }
  
  AgsVstStringListParameter* ags_vst_string_list_parameter_new_with_info(AgsVstParameterInfo **param_info)
  {
    return(new StringListParameter(static_cast<ParameterInfo&>(param_info[0])));
  }

  AgsVstStringListParameter* ags_vst_string_list_parameter_new_full(gchar *title, guint32 tag, gchar *units,
								    gint32 flags,
								    gint32 unit_id, gchar *short_title)
  {
    return(new StringListParameter(title, tag, units,
				   flags,
				   unit_id, short_title));
  }

  void ags_vst_string_list_parameter_append_string(AgsVstStringListParameter *string_list_parameter,
						   gchar *string)
  {
    string_list_parameter->appendString(string);
  }
  
  gboolean ags_vst_string_list_parameter_replace_string(AgsVstStringListParameter *string_list_parameter,
							gint32 index, gchar *string)
  {
    return(string_list_parameter->replaceString(index, string);
  }
  
  void ags_vst_string_list_parameter_to_string(AgsVstStringListParameter *string_list_parameter,
					       gdouble _value_normalized, gchar *string)
  {
    string_list_parameter->toString(_value_normalized, string);
  }
  
  gboolean ags_vst_string_list_parameter_from_string(AgsVstStringListParameter *string_list_parameter,
						     gchar *string, gdouble *_value_normalized)
  {
    return(string_list_parameter->fromString(string, static_cast<ParamValue&>(_value_normalized[0])));
  }
  
  gdouble ags_vst_string_list_parameter_to_plain(AgsVstStringListParameter *string_list_parameter,
						 gdouble _value_normalized)
  {
    return(string_list_parameter->toPlain(_value_normalized));
  }
  
  gdouble ags_vst_string_list_parameter_to_normalized(AgsVstStringListParameter *string_list_parameter,
						      gdouble plain_value)
  {
    return(string_list_parameter->toNormalized(plain_value));
  }
  
  AgsVstParameterContainer* ags_vst_parameter_container_new()
  {
    return(new ParameterContainer());
  }
  
  void ags_vst_parameter_container_init(AgsVstParameterContainer *parameter_container,
					gint32 initial_size, gint32 resize_delta)
  {
    parameter_container->init(initial_size, resize_delta);
  }
  
  AgsVstParameter* ags_vst_parameter_container_add_parameter_with_info(AgsVstParameterContainer *parameter_container,
								       AgsVstParameterInfo **info)
  {
    return(parameter_container->addParameterWithInfo(static_cast<ParameterInfo&>(info[0])));
  }  

  AgsVstParameter* ags_vst_parameter_container_add_parameter_extended(AgsVstParameterContainer *parameter_container,
								      gchar *title, gchar *units, gint32 step_count,
								      gdouble default_value_normalized,
								      gint32 flags, gint32 tag,
								      gint32 unit_id, gchar *short_title)
  {
    return(parameter_container->addParameterExtended(title, units, step_count,
						     default_value_normalized,
						     flags, tag,
						     unit_id, short_title));
  }
  
  AgsVstParameter* ags_vst_parameter_container_add_parameter(AgsVstParameterContainer *parameter_container,
							     AgsVstParameter *p)
  {
    return(parameter_container->addParameter(p));
  }
  
  gint32 ags_vst_parameter_container_get_parameter_count(AgsVstParameterContainer *parameter_container)
  {
    return(parameter_container->getParameterCount());
  }  

  AgsVstParameter* ags_vst_parameter_container_get_parameter_by_index(AgsVstParameterContainer *parameter_container,
								      gint32 index)
  {
    return(parameter_container->getParameterByIndex(index));
  }
  
  void ags_vst_parameter_container_remove_all(AgsVstParameterContainer *parameter_container)
  {
    return(parameter_container->removeAll());
  }
  
  AgsVstParameter* ags_vst_parameter_container_get_parameter(AgsVstParameterContainer *parameter_container,
							     guint32 tag)
  {
    return(parameter_container->getParameter(tag));
  }

}
