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

#include <ags/audio/ags_audio.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/lib/ags_list.h>

#include <ags/object/ags_dynamic_connectable.h>
#include <ags/object/ags_marshal.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/file/ags_audio_file.h>

#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>

#include <ags/audio/task/ags_audio_set_recycling.h>

#include <stdlib.h>
#include <stdio.h>

void ags_audio_class_init(AgsAudioClass *audio_class);
void ags_audio_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_init(AgsAudio *audio);
void ags_audio_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec);
void ags_audio_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec);
void ags_audio_add_to_registry(AgsConnectable *connectable);
void ags_audio_remove_from_registry(AgsConnectable *connectable);
void ags_audio_connect(AgsConnectable *connectable);
void ags_audio_disconnect(AgsConnectable *connectable);
void ags_audio_finalize(GObject *gobject);

void ags_audio_real_set_audio_channels(AgsAudio *audio,
				       guint audio_channels, guint audio_channels_old);
void ags_audio_real_set_pads(AgsAudio *audio,
			     GType type,
			     guint channels, guint channels_old);

enum{
  SET_AUDIO_CHANNELS,
  SET_PADS,
  SET_LINES,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_DEVOUT,
};

static gpointer ags_audio_parent_class = NULL;

static guint audio_signals[LAST_SIGNAL];

