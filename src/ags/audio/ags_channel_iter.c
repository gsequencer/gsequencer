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

#include <ags/audio/ags_channel_iter.h>

#include <ags/audio/ags_audio.h>

AgsChannelIter*
ags_channel_iter_alloc(AgsChannel *start)
{
  AgsChannelIter *iter;

  iter = (AgsChannelIter *) malloc(sizeof(AgsChannelIter));

  iter->current_start = start;

  iter->parent = NULL;
  iter->children = NULL;
  iter->current_iter = iter;

  iter->current = start;

  return(iter);
}


void
ags_channel_iter_free(AgsChannelIter *iter)
{
}

AgsChannel*
ags_channel_iter_prev(AgsChannelIter *iter, guint mode)
{
  //TODO:JK: implement me

  return(NULL);
}

AgsChannel*
ags_channel_iter_next(AgsChannelIter *iter, guint mode)
{
  AgsAudio *audio;
  AgsChannelIter *current_iter, *next_iter;

  auto AgsChannel* ags_channel_iter_axis_to_leafes(AgsChannelIter *iter);
  auto AgsChannelIter* ags_channel_iter_axis_to_leafes_go_up(AgsChannelIter *iter);

  /*
   * Axis to leafes iteration strategy.
   */
  AgsChannel* ags_channel_iter_axis_to_leafes(AgsChannelIter *iter){
    if(AGS_IS_OUTPUT(current_iter->current)){
      if(audio->input == NULL){
	iter->current_iter = ags_channel_iter_axis_to_leafes_go_up(iter);
	
	return(iter->current_iter->current);
      }
    }else{
      if(current_iter->current->link == NULL){
	if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	  if(current_iter->current->next_pad == NULL){
	    iter->current_iter = ags_channel_iter_axis_to_leafes_go_up(iter);
	    
	    return(iter->current_iter->current);
	  }
	}else{
	  iter->current_iter = ags_channel_iter_axis_to_leafes_go_up(iter);
	  
	  return(iter->current_iter->current);
	}
      }
    }
    
    if(AGS_IS_OUTPUT(current_iter->current)){
      next_iter = ags_channel_iter_alloc(ags_channel_nth(audio->input,
							 (((AGS_AUDIO_ASYNC & (audio->flags)) != 0) ? current_iter->current->audio_channel: current_iter->current->line)));
      current_iter->children = g_list_prepend(current_iter->children,
					      next_iter);
      next_iter->parent = current_iter;

      iter->current_iter = next_iter;

      return(next_iter->current);
    }else{
      if(current_iter->current->link == NULL){
	if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	  current_iter->current = current_iter->current->next_pad;

	  return(current_iter->current);
	}
      }else{
	next_iter = ags_channel_iter_alloc(current_iter->current->link);
	current_iter->children = g_list_prepend(current_iter->children,
						next_iter);
	next_iter->parent = current_iter;

	iter->current_iter = next_iter;

	return(next_iter->current);
      }
    }
  }

  /*
   * Returns: next AgsChannelIter
   *
   * goes up in the iteration list and frees unneeded AgsChannelIter.
   */
  AgsChannelIter* ags_channel_iter_axis_to_leafes_go_up(AgsChannelIter *iter){
    AgsChannelIter *current_iter, *old_iter, *new_iter;

    current_iter = iter->current_iter;

    while(current_iter->parent != NULL){
      if(AGS_OUTPUT(current_iter->current)){
	old_iter = current_iter;
	current_iter = current_iter->parent;

	current_iter->children = g_list_remove(current_iter->children,
					       old_iter);
	free(old_iter);
      }else{
	if(current_iter->current->next_pad == NULL){
	  old_iter = current_iter;
	  current_iter = current_iter->parent;

	  current_iter->children = g_list_remove(current_iter->children,
						 old_iter);
	  free(old_iter);
	}else{
	  current_iter->current = current_iter->current->next_pad;

	  return(current_iter);
	}
      }
    }

    if(AGS_OUTPUT(current_iter->current)){
      if(current_iter->current->link != NULL){
	next_iter = ags_channel_iter_alloc(current_iter->current->link);
	next_iter->children = g_list_prepend(next_iter->children,
					     current_iter);
	current_iter->parent = next_iter;

	return(next_iter);
      }
    }else{
      AgsAudio *audio;
      AgsChannel *channel;

      audio = AGS_AUDIO(current_iter->current->audio);

      if(audio->output != NULL){
	if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	  channel = ags_channel_nth(audio->output, current_iter->current->audio_channel);
	}else{
	  channel = ags_channel_nth(audio->output, current_iter->current->line);
	}

	next_iter = ags_channel_iter_alloc(channel);
	next_iter->children = g_list_prepend(next_iter->children,
					     current_iter);
	current_iter->parent = next_iter;

	return(next_iter);
      }
    }

    return(NULL);
  }

  if(iter->current_start == NULL){
    return(NULL);
  }

  current_iter = iter->current_iter;
  audio = AGS_AUDIO(current_iter->current->audio);

  if((AGS_CHANNEL_ITER_DIRECTION_AXIS_TO_LEAFES & (mode)) != 0){
    if((AGS_CHANNEL_ITER_LEVEL_STRICT & (mode)) != 0){
      //TODO:JK: implement me
    }else{
      return(ags_channel_iter_axis_to_leafes(iter));
    }
  }else if((AGS_CHANNEL_ITER_DIRECTION_AXIS_TO_ROOT & (mode)) != 0){
    if((AGS_CHANNEL_ITER_LEVEL_STRICT & (mode)) != 0){
      //TODO:JK: implement me
    }else{
      //TODO:JK: implement me
    }
  }else if((AGS_CHANNEL_ITER_DIRECTION_LEAFES_TO_ROOT & (mode)) != 0){
    if((AGS_CHANNEL_ITER_LEVEL_STRICT & (mode)) != 0){
      //TODO:JK: implement me
    }else{
      //TODO:JK: implement me
    }
  }else if((AGS_CHANNEL_ITER_DIRECTION_ROOT_TO_LEAFES & (mode)) != 0){
    if((AGS_CHANNEL_ITER_LEVEL_STRICT & (mode)) != 0){
      //TODO:JK: implement me
    }else{
      //TODO:JK: implement me
    }
  }

  return(NULL);
}
