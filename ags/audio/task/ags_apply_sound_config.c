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

#include <ags/audio/task/ags_apply_sound_config.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_devin.h>
#include <ags/audio/ags_midiin.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>
#include <ags/audio/thread/ags_sequencer_thread.h>
#include <ags/audio/thread/ags_audio_thread.h>
#include <ags/audio/thread/ags_channel_thread.h>
#include <ags/audio/thread/ags_export_thread.h>

#include <ags/audio/task/ags_notify_soundcard.h>

#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_client.h>
#include <ags/audio/jack/ags_jack_devout.h>
#include <ags/audio/jack/ags_jack_devin.h>
#include <ags/audio/jack/ags_jack_midiin.h>

#include <ags/audio/pulse/ags_pulse_server.h>
#include <ags/audio/pulse/ags_pulse_client.h>
#include <ags/audio/pulse/ags_pulse_devout.h>
#include <ags/audio/pulse/ags_pulse_devin.h>

#include <ags/audio/core-audio/ags_core_audio_server.h>
#include <ags/audio/core-audio/ags_core_audio_client.h>
#include <ags/audio/core-audio/ags_core_audio_devout.h>
#include <ags/audio/core-audio/ags_core_audio_devin.h>
#include <ags/audio/core-audio/ags_core_audio_midiin.h>

#include <math.h>

#include <ags/i18n.h>

void ags_apply_sound_config_class_init(AgsApplySoundConfigClass *apply_sound_config);
void ags_apply_sound_config_init(AgsApplySoundConfig *apply_sound_config);
void ags_apply_sound_config_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_apply_sound_config_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_apply_sound_config_dispose(GObject *gobject);
void ags_apply_sound_config_finalize(GObject *gobject);

void ags_apply_sound_config_change_max_precision(AgsThread *thread,
						 gdouble max_precision);
void ags_apply_sound_config_launch(AgsTask *task);

/**
 * SECTION:ags_apply_sound_config
 * @short_description: apply sound config
 * @title: AgsApplySoundConfig
 * @section_id:
 * @include: ags/audio/task/ags_apply_sound_config.h
 *
 * The #AgsApplySoundConfig task apply the specified sound config.
 */

enum{
  PROP_0,
  PROP_CONFIG_DATA,
  PROP_APPLICATION_CONTEXT,
};

static gpointer ags_apply_sound_config_parent_class = NULL;

GType
ags_apply_sound_config_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_apply_sound_config = 0;

    static const GTypeInfo ags_apply_sound_config_info = {
      sizeof(AgsApplySoundConfigClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_apply_sound_config_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsApplySoundConfig),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_apply_sound_config_init,
    };

    ags_type_apply_sound_config = g_type_register_static(AGS_TYPE_TASK,
							 "AgsApplySoundConfig",
							 &ags_apply_sound_config_info,
							 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_apply_sound_config);
  }

  return g_define_type_id__volatile;
}