GType
ags_audio_get_type (void)
{
  static GType ags_type_audio = 0;

  if(!ags_type_audio){
    static const GTypeInfo ags_audio_info = {
      sizeof(AgsAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio = g_type_register_static(G_TYPE_OBJECT,
					    "AgsAudio\0", &ags_audio_info,
					    0);

    g_type_add_interface_static(ags_type_audio,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_audio);
}

void
ags_audio_class_init(AgsAudioClass *audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_audio_parent_class = g_type_class_peek_parent(audio);

  /* GObjectClass */
  gobject = (GObjectClass *) audio;

  gobject->set_property = ags_audio_set_property;
  gobject->get_property = ags_audio_get_property;

  gobject->finalize = ags_audio_finalize;

  /* properties */
  param_spec = g_param_spec_object("devout\0",
				   "assigned devout\0",
				   "The devout it is assigned with\0",
				   AGS_TYPE_DEVOUT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVOUT,
				  param_spec);

  /* AgsAudioClass */
  audio->set_audio_channels = ags_audio_real_set_audio_channels;
  audio->set_pads = ags_audio_real_set_pads;

  /* signals */
  audio_signals[SET_AUDIO_CHANNELS] = 
    g_signal_new("set_audio_channels\0",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, set_audio_channels),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT, G_TYPE_UINT);

  audio_signals[SET_PADS] = 
    g_signal_new("set_pads\0",
		 G_TYPE_FROM_CLASS(audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioClass, set_pads),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__ULONG_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_ULONG,
		 G_TYPE_UINT, G_TYPE_UINT);
}

void
ags_audio_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->add_to_registry = ags_audio_add_to_registry;
  connectable->remove_from_registry = ags_audio_remove_from_registry;
  connectable->connect = ags_audio_connect;
  connectable->disconnect = ags_audio_disconnect;
}

void
ags_audio_init(AgsAudio *audio)
{
  audio->flags = 0;

  audio->devout = NULL;

  audio->sequence_length = 0;
  audio->audio_channels = 0;
  audio->frequence = 0;

  audio->output_pads = 0;
  audio->output_lines = 0;
  audio->input_pads = 0;
  audio->input_lines = 0;

  audio->output = NULL;
  audio->input = NULL;

  audio->notation = NULL;

  audio->devout_play = ags_devout_play_alloc();
  AGS_DEVOUT_PLAY(audio->devout_play)->source = (GObject *) audio;

  audio->recall_id = NULL;
  audio->run_order = NULL;

  audio->container;

  audio->recall = NULL;
  audio->play = NULL;

  audio->recall_remove= NULL;
  audio->play_remove = NULL;

  audio->machine = NULL;
}

void
ags_audio_set_property(GObject *gobject,
		       guint prop_id,
		       const GValue *value,
		       GParamSpec *param_spec)
{
  AgsAudio *audio;

  audio = AGS_AUDIO(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    {
      AgsDevout *devout;

      devout = (AgsDevout *) g_value_get_object(value);

      ags_audio_set_devout(audio, devout);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_get_property(GObject *gobject,
		       guint prop_id,
		       GValue *value,
		       GParamSpec *param_spec)
{
  AgsAudio *audio;

  audio = AGS_AUDIO(gobject);

  switch(prop_id){
  case PROP_DEVOUT:
    g_value_set_object(value, audio->devout);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_finalize(GObject *gobject)
{
  AgsAudio *audio;
  AgsChannel *channel;

  audio = AGS_AUDIO(gobject);

  if(audio->devout != NULL)
    g_object_unref(audio->devout);

  /* output */
  channel = audio->output;

  if(channel != NULL){
    while(channel->next != NULL){
      channel = channel->next;
      g_object_unref((GObject *) channel->prev);
    }

    g_object_unref((GObject *) channel);
  }

  /* input */
  channel = audio->input;

  if(channel != NULL){
    while(channel->next != NULL){
      channel = channel->next;
      g_object_unref((GObject *) channel->prev);
    }

    g_object_unref(channel);
  }

  /* free some lists */
  ags_list_free_and_unref_link(audio->notation);

  if(audio->devout_play != NULL)
    free(audio->devout_play);

  ags_list_free_and_unref_link(audio->recall_id);
  ags_list_free_and_unref_link(audio->run_order);

  ags_list_free_and_unref_link(audio->recall);
  ags_list_free_and_unref_link(audio->play);

  ags_list_free_and_unref_link(audio->recall_remove);
  ags_list_free_and_unref_link(audio->play_remove);

  /* call parent */
  G_OBJECT_CLASS(ags_audio_parent_class)->finalize(gobject);
}

void
ags_audio_add_to_registry(AgsConnectable *connectable)
{
  AgsMain *main;
  AgsServer *server;
  AgsAudio *audio;
  AgsChannel *channel;
  AgsRegistryEntry *entry;
  GList *list;
  
  audio = AGS_AUDIO(connectable);

  main = AGS_MAIN(AGS_DEVOUT(audio->devout)->main);

  server = main->server;

  entry = ags_registry_entry_alloc(server->registry);
  g_value_set_object(&(entry->entry),
		     (gpointer) audio);
  ags_registry_add(server->registry,
		   entry);

  /* add play */
  list = audio->play;

  while(list != NULL){
    ags_connectable_add_to_registry(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
  
  /* add recall */
  list = audio->recall;

  while(list != NULL){
    ags_connectable_add_to_registry(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* add output */
  channel = audio->output;

  while(channel != NULL){
    ags_connectable_add_to_registry(AGS_CONNECTABLE(channel));

    channel = channel->next;
  }

  /* add input */
  channel = audio->input;

  while(channel != NULL){
    ags_connectable_add_to_registry(AGS_CONNECTABLE(channel));

    channel = channel->next;
  }
}

void
ags_audio_remove_from_registry(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_audio_connect(AgsConnectable *connectable)
{
  AgsAudio *audio;
  AgsChannel *channel;
  GList *list;

  audio = AGS_AUDIO(connectable);

  /* connect channels */
  channel = audio->output;

  while(channel != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(channel));

    channel = channel->next;
  }

  channel = audio->input;

  while(channel != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(channel));

    channel = channel->next;
  }

  /* connect recall ids */
  list = audio->recall_id;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* connect recall containers */
  list = audio->container;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* connect recalls */
  list = audio->recall;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  list = audio->play;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* connect remove recalls */
  list = audio->recall_remove;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  list = audio->play_remove;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  /* connect notation */
  if(audio->notation != NULL)
    ags_connectable_connect(AGS_CONNECTABLE(audio->notation));
}

void
ags_audio_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

/**
 * ags_audio_set_flags:
 * @audio an AgsAudio
 * @flags see enum AgsAudioFlags
 *
 * Enable a feature of AgsAudio.
 */
void
ags_audio_set_flags(AgsAudio *audio, guint flags)
{
  auto GParameter* ags_audio_set_flags_set_recycling_parameter(GType type);
  auto void ags_audio_set_flags_add_recycling_task(GParameter *parameter);

  GParameter* ags_audio_set_flags_set_recycling_parameter(GType type){
    AgsChannel *channel, *start_channel, *end_channel;
    AgsRecycling *recycling, *recycling_next, *start_recycling, *end_recycling;
    GParameter *parameter;
    int i;

    parameter = g_new(GParameter, 4 * audio->audio_channels);

    for(i = 0; i < audio->audio_channels; i++){
      start_channel =
	channel = ags_channel_nth(((g_type_is_a(type, AGS_TYPE_INPUT)) ? audio->input: audio->output), i);
      end_channel = NULL;

      start_recycling =
	recycling = NULL;
      end_recycling = NULL;
	  
      if(channel != NULL){
	start_recycling = 
	  recycling = ags_recycling_new(audio->devout);
	goto ags_audio_set_flags_OUTPUT_RECYCLING;
      }

      while(channel != NULL){
	recycling->next = ags_recycling_new(audio->devout);
      ags_audio_set_flags_OUTPUT_RECYCLING:
	recycling->next->prev = recycling;
	recycling = recycling->next;
	    	    
	channel = channel->next_pad;
      }

      end_channel = ags_channel_pad_last(start_channel);
      end_recycling = recycling;

      /* setting up parameters */
      parameter[i].name = "start_channel";
      g_value_init(&(parameter[i].value), G_TYPE_OBJECT);
      g_value_set_object(&(parameter[i].value), start_channel);

      parameter[i].name = "end_channel";
      g_value_init(&(parameter[i].value), G_TYPE_OBJECT);
      g_value_set_object(&(parameter[i].value), end_channel);

      parameter[i].name = "start_recycling";
      g_value_init(&(parameter[i].value), G_TYPE_OBJECT);
      g_value_set_object(&(parameter[i].value), start_recycling);

      parameter[i].name = "end_recycling";
      g_value_init(&(parameter[i].value), G_TYPE_OBJECT);
      g_value_set_object(&(parameter[i].value), end_recycling);
    }

    return(parameter);
  }
  void ags_audio_set_flags_add_recycling_task(GParameter *parameter){
    AgsAudioSetRecycling *audio_set_recycling;

    /* create set recycling task */
    audio_set_recycling = ags_audio_set_recycling_new(audio,
						      parameter);

    /* append AgsAudioSetRecycling */
    ags_task_thread_append_task(AGS_DEVOUT(audio->devout)->task_thread,
				AGS_TASK(audio_set_recycling));
  }

  if(audio == NULL || !AGS_IS_AUDIO(audio)){
    return;
  }

  if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags))){
    GParameter *parameter;
        
    /* check if output has already recyclings */
    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0){
      if(audio->output_pads > 0){
	parameter = ags_audio_set_flags_set_recycling_parameter(AGS_TYPE_OUTPUT);
	ags_audio_set_flags_add_recycling_task(parameter);
      }
      
      audio->flags |= AGS_AUDIO_OUTPUT_HAS_RECYCLING;
    }

    /* check if input has already recyclings */
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
      if(audio->input_pads > 0){
	parameter = ags_audio_set_flags_set_recycling_parameter(AGS_TYPE_INPUT);
	ags_audio_set_flags_add_recycling_task(parameter);
      }

      audio->flags |= AGS_AUDIO_INPUT_HAS_RECYCLING;
    }
  }
}
    

/**
 * ags_audio_set_flags:
 * @audio an AgsAudio
 * @flags see enum AgsAudioFlags
 *
 * Disable a feature of AgsAudio.
 */
void
ags_audio_unset_flags(AgsAudio *audio, guint flags)
{
  auto GParameter* ags_audio_unset_flags_set_recycling_parameter(GType type);
  auto void ags_audio_unset_flags_add_recycling_task(GParameter *parameter);

  GParameter* ags_audio_unset_flags_set_recycling_parameter(GType type){
    AgsChannel *channel, *start_channel, *end_channel;
    AgsRecycling *recycling, *recycling_next, *start_recycling, *end_recycling;
    GParameter *parameter;
    int i;

    parameter = g_new(GParameter, 4 * audio->audio_channels);

    for(i = 0; i < audio->audio_channels; i++){
      start_channel = ags_channel_nth(((g_type_is_a(type, AGS_TYPE_INPUT)) ? audio->input: audio->output), i);
      end_channel = ags_channel_pad_last(start_channel);

      start_recycling = NULL;
      end_recycling = NULL;

      /* setting up parameters */
      parameter[i].name = "start_channel";
      g_value_init(&(parameter[i].value), G_TYPE_OBJECT);
      g_value_set_object(&(parameter[i].value), start_channel);

      parameter[i].name = "end_channel";
      g_value_init(&(parameter[i].value), G_TYPE_OBJECT);
      g_value_set_object(&(parameter[i].value), end_channel);

      parameter[i].name = "start_recycling";
      g_value_init(&(parameter[i].value), G_TYPE_OBJECT);
      g_value_set_object(&(parameter[i].value), start_recycling);

      parameter[i].name = "end_recycling";
      g_value_init(&(parameter[i].value), G_TYPE_OBJECT);
      g_value_set_object(&(parameter[i].value), end_recycling);
    }

    return(parameter);
  }
  void ags_audio_unset_flags_add_recycling_task(GParameter *parameter){
    AgsAudioSetRecycling *audio_set_recycling;

    /* create set recycling task */
    audio_set_recycling = ags_audio_set_recycling_new(audio,
						      parameter);

    /* append AgsAudioSetRecycling */
    ags_task_thread_append_task(AGS_DEVOUT(audio->devout)->task_thread,
				AGS_TASK(audio_set_recycling));
  }

  if(audio == NULL || !AGS_IS_AUDIO(audio)){
    return;
  }
  
  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags))){
    GParameter *parameter;
    
    /* check if input has already no recyclings */
    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){      
      if(audio->input_pads > 0){
	parameter = ags_audio_unset_flags_set_recycling_parameter(AGS_TYPE_INPUT);
	ags_audio_unset_flags_add_recycling_task(parameter);
	
	audio->flags &= (~AGS_AUDIO_INPUT_HAS_RECYCLING);
      }
      
      /* check if output has already recyclings */
      if(audio->output_pads > 0){
	parameter = ags_audio_unset_flags_set_recycling_parameter(AGS_TYPE_OUTPUT);
	ags_audio_unset_flags_add_recycling_task(parameter);
	
	audio->flags &= (~AGS_AUDIO_OUTPUT_HAS_RECYCLING);
      }
    }
  }
}

/**
 * ags_audio_real_set_audio_channels:
 *
 *
 * Resize audio channels AgsInput will be allocated first.
 */
