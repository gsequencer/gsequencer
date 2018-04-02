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

#include <ags/audio/ags_recall_audio_run.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_recall_container.h>

#include <pthread.h>

#include <ags/i18n.h>

void ags_recall_audio_run_class_init(AgsRecallAudioRunClass *recall_audio_run);
void ags_recall_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_audio_run_packable_interface_init(AgsPackableInterface *packable);
void ags_recall_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_recall_audio_run_init(AgsRecallAudioRun *recall_audio_run);
void ags_recall_audio_run_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_recall_audio_run_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_recall_audio_run_dispose(GObject *gobject);
void ags_recall_audio_run_finalize(GObject *gobject);

gboolean ags_recall_audio_run_pack(AgsPackable *packable, GObject *container);
gboolean ags_recall_audio_run_unpack(AgsPackable *packable);

AgsRecall* ags_recall_audio_run_duplicate(AgsRecall *recall,
					  AgsRecallID *recall_id,
					  guint *n_params, gchar **parameter_name, GValue *value);

/**
 * SECTION:ags_recall_audio_run
 * @short_description: audio context of dynamic recall
 * @title: AgsRecallAudioRun
 * @section_id:
 * @include: ags/audio/ags_recall_audio_run.h
 *
 * #AgsRecallAudioRun acts as dynamic audio recall.
 */

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_RECALL_AUDIO,
};

static gpointer ags_recall_audio_run_parent_class = NULL;
static AgsConnectableInterface* ags_recall_audio_run_parent_connectable_interface;
static AgsPackableInterface* ags_recall_audio_run_parent_packable_interface;
static AgsDynamicConnectableInterface *ags_recall_audio_run_parent_dynamic_connectable_interface;

