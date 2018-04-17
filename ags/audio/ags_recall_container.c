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

#include <ags/audio/ags_recall_container.h>

#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>

#include <ags/i18n.h>

void ags_recall_container_class_init(AgsRecallContainerClass *recall_class);
void ags_recall_container_init(AgsRecallContainer *recall);
void ags_recall_container_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_recall_container_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_recall_container_dispose(GObject *recall);
void ags_recall_container_finalize(GObject *recall);

/**
 * SECTION:ags_recall_container
 * @short_description: Container to group recalls
 * @title: AgsRecallContainer
 * @section_id:
 * @include: ags/audio/ags_recall_container.h
 *
 * #AgsRecallContainer groups recalls of different context.
 */

enum{
  PROP_0,
  PROP_RECALL_AUDIO_TYPE,
  PROP_RECALL_AUDIO,
  PROP_RECALL_AUDIO_RUN_TYPE,
  PROP_RECALL_AUDIO_RUN,
  PROP_RECALL_CHANNEL_TYPE,
  PROP_RECALL_CHANNEL,
  PROP_RECALL_CHANNEL_RUN_TYPE,
  PROP_RECALL_CHANNEL_RUN,
};

static gpointer ags_recall_container_parent_class = NULL;

static pthread_mutex_t ags_recall_container_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_recall_container_get_type (void)
{
  static GType ags_type_recall_container = 0;

  if(!ags_type_recall_container){
    static const GTypeInfo ags_recall_container_info = {
      sizeof (AgsRecallContainerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_container_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallContainer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_container_init,
    };

    ags_type_recall_container = g_type_register_static(G_TYPE_OBJECT,
						       "AgsRecallContainer",
						       &ags_recall_container_info,
						       0);
  }

  return(ags_type_recall_container);
}

void
ags_recall_container_class_init(AgsRecallContainerClass *recall_container)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_recall_container_parent_class = g_type_class_peek_parent(recall_container);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_container;

  gobject->dispose = ags_recall_container_dispose;
  gobject->finalize = ags_recall_container_finalize;

  gobject->set_property = ags_recall_container_set_property;
  gobject->get_property = ags_recall_container_get_property;

  /* properties */
  /**
   * AgsRecallContainer:recall-audio-type:
   *
   * The associated recall type within audio context.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_gtype("recall-audio-type",
				  i18n_pspec("audio level recall type"),
				  i18n_pspec("The recall type which this recall container has on audio level"),
				  G_TYPE_NONE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_AUDIO_TYPE,
				  param_spec);

  /**
   * AgsRecallContainer:recall-audio:
   *
   * The associated recall within audio context.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("recall-audio",
				   i18n_pspec("audio level recall"),
				   i18n_pspec("The recall which this recall container has on audio level"),
				   AGS_TYPE_RECALL_AUDIO,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_AUDIO,
				  param_spec);

  /**
   * AgsRecallContainer:recall-audio-run-type:
   *
   * The associated recall type within dynamic audio context.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_gtype("recall-audio-run-type",
				  i18n_pspec("audio runlevel recall type"),
				  i18n_pspec("The recall type which this recall container has on audio level during a run"),
				  G_TYPE_NONE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_AUDIO_RUN_TYPE,
				  param_spec);

  /**
   * AgsRecallContainer:recall-audio-run:
   *
   * The associated recall within dynamic audio context.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("recall-audio-run",
				    i18n_pspec("audio runlevel recall"),
				    i18n_pspec("The recall which this recall container has on audio level during a run"),
				    G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_AUDIO_RUN,
				  param_spec);

  /**
   * AgsRecallContainer:recall-channel-type:
   *
   * The associated recall type within channel context.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_gtype("recall-channel-type",
				  i18n_pspec("channel level recall type"),
				  i18n_pspec("The recall type which this recall container has on channel level"),
				  G_TYPE_NONE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_CHANNEL_TYPE,
				  param_spec);

  /**
   * AgsRecallContainer:recall-channel:
   *
   * The associated recall within channel context.
   * 
   * Since: 2.0.0
   */
 param_spec = g_param_spec_pointer("recall-channel",
				   i18n_pspec("channel level recall"),
				   i18n_pspec("The recall which this recall container has on channel level"),
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_CHANNEL,
				  param_spec);

  /**
   * AgsRecallContainer:recall-channel-run-type:
   *
   * The associated recall type within dynamic channel context.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_gtype("recall-channel-run-type",
				  i18n_pspec("channel runlevel recall type"),
				  i18n_pspec("The recall type which this recall container has on audio level during a run"),
				  G_TYPE_NONE,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_CHANNEL_RUN_TYPE,
				  param_spec);

  /**
   * AgsRecallContainer:recall-channel-run:
   *
   * The associated recall within dynamic channel context.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("recall-channel-run",
				    i18n_pspec("channel runlevel recall"),
				    i18n_pspec("The recall which this recall container has on audio level during a run"),
				    G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_CHANNEL_RUN,
				  param_spec);
}

void
ags_recall_container_init(AgsRecallContainer *recall_container)
{
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  recall_container->flags = 0;

  /* add recall container mutex */
  recall_container->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  recall_container->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);  

  /*  */
  recall_container->recall_audio = NULL;
  recall_container->recall_audio_run = NULL;
  recall_container->recall_channel = NULL;
  recall_container->recall_channel_run = NULL;
}

