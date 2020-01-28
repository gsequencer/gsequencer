/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/audio/thread/ags_soundcard_thread.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_devin.h>

#include <ags/audio/jack/ags_jack_devout.h>
#include <ags/audio/jack/ags_jack_devin.h>

#include <ags/audio/pulse/ags_pulse_devout.h>
#include <ags/audio/pulse/ags_pulse_devin.h>

#include <ags/audio/wasapi/ags_wasapi_devout.h>
#include <ags/audio/wasapi/ags_wasapi_devin.h>

#include <ags/audio/core-audio/ags_core_audio_devout.h>
#include <ags/audio/core-audio/ags_core_audio_devin.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/i18n.h>

void ags_soundcard_thread_class_init(AgsSoundcardThreadClass *soundcard_thread);
void ags_soundcard_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_soundcard_thread_init(AgsSoundcardThread *soundcard_thread);
void ags_soundcard_thread_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_soundcard_thread_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_soundcard_thread_connect(AgsConnectable *connectable);
void ags_soundcard_thread_disconnect(AgsConnectable *connectable);
void ags_soundcard_thread_dispose(GObject *gobject);
void ags_soundcard_thread_finalize(GObject *gobject);

void ags_soundcard_thread_start(AgsThread *thread);
void ags_soundcard_thread_run(AgsThread *thread);
void ags_soundcard_thread_stop(AgsThread *thread);

void ags_soundcard_thread_stopped_all_callback(AgsAudioLoop *audio_loop,
					       AgsSoundcardThread *soundcard_thread);

/**
 * SECTION:ags_soundcard_thread
 * @short_description: soundcard thread
 * @title: AgsSoundcardThread
 * @section_id:
 * @include: ags/audio/thread/ags_soundcard_thread.h
 *
 * The #AgsSoundcardThread acts as audio output thread to soundcard.
 */

static gpointer ags_soundcard_thread_parent_class = NULL;
static AgsConnectableInterface *ags_soundcard_thread_parent_connectable_interface;

enum{
  PROP_0,
  PROP_SOUNDCARD,
  PROP_SOUNDCARD_CAPABILITY,
};

GType
ags_soundcard_thread_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_soundcard_thread = 0;

    static const GTypeInfo ags_soundcard_thread_info = {
      sizeof (AgsSoundcardThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_soundcard_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSoundcardThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_soundcard_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_soundcard_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_soundcard_thread = g_type_register_static(AGS_TYPE_THREAD,
						       "AgsSoundcardThread",
						       &ags_soundcard_thread_info,
						       0);
    
    g_type_add_interface_static(ags_type_soundcard_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_soundcard_thread);
  }

  return g_define_type_id__volatile;
}

void
ags_soundcard_thread_class_init(AgsSoundcardThreadClass *soundcard_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;
  GParamSpec *param_spec;
  
  ags_soundcard_thread_parent_class = g_type_class_peek_parent(soundcard_thread);

  /* GObject */
  gobject = (GObjectClass *) soundcard_thread;

  gobject->set_property = ags_soundcard_thread_set_property;
  gobject->get_property = ags_soundcard_thread_get_property;

  gobject->dispose = ags_soundcard_thread_dispose;
  gobject->finalize = ags_soundcard_thread_finalize;

  /**
   * AgsSoundcardThread:soundcard:
   *
   * The assigned #AgsSoundcard.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("soundcard assigned to"),
				   i18n_pspec("The AgsSoundcard it is assigned to"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /**
   * AgsSoundcardThread:soundcard-capability:
   *
   * The soundcard capability.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_uint("soundcard-capability",
				  i18n_pspec("soundcard capability"),
				  i18n_pspec("The soundcard capability"),
				  0,
				  G_MAXINT32,
				  0,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD_CAPABILITY,
				  param_spec);

  /* AgsThread */
  thread = (AgsThreadClass *) soundcard_thread;

  thread->start = ags_soundcard_thread_start;
  thread->run = ags_soundcard_thread_run;
  thread->stop = ags_soundcard_thread_stop;
}

void
ags_soundcard_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_soundcard_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_soundcard_thread_connect;
  connectable->disconnect = ags_soundcard_thread_disconnect;
}

