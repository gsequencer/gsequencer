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

#include <ags/thread/ags_recycling_thread.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_tree_iterator.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall_id.h>

#include <math.h>

void ags_recycling_thread_class_init(AgsRecyclingThreadClass *recycling_thread);
void ags_recycling_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recycling_thread_tree_iterator_interface_init(AgsTreeIteratorInterface *tree);
void ags_recycling_thread_init(AgsRecyclingThread *recycling_thread);
void ags_recycling_thread_connect(AgsConnectable *connectable);
void ags_recycling_thread_disconnect(AgsConnectable *connectable);
void ags_recycling_thread_set_inverse_mode(AgsTreeIterator *tree, gboolean mode);
gboolean ags_recycling_thread_is_inverse_mode(AgsTreeIterator *tree);
void ags_recycling_thread_iterate(AgsTreeIterator *tree,
				  gpointer node_id);
void ags_recycling_thread_iterate_nested(AgsTreeIterator *tree,
					 gpointer node_id);
void ags_recycling_thread_safe_iterate(AgsTreeIterator *toplevel, AgsTreeIterator *current,
				       gpointer node_id);
void ags_recycling_thread_safe_iterate_nested(AgsTreeIterator *toplevel, AgsTreeIterator *current,
					      gpointer node_id);
void ags_recycling_thread_finalize(GObject *gobject);

void ags_recycling_thread_start(AgsThread *thread);

void ags_recycling_thread_iterate_inverse(AgsRecyclingThread *recycling_thread);

static gpointer ags_recycling_thread_parent_class = NULL;
static AgsConnectableInterface *ags_recycling_thread_parent_connectable_interface;

