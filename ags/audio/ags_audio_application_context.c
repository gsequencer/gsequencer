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

#include <ags/audio/ags_audio_application_context.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>

#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_main_loop.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_concurrency_provider.h>
#include <ags/thread/ags_thread-posix.h>
#include <ags/thread/ags_thread_pool.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_midiin.h>
#include <ags/audio/ags_recall_channel_run_dummy.h>
#include <ags/audio/ags_recall_ladspa.h>
#include <ags/audio/ags_recall_ladspa_run.h>
#include <ags/audio/ags_recall_lv2.h>
#include <ags/audio/ags_recall_lv2_run.h>
#include <ags/audio/ags_recall_dssi.h>
#include <ags/audio/ags_recall_dssi_run.h>

#include <ags/audio/jack/ags_jack_midiin.h>
#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_client.h>
#include <ags/audio/jack/ags_jack_port.h>
#include <ags/audio/jack/ags_jack_devout.h>

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

#include <ags/audio/file/ags_audio_file_xml.h>

#include <ags/audio/thread/ags_record_thread.h>
#include <ags/audio/thread/ags_recycling_thread.h>

#include <sys/types.h>
#include <pwd.h>

#include <jack/jslist.h>
#include <jack/jack.h>
#include <jack/control.h>
#include <stdbool.h>

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
void ags_audio_application_context_connect(AgsConnectable *connectable);
void ags_audio_application_context_disconnect(AgsConnectable *connectable);
AgsThread* ags_audio_application_context_get_main_loop(AgsConcurrencyProvider *concurrency_provider);
AgsThread* ags_audio_application_context_get_task_thread(AgsConcurrencyProvider *concurrency_provider);
AgsThreadPool* ags_audio_application_context_get_thread_pool(AgsConcurrencyProvider *concurrency_provider);
GList* ags_audio_application_context_get_soundcard(AgsSoundProvider *sound_provider);
GObject* ags_audio_application_context_get_default_soundcard_thread(AgsSoundProvider *sound_provider);
void ags_audio_application_context_set_default_soundcard_thread(AgsSoundProvider *sound_provider,
								GObject *soundcard_thread);
void ags_audio_application_context_set_soundcard(AgsSoundProvider *sound_provider,
						 GList *soundcar);
GList* ags_audio_application_context_get_sequencer(AgsSoundProvider *sound_provider);
void ags_audio_application_context_set_sequencer(AgsSoundProvider *sound_provider,
						 GList *sequencer);
GList* ags_audio_application_context_get_distributed_manager(AgsSoundProvider *sound_provider);
void ags_audio_application_context_finalize(GObject *gobject);

void ags_audio_application_context_register_types(AgsApplicationContext *application_context);
void ags_audio_application_context_load_config(AgsApplicationContext *application_context);
void ags_audio_application_context_read(AgsFile *file, xmlNode *node, GObject **application_context);
xmlNode* ags_audio_application_context_write(AgsFile *file, xmlNode *parent, GObject *application_context);

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

static gpointer ags_audio_application_context_parent_class = NULL;
static AgsConnectableInterface* ags_audio_application_context_parent_connectable_interface;

enum{
  PROP_0,
  PROP_SOUNDCARD,
};

GType
ags_audio_application_context_get_type()
{
  static GType ags_type_audio_application_context = 0;

  if(!ags_type_audio_application_context){
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
								"AgsAudioApplicationContext\0",
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
  }

  return (ags_type_audio_application_context);
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

  gobject->finalize = ags_audio_application_context_finalize;
  
  /**
   * AgsAudioApplicationContext:soundcard:
   *
   * The assigned soundcard.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("soundcard\0",
				   "soundcard of audio application context\0",
				   "The soundcard which this audio application context assigned to\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /* AgsAudioApplicationContextClass */
  application_context = (AgsApplicationContextClass *) audio_application_context;
  
  application_context->load_config = ags_audio_application_context_load_config;
  application_context->register_types = ags_audio_application_context_register_types;
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
}