void
ags_audio_real_set_audio_channels(AgsAudio *audio,
				  guint audio_channels, guint audio_channels_old)
{
  AgsChannel *channel, *input, *input_pad_last;
  AgsRecycling *recycling;
  GObject *parent; // of recycling
  GList *list;
  guint pads, i, j;
  gboolean alloc_recycling;
  gboolean link_recycling; // affects AgsInput
  gboolean set_sync_link, set_async_link; // affects AgsOutput
  void ags_audio_set_audio_channels_init_parameters(GType type){
    if(type == AGS_TYPE_OUTPUT){
      link_recycling = FALSE;

      if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
	alloc_recycling = TRUE;
      }else{
	alloc_recycling = FALSE;

	if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
	  input = ags_channel_nth(audio->input, audio->audio_channels);

	  if((AGS_AUDIO_SYNC & (audio->flags)) != 0 && (AGS_AUDIO_ASYNC & (audio->flags)) == 0){
	    set_sync_link = FALSE;
	    set_async_link = TRUE;
	  }else if((AGS_AUDIO_ASYNC & (audio->flags)) != 0){
	    set_async_link = TRUE;
	    set_sync_link = FALSE;
	    input_pad_last = ags_channel_nth(input, audio->input_lines - (audio_channels - audio->audio_channels));
	  }else{
	    g_message("ags_audio_set_audio_channels - warning: AGS_AUDIO_SYNC nor AGS_AUDIO_ASYNC weren't defined\0");
	    set_sync_link = FALSE;
	    set_async_link = FALSE;
	  }
	}
      }
    }else{
      set_sync_link = FALSE;
      set_async_link = FALSE;
      
      if((AGS_AUDIO_INPUT_HAS_RECYCLING & audio->flags) != 0)
	alloc_recycling = TRUE;
      else
	alloc_recycling = FALSE;

      if((AGS_AUDIO_ASYNC & audio->flags) != 0 && alloc_recycling)
	link_recycling = TRUE;
      else
	link_recycling = FALSE;
    }    
  }
  void ags_audio_set_audio_channels_grow_one(GType type){
    channel = (AgsChannel *) g_object_new(type,
					  "audio\0", (GObject *) audio,
					  NULL);

    if(type == AGS_TYPE_OUTPUT){
      /* AGS_TYPE_OUTPUT */
      audio->output = channel;
      pads = audio->output_pads;
    }else{
      /* AGS_TYPE_INPUT */
      audio->input = channel;
      pads = audio->input_pads;
    }

    if(alloc_recycling){
      recycling =
	channel->first_recycling =
	channel->last_recycling = ags_recycling_new(audio->devout);

      channel->first_recycling->channel = (GObject *) channel;

      //      ags_garbage_collector_add(AGS_DEVOUT(audio->devout)->garbage_collector, recycling);
    }else if(set_sync_link){
      /* set sync link */
      channel->first_recycling = input->first_recycling;
      channel->last_recycling = input->last_recycling;

      input = ags_channel_nth(input, audio_channels);
    }else if(set_async_link){

      /* set async link */
      channel->first_recycling = input->first_recycling;
      channel->last_recycling = input_pad_last->last_recycling;

      input = input->next;
      input_pad_last = input_pad_last->next;
    }

    for(j = 1; j < pads; j++){
      channel->next =
	channel->next_pad = (AgsChannel *) g_object_new(type,
							"audio\0", (GObject *) audio,
							NULL);
      channel->next->prev = channel;
      channel = channel->next;
      channel->prev_pad = channel->prev;

      channel->pad = j;
      channel->line = j;

      if(alloc_recycling){
	channel->first_recycling =
	  channel->last_recycling = ags_recycling_new(audio->devout);

	if(link_recycling){
	  recycling->next = channel->first_recycling;
	  recycling->next->prev = recycling;
	  recycling = recycling->next;
	}

	channel->first_recycling->channel = (GObject *) channel;

	//	ags_garbage_collector_add(AGS_DEVOUT(audio->devout)->garbage_collector, channel->first_recycling);
      }else if(set_sync_link){
	/* set sync link */
	channel->first_recycling = input->first_recycling;
	channel->last_recycling = input->last_recycling;
	
	input = input->next;
      }
      /* set async link is illegal here */
    }
  }
  void ags_audio_set_audio_channels_grow(GType type){
    AgsChannel *prev_channel, *prev_pad, *next_channel, *start;
    AgsRecycling *prev_channel_next_recycling;

    if(type == AGS_TYPE_OUTPUT){
      //      prev_pad = audio->output;
      prev_channel = ags_channel_nth(audio->output, audio->audio_channels - 1);
      pads = audio->output_pads;
    }else{
      //      prev_pad = audio->input;
      prev_channel = ags_channel_nth(audio->input, audio->audio_channels - 1);
      pads = audio->input_pads;
    }

    /*
     * linking with prev_channel is done later else linked channels would be lost
     */
    start =
      channel = (AgsChannel *) g_object_new(type,
					    "audio\0", (GObject *) audio,
					    NULL);

    channel->audio_channel = audio->audio_channels;
    channel->line = audio->audio_channels;

    if(alloc_recycling){
      recycling =
	channel->first_recycling =
	channel->last_recycling = ags_recycling_new(audio->devout);

      if(link_recycling){
	prev_channel->last_recycling->next = recycling;
	recycling->prev = prev_channel->last_recycling;
      }

      channel->first_recycling->channel = (GObject *) channel;

      //      ags_garbage_collector_add(AGS_DEVOUT(audio->devout)->garbage_collector, recycling);
    }else if(set_sync_link){
      /* set sync link */
      channel->first_recycling = input->first_recycling;
      channel->last_recycling = input->last_recycling;
      input = input->next;
    }else if(set_async_link){
      /* set async link*/
      channel->first_recycling = input->first_recycling;
      channel->last_recycling = input_pad_last->last_recycling;

      input = input->next;
      input_pad_last = input_pad_last->next;
    }

    //    i = 1;

    /*
     * link now the new allocated channels
     */
    next_channel = prev_channel->next;

    prev_channel->next = start;
    start->prev = prev_channel;
    prev_channel = next_channel;

    for(j = audio->audio_channels + 1; j < audio_channels; j++){
      channel->next = (AgsChannel *) g_object_new(type,
						  "audio\0", (GObject *) audio,
						  NULL);
      channel->next->prev = channel;
      channel = channel->next;

      channel->pad = 0;
      channel->audio_channel = j;
      channel->line = j;

      if(alloc_recycling){
	channel->first_recycling =
	  channel->last_recycling = ags_recycling_new(audio->devout);

	if(link_recycling){
	  recycling->next = channel->first_recycling;
	  recycling->next->prev = recycling;
	  recycling = recycling->next;
	}

	channel->first_recycling->channel = (GObject *) channel;

	//	ags_garbage_collector_add(AGS_DEVOUT(audio->devout)->garbage_collector, channel->first_recycling);
      }else if(set_sync_link){
	/* set sync link */
	channel->first_recycling = input->first_recycling;
	channel->last_recycling = input->last_recycling;
	input = input->next;
      }else if(set_async_link){
	/* set async link */
	channel->first_recycling = input->first_recycling;
	channel->last_recycling = input_pad_last->last_recycling;

	input = input->next;
	input_pad_last = input_pad_last->next;
      }
    }

    /*
     * join together recycling
     */
    if(link_recycling){
      recycling->next = prev_channel->first_recycling;
      prev_channel->first_recycling->prev = recycling;
    }

    if(pads == 1)
      return;

    /*
     * the main part of allocation
     */
    for(i = 1; i < pads -1; i++){
      channel->next = prev_channel;
      prev_channel->prev = channel;

      /* set async link is illegal here */

      for(j = 0; j < audio->audio_channels -1; j++){
	prev_channel->line = i * audio_channels + j;

	prev_channel = prev_channel->next;

	if(link_recycling){
	  recycling = recycling->next;
	}
      }

      prev_channel->line = i * audio_channels + j;

      // prev_channel = prev_channel->next; will be called later

      if(link_recycling){
	recycling = recycling->next;
      }

      prev_pad = start;

      /* alloc new AgsChannel's */
      start =
	channel = (AgsChannel *) g_object_new(type,
					      "audio\0", (GObject *) audio,
					      NULL);

      next_channel = prev_channel->next;

      prev_channel->next = start;
      start->prev = prev_channel;
      prev_channel = next_channel;

      j++;

      goto ags_audio_set_audio_channels_grow0;

      for(; j < audio_channels; j++){
	channel->next = (AgsChannel *) g_object_new(type,
						    "audio\0", (GObject *) audio,
						    NULL);

	channel->next->prev = channel;
	channel = channel->next;

      ags_audio_set_audio_channels_grow0:

	prev_pad->next_pad = channel;
	channel->prev_pad = prev_pad;

	channel->pad = i;
	channel->audio_channel = j;
	channel->line = (i * audio_channels) + j;

	if(alloc_recycling){
	  channel->first_recycling =
	    channel->last_recycling = ags_recycling_new(audio->devout);

	  if(link_recycling){
	    recycling->next = channel->first_recycling;
	    recycling->next->prev = recycling;
	    recycling = recycling->next;
	  }

	  channel->first_recycling->channel = (GObject *) channel;

	  //	  ags_garbage_collector_add(AGS_DEVOUT(audio->devout)->garbage_collector, channel->first_recycling);
	}else if(set_sync_link){
	  /* set sync link */
	  channel->first_recycling = input->first_recycling;
	  channel->last_recycling = input->last_recycling;
	  input = input->next;
	}
	/* set async link is illegal here */

	prev_pad = prev_pad->next;
      }

      if(link_recycling){
	recycling->next = prev_channel->first_recycling;
	prev_channel->first_recycling->prev = recycling;
      }
    }

    channel->next = prev_channel;
    prev_channel->prev = channel;

    /* set async link is illegal here */

    for(j = 0; j < audio->audio_channels -1; j++){
      prev_channel->line = i * audio_channels + j;

      prev_channel = prev_channel->next;

      if(link_recycling)
	recycling = recycling->next;
    }

    prev_channel->line = i * audio_channels + j;

    start =
      channel = (AgsChannel *) g_object_new(type,
					    "audio\0", (GObject *) audio,
					    NULL);

    prev_channel->next = start;
    start->prev = prev_channel;
    j++;

    goto ags_audio_set_audio_channels_grow1;

    for(; j < audio_channels; j++){
      channel->next = (AgsChannel *) g_object_new(type,
						  "audio\0", (GObject *) audio,
						  NULL);

      channel->next->prev = channel;
      channel = channel->next;

    ags_audio_set_audio_channels_grow1:

      prev_pad->next_pad = channel;
      channel->prev_pad = prev_pad;

      channel->pad = i;
      channel->audio_channel = j;
      channel->line = (i * audio_channels) + j;

      if(alloc_recycling){
	channel->first_recycling =
	  channel->last_recycling = ags_recycling_new(audio->devout);

	if(link_recycling){
	  recycling->next = channel->first_recycling;
	  recycling->next->prev = recycling;
	  recycling = recycling->next;
	}

	channel->first_recycling->channel = (GObject *) channel;

	//	ags_garbage_collector_add(AGS_DEVOUT(audio->devout)->garbage_collector, channel->first_recycling);
      }else if(set_sync_link){
	channel->first_recycling = input->first_recycling;
	channel->last_recycling = input->last_recycling;
	input = input->next;
      }
      /* set async link is illegal here */

      prev_pad = prev_pad->next;
    }
  }
  void ags_audio_set_audio_channels_shrink_zero(){
    AgsChannel *start, *channel_next;
    AgsRecycling *prev_recycling, *next_recycling;
    gboolean first_run;
    GError *error;

    start =
      channel = audio->output;
    first_run = TRUE;

    error = NULL;

  ags_audio_set_audio_channel_shrink_zero0:

    while(channel != NULL){
      ags_channel_set_link(channel, NULL, &error);
      channel = channel->next;
    }

    channel = start;

    while(channel != NULL){
      channel_next = channel->next;

      g_object_unref((GObject *) channel);

      channel = channel_next;
    }

    if(first_run){
      start =
	channel = audio->input;
      first_run = FALSE;
      goto ags_audio_set_audio_channel_shrink_zero0;
    }

    audio->output = NULL;
    audio->input = NULL;
  }
  void ags_audio_set_audio_channels_shrink(){
    AgsChannel *channel0, *channel1, *start;
    AgsRecycling *recycling;
    gboolean first_run;
    GError *error;

    start =
      channel = audio->output;
    pads = audio->output_pads;
    first_run = TRUE;

    error = NULL;

  ags_audio_set_audio_channel_shrink0:

    for(i = 0; i < pads; i++){
      channel = ags_channel_nth(channel, audio_channels);

      for(j = audio_channels; j < audio->audio_channels; j++){
	ags_channel_set_link(channel, NULL, &error);
	channel = channel->next;
      }
    }

    channel = start;

    if(i < pads){
      for(i = 0; ; i++){
	for(j = 0; j < audio_channels -1; j++){
	  channel->pad = i;
	  channel->audio_channel = j;
	  channel->line = i * audio_channels + j;

	  channel = channel->next;
	}

	channel->pad = i;
	channel->audio_channel = j;
	channel->line = i * audio_channels + j;

	channel0 = channel->next;
	
	for(; j < audio->audio_channels; j++){
	  channel1 = channel0->next;

	  g_object_unref((GObject *) channel0);

	  channel0 = channel1;
	}

	channel->next = channel1;

	if(channel1 != NULL)
	  channel1->prev = channel;
	else
	  break;

	channel = channel1;
      }
    }

    if(first_run){
      first_run = FALSE;
      start =
	channel = audio->input;
      pads = audio->input_pads;

      goto ags_audio_set_audio_channel_shrink0;
    }
  }
  void ags_audio_set_audio_channels_grow_notation(){
    GList *list;
    guint i;

    i = audio->audio_channels;

    g_message("ags_audio_set_audio_channels_grow_notation\n\0");

    if(audio->audio_channels == 0){
      audio->notation =
	list = g_list_alloc();
      goto ags_audio_set_audio_channels_grow_notation0;
    }else{
      list = g_list_nth(audio->notation, audio->audio_channels - 1);
    }

    for(; i < audio_channels; i++){
      list->next = g_list_alloc();
      list->next->prev = list;
      list = list->next;

    ags_audio_set_audio_channels_grow_notation0:
      list->data = (gpointer) ags_notation_new(i);
    } 
  }
  void ags_audio_set_audio_channels_shrink_notation(){
    GList *list, *list_next;

    list = g_list_nth(audio->notation, audio_channels);

    if(audio_channels == 0){
      audio->notation = NULL;
    }else{
      list->prev->next = NULL;
    }

    while(list != NULL){
      list_next = list->next;

      g_object_unref((GObject *) list->data);
      g_list_free1(list);

      list = list_next;
    }
  }

  /* entry point */
  if(audio_channels > audio->audio_channels){
    /* grow audio channels*/
    if((AGS_AUDIO_HAS_NOTATION & audio->flags) != 0)
      ags_audio_set_audio_channels_grow_notation();

    if(audio->audio_channels == 0){
      if(audio->input_pads > 0){
	ags_audio_set_audio_channels_init_parameters(AGS_TYPE_INPUT);
	ags_audio_set_audio_channels_grow_one(AGS_TYPE_INPUT);
      }

      audio->input_lines = audio->input_pads;

      if(audio->output_pads > 0){
	ags_audio_set_audio_channels_init_parameters(AGS_TYPE_OUTPUT);
	ags_audio_set_audio_channels_grow_one(AGS_TYPE_OUTPUT);
      }

      audio->audio_channels = 1;
      audio->output_lines = audio->output_pads;
    }

    if(audio_channels > 1){
      if(audio->input_pads > 0){
	ags_audio_set_audio_channels_init_parameters(AGS_TYPE_INPUT);
	ags_audio_set_audio_channels_grow(AGS_TYPE_INPUT);
      }

      audio->input_lines = audio_channels * audio->input_pads;

      if(audio->output_pads > 0){
	ags_audio_set_audio_channels_init_parameters(AGS_TYPE_OUTPUT);
     	ags_audio_set_audio_channels_grow(AGS_TYPE_OUTPUT);
      }
    }
  }else if(audio_channels < audio->audio_channels){
    /* shrink audio channels */
    if((AGS_AUDIO_HAS_NOTATION & audio->flags) != 0)
      ags_audio_set_audio_channels_shrink_notation();

    if(audio_channels == 0){
      ags_audio_set_audio_channels_shrink_zero();
    }else{
      ags_audio_set_audio_channels_shrink();
    }

    audio->input_lines = audio_channels * audio->input_pads;
  }

  audio->audio_channels = audio_channels;
  // input_lines must be set earlier because set_sync_link needs it
  audio->output_lines = audio_channels * audio->output_pads;
}

