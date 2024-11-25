/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2_plugin.h>
#include <ags/plugin/ags_lv2_option_manager.h>
#include <ags/plugin/ags_lv2_urid_manager.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>

#include <ags/audio/file/ags_audio_file_link.h>
#include <ags/audio/file/ags_audio_container.h>
#include <ags/audio/file/ags_audio_file.h>
#include <ags/audio/file/ags_sound_container.h>
#include <ags/audio/file/ags_sound_resource.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>
#include <ags/audio/thread/ags_sequencer_thread.h>
#include <ags/audio/thread/ags_audio_thread.h>
#include <ags/audio/thread/ags_channel_thread.h>
#include <ags/audio/thread/ags_export_thread.h>

#include <ags/audio/alsa/ags_alsa_devout.h>
#include <ags/audio/alsa/ags_alsa_devin.h>
#include <ags/audio/alsa/ags_alsa_midiin.h>

#include <ags/audio/oss/ags_oss_devout.h>
#include <ags/audio/oss/ags_oss_devin.h>
#include <ags/audio/oss/ags_oss_midiin.h>

#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_client.h>
#include <ags/audio/jack/ags_jack_port.h>
#include <ags/audio/jack/ags_jack_devout.h>
#include <ags/audio/jack/ags_jack_devin.h>
#include <ags/audio/jack/ags_jack_midiin.h>

#include <ags/audio/pulse/ags_pulse_server.h>
#include <ags/audio/pulse/ags_pulse_client.h>
#include <ags/audio/pulse/ags_pulse_port.h>
#include <ags/audio/pulse/ags_pulse_devout.h>
#include <ags/audio/pulse/ags_pulse_devin.h>

#include <ags/audio/wasapi/ags_wasapi_devout.h>
#include <ags/audio/wasapi/ags_wasapi_devin.h>

#include <ags/audio/core-audio/ags_core_audio_server.h>
#include <ags/audio/core-audio/ags_core_audio_client.h>
#include <ags/audio/core-audio/ags_core_audio_port.h>
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
void ags_apply_sound_config_soundcard_presets(AgsTask *task,
					      GType soundcard_type, guint soundcard_count);
