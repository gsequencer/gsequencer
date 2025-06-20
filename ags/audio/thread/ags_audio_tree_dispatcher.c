/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/audio/thread/ags_audio_tree_dispatcher.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recycling_context.h>

#include <stdlib.h>
#include <stdio.h>

#include <ags/i18n.h>

void ags_audio_tree_dispatcher_class_init(AgsAudioTreeDispatcherClass *audio_tree_dispatcher);
void ags_audio_tree_dispatcher_init(AgsAudioTreeDispatcher *audio_tree_dispatcher);
void ags_audio_tree_dispatcher_dispose(GObject *gobject);
void ags_audio_tree_dispatcher_finalize(GObject *gobject);

GList* ags_audio_tree_dispatcher_compile_tree_list_audio(AgsAudio *audio,
							 GObject *dispatch_source,
							 gint sound_scope,
							 GList *tree_list);

GList* ags_audio_tree_dispatcher_compile_tree_list_channel_up(AgsChannel *channel,
							      GObject *dispatch_source,
							      gint sound_scope,
							      AgsRecyclingContext *recycling_context,
							      GList *tree_list);
GList* ags_audio_tree_dispatcher_compile_tree_list_channel_down(AgsChannel *channel,
								GObject *dispatch_source,
								gint sound_scope,
								AgsRecyclingContext *recycling_context,
								GList *tree_list);
GList* ags_audio_tree_dispatcher_compile_tree_list_channel_down_input(AgsChannel *channel,
								      GObject *dispatch_source,
								      gint sound_scope,
								      AgsRecyclingContext *recycling_context,
								      GList *tree_list);

GList* ags_audio_tree_dispatcher_compile_tree_list_channel(AgsChannel *channel,
							   GObject *dispatch_source,
							   gint sound_scope,
							   GList *tree_list);

/**
 * SECTION:ags_audio_tree_dispatcher
 * @short_description: The audio tree dispatcher
 * @title: AgsAudioTreeDispatcher
 * @section_id:
 * @include: ags/audio/thread/ags_audio_tree_dispatcher.h
 *
 * #AgsAudioTreeDispatcher runs per dispatch source and sound scope.
 */

static gpointer ags_audio_tree_dispatcher_parent_class = NULL;

GType
ags_audio_tree_dispatcher_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_audio_tree_dispatcher = 0;

    static const GTypeInfo ags_audio_tree_dispatcher_info = {
      sizeof(AgsAudioTreeDispatcherClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_tree_dispatcher_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsAudioTreeDispatcher),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_tree_dispatcher_init,
    };

    ags_type_audio_tree_dispatcher = g_type_register_static(G_TYPE_OBJECT,
							    "AgsAudioTreeDispatcher",
							    &ags_audio_tree_dispatcher_info,
							    0);

    g_once_init_leave(&g_define_type_id__static, ags_type_audio_tree_dispatcher);
  }

  return(g_define_type_id__static);
}

void
ags_audio_tree_dispatcher_class_init(AgsAudioTreeDispatcherClass *audio_tree_dispatcher)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_audio_tree_dispatcher_parent_class = g_type_class_peek_parent(audio_tree_dispatcher);

  /* GObjectClass */
  gobject = (GObjectClass *) audio_tree_dispatcher;

  gobject->dispose = ags_audio_tree_dispatcher_dispose;
  gobject->finalize = ags_audio_tree_dispatcher_finalize;

  /* properties */
}

void
ags_audio_tree_dispatcher_init(AgsAudioTreeDispatcher *audio_tree_dispatcher)
{
  audio_tree_dispatcher->flags = 0;
  
  /* audio tree dispatcher mutex */
  g_rec_mutex_init(&(audio_tree_dispatcher->obj_mutex)); 

  /* tree list */
  audio_tree_dispatcher->tree_list = NULL;
}

void
ags_audio_tree_dispatcher_dispose(GObject *gobject)
{
  AgsAudioTreeDispatcher *audio_tree_dispatcher;

  audio_tree_dispatcher = AGS_AUDIO_TREE_DISPATCHER(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_audio_tree_dispatcher_parent_class)->dispose(gobject);
}

void
ags_audio_tree_dispatcher_finalize(GObject *gobject)
{
  AgsAudioTreeDispatcher *audio_tree_dispatcher;

  audio_tree_dispatcher = AGS_AUDIO_TREE_DISPATCHER(gobject);

  g_list_free_full(audio_tree_dispatcher->tree_list,
		   (GDestroyNotify) ags_dispatch_audio_free);
  
  /* call parent */
  G_OBJECT_CLASS(ags_audio_tree_dispatcher_parent_class)->finalize(gobject);
}

/**
 * ags_dispatch_audio_alloc:
 * @dispatch_source: the dispatch source
 * @sound_scope: the #AgsSoundScope
 *
 * Alloc dispatch audio.
 *
 * Returns: the newly allocated #AgsDispatchAudio-struct
 * 
 * Since: 8.0.0
 */
