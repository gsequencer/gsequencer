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

#include <ags/audio/ags_recall_dependency.h>

#include <ags/libags.h>

#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>

#include <stdio.h>

#include <ags/i18n.h>

void ags_recall_dependency_class_init(AgsRecallDependencyClass *recall_dependency);
void ags_recall_dependency_init(AgsRecallDependency *recall_dependency);
void ags_recall_dependency_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_recall_dependency_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_recall_dependency_dispose(GObject *gobject);
void ags_recall_dependency_finalize(GObject *gobject);

/**
 * SECTION:ags_recall_dependency
 * @short_description: recall dependency
 * @title: AgsRecallDependency
 * @section_id:
 * @include: ags/audio/ags_recall_dependency.h
 *
 * #AgsRecallDependency specifies dependencies on other recalls. Dependencies
 * are resolved during initialization.
 */

enum{
  PROP_0,
  PROP_DEPENDENCY,
};

static gpointer ags_recall_dependency_parent_class = NULL;

static pthread_mutex_t ags_recall_dependency_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_recall_dependency_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_recall_dependency = 0;

    static const GTypeInfo ags_recall_dependency_info = {
      sizeof(AgsRecallDependencyClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_dependency_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsRecallDependency),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_dependency_init,
    };

    ags_type_recall_dependency = g_type_register_static(G_TYPE_OBJECT,
							"AgsRecallDependency",
							&ags_recall_dependency_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_recall_dependency);
  }

  return g_define_type_id__volatile;
}

void
ags_recall_dependency_class_init(AgsRecallDependencyClass *recall_dependency)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_recall_dependency_parent_class = g_type_class_peek_parent(recall_dependency);

  gobject = (GObjectClass *) recall_dependency;

  gobject->set_property = ags_recall_dependency_set_property;
  gobject->get_property = ags_recall_dependency_get_property;
  
  gobject->dispose = ags_recall_dependency_dispose;
  gobject->finalize = ags_recall_dependency_finalize;

  /* properties */
  /**
   * AgsRecallDependency:dependency:
   *
   * The dependency.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("dependency",
				   i18n_pspec("dependency of recall"),
				   i18n_pspec("A dependency of the recall"),
				   AGS_TYPE_RECALL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEPENDENCY,
				  param_spec);
}

void
ags_recall_dependency_init(AgsRecallDependency *recall_dependency)
{
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  recall_dependency->flags = 0;
  
  /* add recall dependency mutex */
  recall_dependency->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  recall_dependency->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  /* dependency */
  recall_dependency->dependency = NULL;
}

