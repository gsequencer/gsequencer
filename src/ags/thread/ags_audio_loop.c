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

#include <ags/thread/ags_audio_loop.h>

#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_main_loop.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall.h>

void ags_audio_loop_class_init(AgsAudioLoopClass *audio_loop);
void ags_audio_loop_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_loop_main_loop_interface_init(AgsMainLoopInterface *main_loop);
void ags_audio_loop_init(AgsAudioLoop *audio_loop);
void ags_audio_loop_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_audio_loop_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_audio_loop_connect(AgsConnectable *connectable);
void ags_audio_loop_disconnect(AgsConnectable *connectable);
void ags_audio_loop_set_tic(AgsMainLoop *main_loop, guint tic);
guint ags_audio_loop_get_tic(AgsMainLoop *main_loop);
void ags_audio_loop_set_last_sync(AgsMainLoop *main_loop, guint last_sync);
guint ags_audio_loop_get_last_sync(AgsMainLoop *main_loop);
void ags_audio_loop_finalize(GObject *gobject);

void ags_audio_loop_start(AgsThread *thread);
void ags_audio_loop_run(AgsThread *thread);

void ags_audio_loop_play_recall(AgsAudioLoop *audio_loop);
void ags_audio_loop_play_channel(AgsAudioLoop *audio_loop);
void ags_audio_loop_play_audio(AgsAudioLoop *audio_loop);

enum{
  PROP_0,
  PROP_PLAY_RECALL,
  PROP_PLAY_CHANNEL,
  PROP_PLAY_AUDIO,
};

static gpointer ags_audio_loop_parent_class = NULL;
static AgsConnectableInterface *ags_audio_loop_parent_connectable_interface;

GType
ags_audio_loop_get_type()
{
  static GType ags_type_audio_loop = 0;

  if(!ags_type_audio_loop){
    static const GTypeInfo ags_audio_loop_info = {
      sizeof (AgsAudioLoopClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_loop_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioLoop),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_loop_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_loop_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_main_loop_interface_info = {
      (GInterfaceInitFunc) ags_audio_loop_main_loop_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_loop = g_type_register_static(AGS_TYPE_THREAD,
						 "AgsAudioLoop\0",
						 &ags_audio_loop_info,
						 0);
    
    g_type_add_interface_static(ags_type_audio_loop,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_audio_loop,
				AGS_TYPE_MAIN_LOOP,
				&ags_main_loop_interface_info);
  }
  
  return (ags_type_audio_loop);
}

void
ags_audio_loop_class_init(AgsAudioLoopClass *audio_loop)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;
  GParamSpec *param_spec;

  ags_audio_loop_parent_class = g_type_class_peek_parent(audio_loop);

  /* GObject */
  gobject = (GObjectClass *) audio_loop;

  gobject->set_property = ags_audio_loop_set_property;
  gobject->get_property = ags_audio_loop_get_property;

  gobject->finalize = ags_audio_loop_finalize;

  /* properties */
  param_spec = g_param_spec_object("play_recall\0",
				   "recall to run\0",
				   "A recall to run\0",
				   AGS_TYPE_RECALL,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAY_RECALL,
				  param_spec);

  param_spec = g_param_spec_object("play_channel\0",
				   "channel to run\0",
				   "A channel to run\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAY_CHANNEL,
				  param_spec);

  param_spec = g_param_spec_object("play_audio\0",
				   "audio to run\0",
				   "A audio to run\0",
				   AGS_TYPE_AUDIO,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAY_AUDIO,
				  param_spec);

  /* AgsThread */
  thread = (AgsThreadClass *) audio_loop;

  thread->start = ags_audio_loop_start;
  thread->run = ags_audio_loop_run;

  /* AgsAudioLoop */
}

void
ags_audio_loop_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_audio_loop_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_audio_loop_connect;
  connectable->disconnect = ags_audio_loop_disconnect;
}

void
ags_audio_loop_main_loop_interface_init(AgsMainLoopInterface *main_loop)
{
  main_loop->set_tic = ags_audio_loop_set_tic;
  main_loop->get_tic = ags_audio_loop_get_tic;
  main_loop->set_last_sync = ags_audio_loop_set_last_sync;
  main_loop->get_last_sync = ags_audio_loop_get_last_sync;
}

void
ags_audio_loop_init(AgsAudioLoop *audio_loop)
{
  //  AGS_THREAD(audio_loop)->flags |= AGS_THREAD_WAIT_FOR_CHILDREN;

  audio_loop->flags = 0;

  audio_loop->tic = 0;
  audio_loop->last_sync = 0;

  audio_loop->main = NULL;
  
  audio_loop->task_thread = (AgsThread *) ags_task_thread_new(NULL);
  ags_thread_add_child(AGS_THREAD(audio_loop), audio_loop->task_thread);

  audio_loop->devout_thread = (AgsThread *) ags_devout_thread_new(NULL);
  ags_thread_add_child(AGS_THREAD(audio_loop), audio_loop->devout_thread);

  audio_loop->play_recall_ref = 0;
  audio_loop->play_recall = NULL;

  audio_loop->play_channel_ref = 0;
  audio_loop->play_channel = NULL;

  audio_loop->play_audio_ref = 0;
  audio_loop->play_audio = NULL;
}

