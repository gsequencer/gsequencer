/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/config.h>

#include <ags/plugin/ags_ladspa_manager.h>
#include <ags/plugin/ags_dssi_manager.h>
#include <ags/plugin/ags_lv2_turtle_parser.h>
#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2ui_manager.h>
#include <ags/plugin/ags_lv2_worker_manager.h>

#if defined(AGS_WITH_VST3)
#include <ags/plugin/ags_vst3_manager.h>
#endif

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_generic_recall_channel_run.h>
#include <ags/audio/ags_recall_ladspa.h>
#include <ags/audio/ags_recall_ladspa_run.h>
#include <ags/audio/ags_recall_lv2.h>
#include <ags/audio/ags_recall_lv2_run.h>
#include <ags/audio/ags_recall_dssi.h>
#include <ags/audio/ags_recall_dssi_run.h>

#include <ags/audio/alsa/ags_alsa_devout.h>
#include <ags/audio/alsa/ags_alsa_devin.h>
#include <ags/audio/alsa/ags_alsa_midiin.h>

#include <ags/audio/oss/ags_oss_devout.h>
#include <ags/audio/oss/ags_oss_devin.h>
#include <ags/audio/oss/ags_oss_midiin.h>

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

#include <ags/audio/wasapi/ags_wasapi_devout.h>
#include <ags/audio/wasapi/ags_wasapi_devin.h>

#include <ags/audio/osc/ags_osc_server.h>

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

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>
#include <ags/audio/thread/ags_sequencer_thread.h>
#include <ags/audio/thread/ags_export_thread.h>

#include <sys/types.h>

#include <unistd.h>

#if !defined(AGS_W32API)
#include <pwd.h>
#endif

#if !defined(AGS_W32API)
#include <sys/utsname.h>
#endif

#include <stdbool.h>

#include <signal.h>

#include <ags/i18n.h>

void ags_audio_application_context_signal_handler(int signr);
static void ags_audio_application_context_signal_cleanup();

void ags_audio_application_context_class_init(AgsAudioApplicationContextClass *audio_application_context);
void ags_audio_application_context_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_application_context_concurrency_provider_interface_init(AgsConcurrencyProviderInterface *concurrency_provider);
void ags_audio_application_context_service_provider_interface_init(AgsServiceProviderInterface *service_provider);
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
void ags_audio_application_context_set_main_loop(AgsConcurrencyProvider *concurrency_provider,
						 AgsThread *main_loop);
AgsTaskLauncher* ags_audio_application_context_get_task_launcher(AgsConcurrencyProvider *concurrency_provider);
void ags_audio_application_context_set_task_launcher(AgsConcurrencyProvider *concurrency_provider,
						     AgsTaskLauncher *task_launcher);
AgsThreadPool* ags_audio_application_context_get_thread_pool(AgsConcurrencyProvider *concurrency_provider);
void ags_audio_application_context_set_thread_pool(AgsConcurrencyProvider *concurrency_provider,
						   AgsThreadPool *thread_pool);
GList* ags_audio_application_context_get_worker(AgsConcurrencyProvider *concurrency_provider);
void ags_audio_application_context_set_worker(AgsConcurrencyProvider *concurrency_provider,
					      GList *worker);

gboolean ags_audio_application_context_is_operating(AgsServiceProvider *service_provider);

AgsServerStatus* ags_audio_application_context_server_status(AgsServiceProvider *service_provider);

void ags_audio_application_context_set_registry(AgsServiceProvider *service_provider,
						AgsRegistry *registry);
AgsRegistry* ags_audio_application_context_get_registry(AgsServiceProvider *service_provider);

void ags_audio_application_context_set_server(AgsServiceProvider *service_provider,
					      GList *server);
GList* ags_audio_application_context_get_server(AgsServiceProvider *service_provider);

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
GList* ags_audio_application_context_get_audio(AgsSoundProvider *sound_provider);
void ags_audio_application_context_set_audio(AgsSoundProvider *sound_provider,
					     GList *audio);
GList* ags_audio_application_context_get_sound_server(AgsSoundProvider *sound_provider);
void ags_audio_application_context_set_sound_server(AgsSoundProvider *sound_provider,
						    GList *sound_server);
GList* ags_audio_application_context_get_osc_server(AgsSoundProvider *sound_provider);
void ags_audio_application_context_set_osc_server(AgsSoundProvider *sound_provider,
						  GList *osc_server);

void ags_audio_application_context_prepare(AgsApplicationContext *application_context);
void ags_audio_application_context_setup(AgsApplicationContext *application_context);

void ags_audio_application_context_register_types(AgsApplicationContext *application_context);

void ags_audio_application_context_read(AgsFile *file, xmlNode *node, GObject **application_context);
xmlNode* ags_audio_application_context_write(AgsFile *file, xmlNode *parent, GObject *application_context);

void ags_audio_application_context_quit(AgsApplicationContext *application_context);

void ags_audio_application_context_set_value_callback(AgsConfig *config, gchar *group, gchar *key, gchar *value,
						      AgsAudioApplicationContext *audio_application_context);

void* ags_audio_application_context_server_main_loop_thread(GMainLoop *main_loop);
void* ags_audio_application_context_audio_main_loop_thread(GMainLoop *main_loop);

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
};

static gpointer ags_audio_application_context_parent_class = NULL;
static AgsConnectableInterface* ags_audio_application_context_parent_connectable_interface;

extern AgsApplicationContext *ags_application_context;

//TODO:JK: implement get functions
#ifndef AGS_W32API
static struct sigaction ags_sigact;
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

    static const GInterfaceInfo ags_service_provider_interface_info = {
      (GInterfaceInitFunc) ags_audio_application_context_service_provider_interface_init,
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
				AGS_TYPE_SERVICE_PROVIDER,
				&ags_service_provider_interface_info);

    g_type_add_interface_static(ags_type_audio_application_context,
				AGS_TYPE_SOUND_PROVIDER,
				&ags_sound_provider_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_audio_application_context);
  }

  return g_define_type_id__volatile;
}

void
ags_audio_application_context_signal_handler(int signr)
{
#ifndef AGS_W32API
  if(signr == SIGINT){
    //TODO:JK: do backup
    
    exit(-1);
  }else{
    sigemptyset(&(ags_sigact.sa_mask));
  }
#endif
}

static void
ags_audio_application_context_signal_cleanup()
{
#ifndef AGS_W32API
  sigemptyset(&(ags_sigact.sa_mask));
#endif
}

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
  
  /* AgsApplicationContextClass */
  application_context = (AgsApplicationContextClass *) audio_application_context;
  
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
  concurrency_provider->set_main_loop = ags_audio_application_context_set_main_loop;

  concurrency_provider->get_task_launcher = ags_audio_application_context_get_task_launcher;
  concurrency_provider->set_task_launcher = ags_audio_application_context_set_task_launcher;

  concurrency_provider->get_thread_pool = ags_audio_application_context_get_thread_pool;
  concurrency_provider->set_thread_pool = ags_audio_application_context_set_thread_pool;

  concurrency_provider->get_worker = ags_audio_application_context_get_worker;
  concurrency_provider->set_worker = ags_audio_application_context_set_worker;
}

void
ags_audio_application_context_service_provider_interface_init(AgsServiceProviderInterface *service_provider)
{
  service_provider->is_operating = ags_audio_application_context_is_operating;

  service_provider->server_status = ags_audio_application_context_server_status;

  service_provider->set_registry = ags_audio_application_context_set_registry;
  service_provider->get_registry = ags_audio_application_context_get_registry;
  
  service_provider->set_server = ags_audio_application_context_set_server;
  service_provider->get_server = ags_audio_application_context_get_server;
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
  sound_provider->set_sound_server = ags_audio_application_context_set_sound_server;

  sound_provider->get_audio = ags_audio_application_context_get_audio;
  sound_provider->set_audio = ags_audio_application_context_set_audio;

  sound_provider->get_osc_server = ags_audio_application_context_get_osc_server;
  sound_provider->set_osc_server = ags_audio_application_context_set_osc_server;
}

