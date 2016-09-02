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

#include <ags/audio/ags_recycling.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_concurrent_tree.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>

#include <string.h>
#include <math.h>

void ags_recycling_class_init(AgsRecyclingClass *recycling_class);
void ags_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recycling_concurrent_tree_interface_init(AgsConcurrentTreeInterface *concurrent_tree);
void ags_recycling_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_recycling_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_recycling_init(AgsRecycling *recycling);
void ags_recycling_connect(AgsConnectable *connectable);
void ags_recycling_disconnect(AgsConnectable *connectable);
pthread_mutex_t* ags_recycling_get_lock(AgsConcurrentTree *concurrent_tree);
pthread_mutex_t* ags_recycling_get_parent_lock(AgsConcurrentTree *concurrent_tree);
void ags_recycling_finalize(GObject *gobject);

void ags_recycling_real_add_audio_signal(AgsRecycling *recycling,
					 AgsAudioSignal *audio_signal);

void ags_recycling_real_remove_audio_signal(AgsRecycling *recycling,
					    AgsAudioSignal *audio_signal);

/**
 * SECTION:ags_recycling
 * @short_description: A container of audio signals
 * @title: AgsRecycling
 * @section_id:
 * @include: ags/audio/ags_recycling.h
 *
 * #AgsRecycling forms the nested tree of AgsChannel. Ever channel
 * having own audio signal contains therefor an #AgsRecycling
 */

enum{
  PROP_0,
  PROP_SOUNDCARD,
  PROP_CHANNEL,
  PROP_PARENT,
  PROP_NEXT,
  PROP_PREV,
  PROP_AUDIO_SIGNAL,
};

enum{
  ADD_AUDIO_SIGNAL,
  REMOVE_AUDIO_SIGNAL,
  LAST_SIGNAL,
};

static gpointer ags_recycling_parent_class = NULL;
static guint recycling_signals[LAST_SIGNAL];