AgsDispatchAudio*
ags_dispatch_audio_alloc(GObject *dispatch_source,
			 gint sound_scope)
{
  AgsDispatchAudio *dispatch_audio;

  dispatch_audio = g_new0(AgsDispatchAudio,
			  1);

  if(dispatch_source != NULL){
    g_object_ref(dispatch_source);
  }

  dispatch_audio->dispatch_source = dispatch_source;
  dispatch_audio->sound_scope = sound_scope;
}

/**
 * ags_dispatch_audio_free:
 * @dispatch_audio: the #AgsDispatchAudio-struct
 * 
 * Free @dispatch_audio.
 * 
 * Since: 8.0.0
 */
void
ags_dispatch_audio_free(AgsDispatchAudio *dispatch_audio)
{
  g_return_if_fail(dispatch_audio != NULL);
  
  if(dispatch_audio->dispatch_source != NULL){
    g_object_unref(dispatch_audio->dispatch_source);
  }

  if(dispatch_audio->tree_element_type != G_TYPE_NONE){
    if(g_type_is_a(dispatch_audio->tree_element_type, AGS_TYPE_AUDIO)){
      if(dispatch_audio->tree_element.audio != NULL){
	g_object_unref(dispatch_audio->tree_element.audio);
      }
    }else if(g_type_is_a(dispatch_audio->tree_element_type, AGS_TYPE_OUTPUT)){
      if(dispatch_audio->tree_element.output != NULL){
	g_object_unref(dispatch_audio->tree_element.output);
      }
    }else if(g_type_is_a(dispatch_audio->tree_element_type, AGS_TYPE_INPUT)){
      if(dispatch_audio->tree_element.input != NULL){
	g_object_unref(dispatch_audio->tree_element.input);
      }
    }else{
      g_warning("unknown tree element type");
    }
  }

  if(dispatch_audio->recall_id != NULL){
    g_object_unref(dispatch_audio->recall_id);
  }
  
  if(dispatch_audio->recycling_context != NULL){
    g_object_unref(dispatch_audio->recycling_context);
  }
  
  g_free(dispatch_audio);
}

GList*
ags_audio_tree_dispatcher_compile_tree_list_audio(AgsAudio *audio,
						  GObject *dispatch_source,
						  gint sound_scope,
						  GList *tree_list)
{
  AgsChannel *channel;

  GRecMutex *audio_mutex;
  GRecMutex *channel_mutex;

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get channel */
  g_rec_mutex_lock(audio_mutex);

  channel = audio->output;

  g_rec_mutex_unlock(audio_mutex);

  while(channel != NULL){
    tree_list = ags_audio_tree_dispatcher_compile_tree_list_channel(channel,
								    dispatch_source,
								    sound_scope,
								    tree_list);
    
    /* get channel mutex */
    channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);

    /* iterate */
    g_rec_mutex_lock(channel_mutex);

    channel = channel->next;

    g_rec_mutex_unlock(channel_mutex);
  }
  
  return(tree_list);
}