void
ags_audio_application_context_init(AgsAudioApplicationContext *audio_application_context)
{
  AgsConfig *config;
  AgsLog *log;
  
  if(ags_application_context == NULL){
    ags_application_context = (AgsApplicationContext *) audio_application_context;
  }
  
  /* fundamental instances */
  config = ags_config_get_instance();

  AGS_APPLICATION_CONTEXT(audio_application_context)->config = config;
  g_object_ref(config);

  log = (GObject *) ags_log_get_instance();

  AGS_APPLICATION_CONTEXT(audio_application_context)->log = log;
  g_object_ref(log);
  
  /* Audio application context */  
  audio_application_context->thread_pool = NULL;

  audio_application_context->worker = NULL;
  
  audio_application_context->is_operating = FALSE;

  audio_application_context->server_status = NULL;

  audio_application_context->registry = NULL;

  audio_application_context->server = NULL;

  audio_application_context->default_soundcard = NULL;

  audio_application_context->default_soundcard_thread = NULL;
  audio_application_context->default_export_thread = NULL;
    
  audio_application_context->soundcard = NULL;
  audio_application_context->sequencer = NULL;

  audio_application_context->sound_server = NULL;
  
  audio_application_context->audio = NULL;

  audio_application_context->osc_server = NULL;

  audio_application_context->start_loader = FALSE;

  audio_application_context->setup_ready = FALSE;
  audio_application_context->loader_ready = FALSE;

  audio_application_context->ladspa_loading = FALSE;
  audio_application_context->dssi_loading = FALSE;
  audio_application_context->lv2_loading = FALSE;
  audio_application_context->vst3_loading = FALSE;
  
  audio_application_context->ladspa_loader = NULL;
  audio_application_context->dssi_loader = NULL;
  audio_application_context->lv2_loader = NULL;
  audio_application_context->vst3_loader = NULL;

  audio_application_context->lv2_turtle_scanner = NULL;
  
  g_timeout_add(AGS_AUDIO_APPLICATION_CONTEXT_DEFAULT_LOADER_INTERVAL,
		ags_audio_application_context_loader_timeout,
		audio_application_context);
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
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_application_context_dispose(GObject *gobject)
{
  AgsAudioApplicationContext *audio_application_context;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(gobject);
    
  /* thread pool */
  if(audio_application_context->thread_pool != NULL){
    g_object_unref(audio_application_context->thread_pool);
    
    audio_application_context->thread_pool = NULL;
  }

  /* worker thread */
  if(audio_application_context->worker != NULL){    
    g_list_free_full(audio_application_context->worker,
		     g_object_unref);

    audio_application_context->worker = NULL;
  }

  /* server status */
  if(audio_application_context->server_status != NULL){
    g_object_unref(audio_application_context->server_status);
    
    audio_application_context->server_status = NULL;
  }

  /* server thread */
  if(audio_application_context->server != NULL){    
    g_list_free_full(audio_application_context->server,
		     g_object_unref);

    audio_application_context->server = NULL;
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

  /* soundcard and sequencer */
  if(audio_application_context->soundcard != NULL){    
    g_list_free_full(audio_application_context->soundcard,
		     g_object_unref);

    audio_application_context->soundcard = NULL;
  }

  if(audio_application_context->sequencer != NULL){
    g_list_free_full(audio_application_context->sequencer,
		     g_object_unref);

    audio_application_context->sequencer = NULL;
  }

  /* audio */
  if(audio_application_context->audio != NULL){
    g_list_free_full(audio_application_context->audio,
		     g_object_unref);

    audio_application_context->audio = NULL;
  }

  /* sound server */
  if(audio_application_context->sound_server != NULL){
    g_list_free_full(audio_application_context->sound_server,
		     g_object_unref);

    audio_application_context->sound_server = NULL;
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

  /* thread pool */
  if(audio_application_context->thread_pool != NULL){
    g_object_unref(audio_application_context->thread_pool);
  }

  /* worker thread */
  if(audio_application_context->worker != NULL){    
    g_list_free_full(audio_application_context->worker,
		     g_object_unref);
  }

  /* server status */
  if(audio_application_context->server_status != NULL){
    g_object_unref(audio_application_context->server_status);
  }

  /* server thread */
  if(audio_application_context->server != NULL){    
    g_list_free_full(audio_application_context->server,
		     g_object_unref);
  }

  /* default soundcard */
  if(audio_application_context->default_soundcard != NULL){
    g_object_unref(audio_application_context->default_soundcard);
  }

  /* soundcard and export thread */
  if(audio_application_context->default_soundcard_thread != NULL){
    g_object_unref(audio_application_context->default_soundcard_thread);
  }

  if(audio_application_context->default_export_thread != NULL){
    g_object_unref(audio_application_context->default_export_thread);
  }

  /* soundcard and sequencer */
  if(audio_application_context->soundcard != NULL){    
    g_list_free_full(audio_application_context->soundcard,
		     g_object_unref);
  }

  if(audio_application_context->sequencer != NULL){
    g_list_free_full(audio_application_context->sequencer,
		     g_object_unref);
  }

  /* audio */
  if(audio_application_context->audio != NULL){
    g_list_free_full(audio_application_context->audio,
		     g_object_unref);
  }

  /* sound server */
  if(audio_application_context->sound_server != NULL){
    g_list_free_full(audio_application_context->sound_server,
		     g_object_unref);
  }

  /* osc server */
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

  GRecMutex *application_context_mutex;
  
  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(connectable);

  if(ags_application_context_test_flags(AGS_APPLICATION_CONTEXT(audio_application_context), AGS_APPLICATION_CONTEXT_CONNECTED)){
    return;
  }

  ags_audio_application_context_parent_connectable_interface->connect(connectable);

  /* get application context mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(audio_application_context);
    
  /* soundcard */
  soundcard = 
    start_soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(audio_application_context));
  
  while(soundcard != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(soundcard->data));

    soundcard = soundcard->next;
  }

  g_list_free_full(start_soundcard,
		   g_object_unref);
  
  /* sequencer */
  sequencer = 
    start_sequencer = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(audio_application_context->sequencer));

  while(sequencer != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(sequencer->data));

    sequencer = sequencer->next;
  }

  g_list_free_full(start_sequencer,
		   g_object_unref);
}

void
ags_audio_application_context_disconnect(AgsConnectable *connectable)
{
  AgsAudioApplicationContext *audio_application_context;

  GList *start_soundcard, *soundcard;
  GList *start_sequencer, *sequencer;

  GRecMutex *application_context_mutex;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(connectable);

  if(!ags_application_context_test_flags(AGS_APPLICATION_CONTEXT(audio_application_context), AGS_APPLICATION_CONTEXT_CONNECTED)){
    return;
  }

  ags_audio_application_context_parent_connectable_interface->disconnect(connectable);

  /* get application context mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(audio_application_context);
  
  /* soundcard */
  soundcard = 
    start_soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(audio_application_context));
  
  while(soundcard != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(soundcard->data));

    soundcard = soundcard->next;
  }

  g_list_free_full(start_soundcard,
		   g_object_unref);
  
  /* sequencer */
  sequencer = 
    start_sequencer = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(audio_application_context->sequencer));

  while(sequencer != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(sequencer->data));

    sequencer = sequencer->next;
  }

  g_list_free_full(start_sequencer,
		   g_object_unref);
}