GType
ags_recycling_get_type(void)
{
  static GType ags_type_recycling = 0;

  if(!ags_type_recycling){
    static const GTypeInfo ags_recycling_info = {
      sizeof (AgsRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recycling_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_concurrent_tree_interface_info = {
      (GInterfaceInitFunc) ags_recycling_concurrent_tree_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recycling = g_type_register_static(G_TYPE_OBJECT,
						"AgsRecycling\0",
						&ags_recycling_info, 0);

    g_type_add_interface_static(ags_type_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recycling,
				AGS_TYPE_CONCURRENT_TREE,
				&ags_concurrent_tree_interface_info);
  }

  return(ags_type_recycling);
}

void
ags_recycling_class_init(AgsRecyclingClass *recycling)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_recycling_parent_class = g_type_class_peek_parent(recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) recycling;

  gobject->set_property = ags_recycling_set_property;
  gobject->get_property = ags_recycling_get_property;

  gobject->finalize = ags_recycling_finalize;

  /* properties */
  /**
   * AgsRecycling:channel:
   *
   * The assigned #AgsChannel.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("channel\0",
				   "assigned channel\0",
				   "The channel it is assigned with\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /**
   * AgsRecycling:soundcard:
   *
   * The assigned #GObject acting as default sink.
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("soundcard\0",
				   "assigned soundcard\0",
				   "The soundcard it is assigned with\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /**
   * AgsRecycling:parent:
   *
   * The assigned parent #AgsRecycling.
   * 
   * Since: 0.7.2
   */
  param_spec = g_param_spec_object("parent\0",
				   "assigned parent\0",
				   "The parent it is assigned with\0",
				   AGS_TYPE_RECYCLING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PARENT,
				  param_spec);

  /**
   * AgsRecycling:prev:
   *
   * The assigned prev #AgsRecycling.
   * 
   * Since: 0.7.2
   */
  param_spec = g_param_spec_object("prev\0",
				   "assigned prev\0",
				   "The prev it is assigned with\0",
				   AGS_TYPE_RECYCLING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PREV,
				  param_spec);

  /**
   * AgsRecycling:next:
   *
   * The assigned next #AgsRecycling.
   * 
   * Since: 0.7.2
   */
  param_spec = g_param_spec_object("next\0",
				   "assigned next\0",
				   "The next it is assigned with\0",
				   AGS_TYPE_RECYCLING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NEXT,
				  param_spec);

  /**
   * AgsRecycling:audio-signal:
   *
   * The containing  #AgsAudioSignal.
   * 
   * Since: 0.7.2
   */
  param_spec = g_param_spec_object("audio-signal\0",
				   "containing audio signal\0",
				   "The audio signal it contains\0",
				   AGS_TYPE_AUDIO_SIGNAL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_SIGNAL,
				  param_spec);
  
  /*  */
  recycling->add_audio_signal = ags_recycling_real_add_audio_signal;
  recycling->remove_audio_signal = ags_recycling_real_remove_audio_signal;

  /**
   * AgsRecycling::add-audio-signal
   * @recycling: an #AgsRecycling
   * @audio_signal: the #AgsAudioSignal to add
   *
   * The ::add-audio-signal signal is emited as adding #AgsAudioSignal
   */
  recycling_signals[ADD_AUDIO_SIGNAL] =
    g_signal_new("add-audio-signal\0",
		 G_TYPE_FROM_CLASS (recycling),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecyclingClass, add_audio_signal),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsRecycling::remove-audio-signal:
   * @recycling: an #AgsRecycling
   * @audio_signal: the #AgsAudioSignal to remove
   *
   * The ::remove-audio-signal signal is emited as removing #AgsAudioSignal
   */
  recycling_signals[REMOVE_AUDIO_SIGNAL] =
    g_signal_new("remove-audio-signal\0",
		 G_TYPE_FROM_CLASS (recycling),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsRecyclingClass, remove_audio_signal),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_recycling_connect;
  connectable->disconnect = ags_recycling_disconnect;
}

void
ags_recycling_concurrent_tree_interface_init(AgsConcurrentTreeInterface *concurrent_tree)
{
  concurrent_tree->get_lock = ags_recycling_get_lock;
  concurrent_tree->get_parent_lock = ags_recycling_get_parent_lock;
}

void
ags_recycling_init(AgsRecycling *recycling)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t attr;

  //FIXME:JK: memory leak
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr,
			    PTHREAD_MUTEX_RECURSIVE);

  mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     &attr);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);
  
  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) recycling,
			   mutex);
  
  pthread_mutex_unlock(application_mutex);

  recycling->flags = 0;

  recycling->soundcard = NULL;

  recycling->channel = NULL;

  recycling->parent = NULL;

  recycling->next = NULL;
  recycling->prev = NULL;

  recycling->audio_signal = NULL;
}

void
ags_recycling_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsRecycling *recycling;
  
  recycling = AGS_RECYCLING(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = (GObject *) g_value_get_object(value);

      ags_recycling_set_soundcard(recycling, (GObject *) soundcard);
    }
    break;
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      if(channel == (AgsChannel *) recycling->channel){
	return;
      }

      if(recycling->channel != NULL){
	g_object_unref(recycling->channel);
      }

      if(channel != NULL){
	g_object_ref(channel);
      }

      recycling->channel = (GObject *) channel;
    }
    break;
  case PROP_PARENT:
    {
      AgsRecycling *recycling;

      recycling = (AgsRecycling *) g_value_get_object(value);

      if(recycling->parent == recycling){
	return;
      }

      if(recycling->parent != NULL){
	g_object_unref(recycling->parent);
      }

      if(recycling != NULL){
	g_object_ref(recycling);
      }

      recycling->parent = recycling;
    }
    break;
  case PROP_NEXT:
    {
      AgsRecycling *recycling;

      recycling = (AgsRecycling *) g_value_get_object(value);

      if(recycling->next == recycling){
	return;
      }

      if(recycling->next != NULL){
	g_object_unref(recycling->next);
      }

      if(recycling != NULL){
	g_object_ref(recycling);
      }

      recycling->next = recycling;
    }
    break;
  case PROP_PREV:
    {
      AgsRecycling *recycling;

      recycling = (AgsRecycling *) g_value_get_object(value);

      if(recycling->prev == recycling){
	return;
      }

      if(recycling->prev != NULL){
	g_object_unref(recycling->prev);
      }

      if(recycling != NULL){
	g_object_ref(recycling);
      }

      recycling->prev = recycling;
    }
    break;
  case PROP_AUDIO_SIGNAL:
    {
      AgsAudioSignal *audio_signal;

      audio_signal = g_value_get_object(value);

      if(audio_signal == NULL ||
	 g_list_find(recycling->audio_signal, audio_signal) != NULL){
	return;
      }

      ags_recycling_add_audio_signal(recycling,
				     audio_signal);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recycling_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsRecycling *recycling;
  
  recycling = AGS_RECYCLING(gobject);
  
  switch(prop_id){
  case PROP_SOUNDCARD:
    g_value_set_object(value, recycling->soundcard);
    break;
  case PROP_CHANNEL:
    g_value_set_object(value, recycling->channel);
    break;
  case PROP_PARENT:
    {
      g_value_set_object(value, recycling->parent);
    }
    break;
  case PROP_NEXT:
    {
      g_value_set_object(value, recycling->next);
    }
    break;
  case PROP_PREV:
    {
      g_value_set_object(value, recycling->prev);
    }
    break;
  case PROP_AUDIO_SIGNAL:
    {
      g_value_set_pointer(value, g_list_copy(recycling->audio_signal));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recycling_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_recycling_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

pthread_mutex_t*
ags_recycling_get_lock(AgsConcurrentTree *concurrent_tree)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *recycling_mutex;
  
  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);
  
  recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					     AGS_RECYCLING(concurrent_tree));

  pthread_mutex_unlock(application_mutex);

  return(recycling_mutex);
}

