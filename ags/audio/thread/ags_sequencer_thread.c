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

#include <ags/audio/thread/ags_sequencer_thread.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>

#include <ags/object/ags_sequencer.h>

#include <ags/thread/ags_timestamp_thread.h>

#include <ags/audio/ags_midiin.h>

#include <ags/audio/jack/ags_jack_client.h>
#include <ags/audio/jack/ags_jack_midiin.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/i18n.h>

void ags_sequencer_thread_class_init(AgsSequencerThreadClass *sequencer_thread);
void ags_sequencer_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sequencer_thread_init(AgsSequencerThread *sequencer_thread);
void ags_sequencer_thread_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_sequencer_thread_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_sequencer_thread_connect(AgsConnectable *connectable);
void ags_sequencer_thread_disconnect(AgsConnectable *connectable);
void ags_sequencer_thread_dispose(GObject *gobject);
void ags_sequencer_thread_finalize(GObject *gobject);

void ags_sequencer_thread_start(AgsThread *thread);
void ags_sequencer_thread_run(AgsThread *thread);
void ags_sequencer_thread_stop(AgsThread *thread);

void ags_sequencer_stopped_all_callback(AgsAudioLoop *audio_loop,
					AgsSequencerThread *sequencer_thread);

/**
 * SECTION:ags_sequencer_thread
 * @short_description: sequencer thread
 * @title: AgsSequencerThread
 * @section_id:
 * @include: ags/thread/ags_sequencer_thread.h
 *
 * The #AgsSequencerThread acts as midi input thread.
 */

enum{
  INTERVAL_TIMEOUT,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_SEQUENCER,
};

static gpointer ags_sequencer_thread_parent_class = NULL;
static AgsConnectableInterface *ags_sequencer_thread_parent_connectable_interface;

static guint sequencer_thread_signals[LAST_SIGNAL];

