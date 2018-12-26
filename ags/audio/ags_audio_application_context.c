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

#include <ags/audio/ags_audio_application_context.h>

#include <ags/libags.h>

#include <ags/plugin/ags_ladspa_manager.h>
#include <ags/plugin/ags_dssi_manager.h>
#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2_worker_manager.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_devin.h>
#include <ags/audio/ags_midiin.h>
#include <ags/audio/ags_generic_recall_channel_run.h>
#include <ags/audio/ags_recall_ladspa.h>
#include <ags/audio/ags_recall_ladspa_run.h>
#include <ags/audio/ags_recall_lv2.h>
#include <ags/audio/ags_recall_lv2_run.h>
#include <ags/audio/ags_recall_dssi.h>
#include <ags/audio/ags_recall_dssi_run.h>

#include <ags/audio/core-audio/ags_core_audio_midiin.h>
#include <ags/audio/core-audio/ags_core_audio_server.h>
#include <ags/audio/core-audio/ags_core_audio_client.h>
#include <ags/audio/core-audio/ags_core_audio_port.h>
#include <ags/audio/core-audio/ags_core_audio_devout.h>
#include <ags/audio/core-audio/ags_core_audio_devin.h>

#include <ags/audio/jack/ags_jack_midiin.h>
#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_client.h>
#include <ags/audio/jack/ags_jack_port.h>
#include <ags/audio/jack/ags_jack_devout.h>
#include <ags/audio/jack/ags_jack_devin.h>

#include <ags/audio/pulse/ags_pulse_server.h>
#include <ags/audio/pulse/ags_pulse_client.h>
#include <ags/audio/pulse/ags_pulse_port.h>
#include <ags/audio/pulse/ags_pulse_devout.h>
#include <ags/audio/pulse/ags_pulse_devin.h>

#include <ags/audio/recall/ags_play_audio.h>
#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_channel_run.h>
#include <ags/audio/recall/ags_play_channel_run_master.h>
#include <ags/audio/recall/ags_stream_channel.h>
#include <ags/audio/recall/ags_stream_channel_run.h>
#include <ags/audio/recall/ags_loop_channel.h>
#include <ags/audio/recall/ags_loop_channel_run.h>
#include <ags/audio/recall/ags_copy_channel.h>
#include <ags/audio/recall/ags_copy_channel_run.h>
#include <ags/audio/recall/ags_volume_channel.h>
#include <ags/audio/recall/ags_volume_channel_run.h>
#include <ags/audio/recall/ags_peak_channel.h>
#include <ags/audio/recall/ags_peak_channel_run.h>
#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_audio.h>
#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>
#include <ags/audio/recall/ags_buffer_channel.h>
#include <ags/audio/recall/ags_buffer_channel_run.h>
#include <ags/audio/recall/ags_play_notation_audio.h>
#include <ags/audio/recall/ags_play_notation_audio_run.h>

#include <ags/audio/task/ags_notify_soundcard.h>

#include <ags/audio/file/ags_audio_file_xml.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>
#include <ags/audio/thread/ags_sequencer_thread.h>
#include <ags/audio/thread/ags_export_thread.h>

#include <sys/types.h>
#include <pwd.h>

#include <stdbool.h>

#include <signal.h>

#include <ags/i18n.h>

void ags_audio_application_context_signal_handler(int signr);
static void ags_audio_application_context_signal_cleanup();

void ags_audio_application_context_class_init(AgsAudioApplicationContextClass *audio_application_context);
void ags_audio_application_context_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_application_context_concurrency_provider_interface_init(AgsConcurrencyProviderInterface *concurrency_provider);
void ags_audio_application_context_sound_provider_interface_init(AgsSoundProviderInterface *sound_provider);
void ags_audio_application_context_init(AgsAudioApplicationContext *audio_application_context);
void ags_audio_application_context_set_property(GObject *gobject,
						guint prop_id,
						const GValue *value,
						GParamSpec *param_spec);
void ags_audio_application_context_get_property(GObject *gobject,
						guint prop_id,
						GValue *value,
						GParamSpec *param_spec);
void ags_audio_application_context_dispose(GObject *gobject);
void ags_audio_application_context_finalize(GObject *gobject);

void ags_audio_application_context_connect(AgsConnectable *connectable);
void ags_audio_application_context_disconnect(AgsConnectable *connectable);

AgsThread* ags_audio_application_context_get_main_loop(AgsConcurrencyProvider *concurrency_provider);
AgsThread* ags_audio_application_context_get_task_thread(AgsConcurrencyProvider *concurrency_provider);
AgsThreadPool* ags_audio_application_context_get_thread_pool(AgsConcurrencyProvider *concurrency_provider);
GList* ags_audio_application_context_get_worker(AgsConcurrencyProvider *concurrency_provider);
void ags_audio_application_context_set_worker(AgsConcurrencyProvider *concurrency_provider,
					      GList *worker);

GObject* ags_audio_application_context_get_default_soundcard(AgsSoundProvider *sound_provider);
void ags_audio_application_context_set_default_soundcard(AgsSoundProvider *sound_provider,
							 GObject *soundcard);
GObject* ags_audio_application_context_get_default_soundcard_thread(AgsSoundProvider *sound_provider);
void ags_audio_application_context_set_default_soundcard_thread(AgsSoundProvider *sound_provider,
								GObject *soundcard_thread);
GList* ags_audio_application_context_get_soundcard(AgsSoundProvider *sound_provider);
void ags_audio_application_context_set_soundcard(AgsSoundProvider *sound_provider,
						 GList *soundcard);
GList* ags_audio_application_context_get_sequencer(AgsSoundProvider *sound_provider);
void ags_audio_application_context_set_sequencer(AgsSoundProvider *sound_provider,
						 GList *sequencer);
GList* ags_audio_application_context_get_sound_server(AgsSoundProvider *sound_provider);
GList* ags_audio_application_context_get_audio(AgsSoundProvider *sound_provider);
void ags_audio_application_context_set_audio(AgsSoundProvider *sound_provider,
					     GList *soundcard);
GList* ags_audio_application_context_get_osc_server(AgsSoundProvider *sound_provider);
void ags_audio_application_context_set_osc_server(AgsSoundProvider *sound_provider,
						  GList *soundcard);

void ags_audio_application_context_load_config(AgsApplicationContext *application_context);

void ags_audio_application_context_prepare(AgsApplicationContext *application_context);
void ags_audio_application_context_setup(AgsApplicationContext *application_context);

void ags_audio_application_context_register_types(AgsApplicationContext *application_context);

void ags_audio_application_context_read(AgsFile *file, xmlNode *node, GObject **application_context);
xmlNode* ags_audio_application_context_write(AgsFile *file, xmlNode *parent, GObject *application_context);

void ags_audio_application_context_quit(AgsApplicationContext *application_context);

void ags_audio_application_context_set_value_callback(AgsConfig *config, gchar *group, gchar *key, gchar *value,
						      AgsAudioApplicationContext *audio_application_context);

/**
 * SECTION:ags_audio_application_context
 * @short_description: audio application context
 * @title: AgsAudioApplicationContext
 * @section_id:
 * @include: ags/audio/ags_audio_application_context.h
 *
 * The #AgsAudioApplicationContext provides you sound processing, output and capturing.
 */

enum{
  PROP_0,
  PROP_SOUNDCARD,
};

static gpointer ags_audio_application_context_parent_class = NULL;
static AgsConnectableInterface* ags_audio_application_context_parent_connectable_interface;

extern AgsApplicationContext *ags_application_context;

//TODO:JK: implement get functions
#ifndef AGS_USE_TIMER
static sigset_t ags_wait_mask;
struct sigaction ags_sigact;
#else
static sigset_t ags_timer_mask;

struct sigaction ags_sigact_timer;

struct sigevent ags_sev_timer;
struct itimerspec its;
#endif