pthread_mutex_t*
ags_recycling_get_parent_lock(AgsConcurrentTree *concurrent_tree)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *parent_mutex;
  
  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);

  if(AGS_RECYCLING(concurrent_tree)->parent != NULL){
    parent_mutex = ags_mutex_manager_lookup(mutex_manager,
					    AGS_RECYCLING(concurrent_tree)->parent);
  }else{
    parent_mutex = ags_mutex_manager_lookup(mutex_manager,
					    AGS_RECYCLING(concurrent_tree)->soundcard);
  }
  
  pthread_mutex_unlock(application_mutex);

  return(parent_mutex);
}

void
ags_recycling_finalize(GObject *gobject)
{
  AgsRecycling *recycling;  

  AgsMutexManager *mutex_manager;
  
  GList *list, *list_next;

  pthread_mutex_t *application_mutex;
  
  //  g_warning("ags_recycling_finalize\0");
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_remove(mutex_manager,
			   gobject);
  
  pthread_mutex_unlock(application_mutex);

  recycling = AGS_RECYCLING(gobject);

  /* AgsAudioSignal */
  g_list_free_full(recycling->audio_signal,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_recycling_parent_class)->finalize(gobject);
}

/**
 * ags_recycling_set_soundcard:
 * @recycling:  an #AgsRecycling
 * @soundcard: the #GObject to set
 *
 * Sets #GObject to recycling.
 *
 * Since: 0.3
 */
void
ags_recycling_set_soundcard(AgsRecycling *recycling, GObject *soundcard)
{
  /* recycling */
  if(recycling->soundcard == soundcard)
    return;

  if(recycling->soundcard != NULL)
    g_object_unref(recycling->soundcard);

  if(soundcard != NULL)
    g_object_ref(soundcard);

  recycling->soundcard = (GObject *) soundcard;
}

/**
 * ags_recycling_add_audio_signal:
 * @recycling:  an #AgsRecycling
 * @audio_signal: the #AgsAudioSignal to add
 *
 * Add #AgsAudioSignal to recycling.
 *
 * Since: 0.3
 */
void
ags_recycling_add_audio_signal(AgsRecycling *recycling,
			       AgsAudioSignal *audio_signal)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *recycling_mutex;

  if(recycling == NULL){
    return;
  }
  
  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) recycling);

  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(recycling_mutex);

  if(!AGS_IS_RECYCLING(recycling)){
    pthread_mutex_unlock(recycling_mutex);

    return;
  }
  
  if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
    pthread_mutex_unlock(recycling_mutex);

    return;
  }

  /* emit signal */
  g_object_ref(G_OBJECT(recycling));
  g_signal_emit(G_OBJECT(recycling),
		recycling_signals[ADD_AUDIO_SIGNAL], 0,
		audio_signal);
  g_object_unref(G_OBJECT(recycling));

  /* release lock */
  pthread_mutex_unlock(recycling_mutex);
}