void
ags_soundcard_thread_init(AgsSoundcardThread *soundcard_thread)
{
  AgsThread *thread;

  AgsConfig *config;

  gdouble frequency;
  guint samplerate;
  guint buffer_size;
  
  thread = (AgsThread *) soundcard_thread;

  ags_thread_set_flags(thread, (AGS_THREAD_START_SYNCED_FREQ |
				AGS_THREAD_INTERMEDIATE_POST_SYNC));
  
  //  g_atomic_int_or(&(thread->flags),
  //		  AGS_THREAD_TIMING);

  config = ags_config_get_instance();
  
  samplerate = (guint) ags_soundcard_helper_config_get_samplerate(config);
  buffer_size = (guint) ags_soundcard_helper_config_get_buffer_size(config);

  frequency = ((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;

  g_object_set(thread,
	       "frequency", frequency,
	       NULL);

  /*  */
  soundcard_thread->soundcard_capability = 0;

  soundcard_thread->soundcard = NULL;
  soundcard_thread->error = NULL;
}

void
ags_soundcard_thread_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsSoundcardThread *soundcard_thread;

  soundcard_thread = AGS_SOUNDCARD_THREAD(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      guint samplerate;
      guint buffer_size;

      soundcard = (GObject *) g_value_get_object(value);

      if(soundcard_thread->soundcard != NULL){
	g_object_unref(G_OBJECT(soundcard_thread->soundcard));
      }

      if(soundcard != NULL){
	g_object_ref(G_OBJECT(soundcard));

	ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
				  NULL,
				  &samplerate,
				  &buffer_size,
				  NULL);
	
	g_object_set(soundcard_thread,
		     "frequency", ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK,
		     NULL);

	/* playback */
	if(AGS_IS_DEVOUT(soundcard)){
	  ags_thread_set_flags(soundcard_thread, AGS_THREAD_INTERMEDIATE_POST_SYNC);
	}else if(AGS_IS_WASAPI_DEVOUT(soundcard)){
	  ags_thread_set_flags(soundcard_thread, AGS_THREAD_INTERMEDIATE_POST_SYNC);
	}else if(AGS_IS_JACK_DEVOUT(soundcard) ||
		 AGS_IS_PULSE_DEVOUT(soundcard)){
	  ags_thread_set_flags(soundcard_thread, AGS_THREAD_INTERMEDIATE_POST_SYNC);
	}else if(AGS_IS_CORE_AUDIO_DEVOUT(soundcard)){
	  ags_thread_set_flags(soundcard_thread, AGS_THREAD_INTERMEDIATE_POST_SYNC);
	}

	/* capture */
	if(AGS_IS_DEVIN(soundcard)){
	  ags_thread_set_flags(soundcard_thread, AGS_THREAD_INTERMEDIATE_PRE_SYNC);
	}else if(AGS_IS_WASAPI_DEVIN(soundcard)){
	  ags_thread_set_flags(soundcard_thread, AGS_THREAD_INTERMEDIATE_PRE_SYNC);
	}else if(AGS_IS_JACK_DEVIN(soundcard) ||
		 AGS_IS_PULSE_DEVIN(soundcard)){
	  ags_thread_set_flags(soundcard_thread, AGS_THREAD_INTERMEDIATE_PRE_SYNC);
	}else if(AGS_IS_CORE_AUDIO_DEVIN(soundcard)){
	  ags_thread_set_flags(soundcard_thread, AGS_THREAD_INTERMEDIATE_PRE_SYNC);
	}

	/* duplex */
	//TODO:JK: implement me
      }

      soundcard_thread->soundcard = G_OBJECT(soundcard);
    }
    break;
  case PROP_SOUNDCARD_CAPABILITY:
    {
      soundcard_thread->soundcard_capability = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_soundcard_thread_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsSoundcardThread *soundcard_thread;

  soundcard_thread = AGS_SOUNDCARD_THREAD(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, G_OBJECT(soundcard_thread->soundcard));
    }
    break;
  case PROP_SOUNDCARD_CAPABILITY:
    {
      g_value_set_uint(value,
		       soundcard_thread->soundcard_capability);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_soundcard_thread_connect(AgsConnectable *connectable)
{
  AgsThread *audio_loop, *soundcard_thread;

  soundcard_thread = AGS_THREAD(connectable);

  if(ags_thread_test_flags(soundcard_thread, AGS_THREAD_CONNECTED)){
    return;
  }  

  ags_soundcard_thread_parent_connectable_interface->connect(connectable);

  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(ags_application_context_get_instance()));
   
  g_signal_connect((GObject *) audio_loop, "stopped-all",
		   G_CALLBACK(ags_soundcard_thread_stopped_all_callback), soundcard_thread);    

  g_object_unref(audio_loop);
}

void
ags_soundcard_thread_disconnect(AgsConnectable *connectable)
{
  ags_soundcard_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_soundcard_thread_dispose(GObject *gobject)
{
  AgsSoundcardThread *soundcard_thread;

  soundcard_thread = AGS_SOUNDCARD_THREAD(gobject);

  /* soundcard */
  if(soundcard_thread->soundcard != NULL){
    g_object_unref(soundcard_thread->soundcard);

    soundcard_thread->soundcard = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_soundcard_thread_parent_class)->dispose(gobject);
}

void
ags_soundcard_thread_finalize(GObject *gobject)
{
  AgsSoundcardThread *soundcard_thread;

  soundcard_thread = AGS_SOUNDCARD_THREAD(gobject);

  /* soundcard */
  if(soundcard_thread->soundcard != NULL){
    g_object_unref(soundcard_thread->soundcard);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_soundcard_thread_parent_class)->finalize(gobject);
}

void
ags_soundcard_thread_start(AgsThread *thread)
{
  AgsSoundcardThread *soundcard_thread;

  soundcard_thread = AGS_SOUNDCARD_THREAD(thread);

  /* disable timing */
  ags_thread_unset_flags(thread, AGS_THREAD_TIME_ACCOUNTING);
    
  AGS_THREAD_CLASS(ags_soundcard_thread_parent_class)->start(thread);
}

void
ags_soundcard_thread_run(AgsThread *thread)
{
  AgsSoundcardThread *soundcard_thread;

  GObject *soundcard;
  
  gboolean is_playing, is_recording;
  
  GError *error;

  soundcard_thread = AGS_SOUNDCARD_THREAD(thread);

  soundcard = soundcard_thread->soundcard;

  /* real-time setup */
#ifdef AGS_WITH_RT
  if(!ags_thread_test_status_flags(thread, AGS_THREAD_STATUS_RT_SETUP)){
    AgsPriority *priority;
    
    struct sched_param param;

    gchar *str;

    priority = ags_priority_get_instance();
    
    /* Declare ourself as a real time task */
    param.sched_priority = 45;

    str = ags_priority_get_value(priority,
				 AGS_PRIORITY_RT_THREAD,
				 AGS_PRIORITY_KEY_AUDIO);

    if(str != NULL){
      param.sched_priority = (int) g_ascii_strtoull(str,
						    NULL,
						    10);
    }
      
    if(str == NULL ||
       ((!g_ascii_strncasecmp(str,
			      "0",
			      2)) != TRUE)){
      if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
	perror("sched_setscheduler failed");
      }
    }

    g_free(str);
    
    ags_thread_set_status_flags(thread, AGS_THREAD_STATUS_RT_SETUP);
  }
#endif

  /* playback */
  if((AGS_SOUNDCARD_CAPABILITY_PLAYBACK & (soundcard_thread->soundcard_capability)) != 0){
    is_playing = ags_soundcard_is_playing(AGS_SOUNDCARD(soundcard));
  
    if(is_playing){
      error = NULL;
      ags_soundcard_play(AGS_SOUNDCARD(soundcard),
			 &error);

      if(error != NULL){
	//TODO:JK: implement me

	g_warning("%s",
		  error->message);

	g_error_free(error);
      }
    }
  }

  /* capture */
  if((AGS_SOUNDCARD_CAPABILITY_CAPTURE & (soundcard_thread->soundcard_capability)) != 0){
    is_recording = ags_soundcard_is_recording(AGS_SOUNDCARD(soundcard));
  
    if(is_recording){
      error = NULL;
      ags_soundcard_record(AGS_SOUNDCARD(soundcard),
			   &error);

      if(error != NULL){
	//TODO:JK: implement me

	g_warning("%s",
		  error->message);

	g_error_free(error);
      }
    }
  }

  /* duplex */
  if((AGS_SOUNDCARD_CAPABILITY_DUPLEX & (soundcard_thread->soundcard_capability)) != 0){
    //TODO:JK: implement me
  }
}

void
ags_soundcard_thread_stop(AgsThread *thread)
{
  AgsSoundcardThread *soundcard_thread;
  
  GObject *soundcard;
    
  soundcard_thread = AGS_SOUNDCARD_THREAD(thread);

  soundcard = soundcard_thread->soundcard;
  
  /* stop thread and soundcard */
  if(AGS_IS_WASAPI_DEVOUT(soundcard)){
    if((AGS_WASAPI_DEVOUT_SHUTDOWN & (AGS_WASAPI_DEVOUT(soundcard)->flags)) == 0){
      ags_soundcard_stop(AGS_SOUNDCARD(soundcard));
    }else{
      /* reset flags */
      AGS_WASAPI_DEVOUT(soundcard)->flags &= (~(AGS_WASAPI_DEVOUT_BUFFER0 |
						AGS_WASAPI_DEVOUT_BUFFER1 |
						AGS_WASAPI_DEVOUT_BUFFER2 |
						AGS_WASAPI_DEVOUT_BUFFER3 |
						AGS_WASAPI_DEVOUT_BUFFER4 |
						AGS_WASAPI_DEVOUT_BUFFER5 |
						AGS_WASAPI_DEVOUT_BUFFER6 |
						AGS_WASAPI_DEVOUT_BUFFER7 |
						AGS_WASAPI_DEVOUT_PLAY |
						AGS_WASAPI_DEVOUT_INITIALIZED |
						AGS_WASAPI_DEVOUT_SHUTDOWN));
      
      AGS_THREAD_CLASS(ags_soundcard_thread_parent_class)->stop(thread);

      g_message("WASAPI thread stopped");
    }
  }else if(AGS_IS_WASAPI_DEVIN(soundcard)){
    if((AGS_WASAPI_DEVIN_SHUTDOWN & (AGS_WASAPI_DEVIN(soundcard)->flags)) == 0){
      ags_soundcard_stop(AGS_SOUNDCARD(soundcard));
    }else{
      /* reset flags */
      AGS_WASAPI_DEVIN(soundcard)->flags &= (~(AGS_WASAPI_DEVIN_BUFFER0 |
					       AGS_WASAPI_DEVIN_BUFFER1 |
					       AGS_WASAPI_DEVIN_BUFFER2 |
					       AGS_WASAPI_DEVIN_BUFFER3 |
					       AGS_WASAPI_DEVIN_BUFFER4 |
					       AGS_WASAPI_DEVIN_BUFFER5 |
					       AGS_WASAPI_DEVIN_BUFFER6 |
					       AGS_WASAPI_DEVIN_BUFFER7 |
					       AGS_WASAPI_DEVIN_RECORD |
					       AGS_WASAPI_DEVIN_INITIALIZED |
					       AGS_WASAPI_DEVIN_SHUTDOWN));
      
      AGS_THREAD_CLASS(ags_soundcard_thread_parent_class)->stop(thread);

      g_message("WASAPI thread stopped");
    }
  }else{
    AGS_THREAD_CLASS(ags_soundcard_thread_parent_class)->stop(thread);
  
    //FIXME:JK: is this safe?
    ags_soundcard_stop(AGS_SOUNDCARD(soundcard));
  }  
  
  ags_thread_set_flags(thread, AGS_THREAD_TIME_ACCOUNTING);
}

void
ags_soundcard_thread_stopped_all_callback(AgsAudioLoop *audio_loop,
					  AgsSoundcardThread *soundcard_thread)
{
  AgsSoundcard *soundcard;
  
  soundcard = AGS_SOUNDCARD(soundcard_thread->soundcard);

  if(ags_soundcard_is_playing(soundcard)){
    ags_thread_stop((AgsThread *) soundcard_thread);
  }
}

/**
 * ags_soundcard_thread_find_soundcard:
 * @soundcard_thread: the #AgsSoundcardThread
 * @soundcard: the #AgsSoundcard to find
 * 
 * Find @soundcard as sibling of @soundcard_thread, if it was found on the returned
 * thread should be called g_object_unref().
 * 
 * Returns: (transfer full): the matching #AgsSoundcardThread, if not
 * found %NULL.
 * 
 * Since: 3.0.0
 */
AgsSoundcardThread*
ags_soundcard_thread_find_soundcard(AgsSoundcardThread *soundcard_thread,
				    GObject *soundcard)
{
  AgsThread *thread, *next_thread;
  
  if(!AGS_IS_SOUNDCARD_THREAD(soundcard_thread)){
    return(NULL);
  }

  thread = soundcard_thread;
  g_object_ref(thread);
  
  while(thread != NULL){
    if(AGS_IS_SOUNDCARD_THREAD(thread) &&
       AGS_SOUNDCARD_THREAD(thread)->soundcard == soundcard){
      return(thread);
    }

    /* iterate */
    next_thread = ags_thread_next(thread);

    g_object_unref(thread);
    
    thread = next_thread;
  }

  return(NULL);
}

/**
 * ags_soundcard_thread_new:
 * @soundcard: the #AgsSoundcard
 * @soundcard_capability: see #AgsSoundcardCapability-enum
 *
 * Create a new instance of #AgsSoundcardThread.
 *
 * Returns: the new #AgsSoundcardThread
 *
 * Since: 3.0.0
 */
AgsSoundcardThread*
ags_soundcard_thread_new(GObject *soundcard,
			 guint soundcard_capability)
{
  AgsSoundcardThread *soundcard_thread;

  soundcard_thread = (AgsSoundcardThread *) g_object_new(AGS_TYPE_SOUNDCARD_THREAD,
							 "soundcard", soundcard,
							 "soundcard-capability", soundcard_capability,
							 NULL);


  return(soundcard_thread);
}