GList*
ags_audio_tree_dispatcher_compile_tree_list_channel_up(AgsChannel *channel,
						       GObject *dispatch_source,
						       gint sound_scope,
						       AgsRecyclingContext *recycling_context,
						       GList *tree_list)
{
  AgsAudio *current_audio;
  AgsChannel *current_channel, *nth_channel;
  AgsChannel *current_link;
  AgsRecallID *current_recall_id;

  GList *start_recall_id, *recall_id;

  guint audio_channel;
  guint line;
    
  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return(tree_list);
  }

  current_audio = NULL;
  
  current_channel = channel;

  if(current_channel != NULL){
    g_object_ref(current_channel);
  }

  current_link = NULL;

  if(AGS_IS_OUTPUT(channel)){
    g_object_get(channel,
		 "audio", &current_audio,
		 NULL);
      
    goto ags_audio_tree_dispatcher_compile_tree_list_channel_up_OUTPUT;
  }

  while(current_channel != NULL){
    /* check sound scope - input */
    recall_id =
      start_recall_id = ags_channel_check_scope(current_channel, sound_scope);

    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     recycling_context);    

    /* play recall */
    if(current_recall_id != NULL){
      AgsDispatchAudio *dispatch_audio;

      dispatch_audio = ags_dispatch_audio_alloc(dispatch_source,
						sound_scope);

      g_object_ref(current_channel);
	
      dispatch_audio->tree_element_type = AGS_TYPE_INPUT;
      dispatch_audio->tree_element.input = (GObject *) current_channel;

      g_object_ref(current_recall_id);

      dispatch_audio->recall_id = (GObject *) current_recall_id;

      g_object_ref(recycling_context);
      
      dispatch_audio->recycling_context = (GObject *) recycling_context;

      tree_list = g_list_prepend(tree_list,
				 dispatch_audio);
    }
      
    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);

    /* get current audio */
    current_audio = NULL;
    
    g_object_get(current_channel,
		 "audio", &current_audio,
		 NULL);

    /* check sound scope - audio */
    recall_id =
      start_recall_id = ags_audio_check_scope(current_audio, sound_scope);
      
    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     recycling_context);
      
    /* play recall */	
    if(current_recall_id != NULL){
      AgsDispatchAudio *dispatch_audio;

      dispatch_audio = ags_dispatch_audio_alloc(dispatch_source,
						sound_scope);

      g_object_ref(channel);
    
      dispatch_audio->tree_element_type = AGS_TYPE_OUTPUT;
      dispatch_audio->tree_element.input = (GObject *) channel;

      g_object_ref(current_recall_id);

      dispatch_audio->recall_id = (GObject *) current_recall_id;

      g_object_ref(recycling_context);
      
      dispatch_audio->recycling_context = (GObject *) recycling_context;

      tree_list = g_list_prepend(tree_list,
				 dispatch_audio);
    }

    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);
      
    /* get some fields */
    audio_channel = 0;
    line = 0;
    
    g_object_get(current_channel,
		 "audio-channel", &audio_channel,
		 "line", &line,
		 NULL);
      
    /* move up */
    if(current_channel != NULL){
      g_object_unref(current_channel);
    }

    current_channel = NULL;
    
    g_object_get(current_audio,
		 "output", &current_channel,
		 NULL);

    if(ags_audio_test_flags(current_audio, AGS_AUDIO_OUTPUT_HAS_RECYCLING)){
      /* unref current audio */
      if(current_audio != NULL){
	g_object_unref(current_audio);
      }
      
      if(current_channel != NULL){
	g_object_unref(current_channel);
      }

      break;
    }

    if(ags_audio_test_flags(current_audio, AGS_AUDIO_ASYNC)){
      nth_channel = ags_channel_nth(current_channel,
				    audio_channel);

      g_object_unref(current_channel);

      current_channel = nth_channel;
    }else{
      nth_channel = ags_channel_nth(current_channel,
				    line);

      g_object_unref(current_channel);

      current_channel = nth_channel;
    }
      
  ags_audio_tree_dispatcher_compile_tree_list_channel_up_OUTPUT:

    /* check sound scope - output */
    recall_id =
      start_recall_id = ags_channel_check_scope(current_channel, sound_scope);
      
    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     recycling_context);
      
    /* play recall */	
    if(current_recall_id != NULL){
      AgsDispatchAudio *dispatch_audio;

      dispatch_audio = ags_dispatch_audio_alloc(dispatch_source,
						sound_scope);

      g_object_ref(current_channel);
	
      dispatch_audio->tree_element_type = AGS_TYPE_OUTPUT;
      dispatch_audio->tree_element.input = (GObject *) current_channel;

      g_object_ref(current_recall_id);

      dispatch_audio->recall_id = (GObject *) current_recall_id;

      g_object_ref(recycling_context);
      
      dispatch_audio->recycling_context = (GObject *) recycling_context;

      tree_list = g_list_prepend(tree_list,
				 dispatch_audio);
    }
      
    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);
      
    /* unref current audio */
    g_object_unref(current_audio);

    /* iterate */
    current_link = ags_channel_get_link(current_channel);

    g_object_unref(current_channel);

    current_channel = current_link;
  }

  if(current_link != NULL){
    g_object_unref(current_link);
  }
  
  return(tree_list);
}