void ags_apply_sound_config_sequencer_presets(AgsTask *task,
					      GType sequencer_type, guint sequencer_count);
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
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("config-data",
				   i18n_pspec("sound config data"),
				   i18n_pspec("The sound config data to apply"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONFIG_DATA,
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
  
  /* call parent */
  G_OBJECT_CLASS(ags_apply_sound_config_parent_class)->dispose(gobject);
}

void
ags_apply_sound_config_finalize(GObject *gobject)
{
  AgsApplySoundConfig *apply_sound_config;

  apply_sound_config = AGS_APPLY_SOUND_CONFIG(gobject);

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

  current = ags_atomic_pointer_get(&(thread->children));

  while(current != NULL){
    ags_apply_sound_config_change_max_precision(current,
						max_precision);
    
    current = ags_atomic_pointer_get(&(thread->next));
  }
}

void
ags_apply_sound_config_soundcard_presets(AgsTask *task,
					 GType soundcard_type, guint soundcard_count)
{
  AgsJackServer *jack_server;
  AgsPulseServer *pulse_server;
  AgsCoreAudioServer *core_audio_server;

  AgsThread *audio_loop;
  AgsThread *soundcard_thread;
  AgsThread *export_thread;

  AgsApplicationContext *application_context;
  AgsConfig *config;

  GObject *io_soundcard;
  
  GList *start_sound_server, *sound_server;
  GList *start_soundcard, *soundcard;

  gchar *soundcard_group;
  gchar *backend;
  gchar *capability;
  gchar *str;

  guint available_soundcard_count;
  guint pcm_channels, buffer_size, samplerate, format;
  guint cache_buffer_size;
  guint nth_card;
  guint i, i_stop;
  guint j;
  gboolean do_grow;
  gboolean is_output;
  gboolean use_cache;

  application_context = ags_application_context_get_instance();

  config = ags_config_get_instance();

  start_soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

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

  backend = NULL;

  if(soundcard_type == AGS_TYPE_ALSA_DEVOUT ||
     soundcard_type == AGS_TYPE_ALSA_DEVIN){
    backend = "alsa";
  }else if(soundcard_type == AGS_TYPE_OSS_DEVOUT ||
	   soundcard_type == AGS_TYPE_OSS_DEVIN){
    backend = "oss";
  }else if(soundcard_type == AGS_TYPE_PULSE_DEVOUT ||
	   soundcard_type == AGS_TYPE_PULSE_DEVIN){
    backend = "pulse";
  }else if(soundcard_type == AGS_TYPE_JACK_DEVOUT ||
	   soundcard_type == AGS_TYPE_JACK_DEVIN){
    backend = "jack";
  }else if(soundcard_type == AGS_TYPE_CORE_AUDIO_DEVOUT ||
	   soundcard_type == AGS_TYPE_CORE_AUDIO_DEVIN){
    backend = "core-audio";
  }else if(soundcard_type == AGS_TYPE_WASAPI_DEVOUT ||
	   soundcard_type == AGS_TYPE_WASAPI_DEVIN){
    backend = "wasapi";
  }
  
  if(backend == NULL){
    g_warning("apply config - unknown soundcard backend");
    
    return;
  }
  
  /* backend - device output */
  soundcard = start_soundcard;

  available_soundcard_count = 0;
  
  while(soundcard != NULL){
    if(soundcard_type == AGS_TYPE_ALSA_DEVOUT && AGS_IS_ALSA_DEVOUT(soundcard->data)){
      available_soundcard_count++;
    }else if(soundcard_type == AGS_TYPE_ALSA_DEVIN && AGS_IS_ALSA_DEVIN(soundcard->data)){
      available_soundcard_count++;
    }else if(soundcard_type == AGS_TYPE_OSS_DEVOUT && AGS_IS_OSS_DEVOUT(soundcard->data)){
      available_soundcard_count++;
    }else if(soundcard_type == AGS_TYPE_OSS_DEVIN && AGS_IS_OSS_DEVIN(soundcard->data)){
      available_soundcard_count++;
    }else if(soundcard_type == AGS_TYPE_PULSE_DEVOUT && AGS_IS_PULSE_DEVOUT(soundcard->data)){
      available_soundcard_count++;
    }else if(soundcard_type == AGS_TYPE_PULSE_DEVIN && AGS_IS_PULSE_DEVIN(soundcard->data)){
      available_soundcard_count++;
    }else if(soundcard_type == AGS_TYPE_JACK_DEVOUT && AGS_IS_JACK_DEVOUT(soundcard->data)){
      available_soundcard_count++;
    }else if(soundcard_type == AGS_TYPE_JACK_DEVIN && AGS_IS_JACK_DEVIN(soundcard->data)){
      available_soundcard_count++;
    }else if(soundcard_type == AGS_TYPE_CORE_AUDIO_DEVOUT && AGS_IS_CORE_AUDIO_DEVOUT(soundcard->data)){
      available_soundcard_count++;
    }else if(soundcard_type == AGS_TYPE_CORE_AUDIO_DEVIN && AGS_IS_CORE_AUDIO_DEVIN(soundcard->data)){
      available_soundcard_count++;
    }else if(soundcard_type == AGS_TYPE_WASAPI_DEVOUT && AGS_IS_WASAPI_DEVOUT(soundcard->data)){
      available_soundcard_count++;
    }else if(soundcard_type == AGS_TYPE_WASAPI_DEVIN && AGS_IS_WASAPI_DEVIN(soundcard->data)){
      available_soundcard_count++;
    }

    soundcard = soundcard->next;
  }

  /* grow/shrink */
  nth_card = 0;
	
  if(soundcard_count > available_soundcard_count ||
     soundcard_count < available_soundcard_count){
    soundcard_group = g_strdup("soundcard");

    do_grow = TRUE;

    if(soundcard_count > available_soundcard_count){
      do_grow = FALSE;
    }
    
    for(i = 0; i < soundcard_count && i < 256;){
      io_soundcard = NULL;

      if(!g_key_file_has_group(config->key_file,
			       soundcard_group)){
	g_free(soundcard_group);    

	soundcard_group = g_strdup_printf("%s-%d",
					  AGS_CONFIG_SOUNDCARD,
					  i);
    	
	continue;
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

	g_free(capability);
      }
      
      if((!g_ascii_strncasecmp(str, backend, strlen(backend) + 1)) == FALSE ||
	 !is_output){
	g_free(str);
	
	/* iter */
	i++;
	
	continue;
      }

      g_free(str);
      
      /* add/remove or get soundcard */
      if(i >= available_soundcard_count){
	if(soundcard_type == AGS_TYPE_ALSA_DEVOUT){
	  io_soundcard = (GObject *) ags_alsa_devout_new();
	}else if(soundcard_type == AGS_TYPE_ALSA_DEVIN){
	  io_soundcard = (GObject *) ags_alsa_devin_new();
	}else if(soundcard_type == AGS_TYPE_OSS_DEVOUT){
	  io_soundcard = (GObject *) ags_oss_devout_new();
	}else if(soundcard_type == AGS_TYPE_OSS_DEVIN){
	  io_soundcard = (GObject *) ags_oss_devin_new();
	}else if(soundcard_type == AGS_TYPE_PULSE_DEVOUT){
	  io_soundcard = ags_sound_server_register_soundcard(AGS_SOUND_SERVER(pulse_server),
							     TRUE);
	}else if(soundcard_type == AGS_TYPE_PULSE_DEVIN){
	  io_soundcard = ags_sound_server_register_soundcard(AGS_SOUND_SERVER(pulse_server),
							     FALSE);
	}else if(soundcard_type == AGS_TYPE_JACK_DEVOUT){
	  io_soundcard = ags_sound_server_register_soundcard(AGS_SOUND_SERVER(jack_server),
							     TRUE);
	}else if(soundcard_type == AGS_TYPE_JACK_DEVIN){
	  io_soundcard = ags_sound_server_register_soundcard(AGS_SOUND_SERVER(jack_server),
							     FALSE);
	}else if(soundcard_type == AGS_TYPE_CORE_AUDIO_DEVOUT){
	  io_soundcard = ags_sound_server_register_soundcard(AGS_SOUND_SERVER(core_audio_server),
							     TRUE);
	}else if(soundcard_type == AGS_TYPE_CORE_AUDIO_DEVIN){
	  io_soundcard = ags_sound_server_register_soundcard(AGS_SOUND_SERVER(core_audio_server),
							     FALSE);
	}else if(soundcard_type == AGS_TYPE_WASAPI_DEVOUT){
	  io_soundcard = (GObject *) ags_wasapi_devout_new();

	  str = ags_config_get_value(config,
				     soundcard_group,
				     "wasapi-share-mode");

	  if(str != NULL &&
	     !g_ascii_strncasecmp(str,
				  "exclusive",
				  10)){
	    ags_wasapi_devout_set_flags(AGS_WASAPI_DEVOUT(io_soundcard),
					AGS_WASAPI_DEVOUT_SHARE_MODE_EXCLUSIVE);
	  }else{
	    ags_wasapi_devout_unset_flags(AGS_WASAPI_DEVOUT(io_soundcard),
					  AGS_WASAPI_DEVOUT_SHARE_MODE_EXCLUSIVE);
	  }
	  
	  str = ags_config_get_value(config,
				     soundcard_group,
				     "wasapi-buffer-size");

	  if(str != NULL){
	    AGS_WASAPI_DEVOUT(soundcard)->wasapi_buffer_size = g_ascii_strtoull(str,
										NULL,
										10);
	    
	    g_free(str);
	  }
	}else if(soundcard_type == AGS_TYPE_WASAPI_DEVIN){
	  io_soundcard = (GObject *) ags_wasapi_devin_new();

	  str = ags_config_get_value(config,
				     soundcard_group,
				     "wasapi-share-mode");

	  if(str != NULL &&
	     !g_ascii_strncasecmp(str,
				  "exclusive",
				  10)){
	    ags_wasapi_devout_set_flags(AGS_WASAPI_DEVOUT(io_soundcard),
					AGS_WASAPI_DEVOUT_SHARE_MODE_EXCLUSIVE);
	  }else{
	    ags_wasapi_devout_unset_flags(AGS_WASAPI_DEVOUT(io_soundcard),
					  AGS_WASAPI_DEVOUT_SHARE_MODE_EXCLUSIVE);
	  }

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

	if(io_soundcard != NULL){
	  guint soundcard_capability;

	  g_object_ref(io_soundcard);

	  start_soundcard = g_list_append(start_soundcard,
					  io_soundcard);

	  soundcard_capability = ags_soundcard_get_capability(AGS_SOUNDCARD(io_soundcard));
    
	  soundcard_thread = (AgsThread *) ags_soundcard_thread_new(io_soundcard,
								    soundcard_capability);
	  ags_thread_add_child_extended(AGS_THREAD(audio_loop),
					(AgsThread *) soundcard_thread,
					TRUE, TRUE);

	  if(is_output){
	    export_thread = (AgsThread *) ags_export_thread_new(io_soundcard,
								NULL);
	    ags_thread_add_child_extended(AGS_THREAD(audio_loop),
					  (AgsThread *) export_thread,
					  TRUE, TRUE);
	  }
	}
      }else{
	soundcard = start_soundcard;

	for(j = 0; soundcard != NULL;){
	  if(((soundcard_type == AGS_TYPE_ALSA_DEVOUT && AGS_IS_ALSA_DEVOUT(soundcard->data)) ||
	      (soundcard_type == AGS_TYPE_ALSA_DEVIN && AGS_IS_ALSA_DEVIN(soundcard->data)) ||
	      (soundcard_type == AGS_TYPE_OSS_DEVOUT && AGS_IS_OSS_DEVOUT(soundcard->data)) ||
	      (soundcard_type == AGS_TYPE_OSS_DEVIN && AGS_IS_OSS_DEVIN(soundcard->data)) ||
	      (soundcard_type == AGS_TYPE_PULSE_DEVOUT && AGS_IS_PULSE_DEVOUT(soundcard->data)) ||
	      (soundcard_type == AGS_TYPE_PULSE_DEVIN && AGS_IS_PULSE_DEVIN(soundcard->data)) ||
	      (soundcard_type == AGS_TYPE_JACK_DEVOUT && AGS_IS_JACK_DEVOUT(soundcard->data)) ||
	      (soundcard_type == AGS_TYPE_JACK_DEVIN && AGS_IS_JACK_DEVIN(soundcard->data)) ||
	      (soundcard_type == AGS_TYPE_CORE_AUDIO_DEVOUT && AGS_IS_CORE_AUDIO_DEVOUT(soundcard->data)) ||
	      (soundcard_type == AGS_TYPE_CORE_AUDIO_DEVIN && AGS_IS_CORE_AUDIO_DEVIN(soundcard->data)) ||
	      (soundcard_type == AGS_TYPE_WASAPI_DEVOUT && AGS_IS_WASAPI_DEVOUT(soundcard->data)) ||
	      (soundcard_type == AGS_TYPE_WASAPI_DEVIN && AGS_IS_WASAPI_DEVIN(soundcard->data))) &&
	     ((do_grow && j == nth_card) ||
	      (!do_grow && j == soundcard_count))){
	    io_soundcard = soundcard->data;
	    nth_card++;

	    break;
	  }

	  /* iter soundcard */
	  if((soundcard_type == AGS_TYPE_ALSA_DEVOUT && AGS_IS_ALSA_DEVOUT(soundcard->data)) ||
	      (soundcard_type == AGS_TYPE_ALSA_DEVIN && AGS_IS_ALSA_DEVIN(soundcard->data)) ||
	      (soundcard_type == AGS_TYPE_OSS_DEVOUT && AGS_IS_OSS_DEVOUT(soundcard->data)) ||
	      (soundcard_type == AGS_TYPE_OSS_DEVIN && AGS_IS_OSS_DEVIN(soundcard->data)) ||
	      (soundcard_type == AGS_TYPE_PULSE_DEVOUT && AGS_IS_PULSE_DEVOUT(soundcard->data)) ||
	      (soundcard_type == AGS_TYPE_PULSE_DEVIN && AGS_IS_PULSE_DEVIN(soundcard->data)) ||
	      (soundcard_type == AGS_TYPE_JACK_DEVOUT && AGS_IS_JACK_DEVOUT(soundcard->data)) ||
	      (soundcard_type == AGS_TYPE_JACK_DEVIN && AGS_IS_JACK_DEVIN(soundcard->data)) ||
	      (soundcard_type == AGS_TYPE_CORE_AUDIO_DEVOUT && AGS_IS_CORE_AUDIO_DEVOUT(soundcard->data)) ||
	      (soundcard_type == AGS_TYPE_CORE_AUDIO_DEVIN && AGS_IS_CORE_AUDIO_DEVIN(soundcard->data)) ||
	      (soundcard_type == AGS_TYPE_WASAPI_DEVOUT && AGS_IS_WASAPI_DEVOUT(soundcard->data)) ||
	      (soundcard_type == AGS_TYPE_WASAPI_DEVIN && AGS_IS_WASAPI_DEVIN(soundcard->data))){
	    j++;
	    nth_card++;
	  }

	  soundcard = soundcard->next;
	}

	if(io_soundcard != NULL){
	  start_soundcard = g_list_remove(start_soundcard,
					  io_soundcard);

	  /* soundcard thread */
	  soundcard_thread = ags_thread_find_type(audio_loop,
						  AGS_TYPE_SOUNDCARD_THREAD);

	  while(soundcard_thread != NULL){
	    AgsThread *next;
    
	    next = ags_atomic_pointer_get(&(soundcard_thread->next));
    
	    if(AGS_IS_SOUNDCARD_THREAD(soundcard_thread) &&
	       AGS_SOUNDCARD_THREAD(soundcard_thread)->soundcard == io_soundcard){
	      ags_thread_remove_child(audio_loop,
				      soundcard_thread);

	      g_object_run_dispose((GObject *) soundcard_thread);
	      g_object_unref((GObject *) soundcard_thread);

	      break;
	    }

	    soundcard_thread = next;
	  }

	  /* export thread */
	  export_thread = ags_thread_find_type(audio_loop,
					       AGS_TYPE_EXPORT_THREAD);

	  while(export_thread != NULL){
	    if(AGS_IS_EXPORT_THREAD(export_thread) &&
	       AGS_EXPORT_THREAD(export_thread)->soundcard == io_soundcard){
	      ags_thread_remove_child(audio_loop,
				      export_thread);

	      g_object_run_dispose((GObject *) export_thread);
	      g_object_unref((GObject *) export_thread);

	      break;
	    }
    
	    export_thread = ags_atomic_pointer_get(&(export_thread->next));
	  }


	  if(AGS_IS_JACK_DEVOUT(io_soundcard) || AGS_IS_JACK_DEVIN(io_soundcard)){
	    ags_sound_server_unregister_soundcard(AGS_SOUND_SERVER(jack_server),
						  io_soundcard);
	  }else if(AGS_IS_PULSE_DEVOUT(io_soundcard)){
	    ags_sound_server_unregister_soundcard(AGS_SOUND_SERVER(pulse_server),
						  io_soundcard);
	  }else if(AGS_IS_CORE_AUDIO_DEVOUT(io_soundcard) || AGS_IS_CORE_AUDIO_DEVIN(io_soundcard)){
	    ags_sound_server_unregister_soundcard(AGS_SOUND_SERVER(core_audio_server),
						  io_soundcard);
	  }
	  
	}
      }

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

	g_free(str);
      }

      cache_buffer_size = 4096;
      str = ags_config_get_value(config,
				 soundcard_group,
				 "cache-buffer-size");

      if(str != NULL){
	cache_buffer_size = g_ascii_strtoull(str,
					     NULL,
					     10);

	g_free(str);
      }

      if(soundcard_type == AGS_TYPE_PULSE_DEVOUT ||
	 soundcard_type == AGS_TYPE_PULSE_DEVIN ||
	 soundcard_type == AGS_TYPE_CORE_AUDIO_DEVOUT ||
	 soundcard_type == AGS_TYPE_CORE_AUDIO_DEVIN ||
	 soundcard_type == AGS_TYPE_WASAPI_DEVOUT ||
	 soundcard_type == AGS_TYPE_WASAPI_DEVIN){
	g_object_set(io_soundcard,
		     "use-cache", use_cache,
		     "cache-buffer-size", cache_buffer_size,
		     NULL);
      }
      
      /* iter */
      i++;
    }
  }
  
  ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(application_context),
				   start_soundcard);
}