GType
ags_audio_application_context_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_audio_application_context = 0;

    static const GTypeInfo ags_audio_application_context_info = {
      sizeof (AgsAudioApplicationContextClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_application_context_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioApplicationContext),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_application_context_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_application_context_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_concurrency_provider_interface_info = {
      (GInterfaceInitFunc) ags_audio_application_context_concurrency_provider_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_sound_provider_interface_info = {
      (GInterfaceInitFunc) ags_audio_application_context_sound_provider_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_application_context = g_type_register_static(AGS_TYPE_APPLICATION_CONTEXT,
								"AgsAudioApplicationContext",
								&ags_audio_application_context_info,
								0);

    g_type_add_interface_static(ags_type_audio_application_context,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_audio_application_context,
				AGS_TYPE_CONCURRENCY_PROVIDER,
				&ags_concurrency_provider_interface_info);

    g_type_add_interface_static(ags_type_audio_application_context,
				AGS_TYPE_SOUND_PROVIDER,
				&ags_sound_provider_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_audio_application_context);
  }

  return g_define_type_id__volatile;
}

#ifndef AGS_USE_TIMER
void
ags_audio_application_context_signal_handler(int signr)
{
  if(signr == SIGINT){
    //TODO:JK: do backup
    
    exit(-1);
  }else{
    sigemptyset(&(ags_sigact.sa_mask));
    
    //    if(signr == AGS_ASYNC_QUEUE_SIGNAL_HIGH){
    // pthread_yield();
    //    }
  }
}

static void
ags_audio_application_context_signal_cleanup()
{
  sigemptyset(&(ags_sigact.sa_mask));
}
#endif

#ifdef AGS_USE_TIMER
void
ags_audio_application_context_signal_handler_timer(int sig, siginfo_t *si, void *uc)
{
  pthread_mutex_lock(AGS_THREAD(ags_application_context->main_loop)->timer_mutex);

  g_atomic_int_set(&(AGS_THREAD(ags_application_context->main_loop)->timer_expired),
		   TRUE);
  
  if(AGS_THREAD(ags_application_context->main_loop)->timer_wait){
    pthread_cond_signal(AGS_THREAD(ags_application_context->main_loop)->timer_cond);
  }
    
  pthread_mutex_unlock(AGS_THREAD(ags_application_context->main_loop)->timer_mutex);
  //  signal(sig, SIG_IGN);
}
#endif

void
ags_audio_application_context_class_init(AgsAudioApplicationContextClass *audio_application_context)
{
  GObjectClass *gobject;
  AgsApplicationContextClass *application_context;
  GParamSpec *param_spec;
  
  ags_audio_application_context_parent_class = g_type_class_peek_parent(audio_application_context);

  /* GObjectClass */
  gobject = (GObjectClass *) audio_application_context;

  gobject->set_property = ags_audio_application_context_set_property;
  gobject->get_property = ags_audio_application_context_get_property;

  gobject->dispose = ags_audio_application_context_dispose;
  gobject->finalize = ags_audio_application_context_finalize;
  
  /**
   * AgsAudioApplicationContext:soundcard:
   *
   * The assigned soundcard.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("soundcard",
				    i18n_pspec("soundcard of audio application context"),
				    i18n_pspec("The soundcard which this audio application context assigned to"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /* AgsAudioApplicationContextClass */
  application_context = (AgsApplicationContextClass *) audio_application_context;
  
  application_context->load_config = ags_audio_application_context_load_config;

  application_context->prepare = ags_audio_application_context_prepare;
  application_context->setup = ags_audio_application_context_setup;

  application_context->register_types = ags_audio_application_context_register_types;

  application_context->quit = ags_audio_application_context_quit;

  application_context->read = ags_audio_application_context_read;
  application_context->write = ags_audio_application_context_write;
}

void
ags_audio_application_context_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_audio_application_context_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_audio_application_context_connect;
  connectable->disconnect = ags_audio_application_context_disconnect;
}

void
ags_audio_application_context_concurrency_provider_interface_init(AgsConcurrencyProviderInterface *concurrency_provider)
{
  concurrency_provider->get_main_loop = ags_audio_application_context_get_main_loop;
  concurrency_provider->get_task_thread = ags_audio_application_context_get_task_thread;
  concurrency_provider->get_thread_pool = ags_audio_application_context_get_thread_pool;
  concurrency_provider->get_worker = ags_audio_application_context_get_worker;
  concurrency_provider->set_worker = ags_audio_application_context_set_worker;
}

void
ags_audio_application_context_sound_provider_interface_init(AgsSoundProviderInterface *sound_provider)
{
  sound_provider->get_default_soundcard = ags_audio_application_context_get_default_soundcard;
  sound_provider->set_default_soundcard = ags_audio_application_context_set_default_soundcard;

  sound_provider->get_default_soundcard_thread = ags_audio_application_context_get_default_soundcard_thread;
  sound_provider->set_default_soundcard_thread = ags_audio_application_context_set_default_soundcard_thread;

  sound_provider->get_soundcard = ags_audio_application_context_get_soundcard;
  sound_provider->set_soundcard = ags_audio_application_context_set_soundcard;

  sound_provider->get_sequencer = ags_audio_application_context_get_sequencer;
  sound_provider->set_sequencer = ags_audio_application_context_set_sequencer;

  sound_provider->get_sound_server = ags_audio_application_context_get_sound_server;

  sound_provider->get_audio = ags_audio_application_context_get_audio;
  sound_provider->set_audio = ags_audio_application_context_set_audio;

  sound_provider->get_osc_server = ags_audio_application_context_get_osc_server;
  sound_provider->set_osc_server = ags_audio_application_context_set_osc_server;
}

void
ags_audio_application_context_init(AgsAudioApplicationContext *audio_application_context)
{
  AgsConfig *config;

  if(ags_application_context == NULL){
    ags_application_context = audio_application_context;
  }

  /* set config */
  config = ags_config_get_instance();
  AGS_APPLICATION_CONTEXT(audio_application_context)->config = config;
  g_object_ref(config);
  g_object_set(config,
	       "application-context", audio_application_context,
	       NULL);

  AGS_APPLICATION_CONTEXT(audio_application_context)->log = (GObject *) ags_log_get_instance();
  g_object_ref(AGS_APPLICATION_CONTEXT(audio_application_context)->log);

  audio_application_context->thread_pool = NULL;

  audio_application_context->polling_thread = NULL;

  audio_application_context->worker = NULL;
  
  audio_application_context->default_soundcard = NULL;

  audio_application_context->default_soundcard_thread = NULL;
  audio_application_context->default_export_thread = NULL;

  audio_application_context->autosave_thread = NULL;

  audio_application_context->server = NULL;
  
  audio_application_context->soundcard = NULL;
  audio_application_context->sequencer = NULL;

  audio_application_context->sound_server = NULL;
  
  audio_application_context->audio = NULL;

  audio_application_context->osc_server = NULL;
}

