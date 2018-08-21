/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/recall/ags_play_channel_run.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_playback.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_recycling.h>
#include <ags/audio/recall/ags_play_audio_signal.h>
#include <ags/audio/recall/ags_stream_channel.h>
#include <ags/audio/recall/ags_stream_channel_run.h>
#include <ags/audio/recall/ags_stream_recycling.h>
#include <ags/audio/recall/ags_stream_audio_signal.h>
#include <ags/audio/recall/ags_rt_stream_channel.h>
#include <ags/audio/recall/ags_rt_stream_channel_run.h>
#include <ags/audio/recall/ags_rt_stream_recycling.h>
#include <ags/audio/recall/ags_rt_stream_audio_signal.h>

#include <ags/audio/task/ags_cancel_channel.h>

#include <stdlib.h>
#include <stdio.h>

#include <ags/i18n.h>

void ags_play_channel_run_class_init(AgsPlayChannelRunClass *play_channel_run);
void ags_play_channel_run_init(AgsPlayChannelRun *play_channel_run);
void ags_play_channel_run_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_play_channel_run_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_play_channel_run_dispose(GObject *gobject);
void ags_play_channel_run_finalize(GObject *gobject);

void ags_play_channel_run_run_init_inter(AgsRecall *recall);
void ags_play_channel_run_run_post(AgsRecall *recall);
void ags_play_channel_run_resolve_dependency(AgsRecall *recall);

/**
 * SECTION:ags_play_channel_run
 * @short_description: plays channel
 * @title: AgsPlayChannel
 * @section_id:
 * @include: ags/audio/recall/ags_play_channel.h
 *
 * The #AgsPlayChannel class plays the channel.
 */

enum{
  PROP_0,
  PROP_STREAM_CHANNEL_RUN,
};

static gpointer ags_play_channel_run_parent_class = NULL;

static const gchar *ags_play_channel_run_plugin_name = "ags-play";

GType
ags_play_channel_run_get_type()
{
  static GType ags_type_play_channel_run = 0;

  if(!ags_type_play_channel_run){
    static const GTypeInfo ags_play_channel_run_info = {
      sizeof (AgsPlayChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_channel_run_init,
    };

    ags_type_play_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
						       "AgsPlayChannelRun",
						       &ags_play_channel_run_info,
						       0);
  }

  return(ags_type_play_channel_run);
}