void
ags_recycling_real_add_audio_signal(AgsRecycling *recycling,
				    AgsAudioSignal *audio_signal)
{
  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) != 0){
    AgsAudioSignal *old_template;

    /* old template */
    old_template = ags_audio_signal_get_template(recycling->audio_signal);
    
    /* remove old template */
    ags_recycling_remove_audio_signal(recycling,
				      old_template);
  }
  
  recycling->audio_signal = g_list_prepend(recycling->audio_signal, (gpointer) audio_signal);
  audio_signal->recycling = (GObject *) recycling;
  g_object_ref(recycling);
  g_object_ref(audio_signal);
}

/**
 * ags_recycling_remove_audio_signal:
 * @recycling:  an #AgsRecycling
 * @audio_signal: the #AgsAudioSignal to remove
 *
 * Remove #AgsAudioSignal of recycling.
 *
 * Since: 0.3
 */
void
ags_recycling_remove_audio_signal(AgsRecycling *recycling,
				  AgsAudioSignal *audio_signal)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *recycling_mutex;

  if(recycling == NULL){
    return;
  }
  
  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) recycling);

  pthread_mutex_unlock(application_mutex);
  
  pthread_mutex_lock(recycling_mutex);

  if(!AGS_IS_RECYCLING(recycling)){
    pthread_mutex_unlock(recycling_mutex);

    return;
  }
  
  if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
    pthread_mutex_unlock(recycling_mutex);

    return;
  }

  /* emit signal */
  g_object_ref((GObject *) recycling);
  g_object_ref((GObject *) audio_signal);
  g_signal_emit(G_OBJECT(recycling),
		recycling_signals[REMOVE_AUDIO_SIGNAL], 0,
		audio_signal);
  g_object_unref((GObject *) audio_signal);
  g_object_unref((GObject *) recycling);

  /* release lock */
  pthread_mutex_unlock(recycling_mutex);
}

void
ags_recycling_real_remove_audio_signal(AgsRecycling *recycling,
				       AgsAudioSignal *audio_signal)
{

  if(g_list_find(recycling->audio_signal,
		 (gpointer) audio_signal) == NULL){
    return;
  }
  
  recycling->audio_signal = g_list_remove(recycling->audio_signal,
					  (gpointer) audio_signal);
  audio_signal->recycling = NULL;
  g_object_unref(audio_signal);
  g_object_unref(recycling);
}

/**
 * ags_recycling_create_audio_signal_with_defaults:
 * @recycling: an #AgsRecycling
 * @audio_signal: the #AgsAudioSignal to apply defaults 
 * @delay: the delay
 * @attack: the attack
 *
 * Create audio signal with defaults.
 *
 * Since: 0.4
 */
void
ags_recycling_create_audio_signal_with_defaults(AgsRecycling *recycling,
						AgsAudioSignal *audio_signal,
						gdouble delay, guint attack)
{
  AgsAudioSignal *template;

  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *recycling_mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) recycling);

  pthread_mutex_unlock(application_mutex);
  
  pthread_mutex_lock(recycling_mutex);

  if(!AGS_IS_RECYCLING(recycling)){
    pthread_mutex_unlock(recycling_mutex);

    return;
  }

  if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
    pthread_mutex_unlock(recycling_mutex);

    return;
  }

  /* create audio signal */
  template = ags_audio_signal_get_template(recycling->audio_signal);

  audio_signal->delay = delay;
  audio_signal->attack = attack;

  if(template == NULL){
    ags_audio_signal_stream_resize(audio_signal,
				   0);

    /* release lock */
    pthread_mutex_unlock(recycling_mutex);

    return;
  }

  audio_signal->soundcard = template->soundcard;

  audio_signal->recycling = (GObject *) recycling;

  audio_signal->samplerate = template->samplerate;
  audio_signal->buffer_size = template->buffer_size;
  audio_signal->format = template->format;

  audio_signal->last_frame = (((guint)(delay *
				       template->buffer_size) +
			       attack +
			       template->last_frame) %
			      template->buffer_size);
  audio_signal->loop_start = (((guint) (delay *
					template->buffer_size) +
			       attack +
			       template->loop_start) %
			      template->buffer_size);
  audio_signal->loop_end = (((guint)(delay *
				     template->buffer_size) +
			     attack +
			     template->loop_end) %
			    template->buffer_size);

  ags_audio_signal_stream_resize(audio_signal,
				 template->length);

  ags_audio_signal_duplicate_stream(audio_signal,
				    template);

  /* release lock */
  pthread_mutex_unlock(recycling_mutex);
}