GList*
ags_audio_tree_dispatcher_compile_tree_list_channel_down(AgsChannel *channel,
							 GObject *dispatch_source,
							 gint sound_scope,
							 AgsRecyclingContext *recycling_context,
							 GList *tree_list)
{
  AgsAudio *current_audio;
  AgsChannel *start_input;
  AgsChannel *current_input, *next_pad, *next_channel, *nth_input;
  AgsRecallID *current_recall_id, *next_recall_id;
  AgsRecyclingContext *next_recycling_context;
    
  GList *start_recall_id, *recall_id;

  guint audio_channel, line;
    
  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return(tree_list);
  }

  /* do */
  next_recycling_context = recycling_context;
    
  /* check sound scope - output */
  recall_id =
    start_recall_id = ags_channel_check_scope(channel, sound_scope);

  current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							   recycling_context);
  
  /* play recall */
  if(current_recall_id != NULL){
    AgsDispatchAudio *dispatch_audio;

    dispatch_audio = ags_dispatch_audio_alloc(dispatch_source,
					      sound_scope);

    g_object_ref(channel);
    
    dispatch_audio->tree_element_type = AGS_TYPE_OUTPUT;
    dispatch_audio->tree_element.input = (GObject *) channel;

    g_object_ref(current_recall_id);

    dispatch_audio->recall_id = (GObject *) current_recall_id;

    g_object_ref(recycling_context);
      
    dispatch_audio->recycling_context = (GObject *) recycling_context;

    tree_list = g_list_prepend(tree_list,
			       dispatch_audio);
  }
    
  /* free recall id */
  g_list_free_full(start_recall_id,
		   g_object_unref);

  /* get current audio */
  current_audio = NULL;
  
  audio_channel = 0;
  line = 0;

  g_object_get(channel,
	       "audio", &current_audio,
	       "audio-channel", &audio_channel,
	       "line", &line,
	       NULL);

  /* check sound scope - audio */
  recall_id =
    start_recall_id = ags_audio_check_scope(current_audio, sound_scope);

  current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							   recycling_context);
  
  /* play recall */	
  if(current_recall_id != NULL){
    AgsDispatchAudio *dispatch_audio;

    dispatch_audio = ags_dispatch_audio_alloc(dispatch_source,
					      sound_scope);

    g_object_ref(current_audio);
    
    dispatch_audio->tree_element_type = AGS_TYPE_AUDIO;
    dispatch_audio->tree_element.input = (GObject *) current_audio;

    g_object_ref(current_recall_id);

    dispatch_audio->recall_id = (GObject *) current_recall_id;

    g_object_ref(recycling_context);
      
    dispatch_audio->recycling_context = (GObject *) recycling_context;

    tree_list = g_list_prepend(tree_list,
			       dispatch_audio);
  }

  /* free recall id */
  g_list_free_full(start_recall_id,
		   g_object_unref);

  /* get some fields */
  start_input = NULL;
  
  g_object_get(current_audio,
	       "input", &start_input,
	       NULL);

  /* check next recycling context */
  if(ags_audio_test_flags(current_audio, AGS_AUDIO_OUTPUT_HAS_RECYCLING)){
    AgsRecycling *first_recycling;

    gint position;
      
    next_recycling_context = NULL;

    if(ags_audio_test_flags(current_audio, AGS_AUDIO_ASYNC)){
      AgsChannel *first_with_recycling;
	  
      nth_input = ags_channel_nth(start_input,
				  audio_channel);

      current_input = nth_input;
	
      first_with_recycling = ags_channel_first_with_recycling(current_input);

      first_recycling = NULL;
      
      g_object_get(first_with_recycling,
		   "first-recycling", &first_recycling,
		   NULL);

      g_object_unref(first_with_recycling);

      if(current_input != NULL){
	g_object_unref(current_input);
      }
    }else{
      nth_input = ags_channel_nth(start_input,
				  line);

      current_input = nth_input;

      first_recycling = NULL;
      
      g_object_get(current_input,
		   "first-recycling", &first_recycling,
		   NULL);

      if(current_input != NULL){
	g_object_unref(current_input);
      }
    }

    if(first_recycling != NULL){
      position = ags_recycling_context_find_child(recycling_context,
						  first_recycling);

      if(position >= 0){
	GList *child_start;

	child_start = NULL;
	
	g_object_get(recycling_context,
		     "child", &child_start,
		     NULL);

	next_recycling_context = g_list_nth_data(child_start, position);
	g_list_free_full(child_start,
			 g_object_unref);
      }

      g_object_unref(first_recycling);
    }
  }

  if(next_recycling_context != NULL &&
     next_recycling_context != recycling_context){
    /* check sound scope - audio */
    recall_id =
      start_recall_id = ags_audio_check_scope(current_audio, sound_scope);

    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     next_recycling_context);

    /* play recall */	
    if(current_recall_id != NULL){
      AgsDispatchAudio *dispatch_audio;

      dispatch_audio = ags_dispatch_audio_alloc(dispatch_source,
						sound_scope);

      g_object_ref(current_audio);
      
      dispatch_audio->tree_element_type = AGS_TYPE_AUDIO;
      dispatch_audio->tree_element.input = (GObject *) current_audio;

      g_object_ref(current_recall_id);

      dispatch_audio->recall_id = (GObject *) current_recall_id;

      g_object_ref(next_recycling_context);
      
      dispatch_audio->recycling_context = (GObject *) next_recycling_context;

      tree_list = g_list_prepend(tree_list,
				 dispatch_audio);
    }
    
    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);
  }

  /* unref */
  if(start_input != NULL){
    g_object_unref(start_input);
  }
    
  /* traverse the tree */
  tree_list = ags_audio_tree_dispatcher_compile_tree_list_channel_down_input(channel,
									     dispatch_source,
									     sound_scope,
									     next_recycling_context,
									     tree_list);

  /* unref */
  if(current_audio != NULL){
    g_object_unref(current_audio);
  }

  return(tree_list);
}