void
ags_recall_dependency_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsRecallDependency *recall_dependency;

  pthread_mutex_t *recall_dependency_mutex;

  recall_dependency = AGS_RECALL_DEPENDENCY(gobject);

  /* get recall dependency mutex */
  pthread_mutex_lock(ags_recall_dependency_get_class_mutex());
  
  recall_dependency_mutex = recall_dependency->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_dependency_get_class_mutex());
  
  switch(prop_id){
  case PROP_DEPENDENCY:
    {
      AgsRecall *dependency;
      
      dependency = (AgsRecall *) g_value_get_object(value);
      
      pthread_mutex_lock(recall_dependency_mutex);

      if(recall_dependency->dependency == (GObject *) dependency){
	pthread_mutex_unlock(recall_dependency_mutex);
	
	return;
      }
	
      if(recall_dependency->dependency != NULL){
	g_object_unref(G_OBJECT(recall_dependency->dependency));
      }

      if(dependency != NULL){
	g_object_ref(G_OBJECT(dependency));
      }

      recall_dependency->dependency = (GObject *) dependency;

      pthread_mutex_unlock(recall_dependency_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_dependency_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsRecallDependency *recall_dependency;

  pthread_mutex_t *recall_dependency_mutex;

  recall_dependency = AGS_RECALL_DEPENDENCY(gobject);

  /* get recall dependency mutex */
  pthread_mutex_lock(ags_recall_dependency_get_class_mutex());
  
  recall_dependency_mutex = recall_dependency->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_dependency_get_class_mutex());

  switch(prop_id){
  case PROP_DEPENDENCY:
    {
      pthread_mutex_lock(recall_dependency_mutex);

      g_value_set_object(value, recall_dependency->dependency);

      pthread_mutex_unlock(recall_dependency_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_dependency_dispose(GObject *gobject)
{
  AgsRecallDependency *recall_dependency;

  recall_dependency = AGS_RECALL_DEPENDENCY(gobject);

  /* dependency */
  if(recall_dependency->dependency != NULL){
    g_object_unref(recall_dependency->dependency);

    recall_dependency->dependency = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_recall_dependency_parent_class)->dispose(gobject);
}

void
ags_recall_dependency_finalize(GObject *gobject)
{
  AgsRecallDependency *recall_dependency;

  recall_dependency = AGS_RECALL_DEPENDENCY(gobject);

  pthread_mutex_destroy(recall_dependency->obj_mutex);
  free(recall_dependency->obj_mutex);

  pthread_mutexattr_destroy(recall_dependency->obj_mutexattr);
  free(recall_dependency->obj_mutexattr);
  
  /* dependency */
  if(recall_dependency->dependency != NULL){
    g_object_unref(recall_dependency->dependency);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_dependency_parent_class)->finalize(gobject);
}

/**
 * ags_recall_dependency_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_recall_dependency_get_class_mutex()
{
  return(&ags_recall_dependency_class_mutex);
}

/**
 * ags_recall_dependency_find_dependency:
 * @recall_dependency: a #GList-struct containing  #AgsRecallDependency
 * @dependency: the #AgsRecall depending on
 *
 * Retrieve dependency.
 *
 * Returns: next matching #GList-struct or %NULL.
 * 
 * Since: 2.0.0
 */
GList*
ags_recall_dependency_find_dependency(GList *recall_dependency, GObject *dependency)
{
  GObject *current_dependency;

  while(recall_dependency != NULL){
    g_object_get(recall_dependency->data,
		 "dependency", &current_dependency,
		 NULL);

    if(current_dependency == dependency){
      return(recall_dependency);
    }

    recall_dependency = recall_dependency->next;
  }

  return(NULL);
}

/**
 * ags_recall_dependency_find_dependency_by_provider:
 * @recall_dependency: a #GList-struct containing  #AgsRecallDependency
 * @provider: the object providing recall, like #AgsAudio or #AgsChannel
 *
 * Retrieve dependency by provider.
 *
 * Returns: next matching #GList-struct or %NULL.
 * 
 * Since: 2.0.0
 */
GList*
ags_recall_dependency_find_dependency_by_provider(GList *recall_dependency,
						  GObject *provider)
{
  GObject *current_dependency;

  while(recall_dependency != NULL){
    g_object_get(recall_dependency->data,
		 "dependency", &current_dependency,
		 NULL);

    if(AGS_IS_CHANNEL(provider) &&
       AGS_IS_RECALL_CHANNEL_RUN(current_dependency)){
      AgsChannel *channel;

      g_object_get(current_dependency,
		   "source", &channel,
		   NULL);
      
      if(channel == AGS_CHANNEL(provider)){
	return(recall_dependency);
      }
    }

    if(AGS_IS_AUDIO(provider) &&
       AGS_IS_RECALL_AUDIO_RUN(current_dependency)){
      AgsAudio *audio;

      g_object_get(current_dependency,
		   "audio", &audio,
		   NULL);
      
      if(audio == AGS_AUDIO(provider)){
	return(recall_dependency);
      }
    }

    recall_dependency = recall_dependency->next;
  }

  return(NULL);
}

/**
 * ags_recall_dependency_resolve:
 * @recall_dependency: an #AgsRecallDependency
 * @recall_id: the #AgsRecallID refering to
 *
 * Resolve dependency.
 *
 * Returns: the #AgsRecall dependency.
 * 
 * Since: 2.0.0
 */
GObject*
ags_recall_dependency_resolve(AgsRecallDependency *recall_dependency, AgsRecallID *recall_id)
{
  AgsRecallContainer *recall_container;
  AgsRecall *dependency;
  
  g_object_get(recall_dependency,
	       "dependency", &dependency,
	       NULL);
  
  if(dependency == NULL){
    return(NULL);
  }

  g_object_get(dependency,
	       "recall-container", &recall_container,
	       NULL);
  
  if(recall_container == NULL){
    return(NULL);
  }
  
  if(AGS_IS_RECALL_AUDIO(dependency)){
    GObject *recall_audio;

    g_object_get(recall_container,
		 "recall-audio", &recall_audio,
		 NULL);
    
    return(recall_audio);
  }else if(AGS_IS_RECALL_AUDIO_RUN(dependency)){
    AgsRecyclingContext *recycling_context;

    GObject *recall_audio_run;
    
    GList *list_start, *list;

    if(recall_id == NULL){
      g_message("dependency resolve: recall_id == NULL");
      
      return(NULL);
    }

    g_object_get(recall_id,
		 "recycling-context", &recycling_context,
		 NULL);
    
    g_object_get(recall_container,
		 "recall-audio-run", &list_start,
		 NULL);
    list = ags_recall_find_recycling_context(list_start,
					     (GObject *) recycling_context);

    if(list != NULL){
      recall_audio_run = list->data;

      g_list_free(list_start);
      
      return(recall_audio_run);
    }

    g_list_free(list_start);
  }else if(AGS_IS_RECALL_CHANNEL(dependency)){
    AgsChannel *source;

    GObject *recall_channel;
    
    GList *list_start, *list;

    g_object_get(recall_container,
		 "recall-channel", &list_start,
		 NULL);

    g_object_get(dependency,
		 "source", &source,
		 NULL);
    
    list = ags_recall_find_provider(list_start,
				    (GObject *) source);

    if(list != NULL){
      recall_channel = list->data;

      g_list_free(list_start);
      
      return(recall_channel);
    }

    g_list_free(list_start);
  }else if(AGS_IS_RECALL_CHANNEL_RUN(dependency)){
    AgsRecyclingContext *recycling_context;

    GObject *recall_channel_run;
    
    GList *list_start, *list;

    if(recall_id == NULL){
      g_message("dependency resolve: recall_id == NULL");
      
      return(NULL);
    }

    g_object_get(recall_id,
		 "recycling-context", &recycling_context,
		 NULL);
    
    g_object_get(recall_container,
		 "recall-channel-run", &list_start,
		 NULL);
    list = ags_recall_find_recycling_context(list_start,
					     (GObject *) recycling_context);

    if(list != NULL){
      recall_channel_run = list->data;

      g_list_free(list_start);
      
      return(recall_channel_run);
    }

    g_list_free(list_start);
  }

  g_warning("dependency not found!");

  return(NULL);
}

/**
 * ags_recall_dependency_new:
 * @dependency: the #AgsRecall depending on
 *
 * Creates an #AgsRecallDependency
 *
 * Returns: the new #AgsRecallDependency
 * 
 * Since: 2.0.0
 */
AgsRecallDependency*
ags_recall_dependency_new(GObject *dependency)
{
  AgsRecallDependency *recall_dependency;

  recall_dependency = (AgsRecallDependency *) g_object_new(AGS_TYPE_RECALL_DEPENDENCY,
							   "dependency", dependency,
							   NULL);

  return(recall_dependency);
}