/**
 * ags_recycling_create_audio_signal_with_frame_count:
 * @recycling: an #AgsRecycling
 * @audio_signal: the #AgsAudioSignal to apply defaults 
 * @frame_count: the audio data size
 * @delay: the delay
 * @attack: the attack
 *
 * Create audio signal with frame count.
 *
 * Since: 0.4
 */
void
ags_recycling_create_audio_signal_with_frame_count(AgsRecycling *recycling,
						   AgsAudioSignal *audio_signal,
						   guint frame_count,
						   gdouble delay, guint attack)
{
  GObject *soundcard;
  AgsAudioSignal *template;

  AgsMutexManager *mutex_manager;

  GList *stream, *template_stream, *template_loop;

  guint first_frame;
  guint loop_frame_count;
  guint n_frames;
  guint i, j, k;

  guint copy_mode;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *recycling_mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) recycling);

  pthread_mutex_unlock(application_mutex);
  
  pthread_mutex_lock(recycling_mutex);
  
  if(!AGS_IS_RECYCLING(recycling)){
    pthread_mutex_unlock(recycling_mutex);

    return;
  }
  
  if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
    pthread_mutex_unlock(recycling_mutex);

    return;
  }

  /* create audio signal */
  template = ags_audio_signal_get_template(recycling->audio_signal);

  audio_signal->soundcard = template->soundcard;

  soundcard = AGS_SOUNDCARD(audio_signal->soundcard);

  audio_signal->recycling = (GObject *) recycling;

  audio_signal->samplerate = template->samplerate;
  audio_signal->buffer_size = template->buffer_size;
  audio_signal->format = template->format;
  audio_signal->word_size = template->word_size;

  audio_signal->last_frame = (((guint)(delay *
				       template->buffer_size) +
			       attack +
			       template->last_frame) %
			      template->buffer_size);
  audio_signal->loop_start = ((guint) (delay *
				       template->buffer_size) +
			      attack +
			      template->loop_start);
  audio_signal->loop_end = ((guint)(delay *
				    template->buffer_size) +
			    attack +
			    template->loop_end);

  /* resize */
  first_frame = (guint) floor((delay * audio_signal->buffer_size) + attack) % audio_signal->buffer_size;
  loop_frame_count = ((frame_count - template->loop_start) / (template->loop_end - template->loop_start)) * template->buffer_size;

  ags_audio_signal_stream_resize(audio_signal,
				 (guint) ceil(loop_frame_count / audio_signal->buffer_size) + 1);
    
  audio_signal->last_frame = ((guint) (delay * audio_signal->buffer_size) + frame_count + attack) % audio_signal->buffer_size;

  if(template->length == 0 ||
     audio_signal->loop_start == audio_signal->loop_end){
    /* release lock */
    pthread_mutex_unlock(recycling_mutex);

    return;
  }

  /* generic copying */
  stream = g_list_nth(audio_signal->stream_beginning,
		      (guint) ((delay * audio_signal->buffer_size) + attack) / audio_signal->buffer_size);
  template_stream = template->stream_beginning;

  /* loop related copying */
  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(audio_signal->format),
						  ags_audio_buffer_util_format_from_soundcard(template->format));

  for(i = 0, j = 0; i < loop_frame_count; i++){
    ags_audio_buffer_util_copy_buffer_to_buffer(stream->data, 1, i % audio_signal->buffer_size,
						template_stream->data, 1, j,
						1, copy_mode);
    
    if(i > template->loop_start &&
       i > template->loop_start + (template->loop_end - template->loop_start) &&
       (i - template->loop_start) / (template->loop_end - template->loop_start) < (loop_frame_count - template->loop_start) / (template->loop_end - template->loop_start) &&
       (i - template->loop_start) % (template->loop_end - template->loop_start) == 0){
	j = template->loop_start % template->buffer_size;

	template_stream = g_list_nth(template->stream_beginning,
				     floor(template->loop_start / loop_frame_count));
    }else{
      j++;
    }

    if(i != 0 &&
       (i + 1) % audio_signal->buffer_size == 0){
      stream = stream->next;
    }
    
    if(j == template->buffer_size){
      j = 0;
      
      if(template_stream->next != NULL){
	template_stream = template_stream->next;
      }else{
	template_stream = g_list_nth(template->stream_beginning,
				     floor(template->loop_start / loop_frame_count));
      }
    }
  }


  /* release lock */
  pthread_mutex_unlock(recycling_mutex);
}

