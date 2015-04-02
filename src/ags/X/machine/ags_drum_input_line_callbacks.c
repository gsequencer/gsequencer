/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/machine/ags_drum_input_line_callbacks.h>
#include <ags/X/machine/ags_drum.h>

#include <ags/main.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_play_channel_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_line_member.h>

void
ags_drum_input_line_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsDrumInputLine *drum_input_line)
{
  AgsDrum *drum;

  if(old_parent != NULL){
    return;
  }

  drum = gtk_widget_get_ancestor(widget,
				 AGS_TYPE_DRUM);
  
  if(drum != NULL &&
     G_OBJECT(AGS_MACHINE(drum)->audio) != NULL){
    /* AgsAudio */
    g_signal_connect_after(G_OBJECT(AGS_MACHINE(drum)->audio), "set_pads\0",
			   G_CALLBACK(ags_drum_input_line_audio_set_pads_callback), drum_input_line);
  }
}

void
ags_drum_input_line_audio_set_pads_callback(AgsAudio *audio, GType type,
					    guint pads, guint pads_old,
					    AgsDrumInputLine *drum_input_line)
{
  if(type == AGS_TYPE_OUTPUT){
    if(pads > pads_old){
      AgsChannel *current, *output;
      GList *recall;

      output = audio->output;

      while(output != NULL){
	current = ags_channel_nth(audio->input,
				  output->audio_channel);

	while(current != NULL){
	  recall = current->play;

	  while(recall != NULL){
	    if(AGS_IS_RECALL_CHANNEL_RUN(recall->data)){
	      g_object_set(G_OBJECT(recall->data),
			   "destination\0", output,
			   NULL);
	    }

	    recall = recall->next;
	  }

	  recall = current->recall;

	  while(recall != NULL){
	    if(AGS_IS_RECALL_CHANNEL_RUN(recall->data)){
	      g_object_set(G_OBJECT(recall->data),
			   "destination\0", output,
			   NULL);
	    }

	    recall = recall->next;
	  }

	  current = current->next_pad;
	}

	output = output->next;
      }
    }else{
      /* empty */
    }
  }
}