AgsThread*
ags_audio_application_context_get_main_loop(AgsConcurrencyProvider *concurrency_provider)
{
  AgsThread *main_loop;

  AgsApplicationContext *application_context;
  
  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get main loop */
  g_rec_mutex_lock(application_context_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  if(main_loop != NULL){
    g_object_ref(main_loop);
  }
  
  g_rec_mutex_unlock(application_context_mutex);
  
  return(main_loop);
}

void
ags_audio_application_context_set_main_loop(AgsConcurrencyProvider *concurrency_provider,
					    AgsThread *main_loop)
{
  AgsApplicationContext *application_context;
  
  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get main loop */
  g_rec_mutex_lock(application_context_mutex);

  if(application_context->main_loop == main_loop){
    g_rec_mutex_unlock(application_context_mutex);
    
    return;
  }

  if(application_context->main_loop != NULL){
    g_object_unref(application_context->main_loop);
  }
  
  if(main_loop != NULL){
    g_object_ref(main_loop);
  }
  
  application_context->main_loop = (GObject *) main_loop;
  
  g_rec_mutex_unlock(application_context_mutex);
}

AgsTaskLauncher*
ags_audio_application_context_get_task_launcher(AgsConcurrencyProvider *concurrency_provider)
{
  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;
  
  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get main loop */
  g_rec_mutex_lock(application_context_mutex);

  task_launcher = (AgsThread *) application_context->task_launcher;

  if(task_launcher != NULL){
    g_object_ref(task_launcher);
  }
  
  g_rec_mutex_unlock(application_context_mutex);
  
  return(task_launcher);
}

void
ags_audio_application_context_set_task_launcher(AgsConcurrencyProvider *concurrency_provider,
						AgsTaskLauncher *task_launcher)
{
  AgsApplicationContext *application_context;
  
  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get main loop */
  g_rec_mutex_lock(application_context_mutex);

  if(application_context->task_launcher == task_launcher){
    g_rec_mutex_unlock(application_context_mutex);
    
    return;
  }

  if(application_context->task_launcher != NULL){
    g_object_unref(application_context->task_launcher);
  }
  
  if(task_launcher != NULL){
    g_object_ref(task_launcher);
  }
  
  application_context->task_launcher = (GObject *) task_launcher;
  
  g_rec_mutex_unlock(application_context_mutex);
}

AgsThreadPool*
ags_audio_application_context_get_thread_pool(AgsConcurrencyProvider *concurrency_provider)
{
  AgsThreadPool *thread_pool;
  
  AgsAudioApplicationContext *audio_application_context;
  
  GRecMutex *application_context_mutex;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(audio_application_context);

  /* get thread pool */
  g_rec_mutex_lock(application_context_mutex);

  thread_pool = audio_application_context->thread_pool;

  if(thread_pool != NULL){
    g_object_ref(thread_pool);
  }
  
  g_rec_mutex_unlock(application_context_mutex);
  
  return(thread_pool);
}

void
ags_audio_application_context_set_thread_pool(AgsConcurrencyProvider *concurrency_provider,
					      AgsThreadPool *thread_pool)
{
  AgsAudioApplicationContext *audio_application_context;
  
  GRecMutex *application_context_mutex;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(audio_application_context);

  /* get main loop */
  g_rec_mutex_lock(application_context_mutex);

  if(audio_application_context->thread_pool == thread_pool){
    g_rec_mutex_unlock(application_context_mutex);
    
    return;
  }

  if(audio_application_context->thread_pool != NULL){
    g_object_unref(audio_application_context->thread_pool);
  }
  
  if(thread_pool != NULL){
    g_object_ref(thread_pool);
  }
  
  audio_application_context->thread_pool = (GObject *) thread_pool;
  
  g_rec_mutex_unlock(application_context_mutex);
}

GList*
ags_audio_application_context_get_worker(AgsConcurrencyProvider *concurrency_provider)
{
  AgsAudioApplicationContext *audio_application_context;
  
  GList *worker;
  
  GRecMutex *application_context_mutex;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(audio_application_context);

  /* get worker */
  g_rec_mutex_lock(application_context_mutex);

  worker = g_list_copy_deep(audio_application_context->worker,
			    (GCopyFunc) g_object_ref,
			    NULL);
  
  g_rec_mutex_unlock(application_context_mutex);

  return(worker);
}

void
ags_audio_application_context_set_worker(AgsConcurrencyProvider *concurrency_provider,
					 GList *worker)
{
  AgsAudioApplicationContext *audio_application_context;
  
  GRecMutex *application_context_mutex;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(audio_application_context);

  /* set worker */
  g_rec_mutex_lock(application_context_mutex);

  if(audio_application_context->worker == worker){
    g_rec_mutex_unlock(application_context_mutex);
    
    return;
  }

  g_list_free_full(audio_application_context->worker,
		   (GDestroyNotify) g_object_unref);
  
  audio_application_context->worker = worker;

  g_rec_mutex_unlock(application_context_mutex);
}

gboolean
ags_audio_application_context_is_operating(AgsServiceProvider *service_provider)
{
  AgsAudioApplicationContext *audio_application_context;

  gboolean is_operating;
  
  GRecMutex *application_context_mutex;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(service_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(audio_application_context);

  /* get gui ready */
  g_rec_mutex_lock(application_context_mutex);

  is_operating = audio_application_context->is_operating;
   
  g_rec_mutex_unlock(application_context_mutex);

  return(is_operating);
}

AgsServerStatus*
ags_audio_application_context_server_status(AgsServiceProvider *service_provider)
{
  AgsServerStatus *server_status;
  
  AgsAudioApplicationContext *audio_application_context;
  
  GRecMutex *application_context_mutex;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(service_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(audio_application_context);

  /* get thread pool */
  g_rec_mutex_lock(application_context_mutex);

  server_status = audio_application_context->server_status;

  if(server_status != NULL){
    g_object_ref(server_status);
  }
  
  g_rec_mutex_unlock(application_context_mutex);
  
  return(server_status);
}

AgsRegistry*
ags_audio_application_context_get_registry(AgsServiceProvider *service_provider)
{
  AgsRegistry *registry;
  
  AgsAudioApplicationContext *audio_application_context;
  
  GRecMutex *application_context_mutex;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(service_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(audio_application_context);

  /* get thread pool */
  g_rec_mutex_lock(application_context_mutex);

  registry = audio_application_context->registry;

  if(registry != NULL){
    g_object_ref(registry);
  }
  
  g_rec_mutex_unlock(application_context_mutex);
  
  return(registry);
}

void
ags_audio_application_context_set_registry(AgsServiceProvider *service_provider,
					   AgsRegistry *registry)
{
  AgsAudioApplicationContext *audio_application_context;
  
  GRecMutex *application_context_mutex;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(service_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(audio_application_context);

  /* get main loop */
  g_rec_mutex_lock(application_context_mutex);

  if(audio_application_context->registry == registry){
    g_rec_mutex_unlock(application_context_mutex);
    
    return;
  }

  if(audio_application_context->registry != NULL){
    g_object_unref(audio_application_context->registry);
  }
  
  if(registry != NULL){
    g_object_ref(registry);
  }
  
  audio_application_context->registry = (GObject *) registry;
  
  g_rec_mutex_unlock(application_context_mutex);
}

GList*
ags_audio_application_context_get_server(AgsServiceProvider *service_provider)
{
  AgsAudioApplicationContext *audio_application_context;
  
  GList *server;
  
  GRecMutex *application_context_mutex;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(service_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(audio_application_context);

  /* get server */
  g_rec_mutex_lock(application_context_mutex);

  server = g_list_copy_deep(audio_application_context->server,
			    (GCopyFunc) g_object_ref,
			    NULL);
  
  g_rec_mutex_unlock(application_context_mutex);

  return(server);
}

void
ags_audio_application_context_set_server(AgsServiceProvider *service_provider,
					 GList *server)
{
  AgsAudioApplicationContext *audio_application_context;
  
  GRecMutex *application_context_mutex;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(service_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(audio_application_context);

  /* set server */
  g_rec_mutex_lock(application_context_mutex);

  if(audio_application_context->server == server){
    g_rec_mutex_unlock(application_context_mutex);
    
    return;
  }

  g_list_free_full(audio_application_context->server,
		   (GDestroyNotify) g_object_unref);
  
  audio_application_context->server = server;

  g_rec_mutex_unlock(application_context_mutex);
}

GObject*
ags_audio_application_context_get_default_soundcard_thread(AgsSoundProvider *sound_provider)
{
  GObject *soundcard_thread;
  
  AgsApplicationContext *application_context;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get default soundcard thread */
  g_rec_mutex_lock(application_context_mutex);

  soundcard_thread = (GObject *) AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->default_soundcard_thread;

  if(soundcard_thread != NULL){
    g_object_ref(soundcard_thread);
  }
  
  g_rec_mutex_unlock(application_context_mutex);
  
  return(soundcard_thread);
}

void
ags_audio_application_context_set_default_soundcard(AgsSoundProvider *sound_provider,
						    GObject *soundcard)
{
  AgsMessageDelivery *message_delivery;

  AgsApplicationContext *application_context;

  GList *start_message_queue;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* set default soundcard */
  g_rec_mutex_lock(application_context_mutex);

  if(AGS_AUDIO_APPLICATION_CONTEXT(application_context)->default_soundcard == soundcard){
    g_rec_mutex_unlock(application_context_mutex);

    return;
  }

  if(AGS_AUDIO_APPLICATION_CONTEXT(application_context)->default_soundcard != NULL){
    g_object_unref(AGS_AUDIO_APPLICATION_CONTEXT(application_context)->default_soundcard);
  }
  
  if(soundcard != NULL){
    g_object_ref(soundcard);
  }
  
  AGS_AUDIO_APPLICATION_CONTEXT(application_context)->default_soundcard = (GObject *) soundcard;

  g_rec_mutex_unlock(application_context_mutex);

  /* emit message */
  message_delivery = ags_message_delivery_get_instance();

  start_message_queue = ags_message_delivery_find_sender_namespace(message_delivery,
								   "libags-audio");

  if(start_message_queue != NULL){
    AgsMessageEnvelope *message;

    xmlDoc *doc;
    xmlNode *root_node;

    /* specify message body */
    doc = xmlNewDoc("1.0");

    root_node = xmlNewNode(NULL,
			   "ags-command");
    xmlDocSetRootElement(doc, root_node);    

    xmlNewProp(root_node,
	       "method",
	       "AgsSoundProvider::set-default-soundcard");

    /* add message */
    message = ags_message_envelope_new(G_OBJECT(sound_provider),
				       NULL,
				       doc);

    /* set parameter */
    message->n_params = 1;

    message->parameter_name = (gchar **) malloc(2 * sizeof(gchar *));
    message->value = g_new0(GValue,
			    1);

    /* audio channels */
    message->parameter_name[0] = "default-soundcard";
    
    g_value_init(&(message->value[0]),
		 G_TYPE_OBJECT);
    g_value_set_object(&(message->value[0]),
		       soundcard);

    /* terminate string vector */
    message->parameter_name[1] = NULL;
    
    /* add message */
    ags_message_delivery_add_message_envelope(message_delivery,
					      "libags-audio",
					      message);

    g_list_free_full(start_message_queue,
		     (GDestroyNotify) g_object_unref);
  }
}

GObject*
ags_audio_application_context_get_default_soundcard(AgsSoundProvider *sound_provider)
{
  GObject *soundcard;
  
  AgsApplicationContext *application_context;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get default soundcard */
  g_rec_mutex_lock(application_context_mutex);

  soundcard = (GObject *) AGS_AUDIO_APPLICATION_CONTEXT(application_context)->default_soundcard;

  if(soundcard != NULL){
    g_object_ref(soundcard);
  }
  
  g_rec_mutex_unlock(application_context_mutex);
  
  return(soundcard);
}

void
ags_audio_application_context_set_default_soundcard_thread(AgsSoundProvider *sound_provider,
							   GObject *soundcard_thread)
{
  AgsApplicationContext *application_context;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* set default soundcard thread */
  g_rec_mutex_lock(application_context_mutex);

  if(AGS_AUDIO_APPLICATION_CONTEXT(application_context)->default_soundcard_thread == (AgsThread *) soundcard_thread){
    g_rec_mutex_unlock(application_context_mutex);
  
    return;
  }

  if(AGS_AUDIO_APPLICATION_CONTEXT(application_context)->default_soundcard_thread != NULL){
    g_object_unref(AGS_AUDIO_APPLICATION_CONTEXT(application_context)->default_soundcard_thread);
  }
  
  if(soundcard_thread != NULL){
    g_object_ref(soundcard_thread);
  }
  
  AGS_AUDIO_APPLICATION_CONTEXT(application_context)->default_soundcard_thread = (AgsThread *) soundcard_thread;

  g_rec_mutex_unlock(application_context_mutex);
}

GList*
ags_audio_application_context_get_soundcard(AgsSoundProvider *sound_provider)
{
  AgsApplicationContext *application_context;

  GList *soundcard;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get soundcard */
  g_rec_mutex_lock(application_context_mutex);
  
  soundcard = g_list_copy_deep(AGS_AUDIO_APPLICATION_CONTEXT(application_context)->soundcard,
			       (GCopyFunc) g_object_ref,
			       NULL);

  g_rec_mutex_unlock(application_context_mutex);
  
  return(soundcard);
}

void
ags_audio_application_context_set_soundcard(AgsSoundProvider *sound_provider,
					    GList *soundcard)
{
  AgsApplicationContext *application_context;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* set soundcard */
  g_rec_mutex_lock(application_context_mutex);

  if(AGS_AUDIO_APPLICATION_CONTEXT(application_context)->soundcard == soundcard){
    g_rec_mutex_unlock(application_context_mutex);

    return;
  }
  
  g_list_free_full(AGS_AUDIO_APPLICATION_CONTEXT(application_context)->soundcard,
		   (GDestroyNotify) g_object_unref);

  AGS_AUDIO_APPLICATION_CONTEXT(application_context)->soundcard = soundcard;

  g_rec_mutex_unlock(application_context_mutex);
}

GList*
ags_audio_application_context_get_sequencer(AgsSoundProvider *sound_provider)
{
  AgsApplicationContext *application_context;

  GList *sequencer;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get sequencer */
  g_rec_mutex_lock(application_context_mutex);
  
  sequencer = g_list_copy_deep(AGS_AUDIO_APPLICATION_CONTEXT(application_context)->sequencer,
			       (GCopyFunc) g_object_ref,
			       NULL);

  g_rec_mutex_unlock(application_context_mutex);
  
  return(sequencer);
}

void
ags_audio_application_context_set_sequencer(AgsSoundProvider *sound_provider,
					    GList *sequencer)
{
  AgsApplicationContext *application_context;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* set sequencer */
  g_rec_mutex_lock(application_context_mutex);

  if(AGS_AUDIO_APPLICATION_CONTEXT(application_context)->sequencer == sequencer){
    g_rec_mutex_unlock(application_context_mutex);

    return;
  }

  g_list_free_full(AGS_AUDIO_APPLICATION_CONTEXT(application_context)->sequencer,
		   (GDestroyNotify) g_object_unref);
  
  AGS_AUDIO_APPLICATION_CONTEXT(application_context)->sequencer = sequencer;

  g_rec_mutex_unlock(application_context_mutex);
}

GList*
ags_audio_application_context_get_sound_server(AgsSoundProvider *sound_provider)
{
  AgsApplicationContext *application_context;

  GList *sound_server;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get sound server */
  g_rec_mutex_lock(application_context_mutex);
  
  sound_server = g_list_copy_deep(AGS_AUDIO_APPLICATION_CONTEXT(application_context)->sound_server,
				  (GCopyFunc) g_object_ref,
				  NULL);

  g_rec_mutex_unlock(application_context_mutex);
  
  return(sound_server);
}

void
ags_audio_application_context_set_sound_server(AgsSoundProvider *concurrency_provider,
					       GList *sound_server)
{
  AgsAudioApplicationContext *audio_application_context;
  
  GRecMutex *application_context_mutex;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(concurrency_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(audio_application_context);

  /* set sound_server */
  g_rec_mutex_lock(application_context_mutex);

  if(audio_application_context->sound_server == sound_server){
    g_rec_mutex_unlock(application_context_mutex);
    
    return;
  }

  g_list_free_full(audio_application_context->sound_server,
		   (GDestroyNotify) g_object_unref);
  
  audio_application_context->sound_server = sound_server;

  g_rec_mutex_unlock(application_context_mutex);
}

GList*
ags_audio_application_context_get_audio(AgsSoundProvider *sound_provider)
{
  AgsApplicationContext *application_context;

  GList *audio;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get audio */
  g_rec_mutex_lock(application_context_mutex);
  
  audio = g_list_copy_deep(AGS_AUDIO_APPLICATION_CONTEXT(application_context)->audio,
			   (GCopyFunc) g_object_ref,
			   NULL);

  g_rec_mutex_unlock(application_context_mutex);
  
  return(audio);
}

void
ags_audio_application_context_set_audio(AgsSoundProvider *sound_provider,
					GList *audio)
{
  AgsApplicationContext *application_context;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* set audio */
  g_rec_mutex_lock(application_context_mutex);

  if(AGS_AUDIO_APPLICATION_CONTEXT(application_context)->audio == audio){
    g_rec_mutex_unlock(application_context_mutex);

    return;
  }

  g_list_free_full(AGS_AUDIO_APPLICATION_CONTEXT(application_context)->audio,
		   (GDestroyNotify) g_object_unref);

  AGS_AUDIO_APPLICATION_CONTEXT(application_context)->audio = audio;

  g_rec_mutex_unlock(application_context_mutex);
}

GList*
ags_audio_application_context_get_osc_server(AgsSoundProvider *sound_provider)
{
  AgsApplicationContext *application_context;

  GList *osc_server;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get osc_server */
  g_rec_mutex_lock(application_context_mutex);
  
  osc_server = g_list_copy_deep(AGS_AUDIO_APPLICATION_CONTEXT(application_context)->osc_server,
				(GCopyFunc) g_object_ref,
				NULL);

  g_rec_mutex_unlock(application_context_mutex);
  
  return(osc_server);
}

void
ags_audio_application_context_set_osc_server(AgsSoundProvider *sound_provider,
					     GList *osc_server)
{
  AgsApplicationContext *application_context;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(sound_provider);
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* set osc_server */
  g_rec_mutex_lock(application_context_mutex);

  if(AGS_AUDIO_APPLICATION_CONTEXT(application_context)->osc_server == osc_server){
    g_rec_mutex_unlock(application_context_mutex);

    return;
  }

  g_list_free_full(AGS_AUDIO_APPLICATION_CONTEXT(application_context)->osc_server,
		   (GDestroyNotify) g_object_unref);

  AGS_AUDIO_APPLICATION_CONTEXT(application_context)->osc_server = osc_server;

  g_rec_mutex_unlock(application_context_mutex);
}

void
ags_audio_application_context_prepare(AgsApplicationContext *application_context)
{
  AgsAudioApplicationContext *audio_application_context;

  AgsThread *audio_loop;
  AgsTaskLauncher *task_launcher;

  GMainContext *server_main_context;
  GMainContext *audio_main_context;
  GMainContext *osc_server_main_context;
  GMainLoop *main_loop;
  GThread *main_loop_thread;

  GList *start_queue;
  
  audio_application_context = (AgsAudioApplicationContext *) application_context;

  /* call parent */
  //  AGS_APPLICATION_CONTEXT_CLASS(ags_audio_application_context_parent_class)->prepare(application_context);
  
  /* register types */
  ags_application_context_register_types(application_context);

  /*
   * fundamental thread setup
   */
  /* server main context and main loop */
  server_main_context = g_main_context_new();
  g_main_context_ref(server_main_context);

  audio_application_context->server_main_context = server_main_context;
  
  main_loop = g_main_loop_new(server_main_context,
			      TRUE);

  g_thread_new("Advanced Gtk+ Sequencer - server main loop",
	       ags_audio_application_context_server_main_loop_thread,
	       main_loop);
  
  /* audio main context and main loop */
  audio_main_context = g_main_context_new();
  g_main_context_ref(audio_main_context);

  audio_application_context->audio_main_context = audio_main_context;

  main_loop = g_main_loop_new(audio_main_context,
			      TRUE);

  g_thread_new("Advanced Gtk+ Sequencer - audio main loop",
	       ags_audio_application_context_audio_main_loop_thread,
	       main_loop);

  /* OSC server main context and main loop */
  osc_server_main_context = g_main_context_new();
  g_main_context_ref(osc_server_main_context);

  audio_application_context->osc_server_main_context = osc_server_main_context;

  g_main_loop_new(osc_server_main_context,
		  TRUE);

  /* AgsAudioLoop */
  audio_loop = (AgsThread *) ags_audio_loop_new();
  g_object_ref(audio_loop);
  
  application_context->main_loop = (GObject *) audio_loop;
  ags_connectable_connect(AGS_CONNECTABLE(audio_loop));

  /* AgsTaskLauncher */
  task_launcher = ags_task_launcher_new();
  g_object_ref(task_launcher);

  application_context->task_launcher = (GObject *) task_launcher;
  ags_connectable_connect(AGS_CONNECTABLE(task_launcher));  

#if 0  
  ags_task_launcher_attach(task_launcher,
			   audio_main_context);
#endif
  
  /* start audio loop and thread pool*/
  ags_thread_start(audio_loop);
  
  /* wait for audio loop */
  g_mutex_lock(AGS_THREAD_GET_START_MUTEX(audio_loop));

  if(!ags_thread_test_status_flags(audio_loop, AGS_THREAD_STATUS_START_DONE)){
    ags_thread_set_status_flags(audio_loop, AGS_THREAD_STATUS_START_WAIT);
      
    while(ags_thread_test_status_flags(audio_loop, AGS_THREAD_STATUS_START_WAIT) &&
	  !ags_thread_test_status_flags(audio_loop, AGS_THREAD_STATUS_START_DONE)){
      g_cond_wait(AGS_THREAD_GET_START_COND(audio_loop),
		  AGS_THREAD_GET_START_MUTEX(audio_loop));
    }
  }

  g_mutex_unlock(AGS_THREAD_GET_START_MUTEX(audio_loop));
}

void
ags_audio_application_context_setup(AgsApplicationContext *application_context)
{
  AgsAudioApplicationContext *audio_application_context;

  AgsJackServer *jack_server;
  AgsPulseServer *pulse_server;
  AgsCoreAudioServer *core_audio_server;

  AgsOscServer *osc_server;
  
  AgsLadspaManager *ladspa_manager;
  AgsDssiManager *dssi_manager;
  AgsLv2Manager *lv2_manager;
  AgsLv2uiManager *lv2ui_manager;
  AgsLv2WorkerManager *lv2_worker_manager;

#if defined(AGS_WITH_VST3)
  AgsVst3Manager *vst3_manager;
#endif
  
  AgsServer *server;

  AgsThread *main_loop;
  AgsThread *soundcard_thread;
  AgsThread *export_thread;
  AgsThread *sequencer_thread;
  AgsDestroyWorker *destroy_worker;
  AgsTaskLauncher *task_launcher;
  AgsMessageDelivery *message_delivery;
  AgsMessageQueue *message_queue;
  AgsMessageQueue *audio_message_queue;

  AgsLog *log;
  AgsConfig *config;

  GObject *soundcard;
  GObject *sequencer;

  GList *list;  

#ifndef AGS_W32API
  struct passwd *pw;

  uid_t uid;
#endif

  gchar *blacklist_path;
  gchar *blacklist_filename;
  gchar *server_group;
  gchar *soundcard_group;
  gchar *sequencer_group;
  gchar *osc_server_group;
  gchar *str;
  gchar *capability;
#if defined AGS_W32API
  gchar *app_dir;
#endif
    
  guint i;
  gboolean has_core_audio;
  gboolean has_pulse;
  gboolean has_jack;
  gboolean is_output;

  GRecMutex *application_context_mutex;
  
  audio_application_context = (AgsAudioApplicationContext *) application_context;
  
  /* get mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(audio_application_context);

  /* config and log */
  config = ags_config_get_instance();

  log = ags_log_get_instance();

  /* main loop and task launcher */
  main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

#if 0  //NOTE:JK: huh zombies might come to here
  atexit(ags_audio_application_context_signal_cleanup);

  /* Ignore interactive and job-control signals.  */
#ifndef AGS_W32API
  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGTTOU, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);

#ifndef AGS_W32API
  ags_sigact.sa_handler = ags_audio_application_context_signal_handler;
  sigemptyset(&ags_sigact.sa_mask);
  ags_sigact.sa_flags = 0;
  sigaction(SIGINT, &ags_sigact, (struct sigaction *) NULL);
  sigaction(SA_RESTART, &ags_sigact, (struct sigaction *) NULL);
#endif
#endif
#endif
  
  /* get user information */
#if defined AGS_W32API
  application_context = ags_application_context_get_instance();

  if(strlen(application_context->argv[0]) > strlen("gsequencer.exe")){
    app_dir = g_strndup(application_context->argv[0],
			strlen(application_context->argv[0]) - strlen("gsequencer.exe"));
  }else{
    app_dir = NULL;
  }
  
  blacklist_path = g_strdup_printf("%s\\%s",
				   g_get_current_dir(),
				   app_dir);

  g_free(app_dir);
#else
  uid = getuid();
  pw = getpwuid(uid);

  blacklist_path = g_strdup_printf("%s/%s",
				   pw->pw_dir,
				   AGS_DEFAULT_DIRECTORY);
#endif
  
  /* message delivery */
  message_delivery = ags_message_delivery_get_instance();

  message_queue = ags_message_queue_new("libags");
  ags_message_delivery_add_message_queue(message_delivery,
					 (GObject *) message_queue);
    
  /* load ladspa manager */
  ladspa_manager = ags_ladspa_manager_get_instance();

  blacklist_filename = g_strdup_printf("%s%c%s",
				       blacklist_path,
				       G_DIR_SEPARATOR,
				       "ladspa_plugin.blacklist");
  ags_ladspa_manager_load_blacklist(ladspa_manager,
				    blacklist_filename);

  /* load dssi manager */
  dssi_manager = ags_dssi_manager_get_instance();

  blacklist_filename = g_strdup_printf("%s%c%s",
				       blacklist_path,
				       G_DIR_SEPARATOR,
				       "dssi_plugin.blacklist");
  ags_dssi_manager_load_blacklist(dssi_manager,
				  blacklist_filename);

  /* load lv2 manager */
  lv2_manager = ags_lv2_manager_get_instance();
  lv2_worker_manager = ags_lv2_worker_manager_get_instance();    

  blacklist_filename = g_strdup_printf("%s%c%s",
				       blacklist_path,
				       G_DIR_SEPARATOR,
				       "lv2_plugin.blacklist");
  ags_lv2_manager_load_blacklist(lv2_manager,
				 blacklist_filename);

  /* load vst3 manager */
#if defined(AGS_WITH_VST3)
  vst3_manager = ags_vst3_manager_get_instance();

  blacklist_filename = g_strdup_printf("%s%c%s",
				       blacklist_path,
				       G_DIR_SEPARATOR,
				       "vst3_plugin.blacklist");
  ags_vst3_manager_load_blacklist(vst3_manager,
				  blacklist_filename);
#endif
  
  audio_application_context->start_loader = TRUE;
  
  /* launch audio */
  ags_log_add_message(log,
		      "* Launch audio");

  /* sound server */
  audio_application_context->sound_server = NULL;

  /* core audio server */
  core_audio_server = ags_core_audio_server_new(NULL);
  audio_application_context->sound_server = g_list_append(audio_application_context->sound_server,
							  core_audio_server);
  g_object_ref(G_OBJECT(core_audio_server));

  has_core_audio = FALSE;

  /* pulse server */
  pulse_server = ags_pulse_server_new(NULL);
  audio_application_context->sound_server = g_list_append(audio_application_context->sound_server,
							  pulse_server);
  g_object_ref(G_OBJECT(pulse_server));

  has_pulse = FALSE;
  
  /* jack server */
  jack_server = ags_jack_server_new(NULL);
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
    guint cache_buffer_size;
    gboolean use_cache;
    
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
	if(!is_output){
	  AgsJackClient *input_client;

	  g_object_get(jack_server,
		       "input-jack-client", &input_client,
		       NULL);

	  if(input_client == NULL){
	    input_client = ags_jack_client_new((GObject *) jack_server);
	    g_object_set(jack_server,
			 "input-jack-client", input_client,
			 NULL);
	    ags_jack_server_add_client(jack_server,
				       (GObject *) input_client);
    
	    ags_jack_client_open((AgsJackClient *) input_client,
				 "ags-input-client");	    
	  }else{
	    g_object_unref(input_client);
	  }
	}
	
	soundcard = ags_sound_server_register_soundcard(AGS_SOUND_SERVER(jack_server),
							is_output);
	
	has_jack = TRUE;
      }else if(!g_ascii_strncasecmp(str,
				    "alsa",
				    5)){
	if(is_output){
	  soundcard = (GObject *) ags_alsa_devout_new();
	}else{
	  soundcard = (GObject *) ags_alsa_devin_new();
	}
      }else if(!g_ascii_strncasecmp(str,
				    "wasapi",
				    7)){
	gchar *str;
	
	if(is_output){
	  soundcard = (GObject *) ags_wasapi_devout_new((GObject *) audio_application_context);	  

	  str = ags_config_get_value(config,
				     soundcard_group,
				     "wasapi-share-mode");

	  if(str != NULL &&
	     !g_ascii_strncasecmp(str,
				  "exclusive",
				  10)){
	    ags_wasapi_devout_set_flags(AGS_WASAPI_DEVOUT(soundcard),
					AGS_WASAPI_DEVOUT_SHARE_MODE_EXCLUSIVE);
	  }else{
	    ags_wasapi_devout_unset_flags(AGS_WASAPI_DEVOUT(soundcard),
					  AGS_WASAPI_DEVOUT_SHARE_MODE_EXCLUSIVE);
	  }

	  g_free(str);
	  
	  str = ags_config_get_value(config,
				     soundcard_group,
				     "wasapi-buffer-size");

	  if(str != NULL){
	    AGS_WASAPI_DEVOUT(soundcard)->wasapi_buffer_size = g_ascii_strtoull(str,
										NULL,
										10);
	    
	    g_free(str);
	  }
	}else{
	  soundcard = (GObject *) ags_wasapi_devin_new((GObject *) audio_application_context);

	  str = ags_config_get_value(config,
				     soundcard_group,
				     "wasapi-share-mode");

	  if(str != NULL &&
	     !g_ascii_strncasecmp(str,
				  "exclusive",
				  10)){
	    ags_wasapi_devin_set_flags(AGS_WASAPI_DEVIN(soundcard),
				       AGS_WASAPI_DEVIN_SHARE_MODE_EXCLUSIVE);
	  }else{
	    ags_wasapi_devin_unset_flags(AGS_WASAPI_DEVIN(soundcard),
					 AGS_WASAPI_DEVIN_SHARE_MODE_EXCLUSIVE);
	  }

	  g_free(str);
	  
	  str = ags_config_get_value(config,
				     soundcard_group,
				     "wasapi-buffer-size");

	  if(str != NULL){
	    AGS_WASAPI_DEVIN(soundcard)->wasapi_buffer_size = g_ascii_strtoull(str,
									       NULL,
									       10);
	    
	    g_free(str);
	  }
	}
      }else if(!g_ascii_strncasecmp(str,
				    "oss",
				    4)){
	if(is_output){
	  soundcard = (GObject *) ags_oss_devout_new();
	}else{
	  soundcard = (GObject *) ags_oss_devin_new();
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

    use_cache = TRUE;
    str = ags_config_get_value(config,
			       soundcard_group,
			       "use-cache");

    if(str != NULL &&
       !g_strncasecmp(str,
		      "false",
		      5)){
      use_cache = FALSE;
    }

    cache_buffer_size = 4096;
    str = ags_config_get_value(config,
			       soundcard_group,
			       "cache-buffer-size");

    if(str != NULL){
      cache_buffer_size = g_ascii_strtoull(str,
					   NULL,
					   10);
    }

    if(AGS_IS_PULSE_DEVOUT(soundcard)){
      GList *start_port, *port;

      g_object_get(soundcard,
		   "pulse-port", &start_port,
		   NULL);

      port = start_port;

      while(port != NULL){
	ags_pulse_port_set_samplerate(port->data,
				      samplerate);
	ags_pulse_port_set_pcm_channels(port->data,
					pcm_channels);
	ags_pulse_port_set_buffer_size(port->data,
				       buffer_size);
	ags_pulse_port_set_format(port->data,
				  format);
	ags_pulse_port_set_cache_buffer_size(port->data,
					     buffer_size * ceil(cache_buffer_size / buffer_size));
	
	port = port->next;
      }

      g_list_free_full(start_port,
		       g_object_unref);
    }else if(AGS_IS_CORE_AUDIO_DEVOUT(soundcard)){
      GList *start_port, *port;

      g_object_get(soundcard,
		   "core_audio-port", &start_port,
		   NULL);

      port = start_port;

      while(port != NULL){
	ags_core_audio_port_set_samplerate(port->data,
					   samplerate);
	ags_core_audio_port_set_pcm_channels(port->data,
					     pcm_channels);
	ags_core_audio_port_set_buffer_size(port->data,
					    buffer_size);
	ags_core_audio_port_set_format(port->data,
				       format);
	ags_core_audio_port_set_cache_buffer_size(port->data,
						  buffer_size * ceil(cache_buffer_size / buffer_size));
	
	port = port->next;
      }

      g_list_free_full(start_port,
		       g_object_unref);
    }
    
    g_free(soundcard_group);    
    soundcard_group = g_strdup_printf("%s-%d",
				      AGS_CONFIG_SOUNDCARD,
				      i);
  }

  if(audio_application_context->soundcard != NULL){
    soundcard = audio_application_context->soundcard->data;
  }  

  g_free(soundcard_group);

  ags_sound_provider_set_default_soundcard(AGS_SOUND_PROVIDER(audio_application_context),
					   soundcard);
  
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
	AgsJackClient *input_client;

	g_object_get(jack_server,
		     "input-jack-client", &input_client,
		     NULL);

	if(input_client == NULL){
	  input_client = ags_jack_client_new((GObject *) jack_server);
	  g_object_set(jack_server,
		       "input-jack-client", input_client,
		       NULL);
	  ags_jack_server_add_client(jack_server,
				     (GObject *) input_client);
    
	  ags_jack_client_open((AgsJackClient *) input_client,
			       "ags-input-client");	    
	}else{
	  g_object_unref(input_client);
	}

	sequencer = ags_sound_server_register_sequencer(AGS_SOUND_SERVER(jack_server),
							FALSE);

	has_jack = TRUE;
      }else if(!g_ascii_strncasecmp(str,
				    "alsa",
				    5)){
	sequencer = (GObject *) ags_alsa_midiin_new();
      }else if(!g_ascii_strncasecmp(str,
				    "oss",
				    4)){
	sequencer = (GObject *) ags_oss_midiin_new();
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
  audio_application_context->server = NULL;
  server = NULL;

  server_group = g_strdup("server");
  
  for(i = 0; ; i++){
    gchar *ip4, *ip6;

    guint server_port;
    gboolean auto_start;
    gboolean any_address;
    gboolean enable_ip4, enable_ip6;
    
    if(!g_key_file_has_group(config->key_file,
			     server_group)){
      if(i == 0){
	g_free(server_group);    
	server_group = g_strdup_printf("%s-%d",
				       AGS_CONFIG_SERVER,
				       i);
    	
	continue;
      }else{
	break;
      }
    }

    server = ags_server_new();

    audio_application_context->server = g_list_append(audio_application_context->server,
						      server);
    g_object_ref(server);

    /* realm */
    str = ags_config_get_value(config,
			       server_group,
			       "realm");
    
    if(str != NULL){
      g_object_set(server,
		   "realm", str,
		   NULL);
      
      g_free(str);
    }

    /* any address */
    any_address = FALSE;

    str = ags_config_get_value(config,
			       server_group,
			       "any-address");
    
    if(str != NULL){
      any_address = (!g_ascii_strncasecmp(str,
					  "true",
					  5)) ? TRUE: FALSE;
      g_free(str);
    }

    if(any_address){
      ags_server_set_flags(server,
			   (AGS_SERVER_ANY_ADDRESS));
    }

    /* enable ip4 and ip6 */
    enable_ip4 = FALSE;
    enable_ip6 = FALSE;

    str = ags_config_get_value(config,
			       server_group,
			       "enable-ip4");
    
    if(str != NULL){
      enable_ip4 = (!g_ascii_strncasecmp(str,
					 "true",
					 5)) ? TRUE: FALSE;
      g_free(str);
    }

    str = ags_config_get_value(config,
			       server_group,
			       "enable-ip6");

    if(str != NULL){
      enable_ip6 = (!g_ascii_strncasecmp(str,
					 "true",
					 5)) ? TRUE: FALSE;
      g_free(str);
    }

    if(enable_ip4){
      ags_server_set_flags(server,
			   (AGS_SERVER_INET4));
    }

    if(enable_ip6){
      ags_server_set_flags(server,
			   (AGS_SERVER_INET6));
    }

    /* ip4 and ip6 address */
    str = ags_config_get_value(config,
			       server_group,
			       "ip4-address");

    if(str != NULL){
      g_object_set(server,
		   "ip4", str,
		   NULL);
      
      g_free(str);
    }

    str = ags_config_get_value(config,
			       server_group,
			       "ip6-address");

    if(str != NULL){
      g_object_set(server,
		   "ip6", str,
		   NULL);
      
      g_free(str);
    }

    /* server port */
    str = ags_config_get_value(config,
			       server_group,
			       "server-port");

    if(str != NULL){
      server_port = (guint) g_ascii_strtoull(str,
					     NULL,
					     10);

      g_object_set(server,
		   "server-port", server_port,
		   NULL);
    }
    
    /* auto-start */
    auto_start = FALSE;
    
    str = ags_config_get_value(config,
			       server_group,
			       "auto-start");

    if(str != NULL){
      auto_start = (!g_ascii_strncasecmp(str,
					 "true",
					 5)) ? TRUE: FALSE;
      g_free(str);
    }

    if(auto_start){
      ags_server_set_flags(server, AGS_SERVER_AUTO_START);
    }

    g_free(server_group);    
    server_group = g_strdup_printf("%s-%d",
				   AGS_CONFIG_SERVER,
				   i);
  }

  if(server == NULL){
    server = ags_server_new();
    ags_server_set_flags(server,
			 (AGS_SERVER_INET4));

    audio_application_context->server = g_list_append(audio_application_context->server,
						      server);
    g_object_ref(server);
  }
  
  /* AgsSoundcardThread and AgsExportThread */
  audio_application_context->default_soundcard_thread = NULL;
  list = audio_application_context->soundcard;
    
  while(list != NULL){
    guint soundcard_capability;

    soundcard_capability = ags_soundcard_get_capability(AGS_SOUNDCARD(list->data));
    
    soundcard_thread = (AgsThread *) ags_soundcard_thread_new(list->data,
							      soundcard_capability);
    ags_thread_add_child_extended(main_loop,
				  (AgsThread *) soundcard_thread,
				  TRUE, TRUE);

    /* export thread */
    export_thread = NULL;
    
    /* export thread */
    if(AGS_IS_ALSA_DEVOUT(list->data) ||
       AGS_IS_OSS_DEVOUT(list->data) ||
       AGS_IS_WASAPI_DEVOUT(list->data) ||
       AGS_IS_JACK_DEVOUT(list->data) ||
       AGS_IS_PULSE_DEVOUT(list->data) ||
       AGS_IS_CORE_AUDIO_DEVOUT(list->data)){
      export_thread = (AgsThread *) ags_export_thread_new(list->data,
							  NULL);
      ags_thread_add_child_extended(main_loop,
				    (AgsThread *) export_thread,
				    TRUE, TRUE);
    }    

    /* default soundcard thread */
    if(audio_application_context->default_soundcard_thread == NULL){
      ags_sound_provider_set_default_soundcard_thread(AGS_SOUND_PROVIDER(audio_application_context),
						      (GObject *) soundcard_thread);
    }

    /* default export thread */
    if(export_thread != NULL){
      if(audio_application_context->default_export_thread == NULL){
	audio_application_context->default_export_thread = export_thread;
      }else{
	g_object_unref(export_thread);
      }
    }

    /* iterate */
    list = list->next;      
  }
  
  /* AgsSequencerThread */
  list = audio_application_context->sequencer;
    
  while(list != NULL){
    sequencer_thread = (AgsThread *) ags_sequencer_thread_new(list->data);
    ags_thread_add_child_extended(main_loop,
				  (AgsThread *) sequencer_thread,
				  TRUE, TRUE);

    list = list->next;      
  }

  /* AgsOscServer */
  audio_application_context->osc_server = NULL;
  osc_server = NULL;

  osc_server_group = g_strdup("osc-server");
  
  for(i = 0; ; i++){
    gchar *ip4, *ip6;

    guint server_port;
    gboolean auto_start;
    gboolean any_address;
    gboolean enable_ip4, enable_ip6;
    
    if(!g_key_file_has_group(config->key_file,
			     osc_server_group)){
      if(i == 0){
	g_free(osc_server_group);    
	osc_server_group = g_strdup_printf("%s-%d",
					   AGS_CONFIG_OSC_SERVER,
					   i);
    	
	continue;
      }else{
	break;
      }
    }

    osc_server = ags_osc_server_new();
    ags_osc_server_set_flags(osc_server,
			     (AGS_OSC_SERVER_TCP));

    audio_application_context->osc_server = g_list_append(audio_application_context->osc_server,
							  osc_server);
    g_object_ref(osc_server);

    /* any address */
    any_address = FALSE;

    str = ags_config_get_value(config,
			       osc_server_group,
			       "any-address");
    
    if(str != NULL){
      any_address = (!g_ascii_strncasecmp(str,
					  "true",
					  5)) ? TRUE: FALSE;
      g_free(str);
    }

    if(any_address){
      ags_osc_server_set_flags(osc_server,
			       (AGS_OSC_SERVER_ANY_ADDRESS));
    }

    /* enable ip4 and ip6 */
    enable_ip4 = FALSE;
    enable_ip6 = FALSE;

    str = ags_config_get_value(config,
			       osc_server_group,
			       "enable-ip4");
    
    if(str != NULL){
      enable_ip4 = (!g_ascii_strncasecmp(str,
					 "true",
					 5)) ? TRUE: FALSE;
      g_free(str);
    }

    str = ags_config_get_value(config,
			       osc_server_group,
			       "enable-ip6");

    if(str != NULL){
      enable_ip6 = (!g_ascii_strncasecmp(str,
					 "true",
					 5)) ? TRUE: FALSE;
      g_free(str);
    }

    if(enable_ip4){
      ags_osc_server_set_flags(osc_server,
			       (AGS_OSC_SERVER_INET4));
    }

    if(enable_ip6){
      ags_osc_server_set_flags(osc_server,
			       (AGS_OSC_SERVER_INET6));
    }
    
    ags_osc_server_add_default_controller(osc_server);

    /* ip4 and ip6 address */
    str = ags_config_get_value(config,
			       osc_server_group,
			       "ip4-address");

    if(str != NULL){
      g_object_set(osc_server,
		   "ip4", str,
		   NULL);
      
      g_free(str);
    }

    str = ags_config_get_value(config,
			       osc_server_group,
			       "ip6-address");

    if(str != NULL){
      g_object_set(osc_server,
		   "ip6", str,
		   NULL);
      
      g_free(str);
    }

    /* server port */
    str = ags_config_get_value(config,
			       osc_server_group,
			       "server-port");

    if(str != NULL){
      server_port = (guint) g_ascii_strtoull(str,
					     NULL,
					     10);

      g_object_set(osc_server,
		   "server-port", server_port,
		   NULL);
    }
    
    /* auto-start */
    auto_start = FALSE;
    
    str = ags_config_get_value(config,
			       osc_server_group,
			       "auto-start");

    if(str != NULL){
      auto_start = (!g_ascii_strncasecmp(str,
					 "true",
					 5)) ? TRUE: FALSE;
      g_free(str);
    }

    if(auto_start){
      ags_osc_server_set_flags(osc_server, AGS_OSC_SERVER_AUTO_START);
    }

    g_free(osc_server_group);    
    osc_server_group = g_strdup_printf("%s-%d",
				       AGS_CONFIG_OSC_SERVER,
				       i);
  }

  if(osc_server == NULL){
    osc_server = ags_osc_server_new();
    ags_osc_server_set_flags(osc_server,
			     (AGS_OSC_SERVER_INET4 |
			      AGS_OSC_SERVER_TCP));

    ags_osc_server_add_default_controller(osc_server);

    audio_application_context->osc_server = g_list_append(audio_application_context->osc_server,
							  osc_server);
    g_object_ref(osc_server);
  }
  
  /* AgsWorkerThread */
  audio_application_context->worker = NULL;

  /* AgsDestroyWorker */
  destroy_worker = ags_destroy_worker_get_instance();
  ags_thread_add_child_extended(main_loop,
				(AgsThread *) destroy_worker,
				TRUE, TRUE);
  audio_application_context->worker = g_list_prepend(audio_application_context->worker,
						     destroy_worker);
  ags_thread_start((AgsThread *) destroy_worker);
  
  /* AgsThreadPool */
  audio_application_context->thread_pool = NULL;

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

  /* set operating */
  g_rec_mutex_lock(application_context_mutex);

  audio_application_context->setup_ready = TRUE;
  
  audio_application_context->is_operating = TRUE;

  g_rec_mutex_unlock(application_context_mutex);
  
  /* unref */
  g_object_unref(main_loop);
  
  g_object_unref(task_launcher);
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
  GList *start_list, *list;

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
  list =
    start_list = ags_sound_provider_get_sound_server(AGS_SOUND_PROVIDER(application_context));
  
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

  g_list_free_full(start_list,
		   g_object_unref);
  
  /* retrieve pulseaudio server */
  list =
    start_list = ags_sound_provider_get_sound_server(AGS_SOUND_PROVIDER(application_context));
  
  while((list = ags_list_util_find_type(list,
					AGS_TYPE_PULSE_SERVER)) != NULL){
    pulse_server = list->data;

#ifdef AGS_WITH_PULSE
    pa_mainloop_quit(pulse_server->main_loop,
		     0);
#endif
 
    list = list->next;
  }
  
  g_list_free_full(start_list,
		   g_object_unref);

  /* retrieve JACK server */
  list =
    start_list = ags_sound_provider_get_sound_server(AGS_SOUND_PROVIDER(application_context));
  
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

  g_list_free_full(start_list,
		   g_object_unref);

  exit(0);
}

void
ags_audio_application_context_read(AgsFile *file, xmlNode *node, GObject **application_context)
{
  //TODO:JK: implement me
}

xmlNode*
ags_audio_application_context_write(AgsFile *file, xmlNode *parent, GObject *application_context)
{
  xmlNode *node;

  node = NULL;

  //TODO:JK: implement me

  return(node);
}

void*
ags_audio_application_context_server_main_loop_thread(GMainLoop *main_loop)
{
  AgsApplicationContext *application_context;

  GList *start_list, *list;

#ifdef AGS_WITH_RT
  AgsPriority *priority;

  struct sched_param param;

  gchar *str;
#endif

  g_main_context_push_thread_default(g_main_loop_get_context(main_loop));
  
  application_context = ags_application_context_get_instance();
  
  while(!ags_service_provider_is_operating(AGS_SERVICE_PROVIDER(application_context))){
    g_usleep(G_USEC_PER_SEC / 30);
  }

  /* real-time setup */
#ifdef AGS_WITH_RT
  priority = ags_priority_get_instance();  

  param.sched_priority = 15;

  str = ags_priority_get_value(priority,
			       AGS_PRIORITY_RT_THREAD,
			       AGS_PRIORITY_KEY_SERVER_MAIN_LOOP);

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
#endif

  list = 
    start_list = ags_service_provider_get_server(AGS_SERVICE_PROVIDER(application_context));

  while(list != NULL){
    if(ags_server_test_flags(list->data, AGS_SERVER_AUTO_START)){
      ags_server_start(AGS_SERVER(list->data));
    }
    
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
  
  g_main_loop_run(main_loop);

  g_thread_exit(NULL);

  return(NULL);
}

void*
ags_audio_application_context_audio_main_loop_thread(GMainLoop *main_loop)
{
  AgsApplicationContext *application_context;

  GList *start_list, *list;

#ifdef AGS_WITH_RT
  AgsPriority *priority;

  struct sched_param param;

  gchar *str;
#endif

  g_main_context_push_thread_default(g_main_loop_get_context(main_loop));
  
  application_context = ags_application_context_get_instance();

  while(!ags_service_provider_is_operating(AGS_SERVICE_PROVIDER(application_context))){
    g_usleep(G_USEC_PER_SEC / 30);
  }

  /* real-time setup */
#ifdef AGS_WITH_RT
  priority = ags_priority_get_instance();  

  param.sched_priority = 95;

  str = ags_priority_get_value(priority,
			       AGS_PRIORITY_RT_THREAD,
			       AGS_PRIORITY_KEY_AUDIO_MAIN_LOOP);

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
#endif
  
  list = 
    start_list = ags_sound_provider_get_osc_server(AGS_SERVICE_PROVIDER(application_context));

  while(list != NULL){
    if(ags_osc_server_test_flags(list->data, AGS_OSC_SERVER_AUTO_START)){
      ags_osc_server_start(AGS_OSC_SERVER(list->data));
    }
    
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);

  g_main_loop_run(main_loop);

  g_thread_exit(NULL);

  return(NULL);
}

gboolean
ags_audio_application_context_loader_timeout(AgsAudioApplicationContext *audio_application_context)
{
  AgsLadspaManager *ladspa_manager;
  AgsDssiManager *dssi_manager;
  AgsLv2Manager *lv2_manager;
  AgsLv2TurtleScanner *lv2_turtle_scanner;  

#if defined(AGS_WITH_VST3)
  AgsVst3Manager *vst3_manager;
#endif
  
  AgsLog *log;

#if !defined(AGS_W32API)
  struct utsname buf;
#endif
  
  gchar **ladspa_path;
  gchar **dssi_path;
  gchar **lv2_path;
  gchar **vst3_path;

  gchar *sysname;

  gint64 start_time;
  gint64 current_time;
  gboolean initial_load;

  GError *error;

  if(!audio_application_context->start_loader){
    return(TRUE);
  }
  
  log = ags_log_get_instance();
  
  if(audio_application_context->loader_ready &&
     audio_application_context->ladspa_loader == NULL &&
     audio_application_context->dssi_loader == NULL &&
     audio_application_context->lv2_loader == NULL &&
     audio_application_context->vst3_loader == NULL){
    ags_log_add_message(log,
			"* Launch audio application context");

    while(!audio_application_context->setup_ready){
      g_main_context_iteration(NULL,
			       FALSE);
    }
    
    /* stop animation */
    if(audio_application_context->setup_ready){
//      ags_ui_provider_set_show_animation(AGS_UI_PROVIDER(audio_application_context), FALSE);
    }
    
    return(FALSE);
  }

  start_time = g_get_monotonic_time();

  initial_load = TRUE;

#if !defined(AGS_W32API)
  uname(&buf);

  sysname = g_ascii_strdown(buf.sysname,
			    -1);
#else
  sysname = g_strdup("win");
#endif
  
  /* get plugin managers */
  ladspa_manager = ags_ladspa_manager_get_instance();
  
  dssi_manager = ags_dssi_manager_get_instance();
  
  lv2_manager = ags_lv2_manager_get_instance();
  
#if defined(AGS_WITH_VST3)
  vst3_manager = ags_vst3_manager_get_instance();
#endif
  
  ladspa_path = ags_ladspa_manager_get_default_path();

  dssi_path = ags_dssi_manager_get_default_path();

  lv2_path = ags_lv2_manager_get_default_path();

#if defined(AGS_WITH_VST3)
  vst3_path = ags_vst3_manager_get_default_path();
#endif
  
  if(audio_application_context->lv2_turtle_scanner == NULL){
    audio_application_context->lv2_turtle_scanner = ags_lv2_turtle_scanner_new();
  }
  
  lv2_turtle_scanner = audio_application_context->lv2_turtle_scanner;
    
  if(!audio_application_context->loader_ready){
    AgsBasePlugin *plugin;
    
    GDir *dir;

    gchar *filename;

    /* prepare loader - ladspa */
    while(*ladspa_path != NULL){
      if(!g_file_test(*ladspa_path,
		      G_FILE_TEST_EXISTS)){
	ladspa_path++;
      
	continue;
      }
    
      error = NULL;
      dir = g_dir_open(*ladspa_path,
		       0,
		       &error);

      if(error != NULL){
	g_warning("%s", error->message);

	ladspa_path++;

	g_error_free(error);
      
	continue;
      }

      while((filename = g_dir_read_name(dir)) != NULL){
	if(g_str_has_suffix(filename,
			    AGS_LIBRARY_SUFFIX) &&
	   !g_list_find_custom(ladspa_manager->ladspa_plugin_blacklist,
			       filename,
			       g_strcmp0)){
	  audio_application_context->ladspa_loader = g_list_prepend(audio_application_context->ladspa_loader,
								    g_strdup_printf("%s%c%s",
										    ladspa_path[0],
										    G_DIR_SEPARATOR,
										    filename));
	}
      }
    
      ladspa_path++;
    }    

    audio_application_context->ladspa_loader = g_list_reverse(audio_application_context->ladspa_loader);

    /* prepare loader - dssi */
    while(*dssi_path != NULL){
      if(!g_file_test(*dssi_path,
		      G_FILE_TEST_EXISTS)){
	dssi_path++;
      
	continue;
      }
    
      error = NULL;
      dir = g_dir_open(*dssi_path,
		       0,
		       &error);

      if(error != NULL){
	g_warning("%s", error->message);

	dssi_path++;

	g_error_free(error);
      
	continue;
      }

      while((filename = g_dir_read_name(dir)) != NULL){
	if(g_str_has_suffix(filename,
			    AGS_LIBRARY_SUFFIX) &&
	   !g_list_find_custom(dssi_manager->dssi_plugin_blacklist,
			       filename,
			       g_strcmp0)){
	  audio_application_context->dssi_loader = g_list_prepend(audio_application_context->dssi_loader,
								  g_strdup_printf("%s%c%s",
										  dssi_path[0],
										  G_DIR_SEPARATOR,
										  filename));
	}
      }
    
      dssi_path++;
    }    

    audio_application_context->dssi_loader = g_list_reverse(audio_application_context->dssi_loader);	

    /* prepare loader - lv2 */    
    while(*lv2_path != NULL){
      gchar *plugin_path;
      
      if(!g_file_test(*lv2_path,
		      G_FILE_TEST_EXISTS)){
	lv2_path++;
      
	continue;
      }
    
      error = NULL;
      dir = g_dir_open(*lv2_path,
		       0,
		       &error);

      if(error != NULL){
	g_warning("%s", error->message);

	lv2_path++;

	g_error_free(error);
      
	continue;
      }

      while((filename = g_dir_read_name(dir)) != NULL){
	if(!g_ascii_strncasecmp(filename,
				"..",
				3) ||
	   !g_ascii_strncasecmp(filename,
				".",
				2)){
	  continue;
	}

	plugin_path = g_strdup_printf("%s%c%s",
				      lv2_path[0],
				      G_DIR_SEPARATOR,
				      filename);
      
	if(g_file_test(plugin_path,
		       G_FILE_TEST_IS_DIR) &&
	   !g_list_find_custom(lv2_manager->lv2_plugin_blacklist,
			       filename,
			       g_strcmp0)){
	  audio_application_context->lv2_loader = g_list_prepend(audio_application_context->lv2_loader,
								 g_strdup(plugin_path));
	}
      }
            
      lv2_path++;
    }

    audio_application_context->lv2_loader = g_list_reverse(audio_application_context->lv2_loader);

#if defined(AGS_WITH_VST3)
    while(vst3_path[0] != NULL){    
      gchar *plugin_path;
      
      if(!g_file_test(vst3_path[0],
		      G_FILE_TEST_EXISTS)){
	vst3_path++;
      
	continue;
      }

      error = NULL;
      dir = g_dir_open(vst3_path[0],
		       0,
		       &error);

      if(error != NULL){
	g_warning("%s", error->message);

	vst3_path++;

	g_error_free(error);
      
	continue;
      }


      while((filename = g_dir_read_name(dir)) != NULL){
	if(!g_ascii_strncasecmp(filename,
				"..",
				3) ||
	   !g_ascii_strncasecmp(filename,
				".",
				2)){
	  continue;
	}
      
	plugin_path = g_strdup_printf("%s%c%s",
				      vst3_path[0],
				      G_DIR_SEPARATOR,
				      filename);
      
	if(g_file_test(plugin_path,
		       G_FILE_TEST_IS_DIR) &&
	   !g_list_find_custom(vst3_manager->vst3_plugin_blacklist,
			       filename,
			       g_strcmp0)){
	  audio_application_context->vst3_loader = g_list_prepend(audio_application_context->vst3_loader,
								  g_strdup(plugin_path));
	}
      }
      
      vst3_path++;
    }

    audio_application_context->vst3_loader = g_list_reverse(audio_application_context->vst3_loader);
#endif
    
    audio_application_context->loader_ready = TRUE;
  }
  
  /* load ladspa */
  current_time = g_get_monotonic_time();
  
  if(current_time < start_time + AGS_AUDIO_APPLICATION_CONTEXT_DEFAULT_LOADER_INTERVAL &&
     !audio_application_context->ladspa_loading){
    ags_log_add_message(log,
			"* Loading LADSPA plugins");

    audio_application_context->ladspa_loading = TRUE;
  }
  
  while(audio_application_context->ladspa_loader != NULL){
    gchar *loader_filename;
    gchar *path;
    gchar *filename;
    
    current_time = g_get_monotonic_time();
    
    if(!initial_load &&
       current_time > start_time + AGS_AUDIO_APPLICATION_CONTEXT_DEFAULT_LOADER_INTERVAL){
      break;
    }

    loader_filename = audio_application_context->ladspa_loader->data;

    path = g_path_get_dirname(loader_filename);
    filename = g_path_get_basename(loader_filename);

    if(path != NULL && filename != NULL){
      ags_ladspa_manager_load_file(ladspa_manager,
				   path,
				   filename);
    }
    
    audio_application_context->ladspa_loader = g_list_remove(audio_application_context->ladspa_loader,
							     loader_filename);
    
    initial_load = FALSE;
  }

  /* load dssi */
  current_time = g_get_monotonic_time();
  
  if(current_time < start_time + AGS_AUDIO_APPLICATION_CONTEXT_DEFAULT_LOADER_INTERVAL &&
     !audio_application_context->dssi_loading){
    ags_log_add_message(log,
			"* Loading DSSI plugins");

    audio_application_context->dssi_loading = TRUE;
  }
  
  while(audio_application_context->dssi_loader != NULL){
    gchar *loader_filename;
    gchar *path;
    gchar *filename;

    current_time = g_get_monotonic_time();
    
    if(!initial_load &&
       current_time > start_time + AGS_AUDIO_APPLICATION_CONTEXT_DEFAULT_LOADER_INTERVAL){
      break;
    }

    loader_filename = audio_application_context->dssi_loader->data;

    path = g_path_get_dirname(loader_filename);
    filename = g_path_get_basename(loader_filename);
    
    if(path != NULL && filename != NULL){
      ags_dssi_manager_load_file(dssi_manager,
				 path,
				 filename);
    }
    
    audio_application_context->dssi_loader = g_list_remove(audio_application_context->dssi_loader,
							   loader_filename);
    
    initial_load = FALSE;
  }

  /* load lv2 */
  current_time = g_get_monotonic_time();
  
  if(current_time < start_time + AGS_AUDIO_APPLICATION_CONTEXT_DEFAULT_LOADER_INTERVAL &&
     !audio_application_context->lv2_loading){
    ags_log_add_message(log,
			"* Loading LV2 plugins");

    audio_application_context->lv2_loading = TRUE;
  }
  
  while(audio_application_context->lv2_loader != NULL){
    gchar *loader_filename;
    
    current_time = g_get_monotonic_time();
    
    if(!initial_load &&
       current_time > start_time + AGS_AUDIO_APPLICATION_CONTEXT_DEFAULT_LOADER_INTERVAL){
      break;
    }

    loader_filename = audio_application_context->lv2_loader->data;

    if(g_file_test(loader_filename,
		   G_FILE_TEST_IS_DIR)){
      AgsLv2TurtleParser *lv2_turtle_parser;
	
      AgsTurtle *manifest;
      AgsTurtle **turtle;

      GList *start_list, *list;

      gchar *manifest_filename;
	
      guint n_turtle;
	
      manifest_filename = g_strdup_printf("%s%c%s",
					  loader_filename,
					  G_DIR_SEPARATOR,
					  "manifest.ttl");

      if(!g_file_test(manifest_filename,
		      G_FILE_TEST_EXISTS)){
	audio_application_context->lv2_loader = g_list_remove(audio_application_context->lv2_loader,
							      loader_filename);

	g_free(manifest_filename);
	  
	continue;
      }

      g_message("quick scan turtle [Manifest] - %s", manifest_filename);
	
      ags_lv2_turtle_scanner_quick_scan(lv2_turtle_scanner,
					manifest_filename);
      
      g_free(manifest_filename);
      
    }
      
    audio_application_context->lv2_loader = g_list_remove(audio_application_context->lv2_loader,
							  loader_filename);
    
    initial_load = FALSE;
  }

  /* load vst3 */
#if defined(AGS_WITH_VST3)
  current_time = g_get_monotonic_time();
  
  if(current_time < start_time + AGS_AUDIO_APPLICATION_CONTEXT_DEFAULT_LOADER_INTERVAL &&
     !audio_application_context->vst3_loading){
    ags_log_add_message(log,
			"* Loading VST3 plugins");

    audio_application_context->vst3_loading = TRUE;
  }

  while(audio_application_context->vst3_loader != NULL){
    GDir *arch_dir;

    gchar *arch_path;
    gchar *loader_filename;
    gchar *arch_filename;
    gchar *path;
    gchar *filename;

    current_time = g_get_monotonic_time();
    
    if(!initial_load &&
       current_time > start_time + AGS_AUDIO_APPLICATION_CONTEXT_DEFAULT_LOADER_INTERVAL){
      break;
    }

    loader_filename = audio_application_context->vst3_loader->data;

#if defined(AGS_OSXAPI)
    arch_path = g_strdup(loader_filename);
#else
    arch_path = g_strdup_printf("%s%cContents%c%s-%s",
				loader_filename,
				G_DIR_SEPARATOR,
				G_DIR_SEPARATOR,
#if defined(__x86_64__)
				"x86_64",
#elif defined(__i386__)
				"i386",
#elif defined(__aarch64__)
				"arm64",
#elif defined(__arm__)
				"arm",
#elif defined(__alpha__)
				"alpha",
#elif defined(__hppa__)
				"hppa",
#elif defined(__m68k__)
				"m68000",
#elif defined(__mips__)
				"mips",
#elif defined(__ppc__)
				"ppc",
#elif defined(__s390x__)
				"s390x",
#else
				"unknown"
#endif
				sysname);	
#endif

    if(g_file_test(arch_path,
		   G_FILE_TEST_IS_DIR)){
      error = NULL;
      arch_dir = g_dir_open(arch_path,
			    0,
			    &error);

      if(error != NULL){
	g_warning("%s", error->message);

	audio_application_context->vst3_loader = g_list_remove(audio_application_context->vst3_loader,
							       loader_filename);

	g_free(arch_path);

	g_error_free(error);
	
	continue;
      }

      while((arch_filename = g_dir_read_name(arch_dir)) != NULL){
#if defined(AGS_OSXAPI)
	if(!g_list_find_custom(vst3_manager->vst3_plugin_blacklist,
			       arch_filename,
			       g_strcmp0)){
	  ags_vst3_manager_load_file(vst3_manager,
				     arch_path,
				     arch_filename);
	}
#else
	if(g_str_has_suffix(arch_filename,
			    AGS_LIBRARY_SUFFIX) &&
	   !g_list_find_custom(vst3_manager->vst3_plugin_blacklist,
			       arch_filename,
			       strcmp)){
	  ags_vst3_manager_load_file(vst3_manager,
				     arch_path,
				     arch_filename);
	}
#endif
      }
    }

    audio_application_context->vst3_loader = g_list_remove(audio_application_context->vst3_loader,
							   loader_filename);

    g_free(arch_path);
    
    initial_load = FALSE;
  }
#endif
  
  return(TRUE);
}

AgsAudioApplicationContext*
ags_audio_application_context_new()
{
  AgsAudioApplicationContext *audio_application_context;

  audio_application_context = (AgsAudioApplicationContext *) g_object_new(AGS_TYPE_AUDIO_APPLICATION_CONTEXT,
									  NULL);

  return(audio_application_context);
}
