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

#include <ags/audio/ags_fx_factory.h>

#include <ags/audio/fx/ags_fx_notation_audio.h>
#include <ags/audio/fx/ags_fx_notation_audio_processor.h>
#include <ags/audio/fx/ags_fx_notation_channel.h>
#include <ags/audio/fx/ags_fx_notation_channel_processor.h>
#include <ags/audio/fx/ags_fx_notation_recycling.h>
#include <ags/audio/fx/ags_fx_notation_audio_signal.h>

#include <ags/audio/fx/ags_fx_dssi_audio.h>
#include <ags/audio/fx/ags_fx_dssi_audio_processor.h>
#include <ags/audio/fx/ags_fx_dssi_channel.h>
#include <ags/audio/fx/ags_fx_dssi_channel_processor.h>
#include <ags/audio/fx/ags_fx_dssi_recycling.h>
#include <ags/audio/fx/ags_fx_dssi_audio_signal.h>

/**
 * SECTION:ags_fx_factory
 * @short_description: Factory pattern
 * @title: AgsFxFactory
 * @section_id:
 * @include: ags/audio/ags_fx_factory.h
 *
 * Factory function to instantiate fx recalls.
 */

/**
 * ags_recall_factory_create:
 * @audio: an #AgsAudio that should keep the recalls
 * @play_container: an #AgsRecallContainer to indetify what recall to use
 * @recall_container: an #AgsRecallContainer to indetify what recall to use
 * @plugin_name: the plugin identifier to instantiate
 * @filename: the plugin filename
 * @effect: the plugin effect
 * @start_audio_channel: the first audio channel to apply
 * @stop_audio_channel: the last audio channel to apply
 * @start_pad: the first pad to apply
 * @stop_pad: the last pad to apply
 * @create_flags: modify the behaviour of this function
 * @recall_flags: flags to be set for #AgsRecall
 *
 * Instantiate #AgsRecall by this factory.
 *
 * Returns: (element-type AgsAudio.Recall) (transfer full): The #GList-struct containing #AgsRecall
 * 
 * Since: 3.3.0
 */
GList*
ags_fx_factory_create(AgsAudio *audio,
		      AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
		      gchar *plugin_name,
		      gchar *filename,
		      gchar *effect,
		      guint start_audio_channel, guint stop_audio_channel,
		      guint start_pad, guint stop_pad,
		      guint create_flags, guint recall_flags);
{
  GList *start_recall;

  if(plugin_name == NULL){
    return(NULL);
  }
  
  start_recall = NULL;

  if(!g_ascii_strncasecmp(plugin_name,
			  "ags-fx-buffer",
			  18)){
    //TODO:JK: implement me
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-play",
				12)){
    //TODO:JK: implement me
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-volume",
				14)){
    //TODO:JK: implement me
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-peak",
				12)){
    //TODO:JK: implement me
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-eq10",
				12)){
    //TODO:JK: implement me
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-analyse",
				15)){
    //TODO:JK: implement me
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-envelope",
				16)){
    //TODO:JK: implement me
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-wave",
				12)){
    //TODO:JK: implement me
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-pattern",
				15)){
    //TODO:JK: implement me
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-notation",
				16)){
    //TODO:JK: implement me
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-ladspa",
				14)){
    //TODO:JK: implement me
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-dssi",
				12)){
    //TODO:JK: implement me
  }else if(!g_ascii_strncasecmp(plugin_name,
				"ags-fx-lv2",
				11)){
    //TODO:JK: implement me
  }else{
    g_warning("no such plugin - %s", plugin_name);
  }

  return(start_recall);
}