GList*
ags_audio_tree_dispatcher_compile_tree_list_channel_down_input(AgsChannel *channel,
							       GObject *dispatch_source,
							       gint sound_scope,
							       AgsRecyclingContext *recycling_context,
							       GList *tree_list)
{
  AgsAudio *current_audio;
  AgsChannel *start_input;
  AgsChannel *current_input, *next_pad, *nth_input;
  AgsChannel *current_link;
  AgsRecallID *current_recall_id;

  GList *start_recall_id, *recall_id;
    
  guint audio_channel, line;

  if(!AGS_IS_CHANNEL(channel) ||
     !AGS_IS_RECYCLING_CONTEXT(recycling_context)){
    return(tree_list);
  }
    
  /* get some fields */
  current_audio = NULL;
  
  audio_channel = 0;
  line = 0;

  g_object_get(channel,
	       "audio", &current_audio,
	       "line", &line,
	       "audio-channel", &audio_channel,
	       NULL);

  if(current_audio == NULL){
    return(tree_list);
  }

  /* get some fields */
  start_input = NULL;
  
  g_object_get(current_audio,
	       "input", &start_input,
	       NULL);
    
  /* sync/async */
  if(ags_audio_test_flags(current_audio, AGS_AUDIO_ASYNC)){
    nth_input = ags_channel_nth(start_input,
				audio_channel);

    current_input = nth_input;

    next_pad = NULL;
      
    while(current_input != NULL){
      /* get some fields */
      current_link = ags_channel_get_link(current_input);
      
      /* check sound scope - input */
      recall_id =
	start_recall_id = ags_channel_check_scope(current_input, sound_scope);

      current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							       recycling_context);

      /* play recall */	
      if(current_recall_id != NULL){
	AgsDispatchAudio *dispatch_audio;

	dispatch_audio = ags_dispatch_audio_alloc(dispatch_source,
						  sound_scope);

	g_object_ref(current_input);
	
	dispatch_audio->tree_element_type = AGS_TYPE_INPUT;
	dispatch_audio->tree_element.input = (GObject *) current_input;

	g_object_ref(current_recall_id);

	dispatch_audio->recall_id = (GObject *) current_recall_id;

	g_object_ref(recycling_context);
      
	dispatch_audio->recycling_context = (GObject *) recycling_context;

	tree_list = g_list_prepend(tree_list,
				   dispatch_audio);
      }

      /* free recall id */
      g_list_free_full(start_recall_id,
		       g_object_unref);

      /* traverse the tree */
      tree_list = ags_audio_tree_dispatcher_compile_tree_list_channel_down(current_link,
									   dispatch_source,
									   sound_scope,
									   recycling_context,
									   tree_list);
      
      if(current_link != NULL){
	g_object_unref(current_link);
      }

      /* iterate */
      next_pad = ags_channel_next_pad(current_input);

      g_object_unref(current_input);

      current_input = next_pad;
    }

    if(next_pad != NULL){
      g_object_unref(next_pad);
    }
  }else{
    nth_input = ags_channel_nth(start_input,
				line);

    current_input = nth_input;
      
    /* get some fields */
    current_link = ags_channel_get_link(current_input);
      
    /* check sound scope - input */
    recall_id =
      start_recall_id = ags_channel_check_scope(current_input, sound_scope);

    current_recall_id = ags_recall_id_find_recycling_context(start_recall_id,
							     recycling_context);      

    /* play recall */	
    if(current_recall_id != NULL){
      AgsDispatchAudio *dispatch_audio;

      dispatch_audio = ags_dispatch_audio_alloc(dispatch_source,
						sound_scope);

      g_object_ref(current_input);
      
      dispatch_audio->tree_element_type = AGS_TYPE_INPUT;
      dispatch_audio->tree_element.input = (GObject *) current_input;

      g_object_ref(current_recall_id);

      dispatch_audio->recall_id = (GObject *) current_recall_id;

      g_object_ref(recycling_context);
      
      dispatch_audio->recycling_context = (GObject *) recycling_context;

      tree_list = g_list_prepend(tree_list,
				 dispatch_audio);
    }

    /* free recall id */
    g_list_free_full(start_recall_id,
		     g_object_unref);

    /* traverse the tree */
    tree_list = ags_audio_tree_dispatcher_compile_tree_list_channel_down(current_link,
									 dispatch_source,
									 sound_scope,
									 recycling_context,
									 tree_list);

    if(current_link != NULL){
      g_object_unref(current_link);
    }

    if(current_input != NULL){
      g_object_unref(current_input);
    }
  }

  /* unref */
  if(current_audio != NULL){
    g_object_unref(current_audio);
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
  
  return(tree_list);
}