void
ags_recall_container_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsRecallContainer *recall_container;

  pthread_mutex_t *recall_container_mutex;

  recall_container = AGS_RECALL_CONTAINER(gobject);

  /* get recall container mutex */
  pthread_mutex_lock(ags_recall_container_get_class_mutex());
  
  recall_container_mutex = recall_container->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_container_get_class_mutex());

  switch(prop_id){
  case PROP_RECALL_AUDIO_TYPE:
    {
      GType recall_audio_type;

      recall_audio_type = (GType) g_value_get_gtype(value);

      pthread_mutex_lock(recall_container_mutex);

      recall_container->recall_audio_type = recall_audio_type;

      pthread_mutex_unlock(recall_container_mutex);
    }
    break;
  case PROP_RECALL_AUDIO:
    {
      AgsAudio *audio;
      AgsRecallAudio *recall_audio;

      GList *list_start, *list;
      
      recall_audio = (AgsRecallAudio *) g_value_get_object(value);

      pthread_mutex_lock(recall_container_mutex);

      if(recall_container->recall_audio == (AgsRecall *) recall_audio){
	pthread_mutex_unlock(recall_container_mutex);
	
	return;
      }

      if(recall_container->recall_audio != NULL){
	g_object_unref(G_OBJECT(recall_container->recall_audio));
      }

      if(recall_audio != NULL){
	g_object_ref(G_OBJECT(recall_audio));
      }

      recall_container->recall_audio = (AgsRecall *) recall_audio;

      pthread_mutex_unlock(recall_container_mutex);

      /* set recall audio - recall audio run */
      pthread_mutex_lock(recall_container_mutex);

      list =
	list_start = g_list_copy(recall_container->recall_audio_run);

      pthread_mutex_unlock(recall_container_mutex);

      while(list != NULL){
	g_object_set(G_OBJECT(list->data),
		     "recall-audio", recall_audio,
		     NULL);

	list = list->next;
      }

      g_list_free(list_start);

      /* set recall audio - recall channel */
      pthread_mutex_lock(recall_container_mutex);

      list =
	list_start = g_list_copy(recall_container->recall_channel);

      pthread_mutex_unlock(recall_container_mutex);

      while(list != NULL){
	g_object_set(G_OBJECT(list->data),
		     "recall-audio", recall_audio,
		     NULL);

	list = list->next;
      }

      g_list_free(list_start);

      /* set recall audio - recall channel run */
      pthread_mutex_lock(recall_container_mutex);

      list =
	list_start = g_list_copy(recall_container->recall_channel_run);

      pthread_mutex_unlock(recall_container_mutex);

      while(list != NULL){
	g_object_set(G_OBJECT(list->data),
		     "recall-audio", recall_audio,
		     NULL);

	list = list->next;
      }

      g_list_free(list_start);
    }
    break;
  case PROP_RECALL_AUDIO_RUN_TYPE:
    {
      GType recall_audio_run_type;

      recall_audio_run_type = g_value_get_gtype(value);

      pthread_mutex_lock(recall_container_mutex);

      recall_container->recall_audio_run_type = recall_audio_run_type;

      pthread_mutex_unlock(recall_container_mutex);
    }
    break;
  case PROP_RECALL_AUDIO_RUN:
    {
      AgsRecallAudioRun *recall_audio_run;
      AgsRecallID *recall_id;
      AgsRecyclingContext *recycling_context;

      GList *list_start, *list;

      guint recall_flags;
      
      pthread_mutex_t *recall_mutex;
	
      recall_audio_run = (AgsRecallAudioRun *) g_value_get_pointer(value);

      pthread_mutex_lock(recall_container_mutex);

      if(recall_audio_run == NULL ||
	 g_list_find(recall_container->recall_audio_run, recall_audio_run) != NULL){
	pthread_mutex_unlock(recall_container_mutex);
	
	return;
      }

      g_object_ref(G_OBJECT(recall_audio_run));
      recall_container->recall_audio_run = g_list_prepend(recall_container->recall_audio_run,
							  recall_audio_run);

      pthread_mutex_unlock(recall_container_mutex);
      
      /* get recall mutex */
      pthread_mutex_lock(ags_recall_get_class_mutex());
  
      recall_mutex = AGS_RECALL(recall_audio_run)->obj_mutex;
  
      pthread_mutex_unlock(ags_recall_get_class_mutex());

      /* set recall audio run - recall channel run */      
      pthread_mutex_lock(recall_mutex);

      recall_flags = AGS_RECALL(recall_audio_run)->flags;
      
      pthread_mutex_unlock(recall_mutex);

      g_object_get(recall_audio_run,
		   "recall-id", &recall_id,
		   NULL);
      
      pthread_mutex_lock(recall_container_mutex);

      list =
	list_start = g_list_copy(recall_container->recall_channel_run);

      pthread_mutex_unlock(recall_container_mutex);

      if(recall_id != NULL){
	g_object_get(recall_id,
		     "recycling-context", &recycling_context,
		     NULL);

	while((list = ags_recall_find_recycling_context(list,
							(GObject *) recycling_context)) != NULL){
	  g_object_set(list->data,
		       "recall-audio-run", recall_audio_run,
		       NULL);

	  list = list->next;
	}
      }else if((AGS_RECALL_TEMPLATE & (recall_flags)) != 0){      
	while((list = ags_recall_find_template(list_start)) != NULL){
	  g_object_set(list->data,
		       "recall-audio-run", recall_audio_run,
		       NULL);

	  list = list->next;
	}
      }
      
      g_list_free(list_start);
    }
    break;
  case PROP_RECALL_CHANNEL_TYPE:
    {
      GType recall_channel_type;

      recall_channel_type = (GType) g_value_get_gtype(value);

      pthread_mutex_lock(recall_container_mutex);

      recall_container->recall_channel_type = recall_channel_type;

      pthread_mutex_unlock(recall_container_mutex);
    }
    break;
  case PROP_RECALL_CHANNEL:
    {
      AgsChannel *source;
      AgsRecallChannel *recall_channel;

      GList *list_start, *list;

      pthread_mutex_t *recall_mutex;

      recall_channel = (AgsRecallChannel *) g_value_get_pointer(value);

      pthread_mutex_lock(recall_container_mutex);

      if(recall_channel == NULL ||
	 g_list_find(recall_container->recall_channel, recall_channel) != NULL){
	pthread_mutex_unlock(recall_container_mutex);
	
	return;
      }

      g_object_ref(G_OBJECT(recall_channel));      
      recall_container->recall_channel = g_list_prepend(recall_container->recall_channel,
							recall_channel);

      pthread_mutex_unlock(recall_container_mutex);
      
      /* get recall mutex */
      pthread_mutex_lock(ags_recall_get_class_mutex());
  
      recall_mutex = AGS_RECALL(recall_channel)->obj_mutex;
  
      pthread_mutex_unlock(ags_recall_get_class_mutex());

      /* set recall channel - recall channel run */
      pthread_mutex_lock(recall_mutex);

      source = recall_channel->source;

      pthread_mutex_unlock(recall_mutex);
      
      pthread_mutex_lock(recall_container_mutex);

      list =
	list_start = g_list_copy(recall_container->recall_channel_run);

      pthread_mutex_unlock(recall_container_mutex);

      while((list = ags_recall_find_provider(list, source)) != NULL){
	g_object_set(G_OBJECT(list->data),
		     "recall-channel", recall_channel,
		     NULL);

	list = list->next;
      }

      g_list_free(list_start);
    }
    break;
  case PROP_RECALL_CHANNEL_RUN_TYPE:
    {
      GType recall_channel_run_type;

      recall_channel_run_type = (GType) g_value_get_gtype(value);

      pthread_mutex_lock(recall_container_mutex);

      recall_container->recall_channel_run_type = recall_channel_run_type;

      pthread_mutex_unlock(recall_container_mutex);
    }
    break;
  case PROP_RECALL_CHANNEL_RUN:
    {
      AgsRecallChannelRun *recall_channel_run;

      recall_channel_run = (AgsRecallChannelRun *) g_value_get_pointer(value);

      pthread_mutex_lock(recall_container_mutex);

      if(recall_channel_run == NULL ||
	 g_list_find(recall_container->recall_channel_run, recall_channel_run) != NULL){
	pthread_mutex_unlock(recall_container_mutex);
	
	return;
      }

      g_object_ref(G_OBJECT(recall_channel_run));
      recall_container->recall_channel_run = g_list_prepend(recall_container->recall_channel_run,
							    recall_channel_run);
      
      pthread_mutex_unlock(recall_container_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_container_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsRecallContainer *recall_container;

  pthread_mutex_t *recall_container_mutex;

  recall_container = AGS_RECALL_CONTAINER(gobject);

  /* get recall container mutex */
  pthread_mutex_lock(ags_recall_container_get_class_mutex());
  
  recall_container_mutex = recall_container->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_container_get_class_mutex());

  switch(prop_id){
  case PROP_RECALL_AUDIO_TYPE:
    {
      pthread_mutex_lock(recall_container_mutex);

      g_value_set_gtype(value, recall_container->recall_audio_type);

      pthread_mutex_unlock(recall_container_mutex);
    }
    break;
  case PROP_RECALL_AUDIO:
    {
      pthread_mutex_lock(recall_container_mutex);

      g_value_set_object(value, recall_container->recall_audio);

      pthread_mutex_unlock(recall_container_mutex);
    }
    break;
  case PROP_RECALL_AUDIO_RUN_TYPE:
    {
      pthread_mutex_lock(recall_container_mutex);

      g_value_set_gtype(value, recall_container->recall_audio_run_type);

      pthread_mutex_unlock(recall_container_mutex);
    }
    break;
  case PROP_RECALL_AUDIO_RUN:
    {
      pthread_mutex_lock(recall_container_mutex);

      g_value_set_pointer(value, g_list_copy(recall_container->recall_audio_run));

      pthread_mutex_unlock(recall_container_mutex);
    }
    break;
  case PROP_RECALL_CHANNEL_TYPE:
    {
      pthread_mutex_lock(recall_container_mutex);

      g_value_set_gtype(value, recall_container->recall_channel_type);

      pthread_mutex_unlock(recall_container_mutex);
    }
    break;
  case PROP_RECALL_CHANNEL:
    {
      pthread_mutex_lock(recall_container_mutex);

      g_value_set_pointer(value, g_list_copy(recall_container->recall_channel));

      pthread_mutex_unlock(recall_container_mutex);
    }
    break;
  case PROP_RECALL_CHANNEL_RUN_TYPE:
    {
      pthread_mutex_lock(recall_container_mutex);

      g_value_set_gtype(value, recall_container->recall_channel_run_type);

      pthread_mutex_unlock(recall_container_mutex);
    }
    break;
  case PROP_RECALL_CHANNEL_RUN:
    {
      pthread_mutex_lock(recall_container_mutex);

      g_value_set_pointer(value, g_list_copy(recall_container->recall_channel_run));

      pthread_mutex_unlock(recall_container_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_container_dispose(GObject *gobject)
{
  AgsRecallContainer *recall_container;

  recall_container = AGS_RECALL_CONTAINER(gobject);

  /* recall */
  if(recall_container->recall_audio != NULL){
    g_object_unref(recall_container->recall_audio);

    recall_container->recall_audio = NULL;
  }
  
  g_list_free_full(recall_container->recall_audio_run,
		   g_object_unref);

  recall_container->recall_audio_run = NULL;
  
  g_list_free_full(recall_container->recall_channel,
		   g_object_unref);

  recall_container->recall_channel = NULL;
  
  g_list_free_full(recall_container->recall_channel_run,
		   g_object_unref);

  recall_container->recall_channel_run = NULL;
  
  /* call parent */
  G_OBJECT_CLASS(ags_recall_container_parent_class)->dispose(gobject);
}

void
ags_recall_container_finalize(GObject *gobject)
{
  AgsRecallContainer *recall_container;

  recall_container = AGS_RECALL_CONTAINER(gobject);

  pthread_mutex_destroy(recall_container->obj_mutex);
  free(recall_container->obj_mutex);

  pthread_mutexattr_destroy(recall_container->obj_mutexattr);
  free(recall_container->obj_mutexattr);

  /* recall */
  g_object_unref(recall_container->recall_audio);
  
  g_list_free_full(recall_container->recall_audio_run,
		   g_object_unref);

  g_list_free_full(recall_container->recall_channel,
		   g_object_unref);


  g_list_free_full(recall_container->recall_channel_run,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_recall_container_parent_class)->finalize(gobject);
}

/**
 * ags_recall_container_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_recall_container_get_class_mutex()
{
  return(&ags_recall_container_class_mutex);
}

/**
 * ags_recall_container_add:
 * @recall_container: the #AgsRecallContainer
 * @recall: the #AgsRecall
 * 
 * Add @recall to @recall_container.
 * 
 * Since: 2.0.0
 */
void
ags_recall_container_add(AgsRecallContainer *recall_container,
			 AgsRecall *recall)
{
  if(!AGS_IS_RECALL_CONTAINER(recall_container)){
    return;
  }

  if(AGS_IS_RECALL_AUDIO(recall)){
    g_object_set(recall_container,
		 "recall-audio", recall,
		 NULL);

    g_object_set(recall,
		 "recall-container", recall_container,
		 NULL);
  }else if(AGS_IS_RECALL_AUDIO_RUN(recall)){
    g_object_set(recall_container,
		 "recall-audio-run", recall,
		 NULL);

    g_object_set(recall,
		 "recall-container", recall_container,
		 NULL);
  }else if(AGS_IS_RECALL_CHANNEL(recall)){
    g_object_set(recall_container,
		 "recall-channel", recall,
		 NULL);

    g_object_set(recall,
		 "recall-container", recall_container,
		 NULL);
  }else if(AGS_IS_RECALL_CHANNEL_RUN(recall)){
    g_object_set(recall_container,
		 "recall-channel-run", recall,
		 NULL);

    g_object_set(recall,
		 "recall-container", recall_container,
		 NULL);
  }
}

/**
 * ags_recall_container_remove:
 * @recall_container: the #AgsRecallContainer
 * @recall: the #AgsRecall
 * 
 * Remove @recall from @recall_container.
 * 
 * Since: 2.0.0
 */
void
ags_recall_container_remove(AgsRecallContainer *recall_container,
			    AgsRecall *recall)
{
  if(!AGS_IS_RECALL_CONTAINER(recall_container)){
    return;
  }

  if(AGS_IS_RECALL_AUDIO(recall)){
    if(recall == recall_container->recall_audio){      
      g_object_set(recall,
		   "recall-container", NULL,
		   NULL);
      
      g_object_set(recall_container,
		   "recall-audio", NULL,
		   NULL);
    }
  }else if(AGS_IS_RECALL_AUDIO_RUN(recall)){
    if(g_list_find(recall_container->recall_audio_run,
		   recall) != NULL){
      g_object_set(recall,
		   "recall-container", NULL,
		   NULL);

      recall_container->recall_audio_run = g_list_remove(recall_container->recall_audio_run,
							 recall);
      g_object_unref(recall);
    }
  }else if(AGS_IS_RECALL_CHANNEL(recall)){
    if(g_list_find(recall_container->recall_channel,
		   recall) != NULL){
      g_object_set(recall,
		   "recall-container", recall_container,
		   NULL);
      
      recall_container->recall_channel = g_list_remove(recall_container->recall_channel,
						       recall);
      g_object_unref(recall);
    }
  }else if(AGS_IS_RECALL_CHANNEL_RUN(recall)){
    if(g_list_find(recall_container->recall_channel_run,
		   recall) != NULL){
      g_object_set(recall,
		   "recall-container", recall_container,
		   NULL);
      
      recall_container->recall_channel_run = g_list_remove(recall_container->recall_channel_run,
							   recall);
      g_object_unref(recall);
    }
  }
}

/**
 * ags_recall_container_get_recall_audio:
 * @recall_container: the #AgsRecallContainer
 *
 * Retrieve recall audio of container.
 *
 * Returns: the #AgsRecallAudio
 *
 * Since: 1.0.0
 */
AgsRecall*
ags_recall_container_get_recall_audio(AgsRecallContainer *recall_container)
{
  if(!AGS_IS_RECALL_CONTAINER(recall_container)){
    return(NULL);
  }

  return(recall_container->recall_audio);
}

/**
 * ags_recall_container_get_recall_audio_run:
 * @recall_container: the #AgsRecallContainer
 *
 * Retrieve recall audio run of container.
 *
 * Returns: the #AgsRecallAudioRun as list
 *
 * Since: 1.0.0
 */
GList*
ags_recall_container_get_recall_audio_run(AgsRecallContainer *recall_container)
{
  if(!AGS_IS_RECALL_CONTAINER(recall_container)){
    return(NULL);
  }

  return(recall_container->recall_audio_run);
}

/**
 * ags_recall_container_get_recall_channel:
 * @recall_container: the #AgsRecallContainer
 *
 * Retrieve the recall channel of container.
 *
 * Returns: the #AgsRecallChannel
 *
 * Since: 1.0.0
 */
GList*
ags_recall_container_get_recall_channel(AgsRecallContainer *recall_container)
{
  if(!AGS_IS_RECALL_CONTAINER(recall_container)){
    return(NULL);
  }

  return(recall_container->recall_channel);
}

/**
 * ags_recall_container_get_recall_channel_run:
 * @recall_container: the #AgsRecallContainer
 *
 * Retrieve the recall channel run of container.
 *
 * Returns: the #AgsRecall
 *
 * Since: 1.0.0
 */
GList*
ags_recall_container_get_recall_channel_run(AgsRecallContainer *recall_container)
{
  if(!AGS_IS_RECALL_CONTAINER(recall_container)){
    return(NULL);
  }

  return(recall_container->recall_channel_run);
}

/**
 * ags_recall_container_find:
 * @recall_container: the #AgsRecallContainer
 * @type: recall type
 * @find_flags: search mask
 * @recall_id: an #AgsRecallID
 *
 * Finds #AgsRecall for appropriate search criteria.
 *
 * Returns: the matching recalls
 *
 * Since: 1.0.0
 */
GList*
ags_recall_container_find(GList *recall_container,
			  GType gtype,
			  guint find_flags,
			  AgsRecallID *recall_id)
{
  AgsRecallContainer *current;
  AgsRecall *recall;

  guint mode;

  if(g_type_is_a(gtype, AGS_TYPE_RECALL_AUDIO)){
    mode = 0;
  }else if(g_type_is_a(gtype, AGS_TYPE_RECALL_AUDIO_RUN)){
    mode = 1;
  }else if(g_type_is_a(gtype, AGS_TYPE_RECALL_CHANNEL)){
    mode = 2;
  }else if(g_type_is_a(gtype, AGS_TYPE_RECALL_CHANNEL_RUN)){
    mode = 3;
  }else{
    g_message("ags_recall_container_find: invalid type\n");
    return(NULL);
  }

  g_message("mode = %d", mode);
  
  while(recall_container != NULL){
    current = AGS_RECALL_CONTAINER(recall_container->data);

    if(mode == 0){
      recall = ags_recall_container_get_recall_audio(current);
    }else if(mode == 1){
      GList *list;

      list = ags_recall_container_get_recall_audio_run(current);
	
      if(list == NULL)
	recall = NULL;
      else
	recall = AGS_RECALL(list->data);
    }else if(mode == 2){
      GList *list;

      list = ags_recall_container_get_recall_channel(current);

      if(list == NULL)
	recall = NULL;
      else
	recall = AGS_RECALL(list->data);
    }else if(mode == 3){
      GList *list;

      list = ags_recall_container_get_recall_channel_run(current);

      if(list == NULL)
	recall = NULL;
      else
	recall = AGS_RECALL(list->data);
    }
   
    if(recall != NULL){      
      if((AGS_RECALL_CONTAINER_FIND_TYPE & find_flags) != 0 && G_OBJECT_TYPE(recall) == gtype){
	break;
      }
      
      if((AGS_RECALL_CONTAINER_FIND_TEMPLATE & find_flags) != 0 && (AGS_RECALL_TEMPLATE & (recall->flags)) != 0){
	break;
      }
      
      if((AGS_RECALL_CONTAINER_FIND_RECALL_ID & find_flags) != 0 && (recall->recall_id != NULL && recall->recall_id == recall_id)){
	break;
      }
    }
      
    recall_container = recall_container->next;
  }

  return(recall_container);
}

/**
 * ags_recall_container_new:
 * 
 * Creates an #AgsRecallContainer
 *
 * Returns: a new #AgsRecallContainer
 *
 * Since: 1.0.0
 */
AgsRecallContainer*
ags_recall_container_new()
{
  AgsRecallContainer *recall_container;

  recall_container = (AgsRecallContainer *) g_object_new(AGS_TYPE_RECALL_CONTAINER,
							 NULL);

  return(recall_container);
}