void
ags_audio_set_audio_channels(AgsAudio *audio, guint audio_channels)
{
  g_return_if_fail(AGS_IS_AUDIO(audio));

  g_object_ref((GObject *) audio);
  g_signal_emit(G_OBJECT(audio),
		audio_signals[SET_AUDIO_CHANNELS], 0,
		audio_channels, audio->audio_channels);
  g_object_unref((GObject *) audio);
}

/*
 * resize
 * AgsInput has to be allocated first
 */
void
ags_audio_real_set_pads(AgsAudio *audio,
			GType type,
			guint pads, guint pads_old)
{
  AgsChannel *start, *channel, *prev_pad, *input, *input_pad_last;
  AgsRecycling *recycling;
  guint i, j;
  gboolean alloc_recycling, link_recycling, set_sync_link, update_async_link, set_async_link;
  void ags_audio_set_pads_grow_one(){
    start =
      channel = (AgsChannel *) g_object_new(type,
					    "audio\0", (GObject *) audio,
					    NULL);

    if(alloc_recycling){
      channel->first_recycling =
	channel->last_recycling = ags_recycling_new(audio->devout);

      if(link_recycling)
	recycling = channel->first_recycling;

      channel->first_recycling->channel = (GObject *) channel;

      //      ags_garbage_collector_add(AGS_DEVOUT(audio->devout)->garbage_collector, channel->first_recycling);
    }else if(set_sync_link){
      input = audio->input;
      channel->first_recycling = input->first_recycling;
      channel->last_recycling = input->last_recycling;
      input = input->next;
    }else if(set_async_link){
      input = audio->input;
      input_pad_last = ags_channel_nth(audio->input, audio->input_lines - audio->audio_channels);

      channel->first_recycling = input->first_recycling;
      channel->last_recycling = input_pad_last->last_recycling;

      input = input->next;
      input_pad_last = input_pad_last->next;
    }

    for(j = 1; j < audio->audio_channels; j++){
      channel->next = (AgsChannel *) g_object_new(type,
						  "audio\0", (GObject *) audio,
						  NULL);
      channel->next->prev = channel;
      channel = channel->next;

      channel->audio_channel = j;
      channel->line = j;

      if(alloc_recycling){
	channel->first_recycling =
	  channel->last_recycling = ags_recycling_new(audio->devout);

	if(link_recycling){
	  recycling->next = channel->first_recycling;
	  recycling->next->prev = recycling;
	  recycling = recycling->next;
	}

	channel->first_recycling->channel = (GObject *) channel;

	//	ags_garbage_collector_add(AGS_DEVOUT(audio->devout)->garbage_collector, channel->first_recycling);
      }else if(set_sync_link){
	channel->first_recycling = input->first_recycling;
	channel->last_recycling = input->last_recycling;

	input = input->next;
      }else if(set_async_link){
	channel->first_recycling = input->first_recycling;
	channel->last_recycling = input_pad_last->last_recycling;

	input = input->next;
	input_pad_last = input_pad_last->next;
      }
    }
  }
  void ags_audio_set_pads_grow(){
    AgsRecycling *recycling_next;

    prev_pad = ags_channel_nth(channel, (pads_old - 1) * audio->audio_channels);
    channel = ags_channel_last(prev_pad);

    if(alloc_recycling){
      recycling = channel->last_recycling;
      recycling_next = channel->last_recycling->next;
    }else if(set_sync_link){
      if(pads_old != 0)
	input = ags_channel_nth(audio->input, pads_old);
    }

    for(i = pads_old; i < pads; i++){
      if(set_async_link){
	input = audio->input;
	input_pad_last = ags_channel_nth(audio->input, audio->input_lines - audio->audio_channels);
      }

      for(j = 0; j < audio->audio_channels; j++){
	channel->next = (AgsChannel *) g_object_new(type,
						    "audio\0", (GObject *) audio,
						    NULL);
	channel->next->prev = channel;
	channel = channel->next;

	channel->prev_pad = prev_pad;
	prev_pad->next_pad = channel;

	channel->pad = i;
	channel->audio_channel = j;
	channel->line = i * audio->audio_channels + j;

	if(alloc_recycling){
	  channel->first_recycling =
	    channel->last_recycling = ags_recycling_new(audio->devout);

	  if(link_recycling){
	    recycling->next = channel->first_recycling;
	    recycling->next->prev = recycling;
	    recycling = recycling->next;
	  }

	  channel->first_recycling->channel = (GObject *) channel;

	  //	  ags_garbage_collector_add(AGS_DEVOUT(audio->devout)->garbage_collector, channel->first_recycling);
	}else if(set_sync_link){
	  channel->first_recycling = input->first_recycling;
	  channel->last_recycling = input->last_recycling;

	  input = input->next;
	}else if(set_async_link){
	  channel->first_recycling = input->first_recycling;
	  channel->last_recycling = input_pad_last->last_recycling;

	  input = input->next;
	  input_pad_last = input_pad_last->next;
	}

	prev_pad = prev_pad->next;
      }
    }

    if(alloc_recycling && link_recycling){
      recycling->next = recycling_next;

      if(recycling_next != NULL)
	recycling_next->prev = recycling;
    }
  }
  void ags_audio_set_pads_unlink_zero(){
    AgsChannel *start;
    GError *error;

    start = channel;

    while(channel != NULL){
      error = NULL;
      ags_channel_set_link(channel, NULL, &error);

      if(error != NULL){
	g_error("%s\0", error->message);
      }

      channel = channel->next;
    }

    channel = start;
  }
  void ags_audio_set_pads_unlink(){
    AgsChannel *start;

    start = channel;
    channel = ags_channel_nth(channel, pads * audio->audio_channels);

    ags_audio_set_pads_unlink_zero();

    channel = start;
  }
  void ags_audio_set_pads_shrink_zero(){
    AgsChannel *channel_next;
    GError *error;

    while(channel != NULL){
      channel_next = channel->next;

      g_object_unref((GObject *) channel);

      channel = channel_next;
    }
  }
  void ags_audio_set_pads_shrink(){
    channel = ags_channel_nth(channel, (pads - 1) * audio->audio_channels);

    for(i = 0; i < audio->audio_channels; i++){
      channel->next_pad = NULL;

      channel = channel->next;
    }

    channel->prev->next = NULL;
    ags_audio_set_pads_shrink_zero();
  }
  void ags_audio_set_pads_alloc_notation(){
    GList *list;
    guint i;

    g_message("ags_audio_set_pads_alloc_notation\n\0");

    if(audio->audio_channels > 0){
      audio->notation =
	list = g_list_alloc();
      i = 0;
      goto ags_audio_set_pads_alloc_notation0;
    }else
      return;

    for(; i < audio->audio_channels; i++){
      list->next = g_list_alloc();
      list->next->prev = list;
      list = list->next;
    ags_audio_set_pads_alloc_notation0:

      list->data = (gpointer) ags_notation_new(i);
    }
  }
  void ags_audio_set_pads_free_notation(){
    GList *list, *list_next;

    if(audio->audio_channels > 0){
      list = audio->notation;
    }else
      return;

    while(list != NULL){
      list_next = list->next;

      g_object_unref(G_OBJECT(list->data));
      g_list_free1(list);

      list = list_next;
    }
  }
  void ags_audio_set_pads_add_notes(){
    /* -- useless --
    GList *list;

    list = audio->notation;

    while(list != NULL){
      AGS_NOTATION(list->data)->pads = pads;

      list = list->next;
    }
    */
  }
  void ags_audio_set_pads_remove_notes(){
    AgsNotation *notation;
    GList *notation_i, *note, *note_next;

    notation_i = audio->notation;

    while(notation_i != NULL){
      notation = AGS_NOTATION(notation_i->data);
      note = notation->notes;

      while(note != NULL){
	note_next = note->next;

	if(AGS_NOTE(note->data)->y >= pads){
	  if(note->prev != NULL)
	    note->prev->next = note_next;
	  else
	    notation->notes = note_next;

	  if(note_next != NULL)
	    note_next->prev = note->prev;

	  free(note->data);
	  g_list_free1(note);
	}

	note = note_next;
      }

      notation_i = notation_i->next;
    }
  }

  alloc_recycling = FALSE;
  link_recycling = FALSE;
  set_sync_link = FALSE;
  update_async_link = FALSE;
  set_async_link = FALSE;

  if(type == AGS_TYPE_OUTPUT){
    pads_old = audio->output_pads;
    link_recycling = FALSE;

    if(pads_old == pads)
      return;

    if(audio->audio_channels == 0){
      audio->output_pads = pads;
      return;
    }

    if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0)
      alloc_recycling = TRUE;
    else if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0)
      if((AGS_AUDIO_SYNC & audio->flags) != 0 && (AGS_AUDIO_ASYNC & audio->flags) == 0)
	set_sync_link = TRUE;
      else if((AGS_AUDIO_ASYNC & audio->flags) == 0)
	input = audio->input;

    if(pads_old == 0){
      if((AGS_AUDIO_HAS_NOTATION & (audio->flags)) != 0 &&
	 audio->notation == NULL){
	ags_audio_set_pads_alloc_notation();
	ags_audio_set_pads_add_notes();
      }

      ags_audio_set_pads_grow_one();
      channel =
	audio->output = start;

      pads_old =
	audio->output_pads = 1;
    }else
      channel = audio->output;

    if(pads > audio->output_pads){
      ags_audio_set_pads_grow();

    }else if(pads == 0){
      if((AGS_AUDIO_HAS_NOTATION & (audio->flags)) != 0 &&
	 audio->notation != NULL)
	ags_audio_set_pads_free_notation();

      ags_audio_set_pads_unlink_zero();


      ags_audio_set_pads_shrink_zero();
      audio->output = NULL;

    }else if(pads < audio->output_pads){
      ags_audio_set_pads_remove_notes();

      ags_audio_set_pads_unlink();

      ags_audio_set_pads_shrink();

    }

    audio->output_pads = pads;
    audio->output_lines = pads * audio->audio_channels;

    if((AGS_AUDIO_SYNC & audio->flags) != 0 && (AGS_AUDIO_ASYNC & audio->flags) == 0){
      audio->input_pads = pads;
      audio->input_lines = pads * audio->audio_channels;
    }
  }else if(type == AGS_TYPE_INPUT){
    pads_old = audio->input_pads;

    if(pads_old == pads)
      return;

    if(audio->audio_channels == 0){
      audio->input_pads = pads;
      return;
    }

    if((AGS_AUDIO_INPUT_HAS_RECYCLING & (audio->flags)) != 0){
      alloc_recycling = TRUE;

      if((AGS_AUDIO_ASYNC & audio->flags) != 0){
	link_recycling = TRUE;

	if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) == 0)
	  update_async_link = TRUE;
      }
    }
    
    if(pads_old == 0){
      ags_audio_set_pads_grow_one();
      channel =
	audio->input = start;

      pads_old =
	audio->input_pads = 1;
    }else
      channel = audio->input;

    if(pads > 1)
      if(pads > audio->input_pads){
	ags_audio_set_pads_grow();

      }else if(pads == 0){
	ags_audio_set_pads_unlink_zero();

	ags_audio_set_pads_shrink_zero();
	audio->input = NULL;

      }else if(pads < audio->input_pads){
	ags_audio_set_pads_unlink();

	ags_audio_set_pads_shrink();
      }

    audio->input_pads = pads;
    audio->input_lines = pads * audio->audio_channels;
  }
}