void
ags_audio_application_context_set_property(GObject *gobject,
					   guint prop_id,
					   const GValue *value,
					   GParamSpec *param_spec)
{
  AgsAudioApplicationContext *audio_application_context;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;
      
      soundcard = (GObject *) g_value_get_pointer(value);

      if(soundcard == NULL){
	return;
      }
      
      if(g_list_find(audio_application_context->soundcard, soundcard) == NULL){
	g_object_ref(G_OBJECT(soundcard));

	audio_application_context->soundcard = g_list_prepend(audio_application_context->soundcard,
							      soundcard);
      }
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_application_context_get_property(GObject *gobject,
					   guint prop_id,
					   GValue *value,
					   GParamSpec *param_spec)
{
  AgsAudioApplicationContext *audio_application_context;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_pointer(value, audio_application_context->soundcard);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_application_context_dispose(GObject *gobject)
{
  AgsAudioApplicationContext *audio_application_context;

  GList *list;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(gobject);

  /* thread pool */
  if(audio_application_context->thread_pool != NULL){
    g_object_unref(audio_application_context->thread_pool);
    
    audio_application_context->thread_pool = NULL;
  }

  /* polling thread */
  if(audio_application_context->polling_thread != NULL){
    g_object_unref(audio_application_context->polling_thread);

    audio_application_context->polling_thread = NULL;
  }

  /* worker thread */
  if(audio_application_context->worker != NULL){
    list = audio_application_context->worker;

    while(list != NULL){
      g_object_run_dispose(list->data);
      
      list = list->next;
    }
    
    g_list_free_full(audio_application_context->worker,
		     g_object_unref);

    audio_application_context->worker = NULL;
  }

  /* default soundcard */
  if(audio_application_context->default_soundcard != NULL){
    g_object_unref(audio_application_context->default_soundcard);

    audio_application_context->default_soundcard = NULL;
  }

  /* soundcard and export thread */
  if(audio_application_context->default_soundcard_thread != NULL){
    g_object_unref(audio_application_context->default_soundcard_thread);

    audio_application_context->default_soundcard_thread = NULL;
  }

  if(audio_application_context->default_export_thread != NULL){
    g_object_unref(audio_application_context->default_export_thread);

    audio_application_context->default_export_thread = NULL;
  }

  /* server */
  if(audio_application_context->server != NULL){
    g_object_set(audio_application_context->server,
		 "application-context", NULL,
		 NULL);
    
    g_object_unref(audio_application_context->server);

    audio_application_context->server = NULL;
  }

  /* soundcard and sequencer */
  if(audio_application_context->soundcard != NULL){
    list = audio_application_context->soundcard;

    while(list != NULL){
      g_object_set(list->data,
		   "application-context", NULL,
		   NULL);

      list = list->next;
    }
    
    g_list_free_full(audio_application_context->soundcard,
		     g_object_unref);

    audio_application_context->soundcard = NULL;
  }

  if(audio_application_context->sequencer != NULL){
    list = audio_application_context->sequencer;

    while(list != NULL){
      g_object_set(list->data,
		   "application-context", NULL,
		   NULL);

      list = list->next;
    }

    g_list_free_full(audio_application_context->sequencer,
		     g_object_unref);

    audio_application_context->sequencer = NULL;
  }

  /* sound server */
  if(audio_application_context->sound_server != NULL){
    list = audio_application_context->sound_server;

    while(list != NULL){
      g_object_set(list->data,
		   "application-context", NULL,
		   NULL);

      list = list->next;
    }

    g_list_free_full(audio_application_context->sound_server,
		     g_object_unref);

    audio_application_context->sound_server = NULL;
  }

  /* audio */
  if(audio_application_context->audio != NULL){
    g_list_free_full(audio_application_context->audio,
		     g_object_unref);

    audio_application_context->audio = NULL;
  }

  /* osc server */
  if(audio_application_context->osc_server != NULL){
    g_list_free_full(audio_application_context->osc_server,
		     g_object_unref);

    audio_application_context->osc_server = NULL;
  }
    
  /* call parent */
  G_OBJECT_CLASS(ags_audio_application_context_parent_class)->dispose(gobject);
}

void
ags_audio_application_context_finalize(GObject *gobject)
{
  AgsAudioApplicationContext *audio_application_context;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(gobject);

  if(audio_application_context->thread_pool != NULL){
    g_object_unref(audio_application_context->thread_pool);
  }

  if(audio_application_context->polling_thread != NULL){
    g_object_unref(audio_application_context->polling_thread);
  }

  if(audio_application_context->worker != NULL){
    g_list_free_full(audio_application_context->worker,
		     g_object_unref);

    audio_application_context->worker = NULL;
  }

  if(audio_application_context->default_soundcard != NULL){
    g_object_unref(audio_application_context->default_soundcard);
  }
  
  if(audio_application_context->default_soundcard_thread != NULL){
    g_object_unref(audio_application_context->default_soundcard_thread);
  }

  if(audio_application_context->default_export_thread != NULL){
    g_object_unref(audio_application_context->default_export_thread);
  }

  if(audio_application_context->server != NULL){
    g_object_unref(audio_application_context->server);
  }

  if(audio_application_context->soundcard != NULL){
    g_list_free_full(audio_application_context->soundcard,
		     g_object_unref);
  }

  if(audio_application_context->sequencer != NULL){
    g_list_free_full(audio_application_context->sequencer,
		     g_object_unref);
  }
  
  if(audio_application_context->sound_server != NULL){
    g_list_free_full(audio_application_context->sound_server,
		     g_object_unref);
  }

  if(audio_application_context->audio != NULL){
    g_list_free_full(audio_application_context->audio,
		     g_object_unref);
  }

  if(audio_application_context->osc_server != NULL){
    g_list_free_full(audio_application_context->osc_server,
		     g_object_unref);
  }
    
  /* call parent */
  G_OBJECT_CLASS(ags_audio_application_context_parent_class)->finalize(gobject);
}

void
ags_audio_application_context_connect(AgsConnectable *connectable)
{
  AgsAudioApplicationContext *audio_application_context;

  GList *start_soundcard, *soundcard;
  GList *start_sequencer, *sequencer;

  pthread_mutex_t *application_context_mutex;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(connectable);

  if(ags_application_context_test_flags(AGS_APPLICATION_CONTEXT(audio_application_context), AGS_APPLICATION_CONTEXT_CONNECTED)){
    return;
  }

  ags_audio_application_context_parent_connectable_interface->connect(connectable);

  /* get application context mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(audio_application_context)->obj_mutex;
  
  pthread_mutex_unlock(ags_application_context_get_class_mutex());
  
  /* soundcard */
  pthread_mutex_lock(application_context_mutex);

  soundcard = 
    start_soundcard = g_list_copy(audio_application_context->soundcard);

  pthread_mutex_unlock(application_context_mutex);
  
  while(soundcard != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(soundcard->data));

    soundcard = soundcard->next;
  }

  g_list_free(start_soundcard);
  
  /* sequencer */
  pthread_mutex_lock(application_context_mutex);

  sequencer = 
    start_sequencer = g_list_copy(audio_application_context->sequencer);

  pthread_mutex_unlock(application_context_mutex);

  while(sequencer != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(sequencer->data));

    sequencer = sequencer->next;
  }

  g_list_free(start_sequencer);
}

void
ags_audio_application_context_disconnect(AgsConnectable *connectable)
{
  AgsAudioApplicationContext *audio_application_context;

  GList *start_soundcard, *soundcard;
  GList *start_sequencer, *sequencer;

  pthread_mutex_t *application_context_mutex;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(connectable);

  if(!ags_application_context_test_flags(AGS_APPLICATION_CONTEXT(audio_application_context), AGS_APPLICATION_CONTEXT_CONNECTED)){
    return;
  }

  ags_audio_application_context_parent_connectable_interface->disconnect(connectable);

  /* get application context mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(audio_application_context)->obj_mutex;
  
  pthread_mutex_unlock(ags_application_context_get_class_mutex());
  
  /* soundcard */
  pthread_mutex_lock(application_context_mutex);

  soundcard = 
    start_soundcard = g_list_copy(audio_application_context->soundcard);

  pthread_mutex_unlock(application_context_mutex);
  
  while(soundcard != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(soundcard->data));

    soundcard = soundcard->next;
  }

  g_list_free(start_soundcard);
  
  /* sequencer */
  pthread_mutex_lock(application_context_mutex);

  sequencer = 
    start_sequencer = g_list_copy(audio_application_context->sequencer);

  pthread_mutex_unlock(application_context_mutex);

  while(sequencer != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(sequencer->data));

    sequencer = sequencer->next;
  }

  g_list_free(start_sequencer);
}

