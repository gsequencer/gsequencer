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

#include <ags/X/machine/ags_ffplayer_input_line_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

void
ags_ffplayer_input_line_notify_channel_callback(GObject *gobject,
						GParamSpec *pspec,
						gpointer user_data)
{
  AgsFFPlayerInputLine *ffplayer_input_line;

  AgsChannel *channel;

  AgsMutexManager *mutex_manager;

  gchar *str;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *channel_mutex;

  ffplayer_input_line = AGS_FFPLAYER_INPUT_LINE(gobject);

  if(AGS_EFFECT_LINE(ffplayer_input_line)->channel == NULL){
    return;
  }

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  channel = AGS_EFFECT_LINE(ffplayer_input_line)->channel;
  
  /* get channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(channel_mutex);

  str = g_strdup_printf("in: %d, %d",
			channel->pad + 1,
			channel->audio_channel + 1);

  pthread_mutex_unlock(channel_mutex);

  gtk_label_set_text(AGS_EFFECT_LINE(ffplayer_input_line)->label,
		     str);

  g_free(str);
}