void
ags_audio_set_pads(AgsAudio *audio, GType type, guint pads)
{
  guint pads_old;

  g_return_if_fail(AGS_IS_AUDIO(audio));

  g_object_ref((GObject *) audio);
  pads_old = (type == AGS_TYPE_OUTPUT) ? audio->output_pads: audio->input_pads;
  g_signal_emit(G_OBJECT(audio),
		audio_signals[SET_PADS], 0,
		type, pads, pads_old);
  g_object_unref((GObject *) audio);
}

/*
 * AgsAudioSignal related
 */
void
ags_audio_set_sequence_length(AgsAudio *audio, guint sequence_length)
{
  audio->sequence_length = sequence_length;
}


void
ags_audio_find_group_id_from_child(AgsAudio *audio,
				   AgsChannel *input, AgsRecallID *input_recall_id, gboolean input_do_recall,
				   AgsRecallID **child_recall_id, gboolean *child_do_recall)
{
  if((AGS_AUDIO_INPUT_HAS_RECYCLING & (AGS_AUDIO(audio)->flags)) != 0){
    (*child_recall_id) = ags_recall_id_find_parent_group_id(input->link->recall_id, input_recall_id->group_id);
    (*child_do_recall) = TRUE;
  }else{
    (*child_recall_id) = ags_recall_id_find_group_id(input->link->recall_id, input_recall_id->group_id);
    (*child_do_recall) = input_do_recall;
  }
}