/**
 * ags_recycling_find_next_channel:
 * @start_region: boundary start
 * @end_region: boundary end
 * @prev_channel: previous channel
 *
 * Retrieve next recycling with different channel.
 *
 * Returns: Matching recycling.
 *
 * Since: 0.4
 */
AgsRecycling*
ags_recycling_find_next_channel(AgsRecycling *start_region, AgsRecycling *end_region,
				GObject *prev_channel)
{
  AgsRecycling *recycling;
  
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *recycling_mutex, *start_recycling_mutex;

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  pthread_mutex_lock(application_mutex);
  
  start_recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
						   (GObject *) start_region);

  pthread_mutex_unlock(application_mutex);

  /* verify objects and get pointer for safe access */
  pthread_mutex_lock(start_recycling_mutex);

  if(!AGS_IS_RECYCLING(start_region)){
    pthread_mutex_unlock(start_recycling_mutex);
    
    return(NULL);
  }

  pthread_mutex_lock(start_recycling_mutex);

  /* find */
  recycling = start_region;

  while(recycling != NULL &&
	recycling != end_region){
    /* lock current */
    pthread_mutex_lock(application_mutex);
    
    recycling_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) recycling);

    pthread_mutex_unlock(application_mutex);

    /* check if new match */
    pthread_mutex_lock(recycling_mutex);

    if(recycling->channel != prev_channel){
      pthread_mutex_unlock(recycling_mutex);

      return(recycling);
    }

    recycling = recycling->next;
    pthread_mutex_unlock(recycling_mutex);
  }

  /* no new channel within region */
  return(NULL);
}

/**
 * ags_recycling_position:
 * @start_region: boundary start
 * @end_region: boundary end
 * @recycling: matching recycling
 *
 * Retrieve position of recycling.
 *
 * Returns: position within boundary.
 *
 * Since: 0.4
 */
gint
ags_recycling_position(AgsRecycling *start_region, AgsRecycling *end_region,
		       AgsRecycling *recycling)
{
  AgsRecycling *current;

  AgsMutexManager *mutex_manager;

  gint position;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *current_mutex;

  if(start_region == NULL){
    return(-1);
  }

  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* determine position */
  current = start_region;
  position = -1;

  while(current != NULL && current != end_region){
    position++;

    /* lock current */
    pthread_mutex_lock(application_mutex);
    
    current_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) current);

    pthread_mutex_unlock(application_mutex);

    /* check if new match */
    pthread_mutex_lock(current_mutex);

    if(current == recycling){
      pthread_mutex_unlock(current_mutex);

      return(position);
    }

    current = current->next;
    pthread_mutex_unlock(current_mutex);
  }

  /* no match within region */
  return(-1);
}

/**
 * ags_recycling_new:
 * @soundcard: the #GObject
 *
 * Creates a #AgsRecycling, with defaults of @soundcard.
 *
 * Returns: a new #AgsRecycling
 *
 * Since: 0.3
 */
AgsRecycling*
ags_recycling_new(GObject *soundcard)
{
  AgsRecycling *recycling;
  AgsAudioSignal *audio_signal;

  recycling = (AgsRecycling *) g_object_new(AGS_TYPE_RECYCLING,
					    "soundcard\0", soundcard,
					    NULL);

  audio_signal = ags_audio_signal_new(soundcard,
				      (GObject *) recycling,
				      NULL);
  audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;

  ags_connectable_connect(AGS_CONNECTABLE(audio_signal));

  recycling->audio_signal = g_list_alloc();
  recycling->audio_signal->data = (gpointer) audio_signal;

  return(recycling);
}