GList*
ags_audio_tree_dispatcher_compile_tree_list_channel(AgsChannel *channel,
						    GObject *dispatch_source,
						    gint sound_scope,
						    GList *tree_list)
{
  
  AgsChannel *link;
  AgsRecyclingContext *recycling_context;

  GList *recall_id, *recall_id_iter;

  guint pad;
  gint i;

  GRecMutex *channel_mutex;
  GRecMutex *recall_id_mutex;
  
  if(sound_scope < 0 ||
     sound_scope >= AGS_SOUND_SCOPE_LAST){
    return(tree_list);
  }

  /* check sound scope - input */
  recall_id = ags_channel_check_scope(channel, sound_scope);

  recycling_context = NULL;
    
  if(recall_id != NULL){
    AgsRecycling *recycling;
    
    GList *iter;

    recycling = NULL;
    
    g_object_get(channel,
		 "first-recycling", &recycling,
		 NULL);
    
    iter = recall_id;

    while(iter != NULL){
      AgsRecallID *current_recall_id;
      AgsRecyclingContext *current_recycling_context;
      
      current_recall_id = iter->data;

      /* get recall id mutex */
      recall_id_mutex = AGS_RECALL_ID_GET_OBJ_MUTEX(current_recall_id);
      
      /* get recycling context */
      g_rec_mutex_lock(recall_id_mutex);
      
      current_recycling_context = current_recall_id->recycling_context;
      
      g_rec_mutex_unlock(recall_id_mutex);

      if(ags_recycling_context_find(current_recycling_context, recycling) >= 0){
	recycling_context = current_recycling_context;

	g_object_ref(recycling_context);
	
	break;
      }
      
      /* iterate */
      iter = iter->next;
    }

    g_object_unref(recycling);

    g_list_free_full(recall_id,
		     g_object_unref);
  }

  /* get channel mutex */
  channel_mutex = AGS_CHANNEL_GET_OBJ_MUTEX(channel);
  
  /* get link and pad */
  g_rec_mutex_lock(channel_mutex);
      
  link = channel->link;

  pad = channel->pad;
  
  g_rec_mutex_unlock(channel_mutex);

  if(AGS_IS_OUTPUT(channel)){
    if(pad == 0){
      tree_list = ags_audio_tree_dispatcher_compile_tree_list_channel_down(channel,
									   dispatch_source,
									   sound_scope,
									   recycling_context,
									   tree_list);
      
      tree_list = ags_audio_tree_dispatcher_compile_tree_list_channel_up(link,
									 dispatch_source,
									 sound_scope,
									 recycling_context,
									 tree_list);
    }else{
      tree_list = ags_audio_tree_dispatcher_compile_tree_list_channel_up(channel,
									 dispatch_source,
									 sound_scope,
									 recycling_context,
									 tree_list);
    }
  }else{
    tree_list = ags_audio_tree_dispatcher_compile_tree_list_channel_down(link,
									 dispatch_source,
									 sound_scope,
									 recycling_context,
									 tree_list);
    tree_list = ags_audio_tree_dispatcher_compile_tree_list_channel_up(channel,
								       dispatch_source,
								       sound_scope,
								       recycling_context,
								       tree_list);
  }
  
  return(tree_list);
}


/**
 * ags_audio_tree_dispatcher_compile_tree_list:
 * @audio_tree_dispatcher: the #AgsAudioTreeDispatcher
 * @dispatch_source: the dispatch source
 * @sound_scope: the sound scope
 *
 * Compile tree list.
 *
 * Returns: (transfer full): the newly created tree list
 * 
 * Since: 8.0.0
 */
GList*
ags_audio_tree_dispatcher_compile_tree_list(AgsAudioTreeDispatcher *audio_tree_dispatcher,
					    GObject *dispatch_source,
					    gint sound_scope)
{
  GList *start_tree_list;

  start_tree_list = NULL;

  if(AGS_IS_AUDIO(dispatch_source)){
    start_tree_list = ags_audio_tree_dispatcher_compile_tree_list_audio((AgsAudio *) dispatch_source,
									dispatch_source,
									sound_scope,
									start_tree_list);
  }else if(AGS_IS_CHANNEL(dispatch_source)){
    start_tree_list = ags_audio_tree_dispatcher_compile_tree_list_channel((AgsChannel *) dispatch_source,
									  dispatch_source,
									  sound_scope,
									  start_tree_list);
  }else{
    g_warning("unknown dispatch source");
  }

  return(g_list_reverse(start_tree_list));
}

/**
 * ags_audio_tree_dispatcher_remove_dispatch_source:
 * @audio_tree_dispatcher: the #AgsAudioTreeDispatcher
 * @dispatch_source: the dispatch source
 * @sound_scope: the sound scope
 *
 * Remove dispatch source from tree list.
 * 
 * Since: 8.0.0
 */