void
ags_apply_sound_config_sequencer_presets(AgsTask *task,
					 GType sequencer_type, guint sequencer_count)
{
  AgsJackServer *jack_server;
  AgsCoreAudioServer *core_audio_server;

  AgsThread *audio_loop;
  AgsThread *sequencer_thread;

  AgsApplicationContext *application_context;
  AgsConfig *config;

  GObject *io_sequencer;
  
  GList *start_sound_server, *sound_server;
  GList *start_sequencer, *sequencer;

  gchar *soundcard_group;
  gchar *sequencer_group;
  gchar *backend;
  gchar *capability;
  gchar *str;

  guint available_sequencer_count;
  guint pcm_channels, buffer_size, samplerate, format;
  guint cache_buffer_size;
  guint nth_card;
  guint i, i_stop;
  guint j;
  gboolean do_grow;
  gboolean is_output;
  gboolean use_cache;

  application_context = ags_application_context_get_instance();

  config = ags_config_get_instance();

  start_sequencer = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));

  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  jack_server = NULL;
  core_audio_server = NULL;
  
  sound_server = 
    start_sound_server = ags_sound_provider_get_sound_server(AGS_SOUND_PROVIDER(application_context));

  while(sound_server != NULL){
    if(AGS_IS_JACK_SERVER(sound_server->data)){
      jack_server = sound_server->data;
    }else if(AGS_IS_CORE_AUDIO_SERVER(sound_server->data)){
      core_audio_server = sound_server->data;
    }
    
    sound_server = sound_server->next;
  }

  backend = NULL;

  if(sequencer_type == AGS_TYPE_ALSA_MIDIIN){
    backend = "alsa";
  }else if(sequencer_type == AGS_TYPE_OSS_MIDIIN){
    backend = "oss";
  }else if(sequencer_type == AGS_TYPE_JACK_MIDIIN){
    backend = "jack";
  }else if(sequencer_type == AGS_TYPE_CORE_AUDIO_MIDIIN){
    backend = "core-audio";
  }
  
  if(backend == NULL){
    g_warning("apply config - unknown sequencer backend");
    
    return;
  }
  
  /* backend - device output */
  sequencer = start_sequencer;

  available_sequencer_count = 0;
  
  while(sequencer != NULL){
    if(sequencer_type == AGS_TYPE_ALSA_MIDIIN && AGS_IS_ALSA_MIDIIN(sequencer->data)){
      available_sequencer_count++;
    }else if(sequencer_type == AGS_TYPE_OSS_MIDIIN && AGS_IS_OSS_MIDIIN(sequencer->data)){
      available_sequencer_count++;
    }else if(sequencer_type == AGS_TYPE_JACK_MIDIIN && AGS_IS_JACK_MIDIIN(sequencer->data)){
      available_sequencer_count++;
    }else if(sequencer_type == AGS_TYPE_CORE_AUDIO_MIDIIN && AGS_IS_CORE_AUDIO_MIDIIN(sequencer->data)){
      available_sequencer_count++;
    }

    sequencer = sequencer->next;
  }

  /* grow/shrink */
  nth_card = 0;
	
  if(sequencer_count > available_sequencer_count ||
     sequencer_count < available_sequencer_count){
    sequencer_group = g_strdup("sequencer");

    do_grow = TRUE;

    if(sequencer_count > available_sequencer_count){
      do_grow = FALSE;
    }
    
    for(i = 0; i < sequencer_count && i < 256;){
      io_sequencer = NULL;

      if(!g_key_file_has_group(config->key_file,
			       sequencer_group)){
	g_free(sequencer_group);    

	sequencer_group = g_strdup_printf("%s-%d",
					  AGS_CONFIG_SEQUENCER,
					  i);
    	
	continue;
      }

      str = ags_config_get_value(config,
				 sequencer_group,
				 "backend");

      capability = ags_config_get_value(config,
					sequencer_group,
					"capability");

      is_output = TRUE;

      if(capability != NULL &&
	 !g_ascii_strncasecmp(capability,
			      "capture",
			      8)){
	is_output = FALSE;

	g_free(capability);
      }
      
      if((!g_ascii_strncasecmp(str, backend, strlen(backend) + 1)) == FALSE ||
	 !is_output){
	g_free(str);
	
	/* iter */
	i++;
	
	continue;
      }

      g_free(str);
      
      /* add/remove or get sequencer */
      if(i >= available_sequencer_count){
	if(sequencer_type == AGS_TYPE_ALSA_MIDIIN){
	  io_sequencer = (GObject *) ags_alsa_midiin_new();
	}else if(sequencer_type == AGS_TYPE_OSS_MIDIIN){
	  io_sequencer = (GObject *) ags_oss_midiin_new();
	}else if(sequencer_type == AGS_TYPE_JACK_MIDIIN){
	  io_sequencer = ags_sound_server_register_sequencer(AGS_SOUND_SERVER(jack_server),
							     FALSE);
	}else if(sequencer_type == AGS_TYPE_CORE_AUDIO_MIDIIN){
	  io_sequencer = ags_sound_server_register_sequencer(AGS_SOUND_SERVER(core_audio_server),
							     FALSE);
	}

	if(io_sequencer != NULL){
	  g_object_ref(io_sequencer);

	  start_sequencer = g_list_append(start_sequencer,
					  io_sequencer);

	  sequencer_thread = (AgsThread *) ags_sequencer_thread_new(io_sequencer);
	  ags_thread_add_child_extended(AGS_THREAD(audio_loop),
					(AgsThread *) sequencer_thread,
					TRUE, TRUE);
	}
      }else{
	sequencer = start_sequencer;

	for(j = 0; sequencer != NULL;){
	  if(((sequencer_type == AGS_TYPE_ALSA_MIDIIN && AGS_IS_ALSA_MIDIIN(sequencer->data)) ||
	      (sequencer_type == AGS_TYPE_OSS_MIDIIN && AGS_IS_OSS_MIDIIN(sequencer->data)) ||
	      (sequencer_type == AGS_TYPE_JACK_MIDIIN && AGS_IS_JACK_MIDIIN(sequencer->data)) ||
	      (sequencer_type == AGS_TYPE_CORE_AUDIO_MIDIIN && AGS_IS_CORE_AUDIO_MIDIIN(sequencer->data))) &&
	     ((do_grow && j == nth_card) ||
	      (!do_grow && j == sequencer_count))){
	    io_sequencer = sequencer->data;
	    nth_card++;

	    break;
	  }

	  /* iter sequencer */
	  if((sequencer_type == AGS_TYPE_ALSA_MIDIIN && AGS_IS_ALSA_MIDIIN(sequencer->data)) ||
	     (sequencer_type == AGS_TYPE_OSS_MIDIIN && AGS_IS_OSS_MIDIIN(sequencer->data)) ||
	     (sequencer_type == AGS_TYPE_JACK_MIDIIN && AGS_IS_JACK_MIDIIN(sequencer->data)) ||
	     (sequencer_type == AGS_TYPE_CORE_AUDIO_MIDIIN && AGS_IS_CORE_AUDIO_MIDIIN(sequencer->data))){
	    j++;
	    nth_card++;
	  }

	  sequencer = sequencer->next;
	}

	if(io_sequencer != NULL){
	  start_sequencer = g_list_remove(start_sequencer,
					  io_sequencer);

	  /* sequencer thread */
	  sequencer_thread = ags_thread_find_type(audio_loop,
						  AGS_TYPE_SEQUENCER_THREAD);

	  while(sequencer_thread != NULL){
	    AgsThread *next;
    
	    next = ags_atomic_pointer_get(&(sequencer_thread->next));
    
	    if(AGS_IS_SEQUENCER_THREAD(sequencer_thread) &&
	       AGS_SEQUENCER_THREAD(sequencer_thread)->sequencer == io_sequencer){
	      ags_thread_remove_child(audio_loop,
				      sequencer_thread);

	      g_object_run_dispose((GObject *) sequencer_thread);
	      g_object_unref((GObject *) sequencer_thread);

	      break;
	    }

	    sequencer_thread = next;
	  }

	  if(AGS_IS_JACK_MIDIIN(io_sequencer)){
	    ags_sound_server_unregister_sequencer(AGS_SOUND_SERVER(jack_server),
						  io_sequencer);
	  }else if(AGS_IS_CORE_AUDIO_MIDIIN(io_sequencer)){
	    ags_sound_server_unregister_sequencer(AGS_SOUND_SERVER(core_audio_server),
						  io_sequencer);
	  }	  
	}
      }

      /* device */
      str = ags_config_get_value(config,
				 sequencer_group,
				 "device");
      
      if(str != NULL){
	ags_sequencer_set_device(AGS_SEQUENCER(sequencer),
				 str);
	g_free(str);
      }
      
      /* iter */
      i++;
    }
  }

  ags_sound_provider_set_sequencer(AGS_SOUND_PROVIDER(application_context),
				   start_sequencer);  
}