GType
ags_recycling_thread_get_type()
{
  static GType ags_type_recycling_thread = 0;

  if(!ags_type_recycling_thread){
    static const GTypeInfo ags_recycling_thread_info = {
      sizeof (AgsRecyclingThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recycling_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecyclingThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recycling_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recycling_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo ags_tree_iterator_interface_info = {
      (GInterfaceInitFunc) ags_recycling_thread_tree_iterator_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_recycling_thread = g_type_register_static(AGS_TYPE_THREAD,
						       "AgsRecyclingThread\0",
						       &ags_recycling_thread_info,
						    0);
    
    g_type_add_interface_static(ags_type_recycling_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recycling_thread,
				AGS_TYPE_TREE_ITERATOR,
				&ags_tree_iterator_interface_info);
  }
  
  return (ags_type_recycling_thread);
}

void
ags_recycling_thread_class_init(AgsRecyclingThreadClass *recycling_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;

  ags_recycling_thread_parent_class = g_type_class_peek_parent(recycling_thread);

  /* GObject */
  gobject = (GObjectClass *) recycling_thread;

  gobject->finalize = ags_recycling_thread_finalize;

  /* AgsThread */
  thread = (AgsThreadClass *) recycling_thread;

  thread->start = ags_recycling_thread_start;

  /* AgsRecyclingThread */
  recycling_thread->run_init_pre = ags_recycling_thread_run_init_pre;
  recycling_thread->run_init_inter = ags_recycling_thread_run_init_inter;
  recycling_thread->run_init_post = ags_recycling_thread_run_init_post;

  recycling_thread->run_pre = ags_recycling_thread_run_pre;
  recycling_thread->run_inter = ags_recycling_thread_run_inter;
  recycling_thread->run_post = ags_recycling_thread_run_post;
}

void
ags_recycling_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recycling_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_recycling_thread_connect;
  connectable->disconnect = ags_recycling_thread_disconnect;
}

void
ags_recycling_thread_tree_iterator_interface_init(AgsTreeIteratorInterface *tree)
{
  tree->set_inverse_mode = ags_recycling_thread_set_inverse_mode;
  tree->is_inverse_mode = ags_recycling_thread_is_inverse_mode;

  tree->iterate = ags_recycling_thread_iterate;
  tree->iterate_nested = ags_recycling_thread_iterate_nested;

  tree->safe_iterate = ags_recycling_thread_safe_iterate;
  tree->safe_iterate_nested = ags_recycling_thread_safe_iterate_nested;
}

void
ags_recycling_thread_init(AgsRecyclingThread *recycling_thread)
{
  recycling_thread->recycling = NULL;
  pthread_mutex_init(&(recycling_thread->iteration_mutex), NULL);
  pthread_cond_init(&(recycling_thread->iteration_cond), NULL);

  recycling_thread->stage = 0;
}

void
ags_recycling_thread_connect(AgsConnectable *connectable)
{
  AgsThread *thread;

  thread = AGS_THREAD(connectable);

  ags_recycling_thread_parent_connectable_interface->connect(connectable);


}

void
ags_recycling_thread_disconnect(AgsConnectable *connectable)
{
  ags_recycling_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_recycling_thread_set_inverse_mode(AgsTreeIterator *tree, gboolean mode)
{
  AgsRecyclingThread *recycling_thread;

  recycling_thread = AGS_RECYCLING_THREAD(tree);

  recycling_thread->tree_mode = !mode;
}

gboolean
ags_recycling_thread_is_inverse_mode(AgsTreeIterator *tree)
{
  AgsRecyclingThread *recycling_thread;

  recycling_thread = AGS_RECYCLING_THREAD(tree);

  return(!recycling_thread->tree_mode);
}

void
ags_recycling_thread_iterate(AgsTreeIterator *tree,
			     gpointer node_id)
{
  AgsRecyclingThread *recycling_thread;

  recycling_thread = AGS_RECYCLING_THREAD(tree);

  /* iterate sibling */
  //TODO:JK: implement me
}

void
ags_recycling_thread_iterate_nested(AgsTreeIterator *tree,
				    gpointer node_id)
{
  AgsRecyclingThread *recycling_thread;

  recycling_thread = AGS_RECYCLING_THREAD(tree);

  //TODO:JK: implement me
}

void
ags_recycling_thread_safe_iterate(AgsTreeIterator *toplevel, AgsTreeIterator *current,
				  gpointer node_id)
{
  AgsRecyclingThread *recycling_thread;

  recycling_thread = AGS_RECYCLING_THREAD(current);

  //TODO:JK: implement me
}

void
ags_recycling_thread_safe_iterate_nested(AgsTreeIterator *toplevel, AgsTreeIterator *current,
					 gpointer node_id)
{
  AgsRecyclingThread *recycling_thread;

  recycling_thread = AGS_RECYCLING_THREAD(current);

  //TODO:JK: implement me
}

void
ags_recycling_thread_finalize(GObject *gobject)
{
  AgsRecyclingThread *recycling_thread;

  recycling_thread = AGS_RECYCLING_THREAD(gobject);

  /*  */
  G_OBJECT_CLASS(ags_recycling_thread_parent_class)->finalize(gobject);
}

void
ags_recycling_thread_start(AgsThread *thread)
{
  AGS_THREAD_CLASS(ags_recycling_thread_parent_class)->start(thread);
}

void
ags_recycling_thread_start_iterate(AgsRecyclingThread *recycling_thread,
				   GObject *recall_id)
{
  pthread_mutex_lock(&(recycling_thread->iteration_mutex));

  while((AGS_RECYCLING_THREAD_WAIT & (recycling_thread->flags)) != 0 &&
	(AGS_RECYCLING_THREAD_DONE & (recycling_thread->flags)) == 0){
    pthread_cond_wait(&(recycling_thread->iteration_cond),
		      &(recycling_thread->iteration_mutex));
  }
  
  recycling_thread->flags |= AGS_RECYCLING_THREAD_WAIT;
  recycling_thread->flags &= (~AGS_RECYCLING_THREAD_DONE);

  switch(recycling_thread->stage)
    {
    case 0:
      {
	if((AGS_THREAD_INITIAL_RUN & (AGS_THREAD(recycling_thread)->flags)) != 0){
	  ags_recycling_thread_run_init_pre(recycling_thread,
					    recall_id);
	}else{
	  ags_recycling_thread_run_pre(recycling_thread,
				       recall_id);
	}
	break;
      }
    case 1:
      {
	if((AGS_THREAD_INITIAL_RUN & (AGS_THREAD(recycling_thread)->flags)) != 0){
	  ags_recycling_thread_run_init_inter(recycling_thread,
					      recall_id);
	}else{
	  ags_recycling_thread_run_inter(recycling_thread,
					 recall_id);
	}
	break;
      }
    case 2:
      {
	if((AGS_THREAD_INITIAL_RUN & (AGS_THREAD(recycling_thread)->flags)) != 0){
	  ags_recycling_thread_run_init_post(recycling_thread,
					     recall_id);
	}else{
	  ags_recycling_thread_run_post(recycling_thread,
					recall_id);
	}
	break;
      }
    }

  pthread_mutex_unlock(&(recycling_thread->iteration_mutex));
}

void
ags_recycling_thread_run_init_pre(AgsRecyclingThread *recycling_thread,
				  GObject *recall_id)
{
  AgsAudio *audio, *current_audio;
  AgsChannel *channel, *current_channel;
  AgsRecycling *recycling, *current_recycling;
  GList *list;

  auto void ags_recycling_thread_run_init_pre_loop(GList *recall);

  void ags_recycling_thread_run_init_pre_loop(GList *recall){
    while(list != NULL){
      ags_recall_run_init_pre(AGS_RECALL(list->data));

      list = list->next;
    }
  }

  recycling = AGS_RECYCLING(recycling_thread->recycling);

  channel =
    current_channel = AGS_CHANNEL(recycling->channel);

  while(current_channel != AGS_CHANNEL(recycling->parent->channel)){
    /* AgsInput */
    if(recycling->parent == NULL){
      list = current_channel->play;
    }else{
      list = current_channel->recall;
    }


    /* AgsAudio */
    current_audio = AGS_AUDIO(current_channel->audio);

    if(recycling->parent == NULL){
      list = current_channel->play;

      ags_recycling_thread_run_init_pre_loop(list);
    }else{
      if(recycling->parent->parent == NULL){
	//FIXME:JK: runs to deep into tree
	list = current_channel->play;
	ags_recycling_thread_run_init_pre_loop(list);	
      }

      list = current_channel->recall;
      ags_recycling_thread_run_init_pre_loop(list);
    }


    /* AgsOutput */
    if((AGS_AUDIO_ASYNC & (current_audio->flags)) != 0){
      current_channel = ags_channel_nth(current_audio->output,
					current_channel->audio_channel);
    }else{
      current_channel = ags_channel_nth(current_audio->output,
					current_channel->line);
    }

    if(recycling->parent == NULL){
      list = current_channel->play;
    }else{
      list = current_channel->recall;
    }

    ags_recycling_thread_run_init_pre_loop(list);

    /* iterate */
    current_channel = current_channel->link;
  }
}

void
ags_recycling_thread_run_init_inter(AgsRecyclingThread *recycling_thread,
				    GObject *recall_id)
{
  AgsAudio *audio, *current_audio;
  AgsChannel *channel, *current_channel;
  AgsRecycling *recycling, *current_recycling;
  GList *list;

  auto void ags_recycling_thread_run_init_inter_loop(GList *recall);

  void ags_recycling_thread_run_init_inter_loop(GList *recall){
    while(list != NULL){
      ags_recall_run_init_inter(AGS_RECALL(list->data));

      list = list->next;
    }
  }

  recycling = AGS_RECYCLING(recycling_thread->recycling);

  channel =
    current_channel = AGS_CHANNEL(recycling->channel);

  while(current_channel != AGS_CHANNEL(recycling->parent->channel)){
    /* AgsInput */
    if(recycling->parent == NULL){
      list = current_channel->play;
    }else{
      list = current_channel->recall;
    }

    ags_recycling_thread_run_init_inter_loop(list);

    /* AgsAudio */
    current_audio = AGS_AUDIO(current_channel->audio);

    if(recycling->parent == NULL){
      list = current_channel->play;

      ags_recycling_thread_run_init_inter_loop(list);
    }else{
      if(recycling->parent->parent == NULL){
	//FIXME:JK: runs to deep into tree
	list = current_channel->play;
	ags_recycling_thread_run_init_inter_loop(list);	
      }

      list = current_channel->recall;
      ags_recycling_thread_run_init_inter_loop(list);
    }


    /* AgsOutput */
    if((AGS_AUDIO_ASYNC & (current_audio->flags)) != 0){
      current_channel = ags_channel_nth(current_audio->output,
					current_channel->audio_channel);
    }else{
      current_channel = ags_channel_nth(current_audio->output,
					current_channel->line);
    }

    if(recycling->parent == NULL){
      list = current_channel->play;
    }else{
      list = current_channel->recall;
    }

    ags_recycling_thread_run_init_inter_loop(list);

    /* iterate */
    current_channel = current_channel->link;
  }
}

void
ags_recycling_thread_run_init_post(AgsRecyclingThread *recycling_thread,
				   GObject *recall_id)
{
  AgsAudio *audio, *current_audio;
  AgsChannel *channel, *current_channel;
  AgsRecycling *recycling, *current_recycling;
  GList *list;

  auto void ags_recycling_thread_run_init_post_loop(GList *recall);

  void ags_recycling_thread_run_init_post_loop(GList *recall){
    while(list != NULL){
      ags_recall_run_init_post(AGS_RECALL(list->data));

      list = list->next;
    }
  }

  recycling = AGS_RECYCLING(recycling_thread->recycling);

  channel =
    current_channel = AGS_CHANNEL(recycling->channel);

  while(current_channel != AGS_CHANNEL(recycling->parent->channel)){
    /* AgsInput */
    if(recycling->parent == NULL){
      list = current_channel->play;
    }else{
      list = current_channel->recall;
    }

    ags_recycling_thread_run_init_post_loop(list);

    /* AgsAudio */
    current_audio = AGS_AUDIO(current_channel->audio);

    if(recycling->parent == NULL){
      list = current_channel->play;

      ags_recycling_thread_run_init_post_loop(list);
    }else{
      if(recycling->parent->parent == NULL){
	//FIXME:JK: runs to deep into tree
	list = current_channel->play;
	ags_recycling_thread_run_init_post_loop(list);	
      }

      list = current_channel->recall;
      ags_recycling_thread_run_init_post_loop(list);
    }


    /* AgsOutput */
    if((AGS_AUDIO_ASYNC & (current_audio->flags)) != 0){
      current_channel = ags_channel_nth(current_audio->output,
					current_channel->audio_channel);
    }else{
      current_channel = ags_channel_nth(current_audio->output,
					current_channel->line);
    }

    if(recycling->parent == NULL){
      list = current_channel->play;
    }else{
      list = current_channel->recall;
    }

    ags_recycling_thread_run_init_post_loop(list);

    /* iterate */
    current_channel = current_channel->link;
  }
}

void
ags_recycling_thread_run_pre(AgsRecyclingThread *recycling_thread,
			     GObject *recall_id)
{
  AgsAudio *audio, *current_audio;
  AgsChannel *channel, *current_channel;
  AgsRecycling *recycling, *current_recycling;
  GList *list;

  auto void ags_recycling_thread_run_pre_loop(GList *recall);

  void ags_recycling_thread_run_pre_loop(GList *recall){
    while(list != NULL){
      ags_recall_run_pre(AGS_RECALL(list->data));

      list = list->next;
    }
  }

  recycling = AGS_RECYCLING(recycling_thread->recycling);

  channel =
    current_channel = AGS_CHANNEL(recycling->channel);

  while(current_channel != AGS_CHANNEL(recycling->parent->channel)){
    /* AgsInput */
    if(recycling->parent == NULL){
      list = current_channel->play;
    }else{
      list = current_channel->recall;
    }

    ags_recycling_thread_run_pre_loop(list);

    /* AgsAudio */
    current_audio = AGS_AUDIO(current_channel->audio);

    if(recycling->parent == NULL){
      list = current_channel->play;

      ags_recycling_thread_run_pre_loop(list);
    }else{
      if(recycling->parent->parent == NULL){
	//FIXME:JK: runs to deep into tree
	list = current_channel->play;
	ags_recycling_thread_run_pre_loop(list);	
      }

      list = current_channel->recall;
      ags_recycling_thread_run_pre_loop(list);
    }


    /* AgsOutput */
    if((AGS_AUDIO_ASYNC & (current_audio->flags)) != 0){
      current_channel = ags_channel_nth(current_audio->output,
					current_channel->audio_channel);
    }else{
      current_channel = ags_channel_nth(current_audio->output,
					current_channel->line);
    }

    if(recycling->parent == NULL){
      list = current_channel->play;
    }else{
      list = current_channel->recall;
    }

    ags_recycling_thread_run_pre_loop(list);

    /* iterate */
    current_channel = current_channel->link;
  }
}

void
ags_recycling_thread_run_inter(AgsRecyclingThread *recycling_thread,
			       GObject *recall_id)
{
  AgsAudio *audio, *current_audio;
  AgsChannel *channel, *current_channel;
  AgsRecycling *recycling, *current_recycling;
  GList *list;

  auto void ags_recycling_thread_run_inter_loop(GList *recall);

  void ags_recycling_thread_run_inter_loop(GList *recall){
    while(list != NULL){
      ags_recall_run_inter(AGS_RECALL(list->data));

      list = list->next;
    }
  }

  recycling = AGS_RECYCLING(recycling_thread->recycling);

  channel =
    current_channel = AGS_CHANNEL(recycling->channel);

  while(current_channel != AGS_CHANNEL(recycling->parent->channel)){
    /* AgsInput */
    if(recycling->parent == NULL){
      list = current_channel->play;
    }else{
      list = current_channel->recall;
    }

    ags_recycling_thread_run_inter_loop(list);

    /* AgsAudio */
    current_audio = AGS_AUDIO(current_channel->audio);

    if(recycling->parent == NULL){
      list = current_channel->play;

      ags_recycling_thread_run_inter_loop(list);
    }else{
      if(recycling->parent->parent == NULL){
	//FIXME:JK: runs to deep into tree
	list = current_channel->play;
	ags_recycling_thread_run_inter_loop(list);	
      }

      list = current_channel->recall;
      ags_recycling_thread_run_inter_loop(list);
    }


    /* AgsOutput */
    if((AGS_AUDIO_ASYNC & (current_audio->flags)) != 0){
      current_channel = ags_channel_nth(current_audio->output,
					current_channel->audio_channel);
    }else{
      current_channel = ags_channel_nth(current_audio->output,
					current_channel->line);
    }

    if(recycling->parent == NULL){
      list = current_channel->play;
    }else{
      list = current_channel->recall;
    }

    ags_recycling_thread_run_inter_loop(list);

    /* iterate */
    current_channel = current_channel->link;
  }
}

void
ags_recycling_thread_run_post(AgsRecyclingThread *recycling_thread,
			      GObject *recall_id)
{
  AgsAudio *audio, *current_audio;
  AgsChannel *channel, *current_channel;
  AgsRecycling *recycling, *current_recycling;
  GList *list;

  auto void ags_recycling_thread_run_post_loop(GList *recall);

  void ags_recycling_thread_run_post_loop(GList *recall){
    while(list != NULL){
      ags_recall_run_post(AGS_RECALL(list->data));

      list = list->next;
    }
  }

  recycling = AGS_RECYCLING(recycling_thread->recycling);

  channel =
    current_channel = AGS_CHANNEL(recycling->channel);

  while(current_channel != AGS_CHANNEL(recycling->parent->channel)){
    /* AgsInput */
    if(recycling->parent == NULL){
      list = current_channel->play;
    }else{
      list = current_channel->recall;
    }

    ags_recycling_thread_run_post_loop(list);	

    /* AgsAudio */
    current_audio = AGS_AUDIO(current_channel->audio);

    if(recycling->parent == NULL){
      list = current_channel->play;

      ags_recycling_thread_run_post_loop(list);
    }else{
      if(recycling->parent->parent == NULL){
	//FIXME:JK: runs to deep into tree
	list = current_channel->play;
	ags_recycling_thread_run_post_loop(list);	
      }

      list = current_channel->recall;
      ags_recycling_thread_run_post_loop(list);
    }


    /* AgsOutput */
    if((AGS_AUDIO_ASYNC & (current_audio->flags)) != 0){
      current_channel = ags_channel_nth(current_audio->output,
					current_channel->audio_channel);
    }else{
      current_channel = ags_channel_nth(current_audio->output,
					current_channel->line);
    }

    if(recycling->parent == NULL){
      list = current_channel->play;
    }else{
      list = current_channel->recall;
    }

    ags_recycling_thread_run_post_loop(list);

    /* iterate */
    current_channel = current_channel->link;
  }
}


AgsRecyclingThread*
ags_recycling_thread_new(GObject *recycling)
{
  AgsRecyclingThread *recycling_thread;
  
  recycling_thread = (AgsRecyclingThread *) g_object_new(AGS_TYPE_RECYCLING_THREAD,
							 NULL);

  recycling_thread->recycling = recycling;

  return(recycling_thread);
}