void
ags_audio_application_context_sound_provider_interface_init(AgsSoundProviderInterface *sound_provider)
{
  sound_provider->get_soundcard = ags_audio_application_context_get_soundcard;
  sound_provider->set_soundcard = ags_audio_application_context_set_soundcard;
  sound_provider->get_default_soundcard_thread = ags_audio_application_context_get_default_soundcard_thread;
  sound_provider->set_default_soundcard_thread = ags_audio_application_context_set_default_soundcard_thread;
  sound_provider->get_sequencer = ags_audio_application_context_get_sequencer;
  sound_provider->set_sequencer = ags_audio_application_context_set_sequencer;
  sound_provider->get_distributed_manager = ags_audio_application_context_get_distributed_manager;
}

void
ags_audio_application_context_init(AgsAudioApplicationContext *audio_application_context)
{
  AgsServer *server;

  AgsAudioLoop *audio_loop;
  GObject *soundcard;
  GObject *sequencer;
  AgsJackServer *jack_server;

  AgsThread *soundcard_thread;
  
  AgsConfig *config;

  GList *list;
  JSList *jslist;

  gchar *soundcard_group;
  gchar *str;

  guint i;
  
  AGS_APPLICATION_CONTEXT(audio_application_context)->log = NULL;

  /**/
  config = ags_config_get_instance();
  AGS_APPLICATION_CONTEXT(audio_application_context)->config = config;
  g_object_set(config,
	       "application-context\0", audio_application_context,
	       NULL);

  /* distributed manager */
  audio_application_context->distributed_manager = NULL;

  /* jack server */
  jack_server = ags_jack_server_new((GObject *) audio_application_context,
				    NULL);
  audio_application_context->distributed_manager = g_list_prepend(audio_application_context->distributed_manager,
								  jack_server);
  g_object_ref(G_OBJECT(jack_server));

  /* soundcard */
  audio_application_context->soundcard = NULL;
  soundcard = NULL;
  
  soundcard_group = g_strdup("soundcard\0");
  
  for(i = 0; ; i++){
    guint pcm_channels, buffer_size, samplerate, format;
    gboolean use_jack, use_alsa, use_oss;
    
    if(!g_key_file_has_group(config->key_file,
			     soundcard_group)){
      if(i == 0){
	g_free(soundcard_group);
	soundcard_group = g_strdup_printf("%s-%d\0",
					  AGS_CONFIG_SOUNDCARD,
					  i);
	continue;
      }else{
	break;
      }
    }

    str = ags_config_get_value(config,
			       soundcard_group,
			       "backend\0");

    g_free(soundcard_group);
    soundcard_group = g_strdup_printf("%s-%d\0",
				      AGS_CONFIG_SOUNDCARD,
				      i);

    use_jack = FALSE;
    use_alsa = FALSE;
    use_oss = FALSE;
    
    if(str != NULL){
      if(!g_ascii_strncasecmp(str,
			      "jack\0",
			      5)){
	soundcard = ags_distributed_manager_register_soundcard(AGS_DISTRIBUTED_MANAGER(jack_server),
							       TRUE);
	ags_jack_server_connect_client(jack_server);
	
	use_jack = TRUE;
      }else if(!g_ascii_strncasecmp(str,
				    "alsa\0",
				    5)){
	soundcard = (GObject *) ags_devout_new((GObject *) audio_application_context);
	AGS_DEVOUT(soundcard)->flags &= (~AGS_DEVOUT_OSS);
	AGS_DEVOUT(soundcard)->flags |= AGS_DEVOUT_ALSA;
		
	use_alsa = TRUE;
      }else if(!g_ascii_strncasecmp(str,
				    "oss\0",
				    4)){
	soundcard = (GObject *) ags_devout_new((GObject *) audio_application_context);
	AGS_DEVOUT(soundcard)->flags &= (~AGS_DEVOUT_ALSA);
	AGS_DEVOUT(soundcard)->flags |= AGS_DEVOUT_OSS;

	use_oss = TRUE;
      }else{
	g_warning("unknown soundcard backend\0");

	continue;
      }
    }else{
      g_warning("unknown soundcard backend\0");
      
      continue;
    }
    
    audio_application_context->soundcard = g_list_append(audio_application_context->soundcard,
							 soundcard);
    g_object_ref(soundcard);

    /* device */
    str = ags_config_get_value(config,
			       soundcard_group,
			       "device\0");

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
			       "pcm-channels\0");

    if(str != NULL){
      pcm_channels = g_ascii_strtoull(str,
				      NULL,
				      10);
      g_free(str);
    }


    str = ags_config_get_value(config,
			       soundcard_group,
			       "buffer-size\0");

    if(str != NULL){
      buffer_size = g_ascii_strtoull(str,
				     NULL,
				     10);
      g_free(str);
    }

    str = ags_config_get_value(config,
			       soundcard_group,
			       "samplerate\0");

    if(str != NULL){
      samplerate = g_ascii_strtoull(str,
				    NULL,
				    10);
      g_free(str);
    }

    str = ags_config_get_value(config,
			       soundcard_group,
			       "format\0");

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
  }

  if(audio_application_context->soundcard != NULL){
    soundcard = audio_application_context->soundcard->data;
  }
  
  g_free(soundcard_group);
  
  /* AgsSequencer */
  audio_application_context->sequencer = NULL;

  sequencer = (GObject *) ags_midiin_new((GObject *) audio_application_context);
  audio_application_context->sequencer = g_list_prepend(audio_application_context->sequencer,
							sequencer);
  g_object_ref(G_OBJECT(sequencer));

  //TODO:JK: comment out
  /*
    if(jack_enabled){
    GObject *tmp;
    
    tmp = ags_distributed_manager_register_sequencer(AGS_DISTRIBUTED_MANAGER(jack_server),
    FALSE);

    if(tmp != NULL){
    sequencer = tmp;
      
    audio_application_context->sequencer = g_list_prepend(audio_application_context->sequencer,
    sequencer);
    g_object_ref(G_OBJECT(sequencer));
    }
    }
  */
  
  /* AgsServer */
  audio_application_context->server = ags_server_new((GObject *) audio_application_context);

  /* AgsAudioLoop */
  audio_loop = ags_audio_loop_new((GObject *) soundcard,
				  (GObject *) audio_application_context);
  g_object_set(audio_application_context,
	       "main-loop\0", audio_loop,
	       NULL);

  g_object_ref(audio_loop);
  ags_connectable_connect(AGS_CONNECTABLE(audio_loop));

  /* AgsTaskThread */
  AGS_APPLICATION_CONTEXT(audio_application_context)->task_thread = (GObject *) ags_task_thread_new();
  ags_main_loop_set_async_queue(AGS_MAIN_LOOP(audio_loop),
				AGS_APPLICATION_CONTEXT(audio_application_context)->task_thread);
  ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				AGS_THREAD(AGS_APPLICATION_CONTEXT(audio_application_context)->task_thread),
				TRUE, TRUE);
  
  /* AgsSoundcardThread */
  audio_application_context->soundcard_thread = NULL;
  list = audio_application_context->soundcard;
    
  while(list != NULL){      
    soundcard_thread = (AgsThread *) ags_soundcard_thread_new(list->data);
    ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				  soundcard_thread,
				  TRUE, TRUE);

    if(audio_application_context->soundcard_thread == NULL){
      audio_application_context->soundcard_thread = soundcard_thread;
    }

    list = list->next;
  }
  
  /* AgsExportThread */
  audio_application_context->export_thread = (AgsThread *) ags_export_thread_new(soundcard,
										 NULL);
  ags_thread_add_child_extended(AGS_THREAD(audio_loop),
				audio_application_context->export_thread,
				TRUE, TRUE);

  /* AgsAutosaveThread */
  audio_application_context->autosave_thread = NULL;

  str = ags_config_get_value(AGS_APPLICATION_CONTEXT(audio_application_context)->config,
			     AGS_CONFIG_GENERIC,
			     "autosave-thread\0");
  if(str != NULL){
    if(!g_strcmp0(str,
		  "true\0")){
      str = ags_config_get_value(AGS_APPLICATION_CONTEXT(audio_application_context)->config,
				 AGS_CONFIG_GENERIC,
				 "simple-file\0");

      if(str != NULL){
	if(g_strcmp0(str,
		     "false\0")){
	  audio_application_context->autosave_thread = (GObject *) ags_autosave_thread_new((GObject *) audio_application_context);
	  ags_thread_add_child_extended(AGS_THREAD(audio_loop),
					audio_application_context->autosave_thread,
					TRUE, TRUE);
	}

	free(str);
      }
    }

    free(str);
  }
  
  /* AgsThreadPool */
  audio_application_context->thread_pool = AGS_TASK_THREAD(AGS_APPLICATION_CONTEXT(audio_application_context)->task_thread)->thread_pool;
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
      
      soundcard = (GObject *) g_value_get_object(value);

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
ags_audio_application_context_connect(AgsConnectable *connectable)
{
  AgsAudioApplicationContext *audio_application_context;
  GList *list;
  
  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(connectable);

  if((AGS_APPLICATION_CONTEXT_CONNECTED & (audio_application_context->flags)) != 0){
    return;
  }

  ags_audio_application_context_parent_connectable_interface->connect(connectable);

  g_message("connecting audio\0");
  
  list = audio_application_context->soundcard;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_audio_application_context_disconnect(AgsConnectable *connectable)
{
  AgsAudioApplicationContext *audio_application_context;

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(connectable);

  if((AGS_APPLICATION_CONTEXT_CONNECTED & (audio_application_context->flags)) == 0){
    return;
  }

  ags_audio_application_context_parent_connectable_interface->disconnect(connectable);
}

AgsThread*
ags_audio_application_context_get_main_loop(AgsConcurrencyProvider *concurrency_provider)
{
  return((AgsThread *) AGS_APPLICATION_CONTEXT(concurrency_provider)->main_loop);
}

AgsThread*
ags_audio_application_context_get_task_thread(AgsConcurrencyProvider *concurrency_provider)
{
  return((AgsThread *) AGS_APPLICATION_CONTEXT(concurrency_provider)->task_thread);
}

AgsThreadPool*
ags_audio_application_context_get_thread_pool(AgsConcurrencyProvider *concurrency_provider)
{
  return(AGS_AUDIO_APPLICATION_CONTEXT(concurrency_provider)->thread_pool);
}

GList*
ags_audio_application_context_get_soundcard(AgsSoundProvider *sound_provider)
{
  return(AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->soundcard);
}

void
ags_audio_application_context_set_soundcard(AgsSoundProvider *sound_provider,
					    GList *soundcard)
{
  AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->soundcard = soundcard;
}

GObject*
ags_audio_application_context_get_default_soundcard_thread(AgsSoundProvider *sound_provider)
{
  return((GObject *) AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->soundcard_thread);
}

void
ags_audio_application_context_set_default_soundcard_thread(AgsSoundProvider *sound_provider,
							   GObject *soundcard_thread)
{
  AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->soundcard_thread = (AgsThread *) soundcard_thread;
}

GList*
ags_audio_application_context_get_sequencer(AgsSoundProvider *sound_provider)
{
  return(AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->sequencer);
}

void
ags_audio_application_context_set_sequencer(AgsSoundProvider *sound_provider,
					    GList *sequencer)
{
  AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->sequencer = sequencer;
}

GList*
ags_audio_application_context_get_distributed_manager(AgsSoundProvider *sound_provider)
{
  return(AGS_AUDIO_APPLICATION_CONTEXT(sound_provider)->distributed_manager);
}

void
ags_audio_application_context_finalize(GObject *gobject)
{
  AgsAudioApplicationContext *audio_application_context;

  G_OBJECT_CLASS(ags_audio_application_context_parent_class)->finalize(gobject);

  audio_application_context = AGS_AUDIO_APPLICATION_CONTEXT(gobject);
}

void
ags_audio_application_context_load_config(AgsApplicationContext *application_context)
{
  AgsConfig *config;
  AgsSoundcard *soundcard;
  GList *list;
  
  gchar *alsa_handle;
  guint samplerate;
  guint buffer_size;
  guint pcm_channels, dsp_channels;

  config = application_context->config;
  
  list = AGS_AUDIO_APPLICATION_CONTEXT(application_context)->soundcard;

  while(soundcard != NULL){
    soundcard = AGS_SOUNDCARD(list->data);
    
    alsa_handle = ags_config_get_value(config,
				 AGS_CONFIG_SOUNDCARD,
				 "alsa-handle\0");

    dsp_channels = strtoul(ags_config_get_value(config,
					  AGS_CONFIG_SOUNDCARD,
					  "dsp-channels\0"),
			   NULL,
			   10);
    
    pcm_channels = strtoul(ags_config_get_value(config,
					  AGS_CONFIG_SOUNDCARD,
					  "pcm-channels\0"),
			   NULL,
			   10);

    samplerate = strtoul(ags_config_get_value(config,
					AGS_CONFIG_SOUNDCARD,
					"samplerate\0"),
			 NULL,
			 10);

    buffer_size = strtoul(ags_config_get_value(config,
					 AGS_CONFIG_SOUNDCARD,
					 "buffer-size\0"),
			  NULL,
			  10);
    
    g_object_set(G_OBJECT(soundcard),
		 "device\0", alsa_handle,
		 "dsp-channels\0", dsp_channels,
		 "pcm-channels\0", pcm_channels,
		 "frequency\0", samplerate,
		 "buffer-size\0", buffer_size,
		 NULL);

    list = list->next;
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
		"samplerate\0",
		10)){    
      guint samplerate;

      samplerate = strtoul(value,
			   NULL,
			   10);

      g_object_set(G_OBJECT(soundcard),
		   "frequency\0", samplerate,
		   NULL);
    }else if(!strncmp(key,
		      "buffer-size\0",
		      11)){
      guint buffer_size;
    
      buffer_size = strtoul(value,
			    NULL,
			    10);

      g_object_set(G_OBJECT(soundcard),
		   "buffer-size\0", buffer_size,
		   NULL);
    }else if(!strncmp(key,
		      "pcm-channels\0",
		      12)){
      guint pcm_channels;

      pcm_channels = strtoul(value,
			     NULL,
			     10);
      
      g_object_set(G_OBJECT(soundcard),
		   "pcm-channels\0", pcm_channels,
		   NULL);
    }else if(!strncmp(key,
		      "dsp-channels\0",
		      12)){
      guint dsp_channels;

      dsp_channels = strtoul(value,
			     NULL,
			     10);
      
      g_object_set(G_OBJECT(soundcard),
		   "dsp-channels\0", dsp_channels,
		   NULL);
    }else if(!strncmp(key,
		      "alsa-handle\0",
		      11)){
      gchar *alsa_handle;
    
      alsa_handle = value;
      g_object_set(G_OBJECT(soundcard),
		   "device\0", alsa_handle,
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
  ags_record_thread_get_type();
  ags_iterator_thread_get_type();
  ags_recycling_thread_get_type();

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
  ags_recall_channel_run_dummy_get_type();
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
	       "application-context\0", gobject,
	       NULL);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
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
      if(!xmlStrncmp("ags-devout-list\0",
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
		    "ags-application-context\0");

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", application_context,
				   NULL));

  xmlNewProp(node,
	     AGS_FILE_CONTEXT_PROP,
	     "audio\0");

  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     AGS_FILE_FLAGS_PROP,
	     g_strdup_printf("%x\0", ((~AGS_APPLICATION_CONTEXT_CONNECTED) & (AGS_APPLICATION_CONTEXT(application_context)->flags))));

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
