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

#include <ags/audio/ags_channel_iter.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_output.h>

AgsChannelIter*
ags_channel_iter_alloc(AgsChannel *start)
{
  AgsChannelIter *iter;

  iter = (AgsChannelIter *) malloc(sizeof(AgsChannelIter));

  iter->flags = 0;
  iter->flags_mask = 0;
  
  iter->current_start = start;

  iter->parent = NULL;
  iter->children = NULL;
  iter->current_iter = iter;

  iter->current = NULL;

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
  AgsChannel *channel;
  AgsChannel *input, *output;
  AgsChannel *next_pad;
  AgsChannel *link;
  
  AgsChannelIter *current_iter, *next_iter, *new_iter;

  guint audio_flags;
  guint audio_channel;
  guint output_line;
  gboolean success;
  
  if(iter->current_start == NULL ||
     iter->flags_mask == 0x1f){
    return(NULL);
  }

  audio = NULL;
  
  input = NULL;
  output = NULL;

  current_iter = NULL;
  next_iter = NULL;

  success = FALSE;
  
  if((AGS_CHANNEL_ITER_DIRECTION_AXIS_TO_LEAFES & (iter->flags_mask)) == 0){
    if(iter->current_iter == NULL){
      current_iter = iter;
    }else{
      current_iter = iter->current_iter;
    }

    next_iter = current_iter;

    channel = next_iter->current;
    
    if((AGS_CHANNEL_ITER_DIRECTION_STRICT_LEVEL & (iter->flags)) != 0){
      if(AGS_IS_OUTPUT(channel)){
	if((AGS_CHANNEL_ITER_DIRECTION_AXIS_TO_LEAFES & (next_iter->flags_mask)) == 0){
	  output = channel;
	  
	  /* go down */
	  audio = output->audio;
	  
	  audio_channel = output->audio_channel;
	  output_line = output->line;

	  input = audio->input;
	  
	  audio_flags = audio->flags;
	  
	  if((AGS_AUDIO_ASYNC & (audio_flags)) != 0){
	    channel = ags_channel_nth(input,
				      audio_channel);
	  }else{
	    channel = ags_channel_nth(input,
				      output_line);
	  }

	  if(channel != NULL){
	    /* allocate new iter */
	    new_iter = ags_channel_iter_alloc(channel);
	    new_iter->current = channel;

	    next_iter->children = g_list_append(next_iter->children,
						new_iter);
	    new_iter->parent = next_iter;

	    /* mark and replace next */
	    next_iter->flags_mask |= AGS_CHANNEL_ITER_DIRECTION_AXIS_TO_LEAFES;
	    next_iter = new_iter;
	  }else{
	    /* go up and select next */
	  }
	}else{
	  /* go up and select next */
	}
      }else{
	if((AGS_CHANNEL_ITER_DIRECTION_AXIS_TO_LEAFES & (next_iter->flags_mask)) == 0){
	  input = channel;
	
	  if((AGS_AUDIO_ASYNC & (audio_flags)) != 0){
	    next_pad = input->next_pad;

	    if(next_pad != NULL){
	      channel = next_pad;

	      /* allocate new iter */
	      new_iter = ags_channel_iter_alloc(channel);
	      new_iter->current = channel;

	      next_iter->children = g_list_append(next_iter->children,
						  new_iter);
	      new_iter->parent = next_iter;
	    }else{
	      /* go down/up and select next */
	    }
	  }else{
	    link = input->link;

	    if(link != NULL){
	      channel = link;

	      /* allocate new iter */
	      new_iter = ags_channel_iter_alloc(channel);
	      new_iter->current = channel;

	      next_iter->children = g_list_append(next_iter->children,
						  new_iter);
	      new_iter->parent = next_iter;

	      /* mark and replace next */
	      next_iter->flags_mask |= AGS_CHANNEL_ITER_DIRECTION_AXIS_TO_LEAFES;
	      next_iter = new_iter;
	    }else{
	      /* go up and select next */
	    }
	  }
	}else{
	  /* go up and select next */
	}
      }
    }else{
      while(channel != NULL &&
	    !success){
	while(channel != NULL &&
	      !success){
	  if(mode == AGS_CHANNEL_ITER_UNTIL_NEXT_LEVEL){
	  }
	}
      }
    }

    /* set return value */
    iter->current_iter = next_iter;
    iter->current = next_iter->current;
  }else if((AGS_CHANNEL_ITER_DIRECTION_AXIS_TO_ROOT & (iter->flags_mask)) == 0){
  }else if((AGS_CHANNEL_ITER_DIRECTION_LEAFES_TO_ROOT & (iter->flags_mask)) == 0){
  }else if((AGS_CHANNEL_ITER_DIRECTION_ROOT_TO_LEAFES & (iter->flags_mask)) == 0){
  }
  
  return(NULL);
}

void
ags_channel_iter_init(AgsChannelIter *iter, guint flags)
{
  iter->flags = flags;
  iter->flags_mask = (0x1f) & (~flags);
}
