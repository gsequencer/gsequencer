/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/machine/ags_ffplayer_input_line_callbacks.h>

void
ags_ffplayer_input_line_notify_channel_callback(GObject *gobject,
						GParamSpec *pspec,
						gpointer user_data)
{
  AgsFFPlayerInputLine *ffplayer_input_line;

  gchar *str;
  
  ffplayer_input_line = AGS_FFPLAYER_INPUT_LINE(gobject);

  if(AGS_EFFECT_LINE(ffplayer_input_line)->channel == NULL){
    return;
  }

  str = g_strdup_printf("in: %d, %d",
			AGS_EFFECT_LINE(ffplayer_input_line)->channel->pad + 1,
			AGS_EFFECT_LINE(ffplayer_input_line)->channel->audio_channel + 1);
  gtk_label_set_text(AGS_EFFECT_LINE(ffplayer_input_line)->label,
		     str);

  g_free(str);
}