void
ags_audio_add_run_order(AgsAudio *audio, AgsRunOrder *run_order)
{
  audio->run_order = g_list_prepend(audio->run_order, run_order);
}

void
ags_audio_remove_run_order(AgsAudio *audio, AgsRunOrder *run_order)
{
  audio->run_order = g_list_remove(audio->run_order, run_order);
}

void
ags_audio_add_recall_id(AgsAudio *audio, GObject *recall_id)
{
  /*
   * TODO:JK: thread synchronisation
   */

  audio->recall_id = g_list_prepend(audio->recall_id, recall_id);
}

void
ags_audio_remove_recall_id(AgsAudio *audio, GObject *recall_id)
{
  /*
   * TODO:JK: thread synchronisation
   */

  audio->recall_id = g_list_remove(audio->recall_id, recall_id);
}

void
ags_audio_add_recall_container(AgsAudio *audio, GObject *recall_container)
{
  /*
   * TODO:JK: thread synchronisation
   */

  audio->container = g_list_prepend(audio->container, recall_container);
}

void
ags_audio_remove_recall_container(AgsAudio *audio, GObject *recall_container, gboolean play)
{
  /*
   * TODO:JK: thread synchronisation
   */

  audio->container = g_list_remove(audio->container, recall_container);
}

void
ags_audio_add_recall(AgsAudio *audio, GObject *recall, gboolean play)
{
  /*
   * TODO:JK: thread synchronisation
   */

  if(play){
    audio->play = g_list_append(audio->play, recall);
  }else{
    audio->recall = g_list_append(audio->recall, recall);
  }
}

void
ags_audio_remove_recall(AgsAudio *audio, GObject *recall, gboolean play)
{
  /*
   * TODO:JK: thread synchronisation
   */

  if(play){
    audio->play = g_list_remove(audio->play, recall);
  }else{
    audio->recall = g_list_remove(audio->recall, recall);
  }
}

/*
 * AgsRecall related
 */
void ags_audio_resolve_recall(AgsAudio *audio,
			      AgsRecycling *first_recycling, AgsRecycling *last_recycling,
			      AgsGroupId group_id)
{
  AgsRecall *recall;
  AgsRecallID *recall_id;
  GList *list_recall;
  
  recall_id = ags_recall_id_find_group_id_with_recycling(audio->recall_id,
							 group_id,
							 first_recycling, last_recycling);

  if(recall_id == NULL)
    g_message("group_id = %llu\n\0", (long long unsigned int) group_id);
  
  if((AGS_RECALL_ID_AUDIO_RESOLVED & (recall_id->flags)) != 0){
    return;
  }

  recall_id->flags |= AGS_RECALL_ID_AUDIO_RESOLVED;
  
  /* get the appropriate lists */
  if(recall_id->parent_group_id == 0){
    list_recall = audio->play;
  }else{
    list_recall = audio->recall;
  }
  
  while((list_recall = ags_recall_find_group_id(list_recall, group_id)) != NULL){
    recall = AGS_RECALL(list_recall->data);
    
    ags_recall_resolve_dependencies(recall);
    
    list_recall = list_recall->next;
  }
}