GType
ags_sequencer_thread_get_type()
{
  static GType ags_type_sequencer_thread = 0;

  if(!ags_type_sequencer_thread){
    static const GTypeInfo ags_sequencer_thread_info = {
      sizeof (AgsSequencerThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sequencer_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSequencerThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sequencer_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_sequencer_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_sequencer_thread = g_type_register_static(AGS_TYPE_THREAD,
						       "AgsSequencerThread",
						       &ags_sequencer_thread_info,
						       0);
    
    g_type_add_interface_static(ags_type_sequencer_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_sequencer_thread);
}

void
ags_sequencer_thread_class_init(AgsSequencerThreadClass *sequencer_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;
  GParamSpec *param_spec;
  
  ags_sequencer_thread_parent_class = g_type_class_peek_parent(sequencer_thread);

  /* GObject */
  gobject = (GObjectClass *) sequencer_thread;

  gobject->set_property = ags_sequencer_thread_set_property;
  gobject->get_property = ags_sequencer_thread_get_property;

  gobject->dispose = ags_sequencer_thread_dispose;
  gobject->finalize = ags_sequencer_thread_finalize;

  /**
   * AgsSequencerThread:sequencer:
   *
   * The assigned #AgsSequencer.
   * 
   * Since: 0.7.121
   */
  param_spec = g_param_spec_object("sequencer",
				   i18n_pspec("sequencer assigned to"),
				   i18n_pspec("The AgsSequencer it is assigned to"),
				   G_TYPE_OBJECT,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEQUENCER,
				  param_spec);

  /* AgsThread */
  thread = (AgsThreadClass *) sequencer_thread;

  thread->start = ags_sequencer_thread_start;
  thread->run = ags_sequencer_thread_run;
  thread->stop = ags_sequencer_thread_stop;

  /* AgsSequencerThread */
  sequencer_thread->interval_timeout = NULL;

  /* signals */
  sequencer_thread_signals[INTERVAL_TIMEOUT] = 
    g_signal_new("interval-timeout",
		 G_TYPE_FROM_CLASS(sequencer_thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsSequencerThreadClass, interval_timeout),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_sequencer_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_sequencer_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_sequencer_thread_connect;
  connectable->disconnect = ags_sequencer_thread_disconnect;
}

void
ags_sequencer_thread_init(AgsSequencerThread *sequencer_thread)
{
  AgsThread *thread;

  AgsConfig *config;
  
  gchar *str0, *str1;
  
  thread = (AgsThread *) sequencer_thread;

  g_atomic_int_or(&(thread->flags),
		  (AGS_THREAD_START_SYNCED_FREQ));  
  
  config = ags_config_get_instance();

  str0 = ags_config_get_value(config,
			      AGS_CONFIG_SOUNDCARD,
			      "samplerate");

  if(str0 == NULL){
    str0 = ags_config_get_value(config,
				AGS_CONFIG_SOUNDCARD_0,
				"samplerate");
  }
  
  str1 = ags_config_get_value(config,
			      AGS_CONFIG_SOUNDCARD,
			      "buffer-size");

  if(str1 == NULL){
    str1 = ags_config_get_value(config,
				AGS_CONFIG_SOUNDCARD_0,
				"buffer-size");
  }

  if(str0 == NULL || str1 == NULL){
    thread->freq = AGS_SEQUENCER_THREAD_DEFAULT_JIFFIE;
  }else{
    guint samplerate;
    guint buffer_size;

    samplerate = g_ascii_strtoull(str0,
				  NULL,
				  10);
    buffer_size = g_ascii_strtoull(str1,
				   NULL,
				   10);

    thread->freq = ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;
  }

  g_free(str0);
  g_free(str1);

  sequencer_thread->sequencer = NULL;

  sequencer_thread->timestamp_thread = NULL;
  //  sequencer_thread->timestamp_thread = (AgsThread *) ags_timestamp_thread_new();
  //  ags_thread_add_child(thread, sequencer_thread->timestamp_thread);

  sequencer_thread->error = NULL;
}

void
ags_sequencer_thread_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsSequencerThread *sequencer_thread;

  sequencer_thread = AGS_SEQUENCER_THREAD(gobject);

  switch(prop_id){
  case PROP_SEQUENCER:
    {
      AgsSequencer *sequencer;

      sequencer = (AgsSequencer *) g_value_get_object(value);

      if(sequencer_thread->sequencer != NULL){
	g_object_unref(G_OBJECT(sequencer_thread->sequencer));
      }

      if(sequencer != NULL){
	g_object_ref(G_OBJECT(sequencer));

	if(AGS_IS_MIDIIN(sequencer)){
	  g_atomic_int_or(&(AGS_THREAD(sequencer_thread)->flags),
			  (AGS_THREAD_INTERMEDIATE_PRE_SYNC));
	}else if(AGS_IS_JACK_MIDIIN(sequencer)){
	  g_atomic_int_and(&(AGS_THREAD(sequencer_thread)->flags),
			   (~AGS_THREAD_INTERMEDIATE_PRE_SYNC));
	}
      }

      sequencer_thread->sequencer = G_OBJECT(sequencer);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_sequencer_thread_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsSequencerThread *sequencer_thread;

  sequencer_thread = AGS_SEQUENCER_THREAD(gobject);

  switch(prop_id){
  case PROP_SEQUENCER:
    {
      g_value_set_object(value, G_OBJECT(sequencer_thread->sequencer));
    }
    break;
  }
}

void
ags_sequencer_thread_connect(AgsConnectable *connectable)
{
  AgsThread *audio_loop, *sequencer_thread;

  sequencer_thread = AGS_THREAD(connectable);

  if((AGS_THREAD_CONNECTED & (g_atomic_int_get(&(sequencer_thread->flags)))) != 0){
    return;
  }  

  ags_sequencer_thread_parent_connectable_interface->connect(connectable);

  audio_loop = ags_thread_get_toplevel(sequencer_thread);
  g_signal_connect((GObject *) audio_loop, "stopped-all",
		   G_CALLBACK(ags_sequencer_stopped_all_callback), sequencer_thread);    
}

void
ags_sequencer_thread_disconnect(AgsConnectable *connectable)
{
  ags_sequencer_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_sequencer_thread_dispose(GObject *gobject)
{
  AgsSequencerThread *sequencer_thread;

  sequencer_thread = AGS_SEQUENCER_THREAD(gobject);

  /* sequencer */
  if(sequencer_thread->sequencer != NULL){
    g_object_unref(sequencer_thread->sequencer);

    sequencer_thread->sequencer = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_sequencer_thread_parent_class)->dispose(gobject);
}

void
ags_sequencer_thread_finalize(GObject *gobject)
{
  AgsSequencerThread *sequencer_thread;

  sequencer_thread = AGS_SEQUENCER_THREAD(gobject);

  /* sequencer */
  if(sequencer_thread->sequencer != NULL){
    g_object_unref(sequencer_thread->sequencer);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_sequencer_thread_parent_class)->finalize(gobject);
}

void
ags_sequencer_thread_start(AgsThread *thread)
{
  AgsSequencer *sequencer;
  AgsSequencerThread *sequencer_thread;
  static gboolean initialized = FALSE;
  GError *error;

  sequencer_thread = AGS_SEQUENCER_THREAD(thread);

  sequencer = AGS_SEQUENCER(sequencer_thread->sequencer);

  /* abort if already recording */
  if(ags_sequencer_is_recording(sequencer)){
    return;
  }

  /* check if already initialized */
  sequencer_thread->error = NULL;

  if(ags_sequencer_get_buffer(sequencer,
			      NULL) == NULL){
    ags_sequencer_record_init(sequencer,
			    &(sequencer_thread->error));
#ifdef AGS_DEBUG
    g_message("ags_sequencer_record");
#endif
  }

  if((AGS_THREAD_SINGLE_LOOP & (g_atomic_int_get(&(thread->flags)))) == 0){
    AGS_THREAD_CLASS(ags_sequencer_thread_parent_class)->start(thread);
  }
}

void
ags_sequencer_thread_run(AgsThread *thread)
{
  AgsSequencer *sequencer;
  AgsSequencerThread *sequencer_thread;
  long delay;
  GError *error;

  sequencer_thread = AGS_SEQUENCER_THREAD(thread);

  sequencer = AGS_SEQUENCER(sequencer_thread->sequencer);

  /* real-time setup */
  if((AGS_THREAD_RT_SETUP & (g_atomic_int_get(&(thread->flags)))) == 0){
    struct sched_param param;
    
    /* Declare ourself as a real time task */
    param.sched_priority = AGS_RT_PRIORITY;
      
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
      perror("sched_setscheduler failed");
    }

    g_atomic_int_or(&(thread->flags),
		    AGS_THREAD_RT_SETUP);
  }

  if(ags_sequencer_is_recording(sequencer)){
    error = NULL;
    ags_sequencer_record(sequencer,
			 &error);
    
    if(error != NULL){
      g_warning("ags_sequencer_thread - %s", error->message);
    }
  }

  /* notify about time exceeded */
  ags_sequencer_thread_interval_timeout(sequencer_thread);
}

void
ags_sequencer_thread_stop(AgsThread *thread)
{
  AgsSequencer *sequencer;
  AgsSequencerThread *sequencer_thread;

  sequencer_thread = AGS_SEQUENCER_THREAD(thread);

  sequencer = AGS_SEQUENCER(sequencer_thread->sequencer);

  AGS_THREAD_CLASS(ags_sequencer_thread_parent_class)->stop(thread);
  ags_sequencer_stop(sequencer);
}

void
ags_sequencer_stopped_all_callback(AgsAudioLoop *audio_loop,
				   AgsSequencerThread *sequencer_thread)
{
  AgsSequencer *sequencer;
  
  sequencer = AGS_SEQUENCER(sequencer_thread->sequencer);
  
  if(ags_sequencer_is_recording(sequencer)){
    ags_thread_stop((AgsThread *) sequencer_thread);
  }
}

void
ags_sequencer_thread_interval_timeout(AgsSequencerThread *sequencer_thread)
{
  g_return_if_fail(AGS_IS_SEQUENCER_THREAD(sequencer_thread));

  g_object_ref((GObject *) sequencer_thread);
  g_signal_emit(G_OBJECT(sequencer_thread),
		sequencer_thread_signals[INTERVAL_TIMEOUT], 0);
  g_object_unref((GObject *) sequencer_thread);
}

/**
 * ags_sequencer_thread_find_sequencer:
 * @sequencer_thread: the #AgsSequencerThread
 * @sequencer: the #AgsSequencer to find
 * 
 * Returns: the matching #AgsSequencerThread, if not
 * found %NULL.
 * 
 * Since: 0.7.119
 */
AgsSequencerThread*
ags_sequencer_thread_find_sequencer(AgsSequencerThread *sequencer_thread,
				    GObject *sequencer)
{
  if(sequencer_thread == NULL ||
     !AGS_IS_SEQUENCER_THREAD(sequencer_thread)){
    return(NULL);
  }
  
  while(sequencer_thread != NULL){
    if(AGS_IS_SEQUENCER_THREAD(sequencer_thread) &&
       sequencer_thread->sequencer == sequencer){
      return(sequencer_thread);
    }
    
    sequencer_thread = g_atomic_pointer_get(&(((AgsThread *) sequencer_thread)->next));
  }

  return(NULL);
}

/**
 * ags_sequencer_thread_new:
 * @sequencer: the #AgsSequencer
 *
 * Create a new #AgsSequencerThread.
 *
 * Returns: the new #AgsSequencerThread
 *
 * Since: 0.7.0
 */
AgsSequencerThread*
ags_sequencer_thread_new(GObject *sequencer)
{
  AgsSequencerThread *sequencer_thread;

  sequencer_thread = (AgsSequencerThread *) g_object_new(AGS_TYPE_SEQUENCER_THREAD,
							 "sequencer", sequencer,
							 NULL);

  return(sequencer_thread);
}