void
ags_play_channel_run_class_init(AgsPlayChannelRunClass *play_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_play_channel_run_parent_class = g_type_class_peek_parent(play_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) play_channel_run;

  gobject->set_property = ags_play_channel_run_set_property;
  gobject->get_property = ags_play_channel_run_get_property;

  gobject->dispose = ags_play_channel_run_dispose;
  gobject->finalize = ags_play_channel_run_finalize;

  /* properties */
  /**
   * AgsPlayChannelRun:stream-channel-run:
   * 
   * The stream channel run dependency.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("stream-channel-run",
				   i18n_pspec("assigned AgsStreamChannelRun"),
				   i18n_pspec("the assigned AgsStreamChannelRun"),
				   AGS_TYPE_STREAM_CHANNEL_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_STREAM_CHANNEL_RUN,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) play_channel_run;

  recall->run_init_inter = ags_play_channel_run_run_init_inter;
  recall->run_post = ags_play_channel_run_run_post;
  recall->resolve_dependency = ags_play_channel_run_resolve_dependency;
}

void
ags_play_channel_run_init(AgsPlayChannelRun *play_channel_run)
{
  ags_recall_set_ability_flags(play_channel_run, (AGS_SOUND_ABILITY_PLAYBACK));

  AGS_RECALL(play_channel_run)->name = "ags-play";
  AGS_RECALL(play_channel_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_channel_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_channel_run)->xml_type = "ags-play-channel-run";
  AGS_RECALL(play_channel_run)->port = NULL;

  AGS_RECALL(play_channel_run)->child_type = AGS_TYPE_PLAY_RECYCLING;

  play_channel_run->flags = 0;

  play_channel_run->stream_channel_run = NULL;
}

void
ags_play_channel_run_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsPlayChannelRun *play_channel_run;

  pthread_mutex_t *recall_mutex;

  play_channel_run = AGS_PLAY_CHANNEL_RUN(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_STREAM_CHANNEL_RUN:
    {
      AgsStreamChannelRun *stream_channel_run, *old_stream_channel_run;

      gboolean is_template;

      stream_channel_run = (AgsStreamChannelRun *) g_value_get_object(value);
      old_stream_channel_run = NULL;
      
      pthread_mutex_lock(recall_mutex);

      if(stream_channel_run == play_channel_run->stream_channel_run){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(play_channel_run->stream_channel_run != NULL){
	old_stream_channel_run = play_channel_run->stream_channel_run;

	g_object_unref(G_OBJECT(play_channel_run->stream_channel_run));
      }

      if(stream_channel_run != NULL){
	g_object_ref(G_OBJECT(stream_channel_run));
      }

      play_channel_run->stream_channel_run = stream_channel_run;

      pthread_mutex_unlock(recall_mutex);

      /* dependency */
      if(ags_recall_test_flags(play_channel_run, AGS_RECALL_TEMPLATE)){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(is_template &&
	 old_stream_channel_run != NULL){
	ags_recall_remove_recall_dependency(AGS_RECALL(play_channel_run),
					    (AgsRecall *) old_stream_channel_run);
      }

      if(is_template &&
	 stream_channel_run != NULL){
	ags_recall_add_recall_dependency(AGS_RECALL(play_channel_run),
					 ags_recall_dependency_new((GObject *) stream_channel_run));
      }
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_channel_run_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsPlayChannelRun *play_channel_run;

  pthread_mutex_t *recall_mutex;

  play_channel_run = AGS_PLAY_CHANNEL_RUN(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_STREAM_CHANNEL_RUN:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, G_OBJECT(play_channel_run->stream_channel_run));

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_channel_run_dispose(GObject *gobject)
{
  AgsPlayChannelRun *play_channel_run;

  play_channel_run = AGS_PLAY_CHANNEL_RUN(gobject);

  /* stream channel run */
  if(play_channel_run->stream_channel_run != NULL){
    g_object_unref(G_OBJECT(play_channel_run->stream_channel_run));

    play_channel_run->stream_channel_run = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_channel_run_parent_class)->dispose(gobject);
}

void
ags_play_channel_run_finalize(GObject *gobject)
{
  AgsPlayChannelRun *play_channel_run;

  play_channel_run = AGS_PLAY_CHANNEL_RUN(gobject);

  /* stream channel run */
  if(play_channel_run->stream_channel_run != NULL){
    g_object_unref(G_OBJECT(play_channel_run->stream_channel_run));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_channel_run_parent_class)->finalize(gobject);
}

void
ags_play_channel_run_run_init_inter(AgsRecall *recall)
{
  AgsPlayChannelRun *play_channel_run;
  
  void (*parent_class_run_init_inter)(AgsRecall *recall);

  play_channel_run = (AgsPlayChannelRun *) recall;
  
  /* get parent class and mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  parent_class_run_init_inter = AGS_RECALL_CLASS(ags_play_channel_run_parent_class)->run_init_inter;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* set flags */
  ags_play_channel_run_set_flags(play_channel_run,
				 AGS_PLAY_CHANNEL_RUN_INITIAL_RUN);

  /* call parent */
  parent_class_run_init_inter(recall);
}

void
ags_play_channel_run_run_post(AgsRecall *recall)
{
  AgsChannel *source;
  AgsRecallID *recall_id;
  AgsRecyclingContext *recycling_context;
  AgsPlayChannelRun *play_channel_run;
  AgsStreamChannelRun *stream_channel_run;
  AgsRtStreamChannelRun *rt_stream_channel_run;
  
  GList *list_start, *list;
  GList *recall_recycling_list, *start_recall_recycling_list;
  GList *recall_audio_signal_list, *start_recall_audio_signal_list;

  gboolean found;
  
  void (*parent_class_run_post)(AgsRecall *recall);

  pthread_mutex_t *recall_mutex;

  play_channel_run = (AgsPlayChannelRun *) recall;
  
  /* get parent class and mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  parent_class_run_post = AGS_RECALL_CLASS(ags_play_channel_run_parent_class)->run_post;

  recall_mutex = recall->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* call parent */
  parent_class_run_post(recall);

  /* get some fields */
  g_object_get(recall,
	       "source", &source,
	       "recall-id", &recall_id,
	       NULL);

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);
  
  found = FALSE;

  if(ags_recall_global_get_rt_safe()){
    /* connect done */
    g_object_get(source,
		 "play", &list_start,
		 NULL);
    
    list = list_start;

    while((list = ags_recall_find_type_with_recycling_context(list,
							      AGS_TYPE_RT_STREAM_CHANNEL_RUN,
							      (GObject *) recycling_context)) != NULL &&
	  !found){
      rt_stream_channel_run = AGS_RT_STREAM_CHANNEL_RUN(list->data);

      if(ags_recall_test_flags(rt_stream_channel_run, AGS_RECALL_TEMPLATE)){
	list = list->next;

	continue;
      }

      g_object_get(rt_stream_channel_run,
		   "child", &start_recall_recycling_list,
		   NULL);

      recall_recycling_list = start_recall_recycling_list;

      while(recall_recycling_list != NULL &&
	    !found){
	recall_audio_signal_list = AGS_RECALL(recall_recycling_list->data)->children;
      
	while(recall_audio_signal_list != NULL){
	  if(!ags_recall_test_staging_flags(recall_audio_signal_list->data, AGS_SOUND_STAGING_DONE) &&
	     !ags_recall_test_flags(recall_audio_signal_list->data, AGS_RECALL_TEMPLATE)){
	    found = TRUE;
	  
	    break;
	  }
	  
	  /* iterate */
	  recall_audio_signal_list = recall_audio_signal_list->next;
	}

	//	g_list_free(start_recall_audio_signal_list);

	/* iterate */
	recall_recycling_list = recall_recycling_list->next;
      }

      //      g_list_free(start_recall_recycling_list);

      /* iterate */
      list = list->next;
    }

    g_list_free(list_start);
  }else{
    /* connect done */
    g_object_get(source,
		 "play", &list_start,
		 NULL);
    
    list = list_start;

    while((list = ags_recall_find_type_with_recycling_context(list,
							      AGS_TYPE_STREAM_CHANNEL_RUN,
							      (GObject *) recycling_context)) != NULL &&
	  !found){
      stream_channel_run = AGS_STREAM_CHANNEL_RUN(list->data);

      if(ags_recall_test_flags(stream_channel_run, AGS_RECALL_TEMPLATE)){
	list = list->next;

	continue;
      }
    
      recall_recycling_list = AGS_RECALL(stream_channel_run)->children;

      while(recall_recycling_list != NULL &&
	    !found){
	recall_audio_signal_list = AGS_RECALL(recall_recycling_list->data)->children;
      
	while(recall_audio_signal_list != NULL){
	  if(!ags_recall_test_staging_flags(recall_audio_signal_list->data, AGS_SOUND_STAGING_DONE) &&
	     !ags_recall_test_flags(recall_audio_signal_list->data, AGS_RECALL_TEMPLATE)){
	    found = TRUE;
	    
	    break;
	  }
	  
	  /* iterate */
	  recall_audio_signal_list = recall_audio_signal_list->next;
	}

	//	g_list_free(start_recall_audio_signal_list);

	/* iterate */
	recall_recycling_list = recall_recycling_list->next;
      }

      //      g_list_free(start_recall_recycling_list);

      /* iterate */
      list = list->next;
    }

    g_list_free(list_start);
  }
  
  if(!found){
    ags_recall_done(recall);
  }
}

void
ags_play_channel_run_resolve_dependency(AgsRecall *recall)
{
  AgsRecall *template;
  AgsRecallContainer *recall_container;
  AgsRecallID *recall_id;
  AgsRecallDependency *recall_dependency;
  
  AgsStreamChannelRun *stream_channel_run;

  GList *list_start, *list;

  guint i, i_stop;

  /* get template */
  g_object_get(recall,
	       "recall-id", &recall_id,
	       "recall-container", &recall_container,
	       NULL);

  g_object_get(recall_container,
	       "recall-channel-run", &list_start,
	       NULL);

  template = NULL;
  list = ags_recall_find_template(list_start);

  if(list != NULL){
    template = AGS_RECALL(list->data);
  }

  /* check recall dependency */
  g_object_get(template,
	       "recall-dependency", &list_start,
	       NULL);

  stream_channel_run = NULL;
  list = list_start;

  i_stop = 1;

  for(i = 0; i < i_stop && list != NULL;){
    GObject *dependency;
    
    recall_dependency = AGS_RECALL_DEPENDENCY(list->data);

    g_object_get(recall_dependency,
		 "dependency", &dependency,
		 NULL);

    if(AGS_IS_STREAM_CHANNEL_RUN(dependency)){
      stream_channel_run = (AgsStreamChannelRun *) ags_recall_dependency_resolve(recall_dependency,
										 recall_id);

      i++;
    }

    list = list->next;
  }

  g_list_free(list_start);

  g_object_set(G_OBJECT(recall),
	       "stream_channel_run", stream_channel_run,
	       NULL);
}

/**
 * ags_play_channel_run_test_flags:
 * @play_channel_run: the #AgsPlayChannelRun
 * @flags: the flags
 *
 * Test @flags to be set on @play_channel_run.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_play_channel_run_test_flags(AgsPlayChannelRun *play_channel_run, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *recall_mutex;

  if(!AGS_IS_PLAY_CHANNEL_RUN(play_channel_run)){
    return(FALSE);
  }

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(play_channel_run)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* test */
  pthread_mutex_lock(recall_mutex);

  retval = (flags & (play_channel_run->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(recall_mutex);

  return(retval);
}

/**
 * ags_play_channel_run_set_flags:
 * @play_channel_run: the #AgsPlayChannelRun
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 2.0.0
 */
void
ags_play_channel_run_set_flags(AgsPlayChannelRun *play_channel_run, guint flags)
{
  pthread_mutex_t *recall_mutex;

  if(!AGS_IS_PLAY_CHANNEL_RUN(play_channel_run)){
    return;
  }

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(play_channel_run)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(recall_mutex);

  play_channel_run->flags |= flags;

  pthread_mutex_unlock(recall_mutex);
}

/**
 * ags_play_channel_run_unset_flags:
 * @play_channel_run: the #AgsPlayChannelRun
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 2.0.0
 */
void
ags_play_channel_run_unset_flags(AgsPlayChannelRun *play_channel_run, guint flags)
{
  pthread_mutex_t *recall_mutex;

  if(!AGS_IS_PLAY_CHANNEL_RUN(play_channel_run)){
    return;
  }

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(play_channel_run)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* set flags */
  pthread_mutex_lock(recall_mutex);

  play_channel_run->flags &= (~flags);

  pthread_mutex_unlock(recall_mutex);
}

/**
 * ags_play_channel_run_new:
 * @source: the #AgsChannel
 * @stream_channel_run: the #AgsStreamChannelRun dependency
 *
 * Create a new instance of #AgsPlayChannelRun
 *
 * Returns: the new #AgsPlayChannelRun
 *
 * Since: 2.0.0
 */
AgsPlayChannelRun*
ags_play_channel_run_new(AgsChannel *source,
			 GObject *stream_channel_run)
{
  AgsPlayChannelRun *play_channel_run;

  play_channel_run = (AgsPlayChannelRun *) g_object_new(AGS_TYPE_PLAY_CHANNEL_RUN,
							"source", source,
							"stream-channel-run", stream_channel_run,
							NULL);
  
  return(play_channel_run);
}