void
ags_apply_sound_config_launch(AgsTask *task)
{
  AgsApplySoundConfig *apply_sound_config;

  AgsJackServer *jack_server;
  AgsPulseServer *pulse_server;
  AgsCoreAudioServer *core_audio_server;

  AgsLv2Manager *lv2_manager;
  
  AgsThread *audio_loop;
  AgsThread *soundcard_thread;
  AgsThread *export_thread;
  AgsThread *sequencer_thread;
  AgsThread *audio_thread;
  AgsThread *channel_thread;
  AgsTaskLauncher *task_launcher;
  
  AgsApplicationContext *application_context;
  AgsConfig *config;

  GObject *default_soundcard;
  
  GList *start_lv2_plugin, *lv2_plugin;
  GList *start_sound_server, *sound_server;
  GList *start_soundcard, *soundcard;
  GList *start_sequencer, *sequencer;
  GList *start_audio, *audio;
  GList *start_list, *list;

  gchar *soundcard_group;
  gchar *sequencer_group;
  gchar *str;
  gchar *capability;
  
  gdouble frequency;
  guint samplerate;
  guint buffer_size;

  guint alsa_devout_count;
  guint alsa_devin_count;
  guint alsa_midiin_count;
  guint oss_devout_count;
  guint oss_devin_count;
  guint oss_midiin_count;
  guint pulse_devout_count;
  guint pulse_devin_count;
  guint available_pulse_devout_count;
  guint jack_devout_count;
  guint jack_devin_count;
  guint jack_midiin_count;
  guint core_audio_devout_count;
  guint core_audio_devin_count;
  guint core_audio_midiin_count;
  guint wasapi_devout_count;
  guint wasapi_devin_count;
  guint wasapi_midiin_count;

  guint i;
  gboolean has_core_audio;
  gboolean has_pulse;
  gboolean has_jack;
  gboolean is_output;  

  static const guint staging_flags = (AGS_SOUND_STAGING_CANCEL |
				      AGS_SOUND_STAGING_REMOVE);

  apply_sound_config = AGS_APPLY_SOUND_CONFIG(task);

  application_context = ags_application_context_get_instance();

  config = ags_config_get_instance();

  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));
  
  start_soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  start_sequencer = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));  

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
  soundcard_thread = ags_thread_find_type(audio_loop,
					  AGS_TYPE_SOUNDCARD_THREAD);

  while(soundcard_thread != NULL){
    if(AGS_IS_SOUNDCARD_THREAD(soundcard_thread)){
      ags_thread_stop(soundcard_thread);
    }
    
    soundcard_thread = ags_atomic_pointer_get(&(soundcard_thread->next));
  }

  export_thread = ags_thread_find_type(audio_loop,
				       AGS_TYPE_EXPORT_THREAD);

  while(export_thread != NULL){
    if(AGS_IS_EXPORT_THREAD(export_thread)){
      ags_thread_stop(export_thread);
    }
    
    export_thread = ags_atomic_pointer_get(&(export_thread->next));
  }

  sequencer_thread = ags_thread_find_type(audio_loop,
					  AGS_TYPE_SEQUENCER_THREAD);

  while(sequencer_thread != NULL){
    if(AGS_IS_SEQUENCER_THREAD(sequencer_thread) &&
       !AGS_IS_CORE_AUDIO_MIDIIN(AGS_SEQUENCER_THREAD(sequencer_thread)->sequencer)){
      ags_thread_stop(sequencer_thread);
    }
    
    sequencer_thread = ags_atomic_pointer_get(&(sequencer_thread->next));
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
      
	ags_audio_thread_set_processing((AgsAudioThread *) audio_thread,
					FALSE);
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
	  ags_channel_thread_set_processing((AgsChannelThread *) channel_thread,
					    FALSE);
	}

	g_object_unref(channel);
      
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
	  ags_channel_thread_set_processing((AgsChannelThread *) channel_thread,
					    FALSE);
	}

	g_object_unref(channel);
	
	/* iterate */
	input = input->next;
      }
    }

    ags_audio_loop_remove_audio((AgsAudioLoop *) audio_loop,
				(GObject *) audio->data);

    g_object_unref(playback_domain);
    
    g_list_free_full(start_output,
		     g_object_unref);
    g_list_free_full(start_input,
		     g_object_unref);

    audio = audio->next;
  }

  alsa_devout_count = 0;
  alsa_devin_count = 0;
    
  oss_devout_count = 0;
  oss_devin_count = 0;
    
  jack_devout_count = 0;
  jack_devin_count = 0;
    
  pulse_devout_count = 0;
  pulse_devin_count = 0;
    
  core_audio_devout_count = 0;
  core_audio_devin_count = 0;

  wasapi_devout_count = 0;
  wasapi_devin_count = 0;

  soundcard = NULL;

  soundcard_group = g_strdup("soundcard");

  for(i = 0; ; i++){
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
    
    /* check soundcard */    
    if(str != NULL){
      if(!g_ascii_strncasecmp(str,
			      "core-audio",
			      11)){
	if(is_output){
	  core_audio_devout_count++;
	}else{
	  core_audio_devin_count++;
	}
      }else if(!g_ascii_strncasecmp(str,
				    "pulse",
				    6)){
	if(is_output){
	  pulse_devout_count++;
	}
      }else if(!g_ascii_strncasecmp(str,
				    "jack",
				    5)){
	if(is_output){
	  jack_devout_count++;
	}else{
	  jack_devin_count++;
	}
      }else if(!g_ascii_strncasecmp(str,
				    "wasapi",
				    7)){
	if(is_output){
	  wasapi_devout_count++;
	}else{
	  wasapi_devin_count++;
	}
      }else if(!g_ascii_strncasecmp(str,
				    "alsa",
				    5)){
	if(is_output){
	  alsa_devout_count++;
	}else{
	  alsa_devin_count++;
	}
      }else if(!g_ascii_strncasecmp(str,
				    "oss",
				    4)){
	if(is_output){
	  oss_devout_count++;
	}else{
	  oss_devin_count++;
	}
      }
    }

    g_free(soundcard_group);    
    soundcard_group = g_strdup_printf("%s-%d",
				      AGS_CONFIG_SOUNDCARD,
				      i);
  }

  g_free(soundcard_group);    

  alsa_midiin_count = 0;
    
  oss_midiin_count = 0;
    
  jack_midiin_count = 0;
  
  core_audio_midiin_count = 0;

  sequencer_group = g_strdup("sequencer");
  
  for(i = 0; ; i++){
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

    /* check sequencer */
    if(str != NULL){
      if(!g_ascii_strncasecmp(str,
			      "core-audio",
			      11)){
	core_audio_midiin_count++;
      }else if(!g_ascii_strncasecmp(str,
				    "pulse",
				    6)){
	alsa_midiin_count++;
      }else if(!g_ascii_strncasecmp(str,
				    "jack",
				    5)){
	jack_midiin_count++;
      }else if(!g_ascii_strncasecmp(str,
				    "wasapi",
				    7)){
	wasapi_midiin_count++;
      }else if(!g_ascii_strncasecmp(str,
				    "alsa",
				    5)){
	alsa_midiin_count++;
      }else if(!g_ascii_strncasecmp(str,
				    "oss",
				    4)){
	oss_midiin_count++;
      }
    }

    g_free(sequencer_group);    
    sequencer_group = g_strdup_printf("%s-%d",
				      AGS_CONFIG_SEQUENCER,
				      i);
  }

  g_free(sequencer_group);    

  /* ALSA - device output/input */  
  ags_apply_sound_config_soundcard_presets(task,
					   AGS_TYPE_ALSA_DEVOUT, alsa_devout_count);

  ags_apply_sound_config_soundcard_presets(task,
					   AGS_TYPE_ALSA_DEVIN, alsa_devin_count);

  ags_apply_sound_config_sequencer_presets(task,
					   AGS_TYPE_ALSA_MIDIIN, alsa_midiin_count);
  
  /* OSSv4 - device output/input */  
  ags_apply_sound_config_soundcard_presets(task,
					   AGS_TYPE_OSS_DEVOUT, oss_devout_count);

  ags_apply_sound_config_soundcard_presets(task,
					   AGS_TYPE_OSS_DEVIN, oss_devin_count);

  ags_apply_sound_config_sequencer_presets(task,
					   AGS_TYPE_OSS_MIDIIN, oss_midiin_count);
  
  /* Pulseaudio - device output/input */  
  ags_apply_sound_config_soundcard_presets(task,
					   AGS_TYPE_PULSE_DEVOUT, pulse_devout_count);

  ags_apply_sound_config_soundcard_presets(task,
					   AGS_TYPE_PULSE_DEVIN, pulse_devin_count);
  
  /* JACK - device output/input */  
  ags_apply_sound_config_soundcard_presets(task,
					   AGS_TYPE_JACK_DEVOUT, jack_devout_count);

  ags_apply_sound_config_soundcard_presets(task,
					   AGS_TYPE_JACK_DEVIN, jack_devin_count);
  
  ags_apply_sound_config_sequencer_presets(task,
					   AGS_TYPE_JACK_MIDIIN, jack_midiin_count);
  
  /* CoreAudio - device output/input */  
  ags_apply_sound_config_soundcard_presets(task,
					   AGS_TYPE_CORE_AUDIO_DEVOUT, core_audio_devout_count);

  ags_apply_sound_config_soundcard_presets(task,
					   AGS_TYPE_CORE_AUDIO_DEVIN, core_audio_devin_count);
  
  ags_apply_sound_config_sequencer_presets(task,
					   AGS_TYPE_CORE_AUDIO_MIDIIN, core_audio_midiin_count);
  
  /* WASAPI - device output/input */  
  ags_apply_sound_config_soundcard_presets(task,
					   AGS_TYPE_WASAPI_DEVOUT, wasapi_devout_count);

  ags_apply_sound_config_soundcard_presets(task,
					   AGS_TYPE_WASAPI_DEVIN, wasapi_devin_count);

  /* default soundcard and soundcard thread */
  start_soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  start_sequencer = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));  
  
  ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(application_context),
				   start_soundcard);

  ags_sound_provider_set_sequencer(AGS_SOUND_PROVIDER(application_context),
				   start_sequencer);

  if(start_soundcard != NULL){
    ags_sound_provider_set_default_soundcard(AGS_SOUND_PROVIDER(application_context),
					     start_soundcard->data);

    soundcard_thread = ags_thread_find_type(audio_loop,
					    AGS_TYPE_SOUNDCARD_THREAD);

    while(soundcard_thread != NULL){
      AgsThread *next;
    
      next = ags_atomic_pointer_get(&(soundcard_thread->next));
    
      if(AGS_IS_SOUNDCARD_THREAD(soundcard_thread) &&
	 AGS_SOUNDCARD_THREAD(soundcard_thread)->soundcard == start_soundcard->data){
	ags_sound_provider_set_default_soundcard_thread(AGS_SOUND_PROVIDER(application_context),
							(GObject *) soundcard_thread);

	break;
      }

      soundcard_thread = next;
    }
  }else{
    ags_sound_provider_set_default_soundcard(AGS_SOUND_PROVIDER(application_context),
					     NULL);

    ags_sound_provider_set_default_soundcard_thread(AGS_SOUND_PROVIDER(application_context),
						    NULL);
  }
  
  /* stop sound server */
  if(jack_server != NULL &&
     jack_devout_count == 0 &&
     jack_devin_count == 0 &&
     jack_midiin_count == 0){
#ifdef AGS_WITH_JACK      
    AgsJackClient *default_client;
    AgsJackClient *input_client;

    g_object_get(jack_server,
		 "default-jack-client", &default_client,
		 "input-jack-client", &input_client,
		 NULL);
    
    ags_jack_server_disconnect_client(jack_server);

    ags_jack_server_remove_client(jack_server,
				  (GObject *) default_client);
    ags_jack_server_remove_client(jack_server,
				  (GObject *) input_client);

    if(default_client != NULL){
      g_object_unref(default_client);
    }
    
    if(input_client != NULL){
      g_object_unref(input_client);
    }

    jack_server->n_soundcards = 0;
    jack_server->n_sequencers = 0;
#endif
  }

  if(pulse_server != NULL &&
     pulse_devout_count == 0){
#ifdef AGS_WITH_PULSE
    AgsPulseClient *pulse_client;

    g_object_get(pulse_server,
		 "default-pulse-client", &pulse_client,
		 NULL);

    if(pulse_server->main_loop != NULL){
      pa_mainloop_quit(pulse_server->main_loop,
		       0);
    }
    
    ags_pulse_server_disconnect_client(pulse_server);

    ags_pulse_server_remove_client(pulse_server,
				   (GObject *) pulse_client);
    
    pulse_server->main_loop = NULL;
    pulse_server->main_loop_api = NULL;
    
    if(pulse_client != NULL){
      g_object_unref(pulse_client);
    }

    pulse_server->n_soundcards = 0;
    pulse_server->n_sequencers = 0;
#endif
  }
  
  if(core_audio_server != NULL &&
     core_audio_devout_count == 0 &&
     core_audio_devin_count == 0 &&
     core_audio_midiin_count == 0){
    list = core_audio_server->client;

    while(list != NULL){
      ags_core_audio_client_deactivate(list->data);

      list = list->next;
    }
  }

  //---------------------------------------------------------  

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

  /* change frequency */
  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  ags_soundcard_get_presets(AGS_SOUNDCARD(default_soundcard),
			    NULL,
			    &samplerate,
			    &buffer_size,
			    NULL);

  frequency = ceil((gdouble) samplerate / (gdouble) buffer_size) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK;
  ags_main_loop_change_frequency(AGS_MAIN_LOOP(audio_loop),
				 frequency);
  
  /* lv2 */
  lv2_manager = ags_lv2_manager_get_instance();
  
  lv2_plugin = 
    start_lv2_plugin = ags_lv2_manager_get_lv2_plugin(lv2_manager);

  while(lv2_plugin != NULL){
    LV2_Feature **feature;
    
    feature = AGS_LV2_PLUGIN(lv2_plugin->data)->feature;

    if(feature != NULL){
      for(; feature[0] != NULL; feature++){
	if(!g_strcmp0(feature[0]->URI, LV2_OPTIONS__options)){
	  LV2_Options_Option *options;

	  guint i;
	  
	  options = feature[0]->data;

	  for(i = 0; ; i++){
	    if(options[i].context == 0 &&
	       options[i].subject == 0 &&
	       options[i].key == 0 &&
	       options[i].size == 0 &&
	       options[i].type == 0 &&
	       options[i].value == NULL){
	      break;
	    }
	    
	    if(options[i].key == ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
							     LV2_PARAMETERS__sampleRate)){
	      ((float *) options[i].value)[0] = (float) samplerate;
	    }
	    
	    if(options[i].key == ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
							     LV2_BUF_SIZE__minBlockLength)){
	      ((float *) options[i].value)[0] = (float) buffer_size;
	    }
	    
	    if(options[i].key == ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
							     LV2_BUF_SIZE__maxBlockLength)){
	      ((float *) options[i].value)[0] = (float) buffer_size;
	    }
	    
	    if(options[i].key == ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
							     LV2_BUF_SIZE__boundedBlockLength)){
	      ((float *) options[i].value)[0] = (float) buffer_size;
	    }

	    if(options[i].key == ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
							     LV2_BUF_SIZE__fixedBlockLength)){
	      ((float *) options[i].value)[0] = (float) buffer_size;
	    }
	  }
	  
	  break;
	}
      }
    }
    
    lv2_plugin = lv2_plugin->next;
  }
  
  g_list_free_full(start_lv2_plugin,
		   (GDestroyNotify) g_object_unref);

  /* reset audio's soundcard */
  audio = start_audio;

  while(audio != NULL){
    g_object_set(audio->data,
		 "output-soundcard", start_soundcard->data,
		 NULL);

    audio = audio->next;
  }

  /* launch */
#if 0
  if(has_core_audio){
    ags_core_audio_client_open((AgsCoreAudioClient *) core_audio_server->default_client,
			       "ags-default-client");

    ags_core_audio_server_connect_client(core_audio_server);
  }

  if(has_pulse){
    ags_pulse_server_connect_client(pulse_server);

    ags_pulse_server_start_poll(pulse_server);
  }

  if(has_jack){    
    ags_jack_server_connect_client(jack_server);
  }
#endif
  
  g_list_free_full(start_sound_server,
		   g_object_unref);
  g_list_free_full(start_audio,
		   g_object_unref);

  g_object_unref(audio_loop);
}

/**
 * ags_apply_sound_config_new:
 * @config_data: the config file as string data
 *
 * Creates a new instance of #AgsApplySoundConfig.
 *
 * Returns: the new #AgsApplySoundConfig.
 *
 * Since: 3.0.0
 */
AgsApplySoundConfig*
ags_apply_sound_config_new(gchar *config_data)
{
  AgsApplySoundConfig *apply_sound_config;

  apply_sound_config = (AgsApplySoundConfig *) g_object_new(AGS_TYPE_APPLY_SOUND_CONFIG,
							    "config-data", config_data,
							    NULL);

  return(apply_sound_config);
}
