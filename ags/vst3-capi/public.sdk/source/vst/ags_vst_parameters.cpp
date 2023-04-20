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

#include <ags/vst3-capi/public.sdk/source/vst/ags_vst_parameters.h>

#include <public.sdk/source/vst/vstparameters.h>

#include <string.h>

extern "C" {

  /**
   * Instantiate new Steinberg::Vst::Parameter the constructor as a C99
   * compatible handle a void pointer.
   * 
   * @return the AgsVstParameter
   * 
   * @since 5.0.0
   */
  AgsVstParameter* ags_vst_parameter_new()
  {
    return((AgsVstParameter *) new Steinberg::Vst::Parameter());
  }
  
  /**
   * Instantiate new Steinberg::Vst::Parameter the constructor as a C99
   * compatible handle a void pointer.
   *
   * @param info the parameter info
   * @return the AgsVstParameter
   * 
   * @since 5.0.0
   */
  AgsVstParameter* ags_vst_parameter_new_with_info(AgsVstParameterInfo *info)
  {
    Steinberg::Vst::Parameter *tmp_info_0 = static_cast<Steinberg::Vst::Parameter*>((void *) info);
    const Steinberg::Vst::Parameter& tmp_info_1 = const_cast<Steinberg::Vst::Parameter&>(tmp_info_0[0]);
    
    return((AgsVstParameter *) new Steinberg::Vst::Parameter(const_cast<Steinberg::Vst::Parameter&>(tmp_info_1)));
  }
  
  /**
   * Instantiate new Steinberg::Vst::Parameter the constructor as a C99
   * compatible handle a void pointer.
   *
   * @param title the title
   * @param tag the tag
   * @param units the units
   * @param default_value_normalized default value normalized
   * @param step_count the step count
   * @param flags the flags
   * @param unit_id the unit identifier
   * @param short_title the short title
   * @return the AgsVstParameter
   * 
   * @since 5.0.0
   */
  AgsVstParameter* ags_vst_parameter_new_full(gunichar2 *title, guint32 tag, gunichar2 *units,
					      gdouble default_value_normalized, gint32 step_count,
					      gint32 flags, gint32 unit_id,
					      gunichar2 *short_title)
  {
    return((AgsVstParameter *) new Steinberg::Vst::Parameter((char16_t *) title, tag, (char16_t *) units,
							     default_value_normalized, step_count,
							     flags, unit_id,
							     (char16_t *) short_title));
  }
  
  /**
   * Delete.
   *
   * @param parameter the parameters
   *
   * @since 5.0.0
   */
  void ags_vst_parameter_delete(AgsVstParameter *parameter)
  {
    delete parameter;
  }  

  /**
   * Get info.
   *
   * @param parameter the parameter
   * @return the parameter info
   *
   * @since 5.0.0
   */
  AgsVstParameterInfo* ags_vst_parameter_get_info(AgsVstParameter *parameter)
  {
    const Steinberg::Vst::ParameterInfo& info = static_cast<Steinberg::Vst::Parameter*>((void *) parameter)->getInfo();
    Steinberg::Vst::ParameterInfo *tmp_info_0 = const_cast<Steinberg::Vst::ParameterInfo*>(&info);

    
    return((AgsVstParameterInfo *) tmp_info_0);
  }
  
  /**
   * Set unit identifier.
   *
   * @param parameter the parameter
   * @param id the identifier
   *
   * @since 5.0.0
   */
  void ags_vst_parameter_set_unit_id(AgsVstParameter *parameter,
				     gint32 id)
  {
    static_cast<Steinberg::Vst::Parameter*>((void *) parameter)->setUnitID(id);
  }
  
  /**
   * Get unit identifier.
   *
   * @param parameter the parameter
   * @return the unit identifier
   *
   * @since 5.0.0
   */
  gint32 ags_vst_parameter_get_unit_id(AgsVstParameter *parameter)
  {
    return(static_cast<Steinberg::Vst::Parameter*>((void *) parameter)->getUnitID());
  }  

  /**
   * Get normalized.
   *
   * @param parameter the parameter
   * @return the normalized value
   *
   * @since 5.0.0
   */
  gdouble ags_vst_parameter_get_normalized(AgsVstParameter *parameter)
  {
    return(static_cast<Steinberg::Vst::Parameter*>((void *) parameter)->getNormalized());
  }
  
  /**
   * Set normalized.
   *
   * @param parameter the parameter
   * @param v the normalized value
   * @return true if success, otherwise false
   *
   * @since 5.0.0
   */
  gboolean ags_vst_parameter_set_normalized(AgsVstParameter *parameter,
					    gdouble v)
  {
    return(static_cast<Steinberg::Vst::Parameter*>((void *) parameter)->setNormalized(v));
  }  

  /**
   * To string.
   *
   * @param parameter the parameter
   * @param value_normalized the normalized value
   * @param string the string
   *
   * @since 5.0.0
   */
  void ags_vst_parameter_to_string(AgsVstParameter *parameter,
				   gdouble value_normalized, AgsVstString128 string)
  {
    static_cast<Steinberg::Vst::Parameter*>((void *) parameter)->toString(value_normalized, (char16_t *) string);
  }

  /**
   * From string.
   *
   * @param parameter the parameter
   * @param string the string
   * @param value_normalized the normalized value
   * @return true if success, otherwise false
   *
   * @since 5.0.0
   */
  gboolean ags_vst_parameter_from_string(AgsVstParameter *parameter,
					 gunichar2 *string, gdouble *value_normalized)
  {
    Steinberg::Vst::ParamValue *tmp_value_normalized_0 = static_cast<Steinberg::Vst::ParamValue*>(value_normalized);
    const Steinberg::Vst::ParamValue& tmp_value_normalized_1 = const_cast<Steinberg::Vst::ParamValue&>(tmp_value_normalized_0[0]);
    
    return(static_cast<Steinberg::Vst::Parameter*>((void *) parameter)->fromString((char16_t *) string, const_cast<Steinberg::Vst::ParamValue&>(tmp_value_normalized_1)));
  }
  
  /**
   * To plain.
   *
   * @param parameter the parameter
   * @param value_normalized the normalized value
   * @return the plain value
   *
   * @since 5.0.0
   */
  gdouble ags_vst_parameter_to_plain(AgsVstParameter *parameter,
				     gdouble value_normalized)
  {
    return(static_cast<Steinberg::Vst::Parameter*>((void *) parameter)->toPlain(value_normalized));
  }
  
  /**
   * To normalized.
   *
   * @param parameter the parameter
   * @param plain_value the plain value
   * @return the normalized value
   *
   * @since 5.0.0
   */
  gdouble ags_vst_parameter_to_normalized(AgsVstParameter *parameter,
					  gdouble plain_value)
  {
    return(static_cast<Steinberg::Vst::Parameter*>((void *) parameter)->toNormalized(plain_value));
  }
  
  /**
   * Get precision.
   *
   * @param parameter the parameter
   * @return the precision
   *
   * @since 5.0.0
   */
  gint32 ags_vst_parameter_get_precision(AgsVstParameter *parameter)
  {
    return(static_cast<Steinberg::Vst::Parameter*>((void *) parameter)->getPrecision());
  }
  
  /**
   * Set precision.
   *
   * @param parameter the parameter
   * @param val the precision
   *
   * @since 5.0.0
   */
  void ags_vst_parameter_set_precision(AgsVstParameter *parameter,
				       gint32 val)
  {
    static_cast<Steinberg::Vst::Parameter*>((void *) parameter)->setPrecision(val);
  }

#if 0  
  AgsVstRangeParameter* ags_vst_range_parameter_new()
  {
    return((AgsVstRangeParameter *) new Steinberg::Vst::RangeParameter());
  }
#endif
  
  /**
   * Instantiate new Steinberg::Vst::RangeParameter the constructor as a C99
   * compatible handle a void pointer.
   *
   * @param param_info the parameter info
   * @param min the minimum
   * @param max the maximum
   * @return the AgsVstRangeParameter
   * 
   * @since 5.0.0
   */
  AgsVstRangeParameter* ags_vst_range_parameter_new_with_info(AgsVstParameterInfo *param_info, gdouble min, gdouble max)
  {
    Steinberg::Vst::ParameterInfo *tmp_param_info_0 = static_cast<Steinberg::Vst::ParameterInfo*>((void *) param_info);
    const Steinberg::Vst::ParameterInfo& tmp_param_info_1 = const_cast<Steinberg::Vst::ParameterInfo&>(tmp_param_info_0[0]);
    
    return((AgsVstRangeParameter *) new Steinberg::Vst::RangeParameter(tmp_param_info_1, static_cast<Steinberg::Vst::ParamValue>(min), static_cast<Steinberg::Vst::ParamValue>(max)));
  }
  
  /**
   * Instantiate new Steinberg::Vst::RangeParameter the constructor as a C99
   * compatible handle a void pointer.
   *
   * @param title the title
   * @param tag the tag
   * @param units the units
   * @param min_plain the plain minimum
   * @param max_plain the plain maximum
   * @param default_value_plain the default plain value
   * @param step_count the step count
   * @param flags the flags
   * @param unit_id the unit identifier
   * @param short_title the short title
   * @return the AgsVstRangeParameter
   * 
   * @since 5.0.0
   */
  AgsVstRangeParameter* ags_vst_range_parameter_new_full(gunichar2 *title, guint32 tag, gunichar2 *units,
							 gdouble min_plain, gdouble max_plain,
							 gdouble default_value_plain, gint32 step_count,
							 gint32 flags, gint32 unit_id,
							 gunichar2 *short_title)
  {
    return((AgsVstRangeParameter *) new Steinberg::Vst::RangeParameter((char16_t *) title, tag, (char16_t *) units,
								       min_plain, max_plain,
								       default_value_plain, step_count,
								       flags, unit_id,
								       (char16_t *) short_title));
  }

  /**
   * Get minimum.
   *
   * @param range_parameter the range parameter
   * @return the minimum
   * 
   * @since 5.0.0
   */
  gdouble ags_vst_range_parameter_get_min(AgsVstRangeParameter *range_parameter)
  {
    return(static_cast<Steinberg::Vst::RangeParameter*>((void *) range_parameter)->getMin());
  }
  
  /**
   * Set minimum.
   *
   * @param range_parameter the range parameter
   * @param value the minimum
   * 
   * @since 5.0.0
   */
  void ags_vst_range_parameter_set_min(AgsVstRangeParameter *range_parameter,
				       gdouble value)
  {
    static_cast<Steinberg::Vst::RangeParameter*>((void *) range_parameter)->setMin(value);
  }
  
  /**
   * Get maximum.
   *
   * @range_parameter the range parameter
   * @return the maximum
   * 
   * @since 5.0.0
   */
  gdouble ags_vst_range_parameter_get_max(AgsVstRangeParameter *range_parameter)
  {
    return(static_cast<Steinberg::Vst::RangeParameter*>((void *) range_parameter)->getMax());
  }
  
  /**
   * Set maximum.
   *
   * @param range_parameter the range parameter
   * @param value the maximum
   * 
   * @since 5.0.0
   */
  void ags_vst_range_parameter_set_max(AgsVstRangeParameter *range_parameter,
				       gdouble value)
  {
    static_cast<Steinberg::Vst::RangeParameter*>((void *) range_parameter)->setMax(value);
  }
  
  /**
   * To string.
   *
   * @param range_parameter the range parameter
   * @param value_normalized the normalized value
   * @param string the string
   * 
   * @since 5.0.0
   */
  void ags_vst_range_parameter_to_string(AgsVstRangeParameter *range_parameter,
					 gdouble value_normalized, AgsVstString128 string)
  {
    static_cast<Steinberg::Vst::RangeParameter*>((void *) range_parameter)->toString(value_normalized, (char16_t *) string);
  }
  
  /**
   * From string.
   *
   * @param range_parameter the range parameter
   * @param string the string
   * @param value_normalized the normalized value
   * @return true if success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_range_parameter_from_string(AgsVstRangeParameter *range_parameter,
					       gunichar2 *string, gdouble *value_normalized)
  {
    return(static_cast<Steinberg::Vst::RangeParameter*>((void *) range_parameter)->fromString((char16_t *) string, static_cast<Steinberg::Vst::ParamValue&>(value_normalized[0])));
  }

  /**
   * To plain.
   *
   * @param range_parameter the range parameter
   * @param value_normalized the normalized value
   * @return the plain value
   * 
   * @since 5.0.0
   */
  gdouble ags_vst_range_parameter_to_plain(AgsVstRangeParameter *range_parameter,
					   gdouble value_normalized)
  {
    return(static_cast<Steinberg::Vst::RangeParameter*>((void *) range_parameter)->toPlain(value_normalized));
  }
  
  /**
   * To normalized.
   *
   * @param range_parameter the range parameter
   * @param plain_value the plain value
   * @return the normalized value
   * 
   * @since 5.0.0
   */
  gdouble ags_vst_range_parameter_to_normalized(AgsVstRangeParameter *range_parameter,
						gdouble plain_value)
  {
    return(static_cast<Steinberg::Vst::RangeParameter*>((void *) range_parameter)->toNormalized(plain_value));
  }

#if 0
  AgsVstStringListParameter* ags_vst_string_list_parameter_new()
  {
    return((AgsVstStringListParameter *) new Steinberg::Vst::StringListParameter());
  }
#endif
  
  /**
   * Instantiate new Steinberg::Vst::StringListParameter the constructor as a C99
   * compatible handle a void pointer.
   *
   * @param param_info the parameter info
   * @return the AgsVstStringListParameter
   * 
   * @since 5.0.0
   */
  AgsVstStringListParameter* ags_vst_string_list_parameter_new_with_info(AgsVstParameterInfo *param_info)
  {
    Steinberg::Vst::ParameterInfo *tmp_param_info_0 = static_cast<Steinberg::Vst::ParameterInfo*>((void *) param_info);
    const Steinberg::Vst::ParameterInfo& tmp_param_info_1 = const_cast<Steinberg::Vst::ParameterInfo&>(tmp_param_info_0[0]);

    return((AgsVstStringListParameter *) new Steinberg::Vst::StringListParameter(tmp_param_info_1));
  }

  /**
   * Instantiate new Steinberg::Vst::StringListParameter the constructor as a C99
   * compatible handle a void pointer.
   *
   * @param title the title
   * @param tag the tag
   * @param units the units
   * @param flags the flags
   * @param unit_id the unit identifier
   * @param short_title the short title
   * @return the AgsVstStringListParameter
   * 
   * @since 5.0.0
   */
  AgsVstStringListParameter* ags_vst_string_list_parameter_new_full(gunichar2 *title, guint32 tag, gunichar2 *units,
								    gint32 flags,
								    gint32 unit_id, gunichar2 *short_title)
  {
    return((AgsVstStringListParameter *) new Steinberg::Vst::StringListParameter((char16_t *) title, tag, (char16_t *) units,
										 flags,
										 unit_id, (char16_t *) short_title));
  }

  /**
   * Append string.
   *
   * @param string_list_parameter the string list parameter
   * @param string the string
   * 
   * @since 5.0.0
   */
  void ags_vst_string_list_parameter_append_string(AgsVstStringListParameter *string_list_parameter,
						   AgsVstString128 string)
  {
    static_cast<Steinberg::Vst::StringListParameter*>((void *) string_list_parameter)->appendString((char16_t *) string);
  }
  
  /**
   * Replace string.
   *
   * @param string_list_parameter the string list parameter
   * @param index the index
   * @param string the string
   * @return true if success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_string_list_parameter_replace_string(AgsVstStringListParameter *string_list_parameter,
							gint32 index, AgsVstString128 string)
  {
    return(static_cast<Steinberg::Vst::StringListParameter*>((void *) string_list_parameter)->replaceString(index, (char16_t *) string));
  }
  
  /**
   * To string.
   *
   * @param string_list_parameter the string list parameter
   * @param value_normalized the value normalized
   * @param string the string
   * 
   * @since 5.0.0
   */
  void ags_vst_string_list_parameter_to_string(AgsVstStringListParameter *string_list_parameter,
					       gdouble value_normalized, AgsVstString128 string)
  {
    static_cast<Steinberg::Vst::StringListParameter*>((void *) string_list_parameter)->toString(_value_normalized, (char16_t *) string);
  }
  
  /**
   * From string.
   *
   * @param string_list_parameter the string list parameter
   * @param string the string
   * @param value_normalized the value normalized
   * @return true if success, otherwise false
   * 
   * @since 5.0.0
   */
  gboolean ags_vst_string_list_parameter_from_string(AgsVstStringListParameter *string_list_parameter,
						     gunichar2 *string, gdouble *value_normalized)
  {
    Steinberg::Vst::ParamValue *tmp_value_normalized_0 = static_cast<Steinberg::Vst::ParamValue*>(value_normalized);
    const Steinberg::Vst::ParamValue& tmp_value_normalized_1 = const_cast<Steinberg::Vst::ParamValue&>(tmp_value_normalized_0[0]);    
    
    return(static_cast<Steinberg::Vst::StringListParameter*>((void *) string_list_parameter)->fromString((char16_t *) string, const_cast<Steinberg::Vst::ParamValue&>(tmp_value_normalized_1)));
  }
  
  /**
   * To plain.
   *
   * @param string_list_parameter the string list parameter
   * @param value_normalized the value normalized
   * @return the plain value
   * 
   * @since 5.0.0
   */
  gdouble ags_vst_string_list_parameter_to_plain(AgsVstStringListParameter *string_list_parameter,
						 gdouble value_normalized)
  {
    return(static_cast<Steinberg::Vst::StringListParameter*>((void *) string_list_parameter)->toPlain(value_normalized));
  }
  
  /**
   * To normalized.
   *
   * @param string_list_parameter the string list parameter
   * @param plain_value the plain value
   * @return the normalized value
   * 
   * @since 5.0.0
   */
  gdouble ags_vst_string_list_parameter_to_normalized(AgsVstStringListParameter *string_list_parameter,
						      gdouble plain_value)
  {
    return(static_cast<Steinberg::Vst::StringListParameter*>((void *) string_list_parameter)->toNormalized(plain_value));
  }
  
  /**
   * Instantiate new Steinberg::Vst::ParameterContainer the constructor as a C99
   * compatible handle a void pointer.
   * 
   * @return the AgsVstParameterContainer
   * 
   * @since 5.0.0
   */
  AgsVstParameterContainer* ags_vst_parameter_container_new()
  {
    return((AgsVstParameterContainer *) new Steinberg::Vst::ParameterContainer());
  }
  
  /**
   * Init.
   *
   * @param parameter_container the parameter container
   * @param initial_size the initial size
   * @param resize_delta the resize delta
   * 
   * @since 5.0.0
   */
  void ags_vst_parameter_container_init(AgsVstParameterContainer *parameter_container,
					gint32 initial_size, gint32 resize_delta)
  {
    static_cast<Steinberg::Vst::ParameterContainer*>((void *) parameter_container)->init(initial_size, resize_delta);
  }
  
  /**
   * Add parameter with info.
   *
   * @param parameter_container the parameter container
   * @param info the parameter info
   * @return the parameter
   * 
   * @since 5.0.0
   */
  AgsVstParameter* ags_vst_parameter_container_add_parameter_with_info(AgsVstParameterContainer *parameter_container,
								       AgsVstParameterInfo *info)
  {
    Steinberg::Vst::ParameterInfo *tmp_info_0 = static_cast<Steinberg::Vst::ParameterInfo*>((void *) info);
    const Steinberg::Vst::ParameterInfo& tmp_info_1 = const_cast<Steinberg::Vst::ParameterInfo&>(tmp_info_0[0]);
    
    return((AgsVstParameter *) static_cast<Steinberg::Vst::ParameterContainer*>((void *) parameter_container)->addParameter(tmp_info_1));
  }  
  
  /**
   * Add parameter extended.
   *
   * @param parameter_container the parameter container
   * @param title the title
   * @param units the units
   * @param step_count the step count
   * @param default_value_normalized the default normalized value
   * @param flags the flags
   * @param tag the tag
   * @param unit_id the unit identifier
   * @param short_title the short title
   * @return the parameter
   * 
   * @since 5.0.0
   */
  AgsVstParameter* ags_vst_parameter_container_add_parameter_extended(AgsVstParameterContainer *parameter_container,
								      gunichar2 *title, gunichar2 *units, gint32 step_count,
								      gdouble default_value_normalized,
								      gint32 flags, gint32 tag,
								      gint32 unit_id, gunichar2 *short_title)
  {
    return((AgsVstParameter *) static_cast<Steinberg::Vst::ParameterContainer*>((void *) parameter_container)->addParameter((char16_t *) title, (char16_t *) units, step_count,
															    default_value_normalized,
															    flags, tag,
															    unit_id, (char16_t *) short_title));
  }
  
  /**
   * Add parameter.
   *
   * @param parameter_container the parameter container
   * @param p the parameter
   * @return the parameter
   * 
   * @since 5.0.0
   */
  AgsVstParameter* ags_vst_parameter_container_add_parameter(AgsVstParameterContainer *parameter_container,
							     AgsVstParameter *p)
  {
    return((AgsVstParameter *) static_cast<Steinberg::Vst::ParameterContainer*>((void *) parameter_container)->addParameter(static_cast<Steinberg::Vst::Parameter*>((void *) p)));
  }
  
  /**
   * Get parameter count.
   *
   * @param parameter_container the parameter container
   * @return the parameter count
   * 
   * @since 5.0.0
   */
  gint32 ags_vst_parameter_container_get_parameter_count(AgsVstParameterContainer *parameter_container)
  {
    return(static_cast<Steinberg::Vst::ParameterContainer*>((void *) parameter_container)->getParameterCount());
  }  

  /**
   * Get parameter by index.
   *
   * @param parameter_container the parameter container
   * @param index the index
   * @return the parameter
   * 
   * @since 5.0.0
   */
  AgsVstParameter* ags_vst_parameter_container_get_parameter_by_index(AgsVstParameterContainer *parameter_container,
								      gint32 index)
  {
    return((AgsVstParameter *) static_cast<Steinberg::Vst::ParameterContainer*>((void *) parameter_container)->getParameterByIndex(index));
  }
  
  /**
   * Remove all.
   *
   * @param parameter_container the parameter container
   * 
   * @since 5.0.0
   */
  void ags_vst_parameter_container_remove_all(AgsVstParameterContainer *parameter_container)
  {
    return(static_cast<Steinberg::Vst::ParameterContainer*>((void *) parameter_container)->removeAll());
  }
  
  /**
   * Get parameter.
   *
   * @param parameter_container the parameter container
   * @param tag the tag
   * 
   * @since 5.0.0
   */
  AgsVstParameter* ags_vst_parameter_container_get_parameter(AgsVstParameterContainer *parameter_container,
							     guint32 tag)
  {
    return((AgsVstParameter *) static_cast<Steinberg::Vst::ParameterContainer*>((void *) parameter_container)->getParameter(tag));
  }

}
