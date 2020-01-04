/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/util/ags_soundcard_helper.h>

#include <ags/object/ags_soundcard.h>

/**
 * SECTION:ags_soundcard_util
 * @short_description: soundcard util
 * @title: AgsSoundcardUtil
 * @section_id:
 * @include: ags/util/ags_soundcard_util.h
 *
 * Common utility functions related to #AgsSoundcard.
 */

/**
 * ags_soundcard_helper_config_get_dsp_channels:
 * @config: the #AgsConfig
 * 
 * Get dsp channels count.
 * 
 * Returns: the count of dsp channels
 * 
 * Since: 3.0.0
 */
guint
ags_soundcard_helper_config_get_dsp_channels(AgsConfig *config)
{
  gchar *str;

  guint dsp_channels;

  if(!AGS_IS_CONFIG(config)){
    return(AGS_SOUNDCARD_DEFAULT_DSP_CHANNELS);
  }
  
  /* dsp-channels */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "dsp-channels");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "dsp-channels");
  }
  
  if(str != NULL){
    dsp_channels = g_ascii_strtoull(str,
				   NULL,
				   10);
    g_free(str);
  }else{
    dsp_channels = AGS_SOUNDCARD_DEFAULT_DSP_CHANNELS;
  }

  return(dsp_channels);
}

/**
 * ags_soundcard_helper_config_get_pcm_channels:
 * @config: the #AgsConfig
 * 
 * Get pcm channels count.
 * 
 * Returns: the count of pcm channels
 * 
 * Since: 3.0.0
 */
guint
ags_soundcard_helper_config_get_pcm_channels(AgsConfig *config)
{
  gchar *str;

  guint pcm_channels;

  if(!AGS_IS_CONFIG(config)){
    return(AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS);
  }
  
  /* pcm-channels */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "pcm-channels");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "pcm-channels");
  }
  
  if(str != NULL){
    pcm_channels = g_ascii_strtoull(str,
				   NULL,
				   10);
    g_free(str);
  }else{
    pcm_channels = AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS;
  }

  return(pcm_channels);
}

/**
 * ags_soundcard_helper_config_get_samplerate:
 * @config: the #AgsConfig
 * 
 * Get samplerate.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.0.0
 */
gdouble
ags_soundcard_helper_config_get_samplerate(AgsConfig *config)
{
  gchar *str;
  
  gdouble samplerate;
  
  if(!AGS_IS_CONFIG(config)){
    return(AGS_SOUNDCARD_DEFAULT_SAMPLERATE);
  }

  /* samplerate */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "samplerate");
  
  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "samplerate");
  }  

  if(str != NULL){
    samplerate = g_ascii_strtod(str,
				NULL);
    g_free(str);
  }else{
    samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  }

  return(samplerate);
}

/**
 * ags_soundcard_helper_config_get_buffer_size:
 * @config: the #AgsConfig
 * 
 * Get buffer size.
 * 
 * Returns: the buffer size
 * 
 * Since: 3.0.0
 */
guint
ags_soundcard_helper_config_get_buffer_size(AgsConfig *config)
{
  gchar *str;

  guint buffer_size;

  if(!AGS_IS_CONFIG(config)){
    return(AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE);
  }
  
  /* buffer-size */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "buffer-size");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "buffer-size");
  }
  
  if(str != NULL){
    buffer_size = g_ascii_strtoull(str,
				   NULL,
				   10);
    g_free(str);
  }else{
    buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  }

  return(buffer_size);
}

/**
 * ags_soundcard_helper_config_get_format:
 * @config: the #AgsConfig
 * 
 * Get format as #AgsSoundcardFormat-enum.
 * 
 * Returns: the format
 * 
 * Since: 3.0.0
 */
guint
ags_soundcard_helper_config_get_format(AgsConfig *config)
{
  gchar *str;

  guint format;

  if(!AGS_IS_CONFIG(config)){
    return(AGS_SOUNDCARD_DEFAULT_FORMAT);
  }
  
  /* format */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "format");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "format");
  }
  
  if(str != NULL){
    format = g_ascii_strtoull(str,
			      NULL,
			      10);
    g_free(str);
  }else{
    format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  }

  return(format);
}
