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

#include <ags/app/machine/ags_desk_callbacks.h>

#include <ags/app/ags_window.h>

void
ags_desk_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsDesk *desk)
{
  AgsWindow *window;

  gchar *str;
  
  if(old_parent != NULL){
    return;
  }

  window = AGS_WINDOW(gtk_widget_get_ancestor((GtkWidget *) desk, AGS_TYPE_WINDOW));

  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_DESK)->counter);

  g_object_set(AGS_MACHINE(desk),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_DESK);
  g_free(str);
}

void
ags_desk_resize_audio_channels_callback(AgsDesk *desk,
					guint audio_channels, guint audio_channels_old,
					gpointer data)
{
  AgsAudio *audio;

  audio = AGS_MACHINE(desk)->audio;
  
  if(audio_channels > audio_channels_old){
    if((AGS_MACHINE_MAPPED_RECALL & (AGS_MACHINE(desk)->flags)) != 0){
      //TODO:JK: implement me
    }
  }else{
    //TODO:JK: implement me
  }
}

void
ags_desk_resize_pads_callback(AgsDesk *desk,
			      GType channel_type,
			      guint pads, guint pads_old,
			      gpointer data)
{  
  if(pads_old == 0 && channel_type == AGS_TYPE_INPUT){
    if((AGS_MACHINE_MAPPED_RECALL & (AGS_MACHINE(desk)->flags)) != 0){
      //TODO:JK: implement me
    }
  }
  
  if(pads == 0 && channel_type == AGS_TYPE_INPUT){
    //TODO:JK: implement me
  }
}