void
ags_audio_play(AgsAudio *audio,
	       AgsRecycling *first_recycling, AgsRecycling *last_recycling,
	       AgsGroupId group_id,
	       gint stage, gboolean do_recall)
{
  AgsRecallID *recall_id;
  AgsRecall *recall;
  GList *list, *list_next;
  
  if(do_recall)
    list = audio->recall;
  else
    list = audio->play;

  recall_id = ags_recall_id_find_group_id_with_recycling(audio->recall_id,
							 group_id,
							 first_recycling, last_recycling);

  while(list != NULL){
    list_next = list->next;

    recall = AGS_RECALL(list->data);

    if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0 ||
       recall->recall_id == NULL ||
       (recall->recall_id->group_id != recall_id->group_id)){
      list = list_next;

      continue;
    }
    
    if((AGS_RECALL_HIDE & (recall->flags)) == 0){
      if(stage == 0)
	ags_recall_run_pre(recall);
      else if(stage == 1)
	ags_recall_run_inter(recall);
      else
	ags_recall_run_post(recall);
    }

    ags_recall_child_check_remove(recall);

    if((AGS_RECALL_REMOVE & (recall->flags)) != 0){
      if(do_recall)
	audio->recall = g_list_remove(audio->recall, recall);
      else
	audio->play = g_list_remove(audio->play, recall);

      ags_recall_remove(recall);
    }
    
    list = list_next;
  }
}

/**
 * ags_audio_duplicate_recall:
 * @audio an #AgsAudio
 * @playback duplicate for playback
 * @sequencer duplicate for sequencer
 * @notation duplicate for notation
 * @first_recycling the first #AgsRecycling the #AgsRecall\s belongs to
 * @last_recycling the last #AgsRecycling the #AgsRecall\s belongs to
 * @group_id the current #AgsGroupId
 * @audio_signal_level how many #AgsRecycling\s has been passed until @first_recycling and @last_recycling
 * were reached.
 * @called_by_output
 * 
 * Duplicate all #AgsRecall templates of this #AgsAudio.
 */
void
ags_audio_duplicate_recall(AgsAudio *audio,
			   gboolean playback, gboolean sequencer, gboolean notation,
			   AgsRecycling *first_recycling, AgsRecycling *last_recycling,
			   AgsGroupId group_id,
			   guint audio_signal_level, gboolean called_by_output)
{
  AgsRecall *recall;
  AgsRecallID *recall_id;
  AgsRunOrder *run_order;
  GList *list_recall_start, *list_recall;
  gboolean matches_reality, immediate_new_level;
  
  g_message("ags_audio_duplicate_recall - audio.lines[%u,%u]\n\0", audio->output_lines, audio->input_lines);

  recall_id = ags_recall_id_find_group_id_with_recycling(audio->recall_id,
							 group_id,
							 first_recycling, last_recycling);
  

  if(audio_signal_level == 0)
    list_recall_start = 
      list_recall = audio->play;
  else
    list_recall_start =
      list_recall = audio->recall;
  
  if((AGS_AUDIO_OUTPUT_HAS_RECYCLING & (audio->flags)) != 0){
    immediate_new_level = TRUE;
  }else{
    immediate_new_level = FALSE;
  }

  while(list_recall != NULL){
    recall = AGS_RECALL(list_recall->data);
    
    matches_reality = TRUE;

    //TODO:JK: remove because it's probably deprecated
    /*
    if(AGS_RECALL_DISTINCTS_REAL & (recall->flags) != 0){
      if(AGS_RECALL_IS_REAL & (recall->flags) != 0){
	if(audio_signal_level < 2)
	  matches_reality = FALSE;
      }else if(audio_signal_level > 1)
	matches_reality = FALSE;
    }
    */

    if(AGS_IS_DELAY_AUDIO_RUN(recall) ||
       AGS_IS_COUNT_BEATS_AUDIO_RUN(recall) ||
       AGS_IS_COPY_PATTERN_AUDIO_RUN(recall))
      g_message("\n\0");


    if(((called_by_output && (AGS_RECALL_INPUT_ORIENTATED & (recall->flags)) != 0) ||
	(!called_by_output && (AGS_RECALL_INPUT_ORIENTATED & (recall->flags)) == 0)) ||
       (AGS_RECALL_RUN_INITIALIZED & (recall->flags)) != 0 ||
       AGS_IS_RECALL_AUDIO(recall) ||
       !matches_reality ||
       !((playback && (AGS_RECALL_PLAYBACK & (recall->flags)) != 0) ||
	 (sequencer && (AGS_RECALL_SEQUENCER & (recall->flags)) != 0) ||
	 (notation && (AGS_RECALL_NOTATION & (recall->flags)) != 0)) ||

       ((audio_signal_level != 0) && ((AGS_RECALL_PLAYBACK & (recall->flags)) != 0 ||
				      (((AGS_RECALL_SEQUENCER & (recall->flags)) != 0 ||
					(AGS_RECALL_NOTATION & (recall->flags)) != 0)) &&
				      !(immediate_new_level && audio_signal_level == 1)))){
      list_recall = list_recall->next;
      continue;
    }
    
    if((AGS_RECALL_TEMPLATE & (recall->flags)) != 0){
      GList *list_recall_run_notify;
      
      /* check if the object has already been duplicated */
      list_recall_run_notify = ags_recall_find_type_with_group_id(list_recall_start, G_OBJECT_TYPE(recall), group_id);
      
      if(list_recall_run_notify != NULL){
	/* notify additional run on the recall */
	recall = AGS_RECALL(list_recall_run_notify->data);
	
	ags_recall_notify_dependency(recall, AGS_RECALL_NOTIFY_RUN, 1);
      }else{
	AgsRecallID *current_recall_id;
	AgsGroupId current_group_id;

	current_group_id = ags_recall_get_appropriate_group_id(recall,
							       (GObject *) audio,
							       recall_id,
							       called_by_output);

	current_recall_id = ags_recall_id_find_group_id_with_recycling(audio->recall_id,
								       current_group_id,
								       first_recycling, last_recycling);

	/* duplicate the recall, notify first run and initialize it */
	recall = ags_recall_duplicate(recall, current_recall_id);
	g_message("duplicated: %s\n\0", G_OBJECT_TYPE_NAME(recall));
      
	if(audio_signal_level == 0)
	  audio->play = g_list_append(audio->play, recall);
	else
	  audio->recall = g_list_append(audio->recall, recall);
	
	ags_connectable_connect(AGS_CONNECTABLE(recall));
	
	ags_recall_notify_dependency(recall, AGS_RECALL_NOTIFY_RUN, 1);
      }
    }
    
    list_recall = list_recall->next;
  }
  
  /* set run order */
  run_order = ags_run_order_find_group_id(audio->run_order,
					  recall_id->group_id);
  
  if(run_order == NULL){
    run_order = ags_run_order_new(recall_id);
    ags_audio_add_run_order(audio, run_order);
  }
}

/**
 * ags_audio_init_recall:
 *
 *
 */
void
ags_audio_init_recall(AgsAudio *audio, gint stage,
		      AgsRecycling *first_recycling, AgsRecycling *last_recycling,
		      AgsGroupId group_id)
{
  AgsRecall *recall;
  AgsRecallID *recall_id;
  GList *list_recall;
  
  recall_id = ags_recall_id_find_group_id_with_recycling(audio->recall_id,
							 group_id,
							 first_recycling, last_recycling);

  if(recall_id->parent_group_id == 0)
    list_recall = audio->play;
  else
    list_recall = audio->recall;

  while(list_recall != NULL){
    recall = AGS_RECALL(list_recall->data);
    
    if((AGS_RECALL_RUN_INITIALIZED & (recall->flags)) != 0 ||
       AGS_IS_RECALL_AUDIO(recall)){
      list_recall = list_recall->next;
      continue;
    }
    
    if((AGS_RECALL_TEMPLATE & (recall->flags)) == 0){
      if(stage == 0){
	recall->flags &= (~AGS_RECALL_HIDE);
	ags_recall_run_init_pre(recall);
	recall->flags &= (~AGS_RECALL_REMOVE);
      }else if(stage == 1){
	ags_recall_run_init_inter(recall);
      }else{
	ags_recall_run_init_post(recall);
	
	ags_dynamic_connectable_connect_dynamic(AGS_DYNAMIC_CONNECTABLE(recall));
	//	  recall->flags |= AGS_RECALL_RUN_INITIALIZED;
      }
    }
    
    list_recall = list_recall->next;
  }
}

