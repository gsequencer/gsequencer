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

#include <ags/audio/thread/ags_midi_loop.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_main_loop.h>
#include <ags/object/ags_sequencer.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_audio_run.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>

#include <ags/audio/thread/ags_sequencer_thread.h>
#include <ags/audio/thread/ags_export_thread.h>
#include <ags/audio/thread/ags_audio_thread.h>
#include <ags/audio/thread/ags_channel_thread.h>

void ags_midi_loop_class_init(AgsMidiLoopClass *midi_loop);
void ags_midi_loop_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_midi_loop_main_loop_interface_init(AgsMainLoopInterface *main_loop);
void ags_midi_loop_init(AgsMidiLoop *midi_loop);
void ags_midi_loop_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_midi_loop_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_midi_loop_connect(AgsConnectable *connectable);
void ags_midi_loop_disconnect(AgsConnectable *connectable);
pthread_mutex_t* ags_midi_loop_get_tree_lock(AgsMainLoop *main_loop);
void ags_midi_loop_set_async_queue(AgsMainLoop *main_loop, GObject *async_queue);
GObject* ags_midi_loop_get_async_queue(AgsMainLoop *main_loop);
void ags_midi_loop_set_tic(AgsMainLoop *main_loop, guint tic);
guint ags_midi_loop_get_tic(AgsMainLoop *main_loop);
void ags_midi_loop_set_last_sync(AgsMainLoop *main_loop, guint last_sync);
guint ags_midi_loop_get_last_sync(AgsMainLoop *main_loop);
void ags_midi_loop_finalize(GObject *gobject);

void ags_midi_loop_start(AgsThread *thread);
void ags_midi_loop_run(AgsThread *thread);

/**
 * SECTION:ags_midi_loop
 * @short_description: midi loop
 * @title: AgsMidiLoop
 * @section_id:
 * @include: ags/thread/ags_midi_loop.h
 *
 * The #AgsMidiLoop is suitable as #AgsMainLoop and does
 * midi processing.
 */

enum{
  PROP_0,
  PROP_SEQUENCER,
  PROP_PLAY_MIDI,
};

static gpointer ags_midi_loop_parent_class = NULL;
static AgsConnectableInterface *ags_midi_loop_parent_connectable_interface;

GType
ags_midi_loop_get_type()
{
  static GType ags_type_midi_loop = 0;

  if(!ags_type_midi_loop){
    static const GTypeInfo ags_midi_loop_info = {
      sizeof (AgsMidiLoopClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_midi_loop_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMidiLoop),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_midi_loop_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_midi_loop_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_main_loop_interface_info = {
      (GInterfaceInitFunc) ags_midi_loop_main_loop_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_midi_loop = g_type_register_static(AGS_TYPE_THREAD,
						"AgsMidiLoop\0",
						&ags_midi_loop_info,
						0);
    
    g_type_add_interface_static(ags_type_midi_loop,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_midi_loop,
				AGS_TYPE_MAIN_LOOP,
				&ags_main_loop_interface_info);
  }
  
  return (ags_type_midi_loop);
}

void
ags_midi_loop_class_init(AgsMidiLoopClass *midi_loop)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;
  GParamSpec *param_spec;

  ags_midi_loop_parent_class = g_type_class_peek_parent(midi_loop);

  /* GObject */
  gobject = (GObjectClass *) midi_loop;

  gobject->set_property = ags_midi_loop_set_property;
  gobject->get_property = ags_midi_loop_get_property;

  gobject->finalize = ags_midi_loop_finalize;

  /* properties */
  /**
   * AgsMidiLoop:sequencer:
   *
   * The assigned #AgsMidi.
   * 
   * Since: 0.7.0
   */
  param_spec = g_param_spec_object("sequencer\0",
				   "sequencer assigned to\0",
				   "The AgsSequencer it is assigned to.\0",
				   G_TYPE_OBJECT,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEQUENCER,
				  param_spec);

  /**
   * AgsMidiLoop:play-audio:
   *
   * An #AgsAudio to add for playback.
   * 
   * Since: 0.7.0
   */
  param_spec = g_param_spec_object("play-audio\0",
				   "audio to run\0",
				   "A audio to run\0",
				   AGS_TYPE_AUDIO,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAY_AUDIO,
				  param_spec);

  /* AgsThread */
  thread = (AgsThreadClass *) midi_loop;
  
  thread->start = ags_midi_loop_start;
  thread->run = ags_midi_loop_run;

  /* AgsMidiLoop */
}

void
ags_midi_loop_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_midi_loop_parent_connectable_interface = g_type_interface_peek_parent(connectable);
  
  connectable->connect = ags_midi_loop_connect;
  connectable->disconnect = ags_midi_loop_disconnect;
}

