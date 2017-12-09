/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/machine/ags_drum_output_line_callbacks.h>
#include <ags/X/machine/ags_drum.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

void
ags_drum_output_line_resize_pads_callback(AgsDrum *drum, GType channel_type,
					  guint pads_new, guint pads_old,
					  AgsDrumOutputLine *output_line)
{
  AgsAudio *audio;
  
  AgsConfig *config;

  gchar *str;

  gboolean performance_mode;

  audio = drum->audio;
  
  config = ags_config_get_instance();
  
  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "engine-mode");
  performance_mode = FALSE;

  if(channel_type == AGS_TYPE_INPUT){
    if(pads_new > pads_old){
      if(str != NULL &&
	 !g_ascii_strncasecmp(str,
			      "performance",
			      12)){
	/* ags-copy */
	ags_recall_factory_create(audio,
				  NULL, NULL,
				  "ags-copy",
				  0, audio->audio_channels, 
				  pads_old, pads,
				  (AGS_RECALL_FACTORY_INPUT |
				   AGS_RECALL_FACTORY_RECALL |
				   AGS_RECALL_FACTORY_ADD),
				  0);
	
	/* set performance mode */
	performance_mode = TRUE;
      }else{
	/* ags-buffer */
	ags_recall_factory_create(audio,
				  NULL, NULL,
				  "ags-buffer",
				  0, audio->audio_channels, 
				  pads_old, pads,
				  (AGS_RECALL_FACTORY_INPUT |
				   AGS_RECALL_FACTORY_RECALL |
				   AGS_RECALL_FACTORY_ADD),
				  0);
      }
    }
  }
}