/*
 * AgsRecall related
 */
guint
ags_audio_recursive_play_init(AgsAudio *audio,
			      gboolean playback, gboolean sequencer, gboolean notation)
{
  AgsChannel *channel;
  AgsGroupId group_id, child_group_id;
  gint stage;
  gboolean arrange_group_id, duplicate_templates, resolve_dependencies;

  group_id = ags_recall_id_generate_group_id();
  child_group_id = ags_recall_id_generate_group_id();

  for(stage = 0; stage < 3; stage++){
    channel = audio->output;

    if(stage == 0){
      arrange_group_id = TRUE;
      duplicate_templates = TRUE;
      resolve_dependencies = TRUE;
    }else{
      arrange_group_id = FALSE;
      duplicate_templates = FALSE;
      resolve_dependencies = FALSE;
    }

    while(channel != NULL){
      ags_channel_recursive_play_init(channel, stage,
				      arrange_group_id, duplicate_templates, playback, sequencer, notation, resolve_dependencies,
				      group_id, child_group_id,
				      0);

      channel = channel->next;
    }
  }

  return(group_id);
}

/*
 * AgsRecall related
 */
void
ags_audio_cancel(AgsAudio *audio,
		 AgsGroupId group_id,
		 AgsRecycling *first_recycling, AgsRecycling *last_recycling,
		 gboolean do_recall)
{
  AgsRecall *recall;
  GList *list, *list_next;
  
  if(do_recall)
    list = audio->recall;
  else
    list = audio->play;

  while(list != NULL){
    list_next = list->next;

    recall = AGS_RECALL(list->data);

    if((AGS_RECALL_TEMPLATE & (recall->flags)) ||
       recall->recall_id == NULL ||
       recall->recall_id->group_id != group_id ||
       !(recall->recall_id->first_recycling != first_recycling && recall->recall_id->last_recycling != last_recycling)){
      list = list_next;

      continue;
    }

    ags_recall_cancel(recall);
    
    list = list_next;
  }
}

/*
 * AgsDevout related
 */
void
ags_audio_set_devout(AgsAudio *audio, GObject *devout)
{
  AgsChannel *channel;
  GList *list;

  /* audio */
  if(audio->devout == devout)
    return;

  if(audio->devout != NULL)
    g_object_unref(audio->devout);

  if(devout != NULL)
    g_object_ref(devout);

  audio->devout = (GObject *) devout;

  /* recall */
  list = audio->play;
  
  while(list != NULL){
    g_object_set(G_OBJECT(list->data),
		 "devout\0", devout,
		 NULL);
    
    list = list->next;
  }
  
  list = audio->recall;
  
  while(list != NULL){
    g_object_set(G_OBJECT(list->data),
		 "devout\0", devout,
		 NULL);
    
    list = list->next;
  }
  
  /* input */
  channel = audio->input;

  while(channel != NULL){
    g_object_set(G_OBJECT(channel),
		 "devout\0", devout,
		 NULL);
    
    channel = channel->next;
  }

  /* output */
  channel = audio->output;

  while(channel != NULL){
    g_object_set(G_OBJECT(channel),
		 "devout\0", devout,
		 NULL);
    
    channel = channel->next;
  }
}

void
ags_audio_open_files(AgsAudio *audio,
		     GSList *filenames,
		     gboolean overwrite_channels,
		     gboolean create_channels)
{
  AgsChannel *channel;
  AgsAudioFile *audio_file;
  AgsAudioSignal *audio_signal_source_old;
  GList *audio_signal_list;
  guint i, j;
  guint list_length;
  GError *error;

  channel = audio->input;

  /* overwriting existing channels */
  if(overwrite_channels){
    if(channel != NULL){
      for(i = 0; i < audio->input_pads && filenames != NULL; i++){
	audio_file = ags_audio_file_new((gchar *) filenames->data,
					(AgsDevout *) audio->devout,
					0, audio->audio_channels);
	if(!ags_audio_file_open(audio_file)){
	  filenames = filenames->next;
	  continue;
	}

	ags_audio_file_read_audio_signal(audio_file);
	ags_audio_file_close(audio_file);
	
	audio_signal_list = audio_file->audio_signal;
	
	for(j = 0; j < audio->audio_channels && audio_signal_list != NULL; j++){
	  /* create task */
	  error = NULL;

	  ags_channel_set_link(channel, NULL,
			       &error);

	  if(error != NULL){
	    g_message(error->message);
	  }

	  AGS_AUDIO_SIGNAL(audio_signal_list->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	  AGS_AUDIO_SIGNAL(audio_signal_list->data)->recycling = (GObject *) channel->first_recycling;
	  audio_signal_source_old = ags_audio_signal_get_template(channel->first_recycling->audio_signal);

	    // FIXME:JK: create a task
	  channel->first_recycling->audio_signal = g_list_remove(channel->first_recycling->audio_signal,
								 (gpointer) audio_signal_source_old);
	  channel->first_recycling->audio_signal = g_list_prepend(channel->first_recycling->audio_signal,
								  audio_signal_list->data);

	  g_object_unref(G_OBJECT(audio_signal_source_old));

	  audio_signal_list = audio_signal_list->next;
	  channel = channel->next;
	}

	if(audio_file->channels < audio->audio_channels)
	  channel = ags_channel_nth(channel,
				    audio->audio_channels - audio_file->channels);
	
	filenames = filenames->next;
      }
    }
  }

  /* appending to channels */
  if(create_channels && filenames != NULL){
    list_length = g_slist_length(filenames);
    
    ags_audio_set_pads((AgsAudio *) audio, AGS_TYPE_INPUT,
		       list_length + AGS_AUDIO(audio)->input_pads);
    channel = ags_channel_nth(AGS_AUDIO(audio)->input,
			      (AGS_AUDIO(audio)->input_pads - list_length) * AGS_AUDIO(audio)->audio_channels);
    
    while(filenames != NULL){
      audio_file = ags_audio_file_new((gchar *) filenames->data,
				      (AgsDevout *) audio->devout,
				      0, audio->audio_channels);
      if(!ags_audio_file_open(audio_file)){
	filenames = filenames->next;
	continue;
      }
      
      ags_audio_file_read_audio_signal(audio_file);
      ags_audio_file_close(audio_file);
	
      audio_signal_list = audio_file->audio_signal;
      
      for(j = 0; j < audio->audio_channels && audio_signal_list != NULL; j++){
	AGS_AUDIO_SIGNAL(audio_signal_list->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	AGS_AUDIO_SIGNAL(audio_signal_list->data)->recycling = (GObject *) channel->first_recycling;
	audio_signal_source_old = ags_audio_signal_get_template(channel->first_recycling->audio_signal);
	
	channel->first_recycling->audio_signal = g_list_remove(channel->first_recycling->audio_signal,
							       (gpointer) audio_signal_source_old);
	channel->first_recycling->audio_signal = g_list_prepend(channel->first_recycling->audio_signal,
								audio_signal_list->data);
	
	g_object_unref(G_OBJECT(audio_signal_source_old));
	
	audio_signal_list = audio_signal_list->next;
	channel = channel->next;
      }
      
      if(audio->audio_channels > audio_file->channels)
	channel = ags_channel_nth(channel,
				  audio->audio_channels - audio_file->channels);
      
      filenames = filenames->next;
    }
  }
}

AgsAudio*
ags_audio_new(AgsDevout *devout)
{
  AgsAudio *audio;

  audio = (AgsAudio *) g_object_new(AGS_TYPE_AUDIO,
				    "devout\0", devout,
				    NULL);

  return(audio);
}
