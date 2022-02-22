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

#include <ags/app/machine/ags_ffplayer_input_line_callbacks.h>

void
ags_ffplayer_input_line_notify_channel_callback(GObject *gobject,
						GParamSpec *pspec,
						gpointer user_data)
{
  AgsFFPlayerInputLine *ffplayer_input_line;

  AgsChannel *channel;

  gchar *str;

  guint pad, audio_channel;

  ffplayer_input_line = AGS_FFPLAYER_INPUT_LINE(gobject);

  if(AGS_EFFECT_LINE(ffplayer_input_line)->channel == NULL){
    return;
  }

  channel = AGS_EFFECT_LINE(ffplayer_input_line)->channel;
  
  /* get channel properties */
  g_object_get(channel,
	       "pad", &pad,
	       "audio-channel", &audio_channel,
	       NULL);

  /* create label */
  str = g_strdup_printf("in: %d, %d",
			pad + 1,
			audio_channel + 1);

  gtk_label_set_text(AGS_EFFECT_LINE(ffplayer_input_line)->label,
		     str);

  g_free(str);
}