void
ags_audio_tree_dispatcher_remove_dispatch_source(AgsAudioTreeDispatcher *audio_tree_dispatcher,
						 GObject *dispatch_source,
						 gint sound_scope)
{
  GList *tree_list, *next_tree_list;

  GRecMutex *audio_tree_dispatcher_mutex;
  
  g_return_if_fail(AGS_IS_AUDIO_TREE_DISPATCHER(audio_tree_dispatcher));
  g_return_if_fail(dispatch_source != NULL);
  g_return_if_fail(sound_scope >= 0 && sound_scope < AGS_SOUND_SCOPE_LAST);

  audio_tree_dispatcher_mutex = AGS_AUDIO_TREE_DISPATCHER_GET_OBJ_MUTEX(audio_tree_dispatcher);

  /* remove dispatch source */
  g_rec_mutex_lock(audio_tree_dispatcher_mutex);

  tree_list = audio_tree_dispatcher->tree_list;

  while(tree_list != NULL){
    AgsDispatchAudio *dispatch_audio;

    dispatch_audio = AGS_DISPATCH_AUDIO(tree_list->data);
    
    next_tree_list = tree_list->next;
    
    if(dispatch_audio->dispatch_source == dispatch_source &&
       dispatch_audio->sound_scope == sound_scope){
      audio_tree_dispatcher->tree_list = g_list_remove(audio_tree_dispatcher->tree_list,
						       dispatch_audio);
      ags_dispatch_audio_free(dispatch_audio);
    }

    tree_list = next_tree_list;
  }
  
  g_rec_mutex_unlock(audio_tree_dispatcher_mutex);
}

/**
 * ags_audio_tree_dispatcher_get_tree_list:
 * @audio_tree_dispatcher: the #AgsAudioTreeDispatcher
 * @staging_program_count: (out): the staging program count return value
 * 
 * Get tree list.
 *
 * Returns: (transfer full): the staging program
 * 
 * Since: 8.0.0
 */
guint*
ags_audio_tree_dispatcher_get_staging_program(AgsAudioTreeDispatcher *audio_tree_dispatcher,
					      guint *staging_program_count)
{
  guint *local_staging_program;

  guint local_staging_program_count;
    
  local_staging_program = NULL;

  local_staging_program_count = 0;

  if(audio_tree_dispatcher->staging_program_count > 0){
    local_staging_program_count = audio_tree_dispatcher->staging_program_count;
    
    local_staging_program = (guint *) g_malloc(local_staging_program_count * sizeof(guint));
    
    memcpy(local_staging_program, audio_tree_dispatcher->staging_program, local_staging_program_count * sizeof(guint));
  }

  if(staging_program_count != NULL){
    staging_program_count[0] = local_staging_program_count;
  }
  
  return(local_staging_program);
}

/**
 * ags_audio_tree_dispatcher_set_staging_program:
 * @audio_tree_dispatcher: the #AgsAudioTreeDispatcher
 * @staging_program: (transfer none): the staging program array
 * @staging_program_count: the staging program count
 * 
 * Get tree list.
 * 
 * Since: 8.0.0
 */
void
ags_audio_tree_dispatcher_set_staging_program(AgsAudioTreeDispatcher *audio_tree_dispatcher,
					      guint *staging_program, guint staging_program_count)
{
  audio_tree_dispatcher->staging_program = g_malloc(staging_program_count * sizeof(guint));
  
  memcpy(audio_tree_dispatcher->staging_program, staging_program, staging_program_count * sizeof(guint));
  
  audio_tree_dispatcher->staging_program_count = staging_program_count;
}

/**
 * ags_audio_tree_dispatcher_get_tree_list:
 * @audio_tree_dispatcher: the #AgsAudioTreeDispatcher
 * 
 * Get tree list.
 *
 * Returns: (transfer container): the tree list
 * 
 * Since: 8.0.0
 */
GList*
ags_audio_tree_dispatcher_get_tree_list(AgsAudioTreeDispatcher *audio_tree_dispatcher)
{
  GList *start_tree_list;
  
  GRecMutex *audio_tree_dispatcher_mutex;

  audio_tree_dispatcher_mutex = AGS_AUDIO_TREE_DISPATCHER_GET_OBJ_MUTEX(audio_tree_dispatcher);

  /* get tree list */
  g_rec_mutex_lock(audio_tree_dispatcher_mutex);

  start_tree_list = g_list_copy(audio_tree_dispatcher->tree_list);
  
  g_rec_mutex_unlock(audio_tree_dispatcher_mutex);

  return(start_tree_list);
}

/**
 * ags_audio_tree_dispatcher_set_tree_list:
 * @audio_tree_dispatcher: the #AgsAudioTreeDispatcher
 * @tree_list: (transfer full): the #GList-struct containing #AgsDispatchAudio-struct
 * 
 * Set tree list.
 * 
 * Since: 8.0.0
 */
void
ags_audio_tree_dispatcher_set_tree_list(AgsAudioTreeDispatcher *audio_tree_dispatcher,
					GList *tree_list)
{
  GRecMutex *audio_tree_dispatcher_mutex;

  /* set tree list */
  g_rec_mutex_lock(audio_tree_dispatcher_mutex);

  audio_tree_dispatcher->tree_list = tree_list;
  
  g_rec_mutex_unlock(audio_tree_dispatcher_mutex);
}