AgsThread*
ags_audio_application_context_get_main_loop(AgsConcurrencyProvider *concurrency_provider)
{
  AgsThread *main_loop;
  
  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(concurrency_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* get main loop */
  pthread_mutex_lock(application_context_mutex);

  main_loop = (AgsThread *) AGS_APPLICATION_CONTEXT(concurrency_provider)->main_loop;

  pthread_mutex_unlock(application_context_mutex);
  
  return(main_loop);
}

AgsThread*
ags_audio_application_context_get_task_thread(AgsConcurrencyProvider *concurrency_provider)
{
  AgsThread *task_thread;
  
  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(concurrency_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* get task thread */
  pthread_mutex_lock(application_context_mutex);

  task_thread = (AgsThread *) AGS_APPLICATION_CONTEXT(concurrency_provider)->task_thread;
  
  pthread_mutex_unlock(application_context_mutex);

  return(task_thread);
}

AgsThreadPool*
ags_audio_application_context_get_thread_pool(AgsConcurrencyProvider *concurrency_provider)
{
  AgsThreadPool *thread_pool;
  
  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(concurrency_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* get thread pool */
  pthread_mutex_lock(application_context_mutex);

  thread_pool = AGS_AUDIO_APPLICATION_CONTEXT(concurrency_provider)->thread_pool;

  pthread_mutex_unlock(application_context_mutex);
  
  return(thread_pool);
}

GList*
ags_audio_application_context_get_worker(AgsConcurrencyProvider *concurrency_provider)
{
  GList *worker;
  
  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(concurrency_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* get worker */
  pthread_mutex_lock(application_context_mutex);

  worker = AGS_AUDIO_APPLICATION_CONTEXT(concurrency_provider)->worker;
  
  pthread_mutex_unlock(application_context_mutex);

  return(worker);
}

void
ags_audio_application_context_set_worker(AgsConcurrencyProvider *concurrency_provider,
					 GList *worker)
{
  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(concurrency_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* get worker */
  pthread_mutex_lock(application_context_mutex);

  AGS_AUDIO_APPLICATION_CONTEXT(concurrency_provider)->worker = worker;

  pthread_mutex_unlock(application_context_mutex);
}
GObject*
ags_audio_application_context_get_default_soundcard_thread(AgsSoundProvider *sound_provider)
{
  GObject *soundcard_thread;
  
  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(sound_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* get default soundcard thread */
  pthread_mutex_lock(application_context_mutex);

  soundcard_thread = (GObject *) AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->default_soundcard_thread;

  pthread_mutex_unlock(application_context_mutex);
  
  return(soundcard_thread);
}

void
ags_audio_application_context_set_default_soundcard(AgsSoundProvider *sound_provider,
						    GObject *soundcard)
{
  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(sound_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* set default soundcard */
  pthread_mutex_lock(application_context_mutex);

  if(AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->default_soundcard == soundcard){
    pthread_mutex_unlock(application_context_mutex);

    return;
  }

  if(AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->default_soundcard != NULL){
    g_object_unref(AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->default_soundcard);
  }
  
  if(soundcard != NULL){
    g_object_ref(soundcard);
  }

  AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->default_soundcard = soundcard;
  
  pthread_mutex_unlock(application_context_mutex);
}

GObject*
ags_audio_application_context_get_default_soundcard(AgsSoundProvider *sound_provider)
{
  GObject *soundcard;
  
  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(sound_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* get default soundcard */
  pthread_mutex_lock(application_context_mutex);

  soundcard = (GObject *) AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->default_soundcard;

  pthread_mutex_unlock(application_context_mutex);
  
  return(soundcard);
}

void
ags_audio_application_context_set_default_soundcard_thread(AgsSoundProvider *sound_provider,
							   GObject *soundcard_thread)
{
  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(sound_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* set default soundcard thread */
  pthread_mutex_lock(application_context_mutex);

  if(AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->default_soundcard_thread == (AgsThread *) soundcard_thread){
    pthread_mutex_unlock(application_context_mutex);

    return;
  }

  if(AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->default_soundcard_thread != NULL){
    g_object_unref(AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->default_soundcard_thread);
  }

  if(soundcard_thread != NULL){
    g_object_ref(soundcard_thread);
  }
  
  AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->default_soundcard_thread = (AgsThread *) soundcard_thread;

  pthread_mutex_unlock(application_context_mutex);
}

GList*
ags_audio_application_context_get_soundcard(AgsSoundProvider *sound_provider)
{
  GList *soundcard;

  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(sound_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* get soundcard */
  pthread_mutex_lock(application_context_mutex);
  
  soundcard = g_list_copy(AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->soundcard);

  pthread_mutex_unlock(application_context_mutex);
  
  return(soundcard);
}

void
ags_audio_application_context_set_soundcard(AgsSoundProvider *sound_provider,
					    GList *soundcard)
{
  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(sound_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* set soundcard */
  pthread_mutex_lock(application_context_mutex);

  if(AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->soundcard != NULL){
    g_list_free(AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->soundcard);
  }

  AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->soundcard = soundcard;

  pthread_mutex_unlock(application_context_mutex);
}

GList*
ags_audio_application_context_get_sequencer(AgsSoundProvider *sound_provider)
{
  GList *sequencer;

  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(sound_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* get sequencer */
  pthread_mutex_lock(application_context_mutex);
  
  sequencer = g_list_copy(AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->sequencer);

  pthread_mutex_unlock(application_context_mutex);
  
  return(sequencer);
}

void
ags_audio_application_context_set_sequencer(AgsSoundProvider *sound_provider,
					    GList *sequencer)
{
  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(sound_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* set sequencer */
  pthread_mutex_lock(application_context_mutex);

  if(AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->sequencer != NULL){
    g_list_free(AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->sequencer);
  }
  
  AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->sequencer = sequencer;

  pthread_mutex_unlock(application_context_mutex);
}

GList*
ags_audio_application_context_get_sound_server(AgsSoundProvider *sound_provider)
{
  GList *sound_server;

  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(sound_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* get sound server */
  pthread_mutex_lock(application_context_mutex);
  
  sound_server = g_list_copy(AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->sound_server);

  pthread_mutex_unlock(application_context_mutex);
  
  return(sound_server);
}

GList*
ags_audio_application_context_get_audio(AgsSoundProvider *sound_provider)
{
  GList *audio;

  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(sound_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* get audio */
  pthread_mutex_lock(application_context_mutex);
  
  audio = g_list_copy(AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->audio);

  pthread_mutex_unlock(application_context_mutex);
  
  return(audio);
}

void
ags_audio_application_context_set_audio(AgsSoundProvider *sound_provider,
					GList *audio)
{
  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(sound_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* set audio */
  pthread_mutex_lock(application_context_mutex);

  if(AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->audio != NULL){
    g_list_free(AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->audio);
  }

  AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->audio = audio;

  pthread_mutex_unlock(application_context_mutex);
}

GList*
ags_audio_application_context_get_osc_server(AgsSoundProvider *sound_provider)
{
  GList *osc_server;

  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(sound_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* get osc_server */
  pthread_mutex_lock(application_context_mutex);
  
  osc_server = g_list_copy(AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->osc_server);

  pthread_mutex_unlock(application_context_mutex);
  
  return(osc_server);
}

void
ags_audio_application_context_set_osc_server(AgsSoundProvider *sound_provider,
					     GList *osc_server)
{
  pthread_mutex_t *application_context_mutex;

  /* get mutex */
  pthread_mutex_lock(ags_application_context_get_class_mutex());
  
  application_context_mutex = AGS_APPLICATION_CONTEXT(sound_provider)->obj_mutex;

  pthread_mutex_unlock(ags_application_context_get_class_mutex());

  /* set osc_server */
  pthread_mutex_lock(application_context_mutex);

  if(AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->osc_server != NULL){
    g_list_free(AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->osc_server);
  }

  AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->osc_server = osc_server;

  pthread_mutex_unlock(application_context_mutex);
}

void
ags_audio_application_context_load_config(AgsApplicationContext *application_context)
{
  //TODO:JK: implement me
}


void
ags_audio_application_context_prepare(AgsApplicationContext *application_context)
{
  AgsAudioApplicationContext *audio_application_context;

  AgsThread *audio_loop, *polling_thread, *task_thread;
  AgsThreadPool *thread_pool;

  GList *start_queue;
  
  audio_application_context = (AgsAudioApplicationContext *) application_context;

  /* call parent */
  //  AGS_APPLICATION_CONTEXT_CLASS(ags_audio_application_context_parent_class)->prepare(application_context);
  
  /* register types */
  ags_application_context_register_types(application_context);

  /*
   * fundamental thread setup
   */
  /* AgsAudioLoop */
  application_context->main_loop = (GObject *) ags_audio_loop_new((GObject *) NULL,
								  (GObject *) audio_application_context);
  g_object_ref(application_context->main_loop);
  audio_loop = (AgsThread *) application_context->main_loop;
  g_object_set(audio_application_context,
	       "main-loop", audio_loop,
	       NULL);

  ags_connectable_connect(AGS_CONNECTABLE(audio_loop));

  /* AgsPollingThread */
  audio_application_context->polling_thread = (AgsThread *) ags_polling_thread_new();
  g_object_ref(audio_application_context->polling_thread);
  
  polling_thread = audio_application_context->polling_thread;
  ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				(AgsThread *) polling_thread,
				TRUE, TRUE);
  
  /* AgsTaskThread */
  application_context->task_thread = (GObject *) ags_task_thread_new();
  g_object_ref(application_context->task_thread);
  
  task_thread = (AgsThread *) application_context->task_thread;
  
  thread_pool = AGS_TASK_THREAD(task_thread)->thread_pool;
  ags_main_loop_set_async_queue(AGS_MAIN_LOOP(audio_loop),
				(GObject *) task_thread);
  ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				(AgsThread *) task_thread,
				TRUE, TRUE);

  /* start engine */
  pthread_mutex_lock(audio_loop->start_mutex);
  
  start_queue = NULL;
  start_queue = g_list_prepend(start_queue,
			       polling_thread);
  start_queue = g_list_prepend(start_queue,
			       task_thread);

  g_atomic_pointer_set(&(audio_loop->start_queue),
		       start_queue);
  
  pthread_mutex_unlock(audio_loop->start_mutex);

  /* start audio loop and thread pool*/
  ags_thread_start(audio_loop);
  
  ags_thread_pool_start(thread_pool);

  /* wait for audio loop */
  pthread_mutex_lock(audio_loop->start_mutex);

  if(g_atomic_int_get(&(audio_loop->start_wait)) == TRUE){	
    g_atomic_int_set(&(audio_loop->start_done),
		     FALSE);
      
    while(g_atomic_int_get(&(audio_loop->start_wait)) == TRUE &&
	  g_atomic_int_get(&(audio_loop->start_done)) == FALSE){
      pthread_cond_wait(audio_loop->start_cond,
			audio_loop->start_mutex);
    }
  }
    
  pthread_mutex_unlock(audio_loop->start_mutex);

  /* */
#if 0
  g_main_loop_run(g_main_loop_new(NULL,
				  TRUE));
#endif
}

void
ags_audio_application_context_setup(AgsApplicationContext *application_context)
{
  AgsAudioApplicationContext *audio_application_context;

  AgsServer *server;

  GObject *soundcard;
  GObject *sequencer;
  AgsJackServer *jack_server;
  AgsPulseServer *pulse_server;
  AgsCoreAudioServer *core_audio_server;

  AgsLadspaManager *ladspa_manager;
  AgsDssiManager *dssi_manager;
  AgsLv2Manager *lv2_manager;
  AgsLv2WorkerManager *lv2_worker_manager;

  AgsMessageDelivery *message_delivery;
  AgsMessageQueue *message_queue;
  AgsMessageQueue *audio_message_queue;
  AgsThread *soundcard_thread;
  AgsThread *export_thread;
  AgsThread *sequencer_thread;
  AgsDestroyWorker *destroy_worker;
  AgsThread *main_loop;

  AgsLog *log;
  AgsConfig *config;

  GList *list;  
  
  struct passwd *pw;

#ifdef AGS_USE_TIMER
  timer_t *timer_id;
#endif

  gchar *blacklist_filename;
  gchar *filename;
  gchar *soundcard_group;
  gchar *sequencer_group;
  gchar *str;
  gchar *capability;
  
  uid_t uid;
  
  guint i;
  gboolean single_thread_enabled;
  gboolean has_core_audio;
  gboolean has_pulse;
  gboolean has_jack;
  gboolean is_output;
  
  audio_application_context = (AgsAudioApplicationContext *) application_context;

  main_loop = (AgsThread *) AGS_APPLICATION_CONTEXT(audio_application_context)->main_loop;

  config = ags_config_get_instance();

  /* call parent */
  //  AGS_APPLICATION_CONTEXT_CLASS(ags_audio_application_context_parent_class)->setup(application_context);

  g_object_get(application_context,
	       "main-loop", &main_loop,
	       NULL);

  log = ags_log_get_instance();

#ifndef AGS_USE_TIMER
  atexit(ags_audio_application_context_signal_cleanup);

  /* Ignore interactive and job-control signals.  */
  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGTTOU, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);
  signal(AGS_THREAD_RESUME_SIG, SIG_IGN);
  signal(AGS_THREAD_SUSPEND_SIG, SIG_IGN);

  ags_sigact.sa_handler = ags_audio_application_context_signal_handler;
  sigemptyset(&ags_sigact.sa_mask);
  ags_sigact.sa_flags = 0;
  sigaction(SIGINT, &ags_sigact, (struct sigaction *) NULL);
  sigaction(SA_RESTART, &ags_sigact, (struct sigaction *) NULL);
#else
  timer_id = (timer_t *) malloc(sizeof(timer_t));
  
  /* create timer */
  ags_sigact_timer.sa_flags = SA_SIGINFO;
  ags_sigact_timer.sa_sigaction = ags_audio_application_context_signal_handler_timer;
  sigemptyset(&ags_sigact_timer.sa_mask);
  
  if(sigaction(SIGRTMIN, &ags_sigact_timer, NULL) == -1){
    perror("sigaction");
    exit(EXIT_FAILURE);
  }
  
  /* Block timer signal temporarily */
  sigemptyset(&ags_timer_mask);
  sigaddset(&ags_timer_mask, SIGRTMIN);
  
  if(sigprocmask(SIG_SETMASK, &ags_timer_mask, NULL) == -1){
    perror("sigprocmask");
    exit(EXIT_FAILURE);
  }

  /* Create the timer */
  ags_sev_timer.sigev_notify = SIGEV_SIGNAL;
  ags_sev_timer.sigev_signo = SIGRTMIN;
  ags_sev_timer.sigev_value.sival_ptr = timer_id;
  
  if(timer_create(CLOCK_MONOTONIC, &ags_sev_timer, timer_id) == -1){
    perror("timer_create");
    exit(EXIT_FAILURE);
  }
#endif
  
  /* check filename */
  filename = NULL;

  //  pthread_mutex_lock(ags_gui_thread_get_dispatch_mutex());
  
  for(i = 0; i < AGS_APPLICATION_CONTEXT(audio_application_context)->argc; i++){
    if(!strncmp(AGS_APPLICATION_CONTEXT(audio_application_context)->argv[i], "--filename", 11)){
      //TODO:JK: implement me
    }
  }

  /* get user information */
  uid = getuid();
  pw = getpwuid(uid);

  /* message delivery */
  message_delivery = ags_message_delivery_get_instance();

  message_queue = ags_message_queue_new("libags");
  ags_message_delivery_add_queue(message_delivery,
				 (GObject *) message_queue);

  audio_message_queue = ags_message_queue_new("libags-audio");
  ags_message_delivery_add_queue(message_delivery,
				 (GObject *) audio_message_queue);
    
  /* load ladspa manager */
  ladspa_manager = ags_ladspa_manager_get_instance();

  blacklist_filename = g_strdup_printf("%s/%s/ladspa_plugin.blacklist",
				       pw->pw_dir,
				       AGS_DEFAULT_DIRECTORY);
  ags_ladspa_manager_load_blacklist(ladspa_manager,
				    blacklist_filename);

  ags_log_add_message(log,
		      "* Loading LADSPA plugins");
  
  ags_ladspa_manager_load_default_directory(ladspa_manager);

  /* load dssi manager */
  dssi_manager = ags_dssi_manager_get_instance();

  blacklist_filename = g_strdup_printf("%s/%s/dssi_plugin.blacklist",
				       pw->pw_dir,
				       AGS_DEFAULT_DIRECTORY);
  ags_dssi_manager_load_blacklist(dssi_manager,
				  blacklist_filename);

  ags_log_add_message(log,
		      "* Loading DSSI plugins");

  ags_dssi_manager_load_default_directory(dssi_manager);

  /* load lv2 manager */
  lv2_manager = ags_lv2_manager_get_instance();
  lv2_worker_manager = ags_lv2_worker_manager_get_instance();    

  blacklist_filename = g_strdup_printf("%s/%s/lv2_plugin.blacklist",
				       pw->pw_dir,
				       AGS_DEFAULT_DIRECTORY);
  ags_lv2_manager_load_blacklist(lv2_manager,
				 blacklist_filename);

  ags_log_add_message(log,
		      "* Loading Lv2 plugins");

  ags_lv2_manager_load_default_directory(lv2_manager);
  
  /* fix cross-references in managers */
  lv2_worker_manager->thread_pool = ((AgsAudioApplicationContext *) ags_application_context)->thread_pool;

  /* launch audio */
  ags_log_add_message(log,
		      "* Launch audio");
  
  single_thread_enabled = FALSE;

  /* distributed manager */
  audio_application_context->sound_server = NULL;

  /* core audio server */
  core_audio_server = ags_core_audio_server_new(application_context,
						NULL);
  audio_application_context->sound_server = g_list_append(audio_application_context->sound_server,
							 core_audio_server);
  g_object_ref(G_OBJECT(core_audio_server));

  has_core_audio = FALSE;

  /* pulse server */
  pulse_server = ags_pulse_server_new(application_context,
				      NULL);
  audio_application_context->sound_server = g_list_append(audio_application_context->sound_server,
							 pulse_server);
  g_object_ref(G_OBJECT(pulse_server));

  has_pulse = FALSE;
  
  /* jack server */
  jack_server = ags_jack_server_new(application_context,
				    NULL);
  audio_application_context->sound_server = g_list_append(audio_application_context->sound_server,
							 jack_server);
  g_object_ref(G_OBJECT(jack_server));

  has_jack = FALSE;

  /* AgsSoundcard */
  audio_application_context->soundcard = NULL;
  soundcard = NULL;

  soundcard_group = g_strdup("soundcard");
  
  for(i = 0; ; i++){
    guint pcm_channels, buffer_size, samplerate, format;

    if(!g_key_file_has_group(config->key_file,
			     soundcard_group)){
      if(i == 0){
	g_free(soundcard_group);    
	soundcard_group = g_strdup_printf("%s-%d",
					  AGS_CONFIG_SOUNDCARD,
					  i);
    	
	continue;
      }else{
	break;
      }
    }

    str = ags_config_get_value(config,
			       soundcard_group,
			       "backend");

    capability = ags_config_get_value(config,
				      soundcard_group,
				      "capability");

    is_output = TRUE;

    if(capability != NULL &&
       !g_ascii_strncasecmp(capability,
			    "capture",
			    8)){
      is_output = FALSE;
    }
    
    /* change soundcard */
    if(str != NULL){
      if(!g_ascii_strncasecmp(str,
			      "core-audio",
			      11)){
	soundcard = ags_sound_server_register_soundcard(AGS_SOUND_SERVER(core_audio_server),
							is_output);

	has_core_audio = TRUE;
      }else if(!g_ascii_strncasecmp(str,
			      "pulse",
			      6)){
	soundcard = ags_sound_server_register_soundcard(AGS_SOUND_SERVER(pulse_server),
							is_output);

	has_pulse = TRUE;
      }else if(!g_ascii_strncasecmp(str,
			      "jack",
			      5)){
	soundcard = ags_sound_server_register_soundcard(AGS_SOUND_SERVER(jack_server),
							is_output);

	has_jack = TRUE;
      }else if(!g_ascii_strncasecmp(str,
				    "alsa",
				    5)){
	if(is_output){
	  soundcard = (GObject *) ags_devout_new((GObject *) audio_application_context);
	  
	  AGS_DEVOUT(soundcard)->flags &= (~AGS_DEVOUT_OSS);
	  AGS_DEVOUT(soundcard)->flags |= AGS_DEVOUT_ALSA;
	}else{
	  soundcard = (GObject *) ags_devin_new((GObject *) audio_application_context);
	  
	  AGS_DEVIN(soundcard)->flags &= (~AGS_DEVIN_OSS);
	  AGS_DEVIN(soundcard)->flags |= AGS_DEVIN_ALSA;
	}
      }else if(!g_ascii_strncasecmp(str,
				    "oss",
				    4)){
	if(is_output){
	  soundcard = (GObject *) ags_devout_new((GObject *) audio_application_context);

	  AGS_DEVOUT(soundcard)->flags &= (~AGS_DEVOUT_ALSA);
	  AGS_DEVOUT(soundcard)->flags |= AGS_DEVOUT_OSS;
	}else{
	  soundcard = (GObject *) ags_devin_new((GObject *) audio_application_context);

	  AGS_DEVIN(soundcard)->flags &= (~AGS_DEVIN_ALSA);
	  AGS_DEVIN(soundcard)->flags |= AGS_DEVIN_OSS;	  
	}
      }else{
	g_warning(i18n("unknown soundcard backend - %s"), str);

	g_free(soundcard_group);    
	soundcard_group = g_strdup_printf("%s-%d",
					  AGS_CONFIG_SOUNDCARD,
					  i);
    
	continue;
      }
    }else{
      g_warning(i18n("unknown soundcard backend - NULL"));

      g_free(soundcard_group);    
      soundcard_group = g_strdup_printf("%s-%d",
					AGS_CONFIG_SOUNDCARD,
					i);
          
      continue;
    }

    if(audio_application_context->soundcard == NULL){
      g_object_set(main_loop,
		   "default-output-soundcard", G_OBJECT(soundcard),
		   NULL);
    }
    
    audio_application_context->soundcard = g_list_append(audio_application_context->soundcard,
							soundcard);
    g_object_ref(soundcard);

    /* device */
    str = ags_config_get_value(config,
			       soundcard_group,
			       "device");

    if(str != NULL){
      ags_soundcard_set_device(AGS_SOUNDCARD(soundcard),
			       str);
      g_free(str);
    }
    
    /* presets */
    pcm_channels = AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS;
    buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
    samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
    format = AGS_SOUNDCARD_DEFAULT_FORMAT;

    str = ags_config_get_value(config,
			       soundcard_group,
			       "pcm-channels");

    if(str != NULL){
      pcm_channels = g_ascii_strtoull(str,
				      NULL,
				      10);
      g_free(str);
    }

    str = ags_config_get_value(config,
			       soundcard_group,
			       "buffer-size");

    if(str != NULL){
      buffer_size = g_ascii_strtoull(str,
				     NULL,
				     10);
      g_free(str);
    }

    str = ags_config_get_value(config,
			       soundcard_group,
			       "samplerate");

    if(str != NULL){
      samplerate = g_ascii_strtoull(str,
				    NULL,
				    10);
      g_free(str);
    }

    str = ags_config_get_value(config,
			       soundcard_group,
			       "format");

    if(str != NULL){
      format = g_ascii_strtoull(str,
				NULL,
				10);
      g_free(str);
    }

    ags_soundcard_set_presets(AGS_SOUNDCARD(soundcard),
			      pcm_channels,
			      samplerate,
			      buffer_size,
			      format);

    g_free(soundcard_group);    
    soundcard_group = g_strdup_printf("%s-%d",
				      AGS_CONFIG_SOUNDCARD,
				      i);
  }

  if(audio_application_context->soundcard != NULL){
    soundcard = audio_application_context->soundcard->data;
  }  

  ags_sound_provider_set_default_soundcard(AGS_SOUND_PROVIDER(audio_application_context),
					   soundcard);

  g_free(soundcard_group);

  /* AgsSequencer */
  audio_application_context->sequencer = NULL;
  sequencer = NULL;

  sequencer_group = g_strdup("sequencer");
  
  for(i = 0; ; i++){
    guint pcm_channels, buffer_size, samplerate, format;

    if(!g_key_file_has_group(config->key_file,
			     sequencer_group)){
      if(i == 0){
	g_free(sequencer_group);    
	sequencer_group = g_strdup_printf("%s-%d",
					  AGS_CONFIG_SEQUENCER,
					  i);
    	
	continue;
      }else{
	break;
      }
    }

    str = ags_config_get_value(config,
			       sequencer_group,
			       "backend");
    
    /* change sequencer */
    if(str != NULL){
      if(!g_ascii_strncasecmp(str,
			      "jack",
			      5)){
	sequencer = ags_sound_server_register_sequencer(AGS_SOUND_SERVER(jack_server),
							       FALSE);

	has_jack = TRUE;
      }else if(!g_ascii_strncasecmp(str,
				    "alsa",
				    5)){
	sequencer = (GObject *) ags_midiin_new((GObject *) audio_application_context);
	AGS_MIDIIN(sequencer)->flags &= (~AGS_MIDIIN_OSS);
	AGS_MIDIIN(sequencer)->flags |= AGS_MIDIIN_ALSA;
      }else if(!g_ascii_strncasecmp(str,
				    "oss",
				    4)){
	sequencer = (GObject *) ags_midiin_new((GObject *) audio_application_context);
	AGS_MIDIIN(sequencer)->flags &= (~AGS_MIDIIN_ALSA);
	AGS_MIDIIN(sequencer)->flags |= AGS_MIDIIN_OSS;
      }else{
	g_warning(i18n("unknown sequencer backend - %s"), str);

	g_free(sequencer_group);    
	sequencer_group = g_strdup_printf("%s-%d",
					  AGS_CONFIG_SEQUENCER,
					  i);
    
	continue;
      }
    }else{
      g_warning(i18n("unknown sequencer backend - NULL"));

      g_free(sequencer_group);    
      sequencer_group = g_strdup_printf("%s-%d",
					AGS_CONFIG_SEQUENCER,
					i);
          
      continue;
    }
    
    audio_application_context->sequencer = g_list_append(audio_application_context->sequencer,
							sequencer);
    g_object_ref(sequencer);

    /* device */
    str = ags_config_get_value(config,
			       sequencer_group,
			       "device");
    
    if(str != NULL){
      ags_sequencer_set_device(AGS_SEQUENCER(sequencer),
			       str);
      g_free(str);
    }

    g_free(sequencer_group);    
    sequencer_group = g_strdup_printf("%s-%d",
				      AGS_CONFIG_SEQUENCER,
				      i);
  }

  g_free(sequencer_group);

  /* AgsServer */
  audio_application_context->server = ags_server_new((GObject *) audio_application_context);
  
  /* AgsSoundcardThread and AgsExportThread */
  audio_application_context->default_soundcard_thread = NULL;
  list = audio_application_context->soundcard;
    
  while(list != NULL){
    AgsNotifySoundcard *notify_soundcard;

    guint soundcard_capability;

    soundcard_capability = ags_soundcard_get_capability(AGS_SOUNDCARD(list->data));
    
    soundcard_thread = (AgsThread *) ags_soundcard_thread_new(list->data,
							      soundcard_capability);
    ags_thread_add_child_extended(AGS_THREAD(main_loop),
				  (AgsThread *) soundcard_thread,
				  TRUE, TRUE);

    /* notify soundcard and export thread */
    export_thread = NULL;
    
    //    if(soundcard_capability == AGS_SOUNDCARD_CAPABILITY_PLAYBACK){
      notify_soundcard = ags_notify_soundcard_new((AgsSoundcardThread *) soundcard_thread);
      g_object_ref(notify_soundcard);
      AGS_TASK(notify_soundcard)->task_thread = application_context->task_thread;
    
      if(AGS_IS_DEVOUT(list->data)){
	AGS_DEVOUT(list->data)->notify_soundcard = (GObject *) notify_soundcard;
      }else if(AGS_IS_JACK_DEVOUT(list->data)){
	AGS_JACK_DEVOUT(list->data)->notify_soundcard = (GObject *) notify_soundcard;
      }else if(AGS_IS_PULSE_DEVOUT(list->data)){
	AGS_PULSE_DEVOUT(list->data)->notify_soundcard = (GObject *) notify_soundcard;
      }else if(AGS_IS_CORE_AUDIO_DEVOUT(list->data)){
	AGS_CORE_AUDIO_DEVOUT(list->data)->notify_soundcard = (GObject *) notify_soundcard;
      }else if(AGS_IS_DEVIN(list->data)){
	AGS_DEVIN(list->data)->notify_soundcard = (GObject *) notify_soundcard;
      }else if(AGS_IS_JACK_DEVIN(list->data)){
	AGS_JACK_DEVIN(list->data)->notify_soundcard = (GObject *) notify_soundcard;
      }else if(AGS_IS_PULSE_DEVIN(list->data)){
	AGS_PULSE_DEVIN(list->data)->notify_soundcard = (GObject *) notify_soundcard;
      }else if(AGS_IS_CORE_AUDIO_DEVIN(list->data)){
	AGS_CORE_AUDIO_DEVIN(list->data)->notify_soundcard = (GObject *) notify_soundcard;
      }

      ags_task_thread_append_cyclic_task((AgsTaskThread *) application_context->task_thread,
					 (AgsTask *) notify_soundcard);

      /* export thread */
      if(AGS_IS_DEVOUT(list->data) ||
	 AGS_IS_JACK_DEVOUT(list->data) ||
	 AGS_IS_PULSE_DEVOUT(list->data) ||
	 AGS_IS_CORE_AUDIO_DEVOUT(list->data)){
	export_thread = (AgsThread *) ags_export_thread_new(list->data,
							    NULL);
	ags_thread_add_child_extended(AGS_THREAD(main_loop),
				      (AgsThread *) export_thread,
				      TRUE, TRUE);
      }    

    /* default soundcard thread */
    if(audio_application_context->default_soundcard_thread == NULL){
      ags_sound_provider_set_default_soundcard_thread(AGS_SOUND_PROVIDER(audio_application_context),
						      (GObject *) soundcard_thread);
    }

    /* default export thread */
    if(export_thread != NULL &&
       audio_application_context->default_export_thread == NULL){
      audio_application_context->default_export_thread = export_thread;
      g_object_ref(export_thread);
    }

    /* iterate */
    list = list->next;      
  }
  
  /* AgsSequencerThread */
  list = audio_application_context->sequencer;
    
  while(list != NULL){
    sequencer_thread = (AgsThread *) ags_sequencer_thread_new(list->data);
    ags_thread_add_child_extended(AGS_THREAD(main_loop),
				  (AgsThread *) sequencer_thread,
				  TRUE, TRUE);

    list = list->next;      
  }

  /* AgsAutosaveThread */
  audio_application_context->autosave_thread = NULL;
  
  if(!g_strcmp0(ags_config_get_value(application_context->config,
				     AGS_CONFIG_GENERIC,
				     "autosave-thread"),
	       "true")){
    if(g_strcmp0(ags_config_get_value(application_context->config,
				      AGS_CONFIG_GENERIC,
				      "simple-file"),
		 "false")){
      audio_application_context->autosave_thread = (AgsThread *) ags_autosave_thread_new((GObject *) audio_application_context);
      ags_thread_add_child_extended(AGS_THREAD(main_loop),
				    (AgsThread *) audio_application_context->autosave_thread,
				    TRUE, TRUE);
    }
  }

  /* AgsWorkerThread */
  audio_application_context->worker = NULL;

  /* AgsDestroyWorker */
  destroy_worker = ags_destroy_worker_new();
  g_object_ref(destroy_worker);
  ags_thread_add_child_extended(AGS_THREAD(main_loop),
				(AgsThread *) destroy_worker,
				TRUE, TRUE);
  audio_application_context->worker = g_list_prepend(audio_application_context->worker,
						    destroy_worker);
  ags_thread_start((AgsThread *) destroy_worker);
  
  /* AgsThreadPool */
  audio_application_context->thread_pool = AGS_TASK_THREAD(application_context->task_thread)->thread_pool;

  /* launch */
  if(has_core_audio){
    ags_core_audio_server_connect_client(core_audio_server);
  }

  if(has_pulse){
    ags_pulse_server_connect_client(pulse_server);

    ags_pulse_server_start_poll(pulse_server);
  }

  if(has_jack){
    ags_jack_server_connect_client(jack_server);
  }
}

void
ags_audio_application_context_set_value_callback(AgsConfig *config, gchar *group, gchar *key, gchar *value,
						 AgsAudioApplicationContext *audio_application_context)
{
  if(!strncmp(group,
	      AGS_CONFIG_SOUNDCARD,
	      7)){
    AgsSoundcard *soundcard;

    if(audio_application_context == NULL ||
       audio_application_context->soundcard == NULL){
      return;
    }

    soundcard = audio_application_context->soundcard->data;

    if(!strncmp(key,
		"samplerate",
		10)){    
      guint samplerate;

      samplerate = strtoul(value,
			   NULL,
			   10);

      g_object_set(G_OBJECT(soundcard),
		   "frequency", samplerate,
		   NULL);
    }else if(!strncmp(key,
		      "buffer-size",
		      11)){
      guint buffer_size;
    
      buffer_size = strtoul(value,
			    NULL,
			    10);

      g_object_set(G_OBJECT(soundcard),
		   "buffer-size", buffer_size,
		   NULL);
    }else if(!strncmp(key,
		      "pcm-channels",
		      12)){
      guint pcm_channels;

      pcm_channels = strtoul(value,
			     NULL,
			     10);
      
      g_object_set(G_OBJECT(soundcard),
		   "pcm-channels", pcm_channels,
		   NULL);
    }else if(!strncmp(key,
		      "dsp-channels",
		      12)){
      guint dsp_channels;

      dsp_channels = strtoul(value,
			     NULL,
			     10);
      
      g_object_set(G_OBJECT(soundcard),
		   "dsp-channels", dsp_channels,
		   NULL);
    }else if(!strncmp(key,
		      "alsa-handle",
		      11)){
      gchar *alsa_handle;
    
      alsa_handle = value;
      g_object_set(G_OBJECT(soundcard),
		   "device", alsa_handle,
		   NULL);
    }
  }
}

void
ags_audio_application_context_register_types(AgsApplicationContext *application_context)
{
  ags_audio_loop_get_type();
  ags_soundcard_thread_get_type();
  ags_export_thread_get_type();

  /* register recalls */
  ags_play_audio_get_type();
  ags_play_channel_get_type();
  ags_play_channel_run_get_type();
  ags_play_channel_run_master_get_type();

  ags_stream_channel_get_type();
  ags_stream_channel_run_get_type();

  ags_loop_channel_get_type();
  ags_loop_channel_run_get_type();

  ags_copy_channel_get_type();
  ags_copy_channel_run_get_type();

  ags_volume_channel_get_type();
  ags_volume_channel_run_get_type();

  ags_peak_channel_get_type();
  ags_peak_channel_run_get_type();

  ags_recall_ladspa_get_type();
  ags_generic_recall_channel_run_get_type();
  ags_recall_ladspa_run_get_type();

  ags_delay_audio_get_type();
  ags_delay_audio_run_get_type();

  ags_count_beats_audio_get_type();
  ags_count_beats_audio_run_get_type();

  ags_copy_pattern_audio_get_type();
  ags_copy_pattern_audio_run_get_type();
  ags_copy_pattern_channel_get_type();
  ags_copy_pattern_channel_run_get_type();

  ags_buffer_channel_get_type();
  ags_buffer_channel_run_get_type();

  ags_play_notation_audio_get_type();
  ags_play_notation_audio_run_get_type();
}

void
ags_audio_application_context_quit(AgsApplicationContext *application_context)
{
  AgsLadspaManager *ladspa_manager;
  AgsDssiManager *dssi_manager;
  AgsLv2Manager *lv2_manager;

  AgsCoreAudioServer *core_audio_server;

  AgsPulseServer *pulse_server;

  AgsJackServer *jack_server;

  AgsConfig *config;

  GList *core_audio_client;
  GList *jack_client;
  GList *list;

  gchar *filename;
  gchar *str;

  gboolean autosave_thread_enabled;

  config = application_context->config;
  
  /* autosave thread */
  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "autosave-thread");
  autosave_thread_enabled = (str != NULL && !g_ascii_strncasecmp(str, "true", 8)) ? TRUE: FALSE;

  /* free managers */
  ladspa_manager = ags_ladspa_manager_get_instance();
  g_object_unref(ladspa_manager);

  dssi_manager = ags_dssi_manager_get_instance();
  g_object_unref(dssi_manager);

  lv2_manager = ags_lv2_manager_get_instance();
  g_object_unref(lv2_manager);
  
  /* retrieve core audio server */
  list = ags_sound_provider_get_sound_server(AGS_SOUND_PROVIDER(application_context));
  
  while((list = ags_list_util_find_type(list,
					AGS_TYPE_CORE_AUDIO_SERVER)) != NULL){
    core_audio_server = list->data;

    /* close client */
    core_audio_client = core_audio_server->client;

    while(core_audio_client != NULL){
#ifdef AGS_WITH_CORE_AUDIO
      AUGraphStop(AGS_CORE_AUDIO_CLIENT(core_audio_client->data)->graph);
#endif

      core_audio_client = core_audio_client->next;
    }

    list = list->next;
  }
  
  /* retrieve pulseaudio server */
  list = ags_sound_provider_get_sound_server(AGS_SOUND_PROVIDER(application_context));
  
  while((list = ags_list_util_find_type(list,
					AGS_TYPE_PULSE_SERVER)) != NULL){
    pulse_server = list->data;

#ifdef AGS_WITH_PULSE
    pa_mainloop_quit(pulse_server->main_loop,
		     0);
#endif
 
    list = list->next;
  }
  
  /* retrieve JACK server */
  list = ags_sound_provider_get_sound_server(AGS_SOUND_PROVIDER(application_context));
  
  if((list = ags_list_util_find_type(list,
				     AGS_TYPE_JACK_SERVER)) != NULL){
    jack_server = list->data;

    /* close client */
    jack_client = jack_server->client;

    while(jack_client != NULL){
#ifdef AGS_WITH_JACK
      jack_client_close(AGS_JACK_CLIENT(jack_client->data)->client);
#endif

      jack_client = jack_client->next;
    }
    
    list = list->next;
  }

  exit(0);
}

void
ags_audio_application_context_read(AgsFile *file, xmlNode *node, GObject **application_context)
{
  AgsAudioApplicationContext *gobject;
  GList *list;
  xmlNode *child;

  if(*application_context == NULL){
    gobject = (AgsAudioApplicationContext *) g_object_new(AGS_TYPE_AUDIO_APPLICATION_CONTEXT,
							  NULL);

    *application_context = (GObject *) gobject;
  }else{
    gobject = (AgsAudioApplicationContext *) *application_context;
  }

  file->application_context = (GObject *) gobject;

  g_object_set(G_OBJECT(file),
	       "application-context", gobject,
	       NULL);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference", gobject,
				   NULL));
  
  /* properties */
  AGS_APPLICATION_CONTEXT(gobject)->flags = (guint) g_ascii_strtoull(xmlGetProp(node, AGS_FILE_FLAGS_PROP),
								     NULL,
								     16);

  AGS_APPLICATION_CONTEXT(gobject)->version = xmlGetProp(node,
							 AGS_FILE_VERSION_PROP);

  AGS_APPLICATION_CONTEXT(gobject)->build_id = xmlGetProp(node,
							  AGS_FILE_BUILD_ID_PROP);

  //TODO:JK: check version compatibelity

  /* child elements */
  child = node->children;

  while(child != NULL){
    if(child->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp("ags-devout-list",
		     child->name,
		     16)){
	ags_file_read_soundcard_list(file,
				     child,
				     &(gobject->soundcard));
      }
    }

    child = child->next;
  }
}

xmlNode*
ags_audio_application_context_write(AgsFile *file, xmlNode *parent, GObject *application_context)
{
  xmlNode *node, *child;
  gchar *id;

  id = ags_id_generator_create_uuid();

  node = xmlNewNode(NULL,
		    "ags-application-context");

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", application_context,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_CONTEXT_PROP,
	     "audio");

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x", ((~AGS_APPLICATION_CONTEXT_CONNECTED) & (AGS_APPLICATION_CONTEXT(application_context)->flags))));

  xmlNewProp(node,
	     AGS_FILE_VERSION_PROP,
	     AGS_APPLICATION_CONTEXT(application_context)->version);

  xmlNewProp(node,
	     AGS_FILE_BUILD_ID_PROP,
	     AGS_APPLICATION_CONTEXT(application_context)->build_id);

  /* add to parent */
  xmlAddChild(parent,
	      node);

  ags_file_write_soundcard_list(file,
				node,
				AGS_AUDIO_APPLICATION_CONTEXT(application_context)->soundcard);

  return(node);
}

AgsAudioApplicationContext*
ags_audio_application_context_new()
{
  AgsAudioApplicationContext *audio_application_context;

  audio_application_context = (AgsAudioApplicationContext *) g_object_new(AGS_TYPE_AUDIO_APPLICATION_CONTEXT,
									  NULL);

  return(audio_application_context);
}