void
ags_apply_sound_config_class_init(AgsApplySoundConfigClass *apply_sound_config)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;
  
  ags_apply_sound_config_parent_class = g_type_class_peek_parent(apply_sound_config);

  /* GObjectClass */
  gobject = (GObjectClass *) apply_sound_config;

  gobject->set_property = ags_apply_sound_config_set_property;
  gobject->get_property = ags_apply_sound_config_get_property;

  gobject->dispose = ags_apply_sound_config_dispose;
  gobject->finalize = ags_apply_sound_config_finalize;

  /* properties */
  /**
   * AgsApplySoundConfig:config-data:
   *
   * The assigned sound config data as string.
   * 
   * Since: 2.1.0
   */
  param_spec = g_param_spec_string("config-data",
				   i18n_pspec("sound config data"),
				   i18n_pspec("The sound config data to apply"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONFIG_DATA,
				  param_spec);

  /**
   * AgsStartSoundcard:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 2.1.0
   */
  param_spec = g_param_spec_object("application-context",
				   i18n_pspec("application context of start soundcard"),
				   i18n_pspec("The application context of start soundcard task"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /* AgsTaskClass */
  task = (AgsTaskClass *) apply_sound_config;

  task->launch = ags_apply_sound_config_launch;
}

void
ags_apply_sound_config_init(AgsApplySoundConfig *apply_sound_config)
{
  apply_sound_config->config_data = NULL;
}

void
ags_apply_sound_config_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsApplySoundConfig *apply_sound_config;

  apply_sound_config = AGS_APPLY_SOUND_CONFIG(gobject);

  switch(prop_id){
  case PROP_CONFIG_DATA:
    {
      gchar *config_data;

      config_data = g_value_get_string(value);

      if(config_data == apply_sound_config->config_data){
	return;
      }

      g_free(apply_sound_config->config_data);
      
      apply_sound_config->config_data = g_strdup(config_data);
    }
    break;
  case PROP_APPLICATION_CONTEXT:
    {
      GObject *application_context;

      application_context = (GObject *) g_value_get_object(value);

      if(apply_sound_config->application_context == (GObject *) application_context){
	return;
      }

      if(apply_sound_config->application_context != NULL){
	g_object_unref(apply_sound_config->application_context);
      }

      if(application_context != NULL){
	g_object_ref(application_context);
      }

      apply_sound_config->application_context = (GObject *) application_context;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_apply_sound_config_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsApplySoundConfig *apply_sound_config;

  apply_sound_config = AGS_APPLY_SOUND_CONFIG(gobject);

  switch(prop_id){
  case PROP_CONFIG_DATA:
    {
      g_value_set_string(value,
			 apply_sound_config->config_data);
    }
    break;
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, apply_sound_config->application_context);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_apply_sound_config_dispose(GObject *gobject)
{
  AgsApplySoundConfig *apply_sound_config;

  apply_sound_config = AGS_APPLY_SOUND_CONFIG(gobject);

  if(apply_sound_config->application_context != NULL){
    g_object_unref(apply_sound_config->application_context);

    apply_sound_config->application_context = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_apply_sound_config_parent_class)->dispose(gobject);
}

void
ags_apply_sound_config_finalize(GObject *gobject)
{
  AgsApplySoundConfig *apply_sound_config;

  apply_sound_config = AGS_APPLY_SOUND_CONFIG(gobject);

  if(apply_sound_config->application_context != NULL){
    g_object_unref(apply_sound_config->application_context);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_apply_sound_config_parent_class)->finalize(gobject);
}

void
ags_apply_sound_config_change_max_precision(AgsThread *thread,
					    gdouble max_precision)
{
  AgsThread *current;
  
  g_object_set(thread,
	       "max-precision", max_precision,
	       NULL);

  current = g_atomic_pointer_get(&(thread->children));

  while(current != NULL){
    ags_apply_sound_config_change_max_precision(current,
						max_precision);
    
    current = g_atomic_pointer_get(&(thread->next));
  }
}

void
ags_apply_sound_config_launch(AgsTask *task)
{
  AgsApplySoundConfig *apply_sound_config;

  AgsJackServer *jack_server;
  AgsPulseServer *pulse_server;
  AgsCoreAudioServer *core_audio_server;

  AgsThread *audio_loop;
  AgsThread *soundcard_thread;
  AgsThread *export_thread;
  AgsThread *sequencer_thread;
  AgsThread *audio_thread;
  AgsThread *channel_thread;

  AgsApplicationContext *application_context;
  AgsConfig *config;

  GObject *soundcard;
  GObject *sequencer;

  GList *start_sound_server, *sound_server;
  GList *start_orig_soundcard, *orig_soundcard;
  GList *start_orig_sequencer, *orig_sequencer;
  GList *start_audio, *audio;
  GList *start_list, *list;

  gchar *soundcard_group;
  gchar *sequencer_group;
  gchar *capability;
  gchar *str;

  gdouble frequency;
  guint samplerate;
  guint buffer_size;
  guint i;
  gboolean has_core_audio;
  gboolean has_pulse;
  gboolean has_jack;
  gboolean is_output;
  
  static const guint staging_flags = (AGS_SOUND_STAGING_CANCEL |
				      AGS_SOUND_STAGING_REMOVE);

  apply_sound_config = AGS_APPLY_SOUND_CONFIG(task);

  application_context = apply_sound_config->application_context;

  g_object_get(application_context,
	       "main-loop", &audio_loop,
	       NULL);
  
  start_orig_soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  start_orig_sequencer = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));
  
  config = ags_config_get_instance();

  if(apply_sound_config->config_data != NULL){
    ags_config_clear(config);
    
    ags_config_load_from_data(config,
			      apply_sound_config->config_data, -1);
  }

  jack_server = NULL;
  pulse_server = NULL;
  core_audio_server = NULL;
  
  sound_server = 
    start_sound_server = ags_sound_provider_get_sound_server(AGS_SOUND_PROVIDER(application_context));

  while(sound_server != NULL){
    if(AGS_IS_JACK_SERVER(sound_server->data)){
      jack_server = sound_server->data;
    }else if(AGS_IS_PULSE_SERVER(sound_server->data)){
      pulse_server = sound_server->data;
    }else if(AGS_IS_CORE_AUDIO_SERVER(sound_server->data)){
      core_audio_server = sound_server->data;
    }
    
    sound_server = sound_server->next;
  }
  
  has_core_audio = FALSE;
  has_pulse = FALSE;
  has_jack = FALSE;

  /* stop all playback */
  ags_thread_find_type(audio_loop,
		       AGS_TYPE_SOUNDCARD_THREAD);

  while(soundcard_thread != NULL){
    if(AGS_IS_SOUNDCARD_THREAD(soundcard_thread)){
      ags_thread_stop(soundcard_thread);
    }
    
    soundcard_thread = g_atomic_pointer_get(&(soundcard_thread->next));
  }

  ags_thread_find_type(audio_loop,
		       AGS_TYPE_EXPORT_THREAD);

  while(export_thread != NULL){
    if(AGS_IS_EXPORT_THREAD(export_thread)){
      ags_thread_stop(export_thread);
    }
    
    export_thread = g_atomic_pointer_get(&(export_thread->next));
  }

  ags_thread_find_type(audio_loop,
		       AGS_TYPE_SEQUENCER_THREAD);

  while(sequencer_thread != NULL){
    if(AGS_IS_SEQUENCER_THREAD(sequencer_thread)){
      ags_thread_stop(sequencer_thread);
    }
    
    sequencer_thread = g_atomic_pointer_get(&(sequencer_thread->next));
  }

  audio = 
    start_audio = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));

  while(audio != NULL){
    AgsPlaybackDomain *playback_domain;

    GList *start_output, *output;
    GList *start_input, *input;

    gint sound_scope;
    
    g_object_get(audio->data,
		 "playback-domain", &playback_domain,
		 NULL);
    
    g_object_get(playback_domain,
		 "output-playback", &start_output,
		 "input-playback", &start_input,
		 NULL);
    
    for(sound_scope = 0; sound_scope < AGS_SOUND_SCOPE_LAST; sound_scope++){
      /* stop audio thread */
      audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							  sound_scope);

      if(audio_thread != NULL){
	/* cancel */
	ags_audio_recursive_run_stage(audio->data,
				      sound_scope, staging_flags);
      
	ags_thread_stop(audio_thread);
      }

      /* output */
      output = start_output;

      while(output != NULL){
	AgsChannel *channel;

	g_object_get(output->data,
		     "channel", &channel,
		     NULL);

	channel_thread = ags_playback_get_channel_thread(output->data,
							 sound_scope);

	ags_playback_set_recall_id(output->data,
				   NULL,
				   sound_scope);

	if(channel_thread != NULL){
	  ags_thread_stop(channel_thread);
	}
      
	/* iterate */
	output = output->next;
      }

      /* input */
      input = start_input;

      while(input != NULL){
	AgsChannel *channel;

	g_object_get(input->data,
		     "channel", &channel,
		     NULL);

	channel_thread = ags_playback_get_channel_thread(input->data,
							 sound_scope);

	ags_playback_set_recall_id(input->data,
				   NULL,
				   sound_scope);

	if(channel_thread != NULL){
	  ags_thread_stop(channel_thread);
	}
      
	/* iterate */
	input = input->next;
      }
    }

    ags_audio_loop_remove_audio(audio_loop,
				(GObject *) audio->data);

    g_list_free(start_output);
    g_list_free(start_input);
    
    audio = audio->next;
  }
  
  /* stop sound server */
  if(jack_server != NULL){
    list = jack_server->client;

    while(list != NULL){
#ifdef AGS_WITH_JACK      
      jack_client_close(AGS_JACK_CLIENT(list->data)->client);

      ags_jack_client_deactivate(list->data);
#endif

      list = list->next;
    }
  }

  if(pulse_server != NULL){
#ifdef AGS_WITH_PULSE
    AgsPulseClient *pulse_client;

    g_object_get(pulse_server,
		 "default-pulse-client", &pulse_client,
		 NULL);
    
    ags_pulse_client_deactivate(pulse_client);
    
    pa_mainloop_quit(pulse_server->main_loop,
		     0);
#endif
  }
  
  if(core_audio_server != NULL){
    list = core_audio_server->client;

    while(list != NULL){
      ags_core_audio_client_deactivate(list->data);

      list = list->next;
    }
  }
  
  /* remove all soundcard, export and sequencer threads */
  while((soundcard_thread = ags_thread_find_type(audio_loop,
						 AGS_TYPE_SOUNDCARD_THREAD)) != NULL){
    ags_thread_remove_child(audio_loop,
			    soundcard_thread);

    g_object_run_dispose(soundcard_thread);
    g_object_unref(soundcard_thread);

    if(ags_sound_provider_get_default_soundcard_thread(AGS_SOUND_PROVIDER(application_context)) == soundcard_thread){
      ags_sound_provider_set_default_soundcard_thread(AGS_SOUND_PROVIDER(application_context),
						      NULL);
      g_object_unref(soundcard_thread);
    }
  }
    
  while((export_thread = ags_thread_find_type(audio_loop,
					      AGS_TYPE_EXPORT_THREAD)) != NULL){
    ags_thread_remove_child(audio_loop,
			    export_thread);

    g_object_run_dispose(export_thread);
    g_object_unref(export_thread);
  }

  while((sequencer_thread = ags_thread_find_type(audio_loop,
						 AGS_TYPE_SEQUENCER_THREAD)) != NULL){
    ags_thread_remove_child(audio_loop,
			    sequencer_thread);

    g_object_run_dispose(sequencer_thread);
    g_object_unref(sequencer_thread);
  }
  
  /* unregister soundcard and sequencer */
  orig_soundcard = start_orig_soundcard;
  
  while(orig_soundcard){
    if(AGS_IS_JACK_DEVOUT(orig_soundcard->data) || AGS_IS_JACK_DEVIN(orig_soundcard->data)){
      ags_sound_server_unregister_soundcard(AGS_SOUND_SERVER(jack_server),
					    orig_soundcard->data);
    }else if(AGS_IS_PULSE_DEVOUT(orig_soundcard->data)){
      ags_sound_server_unregister_soundcard(AGS_SOUND_SERVER(pulse_server),
					    orig_soundcard->data);
    }else if(AGS_IS_CORE_AUDIO_DEVOUT(orig_soundcard->data) || AGS_IS_CORE_AUDIO_DEVIN(orig_soundcard->data)){
      ags_sound_server_unregister_soundcard(AGS_SOUND_SERVER(core_audio_server),
					    orig_soundcard->data);
    }

    orig_soundcard = orig_soundcard->next;
  }

  orig_sequencer = start_orig_sequencer;
  
  while(orig_sequencer){
    if(AGS_IS_JACK_MIDIIN(orig_sequencer->data)){
      ags_sound_server_unregister_sequencer(AGS_SOUND_SERVER(jack_server),
					    orig_sequencer->data);
    }else if(AGS_IS_CORE_AUDIO_MIDIIN(orig_sequencer->data)){
      ags_sound_server_unregister_sequencer(AGS_SOUND_SERVER(core_audio_server),
					    orig_sequencer->data);
    }
    
    orig_soundcard = orig_soundcard->next;
  }

  ags_sound_provider_set_default_soundcard(AGS_SOUND_PROVIDER(application_context),
					   NULL);

  ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(application_context),
				   NULL);
  ags_sound_provider_set_sequencer(AGS_SOUND_PROVIDER(application_context),
				   NULL);

  /* run dispose */
  orig_soundcard = start_orig_soundcard;
  
  while(orig_soundcard != NULL){
    AgsNotifySoundcard *notify_soundcard;

    if(AGS_IS_DEVOUT(orig_soundcard->data)){
      notify_soundcard = AGS_DEVOUT(orig_soundcard->data)->notify_soundcard;
    }else if(AGS_IS_JACK_DEVOUT(orig_soundcard->data)){
      notify_soundcard = AGS_JACK_DEVOUT(orig_soundcard->data)->notify_soundcard;
    }else if(AGS_IS_PULSE_DEVOUT(orig_soundcard->data)){
      notify_soundcard = AGS_PULSE_DEVOUT(orig_soundcard->data)->notify_soundcard;
    }else if(AGS_IS_CORE_AUDIO_DEVOUT(orig_soundcard->data)){
      notify_soundcard = AGS_CORE_AUDIO_DEVOUT(orig_soundcard->data)->notify_soundcard;
    }else if(AGS_IS_DEVIN(orig_soundcard->data)){
      notify_soundcard = AGS_DEVIN(orig_soundcard->data)->notify_soundcard;
    }else if(AGS_IS_JACK_DEVIN(orig_soundcard->data)){
      notify_soundcard = AGS_JACK_DEVIN(orig_soundcard->data)->notify_soundcard;
    }else if(AGS_IS_PULSE_DEVIN(orig_soundcard->data)){
      notify_soundcard = AGS_PULSE_DEVIN(orig_soundcard->data)->notify_soundcard;
    }else if(AGS_IS_CORE_AUDIO_DEVIN(orig_soundcard->data)){
      notify_soundcard = AGS_CORE_AUDIO_DEVIN(orig_soundcard->data)->notify_soundcard;
    }

    ags_task_thread_remove_cyclic_task(application_context->task_thread,
				       notify_soundcard);
    
    g_object_run_dispose(orig_soundcard->data);
    g_object_unref(orig_soundcard->data);
    
    orig_soundcard = orig_soundcard->next;
  }

  g_list_free(start_orig_soundcard);

  orig_sequencer = start_orig_sequencer;
  
  while(orig_sequencer != NULL){
    g_object_run_dispose(orig_sequencer->data);
    g_object_unref(orig_sequencer->data);
    
    orig_sequencer = orig_sequencer->next;
  }

  g_list_free(start_orig_sequencer);
  
  /* read config */
  str = ags_config_get_value(config,
			     AGS_CONFIG_THREAD,
			     "max-precision");
  
  if(str != NULL){
    gdouble max_precision;
    
    /* change max precision */
    max_precision = g_ascii_strtod(str,
				   NULL);
    
    ags_apply_sound_config_change_max_precision(audio_loop,
						max_precision);  
  }

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
	  soundcard = (GObject *) ags_devout_new((GObject *) application_context);
	  
	  AGS_DEVOUT(soundcard)->flags &= (~AGS_DEVOUT_OSS);
	  AGS_DEVOUT(soundcard)->flags |= AGS_DEVOUT_ALSA;
	}else{
	  soundcard = (GObject *) ags_devin_new((GObject *) application_context);
	  
	  AGS_DEVIN(soundcard)->flags &= (~AGS_DEVIN_OSS);
	  AGS_DEVIN(soundcard)->flags |= AGS_DEVIN_ALSA;
	}
      }else if(!g_ascii_strncasecmp(str,
				    "oss",
				    4)){
	if(is_output){
	  soundcard = (GObject *) ags_devout_new((GObject *) application_context);

	  AGS_DEVOUT(soundcard)->flags &= (~AGS_DEVOUT_ALSA);
	  AGS_DEVOUT(soundcard)->flags |= AGS_DEVOUT_OSS;
	}else{
	  soundcard = (GObject *) ags_devin_new((GObject *) application_context);

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

    if(ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context)) == NULL){
      ags_sound_provider_set_default_soundcard(AGS_SOUND_PROVIDER(application_context),
					       soundcard);
      
      g_object_set(audio_loop,
		   "default-output-soundcard", G_OBJECT(soundcard),
		   NULL);
    }
    
    ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(application_context),
				     g_list_append(ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context)),
						   soundcard));
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

  g_free(soundcard_group);

  /* AgsSequencer */
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
	sequencer = (GObject *) ags_midiin_new((GObject *) application_context);
	AGS_MIDIIN(sequencer)->flags &= (~AGS_MIDIIN_OSS);
	AGS_MIDIIN(sequencer)->flags |= AGS_MIDIIN_ALSA;
      }else if(!g_ascii_strncasecmp(str,
				    "oss",
				    4)){
	sequencer = (GObject *) ags_midiin_new((GObject *) application_context);
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
    
    ags_sound_provider_set_sequencer(AGS_SOUND_PROVIDER(application_context),
				     g_list_append(ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context)),
						   sequencer));
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

  /* create threads */
  list = 
    start_list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

  while(list != NULL){
    AgsNotifySoundcard *notify_soundcard;

    guint soundcard_capability;

    soundcard_capability = ags_soundcard_get_capability(AGS_SOUNDCARD(list->data));
    
    soundcard_thread = (AgsThread *) ags_soundcard_thread_new(list->data,
							      soundcard_capability);
    ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				  (AgsThread *) soundcard_thread,
				  TRUE, TRUE);

    /* notify soundcard and export thread */
    export_thread = NULL;
    
    notify_soundcard = ags_notify_soundcard_new(soundcard_thread);
    AGS_TASK(notify_soundcard)->task_thread = application_context->task_thread;
    
    if(AGS_IS_DEVOUT(list->data)){
      AGS_DEVOUT(list->data)->notify_soundcard = notify_soundcard;
    }else if(AGS_IS_JACK_DEVOUT(list->data)){
      AGS_JACK_DEVOUT(list->data)->notify_soundcard = notify_soundcard;
    }else if(AGS_IS_PULSE_DEVOUT(list->data)){
      AGS_PULSE_DEVOUT(list->data)->notify_soundcard = notify_soundcard;
    }else if(AGS_IS_CORE_AUDIO_DEVOUT(list->data)){
      AGS_CORE_AUDIO_DEVOUT(list->data)->notify_soundcard = notify_soundcard;
    }else if(AGS_IS_DEVIN(list->data)){
      AGS_DEVIN(list->data)->notify_soundcard = notify_soundcard;
    }else if(AGS_IS_JACK_DEVIN(list->data)){
      AGS_JACK_DEVIN(list->data)->notify_soundcard = notify_soundcard;
    }else if(AGS_IS_PULSE_DEVIN(list->data)){
      AGS_PULSE_DEVIN(list->data)->notify_soundcard = notify_soundcard;
    }else if(AGS_IS_CORE_AUDIO_DEVIN(list->data)){
      AGS_CORE_AUDIO_DEVIN(list->data)->notify_soundcard = notify_soundcard;
    }

    ags_task_thread_append_cyclic_task(application_context->task_thread,
				       notify_soundcard);

    /* export thread */
    if(AGS_IS_DEVOUT(list->data) ||
       AGS_IS_JACK_DEVOUT(list->data) ||
       AGS_IS_PULSE_DEVOUT(list->data) ||
       AGS_IS_CORE_AUDIO_DEVOUT(list->data)){
      export_thread = (AgsThread *) ags_export_thread_new(list->data,
							  NULL);
      ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				    (AgsThread *) export_thread,
				    TRUE, TRUE);
    }    

    /* default soundcard thread */
    if(ags_sound_provider_get_default_soundcard_thread(AGS_SOUND_PROVIDER(application_context)) == NULL){
      ags_sound_provider_set_default_soundcard_thread(AGS_SOUND_PROVIDER(application_context),
						      soundcard_thread);
      g_object_ref(soundcard_thread);
    }

    list = list->next;
  }
  
  list = 
    start_list = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));
      
  while(list != NULL){
    sequencer_thread = (AgsThread *) ags_sequencer_thread_new(list->data);
    ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				  (AgsThread *) sequencer_thread,
				  TRUE, TRUE);

    list = list->next;      
  }

  /* change frequency */
  soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    NULL,
			    &samplerate,
			    &buffer_size,
			    NULL);

  frequency = ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;
  ags_main_loop_change_frequency(AGS_MAIN_LOOP(audio_loop),
				 frequency);
  
  /* reset audio's soundcard */
  audio = start_audio;

  while(audio != NULL){
    g_object_set(audio->data,
		 "output-soundcard", soundcard,
		 NULL);

    audio = audio->next;
  }

  /* launch */
  if(has_core_audio){
    ags_core_audio_client_open((AgsCoreAudioClient *) core_audio_server->default_client,
			       "ags-default-client");

    ags_core_audio_server_connect_client(core_audio_server);
  }

  if(has_pulse){
    ags_pulse_client_open((AgsPulseClient *) pulse_server->default_client,
			  "ags-default-client");

    ags_pulse_server_connect_client(pulse_server);

    ags_pulse_server_start_poll(pulse_server);
  }

  if(has_jack){
    ags_jack_client_open((AgsJackClient *) jack_server->default_client,
			 "ags-default-client");

    ags_jack_server_connect_client(jack_server);
  }

  g_list_free(start_sound_server);
  g_list_free(start_audio);  
}

/**
 * ags_apply_sound_config_new:
 * @application_context: the #AgsApplicationContext
 * @config_data: the config file as string data
 *
 * Creates a new instance of #AgsApplySoundConfig.
 *
 * Returns: the new #AgsApplySoundConfig.
 *
 * Since: 2.1.0
 */
AgsApplySoundConfig*
ags_apply_sound_config_new(AgsApplicationContext *application_context,
			   gchar *config_data)
{
  AgsApplySoundConfig *apply_sound_config;

  apply_sound_config = (AgsApplySoundConfig *) g_object_new(AGS_TYPE_APPLY_SOUND_CONFIG,
							    "application-context", application_context,
							    "config-data", config_data,
							    NULL);

  return(apply_sound_config);
}