void
ags_midi_loop_main_loop_interface_init(AgsMainLoopInterface *main_loop)
{
  main_loop->get_tree_lock = ags_midi_loop_get_tree_lock;
  main_loop->set_async_queue = ags_midi_loop_set_async_queue;
  main_loop->get_async_queue = ags_midi_loop_get_async_queue;
  main_loop->set_tic = ags_midi_loop_set_tic;
  main_loop->get_tic = ags_midi_loop_get_tic;
  main_loop->set_last_sync = ags_midi_loop_set_last_sync;
  main_loop->get_last_sync = ags_midi_loop_get_last_sync;
}

void
ags_midi_loop_init(AgsMidiLoop *midi_loop)
{
  AgsMutexManager *mutex_manager;
  AgsThread *thread;

  AgsConfig *config;
  
  gchar *str0, *str1;

  pthread_mutex_t *application_mutex;

  /* calculate frequency */
  thread = (AgsThread *) midi_loop;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  //  thread->flags |= AGS_THREAD_WAIT_FOR_CHILDREN;
  pthread_mutex_lock(application_mutex);

  config = ags_config_get_instance();
  
  str0 = ags_config_get_value(config,
			      AGS_CONFIG_SEQUENCER,
			      "samplerate\0");
  str0 = ags_config_get_value(config,
			      AGS_CONFIG_SEQUENCER,
			      "buffer_size\0");

  pthread_mutex_unlock(application_mutex);

  if(str0 == NULL || str1 == NULL){
    thread->freq = AGS_MIDI_LOOP_DEFAULT_JIFFIE;
  }else{
    guint samplerate;
    guint buffer_size;

    samplerate = g_ascii_strtoull(str0,
				  NULL,
				  10);
    buffer_size = g_ascii_strtoull(str0,
				   NULL,
				   10);

    thread->freq = ceil((gdouble) samplerate / (gdouble) buffer_size);
  }

  g_free(str0);
  g_free(str1);
  
  midi_loop->flags = 0;

  g_atomic_int_set(&(midi_loop->tic), 0);
  g_atomic_int_set(&(midi_loop->last_sync), 0);

  midi_loop->application_context = NULL;
  midi_loop->sequencer = NULL;
  
  /* tree lock mutex */
  pthread_mutexattr_init(&(midi_loop->tree_lock_mutexattr));
  pthread_mutexattr_settype(&(midi_loop->tree_lock_mutexattr), PTHREAD_MUTEX_RECURSIVE);

  midi_loop->tree_lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(midi_loop->tree_lock, &(midi_loop->tree_lock_mutexattr));
  
  /* recall mutex */
  midi_loop->recall_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(midi_loop->recall_mutex, NULL);

  /* recall related lists */
  midi_loop->play_midi_ref = 0;
  midi_loop->play_midi = NULL;

  midi_loop->play_notation_ref = 0;
  midi_loop->play_notation = NULL;
}

