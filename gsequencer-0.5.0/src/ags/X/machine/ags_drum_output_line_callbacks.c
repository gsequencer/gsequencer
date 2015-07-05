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

#include <ags/X/machine/ags_drum_output_line_callbacks.h>

#include <ags/X/machine/ags_drum.h>

#include <ags/audio/ags_audio_signal.h>

#include <ags/audio/recall/ags_delay_audio.h>

int
ags_drum_output_line_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, gpointer data)
{
  AgsDrumOutputLine *drum_output_line;

  if(old_parent != NULL)
    return(0);

  drum_output_line = AGS_DRUM_OUTPUT_LINE(widget);

  if(AGS_LINE(drum_output_line)->channel != NULL){
    /*
    AgsDrum *drum;
    AgsChannel *channel;
    AgsAudioSignal *audio_signal;
    AgsDelaySharedAudio *delay_shared_audio;
    GList *recall_shared;
    guint stop;

    drum = (AgsDrum *) gtk_widget_get_ancestor(widget, AGS_TYPE_DRUM);

    channel = AGS_LINE(drum_output_line)->channel;
    recall_shared = ags_recall_shared_find_type(AGS_AUDIO(channel->audio)->recall_shared,
						AGS_TYPE_DELAY_SHARED_AUDIO);
      
    if(recall_shared != NULL){
      delay_shared_audio = (AgsDelaySharedAudio *) recall_shared->data;
      stop = ((guint) drum->length_spin->adjustment->value) * (delay_shared_audio->delay + 1);
    }else{
      stop = 1;
    }
    
    audio_signal = ags_audio_signal_get_template(channel->first_recycling->audio_signal);
    ags_audio_signal_stream_resize(audio_signal, stop);
*/
  }

  return(0);
}