/**
 * ags_audio_tree_dispatcher_get_tree_list_stamp:
 * @audio_tree_dispatcher: the #AgsAudioTreeDispatcher
 * 
 * Get tree list stamp.
 *
 * Returns: the tree list stamp
 * 
 * Since: 8.0.0
 */
gint64
ags_audio_tree_dispatcher_get_tree_list_stamp(AgsAudioTreeDispatcher *audio_tree_dispatcher)
{
  gint64 tree_list_stamp;
  
  GRecMutex *audio_tree_dispatcher_mutex;

  audio_tree_dispatcher_mutex = AGS_AUDIO_TREE_DISPATCHER_GET_OBJ_MUTEX(audio_tree_dispatcher);

  /* get tree list stamp */
  g_rec_mutex_lock(audio_tree_dispatcher_mutex);

  tree_list_stamp = audio_tree_dispatcher->tree_list_stamp;
  
  g_rec_mutex_unlock(audio_tree_dispatcher_mutex);

  return(tree_list_stamp);
}

/**
 * ags_audio_tree_dispatcher_set_tree_list_stamp:
 * @audio_tree_dispatcher: the #AgsAudioTreeDispatcher
 * @tree_list_stamp: the tree list stamp
 * 
 * Set tree list stamp.
 * 
 * Since: 8.0.0
 */
void
ags_audio_tree_dispatcher_set_tree_list_stamp(AgsAudioTreeDispatcher *audio_tree_dispatcher,
					      gint64 tree_list_stamp)
{
  GRecMutex *audio_tree_dispatcher_mutex;

  audio_tree_dispatcher_mutex = AGS_AUDIO_TREE_DISPATCHER_GET_OBJ_MUTEX(audio_tree_dispatcher);

  /* set tree list stamp */
  g_rec_mutex_lock(audio_tree_dispatcher_mutex);

  audio_tree_dispatcher->tree_list_stamp = tree_list_stamp;
  
  g_rec_mutex_unlock(audio_tree_dispatcher_mutex);
}

/**
 * ags_audio_tree_dispatcher_run:
 * @audio_tree_dispatcher: the #AgsAudioTreeDispatcher
 * 
 * Run the dispatcher.
 * 
 * Since: 8.0.0
 */
void
ags_audio_tree_dispatcher_run(AgsAudioTreeDispatcher *audio_tree_dispatcher)
{
  GList *start_tree_list, *tree_list;

  guint *staging_program;
  
  guint staging_program_count;
  guint i;
  
  GRecMutex *audio_tree_dispatcher_mutex;

  audio_tree_dispatcher_mutex = AGS_AUDIO_TREE_DISPATCHER_GET_OBJ_MUTEX(audio_tree_dispatcher);

  /* tree list */
  tree_list = 
    start_tree_list = ags_audio_tree_dispatcher_get_tree_list(audio_tree_dispatcher);

  staging_program_count = 0;
  
  staging_program = ags_audio_tree_dispatcher_get_staging_program(audio_tree_dispatcher,
								  &staging_program_count);

  for(i = 0; i < staging_program_count; i++){
    guint staging_flags;
    
    tree_list = start_tree_list;

    staging_flags = staging_program[i];
    
    while(tree_list != NULL){
      AgsDispatchAudio *dispatch_audio;

      dispatch_audio = (AgsDispatchAudio *) tree_list->data;
    
      if(dispatch_audio->tree_element_type == AGS_TYPE_AUDIO){
	ags_audio_play_recall((AgsAudio *) dispatch_audio->tree_element.audio,
			      (AgsRecallID *) dispatch_audio->recall_id, staging_flags);      
      }else if(dispatch_audio->tree_element_type == AGS_TYPE_OUTPUT){
	ags_channel_play_recall((AgsChannel *) dispatch_audio->tree_element.output,
				(AgsRecallID *) dispatch_audio->recall_id, staging_flags);
      }else if(dispatch_audio->tree_element_type == AGS_TYPE_INPUT){
	ags_channel_play_recall((AgsChannel *) dispatch_audio->tree_element.input,
				(AgsRecallID *) dispatch_audio->recall_id, staging_flags);
      }else{
	g_warning("unknown audio dispatch tree element");
      }

      tree_list = tree_list->next;
    }
  }

  g_free(staging_program);
  
  g_list_free(start_tree_list);
}

/**
 * ags_audio_tree_dispatcher_new:
 *
 * Creates a #AgsAudioTreeDispatcher
 *
 * Returns: a new #AgsAudioTreeDispatcher
 * 
 * Since: 8.0.0
 */
AgsAudioTreeDispatcher*
ags_audio_tree_dispatcher_new()
{
  AgsAudioTreeDispatcher *audio_tree_dispatcher;

  audio_tree_dispatcher = (AgsAudioTreeDispatcher *) g_object_new(AGS_TYPE_AUDIO_TREE_DISPATCHER,
								  NULL);

  return(audio_tree_dispatcher);
}