void
ags_audio_loop_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsAudioLoop *audio_loop;

  audio_loop = AGS_AUDIO_LOOP(gobject);

  switch(prop_id){
  case PROP_PLAY_RECALL:
    {
      AgsRecall *recall;

      recall = (AgsRecall *) g_value_get_pointer(value);

      if(recall != NULL){
	audio_loop->play_recall = g_list_prepend(audio_loop->play_recall,
						 recall);
      }
    }
    break;
  case PROP_PLAY_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_pointer(value);

      if(channel != NULL){
	audio_loop->play_channel = g_list_prepend(audio_loop->play_channel,
						  channel);
      }
    }
    break;
  case PROP_PLAY_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_pointer(value);

      if(audio != NULL){
	audio_loop->play_audio = g_list_prepend(audio_loop->play_audio,
						audio);
      }
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_loop_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsAudioLoop *audio_loop;

  audio_loop = AGS_AUDIO_LOOP(gobject);

  switch(prop_id){
  case PROP_PLAY_RECALL:
    {
      g_value_set_pointer(value, audio_loop->play_recall);
    }
    break;
  case PROP_PLAY_CHANNEL:
    {
      g_value_set_pointer(value, audio_loop->play_channel);
    }
    break;
  case PROP_PLAY_AUDIO:
    {
      g_value_set_pointer(value, audio_loop->play_audio);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_loop_connect(AgsConnectable *connectable)
{
  ags_audio_loop_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_audio_loop_disconnect(AgsConnectable *connectable)
{
  ags_audio_loop_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_audio_loop_set_tic(AgsMainLoop *main_loop, guint tic)
{
  AGS_AUDIO_LOOP(main_loop)->tic = tic;
}

guint
ags_audio_loop_get_tic(AgsMainLoop *main_loop)
{
  return(AGS_AUDIO_LOOP(main_loop)->tic);
}

void
ags_audio_loop_set_last_sync(AgsMainLoop *main_loop, guint last_sync)
{
  g_atomic_int_set(&(AGS_AUDIO_LOOP(main_loop)->last_sync),
		   last_sync);
}

guint
ags_audio_loop_get_last_sync(AgsMainLoop *main_loop)
{
  gint val;

  val = g_atomic_int_get(&(AGS_AUDIO_LOOP(main_loop)->last_sync));

  return(val);
}

void
ags_audio_loop_finalize(GObject *gobject)
{
  AgsAudioLoop *audio_loop;

  audio_loop = AGS_AUDIO_LOOP(gobject);

  /* free AgsDevoutPlay lists */
  ags_list_free_and_free_link(audio_loop->play_recall);
  ags_list_free_and_free_link(audio_loop->play_channel);
  ags_list_free_and_free_link(audio_loop->play_audio);

  /* call parent */
  G_OBJECT_CLASS(ags_audio_loop_parent_class)->finalize(gobject);
}

void
ags_audio_loop_start(AgsThread *thread)
{
  AgsAudioLoop *audio_loop;

  audio_loop = AGS_AUDIO_LOOP(thread);

  /*  */
  AGS_THREAD_CLASS(ags_audio_loop_parent_class)->start(thread);

  /*  */
  ags_thread_start(audio_loop->task_thread);
}

void
ags_audio_loop_run(AgsThread *thread)
{
  AgsAudioLoop *audio_loop;
  AgsDevout *devout;

  audio_loop = AGS_AUDIO_LOOP(thread);

  devout = AGS_DEVOUT(AGS_THREAD(audio_loop)->devout);

  /* play channel */
  if((AGS_AUDIO_LOOP_PLAY_CHANNEL & (audio_loop->flags)) != 0){
    ags_audio_loop_play_channel(audio_loop);
      
    if(audio_loop->play_channel_ref == 0){
      audio_loop->flags &= (~AGS_AUDIO_LOOP_PLAY_CHANNEL);
      ags_thread_stop(AGS_THREAD(devout->devout_thread));
      g_message("audio_loop->play_channel_ref == 0\n\0");
    }
  }
    
  /* play audio */
  if((AGS_AUDIO_LOOP_PLAY_AUDIO & (audio_loop->flags)) != 0){
    ags_audio_loop_play_audio(audio_loop);
      
    if(audio_loop->play_audio_ref == 0){
      audio_loop->flags &= (~AGS_AUDIO_LOOP_PLAY_AUDIO);
      ags_thread_stop(AGS_THREAD(devout->devout_thread));
      g_message("audio_loop->play_audio_ref == 0\n\0");
    }
  }
    
  /* determine if attack should be switched */
  devout->delay_counter += 1;
      
  if(devout->delay_counter == devout->delay){
    if((AGS_DEVOUT_ATTACK_FIRST & (devout->flags)) != 0)
      devout->flags &= (~AGS_DEVOUT_ATTACK_FIRST);
    else
      devout->flags |= AGS_DEVOUT_ATTACK_FIRST;
      
    devout->delay_counter = 0;
  }

  pthread_mutex_lock(&(audio_loop->task_thread->start_mutex));

  if((AGS_THREAD_INITIAL_RUN & (AGS_THREAD(audio_loop->task_thread)->flags)) != 0){

    while((AGS_THREAD_INITIAL_RUN & (AGS_THREAD(audio_loop->task_thread)->flags)) != 0){
      pthread_cond_wait(&(audio_loop->task_thread->start_cond),
			&(audio_loop->task_thread->start_mutex));
    }
  }
  
  pthread_mutex_unlock(&(audio_loop->task_thread->start_mutex));
}

/**
 * ags_audio_loop_play_recall:
 * @audio_loop an #AgsAudioLoop
 *
 * Runs all recalls assigned with @audio_loop. You may want to use
 * #AgsAppendRecall task to add an #AgsRecall.
 */
void
ags_audio_loop_play_recall(AgsAudioLoop *audio_loop)
{
  AgsDevoutPlay *devout_play;
  AgsRecall *recall;
  AgsRecallID *recall_id;
  GList *list, *list_next;
  guint stage;

  audio_loop->flags |= AGS_AUDIO_LOOP_PLAYING_RECALL;
  stage = 0;

 ags_audio_loop_play_recall0:

  list = audio_loop->play_recall;

  if(list == NULL){
    if((AGS_AUDIO_LOOP_PLAY_RECALL_TERMINATING & (audio_loop->flags)) != 0){
      audio_loop->flags &= (~(AGS_AUDIO_LOOP_PLAY_RECALL |
			      AGS_AUDIO_LOOP_PLAY_RECALL_TERMINATING));
    }else{
      audio_loop->flags |= AGS_AUDIO_LOOP_PLAY_RECALL_TERMINATING;
    }
  }

  audio_loop->flags &= (~AGS_AUDIO_LOOP_PLAY_RECALL_TERMINATING);

  while(list != NULL){
    devout_play = (AgsDevoutPlay *) list->data;
    recall = AGS_RECALL(devout_play->source);
    recall_id = devout_play->recall_id;
    list_next = list->next;


    if((AGS_RECALL_HIDE & (recall->flags)) == 0){
      if(stage == 0){
	ags_recall_run_pre(recall);
      }else if(stage == 1){
	ags_recall_run_inter(recall);
      }else{
	ags_recall_run_post(recall);
      }
    }

    ags_recall_child_check_remove(recall);

    if((AGS_RECALL_REMOVE & (recall->flags)) != 0){
      //TODO:JK: check if these mutices can be removed
      audio_loop->play_recall_ref = audio_loop->play_recall_ref - 1;
      audio_loop->play_recall = g_list_remove(audio_loop->play_recall, (gpointer) recall);

      ags_recall_remove(recall);
    }

    list = list_next;
  }

  if(stage == 0){
    stage = 1;
    goto ags_audio_loop_play_recall0;
  }else if(stage == 1){
    stage = 2;
    goto ags_audio_loop_play_recall0;
  }

  audio_loop->flags &= (~AGS_AUDIO_LOOP_PLAYING_RECALL);
}

/**
 * ags_audio_loop_play_channel:
 * @audio_loop an #AgsAudioLoop
 *
 * Runs all recalls descending recursively and ascending till next 
 * #AgsRecycling around prior added #AgsChannel with #AgsAppendChannel
 * task.
 */
void
ags_audio_loop_play_channel(AgsAudioLoop *audio_loop)
{
  AgsDevoutPlay *play;
  AgsChannel *channel;
  GList *list_play, *list_next_play;
  gint stage;
  AgsGroupId group_id;

  if(audio_loop->play_channel == NULL){
    if((AGS_AUDIO_LOOP_PLAY_CHANNEL_TERMINATING & (audio_loop->flags)) != 0){
      audio_loop->flags &= (~(AGS_AUDIO_LOOP_PLAY_CHANNEL |
			      AGS_AUDIO_LOOP_PLAY_CHANNEL_TERMINATING));
    }else{
      audio_loop->flags |= AGS_AUDIO_LOOP_PLAY_CHANNEL_TERMINATING;
    }
  }

  audio_loop->flags &= (~AGS_AUDIO_LOOP_PLAY_CHANNEL_TERMINATING);

  /* entry point */
  audio_loop->flags |= AGS_AUDIO_LOOP_PLAYING_CHANNEL;

  /* run the 3 stages */
  for(stage = 0; stage < 3; stage++){
    list_play = audio_loop->play_channel;

    while(list_play != NULL){
      list_next_play = list_play->next;

      play = (AgsDevoutPlay *) list_play->data;
      channel = AGS_CHANNEL(play->source);
      group_id = play->group_id;

      if((AGS_DEVOUT_PLAY_SUPER_THREADED & (play->flags)) == 0){
	ags_channel_recursive_play(channel, group_id, stage);
      }else{
	play->iterator_thread->flags |= AGS_ITERATOR_THREAD_DONE;
	pthread_cond_signal(&(play->iterator_thread->tic_cond));
      }

      if((AGS_DEVOUT_PLAY_REMOVE & (play->flags)) != 0){
	audio_loop->play_channel_ref = audio_loop->play_channel_ref - 1;
	audio_loop->play_channel = g_list_remove(audio_loop->play_channel, (gpointer) play);
      }

      list_play = list_next_play;
    }
  }
}

/**
 * ags_audio_loop_play_audio:
 * @audio_loop an #AgsAudioLoop
 *
 * Like ags_audio_loop_play_channel() except that it runs all channels within
 * #AgsAudio.
 */
void
ags_audio_loop_play_audio(AgsAudioLoop *audio_loop)
{
  AgsDevoutPlay *play;
  AgsAudio *audio;
  AgsChannel *output;
  GList *list_play, *list_next_play;
  gint stage;
  AgsGroupId group_id;

  if(audio_loop->play_audio == NULL){
    if((AGS_AUDIO_LOOP_PLAY_AUDIO_TERMINATING & (audio_loop->flags)) != 0){
      audio_loop->flags &= (~(AGS_AUDIO_LOOP_PLAY_AUDIO |
			      AGS_AUDIO_LOOP_PLAY_AUDIO_TERMINATING));
    }else{
      audio_loop->flags |= AGS_AUDIO_LOOP_PLAY_AUDIO_TERMINATING;
    }
  }

  audio_loop->flags &= (~AGS_AUDIO_LOOP_PLAY_AUDIO_TERMINATING);

  /* entry point */
  audio_loop->flags |= AGS_AUDIO_LOOP_PLAYING_AUDIO;

  /* run the 3 stages */
  for(stage = 0; stage < 3; stage++){
    list_play = audio_loop->play_audio;

    while(list_play != NULL){
      list_next_play = list_play->next;

      play = (AgsDevoutPlay *) list_play->data;
      audio = AGS_AUDIO(play->source);
      group_id = play->group_id;

      output = audio->output;

      while(output != NULL){
	if((AGS_DEVOUT_PLAY_SUPER_THREADED & (play->flags)) == 0){
	  ags_channel_recursive_play(output, group_id, stage);
	}else{
	  play->iterator_thread->flags |= AGS_ITERATOR_THREAD_DONE;
	  pthread_cond_signal(&(play->iterator_thread->tic_cond));
	}

	output = output->next;
      }

      if((AGS_DEVOUT_PLAY_REMOVE & (play->flags)) != 0){
	audio_loop->play_audio_ref = audio_loop->play_audio_ref - 1;
	audio_loop->play_audio = g_list_remove(audio_loop->play_audio, (gpointer) play);
      }

      list_play = list_next_play;
    }
  }
}

void
ags_audio_loop_add_audio(AgsAudioLoop *audio_loop, GObject *audio)
{
  //TODO:JK: implement me
}

void
ags_audio_loop_remove_audio(AgsAudioLoop *audio_loop, GObject *audio)
{
  //TODO:JK: implement me
}

void
ags_audio_loop_add_channel(AgsAudioLoop *audio_loop, GObject *channel)
{
  //TODO:JK: implement me
}

void
ags_audio_loop_remove_channel(AgsAudioLoop *audio_loop, GObject *channel)
{
  //TODO:JK: implement me
}

void
ags_audio_loop_add_recall(AgsAudioLoop *audio_loop, GObject *recall)
{
  //TODO:JK: implement me
}

void
ags_audio_loop_remove_recall(AgsAudioLoop *audio_loop, GObject *recall)
{
  //TODO:JK: implement me
}

AgsAudioLoop*
ags_audio_loop_new(GObject *devout)
{
  AgsAudioLoop *audio_loop;

  audio_loop = (AgsAudioLoop *) g_object_new(AGS_TYPE_AUDIO_LOOP,
					     "devout\0", devout,
					     NULL);

  audio_loop->main = AGS_DEVOUT(devout)->main;

  return(audio_loop);
}