GType
ags_recall_audio_run_get_type()
{
  static GType ags_type_recall_audio_run = 0;

  if(!ags_type_recall_audio_run){
    static const GTypeInfo ags_recall_audio_run_info = {
      sizeof (AgsRecallAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_audio_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_audio_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_packable_interface_info = {
      (GInterfaceInitFunc) ags_recall_audio_run_packable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_audio_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_audio_run = g_type_register_static(AGS_TYPE_RECALL,
						       "AgsRecallAudioRun",
						       &ags_recall_audio_run_info,
						       0);

    g_type_add_interface_static(ags_type_recall_audio_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_audio_run,
				AGS_TYPE_PACKABLE,
				&ags_packable_interface_info);

    g_type_add_interface_static(ags_type_recall_audio_run,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return(ags_type_recall_audio_run);
}

void
ags_recall_audio_run_class_init(AgsRecallAudioRunClass *recall_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_recall_audio_run_parent_class = g_type_class_peek_parent(recall_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_audio_run;

  gobject->set_property = ags_recall_audio_run_set_property;
  gobject->get_property = ags_recall_audio_run_get_property;

  gobject->dispose = ags_recall_audio_run_dispose;
  gobject->finalize = ags_recall_audio_run_finalize;

  /* properties */
  /**
   * AgsRecallAudioRun:audio:
   *
   * The assigned audio.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("assigned audio"),
				   i18n_pspec("The audio object it is assigned to"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsRecallAudioRun:recall-audio:
   *
   * The recall audio belonging to.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("recall-audio",
				   i18n_pspec("AgsRecallAudio of this recall"),
				   i18n_pspec("The AgsRecallAudio which this recall needs"),
				   AGS_TYPE_RECALL_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_AUDIO,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_audio_run;

  recall->duplicate = ags_recall_audio_run_duplicate;
}

void
ags_recall_audio_runconnectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_recall_audio_run_packable_interface_init(AgsPackableInterface *packable)
{
  ags_recall_audio_run_parent_packable_interface = g_type_interface_peek_parent(packable);

  packable->pack = ags_recall_audio_run_pack;
  packable->unpack = ags_recall_audio_run_unpack;
}

void
ags_recall_audio_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_recall_audio_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);
}

void
ags_recall_audio_run_init(AgsRecallAudioRun *recall_audio_run)
{
  recall_audio_run->audio = NULL;
  recall_audio_run->recall_audio = NULL;
}

void
ags_recall_audio_run_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsRecallAudioRun *recall_audio_run;

  pthread_mutex_t *recall_mutex;

  recall_audio_run = AGS_RECALL_AUDIO_RUN(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsRecallAudio *audio;

      audio = (AgsRecallAudio *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(recall_audio_run->audio == audio){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(recall_audio_run->audio != NULL){
	g_object_unref(G_OBJECT(recall_audio_run->audio));
      }

      if(audio != NULL){
	g_object_ref(G_OBJECT(audio));
      }

      recall_audio_run->audio = audio;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RECALL_AUDIO:
    {
      AgsRecallAudio *recall_audio;

      recall_audio = (AgsRecallAudio *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(recall_audio_run->recall_audio == recall_audio){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(recall_audio_run->recall_audio != NULL){
	g_object_unref(G_OBJECT(recall_audio_run->recall_audio));
      }

      if(recall_audio != NULL){
	g_object_ref(G_OBJECT(recall_audio));
      }

      recall_audio_run->recall_audio = recall_audio;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_audio_run_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsRecallAudioRun *recall_audio_run;

  pthread_mutex_t *recall_mutex;

  recall_audio_run = AGS_RECALL_AUDIO_RUN(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_AUDIO:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, recall_audio_run->audio);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RECALL_AUDIO:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, recall_audio_run->recall_audio);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_audio_run_dispose(GObject *gobject)
{
  AgsRecallAudioRun *recall_audio_run;

  recall_audio_run = AGS_RECALL_AUDIO_RUN(gobject);

  /* unpack */
  ags_packable_unpack(AGS_PACKABLE(recall_audio_run));

  if(AGS_RECALL(gobject)->recall_container != NULL){
    AgsRecallContainer *recall_container;

    recall_container = AGS_RECALL(gobject)->recall_container;

    recall_container->recall_audio_run = g_list_remove(recall_container->recall_audio_run,
						       gobject);
    g_object_unref(gobject);
    g_object_unref(AGS_RECALL(gobject)->recall_container);

    AGS_RECALL(gobject)->recall_container = NULL;
  }

  /* audio */
  if(recall_audio_run->audio != NULL){
    g_object_unref(G_OBJECT(recall_audio_run->audio));

    recall_audio_run->audio = NULL;
  }

  /* recall audio */
  if(recall_audio_run->recall_audio != NULL){
    g_object_unref(G_OBJECT(recall_audio_run->recall_audio));

    recall_audio_run->recall_audio = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_audio_run_parent_class)->dispose(gobject);
}

void
ags_recall_audio_run_finalize(GObject *gobject)
{
  AgsRecallAudioRun *recall_audio_run;

  recall_audio_run = AGS_RECALL_AUDIO_RUN(gobject);

  if(AGS_RECALL(gobject)->recall_container != NULL){
    AgsRecallContainer *recall_container;

    recall_container = AGS_RECALL(gobject)->recall_container;

    recall_container->recall_audio_run = g_list_remove(recall_container->recall_audio_run,
						       gobject);
    g_object_unref(AGS_RECALL(gobject)->recall_container);
  }
  
  /* audio */
  if(recall_audio_run->audio != NULL){
    g_object_unref(G_OBJECT(recall_audio_run->audio));
  }

  /* recall audio */
  if(recall_audio_run->recall_audio != NULL){
    g_object_unref(G_OBJECT(recall_audio_run->recall_audio));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_audio_run_parent_class)->finalize(gobject);
}

gboolean
ags_recall_audio_run_pack(AgsPackable *packable, GObject *recall_container)
{
  AgsRecallAudio *recall_audio;
  AgsRecallAudioRun *recall_audio_run;
  AgsRecallID *recall_id;
  AgsRecyclingContext *recycling_context;
  
  GList *list_start, *list;

  guint recall_flags;
  
  pthread_mutex_t *recall_mutex;
  pthread_mutex_t *recall_id_mutex;

  if(ags_recall_audio_run_parent_packable_interface->pack(packable, recall_container)){
    return(TRUE);
  }

  recall_audio_run = AGS_RECALL_AUDIO_RUN(packable);

  g_object_get(recall_container,
	       "recall-audio", &recall_audio,
	       NULL);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(recall_audio_run)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());
  
  /* set AgsRecallAudio */
  g_object_set(G_OBJECT(recall_audio_run),
	       "recall-audio", recall_audio,
	       NULL);

  /* set in AgsRecallChannelRun */
  g_object_get(recall_container,
	       "recall-channel-run", &list_start,
	       NULL);

  /* get some fields */
  pthread_mutex_lock(recall_mutex);
  
  recall_flags = AGS_RECALL(recall_audio_run)->flags;
  recall_id = AGS_RECALL(recall_audio_run)->recall_id;

  pthread_mutex_unlock(recall_mutex);
  
  list = list_start;

  if(recall_id != NULL){
    /* get recall id mutex */
    pthread_mutex_lock(ags_recall_id_get_class_mutex());
    
    recall_id_mutex = AGS_RECALL(recall_id)->obj_mutex;
    
    pthread_mutex_unlock(ags_recall_id_get_class_mutex());

    /* get recycling context */
    pthread_mutex_lock(recall_id_mutex);
  
    recycling_context = recall_id->recycling_context;
    
    pthread_mutex_unlock(recall_id_mutex);

    while((list = ags_recall_find_recycling_context(list, (GObject *) recycling_context)) != NULL){
      g_object_set(G_OBJECT(list->data),
		   "recall-audio-run", recall_audio_run,
		   NULL);

      list= list->next;
    }
  }else if((AGS_RECALL_TEMPLATE & (recall_flags)) != 0){
    while((list = ags_recall_find_template(list)) != NULL){
      g_object_set(G_OBJECT(list->data),
		   "recall-audio-run", recall_audio_run,
		   NULL);

      list= list->next;
    }
  }

  g_list_free(list_start);
  
  g_object_set(G_OBJECT(recall_container),
	       "recall-audio-run", recall_audio_run,
	       NULL);

  return(FALSE);
}

gboolean
ags_recall_audio_run_unpack(AgsPackable *packable)
{
  AgsRecallAudioRun *recall_audio_run;
  AgsRecallContainer *recall_container;
  AgsRecallID *recall_id;
  AgsRecyclingContext *recycling_context;

  GList *list_start, *list;

  guint recall_flags;
  
  pthread_mutex_t *recall_mutex;
  pthread_mutex_t *recall_container_mutex;
  pthread_mutex_t *recall_id_mutex;

  if(!AGS_IS_RECALL_AUDIO_RUN(packable)){
    return(TRUE);
  }

  recall_audio_run = AGS_RECALL_AUDIO_RUN(packable);

  g_object_get(recall_audio_run,
	       "recall-container", &recall_container,
	       NULL);
 
  if(!AGS_IS_RECALL_CONTAINER(recall_container)){
    return(TRUE);
  }
  
  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(recall_audio_run)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* ref */
  g_object_ref(recall_audio_run);
  g_object_ref(recall_container);
  
  /* unset AgsRecallAudio */
  g_object_set(recall_audio_run,
	       "recall-audio", NULL,
	       NULL);

  /* set in AgsRecallChannelRun */
  g_object_get(recall_container,
	       "recall-channel-run", &list_start,
	       NULL);

  /* get some fields */
  pthread_mutex_lock(recall_mutex);
  
  recall_flags = AGS_RECALL(recall_audio_run)->flags;
  recall_id = AGS_RECALL(recall_audio_run)->recall_id;

  pthread_mutex_unlock(recall_mutex);
  
  list = list_start;

  if(recall_id != NULL){
    /* get recall id mutex */
    pthread_mutex_lock(ags_recall_id_get_class_mutex());
    
    recall_id_mutex = AGS_RECALL(recall_id)->obj_mutex;
    
    pthread_mutex_unlock(ags_recall_id_get_class_mutex());

    /* get recycling context */
    pthread_mutex_lock(recall_id_mutex);
  
    recycling_context = recall_id->recycling_context;
    
    pthread_mutex_unlock(recall_id_mutex);

    while((list = ags_recall_find_recycling_context(list, recycling_context)) != NULL){
      g_object_set(G_OBJECT(list->data),
		   "recall-audio-run", NULL,
		   NULL);

      list= list->next;
    }
  }else if((AGS_RECALL_TEMPLATE & (recall_flags)) != 0){
    while((list = ags_recall_find_template(list)) != NULL){
      g_object_set(G_OBJECT(list->data),
		   "recall-audio-run", NULL,
		   NULL);

      list= list->next;
    }
  }

  g_list_free(list_start);

  /* call parent */
  if(ags_recall_audio_run_parent_packable_interface->unpack(packable)){
    g_object_unref(recall_audio_run);
    g_object_unref(recall_container);

    return(TRUE);
  }

  /* get recall container mutex */
  pthread_mutex_lock(ags_recall_container_get_class_mutex());
  
  recall_container_mutex = recall_container->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_container_get_class_mutex());

  /* remove from list */
  pthread_mutex_lock(recall_container_mutex);

  recall_container->recall_audio_run = g_list_remove(recall_container->recall_audio_run,
						     recall_audio_run);

  pthread_mutex_unlock(recall_container_mutex);

  /* unref */
  g_object_unref(recall_audio_run);
  g_object_unref(recall_container);

  return(FALSE);
}

AgsRecall*
ags_recall_audio_run_duplicate(AgsRecall *recall,
			       AgsRecallID *recall_id,
			       guint *n_params, gchar **parameter_name, GValue *value)
{
  AgsRecallAudioRun *recall_audio_run, *copy;

  recall_audio_run = AGS_RECALL_AUDIO_RUN(recall);

  copy = AGS_RECALL_AUDIO_RUN(AGS_RECALL_CLASS(ags_recall_audio_run_parent_class)->duplicate(recall,
											     recall_id,
											     n_params, parameter_name, value));

  g_object_set(G_OBJECT(copy),
	       "recall-audio", recall_audio_run->recall_audio,
	       NULL);

  return((AgsRecall *) copy);
}

/**
 * ags_recall_audio_run_new:
 *
 * Creates an #AgsRecallAudioRun.
 *
 * Returns: a new #AgsRecallAudioRun.
 *
 * Since: 2.0.0
 */
AgsRecallAudioRun*
ags_recall_audio_run_new()
{
  AgsRecallAudioRun *recall_audio_run;

  recall_audio_run = (AgsRecallAudioRun *) g_object_new(AGS_TYPE_RECALL_AUDIO_RUN,
							NULL);

  return(recall_audio_run);
}