void
ags_midi_loop_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsMidiLoop *midi_loop;

  midi_loop = AGS_MIDI_LOOP(gobject);

  switch(prop_id){
  case PROP_SEQUENCER:
    {
      GObject *sequencer;

      sequencer = g_value_get_object(value);

      if(midi_loop->sequencer != NULL){
	g_object_unref(midi_loop->sequencer);
      }

      if(sequencer != NULL){
	g_object_ref(sequencer);
      }

      midi_loop->sequencer = sequencer;
    }
    break;
  case PROP_PLAY_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_pointer(value);

      if(audio != NULL){
	midi_loop->play_audio = g_list_prepend(midi_loop->play_audio,
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
ags_midi_loop_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsMidiLoop *midi_loop;

  midi_loop = AGS_MIDI_LOOP(gobject);

  switch(prop_id){
  case PROP_SEQUENCER:
    {
      g_value_set_object(value, midi_loop->sequencer);
    }
    break;
  case PROP_PLAY_AUDIO:
    {
      g_value_set_pointer(value, midi_loop->play_audio);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_loop_connect(AgsConnectable *connectable)
{
  ags_midi_loop_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_midi_loop_disconnect(AgsConnectable *connectable)
{
  ags_midi_loop_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

pthread_mutex_t*
ags_midi_loop_get_tree_lock(AgsMainLoop *main_loop)
{
  return(AGS_MIDI_LOOP(main_loop)->tree_lock);
}

void
ags_midi_loop_set_async_queue(AgsMainLoop *main_loop, GObject *async_queue)
{
  AGS_MIDI_LOOP(main_loop)->async_queue = async_queue;
}

GObject*
ags_midi_loop_get_async_queue(AgsMainLoop *main_loop)
{
  return(AGS_MIDI_LOOP(main_loop)->async_queue);
}

void
ags_midi_loop_set_tic(AgsMainLoop *main_loop, guint tic)
{
  g_atomic_int_set(&(AGS_MIDI_LOOP(main_loop)->tic),
		   tic);
}

guint
ags_midi_loop_get_tic(AgsMainLoop *main_loop)
{
  return(g_atomic_int_get(&(AGS_MIDI_LOOP(main_loop)->tic)));
}

void
ags_midi_loop_set_last_sync(AgsMainLoop *main_loop, guint last_sync)
{
  g_atomic_int_set(&(AGS_MIDI_LOOP(main_loop)->last_sync),
		   last_sync);
}

guint
ags_midi_loop_get_last_sync(AgsMainLoop *main_loop)
{
  gint val;

  val = g_atomic_int_get(&(AGS_MIDI_LOOP(main_loop)->last_sync));

  return(val);
}

void
ags_midi_loop_finalize(GObject *gobject)
{
  AgsMidiLoop *midi_loop;

  midi_loop = AGS_MIDI_LOOP(gobject);

  /* unref AgsPlayback lists */
  g_list_free_full(midi_loop->play_midi,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_midi_loop_parent_class)->finalize(gobject);
}

void
ags_midi_loop_start(AgsThread *thread)
{
  AgsMidiLoop *midi_loop;

  midi_loop = AGS_MIDI_LOOP(thread);

  if((AGS_THREAD_SINGLE_LOOP & (thread->flags)) == 0){
    AgsMutexManager *mutex_manager;
    
    AgsThread *async_queue;

    pthread_mutex_t *application_mutex;
    pthread_mutex_t *mutex;

    /* lookup thread mutex */
    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
    
    pthread_mutex_lock(application_mutex);
    
    mutex = ags_mutex_manager_lookup(mutex_manager,
				     thread);

    pthread_mutex_unlock(application_mutex);

    /* get async queue and gui thread */
    pthread_mutex_lock(mutex);
    
    async_queue = ags_main_loop_get_async_queue(AGS_MAIN_LOOP(thread));

    pthread_mutex_unlock(mutex);

    /*  */
    ags_thread_start(async_queue);

    /*  */
    AGS_THREAD_CLASS(ags_midi_loop_parent_class)->start(thread);
  }
}

void
ags_midi_loop_run(AgsThread *thread)
{
  AgsMidiLoop *midi_loop;

  midi_loop = AGS_MIDI_LOOP(thread);

  //TODO:JK: implement me
}

/**
 * ags_midi_loop_add_midi:
 * @midi_loop: the #AgsMidiLoop
 * @midi: an #AgsMidi
 *
 * Add midi for playback.
 *
 * Since: 0.7.0
 */
void
ags_midi_loop_add_midi(AgsMidiLoop *midi_loop, GObject *midi)
{
  if(g_list_find(midi_loop->play_midi,
		 AGS_MIDI(midi)->playback_domain) == NULL){
    midi_loop->play_midi_ref = midi_loop->play_midi_ref + 1;
    g_object_ref(G_OBJECT(midi));

    midi_loop->play_midi = g_list_prepend(midi_loop->play_midi,
					  AGS_MIDI(midi)->playback_domain);
  }
}

/**
 * ags_midi_loop_remove_midi:
 * @midi_loop: the #AgsMidiLoop
 * @midi: an #AgsMidi
 *
 * Remove midi of playback.
 *
 * Since: 0.7.0
 */
void
ags_midi_loop_remove_midi(AgsMidiLoop *midi_loop, GObject *midi)
{  
  if(g_list_find(midi_loop->play_midi,
		 AGS_MIDI(midi)->playback_domain) != NULL){
    midi_loop->play_midi = g_list_remove(midi_loop->play_midi,
					 AGS_MIDI(midi)->playback_domain);
    midi_loop->play_midi_ref = midi_loop->play_midi_ref - 1;
    
    g_object_unref(midi);
  }
}

/**
 * ags_midi_loop_new:
 * @sequencer: the #GObject
 * @application_context: the #AgsMain
 *
 * Create a new #AgsMidiLoop.
 *
 * Returns: the new #AgsMidiLoop
 *
 * Since: 0.7.0
 */
AgsMidiLoop*
ags_midi_loop_new(GObject *sequencer, GObject *application_context)
{
  AgsMidiLoop *midi_loop;

  midi_loop = (AgsMidiLoop *) g_object_new(AGS_TYPE_MIDI_LOOP,
					   "sequencer\0", sequencer,
					   NULL);

  if(application_context != NULL){
    g_object_ref(G_OBJECT(application_context));
    midi_loop->application_context = application_context;
  }

  return(midi_loop);
}
